#include "../detail.hpp"

#include <wjr/math/prefix_xor.hpp>

using namespace wjr;

TEST(prefix_xor, width_specific_values) {
    EXPECT_EQ(prefix_xor<uint8_t>(0), 0);
    EXPECT_EQ(prefix_xor<uint8_t>(1), 0xff);
    EXPECT_EQ(prefix_xor<uint8_t>(0x0f), 0x05);
    EXPECT_EQ(prefix_xor<uint8_t>(0x80), 0x80);

    EXPECT_EQ(prefix_xor<uint16_t>(1), 0xffff);
    EXPECT_EQ(prefix_xor<uint16_t>(0x00ff), 0x0055);
    EXPECT_EQ(prefix_xor<uint32_t>(1), 0xffffffffu);
    EXPECT_EQ(prefix_xor<uint64_t>(1), 0xffffffffffffffffull);
}

TEST(prefix_xor, constexpr_fallback) {
    static_assert(fallback_prefix_xor<uint8_t>(0x0f) == 0x05);
    static_assert(fallback_prefix_xor<uint16_t>(0x00ff) == 0x0055);
    static_assert(fallback_prefix_xor<uint32_t>(1) == 0xffffffffu);
}
