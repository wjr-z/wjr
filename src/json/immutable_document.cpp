#include <wjr/json/immutable_document.hpp>

#include <algorithm>
#include <cstring>
#include <limits>
#include <locale>
#include <vector>

#include <wjr/json/detail/utf8.hpp>
#include <wjr/json/visitor.hpp>

namespace wjr::json {

result<value_view> array_view::at(uint32_t index) const noexcept {
    if (index >= size())
        return unexpected(error_code::INDEX_OUT_OF_BOUNDS);
    auto current = begin();
    for (uint32_t i = 0; i != index; ++i)
        ++current;
    return *current;
}

result<value_view> object_view::find(std::string_view key) const noexcept {
    uint32_t first = static_cast<uint32_t>(m_owner->node_at(m_index).payload);
    uint32_t count = size();
    while (count != 0) {
        const uint32_t step = count / 2;
        const uint32_t middle = first + step;
        const auto &member = m_owner->member_at(middle);
        const auto candidate = m_owner->string_at(member.key_offset, member.key_length);
        const int comparison = candidate.compare(key);
        if (comparison < 0) {
            first = middle + 1;
            count -= step + 1;
        } else if (comparison > 0) {
            count = step;
        } else {
            return value_view(m_owner, member.value_index);
        }
    }
    return unexpected(error_code::NO_SUCH_FIELD);
}

namespace visitor_detail {
template <>
inline constexpr bool parse_is_nothrow<detail::immutable_builder> = false;
} // namespace visitor_detail

namespace detail {

class immutable_builder {
    template <typename Parser>
    friend result<void> visitor_detail::parse(Parser &&, ondemand_reader &) noexcept(
        ::wjr::json::visitor_detail::parse_is_nothrow<::wjr::remove_cvref_t<Parser>>);

    using node = immutable_node;
    using member = immutable_member;
    enum class location { root, object, array };
    struct frame {
        uint32_t node_index;
        size_t members_begin;
        uint32_t children = 0;
    };
    struct string_range {
        uint32_t offset;
        uint32_t length;
    };
    struct totals {
        uint64_t nodes = 1;
        uint64_t members = 0;
        uint64_t bytes = 0;
    };
    struct output_position {
        uint32_t node = 0;
        uint32_t member = 0;
        uint32_t byte = 0;
    };

public:
    explicit immutable_builder(ondemand_reader &reader) noexcept : m_reader(reader) {}
    immutable_builder(const immutable_builder &) = delete;
    immutable_builder &operator=(const immutable_builder &) = delete;

    result<void> run() { return visitor_detail::parse(*this, m_reader); }
    parse_error error(error_code code) const noexcept {
        return {code, m_error_offset == ondemand_reader::npos ? m_position : m_error_offset};
    }

