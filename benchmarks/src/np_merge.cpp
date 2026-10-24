#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/merge.hpp>

namespace {
namespace np = wjr::np;

template <bool Library, bool Strided>
WJR_NOINLINE void merge_values(double *output, const double *left, size_t left_count,
                               const double *right, size_t right_count) {
    using shape = np::dextents<size_t, 1>;
    constexpr size_t step = Strided ? 2 : 1;
    if constexpr (Library) {
        if constexpr (Strided) {
            const auto mapping = [](size_t n) {
                return np::layout_stride::mapping<shape>(shape(n), std::array<size_t, 1>{step});
            };
            np::merge_sorted_to(
                np::tensor_view<double, shape, np::layout_stride>(
                    output, mapping(left_count + right_count)),
                np::tensor_view<const double, shape, np::layout_stride>(left, mapping(left_count)),
                np::tensor_view<const double, shape, np::layout_stride>(right,
                                                                        mapping(right_count)));
        } else {
            np::merge_sorted_to(np::tensor_view<double, shape>(output, left_count + right_count),
                                np::tensor_view<const double, shape>(left, left_count),
                                np::tensor_view<const double, shape>(right, right_count));
        }
    } else if constexpr (!Strided) {
        if (left_count == 0) {
            if (right_count != 0)
                std::copy_n(right, right_count, output);
        } else if (right_count == 0) {
            std::copy_n(left, left_count, output);
        } else {
            std::merge(left, left + left_count, right, right + right_count, output,
                       np::detail::partition_less{});
        }
    } else {
        size_t i = 0, j = 0, k = 0;
        const np::detail::partition_less compare;
        while (i != left_count && j != right_count) {
            if (compare(right[j * step], left[i * step]))
                output[k++ * step] = right[j++ * step];
            else
                output[k++ * step] = left[i++ * step];
        }
        while (i != left_count)
            output[k++ * step] = left[i++ * step];
        while (j != right_count)
            output[k++ * step] = right[j++ * step];
    }
}

template <bool Library, bool Strided>
void BM_NpMerge(benchmark::State &state) {
    constexpr size_t step = Strided ? 2 : 1;
    const auto n = static_cast<size_t>(state.range(0));
    const auto m = static_cast<size_t>(state.range(1));
    const auto distribution = state.range(2);
    std::vector<double> left(n * step), right(m * step), output((n + m) * step, -99);
    std::vector<double> a(n), b(m), expected(n + m);
    std::mt19937 engine(71);
    const auto fill = [&](auto &values) {
        for (size_t i = 0; i != values.size(); ++i) {
            values[i] = distribution == 0 ? static_cast<double>(engine())
                                          : static_cast<double>(engine() % 16);
            if (distribution == 2 && i % 13 == 0)
                values[i] = std::numeric_limits<double>::quiet_NaN();
        }
        std::sort(values.begin(), values.end(), np::detail::partition_less{});
    };
    fill(a);
    fill(b);
    for (size_t i = 0; i != n; ++i)
        left[i * step] = a[i];
    for (size_t i = 0; i != m; ++i)
        right[i * step] = b[i];
    std::merge(a.begin(), a.end(), b.begin(), b.end(), expected.begin(),
               np::detail::partition_less{});
    merge_values<Library, Strided>(output.data(), left.data(), n, right.data(), m);
    for (size_t i = 0; i != n + m; ++i) {
        if (!(output[i * step] == expected[i] ||
              (std::isnan(output[i * step]) && std::isnan(expected[i])))) {
            state.SkipWithError("merge mismatch");
            return;
        }
        if constexpr (Strided) {
            if (output[i * step + 1] != -99) {
                state.SkipWithError("padding overwritten");
                return;
            }
        }
    }
    for (auto _ : state) {
        merge_values<Library, Strided>(output.data(), left.data(), n, right.data(), m);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(n + m));
}

void merge_arguments(benchmark::internal::Benchmark *bench) {
    for (const auto sizes : {std::array<int64_t, 2>{0, 0},
                             {0, 1024},
                             {1024, 0},
                             {1, 1},
                             {16, 1024},
                             {1024, 16},
                             {1024, 1024},
                             {65536, 65536}})
        for (int64_t distribution : {0, 1, 2})
            bench->Args({sizes[0], sizes[1], distribution});
}
BENCHMARK_TEMPLATE(BM_NpMerge, false, false)->Apply(merge_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpMerge, true, false)->Apply(merge_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpMerge, false, true)->Apply(merge_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpMerge, true, true)->Apply(merge_arguments)->UseRealTime();
} // namespace
