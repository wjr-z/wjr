#ifndef WJR_JSON_INPLACE_PARSER_HPP__
#define WJR_JSON_INPLACE_PARSER_HPP__

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <wjr/json/detail/utf8.hpp>
#include <wjr/json/fields.hpp>
#include <wjr/json/visitor.hpp>

namespace wjr::json {

namespace detail {
class inplace_parser_context;
}

namespace visitor_detail {
template <>
inline constexpr bool parse_is_nothrow<detail::inplace_parser_context> = false;
}

namespace detail {

enum class typed_kind : uint8_t { boolean, number, string, object, array };
struct typed_operations;
struct typed_target {
    void *object = nullptr;
    const typed_operations *operations = nullptr;
};
struct typed_operations {
    typed_kind kind;
    void (*boolean)(void *, bool) = nullptr;
    result<void> (*number)(void *, const basic_value &) = nullptr;
    result<void> (*text_number)(void *, const char *, const char *) = nullptr;
    result<void> (*string)(void *, const char *, const char *) = nullptr;
    typed_target (*append)(void *) = nullptr;
    void (*clear)(void *) = nullptr;
    bool (*select)(void *, std::string_view, typed_target &, size_t &) = nullptr;
    size_t field_count = 0;
};

template <typename T>
concept registered_json_object = requires { T::template wjr_json_fields<>(); };

template <typename T>
struct typed_string : std::false_type {};
template <typename Traits, typename Allocator>
struct typed_string<std::basic_string<char, Traits, Allocator>> : std::true_type {};

template <typename T>
struct typed_array : std::false_type {};
template <typename T, typename Allocator>
struct typed_array<std::vector<T, Allocator>> : std::true_type {
    using element_type = T;
};
template <typename T, typename Allocator>
struct typed_array<basic_vector<default_vector_storage<T, Allocator>>> : std::true_type {
    using element_type = T;
};

template <typename T>
const typed_operations &typed_operations_for();

template <typename T, size_t Index>
WJR_INTRINSIC_INLINE bool select_typed_field_at(void *object, std::string_view key,
                                                typed_target &target, size_t &field_index) {
    constexpr auto field = std::get<Index>(T::template wjr_json_fields<>());
    if (key != field.name) {
        return false;
    }
    static_assert(std::is_member_object_pointer_v<decltype(field.member)>,
                  "Direct JSON fields must be instance data members");
    auto &value = static_cast<T *>(object)->*field.member;
    using value_type = std::remove_reference_t<decltype(value)>;
    static_assert(!std::is_const_v<value_type>, "Direct JSON fields must be writable");
    target = {std::addressof(value), std::addressof(typed_operations_for<value_type>())};
    field_index = Index;
    return true;
}

template <typename T, size_t... Index>
bool select_typed_field(void *object, std::string_view key, typed_target &target,
                        size_t &field_index) {
    return (select_typed_field_at<T, Index>(object, key, target, field_index) || ...);
}

template <typename T>
result<void> assign_typed_number(void *object, const basic_value &value) {
    T &output = *static_cast<T *>(object);
    if constexpr (std::is_integral_v<T>) {
        if (value.m_type == value_t::number_unsigned) {
            if (value.m_number_unsigned > static_cast<uint64_t>(std::numeric_limits<T>::max())) {
                return unexpected(error_code::NUMBER_OUT_OF_RANGE);
            }
            output = static_cast<T>(value.m_number_unsigned);
        } else if (value.m_type == value_t::number_signed) {
            const auto number = value.m_number_signed;
            if constexpr (std::is_signed_v<T>) {
                if (number < static_cast<int64_t>(std::numeric_limits<T>::min()) ||
                    number > static_cast<int64_t>(std::numeric_limits<T>::max())) {
                    return unexpected(error_code::NUMBER_OUT_OF_RANGE);
                }
            } else {
                if (number < 0 || static_cast<uint64_t>(number) >
                                      static_cast<uint64_t>(std::numeric_limits<T>::max())) {
                    return unexpected(error_code::NUMBER_OUT_OF_RANGE);
                }
            }
            output = static_cast<T>(number);
        } else {
            return unexpected(error_code::INCORRECT_TYPE);
        }
    }
    return {};
}

template <fastfloat::detail::supported_float T>
result<void> parse_typed_float(void *object, const char *first, const char *last) {
    auto &value = *static_cast<T *>(object);
    const auto parsed = fastfloat::from_chars<chars_format::json>(first, last, value);
    if (!parsed) {
        return unexpected(parsed.ec == std::errc::result_out_of_range
                              ? error_code::NUMBER_OUT_OF_RANGE
                              : error_code::TAPE_ERROR);
    }
    if (parsed.ptr != last && !is_json_space(*parsed.ptr)) {
        return unexpected(error_code::TAPE_ERROR);
    }
    return {};
}

template <typename Array>
void append_typed_boolean(void *object, bool value) {
    static_cast<Array *>(object)->emplace_back(value);
}

template <typename Array>
typed_target append_typed_element(void *object) {
    using element_type = typename typed_array<Array>::element_type;
    if constexpr (std::is_same_v<element_type, bool>) {
        static constexpr typed_operations operations{typed_kind::boolean,
                                                     &append_typed_boolean<Array>};
        return {object, &operations};
    } else {
        auto &value = static_cast<Array *>(object)->emplace_back();
        return {std::addressof(value), std::addressof(typed_operations_for<element_type>())};
    }
}

template <typename T>
constexpr typed_operations make_typed_operations() {
    if constexpr (std::is_same_v<T, bool>) {
        return {typed_kind::boolean,
                [](void *object, bool value) { *static_cast<T *>(object) = value; }};
    } else if constexpr (std::is_integral_v<T>) {
        typed_operations operations{typed_kind::number};
        operations.number = &assign_typed_number<T>;
        return operations;
    } else if constexpr (fastfloat::detail::supported_float<T>) {
        typed_operations operations{typed_kind::number};
        operations.text_number = &parse_typed_float<T>;
        return operations;
    } else if constexpr (typed_string<T>::value) {
        typed_operations operations{typed_kind::string};
        operations.string = [](void *object, const char *first, const char *last) {
            return decode_owned_string(*static_cast<T *>(object), first, last);
        };
        return operations;
    } else if constexpr (registered_json_object<T>) {
        typed_operations operations{typed_kind::object};
        constexpr auto field_count = std::tuple_size_v<decltype(T::template wjr_json_fields<>())>;
        operations.select = []<size_t... Index>(std::index_sequence<Index...>) {
            return &select_typed_field<T, Index...>;
        }(std::make_index_sequence<field_count>{});
        operations.field_count = field_count;
        return operations;
    } else {
        static_assert(
            typed_array<T>::value,
            "Direct JSON parsing supports integers, float/double, owning strings, registered "
            "objects, and vectors");
        typed_operations operations{typed_kind::array};
        operations.append = &append_typed_element<T>;
        operations.clear = [](void *object) { static_cast<T *>(object)->clear(); };
        return operations;
    }
}

template <typename T>
inline constexpr typed_operations typed_operations_value = make_typed_operations<T>();

template <typename T>
const typed_operations &typed_operations_for() {
    return typed_operations_value<T>;
}

/// @private Typed destinations for the shared JSON grammar visitor.
class inplace_parser_context {
    template <typename Parser>
    friend result<void> visitor_detail::parse(Parser &&par, ondemand_reader &reader) noexcept(
        ::wjr::json::visitor_detail::parse_is_nothrow<::wjr::remove_cvref_t<Parser>>);

