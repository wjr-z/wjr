#ifndef WJR_MATH_MUL_HPP__
#define WJR_MATH_MUL_HPP__

/**
 * @todo optimize temporary memory usage of mul_s, mul_n, sqr
 *
 */

#include <wjr/math/bignum-config.hpp>
#include <wjr/math/div-impl.hpp>

#include <wjr/math/add.hpp>
#include <wjr/math/bit.hpp>
#include <wjr/math/shift.hpp>
#include <wjr/math/stack_allocator.hpp>
#include <wjr/math/sub.hpp>

#if defined(WJR_X86)
#include <wjr/x86/math/mul.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul(T a, T b, T &hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    using T2 = uint_t<nd * 2>;
    T2 x = static_cast<T2>(a) * b;
    hi = x >> nd;
    return static_cast<T>(x);
}

WJR_INTRINSIC_CONSTEXPR_E uint64_t fallback_mul64(uint64_t a, uint64_t b, uint64_t &hi) {
    uint64_t ah = a >> 32;
    uint64_t al = a & 0xFFFFFFFF;
    uint64_t bh = b >> 32;
    uint64_t bl = b & 0xFFFFFFFF;

    uint64_t rh = ah * bh;
    uint64_t rm0 = ah * bl;
    uint64_t rm1 = al * bh;
    uint64_t rl = al * bl;

    __add_128(rl, rh, rl, rh, rm0 << 32, rm0 >> 32);
    __add_128(rl, rh, rl, rh, rm1 << 32, rm1 >> 32);

    hi = rh;
    return rl;
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR_E T mul(T a, T b, T &hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if ((WJR_BUILTIN_CONSTANT_P(a == 0) && a == 0) ||
        (WJR_BUILTIN_CONSTANT_P(b == 0) && b == 0)) {
        hi = 0;
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(a == 1) && a == 1) {
        hi = 0;
        return b;
    }

    if (WJR_BUILTIN_CONSTANT_P(b == 1) && b == 1) {
        hi = 0;
        return a;
    }

    if constexpr (nd < 64) {
        return fallback_mul(a, b, hi);
    } else {

#if WJR_HAS_BUILTIN(MUL64)
        if (is_constant_evaluated()
#if WJR_HAS_BUILTIN(ASM_MUL64)
            || (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))
#endif
        ) {
            return fallback_mul64(a, b, hi);
        }

#if WJR_HAS_BUILTIN(ASM_MUL64)
        // mov b to rax, then mul a
        // instead of mov a to rax, mov b to register, then mul
        if (WJR_BUILTIN_CONSTANT_P(b)) {
            return builtin_mul64(b, a, hi);
        }
#endif

        return builtin_mul64(a, b, hi);
#else
        return fallback_mul64(a, b, hi);
#endif
    }
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T fallback_mulhi(T a, T b) {
    T hi = 0;
    (void)fallback_mul(a, b, hi);
    return hi;
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T mulhi(T a, T b) {
    T ret = 0;
    (void)mul(a, b, ret);
    return ret;
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T mullo(T a, T b) {
    return a * b;
}

#if WJR_HAS_BUILTIN(__builtin_mul_overflow)
#define WJR_HAS_BUILTIN_MUL_OVERFLOW WJR_HAS_DEF
#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E bool fallback_mul_overflow(T a, T b, T &ret) {
    T hi;
    ret = mul(a, b, hi);
    return hi != 0;
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR_E bool mul_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret) {
#if WJR_HAS_BUILTIN(MUL_OVERFLOW)
    if (is_constant_evaluated() ||
        (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
        return fallback_mul_overflow(a, b, ret);
    }

    return __builtin_mul_overflow(a, b, &ret);
#else
    return fallback_mul_overflow(a, b, ret);
#endif
}

WJR_INTRINSIC_CONSTEXPR uint64_t fallback_mul_1(uint64_t *dst, const uint64_t *src,
                                                size_t n, uint64_t ml) {
    uint64_t lo = 0, hi = 0;
    uint64_t c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        dst[i] = addc(lo, c_in, 0, c_in);
        c_in += hi;
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
WJR_INTRINSIC_CONSTEXPR_E uint64_t mul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                         uint64_t ml) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        if (src != dst) {
            std::copy(src, src + n, dst);
        }

        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(is_zero_or_single_bit(ml)) && is_zero_or_single_bit(ml)) {
        if (ml == 0) {
            set_n(dst, 0, n);
            return 0;
        }

        unsigned int k = ctz(ml);
        return lshift_n(dst, src, n, k);
    }

#if WJR_HAS_BUILTIN(ASM_MUL_1)
    if (is_constant_evaluated()) {
        return fallback_mul_1(dst, src, n, ml);
    }

    return asm_mul_1(dst, src, n, ml);
#else
    return fallback_mul_1(dst, src, n, ml);
#endif
}

// dst = src0 + (src1 << cl)
WJR_INTRINSIC_CONSTEXPR_E uint64_t addlsh_n(uint64_t *dst, const uint64_t *src0,
                                            const uint64_t *src1, size_t n, uint64_t cl);

// dst = (src1 << cl) - src0
WJR_INTRINSIC_CONSTEXPR_E uint64_t rsblsh_n(uint64_t *dst, const uint64_t *src0,
                                            const uint64_t *src1, size_t n, uint64_t cl);

WJR_INTRINSIC_CONSTEXPR uint64_t fallback_addmul_1(uint64_t *dst, const uint64_t *src,
                                                   size_t n, uint64_t ml) {
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

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
WJR_INTRINSIC_CONSTEXPR_E uint64_t addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                            uint64_t ml) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        return addc_n(dst, dst, src, n);
    }

    if (WJR_BUILTIN_CONSTANT_P(is_zero_or_single_bit(ml)) && is_zero_or_single_bit(ml)) {
        if (ml == 0) {
            return 0;
        }

        unsigned int c = ctz(ml);
        return addlsh_n(dst, dst, src, n, c);
    }

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
    if (is_constant_evaluated()) {
        return fallback_addmul_1(dst, src, n, ml);
    }

    return asm_addmul_1(dst, src, n, ml);
