#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/math/set.hpp>

#if defined(WJR_BENCHMARK_XSIMD)
    #include <xsimd/xsimd.hpp>
#endif

namespace {

struct wjr_fill {
    template <typename T>
    static void apply(T *dst, T value, size_t n) noexcept {
        wjr::set_n(dst, value, n);
    }
};

struct loop_fill {
    template <typename T>
    static void apply(T *dst, T value, size_t n) noexcept {
        // Deliberately allow the compiler to vectorize or recognize memset.
        for (size_t i = 0; i < n; ++i) {
            dst[i] = value;
        }
    }
};

#if defined(WJR_BENCHMARK_XSIMD)
struct xsimd_fill {
    template <typename T>
    static void apply(T *dst, T value, size_t n) noexcept {
        using batch = xsimd::batch<T>;
        const batch values(value);
        size_t i = 0;
        for (; n - i >= batch::size; i += batch::size) {
            values.store_unaligned(dst + i);
        }
        for (; i < n; ++i) {
            dst[i] = value;
        }
    }
};
#endif

template <typename T, typename Backend>
void label_backend(benchmark::State &state) {
#if defined(WJR_BENCHMARK_XSIMD)
    if constexpr (std::is_same_v<Backend, xsimd_fill>) {
        state.SetLabel(std::string(xsimd::default_arch::name()) +
                       "; lanes=" + std::to_string(xsimd::batch<T>::size) + "; xsimd=14.3.0");
    }
#else
    (void)state;
#endif
}

template <typename T>
T *aligned_start(std::vector<T> &storage, size_t offset) {
    const auto address = reinterpret_cast<uintptr_t>(storage.data());
    const auto padding = ((64 - address % 64) % 64) / sizeof(T);
    return storage.data() + padding + offset;
}

template <typename T, typename Backend>
void verify_fill(std::vector<T> &storage, T *dst, T value, size_t n) {
    constexpr T sentinel = static_cast<T>(0xa5a5a5a5a5a5a5a5ULL);
    std::fill(storage.begin(), storage.end(), sentinel);
    Backend::apply(dst, value, n);
    const auto start = static_cast<size_t>(dst - storage.data());
    for (size_t i = 0; i < storage.size(); ++i) {
        const bool inside = i >= start && i - start < n;
        if (storage[i] != (inside ? value : sentinel)) {
            std::fprintf(stderr, "fill validation failed: n=%zu, start=%zu, element_bytes=%zu\n", n,
                         start, sizeof(T));
            std::exit(EXIT_FAILURE);
        }
    }
}

template <typename T, typename Backend, bool Zero>
void fill(benchmark::State &state) {
    const auto n = static_cast<size_t>(state.range(0));
    const auto offset = static_cast<size_t>(state.range(1));
    std::vector<T> storage(n + 64 / sizeof(T) + offset + 1);
    T *dst = aligned_start(storage, offset);
    T value = Zero ? T(0) : static_cast<T>(0x0123456789abcdefULL);
    if constexpr (!Zero) {
        benchmark::DoNotOptimize(value);
    }

    // Verify each implementation and its guards before entering the timed loop.
    verify_fill<T, Backend>(storage, dst, value, n);

    benchmark::DoNotOptimize(dst);
    for (auto _ : state) {
        Backend::apply(dst, value, n);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n));
    state.SetBytesProcessed(state.iterations() * static_cast<int64_t>(n * sizeof(T)));
    label_backend<T, Backend>(state);
}

