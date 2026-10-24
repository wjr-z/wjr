#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/argsort.hpp>

namespace {
namespace np = wjr::np;

template <bool Library, bool Stable, bool Strided>
WJR_NOINLINE void argsort_call(size_t *output, const double *input, size_t count) noexcept {
    constexpr size_t step = Strided ? 2 : 1;
    if constexpr (Library) {
        using shape = np::dextents<size_t, 1>;
        const auto source = [input, count] {
            if constexpr (Strided)
                return np::tensor_view<const double, shape, np::layout_stride>(
                    input, np::layout_stride::mapping(shape(count), std::array<size_t, 1>{step}));
            else
                return np::tensor_view<const double, shape>(input, count);
        }();
        np::argsort_to<np::sort_order::ascending,
                       Stable ? np::sort_stability::stable : np::sort_stability::unstable>(
            np::tensor_view<size_t, shape>(output, count), source);
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
        std::sort(output, output + count, [read](size_t a, size_t b) noexcept {
            const double left = read(a), right = read(b);
            if (left != left)
                return Stable && right != right && a < b;
            if (right != right)
                return true;
            if constexpr (Stable)
                if (left == right)
                    return a < b;
            return left < right;
        });
    }
}

template <bool Library, bool Stable, bool Strided>
void argsort_kernel(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    constexpr size_t step = Strided ? 2 : 1;
    std::vector<double> input(count * step);
    std::vector<size_t> output(count), reference(count);
    std::mt19937 engine(932);
    for (size_t i = 0; i != count; ++i) {
        input[i * step] = static_cast<double>(engine() % (state.range(1) ? 7 : 65536));
        if (state.range(1) == 2 && i % 7 == 0)
            input[i * step] = std::numeric_limits<double>::quiet_NaN();
    }
    argsort_call<Library, Stable, Strided>(output.data(), input.data(), count);
    argsort_call<false, Stable, Strided>(reference.data(), input.data(), count);
    if (output != reference) {
        state.SkipWithError("permutation differs from equivalent manual kernel");
        return;
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(output.data());
        argsort_call<Library, Stable, Strided>(output.data(), input.data(), count);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}

void sizes(benchmark::internal::Benchmark *bench) {
    for (int64_t count : {0, 1, 32, 1024, 65536})
        for (int64_t distribution : {0, 1, 2})
            bench->Args({count, distribution});
}

BENCHMARK(argsort_kernel<false, false, false>)->Apply(sizes)->UseRealTime();
BENCHMARK(argsort_kernel<true, false, false>)->Apply(sizes)->UseRealTime();
BENCHMARK(argsort_kernel<false, true, false>)->Apply(sizes)->UseRealTime();
BENCHMARK(argsort_kernel<true, true, false>)->Apply(sizes)->UseRealTime();
BENCHMARK(argsort_kernel<false, false, true>)->Apply(sizes)->UseRealTime();
BENCHMARK(argsort_kernel<true, false, true>)->Apply(sizes)->UseRealTime();
BENCHMARK(argsort_kernel<false, true, true>)->Apply(sizes)->UseRealTime();
BENCHMARK(argsort_kernel<true, true, true>)->Apply(sizes)->UseRealTime();
} // namespace
