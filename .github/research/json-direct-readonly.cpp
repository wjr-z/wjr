#include <algorithm>
#include <atomic>
#include <bit>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wjr/json/document.hpp>
#include <wjr/vector.hpp>

#ifdef WJR_WITH_MATURE
    #include "simdjson.hpp"
    #include "yyjson.h"
#endif

#ifdef WJR_READONLY_COUNT
namespace accounting {
struct alignas(std::max_align_t) header {
    size_t bytes;
    bool counted;
};
bool enabled = false;
size_t calls = 0, requested = 0, live = 0, peak = 0;
} // namespace accounting
void *operator new(size_t bytes) {
    if (bytes > std::numeric_limits<size_t>::max() - sizeof(accounting::header))
        throw std::bad_alloc();
    auto *block = static_cast<accounting::header *>(
        std::malloc(sizeof(accounting::header) + (bytes ? bytes : 1)));
    if (!block)
        throw std::bad_alloc();
    block->bytes = bytes;
    block->counted = accounting::enabled;
    if (block->counted) {
        ++accounting::calls;
        accounting::requested += bytes;
        accounting::live += bytes;
        accounting::peak = std::max(accounting::peak, accounting::live);
    }
    return block + 1;
}
void *operator new[](size_t bytes) { return ::operator new(bytes); }
void operator delete(void *pointer) noexcept {
    if (!pointer)
        return;
    auto *block = static_cast<accounting::header *>(pointer) - 1;
    if (block->counted)
        accounting::live -= block->bytes;
    std::free(block);
}
void operator delete[](void *pointer) noexcept { ::operator delete(pointer); }
void operator delete(void *pointer, size_t) noexcept { ::operator delete(pointer); }
void operator delete[](void *pointer, size_t) noexcept { ::operator delete(pointer); }
#endif

namespace {
using doc = wjr::json::document;
using namespace wjr::json;

template <typename T>
void escape(const T &value) {
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : : "g"(&value) : "memory");
#else
    static const void *volatile location;
    location = &value;
    std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
}

uint32_t index32(size_t value) {
    if (value >= UINT32_MAX)
        throw std::length_error("prototype capacity");
    return static_cast<uint32_t>(value);
}

size_t add(size_t left, size_t right) {
    if (right > std::numeric_limits<size_t>::max() - left)
        throw std::length_error("prototype capacity");
    return left + right;
}

struct node {
    uint32_t kind, extent;
    uint64_t payload;
};
struct member {
    uint32_t offset, length, child;
};
static_assert(sizeof(node) == 16 && sizeof(member) == 12);

/** @brief Private direct-parser cost prototype, not an installed API. */
struct frozen {
    wjr::vector<node> nodes;
    wjr::vector<member> members;
    wjr::vector<char> bytes;
    struct raw_member {
        uint32_t offset, length, child, parent;
    };
    struct builder {
        frozen &out;
        wjr::vector<raw_member> raw;
        wjr::inplace_vector<uint32_t, 257> stack;

