#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/detail/elementwise.hpp>
#define min(a, b) unexpected_min_macro(a, b)
#define max(a, b) unexpected_max_macro(a, b)
#include <wjr/np/extrema.hpp>
#undef min
#undef max
#include <wjr/np/slice.hpp>

namespace {
namespace np = wjr::np;
using matrix = np::extents<size_t, 2, 3>;
using line = np::dextents<size_t, 1>;
template <class T, class Shape = matrix, class Layout = np::layout_right>
using view = np::tensor_view<T, Shape, Layout>;
template <class V>
concept extrema = requires(V v) {
    np::min(v);
    np::max(v);
    np::argmin(v);
    np::argmax(v);
};
struct custom_accessor : np::default_accessor<int> {};
static_assert(extrema<view<const int>>);
static_assert(extrema<view<const bool>>);
static_assert(extrema<view<const long double>>);
static_assert(!extrema<view<volatile int>>);
static_assert(!extrema<np::tensor_view<int, matrix, np::layout_right, custom_accessor>>);
static_assert(
    std::same_as<decltype(np::min(std::declval<view<const std::uint8_t>>())), std::uint8_t>);
static_assert(std::same_as<decltype(np::max(std::declval<view<const bool>>())), bool>);
static_assert(std::same_as<decltype(np::argmin(std::declval<view<const float>>())), size_t>);
static_assert(noexcept(np::min(std::declval<view<const int>>())));
static_assert(noexcept(np::argmax(std::declval<view<const float>>())));

constexpr bool constant_cases() {
    const int right[]{3, -2, 7, -2, 7, 4};
    const int left[]{3, -2, -2, 7, 7, 4};
    const view<const int> a(right);
    const view<const int, matrix, np::layout_left> b(left);
    if (np::min(a) != -2 || np::max(a) != 7 || np::argmin(a) != 1 || np::argmax(a) != 2)
        return false;
    if (np::min(b) != -2 || np::max(b) != 7 || np::argmin(b) != 1 || np::argmax(b) != 2)
        return false;
    const np::layout_stride::mapping<np::extents<size_t, 3>> mapping(np::extents<size_t, 3>{},
                                                                     std::array<size_t, 1>{2});
    const view<const int, np::extents<size_t, 3>, np::layout_stride> c(right, mapping);
    const int scalar = -9;
    const view<const int, np::extents<size_t>> d(&scalar);
    return np::min(c) == 3 && np::max(c) == 7 && np::argmax(c) == 1 && np::min(d) == -9 &&
           np::max(d) == -9 && np::argmin(d) == 0 && np::argmax(d) == 0;
}
static_assert(constant_cases());

TEST(NpExtrema, ConstantEvaluationAndFirstTie) {
    EXPECT_TRUE(constant_cases());
    int equal[]{7, 7, 7};
    const view<const int, line> source(equal, 3);
    EXPECT_EQ(np::min(source), 7);
    EXPECT_EQ(np::max(source), 7);
    EXPECT_EQ(np::argmin(source), 0);
    EXPECT_EQ(np::argmax(source), 0);
}

TEST(NpExtrema, IntegerWidthsAndBoolKeepTheirDtype) {
    const std::uint64_t integers[]{UINT64_MAX - 1, UINT64_MAX, 0, UINT64_MAX};
    const view<const std::uint64_t, line> source(integers, 4);
    EXPECT_EQ(np::min(source), 0);
    EXPECT_EQ(np::max(source), UINT64_MAX);
    EXPECT_EQ(np::argmin(source), 2);
    EXPECT_EQ(np::argmax(source), 1);
    const std::int64_t signed_values[]{INT64_MIN, INT64_MAX, INT64_MIN};
    const view<const std::int64_t, line> signed_source(signed_values, 3);
    EXPECT_EQ(np::min(signed_source), INT64_MIN);
    EXPECT_EQ(np::max(signed_source), INT64_MAX);
    EXPECT_EQ(np::argmin(signed_source), 0);
    EXPECT_EQ(np::argmax(signed_source), 1);
    const bool flags[]{true, false, false, true};
    const view<const bool, line> boolean(flags, 4);
    EXPECT_FALSE(np::min(boolean));
    EXPECT_TRUE(np::max(boolean));
    EXPECT_EQ(np::argmin(boolean), 1);
    EXPECT_EQ(np::argmax(boolean), 0);
}

TEST(NpExtrema, LogicalIndicesIgnorePhysicalLayoutAndPadding) {
    std::array<double, 25> values;
    values.fill(std::numeric_limits<double>::quiet_NaN());
    const np::layout_stride::mapping<matrix> mapping(matrix{}, std::array<size_t, 2>{8, 2});
    const view<double, matrix, np::layout_stride> source(values.data() + 1, mapping);
    const double logical[]{3, -2, 7, -2, 7, 4};
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 3; ++col)
            source(row, col) = logical[row * 3 + col];
    EXPECT_EQ(np::min(source), -2.);
    EXPECT_EQ(np::max(source), 7.);
    EXPECT_EQ(np::argmin(source), 1);
    EXPECT_EQ(np::argmax(source), 2);
    using padded_shape = np::extents<size_t, 3, 5>;
    std::array<double, 15> packed;
    packed.fill(std::numeric_limits<double>::quiet_NaN());
    const auto window =
        np::slice_view(view<double, padded_shape>(packed.data()), std::pair{0, 2}, std::pair{1, 4});
    static_assert(np::detail::padded_layout<typename decltype(window)::layout_type>);
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 3; ++col)
            window(row, col) = logical[row * 3 + col];
    EXPECT_EQ(np::min(window), -2.);
    EXPECT_EQ(np::max(window), 7.);
    EXPECT_EQ(np::argmin(window), 1);
    EXPECT_EQ(np::argmax(window), 2);
    using cube_shape = np::extents<size_t, 2, 1, 3>;
    const np::layout_stride::mapping<cube_shape> cube_mapping(cube_shape{},
                                                              std::array<size_t, 3>{8, 7, 2});
    const view<const double, cube_shape, np::layout_stride> cube(values.data() + 1, cube_mapping);
    EXPECT_EQ(np::min(cube), -2.);
    EXPECT_EQ(np::argmax(cube), 2);
}

