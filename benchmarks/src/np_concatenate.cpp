#include <algorithm>
#include <array>
#include <cstring>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/concatenate.hpp>

namespace {
namespace np = wjr::np;

// Same disjoint output contract, including the standard algorithm's small-block path.
template <bool CopyN>
void copy_concatenate_block(double *WJR_RESTRICT output, const double *input,
                            size_t bytes) noexcept {
    if constexpr (CopyN)
        std::copy_n(input, bytes / sizeof(double), output);
    else {
        // Flawfinder: ignore - callers copy bounded blocks in allocated fixture buffers.
        std::memcpy(output, input, bytes);
    }
}

template <bool Library, size_t Axis, bool Strided, bool CopyN = false, bool UnitReference = false>
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
        if constexpr (UnitReference && Strided && Axis == 1) {
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
            copy_concatenate_block<CopyN>(output, first, count * sizeof(double));
            // Flawfinder: ignore - the second block has count elements too.
            copy_concatenate_block<CopyN>(output + count, second, count * sizeof(double));
        } else if constexpr (!Strided) {
            for (size_t row = 0; row != rows; ++row) {
                // Flawfinder: ignore - each output row has space for both inputs.
                copy_concatenate_block<CopyN>(output + row * output_row_step,
                                              first + row * input_row_step, cols * sizeof(double));
                // Flawfinder: ignore - the second cols-wide block fits this row.
                copy_concatenate_block<CopyN>(output + row * output_row_step + cols,
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

template <bool Library, size_t Axis, bool Strided, bool CopyN = false, bool UnitReference = false>
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
    concatenate<Library, Axis, Strided, CopyN, UnitReference>(output.data(), first.data(),
                                                              second.data(), rows, cols);
    if (output != expected) {
        state.SkipWithError("concatenation result mismatch");
        return;
    }
    for (auto _ : state) {
        concatenate<Library, Axis, Strided, CopyN, UnitReference>(output.data(), first.data(),
                                                                  second.data(), rows, cols);
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
// Keep the memcpy reference and also compare the standard algorithm's one-element
// path; runtime memcpy alone is not a strong baseline for narrow inner blocks.
BENCHMARK_TEMPLATE(BM_NpConcatenate, false, 0, false, true)
    ->Apply(concatenate_arguments)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenate, false, 1, false, true)
    ->Apply(concatenate_arguments)
    ->UseRealTime();
template <bool Library, bool UnitReference = false>
WJR_NOINLINE void concatenate_unequal(double *WJR_RESTRICT output, const double *first,
                                      const double *second, size_t rows, size_t left_cols,
                                      size_t right_cols) noexcept {
    const auto left_row_step = left_cols * 2 + 3;
    const auto right_row_step = right_cols * 2 + 3;
    const auto output_row_step = (left_cols + right_cols) * 2 + 3;
    if constexpr (!Library) {
        if constexpr (UnitReference) {
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
        np::concatenate_to(view(output, left_cols + right_cols, output_row_step), np::axes<1>,
                           view(first, left_cols, left_row_step),
                           view(second, right_cols, right_row_step));
    }
}

template <bool Library, bool UnitReference = false>
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
    concatenate_unequal<Library, UnitReference>(output.data(), a, b, rows, left_cols, right_cols);
    if (output != expected) {
        state.SkipWithError("unequal concatenation mismatch");
        return;
    }
    for (auto _ : state) {
        concatenate_unequal<Library, UnitReference>(output.data(), a, b, rows, left_cols,
                                                    right_cols);
        benchmark::ClobberMemory();
    }
}
void unequal_arguments(benchmark::internal::Benchmark *bench) {
    for (int64_t rows : {32, 1024})
        for (auto cols :
             {std::array<int64_t, 2>{1, 1}, {1, 8}, {8, 1}, {7, 64}, {64, 7}, {0, 8}, {8, 0}})
            bench->Args({rows, cols[0], cols[1]});
}
inline constexpr size_t concatenate_mixed_sequence_size = 96;
template <bool Library, bool Shuffled, bool UnitReference = false>
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
        concatenate<Library, 1, true, false, UnitReference>(f.output.data(), f.first.data(),
                                                            f.second.data(), rows, cols);
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
        concatenate<Library, 1, true, false, UnitReference>(f.output.data(), f.first.data(),
                                                            f.second.data(), rows, f.cols);
        i = (i + 1) % sequence.size();
        benchmark::ClobberMemory();
    }
}

// Keep the generic references and also measure a handwritten scalar-column path.
BENCHMARK_TEMPLATE(BM_NpConcatenate, false, 1, true, false, true)
    ->Apply(concatenate_arguments)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateUnequal, false)->Apply(unequal_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateUnequal, true)->Apply(unequal_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateUnequal, false, true)->Apply(unequal_arguments)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, false, false)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, true, false)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, false, false, true)
    ->Arg(1)
    ->Arg(32)
    ->Arg(1024)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, false, true)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, true, true)->Arg(1)->Arg(32)->Arg(1024)->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateMixed, false, true, true)
    ->Arg(1)
    ->Arg(32)
    ->Arg(1024)
    ->UseRealTime();

