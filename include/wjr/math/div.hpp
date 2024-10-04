#ifndef WJR_MATH_DIV_HPP__
#define WJR_MATH_DIV_HPP__

#include <wjr/math/uint128_t.hpp>

#if defined(WJR_X86)
    #include <wjr/arch/x86/math/div.hpp>
#endif

namespace wjr {

/*
 TODO :
 1. __div_constant_128
 2. __mod_constant_128
 3. __div_qr_constant_128
 4. optimize constant divisor of div_qr_1
 1, 2, 3, 4: constant numbers that can be divisible by (uint64_t)(-1),
*/

WJR_INLINE_CONSTEXPR20 uint64_t
div128by64to64_noshift(uint64_t &rem, uint64_t lo, uint64_t hi,
                       const wjr::div2by1_divider_noshift<uint64_t> &divider) noexcept {
    const uint64_t result = divider.divide(lo, hi);
    rem = hi;
    return result;
}

WJR_INLINE_CONSTEXPR20 uint64_t
div128by64to64_shift(uint64_t &rem, uint64_t lo, uint64_t hi,
                     const wjr::div2by1_divider<uint64_t> &divider) noexcept {
    const auto shift = divider.get_shift();
    hi = shld(hi, lo, shift);
    lo <<= shift;
    const uint64_t result = divider.get_base().divide(lo, hi);
    rem = hi >> shift;
    return result;
}

WJR_INLINE_CONSTEXPR20 uint64_t
div128by64to64_impl(uint64_t &rem, uint64_t lo, uint64_t hi,
                    const wjr::div2by1_divider<uint64_t> &divider) noexcept {
    if (divider.get_shift() == 0) {
        return div128by64to64_noshift(rem, lo, hi, divider);
    }

    return div128by64to64_shift(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
WJR_INLINE_CONSTEXPR20 uint64_t
div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
               const div2by1_divider<uint64_t> &divider) noexcept {
    return div128by64to64_impl(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
WJR_INLINE_CONSTEXPR20 uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                                               uint64_t div) noexcept {
    return div128by64to64_impl(rem, lo, hi, wjr::div2by1_divider<uint64_t>(div));
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

    q0 = divider.divide(lo, hi);
    rem = hi;
    return {q0, q1};
}

inline uint128_t
div128by64to128_shift(uint64_t &rem, uint64_t lo, uint64_t hi,
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

inline uint128_t div128by64to128_impl(uint64_t &rem, uint64_t lo, uint64_t hi,
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
inline uint128_t div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                                 const div2by1_divider<uint64_t> &divider) noexcept {
    return div128by64to128_impl(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline uint128_t div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                                 uint64_t div) noexcept {
    return div128by64to128_impl(rem, lo, hi, div2by1_divider<uint64_t>(div));
}

// reference : https://ieeexplore.ieee.org/document/5487506
inline uint64_t div_qr_1_noshift(uint64_t *dst, uint64_t &rem, const uint64_t *src,
                                 size_t n,
                                 const div2by1_divider_noshift<uint64_t> &div) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_DECR_P(dst, n - 1, src, n - 1));

    const uint64_t divisor = div.get_divisor();
    const uint64_t value = div.get_value();

    uint64_t qh = 0;
    uint64_t lo, hi;

    hi = src[n - 1];

    if (hi >= divisor) {
        hi -= divisor;
        qh = 1;
    }

    do {
        if (WJR_UNLIKELY(n == 1)) {
            break;
        }

        --n;

        do {
            lo = src[n - 1];
            dst[n - 1] = div.divide(divisor, value, lo, hi);
            --n;
        } while (WJR_LIKELY(n != 0));

    } while (false);

    rem = hi;
    return qh;
}

extern uint64_t div_qr_1_shift(uint64_t *dst, uint64_t &rem, const uint64_t *src,
                               size_t n, const div2by1_divider<uint64_t> &div) noexcept;

WJR_INTRINSIC_INLINE uint64_t
div_qr_1_impl(uint64_t *dst, uint64_t &rem, const uint64_t *src, size_t n,
              const div2by1_divider<uint64_t> &div) noexcept {
    if (div.get_shift() == 0) {
        return div_qr_1_noshift(dst, rem, src, n, div);
    }

    return div_qr_1_shift(dst, rem, src, n, div);
}

// return high quotient limb
WJR_INTRINSIC_INLINE void div_qr_1(uint64_t *dst, uint64_t &rem, const uint64_t *src,
                                   size_t n,
                                   const div2by1_divider<uint64_t> &div) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(div.is_zero_or_single_bit())) {
        const unsigned int c = 63 - div.get_shift();
        rem = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n == 2)) {
        const auto [ax, dx] = div128by64to128(rem, src[0], src[1], div);
        dst[0] = ax;
        dst[1] = dx;
        return;
    }