    expected<immutable_document, parse_error> finish() const {
        const totals sizes = measure(0);
        const uint64_t bytes =
            sizes.nodes * sizeof(node) + sizes.members * sizeof(member) + sizes.bytes;
        if (bytes > std::numeric_limits<size_t>::max()) {
            return unexpected(error(error_code::CAPACITY));
        }
        std::unique_ptr<std::byte[]> storage(new std::byte[static_cast<size_t>(bytes)]);
        immutable_document owner(std::move(storage), static_cast<uint32_t>(sizes.nodes),
                                 static_cast<uint32_t>(sizes.members),
                                 static_cast<uint32_t>(sizes.bytes));
        output_position position;
        pack(0, owner, position);
        return owner;
    }

private:
    static bool room(size_t size, size_t extra, size_t max_size) noexcept {
        const auto limit = std::min(max_size, size_t(std::numeric_limits<uint32_t>::max()));
        return size <= limit && extra <= limit - size;
    }
    static uint64_t container_payload(uint32_t begin, uint32_t end) noexcept {
        return uint64_t(begin) | (uint64_t(end) << 32);
    }
    uint32_t offset(const char *pointer) const noexcept {
        return static_cast<uint32_t>(pointer - m_reader.data());
    }
    void observe_token(uint32_t token) noexcept {
        m_position = token == ondemand_reader::npos ? m_reader.size() : token;
    }
    result<void> failure(error_code code, uint32_t position) noexcept {
        m_error_offset = position;
        return unexpected(code);
    }
    bool boundary(const char *position) const noexcept {
        if (position == m_reader.data() + m_reader.size())
            return true;
        const char ch = *position;
        return is_json_space(ch) || ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ',' ||
               ch == ':' || ch == '"';
    }
    result<void> string_syntax(const char *first, const char *last) noexcept {
        if (!boundary(last + 1))
            return failure(error_code::TAPE_ERROR, offset(last + 1));
        if (contains_unescaped_control(first, last))
            return failure(error_code::UNESCAPED_CHARS, offset(first) - 1);
        return {};
    }
    result<string_range> append_string(const char *first, const char *last) {
        const auto raw_size = static_cast<size_t>(last - first);
        if (!room(m_bytes.size(), raw_size, m_bytes.max_size()))
            return unexpected(error_code::CAPACITY);
        const auto begin = static_cast<uint32_t>(m_bytes.size());
        if (raw_size == 0)
            return string_range{begin, 0};
        try_uninitialized_resize(m_bytes, m_bytes.size() + raw_size);
        auto parsed = parse_string(m_bytes.data() + begin, first, last);
        if (!parsed)
            return unexpected(parsed.error());
        const auto length = static_cast<uint32_t>(*parsed - (m_bytes.data() + begin));
        m_bytes.resize(size_t(begin) + length);
        return string_range{begin, length};
    }
    result<void> append_node(location where, node value) {
        if (!room(m_nodes.size(), 1, m_nodes.max_size()))
            return failure(error_code::CAPACITY, m_position);
        if (where == location::array)
            ++m_frames.back().children;
        m_nodes.push_back(value);
        return {};
    }
    result<void> null(location where, const char *first) {
        auto checked = check_null(first);
        if (!checked)
            return failure(checked.error(), offset(first));
        if (!boundary(first + 4))
            return failure(error_code::TAPE_ERROR, offset(first));
        return append_node(where, {0, 0, static_cast<uint32_t>(value_t::null)});
    }
    result<void> boolean(location where, const char *first, bool value) {
        auto checked = value ? check_true(first) : check_false(first);
        if (!checked)
            return failure(checked.error(), offset(first));
        if (!boundary(first + (value ? 4 : 5)))
            return failure(error_code::TAPE_ERROR, offset(first));
        return append_node(where, {uint64_t(value), 0, static_cast<uint32_t>(value_t::boolean)});
    }
    result<void> number(location where, const char *first, const char *last) {
        basic_value value(default_construct);
        auto parsed = number_detail::from_chars_json(first, last, value);
        if (!parsed)
            return failure(parsed.ec == std::errc::result_out_of_range ? error_code::BIGINT_ERROR
                                                                       : error_code::TAPE_ERROR,
                           offset(first));
        if (parsed.ptr != last && !is_json_space(*parsed.ptr))
            return failure(error_code::TAPE_ERROR, offset(first));
        uint64_t bits;
        switch (value.m_type) {
        case value_t::number_unsigned:
            bits = value.m_number_unsigned;
            break;
        case value_t::number_signed:
            bits = std::bit_cast<uint64_t>(value.m_number_signed);
            break;
        default:
            bits = std::bit_cast<uint64_t>(value.m_number_float);
            break;
        }
        return append_node(where, {bits, 0, static_cast<uint32_t>(value.m_type)});
    }
    result<void> string(location where, const char *first, const char *last) {
        WJR_EXPECTED_TRY(string_syntax(first, last));
        auto decoded = append_string(first, last);
        if (!decoded)
            return failure(decoded.error(), offset(first) - 1);
        return append_node(
            where, {decoded->offset, decoded->length, static_cast<uint32_t>(value_t::string)});
    }
    result<void> start_container(location where, value_t kind) {
        const auto index = static_cast<uint32_t>(m_nodes.size());
        WJR_EXPECTED_TRY(append_node(where, {0, 0, static_cast<uint32_t>(kind)}));
        m_frames.push_back({index, m_scratch.size()});
        return {};
    }
    std::string_view key(const member &value) const noexcept {
        if (value.key_length == 0)
            return {};
        return {m_bytes.data() + value.key_offset, value.key_length};
    }
    result<void> end() {
        const frame current = m_frames.back();
        node &value = m_nodes[current.node_index];
        if (value.kind == static_cast<uint32_t>(value_t::object)) {
            auto first = m_scratch.begin() + current.members_begin;
            auto last = m_scratch.end();
            std::sort(first, last, [this](const member &left, const member &right) {
                const int comparison = key(left).compare(key(right));
                return comparison < 0 || (comparison == 0 && left.value_index < right.value_index);
            });
            const auto count = static_cast<size_t>(last - first);
            if (!room(m_members.size(), count, m_members.max_size()))
                return failure(error_code::CAPACITY, m_position);
            const auto begin = static_cast<uint32_t>(m_members.size());
            while (first != last) {
                auto next = first + 1;
                while (next != last && key(*first) == key(*next))
                    ++next;
                m_members.push_back(*(next - 1));
                first = next;
            }
            value.count = static_cast<uint32_t>(m_members.size() - begin);
            value.payload = container_payload(begin, static_cast<uint32_t>(m_nodes.size()));
            m_scratch.resize(current.members_begin);
        } else {
            value.count = current.children;
            value.payload =
                container_payload(current.node_index + 1, static_cast<uint32_t>(m_nodes.size()));
        }
        m_frames.pop_back();
        return {};
    }
    result<void> visit_object_key_string(const char *first, const char *last) {
        WJR_EXPECTED_TRY(string_syntax(first, last));
        auto decoded = append_string(first, last);
        if (!decoded)
            return failure(decoded.error(), offset(first) - 1);
        if (!room(m_scratch.size(), 1, m_scratch.max_size()))
            return failure(error_code::CAPACITY, offset(first) - 1);
        m_scratch.push_back(
            {decoded->offset, decoded->length, static_cast<uint32_t>(m_nodes.size())});
        return {};
    }
    uint32_t next_node(uint32_t index) const noexcept {
        const auto &value = m_nodes[index];
        return value.kind == static_cast<uint32_t>(value_t::object) ||
                       value.kind == static_cast<uint32_t>(value_t::array)
                   ? static_cast<uint32_t>(value.payload >> 32)
                   : index + 1;
    }
    /// @note The shared visitor validates nesting depth before this bounded tree walk.
    totals measure(uint32_t index) const noexcept {
        totals result;
        const auto &value = m_nodes[index];
        if (value.kind == static_cast<uint32_t>(value_t::string))
            result.bytes = value.count;
        if (value.kind == static_cast<uint32_t>(value_t::object)) {
            result.members = value.count;
            const auto begin = static_cast<uint32_t>(value.payload);
            for (uint32_t i = 0; i != value.count; ++i) {
                const auto &field = m_members[begin + i];
                // cppcheck-suppress misra-c2012-17.2
                const auto child = measure(field.value_index);
                result.nodes += child.nodes;
                result.members += child.members;
                result.bytes += field.key_length + child.bytes;
            }
        } else if (value.kind == static_cast<uint32_t>(value_t::array)) {
            uint32_t child_index = index + 1;
            for (uint32_t i = 0; i != value.count; ++i) {
                // cppcheck-suppress misra-c2012-17.2
                const auto child = measure(child_index);
                result.nodes += child.nodes;
                result.members += child.members;
                result.bytes += child.bytes;
                child_index = next_node(child_index);
            }
        }
        return result;
    }
    static std::byte *member_position(immutable_document &owner, uint32_t index) noexcept {
        return owner.m_storage.get() + sizeof(node) * size_t(owner.m_node_count) +
               sizeof(member) * size_t(index);
    }
    uint32_t copy_bytes(immutable_document &owner, output_position &position, uint32_t first,
                        uint32_t length) const noexcept {
        const uint32_t destination_offset = position.byte;
        auto *destination = member_position(owner, owner.m_member_count) + destination_offset;
        WJR_ASSERT(first <= m_bytes.size() && length <= m_bytes.size() - first);
        WJR_ASSERT(destination_offset <= owner.m_byte_count &&
                   length <= owner.m_byte_count - destination_offset);
        if (length != 0) {
            /// @note measure() reserves all reachable bytes before pack() copies those same ranges.
            // Flawfinder: ignore
            std::memcpy(destination, m_bytes.data() + first, length);
        }
        position.byte += length;
        return destination_offset;
    }
    uint32_t pack(uint32_t index, immutable_document &owner,
                  output_position &position) const noexcept {
        const uint32_t output_index = position.node++;
        node value = m_nodes[index];
        if (value.kind == static_cast<uint32_t>(value_t::string)) {
            value.payload =
                copy_bytes(owner, position, static_cast<uint32_t>(value.payload), value.count);
        } else if (value.kind == static_cast<uint32_t>(value_t::object)) {
            const uint32_t source_begin = static_cast<uint32_t>(value.payload);
            const uint32_t begin = position.member;
            position.member += value.count;
            for (uint32_t i = 0; i != value.count; ++i) {
                const member &field = m_members[source_begin + i];
                const uint32_t key_offset =
                    copy_bytes(owner, position, field.key_offset, field.key_length);
                // cppcheck-suppress misra-c2012-17.2
                const uint32_t child = pack(field.value_index, owner, position);
                wjr::construct_at(reinterpret_cast<member *>(member_position(owner, begin + i)),
                                  member{key_offset, field.key_length, child});
            }
            value.payload = container_payload(begin, position.node);
        } else if (value.kind == static_cast<uint32_t>(value_t::array)) {
            const uint32_t begin = position.node;
            uint32_t child = index + 1;
            for (uint32_t i = 0; i != value.count; ++i) {
                // cppcheck-suppress misra-c2012-17.2
                pack(child, owner, position);
                child = next_node(child);
            }
            value.payload = container_payload(begin, position.node);
        }
        wjr::construct_at(
            reinterpret_cast<node *>(owner.m_storage.get() + sizeof(node) * size_t(output_index)),
            value);
        return output_index;
    }

#define WJR_JSON_IMMUTABLE_CALLBACKS(place)                                                        \
    result<void> visit_##place##_null(const char *first) { return null(location::place, first); }  \
    result<void> visit_##place##_true(const char *first) {                                         \
        return boolean(location::place, first, true);                                              \
    }                                                                                              \
    result<void> visit_##place##_false(const char *first) {                                        \
        return boolean(location::place, first, false);                                             \
    }                                                                                              \
    result<void> visit_##place##_number(const char *first, const char *last) {                     \
        return number(location::place, first, last);                                               \
    }                                                                                              \
    result<void> visit_##place##_string(const char *first, const char *last) {                     \
        return string(location::place, first, last);                                               \
    }                                                                                              \
    result<void> visit_##place##_start_object(uint32_t) {                                          \
        return start_container(location::place, value_t::object);                                  \
    }                                                                                              \
    result<void> visit_##place##_start_array(uint32_t) {                                           \
        return start_container(location::place, value_t::array);                                   \
    }
    WJR_JSON_IMMUTABLE_CALLBACKS(root)
    WJR_JSON_IMMUTABLE_CALLBACKS(object)
    WJR_JSON_IMMUTABLE_CALLBACKS(array)
