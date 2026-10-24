#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/put.hpp>

namespace {
namespace np = wjr::np;

template <bool Library, bool Strided>
WJR_NOINLINE void scatter(double *WJR_RESTRICT output, const uint32_t *indices,
                          const double *values, size_t count, size_t destination_count) noexcept {
    constexpr size_t output_step = Strided ? 3 : 1;
    constexpr size_t index_step = Strided ? 2 : 1;
    constexpr size_t value_step = Strided ? 2 : 1;
    if constexpr (Library) {
        using shape = np::dextents<size_t, 1>;
        if constexpr (Strided) {
            const np::layout_stride::mapping<shape> output_mapping(
                shape(destination_count), std::array<size_t, 1>{output_step});
            const np::layout_stride::mapping<shape> update_mapping(
                shape(count), std::array<size_t, 1>{index_step});
            np::put_to(
                np::tensor_view<double, shape, np::layout_stride>(output, output_mapping),
                np::tensor_view<const uint32_t, shape, np::layout_stride>(indices, update_mapping),
                np::tensor_view<const double, shape, np::layout_stride>(values, update_mapping));
        } else {
            np::put_to(np::tensor_view<double, shape>(output, destination_count),
                       np::tensor_view<const uint32_t, shape>(indices, count),
                       np::tensor_view<const double, shape>(values, count));
        }
    } else {
        for (size_t i = 0; i != count; ++i) {
            const auto index = static_cast<size_t>(indices[i * index_step]);
            WJR_ASSERT_L2(index < destination_count);
            output[index * output_step] = values[i * value_step];
        }
    }
}

template <bool Library, bool Strided>
void BM_NpPut(benchmark::State &state) {
    const auto count = static_cast<size_t>(state.range(0));
    const auto destination_count = static_cast<size_t>(state.range(1));
    const auto distribution = state.range(2);
    constexpr size_t output_step = Strided ? 3 : 1;
    constexpr size_t update_step = Strided ? 2 : 1;
    std::vector<double> output(destination_count * output_step, -1), expected(output);
    std::vector<double> values(count * update_step);
    std::vector<uint32_t> indices(count * update_step);
    std::mt19937 engine(17);
    for (size_t i = 0; i != count; ++i) {
        const auto index =
            static_cast<uint32_t>(distribution == 0   ? i % destination_count
                                  : distribution == 1 ? destination_count / 2
                                  : distribution == 3 ? i * destination_count / count
                                                      : engine() % destination_count);
        indices[i * update_step] = index;
        values[i * update_step] = static_cast<double>(i);
        expected[index * output_step] = static_cast<double>(i);
    }
    scatter<Library, Strided>(output.data(), indices.data(), values.data(), count,
                              destination_count);
    if (output != expected) {
        state.SkipWithError("scatter result mismatch");
        return;
    }
    for (auto _ : state) {
        scatter<Library, Strided>(output.data(), indices.data(), values.data(), count,
                                  destination_count);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(count));
}

void put_arguments(benchmark::internal::Benchmark *bench) {
    for (int64_t count : {0, 1, 32, 1024, 65536})
        for (int64_t destination_count : {16, 65536})
            for (int64_t distribution : {0, 1, 2, 3})
                bench->Args({count, destination_count, distribution});
}

BENCHMARK_TEMPLATE(BM_NpPut, false, false)->Apply(put_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpPut, true, false)->Apply(put_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpPut, false, true)->Apply(put_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpPut, true, true)->Apply(put_arguments)->UseRealTime();
} // namespace
