#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/expression_extrema.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;
using matrix = np::extents<size_t, 2, 3>;
using matrix_expression = decltype(np::as_expr(std::declval<view<const int, matrix>>()) + 1);

template <class E>
concept has_expression_extrema = requires(const E &expression) {
    np::min(expression);
    np::max(expression);
};
template <class Dst, class E>
concept has_axis_extrema = requires(Dst dst, const E &expression) {
    np::min_to(dst, expression, np::axes<1>);
    np::max_to(dst, expression, np::axes<1>);
};
static_assert(has_expression_extrema<matrix_expression>);
static_assert(has_expression_extrema<decltype(np::as_expr(3))>);
static_assert(has_axis_extrema<view<int, np::extents<size_t, 2>>, matrix_expression>);
static_assert(!has_axis_extrema<view<int, np::extents<size_t, 3>>, matrix_expression>);
static_assert(std::same_as<decltype(np::min(std::declval<matrix_expression>())), int>);
static_assert(noexcept(np::min(std::declval<matrix_expression>())));

constexpr bool constant_cases() {
    const int values[]{3, -2, 7, -2, 7, 4};
    const auto expression = np::as_expr(view<const int, matrix>(values)) * 2 + 1;
    if (np::min(expression) != -3 || np::max(expression) != 15)
        return false;
    int rows[2]{};
    np::min_to(view<int, np::extents<size_t, 2>>(rows), expression, np::axes<1>);
    return rows[0] == -3 && rows[1] == -3;
}
static_assert(constant_cases());

TEST(NpExpressionExtrema, FullFoldKeepsDtypeAndLogicalRowMajorOrder) {
    const std::uint64_t values[]{UINT64_MAX, 0, UINT64_MAX - 1, 5};
    const auto expression =
        np::as_expr(view<const std::uint64_t, np::extents<size_t, 2, 2>>(values)) +
        std::uint64_t{0};
    static_assert(std::same_as<decltype(np::max(expression)), std::uint64_t>);
    EXPECT_EQ(np::min(expression), 0);
    EXPECT_EQ(np::max(expression), UINT64_MAX);

    const int left_values[]{5, 30, 1, 8};
    const int right_values[]{2, -40, 7, 3};
    const auto sum = np::as_expr(view<const int, np::extents<size_t, 2, 2>>(left_values)) +
                     np::as_expr(view<const int, np::extents<size_t, 2, 2>>(right_values));
    EXPECT_EQ(np::min(sum), -10);
    EXPECT_EQ(np::max(sum), 11);

    const double physical_left_values[]{1, 100, -2, 3, -200, 4};
    const auto left =
        np::as_expr(view<const double, matrix, np::layout_left>(physical_left_values)) * 1.0;
    EXPECT_EQ(np::min(left), -200);
    EXPECT_EQ(np::max(left), 100);
    EXPECT_EQ(np::min(np::as_expr(17)), 17);
}

TEST(NpExpressionExtrema, BroadcastAndPaddedInputs) {
    const int pitched_data[]{1, 99, 2, 99, 3, 99, 99, 4, 99, 5, 99, 6};
    using pitched_view = view<const int, matrix, np::layout_stride>;
    const pitched_view pitched(pitched_data,
                               pitched_view::mapping_type({}, std::array<size_t, 2>{7, 2}));
    const int bias[]{10, -20, 30};
    const auto expression =
        np::as_expr(pitched) + np::as_expr(view<const int, np::extents<size_t, 3>>(bias));
    EXPECT_EQ(np::min(expression), -18);
    EXPECT_EQ(np::max(expression), 36);
    int row_min[2]{};
    int col_max[3]{};
    np::min_to(view<int, np::extents<size_t, 2>>(row_min), expression, np::axes<1>);
    np::max_to(view<int, np::extents<size_t, 3>>(col_max), expression, np::axes<0>);
    EXPECT_EQ((std::array{row_min[0], row_min[1]}), (std::array{-18, -15}));
    EXPECT_EQ((std::array{col_max[0], col_max[1], col_max[2]}), (std::array{14, -15, 36}));
}

TEST(NpExpressionExtrema, FirstNaNAndEqualSignedZeroAreStable) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double values[]{4, nan, -2, 7, nan, -3};
    const auto expression = np::as_expr(view<const double, matrix>(values)) + 0.0;
    EXPECT_TRUE(std::isnan(np::min(expression)));
    EXPECT_TRUE(std::isnan(np::max(expression)));
    double row_min[2]{};
    np::min_to(view<double, np::extents<size_t, 2>>(row_min), expression, np::axes<1>);
    EXPECT_TRUE(std::isnan(row_min[0]));
    EXPECT_TRUE(std::isnan(row_min[1]));

    const double zeros[]{-0.0, 0.0, 0.0, -0.0};
    const auto zero_expression = np::as_expr(view<const double, np::extents<size_t, 2, 2>>(zeros));
    EXPECT_TRUE(std::signbit(np::min(zero_expression)));
    EXPECT_TRUE(std::signbit(np::max(zero_expression)));
    double row_max[2]{};
    np::max_to(view<double, np::extents<size_t, 2>>(row_max), zero_expression, np::axes<1>);
    EXPECT_FALSE(std::signbit(row_max[1]));
}

