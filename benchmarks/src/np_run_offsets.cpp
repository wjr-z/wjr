#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/research_old_run_offsets.hpp>
#include <wjr/np/run_offsets.hpp>

namespace {
namespace np = wjr::np;
inline constexpr size_t run_fixture_block_size = 16;

template <int Method, bool Strided>
WJR_NOINLINE size_t run_offsets_call(size_t *output, const uint64_t *keys, size_t count) noexcept {
    constexpr size_t step = Strided ? 2 : 1;
    if constexpr (Method != 0) {
        using shape = np::dextents<size_t, 1>;
        const np::tensor_view<size_t, shape> dst(output, count + 1);
        if constexpr (Strided) {
            const np::tensor_view<const uint64_t, shape, np::layout_stride> src(
                keys, np::layout_stride::mapping<shape>(shape(count), std::array<size_t, 1>{step}));
            if constexpr (Method == 1)
                return np::run_offsets_to(dst, src);
            else
                return np::old_run_offsets_to(dst, src);
        } else {
            if constexpr (Method == 1)
                return np::run_offsets_to(dst, np::tensor_view<const uint64_t, shape>(keys, count));
            else
                return np::old_run_offsets_to(dst,
                                              np::tensor_view<const uint64_t, shape>(keys, count));
        }
    } else {
        size_t *WJR_RESTRICT dst = output;
        dst[0] = 0;
        if (count == 0)
            return 0;
        size_t runs = 1;
        for (size_t i = 1; i != count; ++i)
            if (keys[(i - 1) * step] != keys[i * step])
                dst[runs++] = i;
        dst[runs] = count;
        return runs;
    }
}

template <int Method, bool Strided>
void BM_NpRunOffsets(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto distribution = state.range(1);
    constexpr size_t step = Strided ? 2 : 1;
    std::vector<uint64_t> keys(count * step);
    std::vector<size_t> output(count + 1), expected{0};
    std::mt19937 random(6367);
    for (size_t i = 0; i != count; ++i) {
        keys[i * step] = distribution == 0   ? 7
                         : distribution == 1 ? i % 2
                         : distribution == 2 ? i / run_fixture_block_size
                                             : random() % 7;
        if (i != 0 && keys[(i - 1) * step] != keys[i * step])
            expected.push_back(i);
    }
    if (count != 0)
        expected.push_back(count);
    const auto runs = run_offsets_call<Method, Strided>(output.data(), keys.data(), count);
    if (runs + 1 != expected.size() ||
        !std::equal(expected.begin(), expected.end(), output.begin())) {
        state.SkipWithError("run offsets differ from adjacent comparison oracle");
        return;
    }
    for (auto _ : state) {
        auto found = run_offsets_call<Method, Strided>(output.data(), keys.data(), count);
        benchmark::DoNotOptimize(found);
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}
void run_offsets_cases(benchmark::internal::Benchmark *benchmark) {
    for (int64_t count : {0, 1, 32, 1024, 65536})
        for (int64_t distribution : {0, 1, 2, 3})
            benchmark->Args({count, distribution});
}

// Research only: identical view setup and unknown runtime step across requests.
template <int Method>
WJR_NOINLINE size_t run_offsets_runtime_call(size_t *output, const uint64_t *keys, size_t count,
                                             size_t step) noexcept {
    using shape = np::dextents<size_t, 1>;
    const np::tensor_view<size_t, shape> dst(output, count + 1);
    const np::tensor_view<const uint64_t, shape, np::layout_stride> src(
        keys, np::layout_stride::mapping<shape>(shape(count), std::array<size_t, 1>{step}));
    if constexpr (Method == 1)
        return np::run_offsets_to(dst, src);
    else if constexpr (Method == 2)
        return np::old_run_offsets_to(dst, src);
    else {
        size_t *WJR_RESTRICT out = output;
        out[0] = 0;
        if (count == 0)
            return 0;
        size_t runs = 1;
        for (size_t i = 1; i != count; ++i)
            if (keys[(i - 1) * step] != keys[i * step])
                out[runs++] = i;
        out[runs] = count;
        return runs;
    }
}

template <int Method>
void BM_NpRunOffsetsMixed(benchmark::State &state) {
    constexpr std::array<size_t, 4> steps{1, 2, 3, 5};
    const auto count = static_cast<size_t>(state.range(0));
    const auto distribution = state.range(1);
    const auto selected_step = static_cast<size_t>(state.range(2));
    std::array<std::vector<uint64_t>, steps.size()> keys;
    std::vector<uint64_t> logical(count);
    std::vector<size_t> expected{0}, output(count + 1);
    std::mt19937 random(6367);
    for (size_t i = 0; i != count; ++i) {
        logical[i] = distribution == 0   ? 7
                     : distribution == 1 ? i % 2
                     : distribution == 2 ? i / run_fixture_block_size
                                         : random() % 7;
        if (i != 0 && logical[i - 1] != logical[i])
            expected.push_back(i);
    }
    if (count != 0)
        expected.push_back(count);
    for (size_t stream = 0; stream != steps.size(); ++stream) {
        const auto step = steps[stream];
        keys[stream].resize(count == 0 ? 0 : (count - 1) * step + 1);
        for (size_t i = 0; i != count; ++i)
            keys[stream][i * step] = logical[i];
        const auto runs =
            run_offsets_runtime_call<Method>(output.data(), keys[stream].data(), count, step);
        if (runs + 1 != expected.size() ||
            !std::equal(expected.begin(), expected.end(), output.begin())) {
            state.SkipWithError("runtime-stride boundaries differ from oracle");
            return;
        }
    }
    size_t stream = selected_step == 0
                        ? 0
                        : static_cast<size_t>(std::find(steps.begin(), steps.end(), selected_step) -
                                              steps.begin());
    for (auto _ : state) {
        auto runs = run_offsets_runtime_call<Method>(output.data(), keys[stream].data(), count,
                                                     steps[stream]);
        benchmark::DoNotOptimize(runs);
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
        if (selected_step == 0)
            stream = (stream + 1) % steps.size();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}

void run_offsets_runtime_cases(benchmark::internal::Benchmark *benchmark) {
    for (int64_t count : {0, 1, 32, 1024, 65536})
        for (int64_t distribution : {0, 1, 2, 3})
            benchmark->Args({count, distribution, 0});
    for (int64_t count : {1024, 65536})
        for (int64_t distribution : {0, 1, 2, 3})
            for (int64_t step : {1, 2, 3, 5})
                benchmark->Args({count, distribution, step});
}

BENCHMARK_TEMPLATE(BM_NpRunOffsets, 0, false)->Apply(run_offsets_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsets, 0, true)->Apply(run_offsets_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsets, 1, false)->Apply(run_offsets_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsets, 1, true)->Apply(run_offsets_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsets, 2, false)->Apply(run_offsets_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsets, 2, true)->Apply(run_offsets_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsetsMixed, 0)->Apply(run_offsets_runtime_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsetsMixed, 1)->Apply(run_offsets_runtime_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsetsMixed, 2)->Apply(run_offsets_runtime_cases)->UseRealTime();
} // namespace