#else
    return fallback_addmul_1(dst, src, n, ml);
#endif
}

WJR_INTRINSIC_CONSTEXPR uint64_t fallback_submul_1(uint64_t *dst, const uint64_t *src,
                                                   size_t n, uint64_t ml) {
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

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
WJR_INTRINSIC_CONSTEXPR_E uint64_t submul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                            uint64_t ml) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 0) && ml == 0) {
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        return subc_n(dst, dst, src, n);
    }

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)
    if (is_constant_evaluated()) {
        return fallback_submul_1(dst, src, n, ml);
    }

    return asm_submul_1(dst, src, n, ml);
#else
    return fallback_submul_1(dst, src, n, ml);
#endif
}

WJR_INTRINSIC_CONSTEXPR uint64_t fallback_addlsh_n(uint64_t *dst, const uint64_t *src0,
                                                   const uint64_t *src1, size_t n,
                                                   uint64_t cl) {
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

/*
dst = src0 + (src1 << cl);

require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
3. WJR_IS_SAME_OR_INCR_P(sdt, n, src1, n)
*/
WJR_INTRINSIC_CONSTEXPR_E uint64_t addlsh_n(uint64_t *dst, const uint64_t *src0,
                                            const uint64_t *src1, size_t n, uint64_t cl) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<uint64_t>::digits);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    if (WJR_UNLIKELY(cl == 0)) {
        return addc_n(dst, src0, src1, n);
    }

#if WJR_HAS_BUILTIN(ASM_ADDLSH_N)
    if (is_constant_evaluated()) {
        return fallback_addlsh_n(dst, src0, src1, n, cl);
    }

    return asm_addlsh_n(dst, src0, src1, n, cl);
#else
    return fallback_addlsh_n(dst, src0, src1, n, cl);
#endif
}