    dst[n - 1] = div_qr_1_impl(dst, rem, src, n, div);
}

WJR_INTRINSIC_INLINE void div_qr_1(uint64_t *dst, uint64_t &rem, const uint64_t *src,
                                   size_t n, type_identity_t<uint64_t> div) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(div != 0);

    if (WJR_UNLIKELY(is_zero_or_single_bit(div))) {
        const unsigned int c = ctz(div);
        rem = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    if (WJR_UNLIKELY(n == 1)) {
        const uint64_t tmp = src[0];

        if (__has_high_bit(div)) {
            if (tmp >= div) {
                rem = tmp - div;
                dst[0] = 1;
                return;
            }
            rem = tmp;
            dst[0] = 0;
            return;
        }

        dst[0] = tmp / div;
        rem = tmp % div;
        return;
    }

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n == 2)) {
        const auto [ax, dx] = div128by64to128(rem, src[0], src[1], div);
        dst[0] = ax;
        dst[1] = dx;
        return;
    }

    dst[n - 1] = div_qr_1_impl(dst, rem, src, n, div2by1_divider<uint64_t>(div));
}

extern uint64_t div_qr_2_noshift(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                                 size_t n,
                                 const div3by2_divider_noshift<uint64_t> &div) noexcept;

extern uint64_t div_qr_2_shift(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                               size_t n, const div3by2_divider<uint64_t> &div) noexcept;

WJR_INTRINSIC_INLINE uint64_t
div_qr_2_impl(uint64_t *dst, uint64_t *rem, const uint64_t *src, size_t n,
              const div3by2_divider<uint64_t> &div) noexcept {
    if (div.get_shift() == 0) {
        return div_qr_2_noshift(dst, rem, src, n, div);
    }

    return div_qr_2_shift(dst, rem, src, n, div);
}

WJR_INTRINSIC_INLINE void div_qr_2(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                                   size_t n,
                                   const div3by2_divider<uint64_t> &div) noexcept {
    WJR_ASSERT_ASSUME(n >= 2);

    dst[n - 2] = div_qr_2_impl(dst, rem, src, n, div);
}

WJR_INTRINSIC_INLINE void div_qr_2(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                                   size_t n, const uint64_t *div) noexcept {
    WJR_ASSERT_ASSUME(n >= 2);

    dst[n - 2] =
        div_qr_2_impl(dst, rem, src, n, div3by2_divider<uint64_t>(div[0], div[1]));
}

// reference : GMP
// return qh;
extern uint64_t sb_div_qr_s(uint64_t *dst, uint64_t *src, size_t n, const uint64_t *div,
                            size_t m, uint64_t dinv) noexcept;

extern uint64_t dc_div_qr_s(uint64_t *dst, uint64_t *src, size_t n, const uint64_t *div,
                            size_t m, uint64_t dinv) noexcept;

extern void __div_qr_s_impl(uint64_t *dst, uint64_t *rem, const uint64_t *src, size_t n,
                            const uint64_t *div, size_t m) noexcept;

