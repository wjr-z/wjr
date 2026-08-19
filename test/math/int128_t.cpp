#include "../detail.hpp"

#include <limits>

#include <wjr/math.hpp>

using namespace wjr;

namespace {

void expect_bits(const int128_t &value, uint64_t low, uint64_t high) {
    EXPECT_EQ(value.bits().low, low);
    EXPECT_EQ(value.bits().high, high);
}

void expect_parts(const uint128_t &value, uint64_t low, uint64_t high) {
    EXPECT_EQ(value.low, low);
    EXPECT_EQ(value.high, high);
}

} // namespace

TEST(int128_t, construct_and_compare) {
    const int128_t minus_one(-1);
    const int128_t min(0, UINT64_C(0x8000000000000000));
    const int128_t max(UINT64_MAX, UINT64_C(0x7fffffffffffffff));

    expect_bits(minus_one, UINT64_MAX, UINT64_MAX);
    expect_bits(min, 0, UINT64_C(0x8000000000000000));
    expect_bits(max, UINT64_MAX, UINT64_C(0x7fffffffffffffff));

    EXPECT_TRUE(minus_one < int128_t(0));
    EXPECT_TRUE(int128_t(-2) < minus_one);
    EXPECT_FALSE(minus_one < int128_t(-2));
    EXPECT_TRUE(min < minus_one);
    EXPECT_TRUE(max > int128_t(0));
    EXPECT_EQ(minus_one, int128_t(-1));
}

TEST(int128_t, arithmetic) {
    expect_bits(int128_t(-7) + 3, static_cast<uint64_t>(-4), UINT64_MAX);
    expect_bits(int128_t(-7) - 3, static_cast<uint64_t>(-10), UINT64_MAX);
    expect_bits(int128_t(-7) * 3, static_cast<uint64_t>(-21), UINT64_MAX);

    const int128_t min(0, UINT64_C(0x8000000000000000));
    const int128_t max(UINT64_MAX, UINT64_C(0x7fffffffffffffff));
    expect_bits(max + 1, 0, UINT64_C(0x8000000000000000));
    expect_bits(-min, 0, UINT64_C(0x8000000000000000));

    expect_bits(+int128_t(-7), static_cast<uint64_t>(-7), UINT64_MAX);
    expect_parts(int128_t(1) + uint128_t(UINT64_MAX, UINT64_MAX), 0, 0);
    expect_parts(uint128_t(3, 0) - int128_t(-1), 4, 0);
    expect_parts(uint128_t(3, 0) * int128_t(-2), UINT64_MAX - 5, UINT64_MAX);

    int128_t number(0);
    ++number;
    EXPECT_EQ(static_cast<int64_t>(number++), 1);
    EXPECT_EQ(static_cast<int64_t>(number), 2);
    --number;
    EXPECT_EQ(static_cast<int64_t>(number--), 1);
    EXPECT_EQ(static_cast<int64_t>(number), 0);
}

TEST(int128_t, division) {
    EXPECT_EQ(static_cast<int64_t>(int128_t(-7) / 3), -2);
    EXPECT_EQ(static_cast<int64_t>(int128_t(-7) % 3), -1);
    EXPECT_EQ(static_cast<int64_t>(int128_t(7) / -3), -2);
    EXPECT_EQ(static_cast<int64_t>(int128_t(7) % -3), 1);

    const int128_t min(0, UINT64_C(0x8000000000000000));
    expect_bits(min / 2, 0, UINT64_C(0xc000000000000000));
    expect_bits(min / -1, 0, UINT64_C(0x8000000000000000));
}

TEST(int128_t, bitwise_and_shift) {
    const int128_t min(0, UINT64_C(0x8000000000000000));

    expect_bits(~int128_t(0), UINT64_MAX, UINT64_MAX);
    expect_bits(int128_t(-1) >> 1, UINT64_MAX, UINT64_MAX);
    expect_bits(min >> 1, 0, UINT64_C(0xc000000000000000));
    expect_bits(min >> 64, UINT64_C(0x8000000000000000), UINT64_C(0xffffffffffffffff));
    expect_bits(min >> 128, UINT64_MAX, UINT64_MAX);
    expect_bits(int128_t(1) << 127, 0, UINT64_C(0x8000000000000000));
    expect_bits(int128_t(3) << 1, 6, 0);
    expect_bits(int128_t(6) >> 1, 3, 0);

    expect_parts(uint128_t(UINT64_MAX, UINT64_MAX) & int128_t(-1), UINT64_MAX, UINT64_MAX);
    expect_parts(uint128_t(0, 0) | int128_t(-1), UINT64_MAX, UINT64_MAX);
    expect_parts(uint128_t(UINT64_MAX, UINT64_MAX) ^ int128_t(-1), 0, 0);
}

TEST(int128_t, mixed_unsigned_operations) {
    const int128_t negative(-1);
    const uint128_t maximum(UINT64_MAX, UINT64_MAX);

    expect_parts(negative + maximum, UINT64_MAX - 1, UINT64_MAX);
    expect_parts(negative - uint128_t(3, 0), UINT64_MAX - 3, UINT64_MAX);
    expect_parts(maximum / negative, 1, 0);
    expect_parts(maximum % negative, 0, 0);

    EXPECT_FALSE(negative < uint128_t(0, 0));
    EXPECT_TRUE(uint128_t(0, 0) < negative);
    EXPECT_FALSE(negative == uint128_t(0, 0));
    EXPECT_FALSE(negative <= uint128_t(0, 0));
    EXPECT_FALSE(uint128_t(0, 0) >= negative);

    int128_t signed_value(12);
    signed_value += uint128_t(3, 0);
    signed_value -= uint128_t(2, 0);
    signed_value *= uint128_t(2, 0);
    signed_value /= uint128_t(2, 0);
    signed_value %= uint128_t(5, 0);
    signed_value &= uint128_t(7, 0);
    signed_value |= uint128_t(8, 0);
    signed_value ^= uint128_t(1, 0);
    EXPECT_EQ(static_cast<int64_t>(signed_value), 10);

    uint128_t unsigned_value(12, 0);
    unsigned_value += int128_t(3);
    unsigned_value -= int128_t(2);
    unsigned_value *= int128_t(2);
    unsigned_value /= int128_t(2);
    unsigned_value %= int128_t(5);
    unsigned_value &= int128_t(7);
    unsigned_value |= int128_t(8);
    unsigned_value ^= int128_t(1);
    EXPECT_EQ(unsigned_value, uint128_t(10, 0));
}

TEST(int128_t, numeric_limits) {
    EXPECT_EQ(std::numeric_limits<int128_t>::digits, 127);
    EXPECT_TRUE(std::numeric_limits<int128_t>::is_signed);
    EXPECT_EQ(std::numeric_limits<int128_t>::min().bits(),
              uint128_t(0, UINT64_C(0x8000000000000000)));
    EXPECT_EQ(std::numeric_limits<int128_t>::max().bits(),
              uint128_t(UINT64_MAX, UINT64_C(0x7fffffffffffffff)));
    EXPECT_EQ(std::numeric_limits<int128_t>::lowest(), std::numeric_limits<int128_t>::min());
}
