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

TEST(div, uint128_operators) {
    const uint128_t dividend(UINT64_MAX, UINT64_MAX);
    const uint128_t divisor(3, 0);
    const uint128_t quotient_value(UINT64_MAX / 3, UINT64_MAX / 3);

    EXPECT_EQ(dividend / divisor, quotient_value);
    EXPECT_EQ(dividend % divisor, uint128_t(0, 0));
    EXPECT_EQ(dividend / uint64_t(3), quotient_value);
    EXPECT_EQ(dividend % uint64_t(3), uint128_t(0, 0));

    const uint128_t wide_divisor(1, 1);
    EXPECT_EQ(uint128_t(0, 1) / wide_divisor, uint128_t(0, 0));
    EXPECT_EQ(uint128_t(0, 1) % wide_divisor, uint128_t(0, 1));
    EXPECT_EQ(uint128_t(1, 1) / wide_divisor, uint128_t(1, 0));
    EXPECT_EQ(uint128_t(1, 1) % wide_divisor, uint128_t(0, 0));

    const uint128_t high_word_divisor(0, 1);
    EXPECT_EQ(dividend / high_word_divisor, uint128_t(UINT64_MAX, 0));
    EXPECT_EQ(dividend % high_word_divisor, uint128_t(UINT64_MAX, 0));

    uint128_t quotient = dividend;
    quotient /= 3U;
    EXPECT_EQ(quotient, quotient_value);

    uint128_t remainder = dividend;
    remainder %= 3U;
    EXPECT_EQ(remainder, uint128_t(0, 0));
}