#undef WJR_JSON_IMMUTABLE_CALLBACKS
#define WJR_JSON_IMMUTABLE_END(container)                                                          \
    result<void> visit_end_##container##_to_root(uint32_t) { return end(); }                       \
    result<void> visit_end_##container##_to_object(uint32_t) { return end(); }                     \
    result<void> visit_end_##container##_to_array(uint32_t) { return end(); }
    WJR_JSON_IMMUTABLE_END(object)
    WJR_JSON_IMMUTABLE_END(array)
#undef WJR_JSON_IMMUTABLE_END

    ondemand_reader &m_reader;
    std::vector<node> m_nodes;
    std::vector<member> m_members;
    vector<char> m_bytes;
    small_vector<frame, 16> m_frames;
    small_vector<member, 64> m_scratch;
    uint32_t m_position = 0;
    uint32_t m_error_offset = ondemand_reader::npos;
};

} // namespace detail

expected<immutable_document, parse_error> immutable_document::parse(ondemand_reader &reader) {
    if (reader.m_input.size() > std::numeric_limits<uint32_t>::max())
        return unexpected(parse_error{error_code::CAPACITY, 0});
    if (const auto offset = detail::find_invalid_utf8(reader.m_input);
        offset != std::string_view::npos)
        return unexpected(parse_error{error_code::UTF8_ERROR, static_cast<uint32_t>(offset)});
    detail::immutable_builder builder(reader);
    auto parsed = builder.run();
    if (!parsed)
        return unexpected(builder.error(parsed.error()));
    return builder.finish();
}

} // namespace wjr::json
