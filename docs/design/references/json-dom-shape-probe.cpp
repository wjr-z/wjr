#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <new>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <wjr/json/document.hpp>

namespace {
struct alignas(std::max_align_t) allocation_header {
    size_t bytes;
    bool counted;
};
bool enabled = false;
size_t calls = 0, requested = 0, live = 0, peak = 0;
struct candidate_record {
    uint32_t kind, extent;
    uint64_t payload;
};
struct candidate_member {
    uint32_t offset, length, child;
};
struct candidate_view {
    const void *owner;
    uint32_t index;
};
static_assert(sizeof(candidate_record) == 16 && sizeof(candidate_member) == 12);
static_assert(std::is_trivially_copyable_v<candidate_record>);
static_assert(std::is_trivially_copyable_v<candidate_member>);
static_assert(std::is_trivially_copyable_v<candidate_view>);
} // namespace
void *operator new(size_t bytes) {
    if (bytes > std::numeric_limits<size_t>::max() - sizeof(allocation_header))
        throw std::bad_alloc();
    auto *header = static_cast<allocation_header *>(
        std::malloc(sizeof(allocation_header) + (bytes ? bytes : 1)));
    if (!header)
        throw std::bad_alloc();
    header->bytes = bytes;
    header->counted = enabled;
    if (enabled) {
        ++calls;
        requested += bytes;
        live += bytes;
        peak = std::max(peak, live);
    }
    return header + 1;
}
void *operator new[](size_t bytes) { return ::operator new(bytes); }
void operator delete(void *pointer) noexcept {
    if (!pointer)
        return;
    auto *header = static_cast<allocation_header *>(pointer) - 1;
    if (header->counted)
        live -= header->bytes;
    std::free(header);
}
void operator delete[](void *pointer) noexcept { ::operator delete(pointer); }
void operator delete(void *pointer, size_t) noexcept { ::operator delete(pointer); }
void operator delete[](void *pointer, size_t) noexcept { ::operator delete(pointer); }

struct shape {
    size_t values = 0, objects = 0, arrays = 0, strings = 0, numbers = 0, members = 0;
    size_t key_bytes = 0, string_bytes = 0, max_members = 0, max_array = 0;
    size_t array_size = 0, array_capacity = 0, max_depth = 0;
    std::map<size_t, size_t> object_sizes;
};

void count(const wjr::json::document &value, shape &out, size_t depth = 0) {
    using namespace wjr::json;
    ++out.values;
    out.max_depth = std::max(out.max_depth, depth);
    if (value.is_string()) {
        ++out.strings;
        out.string_bytes += value.get<string_t>().size();
    } else if (value.is_object()) {
        ++out.objects;
        const auto &object = value.get<object_t>();
        out.members += object.size();
        ++out.object_sizes[object.size()];
        out.max_members = std::max(out.max_members, object.size());
        for (const auto &[key, child] : object) {
            out.key_bytes += key.size();
            count(child, out, depth + 1);
        }
    } else if (value.is_array()) {
        ++out.arrays;
        const auto &array = value.get<array_t>();
        out.array_size += array.size();
        out.array_capacity += array.capacity();
        out.max_array = std::max(out.max_array, array.size());
        for (const auto &child : array)
            count(child, out, depth + 1);
    } else if (value.type() >= value_t::number_unsigned && value.type() <= value_t::number_float) {
        ++out.numbers;
    }
}

int main() {
    using doc = wjr::json::document;
    std::cout << "sizes,value=" << sizeof(doc) << ",string=" << sizeof(doc::string_type)
              << ",object=" << sizeof(doc::object_type) << ",array=" << sizeof(doc::array_type)
              << ",candidate_record=" << sizeof(candidate_record)
              << ",candidate_member=" << sizeof(candidate_member)
              << ",candidate_view=" << sizeof(candidate_view) << '\n';
    std::vector<std::pair<std::string, std::string>> inputs;
    inputs.emplace_back(
        "small_config",
        R"({"service":"wjr","port":8080,"enabled":true,"routes":["/orders","/users"],"metadata":{"region":"test","version":"1"}})");
    for (size_t width : {8, 64, 256}) {
        std::string text = "[";
        for (size_t i = 0; i < 512; ++i) {
            if (i)
                text += ',';
            text += '"' + std::string(width, 'a') + '"';
        }
        inputs.emplace_back("strings_" + std::to_string(width), text + ']');
    }
    std::string numbers = "[";
    for (size_t i = 0; i < 4096; ++i) {
        if (i)
            numbers += ',';
        numbers += std::to_string(i);
    }
    inputs.emplace_back("numbers_4096", numbers + ']');
    std::ifstream file("test/data/success/twitter.json");
    inputs.emplace_back("twitter", std::string((std::istreambuf_iterator<char>(file)), {}));
    std::cout
        << "fixture,input_bytes,values,objects,arrays,strings,numbers,members,key_bytes,string_bytes,max_members,max_array,array_elements,array_capacity,max_depth,allocation_calls,requested_bytes,live_bytes,peak_bytes\n";
    for (const auto &[name, input] : inputs) {
        if (live)
            return 2;
        wjr::json::ondemand_reader reader;
        reader.read(input);
        calls = requested = live = peak = 0;
        enabled = true;
        auto parsed = doc::parse(reader);
        enabled = false;
        if (!parsed)
            return 1;
        shape s;
        count(*parsed, s);
        std::cout << name << ',' << input.size() << ',' << s.values << ',' << s.objects << ','
                  << s.arrays << ',' << s.strings << ',' << s.numbers << ',' << s.members << ','
                  << s.key_bytes << ',' << s.string_bytes << ',' << s.max_members << ','
                  << s.max_array << ',' << s.array_size << ',' << s.array_capacity << ','
                  << s.max_depth << ',' << calls << ',' << requested << ',' << live << ',' << peak
                  << '\n';
        std::cerr << name << " object_histogram:";
        for (const auto &[size, count] : s.object_sizes)
            std::cerr << ' ' << size << ':' << count;
        std::cerr << '\n';
    }
    return live == 0 ? 0 : 2;
}