WJR_INTRINSIC_CONSTEXPR uint64_t fallback_rsblsh_n(uint64_t *dst, const uint64_t *src0,
                                                   const uint64_t *src1, size_t n,
                                                   uint64_t cl) {
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

/*
dst = (src1 << cl) - src0;

require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
3. WJR_IS_SAME_OR_INCR_P(sdt, n, src1, n)
*/
WJR_INTRINSIC_CONSTEXPR_E uint64_t rsblsh_n(uint64_t *dst, const uint64_t *src0,
                                            const uint64_t *src1, size_t n, uint64_t cl) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<uint64_t>::digits);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    if (WJR_UNLIKELY(cl == 0)) {
        return uint64_t{0} - subc_n(dst, src1, src0, n);
    }

#if WJR_HAS_BUILTIN(ASM_RSBLSH_N)
    if (is_constant_evaluated()) {
        return fallback_rsblsh_n(dst, src0, src1, n, cl);
    }

    return asm_rsblsh_n(dst, src0, src1, n, cl);
#else
    return fallback_rsblsh_n(dst, src0, src1, n, cl);
#endif
}

template <uint64_t maxn = in_place_max>
WJR_INTRINSIC_CONSTEXPR_E uint64_t
try_addmul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t ml,
             std::integral_constant<uint64_t, maxn> = {}) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    WJR_ASSERT_ASSUME(ml <= maxn);

    if constexpr (maxn == 0) {
        return 0;
    }

    if (ml == 0) {
        return 0;
    }

    if constexpr (maxn == 1) {
        return addc_n(dst, dst, src, n);
    } else {
        if (ml == 1) {
            return addc_n(dst, dst, src, n);
        }

        if constexpr (maxn <= 3) {
            // addlsh is slightly faster than addmul
            if constexpr (maxn == 2) {
                return addlsh_n(dst, dst, src, n, 1);
            } else {
                if (ml == 2) {
                    return addlsh_n(dst, dst, src, n, 1);
                }

                return addmul_1(dst, src, n, ml);
            }
        } else {
            return addmul_1(dst, src, n, ml);
        }
    }
}

inline constexpr size_t toom22_mul_threshold = WJR_TOOM22_MUL_THRESHOLD;
inline constexpr size_t toom33_mul_threshold = WJR_TOOM33_MUL_THRESHOLD;
inline constexpr size_t toom44_mul_threshold = WJR_TOOM44_MUL_THRESHOLD;
inline constexpr size_t toom32_to_toom43_mul_threshold =
    WJR_TOOM32_TO_TOOM43_MUL_THRESHOLD;
inline constexpr size_t toom32_to_toom53_mul_threshold =
    WJR_TOOM32_TO_TOOM53_MUL_THRESHOLD;
inline constexpr size_t toom42_to_toom53_mul_threshold =
    WJR_TOOM42_TO_TOOM53_MUL_THRESHOLD;
inline constexpr size_t toom2_sqr_threshold = WJR_TOOM2_SQR_THRESHOLD;
inline constexpr size_t toom3_sqr_threshold = WJR_TOOM3_SQR_THRESHOLD;
inline constexpr size_t toom4_sqr_threshold = WJR_TOOM4_SQR_THRESHOLD;

// only toom22 is optimized to inline
enum class __mul_mode : uint8_t {
    toom22 = 0x00,
    toom33 = 0x01,
    toom44 = 0x02,
    all = 0x03,
};

template <__mul_mode mode, bool reserved>
WJR_INTRINSIC_INLINE void
__mul_s_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
             const uint64_t *src1, size_t m,
             std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal);

template <__mul_mode mode>
WJR_INTRINSIC_INLINE void __mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                  size_t n, const uint64_t *src1, size_t m,
                                  uint64_t *stk);

WJR_INTRINSIC_INLINE void mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                size_t n, const uint64_t *src1, size_t m);

WJR_INTRINSIC_INLINE void mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                size_t n, const uint64_t *src1, size_t m, uint64_t *stk);

template <__mul_mode mode, bool reserved>
WJR_INTRINSIC_INLINE void
__mul_n_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, const uint64_t *src1,
             size_t n, std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal);

