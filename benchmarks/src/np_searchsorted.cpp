#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/searchsorted.hpp>

namespace {
namespace np = wjr::np;

template <bool Library, np::search_side Side>
WJR_NOINLINE void searchsorted_call(size_t *output, const int64_t *sorted, size_t count,
                                    const int64_t *queries, size_t query_count) noexcept {
    if constexpr (Library) {
        using shape = np::dextents<size_t, 1>;
        np::searchsorted_to<Side>(np::tensor_view<size_t, shape>(output, query_count),
                                  np::tensor_view<const int64_t, shape>(sorted, count),
                                  np::tensor_view<const int64_t, shape>(queries, query_count));
    } else {
        if (count == 0) {
            std::fill_n(output, query_count, size_t{0});
            return;
        }
        for (size_t i = 0; i != query_count; ++i) {
            if constexpr (Side == np::search_side::left)
                output[i] = std::lower_bound(sorted, sorted + count, queries[i]) - sorted;
            else
                output[i] = std::upper_bound(sorted, sorted + count, queries[i]) - sorted;
        }
    }
}

template <bool Library, np::search_side Side>
void searchsorted_kernel(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto query_count = static_cast<size_t>(state.range(1));
    const bool repeated = state.range(2) != 0;
    std::vector<int64_t> sorted(count), queries(query_count);
    std::vector<size_t> output(query_count), expected(query_count);
    for (size_t i = 0; i != count; ++i)
        sorted[i] = static_cast<int64_t>(i / 2);
    std::mt19937 engine(842);
    for (auto &value : queries)
        value = repeated ? static_cast<int64_t>(count / 4)
                         : static_cast<int64_t>(engine() % (count / 2 + 3)) - 1;
    searchsorted_call<false, Side>(expected.data(), sorted.data(), count, queries.data(),
                                   query_count);
    searchsorted_call<Library, Side>(output.data(), sorted.data(), count, queries.data(),
                                     query_count);
    if (output != expected) {
        state.SkipWithError("searchsorted differs from standard binary search");
        return;
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(sorted.data());
        benchmark::DoNotOptimize(queries.data());
        searchsorted_call<Library, Side>(output.data(), sorted.data(), count, queries.data(),
                                         query_count);
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * query_count);
}

void searchsorted_arguments(benchmark::internal::Benchmark *benchmark) {
    for (int64_t count : {0, 16, 1024, 65536})
        for (int64_t repeated : {0, 1})
            benchmark->Args({count, 256, repeated});
    benchmark->UseRealTime();
}
BENCHMARK_TEMPLATE(searchsorted_kernel, false, np::search_side::left)
    ->Apply(searchsorted_arguments);
BENCHMARK_TEMPLATE(searchsorted_kernel, true, np::search_side::left)->Apply(searchsorted_arguments);
BENCHMARK_TEMPLATE(searchsorted_kernel, false, np::search_side::right)
    ->Apply(searchsorted_arguments);
BENCHMARK_TEMPLATE(searchsorted_kernel, true, np::search_side::right)
    ->Apply(searchsorted_arguments);
} // namespace
