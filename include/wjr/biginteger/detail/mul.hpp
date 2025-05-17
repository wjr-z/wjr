#ifndef WJR_BIGINTEGER_DETAIL_MUL_HPP__
#define WJR_BIGINTEGER_DETAIL_MUL_HPP__

/**
 * @todo optimize temporary memory usage of mul_s, mul_n, sqr
 *
 */

#include <wjr/biginteger/detail/add.hpp>
#include <wjr/biginteger/detail/bignum-config.hpp>
#include <wjr/biginteger/detail/sub.hpp>
#include <wjr/math/bit.hpp>
#include <wjr/math/mul.hpp>
#include <wjr/math/shift.hpp>

#if defined(_MSC_VER) && defined(WJR_ARCH_X86)
    #define WJR_HAS_BUILTIN_MSVC_MULH64 WJR_HAS_DEF
#endif

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/biginteger/detail/mul.hpp>
#endif

#if WJR_HAS_BUILTIN(MSVC_MULH64)
    #include <wjr/arch/x86/simd/intrin.hpp>
#endif

namespace wjr {
#if !WJR_HAS_BUILTIN(ASM_MUL_1)
WJR_INTRINSIC_INLINE uint64_t fallback_mul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                             uint64_t ml) noexcept {
    uint64_t lo = 0, hi = 0;
    uint64_t c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        dst[i] = addc(lo, c_in, 0, c_in);
        c_in += hi;
    }

    return c_in;
}
#endif

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
WJR_INTRINSIC_INLINE uint64_t mul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                    uint64_t ml) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(ml == 1)) {
        if (src != dst) {
            std::copy(src, src + n, dst);
        }

        return 0;
    }

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(is_zero_or_single_bit(ml))) {
        if (ml == 0) {
            set_n(dst, 0, n);
            return 0;
        }

        const unsigned int c = to_unsigned(ctz(ml));
        return lshift_n(dst, src, n, c);
    }

#if WJR_HAS_BUILTIN(ASM_MUL_1)
    return asm_mul_1(dst, src, n, ml);
#else
    return fallback_mul_1(dst, src, n, ml);
#endif
}

// dst = src0 + (src1 << cl)
WJR_INTRINSIC_INLINE uint64_t addlsh_n(uint64_t *dst, const uint64_t *src0, const uint64_t *src1,
                                       size_t n, uint64_t cl) noexcept;

// dst = (src1 << cl) - src0
WJR_INTRINSIC_INLINE uint64_t rsblsh_n(uint64_t *dst, const uint64_t *src0, const uint64_t *src1,
                                       size_t n, uint64_t cl) noexcept;

#if !WJR_HAS_BUILTIN(ASM_ADDMUL_1)
WJR_INTRINSIC_INLINE uint64_t fallback_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                                uint64_t ml) noexcept {
    uint64_t lo = 0, hi = 0;
    uint64_t o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        lo = addc(lo, c_in, 0, c_in);
        dst[i] = addc(lo, dst[i], 0, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}
#endif

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
WJR_INTRINSIC_INLINE uint64_t addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                       uint64_t ml) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(ml == 1)) {
        return addc_n(dst, dst, src, n);
    }

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(is_zero_or_single_bit(ml))) {
        if (ml == 0) {
            return 0;
        }

        const unsigned int c = ctz(ml);
        return addlsh_n(dst, dst, src, n, c);
    }

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
    return asm_addmul_1(dst, src, n, ml);
#else
    return fallback_addmul_1(dst, src, n, ml);
#endif
}

#if !WJR_HAS_BUILTIN(ASM_SUBMUL_1)
WJR_INTRINSIC_INLINE uint64_t fallback_submul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                                uint64_t ml) noexcept {
    uint64_t lo = 0, hi = 0;
    uint64_t o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        lo = addc(lo, c_in, 0, c_in);
        dst[i] = subc(dst[i], lo, 0, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}
#endif

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
WJR_INTRINSIC_INLINE uint64_t submul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                       uint64_t ml) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(ml == 0)) {
        return 0;
    }

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(ml == 1)) {
        return subc_n(dst, dst, src, n);
    }

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)
    return asm_submul_1(dst, src, n, ml);
