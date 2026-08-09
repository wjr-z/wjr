#include "detail.hpp"

#include <wjr/utility/bitmask_enum.hpp>

using namespace wjr;

namespace wjr {
enum class test_flags : unsigned { none = 0, first = 1, second = 2, fourth = 4 };

WJR_DECLARE_ENUM_AS_BITMASK(test_flags);
} // namespace wjr

TEST(bitmask_enum, predicates_and_binary_operations) {
    using flags = wjr::test_flags;

    EXPECT_TRUE(!flags::none);
    EXPECT_FALSE(!flags::first);
    EXPECT_FALSE(any(flags::none));
    EXPECT_TRUE(any(flags::first | flags::second));

    EXPECT_EQ(flags::first | flags::second, static_cast<wjr::test_flags>(3));
    EXPECT_EQ(flags::first & flags::second, flags::none);
    EXPECT_EQ((flags::first | flags::second) ^ flags::second, flags::first);
    EXPECT_EQ(flags::first << flags::second, flags::fourth);
    EXPECT_EQ(flags::fourth >> flags::second, flags::first);
}

TEST(bitmask_enum, compound_operations_return_lvalue) {
    using flags = wjr::test_flags;

    auto value = flags::first;
    auto &or_result = (value |= flags::second);
    EXPECT_EQ(&or_result, &value);
    EXPECT_EQ(value, static_cast<wjr::test_flags>(3));

    auto &and_result = (value &= flags::fourth);
    EXPECT_EQ(&and_result, &value);
    EXPECT_EQ(value, flags::none);

    value = flags::first | flags::second;
    auto &xor_result = (value ^= flags::second);
    EXPECT_EQ(&xor_result, &value);
    EXPECT_EQ(value, flags::first);

    auto &left_shift_result = (value <<= flags::second);
    EXPECT_EQ(&left_shift_result, &value);
    EXPECT_EQ(value, flags::fourth);

    auto &right_shift_result = (value >>= flags::second);
    EXPECT_EQ(&right_shift_result, &value);
    EXPECT_EQ(value, flags::first);
}

TEST(bitmask_enum, constexpr_trait_and_operations) {
    using flags = wjr::test_flags;

    static_assert(wjr::is_bitmask_enum<wjr::test_flags>::value);
    static_assert((flags::first | flags::second) == static_cast<wjr::test_flags>(3));
    static_assert(any(flags::fourth));
    static_assert(!any(flags::none));
}