template <typename T, typename Backend, bool Zero>
void mixed_fill(benchmark::State &state) {
    constexpr std::array<size_t, 16> lengths = {0,  1,  3,  4,  5,  7,  8,  9,
                                                15, 16, 17, 31, 32, 33, 64, 300};
    constexpr size_t lanes = (WJR_HAS_SIMD(AVX) ? 32 : 16) / sizeof(T);
    std::array<size_t, 4096> counts{};
    for (size_t i = 0; i < counts.size(); ++i) {
        counts[i] =
            state.range(0) == 0 ? (i % 2 == 0 ? lanes : lanes * 3) : lengths[i % lengths.size()];
    }
    if (state.range(0) == 1) {
        std::mt19937 random(42);
        std::shuffle(counts.begin(), counts.end(), random);
    }
    const auto offset = static_cast<size_t>(state.range(1));
    std::vector<T> storage(300 + 64 / sizeof(T) + offset + 1);
    T *dst = aligned_start(storage, offset);
    T value = Zero ? T(0) : static_cast<T>(0x0123456789abcdefULL);
    if constexpr (!Zero) {
        benchmark::DoNotOptimize(value);
    }
    for (size_t n : lengths) {
        verify_fill<T, Backend>(storage, dst, value, n);
    }
    verify_fill<T, Backend>(storage, dst, value, lanes * 3);

    benchmark::DoNotOptimize(dst);
    auto *sequence = counts.data();
    benchmark::DoNotOptimize(sequence);
    size_t position = 0;
    for (auto _ : state) {
        Backend::apply(dst, value, sequence[position++ % counts.size()]);
        benchmark::ClobberMemory();
    }
    int64_t total = 0;
    for (size_t n : counts) {
        total += static_cast<int64_t>(n);
    }
    total *= state.iterations() / static_cast<int64_t>(counts.size());
    for (size_t i = 0; i < static_cast<size_t>(state.iterations()) % counts.size(); ++i) {
        total += static_cast<int64_t>(counts[i]);
    }
    state.SetItemsProcessed(total);
    state.SetBytesProcessed(total * static_cast<int64_t>(sizeof(T)));
    label_backend<T, Backend>(state);
}

void fill_sizes(benchmark::internal::Benchmark *bench) {
    for (int64_t n :
         {0, 1, 3, 4, 5, 7, 8, 9, 15, 16, 17, 31, 32, 33, 64, 300, 1024, 4096, 65536, 1048576}) {
        for (int64_t offset : {0, 1}) {
            bench->Args({n, offset});
        }
    }
    bench->ArgNames({"n", "offset"});
    bench->UseRealTime();
}

void mixed_sizes(benchmark::internal::Benchmark *bench) {
    bench->Args({0, 0})->Args({0, 1})->Args({1, 0})->Args({1, 1});
    bench->ArgNames({"workload", "offset"}); // 0: alternating, 1: shuffled.
    bench->UseRealTime();
}

#define WJR_REGISTER_FILL(BACKEND, TYPE, NAME)                                                     \
    BENCHMARK_TEMPLATE(fill, TYPE, BACKEND, false)                                                 \
        ->Name("fill/" NAME "/value")                                                              \
        ->Apply(fill_sizes);                                                                       \
    BENCHMARK_TEMPLATE(fill, TYPE, BACKEND, true)->Name("fill/" NAME "/zero")->Apply(fill_sizes);  \
    BENCHMARK_TEMPLATE(mixed_fill, TYPE, BACKEND, false)                                           \
        ->Name("mixed_fill/" NAME "/value")                                                        \
        ->Apply(mixed_sizes);                                                                      \
    BENCHMARK_TEMPLATE(mixed_fill, TYPE, BACKEND, true)                                            \
        ->Name("mixed_fill/" NAME "/zero")                                                         \
        ->Apply(mixed_sizes)

WJR_REGISTER_FILL(wjr_fill, uint16_t, "wjr/u16");
WJR_REGISTER_FILL(wjr_fill, uint32_t, "wjr/u32");
WJR_REGISTER_FILL(wjr_fill, uint64_t, "wjr/u64");
WJR_REGISTER_FILL(loop_fill, uint16_t, "loop/u16");
WJR_REGISTER_FILL(loop_fill, uint32_t, "loop/u32");
WJR_REGISTER_FILL(loop_fill, uint64_t, "loop/u64");

#if defined(WJR_BENCHMARK_XSIMD)
WJR_REGISTER_FILL(xsimd_fill, uint16_t, "xsimd/u16");
WJR_REGISTER_FILL(xsimd_fill, uint32_t, "xsimd/u32");
WJR_REGISTER_FILL(xsimd_fill, uint64_t, "xsimd/u64");
#endif

#undef WJR_REGISTER_FILL

} // namespace
