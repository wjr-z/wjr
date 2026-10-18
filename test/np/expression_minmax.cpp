#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_reduce.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape>
using view = np::tensor_view<T, Shape>;
using shape = np::extents<size_t, 4>;
using float_view = view<const float, shape>;
using minimum_expression = decltype(np::minimum(std::declval<float_view>(), 1.));
using clip_expression = decltype(np::clip(std::declval<float_view>(), 0u, 1.));
using homogeneous_clip_expression = decltype(np::clip(std::declval<float_view>(), 0.f, 1.f));
using two_elements = view<const int, np::extents<size_t, 2>>;
using three_elements = view<const int, np::extents<size_t, 3>>;

template <class L, class R>
concept has_minimum = requires(L &&left, R &&right) {
    np::minimum(std::forward<L>(left), std::forward<R>(right));
    np::maximum(std::forward<L>(left), std::forward<R>(right));
};

template <class E, class L, class H>
concept has_clip = requires(E &&value, L &&lower, H &&upper) {
    np::clip(std::forward<E>(value), std::forward<L>(lower), std::forward<H>(upper));
};

static_assert(has_minimum<int, double> && has_minimum<float_view, int>);
static_assert(!has_minimum<int *, int> && !has_minimum<np::tensor<int, shape> &, int>);
static_assert(has_clip<int, unsigned, double>);
static_assert(!has_clip<int *, int, int>);
static_assert(!has_minimum<two_elements, three_elements>);
static_assert(!has_clip<two_elements, three_elements, int>);
static_assert(std::same_as<typename minimum_expression::value_type, double>);
static_assert(std::is_trivially_copyable_v<minimum_expression>);
static_assert(np::detail::interleavable_sum_expression<minimum_expression>);
static_assert(std::same_as<typename clip_expression::value_type, double>);
static_assert(!np::detail::interleavable_sum_expression<clip_expression>);
static_assert(np::detail::interleavable_sum_expression<homogeneous_clip_expression>);
constexpr auto constexpr_min_nan =
    np::minimum(std::numeric_limits<double>::quiet_NaN(), 1.).read(0);
constexpr auto constexpr_max_nan =
    np::maximum(1., std::numeric_limits<double>::quiet_NaN()).read(0);
static_assert(np::minimum(2, 3).read(0) == 2);
static_assert(np::maximum(2, 3).read(0) == 3);
static_assert(np::minimum(1.f, 2.f).read(0) == 1.f);
static_assert(np::clip(-1, 0u, 1.).read(0) == 0.);
static_assert(np::clip(4., 3., 1.).read(0) == 1.);
static_assert(constexpr_min_nan != constexpr_min_nan && constexpr_max_nan != constexpr_max_nan);
static_assert(std::bit_cast<std::uint64_t>(np::minimum(-0., 0.).read(0)) ==
              std::uint64_t{0x8000000000000000});
static_assert(std::bit_cast<std::uint64_t>(np::maximum(0., -0.).read(0)) == 0);
static_assert(std::bit_cast<std::uint32_t>(np::minimum(-0.f, 0.f).read(0)) ==
              std::uint32_t{0x80000000});

template <int Id>
struct count_value {
    static inline int calls = 0;
    static int apply(int value) noexcept {
        ++calls;
        return value;
    }
};

TEST(NpExpressionMinMax, MixedTypesBroadcastAndClipPromotesAllOperandsTogether) {
    int input[]{-1, 0, 2, 4};
    unsigned lower[]{0, 1, 1, 1};
    double upper[]{1.5, 1.5, 1.5, 1.5};
    double output[4]{};
    const auto x = np::as_expr(view<const int, shape>(input));
    const auto lo = np::as_expr(view<const unsigned, shape>(lower));
    np::eval_to(view<double, shape>(output), np::clip(x, lo, 1.5));
    EXPECT_EQ((std::to_array(output)), (std::array{0., 1., 1.5, 1.5}));

    // A pairwise conversion would turn -1 into a large unsigned value here.
    double scalar = 0.;
    np::eval_to(view<double, np::extents<size_t>>(&scalar), np::clip(-1, 0u, 1.));
    EXPECT_EQ(scalar, 0.);

    float row[]{-2.f, 0.f, 3.f};
    double column[]{-1., 1.};
    double broadcast[6]{};
    using matrix = np::extents<size_t, 2, 3>;
    using column_shape = np::extents<size_t, 2, 1>;
    using row_shape = np::extents<size_t, 1, 3>;
    np::eval_to(view<double, matrix>(broadcast),
                np::minimum(view<const double, column_shape>(column),
                            np::maximum(view<const float, row_shape>(row), 0.f)));
    EXPECT_EQ((std::to_array(broadcast)), (std::array{-1., -1., -1., 0., 0., 1.}));

    const float matrix_values[]{-1.f, 0.5f, 2.f, 2.f, -2.f, 3.f};
    float column_sums[3]{};
    using sum_output_shape = np::extents<size_t, 3>;
    np::sum_to(view<float, sum_output_shape>(column_sums),
               np::clip(view<const float, matrix>(matrix_values), 0.f, 1.f), np::axes<0>);
    EXPECT_EQ((std::to_array(column_sums)), (std::array{1.f, 0.5f, 2.f}));
}