    enum class location { root, object, array };
    struct frame {
        typed_target target;
        typed_target pending;
        size_t seen_begin;
        size_t seen_count;
        bool object;
    };

public:
    inplace_parser_context(ondemand_reader &reader, typed_target root) noexcept
        : m_reader(reader), m_root(root) {}
    inplace_parser_context(const inplace_parser_context &) = delete;
    inplace_parser_context &operator=(const inplace_parser_context &) = delete;

    result<void> run() { return visitor_detail::parse(*this, m_reader); }

    parse_error error(error_code code) const noexcept {
        return {code, m_error_offset == ondemand_reader::npos ? m_position : m_error_offset};
    }

private:
    void observe_token(uint32_t token) noexcept {
        m_position = token == ondemand_reader::npos ? m_reader.size() : token;
    }

    uint32_t offset(const char *pointer) const noexcept {
        return static_cast<uint32_t>(pointer - m_reader.data());
    }

    bool value_boundary(const char *position) const noexcept {
        if (position == m_reader.data() + m_reader.size()) {
            return true;
        }
        const char ch = *position;
        return is_json_space(ch) || ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ',' ||
               ch == ':' || ch == '"';
    }

    result<void> failure(error_code code, uint32_t position) noexcept {
        m_error_offset = position;
        return unexpected(code);
    }

