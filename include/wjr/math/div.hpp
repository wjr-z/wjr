/**
 * @file div.hpp
 * @brief Division operations for multi-precision arithmetic
 * @author wjr
 * @version 0.1
 * @date 2025-10-06
 *
 * This file contains optimized division functions for 128-bit by 64-bit and
 * 128-bit by 128-bit operations. These functions are critical for implementing
 * arbitrary-precision arithmetic.
 *
 * @todo
 * - Implement _div_constant_128 for constant division optimization
 * - Implement _mod_constant_128 for constant modulo optimization
 * - Implement _div_qr_constant_128 for combined quotient and remainder
 * - Optimize constant divisor of div_qr_1 for edge cases
 * - Handle constant numbers divisible by (uint64_t)(-1)
 */

#ifndef WJR_MATH_DIV_HPP__
#define WJR_MATH_DIV_HPP__

#include <wjr/math/uint128_t.hpp>

namespace wjr {

/**
 * @brief Divide 128-bit by 64-bit (optimized for normalized divisor)
 *
 * Fast path division when divisor is already normalized (MSB set).
 * Users may call this directly when they know the divisor is normalized.
 *
 * @param[out] rem Reference to store the remainder
 * @param[in] lo Lower 64 bits of dividend
 * @param[in] hi Higher 64 bits of dividend
 * @param[in] divider Precomputed normalized divider
 * @return uint64_t The 64-bit quotient
 */
WJR_INLINE_CONSTEXPR20 uint64_t
div128by64to64_noshift(uint64_t &rem, uint64_t lo, uint64_t hi,
                       const div2by1_divider_noshift<uint64_t> &divider) noexcept {
    const uint64_t result = divider.divide(lo, hi);
    rem = hi;
    return result;
}

/**
 * @brief Divide 128-bit by 64-bit (with normalization)
 *
 * Division path for non-normalized divisors. Performs shift normalization.
 * Users may call this when divisor requires normalization.
 *
 * @param[out] rem Reference to store the remainder
 * @param[in] lo Lower 64 bits of dividend
 * @param[in] hi Higher 64 bits of dividend
 * @param[in] divider Precomputed divider with shift information
 * @return uint64_t The 64-bit quotient
 */
WJR_INLINE_CONSTEXPR20 uint64_t div128by64to64_shift(
    uint64_t &rem, uint64_t lo, uint64_t hi, const div2by1_divider<uint64_t> &divider) noexcept {
    const auto shift = divider.get_shift();
    hi = shld(hi, lo, shift);
    lo <<= shift;
    const uint64_t result = divider.get_base().divide(lo, hi);
    rem = hi >> shift;
    return result;
}

/**
 * @brief Divide 128-bit number by 64-bit number (general version)
 *
 * Automatically selects the optimized division path based on whether the
 * divisor requires shift normalization. Dispatches to either the noshift
 * or shift version accordingly.
 *
 * @param[out] rem Reference to store the remainder of the division
 * @param[in] lo Lower 64 bits of the dividend
 * @param[in] hi Higher 64 bits of the dividend
 * @param[in] divider Precomputed divider object
 * @return uint64_t The 64-bit quotient
 * @warning Not optimized for power-of-2 divisors; consider manual optimization
 * @note For power-of-2 divisors, bit shifting may be more efficient
 */
WJR_INLINE_CONSTEXPR20 uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                                               const div2by1_divider<uint64_t> &divider) noexcept {
    if (divider.get_shift() == 0) {
        return div128by64to64_noshift(rem, lo, hi, divider);
    }

    return div128by64to64_shift(rem, lo, hi, divider);
}

/**
 * @brief Divide 128-bit number by 64-bit number (convenience overload)
 *
 * Convenience wrapper that constructs the divider object on-the-fly.
 * For repeated divisions by the same divisor, prefer constructing the
 * divider once and reusing it.
 *
 * @param[out] rem Reference to store the remainder of the division
 * @param[in] lo Lower 64 bits of the dividend
 * @param[in] hi Higher 64 bits of the dividend
 * @param[in] div The 64-bit divisor value
 * @return uint64_t The 64-bit quotient
 * @warning Not optimized for power-of-2 divisors; consider manual optimization
 * @note This creates a temporary divider; cache the divider for better performance
 */
WJR_INLINE_CONSTEXPR20 uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                                               uint64_t div) noexcept {
    return div128by64to64(rem, lo, hi, div2by1_divider<uint64_t>(div));
}