TEST(NpExpressionMinMax, NanPropagationAndLeftBiasedEqualValues) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    double input[]{nan, 2., 3., -inf};
    double other[]{1., nan, 4., inf};
    double output[4]{};
    const auto a = np::as_expr(view<const double, shape>(input));
    const auto b = np::as_expr(view<const double, shape>(other));
    np::eval_to(view<double, shape>(output), np::minimum(a, b));
    EXPECT_TRUE(std::isnan(output[0]));
    EXPECT_TRUE(std::isnan(output[1]));
    EXPECT_EQ((std::array{output[2], output[3]}), (std::array{3., -inf}));
    np::eval_to(view<double, shape>(output), np::maximum(a, b));
    EXPECT_TRUE(std::isnan(output[0]));
    EXPECT_TRUE(std::isnan(output[1]));
    EXPECT_EQ((std::array{output[2], output[3]}), (std::array{4., inf}));

    const auto min_left_negative_zero = np::minimum(-0., 0.).read(0);
    const auto min_left_positive_zero = np::minimum(0., -0.).read(0);
    const auto max_left_negative_zero = np::maximum(-0., 0.).read(0);
    const auto max_left_positive_zero = np::maximum(0., -0.).read(0);
    EXPECT_TRUE(std::signbit(min_left_negative_zero));
    EXPECT_FALSE(std::signbit(min_left_positive_zero));
    EXPECT_TRUE(std::signbit(max_left_negative_zero));
    EXPECT_FALSE(std::signbit(max_left_positive_zero));
    EXPECT_TRUE(std::isnan(np::clip(nan, 0., 1.).read(0)));
    EXPECT_TRUE(std::isnan(np::clip(0., nan, 1.).read(0)));
    EXPECT_TRUE(std::isnan(np::clip(0., 0., nan).read(0)));
    EXPECT_EQ(np::minimum(4u, 2).read(0), 2u);
}

TEST(NpExpressionMinMax, EmptyScalarAndExactInPlace) {
    using empty = np::extents<size_t, 2, 0>;
    np::eval_to(
        view<double, empty>(nullptr),
        np::minimum(view<const double, empty>(nullptr), std::numeric_limits<double>::quiet_NaN()));
    double scalar = -2.;
    np::eval_to(view<double, np::extents<size_t>>(&scalar), np::clip(scalar, -1., 1.));
    EXPECT_EQ(scalar, -1.);
    double values[]{-2., -0., 0.5, 3.};
    const auto values_view = view<double, shape>(values);
    np::eval_to(values_view, np::clip(values_view, 0., 1.));
    EXPECT_EQ((std::to_array(values)), (std::array{0., -0., 0.5, 1.}));
    EXPECT_TRUE(std::signbit(values[1]));
    const double finite[]{-2., 0., 2., 4.};
    np::eval_to(view<double, shape>(values), np::clip(view<const double, shape>(finite), 3., 1.));
    EXPECT_EQ((std::to_array(values)), (std::array{1., 1., 1., 1.}));

    using scalar_expression = decltype(np::as_expr(1));
    const auto value = np::unary_expression<count_value<0>, scalar_expression>(np::as_expr(-1));
    const auto lower = np::unary_expression<count_value<1>, scalar_expression>(np::as_expr(0));
    const auto upper = np::unary_expression<count_value<2>, scalar_expression>(np::as_expr(1));
    count_value<0>::calls = count_value<1>::calls = count_value<2>::calls = 0;
    EXPECT_EQ(np::clip(value, lower, upper).read(0), 0);
    EXPECT_EQ(count_value<0>::calls, 1);
    EXPECT_EQ(count_value<1>::calls, 1);
    EXPECT_EQ(count_value<2>::calls, 1);
}
} // namespace
