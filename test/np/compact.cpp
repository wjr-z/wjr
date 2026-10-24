#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/compact.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;
template <class T>
constexpr auto strided(T *data, size_t size, size_t stride) {
    using shape = np::dextents<size_t, 1>;
    return np::tensor_view<T, shape, np::layout_stride>(
        data, np::layout_stride::mapping<shape>(shape(size), std::array<size_t, 1>{stride}));
}
template <class D, class S, class M>
concept compressible = requires(D dst, S src, M mask) { np::compress_to(dst, src, mask); };
template <class D, class S>
concept nonzeroable = requires(D dst, S src) { np::flatnonzero_to(dst, src); };
static_assert(compressible<view<bool>, view<const uint64_t>, view<const float>>);
static_assert(!compressible<view<const int>, view<int>, view<bool>>);
static_assert(!compressible<view<volatile int>, view<int>, view<bool>>);
static_assert(!compressible<view<int>, view<volatile int>, view<bool>>);
static_assert(!compressible<view<int>, view<int>, view<volatile bool>>);
static_assert(!compressible<view<int>, np::tensor_view<int, np::extents<size_t, 2>>,
                            np::tensor_view<bool, np::extents<size_t, 3>>>);
static_assert(nonzeroable<view<uint8_t>, np::tensor_view<const double, np::extents<size_t>>>);
static_assert(!nonzeroable<view<bool>, view<int>>);
static_assert(!nonzeroable<view<const int>, view<int>>);
static_assert(!nonzeroable<view<volatile int>, view<int>>);
static_assert(!nonzeroable<view<float>, view<int>>);
static_assert(!nonzeroable<view<int>, view<volatile int>>);
static_assert(!nonzeroable<decltype(strided(static_cast<int *>(nullptr), 0, 2)), view<int>>);

constexpr bool constants() {
    const int input[] = {0, 4, 5, 0, 6};
    const bool mask[] = {true, false, true, false, true};
    int selected[] = {-99, -99, -99, -99};
    unsigned positions[] = {99, 99, 99, 99};
    return np::compress_to(view<int>(selected, 4), view<const int>(input, 5),
                           view<const bool>(mask, 5)) == 3 &&
           selected[0] == 0 && selected[1] == 5 && selected[2] == 6 && selected[3] == -99 &&
           np::flatnonzero_to(view<unsigned>(positions, 4), view<const int>(input, 5)) == 3 &&
           positions[0] == 1 && positions[1] == 2 && positions[2] == 4 && positions[3] == 99;
}
static_assert(constants());

constexpr bool constant_column_major() {
    const int input[] = {0, 1, 2, 0};
    int positions[2];
    using shape = np::extents<size_t, 2, 2>;
    return np::flatnonzero_to(view<int>(positions, 2),
                              np::tensor_view<const int, shape, np::layout_left>(input)) == 2 &&
           positions[0] == 1 && positions[1] == 2;
}
static_assert(constant_column_major());

TEST(NpCompaction, ConstantsEmptyAndUnwrittenSuffix) {
    EXPECT_TRUE(constants());
    EXPECT_EQ(np::compress_to(view<int>(nullptr, 0), view<const int>(nullptr, 0),
                              view<const bool>(nullptr, 0)),
              0);
    EXPECT_EQ(np::flatnonzero_to(view<size_t>(nullptr, 0), view<const int>(nullptr, 0)), 0);
    const int input[] = {0, 0, 0};
    const double mask[] = {0., -0., 0.};
    EXPECT_EQ(np::compress_to(view<int>(nullptr, 0), view<const int>(input, 3),
                              view<const double>(mask, 3)),
              0);
    EXPECT_EQ(np::flatnonzero_to(view<size_t>(nullptr, 0), view<const int>(input, 3)), 0);
}

TEST(NpCompaction, TruthValuesAndOnlySelectedValuesAreConverted) {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    const double input[] = {nan, 4., 5., inf, 7.};
    const double mask[] = {0., nan, -inf, -0., 1.};
    int selected[] = {-99, -99, -99, -99};
    EXPECT_EQ(np::compress_to(view<int>(selected, 4), view<const double>(input, 5),
                              view<const double>(mask, 5)),
              3);
    EXPECT_EQ((std::array{selected[0], selected[1], selected[2], selected[3]}),
              (std::array{4, 5, 7, -99}));
    const double values[] = {0., -0., nan, inf, -inf};
    int positions[5] = {-99, -99, -99, -99, -99};
    EXPECT_EQ(np::flatnonzero_to(view<int>(positions, 5), view<const double>(values, 5)), 3);
    EXPECT_EQ((std::array{positions[0], positions[1], positions[2], positions[3]}),
              (std::array{2, 3, 4, -99}));
}

TEST(NpCompaction, FullWidthValuesAndNarrowPositions) {
    const uint64_t input[] = {UINT64_MAX, UINT64_MAX - 1, UINT64_MAX - 2};
    const int mask[] = {1, 0, -1};
    uint64_t selected[2];
    EXPECT_EQ(np::compress_to(view<uint64_t>(selected, 2), view<const uint64_t>(input, 3),
                              view<const int>(mask, 3)),
              2);
    EXPECT_EQ(selected[0], UINT64_MAX);
    EXPECT_EQ(selected[1], UINT64_MAX - 2);
    std::array<uint64_t, 256> values{};
    values[255] = UINT64_MAX;
    uint8_t position;
    EXPECT_EQ(np::flatnonzero_to(view<uint8_t>(&position, 1),
                                 view<const uint64_t>(values.data(), values.size())),
              1);
    EXPECT_EQ(position, 255);
}