template <__mul_mode mode>
WJR_INTRINSIC_INLINE void __mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                  const uint64_t *src1, size_t n, uint64_t *stk);

template <__mul_mode mode, uint64_t m0 = in_place_max, uint64_t m1 = in_place_max>
WJR_INTRINSIC_INLINE void __mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                  const uint64_t *src1, size_t n, uint64_t *stk,
                                  uint64_t &c_out, uint64_t cf0, uint64_t cf1,
                                  std::integral_constant<uint64_t, m0> = {},
                                  std::integral_constant<uint64_t, m1> = {});

WJR_INTRINSIC_INLINE void mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                const uint64_t *src1, size_t n);

WJR_INTRINSIC_INLINE void mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                const uint64_t *src1, size_t n, uint64_t *stk);

WJR_INTRINSIC_INLINE void basecase_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                         size_t n, const uint64_t *src1, size_t m);

WJR_INTRINSIC_INLINE void basecase_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                       size_t n);

template <__mul_mode mode, bool reserved>
WJR_INTRINSIC_INLINE void
__sqr_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
           std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal);

WJR_INTRINSIC_INLINE void __sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                                uint64_t *stk);

template <__mul_mode mode, uint64_t m = in_place_max>
WJR_INTRINSIC_INLINE void __sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                                uint64_t *stk, uint64_t &c_out, uint64_t cf,
                                std::integral_constant<uint64_t, m> x = {});

WJR_INTRINSIC_INLINE void sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n);

WJR_INTRINSIC_INLINE void sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                              uint64_t *stk);

struct toom_interpolation_5p_struct {
    bool neg1;
    uint64_t cf1;
    uint64_t cf2;
    uint64_t cf3;
};

struct toom_interpolation_6p_struct {
    uint8_t neg1 : 1;
    uint8_t neg3 : 1;
    uint64_t cf1;
    uint64_t cf2;
    uint64_t cf3;
    uint64_t cf4;
};

struct toom_interpolation_7p_struct {
    uint8_t neg1 : 1;
    uint8_t neg3 : 1;
    uint64_t cf1;
    uint64_t cf2;
    uint64_t cf3;
    uint64_t cf4;
    uint64_t cf5;
};

struct toom_interpolation_8p_struct {
    uint8_t neg1 : 1;
    uint8_t neg3 : 1;
    uint8_t neg5 : 1;
    uint64_t cf1;
    uint64_t cf2;
    uint64_t cf3;
    uint64_t cf4;
    uint64_t cf5;
    uint64_t cf6;
};

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
 l = ceIl(n/2)
 stk usage : l * 2
*/
extern void toom22_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                         const uint64_t *src1, size_t m, uint64_t *stk);

extern void toom2_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                      uint64_t *stk);

/*
 l = max(ceil(n/3), ceil(m/2))
 stk usage : l * 4
*/
extern void toom32_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                         const uint64_t *src1, size_t m, uint64_t *stk);

extern void toom_interpolation_5p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                    size_t rn, size_t rm,
                                    toom_interpolation_5p_struct &&flag);

/*
 l = max(ceil(n/4), ceil(m/2))
 stk usage : l * 4
*/
extern void toom42_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                         const uint64_t *src1, size_t m, uint64_t *stk);

/*
 l = ceil(n/3)
 stk usage : l * 4
*/
extern void toom33_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                         const uint64_t *src1, size_t m, uint64_t *stk);

extern void toom3_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                      uint64_t *stk);

extern void toom_interpolation_6p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                    size_t rn, size_t rm,
                                    toom_interpolation_6p_struct &&flag);

/*
 l = max(ceil(n/4), ceil(m/3))
 stk usage : l * 6
*/
extern void toom43_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                         const uint64_t *src1, size_t m, uint64_t *stk);

extern void toom_interpolation_7p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                    size_t rn, size_t rm,
                                    toom_interpolation_7p_struct &&flag);