/**
 * @brief Divide 128-bit by 64-bit producing 128-bit quotient (normalized)
 *
 * For cases where quotient exceeds 64 bits. Optimized for normalized divisor.
 *
 * @param[out] rem Reference to store the remainder
 * @param[in] lo Lower 64 bits of dividend
 * @param[in] hi Higher 64 bits of dividend
 * @param[in] divider Precomputed normalized divider
 * @return uint128_t The 128-bit quotient {low, high}
 */
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

/**
 * @brief Divide 128-bit by 64-bit producing 128-bit quotient (with normalization)
 *
 * For cases where quotient exceeds 64 bits and divisor needs normalization.
 *
 * @param[out] rem Reference to store the remainder
 * @param[in] lo Lower 64 bits of dividend
 * @param[in] hi Higher 64 bits of dividend
 * @param[in] divider Precomputed divider with shift
 * @return uint128_t The 128-bit quotient {low, high}
 */
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

/**
 * @brief Divide 128-bit by 64-bit to produce 128-bit quotient (general)
 *
 * Automatically dispatches to the appropriate implementation based on
 * whether shift normalization is required.
 *
 * @param[out] rem Reference to store the 64-bit remainder
 * @param[in] lo Lower 64 bits of the dividend
 * @param[in] hi Higher 64 bits of the dividend
 * @param[in] divider Precomputed divider object
 * @return uint128_t The 128-bit quotient
 * @warning Not optimized for power-of-2 divisors; consider manual optimization
 */
inline uint128_t div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                                 const div2by1_divider<uint64_t> &divider) noexcept {
    if (divider.get_shift() == 0) {
        return div128by64to128_noshift(rem, lo, hi, divider);
    }

    return div128by64to128_shift(rem, lo, hi, divider);
}

/**
 * @brief Divide 128-bit by 64-bit to produce 128-bit quotient (convenience)
 *
 * Convenience overload that constructs the divider on-the-fly. For multiple
 * divisions by the same divisor, construct and cache the divider object.
 *
 * @param[out] rem Reference to store the 64-bit remainder
 * @param[in] lo Lower 64 bits of the dividend
 * @param[in] hi Higher 64 bits of the dividend
 * @param[in] div The 64-bit divisor value
 * @return uint128_t The 128-bit quotient
 * @warning Not optimized for power-of-2 divisors; consider manual optimization
 */
inline uint128_t div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi, uint64_t div) noexcept {
    return div128by64to128(rem, lo, hi, div2by1_divider<uint64_t>(div));
}

/**
 * @brief Divide 128-bit by 128-bit (normalized divisor)
 *
 * Optimized path when the 128-bit divisor is normalized.
 *
 * @param[out] rem Reference to store the 128-bit remainder
 * @param[in] lo Lower 64 bits of dividend
 * @param[in] hi Higher 64 bits of dividend
 * @param[in] divider Precomputed normalized 3-by-2 divider
 * @return uint64_t The quotient (0 or 1)
 */
inline uint64_t div128by128to64_noshift(uint128_t &rem, uint64_t lo, uint64_t hi,
                                        const div3by2_divider_noshift<uint64_t> &divider) {
    const uint64_t divisor0 = divider.get_divisor0();
    const uint64_t divisor1 = divider.get_divisor1();

    uint64_t qh = 0;
    if (_less_equal_128(divisor0, divisor1, lo, hi)) {
        sub_128(lo, hi, lo, hi, divisor0, divisor1);
        qh = 1;
    }

    rem.low = lo;
    rem.high = hi;
    return qh;
}

/**
 * @brief Divide 128-bit by 128-bit (with normalization)
 *
 * Division path when the 128-bit divisor requires normalization.
 *
 * @param[out] rem Reference to store the 128-bit remainder
 * @param[in] lo Lower 64 bits of dividend
 * @param[in] hi Higher 64 bits of dividend
 * @param[in] divider Precomputed 3-by-2 divider with shift
 * @return uint64_t The quotient
 */
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

/**
 * @brief Divide 128-bit by 128-bit to produce 64-bit quotient (general)
 *
 * Automatically dispatches to the appropriate implementation based on
 * whether the divisor requires shift normalization.
 *
 * @param[out] rem Reference to store the 128-bit remainder
 * @param[in] lo Lower 64 bits of the dividend
 * @param[in] hi Higher 64 bits of the dividend
 * @param[in] divider Precomputed 3-by-2 divider object
 * @return uint64_t The 64-bit quotient
 */
inline uint64_t div128by128to64(uint128_t &rem, uint64_t lo, uint64_t hi,
                                const div3by2_divider<uint64_t> &divider) noexcept {
    if (divider.get_shift() == 0) {
        return div128by128to64_noshift(rem, lo, hi, divider);
    }

    return div128by128to64_shift(rem, lo, hi, divider);
}

} // namespace wjr

#endif // WJR_MATH_DIV_HPP__