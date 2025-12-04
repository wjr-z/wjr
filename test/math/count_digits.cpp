#include "../detail.hpp"

#include <wjr/math/count_digits.hpp>

using namespace wjr;

// Test count_digits for base 10 (decimal)
TEST(count_digits, base10) {
    // Test single digit numbers
    EXPECT_EQ(count_digits<10>(0u), 1);
    EXPECT_EQ(count_digits<10>(1u), 1);
    EXPECT_EQ(count_digits<10>(5u), 1);
    EXPECT_EQ(count_digits<10>(9u), 1);

    // Test two digit numbers
    EXPECT_EQ(count_digits<10>(10u), 2);
    EXPECT_EQ(count_digits<10>(50u), 2);
    EXPECT_EQ(count_digits<10>(99u), 2);

    // Test three digit numbers
    EXPECT_EQ(count_digits<10>(100u), 3);
    EXPECT_EQ(count_digits<10>(500u), 3);
    EXPECT_EQ(count_digits<10>(999u), 3);

    // Test boundary cases for powers of 10
    EXPECT_EQ(count_digits<10>(1000u), 4);
    EXPECT_EQ(count_digits<10>(9999u), 4);
    EXPECT_EQ(count_digits<10>(10000u), 5);
    EXPECT_EQ(count_digits<10>(99999u), 5);
    EXPECT_EQ(count_digits<10>(100000u), 6);
    EXPECT_EQ(count_digits<10>(999999u), 6);
    EXPECT_EQ(count_digits<10>(1000000u), 7);
    EXPECT_EQ(count_digits<10>(9999999u), 7);
    EXPECT_EQ(count_digits<10>(10000000u), 8);
    EXPECT_EQ(count_digits<10>(99999999u), 8);
    EXPECT_EQ(count_digits<10>(100000000u), 9);
    EXPECT_EQ(count_digits<10>(999999999u), 9);
    EXPECT_EQ(count_digits<10>(1000000000u), 10);

    // Test with uint64_t
    EXPECT_EQ(count_digits<10>(1234567890ull), 10);
    EXPECT_EQ(count_digits<10>(9999999999ull), 10);
    EXPECT_EQ(count_digits<10>(10000000000ull), 11);
    EXPECT_EQ(count_digits<10>(99999999999ull), 11);
    EXPECT_EQ(count_digits<10>(100000000000ull), 12);
    EXPECT_EQ(count_digits<10>(999999999999ull), 12);
    EXPECT_EQ(count_digits<10>(1000000000000ull), 13);
    EXPECT_EQ(count_digits<10>(9999999999999ull), 13);
    EXPECT_EQ(count_digits<10>(10000000000000ull), 14);
    EXPECT_EQ(count_digits<10>(99999999999999ull), 14);
    EXPECT_EQ(count_digits<10>(100000000000000ull), 15);
    EXPECT_EQ(count_digits<10>(999999999999999ull), 15);
    EXPECT_EQ(count_digits<10>(1000000000000000ull), 16);
    EXPECT_EQ(count_digits<10>(9999999999999999ull), 16);
    EXPECT_EQ(count_digits<10>(10000000000000000ull), 17);
    EXPECT_EQ(count_digits<10>(99999999999999999ull), 17);
    EXPECT_EQ(count_digits<10>(100000000000000000ull), 18);
    EXPECT_EQ(count_digits<10>(999999999999999999ull), 18);
    EXPECT_EQ(count_digits<10>(1000000000000000000ull), 19);
    EXPECT_EQ(count_digits<10>(9999999999999999999ull), 19);
    EXPECT_EQ(count_digits<10>(10000000000000000000ull), 20);

    // Test maximum values
    EXPECT_EQ(count_digits<10>(std::numeric_limits<uint32_t>::max()), 10);
    EXPECT_EQ(count_digits<10>(std::numeric_limits<uint64_t>::max()), 20);
}

