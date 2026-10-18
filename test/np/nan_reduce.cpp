#include <array>
#include <cmath>
#include <limits>
#include <type_traits>

#include <gtest/gtest.h>

#include <wjr/np/nan_reduce.hpp>
#include <wjr/np/transpose.hpp>

namespace {
namespace np = wjr::np;
template <class T, class Shape, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;
using four = np::extents<size_t, 4>;
using float_view = view<const float, four>;

template <class T>
struct custom_accessor : np::default_accessor<T> {};

template <class V, class A = void>
concept has_nansum = requires(V value) { np::nansum<A>(value); };
template <class V, class A = void>
concept has_nanmean = requires(V value) { np::nanmean<A>(value); };

static_assert(has_nansum<float_view> && has_nansum<float_view, long double> &&
              has_nansum<float_view, int>);
static_assert(has_nanmean<float_view> && has_nanmean<float_view, long double>);
static_assert(!has_nanmean<float_view, int>);
static_assert(!has_nansum<np::tensor_view<volatile float, four>>);
static_assert(!has_nansum<
              np::tensor_view<const float, four, np::layout_right, custom_accessor<const float>>>);
static_assert(!has_nanmean<np::tensor_view<int, four>, const double>);
static_assert(std::same_as<decltype(np::nansum(std::declval<float_view>())), float>);
static_assert(std::same_as<decltype(np::nansum(std::declval<view<const int, four>>())), int>);
static_assert(std::same_as<decltype(np::nanmean(std::declval<float_view>())), float>);
static_assert(std::same_as<decltype(np::nanmean(std::declval<view<const int, four>>())), double>);
static_assert(
    std::same_as<decltype(np::nansum<long double>(std::declval<float_view>())), long double>);
static_assert(
    std::same_as<decltype(np::nanmean<long double>(std::declval<float_view>())), long double>);
static_assert(noexcept(np::nansum(std::declval<float_view>())));
static_assert(noexcept(np::nanmean(std::declval<float_view>())));

constexpr double quiet_nan = std::numeric_limits<double>::quiet_NaN();
constexpr double constexpr_values[]{1.0, quiet_nan, 2.0, quiet_nan};
constexpr view<const double, np::extents<size_t, 4>> constexpr_view(constexpr_values);
static_assert(np::nansum(constexpr_view) == 3.0);
static_assert(np::nanmean(constexpr_view) == 1.5);
constexpr float precision_values[]{16777216.f, std::numeric_limits<float>::quiet_NaN(), 1.f,
                                   -16777216.f};
constexpr view<const float, np::extents<size_t, 4>> constexpr_precision(precision_values);
static_assert(np::nansum(constexpr_precision) == 0.f);
static_assert(np::nansum<double>(constexpr_precision) == 1.0);
static_assert(np::nanmean(constexpr_precision) == 0.f);
static_assert(np::nanmean<double>(constexpr_precision) == 1.0 / 3.0);

TEST(np_nan_reduce, FloatingPromotionAndFilterBeforeAccumulatorConversion) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double input[]{1.5, nan, 2.25, nan};
    const view<const double, np::extents<size_t, 4>> source(input);
    EXPECT_EQ(np::nansum(source), 3.75);
    EXPECT_EQ(np::nanmean(source), 1.875);
    EXPECT_EQ(np::nansum<long double>(source), 3.75L);
    EXPECT_EQ(np::nanmean<long double>(source), 1.875L);

    const double cast_input[]{nan, 3.75, nan, 2.25};
    const view<const double, np::extents<size_t, 4>> cast_source(cast_input);
    EXPECT_EQ(np::nansum<int>(cast_source), 5);
}

TEST(np_nan_reduce, EmptyAllNanRankZeroAndSignedZero) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double all_nan[]{nan, nan, nan};
    const view<const double, np::extents<size_t, 3>> nan_view(all_nan);
    const double sum = np::nansum(nan_view);
    EXPECT_EQ(sum, 0.0);
    EXPECT_FALSE(std::signbit(sum));
    EXPECT_TRUE(std::isnan(np::nanmean(nan_view)));

    using empty_shape = np::extents<size_t, 2, 0>;
    const view<const double, empty_shape> empty(nullptr);
    EXPECT_EQ(np::nansum(empty), 0.0);
    EXPECT_FALSE(std::signbit(np::nansum(empty)));
    EXPECT_TRUE(std::isnan(np::nanmean(empty)));

    const double scalar_nan = nan;
    const view<const double, np::extents<size_t>> nan_scalar(&scalar_nan);
    EXPECT_EQ(np::nansum(nan_scalar), 0.0);
    EXPECT_TRUE(std::isnan(np::nanmean(nan_scalar)));

    const double negative_zero[]{-0.0, nan};
    const view<const double, np::extents<size_t, 2>> zero_view(negative_zero);
    EXPECT_FALSE(std::signbit(np::nansum(zero_view)));
    EXPECT_FALSE(std::signbit(np::nanmean(zero_view)));
}

TEST(np_nan_reduce, InfinitiesRemainInOrderedArithmetic) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();
    double input[]{infinity, nan, 2.0};
    const view<const double, np::extents<size_t, 3>> source(input);
    EXPECT_EQ(np::nansum(source), infinity);
    EXPECT_EQ(np::nanmean(source), infinity);
    input[2] = -infinity;
    EXPECT_TRUE(std::isnan(np::nansum(source)));
    EXPECT_TRUE(std::isnan(np::nanmean(source)));
}

TEST(np_nan_reduce, LogicalRowMajorAcrossLayoutsTransposeAndPadding) {
    using matrix = np::extents<size_t, 2, 3>;
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double row_major[]{1.0e20, nan, -1.0e20, 3.0, 4.0, nan};
    const view<const double, matrix> right(row_major);
    EXPECT_EQ(np::nansum(right), 7.0);
    EXPECT_EQ(np::nanmean(right), 1.75);

    const double column_major[]{1.0e20, 3.0, nan, 4.0, -1.0e20, nan};
    const view<const double, matrix, np::layout_left> left(column_major);
    EXPECT_EQ(np::nansum(left), 7.0);
    EXPECT_EQ(np::nanmean(left), 1.75);

    const auto transposed = np::transpose_view(right);
    EXPECT_EQ(np::nansum(transposed), 0.0);
    EXPECT_EQ(np::nanmean(transposed), 0.0);

    using mapping = np::layout_stride::mapping<matrix>;
    const double padded[]{1.0e20, nan, nan, nan, -1.0e20, 3.0, nan, 4.0, nan, nan};
    const mapping padded_map(matrix{}, std::array<size_t, 2>{5, 2});
    const view<const double, matrix, np::layout_stride> strided(padded, padded_map);
    EXPECT_EQ(np::nansum(strided), 7.0);
    EXPECT_EQ(np::nanmean(strided), 1.75);
}

TEST(np_nan_reduce, IntegerViewsDelegateToExistingReductions) {
    const int values[]{1, 2, 3, 4};
    const view<const int, np::extents<size_t, 4>> source(values);
    EXPECT_EQ(np::nansum(source), np::sum(source));
    EXPECT_EQ(np::nanmean(source), np::mean(source));
    EXPECT_EQ(np::nansum<long long>(source), np::sum<long long>(source));
    EXPECT_EQ(np::nanmean<float>(source), np::mean<float>(source));
    const bool flags[]{true, false, true, true};
    const view<const bool, np::extents<size_t, 4>> bool_source(flags);
    EXPECT_EQ(np::nansum(bool_source), np::sum(bool_source));
    EXPECT_EQ(np::nanmean(bool_source), np::mean(bool_source));
}
} // namespace
