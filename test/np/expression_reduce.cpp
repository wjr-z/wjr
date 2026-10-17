#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/expression_reduce.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;

using matrix_shape = np::extents<size_t, 2, 3>;
using expression = decltype(np::as_expr(std::declval<view<int, matrix_shape>>()));
template <class Accumulator, class E>
concept sums = requires(const E &e) { np::sum<Accumulator>(e); };
template <class Out, class E, class Axes>
concept reduces_to = requires(Out out, const E &e, Axes axes) { np::sum_to(out, e, axes); };
static_assert(sums<void, expression> && sums<double, expression>);
static_assert(!sums<bool, expression> && !sums<short, expression>);
static_assert(!sums<void, int>);
static_assert(!reduces_to<view<const int, np::extents<size_t, 2>>, expression, np::axes_t<1>>);
static_assert(!reduces_to<view<int, np::extents<size_t, 3>>, expression, np::axes_t<1>>);
static_assert(!reduces_to<view<int, np::extents<size_t, 2>>, expression, np::axes_t<>>);
static_assert(!reduces_to<view<int, np::extents<size_t, 2>>, expression, np::axes_t<0, 1>>);
static_assert(!reduces_to<view<int, np::extents<size_t, 2>>, expression, np::axes_t<2>>);

constexpr bool constant_reduction() {
    int input[] = {1, 2, 3, 4, 5, 6};
    int rows[2]{};
    int columns[3]{};
    const auto a = np::as_expr(view<const int, matrix_shape>(input));
    np::sum_to(view<int, np::extents<size_t, 2>>(rows), a * a + 1, np::axes<1>);
    np::sum_to(view<int, np::extents<size_t, 1, 3>>(columns), a + 2, np::axes<0>, np::keepdims);
    return np::sum(a * a + 1) == 97 && rows[0] == 17 && rows[1] == 80 && columns[0] == 9 &&
           columns[1] == 11 && columns[2] == 13 && np::sum(np::as_expr(7)) == 7;
}
static_assert(constant_reduction());

TEST(NpExpressionReduce, ConstantEvaluationAndNaturalBroadcastShape) {
    EXPECT_TRUE(constant_reduction());
    int a[] = {10, 20};
    int b[] = {1, 2, 3};
    const auto left = np::as_expr(view<const int, np::dextents<int, 2>>(a, 2, 1));
    const auto right = np::as_expr(view<const int, np::dextents<unsigned, 1>>(b, 3));
    auto expression = left + right;
    EXPECT_EQ(np::sum(expression), 102);
    int rows[2]{}, columns[3]{};
    np::sum_to(view<int, np::dextents<size_t, 1>>(rows, 2), expression, np::axes<1>);
    np::sum_to(view<int, np::dextents<size_t, 1>>(columns, 3), expression, np::axes<0>);
    EXPECT_EQ((std::array{rows[0], rows[1]}), (std::array{36, 66}));
    EXPECT_EQ((std::array{columns[0], columns[1], columns[2]}), (std::array{32, 34, 36}));
}

TEST(NpExpressionReduce, PreservesNodeTypesAndAccumulatorIndependence) {
    int numerator[] = {3, 7, 8}, denominator[] = {2, 2, 3};
    float bias[] = {.5f, .25f, 1.f};
    using shape = np::extents<size_t, 3>;
    const auto mixed = np::as_expr(view<const int, shape>(numerator)) /
                           np::as_expr(view<const int, shape>(denominator)) +
                       np::as_expr(view<const float, shape>(bias));
    static_assert(std::same_as<decltype(np::sum(mixed)), float>);
    EXPECT_EQ(np::sum(mixed), 7.75f);
    uint8_t small[] = {250, 10};
    auto promoted = (np::as_expr(view<const uint8_t, np::extents<size_t, 2>>(small)) + 10) / 2;
    static_assert(std::same_as<decltype(np::sum(promoted)), int>);
    EXPECT_EQ(np::sum(promoted), 140);
    float cancellation[] = {1e8f, 1.f, -1e8f};
    auto values = np::as_expr(view<const float, shape>(cancellation)) + 0.f;
    double result = -1.;
    np::sum_to(view<double, np::extents<size_t>>(&result), values, np::axes<0>);
    EXPECT_EQ(result, 0.);
    np::sum_to<double>(view<double, np::extents<size_t>>(&result), values, np::axes<0>);
    EXPECT_EQ(result, 1.);
    EXPECT_EQ(np::sum<double>(values), 1.);
    double fractional[] = {.75, .75};
    int converted = -1;
    np::sum_to(view<int, np::extents<size_t>>(&converted),
               np::as_expr(view<const double, np::extents<size_t, 2>>(fractional)) + 0.,
               np::axes<0>);
    EXPECT_EQ(converted, 1);
}