// Test count_digits for base 2 (binary)
TEST(count_digits, base2) {
    // Test powers of 2 and boundaries
    EXPECT_EQ(count_digits<2>(0u), 0);
    EXPECT_EQ(count_digits<2>(1u), 1);
    EXPECT_EQ(count_digits<2>(2u), 2);
    EXPECT_EQ(count_digits<2>(3u), 2);
    EXPECT_EQ(count_digits<2>(4u), 3);
    EXPECT_EQ(count_digits<2>(7u), 3);
    EXPECT_EQ(count_digits<2>(8u), 4);
    EXPECT_EQ(count_digits<2>(15u), 4);
    EXPECT_EQ(count_digits<2>(16u), 5);
    EXPECT_EQ(count_digits<2>(31u), 5);
    EXPECT_EQ(count_digits<2>(32u), 6);
    EXPECT_EQ(count_digits<2>(63u), 6);
    EXPECT_EQ(count_digits<2>(64u), 7);
    EXPECT_EQ(count_digits<2>(127u), 7);
    EXPECT_EQ(count_digits<2>(128u), 8);
    EXPECT_EQ(count_digits<2>(255u), 8);
    EXPECT_EQ(count_digits<2>(256u), 9);
    EXPECT_EQ(count_digits<2>(511u), 9);
    EXPECT_EQ(count_digits<2>(512u), 10);
    EXPECT_EQ(count_digits<2>(1023u), 10);
    EXPECT_EQ(count_digits<2>(1024u), 11);

    // Test with larger values
    EXPECT_EQ(count_digits<2>(0xFFFFu), 16);
    EXPECT_EQ(count_digits<2>(0x10000u), 17);
    EXPECT_EQ(count_digits<2>(0xFFFFFFFFu), 32);
    EXPECT_EQ(count_digits<2>(0x100000000ull), 33);
    EXPECT_EQ(count_digits<2>(0xFFFFFFFFFFFFFFFFull), 64);

    // Test maximum values
    EXPECT_EQ(count_digits<2>(std::numeric_limits<uint8_t>::max()), 8);
    EXPECT_EQ(count_digits<2>(std::numeric_limits<uint16_t>::max()), 16);
    EXPECT_EQ(count_digits<2>(std::numeric_limits<uint32_t>::max()), 32);
    EXPECT_EQ(count_digits<2>(std::numeric_limits<uint64_t>::max()), 64);
}

// Test count_digits for base 16 (hexadecimal)
TEST(count_digits, base16) {
    // Test powers of 16 and boundaries
    EXPECT_EQ(count_digits<16>(0u), 0);
    EXPECT_EQ(count_digits<16>(1u), 1);
    EXPECT_EQ(count_digits<16>(15u), 1);
    EXPECT_EQ(count_digits<16>(16u), 2);
    EXPECT_EQ(count_digits<16>(255u), 2);
    EXPECT_EQ(count_digits<16>(256u), 3);
    EXPECT_EQ(count_digits<16>(4095u), 3);
    EXPECT_EQ(count_digits<16>(4096u), 4);
    EXPECT_EQ(count_digits<16>(65535u), 4);
    EXPECT_EQ(count_digits<16>(65536u), 5);

    // Test with larger values
    EXPECT_EQ(count_digits<16>(0xFFu), 2);
    EXPECT_EQ(count_digits<16>(0x100u), 3);
    EXPECT_EQ(count_digits<16>(0xFFFu), 3);
    EXPECT_EQ(count_digits<16>(0x1000u), 4);
    EXPECT_EQ(count_digits<16>(0xFFFFu), 4);
    EXPECT_EQ(count_digits<16>(0x10000u), 5);
    EXPECT_EQ(count_digits<16>(0xFFFFFu), 5);
    EXPECT_EQ(count_digits<16>(0x100000u), 6);
    EXPECT_EQ(count_digits<16>(0xFFFFFFu), 6);
    EXPECT_EQ(count_digits<16>(0x1000000u), 7);
    EXPECT_EQ(count_digits<16>(0xFFFFFFFu), 7);
    EXPECT_EQ(count_digits<16>(0x10000000u), 8);
    EXPECT_EQ(count_digits<16>(0xFFFFFFFFu), 8);
    EXPECT_EQ(count_digits<16>(0x100000000ull), 9);
    EXPECT_EQ(count_digits<16>(0xFFFFFFFFFull), 9);
    EXPECT_EQ(count_digits<16>(0x1000000000ull), 10);
    EXPECT_EQ(count_digits<16>(0xFFFFFFFFFFull), 10);
    EXPECT_EQ(count_digits<16>(0x10000000000ull), 11);
    EXPECT_EQ(count_digits<16>(0xFFFFFFFFFFFull), 11);
    EXPECT_EQ(count_digits<16>(0x100000000000ull), 12);
    EXPECT_EQ(count_digits<16>(0xFFFFFFFFFFFFull), 12);
    EXPECT_EQ(count_digits<16>(0x1000000000000ull), 13);
    EXPECT_EQ(count_digits<16>(0xFFFFFFFFFFFFFull), 13);
    EXPECT_EQ(count_digits<16>(0x10000000000000ull), 14);
    EXPECT_EQ(count_digits<16>(0xFFFFFFFFFFFFFFull), 14);
    EXPECT_EQ(count_digits<16>(0x100000000000000ull), 15);
    EXPECT_EQ(count_digits<16>(0xFFFFFFFFFFFFFFFull), 15);
    EXPECT_EQ(count_digits<16>(0x1000000000000000ull), 16);
    EXPECT_EQ(count_digits<16>(0xFFFFFFFFFFFFFFFFull), 16);

    // Test maximum values
    EXPECT_EQ(count_digits<16>(std::numeric_limits<uint8_t>::max()), 2);
    EXPECT_EQ(count_digits<16>(std::numeric_limits<uint16_t>::max()), 4);
    EXPECT_EQ(count_digits<16>(std::numeric_limits<uint32_t>::max()), 8);
    EXPECT_EQ(count_digits<16>(std::numeric_limits<uint64_t>::max()), 16);
}

