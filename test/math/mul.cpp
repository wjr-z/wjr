#include <gtest/gtest.h>

#include <array>
#include <limits>

#include <wjr/math/mul.hpp>

using namespace wjr;

static_assert(mullo<uint8_t>(UINT8_MAX, UINT8_MAX) == 1);
static_assert(mullo<uint16_t>(UINT16_MAX, UINT16_MAX) == 1);
static_assert(mullo<uint32_t>(UINT32_MAX, UINT32_MAX) == 1);
static_assert(mullo<uint64_t>(UINT64_MAX, UINT64_MAX) == 1);
static_assert(mulhi<uint8_t>(UINT8_MAX, UINT8_MAX) == UINT8_MAX - 1);
static_assert(mulhi<uint16_t>(UINT16_MAX, UINT16_MAX) == UINT16_MAX - 1);
static_assert(mulhi<uint32_t>(UINT32_MAX, UINT32_MAX) == UINT32_MAX - 1);
static_assert(mulhi<uint64_t>(UINT64_MAX, UINT64_MAX) == UINT64_MAX - 1);

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
    expect_product<uint16_t>(UINT16_MAX, UINT16_MAX, 1, UINT16_MAX - 1);
    expect_product<uint32_t>(0xffff'ffffU, 2, 0xffff'fffeU, 1);
    expect_product<uint64_t>(UINT64_MAX, 2, UINT64_MAX - 1, 1);

    uint64_t high = 0;
    EXPECT_EQ(fallback_mul64(UINT64_MAX, 2, high), UINT64_MAX - 1);
    EXPECT_EQ(high, 1);
}

TEST(mul, narrow_low_product) {
    constexpr std::array<uint16_t, 9> values = {0, 1, 2, 32767, 32768, 46340, 46341, 65534, 65535};
    for (const auto lhs : values) {
        for (const auto rhs : values) {
            SCOPED_TRACE(lhs);
            SCOPED_TRACE(rhs);
            const auto expected = static_cast<uint16_t>(static_cast<uint32_t>(lhs) * rhs);
            volatile uint16_t runtime_lhs = lhs;
            volatile uint16_t runtime_rhs = rhs;
            EXPECT_EQ(mullo<uint16_t>(runtime_lhs, runtime_rhs), expected);
        }
    }
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