TEST(NpExpressionReduce, FullFoldRemainsLogicalRowMajorAcrossRows) {
    double physical[] = {1e16, -1e16, 1., 1.};
    auto a =
        np::as_expr(view<const double, np::dextents<size_t, 2>, np::layout_left>(physical, 2, 2));
    EXPECT_EQ(np::sum(a), 1.);
    double rows[2]{}, columns[2]{};
    np::sum_to(view<double, np::extents<size_t, 2>>(rows), a, np::axes<1>);
    np::sum_to(view<double, np::extents<size_t, 2>>(columns), a, np::axes<0>);
    EXPECT_EQ(rows[0], 1e16);
    EXPECT_EQ(rows[1], -1e16);
    EXPECT_EQ(columns[0], 0.);
    EXPECT_EQ(columns[1], 2.);
}

TEST(NpExpressionReduce, StridedInputsAndOutputsPreservePadding) {
    using input_view = view<const int, matrix_shape, np::layout_stride>;
    int input[12] = {1, 999, 2, 999, 3, 999, 999, 4, 999, 5, 999, 6};
    input_view pitched(input,
                       input_view::mapping_type(matrix_shape{}, std::array<size_t, 2>{7, 2}));
    int factors[] = {10, 20, 30};
    auto product =
        np::as_expr(pitched) * np::as_expr(view<const int, np::extents<size_t, 3>>(factors));
    EXPECT_EQ(np::sum(product), 460);
    using output_view = view<int, np::extents<size_t, 2>, np::layout_stride>;
    int output[] = {-1, -1, -1};
    output_view separated(output, output_view::mapping_type({}, std::array<size_t, 1>{2}));
    np::sum_to(separated, product, np::axes<1>);
    EXPECT_EQ(output[0], 140);
    EXPECT_EQ(output[1], -1);
    EXPECT_EQ(output[2], 320);
    EXPECT_EQ(input[1], 999);
}

TEST(NpExpressionReduce, RankThreeMiddleAxisAndMixedLayouts) {
    int input[] = {1, 2, 3, 4, 5, 6}, factors[] = {1, 2, 3, 4};
    auto a = np::as_expr(view<const int, np::extents<size_t, 2, 1, 3>>(input));
    auto b = np::as_expr(view<const int, np::extents<size_t, 1, 4, 1>, np::layout_left>(factors));
    auto expression = a * b;
    EXPECT_EQ(np::sum(expression), 210);
    int output[6]{}, kept[6]{};
    np::sum_to(view<int, np::extents<size_t, 2, 3>, np::layout_left>(output), expression,
               np::axes<1>);
    np::sum_to(view<int, np::extents<size_t, 2, 1, 3>>(kept), expression, np::axes<1>,
               np::keepdims);
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3], output[4], output[5]}),
              (std::array{10, 40, 20, 50, 30, 60}));
    EXPECT_EQ((std::array{kept[0], kept[1], kept[2], kept[3], kept[4], kept[5]}),
              (std::array{10, 20, 30, 40, 50, 60}));
}

struct counted_operation {
    static inline int calls = 0;
    static int apply(int value) noexcept {
        ++calls;
        return value;
    }
};

