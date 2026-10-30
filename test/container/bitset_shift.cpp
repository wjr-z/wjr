#include "../detail.hpp"

#include <bitset>
#include <limits>

#include <wjr/bitset.hpp>

using namespace wjr;

namespace {
constexpr bool word_shift_is_constant_evaluated() {
    bitset<129> left;
    left.set(0).set(64).set(128);
    left <<= 64;
    if (left.count() != 2 || !left.test(64) || !left.test(128)) {
        return false;
    }
    bitset<129> right;
    right.set(0).set(64).set(128);
    right >>= 64;
    return right.count() == 2 && right.test(0) && right.test(64);
}

static_assert(word_shift_is_constant_evaluated());

template <size_t N>
void check_shift_boundaries() {
    bitset<N> original;
    std::bitset<N> expected;
    for (size_t i = 0; i < N; ++i) {
        if (i % 3 == 0 || i == N - 1) {
            original.set(i);
            expected.set(i);
        }
    }

    const size_t offsets[] = {0,   1,   2,     31,  32,    33,
                              63,  64,  65,    127, 128,   191,
                              192, 193, N - 1, N,   N + 1, std::numeric_limits<size_t>::max()};
    for (const size_t offset : offsets) {
        SCOPED_TRACE(N);
        SCOPED_TRACE(offset);
        auto left = original;
        EXPECT_EQ(&(left <<= offset), &left);
        EXPECT_EQ(left.to_string(), (expected << offset).to_string());
        EXPECT_EQ((original << offset).to_string(), left.to_string());

        auto right = original;
        EXPECT_EQ(&(right >>= offset), &right);
        EXPECT_EQ(right.to_string(), (expected >> offset).to_string());
        EXPECT_EQ((original >> offset).to_string(), right.to_string());
    }
}
} // namespace

TEST(bitset, full_range_shifts) {
    check_shift_boundaries<1>();
    check_shift_boundaries<8>();
    check_shift_boundaries<31>();
    check_shift_boundaries<32>();
    check_shift_boundaries<33>();
    check_shift_boundaries<63>();
    check_shift_boundaries<64>();
    check_shift_boundaries<65>();
    check_shift_boundaries<127>();
    check_shift_boundaries<128>();
    check_shift_boundaries<129>();
    check_shift_boundaries<192>();
    check_shift_boundaries<255>();
    check_shift_boundaries<256>();
    check_shift_boundaries<257>();
}
