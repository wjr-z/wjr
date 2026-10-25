#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/histogram.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;
template <class T>
constexpr auto strided(T *data, size_t count, size_t step) {
    using shape = np::dextents<size_t, 1>;
    return np::tensor_view<T, shape, np::layout_stride>(
        data, np::layout_stride::mapping<shape>(shape(count), std::array<size_t, 1>{step}));
}
template <class D, class V, class E>
concept available = requires(D dst, V values, E edges) { np::histogram_to(dst, values, edges); };
static_assert(available<view<char>, view<int>, view<const int>>);
static_assert(available<view<uint64_t>, view<const double>, view<double>>);
static_assert(!available<view<bool>, view<int>, view<int>>);
static_assert(!available<view<float>, view<int>, view<int>>);
static_assert(!available<view<const int>, view<int>, view<int>>);
static_assert(!available<view<volatile int>, view<int>, view<int>>);
static_assert(!available<view<int>, view<bool>, view<bool>>);
static_assert(!available<view<int>, view<volatile int>, view<int>>);
static_assert(!available<view<int>, view<int>, view<volatile int>>);
static_assert(!available<view<int>, view<float>, view<double>>);
static_assert(!available<view<int>, np::tensor_view<int, np::extents<size_t, 2, 2>>, view<int>>);
static_assert(!available<view<int>, view<int>, np::tensor_view<int, np::extents<size_t>>>);
static_assert(noexcept(np::histogram_to(std::declval<view<int>>(), std::declval<view<float>>(),
                                        std::declval<view<const float>>())));

constexpr bool constants() {
    const int edges[] = {0, 1, 1, 3};
    const int values[] = {-1, 0, 1, 2, 3, 4};
    unsigned counts[3] = {99, 99, 99};
    np::histogram_to(view<unsigned>(counts, 3), view<const int>(values, 6),
                     view<const int>(edges, 4));
    return counts[0] == 1 && counts[1] == 0 && counts[2] == 3;
}
static_assert(constants());

constexpr bool strided_constants() {
    const int input[] = {0, -99, 1, -99, 3};
    unsigned counts[] = {99, 99, 99};
    np::histogram_to(strided(counts, 2, 2), strided(input, 3, 2), strided(input, 3, 2));
    return counts[0] == 1 && counts[1] == 99 && counts[2] == 2;
}
static_assert(strided_constants());

constexpr bool singleton_constants() {
    const int edges[] = {2, 2};
    const int values[] = {1, 2, 2, 3};
    unsigned counts[] = {99};
    np::histogram_to(view<unsigned>(counts, 1), view<const int>(values, 4),
                     view<const int>(edges, 2));
    return counts[0] == 2;
}
static_assert(singleton_constants());

TEST(NpHistogram, ConstantsEndpointsAndRequiredReset) {
    EXPECT_TRUE(constants());
    const int edges[] = {0, 1, 2};
    const int values[] = {0, 1, 2};
    int counts[] = {99, 99};
    np::histogram_to(view<int>(counts, 2), view<const int>(values, 3), view<const int>(edges, 3));
    EXPECT_EQ(counts[0], 1);
    EXPECT_EQ(counts[1], 2);
    np::histogram_to(view<int>(counts, 2), view<const int>(nullptr, 0), view<const int>(edges, 3));
    EXPECT_EQ(counts[0], 0);
    EXPECT_EQ(counts[1], 0);
}

TEST(NpHistogram, DuplicateBoundariesAndSingleBin) {
    const int zero_width[] = {2, 2, 2, 2};
    const int values[] = {1, 2, 2, 3};
    int counts[3];
    np::histogram_to(view<int>(counts, 3), view<const int>(values, 4),
                     view<const int>(zero_width, 4));
    EXPECT_EQ((std::array{counts[0], counts[1], counts[2]}), (std::array{0, 0, 2}));
    const int single[] = {1, 3};
    np::histogram_to(view<int>(counts, 1), view<const int>(values, 4), view<const int>(single, 2));
    EXPECT_EQ(counts[0], 4);
    const int final_duplicate[] = {0, 2, 2};
    np::histogram_to(view<int>(counts, 2), view<const int>(values, 4),
                     view<const int>(final_duplicate, 3));
    EXPECT_EQ(counts[0], 1);
    EXPECT_EQ(counts[1], 2);
}

TEST(NpHistogram, NaNsInfinitiesAndSignedZero) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    const double edges[] = {-inf, -0., 0., inf};
    const double values[] = {nan, -inf, -1., -0., 0., 1., inf, nan};
    unsigned counts[3];
    np::histogram_to(view<unsigned>(counts, 3), view<const double>(values, 8),
                     view<const double>(edges, 4));
    EXPECT_EQ((std::array{counts[0], counts[1], counts[2]}), (std::array{2u, 0u, 4u}));
    const double infinite_bin[] = {inf, inf};
    np::histogram_to(view<unsigned>(counts, 1), view<const double>(values, 8),
                     view<const double>(infinite_bin, 2));
    EXPECT_EQ(counts[0], 1);
}

