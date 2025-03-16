#ifndef WJR_MATH_DIV_HPP__
#define WJR_MATH_DIV_HPP__

#include <wjr/math/uint128_t.hpp>

namespace wjr {

/*
 todo :
 1. __div_constant_128
 2. __mod_constant_128
 3. __div_qr_constant_128
 4. optimize constant divisor of div_qr_1
 1, 2, 3, 4: constant numbers that can be divisible by (uint64_t)(-1),
*/

WJR_INLINE_CONSTEXPR20 uint64_t
div128by64to64_noshift(uint64_t &rem, uint64_t lo, uint64_t hi,
                       const div2by1_divider_noshift<uint64_t> &divider) noexcept {
    const uint64_t result = divider.divide(lo, hi);
    rem = hi;
    return result;
}

WJR_INLINE_CONSTEXPR20 uint64_t div128by64to64_shift(
    uint64_t &rem, uint64_t lo, uint64_t hi, const div2by1_divider<uint64_t> &divider) noexcept {
    const auto shift = divider.get_shift();
    hi = shld(hi, lo, shift);
    lo <<= shift;
    const uint64_t result = divider.get_base().divide(lo, hi);
    rem = hi >> shift;
    return result;
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
WJR_INLINE_CONSTEXPR20 uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                                               const div2by1_divider<uint64_t> &divider) noexcept {
    if (divider.get_shift() == 0) {
        return div128by64to64_noshift(rem, lo, hi, divider);
    }

    return div128by64to64_shift(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
WJR_INLINE_CONSTEXPR20 uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                                               uint64_t div) noexcept {
    return div128by64to64(rem, lo, hi, div2by1_divider<uint64_t>(div));
}

inline uint128_t
div128by64to128_noshift(uint64_t &rem, uint64_t lo, uint64_t hi,
                        const div2by1_divider_noshift<uint64_t> &divider) noexcept {
    const auto divisor = divider.get_divisor();
    uint64_t q0, q1 = 0;

    if (hi >= divisor) {
        q1 = 1;
        hi -= divisor;
    }

    q0 = div128by64to64_noshift(rem, lo, hi, divider);
    return {q0, q1};
}

inline uint128_t div128by64to128_shift(uint64_t &rem, uint64_t lo, uint64_t hi,
                                       const div2by1_divider<uint64_t> &divider) noexcept {
    const auto shift = divider.get_shift();
    uint64_t u0, u1, u2;
    uint64_t q0, q1;

    u2 = hi >> (64 - shift);
    u1 = shld(hi, lo, shift);
    u0 = lo << shift;

    const auto &div = divider.get_base();
    q1 = div.divide(u1, u2);
    q0 = div.divide(u0, u2);

    rem = u2 >> shift;
    return {q0, q1};
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline uint128_t div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                                 const div2by1_divider<uint64_t> &divider) noexcept {
    if (divider.get_shift() == 0) {
        return div128by64to128_noshift(rem, lo, hi, divider);
    }

    return div128by64to128_shift(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline uint128_t div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi, uint64_t div) noexcept {
    return div128by64to128(rem, lo, hi, div2by1_divider<uint64_t>(div));
}

inline uint64_t div128by128to64_noshift(uint128_t &rem, uint64_t lo, uint64_t hi,
                                        const div3by2_divider_noshift<uint64_t> &divider) {
    const uint64_t divisor0 = divider.get_divisor0();
    const uint64_t divisor1 = divider.get_divisor1();

    uint64_t qh = 0;
    if (__less_equal_128(divisor0, divisor1, lo, hi)) {
        sub_128(lo, hi, lo, hi, divisor0, divisor1);
        qh = 1;
    }

    rem.low = lo;
    rem.high = hi;
    return qh;
}

inline uint64_t div128by128to64_shift(uint128_t &rem, uint64_t lo, uint64_t hi,
                                      const div3by2_divider<uint64_t> &divider) {
    const uint64_t divisor0 = divider.get_divisor0();
    const uint64_t divisor1 = divider.get_divisor1();
    const uint64_t value = divider.get_value();
    const auto shift = divider.get_shift();

    uint64_t qh;
    uint64_t u1, u2;
    uint64_t rbp;

    rbp = lo;
    u2 = hi;
    u1 = shld(u2, rbp, shift);
    u2 >>= (64 - shift);

    qh = divider.divide(divisor0, divisor1, value, rbp << shift, u1, u2);

    rem.low = shrd(u1, u2, shift);
    rem.high = u2 >> shift;
    return qh;
}

inline uint64_t div128by128to64(uint128_t &rem, uint64_t lo, uint64_t hi,
                                const div3by2_divider<uint64_t> &divider) noexcept {
    if (divider.get_shift() == 0) {
        return div128by128to64_noshift(rem, lo, hi, divider);
    }

    return div128by128to64_shift(rem, lo, hi, divider);
}

} // namespace wjr

#endif // WJR_MATH_DIV_HPP__