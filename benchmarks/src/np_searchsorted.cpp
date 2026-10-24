#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/searchsorted.hpp>

namespace {
namespace np = wjr::np;

enum class implementation { standard, independent, batch, level, block4, block8, block32, shared8 };

// NumPy-style whole-batch alternative: output holds intermediate bases.
template <np::search_side Side, size_t Step>
void level_search(size_t *output, const int64_t *sorted, size_t count, const int64_t *queries,
                  size_t query_count) noexcept {
    const auto before = [](int64_t element, int64_t value) {
        if constexpr (Side == np::search_side::left)
            return element < value;
        else
            return element <= value;
    };
    if (count == 0) {
        for (size_t i = 0; i != query_count; ++i)
            output[i * Step] = 0;
        return;
    }
    auto length = count;
    auto half = length / 2;
    length -= half;
    const auto pivot = sorted[half * Step];
    for (size_t i = 0; i != query_count; ++i)
        output[i * Step] = before(pivot, queries[i * Step]) * half;
    while (length > 1) {
        half = length / 2;
        length -= half;
        for (size_t i = 0; i != query_count; ++i)
            output[i * Step] +=
                before(sorted[(output[i * Step] + half) * Step], queries[i * Step]) * half;
    }
    for (size_t i = 0; i != query_count; ++i)
        output[i * Step] += before(sorted[output[i * Step] * Step], queries[i * Step]);
}

// Competent bounded-workspace alternative: retain queries and bases in a small
// block, write only the final result. The library is compared with the same
// mechanism and alternative widths rather than only independent binary search.
template <size_t Block, np::search_side Side, size_t Step, bool SharedPrefix = false>
void blocked_search(size_t *output, const int64_t *sorted, size_t count, const int64_t *queries,
                    size_t query_count) noexcept {
    const auto before = [](int64_t element, int64_t value) {
        if constexpr (Side == np::search_side::left)
            return element < value;
        else
            return element <= value;
    };
    if (count == 0) {
        for (size_t i = 0; i != query_count; ++i)
            output[i * Step] = 0;
        return;
    }
    size_t start = 0;
    const auto run = [&]<size_t Active>(std::integral_constant<size_t, Active>) {
        constexpr auto active = Active;
        std::array<size_t, Block> bases;
        std::array<int64_t, Block> keys;
        for (size_t i = 0; i != active; ++i)
            keys[i] = queries[(start + i) * Step];
        auto length = count;
        size_t common_base = 0;
        if constexpr (SharedPrefix && Active > 1) {
            auto minimum = keys[0], maximum = keys[0];
            for (size_t i = 1; i != active; ++i) {
                minimum = std::min(minimum, keys[i]);
                maximum = std::max(maximum, keys[i]);
            }
            while (length > 1) {
                const auto half = length / 2;
                const auto pivot = sorted[(common_base + half) * Step];
                if (before(pivot, minimum))
                    common_base += half;
                else if (before(pivot, maximum))
                    break;
                length -= half;
            }
        }
        auto half = length / 2;
        length -= half;
        const auto pivot = sorted[(common_base + half) * Step];
        for (size_t i = 0; i != active; ++i) {
            bases[i] = common_base + before(pivot, keys[i]) * half;
        }
        while (length > 1) {
            half = length / 2;
            length -= half;
            for (size_t i = 0; i != active; ++i)
                bases[i] += before(sorted[(bases[i] + half) * Step], keys[i]) * half;
        }
        for (size_t i = 0; i != active; ++i)
            output[(start + i) * Step] = bases[i] + before(sorted[bases[i] * Step], keys[i]);
        start += active;
    };
    while (query_count - start >= Block)
        run(std::integral_constant<size_t, Block>{});
    while (start != query_count)
        run(std::integral_constant<size_t, 1>{});
}

template <implementation Impl, np::search_side Side, bool Strided>
WJR_NOINLINE void searchsorted_call(size_t *output, const int64_t *sorted, size_t count,
                                    const int64_t *queries, size_t query_count) noexcept {
    constexpr size_t step = Strided ? 2 : 1;
    if constexpr (Impl == implementation::level) {
        level_search<Side, step>(output, sorted, count, queries, query_count);
    } else if constexpr (Impl == implementation::shared8) {
        blocked_search<8, Side, step, true>(output, sorted, count, queries, query_count);
    } else if constexpr (Impl == implementation::block4 || Impl == implementation::block8 ||
                         Impl == implementation::block32) {
        blocked_search<Impl == implementation::block4   ? 4
                       : Impl == implementation::block8 ? 8
                                                        : 32,
                       Side, step>(output, sorted, count, queries, query_count);
    } else if constexpr (Impl != implementation::standard) {
        using shape = np::dextents<size_t, 1>;
        using layout = std::conditional_t<Strided, np::layout_stride, np::layout_right>;
        const auto mapping = [](size_t size) {
            if constexpr (Strided)
                return typename layout::template mapping<shape>(shape(size),
                                                                std::array<size_t, 1>{step});
            else
                return typename layout::template mapping<shape>(shape(size));
        };
        const np::tensor_view<size_t, shape, layout> dst(output, mapping(query_count));
        const np::tensor_view<const int64_t, shape, layout> source(sorted, mapping(count)),
            values(queries, mapping(query_count));
        if constexpr (Impl == implementation::batch)
            np::searchsorted_batch_to<Side>(dst, source, values);
        else
            np::searchsorted_to<Side>(dst, source, values);
    } else {
        if (count == 0) {
            for (size_t i = 0; i != query_count; ++i)
                output[i * step] = 0;
            return;
        }
        for (size_t i = 0; i != query_count; ++i) {
            if constexpr (Strided) {
                size_t base = 0, length = count;
                while (length) {
                    const auto half = length / 2;
                    const auto middle = base + half;
                    const bool before = Side == np::search_side::left
                                            ? sorted[middle * step] < queries[i * step]
                                            : sorted[middle * step] <= queries[i * step];
                    if (before) {
                        base = middle + 1;
                        length -= half + 1;
                    } else {
                        length = half;
                    }
                }
                output[i * step] = base;
            } else if constexpr (Side == np::search_side::left)
                output[i] = std::lower_bound(sorted, sorted + count, queries[i]) - sorted;
            else
                output[i] = std::upper_bound(sorted, sorted + count, queries[i]) - sorted;
        }
    }
}

template <implementation Impl, np::search_side Side, bool Strided>
void searchsorted_kernel(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto query_count = static_cast<size_t>(state.range(1));
    const auto order = state.range(2);
    constexpr size_t step = Strided ? 2 : 1;
    std::vector<int64_t> sorted(count * step, -999), queries(query_count * step, -999);
    std::vector<size_t> output(query_count * step, SIZE_MAX),
        expected(query_count * step, SIZE_MAX);
    for (size_t i = 0; i != count; ++i)
        sorted[i * step] = static_cast<int64_t>(i / 2);
    std::mt19937 engine(842);
    for (size_t i = 0; i != query_count; ++i)
        queries[i * step] = order == 1   ? static_cast<int64_t>(count / 4)
                            : order == 2 ? static_cast<int64_t>(i * count / (2 * query_count))
                                         : static_cast<int64_t>(engine() % (count / 2 + 3)) - 1;
    searchsorted_call<implementation::standard, Side, Strided>(expected.data(), sorted.data(),
                                                               count, queries.data(), query_count);
    searchsorted_call<Impl, Side, Strided>(output.data(), sorted.data(), count, queries.data(),
                                           query_count);
    if (output != expected) {
        state.SkipWithError("searchsorted differs from standard binary search");
        return;
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(sorted.data());
        benchmark::DoNotOptimize(queries.data());
        searchsorted_call<Impl, Side, Strided>(output.data(), sorted.data(), count, queries.data(),
                                               query_count);
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * query_count);
}

void searchsorted_arguments(benchmark::internal::Benchmark *benchmark) {
    for (int64_t count : {0, 16, 1024, 65536})
        for (int64_t queries : {1, 32, 1024})
            for (int64_t order : {0, 1, 2})
                benchmark->Args({count, queries, order});
    benchmark->UseRealTime();
}
#define WJR_BENCH_SEARCH(Impl, Side, Strided)                                                      \
    BENCHMARK_TEMPLATE(searchsorted_kernel, implementation::Impl, np::search_side::Side, Strided)  \
        ->Apply(searchsorted_arguments)
#define WJR_BENCH_SEARCH_IMPL(Impl)                                                                \
    WJR_BENCH_SEARCH(Impl, left, false);                                                           \
    WJR_BENCH_SEARCH(Impl, right, false);                                                          \
    WJR_BENCH_SEARCH(Impl, left, true);                                                            \
    WJR_BENCH_SEARCH(Impl, right, true)
WJR_BENCH_SEARCH_IMPL(standard);
WJR_BENCH_SEARCH_IMPL(independent);
WJR_BENCH_SEARCH_IMPL(batch);
WJR_BENCH_SEARCH_IMPL(level);
WJR_BENCH_SEARCH_IMPL(block4);
WJR_BENCH_SEARCH_IMPL(block8);
WJR_BENCH_SEARCH_IMPL(block32);
WJR_BENCH_SEARCH_IMPL(shared8);
#undef WJR_BENCH_SEARCH_IMPL
#undef WJR_BENCH_SEARCH
} // namespace
