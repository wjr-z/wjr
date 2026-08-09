#include "../detail.hpp"

#include <limits>

#include <wjr/math/add.hpp>

using namespace wjr;

TEST(add_overflow, signed_values) {
    int result = 0;

    EXPECT_FALSE(add_overflow(10, 20, result));
    EXPECT_EQ(result, 30);
    EXPECT_FALSE(add_overflow(-10, 3, result));
    EXPECT_EQ(result, -7);

    EXPECT_TRUE(add_overflow(std::numeric_limits<int>::max(), 1, result));
    EXPECT_EQ(result, std::numeric_limits<int>::min());
    EXPECT_TRUE(add_overflow(std::numeric_limits<int>::min(), -1, result));
    EXPECT_EQ(result, std::numeric_limits<int>::max());
}

TEST(add_overflow, signed_widths_and_constexpr) {
    int8_t result8 = 0;
    EXPECT_TRUE(add_overflow(std::numeric_limits<int8_t>::max(), int8_t(1), result8));
    EXPECT_EQ(result8, std::numeric_limits<int8_t>::min());

    int64_t result64 = 0;
    EXPECT_TRUE(add_overflow(std::numeric_limits<int64_t>::min(), int64_t(-1), result64));
    EXPECT_EQ(result64, std::numeric_limits<int64_t>::max());

    int fallback_result = 0;
    EXPECT_FALSE(math::fallback_add_overflow_signed(12, -5, fallback_result));
    EXPECT_EQ(fallback_result, 7);
    EXPECT_TRUE(
        math::fallback_add_overflow_signed(std::numeric_limits<int>::max(), 1, fallback_result));
    EXPECT_EQ(fallback_result, std::numeric_limits<int>::min());
    EXPECT_TRUE(
        math::fallback_add_overflow_signed(std::numeric_limits<int>::min(), -1, fallback_result));
    EXPECT_EQ(fallback_result, std::numeric_limits<int>::max());
}

TEST(add_overflow, unsigned_regression) {
    uint32_t result = 0;

    EXPECT_FALSE(add_overflow(uint32_t(10), uint32_t(20), result));
    EXPECT_EQ(result, 30u);
    EXPECT_TRUE(add_overflow(std::numeric_limits<uint32_t>::max(), uint32_t(1), result));
    EXPECT_EQ(result, 0u);
}