#else
    return fallback_submul_1(dst, src, n, ml);
#endif
}

#if !WJR_HAS_BUILTIN(ASM_ADDLSH_N)
WJR_INTRINSIC_INLINE uint64_t fallback_addlsh_n(uint64_t *dst, const uint64_t *src0,
                                                const uint64_t *src1, size_t n,
                                                uint64_t cl) noexcept {
    uint64_t tcl = std::numeric_limits<uint64_t>::digits - cl;
    uint64_t lo = 0, hi = 0;
    uint64_t c_in = 0, x = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = src1[i] << cl;
        hi = src1[i] >> tcl;

        lo += x;
        dst[i] = addc(lo, src0[i], c_in, c_in);
        x = hi;
    }

    return x + c_in;
}
#endif

/*
dst = src0 + (src1 << cl);

require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
3. WJR_IS_SAME_OR_INCR_P(sdt, n, src1, n)
*/
WJR_INTRINSIC_INLINE uint64_t addlsh_n(uint64_t *dst, const uint64_t *src0, const uint64_t *src1,
                                       size_t n, uint64_t cl) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<uint64_t>::digits);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    if (WJR_UNLIKELY(cl == 0)) {
        return addc_n(dst, src0, src1, n);
    }

#if WJR_HAS_BUILTIN(ASM_ADDLSH_N)
    return asm_addlsh_n(dst, src0, src1, n, cl);
#else
    return fallback_addlsh_n(dst, src0, src1, n, cl);
#endif
}

#if !WJR_HAS_BUILTIN(ASM_RSBLSH_N)
WJR_INTRINSIC_INLINE uint64_t fallback_rsblsh_n(uint64_t *dst, const uint64_t *src0,
                                                const uint64_t *src1, size_t n,
                                                uint64_t cl) noexcept {
    uint64_t tcl = std::numeric_limits<uint64_t>::digits - cl;
    uint64_t lo = 0, hi = 0;
    uint64_t c_in = 0, x = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = src1[i] << cl;
        hi = src1[i] >> tcl;

        lo += x;
        dst[i] = subc(lo, src0[i], c_in, c_in);
        x = hi;
    }

    return x - c_in;
}
#endif

/*
dst = (src1 << cl) - src0;

require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
3. WJR_IS_SAME_OR_INCR_P(sdt, n, src1, n)
*/
WJR_INTRINSIC_INLINE uint64_t rsblsh_n(uint64_t *dst, const uint64_t *src0, const uint64_t *src1,
                                       size_t n, uint64_t cl) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<uint64_t>::digits);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    if (WJR_UNLIKELY(cl == 0)) {
        return uint64_t{0} - subc_n(dst, src1, src0, n);
    }

#if WJR_HAS_BUILTIN(ASM_RSBLSH_N)
    return asm_rsblsh_n(dst, src0, src1, n, cl);
#else
    return fallback_rsblsh_n(dst, src0, src1, n, cl);
#endif
}

template <uint64_t maxn = UINT64_MAX>
WJR_INTRINSIC_INLINE uint64_t try_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                           uint64_t ml) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    WJR_ASSERT_ASSUME(ml <= maxn);

    if constexpr (maxn == 0) {
        return 0;
    } else {
        if constexpr (maxn <= 3) {
            if (ml == 0) {
                return 0;
            }

            if constexpr (maxn == 1) {
                return addc_n(dst, dst, src, n);
            } else {
                if (ml == 1) {
                    return addc_n(dst, dst, src, n);
                }

                if constexpr (maxn == 2) {
                    return addlsh_n(dst, dst, src, n, 1);
                } else {
                    if (ml == 2) {
                        return addlsh_n(dst, dst, src, n, 1);
                    }

                    return addmul_1(dst, src, n, ml);
                }
            }
        } else {
            if (WJR_UNLIKELY(ml <= 2)) {
                switch (ml) {
                case 0: {
                    return 0;
                }
                case 1: {
                    return addc_n(dst, dst, src, n);
                }
                default: {
                    return addlsh_n(dst, dst, src, n, 1);
                }
                }
            }

            return addmul_1(dst, src, n, ml);
        }
    }
}

