#include <cstdint>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/compact.hpp>

namespace {
namespace np = wjr::np;

template <bool Library, bool Positions>
WJR_NOINLINE size_t compact_call(std::conditional_t<Positions, size_t, double> *output,
                                 const double *input, const uint8_t *mask, size_t count) noexcept {
    using D = std::conditional_t<Positions, size_t, double>;
    if constexpr (Library) {
        using shape = np::dextents<size_t, 1>;
        const np::tensor_view<D, shape> dst(output, count);
        const np::tensor_view<const double, shape> src(input, count);
        if constexpr (Positions)
            return np::flatnonzero_to(dst, src);
        else
            return np::compress_to(dst, src, np::tensor_view<const uint8_t, shape>(mask, count));
    } else {
        D *WJR_RESTRICT dst = output;
        size_t written = 0;
        for (size_t i = 0; i != count; ++i) {
            if constexpr (Positions) {
                if (input[i] != 0)
                    dst[written++] = i;
            } else {
                if (mask[i] != 0)
                    dst[written++] = input[i];
            }
        }
        return written;
    }
}

template <bool Library, bool Positions>
void BM_NpCompact(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto distribution = state.range(1);
    using D = std::conditional_t<Positions, size_t, double>;
    std::vector<double> input(count);
    std::vector<uint8_t> mask(count);
    std::vector<D> output(count), expected;
    std::mt19937 engine(1627);
    for (size_t i = 0; i != count; ++i) {
        const bool selected = distribution == 0   ? false
                              : distribution == 1 ? true
                              : distribution == 2 ? i % 2 != 0
                                                  : (engine() % 2 != 0);
        input[i] = selected ? static_cast<double>(i + 1) : 0.;
        mask[i] = selected;
        if (selected) {
            if constexpr (Positions)
                expected.push_back(i);
            else
                expected.push_back(input[i]);
        }
    }
    const auto found =
        compact_call<Library, Positions>(output.data(), input.data(), mask.data(), count);
    if (found != expected.size() || !std::equal(expected.begin(), expected.end(), output.begin())) {
        state.SkipWithError("compaction differs from scalar oracle");
        return;
    }
    for (auto _ : state) {
        auto written =
            compact_call<Library, Positions>(output.data(), input.data(), mask.data(), count);
        benchmark::DoNotOptimize(written);
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}

void compaction_cases(benchmark::internal::Benchmark *benchmark) {
    for (int64_t count : {0, 1, 17, 1024, 65536})
        for (int64_t distribution : {0, 1, 2, 3})
            benchmark->Args({count, distribution});
}

BENCHMARK_TEMPLATE(BM_NpCompact, true, false)->Apply(compaction_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpCompact, false, false)->Apply(compaction_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpCompact, true, true)->Apply(compaction_cases)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpCompact, false, true)->Apply(compaction_cases)->UseRealTime();
} // namespace
