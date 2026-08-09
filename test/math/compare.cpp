#include "../detail.hpp"

#include <wjr/math/compare.hpp>

using namespace wjr;

TEST(compare, forward_and_reverse_arrays) {
    const uint64_t left[] = {1, 2, 3, 4};
    const uint64_t right[] = {1, 2, 3, 5};
    const uint64_t greater[] = {1, 2, 4, 0};

    EXPECT_EQ(compare_n(left, left, 4), 0);
    EXPECT_EQ(compare_n(left, right, 0), 0);
    EXPECT_EQ(compare_n(left, right, 4), -1);
    EXPECT_EQ(compare_n(right, left, 4), 1);
    EXPECT_EQ(compare_n(left, greater, 4), -1);

    EXPECT_EQ(reverse_compare_n(left, left, 4), 0);
    EXPECT_EQ(reverse_compare_n(left, right, 4), -1);
    EXPECT_EQ(reverse_compare_n(right, left, 4), 1);
    EXPECT_EQ(reverse_compare_n(left, greater, 4), 1);

    const int signed_left[] = {-2, 0, 3};
    const int signed_right[] = {-1, 0, 3};
    EXPECT_EQ(compare_n(signed_left, signed_right, 3), -1);
    EXPECT_EQ(reverse_compare_n(signed_left, signed_right, 3), -1);
}

TEST(compare, compare_128_helpers) {
    constexpr uint64_t low = 0x1234;
    constexpr uint64_t high = 0x5678;

    EXPECT_TRUE(_equal_128(low, high, low, high));
    EXPECT_FALSE(_equal_128(low, high, low, high + 1));
    EXPECT_TRUE(_not_equal_128(low, high, low, high + 1));

    EXPECT_TRUE(_less_128(low, high, low + 1, high));
    EXPECT_TRUE(_less_equal_128(low, high, low, high));
    EXPECT_TRUE(_greater_128(low + 1, high, low, high));
    EXPECT_TRUE(_greater_equal_128(low, high, low, high));
    EXPECT_FALSE(_less_128(low, high + 1, low, high));

    EXPECT_TRUE(_equal_128_64(low, 0, low));
    EXPECT_FALSE(_equal_128_64(low, 1, low));
    EXPECT_TRUE(_not_equal_128_64(low, 1, low));
    EXPECT_TRUE(_less_128_64(low, 0, low + 1));
    EXPECT_TRUE(_less_equal_128_64(low, 0, low));
    EXPECT_TRUE(_greater_128_64(low + 1, 0, low));
    EXPECT_TRUE(_greater_equal_128_64(low, 0, low));
}