// Test count_digits for base 8 (octal)
TEST(count_digits, base8) {
    // Test powers of 8 and boundaries
    EXPECT_EQ(count_digits<8>(0u), 0);
    EXPECT_EQ(count_digits<8>(1u), 1);
    EXPECT_EQ(count_digits<8>(7u), 1);
    EXPECT_EQ(count_digits<8>(8u), 2);
    EXPECT_EQ(count_digits<8>(63u), 2);
    EXPECT_EQ(count_digits<8>(64u), 3);
    EXPECT_EQ(count_digits<8>(511u), 3);
    EXPECT_EQ(count_digits<8>(512u), 4);
    EXPECT_EQ(count_digits<8>(4095u), 4);
    EXPECT_EQ(count_digits<8>(4096u), 5);

    // Test with larger values
    EXPECT_EQ(count_digits<8>(077u), 2);
    EXPECT_EQ(count_digits<8>(0100u), 3);
    EXPECT_EQ(count_digits<8>(0777u), 3);
    EXPECT_EQ(count_digits<8>(01000u), 4);
    EXPECT_EQ(count_digits<8>(07777u), 4);
    EXPECT_EQ(count_digits<8>(010000u), 5);
    EXPECT_EQ(count_digits<8>(077777u), 5);
    EXPECT_EQ(count_digits<8>(0100000u), 6);

    // Test maximum values
    EXPECT_EQ(count_digits<8>(std::numeric_limits<uint8_t>::max()), 3);
    EXPECT_EQ(count_digits<8>(std::numeric_limits<uint16_t>::max()), 6);
    EXPECT_EQ(count_digits<8>(std::numeric_limits<uint32_t>::max()), 11);
    EXPECT_EQ(count_digits<8>(std::numeric_limits<uint64_t>::max()), 22);
}

// Test count_digits for dynamic power-of-2 base
TEST(count_digits, base1_dynamic) {
    // Base-4 (2 bits per digit)
    EXPECT_EQ(count_digits<1>(0u, 2), 0);
    EXPECT_EQ(count_digits<1>(1u, 2), 1);
    EXPECT_EQ(count_digits<1>(3u, 2), 1);
    EXPECT_EQ(count_digits<1>(4u, 2), 2);
    EXPECT_EQ(count_digits<1>(15u, 2), 2);
    EXPECT_EQ(count_digits<1>(16u, 2), 3);
    EXPECT_EQ(count_digits<1>(63u, 2), 3);
    EXPECT_EQ(count_digits<1>(64u, 2), 4);

    // Base-8 (3 bits per digit) - should match base 8 test
    EXPECT_EQ(count_digits<1>(0u, 3), 0);
    EXPECT_EQ(count_digits<1>(1u, 3), 1);
    EXPECT_EQ(count_digits<1>(7u, 3), 1);
    EXPECT_EQ(count_digits<1>(8u, 3), 2);
    EXPECT_EQ(count_digits<1>(63u, 3), 2);
    EXPECT_EQ(count_digits<1>(64u, 3), 3);

    // Base-16 (4 bits per digit) - should match base 16 test
    EXPECT_EQ(count_digits<1>(0u, 4), 0);
    EXPECT_EQ(count_digits<1>(1u, 4), 1);
    EXPECT_EQ(count_digits<1>(15u, 4), 1);
    EXPECT_EQ(count_digits<1>(16u, 4), 2);
    EXPECT_EQ(count_digits<1>(255u, 4), 2);
    EXPECT_EQ(count_digits<1>(256u, 4), 3);

    // Base-32 (5 bits per digit)
    EXPECT_EQ(count_digits<1>(0u, 5), 0);
    EXPECT_EQ(count_digits<1>(1u, 5), 1);
    EXPECT_EQ(count_digits<1>(31u, 5), 1);
    EXPECT_EQ(count_digits<1>(32u, 5), 2);
    EXPECT_EQ(count_digits<1>(1023u, 5), 2);
    EXPECT_EQ(count_digits<1>(1024u, 5), 3);

    // Base-64 (6 bits per digit)
    EXPECT_EQ(count_digits<1>(0u, 6), 0);
    EXPECT_EQ(count_digits<1>(1u, 6), 1);
    EXPECT_EQ(count_digits<1>(63u, 6), 1);
    EXPECT_EQ(count_digits<1>(64u, 6), 2);
    EXPECT_EQ(count_digits<1>(4095u, 6), 2);
    EXPECT_EQ(count_digits<1>(4096u, 6), 3);
}

