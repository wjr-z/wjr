#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/quantile.hpp>

namespace {
namespace np = wjr::np;
enum class selection { balanced, ascending, shared, library, full_sort };

// A request-balanced baseline reuses nth_element on disjoint subranges.
void balanced_select(int64_t *values, size_t first, size_t last,
                     std::span<const size_t> ranks) noexcept {
    while (!ranks.empty()) {
        const auto middle = ranks.size() / 2;
        const auto rank = ranks[middle];
        if (rank == first)
            std::iter_swap(values + rank, std::min_element(values + first, values + last));
        else if (rank + 1 == last)
            std::iter_swap(values + rank, std::max_element(values + first, values + last));
        else
            std::nth_element(values + first, values + rank, values + last);
        if (ranks.front() == ranks.back())
            return;
        const auto lower = std::lower_bound(ranks.begin(), ranks.begin() + middle, rank);
        const auto upper = std::upper_bound(ranks.begin() + middle + 1, ranks.end(), rank);
        balanced_select(values, first, rank, std::span<const size_t>(ranks.begin(), lower));
        first = rank + 1;
        ranks = std::span<const size_t>(upper, ranks.end());
    }
}

template <selection Algorithm>
WJR_NOINLINE void partition_copy_call(int64_t *values, const int64_t *input, size_t count,
                                      const size_t *ranks, size_t requests) noexcept {
    std::copy_n(input, count, values);
    if constexpr (Algorithm == selection::full_sort) {
        std::sort(values, values + count);
    } else if constexpr (Algorithm == selection::shared) {
        using shape = np::dextents<size_t, 1>;
        np::partition_at<np::partition_method::shared_pivot>(
            np::tensor_view<int64_t, shape>(values, count),
            std::span<const size_t>(ranks, requests));
    } else if constexpr (Algorithm == selection::library) {
        using shape = np::dextents<size_t, 1>;
        np::partition_at(np::tensor_view<int64_t, shape>(values, count),
                         std::span<const size_t>(ranks, requests));
    } else if constexpr (Algorithm == selection::ascending) {
        size_t first = 0;
        for (size_t i = 0; i != requests; ++i) {
            if (i != 0 && ranks[i] == ranks[i - 1])
                continue;
            std::nth_element(values + first, values + ranks[i], values + count);
            first = ranks[i] + 1;
        }
    } else {
        balanced_select(values, 0, count, std::span<const size_t>(ranks, requests));
    }
}

template <selection Algorithm>
void partition_copy_kernel(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto requests = static_cast<size_t>(state.range(1));
    const auto distribution = state.range(2);
    std::vector<int64_t> input(count), values(count);
    std::vector<size_t> ranks(requests);
    std::mt19937 engine(1451);
    for (auto &value : input)
        value = static_cast<int64_t>(engine());
    for (size_t i = 0; i != requests; ++i)
        ranks[i] = distribution == 1   ? count / 2 + i % 3
                   : distribution == 2 ? i % 3
                   : distribution == 3 ? count - 3 + i % 3
                                       : (i + 1) * count / (requests + 1);
    std::sort(ranks.begin(), ranks.end());
    auto expected = input;
    std::sort(expected.begin(), expected.end());
    partition_copy_call<Algorithm>(values.data(), input.data(), count, ranks.data(), requests);
    for (size_t rank : ranks)
        if (values[rank] != expected[rank]) {
            state.SkipWithError("multi-selection differs from full sort");
            return;
        }
    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(ranks.data());
        partition_copy_call<Algorithm>(values.data(), input.data(), count, ranks.data(), requests);
        benchmark::DoNotOptimize(values.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * count);
}

void partition_arguments(benchmark::internal::Benchmark *benchmark) {
    for (int64_t distribution : {0, 1, 2, 3})
        benchmark->Args({16, 1, distribution})
            ->Args({16, 8, distribution})
            ->Args({1024, 3, distribution})
            ->Args({65536, 1, distribution})
            ->Args({65536, 3, distribution})
            ->Args({65536, 31, distribution})
            ->Args({65536, 257, distribution});
    benchmark->UseRealTime();
}

template <bool Library, class T>
WJR_NOINLINE void quantile_copy_call(double *output, const T *input, size_t count,
                                     const double *probabilities, size_t queries, T *workspace,
                                     size_t *ranks) noexcept {
    if constexpr (Library) {
        using shape = np::dextents<size_t, 1>;
        np::quantile_to(np::tensor_view<double, shape>(output, queries),
                        np::tensor_view<const T, shape>(input, count),
                        np::tensor_view<const double, shape>(probabilities, queries),
                        std::span<T>(workspace, count), std::span<size_t>(ranks, queries * 2));
    } else {
        std::copy_n(input, count, workspace);
        std::sort(workspace, workspace + count);
        for (size_t i = 0; i != queries; ++i) {
            const double position = probabilities[i] * static_cast<double>(count - 1);
            const auto low = static_cast<size_t>(position);
            const auto high = std::min(low + 1, count - 1);
            output[i] = std::lerp(static_cast<double>(workspace[low]),
                                  static_cast<double>(workspace[high]), position - low);
        }
    }
}

template <bool Library, class T>
void quantile_copy_kernel(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto queries = static_cast<size_t>(state.range(1));
    std::vector<T> input(count), workspace(count);
    std::vector<double> probabilities(queries), output(queries), expected(queries);
    std::vector<size_t> ranks(queries * 2);
    std::mt19937 engine(1453);
    for (auto &value : input)
        value = static_cast<T>(engine());
    for (auto &q : probabilities)
        q = static_cast<double>(engine() % 1025) / 1024;
    quantile_copy_call<false>(expected.data(), input.data(), count, probabilities.data(), queries,
                              workspace.data(), ranks.data());
    quantile_copy_call<Library>(output.data(), input.data(), count, probabilities.data(), queries,
                                workspace.data(), ranks.data());
    if (output != expected) {
        state.SkipWithError("quantile differs from sorted interpolation reference");
        return;
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(probabilities.data());
        quantile_copy_call<Library>(output.data(), input.data(), count, probabilities.data(),
                                    queries, workspace.data(), ranks.data());
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * count);
}

BENCHMARK_TEMPLATE(partition_copy_kernel, selection::balanced)->Apply(partition_arguments);
BENCHMARK_TEMPLATE(partition_copy_kernel, selection::ascending)->Apply(partition_arguments);
BENCHMARK_TEMPLATE(partition_copy_kernel, selection::library)->Apply(partition_arguments);
BENCHMARK_TEMPLATE(partition_copy_kernel, selection::shared)->Apply(partition_arguments);
BENCHMARK_TEMPLATE(partition_copy_kernel, selection::full_sort)->Apply(partition_arguments);
BENCHMARK_TEMPLATE(quantile_copy_kernel, false, int64_t)
    ->Args({65536, 3})
    ->Args({65536, 31})
    ->UseRealTime();
BENCHMARK_TEMPLATE(quantile_copy_kernel, true, int64_t)
    ->Args({65536, 3})
    ->Args({65536, 31})
    ->UseRealTime();
BENCHMARK_TEMPLATE(quantile_copy_kernel, false, double)
    ->Args({65536, 3})
    ->Args({65536, 31})
    ->UseRealTime();
BENCHMARK_TEMPLATE(quantile_copy_kernel, true, double)
    ->Args({65536, 3})
    ->Args({65536, 31})
    ->UseRealTime();
} // namespace
