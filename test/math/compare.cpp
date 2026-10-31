#include "../detail.hpp"

#include <array>

#include <wjr/math/compare.hpp>

using namespace wjr;

namespace {

struct comparison_result {
    bool less;
    bool less_equal;
    std::array<uint64_t, 4> operands;
};

#if WJR_HAS_BUILTIN(__ASM_LESS_128) && WJR_HAS_BUILTIN(__ASM_LESS_EQUAL_128)
/// @brief Keep every input live across both destructive high-limb comparisons.
WJR_NOINLINE comparison_result asm_compare_preserving_inputs(uint64_t lo0, uint64_t hi0,
                                                             uint64_t lo1, uint64_t hi1) {
    const bool less = _asm_less_128(lo0, hi0, lo1, hi1);
    const bool less_equal = _asm_less_equal_128(lo0, hi0, lo1, hi1);
    return {less, less_equal, {lo0, hi0, lo1, hi1}};
}
#endif

void check_128_comparison(uint64_t lo0, uint64_t hi0, uint64_t lo1, uint64_t hi1) {
    const bool less = hi0 < hi1 || (hi0 == hi1 && lo0 < lo1);
    const bool less_equal = hi0 < hi1 || (hi0 == hi1 && lo0 <= lo1);
    EXPECT_EQ(_less_128(lo0, hi0, lo1, hi1), less);
    EXPECT_EQ(_less_equal_128(lo0, hi0, lo1, hi1), less_equal);

#if WJR_HAS_BUILTIN(__ASM_LESS_128) && WJR_HAS_BUILTIN(__ASM_LESS_EQUAL_128)
    const auto result = asm_compare_preserving_inputs(lo0, hi0, lo1, hi1);
    EXPECT_EQ(result.less, less);
    EXPECT_EQ(result.less_equal, less_equal);
    EXPECT_EQ(result.operands, (std::array<uint64_t, 4>{lo0, hi0, lo1, hi1}));
#endif
}

} // namespace

TEST(compare, forward_and_reverse_arrays) {
    constexpr uint64_t left[] = {1, 2, 3, 4};
    constexpr uint64_t right[] = {1, 2, 3, 5};
    constexpr uint64_t greater[] = {1, 2, 4, 0};

    static_assert(reverse_compare_n(left, right, 4) == -1);
    static_assert(reverse_compare_n(right, left, 4) == 1);
    static_assert(reverse_compare_n(left, greater, 4) == 1);
    static_assert(fallback_reverse_compare_n(left, right, 0) == 0);
    static_assert(fallback_reverse_compare_n(left, left, 4) == 0);

    EXPECT_EQ(compare_n(left, left, 4), 0);
    EXPECT_EQ(compare_n(left, right, 0), 0);
    EXPECT_EQ(compare_n(left, right, 4), -1);
    EXPECT_EQ(compare_n(right, left, 4), 1);
    EXPECT_EQ(compare_n(left, greater, 4), -1);

    EXPECT_EQ(reverse_compare_n(left, left, 4), 0);
    EXPECT_EQ(reverse_compare_n(left, right, 4), -1);
    EXPECT_EQ(reverse_compare_n(right, left, 4), 1);
    EXPECT_EQ(reverse_compare_n(left, greater, 4), 1);

    constexpr int signed_left[] = {-2, 0, 3};
    constexpr int signed_right[] = {-1, 0, 3};
    static_assert(reverse_compare_n(signed_left, signed_right, 3) == -1);
    EXPECT_EQ(compare_n(signed_left, signed_right, 3), -1);
    EXPECT_EQ(reverse_compare_n(signed_left, signed_right, 3), -1);
}

TEST(compare, compare_128_helpers) {
    constexpr uint64_t low = 0x1234;
    constexpr uint64_t high = 0x5678;

    EXPECT_TRUE(_equal_128(low, high, low, high));
    EXPECT_FALSE(_equal_128(low, high, low, high + 1));
    EXPECT_TRUE(_not_equal_128(low, high, low, high + 1));

    EXPECT_TRUE(_less_128(low, high, low + 1, high));
    EXPECT_TRUE(_less_equal_128(low, high, low, high));
    EXPECT_TRUE(_greater_128(low + 1, high, low, high));
    EXPECT_TRUE(_greater_equal_128(low, high, low, high));
    EXPECT_FALSE(_less_128(low, high + 1, low, high));

    EXPECT_TRUE(_equal_128_64(low, 0, low));
    EXPECT_FALSE(_equal_128_64(low, 1, low));
    EXPECT_TRUE(_not_equal_128_64(low, 1, low));
    EXPECT_TRUE(_less_128_64(low, 0, low + 1));
    EXPECT_TRUE(_less_equal_128_64(low, 0, low));
    EXPECT_TRUE(_greater_128_64(low + 1, 0, low));
    EXPECT_TRUE(_greater_equal_128_64(low, 0, low));
}

TEST(compare, compare_128_borrow_and_preserved_operands) {
    constexpr std::array<uint64_t, 7> values = {
        0, 1, 2, (UINT64_C(1) << 63) - 1, UINT64_C(1) << 63, UINT64_MAX - 1, UINT64_MAX};
    for (auto hi0 : values) {
        for (auto lo0 : values) {
            for (auto hi1 : values) {
                for (auto lo1 : values) {
                    check_128_comparison(lo0, hi0, lo1, hi1);
                }
            }
        }
    }

    std::mt19937_64 random(42);
    for (size_t i = 0; i < 10000; ++i) {
        const auto lo0 = random();
        const auto hi0 = random();
        const auto lo1 = random();
        const auto hi1 = random();
        check_128_comparison(lo0, hi0, lo1, hi1);
        check_128_comparison(lo0, hi0, lo1, hi0);
        check_128_comparison(lo0, hi0, lo0, hi0);
    }
}