        uint32_t append(value_t kind, uint64_t payload = 0, uint32_t extent = 0) {
            const auto index = index32(out.nodes.size());
            if (!stack.empty() && out.nodes[stack.back()].kind == uint32_t(value_t::array))
                ++out.nodes[stack.back()].extent;
            out.nodes.push_back({uint32_t(kind), extent, payload});
            return index;
        }
        result<member> decode(const char *first, const char *last) {
            const auto offset = index32(out.bytes.size());
            index32(add(offset, size_t(last - first)));
            out.bytes.append(size_t(last - first), wjr::default_construct);
            char empty;
            char *const destination = first == last ? &empty : out.bytes.data() + offset;
            auto decoded = detail::parse_string(destination, first, last);
            if (!decoded)
                return wjr::unexpected(decoded.error());
            const auto length = index32(*decoded - destination);
            out.bytes.resize(offset + length);
            return member{offset, length, 0};
        }
        result<void> null(const char *first) {
            WJR_EXPECTED_TRY(detail::check_null(first));
            append(value_t::null);
            return {};
        }
        result<void> boolean(const char *first, bool value) {
            if (value) {
                WJR_EXPECTED_TRY(detail::check_true(first));
            } else {
                WJR_EXPECTED_TRY(detail::check_false(first));
            }
            append(value_t::boolean, value);
            return {};
        }
        result<void> number(const char *first, const char *last) {
            basic_value value(wjr::default_construct);
            WJR_EXPECTED_TRY(detail::parse_number(first, last, value));
            uint64_t bits;
            switch (value.type()) {
            case value_t::number_unsigned:
                bits = value.get_number_unsigned();
                break;
            case value_t::number_signed:
                bits = std::bit_cast<uint64_t>(value.get_number_signed());
                break;
            case value_t::number_float:
                bits = std::bit_cast<uint64_t>(value.get_number_float());
                break;
            default:
                std::abort();
            }
            append(value.type(), bits);
            return {};
        }
        result<void> string(const char *first, const char *last) {
            auto decoded = decode(first, last);
            if (!decoded)
                return wjr::unexpected(decoded.error());
            append(value_t::string, decoded->offset, decoded->length);
            return {};
        }
        result<void> start(value_t kind) {
            const auto index = append(kind);
            if (kind == value_t::array)
                out.nodes[index].payload = uint64_t(index) + 1;
            stack.push_back(index);
            return {};
        }
        result<void> end(uint32_t) {
            auto &current = out.nodes[stack.back()];
            current.payload |= uint64_t(index32(out.nodes.size())) << 32;
            stack.pop_back();
            return {};
        }
        result<void> visit_object_key_string(const char *first, const char *last) {
            auto key = decode(first, last);
            if (!key)
                return wjr::unexpected(key.error());
            const auto parent = stack.back();
            ++out.nodes[parent].extent;
            raw.push_back({key->offset, key->length, index32(out.nodes.size()), parent});
            return {};
        }
#define DIRECT_SCALAR_CALLBACKS(where)                                                             \
    result<void> visit_##where##_null(const char *first) { return null(first); }                   \
    result<void> visit_##where##_true(const char *first) { return boolean(first, true); }          \
    result<void> visit_##where##_false(const char *first) { return boolean(first, false); }        \
    result<void> visit_##where##_number(const char *first, const char *last) {                     \
        return number(first, last);                                                                \
    }                                                                                              \
    result<void> visit_##where##_string(const char *first, const char *last) {                     \
        return string(first, last);                                                                \
    }                                                                                              \
    result<void> visit_##where##_start_object(uint32_t) { return start(value_t::object); }         \
    result<void> visit_##where##_start_array(uint32_t) { return start(value_t::array); }
        DIRECT_SCALAR_CALLBACKS(root)
        DIRECT_SCALAR_CALLBACKS(object)
        DIRECT_SCALAR_CALLBACKS(array)
#undef DIRECT_SCALAR_CALLBACKS
#define DIRECT_END_CALLBACKS(kind)                                                                 \
    result<void> visit_end_##kind##_to_root(uint32_t token) { return end(token); }                 \
    result<void> visit_end_##kind##_to_object(uint32_t token) { return end(token); }               \
    result<void> visit_end_##kind##_to_array(uint32_t token) { return end(token); }
        DIRECT_END_CALLBACKS(object)
        DIRECT_END_CALLBACKS(array)
