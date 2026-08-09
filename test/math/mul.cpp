#include <gtest/gtest.h>

#include <limits>

#include <wjr/math/mul.hpp>

using namespace wjr;

namespace {

template <typename T>
void expect_product(T lhs, T rhs, T expected_low, T expected_high) {
    T high = 0;
    EXPECT_EQ(mul(lhs, rhs, high), expected_low);
    EXPECT_EQ(high, expected_high);
    EXPECT_EQ(mulhi(lhs, rhs), expected_high);
    EXPECT_EQ(mullo(lhs, rhs), expected_low);
}

} // namespace

TEST(mul, full_product) {
    expect_product<uint8_t>(0xff, 2, 0xfe, 1);
    expect_product<uint32_t>(0xffff'ffffU, 2, 0xffff'fffeU, 1);
    expect_product<uint64_t>(UINT64_MAX, 2, UINT64_MAX - 1, 1);

    uint64_t high = 0;
    EXPECT_EQ(fallback_mul64(UINT64_MAX, 2, high), UINT64_MAX - 1);
    EXPECT_EQ(high, 1);
}

TEST(mul, overflow) {
    uint32_t result = 0;

    EXPECT_FALSE(mul_overflow(uint32_t(3), uint32_t(7), result));
    EXPECT_EQ(result, 21U);
    EXPECT_TRUE(mul_overflow(std::numeric_limits<uint32_t>::max(), uint32_t(2), result));
    EXPECT_EQ(result, std::numeric_limits<uint32_t>::max() - 1);

    EXPECT_FALSE(fallback_mul_overflow(uint32_t(3), uint32_t(7), result));
    EXPECT_TRUE(fallback_mul_overflow(std::numeric_limits<uint32_t>::max(), uint32_t(2), result));
}