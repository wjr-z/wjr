#include <array>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/expression_reduce.hpp>

namespace {
namespace np = wjr::np;

template <size_t N, bool Unit, class Acc, class D, class E>
constexpr void run_width(D *output, const E &expression, size_t rows, size_t columns,
                         size_t output_step = 1) {
    constexpr auto count = np::detail::expression_leaf_count<E>;
    const auto values =
        np::detail::bind_expression_values(expression, std::make_index_sequence<count>{});
    std::array<std::array<size_t, 2>, count> strides{};
    if (rows != 0 && columns != 0)
        np::detail::bind_expression_strides(expression, strides);
    std::array<size_t, count> row_steps{}, column_steps{};
    for (size_t i = 0; i != count; ++i) {
        row_steps[i] = strides[i][0];
        column_steps[i] = strides[i][1];
    }
    np::detail::sum_expression_columns_to<N, Unit, E, Acc>(output, values, rows, columns,
                                                           output_step, row_steps, column_steps,
                                                           std::make_index_sequence<count>{});
}

using scalar = decltype(np::as_expr(1));
template <size_t N>
concept positive_width =
    requires(int *output, std::tuple<int> values, std::array<size_t, 1> steps) {
        np::detail::sum_expression_columns_to<N, false, scalar, int>(
            output, values, 0, 0, 1, steps, steps, std::index_sequence<0>{});
    };
static_assert(!positive_width<0>);
static_assert(positive_width<1> && positive_width<4> && positive_width<8>);
static_assert(positive_width<3>);

template <size_t N>
constexpr bool constant_width() {
    using shape = np::extents<size_t, 3, 15>;
    int input[45];
    std::array<int, 30> output;
    output.fill(-1);
    for (size_t i = 0; i != 45; ++i)
        input[i] = int(i);
    const auto expression = np::as_expr(np::tensor_view<const int, shape>(input)) * 2 + 1;
    run_width<N, true, int>(output.data(), expression, 3, 15, 2);
    for (size_t col = 0; col != 15; ++col) {
        int expected = 0;
        for (size_t row = 0; row != 3; ++row)
            expected += input[row * 15 + col] * 2 + 1;
        if (output[col * 2] != expected || output[col * 2 + 1] != -1)
            return false;
    }
    return true;
}
static_assert(constant_width<1>());
static_assert(constant_width<4>());
static_assert(constant_width<8>());

TEST(NpExpressionColumnWidth, ConstantEvaluationAndNonunitOutput) {
    EXPECT_TRUE(constant_width<1>());
    EXPECT_TRUE(constant_width<4>());
    EXPECT_TRUE(constant_width<8>());
}

template <size_t N, bool Unit>
void compare_sizes() {
    using shape = np::dextents<size_t, 2>;
    for (size_t rows = 0; rows != 6; ++rows) {
        for (size_t columns = 0; columns != 26; ++columns) {
            const auto input_columns = Unit ? columns : size_t{1};
            const auto pitch = input_columns + 3;
            std::vector<int> input(rows * pitch), bias(columns), output(columns + 2, -9);
            for (size_t i = 0; i != input.size(); ++i)
                input[i] = int(i % 13);
            for (size_t i = 0; i != columns; ++i)
                bias[i] = int(i % 5);
            const np::layout_stride::mapping<shape> mapping(shape(rows, input_columns),
                                                            std::array<size_t, 2>{pitch, 1});
            const auto source =
                np::tensor_view<const int, shape, np::layout_stride>(input.data(), mapping);
            const auto expr =
                np::map([](int x, int b) noexcept { return x * 2 + b; }, source,
                        np::tensor_view<const int, np::dextents<size_t, 1>>(bias.data(), columns));
            run_width<N, Unit, int>(output.data() + 1, expr, rows, columns);
            EXPECT_EQ(output.front(), -9);
            EXPECT_EQ(output.back(), -9);
            for (size_t col = 0; col != columns; ++col) {
                int expected = 0;
                for (size_t row = 0; row != rows; ++row)
                    expected += source(row, Unit ? col : 0) * 2 + bias[col];
                EXPECT_EQ(output[col + 1], expected);
            }
        }
    }
}

TEST(NpExpressionColumnWidth, BlocksAndEveryTailIncludingSevenColumns) {
    compare_sizes<1, true>();
    compare_sizes<4, true>();
    compare_sizes<8, true>();
    compare_sizes<1, false>();
    compare_sizes<4, false>();
    compare_sizes<8, false>();
}

template <size_t N>
void check_accumulator() {
    using shape = np::extents<size_t, 3, 15>;
    std::array<double, 45> input;
    std::array<double, 15> output;
    for (size_t col = 0; col != 15; ++col) {
        input[col] = 1e20;
        input[15 + col] = 1;
        input[30 + col] = -1e20;
    }
    const auto expr = np::as_expr(np::tensor_view<const double, shape>(input.data()));
    run_width<N, true, double>(output.data(), expr, 3, 15);
    for (auto value : output)
        EXPECT_EQ(value, 0.);
    std::array<int, 45> integers;
    integers.fill(1000000000);
    std::array<long long, 15> wide;
    run_width<N, true, long long>(
        wide.data(), np::as_expr(np::tensor_view<const int, shape>(integers.data())), 3, 15);
    for (auto value : wide)
        EXPECT_EQ(value, 3000000000ll);
}

TEST(NpExpressionColumnWidth, FoldOrderAndExplicitAccumulatorStayIndependent) {
    check_accumulator<1>();
    check_accumulator<4>();
    check_accumulator<8>();
}
} // namespace