/*
 l = max(ceil(n/5), ceil(m/3))
 stk usage : l * 6
*/
extern void toom53_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                         const uint64_t *src1, size_t m, uint64_t *stk);

/*
 l = ceil(n/4)
 stk usage : l * 6
*/
extern void toom44_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                         const uint64_t *src1, size_t m, uint64_t *stk);

extern void toom4_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                      uint64_t *stk);

struct __mul_s_unique_stack_allocator {
    template <typename... Args>
    constexpr __mul_s_unique_stack_allocator(Args &&...) {}
};

template <typename T, typename U>
WJR_INTRINSIC_INLINE uint64_t *__mul_s_allocate(WJR_MAYBE_UNUSED T &mal,
                                                WJR_MAYBE_UNUSED U &alloc, size_t n) {
    if constexpr (std::is_same_v<remove_cvref_t<U>, __mul_s_unique_stack_allocator>) {
        static_assert(std::is_pointer_v<T>, "");
        (void)(alloc);
        uint64_t *ret = mal;
        mal += n;
        return ret;
    } else {
        (void)(mal);
        return static_cast<uint64_t *>(alloc.allocate(n));
    }
}

template <bool reserved>
void __toom22_mul_s_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                         const uint64_t *src1, size_t m,
                         std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n + m, src0, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n + m, src1, m));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (m < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, m);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (n >= 3 * m) {
        uint64_t *tmp = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (4 * m));
        uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (6 * m + 67));

        toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
        n -= 2 * m;
        src0 += 2 * m;
        dst += 2 * m;

        uint64_t cf = 0;

        while (n >= 3 * m) {
            toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;

            cf = addc_n(dst, dst, tmp, m, cf);
            std::copy(tmp + m, tmp + 3 * m, dst + m);
            cf = addc_1(dst + m, dst + m, 2 * m, 0, cf);

            dst += 2 * m;
        }

        if (4 * n < 5 * m) {
            toom22_mul_s(tmp, src0, n, src1, m, stk);
        } else if (4 * n < 7 * m) {
            toom32_mul_s(tmp, src0, n, src1, m, stk);
        } else {
            toom42_mul_s(tmp, src0, n, src1, m, stk);
        }

        cf = addc_n(dst, dst, tmp, m, cf);
        std::copy(tmp + m, tmp + m + n, dst + m);
        cf = addc_1(dst + m, dst + m, n, 0, cf);
        WJR_ASSERT(cf == 0);
    } else {
        uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (6 * m + 67));

        if (4 * n < 5 * m) {
            toom22_mul_s(dst, src0, n, src1, m, stk);
        } else if (4 * n < 7 * m) {
            toom32_mul_s(dst, src0, n, src1, m, stk);
        } else {
            toom42_mul_s(dst, src0, n, src1, m, stk);
        }
    }

    return;
}

