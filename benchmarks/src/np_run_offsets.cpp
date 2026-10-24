#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/run_offsets.hpp>

namespace {
namespace np = wjr::np;
inline constexpr size_t run_fixture_block_size = 16;

template <bool Library, bool Strided>
WJR_NOINLINE size_t run_offsets_call(size_t *output, const uint64_t *keys, size_t count) noexcept {
    constexpr size_t step = Strided ? 2 : 1;
    if constexpr (Library) {
        using shape = np::dextents<size_t, 1>;
        const np::tensor_view<size_t, shape> dst(output, count + 1);
        if constexpr (Strided) {
            const np::tensor_view<const uint64_t, shape, np::layout_stride> src(
                keys, np::layout_stride::mapping<shape>(shape(count), std::array<size_t, 1>{step}));
            return np::run_offsets_to(dst, src);
        } else {
            return np::run_offsets_to(dst, np::tensor_view<const uint64_t, shape>(keys, count));
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

template <bool Library, bool Strided>
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
    const auto runs = run_offsets_call<Library, Strided>(output.data(), keys.data(), count);
    if (runs + 1 != expected.size() ||
        !std::equal(expected.begin(), expected.end(), output.begin())) {
        state.SkipWithError("run offsets differ from adjacent comparison oracle");
        return;
    }
    for (auto _ : state) {
        auto found = run_offsets_call<Library, Strided>(output.data(), keys.data(), count);
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
BENCHMARK_TEMPLATE(BM_NpRunOffsets, true, false)->Apply(run_offsets_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsets, false, false)->Apply(run_offsets_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsets, true, true)->Apply(run_offsets_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpRunOffsets, false, true)->Apply(run_offsets_cases)->UseRealTime();
} // namespace
