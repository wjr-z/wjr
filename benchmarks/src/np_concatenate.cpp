#include <algorithm>
#include <array>
#include <cstring>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/concatenate.hpp>

namespace {
namespace np = wjr::np;

template <bool Library, size_t Axis, bool Strided>
WJR_NOINLINE void concatenate(double *WJR_RESTRICT output, const double *first,
                              const double *second, size_t rows, size_t cols) noexcept {
    const auto output_cols = Axis == 1 ? cols * 2 : cols;
    constexpr size_t step = Strided ? 2 : 1;
    const auto input_row_step = cols * step + (Strided ? 3 : 0);
    const auto output_row_step = output_cols * step + (Strided ? 3 : 0);
    if constexpr (Library) {
        using shape = np::dextents<size_t, 2>;
        const shape input_shape(rows, cols);
        const shape output_shape(Axis == 0 ? rows * 2 : rows, output_cols);
        if constexpr (Strided) {
            const np::layout_stride::mapping<shape> im(input_shape,
                                                       std::array<size_t, 2>{input_row_step, step});
            const np::layout_stride::mapping<shape> om(
                output_shape, std::array<size_t, 2>{output_row_step, step});
            np::concatenate_to(np::tensor_view<double, shape, np::layout_stride>(output, om),
                               np::axes<Axis>,
                               np::tensor_view<const double, shape, np::layout_stride>(first, im),
                               np::tensor_view<const double, shape, np::layout_stride>(second, im));
        } else {
            np::concatenate_to(np::tensor_view<double, shape>(output, output_shape), np::axes<Axis>,
                               np::tensor_view<const double, shape>(first, input_shape),
                               np::tensor_view<const double, shape>(second, input_shape));
        }
    } else {
        if (rows == 0 || cols == 0)
            return;
        if constexpr (!Strided && Axis == 0) {
            const auto count = rows * cols;
            // Flawfinder: ignore - fixture allocates both rows*cols output blocks.
            std::memcpy(output, first, count * sizeof(double));
            // Flawfinder: ignore - the second block has count elements too.
            std::memcpy(output + count, second, count * sizeof(double));
        } else if constexpr (!Strided) {
            for (size_t row = 0; row != rows; ++row) {
                // Flawfinder: ignore - each output row has space for both inputs.
                std::memcpy(output + row * output_row_step, first + row * input_row_step,
                            cols * sizeof(double));
                // Flawfinder: ignore - the second cols-wide block fits this row.
                std::memcpy(output + row * output_row_step + cols, second + row * input_row_step,
                            cols * sizeof(double));
            }
        } else {
            for (size_t row = 0; row != rows; ++row)
                for (size_t col = 0; col != cols; ++col) {
                    output[row * output_row_step + col * step] =
                        first[row * input_row_step + col * step];
                    output[row * output_row_step + (cols + col) * step] =
                        second[row * input_row_step + col * step];
                }
        }
    }
}

template <bool Library, size_t Axis, bool Strided>
void BM_NpConcatenate(benchmark::State &state) {
    const auto rows = static_cast<size_t>(state.range(0));
    const auto cols = static_cast<size_t>(state.range(1));
    constexpr size_t step = Strided ? 2 : 1;
    const auto input_row_step = cols * step + (Strided ? 3 : 0);
    const auto output_cols = Axis == 1 ? cols * 2 : cols;
    const auto output_row_step = output_cols * step + (Strided ? 3 : 0);
    std::vector<double> first(rows * input_row_step), second(first.size());
    std::vector<double> output((Axis == 0 ? rows * 2 : rows) * output_row_step, -1),
        expected(output);
    for (size_t row = 0; row != rows; ++row)
        for (size_t col = 0; col != cols; ++col) {
            const auto offset = row * input_row_step + col * step;
            first[offset] = static_cast<double>(row * cols + col);
            second[offset] = -first[offset];
            expected[row * output_row_step + col * step] = first[offset];
            const auto out = Axis == 0 ? (rows + row) * output_row_step + col * step
                                       : row * output_row_step + (cols + col) * step;
            expected[out] = second[offset];
        }
    concatenate<Library, Axis, Strided>(output.data(), first.data(), second.data(), rows, cols);
    if (output != expected) {
        state.SkipWithError("concatenation result mismatch");
        return;
    }
    for (auto _ : state) {
        concatenate<Library, Axis, Strided>(output.data(), first.data(), second.data(), rows, cols);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(rows * cols * 2));
}

void concatenate_arguments(benchmark::internal::Benchmark *bench) {
    for (int64_t rows : {0, 1, 32, 1024})
        for (int64_t cols : {1, 8, 64})
            bench->Args({rows, cols});
}

BENCHMARK_TEMPLATE(BM_NpConcatenate, false, 0, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, true, 0, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, false, 1, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, true, 1, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, false, 1, true)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, true, 1, true)->Apply(concatenate_arguments)->UseRealTime();
} // namespace