WJR_INTRINSIC_INLINE void div_qr_s(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                                   size_t n, const uint64_t *div, size_t m) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P(m)) {
        switch (m) {
        case 0: {
            WJR_UNREACHABLE();
            break;
        }
        case 1: {
            return div_qr_1(dst, rem[0], src, n, div[0]);
        }
        case 2: {
            return div_qr_2(dst, rem, src, n, div);
        }
        default: {
            break;
        }
        }
    }

    return __div_qr_s_impl(dst, rem, src, n, div, m);
}

WJR_INTRINSIC_CONSTEXPR20 uint64_t fallback_divexact_dbm1c(uint64_t *dst,
                                                           const uint64_t *src, size_t n,
                                                           uint64_t bd,
                                                           uint64_t h) noexcept {
    uint64_t a = 0;
    uint64_t p0, p1, cf;

    // GCC can't optimize well
    WJR_UNROLL(4)
    for (size_t i = 0; i < n; i++) {
        a = src[i];
        p0 = mul(a, bd, p1);
        h = subc(h, p0, 0, cf);
        dst[i] = h;
        h -= p1 + cf;
    }

    return h;
}

WJR_INTRINSIC_CONSTEXPR20 uint64_t divexact_dbm1c(uint64_t *dst, const uint64_t *src,
                                                  size_t n, uint64_t bd,
                                                  uint64_t h) noexcept {
#if WJR_HAS_BUILTIN(ASM_DIVEXACT_DBM1C)
    if (is_constant_evaluated()) {
        return fallback_divexact_dbm1c(dst, src, n, bd, h);
    }

    return asm_divexact_dbm1c(dst, src, n, bd, h);
#else
    return fallback_divexact_dbm1c(dst, src, n, bd, h);
#endif
}

// reference : ftp://ftp.risc.uni-linz.ac.at/pub/techreports/1992/92-35.ps.gz
WJR_INLINE_CONSTEXPR20
void fallback_divexact_1_noshift(uint64_t *dst, const uint64_t *src, size_t n,
                                 const divexact1_divider<uint64_t> &div) noexcept {
    const uint64_t divisor = div.get_divisor();
    const uint64_t value = div.get_value();

    uint64_t rdx = 0, r10 = 0;
    uint64_t cf = 0;
    size_t idx = 0;

    --n;

    if (WJR_LIKELY(n != 0)) {
        do {
            r10 = src[idx];
            r10 = subc(r10, rdx, cf, cf);
            r10 = mullo(r10, value);
            dst[idx] = r10;
            ++idx;
            rdx = mulhi(r10, divisor);
        } while (WJR_LIKELY(idx != n));
    }

    r10 = src[n];
    r10 -= rdx + cf;
    r10 = mullo(r10, value);
    dst[n] = r10;
    return;
}

WJR_INLINE_CONSTEXPR20 void
fallback_divexact_1_shift(uint64_t *dst, const uint64_t *src, size_t n,
                          const divexact1_divider<uint64_t> &div,
                          uint64_t hicf = 0) noexcept {
    const uint64_t divisor = div.get_divisor();
    const uint64_t value = div.get_value();
    const auto shift = div.get_shift();

    uint64_t rdx = 0, r10 = 0;
    uint64_t cf = 0;
    size_t idx = 0;

    --n;
    r10 = src[0];

    if (WJR_LIKELY(n != 0)) {
        do {
            uint64_t r11 = src[idx + 1];
            r10 = shrd(r10, r11, shift);
            r10 = subc(r10, rdx, cf, cf);
            r10 = mullo(r10, value);
            dst[idx] = r10;
            ++idx;
            rdx = mulhi(r10, divisor);
            r10 = r11;
        } while (WJR_LIKELY(idx != n));
    }

    r10 = shrd(r10, hicf, shift);
    r10 -= rdx + cf;
    r10 = mullo(r10, value);
    dst[n] = r10;
    return;
}

template <uint64_t c>
WJR_INTRINSIC_CONSTEXPR uint64_t __divexact_get_impl() noexcept {
    return 1;
}

