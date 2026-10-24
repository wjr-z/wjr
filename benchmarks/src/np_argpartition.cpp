#include <algorithm>
#include <array>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/argpartition.hpp>

namespace {
namespace np = wjr::np;
enum class selection { handwritten, library, shared, full_sort };

template <class Compare>
void select_indices(size_t *data, size_t first, size_t last, std::span<const size_t> ranks,
                    Compare compare) noexcept {
    while (!ranks.empty()) {
        const auto middle = ranks.size() / 2;
        const auto rank = ranks[middle];
        if (rank == first)
            std::iter_swap(data + rank, std::min_element(data + first, data + last, compare));
        else if (rank + 1 == last)
            std::iter_swap(data + rank, std::max_element(data + first, data + last, compare));
        else
            std::nth_element(data + first, data + rank, data + last, compare);
        if (ranks.front() == ranks.back())
            return;
        const auto lower = std::lower_bound(ranks.begin(), ranks.begin() + middle, rank);
        const auto upper = std::upper_bound(ranks.begin() + middle + 1, ranks.end(), rank);
        select_indices(data, first, rank, std::span<const size_t>(ranks.begin(), lower), compare);
        first = rank + 1;
        ranks = std::span<const size_t>(upper, ranks.end());
    }
}

template <selection Algorithm, bool Strided>
WJR_NOINLINE void argpartition_call(size_t *output, const double *input, size_t count,
                                    const size_t *ranks, size_t requests) noexcept {
    constexpr size_t step = Strided ? 2 : 1;
    if constexpr (Algorithm == selection::library || Algorithm == selection::shared) {
        using shape = np::dextents<size_t, 1>;
        const auto source = [input, count] {
            if constexpr (Strided)
                return np::tensor_view<const double, shape, np::layout_stride>(
                    input, np::layout_stride::mapping(shape(count), std::array<size_t, 1>{step}));
            else
                return np::tensor_view<const double, shape>(input, count);
        }();
        np::argpartition_to<np::sort_order::ascending, Algorithm == selection::shared
                                                           ? np::partition_method::shared_pivot
                                                           : np::partition_method::nth_element>(
            np::tensor_view<size_t, shape>(output, count), source,
            std::span<const size_t>(ranks, requests));
    } else {
        if (count == 0)
            return;
        std::iota(output, output + count, size_t{0});
        const auto read = [input] {
            if constexpr (Strided)
                return [input, stride = step](size_t i) noexcept { return input[i * stride]; };
            else
                return [input](size_t i) noexcept { return input[i]; };
        }();
        const auto compare = [read](size_t a, size_t b) noexcept {
            const auto left = read(a), right = read(b);
            return left < right || (left == left && right != right);
        };
        if constexpr (Algorithm == selection::full_sort)
            std::sort(output, output + count, compare);
        else
            select_indices(output, 0, count, std::span<const size_t>(ranks, requests), compare);
    }
}

template <selection Algorithm, bool Strided>
void argpartition_kernel(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto requests = static_cast<size_t>(state.range(1));
    const auto distribution = state.range(2);
    constexpr size_t step = Strided ? 2 : 1;
    std::vector<double> input(count * step);
    std::vector<size_t> output(count), expected(count), ranks(requests);
    std::mt19937 engine(71);
    for (size_t i = 0; i != count; ++i) {
        input[i * step] = static_cast<double>(engine() % (distribution == 1 ? 7 : 65536));
        if (distribution == 2 && i % 7 == 0)
            input[i * step] = std::numeric_limits<double>::quiet_NaN();
    }
    for (size_t i = 0; i != requests; ++i)
        ranks[i] = (i + 1) * count / (requests + 1);
    argpartition_call<Algorithm, Strided>(output.data(), input.data(), count, ranks.data(),
                                          requests);
    argpartition_call<selection::full_sort, Strided>(expected.data(), input.data(), count,
                                                     ranks.data(), requests);
    for (const auto rank : ranks) {
        const auto actual = input[output[rank] * step], correct = input[expected[rank] * step];
        if (!(actual == correct || (actual != actual && correct != correct))) {
            state.SkipWithError("indirect selection differs from sorted values");
            return;
        }
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        argpartition_call<Algorithm, Strided>(output.data(), input.data(), count, ranks.data(),
                                              requests);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}

void selection_arguments(benchmark::internal::Benchmark *bench) {
    bench->Args({0, 0, 0})->Args({1, 1, 0})->Args({32, 3, 0});
    for (int64_t count : {1024, 65536})
        for (int64_t requests : {1, 3, 31})
            for (int64_t distribution : {0, 1, 2})
                bench->Args({count, requests, distribution});
}

BENCHMARK(argpartition_kernel<selection::handwritten, false>)
    ->Apply(selection_arguments)
    ->UseRealTime();
BENCHMARK(argpartition_kernel<selection::library, false>)
    ->Apply(selection_arguments)
    ->UseRealTime();
BENCHMARK(argpartition_kernel<selection::shared, false>)->Apply(selection_arguments)->UseRealTime();
BENCHMARK(argpartition_kernel<selection::full_sort, false>)
    ->Apply(selection_arguments)
    ->UseRealTime();
BENCHMARK(argpartition_kernel<selection::handwritten, true>)
    ->Apply(selection_arguments)
    ->UseRealTime();
BENCHMARK(argpartition_kernel<selection::library, true>)->Apply(selection_arguments)->UseRealTime();
BENCHMARK(argpartition_kernel<selection::shared, true>)->Apply(selection_arguments)->UseRealTime();
BENCHMARK(argpartition_kernel<selection::full_sort, true>)
    ->Apply(selection_arguments)
    ->UseRealTime();
} // namespace