#undef DIRECT_END_CALLBACKS
        void finish() {
            out.members.append(raw.size(), wjr::default_construct);
            uint32_t offset = 0;
            for (auto &entry : out.nodes) {
                if (entry.kind != uint32_t(value_t::object))
                    continue;
                entry.payload |= offset;
                offset += entry.extent;
            }
            for (const auto &entry : raw) {
                auto &parent = out.nodes[entry.parent];
                const auto cursor = uint32_t(parent.payload);
                out.members[cursor] = {entry.offset, entry.length, entry.child};
                ++parent.payload;
            }
            for (auto &entry : out.nodes) {
                if (entry.kind != uint32_t(value_t::object))
                    continue;
                entry.payload -= entry.extent;
                auto first = out.members.begin() + uint32_t(entry.payload);
                auto last = first + entry.extent;
                std::sort(first, last, [&](const member &left, const member &right) {
                    const auto lhs = out.key(left), rhs = out.key(right);
                    return lhs < rhs || (lhs == rhs && left.child < right.child);
                });
                auto output = first;
                for (auto current = first; current != last; ++current) {
                    if (output != first && out.key(*(output - 1)) == out.key(*current))
                        *(output - 1) = *current;
                    else
                        *output++ = *current;
                }
                entry.extent = index32(output - first);
            }
        }
    };

    frozen() = default;
    result<void> read(ondemand_reader &reader, std::string_view input) {
        reader.read(input);
        builder parser{*this};
#ifdef WJR_DIRECT_RESERVE
        size_t value_count = 0, member_count = 0, byte_count = 0;
        for (auto token = reader.get(); token != ondemand_reader::npos; token = reader.get()) {
            const auto ch = input[token];
            if (ch == '"') {
                const auto closing = reader.get();
                if (closing == ondemand_reader::npos || input[closing] != '"')
                    break;
                byte_count += closing - token - 1;
                size_t following = closing + 1;
                while (following < input.size() &&
                       (input[following] == ' ' || input[following] == '\t' ||
                        input[following] == '\r' || input[following] == '\n'))
                    ++following;
                if (following != input.size() && input[following] == ':')
                    ++member_count;
                else
                    ++value_count;
            } else if (ch == '{' || ch == '[' || ch == 'n' || ch == 't' || ch == 'f' || ch == '-' ||
                       (ch >= '0' && ch <= '9'))
                ++value_count;
        }
        nodes.reserve(index32(value_count));
        bytes.reserve(index32(byte_count));
        parser.raw.reserve(index32(member_count));
        reader.read(input);
#endif
        auto parsed = visitor_detail::parse(parser, reader);
        if (!parsed)
            return wjr::unexpected(parsed.error());
        parser.finish();
        return {};
    }
    frozen(ondemand_reader &reader, std::string_view input) {
        if (!read(reader, input))
            throw std::runtime_error("direct parse failed");
    }

    std::string_view key(const member &entry) const {
        return {byte_pointer() + entry.offset, entry.length};
    }
    const char *byte_pointer() const {
        static constexpr char empty = '\0';
        return bytes.empty() ? &empty : bytes.data();
    }
    std::string_view string(uint32_t index) const {
        const auto &entry = nodes[index];
        if (entry.kind != static_cast<uint32_t>(value_t::string))
            throw std::logic_error("type");
        return {byte_pointer() + static_cast<uint32_t>(entry.payload), entry.extent};
    }
    template <typename T = uint64_t>
    T number(uint32_t index) const {
        const auto &entry = nodes[index];
        switch (static_cast<value_t>(entry.kind)) {
        case value_t::number_unsigned:
            return static_cast<T>(entry.payload);
        case value_t::number_signed:
            return static_cast<T>(std::bit_cast<int64_t>(entry.payload));
        case value_t::number_float:
            return static_cast<T>(std::bit_cast<double>(entry.payload));
        default:
            throw std::logic_error("not numeric");
        }
    }
    uint32_t next(uint32_t index) const {
        const auto &entry = nodes[index];
        return entry.kind >= static_cast<uint32_t>(value_t::object)
                   ? static_cast<uint32_t>(entry.payload >> 32)
                   : index + 1;
    }
    template <bool Linear>
    uint32_t find(uint32_t index, std::string_view wanted) const {
        const auto &entry = nodes[index];
        if (entry.kind != static_cast<uint32_t>(value_t::object))
            throw std::logic_error("type");
        const auto first = members.begin() + static_cast<uint32_t>(entry.payload);
        const auto end = first + entry.extent;
        auto result = end;
        if constexpr (Linear) {
            result =
                std::find_if(first, end, [&](const member &value) { return key(value) == wanted; });
        } else {
            result = std::lower_bound(
                first, end, wanted,
                [&](const member &value, std::string_view query) { return key(value) < query; });
        }
        if (result == end || key(*result) != wanted)
            throw std::out_of_range("field");
        return result->child;
    }
    size_t used() const {
        return nodes.size() * sizeof(node) + members.size() * sizeof(member) + bytes.size();
    }
    size_t capacity() const {
        return nodes.capacity() * sizeof(node) + members.capacity() * sizeof(member) +
               bytes.capacity();
    }
};

void equal(const doc &source, const frozen &target, uint32_t index) {
    const auto &entry = target.nodes.at(index);
    if (entry.kind != static_cast<uint32_t>(source.type()))
        throw std::logic_error("kind mismatch");
    switch (source.type()) {
    case value_t::null:
        break;
    case value_t::boolean:
        if (entry.payload != source.get<boolean_t>())
            throw std::logic_error("bool");
        break;
    case value_t::number_unsigned:
        if (entry.payload != source.get<number_unsigned_t>())
            throw std::logic_error("u64");
        break;
    case value_t::number_signed:
        if (entry.payload != std::bit_cast<uint64_t>(source.get<number_signed_t>()))
            throw std::logic_error("i64");
        break;
    case value_t::number_float:
        if (entry.payload != std::bit_cast<uint64_t>(source.get<number_float_t>()))
            throw std::logic_error("f64");
        break;
    case value_t::string:
        if (target.string(index) != source.get<string_t>())
            throw std::logic_error("string");
        break;
    case value_t::object: {
        if (entry.extent != source.get<object_t>().size())
            throw std::logic_error("object size");
        auto position = static_cast<uint32_t>(entry.payload);
        for (const auto &[key, value] : source.get<object_t>()) {
            if (target.key(target.members.at(position)) != key)
                throw std::logic_error("key");
            equal(value, target, target.members[position++].child);
        }
        if (target.next(index) > target.nodes.size())
            throw std::logic_error("subtree end");
        break;
    }
    case value_t::array: {
        if (entry.extent != source.get<array_t>().size())
            throw std::logic_error("array size");
        auto position = static_cast<uint32_t>(entry.payload);
        for (const auto &value : source.get<array_t>()) {
            equal(value, target, position);
            position = target.next(position);
        }
        if (position != target.next(index))
            throw std::logic_error("array end");
        break;
    }
    }
}

