#ifndef WJR_MATH_CONVERT_HPP__
#define WJR_MATH_CONVERT_HPP__

#include <wjr/math/div.hpp>

namespace wjr {

inline constexpr size_t dc_biginteger_to_string_threshold =
    WJR_DC_BIGINTEGER_TO_STRING_THRESHOLD;

inline constexpr auto div2by1_divider_of_big_base_10 = div2by1_divider<uint64_t>(
    10'000'000'000'000'000'000ull, 15'581'492'618'384'294'730ull, 0);

template <typename Iter>
WJR_INTRINSIC_INLINE Iter bc_biginteger_to_string_base10(Iter first,
                                                         WJR_MAYBE_UNUSED Iter last,
                                                         int base, uint64_t *up,
                                                         size_t n) {
    uint64_t rem;
    uint64_t q;
    do {
        q = div_qr_1_without_shift(up, rem, up, n, div2by1_divider_of_big_base_10);
        n -= q == 0;
        if (q != 0) {
            up[n - 1] = q;
        }
        for (int i = 0; i < 19; ++i) {
            WJR_ASSERT_L(1, first != last);
            *first++ = rem % 10;
            rem /= 10;
        }
    } while (n);

    return first;
}

template <typename Iter>
Iter biginteger_to_string(Iter first, WJR_MAYBE_UNUSED Iter last, int base, uint64_t *p,
                          size_t n) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(base != 0);
    WJR_ASSERT(base == 10 || is_power_of_two(base),
               "Only support simple case currently.");
    WJR_ASSERT_L(1, first != last);

    uint64_t up[WJR_DC_BIGINTEGER_TO_STRING_THRESHOLD];
    std::copy_n(p, n, up);
    return bc_biginteger_to_string_base10(first, last, base, up, n);
}

} // namespace wjr

#endif // WJR_MATH_CONVERT_HPP__