TEST(NpExpressionExtrema, AxisKeepdimsViewsAndOutputPadding) {
    const int values[]{1, 9, 3, 4, 5, 6};
    const auto source = view<const int, matrix>(values);
    int row_max[2]{};
    int kept[2]{};
    np::max_to(view<int, np::extents<size_t, 2>>(row_max), source, np::axes<1>);
    np::min_to(view<int, np::extents<size_t, 2, 1>>(kept), np::as_expr(source) + 0, np::axes<1>,
               np::keepdims);
    EXPECT_EQ((std::array{row_max[0], row_max[1]}), (std::array{9, 6}));
    EXPECT_EQ((std::array{kept[0], kept[1]}), (std::array{1, 4}));

    int output[]{-1, -1, -1, -1, -1};
    using output_view = view<int, np::extents<size_t, 3>, np::layout_stride>;
    output_view separated(output, output_view::mapping_type({}, std::array<size_t, 1>{2}));
    np::min_to(separated, source, np::axes<0>);
    EXPECT_EQ(output[0], 1);
    EXPECT_EQ(output[1], -1);
    EXPECT_EQ(output[2], 5);
    EXPECT_EQ(output[3], -1);
    EXPECT_EQ(output[4], 3);

    const int line[]{8, -3, 5};
    int scalar_min = 0;
    int kept_line[1]{};
    np::min_to(view<int, np::extents<size_t>>(&scalar_min),
               view<const int, np::extents<size_t, 3>>(line), np::axes<0>);
    np::max_to(view<int, np::extents<size_t, 1>>(kept_line),
               view<const int, np::extents<size_t, 3>>(line), np::axes<0>, np::keepdims);
    EXPECT_EQ(scalar_min, -3);
    EXPECT_EQ(kept_line[0], 8);

    const int physical_left[]{1, 10, 2, 20, 3, 30};
    int left_rows[2]{};
    np::max_to(view<int, np::extents<size_t, 2>>(left_rows),
               view<const int, matrix, np::layout_left>(physical_left), np::axes<1>);
    EXPECT_EQ((std::array{left_rows[0], left_rows[1]}), (std::array{3, 30}));

    using dynamic_shape = np::dextents<size_t, 2>;
    const auto dynamic = np::as_expr(view<const int, dynamic_shape>(values, 2, 3)) + 0;
    int dynamic_rows[2]{};
    np::min_to(view<int, np::dextents<size_t, 1>>(dynamic_rows, 2), dynamic, np::axes<1>);
    EXPECT_EQ((std::array{dynamic_rows[0], dynamic_rows[1]}), (std::array{1, 4}));
}

struct counting_map {
    int *calls;
    int operator()(int value) const noexcept {
        ++*calls;
        return value;
    }
};

TEST(NpExpressionExtrema, EmptyDestinationDoesNotReadExpression) {
    using empty_matrix = np::extents<size_t, 0, 3>;
    const auto source = np::as_expr(view<const int, empty_matrix>(nullptr));
    int calls = 0;
    const auto expression = np::map(counting_map{&calls}, source);
    np::min_to(view<int, np::extents<size_t, 0>>(nullptr), expression, np::axes<1>);
    np::max_to(view<int, np::extents<size_t, 0, 1>>(nullptr), expression, np::axes<1>,
               np::keepdims);
    EXPECT_EQ(calls, 0);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
struct throwing_map {
    int threshold;
    int operator()(int value) const {
        if (value == threshold)
            throw std::runtime_error("extrema map failed");
        return value;
    }
};

struct throw_on_second_call {
    int *calls;
    double operator()(double value) const {
        if (++*calls == 2)
            throw std::runtime_error("second extrema read");
        return value;
    }
};

TEST(NpExpressionExtrema, FirstNaNStopsBeforeLaterThrowingMapCall) {
    const double values[]{std::numeric_limits<double>::quiet_NaN(), 1.0};
    int calls = 0;
    const auto expression =
        np::map(throw_on_second_call{&calls},
                np::as_expr(view<const double, np::extents<size_t, 2>>(values)));
    EXPECT_TRUE(std::isnan(np::min(expression)));
    EXPECT_EQ(calls, 1);
}

TEST(NpExpressionExtrema, ThrowingAxisLineIsNotCommitted) {
    int input[]{1, 2, 3, 4, 5, 6};
    const auto expression = np::map(throwing_map{5}, np::as_expr(view<const int, matrix>(input)));
    double output[]{-1, -1};
    EXPECT_THROW(np::max_to(view<double, np::extents<size_t, 2>>(output), expression, np::axes<1>),
                 std::runtime_error);
    EXPECT_EQ(output[0], 3);
    EXPECT_EQ(output[1], -1);
}
#endif
} // namespace