    typed_target destination(location where) {
        if (where == location::root) {
            return m_root;
        }
        auto &parent = m_frames.back();
        if (where == location::object) {
            return parent.pending;
        }
        if (!parent.target.operations) {
            return {};
        }
        return parent.target.operations->append(parent.target.object);
    }

    result<void> scalar_null(location where, const char *first) {
        auto checked = check_null(first);
        if (!checked) {
            return failure(checked.error(), offset(first));
        }
        if (!value_boundary(first + 4)) {
            return failure(error_code::TAPE_ERROR, offset(first));
        }
        if (destination(where).operations) {
            return failure(error_code::INCORRECT_TYPE, offset(first));
        }
        return {};
    }

    result<void> scalar_boolean(location where, const char *first, bool value) {
        auto checked = value ? check_true(first) : check_false(first);
        if (!checked) {
            return failure(checked.error(), offset(first));
        }
        if (!value_boundary(first + (value ? 4 : 5))) {
            return failure(error_code::TAPE_ERROR, offset(first));
        }
        const auto target = destination(where);
        if (target.operations) {
            if (target.operations->kind != typed_kind::boolean) {
                return failure(error_code::INCORRECT_TYPE, offset(first));
            }
            target.operations->boolean(target.object, value);
        }
        return {};
    }

    result<void> scalar_number(location where, const char *first, const char *last) {
        const auto target = destination(where);
        if (target.operations && target.operations->text_number) {
            auto parsed = target.operations->text_number(target.object, first, last);
            return parsed ? result<void>{} : failure(parsed.error(), offset(first));
        }
        basic_value value(default_construct);
        auto parsed = number_detail::from_chars_json(first, last, value);
        if (!parsed) {
            return failure(parsed.ec == std::errc::result_out_of_range ? error_code::BIGINT_ERROR
                                                                       : error_code::TAPE_ERROR,
                           offset(first));
        }
        if (parsed.ptr != last && !is_json_space(*parsed.ptr)) {
            return failure(error_code::TAPE_ERROR, offset(first));
        }
        if (target.operations) {
            if (target.operations->kind != typed_kind::number) {
                return failure(error_code::INCORRECT_TYPE, offset(first));
            }
            if (value.m_type == value_t::number_float) {
                const bool floating_syntax = std::find_if(first, last, [](char ch) {
                                                 return ch == '.' || ch == 'e' || ch == 'E';
                                             }) != last;
                return failure(floating_syntax ? error_code::INCORRECT_TYPE
                                               : error_code::NUMBER_OUT_OF_RANGE,
                               offset(first));
            }
            auto assigned = target.operations->number(target.object, value);
            if (!assigned) {
                return failure(assigned.error(), offset(first));
            }
        }
        return {};
    }

    result<void> scalar_string(location where, const char *first, const char *last) {
        if (!value_boundary(last + 1)) {
            return failure(error_code::TAPE_ERROR, offset(last + 1));
        }
        if (contains_unescaped_control(first, last)) {
            return failure(error_code::UNESCAPED_CHARS, offset(first) - 1);
        }
        const auto target = destination(where);
        if (!target.operations) {
            auto checked = check_string(first, last);
            return checked ? result<void>{} : failure(checked.error(), offset(first) - 1);
        }
        if (target.operations->kind != typed_kind::string) {
            return failure(error_code::INCORRECT_TYPE, offset(first) - 1);
        }
        auto parsed = target.operations->string(target.object, first, last);
        return parsed ? result<void>{} : failure(parsed.error(), offset(first) - 1);
    }

    result<void> begin_container(location where, typed_kind kind, uint32_t position) {
        const auto target = destination(where);
        if (target.operations && target.operations->kind != kind) {
            return failure(error_code::INCORRECT_TYPE, position);
        }
        const bool object = kind == typed_kind::object;
        const size_t seen_begin = m_seen.size();
        if (target.operations) {
            if (object) {
                m_seen.resize(seen_begin + (target.operations->field_count + 63) / 64, 0);
            } else {
                target.operations->clear(target.object);
            }
        }
        m_frames.emplace_back(frame{target, {}, seen_begin, 0, object});
        return {};
    }