// Test fallback implementation directly
TEST(count_digits, fallback_count_digits10) {
    // Test the fallback implementation for compile-time evaluation
    static_assert(fallback_count_digits10(0u) == 1);
    static_assert(fallback_count_digits10(9u) == 1);
    static_assert(fallback_count_digits10(10u) == 2);
    static_assert(fallback_count_digits10(99u) == 2);
    static_assert(fallback_count_digits10(100u) == 3);
    static_assert(fallback_count_digits10(999u) == 3);
    static_assert(fallback_count_digits10(1000u) == 4);
    static_assert(fallback_count_digits10(9999u) == 4);
    static_assert(fallback_count_digits10(10000u) == 5);
    static_assert(fallback_count_digits10(99999u) == 5);
    static_assert(fallback_count_digits10(100000u) == 6);
    static_assert(fallback_count_digits10(999999u) == 6);
    static_assert(fallback_count_digits10(1000000u) == 7);
    static_assert(fallback_count_digits10(9999999u) == 7);
    static_assert(fallback_count_digits10(10000000u) == 8);
    static_assert(fallback_count_digits10(99999999u) == 8);
    static_assert(fallback_count_digits10(100000000u) == 9);
    static_assert(fallback_count_digits10(999999999u) == 9);
    static_assert(fallback_count_digits10(1000000000u) == 10);

    // Runtime tests
    EXPECT_EQ(fallback_count_digits10(12345u), 5);
    EXPECT_EQ(fallback_count_digits10(1234567890ull), 10);
}

// Test builtin implementations
TEST(count_digits, builtin_count_digits10_u32) {
    EXPECT_EQ(builtin_count_digits10_u32(0u), 1);
    EXPECT_EQ(builtin_count_digits10_u32(9u), 1);
    EXPECT_EQ(builtin_count_digits10_u32(10u), 2);
    EXPECT_EQ(builtin_count_digits10_u32(99u), 2);
    EXPECT_EQ(builtin_count_digits10_u32(100u), 3);
    EXPECT_EQ(builtin_count_digits10_u32(999u), 3);
    EXPECT_EQ(builtin_count_digits10_u32(1000u), 4);
    EXPECT_EQ(builtin_count_digits10_u32(9999u), 4);
    EXPECT_EQ(builtin_count_digits10_u32(10000u), 5);
    EXPECT_EQ(builtin_count_digits10_u32(99999u), 5);
    EXPECT_EQ(builtin_count_digits10_u32(100000u), 6);
    EXPECT_EQ(builtin_count_digits10_u32(999999u), 6);
    EXPECT_EQ(builtin_count_digits10_u32(1000000u), 7);
    EXPECT_EQ(builtin_count_digits10_u32(9999999u), 7);
    EXPECT_EQ(builtin_count_digits10_u32(10000000u), 8);
    EXPECT_EQ(builtin_count_digits10_u32(99999999u), 8);
    EXPECT_EQ(builtin_count_digits10_u32(100000000u), 9);
    EXPECT_EQ(builtin_count_digits10_u32(999999999u), 9);
    EXPECT_EQ(builtin_count_digits10_u32(1000000000u), 10);
    EXPECT_EQ(builtin_count_digits10_u32(std::numeric_limits<uint32_t>::max()), 10);
}