struct config_record {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(config_record);
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(config_record, name, version, age)
    config_record(std::string n, std::string v, int a)
        : name(std::move(n)), version(std::move(v)), age(a) {}
    bool operator==(const config_record &) const = default;
    std::string name, version;
    int age = 0;
};
struct user_record {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(user_record);
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(user_record, id, name)
    user_record(uint64_t i, std::string n) : id(i), name(std::move(n)) {}
    bool operator==(const user_record &) const = default;
    uint64_t id = 0;
    std::string name;
};
struct status_record {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(status_record);
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(status_record, id, text, user)
    status_record(uint64_t i, std::string t, user_record u)
        : id(i), text(std::move(t)), user(std::move(u)) {}
    bool operator==(const status_record &) const = default;
    uint64_t id = 0;
    std::string text;
    user_record user;
};

uint64_t characters(std::string_view text) {
    return text.size() + (text.empty() ? 0
                                       : static_cast<unsigned char>(text.front()) +
                                             static_cast<unsigned char>(text.back()));
}
uint64_t consume(const config_record &record) {
    escape(record);
    return characters(record.name) + characters(record.version) + record.age;
}
uint64_t consume(const status_record &record) {
    escape(record);
    return record.id + record.user.id + characters(record.text) + characters(record.user.name);
}

enum class schema { config, statuses, numbers };
struct fixture {
    std::string name, input;
    schema type;
};
std::vector<fixture> fixtures() {
    std::vector<fixture> result;
    result.push_back(
        {"registered_config", R"({"name":"wjr","version":"1.0.0","age":22})", schema::config});
    result.push_back({"registered_config_long",
                      "{\"name\":\"" + std::string(80, 'n') + "\",\"version\":\"" +
                          std::string(48, 'v') + "\",\"age\":22}",
                      schema::config});
    std::string rows = "{\"statuses\":[";
    for (size_t i = 0; i < 128; ++i) {
        if (i)
            rows += ',';
        rows += "{\"id\":" + std::to_string(i) + ",\"text\":\"" + std::string(96, 't') +
                "\\n\\u00e9\",\"user\":{\"id\":" + std::to_string(i + 1000) + ",\"name\":\"" +
                std::string(32, 'u') + "\"}}";
    }
    result.push_back({"records_128", rows + "]}", schema::statuses});
    std::string numbers = "[";
    for (size_t i = 0; i < 4096; ++i) {
        if (i)
            numbers += ',';
        numbers += std::to_string(i);
    }
    numbers += ']';
    result.push_back({"numbers_4096", std::move(numbers), schema::numbers});
    std::ifstream file("test/data/success/twitter.json");
    if (!file)
        throw std::runtime_error("missing Twitter fixture");
    result.push_back(
        {"twitter", std::string((std::istreambuf_iterator<char>(file)), {}), schema::statuses});
    return result;
}

uint64_t query_mutable(const doc &source, schema type, bool owning) {
    escape(source);
    if (type == schema::config) {
        if (owning)
            return consume(config_record(source));
        return characters(source.at("name").get<string_t>()) +
               characters(source.at("version").get<string_t>()) +
               static_cast<int>(source.at("age"));
    }
    uint64_t sum = 0;
    if (type == schema::numbers) {
        if (owning) {
            std::vector<uint64_t> values;
            source.get_to(values);
            escape(values);
            for (auto value : values)
                sum += value;
        } else
            for (const auto &value : source.get<array_t>())
                sum += static_cast<uint64_t>(value);
        return sum;
    }
    for (const auto &value : source.at("statuses").get<array_t>()) {
        if (owning)
            sum += consume(status_record(value));
        else {
            const auto &user = value.at("user");
            sum += static_cast<uint64_t>(value.at("id")) + static_cast<uint64_t>(user.at("id")) +
                   characters(value.at("text").get<string_t>()) +
                   characters(user.at("name").get<string_t>());
        }
    }
    return sum;
}

template <bool Linear>
config_record project_config(const frozen &source) {
    return {std::string(source.string(source.find<Linear>(0, "name"))),
            std::string(source.string(source.find<Linear>(0, "version"))),
            source.number<int>(source.find<Linear>(0, "age"))};
}
template <bool Linear>
status_record project_status(const frozen &source, uint32_t index) {
    const auto user = source.find<Linear>(index, "user");
    return {source.number(source.find<Linear>(index, "id")),
            std::string(source.string(source.find<Linear>(index, "text"))),
            user_record(source.number(source.find<Linear>(user, "id")),
                        std::string(source.string(source.find<Linear>(user, "name"))))};
}

