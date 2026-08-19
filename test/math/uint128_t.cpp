#include "../detail.hpp"
#include <limits>
#include <tuple>
#include <utility>

#include <wjr/math.hpp>

using namespace wjr;

namespace {

template <typename T>
void expect_scalar(T value, uint64_t low, uint64_t high) {
    uint128_t number(value);

    EXPECT_EQ(number.low, low);
    EXPECT_EQ(number.high, high);
    EXPECT_EQ(static_cast<T>(number), value);

    number = value;
    EXPECT_EQ(number, uint128_t(low, high));
}

void expect_parts(const uint128_t &number, uint64_t low, uint64_t high) {
    EXPECT_EQ(number.low, low);
    EXPECT_EQ(number.high, high);
    EXPECT_EQ(std::get<0>(number), low);
    EXPECT_EQ(std::get<1>(number), high);
}

} // namespace

TEST(uint128_t, construct_and_access) {
    expect_scalar<uint8_t>(0xab, 0xab, 0);
    expect_scalar<uint64_t>(0x0123456789abcdef, 0x0123456789abcdef, 0);
    expect_scalar<int64_t>(42, 42, 0);
    expect_scalar<int64_t>(-1, UINT64_MAX, UINT64_MAX);

    uint128_t number(1, 2);
    std::get<0>(number) = 3;
    std::get<1>(number) = 4;
    expect_parts(number, 3, 4);

    const auto &constant = number;
    EXPECT_EQ(std::get<0>(constant), 3);
    EXPECT_EQ(std::get<1>(constant), 4);
    EXPECT_EQ(std::get<0>(std::move(number)), 3);
    EXPECT_EQ(std::get<1>(std::move(constant)), 4);
}

TEST(uint128_t, add_and_subtract) {
    const uint128_t max_low(UINT64_MAX, 0);
    const uint128_t one(1, 0);

    expect_parts(max_low + one, 0, 1);
    expect_parts(uint128_t(1, 2) + uint128_t(3, 4), 4, 6);
    expect_parts(uint128_t(UINT64_MAX, 1) + 1, 0, 2);
    expect_parts(uint128_t(0, 2) - uint128_t(1, 1), UINT64_MAX, 0);
    expect_parts(uint128_t(5, 2) - 3, 2, 2);

    uint128_t number(1, 2);
    number += uint128_t(3, 4);
    expect_parts(number, 4, 6);
    number -= uint128_t(1, 1);
    expect_parts(number, 3, 5);
    number += UINT64_MAX;
    expect_parts(number, 2, 6);
    number -= UINT64_MAX;
    expect_parts(number, 3, 5);

    expect_parts(uint128_t(0, 0) + int64_t(-1), UINT64_MAX, UINT64_MAX);
    expect_parts(int64_t(-1) + uint128_t(1, 0), 0, 0);
    expect_parts(uint128_t(3, 0) - int64_t(-1), 4, 0);
    expect_parts(int64_t(-1) - uint128_t(3, 0), UINT64_MAX - 3, UINT64_MAX);

    number = uint128_t(10, 0);
    number += int64_t(-3);
    EXPECT_EQ(number, uint128_t(7, 0));
    number -= int64_t(-3);
    EXPECT_EQ(number, uint128_t(10, 0));
}

TEST(uint128_t, multiply) {
    expect_parts(uint128_t(3, 4) * uint128_t(5, 6), 15, 38);
    expect_parts(uint128_t(3, 4) * uint64_t(5), 15, 20);
    expect_parts(uint64_t(5) * uint128_t(3, 4), 15, 20);
    expect_parts(uint128_t(3, 4) * int64_t(5), 15, 20);
    expect_parts(uint128_t(3, 4) * int64_t(-5), UINT64_MAX - 14, UINT64_MAX - 20);

    uint128_t number(UINT64_MAX, 0);
    number *= uint128_t(2, 0);
    expect_parts(number, UINT64_MAX - 1, 1);
    number *= uint64_t(3);
    expect_parts(number, UINT64_MAX - 5, 5);

    number = uint128_t(1, 0);
    ++number;
    expect_parts(number, 2, 0);
    expect_parts(number++, 2, 0);
    expect_parts(number, 3, 0);
    --number;
    expect_parts(number, 2, 0);
    expect_parts(number--, 2, 0);
    expect_parts(number, 1, 0);

    number = uint128_t(UINT64_MAX, 1);
    number <<= 1;
    expect_parts(number, UINT64_MAX - 1, 3);
    number >>= 1;
    expect_parts(number, UINT64_MAX, 1);
}

