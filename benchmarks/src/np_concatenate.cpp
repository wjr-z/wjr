#include <algorithm>
#include <array>
#include <cstring>
#include <vector>

#include <benchmark/benchmark.h>

#include "np_inline_concatenate.hpp"
#include "np_old_concatenate.hpp"
#include "np_static_concatenate.hpp"
#include <random>
#include <wjr/np/concatenate.hpp>

namespace {
namespace np = wjr::np;
template <int Method, class... Args>
WJR_INTRINSIC_INLINE constexpr void invoke_concatenate(Args... args) noexcept {
    if constexpr (Method == 2)
        np::old_concat::concatenate_to(args...);
    else if constexpr (Method == 7)
        np::inline_unit_concat::concatenate_to(args...);
    else if constexpr (Method == 8)
        np::static_unit_concat::concatenate_to(args...);
    else
        np::concatenate_to(args...);
}

template <int Method>
void copy_block(double *WJR_RESTRICT output, const double *input, size_t bytes) noexcept {
    if constexpr (Method == 3)
        std::copy_n(input, bytes / sizeof(double), output);
    else
        std::memcpy(output, input, bytes);
}

template <int Method, size_t Axis, bool Strided>
WJR_NOINLINE void concatenate(double *WJR_RESTRICT output, const double *first,
                              const double *second, size_t rows, size_t cols) noexcept {
    const auto output_cols = Axis == 1 ? cols * 2 : cols;
    constexpr size_t step = Strided ? 2 : 1;
    const auto input_row_step = cols * step + (Strided ? 3 : 0);
    const auto output_row_step = output_cols * step + (Strided ? 3 : 0);
    if constexpr (Method == 1 || Method == 2 || Method == 7 || Method == 8) {
        using shape = np::dextents<size_t, 2>;
        const shape input_shape(rows, cols);
        const shape output_shape(Axis == 0 ? rows * 2 : rows, output_cols);
        if constexpr (Strided) {
            const np::layout_stride::mapping<shape> im(input_shape,
                                                       std::array<size_t, 2>{input_row_step, step});
            const np::layout_stride::mapping<shape> om(
                output_shape, std::array<size_t, 2>{output_row_step, step});
            invoke_concatenate<Method>(
                np::tensor_view<double, shape, np::layout_stride>(output, om), np::axes<Axis>,
                np::tensor_view<const double, shape, np::layout_stride>(first, im),
                np::tensor_view<const double, shape, np::layout_stride>(second, im));
        } else {
            invoke_concatenate<Method>(np::tensor_view<double, shape>(output, output_shape),
                                       np::axes<Axis>,
                                       np::tensor_view<const double, shape>(first, input_shape),
                                       np::tensor_view<const double, shape>(second, input_shape));
        }
    } else {
        if (rows == 0 || cols == 0)
            return;
        if constexpr (Method == 6 && Strided && Axis == 1) {
            if (cols == 1) {
                for (size_t row = 0; row != rows; ++row) {
                    output[row * output_row_step] = first[row * input_row_step];
                    output[row * output_row_step + step] = second[row * input_row_step];
                }
                return;
            }
        }
        if constexpr (!Strided && Axis == 0) {
            const auto count = rows * cols;
            // Flawfinder: ignore - fixture allocates both rows*cols output blocks.
            copy_block<Method>(output, first, count * sizeof(double));
            // Flawfinder: ignore - the second block has count elements too.
            copy_block<Method>(output + count, second, count * sizeof(double));
        } else if constexpr (!Strided) {
            for (size_t row = 0; row != rows; ++row) {
                // Flawfinder: ignore - each output row has space for both inputs.
                copy_block<Method>(output + row * output_row_step, first + row * input_row_step,
                                   cols * sizeof(double));
                // Flawfinder: ignore - the second cols-wide block fits this row.
                copy_block<Method>(output + row * output_row_step + cols,
                                   second + row * input_row_step, cols * sizeof(double));
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

template <int Method, size_t Axis, bool Strided>
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
    concatenate<Method, Axis, Strided>(output.data(), first.data(), second.data(), rows, cols);
    if (output != expected) {
        state.SkipWithError("concatenation result mismatch");
        return;
    }
    for (auto _ : state) {
        concatenate<Method, Axis, Strided>(output.data(), first.data(), second.data(), rows, cols);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(rows * cols * 2));
}

void concatenate_arguments(benchmark::internal::Benchmark *bench) {
    for (int64_t rows : {0, 1, 32, 1024})
        for (int64_t cols : {1, 8, 64})
            bench->Args({rows, cols});
}

BENCHMARK_TEMPLATE(BM_NpConcatenate, 0, 0, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 1, 0, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 0, 1, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 1, 1, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 0, 1, true)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 1, 1, true)->Apply(concatenate_arguments)->UseRealTime();

BENCHMARK_TEMPLATE(BM_NpConcatenate, 2, 0, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 2, 1, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 2, 1, true)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 3, 0, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 3, 1, false)->Apply(concatenate_arguments)->UseRealTime();
template <int Method>
WJR_NOINLINE void concatenate_unequal(double *WJR_RESTRICT output, const double *first,
                                      const double *second, size_t rows, size_t left_cols,
                                      size_t right_cols) noexcept {
    const auto left_row_step = left_cols * 2 + 3;
    const auto right_row_step = right_cols * 2 + 3;
    const auto output_row_step = (left_cols + right_cols) * 2 + 3;
    if constexpr (Method == 0 || Method == 6) {
        if constexpr (Method == 6) {
            if (left_cols == 1 && right_cols == 1) {
                for (size_t row = 0; row != rows; ++row) {
                    output[row * output_row_step] = first[row * left_row_step];
                    output[row * output_row_step + 2] = second[row * right_row_step];
                }
                return;
            }
        }
        const auto common = (std::min)(left_cols, right_cols);
        for (size_t row = 0; row != rows; ++row) {
            for (size_t col = 0; col != common; ++col) {
                output[row * output_row_step + col * 2] = first[row * left_row_step + col * 2];
                output[row * output_row_step + (left_cols + col) * 2] =
                    second[row * right_row_step + col * 2];
            }
            for (size_t col = common; col != left_cols; ++col)
                output[row * output_row_step + col * 2] = first[row * left_row_step + col * 2];
            for (size_t col = common; col != right_cols; ++col)
                output[row * output_row_step + (left_cols + col) * 2] =
                    second[row * right_row_step + col * 2];
        }
    } else {
        using shape = np::dextents<size_t, 2>;
        const auto view = [rows]<class T>(T *ptr, size_t cols, size_t row_step) {
            return np::tensor_view<T, shape, np::layout_stride>(
                ptr, np::layout_stride::mapping<shape>(shape(rows, cols),
                                                       std::array<size_t, 2>{row_step, 2}));
        };
        invoke_concatenate<Method>(view(output, left_cols + right_cols, output_row_step),
                                   np::axes<1>, view(first, left_cols, left_row_step),
                                   view(second, right_cols, right_row_step));
    }
}

template <int Method>
void BM_NpConcatenateUnequal(benchmark::State &state) {
    const auto rows = static_cast<size_t>(state.range(0));
    const auto left_cols = static_cast<size_t>(state.range(1));
    const auto right_cols = static_cast<size_t>(state.range(2));
    const auto left_row_step = left_cols * 2 + 3;
    const auto right_row_step = right_cols * 2 + 3;
    const auto output_row_step = (left_cols + right_cols) * 2 + 3;
    std::vector<double> first(rows * left_row_step), second(rows * right_row_step);
    std::vector<double> output(rows * output_row_step, -99), expected(output);
    for (size_t row = 0; row != rows; ++row) {
        for (size_t col = 0; col != left_cols; ++col) {
            const auto offset = row * left_row_step + col * 2;
            first[offset] = static_cast<double>(row * left_cols + col);
            expected[row * output_row_step + col * 2] = first[offset];
        }
        for (size_t col = 0; col != right_cols; ++col) {
            const auto offset = row * right_row_step + col * 2;
            second[offset] = -static_cast<double>(row * right_cols + col);
            expected[row * output_row_step + (left_cols + col) * 2] = second[offset];
        }
    }
    const auto *a = left_cols ? first.data() : nullptr;
    const auto *b = right_cols ? second.data() : nullptr;
    concatenate_unequal<Method>(output.data(), a, b, rows, left_cols, right_cols);
    if (output != expected) {
        state.SkipWithError("unequal concatenation mismatch");
        return;
    }
    for (auto _ : state) {
        concatenate_unequal<Method>(output.data(), a, b, rows, left_cols, right_cols);
        benchmark::ClobberMemory();
    }
}
void unequal_arguments(benchmark::internal::Benchmark *bench) {
    for (int64_t rows : {32, 1024})
        for (auto cols :
             {std::array<int64_t, 2>{1, 1}, {1, 8}, {8, 1}, {7, 64}, {64, 7}, {0, 8}, {8, 0}})
            bench->Args({rows, cols[0], cols[1]});
}
BENCHMARK_TEMPLATE(BM_NpConcatenateUnequal, 0)->Apply(unequal_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateUnequal, 1)->Apply(unequal_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateUnequal, 2)->Apply(unequal_arguments)->UseRealTime();

inline constexpr size_t concatenate_mixed_sequence_size = 96;
template <int Method, bool Shuffled>
void BM_NpConcatenateMixed(benchmark::State &state) {
    struct fixture {
        size_t cols;
        std::vector<double> first, second, output, expected;
    };
    const auto rows = static_cast<size_t>(state.range(0));
    std::array<fixture, 3> fixtures;
    size_t slot = 0;
    for (size_t cols : {size_t{1}, size_t{8}, size_t{64}}) {
        auto &f = fixtures[slot++];
        f.cols = cols;
        const auto input_step = cols * 2 + 3;
        const auto output_step = cols * 4 + 3;
        f.first.resize(rows * input_step);
        f.second.resize(rows * input_step);
        f.output.assign(rows * output_step, -99);
        f.expected = f.output;
        for (size_t row = 0; row != rows; ++row)
            for (size_t col = 0; col != cols; ++col) {
                f.first[row * input_step + col * 2] = static_cast<double>(row * cols + col);
                f.second[row * input_step + col * 2] = -f.first[row * input_step + col * 2];
                f.expected[row * output_step + col * 2] = f.first[row * input_step + col * 2];
                f.expected[row * output_step + (cols + col) * 2] =
                    f.second[row * input_step + col * 2];
            }
        concatenate<Method, 1, true>(f.output.data(), f.first.data(), f.second.data(), rows, cols);
        if (f.output != f.expected) {
            state.SkipWithError("mixed concatenation mismatch");
            return;
        }
    }
    std::array<size_t, concatenate_mixed_sequence_size> sequence;
    for (size_t i = 0; i != sequence.size(); ++i)
        sequence[i] = i % fixtures.size();
    if constexpr (Shuffled) {
        std::mt19937 engine(41);
        std::shuffle(sequence.begin(), sequence.end(), engine);
    }
    size_t i = 0;
    for (auto _ : state) {
        auto &f = fixtures[sequence[i]];
        concatenate<Method, 1, true>(f.output.data(), f.first.data(), f.second.data(), rows,
                                     f.cols);
        i = (i + 1) % sequence.size();
        benchmark::ClobberMemory();
    }
}
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 0, false)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 1, false)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 2, false)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 0, true)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 1, true)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 2, true)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();

BENCHMARK_TEMPLATE(BM_NpConcatenate, 6, 1, true)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateUnequal, 6)->Apply(unequal_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 6, false)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 6, true)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();

BENCHMARK_TEMPLATE(BM_NpConcatenate, 7, 0, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 7, 1, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 7, 1, true)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateUnequal, 7)->Apply(unequal_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 7, false)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 7, true)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 8, 0, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 8, 1, false)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, 8, 1, true)->Apply(concatenate_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateUnequal, 8)->Apply(unequal_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 8, false)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, 8, true)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();

} // namespace