TEST(NpHistogram, NativeIntegerPrecisionAndPerBinRepresentability) {
    const uint64_t edges[] = {UINT64_MAX - 3, UINT64_MAX - 2, UINT64_MAX - 1, UINT64_MAX};
    const uint64_t values[] = {0, UINT64_MAX - 3, UINT64_MAX - 2, UINT64_MAX - 1, UINT64_MAX};
    unsigned counts[3];
    np::histogram_to(view<unsigned>(counts, 3), view<const uint64_t>(values, 5),
                     view<const uint64_t>(edges, 4));
    EXPECT_EQ((std::array{counts[0], counts[1], counts[2]}), (std::array{1u, 1u, 2u}));
    const int bins[] = {-1, 1, 3};
    std::array<int, 400> many_values;
    for (size_t i = 0; i != many_values.size(); ++i)
        many_values[i] = i % 2 == 0 ? 0 : 2;
    uint8_t narrow[2];
    np::histogram_to(view<uint8_t>(narrow, 2),
                     view<const int>(many_values.data(), many_values.size()),
                     view<const int>(bins, 3));
    EXPECT_EQ(narrow[0], 200);
    EXPECT_EQ(narrow[1], 200);
}

TEST(NpHistogram, SingleBinNarrowCountAndStridedSpecialValues) {
    const int point[] = {2, 2};
    std::array<int, 255> values;
    values.fill(2);
    uint8_t narrow[] = {99, 73};
    np::histogram_to(strided(narrow, 1, 2), view<const int>(values.data(), values.size()),
                     view<const int>(point, 2));
    EXPECT_EQ(narrow[0], 255);
    EXPECT_EQ(narrow[1], 73);

    const auto infinity = std::numeric_limits<double>::infinity();
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const double input[] = {-infinity, 99., -0., 99., 0., 99., 2., 99., infinity, 99., nan};
    const double edges[] = {-0., 99., infinity};
    np::histogram_to(strided(narrow, 1, 2), strided(input, 6, 2), strided(edges, 2, 2));
    EXPECT_EQ(narrow[0], 4);
    EXPECT_EQ(narrow[1], 73);
    np::histogram_to(strided(narrow, 1, 2), view<const double>(nullptr, 0), strided(edges, 2, 2));
    EXPECT_EQ(narrow[0], 0);
    EXPECT_EQ(narrow[1], 73);
}

TEST(NpHistogram, InputOverlapAndStridedHoles) {
    const int data[] = {0, -99, 2, -99, 4, -99, 6};
    int counts[] = {-99, -99, -99, -99, -99};
    const auto edges = strided(data, 4, 2);
    np::histogram_to(strided(counts, 3, 2), edges, edges);
    EXPECT_EQ((std::array{counts[0], counts[1], counts[2], counts[3], counts[4]}),
              (std::array{1, -99, 1, -99, 2}));
    EXPECT_EQ(data[1], -99);
    EXPECT_EQ(data[3], -99);
    EXPECT_EQ(data[5], -99);
    np::histogram_to(strided(counts, 3, 2), view<const int>(nullptr, 0), edges);
    EXPECT_EQ((std::array{counts[0], counts[1], counts[2], counts[3], counts[4]}),
              (std::array{0, -99, 0, -99, 0}));
}

TEST(NpHistogram, PaddedRankOneViews) {
    using shape = np::dextents<size_t, 1>;
    using layout = Kokkos::Experimental::layout_left_padded<8>;
    const int edges[] = {0, 2, 4};
    const int values[] = {-1, 0, 1, 2, 3, 4, 5};
    int counts[2];
    np::histogram_to(
        np::tensor_view<int, shape, layout>(counts, typename layout::mapping<shape>(shape(2))),
        np::tensor_view<const int, shape, layout>(values,
                                                  typename layout::mapping<shape>(shape(7))),
        np::tensor_view<const int, shape, layout>(edges,
                                                  typename layout::mapping<shape>(shape(3))));
    EXPECT_EQ(counts[0], 2);
    EXPECT_EQ(counts[1], 3);
}

TEST(NpHistogram, RandomIndependentIntervalOracle) {
    std::mt19937 random(9053);
    for (size_t bins : {size_t{1}, size_t{2}, size_t{17}, size_t{64}}) {
        std::vector<int> edges(bins + 1), values(1024);
        edges[0] = -7;
        for (size_t i = 1; i != edges.size(); ++i)
            edges[i] = edges[i - 1] + static_cast<int>(random() % 3);
        for (auto &value : values)
            value = static_cast<int>(random() % (bins * 3 + 20)) - 10;
        const auto original_edges = edges, original_values = values;
        std::vector<unsigned> counts(bins, 99), expected(bins, 0);
        for (int value : values)
            for (size_t bin = 0; bin != bins; ++bin)
                if (edges[bin] <= value &&
                    (value < edges[bin + 1] || (bin + 1 == bins && value == edges[bin + 1])))
                    ++expected[bin];
        np::histogram_to(view<unsigned>(counts.data(), bins),
                         view<const int>(values.data(), values.size()),
                         view<const int>(edges.data(), edges.size()));
        EXPECT_EQ(counts, expected);
        EXPECT_EQ(edges, original_edges);
        EXPECT_EQ(values, original_values);
    }
}
} // namespace
