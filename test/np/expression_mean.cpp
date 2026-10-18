#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/expression_mean.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;
template <class T>
using expression = decltype(np::as_expr(std::declval<view<T, np::dextents<size_t, 1>>>()));
template <class E, class Acc = void>
concept has_mean = requires(const E &e) { np::mean<Acc>(e); };

static_assert(has_mean<expression<const int>> && has_mean<expression<float>, long double>);
static_assert(!has_mean<expression<int>, bool> && !has_mean<expression<int>, int>);
static_assert(!has_mean<expression<int>, const double> && !has_mean<int>);
static_assert(std::same_as<decltype(np::mean(std::declval<expression<int>>())), double>);
static_assert(std::same_as<decltype(np::mean(std::declval<expression<bool>>())), double>);
static_assert(std::same_as<decltype(np::mean(std::declval<expression<float>>())), float>);
static_assert(
    std::same_as<decltype(np::mean(std::declval<expression<long double>>())), long double>);
static_assert(noexcept(np::mean(std::declval<expression<float>>())));

constexpr bool constant_means() {
    const int data[] = {1, 2, 3, 4, 5, 6};
    const auto a = np::as_expr(view<const int, np::extents<size_t, 2, 3>>(data));
    if (np::mean(a * a) != 91.0 / 6 || np::mean(np::as_expr(true)) != 1)
        return false;
    const auto empty = np::as_expr(view<const int, np::extents<size_t, 0>>(nullptr));
    const auto result = np::mean(empty);
    return result != result && np::mean(np::as_expr(-7)) == -7;
}
static_assert(constant_means());

TEST(NpExpressionMean, ConstantEvaluationAndScalarDomains) {
    EXPECT_TRUE(constant_means());
    const int value = -3;
    const auto scalar = np::as_expr(view<const int, np::extents<size_t>>(&value));
    EXPECT_EQ(np::mean(scalar + 5), 2);
    EXPECT_EQ(np::mean(np::as_expr(false)), 0);
}

TEST(NpExpressionMean, PreservesChildDtypesBeforeFloatingAccumulation) {
    const int numerators[] = {3, 7, 8}, denominators[] = {2, 2, 3};
    const float bias[] = {.5f, .25f, 1.f};
    using shape = np::extents<size_t, 3>;
    const auto divided = np::as_expr(view<const int, shape>(numerators)) /
                         np::as_expr(view<const int, shape>(denominators));
    EXPECT_EQ(np::mean(divided), 2);
    const auto mixed = divided + np::as_expr(view<const float, shape>(bias));
    static_assert(std::same_as<decltype(np::mean(mixed)), float>);
    EXPECT_EQ(np::mean(mixed), 7.75f / 3);
    EXPECT_EQ(np::mean<double>(mixed), 7.75 / 3);
    const std::uint8_t bytes[] = {250, 10};
    const auto promoted =
        (np::as_expr(view<const std::uint8_t, np::extents<size_t, 2>>(bytes)) + 10) / 2;
    EXPECT_EQ(np::mean(promoted), 70);
    const bool flags[] = {true, false, true, true};
    EXPECT_EQ(np::mean(np::as_expr(view<const bool, np::extents<size_t, 4>>(flags))), .75);
}

TEST(NpExpressionMean, ExplicitAccumulatorAndLogicalRowMajorOrder) {
    const float values[] = {1e8f, 1, -1e8f};
    const auto a = np::as_expr(view<const float, np::extents<size_t, 3>>(values));
    EXPECT_EQ(np::mean(a + 0.f), 0);
    EXPECT_EQ(np::mean<double>(a + 0.f), 1.0 / 3);
    const double column_major[] = {1e16, -1e16, 1, 1};
    const auto column =
        np::as_expr(view<const double, np::extents<size_t, 2, 2>, np::layout_left>(column_major));
    EXPECT_EQ(np::mean(column), .25);
}

TEST(NpExpressionMean, NaturalDynamicRightAlignedBroadcastDomain) {
    const int data[] = {1, 2, 3, 4, 5, 6};
    const int bias[] = {10, 20, 30, 40};
    const auto a = np::as_expr(view<const int, np::dextents<size_t, 3>>(data, 2, 1, 3));
    const auto b = np::as_expr(view<const int, np::dextents<unsigned, 2>>(bias, 4, 1));
    EXPECT_EQ(np::mean(a + b), 28.5);
    const auto constant = np::as_expr(view<const int, np::extents<size_t, 2, 1, 3>>(data));
    EXPECT_EQ(np::mean(constant + b), 28.5);
}