template <bool Linear>
uint64_t query_frozen(const frozen &source, schema type, bool owning) {
    escape(source);
    if (type == schema::config) {
        if (owning)
            return consume(project_config<Linear>(source));
        return characters(source.string(source.find<Linear>(0, "name"))) +
               characters(source.string(source.find<Linear>(0, "version"))) +
               source.number<int>(source.find<Linear>(0, "age"));
    }
    uint64_t sum = 0;
    if (type == schema::numbers) {
        std::vector<uint64_t> values;
        if (owning)
            values.reserve(source.nodes[0].extent);
        auto position = static_cast<uint32_t>(source.nodes[0].payload);
        for (size_t i = 0; i < source.nodes[0].extent; ++i) {
            const auto value = source.number(position);
            position = source.next(position);
            if (owning)
                values.push_back(value);
            else
                sum += value;
        }
        if (owning) {
            escape(values);
            for (auto value : values)
                sum += value;
        }
        return sum;
    }
    const auto array = source.find<Linear>(0, "statuses");
    auto position = static_cast<uint32_t>(source.nodes[array].payload);
    for (size_t i = 0; i < source.nodes[array].extent; ++i) {
        if (owning)
            sum += consume(project_status<Linear>(source, position));
        else {
            const auto user = source.find<Linear>(position, "user");
            sum += source.number(source.find<Linear>(position, "id")) +
                   source.number(source.find<Linear>(user, "id")) +
                   characters(source.string(source.find<Linear>(position, "text"))) +
                   characters(source.string(source.find<Linear>(user, "name")));
        }
        position = source.next(position);
    }
    return sum;
}

doc parse(ondemand_reader &reader, const std::string &input) {
    reader.read(input);
    auto parsed = doc::parse(reader);
    if (!parsed)
        throw std::runtime_error("parse failed");
    return std::move(*parsed);
}

void verify(const std::vector<fixture> &inputs) {
    for (const auto &input : inputs) {
        ondemand_reader reader;
        auto source = parse(reader, input.input);
        frozen compact(reader, std::string_view(reader.data(), reader.size()));
        equal(source, compact, 0);
        if (input.type == schema::config) {
            if (config_record(source) != project_config<false>(compact) ||
                config_record(source) != project_config<true>(compact))
                throw std::logic_error("config");
        } else if (input.type == schema::statuses) {
            auto position =
                static_cast<uint32_t>(compact.nodes[compact.find<false>(0, "statuses")].payload);
            for (const auto &value : source.at("statuses").get<array_t>()) {
                if (status_record(value) != project_status<false>(compact, position) ||
                    status_record(value) != project_status<true>(compact, position))
                    throw std::logic_error("status");
                position = compact.next(position);
            }
        }
        for (bool owning : {false, true}) {
            const auto expected = query_mutable(source, input.type, owning);
            if (expected != query_frozen<false>(compact, input.type, owning) ||
                expected != query_frozen<true>(compact, input.type, owning))
                throw std::logic_error("checksum");
            source.reset();
            if (expected != query_frozen<false>(compact, input.type, owning))
                throw std::logic_error("owner lifetime");
            source = parse(reader, input.input);
        }
    }
    for (const std::string input :
         {"null", "true", R"("")", R"({"":""})",
          "[0,-1,1.5,-0.0,-9223372036854775808,18446744073709551615]",
          R"({"\u006b":"first","k":"last","u":"\ud83d\ude00","e":"\u0000","a":[{},[],null]})"}) {
        ondemand_reader reader;
        auto source = parse(reader, input);
        frozen compact(reader, std::string_view(reader.data(), reader.size()));
        equal(source, compact, 0);
    }
    std::string temporary =
        R"({"name":"owned decoded string","version":"after input release","age":22})";
    ondemand_reader reader;
    auto source = parse(reader, temporary);
    frozen compact(reader, std::string_view(reader.data(), reader.size()));
    source.reset();
    temporary.clear();
    temporary.shrink_to_fit();
    if (project_config<false>(compact).name != "owned decoded string")
        throw std::logic_error("input lifetime");
    for (const std::string input : {R"({"name":"signed","version":"v","age":-22})",
                                    R"({"name":"floating","version":"v","age":22.0})"}) {
        auto value = parse(reader, input);
        frozen snapshot(reader, std::string_view(reader.data(), reader.size()));
        if (config_record(value) != project_config<false>(snapshot))
            throw std::logic_error("numeric conversion");
    }
    std::vector<std::string> invalid = {"",     " ",           "[",           "{",
                                        "[1,]", "{\"a\":}",    "true false",  "1e100000",
                                        "[01]", "\"\\uD800\"", "\"\\uDC00\"", "\"\\q\""};
    invalid.push_back(std::string("\"") + char(0xff) + "\"");
    invalid.push_back(std::string(260, '[') + "null" + std::string(260, ']'));
    for (const auto &input : invalid) {
        reader.read(input);
        auto expected = doc::parse(reader);
        frozen actual;
        auto status = actual.read(reader, input);
        if (bool(expected) != bool(status) || (!expected && expected.error() != status.error()))
            throw std::logic_error("error mismatch");
    }
}

