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

/// @brief Private freeze-only model; it adds no parser or installed API.
struct frozen {
    std::vector<node> nodes;
    std::vector<member> members;
    std::vector<char> bytes;

    struct counts {
        size_t nodes = 0, members = 0, bytes = 0;
    };
    static void count(const doc &source, counts &out) {
        out.nodes = add(out.nodes, 1);
        if (source.is_string())
            out.bytes = add(out.bytes, source.get<string_t>().size());
        else if (source.is_object()) {
            const auto &object = source.get<object_t>();
            out.members = add(out.members, object.size());
            for (const auto &[key, value] : object) {
                out.bytes = add(out.bytes, key.size());
                count(value, out);
            }
        } else if (source.is_array()) {
            for (const auto &value : source.get<array_t>())
                count(value, out);
        }
    }

    explicit frozen(const doc *source) {
        counts total;
        count(*source, total);
        index32(total.nodes);
        index32(total.members);
        index32(total.bytes);
        nodes.reserve(total.nodes);
        members.reserve(total.members);
        bytes.reserve(total.bytes);
        append(*source);
    }

    uint32_t copy(std::string_view text) {
        const auto offset = index32(bytes.size());
        bytes.insert(bytes.end(), text.begin(), text.end());
        return offset;
    }

    uint32_t append(const doc &source) {
        const auto current = index32(nodes.size());
        nodes.push_back({static_cast<uint32_t>(source.type()), 0, 0});
        switch (source.type()) {
        case value_t::null:
            break;
        case value_t::boolean:
            nodes[current].payload = source.get<boolean_t>();
            break;
        case value_t::number_unsigned:
            nodes[current].payload = source.get<number_unsigned_t>();
            break;
        case value_t::number_signed:
            nodes[current].payload = std::bit_cast<uint64_t>(source.get<number_signed_t>());
            break;
        case value_t::number_float:
            nodes[current].payload = std::bit_cast<uint64_t>(source.get<number_float_t>());
            break;
        case value_t::string: {
            const auto &text = source.get<string_t>();
            nodes[current].extent = index32(text.size());
            nodes[current].payload = copy(text);
            break;
        }
        case value_t::object: {
            const auto &object = source.get<object_t>();
            const auto first = index32(members.size());
            members.resize(add(members.size(), object.size()));
            size_t position = first;
            for (const auto &[key, value] : object) {
                const auto offset = copy(key);
                const auto child = append(value);
                members[position++] = {offset, index32(key.size()), child};
            }
            nodes[current].extent = index32(object.size());
            nodes[current].payload = first | (uint64_t(index32(nodes.size())) << 32);
            break;
        }
        case value_t::array: {
            const auto &array = source.get<array_t>();
            const auto first = index32(nodes.size());
            for (const auto &value : array)
                append(value);
            nodes[current].extent = index32(array.size());
            nodes[current].payload = first | (uint64_t(index32(nodes.size())) << 32);
            break;
        }
        }
        return current;
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
        frozen compact(&source);
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
        frozen compact(&source);
        equal(source, compact, 0);
    }
    std::string temporary =
        R"({"name":"owned decoded string","version":"after input release","age":22})";
    ondemand_reader reader;
    auto source = parse(reader, temporary);
    frozen compact(&source);
    source.reset();
    temporary.clear();
    temporary.shrink_to_fit();
    if (project_config<false>(compact).name != "owned decoded string")
        throw std::logic_error("input lifetime");
    for (const std::string input : {R"({"name":"signed","version":"v","age":-22})",
                                    R"({"name":"floating","version":"v","age":22.0})"}) {
        auto value = parse(reader, input);
        frozen snapshot(&value);
        if (config_record(value) != project_config<false>(snapshot))
            throw std::logic_error("numeric conversion");
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

void bench(const fixture &input, bool reverse) {
    ondemand_reader reader;
    auto source = parse(reader, input.input);
    frozen compact(&source);
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
    time(input, "freeze_and_destroy", [&] {
        frozen value(&source);
        escape(value);
        return value.used();
    });
    auto normal = [&] {
        auto value = parse(reader, input.input);
        return query_mutable(value, input.type, true);
    };
    auto frozen_pipeline = [&]<bool Linear>() {
        auto value = parse(reader, input.input);
        frozen snapshot(&value);
        value.reset();
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
        auto parsed = doc::parse(reader);
        if (!parsed)
            std::abort();
        if (compact) {
            frozen value(std::addressof(*parsed));
            parsed->reset();
            retained = accounting::live;
            const auto before_calls = accounting::calls, before_bytes = accounting::requested;
            result_sink = query_frozen<false>(value, input.type, true);
            query_calls = accounting::calls - before_calls;
            query_bytes = accounting::requested - before_bytes;
            const auto before_borrow = accounting::calls;
            result_sink = query_frozen<false>(value, input.type, false);
            borrowed_calls = accounting::calls - before_borrow;
        } else {
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
    std::cout << input.name << ',' << (compact ? "frozen" : "mutable") << ',' << accounting::calls
              << ',' << accounting::requested << ',' << retained << ',' << accounting::peak << ','
              << query_calls << ',' << query_bytes << ',' << borrowed_calls << '\n';
}
#endif
} // namespace

int main(int argc, char **argv) {
    const auto inputs = fixtures();
    verify(inputs);
    std::cerr << "verified=5fixtures+6boundary_cases+input_release,cxx=" << __cplusplus
              << ",owner_size=" << sizeof(frozen) << '\n';
    if (argc == 1)
        return 0;
#ifdef WJR_READONLY_COUNT
    std::cout
        << "fixture,backend,total_calls,total_requested,retained,peak,owning_query_calls,owning_query_bytes,borrowed_query_calls\n";
    for (const auto &input : inputs) {
        counts(input, false);
        counts(input, true);
    }
#else
    std::cout << "fixture,phase,sample,iterations,ns_per_batch\n";
    for (const auto &input : inputs)
        bench(input, argc > 2);
#endif
}