template <uint64_t c, uint64_t p, uint64_t... ps>
WJR_INTRINSIC_CONSTEXPR uint64_t __divexact_get_impl() noexcept {
    constexpr auto ret = __divexact_get_impl<c, ps...>();
    if constexpr (c % p == 0) {
        return ret * p;
    } else {
        return ret;
    }
}

template <uint64_t c>
WJR_INTRINSIC_CONSTEXPR uint64_t __divexact_get() noexcept {
    return __divexact_get_impl<c, 3, 5, 17>();
}

struct __divexact_get_struct {
    int mode;
    int cl;
    uint64_t p0, p1;
};

template <uint64_t c>
constexpr __divexact_get_struct __divexact_init() noexcept {
    if constexpr (is_zero_or_single_bit(c)) {
        return {0, constant::ctz(c), 0, 0};
    } else {
        constexpr auto p0 = __divexact_get<c>();
        if constexpr (p0 == 1) {
            return {1, 0, 0, 0};
        } else {
            constexpr auto c0 = c / p0;
            if constexpr (is_zero_or_single_bit(c0)) {
                return {2, constant::ctz(c), p0, 0};
            } else {
                constexpr auto p1 = __divexact_get<c0>();
                if constexpr (p1 == 1) {
                    return {1, 0, 0, 0};
                } else {
                    constexpr auto c1 = c0 / p1;
                    if constexpr (is_zero_or_single_bit(c1)) {
                        return {3, constant::ctz(c1), p0, p1};
                    } else {
                        return {1, 0, 0, 0};
                    }
                }
            }
        }
    }
}

template <uint64_t c>
WJR_INTRINSIC_CONSTEXPR20 void divexact_byc(uint64_t *dst, const uint64_t *src, size_t n,
                                            integral_constant<uint64_t, c>,
                                            uint64_t cf) noexcept {
    // cost : divexact_dbm1c * 2 + shift * 1 <= divexact_1
    static_assert(c != 0);
    constexpr auto ss = __divexact_init<c>();

    if constexpr (ss.mode == 0) {
        (void)rshift_n(dst, src, n, ss.cl, cf);
    }

    if constexpr (ss.mode == 1) {
        constexpr auto shift = constant::ctz(c);
        using divider_t = divexact1_divider<uint64_t>;
        constexpr auto divisor = c >> shift;
        constexpr auto value = divider_t::reciprocal(divisor);
        constexpr auto divider = divider_t(divisor, value, shift);

        static_assert(!divider.is_zero_or_single_bit());

        if constexpr (shift == 0) {
            fallback_divexact_1_noshift(dst, src, n, divider);
        } else {
            fallback_divexact_1_shift(dst, src, n, divider, cf);
        }
    }

    if constexpr (ss.mode == 2 || ss.mode == 3) {
        constexpr uint64_t maxn = UINT64_MAX;

        if constexpr (ss.cl == 0) {
            (void)divexact_dbm1c(dst, src, n, maxn / ss.p0, 0);
        } else {
            (void)rshift_n(dst, src, n, ss.cl, cf);
            (void)divexact_dbm1c(dst, dst, n, maxn / ss.p0, 0);
        }

        if constexpr (ss.mode == 3) {
            (void)divexact_dbm1c(dst, dst, n, maxn / ss.p1, 0);
        }
    }
}

WJR_INTRINSIC_CONSTEXPR20 void
fallback_divexact_1(uint64_t *dst, const uint64_t *src, size_t n,
                    const divexact1_divider<uint64_t> &div) noexcept {
    if (div.get_shift() == 0) {
        return fallback_divexact_1_noshift(dst, src, n, div);
    }

    return fallback_divexact_1_shift(dst, src, n, div);
}

WJR_INTRINSIC_CONSTEXPR20 void
divexact_1(uint64_t *dst, const uint64_t *src, size_t n,
           const divexact1_divider<uint64_t> &div) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(div.is_zero_or_single_bit())) {
        unsigned int c = div.get_shift();
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, div);
}

