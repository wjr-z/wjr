#include "simdjson.hpp"
#include <algorithm>
#include <atomic>
#include <bit>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <new>
#include <optional>
#include <string>
#include <vector>
#include <wjr/json/document.hpp>
#include <wjr/json/immutable_document.hpp>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#else
    #include <sched.h>
#endif

namespace allocations {
bool enabled = false;
size_t calls = 0, total = 0, live = 0, peak = 0, fail_at = SIZE_MAX;
struct alignas(std::max_align_t) header {
    size_t bytes;
    bool counted;
};
void reset() {
    if (live)
        std::abort();
    calls = total = peak = 0;
}
} // namespace allocations
#ifdef COUNT_ALLOCATIONS
void *operator new(size_t size) {
    using namespace allocations;
    if (enabled && calls == fail_at)
        throw std::bad_alloc{};
    if (size > SIZE_MAX - sizeof(header))
        throw std::bad_alloc{};
    auto *raw = static_cast<header *>(std::malloc(sizeof(header) + std::max(size, size_t{1})));
    if (!raw)
        throw std::bad_alloc{};
    *raw = {size, enabled};
    if (enabled) {
        ++calls;
        total += size;
        live += size;
        peak = std::max(peak, live);
    }
    return raw + 1;
}
void *operator new[](size_t size) { return ::operator new(size); }
void operator delete(void *pointer) noexcept {
    if (!pointer)
        return;
    auto *raw = static_cast<allocations::header *>(pointer) - 1;
    if (raw->counted)
        allocations::live -= raw->bytes;
    std::free(raw);
}
void operator delete[](void *pointer) noexcept { ::operator delete(pointer); }
void operator delete(void *pointer, size_t) noexcept { ::operator delete(pointer); }
void operator delete[](void *pointer, size_t) noexcept { ::operator delete(pointer); }
#endif

using namespace wjr::json;
volatile uint64_t sink;
uint64_t hash(std::string_view string) {
    uint64_t value = 1469598103934665603ULL;
    for (unsigned char ch : string)
        value = (value ^ ch) * 1099511628211ULL;
    return value;
}
uint64_t consume(value_view value) {
    switch (value.type()) {
    case value_t::null:
        return 17;
    case value_t::boolean:
        return *value.get_boolean() + 19;
    case value_t::number_unsigned:
        return *value.get_unsigned();
    case value_t::number_signed:
        return std::bit_cast<uint64_t>(*value.get_signed());
    case value_t::number_float:
        return std::bit_cast<uint64_t>(*value.get_float());
    case value_t::string:
        return hash(*value.get_string());
    case value_t::array: {
        uint64_t sum = 23;
        const auto array = value.get_array();
        for (auto child : *array)
            sum = sum * 31 + consume(child);
        return sum;
    }
    case value_t::object: {
        uint64_t sum = 29;
        const auto object = value.get_object();
        for (auto field : *object)
            sum += hash(field.key) ^ consume(field.value);
        return sum;
    }
    }
    std::abort();
}
uint64_t consume(const document &value) {
    switch (value.type()) {
    case value_t::null:
        return 17;
    case value_t::boolean:
        return value.get<boolean_t>() + 19;
    case value_t::number_unsigned:
        return value.get<number_unsigned_t>();
    case value_t::number_signed:
        return std::bit_cast<uint64_t>(value.get<number_signed_t>());
    case value_t::number_float:
        return std::bit_cast<uint64_t>(value.get<number_float_t>());
    case value_t::string:
        return hash(value.get<string_t>());
    case value_t::array: {
        uint64_t sum = 23;
        for (auto &child : value.get<array_t>())
            sum = sum * 31 + consume(child);
        return sum;
    }
    case value_t::object: {
        uint64_t sum = 29;
        for (auto &field : value.get<object_t>())
            sum += hash(field.first) ^ consume(field.second);
        return sum;
    }
    }
    std::abort();
}
uint64_t consume(simdjson::dom::element value) {
    using type = simdjson::dom::element_type;
    switch (value.type()) {
    case type::NULL_VALUE:
        return 17;
    case type::BOOL:
        return bool(value) + 19;
    case type::UINT64:
        return uint64_t(value);
    case type::INT64:
        return std::bit_cast<uint64_t>(int64_t(value));
    case type::DOUBLE:
        return std::bit_cast<uint64_t>(double(value));
    case type::STRING:
        return hash(std::string_view(value));
    case type::ARRAY: {
        uint64_t sum = 23;
        for (auto child : simdjson::dom::array(value))
            sum = sum * 31 + consume(child);
        return sum;
    }
    case type::OBJECT: {
        uint64_t sum = 29;
        for (auto field : simdjson::dom::object(value))
            sum += hash(field.key) ^ consume(field.value);
        return sum;
    }
    }
    std::abort();
}

