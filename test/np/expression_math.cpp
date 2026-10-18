#include <array>
#include <cerrno>
#include <cfenv>
#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression_cast.hpp>
#include <wjr/np/expression_math.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;
using shape = np::extents<size_t, 4>;
using leaf = decltype(np::as_expr(std::declval<view<float, shape>>()));
using rooted = decltype(np::sqrt(std::declval<const leaf &>()));

template <class E>
concept roots = requires(E &&value) { np::sqrt(std::forward<E>(value)); };
template <class E>
concept magnitudes = requires(E &&value) { np::abs(std::forward<E>(value)); };

static_assert(roots<int> && roots<bool> && roots<view<const double, shape>>);
static_assert(!roots<int *> && !roots<np::tensor<double, shape> &>);
static_assert(!roots<view<volatile double, shape>>);
static_assert(magnitudes<signed char> && magnitudes<int> && magnitudes<long double>);
static_assert(!magnitudes<bool> && !magnitudes<unsigned char> && !magnitudes<unsigned int>);
static_assert(std::same_as<typename rooted::value_type, decltype(std::sqrt(float{}))>);
static_assert(std::same_as<typename rooted::extents_type, shape>);
static_assert(std::same_as<typename rooted::layout_type, np::layout_right>);
static_assert(sizeof(rooted) == sizeof(leaf) && alignof(rooted) == alignof(leaf));
static_assert(np::detail::expression_function_count<rooted> == 0);
static_assert(noexcept(std::declval<const rooted &>().read(0)) == noexcept(std::sqrt(float{})));
static_assert(std::same_as<typename decltype(np::sqrt(1))::value_type, double>);
static_assert(std::same_as<typename decltype(np::exp(1.f))::value_type, float>);
static_assert(std::same_as<typename decltype(np::log(1.L))::value_type, long double>);
static_assert(
    std::same_as<typename decltype(np::abs(std::declval<signed char>()))::value_type, int>);
constexpr auto constructed_only = np::sqrt(9);
static_assert(constructed_only.rank() == 0); // Construction never calls cmath.

TEST(NpExpressionMath, UsesActualStdOverloadsAndPreservesChildTypes) {
    float input[] = {.25f, .5f, 1.f, 2.f};
    const auto a = np::as_expr(view<const float, shape>(input));
    double output[4]{};
    np::eval_to(view<double, shape>(output), np::exp(a));
    for (size_t i = 0; i != 4; ++i)
        EXPECT_EQ(output[i], static_cast<double>(std::exp(input[i])));
    np::eval_to(view<double, shape>(output), np::log(np::sqrt(a)));
    for (size_t i = 0; i != 4; ++i)
        EXPECT_EQ(output[i], static_cast<double>(std::log(std::sqrt(input[i]))));
    int numerator[] = {3, 7, 8, 11}, denominator[] = {2, 2, 3, 4};
    const auto n = np::as_expr(view<const int, shape>(numerator));
    const auto d = np::as_expr(view<const int, shape>(denominator));
    np::eval_to(view<double, shape>(output), np::sqrt(n / d));
    EXPECT_EQ(output[0], 1.);
    EXPECT_EQ(output[1], std::sqrt(3.));
    np::eval_to(view<double, shape>(output), np::sqrt(np::cast<double>(n) / d));
    EXPECT_EQ(output[0], std::sqrt(1.5));
    EXPECT_EQ(output[1], std::sqrt(3.5));
    EXPECT_EQ(np::abs(std::numeric_limits<signed char>::min()).read(0),
              -static_cast<int>(std::numeric_limits<signed char>::min()));
    auto owner = np::eval(np::abs(a));
    ASSERT_TRUE(owner);
    static_assert(std::same_as<typename decltype(owner)::value_type::element_type, float>);
    EXPECT_EQ(owner->view()(2), 1.f);
}

TEST(NpExpressionMath, PositiveStridesBroadcastAndExactInPlaceEvaluation) {
    using matrix_shape = np::extents<size_t, 2, 3>;
    using strided = view<double, matrix_shape, np::layout_stride>;
    double input[] = {1, 999, 4, 999, 9, 999, 999, 16, 999, 25, 999, 36};
    strided source(input, strided::mapping_type({}, std::array<size_t, 2>{7, 2}));
    float bias[] = {0.f, .5f, 1.f};
    double output[6]{};
    const auto expression =
        np::sqrt(source) + np::exp(view<const float, np::extents<size_t, 3>>(bias));
    np::eval_to(view<double, matrix_shape, np::layout_left>(output), expression);
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 3; ++col)
            EXPECT_EQ(output[col * 2 + row],
                      std::sqrt(input[row * 7 + col * 2]) + std::exp(bias[col]));
    np::eval_to(source, np::sqrt(source));
    EXPECT_EQ(input[0], 1.);
    EXPECT_EQ(input[2], 2.);
    EXPECT_EQ(input[11], 6.);
    EXPECT_EQ(input[1], 999.);
    using column = decltype(np::abs(view<float, matrix_shape, np::layout_left>(nullptr)));
    static_assert(std::same_as<typename column::layout_type, np::layout_left>);
}