    result<void> end_container(uint32_t position) {
        const auto &current = m_frames.back();
        if (current.object && current.target.operations &&
            current.seen_count != current.target.operations->field_count) {
            return failure(error_code::NO_SUCH_FIELD, position);
        }
        m_seen.resize(current.seen_begin);
        m_frames.pop_back();
        return {};
    }

    result<void> visit_object_key_string(const char *first, const char *last) {
        if (!value_boundary(last + 1)) {
            return failure(error_code::TAPE_ERROR, offset(last + 1));
        }
        if (contains_unescaped_control(first, last)) {
            return failure(error_code::UNESCAPED_CHARS, offset(first) - 1);
        }
        auto &current = m_frames.back();
        current.pending = {};
        if (!current.target.operations) {
            auto checked = check_string(first, last);
            return checked ? result<void>{} : failure(checked.error(), offset(first) - 1);
        }
        auto decoded = decode_owned_string(m_key, first, last);
        if (!decoded) {
            return failure(decoded.error(), offset(first) - 1);
        }
        size_t index;
        if (current.target.operations->select(current.target.object, m_key, current.pending,
                                              index)) {
            auto &bits = m_seen[current.seen_begin + index / 64];
            const uint64_t mask = uint64_t{1} << (index % 64);
            current.seen_count += (bits & mask) == 0;
            bits |= mask;
        }
        return {};
    }

#define WJR_JSON_TYPED_SCALAR_CALLBACKS(place)                                                     \
    result<void> visit_##place##_null(const char *first) {                                         \
        return scalar_null(location::place, first);                                                \
    }                                                                                              \
    result<void> visit_##place##_true(const char *first) {                                         \
        return scalar_boolean(location::place, first, true);                                       \
    }                                                                                              \
    result<void> visit_##place##_false(const char *first) {                                        \
        return scalar_boolean(location::place, first, false);                                      \
    }                                                                                              \
    result<void> visit_##place##_number(const char *first, const char *last) {                     \
        return scalar_number(location::place, first, last);                                        \
    }                                                                                              \
    result<void> visit_##place##_string(const char *first, const char *last) {                     \
        return scalar_string(location::place, first, last);                                        \
    }                                                                                              \
    result<void> visit_##place##_start_object(uint32_t token) {                                    \
        return begin_container(location::place, typed_kind::object, token);                        \
    }                                                                                              \
    result<void> visit_##place##_start_array(uint32_t token) {                                     \
        return begin_container(location::place, typed_kind::array, token);                         \
    }

    WJR_JSON_TYPED_SCALAR_CALLBACKS(root)
    WJR_JSON_TYPED_SCALAR_CALLBACKS(object)
    WJR_JSON_TYPED_SCALAR_CALLBACKS(array)
#undef WJR_JSON_TYPED_SCALAR_CALLBACKS

#define WJR_JSON_TYPED_END_CALLBACKS(container)                                                    \
    result<void> visit_end_##container##_to_root(uint32_t token) { return end_container(token); }  \
    result<void> visit_end_##container##_to_object(uint32_t token) {                               \
        return end_container(token);                                                               \
    }                                                                                              \
    result<void> visit_end_##container##_to_array(uint32_t token) { return end_container(token); }

    WJR_JSON_TYPED_END_CALLBACKS(object)
    WJR_JSON_TYPED_END_CALLBACKS(array)
#undef WJR_JSON_TYPED_END_CALLBACKS

    ondemand_reader &m_reader;
    typed_target m_root;
    small_vector<frame, 16> m_frames;
    small_vector<uint64_t, 16> m_seen;
    std::string m_key;
    uint32_t m_position = 0;
    uint32_t m_error_offset = ondemand_reader::npos;
};

template <typename T>
concept typed_destination =
    std::is_same_v<T, std::remove_cv_t<T>> &&
    (std::is_integral_v<T> || fastfloat::detail::supported_float<T> || typed_string<T>::value ||
     registered_json_object<T> || typed_array<T>::value);