size_t used_bytes(const simdjson::dom::document &doc) {
    constexpr uint64_t mask = (uint64_t{1} << 56) - 1;
    const size_t end = doc.tape[0] & mask;
    size_t strings = 0;
    for (size_t i = 1; i != end; ++i) {
        const uint64_t word = doc.tape[i];
        const unsigned char kind = static_cast<unsigned char>(word >> 56);
        if (kind == '"') {
            const size_t offset = word & mask;
            uint32_t length;
            std::memcpy(&length, doc.string_buf.get() + offset, sizeof(length));
            strings = std::max(strings, offset + sizeof(length) + length + 1);
        } else if (kind == 'l' || kind == 'u' || kind == 'd') {
            ++i;
        }
    }
    return (end + 1) * sizeof(uint64_t) + strings;
}

struct fixture {
    std::string name, input;
    bool unique = true;
};
std::vector<fixture> fixtures(const char *twitter) {
    std::vector<fixture> out;
    out.push_back(
        {"small",
         R"({"service":"wjr","port":8080,"enabled":true,"routes":["/orders","/users"],"metadata":{"region":"test","version":"1"}})"});
    for (unsigned width : {8, 64, 256}) {
        std::string text = "[";
        for (unsigned i = 0; i != 512; ++i) {
            if (i)
                text += ',';
            text += '"' + std::string(width, 'a') + '"';
        }
        out.push_back({"strings" + std::to_string(width), text + ']'});
    }
    std::string numbers = "[";
    for (unsigned i = 0; i != 4096; ++i) {
        if (i)
            numbers += ',';
        numbers += std::to_string(i);
    }
    out.push_back({"numbers4096", numbers + ']'});
    std::string duplicate = "{";
    for (unsigned i = 0; i != 40; ++i)
        duplicate += "\"x\":\"" + std::string(4096, 'a') + "\",";
    out.push_back({"duplicate40", duplicate + "\"x\":\"last\"}", false});
    std::ifstream file(twitter, std::ios::binary);
    std::string text((std::istreambuf_iterator<char>(file)), {});
    if (text.empty())
        std::abort();
    out.push_back({"twitter", std::move(text)});
    for (unsigned width : {5, 40, 128}) {
        std::string object = "{";
        for (unsigned i = 0; i != width; ++i) {
            if (i)
                object += ',';
            object += "\"f" + std::to_string(1000 + i) + "\":" + std::to_string(i);
        }
        out.push_back({"fields" + std::to_string(width), object + '}'});
    }
    return out;
}

uint64_t query(value_view root, bool twitter, unsigned width) {
    if (twitter) {
        uint64_t sum = 0;
        const auto statuses = root.get_object()->find("statuses")->get_array();
        for (auto status : *statuses) {
            auto object = status.get_object();
            sum += hash(*object->find("text")->get_string());
            sum += hash(*object->find("user")->get_object()->find("screen_name")->get_string());
            sum += !object->find("__missing__");
        }
        return sum;
    }
    auto object = root.get_object();
    uint64_t sum = 0;
    for (unsigned i : {0U, width / 2, width - 1}) {
        const auto key = "f" + std::to_string(1000 + i);
        sum += *object->find(key)->get_unsigned();
    }
    return sum + !object->find("__missing__");
}
uint64_t query(const document &root, bool twitter, unsigned width) {
    if (twitter) {
        uint64_t sum = 0;
        for (auto &status : root.get<object_t>().at("statuses").get<array_t>()) {
            const auto &object = status.get<object_t>();
            sum += hash(object.at("text").get<string_t>());
            sum += hash(object.at("user").get<object_t>().at("screen_name").get<string_t>());
            sum += object.find("__missing__") == object.end();
        }
        return sum;
    }
    const auto &object = root.get<object_t>();
    uint64_t sum = 0;
    for (unsigned i : {0U, width / 2, width - 1}) {
        const auto key = "f" + std::to_string(1000 + i);
        sum += object.at(key).get<number_unsigned_t>();
    }
    return sum + (object.find("__missing__") == object.end());
}
uint64_t query(simdjson::dom::element root, bool twitter, unsigned width) {
    if (twitter) {
        uint64_t sum = 0;
        for (auto status : simdjson::dom::array(root["statuses"])) {
            sum += hash(std::string_view(status["text"]));
            sum += hash(std::string_view(status["user"]["screen_name"]));
            sum += status["__missing__"].error() == simdjson::NO_SUCH_FIELD;
        }
        return sum;
    }
    uint64_t sum = 0;
    for (unsigned i : {0U, width / 2, width - 1}) {
        const auto key = "f" + std::to_string(1000 + i);
        sum += uint64_t(root[key]);
    }
    return sum + (root["__missing__"].error() == simdjson::NO_SUCH_FIELD);
}