TEST(NpCompaction, StridesAndPaddingPreserveHoles) {
    const int input[] = {2, -99, 3, -99, 4, -99, 5};
    const int mask[] = {0, -99, -99, 1, -99, -99, 1, -99, -99, 0};
    int selected[] = {-99, -99, -99, -99, -99, -99};
    EXPECT_EQ(np::compress_to(strided(selected, 3, 2), strided(input, 4, 2), strided(mask, 4, 3)),
              2);
    EXPECT_EQ(selected[0], 3);
    EXPECT_EQ(selected[2], 4);
    for (size_t i : {1, 3, 4, 5})
        EXPECT_EQ(selected[i], -99);
    const bool flags[] = {false, true, true, false};
    using shape = np::extents<size_t, 4>;
    using layout = Kokkos::Experimental::layout_right_padded<8>;
    int positions[4] = {-99, -99, -99, -99};
    EXPECT_EQ(np::flatnonzero_to(view<int>(positions, 4),
                                 np::tensor_view<const bool, shape, layout>(flags)),
              2);
    EXPECT_EQ(positions[0], 1);
    EXPECT_EQ(positions[1], 2);
    EXPECT_EQ(positions[2], -99);
    int compressed[4] = {-99, -99, -99, -99};
    EXPECT_EQ(np::compress_to(np::tensor_view<int, shape, layout>(compressed),
                              np::tensor_view<const bool, shape, layout>(flags),
                              np::tensor_view<const bool, shape, layout>(flags)),
              2);
    EXPECT_EQ(compressed[0], 1);
    EXPECT_EQ(compressed[1], 1);
    EXPECT_EQ(compressed[2], -99);
    EXPECT_EQ(np::flatnonzero_to(view<int>(positions, 4), strided(input, 4, 2)), 4);
    EXPECT_EQ((std::array{positions[0], positions[1], positions[2], positions[3]}),
              (std::array{0, 1, 2, 3}));
}

TEST(NpCompaction, RowMajorPositionsForOtherPhysicalLayouts) {
    EXPECT_TRUE(constant_column_major());
    using shape = np::extents<size_t, 2, 3>;
    const int column_major[] = {0, 1, 2, 0, 0, 3};
    int positions[6] = {-99, -99, -99, -99, -99, -99};
    EXPECT_EQ(np::flatnonzero_to(view<int>(positions, 6),
                                 np::tensor_view<const int, shape, np::layout_left>(column_major)),
              3);
    EXPECT_EQ((std::array{positions[0], positions[1], positions[2], positions[3]}),
              (std::array{1, 3, 5, -99}));
    const int strided_values[] = {0, -99, 2, -99, 0, -99, -99, 1, -99, 0, -99, 3};
    const np::layout_stride::mapping<shape> mapping(shape{}, std::array<size_t, 2>{7, 2});
    EXPECT_EQ(np::flatnonzero_to(
                  view<int>(positions, 6),
                  np::tensor_view<const int, shape, np::layout_stride>(strided_values, mapping)),
              3);
    EXPECT_EQ((std::array{positions[0], positions[1], positions[2]}), (std::array{1, 3, 5}));
    const int padded[] = {0, 2, 0, -99, -99, -99, -99, -99, 1, 0, 3};
    using layout = Kokkos::Experimental::layout_right_padded<8>;
    EXPECT_EQ(np::flatnonzero_to(view<int>(positions, 6),
                                 np::tensor_view<const int, shape, layout>(padded)),
              3);
    EXPECT_EQ((std::array{positions[0], positions[1], positions[2]}), (std::array{1, 3, 5}));
}

TEST(NpCompaction, ScalarAndEmptyMultidimensionalViews) {
    using scalar = np::tensor_view<const int, np::extents<size_t>>;
    const int zero = 0, nonzero = 3;
    int index = -99;
    EXPECT_EQ(np::flatnonzero_to(view<int>(nullptr, 0), scalar(&zero)), 0);
    EXPECT_EQ(np::flatnonzero_to(view<int>(&index, 1), scalar(&nonzero)), 1);
    EXPECT_EQ(index, 0);
    using shape = np::dextents<size_t, 2>;
    EXPECT_EQ(np::flatnonzero_to(view<int>(nullptr, 0),
                                 np::tensor_view<const int, shape, np::layout_left>(nullptr, 0, 3)),
              0);
}

TEST(NpCompaction, RandomInputsMatchIndependentOracle) {
    std::mt19937 engine(8041);
    for (size_t count : {0, 1, 17, 1024}) {
        std::vector<int> input(count), mask(count);
        std::vector<int> actual(count, -99), positions(count, -99), expected, expected_positions;
        for (size_t i = 0; i != count; ++i) {
            input[i] = static_cast<int>(engine() % 5) - 2;
            mask[i] = static_cast<int>(engine() % 3) - 1;
            if (mask[i] != 0)
                expected.push_back(input[i]);
            if (input[i] != 0)
                expected_positions.push_back(static_cast<int>(i));
        }
        const auto written =
            np::compress_to(view<int>(actual.data(), count), view<const int>(input.data(), count),
                            view<const int>(mask.data(), count));
        const auto found = np::flatnonzero_to(view<int>(positions.data(), count),
                                              view<const int>(input.data(), count));
        EXPECT_EQ(written, expected.size());
        EXPECT_EQ(found, expected_positions.size());
        expected.resize(count, -99);
        expected_positions.resize(count, -99);
        EXPECT_EQ(actual, expected);
        EXPECT_EQ(positions, expected_positions);
    }
}
} // namespace
