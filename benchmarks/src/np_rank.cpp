#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/rank.hpp>

namespace {
namespace np = wjr::np;

// Same-contract handwritten baseline: NaN propagation, one-based average ties,
// indirect comparison without narrowing, and caller-owned reusable indices.
void manual_rank(double *dst, const double *src, size_t count, size_t *order) noexcept {
    for (size_t i = 0; i != count; ++i) {
        if (std::isnan(src[i])) {
            std::fill_n(dst, count, std::numeric_limits<double>::quiet_NaN());
            return;
        }
        order[i] = i;
    }
    if (count == 0)
        return;
    std::sort(order, order + count, [src](size_t a, size_t b) { return src[a] < src[b]; });
    for (size_t first = 0; first != count;) {
        size_t last = first + 1;
        const auto value = src[order[first]];
        while (last != count && value == src[order[last]])
            ++last;
        const double rank = static_cast<double>(first + 1) + (last - first - 1) / 2.0;
        for (size_t i = first; i != last; ++i)
            dst[order[i]] = rank;
        first = last;
    }
}

template <bool Library>
WJR_NOINLINE void rank_call(double *dst, const double *src, size_t count,
                            size_t *workspace) noexcept {
    if constexpr (Library) {
        using shape = np::dextents<size_t, 1>;
        np::rankdata_to(np::tensor_view<double, shape>(dst, count),
                        np::tensor_view<const double, shape>(src, count),
                        std::span<size_t>(workspace, count));
    } else {
        manual_rank(dst, src, count, workspace);
    }
}

template <bool Library>
void rank_average(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto distribution = state.range(1);
    std::vector<double> input(count), output(count), expected(count);
    std::vector<size_t> workspace(count);
    std::mt19937 engine(742);
    for (auto &value : input)
        value =
            distribution == 3 ? 1 : static_cast<double>(engine() % (distribution == 4 ? 8 : 65536));
    if (distribution == 1)
        std::sort(input.begin(), input.end());
    if (distribution == 2)
        std::sort(input.begin(), input.end(), std::greater<>());
    using shape = np::dextents<size_t, 1>;
    const np::tensor_view<const double, shape> src(input.data(), count);
    const np::tensor_view<double, shape> dst(output.data(), count);
    manual_rank(expected.data(), input.data(), count, workspace.data());
    np::rankdata_to(dst, src, workspace);
    if (output != expected) {
        state.SkipWithError("rank result differs from handwritten baseline");
        return;
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        rank_call<Library>(output.data(), input.data(), count, workspace.data());
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * count);
}

void rank_arguments(benchmark::internal::Benchmark *benchmark) {
    // random, ascending, descending, constant, and many ties.
    for (const int count : {0, 1, 16, 300, 4096})
        for (int distribution = 0; distribution != 5; ++distribution)
            benchmark->Args({count, distribution});
}

BENCHMARK_TEMPLATE(rank_average, false)->Apply(rank_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(rank_average, true)->Apply(rank_arguments)->UseRealTime();
} // namespace