template <bool reserved>
void __noinline_mul_s_impl(
    uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
    size_t m, std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n + m, src0, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n + m, src1, m));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (m < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, m);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (m < toom33_mul_threshold) {
        if (n >= 3 * m) {
            uint64_t *tmp = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (4 * m));
            uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (6 * m + 67));

            toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;
            dst += 2 * m;

            uint64_t cf = 0;

            while (n >= 3 * m) {
                toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
                n -= 2 * m;
                src0 += 2 * m;

                cf = addc_n(dst, dst, tmp, m, cf);
                std::copy(tmp + m, tmp + 3 * m, dst + m);
                cf = addc_1(dst + m, dst + m, 2 * m, 0, cf);

                dst += 2 * m;
            }

            if (4 * n < 5 * m) {
                toom22_mul_s(tmp, src0, n, src1, m, stk);
            } else if (4 * n < 7 * m) {
                toom32_mul_s(tmp, src0, n, src1, m, stk);
            } else {
                toom42_mul_s(tmp, src0, n, src1, m, stk);
            }

            cf = addc_n(dst, dst, tmp, m, cf);
            std::copy(tmp + m, tmp + m + n, dst + m);
            cf = addc_1(dst + m, dst + m, n, 0, cf);
            WJR_ASSERT(cf == 0);
        } else {
            uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (6 * m + 67));

            if (4 * n < 5 * m) {
                toom22_mul_s(dst, src0, n, src1, m, stk);
            } else if (4 * n < 7 * m) {
                toom32_mul_s(dst, src0, n, src1, m, stk);
            } else {
                toom42_mul_s(dst, src0, n, src1, m, stk);
            }
        }

        return;
    }

    if (m < toom44_mul_threshold || 3 * n + 21 > 4 * m) {
        if (n >= 3 * m) {
            uint64_t *tmp = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (4 * m));
            uint64_t *stk =
                __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (9 * m + 288));

            toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;
            dst += 2 * m;

            uint64_t cf = 0;

            while (n >= 3 * m) {
                toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
                n -= 2 * m;
                src0 += 2 * m;

                cf = addc_n(dst, dst, tmp, m, cf);
                std::copy(tmp + m, tmp + 3 * m, dst + m);
                cf = addc_1(dst + m, dst + m, 2 * m, 0, cf);

                dst += 2 * m;
            }

            __mul_s_impl<__mul_mode::all, true>(tmp, src0, n, src1, m, stk);

            cf = addc_n(dst, dst, tmp, m, cf);
            std::copy(tmp + m, tmp + m + n, dst + m);
            cf = addc_1(dst + m, dst + m, n, 0, cf);
            WJR_ASSERT(cf == 0);
        } else {
            uint64_t *stk =
                __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (9 * m + 288));
            if (6 * n < 7 * m) {
                toom33_mul_s(dst, src0, n, src1, m, stk);
            } else if (2 * n < 3 * m) {
                if (m < toom32_to_toom43_mul_threshold) {
                    toom32_mul_s(dst, src0, n, src1, m, stk);
                } else {
                    toom43_mul_s(dst, src0, n, src1, m, stk);
                }
            } else if (6 * n < 11 * m) {
                if (4 * n < 7 * m) {
                    if (m < toom32_to_toom53_mul_threshold) {
                        toom32_mul_s(dst, src0, n, src1, m, stk);
                    } else {
                        toom53_mul_s(dst, src0, n, src1, m, stk);
                    }
                } else {
                    if (m < toom42_to_toom53_mul_threshold) {
                        toom42_mul_s(dst, src0, n, src1, m, stk);
                    } else {
                        toom53_mul_s(dst, src0, n, src1, m, stk);
                    }
                }
            } else {
                toom42_mul_s(dst, src0, n, src1, m, stk);
            }
        }

        return;
    }

    uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (9 * m + 288));
    toom44_mul_s(dst, src0, n, src1, m, stk);
    return;
}

extern template void __noinline_mul_s_impl<true>(uint64_t *WJR_RESTRICT dst,
                                                 const uint64_t *src0, size_t n,
                                                 const uint64_t *src1, size_t m,
                                                 uint64_t *mal);

extern template void __noinline_mul_s_impl<false>(uint64_t *WJR_RESTRICT dst,
                                                  const uint64_t *src0, size_t n,
                                                  const uint64_t *src1, size_t m,
                                                  in_place_empty_t mal);

template <__mul_mode mode, bool reserved>
WJR_INTRINSIC_INLINE void
__mul_s_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
             const uint64_t *src1, size_t m,
             std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal) {
    static_assert((int)__mul_mode::toom22 == 0, "");
    if (WJR_BUILTIN_CONSTANT_P(n == m) && n == m) {
        return __mul_n_impl<mode, reserved>(dst, src0, src1, n, mal);
    }

    if constexpr (mode == __mul_mode::toom22) {
        return __toom22_mul_s_impl<reserved>(dst, src0, n, src1, m, mal);
    } else {
        return __noinline_mul_s_impl<reserved>(dst, src0, n, src1, m, mal);
    }
}

template <__mul_mode mode>
WJR_INTRINSIC_INLINE void __mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                  size_t n, const uint64_t *src1, size_t m,
                                  uint64_t *stk) {
    return __mul_s_impl<mode, true>(dst, src0, n, src1, m, stk);
}

