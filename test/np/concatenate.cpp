#include <algorithm>
#include <array>
#include <cstdint>

#include <gtest/gtest.h>

#include <wjr/np/concatenate.hpp>

namespace {
namespace np = wjr::np;
template <class T, size_t... N>
using view = np::tensor_view<T, np::extents<size_t, N...>>;
template <size_t Axis, class D, class... S>
concept concatenates =
    requires(D dst, S... src) { np::concatenate_to(dst, np::axes<Axis>, src...); };
static_assert(concatenates<0, view<int, 5>, view<const int, 2>, view<int, 3>>);
static_assert(!concatenates<0, view<int, 5>>);
static_assert(!concatenates<1, view<int, 5>, view<int, 5>>);
static_assert(!concatenates<0, view<const int, 5>, view<int, 5>>);
static_assert(!concatenates<0, view<int, 5>, view<volatile int, 5>>);
static_assert(!concatenates<0, view<int, 5>, view<float, 5>>);
static_assert(!concatenates<0, view<int, 2, 3>, view<int, 2, 4>>);
static_assert(!concatenates<0, view<int, 2, 3>, view<int, 6>>);
static_assert(!concatenates<0, view<int>, view<int>>);

constexpr bool constant_concatenate() {
    const int left[] = {1, 2, 3, 4};
    const int right[] = {5, 6};
    int output[6]{};
    np::concatenate_to(view<int, 2, 3>(output), np::axes<1>, view<const int, 2, 2>(left),
                       view<const int, 2, 1>(right));
    return output[0] == 1 && output[1] == 2 && output[2] == 5 && output[3] == 3 && output[4] == 4 &&
           output[5] == 6;
}
static_assert(constant_concatenate());

TEST(NpConcatenate, PackedOuterAndInnerAxesAndConstantEvaluation) {
    EXPECT_TRUE(constant_concatenate());
    const int first[] = {1, 2, 3, 4};
    const int second[] = {5, 6};
    int output[6];
    np::concatenate_to(view<int, 3, 2>(output), np::axes<0>, view<const int, 2, 2>(first),
                       view<const int, 1, 2>(second));
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3], output[4], output[5]}),
              (std::array{1, 2, 3, 4, 5, 6}));
}

TEST(NpConcatenate, RankOneManySourcesAndReadOnlyOverlap) {
    const uint64_t input[] = {uint64_t{1} << 63, 7, 9};
    uint64_t output[7];
    np::concatenate_to(view<uint64_t, 7>(output), np::axes<0>, view<const uint64_t, 3>(input),
                       view<const uint64_t, 1>(input + 1), view<const uint64_t, 3>(input));
    EXPECT_EQ(
        (std::array{output[0], output[1], output[2], output[3], output[4], output[5], output[6]}),
        (std::array{input[0], uint64_t{7}, uint64_t{9}, uint64_t{7}, input[0], uint64_t{7},
                    uint64_t{9}}));
}

TEST(NpConcatenate, EmptySourcesAndZeroNonAxisExtentNeverAccessNull) {
    const int input[] = {7, 8};
    int output[2];
    np::concatenate_to(view<int, 2>(output), np::axes<0>, view<const int, 0>(nullptr),
                       view<const int, 2>(input), view<const int, 0>(nullptr));
    EXPECT_EQ((std::array{output[0], output[1]}), (std::array{7, 8}));
    np::concatenate_to(view<int, 0>(nullptr), np::axes<0>, view<const int, 0>(nullptr));
    np::concatenate_to(view<int, 4, 0>(nullptr), np::axes<0>, view<const int, 1, 0>(nullptr),
                       view<const int, 3, 0>(nullptr));
}

TEST(NpConcatenate, ColumnMajorAndMixedLayoutsFollowLogicalCoordinates) {
    using left = np::layout_left;
    using a = np::extents<size_t, 2, 2>;
    using b = np::extents<size_t, 2, 1>;
    using d = np::extents<size_t, 2, 3>;
    const int first[] = {1, 3, 2, 4};
    const int second[] = {5, 6};
    int output[6];
    const np::tensor_view<int, d, left> dst(output);
    const np::tensor_view<const int, a, left> src(first);
    np::concatenate_to(dst, np::axes<1>, src, np::tensor_view<const int, b, left>(second));
    for (size_t r = 0; r != 2; ++r) {
        EXPECT_EQ(dst(r, 0), src(r, 0));
        EXPECT_EQ(dst(r, 1), src(r, 1));
        EXPECT_EQ(dst(r, 2), second[r]);
    }
    int mixed[6];
    const np::tensor_view<int, d> mixed_dst(mixed);
    np::concatenate_to(mixed_dst, np::axes<1>, src, np::tensor_view<const int, b>(second));
    for (size_t r = 0; r != 2; ++r)
        for (size_t c = 0; c != 3; ++c)
            EXPECT_EQ(mixed_dst(r, c), dst(r, c));
}

