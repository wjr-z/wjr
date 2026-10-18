#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

#include <gtest/gtest.h>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_pairwise.hpp>
#include <wjr/np/expression_reduce.hpp>

namespace {
namespace np = wjr::np;
using shape = np::extents<size_t, 4>;
template <class T>
using view = np::tensor_view<T, shape>;
using leaf = decltype(np::as_expr(std::declval<view<const float>>()));
using squared = decltype(np::square(std::declval<leaf>()));
static_assert(sizeof(squared) == sizeof(leaf));
static_assert(np::detail::expression_leaf_count<squared> == 1);
static_assert(np::detail::expression_function_count<squared> == 0);
static_assert(np::detail::interleavable_sum_expression<squared>);
static_assert(std::same_as<typename decltype(np::square(true))::value_type, int>);
static_assert(std::same_as<typename decltype(np::square(short{}))::value_type, int>);
static_assert(std::same_as<typename decltype(np::square(1u))::value_type, unsigned>);
static_assert(std::same_as<typename decltype(np::square(1.f))::value_type, float>);
static_assert(np::square(3).read(0) == 9);

TEST(NpSquare, RetainsChildArithmeticBeforeAccumulationAndConversion) {
    const int numerator[]{3, 7, 8, 11}, denominator[]{2, 2, 3, 4};
    const auto expression = np::square(np::as_expr(view<const int>(numerator)) /
                                       np::as_expr(view<const int>(denominator)));
    double output[4];
    np::eval_to(view<double>(output), expression);
    for (size_t i = 0; i != 4; ++i) {
        const auto value = numerator[i] / denominator[i];
        EXPECT_EQ(output[i], value * value);
    }
    EXPECT_EQ(np::sum<double>(expression), 18.);
    EXPECT_EQ(np::pairwise_sum<double>(expression), 18.);
    EXPECT_EQ(np::square(std::numeric_limits<unsigned>::max()).read(0), 1u);
}

#ifndef WJR_DISABLE_EXCEPTIONS
TEST(NpSquare, EvaluatesMappedChildOnceAndPropagatesExceptions) {
    const float values[]{1, 2, 3, 4};
    unsigned calls = 0;
    auto expression = np::square(np::map(
        [&](float value) {
            ++calls;
            if (value == 3)
                throw std::runtime_error("square child");
            return value + 1;
        },
        view<const float>(values)));
    static_assert(!noexcept(expression.read(0)));
    EXPECT_EQ(expression.read(0), 4.f);
    EXPECT_EQ(calls, 1u);
    EXPECT_THROW(expression.read(2), std::runtime_error);
    EXPECT_EQ(calls, 2u);
}
#endif

TEST(NpSquare, EvaluatesNothrowMappedChildOncePerPoint) {
    const float values[]{1, 2, 3, 4};
    unsigned calls = 0;
    const auto nothrow = np::square(np::map(
        [&](float value) noexcept {
            ++calls;
            return value;
        },
        view<const float>(values)));
    EXPECT_EQ(np::sum(nothrow), 30.f);
    EXPECT_EQ(calls, 4u);
}

TEST(NpSquare, PositiveStridesBroadcastAndInPlace) {
    using matrix = np::extents<size_t, 2, 3>;
    const int input[]{1, 2, 3, -99, -99, 4, 5, 6};
    const np::layout_stride::mapping<matrix> mapping(matrix{}, std::array<size_t, 2>{5, 1});
    const auto source = np::tensor_view<const int, matrix, np::layout_stride>(input, mapping);
    const int shift[]{1, 2, 3};
    const auto offsets = np::tensor_view<const int, np::extents<size_t, 3>>(shift);
    int output[6];
    const auto destination = np::tensor_view<int, matrix>(output);
    np::eval_to(destination, np::square(np::as_expr(source) + np::as_expr(offsets)));
    EXPECT_EQ(output[0], 4);
    EXPECT_EQ(output[5], 81);
    np::eval_to(destination, np::square(destination));
    EXPECT_EQ(output[0], 16);
    EXPECT_EQ(output[5], 6561);
}

TEST(NpSquare, FloatingCornersAndEmptyDomain) {
    const double values[]{-0., std::numeric_limits<double>::infinity(),
                          std::numeric_limits<double>::quiet_NaN(), -2.};
    double output[4];
    np::eval_to(view<double>(output), np::square(view<const double>(values)));
    EXPECT_EQ(output[0], 0.);
    EXPECT_FALSE(std::signbit(output[0]));
    EXPECT_TRUE(std::isinf(output[1]));
    EXPECT_TRUE(std::isnan(output[2]));
    EXPECT_EQ(output[3], 4.);
    using empty = np::tensor_view<const float, np::extents<size_t, 0>>;
    EXPECT_EQ(np::pairwise_sum(np::square(empty(nullptr))), 0.f);
}
} // namespace