volatile uint64_t result_sink = 0;
template <typename Function>
void time(const fixture &input, const char *phase, Function function) {
    size_t iterations = 1;
    double elapsed = 0;
    uint64_t checksum = 0;
    do {
        const auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < iterations; ++i)
            checksum += function();
        elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        if (elapsed >= 0.02)
            break;
        iterations *= 2;
    } while (iterations < 1U << 24);
    for (size_t sample = 0; sample < 3; ++sample) {
        const auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < iterations; ++i)
            checksum += function();
        const auto seconds =
            std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        std::cout << input.name << ',' << phase << ',' << sample << ',' << iterations << ','
                  << seconds * 1e9 / iterations << '\n';
    }
    result_sink = checksum;
}

#ifdef WJR_WITH_MATURE
struct simd_value {
    simdjson::dom::element value;
    simd_value at(std::string_view key) const { return {value.at_key(key).value()}; }
    std::string_view string() const { return value.get_string().value(); }
    uint64_t number() const { return value.get_uint64().value(); }
    size_t size() const { return value.get_array().value().size(); }
    template <typename F>
    void each(F &&function) const {
        for (auto child : value.get_array().value())
            function(simd_value{child});
    }
};
struct yy_value {
    yyjson_val *value;
    yy_value at(std::string_view key) const {
        auto *child = yyjson_obj_getn(value, key.data(), key.size());
        if (!child)
            throw std::out_of_range("yy field");
        return {child};
    }
    std::string_view string() const {
        if (!yyjson_is_str(value))
            throw std::logic_error("yy string");
        return {yyjson_get_str(value), yyjson_get_len(value)};
    }
    uint64_t number() const {
        if (!yyjson_is_uint(value))
            throw std::logic_error("yy uint fixture");
        return yyjson_get_uint(value);
    }
    size_t size() const { return yyjson_arr_size(value); }
    template <typename F>
    void each(F &&function) const {
        size_t index, length;
        yyjson_val *child;
        yyjson_arr_foreach(value, index, length, child) function(yy_value{child});
    }
};
template <typename View>
uint64_t query_mature(View source, schema type, bool owning) {
    if (type == schema::config) {
        if (owning)
            return consume(config_record(std::string(source.at("name").string()),
                                         std::string(source.at("version").string()),
                                         static_cast<int>(source.at("age").number())));
        return characters(source.at("name").string()) + characters(source.at("version").string()) +
               source.at("age").number();
    }
    uint64_t checksum = 0;
    if (type == schema::numbers) {
        std::vector<uint64_t> projected;
        if (owning)
            projected.reserve(source.size());
        source.each([&](View child) {
            const auto value = child.number();
            if (owning)
                projected.push_back(value);
            else
                checksum += value;
        });
        if (owning) {
            escape(projected);
            for (auto value : projected)
                checksum += value;
        }
    } else {
        source.at("statuses").each([&](View child) {
            auto user = child.at("user");
            if (owning)
                checksum += consume(status_record(
                    child.at("id").number(), std::string(child.at("text").string()),
                    user_record(user.at("id").number(), std::string(user.at("name").string()))));
            else
                checksum += child.at("id").number() + user.at("id").number() +
                            characters(child.at("text").string()) +
                            characters(user.at("name").string());
        });
    }
    return checksum;
}
void bench_mature(const fixture &input, bool reverse, bool measure = true) {
    simdjson::padded_string padded(input.input);
    simdjson::dom::parser parser;
    simdjson::dom::document held;
    auto root = parser.parse_into_document(held, padded).value();
    std::unique_ptr<yyjson_doc, decltype(&yyjson_doc_free)> yy(
        yyjson_read(input.input.data(), input.input.size(), 0), yyjson_doc_free);
    if (!yy)
        throw std::runtime_error("yy parse");
    ondemand_reader reader;
    auto normal = parse(reader, input.input);
    for (const bool owning : {false, true}) {
        const auto expected = query_mutable(normal, input.type, owning);
        if (query_mature(simd_value{root}, input.type, owning) != expected ||
            query_mature(yy_value{yyjson_doc_get_root(yy.get())}, input.type, owning) != expected)
            throw std::logic_error("mature fixture projection mismatch");
    }
    if (!measure)
        return;
    auto simd = [&] {
        simdjson::dom::document value;
        auto root = parser.parse_into_document(value, padded).value();
        return query_mature(simd_value{root}, input.type, true);
    };
    auto yyjson = [&] {
        std::unique_ptr<yyjson_doc, decltype(&yyjson_doc_free)> value(
            yyjson_read(input.input.data(), input.input.size(), 0), yyjson_doc_free);
        if (!value)
            throw std::runtime_error("yy parse");
        return query_mature(yy_value{yyjson_doc_get_root(value.get())}, input.type, true);
    };
    if (reverse) {
        time(input, "yyjson.e2e", yyjson);
        time(input, "simdjson.e2e", simd);
    } else {
        time(input, "simdjson.e2e", simd);
        time(input, "yyjson.e2e", yyjson);
    }
}
#endif