TEST(NpConcatenate, PositiveStridesAndPaddedOutputPreserveHoles) {
    using src_shape = np::extents<size_t, 2, 2>;
    using dst_shape = np::extents<size_t, 2, 4>;
    using padded = Kokkos::Experimental::layout_right_padded<8>;
    const np::layout_stride::mapping<src_shape> mapping(src_shape{}, std::array<size_t, 2>{7, 2});
    const int first[] = {1, -9, 2, -9, -9, -9, -9, 3, -9, 4};
    const int second[] = {5, 6, 7, 8};
    int output[12];
    std::fill_n(output, 12, -99);
    const np::tensor_view<int, dst_shape, padded> dst(output);
    np::concatenate_to(dst, np::axes<1>,
                       np::tensor_view<const int, src_shape, np::layout_stride>(first, mapping),
                       view<const int, 2, 2>(second));
    EXPECT_EQ((std::array{dst(0, 0), dst(0, 1), dst(0, 2), dst(0, 3)}), (std::array{1, 2, 5, 6}));
    EXPECT_EQ((std::array{dst(1, 0), dst(1, 1), dst(1, 2), dst(1, 3)}), (std::array{3, 4, 7, 8}));
    for (size_t hole : {4, 5, 6, 7})
        EXPECT_EQ(output[hole], -99);
}

TEST(NpConcatenate, DynamicThreeDimensionalMiddleAxis) {
    using shape = np::dextents<size_t, 3>;
    const int first[] = {1, 2, 3, 4};
    const int second[] = {5, 6, 7, 8, 9, 10, 11, 12};
    int output[12];
    np::concatenate_to(np::tensor_view<int, shape>(output, 2, 3, 2), np::axes<1>,
                       np::tensor_view<const int, shape>(first, 2, 1, 2),
                       np::tensor_view<const int, shape>(second, 2, 2, 2));
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3], output[4], output[5],
                          output[6], output[7], output[8], output[9], output[10], output[11]}),
              (std::array{1, 2, 5, 6, 7, 8, 3, 4, 9, 10, 11, 12}));
}

TEST(NpConcatenate, BooleanStorageAndRankOneLayoutMix) {
    bool output[3];
    const bool first[] = {true, false};
    const bool second[] = {true};
    np::concatenate_to(view<bool, 3>(output), np::axes<0>,
                       np::tensor_view<const bool, np::extents<size_t, 2>, np::layout_left>(first),
                       view<const bool, 1>(second));
    EXPECT_TRUE(output[0]);
    EXPECT_FALSE(output[1]);
    EXPECT_TRUE(output[2]);
}

TEST(NpConcatenate, StridedDestinationAndEmptyInnerAxisInput) {
    using shape = np::extents<size_t, 2, 3>;
    const np::layout_stride::mapping<shape> mapping(shape{}, std::array<size_t, 2>{9, 2});
    int output[14];
    std::fill_n(output, 14, -99);
    const int first[] = {1, 2, 3, 4};
    const int second[] = {5, 6};
    const np::tensor_view<int, shape, np::layout_stride> dst(output, mapping);
    np::concatenate_to(dst, np::axes<1>, view<const int, 2, 0>(nullptr),
                       view<const int, 2, 2>(first), view<const int, 2, 0>(nullptr),
                       view<const int, 2, 1>(second));
    EXPECT_EQ((std::array{dst(0, 0), dst(0, 1), dst(0, 2), dst(1, 0), dst(1, 1), dst(1, 2)}),
              (std::array{1, 2, 5, 3, 4, 6}));
    for (size_t hole : {1, 3, 5, 6, 7, 8, 10, 12})
        EXPECT_EQ(output[hole], -99);
}
} // namespace
