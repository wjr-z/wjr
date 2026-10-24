#include <algorithm>
#include <array>
#include <limits>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/sort.hpp>

namespace {
namespace np = wjr::np;

template <bool Library, bool Strided>
WJR_NOINLINE void sort_copy_call(double *output, const double *input, size_t count) noexcept {
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
        np::sort_to(np::tensor_view<double, shape>(output, count), source);
    } else {
        if (count == 0)
            return;
        if constexpr (Strided)
            for (size_t i = 0; i != count; ++i)
                output[i] = input[i * step];
        else
            std::copy_n(input, count, output);
        if (count > 1)
            std::sort(output, output + count, [](double left, double right) noexcept {
                return left < right || (left == left && right != right);
            });
    }
}

template <bool Library, bool Strided>
void sort_copy_kernel(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto distribution = state.range(1);
    constexpr size_t step = Strided ? 2 : 1;
    std::vector<double> values(count), input(count * step), output(count), expected(count);
    std::mt19937 engine(812);
    for (size_t i = 0; i != count; ++i)
        values[i] = static_cast<double>(engine() % (distribution == 3 ? 7 : 65536));
    if (distribution == 1)
        std::sort(values.begin(), values.end());
    else if (distribution == 2)
        std::sort(values.begin(), values.end(), std::greater<>{});
    if (distribution == 4)
        for (size_t i = 0; i != count; i += 7)
            values[i] = std::numeric_limits<double>::quiet_NaN();
    for (size_t i = 0; i != count; ++i)
        input[i * step] = values[i];
    sort_copy_call<Library, Strided>(output.data(), input.data(), count);
    sort_copy_call<false, Strided>(expected.data(), input.data(), count);
    for (size_t i = 0; i != count; ++i) {
        if (!(output[i] == expected[i] || (output[i] != output[i] && expected[i] != expected[i]))) {
            state.SkipWithError("sort differs from manual sorted values");
            return;
        }
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        sort_copy_call<Library, Strided>(output.data(), input.data(), count);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}

void sort_arguments(benchmark::internal::Benchmark *bench) {
    for (int64_t count : {0, 1, 32, 1024, 65536})
        for (int64_t distribution : {0, 1, 2, 3, 4})
            bench->Args({count, distribution});
}

BENCHMARK(sort_copy_kernel<false, false>)->Apply(sort_arguments)->UseRealTime();
BENCHMARK(sort_copy_kernel<true, false>)->Apply(sort_arguments)->UseRealTime();
BENCHMARK(sort_copy_kernel<false, true>)->Apply(sort_arguments)->UseRealTime();
BENCHMARK(sort_copy_kernel<true, true>)->Apply(sort_arguments)->UseRealTime();
} // namespace
