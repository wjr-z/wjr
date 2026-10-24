#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/statistics.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;

template <class X, class Y, class Acc = void>
concept has_statistics = requires(X x, Y y) {
    np::covariance<Acc>(x, y);
    np::nancovariance<Acc>(x, y);
    np::pearson_correlation<Acc>(x, y);
    np::nanpearson_correlation<Acc>(x, y);
};
static_assert(has_statistics<view<const int>, view<float>>);
static_assert(!has_statistics<view<volatile double>, view<double>>);
static_assert(!has_statistics<view<double>, view<double>, int>);
static_assert(!has_statistics<view<double>, view<double>, const double>);
static_assert(!has_statistics<np::tensor_view<int, np::extents<size_t, 2>>,
                              np::tensor_view<int, np::extents<size_t, 3>>>);
static_assert(!has_statistics<np::tensor_view<int, np::extents<size_t, 2, 3>>, view<double>>);
static_assert(
    std::same_as<decltype(np::covariance(std::declval<view<int>>(), std::declval<view<int>>())),
                 double>);
static_assert(
    std::same_as<decltype(np::covariance(std::declval<view<float>>(), std::declval<view<float>>())),
                 float>);
static_assert(noexcept(np::pearson_correlation(std::declval<view<double>>(),
                                               std::declval<view<double>>())));

constexpr bool constant_covariance() {
    const int x[] = {1, 2, 3};
    const int y[] = {2, 4, 6};
    using shape = np::extents<size_t, 3>;
    const np::tensor_view<const int, shape> a(x), b(y);
    return np::covariance(a, b) == 2 && np::covariance(a, b, 0) == 4.0 / 3.0;
}
static_assert(constant_covariance());

TEST(NpStatistics, SampleAndPopulationCovariance) {
    EXPECT_TRUE(constant_covariance());
    const int x[] = {1, 2, 3};
    const double y[] = {6, 4, 2};
    const view<const int> a(x, 3);
    const view<const double> b(y, 3);
    EXPECT_EQ(np::covariance(a, b), -2);
    EXPECT_NEAR(np::pearson_correlation(a, b), -1, 1e-15);
    EXPECT_EQ(np::covariance(a, a), 1);
    EXPECT_NEAR(np::pearson_correlation(a, a), 1, 1e-15);
    const bool flags[] = {true, false, true, false};
    const view<const bool> boolean(flags, 4);
    EXPECT_EQ(np::nancovariance(boolean, boolean, 0), 0.25);
}

TEST(NpStatistics, EmptySingletonAndCorrection) {
    const view<const double> empty(nullptr, 0);
    EXPECT_TRUE(std::isnan(np::covariance(empty, empty)));
    EXPECT_TRUE(std::isnan(np::pearson_correlation(empty, empty)));
    const double input[] = {7};
    const view<const double> one(input, 1);
    EXPECT_EQ(np::covariance(one, one, 0), 0);
    EXPECT_TRUE(std::isnan(np::covariance(one, one)));
    EXPECT_TRUE(std::isnan(np::pearson_correlation(one, one)));
    EXPECT_TRUE(std::isnan(np::covariance(one, one, std::numeric_limits<size_t>::max())));
}

TEST(NpStatistics, PairwiseMissingValues) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const double x[] = {nan, 1, 100, 2, nan, 3};
    const double y[] = {9, 2, nan, 4, 7, 6};
    const view<const double> a(x, 6), b(y, 6);
    EXPECT_TRUE(std::isnan(np::covariance(a, b)));
    EXPECT_TRUE(std::isnan(np::pearson_correlation(a, b)));
    EXPECT_EQ(np::nancovariance(a, b), 2);
    EXPECT_EQ(np::nancovariance(a, b, 0), 4.0 / 3.0);
    EXPECT_NEAR(np::nanpearson_correlation(a, b), 1, 1e-15);
    EXPECT_TRUE(std::isnan(np::nancovariance(a, b, 3)));
    const double missing[] = {nan, nan};
    const view<const double> all_missing(missing, 2);
    EXPECT_TRUE(std::isnan(np::nancovariance(all_missing, all_missing)));
    EXPECT_TRUE(std::isnan(np::nanpearson_correlation(all_missing, all_missing)));
    const double single_pair[] = {nan, 1};
    const view<const double> single(single_pair, 2);
    EXPECT_EQ(np::nancovariance(single, single, 0), 0);
    EXPECT_TRUE(std::isnan(np::nanpearson_correlation(single, single)));
}

TEST(NpStatistics, ConstantAndNonfiniteVectors) {
    std::array<double, 300> constants;
    constants.fill(0.1);
    const view<const double> constant(constants.data(), constants.size());
    EXPECT_EQ(np::covariance(constant, constant), 0);
    EXPECT_TRUE(std::isnan(np::pearson_correlation(constant, constant)));
    EXPECT_TRUE(std::isnan(np::nanpearson_correlation(constant, constant)));
    const double inf = std::numeric_limits<double>::infinity();
    const double x[] = {1, inf, 3};
    const double y[] = {1, 2, 3};
    const view<const double> a(x, 3), b(y, 3);
    EXPECT_TRUE(std::isnan(np::nanpearson_correlation(a, b)));
    EXPECT_TRUE(std::isnan(np::pearson_correlation(a, b)));
    EXPECT_TRUE(std::isnan(np::nancovariance(a, b)));
}

