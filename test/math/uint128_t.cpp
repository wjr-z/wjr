#include "../detail.hpp"
#include <tuple>

#include <wjr/math/uint128_t.hpp>

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
}