TEST(uint128_t, compare) {
    const uint128_t low(1, 0);
    const uint128_t high(0, 1);
    const uint128_t same(1, 0);

    EXPECT_TRUE(low < high);
    EXPECT_TRUE(high > low);
    EXPECT_TRUE(low <= same);
    EXPECT_TRUE(low >= same);
    EXPECT_TRUE(low == same);
    EXPECT_TRUE(low != high);
    EXPECT_TRUE(high >= low);
    EXPECT_TRUE(low <= high);
    EXPECT_TRUE(uint128_t(0, 0) < int64_t(-1));
    EXPECT_TRUE(int64_t(-1) > uint128_t(0, 0));
    EXPECT_TRUE(uint128_t(UINT64_MAX, UINT64_MAX) == int64_t(-1));
    EXPECT_TRUE(uint128_t(0, 0) != int64_t(-1));
}

TEST(uint128_t, divide) {
    expect_parts(uint128_t(0, 1) / uint64_t(3), 0x5555555555555555, 0);
    expect_parts(uint128_t(0, 1) % uint64_t(3), 1, 0);
    expect_parts(uint128_t(UINT64_MAX, UINT64_MAX) / int64_t(-1), 1, 0);
    expect_parts(uint128_t(UINT64_MAX, UINT64_MAX) % int64_t(-1), 0, 0);
    expect_parts(uint128_t(0, 1) / uint128_t(1, 0), 0, 1);
    expect_parts(uint128_t(0, 1) % uint128_t(1, 0), 0, 0);

    uint128_t value(UINT64_MAX, UINT64_MAX);
    value /= int64_t(-1);
    EXPECT_EQ(value, uint128_t(1, 0));
    value %= int64_t(-1);
    EXPECT_EQ(value, uint128_t(1, 0));
}

TEST(uint128_t, bitwise_and_shift) {
    const uint128_t value(UINT64_C(0x0123456789abcdef), UINT64_C(0xfedcba9876543210));

    expect_parts(~value, UINT64_C(0xfedcba9876543210), UINT64_C(0x0123456789abcdef));
    expect_parts(value & uint128_t(UINT64_MAX, 0), UINT64_C(0x0123456789abcdef), 0);
    expect_parts(value | uint128_t(0, UINT64_MAX), UINT64_C(0x0123456789abcdef), UINT64_MAX);
    expect_parts(value ^ value, 0, 0);
    expect_parts(value << 64, 0, UINT64_C(0x0123456789abcdef));
    expect_parts(value >> 64, UINT64_C(0xfedcba9876543210), 0);
    expect_parts(value << 1, UINT64_C(0x02468acf13579bde), UINT64_C(0xfdb97530eca86420));
    expect_parts(value >> 1, UINT64_C(0x0091a2b3c4d5e6f7), UINT64_C(0x7f6e5d4c3b2a1908));
    expect_parts(value << 128, 0, 0);
    expect_parts(value >> 128, 0, 0);
}

TEST(uint128_t, numeric_limits) {
    EXPECT_EQ(std::numeric_limits<uint128_t>::digits, 128);
    EXPECT_FALSE(std::numeric_limits<uint128_t>::is_signed);
    EXPECT_EQ(std::numeric_limits<uint128_t>::min(), uint128_t(0, 0));
    EXPECT_EQ(std::numeric_limits<uint128_t>::max(), uint128_t(UINT64_MAX, UINT64_MAX));
    EXPECT_EQ(std::numeric_limits<uint128_t>::lowest(), std::numeric_limits<uint128_t>::min());
}