WJR_INTRINSIC_INLINE void mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                size_t n, const uint64_t *src1, size_t m) {
    return __mul_s_impl<__mul_mode::all, false>(dst, src0, n, src1, m, in_place_empty);
}

WJR_INTRINSIC_INLINE void mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                size_t n, const uint64_t *src1, size_t m, uint64_t *stk) {
    return __mul_s_impl<__mul_mode::all, true>(dst, src0, n, src1, m, stk);
}

template <__mul_mode mode, bool reserved>
WJR_INTRINSIC_INLINE void
__inline_mul_n_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                    const uint64_t *src1, size_t n,
                    std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src0, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src1, n));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (n < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, n);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (mode <= __mul_mode::toom22 || n < toom33_mul_threshold) {
        uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (6 * n + 67));
        return toom22_mul_s(dst, src0, n, src1, n, stk);
    }

    uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (9 * n + 288));
    return toom33_mul_s(dst, src0, n, src1, n, stk);
}

template <bool reserved>
void __noinline_mul_n_impl(
    uint64_t *WJR_RESTRICT dst, const uint64_t *src0, const uint64_t *src1, size_t n,
    std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src0, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src1, n));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (n < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, n);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (n < toom33_mul_threshold) {
        uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (6 * n + 67));
        return toom22_mul_s(dst, src0, n, src1, n, stk);
    }

    if (n < toom44_mul_threshold) {
        uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (9 * n + 288));
        return toom33_mul_s(dst, src0, n, src1, n, stk);
    }

    uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (9 * n + 288));
    return toom44_mul_s(dst, src0, n, src1, n, stk);
}

template <__mul_mode mode, bool reserved>
WJR_INTRINSIC_INLINE void
__mul_n_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, const uint64_t *src1,
             size_t n, std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return __sqr_impl<mode, reserved>(dst, src0, n, mal);
    }

    if constexpr (mode <= __mul_mode::toom33) {
        return __inline_mul_n_impl<mode, reserved>(dst, src0, src1, n, mal);
    } else {
        return __noinline_mul_n_impl<reserved>(dst, src0, src1, n, mal);
    }
}

template <__mul_mode mode>
WJR_INTRINSIC_INLINE void __mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                  const uint64_t *src1, size_t n, uint64_t *stk) {
    return __mul_n_impl<mode, true>(dst, src0, src1, n, stk);
}

template <__mul_mode mode, uint64_t m0, uint64_t m1>
void __mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, const uint64_t *src1,
             size_t n, uint64_t *stk, uint64_t &c_out, uint64_t cf0, uint64_t cf1,
             std::integral_constant<uint64_t, m0> x0,
             std::integral_constant<uint64_t, m1> x1) {
    WJR_ASSERT_ASSUME(cf0 <= m0);
    WJR_ASSERT_ASSUME(cf1 <= m1);
    __mul_n<mode>(dst, src0, src1, n, stk);
    if constexpr (m0 == 0 || m1 == 0) {
        c_out = 0;
    } else if constexpr (m0 == 1 || m1 == 1) {
        if constexpr (m0 == 1 && m1 == 1) {
            c_out = cf0 && cf1;
        } else if constexpr (m0 == 1) {
            c_out = cf0 ? cf1 : 0;
        } else {
            c_out = cf1 ? cf0 : 0;
        }
    } else {
        c_out = cf0 * cf1;
    }
    c_out += try_addmul_1(dst + n, src1, n, cf0, x0);
    c_out += try_addmul_1(dst + n, src0, n, cf1, x1);
}

WJR_INTRINSIC_INLINE void mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                const uint64_t *src1, size_t n) {
    return __mul_n_impl<__mul_mode::all, false>(dst, src0, src1, n, in_place_empty);
}

WJR_INTRINSIC_INLINE void mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                const uint64_t *src1, size_t n, uint64_t *stk) {
    return __mul_n_impl<__mul_mode::all, true>(dst, src0, src1, n, stk);
}