WJR_INTRINSIC_CONSTEXPR20 void divexact_1(uint64_t *dst, const uint64_t *src, size_t n,
                                          uint64_t div) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(div != 0);

    if (WJR_UNLIKELY(is_zero_or_single_bit(div))) {
        const unsigned int c = ctz(div);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, divexact1_divider<uint64_t>(div));
}

WJR_PURE WJR_INLINE_CONSTEXPR20 uint64_t
mod_1_noshift(const uint64_t *src, size_t n,
              const div2by1_divider_noshift<uint64_t> &div) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);

    const uint64_t divisor = div.get_divisor();
    const uint64_t value = div.get_value();

    uint64_t lo, hi;

    hi = src[n - 1];

    if (hi >= divisor) {
        hi -= divisor;
    }

    do {
        if (WJR_UNLIKELY(n == 1)) {
            break;
        }

        --n;

        do {
            lo = src[n - 1];
            (void)div.divide(divisor, value, lo, hi);
            --n;
        } while (WJR_LIKELY(n != 0));

    } while (false);

    return hi;
}

WJR_PURE WJR_INLINE_CONSTEXPR20 uint64_t mod_1_shift(
    const uint64_t *src, size_t n, const div2by1_divider<uint64_t> &div) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(div.get_shift() != 0);

    const uint64_t divisor = div.get_divisor();
    const uint64_t value = div.get_value();
    const auto shift = div.get_shift();

    uint64_t lo, hi;

    uint64_t rbp = src[n - 1];
    --n;
    hi = rbp >> (64 - shift);

    do {
        if (WJR_UNLIKELY(n == 0)) {
            (void)div.divide(divisor, value, rbp << shift, hi);
            break;
        }

        lo = src[n - 1];
        (void)div.divide(divisor, value, shld(rbp, lo, shift), hi);
        rbp = lo;
        --n;

        if (WJR_LIKELY(n != 0)) {
            do {
                lo = src[n - 1];
                (void)div.divide(divisor, value, shld(rbp, lo, shift), hi);
                rbp = lo;
                --n;
            } while (WJR_LIKELY(n != 0));
        }

        (void)div.divide(divisor, value, rbp << shift, hi);
    } while (false);

    return hi >> shift;
}

WJR_PURE WJR_INTRINSIC_CONSTEXPR20 uint64_t
mod_1_impl(const uint64_t *src, size_t n, const div2by1_divider<uint64_t> &div) noexcept {
    if (div.get_shift() == 0) {
        return mod_1_noshift(src, n, div);
    }

    return mod_1_shift(src, n, div);
}

WJR_INTRINSIC_CONSTEXPR20 uint64_t mod_1(const uint64_t *src, size_t n,
                                         const div2by1_divider<uint64_t> &div) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(div.is_zero_or_single_bit())) {
        const unsigned int c = 63 - div.get_shift();
        return src[0] & ((1ull << c) - 1);
    }

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n == 2)) {
        uint64_t rem;
        (void)div128by64to128(rem, src[0], src[1], div);
        return rem;
    }

    return mod_1_impl(src, n, div);
}

WJR_INTRINSIC_CONSTEXPR20 uint64_t mod_1(const uint64_t *src, size_t n,
                                         type_identity_t<uint64_t> div) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(div != 0);

    if (WJR_UNLIKELY(is_zero_or_single_bit(div))) {
        const unsigned int c = ctz(div);
        return src[0] & ((1ull << c) - 1);
    }

    if (WJR_UNLIKELY(n == 1)) {
        const uint64_t tmp = src[0];

        if (__has_high_bit(div)) {
            if (tmp >= div) {
                return tmp - div;
            }

            return tmp;
        }

        return tmp % div;
    }

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n == 2)) {
        uint64_t rem;
        (void)div128by64to128(rem, src[0], src[1], div);
        return rem;
    }

    return mod_1_impl(src, n, div2by1_divider<uint64_t>(div));
}

} // namespace wjr

#endif // WJR_MATH_DIV_HPP__