#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/take.hpp>

namespace {
namespace np = wjr::np;

template <bool Library>
WJR_NOINLINE void gather(double *WJR_RESTRICT output, const double *WJR_RESTRICT source,
                         const uint32_t *WJR_RESTRICT indices, size_t count,
                         [[maybe_unused]] size_t source_count) {
    if constexpr (Library) {
        np::take_to(np::tensor_view<double, np::dextents<size_t, 1>>(output, count),
                    np::tensor_view<const double, np::dextents<size_t, 1>>(source, source_count),
                    np::tensor_view<const uint32_t, np::dextents<size_t, 1>>(indices, count));
    } else {
        for (size_t i = 0; i != count; ++i) {
            const auto index = static_cast<size_t>(indices[i]);
            WJR_ASSERT_L2(index < source_count);
            output[i] = source[index];
        }
    }
}

template <bool Library>
void BM_NpTake(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto source_count = static_cast<size_t>(state.range(1));
    const auto distribution = state.range(2);
    std::vector<double> source(source_count), output(count);
    std::vector<uint32_t> indices(count);
    std::mt19937 engine(17);
    for (size_t i = 0; i != source_count; ++i)
        source[i] = static_cast<double>(i);
    for (size_t i = 0; i != count; ++i)
        indices[i] = static_cast<uint32_t>(distribution == 0   ? i % source_count
                                           : distribution == 1 ? source_count / 2
                                                               : engine() % source_count);
    gather<Library>(output.data(), source.data(), indices.data(), count, source_count);
    for (size_t i = 0; i != count; ++i) {
        if (output[i] != source[indices[i]]) {
            state.SkipWithError("gather result mismatch");
            return;
        }
    }
    for (auto _ : state) {
        gather<Library>(output.data(), source.data(), indices.data(), count, source_count);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}

void take_arguments(benchmark::internal::Benchmark *bench) {
    for (int64_t count : {0, 1, 32, 1024, 65536})
        for (int64_t source_count : {16, 65536})
            for (int64_t distribution : {0, 1, 2})
                bench->Args({count, source_count, distribution});
}

BENCHMARK_TEMPLATE(BM_NpTake, false)->Apply(take_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpTake, true)->Apply(take_arguments)->UseRealTime();
} // namespace
