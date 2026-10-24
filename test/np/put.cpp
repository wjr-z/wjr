#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <random>

#include <gtest/gtest.h>

#include <wjr/np/put.hpp>

namespace {
namespace np = wjr::np;
template <class T, size_t N>
using view = np::tensor_view<T, np::extents<size_t, N>>;

template <class D, class I, class V>
concept puts = requires(D dst, I indices, V values) { np::put_to(dst, indices, values); };
static_assert(puts<view<int, 5>, view<const unsigned, 3>, view<const double, 3>>);
static_assert(!puts<view<const int, 5>, view<int, 3>, view<int, 3>>);
static_assert(!puts<view<volatile int, 5>, view<int, 3>, view<int, 3>>);
static_assert(!puts<view<int, 5>, view<bool, 3>, view<int, 3>>);
static_assert(!puts<view<int, 5>, view<float, 3>, view<int, 3>>);
static_assert(!puts<view<int, 5>, view<volatile int, 3>, view<int, 3>>);
static_assert(!puts<view<int, 5>, view<int, 3>, view<volatile int, 3>>);
static_assert(!puts<view<int, 5>, view<int, 3>, view<int, 2>>);
using matrix = np::tensor_view<int, np::extents<size_t, 2, 2>>;
static_assert(!puts<matrix, view<int, 3>, view<int, 3>>);
static_assert(!puts<view<int, 5>, matrix, matrix>);

constexpr bool constant_put() {
    int destination[] = {-1, -1, -1, -1, -1};
    const uint8_t indices[] = {3, 1, 3, 0};
    const int values[] = {10, 20, 30, 40};
    np::put_to(view<int, 5>(destination), view<const uint8_t, 4>(indices),
               view<const int, 4>(values));
    return destination[0] == 40 && destination[1] == 20 && destination[2] == -1 &&
           destination[3] == 30 && destination[4] == -1;
}
static_assert(constant_put());

TEST(NpPut, RepeatedIndicesKeepLastValueAndUntouchedEntries) { EXPECT_TRUE(constant_put()); }

TEST(NpPut, EmptyInputsAllowNullHandles) {
    np::put_to(view<int, 0>(nullptr), view<const int, 0>(nullptr), view<const int, 0>(nullptr));
    int output[] = {7, 8};
    np::put_to(view<int, 2>(output), view<const int, 0>(nullptr), view<const int, 0>(nullptr));
    EXPECT_EQ((std::array{output[0], output[1]}), (std::array{7, 8}));
}

TEST(NpPut, StridedViewsPreserveHolesAndUnaddressedEntries) {
    using shape = np::extents<size_t, 4>;
    const np::layout_stride::mapping<shape> output_mapping(shape{}, std::array<size_t, 1>{3});
    const np::layout_stride::mapping<shape> index_mapping(shape{}, std::array<size_t, 1>{2});
    const np::layout_stride::mapping<shape> value_mapping(shape{}, std::array<size_t, 1>{4});
    int output[10];
    std::fill_n(output, 10, -99);
    const int indices[] = {3, -1, 0, -1, 3, -1, 1};
    const int values[] = {10, -2, -2, -2, 20, -2, -2, -2, 30, -2, -2, -2, 40};
    np::put_to(np::tensor_view<int, shape, np::layout_stride>(output, output_mapping),
               np::tensor_view<const int, shape, np::layout_stride>(indices, index_mapping),
               np::tensor_view<const int, shape, np::layout_stride>(values, value_mapping));
    EXPECT_EQ((std::array{output[0], output[3], output[6], output[9]}),
              (std::array{20, 40, -99, 30}));
    for (size_t hole : {1, 2, 4, 5, 7, 8})
        EXPECT_EQ(output[hole], -99);
}

TEST(NpPut, PaddedAndColumnMajorRankOneLayouts) {
    using shape = np::extents<size_t, 3>;
    using padded = Kokkos::Experimental::layout_right_padded<8>;
    int output[] = {-1, -1, -1};
    const int indices[] = {2, 0, 2};
    const int values[] = {7, 8, 9};
    np::put_to(np::tensor_view<int, shape, padded>(output),
               np::tensor_view<const int, shape, np::layout_left>(indices),
               np::tensor_view<const int, shape, padded>(values));
    EXPECT_EQ((std::array{output[0], output[1], output[2]}), (std::array{8, -1, 9}));
}

TEST(NpPut, ReadOnlyInputsMayAlias) {
    const unsigned data[] = {2, 0, 2, 1};
    unsigned output[3]{};
    np::put_to(view<unsigned, 3>(output), view<const unsigned, 4>(data),
               view<const unsigned, 4>(data));
    EXPECT_EQ((std::array{output[0], output[1], output[2]}), (std::array{0u, 1u, 2u}));
}

TEST(NpPut, WideIndicesAndExactIntegerAndBooleanConversions) {
    const uint64_t indices[] = {2, 0, 1};
    const uint64_t values[] = {std::numeric_limits<uint64_t>::max(), 0, uint64_t{1} << 63};
    uint64_t output[3];
    np::put_to(view<uint64_t, 3>(output), view<const uint64_t, 3>(indices),
               view<const uint64_t, 3>(values));
    EXPECT_EQ(output[2], values[0]);
    EXPECT_EQ(output[1], values[2]);
    bool flags[3];
    np::put_to(view<bool, 3>(flags), view<const uint64_t, 3>(indices),
               view<const uint64_t, 3>(values));
    EXPECT_FALSE(flags[0]);
    EXPECT_TRUE(flags[1]);
    EXPECT_TRUE(flags[2]);
    const double floats[] = {2.75, -3.5, 9.0};
    int integers[3];
    np::put_to(view<int, 3>(integers), view<const uint64_t, 3>(indices),
               view<const double, 3>(floats));
    EXPECT_EQ((std::array{integers[0], integers[1], integers[2]}), (std::array{-3, 9, 2}));
}

TEST(NpPut, RandomScatterMatchesOrderedDirectWrites) {
    constexpr size_t destination_count = 67;
    constexpr size_t count = 257;
    std::array<int, destination_count> output, expected;
    output.fill(-1);
    expected.fill(-1);
    std::array<uint16_t, count> indices;
    std::array<int, count> values;
    std::mt19937 engine(17);
    for (size_t i = 0; i != count; ++i) {
        indices[i] = static_cast<uint16_t>(engine() % destination_count);
        values[i] = static_cast<int>(i);
        expected[indices[i]] = values[i];
    }
    np::put_to(view<int, destination_count>(output.data()),
               view<const uint16_t, count>(indices.data()), view<const int, count>(values.data()));
    EXPECT_EQ(output, expected);
}
} // namespace
