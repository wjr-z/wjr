#include <array>
#include <cstdint>
#include <limits>
#include <random>

#include <gtest/gtest.h>

#include <wjr/np/take.hpp>

namespace {
namespace np = wjr::np;
template <class T, size_t N>
using view = np::tensor_view<T, np::extents<size_t, N>>;
template <class D, class S, class I>
concept takes = requires(D dst, S src, I indices) { np::take_to(dst, src, indices); };
static_assert(takes<view<int, 3>, view<const int, 4>, view<const unsigned, 3>>);
static_assert(!takes<view<const int, 3>, view<int, 4>, view<int, 3>>);
static_assert(!takes<view<int, 3>, view<volatile int, 4>, view<int, 3>>);
static_assert(!takes<view<int, 3>, view<int, 4>, view<bool, 3>>);
static_assert(!takes<view<int, 3>, view<int, 4>, view<float, 3>>);
static_assert(!takes<view<int, 3>, view<int, 4>, view<volatile int, 3>>);
static_assert(!takes<view<int, 3>, view<int, 4>, view<int, 2>>);
static_assert(!takes<view<int, 3>, np::tensor_view<int, np::extents<size_t, 2, 2>>, view<int, 3>>);

constexpr bool constant_take() {
    const int source[] = {10, 20, 30, 40};
    const uint8_t indices[] = {3, 1, 1, 0};
    int output[4]{};
    np::take_to(view<int, 4>(output), view<const int, 4>(source), view<const uint8_t, 4>(indices));
    return output[0] == 40 && output[1] == 20 && output[2] == 20 && output[3] == 10;
}
static_assert(constant_take());

TEST(np_take, PackedRepeatedIndicesAndConstantEvaluation) { EXPECT_TRUE(constant_take()); }

TEST(np_take, EmptyOutputDoesNotReadInputs) {
    np::take_to(view<int, 0>(nullptr), view<const int, 0>(nullptr), view<const int, 0>(nullptr));
    using shape = np::extents<size_t, 0, 3>;
    np::take_to(np::tensor_view<int, shape>(nullptr), view<const int, 0>(nullptr),
                np::tensor_view<const int, shape>(nullptr));
}

TEST(np_take, ScalarIndexAndNumericConversions) {
    const double source[] = {1.5, -2.5, 8.0};
    const int index = 1;
    double output = 0;
    np::take_to(np::tensor_view<double, np::extents<size_t>>(&output),
                view<const double, 3>(source),
                np::tensor_view<const int, np::extents<size_t>>(&index));
    EXPECT_EQ(output, -2.5);
    const bool booleans[] = {true, false};
    const int indices[] = {1, 0, 0};
    int integers[3];
    np::take_to(view<int, 3>(integers), view<const bool, 2>(booleans), view<const int, 3>(indices));
    EXPECT_EQ((std::array{integers[0], integers[1], integers[2]}), (std::array{0, 1, 1}));
    bool result[3];
    const uint64_t wide[] = {0, std::numeric_limits<uint64_t>::max()};
    np::take_to(view<bool, 3>(result), view<const uint64_t, 2>(wide), view<const int, 3>(indices));
    EXPECT_TRUE(result[0]);
    EXPECT_FALSE(result[1]);
    uint64_t exact[3];
    np::take_to(view<uint64_t, 3>(exact), view<const uint64_t, 2>(wide),
                view<const int, 3>(indices));
    EXPECT_EQ(exact[0], wide[1]);
}

TEST(np_take, StridedSourceAndMixedOutputLayoutsPreserveHoles) {
    using shape = np::extents<size_t, 2, 3>;
    const int source[] = {10, -1, 20, -1, 30, -1, 40};
    const np::layout_stride::mapping<np::extents<size_t, 4>> source_mapping(
        np::extents<size_t, 4>{}, std::array<size_t, 1>{2});
    const int indices[] = {3, 0, 1, 2, 1, 3};
    const np::tensor_view<const int, shape, np::layout_left> index_view(indices);
    int output[12];
    std::fill_n(output, 12, -99);
    const np::layout_stride::mapping<shape> mapping(shape{}, std::array<size_t, 2>{7, 2});
    const np::tensor_view<int, shape, np::layout_stride> destination(output, mapping);
    np::take_to(destination,
                np::tensor_view<const int, np::extents<size_t, 4>, np::layout_stride>(
                    source, source_mapping),
                index_view);
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 3; ++col)
            EXPECT_EQ(destination(row, col), source[index_view(row, col) * 2]);
    for (size_t hole : {1, 3, 5, 6, 8, 10})
        EXPECT_EQ(output[hole], -99);
}

TEST(np_take, StridedIndicesAndOutput) {
    const int source[] = {10, 20, 30};
    const unsigned indices[] = {2, 99, 0, 99, 1};
    int output[] = {-1, -1, -1, -1, -1};
    using shape = np::extents<size_t, 3>;
    const np::layout_stride::mapping<shape> mapping(shape{}, std::array<size_t, 1>{2});
    np::take_to(np::tensor_view<int, shape, np::layout_stride>(output, mapping),
                view<const int, 3>(source),
                np::tensor_view<const unsigned, shape, np::layout_stride>(indices, mapping));
    EXPECT_EQ((std::array{output[0], output[1], output[2], output[3], output[4]}),
              (std::array{30, -1, 10, -1, 20}));
}

TEST(np_take, PackedColumnMajorAndPaddedOutput) {
    using shape = np::extents<size_t, 2, 3>;
    const int source[] = {10, 20, 30, 40};
    const unsigned indices[] = {3, 0, 1, 2, 1, 3};
    int output[6];
    const np::tensor_view<const unsigned, shape, np::layout_left> index_view(indices);
    np::take_to(np::tensor_view<int, shape, np::layout_left>(output), view<const int, 4>(source),
                index_view);
    for (size_t i = 0; i != 6; ++i)
        EXPECT_EQ(output[i], source[indices[i]]);
    using padded = Kokkos::Experimental::layout_right_padded<4>;
    int padded_data[8];
    std::fill_n(padded_data, 8, -99);
    const np::tensor_view<int, shape, padded> destination(padded_data);
    np::take_to(destination, view<const int, 4>(source), index_view);
    for (size_t row = 0; row != 2; ++row)
        for (size_t col = 0; col != 3; ++col)
            EXPECT_EQ(destination(row, col), source[index_view(row, col)]);
    EXPECT_EQ(padded_data[3], -99);
    EXPECT_EQ(padded_data[7], -99);
}

TEST(np_take, RandomGatherMatchesDirectIndexing) {
    constexpr size_t count = 257;
    std::array<int, count> source, output;
    std::array<unsigned, count> indices;
    std::mt19937 engine(17);
    for (size_t i = 0; i != count; ++i) {
        source[i] = static_cast<int>(i * 7);
        indices[i] = engine() % count;
    }
    np::take_to(view<int, count>(output.data()), view<const int, count>(source.data()),
                view<const unsigned, count>(indices.data()));
    for (size_t i = 0; i != count; ++i)
        EXPECT_EQ(output[i], source[indices[i]]);
}
} // namespace
