#include <array>
#include <cstdint>
#include <functional>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/segmented_reduce.hpp>

namespace {
namespace np = wjr::np;
struct manual {};
struct convenience {};
struct generic {};

template <class Implementation, bool Strided>
WJR_NOINLINE void segmented_sum_call(double *output, const double *input, const uint32_t *offsets,
                                     size_t groups, size_t count) noexcept {
    constexpr size_t step = Strided ? 2 : 1;
    if constexpr (std::same_as<Implementation, manual>) {
        if (groups == 0)
            return;
        size_t begin = offsets[0];
        for (size_t group = 0; group != groups; ++group) {
            const size_t end = offsets[group + 1];
            double value = 0;
            for (size_t i = begin; i != end; ++i)
                value += input[i * step];
            output[group * step] = value;
            begin = end;
        }
    } else {
        using shape = np::dextents<size_t, 1>;
        using layout = std::conditional_t<Strided, np::layout_stride, np::layout_right>;
        const auto mapping = [](size_t length) {
            if constexpr (Strided)
                return typename layout::template mapping<shape>(shape(length),
                                                                std::array<size_t, 1>{step});
            else
                return typename layout::template mapping<shape>(shape(length));
        };
        const np::tensor_view<double, shape, layout> dst(output, mapping(groups));
        const np::tensor_view<const double, shape, layout> src(input, mapping(count));
        const std::span<const uint32_t> boundaries(offsets, groups + 1);
        if constexpr (std::same_as<Implementation, convenience>)
            np::segmented_sum_to(dst, src, boundaries);
        else
            np::segmented_reduce_to(dst, src, boundaries, 0., std::plus<>{});
    }
}

template <class Implementation, bool Strided>
void segmented_sum_kernel(benchmark::State &state) {
    const auto groups = static_cast<size_t>(state.range(0));
    const auto length = static_cast<size_t>(state.range(1));
    const auto count = groups * length;
    constexpr size_t step = Strided ? 2 : 1;
    std::vector<double> input(count * step), output(groups * step, -999), expected = output;
    std::vector<uint32_t> offsets(groups + 1);
    std::mt19937 engine(730);
    for (auto &value : input)
        value = static_cast<double>(engine() % 100);
    for (size_t group = 0; group != offsets.size(); ++group)
        offsets[group] = static_cast<uint32_t>(group * length);
    segmented_sum_call<manual, Strided>(expected.data(), input.data(), offsets.data(), groups,
                                        count);
    segmented_sum_call<Implementation, Strided>(output.data(), input.data(), offsets.data(), groups,
                                                count);
    if (output != expected) {
        state.SkipWithError("segmented sum differs from same-order handwritten baseline");
        return;
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(offsets.data());
        segmented_sum_call<Implementation, Strided>(output.data(), input.data(), offsets.data(),
                                                    groups, count);
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * count);
}

void segmented_arguments(benchmark::internal::Benchmark *benchmark) {
    benchmark->Args({0, 0})
        ->Args({64, 0})
        ->Args({64, 1})
        ->Args({64, 4})
        ->Args({64, 300})
        ->Args({64, 4096})
        ->UseRealTime();
}
BENCHMARK_TEMPLATE(segmented_sum_kernel, manual, false)->Apply(segmented_arguments);
BENCHMARK_TEMPLATE(segmented_sum_kernel, convenience, false)->Apply(segmented_arguments);
BENCHMARK_TEMPLATE(segmented_sum_kernel, generic, false)->Apply(segmented_arguments);
BENCHMARK_TEMPLATE(segmented_sum_kernel, manual, true)->Apply(segmented_arguments);
BENCHMARK_TEMPLATE(segmented_sum_kernel, convenience, true)->Apply(segmented_arguments);
BENCHMARK_TEMPLATE(segmented_sum_kernel, generic, true)->Apply(segmented_arguments);
} // namespace
