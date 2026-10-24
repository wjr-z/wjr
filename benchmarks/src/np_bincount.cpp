#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/bincount.hpp>

namespace {
namespace np = wjr::np;

template <bool Library, bool Weighted, bool Strided>
WJR_NOINLINE void bincount_call(std::conditional_t<Weighted, double, uint64_t> *output, size_t bins,
                                const uint32_t *indices, const double *weights,
                                size_t count) noexcept {
    using D = std::conditional_t<Weighted, double, uint64_t>;
    constexpr size_t step = Strided ? 2 : 1;
    if constexpr (Library) {
        using shape = np::dextents<size_t, 1>;
        using layout = std::conditional_t<Strided, np::layout_stride, np::layout_right>;
        const auto make = [](auto *pointer, size_t size) {
            using T = std::remove_pointer_t<decltype(pointer)>;
            if constexpr (Strided)
                return np::tensor_view<T, shape, layout>(
                    pointer,
                    np::layout_stride::mapping<shape>(shape(size), std::array<size_t, 1>{step}));
            else
                return np::tensor_view<T, shape, layout>(pointer, size);
        };
        if constexpr (Weighted)
            np::bincount_to(make(output, bins), make(indices, count), make(weights, count));
        else
            np::bincount_to(make(output, bins), make(indices, count));
    } else {
        D *WJR_RESTRICT dst = output;
        for (size_t i = 0; i != bins; ++i)
            dst[i * step] = D{};
        for (size_t i = 0; i != count; ++i) {
            if constexpr (Weighted)
                dst[indices[i * step] * step] += static_cast<D>(weights[i * step]);
            else
                ++dst[indices[i * step] * step];
        }
    }
}

template <bool Library, bool Weighted, bool Strided>
void BM_NpBincount(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto bins = static_cast<size_t>(state.range(1));
    const bool repeated = state.range(2) != 0;
    constexpr size_t step = Strided ? 2 : 1;
    using D = std::conditional_t<Weighted, double, uint64_t>;
    std::vector<uint32_t> indices(count * step);
    std::vector<double> weights(count * step);
    std::vector<D> output(bins * step);
    std::mt19937 engine(6109);
    for (size_t i = 0; i != count; ++i) {
        indices[i * step] = repeated ? 0 : engine() % bins;
        weights[i * step] = static_cast<int>(engine() % 101) - 50;
    }
    // Verify the timed path independently before measuring it.
    std::vector<D> expected(bins);
    for (size_t i = 0; i != count; ++i)
        if constexpr (Weighted)
            expected[indices[i * step]] += weights[i * step];
        else
            ++expected[indices[i * step]];
    bincount_call<Library, Weighted, Strided>(output.data(), bins, indices.data(), weights.data(),
                                              count);
    for (size_t i = 0; i != bins; ++i)
        if (output[i * step] != expected[i]) {
            state.SkipWithError("bincount result differs from scalar oracle");
            return;
        }
    for (auto _ : state) {
        bincount_call<Library, Weighted, Strided>(output.data(), bins, indices.data(),
                                                  weights.data(), count);
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}

void bincount_cases(benchmark::internal::Benchmark *benchmark) {
    for (int64_t count : {0, 1, 17, 1024, 65536})
        for (int64_t bins : {16, 4096})
            for (int64_t repeated : {0, 1})
                benchmark->Args({count, bins, repeated});
}

BENCHMARK_TEMPLATE(BM_NpBincount, true, false, false)->Apply(bincount_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpBincount, false, false, false)->Apply(bincount_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpBincount, true, true, false)->Apply(bincount_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpBincount, false, true, false)->Apply(bincount_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpBincount, true, false, true)->Apply(bincount_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpBincount, false, false, true)->Apply(bincount_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpBincount, true, true, true)->Apply(bincount_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpBincount, false, true, true)->Apply(bincount_cases)->UseRealTime();
} // namespace
