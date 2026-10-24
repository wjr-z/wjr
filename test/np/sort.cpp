#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include <wjr/np/sort.hpp>

namespace {
namespace np = wjr::np;
template <class T>
using view = np::tensor_view<T, np::dextents<size_t, 1>>;
template <class V>
concept sortable = requires(V v) { np::sort_inplace(v); };
template <class D, class S>
concept copy_sortable = requires(D d, S s) { np::sort_to(d, s); };
static_assert(sortable<view<int>>);
static_assert(!sortable<view<const int>>);
static_assert(!sortable<view<volatile int>>);
static_assert(!sortable<np::tensor_view<int, np::extents<size_t, 2, 2>>>);
static_assert(!sortable<np::tensor_view<int, np::dextents<size_t, 1>, np::layout_stride>>);
static_assert(copy_sortable<view<int>, view<const int>>);
static_assert(!copy_sortable<view<double>, view<int>>);
static_assert(!copy_sortable<view<int>, view<volatile int>>);
static_assert(!copy_sortable<np::tensor_view<int, np::extents<size_t, 3>>,
                             np::tensor_view<int, np::extents<size_t, 4>>>);

constexpr bool constants() {
    int data[] = {4, 1, 3, 2};
    np::sort_inplace(view<int>(data, 4));
    int reversed[4];
    np::sort_to<np::sort_order::descending>(view<int>(reversed, 4), view<const int>(data, 4));
    return data[0] == 1 && data[3] == 4 && reversed[0] == 4 && reversed[3] == 1;
}
static_assert(constants());

TEST(NpSort, ConstantsEmptyAndSingleton) {
    EXPECT_TRUE(constants());
    np::sort_inplace(view<int>(nullptr, 0));
    np::sort_to(view<int>(nullptr, 0), view<const int>(nullptr, 0));
    const int one = 17;
    int output = 0;
    np::sort_to(view<int>(&output, 1), view<const int>(&one, 1));
    np::sort_inplace(view<int>(&output, 1));
    EXPECT_EQ(output, 17);
}

TEST(NpSort, StridedSourceAndPaddedOneDimensionalMapping) {
    using shape = np::extents<size_t, 4>;
    const int input[] = {4, -99, 1, -99, 3, -99, 2};
    int output[4];
    const np::layout_stride::mapping<shape> mapping(shape{}, std::array<size_t, 1>{2});
    np::sort_to(np::tensor_view<int, shape, np::layout_left>(output),
                np::tensor_view<const int, shape, np::layout_stride>(input, mapping));
    EXPECT_EQ(std::to_array(output), (std::array{1, 2, 3, 4}));
    EXPECT_EQ(std::to_array(input), (std::array{4, -99, 1, -99, 3, -99, 2}));
    using padded = Kokkos::Experimental::layout_right_padded<8>;
    const int values[] = {3, 1, 4, 2};
    np::sort_to<np::sort_order::descending>(np::tensor_view<int, shape>(output),
                                            np::tensor_view<const int, shape, padded>(values));
    EXPECT_EQ(std::to_array(output), (std::array{4, 3, 2, 1}));
}

template <np::sort_order Order>
void check_special_values() {
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    const auto inf = std::numeric_limits<double>::infinity();
    double data[] = {nan, 1, -0., inf, nan, -1, 0., -inf};
    np::sort_inplace<Order>(view<double>(data, 8));
    EXPECT_TRUE(std::isnan(data[6]));
    EXPECT_TRUE(std::isnan(data[7]));
    EXPECT_EQ(data[0], Order == np::sort_order::ascending ? -inf : inf);
    EXPECT_EQ(data[5], Order == np::sort_order::ascending ? inf : -inf);
    EXPECT_EQ(data[2], 0.);
    EXPECT_EQ(data[3], 0.);
    EXPECT_NE(std::signbit(data[2]), std::signbit(data[3]));
    for (size_t i = 1; i != 6; ++i)
        if constexpr (Order == np::sort_order::ascending)
            EXPECT_LE(data[i - 1], data[i]);
        else
            EXPECT_GE(data[i - 1], data[i]);
}

TEST(NpSort, NaNsInfinitiesAndSignedZeros) {
    check_special_values<np::sort_order::ascending>();
    check_special_values<np::sort_order::descending>();
}

TEST(NpSort, FullWidthIntegerAndRealBoolStorage) {
    constexpr auto max = std::numeric_limits<uint64_t>::max();
    uint64_t data[] = {max, max - 2, 0, max - 1};
    np::sort_inplace(view<uint64_t>(data, 4));
    EXPECT_EQ(std::to_array(data), (std::array<uint64_t, 4>{0, max - 2, max - 1, max}));
    bool flags[] = {true, false, false, true};
    np::sort_inplace(view<bool>(flags, 4));
    EXPECT_EQ(std::to_array(flags), (std::array{false, false, true, true}));
}

TEST(NpSort, RandomValuesMatchStandardSortBothDirections) {
    std::mt19937 engine(551);
    for (size_t count : {0, 1, 17, 1024}) {
        std::vector<int> source(count), output(count);
        for (auto &value : source)
            value = static_cast<int>(engine() % 37) - 18;
        auto expected = source;
        std::sort(expected.begin(), expected.end());
        np::sort_to(view<int>(output.data(), count), view<const int>(source.data(), count));
        EXPECT_EQ(output, expected);
        std::sort(expected.begin(), expected.end(), std::greater<>{});
        np::sort_inplace<np::sort_order::descending>(view<int>(output.data(), count));
        EXPECT_EQ(output, expected);
    }
}
} // namespace
