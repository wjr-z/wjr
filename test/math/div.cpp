#include <gtest/gtest.h>

#include <wjr/math/div.hpp>

using namespace wjr;

TEST(div, div128by128_noshift) {
    constexpr uint64_t high_bit = UINT64_C(0x8000000000000000);
    const div3by2_divider<uint64_t> divider(1, high_bit);
    uint128_t remainder(0, 0);

    EXPECT_EQ(div128by128to64_noshift(remainder, 1, high_bit, divider), 1);
    EXPECT_EQ(remainder, uint128_t(0, 0));

    EXPECT_EQ(div128by128to64(remainder, 0, high_bit, divider), 0);
    EXPECT_EQ(remainder, uint128_t(0, high_bit));

    EXPECT_EQ(div128by128to64(remainder, 2, high_bit, divider), 1);
    EXPECT_EQ(remainder, uint128_t(1, 0));
}

TEST(div, div128by128_shift) {
    const div3by2_divider<uint64_t> divider(3, 1);
    uint128_t remainder(0, 0);

    EXPECT_EQ(divider.get_shift(), 63);
    EXPECT_EQ(div128by128to64_shift(remainder, 3, 1, divider), 1);
    EXPECT_EQ(remainder, uint128_t(0, 0));

    EXPECT_EQ(div128by128to64(remainder, 5, 2, divider), 1);
    EXPECT_EQ(remainder, uint128_t(2, 1));
}