void bench(const fixture &input, bool reverse) {
    ondemand_reader reader;
    auto source = parse(reader, input.input);
    frozen compact(reader, std::string_view(reader.data(), reader.size()));
    auto mutable_query = [&](bool owning) { return query_mutable(source, input.type, owning); };
    auto binary_query = [&](bool owning) {
        return query_frozen<false>(compact, input.type, owning);
    };
    auto linear_query = [&](bool owning) {
        return query_frozen<true>(compact, input.type, owning);
    };
    for (bool owning : {false, true}) {
        const auto *m = owning ? "mutable.owning" : "mutable.borrowed";
        const auto *b = owning ? "binary.owning" : "binary.borrowed";
        const auto *l = owning ? "linear.owning" : "linear.borrowed";
        if (reverse) {
            time(input, l, [&] { return linear_query(owning); });
            time(input, b, [&] { return binary_query(owning); });
            time(input, m, [&] { return mutable_query(owning); });
        } else {
            time(input, m, [&] { return mutable_query(owning); });
            time(input, b, [&] { return binary_query(owning); });
            time(input, l, [&] { return linear_query(owning); });
        }
    }
    time(input, "mutable_parse_and_destroy", [&] {
        auto value = parse(reader, input.input);
        escape(value);
        return static_cast<uint64_t>(value.type());
    });
    time(input, "direct_parse_and_destroy", [&] {
        frozen value(reader, input.input);
        escape(value);
        return uint64_t(value.nodes[0].kind);
    });
    auto normal = [&] {
        auto value = parse(reader, input.input);
        return query_mutable(value, input.type, true);
    };
    auto frozen_pipeline = [&]<bool Linear>() {
        frozen snapshot(reader, input.input);
        return query_frozen<Linear>(snapshot, input.type, true);
    };
    if (reverse) {
        time(input, "linear.e2e", [&] { return frozen_pipeline.template operator()<true>(); });
        time(input, "binary.e2e", [&] { return frozen_pipeline.template operator()<false>(); });
        time(input, "mutable.e2e", normal);
    } else {
        time(input, "mutable.e2e", normal);
        time(input, "binary.e2e", [&] { return frozen_pipeline.template operator()<false>(); });
        time(input, "linear.e2e", [&] { return frozen_pipeline.template operator()<true>(); });
    }
}