TEST(NpExpressionReduce, EmptyDomainsSkipOperationsAndRankZeroRunsOnce) {
    using empty_view = view<const int, np::extents<size_t, 2, 0, 3>, np::layout_left>;
    auto leaf = np::as_expr(empty_view(nullptr));
    np::unary_expression<counted_operation, decltype(leaf)> expression(leaf);
    counted_operation::calls = 0;
    EXPECT_EQ(np::sum(expression), 0);
    int output[6] = {-1, -1, -1, -1, -1, -1};
    np::sum_to(view<int, np::extents<size_t, 2, 3>>(output), expression, np::axes<1>);
    EXPECT_EQ(np::sum(view<int, np::extents<size_t, 2, 3>>(output)), 0);
    np::sum_to(view<int, np::extents<size_t, 0, 3>>(nullptr), expression, np::axes<0>);
    EXPECT_EQ(counted_operation::calls, 0);
    auto scalar = np::as_expr(9);
    const np::unary_expression<counted_operation, decltype(scalar)> rank_zero(scalar);
    EXPECT_EQ(np::sum(rank_zero), 9);
    EXPECT_EQ(counted_operation::calls, 1);
    int input[] = {1, 2, 3};
    auto three = np::as_expr(view<const int, np::extents<size_t, 3>>(input));
    const np::unary_expression<counted_operation, decltype(three)> counted(three);
    counted_operation::calls = 0;
    EXPECT_EQ(np::sum(counted + counted), 12);
    EXPECT_EQ(counted_operation::calls, 6);
}

TEST(NpExpressionReduce, EmptyBroadcastDomainAndRealBooleanValues) {
    using empty_shape = np::dextents<size_t, 2>;
    const auto empty = np::as_expr(view<const double, empty_shape>(nullptr, 0, 1));
    double bias[] = {1., 2., 3.}, result[] = {-1., -1., -1.};
    auto expression = empty + np::as_expr(view<const double, np::extents<size_t, 1, 3>>(bias));
    EXPECT_EQ(np::sum(expression), 0.);
    np::sum_to(view<double, np::extents<size_t, 3>>(result), expression, np::axes<0>);
    EXPECT_EQ((std::array{result[0], result[1], result[2]}), (std::array{0., 0., 0.}));
    bool flags[] = {true, false, true};
    auto booleans = np::as_expr(view<const bool, np::extents<size_t, 3>>(flags));
    static_assert(std::same_as<decltype(np::sum(booleans)), int>);
    EXPECT_EQ(np::sum(booleans), 2);
    EXPECT_EQ(np::sum(np::as_expr(true)), 1);
    double special[] = {std::numeric_limits<double>::quiet_NaN(), 1.};
    EXPECT_TRUE(
        std::isnan(np::sum(np::as_expr(view<const double, np::extents<size_t, 2>>(special)))));
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
struct throwing_operation {
    static inline int budget = -1;
    static int apply(int value) {
        if (budget == 0)
            throw std::runtime_error("operation failed");
        --budget;
        return value;
    }
};
using throwing_expression = np::unary_expression<throwing_operation, expression>;
static_assert(!noexcept(np::sum(std::declval<const throwing_expression &>())));
static_assert(noexcept(np::sum(std::declval<const expression &>())));

WJR_NOINLINE void reduce_throwing_rows(int *output, const throwing_expression &expression) {
    np::sum_to(view<int, np::extents<size_t, 2>>(output), expression, np::axes<1>);
}

TEST(NpExpressionReduce, PropagatesOperationsAndCommitsOnlyCompleteAxisResults) {
    int input[] = {1, 2, 3, 4, 5, 6}, output[] = {-1, -1};
    auto leaf = np::as_expr(view<int, matrix_shape>(input));
    throwing_expression expression(leaf);
    throwing_operation::budget = 4;
    EXPECT_THROW(reduce_throwing_rows(output, expression), std::runtime_error);
    EXPECT_EQ(output[0], 6);
    EXPECT_EQ(output[1], -1);
    throwing_operation::budget = 0;
    EXPECT_THROW((void)np::sum(expression), std::runtime_error);
    throwing_operation::budget = -1;
}
#endif
} // namespace
