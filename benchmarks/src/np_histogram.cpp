#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/histogram.hpp>

namespace {
namespace np = wjr::np;
inline constexpr size_t histogram_nan_period = 17;

template <bool Library, bool Strided, class T>
WJR_NOINLINE void histogram_call(size_t *WJR_RESTRICT output, const T *values, size_t count,
                                 const T *edges, size_t bins) noexcept {
    constexpr size_t output_step = Strided ? 2 : 1;
    constexpr size_t value_step = Strided ? 2 : 1;
    constexpr size_t edge_step = Strided ? 3 : 1;
    if constexpr (Library) {
        using shape = np::dextents<size_t, 1>;
        if constexpr (Strided) {
            const auto view = []<class U>(U *data, size_t size, size_t step) {
                return np::tensor_view<U, shape, np::layout_stride>(
                    data,
                    np::layout_stride::mapping<shape>(shape(size), std::array<size_t, 1>{step}));
            };
            np::histogram_to(view(output, bins, output_step), view(values, count, value_step),
                             view(edges, bins + 1, edge_step));
        } else {
            np::histogram_to(np::tensor_view<size_t, shape>(output, bins),
                             np::tensor_view<const T, shape>(values, count),
                             np::tensor_view<const T, shape>(edges, bins + 1));
        }
    } else {
        if constexpr (Strided)
            for (size_t bin = 0; bin != bins; ++bin)
                output[bin * output_step] = 0;
        else
            std::fill_n(output, bins, size_t{});
        if (count == 0)
            return;
        const T first = edges[0], last = edges[bins * edge_step];
        for (size_t i = 0; i != count; ++i) {
            const T value = values[i * value_step];
            if (!(first <= value && value <= last))
                continue;
            size_t position;
            if constexpr (!Strided) {
                position = static_cast<size_t>(std::upper_bound(edges + 1, edges + bins, value) -
                                               (edges + 1));
            } else {
                size_t begin = 0, length = bins - 1;
                while (length != 0) {
                    const auto half = length / 2;
                    const auto middle = begin + half;
                    if (value < edges[(middle + 1) * edge_step])
                        length = half;
                    else {
                        begin = middle + 1;
                        length -= half + 1;
                    }
                }
                position = begin;
            }
            ++output[position * output_step];
        }
    }
}

template <bool Library, bool Strided, class T>
void BM_NpHistogram(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto bins = static_cast<size_t>(state.range(1));
    const auto distribution = state.range(2);
    constexpr size_t output_step = Strided ? 2 : 1;
    constexpr size_t value_step = Strided ? 2 : 1;
    constexpr size_t edge_step = Strided ? 3 : 1;
    std::vector<T> edges(bins * edge_step + 1),
        values(count == 0 ? 0 : (count - 1) * value_step + 1);
    std::vector<size_t> output(bins * output_step, SIZE_MAX),
        expected(bins * output_step, SIZE_MAX);
    std::mt19937 random(9077);
    const T origin = [](size_t bins) {
        if constexpr (std::is_integral_v<T>)
            return std::numeric_limits<T>::max() - static_cast<T>(bins * 4 + 16);
        else
            return T{};
    }(bins);
    for (size_t i = 0; i <= bins; ++i)
        edges[i * edge_step] = origin + static_cast<T>(distribution == 2 ? i / 2 : i * 2);
    for (size_t i = 0; i != count; ++i) {
        T value = origin + static_cast<T>(distribution == 1 ? bins : random() % (bins * 3 + 8));
        if constexpr (std::is_floating_point_v<T>)
            if (distribution == 3 && i % histogram_nan_period == 0)
                value = std::numeric_limits<T>::quiet_NaN();
        values[i * value_step] = value;
    }
    histogram_call<false, Strided>(expected.data(), values.data(), count, edges.data(), bins);
    histogram_call<Library, Strided>(output.data(), values.data(), count, edges.data(), bins);
    if (output != expected) {
        state.SkipWithError("histogram differs from scalar upper-bound oracle");
        return;
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(values.data());
        benchmark::DoNotOptimize(edges.data());
        histogram_call<Library, Strided>(output.data(), values.data(), count, edges.data(), bins);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}

void histogram_cases(benchmark::internal::Benchmark *benchmark) {
    for (int64_t count : {0, 1, 32, 1024, 65536})
        for (int64_t bins : {1, 16, 257})
            for (int64_t distribution : {0, 1, 2, 3})
                benchmark->Args({count, bins, distribution});
}
BENCHMARK_TEMPLATE(BM_NpHistogram, false, false, double)->Apply(histogram_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpHistogram, true, false, double)->Apply(histogram_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpHistogram, false, true, double)->Apply(histogram_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpHistogram, true, true, double)->Apply(histogram_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpHistogram, false, false, uint64_t)->Apply(histogram_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpHistogram, true, false, uint64_t)->Apply(histogram_cases)->UseRealTime();
} // namespace
