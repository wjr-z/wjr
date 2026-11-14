#include <atomic>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <locale>
#include <string>
#ifdef _WIN32
    #define NOMINMAX
    #include <windows.h>
#else
    #include <sched.h>
#endif
#include <wjr/biginteger.hpp>
#include <wjr/json/document.hpp>

extern "C" uint64_t observe_bytes(const char *, size_t) noexcept;
volatile uint64_t output_sink = 0;
#ifdef GROWING_ALLOCATOR
using allocator = wjr::stack_allocator_detail::growing_allocator<char>;
#else
using allocator = wjr::weak_stack_allocator<char>;
#endif
using buffer_type = wjr::vector<char, allocator>;

template <class Format>
double time_format(size_t iterations, Format &format) {
    uint64_t sum = 0;
    auto begin = std::chrono::steady_clock::now();
    for (size_t index = 0; index != iterations; ++index) {
        std::atomic_signal_fence(std::memory_order_seq_cst);
        wjr::unique_stack_allocator owner;
        buffer_type buffer{allocator(owner)};
        buffer.clear_if_reserved(256);
        format(buffer);
        sum += observe_bytes(buffer.data(), buffer.size());
    }
    auto end = std::chrono::steady_clock::now();
    output_sink = sum;
    return std::chrono::duration<double>(end - begin).count();
}

template <class Format>
void fixture(const std::string &name, const std::string &expected, Format format) {
    {
        wjr::unique_stack_allocator owner;
        buffer_type buffer{allocator(owner)};
        buffer.clear_if_reserved(256);
        format(buffer);
        if (std::string_view(buffer.data(), buffer.size()) != expected)
            std::abort();
    }
    const auto calibration = time_format(128, format);
    const size_t iterations = std::max(size_t{128}, size_t(0.02 * 128 / calibration));
    for (int repeat = 0; repeat != 7; ++repeat) {
        const auto seconds = time_format(iterations, format);
        std::cout << name << ',' << expected.size() << ',' << repeat << ',' << iterations << ','
                  << seconds << ',' << seconds * 1e9 / iterations << '\n';
    }
}

int main() {
#ifdef _WIN32
    if (!SetThreadAffinityMask(GetCurrentThread(), 1))
        return 1;
#else
    cpu_set_t allowed;
    CPU_ZERO(&allowed);
    if (sched_getaffinity(0, sizeof(allowed), &allowed) != 0)
        return 1;
    int selected = 0;
    while (selected != CPU_SETSIZE && !CPU_ISSET(selected, &allowed))
        ++selected;
    if (selected == CPU_SETSIZE)
        return 1;
    cpu_set_t pinned;
    CPU_ZERO(&pinned);
    CPU_SET(selected, &pinned);
    if (sched_setaffinity(0, sizeof(pinned), &pinned) != 0)
        return 1;
    std::cerr << "cpu=" << selected << '\n';
#endif
    std::cout << "fixture,bytes,repeat,iterations,seconds,ns_per_call\n";
    for (size_t count : {16, 64, 128, 256, 512, 1024}) {
        std::string input = "[";
        for (size_t index = 0; index != count; ++index) {
            if (index)
                input += ',';
            input += "{\"id\":" + std::to_string(index) + ",\"price\":1.25,\"name\":\"abcdefgh\"}";
        }
        input += ']';
        wjr::json::ondemand_reader reader;
        reader.read(input);
        auto document = wjr::json::document::parse(reader);
        if (!document)
            return 1;
        fixture("json-" + std::to_string(count), document->to_string(),
                [&](auto &buffer) { document->dump_impl(buffer); });
    }
    for (size_t bits : {1024, 4096, 16384, 65536}) {
        wjr::biginteger number = (wjr::biginteger(1) << bits) - 1;
        for (unsigned base : {10, 16}) {
            auto format = [&](auto &buffer) {
                wjr::biginteger_to_chars(std::back_inserter(buffer), number.data(), number.size(),
                                         base);
            };
            const auto expected =
                base == 16 ? std::string(bits / 4, 'f') : fmt::format("{}", number);
            fixture("bigint-" + std::to_string(bits) + "-base" + std::to_string(base), expected,
                    format);
        }
    }
}