TEST(NpExtrema, FirstNaNAndSignedZeroAreStable) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    for (size_t first = 0; first != 6; ++first) {
        double values[]{3., -2., 7., -2., 7., 4.};
        values[5] = nan;
        values[first] = nan;
        const view<const double> source(values);
        EXPECT_TRUE(std::isnan(np::min(source)));
        EXPECT_TRUE(std::isnan(np::max(source)));
        EXPECT_EQ(np::argmin(source), first);
        EXPECT_EQ(np::argmax(source), first);
    }
    for (bool negative : {false, true}) {
        double zeros[]{negative ? -0. : 0., negative ? 0. : -0., 0.};
        const view<const double, line> source(zeros, 3);
        EXPECT_EQ(std::signbit(np::min(source)), negative);
        EXPECT_EQ(std::signbit(np::max(source)), negative);
        EXPECT_EQ(np::argmin(source), 0);
        EXPECT_EQ(np::argmax(source), 0);
    }
    const double infinity = std::numeric_limits<double>::infinity();
    double values[]{infinity, -infinity, -infinity, infinity};
    const view<const double, line> source(values, 4);
    EXPECT_EQ(np::min(source), -infinity);
    EXPECT_EQ(np::max(source), infinity);
    EXPECT_EQ(np::argmin(source), 1);
    EXPECT_EQ(np::argmax(source), 0);
    const view<const double, np::extents<size_t>> scalar_nan(&nan);
    EXPECT_TRUE(std::isnan(np::min(scalar_nan)));
    EXPECT_EQ(np::argmax(scalar_nan), 0);
    // The first physical NaN is logical index 3; logical index 1 wins.
    const double left_values[]{3., nan, nan, 7., 8., 4.};
    const view<const double, matrix, np::layout_left> left(left_values);
    EXPECT_TRUE(std::isnan(np::min(left)));
    EXPECT_TRUE(std::isnan(np::max(left)));
    EXPECT_EQ(np::argmin(left), 1);
    EXPECT_EQ(np::argmax(left), 1);
}

TEST(NpExtrema, StridedNaNUsesLogicalOrdinalAndTailsDoNotOverread) {
    for (size_t count = 1; count != 130; ++count) {
        std::vector<float> values(count);
        for (size_t i = 0; i != count; ++i)
            values[i] = static_cast<float>(i);
        const view<const float, line> source(values.data(), count);
        EXPECT_EQ(np::argmin(source), 0);
        EXPECT_EQ(np::argmax(source), count - 1);
        EXPECT_EQ(np::max(source), static_cast<float>(count - 1));
    }
    double values[]{4., 99., 3., 99., std::numeric_limits<double>::quiet_NaN(), 99., -8.};
    const np::layout_stride::mapping<np::extents<size_t, 4>> mapping(np::extents<size_t, 4>{},
                                                                     std::array<size_t, 1>{2});
    const view<const double, np::extents<size_t, 4>, np::layout_stride> source(values, mapping);
    EXPECT_TRUE(std::isnan(np::max(source)));
    EXPECT_EQ(np::argmin(source), 2);
    EXPECT_EQ(np::argmax(source), 2);
}
} // namespace
