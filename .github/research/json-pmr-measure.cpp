#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory_resource>
#include <string>
#include <string_view>
#include <vector>
#include <wjr/json/pmr.hpp>

using regular = wjr::json::document;
using pooled = wjr::json::pmr_document;
using reader = wjr::json::ondemand_reader;

struct workload {
    std::string name;
    std::string input;
};

std::vector<workload> fixtures() {
    std::vector<workload> all;
    all.push_back(
        {"small_config",
         R"({"service":"wjr","port":8080,"enabled":true,"routes":["/orders","/users"],"metadata":{"region":"test","version":"1"}})"});
    for (size_t width : {size_t{8}, size_t{64}, size_t{256}}) {
        std::string input = "[";
        for (size_t i = 0; i < 512; ++i) {
            if (i)
                input += ',';
            input += '"' + std::string(width, 'a') + '"';
        }
        all.push_back({"strings_" + std::to_string(width), input + ']'});
    }
    {
        std::string input = "[";
        for (size_t i = 0; i < 128; ++i) {
            if (i)
                input += ',';
            input +=
                "{\"id\":" + std::to_string(i) +
                R"(,"name":"sample","text":"alpha\\beta\n\u00e9","tags":["a","b"],"enabled":true})";
        }
        all.push_back({"objects_128", input + ']'});
    }
    {
        std::string input = "[";
        for (size_t i = 0; i < 4096; ++i) {
            if (i)
                input += ',';
            input += std::to_string(i);
        }
        all.push_back({"numbers_4096", input + ']'});
    }
    all.push_back({"root_string_65536", '"' + std::string(65536, 'a') + '"'});
    std::ifstream file("test/data/success/twitter.json");
    if (file)
        all.push_back({"twitter", std::string((std::istreambuf_iterator<char>(file)), {})});
    return all;
}

template <class Document>
WJR_INTRINSIC_INLINE size_t parse_once(reader &rd, const std::string &input,
                                       std::pmr::memory_resource *resource) {
    rd.read(input);
    auto parsed = [&] {
        if constexpr (std::is_same_v<Document, regular>)
            return Document::parse(rd);
        else
            return Document::parse_with_allocator(rd, typename Document::allocator_type(resource));
    }();
    if (!parsed)
        std::abort();
    return static_cast<size_t>(parsed->type());
}

volatile size_t sink = 0;
enum class backend { regular, new_delete, pool_warm, pool_cold };

template <backend Backend>
WJR_NOINLINE void measure(const workload &data) {
    reader rd;
    rd.read(data.input);
    const size_t iterations = std::max<size_t>(8, (4 * 1024 * 1024) / data.input.size());
    size_t checksum = 0;
    auto output = [&](double seconds, const char *label) {
        sink = checksum;
        std::cout << data.name << ',' << label << ','
                  << data.input.size() * iterations / seconds / 1e9 << '\n';
    };
    if constexpr (Backend == backend::regular || Backend == backend::new_delete) {
        using Document = std::conditional_t<Backend == backend::regular, regular, pooled>;
        auto *resource = std::pmr::new_delete_resource();
        (void)parse_once<Document>(rd, data.input, resource);
        const auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < iterations; ++i)
            checksum += parse_once<Document>(rd, data.input, resource);
        const auto seconds =
            std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        output(seconds, Backend == backend::regular ? "default" : "pmr_new_delete");
    } else if constexpr (Backend == backend::pool_warm) {
        std::pmr::unsynchronized_pool_resource resource;
        (void)parse_once<pooled>(rd, data.input, &resource);
        const auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < iterations; ++i)
            checksum += parse_once<pooled>(rd, data.input, &resource);
        const auto seconds =
            std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        output(seconds, "pmr_pool_warm");
    } else {
        const auto start = std::chrono::steady_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            std::pmr::unsynchronized_pool_resource resource;
            checksum += parse_once<pooled>(rd, data.input, &resource);
        }
        const auto seconds =
            std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        output(seconds, "pmr_pool_cold");
    }
}

int main(int argc, char **argv) {
    const auto data = fixtures();
    std::cerr << "cxx=" << __cplusplus << ",document=" << sizeof(regular)
              << ",pmr_document=" << sizeof(pooled) << '\n';
    for (const auto &input : data) {
        reader rd;
        rd.read(input.input);
        const auto reference = regular::parse(rd);
        rd.read(input.input);
        const auto candidate = pooled::parse_with_allocator(
            rd, pooled::allocator_type(std::pmr::new_delete_resource()));
        if (!reference || !candidate || reference->to_string() != candidate->to_string())
            return 1;
    }
    if (argc == 1)
        return 0;
    for (const auto &input : data) {
        if (argc > 2) {
            measure<backend::pool_cold>(input);
            measure<backend::pool_warm>(input);
            measure<backend::new_delete>(input);
            measure<backend::regular>(input);
        } else {
            measure<backend::regular>(input);
            measure<backend::new_delete>(input);
            measure<backend::pool_warm>(input);
            measure<backend::pool_cold>(input);
        }
    }
}