#ifdef WJR_READONLY_COUNT
void counts(const fixture &input, bool compact) {
    if (accounting::live)
        throw std::logic_error("unbalanced allocation");
    ondemand_reader reader;
    reader.read(input.input);
    accounting::calls = accounting::requested = accounting::peak = 0;
    accounting::enabled = true;
    size_t retained = 0, query_calls = 0, query_bytes = 0, borrowed_calls = 0;
    {
        if (compact) {
            frozen value(reader, input.input);
            retained = accounting::live;
            const auto before_calls = accounting::calls, before_bytes = accounting::requested;
            result_sink = query_frozen<false>(value, input.type, true);
            query_calls = accounting::calls - before_calls;
            query_bytes = accounting::requested - before_bytes;
            const auto before_borrow = accounting::calls;
            result_sink = query_frozen<false>(value, input.type, false);
            borrowed_calls = accounting::calls - before_borrow;
        } else {
            auto parsed = doc::parse(reader);
            if (!parsed)
                std::abort();
            retained = accounting::live;
            const auto before_calls = accounting::calls, before_bytes = accounting::requested;
            result_sink = query_mutable(*parsed, input.type, true);
            query_calls = accounting::calls - before_calls;
            query_bytes = accounting::requested - before_bytes;
            const auto before_borrow = accounting::calls;
            result_sink = query_mutable(*parsed, input.type, false);
            borrowed_calls = accounting::calls - before_borrow;
        }
    }
    accounting::enabled = false;
    if (accounting::live)
        throw std::logic_error("unbalanced allocation");
    std::cout << input.name << ',' << (compact ? "direct" : "mutable") << ',' << accounting::calls
              << ',' << accounting::requested << ',' << retained << ',' << accounting::peak << ','
              << query_calls << ',' << query_bytes << ',' << borrowed_calls << '\n';
}
#endif
#if defined(WJR_WITH_MATURE) && defined(WJR_READONLY_COUNT)
void *yy_allocate(void *, size_t bytes) {
    try {
        return ::operator new(bytes);
    } catch (const std::bad_alloc &) {
        return nullptr;
    }
}
void yy_free(void *, void *pointer) { ::operator delete(pointer); }
void *yy_reallocate(void *context, void *pointer, size_t, size_t bytes) {
    if (!pointer)
        return yy_allocate(context, bytes);
    if (bytes > SIZE_MAX - sizeof(accounting::header))
        return nullptr;
    auto *old = static_cast<accounting::header *>(pointer) - 1;
    const bool counted = old->counted || accounting::enabled;
    const auto old_live = old->counted ? old->bytes : 0;
    auto *replacement = static_cast<accounting::header *>(
        std::realloc(old, sizeof(accounting::header) + (bytes ? bytes : 1)));
    if (!replacement)
        return nullptr;
    replacement->bytes = bytes;
    replacement->counted = counted;
    if (accounting::enabled) {
        ++accounting::calls;
        accounting::requested += bytes;
    }
    if (counted) {
        accounting::live = accounting::live - old_live + bytes;
        accounting::peak = std::max(accounting::peak, accounting::live);
    }
    return replacement + 1;
}
void counts_mature(const fixture &input, bool simd) {
    if (accounting::live)
        throw std::logic_error("mature allocation unbalanced before");
    simdjson::padded_string padded(input.input);
    simdjson::dom::parser parser;
    simdjson::dom::document warm;
    parser.parse_into_document(warm, padded).value();
    const yyjson_alc allocator{yy_allocate, yy_reallocate, yy_free, nullptr};
    accounting::calls = accounting::requested = accounting::peak = 0;
    accounting::enabled = true;
    size_t retained, query_calls, query_bytes, borrowed_calls;
    const auto count = [&](auto root) {
        retained = accounting::live;
        const auto before_calls = accounting::calls, before_bytes = accounting::requested;
        result_sink = query_mature(root, input.type, true);
        query_calls = accounting::calls - before_calls;
        query_bytes = accounting::requested - before_bytes;
        const auto before_borrow = accounting::calls;
        result_sink = query_mature(root, input.type, false);
        borrowed_calls = accounting::calls - before_borrow;
    };
    if (simd) {
        simdjson::dom::document value;
        count(simd_value{parser.parse_into_document(value, padded).value()});
    } else {
        std::unique_ptr<yyjson_doc, decltype(&yyjson_doc_free)> value(
            yyjson_read_opts(const_cast<char *>(input.input.data()), input.input.size(), 0,
                             &allocator, nullptr),
            yyjson_doc_free);
        if (!value)
            std::abort();
        count(yy_value{yyjson_doc_get_root(value.get())});
    }
    accounting::enabled = false;
    if (accounting::live)
        throw std::logic_error("mature allocation unbalanced after");
    std::cout << input.name << ',' << (simd ? "simdjson" : "yyjson") << ',' << accounting::calls
              << ',' << accounting::requested << ',' << retained << ',' << accounting::peak << ','
              << query_calls << ',' << query_bytes << ',' << borrowed_calls << '\n';
}
#endif

} // namespace

int main(int argc, char **argv) {
#ifdef WJR_WITH_MATURE
    const auto *haswell = simdjson::get_available_implementations()["haswell"];
    if (!haswell || !haswell->supported_by_runtime_system())
        throw std::runtime_error("haswell unavailable");
    simdjson::get_active_implementation() = haswell;
#endif
    const auto inputs = fixtures();
    verify(inputs);
#ifdef WJR_WITH_MATURE
    for (const auto &input : inputs)
        bench_mature(input, false, false);
#endif
    std::cerr
        << "verified=5fixtures+6boundary_cases+14invalid+input_release+mature_projections,cxx="
        << __cplusplus << ",owner_size=" << sizeof(frozen) << '\n';
    if (argc == 1)
        return 0;
#ifdef WJR_READONLY_COUNT
    std::cout
        << "fixture,backend,total_calls,total_requested,retained,peak,owning_query_calls,owning_query_bytes,borrowed_query_calls\n";
    for (const auto &input : inputs) {
        counts(input, false);
        counts(input, true);
    #ifdef WJR_WITH_MATURE
        counts_mature(input, true);
        counts_mature(input, false);
    #endif
    }
#else
    std::cout << "fixture,phase,sample,iterations,ns_per_batch\n";
    for (const auto &input : inputs) {
        bench(input, argc > 2);
    #ifdef WJR_WITH_MATURE
        bench_mature(input, argc > 2);
    #endif
    }
#endif
}
