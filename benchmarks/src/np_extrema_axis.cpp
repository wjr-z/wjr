#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

#include <benchmark/benchmark.h>

#include <wjr/np/extrema_axis.hpp>

namespace {
namespace np = wjr::np;
inline constexpr size_t argextrema_nan_period = 17;

template <bool Library, bool Maximum, size_t Axis, bool Strided, class T>
WJR_NOINLINE void axis_argextrema_call(size_t *WJR_RESTRICT output, const T *input, size_t rows,
                                       size_t columns) noexcept {
    const auto row_step = Strided ? columns * 2 + 3 : columns;
    constexpr size_t column_step = Strided ? 2 : 1;
    constexpr size_t output_step = Strided ? 2 : 1;
    const auto lines = Axis == 0 ? columns : rows;
    const auto count = Axis == 0 ? rows : columns;
    if constexpr (Library) {
        using shape = np::dextents<size_t, 2>;
        using output_shape = np::dextents<size_t, 1>;
        const auto src = [&] {
            if constexpr (Strided)
                return np::tensor_view<const T, shape, np::layout_stride>(
                    input, np::layout_stride::mapping<shape>(shape(rows, columns),
                                                             std::array{row_step, column_step}));
            else
                return np::tensor_view<const T, shape>(input, rows, columns);
        }();
        const auto dst = [&] {
            if constexpr (Strided)
                return np::tensor_view<size_t, output_shape, np::layout_stride>(
                    output, np::layout_stride::mapping<output_shape>(output_shape(lines),
                                                                     std::array{output_step}));
            else
                return np::tensor_view<size_t, output_shape>(output, lines);
        }();
        if constexpr (Maximum)
            np::argmax_to(dst, src, np::axes<Axis>);
        else
            np::argmin_to(dst, src, np::axes<Axis>);
    } else {
        const auto line_step = Axis == 0 ? column_step : row_step;
        const auto step = Axis == 0 ? row_step : column_step;
        for (size_t line = 0; line != lines; ++line) {
            const T *values = input + line * line_step;
            T best = values[0];
            size_t selected = 0;
            if constexpr (std::is_floating_point_v<T>) {
                if (best != best) {
                    output[line * output_step] = 0;
                    continue;
                }
            }
            for (size_t i = 1; i != count; ++i) {
                const T value = values[i * step];
                if constexpr (std::is_floating_point_v<T>) {
                    if (value != value) {
                        selected = i;
                        break;
                    }
                }
                if (Maximum ? best < value : value < best) {
                    best = value;
                    selected = i;
                }
            }
            output[line * output_step] = selected;
        }
    }
}

template <bool Library, bool Maximum, size_t Axis, bool Strided, class T>
void BM_NpAxisArgextrema(benchmark::State &state) {
    const auto rows = static_cast<size_t>(state.range(0));
    const auto columns = static_cast<size_t>(state.range(1));
    const auto distribution = state.range(2);
    const auto row_step = Strided ? columns * 2 + 3 : columns;
    constexpr size_t column_step = Strided ? 2 : 1;
    constexpr size_t output_step = Strided ? 2 : 1;
    const auto lines = Axis == 0 ? columns : rows;
    const auto storage =
        rows == 0 || columns == 0 ? 0 : (rows - 1) * row_step + (columns - 1) * column_step + 1;
    std::vector<T> input(storage);
    std::vector<size_t> output(lines * output_step, SIZE_MAX),
        expected(lines * output_step, SIZE_MAX);
    std::mt19937 random(8017);
    for (size_t row = 0; row != rows; ++row)
        for (size_t column = 0; column != columns; ++column) {
            const auto position = row * row_step + column * column_step;
            input[position] = static_cast<T>(distribution == 1 ? column + row : random() % 7);
            if constexpr (std::is_floating_point_v<T>)
                if (distribution == 2 && (row + column) % argextrema_nan_period == 0)
                    input[position] = std::numeric_limits<T>::quiet_NaN();
        }
    axis_argextrema_call<false, Maximum, Axis, Strided>(expected.data(), input.data(), rows,
                                                        columns);
    axis_argextrema_call<Library, Maximum, Axis, Strided>(output.data(), input.data(), rows,
                                                          columns);
    if (output != expected) {
        state.SkipWithError("axis argextremum differs from manual oracle");
        return;
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(input.data());
        axis_argextrema_call<Library, Maximum, Axis, Strided>(output.data(), input.data(), rows,
                                                              columns);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(rows * columns));
}

template <size_t Axis>
void axis_argextrema_cases(benchmark::internal::Benchmark *benchmark) {
    for (const auto shape : {std::array<int64_t, 2>{1, 1}, {8, 32}, {32, 1024}, {1024, 32}})
        for (int64_t distribution : {0, 1, 2})
            benchmark->Args({shape[0], shape[1], distribution});
    if constexpr (Axis == 0)
        benchmark->Args({16, 0, 0});
    else
        benchmark->Args({0, 16, 0});
}

BENCHMARK_TEMPLATE(BM_NpAxisArgextrema, false, false, 0, false, double)
    ->Apply(axis_argextrema_cases<0>)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpAxisArgextrema, true, false, 0, false, double)
    ->Apply(axis_argextrema_cases<0>)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpAxisArgextrema, false, false, 1, false, double)
    ->Apply(axis_argextrema_cases<1>)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpAxisArgextrema, true, false, 1, false, double)
    ->Apply(axis_argextrema_cases<1>)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpAxisArgextrema, false, true, 0, true, double)
    ->Apply(axis_argextrema_cases<0>)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpAxisArgextrema, true, true, 0, true, double)
    ->Apply(axis_argextrema_cases<0>)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpAxisArgextrema, false, true, 1, true, double)
    ->Apply(axis_argextrema_cases<1>)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpAxisArgextrema, true, true, 1, true, double)
    ->Apply(axis_argextrema_cases<1>)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpAxisArgextrema, false, true, 1, false, int64_t)
    ->Apply(axis_argextrema_cases<1>)
    ->UseRealTime();
BENCHMARK_TEMPLATE(BM_NpAxisArgextrema, true, true, 1, false, int64_t)
    ->Apply(axis_argextrema_cases<1>)
    ->UseRealTime();
} // namespace