// The concatenated extent is encoded in the view type, without runtime dispatch.
template <bool Library>
WJR_NOINLINE void concatenate_static_unit(double *WJR_RESTRICT output, const double *first,
                                          const double *second, size_t rows) noexcept {
    constexpr size_t element_step = 2, padding = 3;
    constexpr size_t input_row_step = element_step + padding;
    constexpr size_t output_row_step = element_step * 2 + padding;
    if constexpr (Library) {
        using source_shape = np::extents<size_t, np::dynamic_extent, 1>;
        using target_shape = np::extents<size_t, np::dynamic_extent, 2>;
        const np::layout_stride::mapping<source_shape> input_mapping(
            source_shape(rows), std::array<size_t, 2>{input_row_step, element_step});
        const np::layout_stride::mapping<target_shape> output_mapping(
            target_shape(rows), std::array<size_t, 2>{output_row_step, element_step});
        np::concatenate_to(
            np::tensor_view<double, target_shape, np::layout_stride>(output, output_mapping),
            np::axes<1>,
            np::tensor_view<const double, source_shape, np::layout_stride>(first, input_mapping),
            np::tensor_view<const double, source_shape, np::layout_stride>(second, input_mapping));
    } else {
        for (size_t row = 0; row != rows; ++row) {
            output[row * output_row_step] = first[row * input_row_step];
            output[row * output_row_step + element_step] = second[row * input_row_step];
        }
    }
}

template <bool Library>
void BM_NpConcatenateStaticUnit(benchmark::State &state) {
    constexpr size_t element_step = 2, padding = 3;
    constexpr size_t input_row_step = element_step + padding;
    constexpr size_t output_row_step = element_step * 2 + padding;
    const auto rows = static_cast<size_t>(state.range(0));
    std::vector<double> first(rows ? (rows - 1) * input_row_step + 1 : 0), second(first.size());
    std::vector<double> output(rows ? (rows - 1) * output_row_step + element_step + 1 : 0, -99),
        expected(output);
    for (size_t row = 0; row != rows; ++row) {
        first[row * input_row_step] = static_cast<double>(row + 1);
        second[row * input_row_step] = -first[row * input_row_step];
        expected[row * output_row_step] = first[row * input_row_step];
        expected[row * output_row_step + element_step] = second[row * input_row_step];
    }
    concatenate_static_unit<Library>(output.data(), first.data(), second.data(), rows);
    if (output != expected) {
        state.SkipWithError("static-unit concatenation mismatch");
        return;
    }
    for (auto _ : state) {
        concatenate_static_unit<Library>(output.data(), first.data(), second.data(), rows);
        benchmark::ClobberMemory();
    }
}

BENCHMARK_TEMPLATE(BM_NpConcatenateStaticUnit, false)
    ->Arg(0)
    ->Arg(1)
    ->Arg(32)
    ->Arg(1024)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpConcatenateStaticUnit, true)
    ->Arg(0)
    ->Arg(1)
    ->Arg(32)
    ->Arg(1024)
    ->UseRealTime();
} // namespace