TEST(count_digits, builtin_count_digits10_u64) {
    EXPECT_EQ(builtin_count_digits10_u64(0ull), 1);
    EXPECT_EQ(builtin_count_digits10_u64(9ull), 1);
    EXPECT_EQ(builtin_count_digits10_u64(10ull), 2);
    EXPECT_EQ(builtin_count_digits10_u64(99ull), 2);
    EXPECT_EQ(builtin_count_digits10_u64(100ull), 3);
    EXPECT_EQ(builtin_count_digits10_u64(1234567890ull), 10);
    EXPECT_EQ(builtin_count_digits10_u64(9999999999ull), 10);
    EXPECT_EQ(builtin_count_digits10_u64(10000000000ull), 11);
    EXPECT_EQ(builtin_count_digits10_u64(99999999999ull), 11);
    EXPECT_EQ(builtin_count_digits10_u64(100000000000ull), 12);
    EXPECT_EQ(builtin_count_digits10_u64(999999999999ull), 12);
    EXPECT_EQ(builtin_count_digits10_u64(1000000000000ull), 13);
    EXPECT_EQ(builtin_count_digits10_u64(9999999999999ull), 13);
    EXPECT_EQ(builtin_count_digits10_u64(10000000000000ull), 14);
    EXPECT_EQ(builtin_count_digits10_u64(99999999999999ull), 14);
    EXPECT_EQ(builtin_count_digits10_u64(100000000000000ull), 15);
    EXPECT_EQ(builtin_count_digits10_u64(999999999999999ull), 15);
    EXPECT_EQ(builtin_count_digits10_u64(1000000000000000ull), 16);
    EXPECT_EQ(builtin_count_digits10_u64(9999999999999999ull), 16);
    EXPECT_EQ(builtin_count_digits10_u64(10000000000000000ull), 17);
    EXPECT_EQ(builtin_count_digits10_u64(99999999999999999ull), 17);
    EXPECT_EQ(builtin_count_digits10_u64(100000000000000000ull), 18);
    EXPECT_EQ(builtin_count_digits10_u64(999999999999999999ull), 18);
    EXPECT_EQ(builtin_count_digits10_u64(1000000000000000000ull), 19);
    EXPECT_EQ(builtin_count_digits10_u64(9999999999999999999ull), 19);
    EXPECT_EQ(builtin_count_digits10_u64(10000000000000000000ull), 20);
    EXPECT_EQ(builtin_count_digits10_u64(std::numeric_limits<uint64_t>::max()), 20);
}

// Test constexpr evaluation
TEST(count_digits, constexpr_evaluation) {
    // Test that count_digits works correctly at runtime
    // Note: In C++17, these functions use WJR_INTRINSIC_CONSTEXPR20 which means
    // they're only constexpr in C++20 and later
    EXPECT_EQ(count_digits<10>(12345u), 5);
    EXPECT_EQ(count_digits<2>(255u), 8);
    EXPECT_EQ(count_digits<16>(255u), 2);
    EXPECT_EQ(count_digits<8>(511u), 3);

    // Test with different integer types
    EXPECT_EQ(count_digits<10>(static_cast<uint8_t>(99)), 2);
    EXPECT_EQ(count_digits<10>(static_cast<uint16_t>(9999)), 4);
    EXPECT_EQ(count_digits<10>(static_cast<uint32_t>(999999999)), 9);
    EXPECT_EQ(count_digits<10>(static_cast<uint64_t>(9999999999999999999ull)), 19);
}

// Comprehensive test with sequential numbers
TEST(count_digits, sequential_numbers) {
    // Test all single-digit numbers
    for (uint32_t i = 0; i <= 9; ++i) {
        EXPECT_EQ(count_digits<10>(i), 1);
    }

    // Test all two-digit numbers
    for (uint32_t i = 10; i <= 99; ++i) {
        EXPECT_EQ(count_digits<10>(i), 2);
    }

    // Test sample three-digit numbers
    for (uint32_t i = 100; i <= 999; i += 100) {
        EXPECT_EQ(count_digits<10>(i), 3);
    }

    // Test powers of 2 for binary
    for (int i = 0; i < 32; ++i) {
        uint32_t val = (1u << i);
        EXPECT_EQ(count_digits<2>(val), i + 1);
        if (val > 1) {
            EXPECT_EQ(count_digits<2>(val - 1), i);
        }
    }
}