template <__mul_mode mode, bool reserved>
WJR_INTRINSIC_INLINE void
__inline_sqr_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                  std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src, n));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (n < toom2_sqr_threshold) {
        return basecase_sqr(dst, src, n);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (mode <= __mul_mode::toom22 || n < toom3_sqr_threshold) {
        uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (6 * n + 67));
        return toom2_sqr(dst, src, n, stk);
    }

    uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (9 * n + 288));
    return toom3_sqr(dst, src, n, stk);
}

template <bool reserved>
void __noinline_sqr_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                         std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n * 2, src, n));

    using unique_alloc =
        std::conditional_t<reserved, __mul_s_unique_stack_allocator,
                           unique_stack_allocator<wjr::math_details::stack_alloc_object>>;

    if (n < toom2_sqr_threshold) {
        return basecase_sqr(dst, src, n);
    }

    unique_alloc stkal(math_details::stack_alloc);

    if (n < toom3_sqr_threshold) {
        uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (6 * n + 67));
        return toom2_sqr(dst, src, n, stk);
    }

    if (n < toom4_sqr_threshold) {
        uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (9 * n + 288));
        return toom3_sqr(dst, src, n, stk);
    }

    uint64_t *stk = __mul_s_allocate(mal, stkal, sizeof(uint64_t) * (9 * n + 288));
    return toom4_sqr(dst, src, n, stk);
}

template <__mul_mode mode, bool reserved>
WJR_INTRINSIC_INLINE void
__sqr_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
           std::conditional_t<reserved, uint64_t *, in_place_empty_t> mal) {
    if constexpr (mode <= __mul_mode::toom33) {
        return __inline_sqr_impl<mode, reserved>(dst, src, n, mal);
    } else {
        return __noinline_sqr_impl<reserved>(dst, src, n, mal);
    }
}

template <__mul_mode mode>
WJR_INTRINSIC_INLINE void __sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                                uint64_t *stk) {
    return __sqr_impl<mode, true>(dst, src, n, stk);
}

template <__mul_mode mode, uint64_t m>
void __sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n, uint64_t *stk,
           uint64_t &c_out, uint64_t cf, std::integral_constant<uint64_t, m>) {
    WJR_ASSERT_ASSUME(cf <= m);
    __sqr<mode>(dst, src, n, stk);
    if constexpr (m == 0) {
        c_out = 0;
    } else if constexpr (m == 1) {
        c_out = cf;
    } else {
        c_out = cf * cf;
    }

    constexpr auto m2 = m <= ((uint32_t)in_place_max) ? m * 2 : m;

    c_out +=
        try_addmul_1(dst + n, src, n, 2 * cf, std::integral_constant<uint64_t, m2>{});
}

WJR_INTRINSIC_INLINE void sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n) {
    __sqr_impl<__mul_mode::all, false>(dst, src, n, in_place_empty);
}

WJR_INTRINSIC_INLINE void sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                              uint64_t *stk) {
    __sqr_impl<__mul_mode::all, true>(dst, src, n, stk);
}

WJR_INTRINSIC_INLINE void fallback_basecase_mul_s(uint64_t *WJR_RESTRICT dst,
                                                  const uint64_t *src0, size_t n,
                                                  const uint64_t *src1, size_t m) {
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
WJR_INTRINSIC_INLINE void basecase_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                         size_t n, const uint64_t *src1, size_t m) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_SEPARATE_P(dst, n + m, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_SEPARATE_P(dst, n + m, src1, m));

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)
    return asm_basecase_mul_s(dst, src0, n, src1, m);
#else
    return fallback_basecase_mul_s(dst, src0, n, src1, m);
#endif
}

WJR_INTRINSIC_INLINE void basecase_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                       size_t n) {
#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR)
    return asm_basecase_sqr(dst, src, n);
#else
    return basecase_mul_s(dst, src, n, src, n);
#endif
}

} // namespace wjr

#endif // WJR_MATH_MUL_HPP__