TEST(NpStatistics, LargeOffsetAndMixedAccumulator) {
    const double x[] = {1e12 + 1, 1e12 + 2, 1e12 + 3};
    const double y[] = {-1e12 + 2, -1e12 + 4, -1e12 + 6};
    EXPECT_EQ(np::covariance(view<const double>(x, 3), view<const double>(y, 3)), 2);
    EXPECT_NEAR(np::pearson_correlation(view<const double>(x, 3), view<const double>(y, 3)), 1,
                1e-15);
    const float f[] = {16777216, 1, -16777216, 3};
    const view<const float> input(f, 4);
    const auto expected = np::covariance<double>(input, input);
    EXPECT_GT(expected, 0);
    EXPECT_NEAR(np::pearson_correlation<double>(input, input), 1, 1e-15);
}

TEST(NpStatistics, ExtremeScaleRecoveryAndNormDivision) {
    for (const double scale : {1e-200, 1e200}) {
        const double x[] = {scale, 2 * scale, 3 * scale};
        const double y[] = {3, 2, 1};
        EXPECT_NEAR(np::pearson_correlation(view<const double>(x, 3), view<const double>(y, 3)), -1,
                    1e-15);
    }
    const auto largest = std::numeric_limits<double>::max();
    const double huge[] = {-largest, largest};
    const double opposite[] = {largest, -largest};
    EXPECT_NEAR(
        np::pearson_correlation(view<const double>(huge, 2), view<const double>(opposite, 2)), -1,
        1e-15);
    const double large[] = {-1e100, 1e100};
    const double small[] = {-1e-150, 1e-150};
    EXPECT_NEAR(np::pearson_correlation(view<const double>(large, 2), view<const double>(small, 2)),
                1, 1e-15);
    const auto tiny = std::numeric_limits<double>::denorm_min();
    const double subnormal[] = {tiny, 2 * tiny, 3 * tiny};
    EXPECT_NEAR(
        np::pearson_correlation(view<const double>(subnormal, 3), view<const double>(subnormal, 3)),
        1, 1e-15);
}

TEST(NpStatistics, StridedInputsSkipPadding) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const double x[] = {1, nan, 2, nan, 3};
    const double y[] = {2, nan, nan, 4, nan, nan, 6};
    using shape = np::extents<size_t, 3>;
    const np::layout_stride::mapping<shape> xmap(shape{}, std::array<size_t, 1>{2});
    const np::layout_stride::mapping<shape> ymap(shape{}, std::array<size_t, 1>{3});
    const np::tensor_view<const double, shape, np::layout_stride> a(x, xmap), b(y, ymap);
    EXPECT_EQ(np::covariance(a, b), 2);
    EXPECT_NEAR(np::pearson_correlation(a, b), 1, 1e-15);
}

TEST(NpStatistics, PaddedVectorMapping) {
    using shape = np::extents<size_t, 3>;
    using layout = Kokkos::Experimental::layout_right_padded<8>;
    const double x[] = {1, 2, 3};
    const double y[] = {2, 4, 6};
    const np::tensor_view<const double, shape, layout> a(x), b(y);
    EXPECT_EQ(np::covariance(a, b), 2);
    EXPECT_NEAR(np::pearson_correlation(a, b), 1, 1e-15);
}

TEST(NpStatistics, RandomAgainstExtendedPrecisionReference) {
    std::mt19937 engine(341);
    for (const size_t count : {2, 3, 16, 300, 4096}) {
        std::vector<double> x(count), y(count);
        long double sx = 0, sy = 0;
        for (size_t i = 0; i != count; ++i) {
            x[i] = static_cast<int>(engine() % 1000) - 500;
            y[i] = 0.3 * x[i] + static_cast<int>(engine() % 1000) - 500;
            sx += x[i];
            sy += y[i];
        }
        sx /= count;
        sy /= count;
        long double cross = 0, xx = 0, yy = 0;
        for (size_t i = 0; i != count; ++i) {
            const auto dx = x[i] - sx;
            const auto dy = y[i] - sy;
            cross += dx * dy;
            xx += dx * dx;
            yy += dy * dy;
        }
        const view<const double> a(x.data(), count), b(y.data(), count);
        EXPECT_NEAR(np::covariance(a, b), static_cast<double>(cross / (count - 1)), 1e-8);
        EXPECT_NEAR(np::pearson_correlation(a, b), static_cast<double>(cross / std::sqrt(xx * yy)),
                    2e-14);
    }
}

TEST(NpStatistics, MissingPairsMatchExplicitCompaction) {
    std::mt19937 engine(771);
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    for (const size_t period : {2, 3, 17}) {
        std::array<double, 300> x, y;
        std::vector<double> kept_x, kept_y;
        for (size_t i = 0; i != x.size(); ++i) {
            x[i] = i % period == 0 ? nan : static_cast<double>(engine() % 1000);
            y[i] = i % 5 == 0 ? nan : static_cast<double>(engine() % 1000);
            if (!std::isnan(x[i]) && !std::isnan(y[i])) {
                kept_x.push_back(x[i]);
                kept_y.push_back(y[i]);
            }
        }
        const view<const double> a(x.data(), x.size()), b(y.data(), y.size());
        const view<const double> compact_a(kept_x.data(), kept_x.size());
        const view<const double> compact_b(kept_y.data(), kept_y.size());
        const auto expected = np::covariance(compact_a, compact_b);
        EXPECT_NEAR(np::nancovariance(a, b), expected, std::max(1., std::abs(expected)) * 1e-13);
        EXPECT_NEAR(np::nanpearson_correlation(a, b), np::pearson_correlation(compact_a, compact_b),
                    1e-13);
    }
}
} // namespace
