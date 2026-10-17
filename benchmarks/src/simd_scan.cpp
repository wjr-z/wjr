#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/math/find.hpp>

#if defined(WJR_BENCHMARK_XSIMD)
    #include <wjr/simd/xsimd.hpp>
#endif

namespace {

using value_type = std::uint64_t;
constexpr value_type needle = 0x0123456789abcdefULL;

struct wjr_scan {
    static std::size_t apply(const value_type *src, value_type value, std::size_t n) noexcept {
        return wjr::find_n(src, value, n);
    }
};

struct loop_scan {
    static std::size_t apply(const value_type *src, value_type value, std::size_t n) noexcept {
        for (std::size_t i = 0; i < n; ++i) {
            if (src[i] == value) {
                return i;
            }
        }
        return n;
    }
};

#if defined(WJR_BENCHMARK_XSIMD)
struct xsimd_scan {
    static std::size_t apply(const value_type *src, value_type value, std::size_t n) noexcept {
        return wjr::simd::xsimd_backend::find_equal(src, value, n);
    }
};
#endif

value_type *aligned_start(std::vector<value_type> &storage, std::size_t offset) {
    const auto address = reinterpret_cast<std::uintptr_t>(storage.data());
    return storage.data() + ((64 - address % 64) % 64) / sizeof(value_type) + offset;
}

std::size_t hit_position(std::size_t n, int pattern) {
    if (n == 0 || pattern == 0) {
        return n;
    }
    return pattern == 1 ? 0 : (pattern == 2 ? n / 2 : n - 1);
}

template <typename Backend>
void verify_scan(const value_type *src, value_type value, std::size_t n, std::size_t expected) {
    if (Backend::apply(src, value, n) != expected) {
        std::fprintf(stderr, "scan validation failed: n=%zu, expected=%zu\n", n, expected);
        std::exit(EXIT_FAILURE);
    }
}

template <typename Backend>
void scan(benchmark::State &state) {
    const auto n = static_cast<std::size_t>(state.range(0));
    const auto offset = static_cast<std::size_t>(state.range(1));
    const auto expected = hit_position(n, static_cast<int>(state.range(2)));
    std::vector<value_type> storage(n + 16, 0);
    auto *src = aligned_start(storage, offset);
    if (expected < n) {
        src[expected] = needle;
    }
    value_type value = needle;
    benchmark::DoNotOptimize(src);
    benchmark::DoNotOptimize(value);
    verify_scan<Backend>(src, value, n, expected);
    for (auto _ : state) {
        auto found = Backend::apply(src, value, n);
        benchmark::DoNotOptimize(found);
        benchmark::ClobberMemory();
    }
    // Logical elements examined, not actual bytes loaded by the implementation.
    const auto examined = expected < n ? expected + 1 : n;
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(examined));
}

struct scan_case {
    const value_type *src;
    std::size_t count;
};

template <typename Backend>
void mixed_scan(benchmark::State &state) {
    constexpr std::array<std::size_t, 16> lengths = {0,  1,  2,  3,  4,  7,  8,  9,
                                                     15, 16, 17, 31, 32, 33, 64, 300};
    // Every (length, pattern, offset) has immutable storage outside the timed loop.
    std::array<std::array<std::array<std::vector<value_type>, 2>, 4>, 16> buffers;
    std::array<std::array<std::array<const value_type *, 2>, 4>, 16> inputs{};
    for (std::size_t i = 0; i < lengths.size(); ++i) {
        for (int pattern = 0; pattern < 4; ++pattern) {
            for (std::size_t offset = 0; offset < 2; ++offset) {
                auto &buffer = buffers[i][pattern][offset];
                buffer.assign(lengths[i] + 16, 0);
                auto *src = aligned_start(buffer, offset);
                const auto position = hit_position(lengths[i], pattern);
                if (position < lengths[i]) {
                    src[position] = needle;
                }
                inputs[i][pattern][offset] = src;
                verify_scan<Backend>(src, needle, lengths[i], position);
            }
        }
    }
    std::array<scan_case, 4096> cases{};
    for (std::size_t i = 0; i < cases.size(); ++i) {
        const auto length_index = i % lengths.size();
        const auto pattern = (i / lengths.size()) % 4;
        const auto offset = (i / (lengths.size() * 4)) % 2;
        cases[i] = {inputs[length_index][pattern][offset], lengths[length_index]};
    }
    std::mt19937 random(42);
    std::shuffle(cases.begin(), cases.end(), random);
    auto *sequence = cases.data();
    value_type value = needle;
    benchmark::DoNotOptimize(sequence);
    benchmark::DoNotOptimize(value);
    std::size_t position = 0;
    for (auto _ : state) {
        const auto i = position++ % cases.size();
        auto found = Backend::apply(sequence[i].src, value, sequence[i].count);
        benchmark::DoNotOptimize(found);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations()); // Calls; includes sequence lookup.
}

void scan_sizes(benchmark::internal::Benchmark *bench) {
    for (int64_t n : {0, 1, 2, 3, 4, 7, 8, 9, 16, 17, 31, 32, 33, 64, 300, 1024, 65536}) {
        for (int64_t offset : {0, 1}) {
            for (int64_t pattern : {0, 1, 2, 3}) {
                bench->Args({n, offset, pattern});
            }
        }
    }
    bench->ArgNames({"n", "offset", "hit"}); // 0: none, 1: first, 2: middle, 3: last.
    bench->UseRealTime();
}

#define WJR_REGISTER_SCAN(BACKEND, NAME)                                                           \
    BENCHMARK_TEMPLATE(scan, BACKEND)->Name("scan/" NAME "/u64")->Apply(scan_sizes);               \
    BENCHMARK_TEMPLATE(mixed_scan, BACKEND)->Name("mixed_scan/" NAME "/u64")->UseRealTime()

WJR_REGISTER_SCAN(wjr_scan, "wjr");
WJR_REGISTER_SCAN(loop_scan, "loop");
#if defined(WJR_BENCHMARK_XSIMD)
WJR_REGISTER_SCAN(xsimd_scan, "xsimd");
#endif

#undef WJR_REGISTER_SCAN

} // namespace
