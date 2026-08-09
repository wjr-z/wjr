#include "../detail.hpp"

#include <wjr/math/shift.hpp>

using namespace wjr;

TEST(shift, double_precision) {
    constexpr uint64_t high = 0x0123456789abcdef;
    constexpr uint64_t low = 0xfedcba9876543210;

    EXPECT_EQ(shld(high, low, 4), 0x123456789abcdeffull);
    EXPECT_EQ(shrd(low, high, 4), 0xffedcba987654321ull);
}

TEST(shift, array_left_and_right) {
    constexpr uint64_t source[] = {0x0123456789abcdefull, 0xfedcba9876543210ull};

    uint64_t left[2] = {};
    EXPECT_EQ(lshift_n(left, source, 2, 4, 0xa000000000000000ull), 0xfull);
    EXPECT_EQ(left[0], 0x123456789abcdefaull);
    EXPECT_EQ(left[1], 0xedcba98765432100ull);

    uint64_t right[2] = {};
    EXPECT_EQ(rshift_n(right, source, 2, 4, 0xb000000000000000ull), 0xf000000000000000ull);
    EXPECT_EQ(right[0], 0x00123456789abcdeull);
    EXPECT_EQ(right[1], 0x0fedcba987654321ull);

    uint64_t copied_left[2] = {};
    EXPECT_EQ(lshift_n(copied_left, source, 2, 0), 0);
    EXPECT_EQ(copied_left[0], source[0]);
    EXPECT_EQ(copied_left[1], source[1]);

    uint64_t copied_right[2] = {};
    EXPECT_EQ(rshift_n(copied_right, source, 2, 0), 0);
    EXPECT_EQ(copied_right[0], source[0]);
    EXPECT_EQ(copied_right[1], source[1]);
}

TEST(shift, complemented_array_shifts) {
    constexpr uint32_t source[] = {0x01234567u, 0x89abcdefu};

    uint32_t left[2] = {};
    EXPECT_EQ(lshiftc_n(left, source, 2, 4, 0xa0000000u), 0x8u);
    EXPECT_EQ(left[0], 0xedcba985u);
    EXPECT_EQ(left[1], 0x6543210fu);

    uint32_t right[2] = {};
    EXPECT_EQ(rshiftc_n(right, source, 2, 4, 0xb0000000u), 0x70000000u);
    EXPECT_EQ(right[0], 0x0fedcba9u);
    EXPECT_EQ(right[1], 0xf7654321u);
}