/*
 all toom-cook need to ensure rn + rm >= l to reserve memory
 for toom-cook-u-v
 1. (v-1) * n + g <= u * m
    g = (u-1)^2 + (u-1)*v
    e.g.
    TOOM-COOK32 :
    g = 8
    n + 8 <= 3 * m
 2. v * n >= (u - 1) * m + g
    g = (v-1)^2 + (v-1)*u
    e.g.
    TOOM-COOK32 :
    g = 4
    2 * n >= 2 * m + 4 => n >= m + 2
*/

/*
 l = ceil(n/2)
 stk usage : l * 2
*/
extern WJR_ALL_NONNULL void toom22_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m, uint64_t *stk) noexcept;

extern WJR_ALL_NONNULL void toom2_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                                      uint64_t *stk) noexcept;

/*
 l = ceil(n/3)
 stk usage : l * 4
*/
extern WJR_ALL_NONNULL void toom33_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m, uint64_t *stk) noexcept;

extern WJR_ALL_NONNULL void toom3_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                                      uint64_t *stk) noexcept;

/**
 * @details \n
 * l = max(ceil(n/3), ceil(m/2)) \n
 * stk usage : l * 4
 *
 */
extern WJR_ALL_NONNULL void toom32_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m, uint64_t *stk) noexcept;

/*
l = max(ceil(n/4), ceil(m/2))
stk usage : l * 4
*/
extern WJR_ALL_NONNULL void toom42_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m, uint64_t *stk) noexcept;

/*
 l = max(ceil(n/4), ceil(m/3))
 stk usage : l * 6
*/
extern WJR_ALL_NONNULL void toom43_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m, uint64_t *stk) noexcept;

/*
 l = max(ceil(n/5), ceil(m/3))
 stk usage : l * 6
*/
extern WJR_ALL_NONNULL void toom53_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m, uint64_t *stk) noexcept;

/*
 l = ceil(n/4)
 stk usage : l * 6
*/
extern WJR_ALL_NONNULL void toom44_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m, uint64_t *stk) noexcept;

extern WJR_ALL_NONNULL void toom4_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                                      uint64_t *stk) noexcept;

/**
 * @details \n
 * l = max(ceil(n/6), ceil(m/3)) \n
 * stk usage : l * 10 \n
 *
 */
extern WJR_ALL_NONNULL void toom63_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m, uint64_t *stk) noexcept;

/*
 l = ceil(n/5)
 stk usage : l * 10
*/
extern WJR_ALL_NONNULL void toom55_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m, uint64_t *stk) noexcept;

/*
 l = ceil(n/5)
 stk usage : l * 10
*/
extern WJR_ALL_NONNULL void toom5_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                                      uint64_t *stk) noexcept;

extern WJR_ALL_NONNULL void nussbaumer_mul(uint64_t *pp, const uint64_t *ap, size_t an,
                                           const uint64_t *bp, size_t bn) noexcept;

extern WJR_ALL_NONNULL uint64_t mul_fft(uint64_t *op, size_t pl, const uint64_t *n, size_t nl,
                                        const uint64_t *m, size_t ml, int k) noexcept;

extern WJR_ALL_NONNULL void __mul_s_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m) noexcept;

extern WJR_ALL_NONNULL void __mul_n_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                         const uint64_t *src1, size_t n) noexcept;

extern WJR_ALL_NONNULL void __sqr_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                       size_t n) noexcept;

extern WJR_CONST int fft_best_k(size_t n, int sqr) noexcept;

extern WJR_PURE WJR_ALL_NONNULL uint64_t mod_34lsub1(const uint64_t *p, size_t n) noexcept;

extern WJR_ALL_NONNULL void mulmod_bknp1(uint64_t *rp, const uint64_t *ap, const uint64_t *bp,
                                         size_t n, unsigned k, uint64_t *tp) noexcept;

extern WJR_ALL_NONNULL void sqrmod_bknp1(uint64_t *rp, const uint64_t *ap, size_t n, unsigned k,
                                         uint64_t *tp) noexcept;