struct counted_operation {
    static inline int calls = 0;
    static double apply(double value) noexcept {
        ++calls;
        return value;
    }
};

TEST(NpExpressionMath, EmptyDomainSkipsMathAndRankZeroRunsNormally) {
    using empty = np::extents<size_t, 2, 0>;
    auto a = np::as_expr(view<const double, empty, np::layout_left>(nullptr));
    const np::unary_expression<counted_operation, decltype(a)> counted(a);
    counted_operation::calls = 0;
    errno = E2BIG;
    np::eval_to(view<double, empty>(nullptr), np::log(np::sqrt(counted / 0.)));
    const int error = errno;
    EXPECT_EQ(counted_operation::calls, 0);
    EXPECT_EQ(error, E2BIG);
    int value = 9;
    double output = 0.;
    np::eval_to(view<double, np::extents<size_t>>(&output),
                np::sqrt(view<int, np::extents<size_t>>(&value)));
    EXPECT_EQ(output, 3.);
    auto movable =
        np::map([scale = std::make_unique<double>(4.)](double x) noexcept { return x * *scale; },
                np::as_expr(4.));
    static_assert(!roots<decltype(movable) &>);
    auto rooted_move = np::sqrt(std::move(movable));
    EXPECT_EQ(rooted_move.read(0), 4.);
}

TEST(NpExpressionMath, ExceptionalValuesMatchStdWithoutNumericSubstitution) {
    const auto infinity = std::numeric_limits<double>::infinity();
    EXPECT_TRUE(std::signbit(np::sqrt(-0.).read(0)));
    EXPECT_FALSE(std::signbit(np::abs(-0.).read(0)));
    EXPECT_EQ(np::exp(-infinity).read(0), 0.);
    EXPECT_EQ(np::log(infinity).read(0), infinity);
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    EXPECT_TRUE(std::isnan(np::sqrt(nan).read(0)));
    EXPECT_TRUE(std::isnan(np::exp(nan).read(0)));
    EXPECT_TRUE(std::isnan(np::log(nan).read(0)));
    EXPECT_TRUE(std::isnan(np::abs(nan).read(0)));
}

struct math_observation {
    double value;
    int error;
    int flags;
};
template <class Function>
WJR_NOINLINE math_observation observe(Function function) {
    std::feclearexcept(FE_ALL_EXCEPT);
    errno = 0;
    double result = function();
    const int error = errno;
    return {result, error, std::fetestexcept(FE_ALL_EXCEPT)};
}
template <class Reference, class Expression>
void compare_observations(Reference reference, Expression expression) {
    const auto expected = observe(reference);
    const auto actual = observe(expression);
    EXPECT_EQ(actual.error, expected.error);
    EXPECT_EQ(actual.flags, expected.flags);
    if (std::isnan(expected.value))
        EXPECT_TRUE(std::isnan(actual.value));
    else
        EXPECT_EQ(actual.value, expected.value);
}

TEST(NpExpressionMath, DomainAndRangeObservationsFollowTheSameStdCalls) {
    volatile double input[] = {-1., 0., 10000.};
    const double negative = input[0], zero = input[1], large = input[2];
    compare_observations([negative] { return std::sqrt(negative); },
                         [negative] { return np::sqrt(negative).read(0); });
    compare_observations([zero] { return std::log(zero); },
                         [zero] { return np::log(zero).read(0); });
    compare_observations([large] { return std::exp(large); },
                         [large] { return np::exp(large).read(0); });
    std::feclearexcept(FE_ALL_EXCEPT);
    errno = 0;
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
struct throwing_operation {
    static double apply(double) { throw std::runtime_error("child failed"); }
};
struct throwing_copy {
    throwing_copy() = default;
    throwing_copy(const throwing_copy &) { throw std::runtime_error("copy failed"); }
    throwing_copy(throwing_copy &&) noexcept = default;
    double operator()(double value) const noexcept { return value; }
};
TEST(NpExpressionMath, BranchAndClosureExceptionsRemainObservable) {
    auto scalar = np::as_expr(4.);
    const np::unary_expression<throwing_operation, decltype(scalar)> child(scalar);
    static_assert(!noexcept(np::sqrt(child).read(0)));
    EXPECT_THROW((void)np::sqrt(child).read(0), std::runtime_error);
    const auto copied = np::map(throwing_copy{}, scalar);
    static_assert(!noexcept(np::sqrt(copied)) && !noexcept(np::exp(copied)));
    static_assert(!noexcept(np::log(copied)) && !noexcept(np::abs(copied)));
    EXPECT_THROW((void)np::sqrt(copied), std::runtime_error);
    EXPECT_THROW((void)np::exp(copied), std::runtime_error);
    EXPECT_THROW((void)np::log(copied), std::runtime_error);
    EXPECT_THROW((void)np::abs(copied), std::runtime_error);
}
#endif
} // namespace
