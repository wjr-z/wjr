#include <gtest/gtest.h>

#include <wjr/math/bit.hpp>

using namespace wjr;

namespace {

template <typename T>
void expect_runtime_counts(T value, int leading_zero, int trailing_zero, int leading_one,
                           int trailing_one) {
    EXPECT_EQ(countl_zero(value), leading_zero);
    EXPECT_EQ(countr_zero(value), trailing_zero);
    EXPECT_EQ(countl_one(value), leading_one);
    EXPECT_EQ(countr_one(value), trailing_one);
}

template <typename T>
void expect_constant_counts(T value, int leading_zero, int trailing_zero, int leading_one,
                            int trailing_one) {
    EXPECT_EQ(constant::countl_zero(value), leading_zero);
    EXPECT_EQ(constant::countr_zero(value), trailing_zero);
    EXPECT_EQ(constant::countl_one(value), leading_one);
    EXPECT_EQ(constant::countr_one(value), trailing_one);
}

template <typename T>
void expect_bit_rounding(T value, int width, T ceil, T floor) {
    EXPECT_EQ(constant::bit_width(value), width);
    EXPECT_EQ(constant::bit_ceil(value), ceil);
    EXPECT_EQ(constant::bit_floor(value), floor);
    EXPECT_EQ(bit_width(value), width);
    EXPECT_EQ(bit_ceil(value), ceil);
    EXPECT_EQ(bit_floor(value), floor);
}

template <typename T>
void expect_bits(T value, int leading_zero, int trailing_zero, int leading_one, int trailing_one,
                 int width, T ceil, T floor) {
    expect_runtime_counts(value, leading_zero, trailing_zero, leading_one, trailing_one);
    expect_constant_counts(value, leading_zero, trailing_zero, leading_one, trailing_one);
    expect_bit_rounding(value, width, ceil, floor);
}

} // namespace

TEST(bit, uint8_boundaries) {
    expect_bits<uint8_t>(0, 8, 8, 0, 0, 0, 1, 0);
    expect_bits<uint8_t>(1, 7, 0, 0, 1, 1, 1, 1);
    expect_bits<uint8_t>(0x0f, 4, 0, 0, 4, 4, 0x10, 0x08);
    expect_bits<uint8_t>(0x80, 0, 7, 1, 0, 8, 0x80, 0x80);
}

TEST(bit, wider_values) {
    expect_bits<uint32_t>(0x0001'0000U, 15, 16, 0, 0, 17, 0x0001'0000U, 0x0001'0000U);
    expect_bits<uint64_t>(0x8000'0000'0000'0000ULL, 0, 63, 1, 0, 64, 0x8000'0000'0000'0000ULL,
                          0x8000'0000'0000'0000ULL);
}

TEST(bit, single_bit) {
    EXPECT_FALSE(has_single_bit(uint8_t(0)));
    EXPECT_TRUE(has_single_bit(uint8_t(1)));
    EXPECT_FALSE(has_single_bit(uint8_t(3)));
    EXPECT_TRUE(has_single_bit(uint8_t(0x80)));
    EXPECT_FALSE(has_single_bit(uint8_t(0xff)));
}