template <class Callback>
double timed(size_t iterations, Callback callback) {
    const auto start = std::chrono::steady_clock::now();
    uint64_t sum = 0;
    for (size_t i = 0; i != iterations; ++i) {
        std::atomic_signal_fence(std::memory_order_seq_cst);
        sum += callback();
    }
    sink = sum;
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

void pin() {
#ifdef _WIN32
    if (!SetThreadAffinityMask(GetCurrentThread(), DWORD_PTR{1} << 2))
        std::abort();
#else
    cpu_set_t allowed;
    CPU_ZERO(&allowed);
    sched_getaffinity(0, sizeof(allowed), &allowed);
    for (int cpu = 0; cpu != CPU_SETSIZE; ++cpu) {
        if (CPU_ISSET(cpu, &allowed)) {
            CPU_ZERO(&allowed);
            CPU_SET(cpu, &allowed);
            if (sched_setaffinity(0, sizeof(allowed), &allowed))
                std::abort();
            break;
        }
    }
#endif
}

int main(int argc, char **argv) {
    if (argc != 2)
        return 1;
    pin();
    std::cerr << "simdjson=" << SIMDJSON_VERSION
              << ",active=" << simdjson::get_active_implementation()->name()
              << ",owner=" << sizeof(immutable_document) << ",view=" << sizeof(value_view) << '\n';
    auto inputs = fixtures(argv[1]);
#ifdef COUNT_ALLOCATIONS
    std::cout
        << "fixture,backend,input_bytes,calls,cumulative_bytes,peak_bytes,parser_and_output_bytes,output_bytes,logical_used_bytes\n";
    for (const auto &input : inputs) {
        allocations::reset();
        allocations::enabled = true;
        std::optional<immutable_document> output;
        size_t with_reader;
        {
            ondemand_reader reader;
            reader.read(input.input);
            auto value = immutable_document::parse(reader);
            if (!value)
                std::abort();
            output.emplace(std::move(*value));
            with_reader = allocations::live;
        }
        allocations::enabled = false;
        std::cout << input.name << ",immutable," << input.input.size() << ',' << allocations::calls
                  << ',' << allocations::total << ',' << allocations::peak << ',' << with_reader
                  << ',' << allocations::live << ',' << output->storage_size() << '\n';
        const auto expected = consume(output->root());
        output.reset();
        allocations::reset();
        allocations::enabled = true;
        std::optional<document> mutable_output;
        {
            ondemand_reader reader;
            reader.read(input.input);
            auto value = document::parse(reader);
            if (!value)
                std::abort();
            mutable_output.emplace(std::move(*value));
            with_reader = allocations::live;
        }
        allocations::enabled = false;
        if (consume(*mutable_output) != expected)
            std::abort();
        std::cout << input.name << ",mutable," << input.input.size() << ',' << allocations::calls
                  << ',' << allocations::total << ',' << allocations::peak << ',' << with_reader
                  << ',' << allocations::live << ",0\n";
        mutable_output.reset();
        allocations::reset();
        std::optional<simdjson::padded_string> padded(std::in_place, input.input);
        allocations::enabled = true;
        std::optional<simdjson::dom::document> simd_output;
        simd_output.emplace();
        {
            simdjson::dom::parser parser;
            auto value = parser.parse_into_document(*simd_output, *padded);
            if (value.error())
                std::abort();
            with_reader = allocations::live;
        }
        allocations::enabled = false;
        padded.reset();
        if (input.unique && consume(simd_output->root()) != expected)
            std::abort();
        std::cout << input.name << ",simdjson," << input.input.size() << ',' << allocations::calls
                  << ',' << allocations::total << ',' << allocations::peak << ',' << with_reader
                  << ',' << allocations::live << ',' << used_bytes(*simd_output) << '\n';
        simd_output.reset();
    }
    for (const auto &input : inputs) {
        ondemand_reader reader;
        reader.read(input.input);
        allocations::reset();
        size_t attempted = 0;
        for (size_t fail = 0;; ++fail) {
            allocations::fail_at = fail;
            allocations::enabled = true;
            bool complete = false;
            try {
                auto output = immutable_document::parse(reader);
                complete = bool(output);
            } catch (const std::bad_alloc &) {
            }
            allocations::enabled = false;
            if (allocations::live)
                std::abort();
            ++attempted;
            allocations::reset();
            reader.read(input.input);
            if (complete)
                break;
        }
        allocations::fail_at = SIZE_MAX;
        std::cerr << "allocation-failure," << input.name << ",attempts=" << attempted
                  << ",clean=1\n";
    }
#else
    std::cout
        << "fixture,phase,repeat,iterations,mutable_seconds,immutable_seconds,simdjson_seconds\n";
    for (const auto &input : inputs) {
        ondemand_reader reader;
        reader.read(input.input);
        simdjson::padded_string padded(input.input);
        simdjson::dom::parser parser;
        const bool twitter = input.name == "twitter";
        const bool query_case = twitter || input.name.find("fields") == 0;
        const unsigned width =
            twitter || !query_case ? 0 : unsigned(std::stoi(input.name.substr(6)));
        unsigned query_repeats = 0;
        auto normal = [&] {
            reader.read(input.input);
            auto value = document::parse(reader);
            if (!value)
                std::abort();
            uint64_t sum = consume(*value);
            for (unsigned i = 0; i != query_repeats; ++i)
                sum += query(*value, twitter, width);
            return sum;
        };
        auto compact = [&] {
            reader.read(input.input);
            auto value = immutable_document::parse(reader);
            if (!value)
                std::abort();
            uint64_t sum = consume(value->root());
            for (unsigned i = 0; i != query_repeats; ++i)
                sum += query(value->root(), twitter, width);
            return sum;
        };
        auto strong = [&] {
            simdjson::dom::document doc;
            auto value = parser.parse_into_document(doc, padded);
            if (value.error())
                std::abort();
            uint64_t sum = consume(value.value());
            for (unsigned i = 0; i != query_repeats; ++i)
                sum += query(value.value(), twitter, width);
            return sum;
        };
        const auto expected = normal();
        if (compact() != expected || (input.unique && strong() != expected))
            std::abort();
        const auto calibration = timed(8, normal);
        const size_t iterations =
            std::clamp(size_t(0.08 * 8 / calibration), size_t{4}, size_t{1} << 19);
        for (int repeat = 0; repeat != 5; ++repeat) {
            double a, b, c;
            if (repeat % 2) {
                c = timed(iterations, strong);
                b = timed(iterations, compact);
                a = timed(iterations, normal);
            } else {
                a = timed(iterations, normal);
                b = timed(iterations, compact);
                c = timed(iterations, strong);
            }
            std::cout << input.name << ",parse_consume_destroy," << repeat << ',' << iterations
                      << ',' << a << ',' << b << ',' << c << '\n';
        }
        if (!query_case)
            continue;
        query_repeats = 64;
        if (normal() != compact() || normal() != strong())
            std::abort();
        const size_t consumer_count =
            std::clamp(size_t(0.08 * 4 / timed(4, normal)), size_t{4}, size_t{1} << 19);
        for (int repeat = 0; repeat != 5; ++repeat) {
            double a, b, c;
            if (repeat % 2) {
                c = timed(consumer_count, strong);
                b = timed(consumer_count, compact);
                a = timed(consumer_count, normal);
            } else {
                a = timed(consumer_count, normal);
                b = timed(consumer_count, compact);
                c = timed(consumer_count, strong);
            }
            std::cout << input.name << ",parse_scan_query64_destroy," << repeat << ','
                      << consumer_count << ',' << a << ',' << b << ',' << c << '\n';
        }
        reader.read(input.input);
        auto mutable_doc = document::parse(reader);
        reader.read(input.input);
        auto immutable_doc = immutable_document::parse(reader);
        simdjson::dom::document simd_doc;
        if (parser.parse_into_document(simd_doc, padded).error())
            std::abort();
        const auto check = query(*mutable_doc, twitter, width);
        if (query(immutable_doc->root(), twitter, width) != check ||
            query(simd_doc.root(), twitter, width) != check)
            std::abort();
        auto a_query = [&] { return query(*mutable_doc, twitter, width); };
        auto b_query = [&] { return query(immutable_doc->root(), twitter, width); };
        auto c_query = [&] { return query(simd_doc.root(), twitter, width); };
        const auto base = timed(16, a_query);
        const size_t count = std::clamp(size_t(0.08 * 16 / base), size_t{4}, size_t{1} << 20);
        for (int repeat = 0; repeat != 5; ++repeat) {
            double a, b, c;
            if (repeat % 2) {
                c = timed(count, c_query);
                b = timed(count, b_query);
                a = timed(count, a_query);
            } else {
                a = timed(count, a_query);
                b = timed(count, b_query);
                c = timed(count, c_query);
            }
            std::cout << input.name << ",query_only," << repeat << ',' << count << ',' << a << ','
                      << b << ',' << c << '\n';
        }
    }
#endif
}