template <typename T>
concept typed_parse_target =
    std::is_default_constructible_v<T> && std::is_move_constructible_v<T> && typed_destination<T>;

} // namespace detail

struct inplace_parser_base {};

template <typename T>
struct inplace_parser_impl : inplace_parser_base {};

/**
 * @brief Parse into an existing writable target, retaining the legacy noexcept entry.
 * @pre target points to a live T. Its contents may be partially updated on failure.
 */
template <typename T>
class inplace_parser {
public:
    static result<void> parse(T *target, ondemand_reader &reader) noexcept
    requires detail::typed_destination<T>
    {
        if (detail::find_invalid_utf8({reader.data(), reader.size()}) != std::string_view::npos) {
            return unexpected(error_code::UTF8_ERROR);
        }
        detail::inplace_parser_context parser(
            reader, {target, std::addressof(detail::typed_operations_for<T>())});
        return parser.run();
    }

protected:
#define WJR_JSON_LEGACY_SCALAR_CALLBACKS(place)                                                    \
    result<void> visit_##place##_null(const char *first) const noexcept {                          \
        return detail::check_null(first);                                                          \
    }                                                                                              \
    result<void> visit_##place##_true(const char *first) const noexcept {                          \
        return detail::check_true(first);                                                          \
    }                                                                                              \
    result<void> visit_##place##_false(const char *first) const noexcept {                         \
        return detail::check_false(first);                                                         \
    }                                                                                              \
    result<void> visit_##place##_number(const char *first, const char *last) const noexcept {      \
        return detail::check_number(first, last);                                                  \
    }                                                                                              \
    result<void> visit_##place##_string(const char *first, const char *last) const noexcept {      \
        return detail::check_string(first, last);                                                  \
    }                                                                                              \
    result<void> visit_##place##_start_object(uint32_t) const noexcept { return {}; }              \
    result<void> visit_##place##_start_array(uint32_t) const noexcept { return {}; }
    WJR_JSON_LEGACY_SCALAR_CALLBACKS(root)
    WJR_JSON_LEGACY_SCALAR_CALLBACKS(object)
    WJR_JSON_LEGACY_SCALAR_CALLBACKS(array)
#undef WJR_JSON_LEGACY_SCALAR_CALLBACKS

    result<void> visit_object_key_string(const char *first, const char *last) const noexcept {
        return detail::check_string(first, last);
    }

#define WJR_JSON_LEGACY_END_CALLBACKS(container)                                                   \
    result<void> visit_end_##container##_to_root(uint32_t) const noexcept { return {}; }           \
    result<void> visit_end_##container##_to_object(uint32_t) const noexcept { return {}; }         \
    result<void> visit_end_##container##_to_array(uint32_t) const noexcept { return {}; }
    WJR_JSON_LEGACY_END_CALLBACKS(object)
    WJR_JSON_LEGACY_END_CALLBACKS(array)
#undef WJR_JSON_LEGACY_END_CALLBACKS
};

/**
 * @brief Parse an owning typed value through the shared JSON grammar.
 * @details Registered fields are required. Each duplicate must match its field type;
 * the final occurrence supplies the result. Unknown values receive full validation.
 * Integral destinations require integral JSON numbers within range. Float/double
 * destinations parse directly at their target precision, including signed zero.
 * Raw UTF-8 and literal control bytes in strings are validated before use.
 * @return A complete value, or an error code and its source byte offset.
 * @throws Propagates allocation and user construction exceptions after local cleanup.
 */
template <typename T>
requires detail::typed_parse_target<T>
expected<T, parse_error> parse(ondemand_reader &reader) {
    if (const auto offset = detail::find_invalid_utf8({reader.data(), reader.size()});
        offset != std::string_view::npos) {
        return unexpected(parse_error{error_code::UTF8_ERROR, static_cast<uint32_t>(offset)});
    }
    T value{};
    detail::inplace_parser_context parser(
        reader, {std::addressof(value), std::addressof(detail::typed_operations_for<T>())});
    auto parsed = parser.run();
    if (!parsed) {
        return unexpected(parser.error(parsed.error()));
    }
    return expected<T, parse_error>(std::in_place, std::move(value));
}

} // namespace wjr::json

#endif // WJR_JSON_INPLACE_PARSER_HPP__