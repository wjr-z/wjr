#include <gtest/gtest.h>

#include <wjr/math/integral_constant.hpp>

using namespace wjr;

TEST(integral_constant, parse_lengths) {
    static_assert(digits_literal_detail::parse<unsigned>() == 0);
    static_assert(digits_literal_detail::parse<unsigned, '7'>() == 7);
    static_assert(digits_literal_detail::parse<unsigned, '1', '2'>() == 12);
    static_assert(digits_literal_detail::parse<unsigned, '1', '2', '3'>() == 123);
    static_assert(digits_literal_detail::parse<unsigned, '1', '2', '3', '4'>() == 1234);
    static_assert(
        digits_literal_detail::parse<unsigned, '1', '2', '3', '4', '5', '6', '7', '8'>() ==
        12345678);
    static_assert(digits_literal_detail::parse<unsigned, '1', '2', '3', '4', '5', '6', '7', '8',
                                               '9', '0'>() == 1234567890);

    EXPECT_EQ(1234_u, 1234U);
    EXPECT_EQ(1234_ul, 1234UL);
    EXPECT_EQ(1234_ull, 1234ULL);
    EXPECT_EQ(1234_i, 1234);
    EXPECT_EQ(1234_l, 1234L);
    EXPECT_EQ(1234_ll, 1234LL);
}

TEST(integral_constant, fixed_width_literals) {
    static_assert(12_i8 == int8_t(12));
    static_assert(1234_i16 == int16_t(1234));
    static_assert(123456_i32 == int32_t(123456));
    static_assert(1234567890_i64 == int64_t(1234567890));
    static_assert(12_u8 == uint8_t(12));
    static_assert(1234_u16 == uint16_t(1234));
    static_assert(123456_u32 == uint32_t(123456));
    static_assert(1234567890_u64 == uint64_t(1234567890));
    static_assert(1234_zu == size_t(1234));
    static_assert(1234_z == ssize_t(1234));

    EXPECT_EQ(42_u32, uint32_t(42));
    EXPECT_EQ(42_i64, int64_t(42));
}