extern WJR_ALL_NONNULL void mulmod_bnm1(uint64_t *rp, size_t rn, const uint64_t *ap, size_t an,
                                        const uint64_t *bp, size_t bn, uint64_t *tp) noexcept;

extern WJR_ALL_NONNULL void sqrmod_bnm1(uint64_t *rp, size_t rn, const uint64_t *ap, size_t an,
                                        uint64_t *tp) noexcept;

WJR_CONST constexpr size_t mulmod_bknp1_itch(size_t rn) noexcept { return rn << 2; }
WJR_CONST constexpr size_t sqrmod_bknp1_itch(size_t rn) noexcept { return rn * 3; }

WJR_CONST WJR_INLINE_CONSTEXPR20 size_t mulmod_bnm1_itch(size_t rn, size_t an, size_t bn) {
    size_t n, itch;
    n = rn >> 1;
    itch = rn + 4 + (an > n ? (bn > n ? rn : n) : 0);
    return itch;
}

WJR_CONST WJR_INLINE_CONSTEXPR20 size_t sqrmod_bnm1_itch(size_t rn, size_t an) {
    size_t n, itch;
    n = rn >> 1;
    itch = rn + 3 + (an > n ? an : 0);
    return itch;
}

WJR_CONST constexpr bool toom44_ok(size_t n, size_t m) noexcept { return 3 * n + 21 <= 4 * m; }

WJR_CONST constexpr bool toom55_ok(size_t n, size_t m) noexcept { return 4 * n + 36 <= 5 * m; }

/* Returns smallest possible number of limbs >= pl for a fft of size 2^k,
   i.e. smallest multiple of 2^k >= pl.

   Don't declare static: needed by tuneup.
*/
WJR_CONST WJR_INTRINSIC_CONSTEXPR size_t fft_next_size(size_t pl, int k) noexcept {
    pl = 1 + ((pl - 1) >> k); /* ceil (pl/2^k) */
    return pl << k;
}

inline void mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m) noexcept;

inline void mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, const uint64_t *src1,
                  size_t n) noexcept;

inline void basecase_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                           const uint64_t *src1, size_t m) noexcept;

inline void basecase_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n) noexcept;

inline void sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n) noexcept;

inline void fft_mul(uint64_t *pp, const uint64_t *ap, size_t an, const uint64_t *bp,
                    size_t bn) noexcept {
    nussbaumer_mul(pp, ap, an, bp, bn);
}

inline void mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n == m)) {
        return mul_n(dst, src0, src1, n);
    }

    return __mul_s_impl(dst, src0, n, src1, m);
}

inline void mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, const uint64_t *src1,
                  size_t n) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(src0 == src1)) {
        return sqr(dst, src0, n);
    }

    return __mul_n_impl(dst, src0, src1, n);
}

inline void sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n) noexcept {
    return __sqr_impl(dst, src, n);
}

WJR_INTRINSIC_INLINE void fallback_basecase_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                                  size_t n, const uint64_t *src1,
                                                  size_t m) noexcept {
    dst[n] = mul_1(dst, src0, n, src1[0]);
    for (size_t i = 1; i < m; ++i) {
        ++dst;
        dst[n] = addmul_1(dst, src0, n, src1[i]);
    }
}

/*
require :
1. m >= 1
2. n >= m
3. WJR_IS_SAME_OR_SEPARATE_P(dst, n + m, src0, n)
4. WJR_IS_SAME_OR_SEPARATE_P(dst, n + m, src1, m)
*/
inline void basecase_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                           const uint64_t *src1, size_t m) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_SEPARATE_P(dst, n + m, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_SEPARATE_P(dst, n + m, src1, m));

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)
    return asm_basecase_mul_s(dst, src0, n, src1, m);
#else
    return fallback_basecase_mul_s(dst, src0, n, src1, m);
#endif
}

inline void basecase_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n) noexcept {
#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR)
    return asm_basecase_sqr(dst, src, n);
#else
    return basecase_mul_s(dst, src, n, src, n);
#endif
}

} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_MUL_HPP__