TEST(NpExpressionMean, PositiveStridesSkipPaddingAndBroadcastFactors) {
    const int data[] = {1, 999, 2, 999, 3, 999, 999, 4, 999, 5, 999, 6};
    using input_view = view<const int, np::extents<size_t, 2, 3>, np::layout_stride>;
    const input_view pitched(data, input_view::mapping_type({}, std::array<size_t, 2>{7, 2}));
    const int factors[] = {10, 20, 30};
    const auto product =
        np::as_expr(pitched) * np::as_expr(view<const int, np::extents<size_t, 3>>(factors));
    EXPECT_EQ(np::mean(product), 460.0 / 6);
}

struct observed_operation {
    static inline int calls = 0;
    static int apply(int value) noexcept {
        ++calls;
        return value;
    }
};

TEST(NpExpressionMean, EmptyDomainSkipsReadsAndRepeatedSubtreesRemainExplicit) {
    const auto empty =
        np::as_expr(view<const int, np::dextents<size_t, 2>, np::layout_left>(nullptr, 0, 1));
    np::unary_expression<observed_operation, decltype(empty)> observed(empty);
    const int bias[] = {1, 2, 3};
    observed_operation::calls = 0;
    EXPECT_TRUE(std::isnan(
        np::mean(observed + np::as_expr(view<const int, np::extents<size_t, 1, 3>>(bias)))));
    EXPECT_EQ(observed_operation::calls, 0);
    const auto leaf = np::as_expr(view<const int, np::extents<size_t, 3>>(bias));
    np::unary_expression<observed_operation, decltype(leaf)> counted(leaf);
    EXPECT_EQ(np::mean(counted + counted), 4);
    EXPECT_EQ(observed_operation::calls, 6);
}

struct tracked_square {
    static inline int copies = 0, moves = 0;
    int center;
    explicit tracked_square(int value) noexcept : center(value) {}
    tracked_square(const tracked_square &other) noexcept : center(other.center) { ++copies; }
    tracked_square(tracked_square &&other) noexcept : center(other.center) { ++moves; }
    double operator()(double value) const noexcept { return (value - center) * (value - center); }
};

struct move_only_square {
    std::unique_ptr<int> factor;
    explicit move_only_square(int value) : factor(std::make_unique<int>(value)) {}
    double operator()(double value) const noexcept { return value * value * *factor; }
};

TEST(NpExpressionMean, EvaluationDoesNotCopyOrMoveCallables) {
    const double values[] = {1, 2, 3};
    const auto a = np::as_expr(view<const double, np::extents<size_t, 3>>(values));
    const auto mapped = np::map(tracked_square(1), a);
    const auto copies = tracked_square::copies, moves = tracked_square::moves;
    EXPECT_EQ(np::mean(mapped), 5.0 / 3);
    EXPECT_EQ(np::mean(mapped), 5.0 / 3);
    EXPECT_EQ(tracked_square::copies, copies);
    EXPECT_EQ(tracked_square::moves, moves);
    const auto move_only = np::map(move_only_square(2), a);
    static_assert(!std::copy_constructible<decltype(move_only)>);
    EXPECT_EQ(np::mean(move_only), 28.0 / 3);
}

TEST(NpExpressionMean, NonfiniteAndSignedZeroFollowOrderedArithmetic) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    const double values[] = {nan, inf, -inf, -0.0};
    for (size_t i = 0; i != 4; ++i) {
        const auto a = np::as_expr(view<const double, np::extents<size_t>>(&values[i]));
        const auto result = np::mean(a);
        if (i == 0)
            EXPECT_TRUE(std::isnan(result));
        else if (i == 3)
            EXPECT_FALSE(std::signbit(result));
        else
            EXPECT_EQ(result, values[i]);
    }
    const auto a = np::as_expr(view<const double, np::extents<size_t, 2>>(values + 1));
    EXPECT_TRUE(std::isnan(np::mean(a)));
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
struct throwing_square {
    int operator()(int value) const {
        if (value == 3)
            throw std::runtime_error("map failed");
        return value * value;
    }
};

TEST(NpExpressionMean, ThrowingMapPropagatesAndExpressionRemainsReusable) {
    int values[] = {1, 2, 3};
    const auto expression =
        np::map(throwing_square{}, np::as_expr(view<const int, np::extents<size_t, 3>>(values)));
    static_assert(!noexcept(np::mean(expression)));
    EXPECT_THROW((void)np::mean(expression), std::runtime_error);
    values[2] = 4;
    EXPECT_EQ(np::mean(expression), 7);
}
#endif
} // namespace
