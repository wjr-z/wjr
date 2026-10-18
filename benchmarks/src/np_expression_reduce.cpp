#include <algorithm>
#include <array>
#include <random>
#include <string>
#include <vector>

#include <benchmark/benchmark.h>

#include "np_detail.hpp"

#include <wjr/np/expression_reduce.hpp>

namespace {
namespace np = wjr::np;
enum class backend { expression, manual };

template <class T>
struct scale_add {
    T scale;
    constexpr T operator()(T x, T bias) const noexcept {
        const auto scaled = scale * x;
        return scaled + bias;
    }
};

template <backend Backend, bool Padded, bool Mapped, class T, class Acc, bool Splat = false>
WJR_NOINLINE void kernel(T *out, const T *input, const T *bias, size_t rows, size_t columns,
                         size_t pitch, T alpha) noexcept {
    using shape = np::dextents<size_t, 2>;
    using vector = np::tensor_view<const T, np::dextents<size_t, 1>>;
    const scale_add<T> operation{alpha};
    if constexpr (Backend == backend::manual) {
        pitch = Splat ? 1 : (Padded ? pitch : columns);
        const auto read = [&](size_t row, size_t col) noexcept {
            if constexpr (Mapped)
                return operation(input[row * pitch + (Splat ? 0 : col)], bias[col]);
            else {
                const auto x = input[row * pitch + (Splat ? 0 : col)];
                const auto square = x * x;
                return square + alpha;
            }
        };
        const auto bound = columns - columns % 4;
        size_t col = 0;
        for (; col != bound; col += 4) {
            Acc a{}, b{}, c{}, d{};
            for (size_t row = 0; row != rows; ++row) {
                a += static_cast<Acc>(read(row, col));
                b += static_cast<Acc>(read(row, col + 1));
                c += static_cast<Acc>(read(row, col + 2));
                d += static_cast<Acc>(read(row, col + 3));
            }
            out[col] = a;
            out[col + 1] = b;
            out[col + 2] = c;
            out[col + 3] = d;
        }
        for (; col != columns; ++col) {
            Acc value{};
            for (size_t row = 0; row != rows; ++row)
                value += static_cast<Acc>(read(row, col));
            out[col] = value;
        }
    } else {
        const auto source = [&] {
            if constexpr (Padded) {
                using view = np::tensor_view<const T, shape, np::layout_stride>;
                return view(input, typename view::mapping_type(shape(rows, Splat ? 1 : columns),
                                                               std::array<size_t, 2>{pitch, 1}));
            } else
                return np::tensor_view<const T, shape>(input, rows, Splat ? 1 : columns);
        }();
        const auto expression = [&] {
            if constexpr (Mapped)
                return np::map(operation, source, vector(bias, columns));
            else {
                const auto x = np::as_expr(source);
                return x * x + alpha;
            }
        }();
        const np::tensor_view<T, np::dextents<size_t, 1>> destination(out, columns);
        np::sum_to<Acc>(destination, expression, np::axes<0>);
    }
}

struct dimensions {
    size_t rows, columns;
};

template <bool Padded, class T>
struct data_set {
    std::vector<T> input, bias, actual, expected;
    data_set(size_t rows, size_t columns)
        : input(std::max(rows, rows * (columns + (Padded ? 5 : 0)))), bias(columns),
          actual(columns), expected(columns) {
        for (size_t i = 0; i != input.size(); ++i)
            input[i] = static_cast<T>(static_cast<int>(i % 37) - 18) / 8;
        for (size_t i = 0; i != columns; ++i)
            bias[i] = static_cast<T>(static_cast<int>(i % 11) - 5) / 4;
    }
};

template <backend Backend, bool Padded, bool Mapped, class T, class Acc, bool Splat = false>
void fixed(benchmark::State &state) {
    const auto rows = static_cast<size_t>(state.range(0));
    const auto columns = static_cast<size_t>(state.range(1));
    const auto pitch = columns + (Padded ? 5 : 0);
    data_set<Padded, T> data(rows, columns);
    constexpr T coefficient = static_cast<T>(1.25);
    kernel<backend::manual, Padded, Mapped, T, Acc, Splat>(data.expected.data(), data.input.data(),
                                                           data.bias.data(), rows, columns, pitch,
                                                           coefficient);
    kernel<Backend, Padded, Mapped, T, Acc, Splat>(
        data.actual.data(), data.input.data(), data.bias.data(), rows, columns, pitch, coefficient);
    if (data.actual != data.expected) {
        state.SkipWithError("ordered column result differs from hand-blocked reference");
        return;
    }
    auto *const output = np_benchmark::runtime_input(data.actual.data());
    const auto *const input = np_benchmark::runtime_input(data.input.data());
    const auto *const bias = np_benchmark::runtime_input(data.bias.data());
    const auto alpha = np_benchmark::runtime_input(coefficient);
    const auto row_count = np_benchmark::runtime_input(rows);
    const auto column_count = np_benchmark::runtime_input(columns);
    const auto row_pitch = np_benchmark::runtime_input(pitch);
    for (auto _ : state) {
        kernel<Backend, Padded, Mapped, T, Acc, Splat>(output, input, bias, row_count, column_count,
                                                       row_pitch, alpha);
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * rows * columns);
    state.SetLabel("ordered; fp-contract=off; includes construction and binding");
}

template <backend Backend, bool Padded, bool Mapped, class T, class Acc, bool Splat = false>
void shuffled(benchmark::State &state) {
    data_set<Padded, T> data(2048, 300);
    std::vector<dimensions> schedule;
    for (size_t rows : {size_t{0}, size_t{1}, size_t{8}, size_t{64}, size_t{2048}})
        for (size_t columns : {size_t{0}, size_t{1}, size_t{2}, size_t{3}, size_t{4}, size_t{7},
                               size_t{8}, size_t{15}, size_t{300}})
            schedule.push_back({rows, columns});
    std::mt19937 random(2518);
    std::shuffle(schedule.begin(), schedule.end(), random);
    for (auto shape : schedule) {
        const auto pitch = shape.columns + (Padded ? 5 : 0);
        kernel<backend::manual, Padded, Mapped, T, Acc, Splat>(
            data.expected.data(), data.input.data(), data.bias.data(), shape.rows, shape.columns,
            pitch, static_cast<T>(1.25));
        kernel<Backend, Padded, Mapped, T, Acc, Splat>(data.actual.data(), data.input.data(),
                                                       data.bias.data(), shape.rows, shape.columns,
                                                       pitch, static_cast<T>(1.25));
        if (!std::equal(data.actual.begin(), data.actual.begin() + shape.columns,
                        data.expected.begin())) {
            state.SkipWithError("mixed dimensions differ from ordered reference");
            return;
        }
    }
    auto *const output = np_benchmark::runtime_input(data.actual.data());
    const auto *const input = np_benchmark::runtime_input(data.input.data());
    const auto *const bias = np_benchmark::runtime_input(data.bias.data());
    const auto alpha = np_benchmark::runtime_input(static_cast<T>(1.25));
    size_t elements = 0;
    for (auto shape : schedule)
        elements += shape.rows * shape.columns;
    for (auto _ : state) {
        for (auto shape : schedule) {
            kernel<Backend, Padded, Mapped, T, Acc, Splat>(output, input, bias, shape.rows,
                                                           shape.columns,
                                                           shape.columns + (Padded ? 5 : 0), alpha);
            benchmark::ClobberMemory();
        }
    }
    state.SetItemsProcessed(state.iterations() * elements);
    state.SetLabel("ordered; shuffled shapes; fp-contract=off");
}

void sizes(benchmark::internal::Benchmark *benchmark) {
    for (const auto shape :
         {dimensions{0, 0},    dimensions{0, 1},    dimensions{0, 4},     dimensions{1, 1},
          dimensions{1, 3},    dimensions{1, 4},    dimensions{1, 7},     dimensions{8, 4},
          dimensions{8, 7},    dimensions{8, 300},  dimensions{64, 4},    dimensions{64, 7},
          dimensions{64, 300}, dimensions{2048, 1}, dimensions{2048, 3},  dimensions{2048, 4},
          dimensions{2048, 7}, dimensions{2048, 8}, dimensions{2048, 15}, dimensions{2048, 300}})
        benchmark->Args({static_cast<int64_t>(shape.rows), static_cast<int64_t>(shape.columns)});
}

template <backend Backend>
void register_cases(const char *backend_name) {
    const auto prefix = std::string("NPExpressionColumns/") + backend_name + "/";
    benchmark::RegisterBenchmark((prefix + "double-square").c_str(),
                                 fixed<Backend, false, false, double, double>)
        ->Apply(sizes);
    benchmark::RegisterBenchmark((prefix + "double-map-bias").c_str(),
                                 fixed<Backend, false, true, double, double>)
        ->Apply(sizes);
    benchmark::RegisterBenchmark((prefix + "double-padded-map-bias").c_str(),
                                 fixed<Backend, true, true, double, double>)
        ->Apply(sizes);
    benchmark::RegisterBenchmark((prefix + "float-square-wide").c_str(),
                                 fixed<Backend, false, false, float, double>)
        ->Apply(sizes);
    benchmark::RegisterBenchmark((prefix + "double-column-splat").c_str(),
                                 fixed<Backend, false, true, double, double, true>)
        ->Apply(sizes);
    benchmark::RegisterBenchmark((prefix + "shuffled-splat").c_str(),
                                 shuffled<Backend, false, true, double, double, true>);
    benchmark::RegisterBenchmark((prefix + "shuffled-double").c_str(),
                                 shuffled<Backend, false, false, double, double>);
    benchmark::RegisterBenchmark((prefix + "shuffled-padded-map").c_str(),
                                 shuffled<Backend, true, true, double, double>);
}

const auto registered = [] {
    register_cases<backend::expression>("expression");
    register_cases<backend::manual>("manual4");
    return true;
}();
} // namespace
