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

// TODO : optimize
template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul_1(T *dst, const T *src, size_t n, T ml) {
    T lo = 0, hi = 0;
    T c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        dst[i] = addc<T>(lo, c_in, 0, c_in);
        c_in += hi;
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T mul_1(T *dst, const T *src, size_t n, type_identity_t<T> ml) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 0) && ml == 0) {
        set_n(dst, 0, n);
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        if (src != dst) {
            std::copy(src, src + n, dst);
        }

        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(is_zero_or_single_bit(ml)) && is_zero_or_single_bit(ml)) {
        unsigned int k = countr_zero(ml);
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
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addlsh_n(T *dst, const T *src0, const T *src1, size_t n,
                                     type_identity_t<T> cl);

// dst = (src1 << cl) - src0
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T rsblsh_n(T *dst, const T *src0, const T *src1, size_t n,
                                     type_identity_t<T> cl);

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_addmul_1(T *dst, const T *src, size_t n, T ml) {
    T lo = 0, hi = 0;
    T o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        lo = addc<T>(lo, c_in, 0, c_in);
        dst[i] = addc<T>(lo, dst[i], 0, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addmul_1(T *dst, const T *src, size_t n,
                                     type_identity_t<T> ml) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 0) && ml == 0) {
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        return addc_n(dst, dst, src, n);
    }

    if (WJR_BUILTIN_CONSTANT_P(is_zero_or_single_bit(ml)) && is_zero_or_single_bit(ml)) {
        unsigned int c = countr_zero(ml);
        return addlsh_n(dst, dst, src, n, c);
    }

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_addmul_1(dst, src, n, ml);
        }

        return asm_addmul_1(dst, src, n, ml);
    } else {
        return fallback_addmul_1(dst, src, n, ml);
    }
#else
    return fallback_addmul_1(dst, src, n, ml);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_submul_1(T *dst, const T *src, size_t n, T ml) {
    T lo = 0, hi = 0;
    T o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        lo = addc<T>(lo, c_in, 0, c_in);
        dst[i] = subc<T>(dst[i], lo, 0, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T submul_1(T *dst, const T *src, size_t n,
                                     type_identity_t<T> ml) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
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

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_addlsh_n(T *dst, const T *src0, const T *src1,
                                            size_t n, type_identity_t<T> cl) {
    T tcl = std::numeric_limits<T>::digits - cl;
    T lo = 0, hi = 0;
    T c_in = 0, x = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = src1[i] << cl;
        hi = src1[i] >> tcl;

        lo += x;
        dst[i] = addc<T>(lo, src0[i], c_in, c_in);
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
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addlsh_n(T *dst, const T *src0, const T *src1, size_t n,
                                     type_identity_t<T> cl) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<T>::digits);
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

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_rsblsh_n(T *dst, const T *src0, const T *src1,
                                            size_t n, type_identity_t<T> cl) {
    T tcl = std::numeric_limits<T>::digits - cl;
    T lo = 0, hi = 0;
    T c_in = 0, x = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = src1[i] << cl;
        hi = src1[i] >> tcl;

        lo += x;
        dst[i] = subc<T>(lo, src0[i], c_in, c_in);
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
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T rsblsh_n(T *dst, const T *src0, const T *src1, size_t n,
                                     type_identity_t<T> cl) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<T>::digits);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    if (WJR_UNLIKELY(cl == 0)) {
        return T{0} - subc_n(dst, src1, src0, n);
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

template <typename T, T maxn = in_place_max>
WJR_INTRINSIC_CONSTEXPR_E T try_addmul_1(T *dst, const T *src, size_t n,
                                         type_identity_t<T> ml,
                                         std::integral_constant<T, maxn> = {}) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
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

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__mul_s_impl(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
             std::conditional_t<reserved, T *, in_place_empty_t> mal);

template <__mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                  const T *src1, size_t m, T *stk);

template <typename T>
WJR_INTRINSIC_INLINE void mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                const T *src1, size_t m);

template <typename T>
WJR_INTRINSIC_INLINE void mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                const T *src1, size_t m, T *stk);

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__mul_n_impl(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n,
             std::conditional_t<reserved, T *, in_place_empty_t> mal);

template <__mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                  size_t n, T *stk);

template <__mul_mode mode, typename T, T m0 = in_place_max, T m1 = in_place_max>
WJR_INTRINSIC_INLINE void
__mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n, T *stk, T &c_out,
        type_identity_t<T> cf0, type_identity_t<T> cf1,
        std::integral_constant<T, m0> = {}, std::integral_constant<T, m1> = {});

template <typename T>
WJR_INTRINSIC_INLINE void mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                size_t n);

template <typename T>
WJR_INTRINSIC_INLINE void mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                size_t n, T *stk);

template <typename T>
void basecase_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1,
                    size_t m);

template <typename T>
void basecase_sqr(T *WJR_RESTRICT dst, const T *src, size_t n);

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__sqr_impl(T *WJR_RESTRICT dst, const T *src, size_t n,
           std::conditional_t<reserved, T *, in_place_empty_t> mal);

template <typename T>
WJR_INTRINSIC_INLINE void __sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk);

template <__mul_mode mode, typename T, T m = in_place_max>
WJR_INTRINSIC_INLINE void __sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk,
                                T &c_out, type_identity_t<T> cf,
                                std::integral_constant<T, m> x = {});

template <typename T>
WJR_INTRINSIC_INLINE void sqr(T *WJR_RESTRICT dst, const T *src, size_t n);

template <typename T>
WJR_INTRINSIC_INLINE void sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk);

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
template <typename T>
void toom22_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom22_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

template <typename T>
void toom2_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk);

extern template void toom2_sqr<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                         size_t n, uint64_t *stk);

/*
 l = max(ceil(n/3), ceil(m/2))
 stk usage : l * 4
*/
template <typename T>
void toom32_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom32_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

template <typename T>
struct toom_interpolation_5p_struct;

template <typename T>
void toom_interpolation_5p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_5p_struct<T> &&flag);

extern template void
toom_interpolation_5p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_5p_struct<uint64_t> &&flag);

/*
 l = max(ceil(n/4), ceil(m/2))
 stk usage : l * 4
*/
template <typename T>
void toom42_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom42_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

/*
 l = ceil(n/3)
 stk usage : l * 4
*/
template <typename T>
void toom33_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom33_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

template <typename T>
void toom3_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk);

extern template void toom3_sqr<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                         size_t n, uint64_t *stk);

template <typename T>
struct toom_interpolation_6p_struct;

template <typename T>
void toom_interpolation_6p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_6p_struct<T> &&flag);

extern template void
toom_interpolation_6p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_6p_struct<uint64_t> &&flag);

/*
 l = max(ceil(n/4), ceil(m/3))
 stk usage : l * 6
*/
template <typename T>
void toom43_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom43_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

template <typename T>
struct toom_interpolation_7p_struct;

template <typename T>
void toom_interpolation_7p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_7p_struct<T> &&flag);

extern template void
toom_interpolation_7p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_7p_struct<uint64_t> &&flag);

/*
 l = max(ceil(n/5), ceil(m/3))
 stk usage : l * 6
*/
template <typename T>
void toom53_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom53_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

/*
 l = ceil(n/4)
 stk usage : l * 6
*/
template <typename T>
void toom44_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk);

extern template void toom44_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst,
                                            const uint64_t *src0, size_t n,
                                            const uint64_t *src1, size_t m,
                                            uint64_t *stk);

template <typename T>
void toom4_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk);

extern template void toom4_sqr<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                         size_t n, uint64_t *stk);

struct __mul_s_unique_stack_allocator {
    template <typename... Args>
    constexpr __mul_s_unique_stack_allocator(Args &&...) {}
};

template <typename P, typename T, typename U>
WJR_INTRINSIC_INLINE P *__mul_s_allocate(WJR_MAYBE_UNUSED T &mal,
                                         WJR_MAYBE_UNUSED U &alloc, size_t n) {
    if constexpr (std::is_same_v<remove_cvref_t<U>, __mul_s_unique_stack_allocator>) {
        static_assert(std::is_pointer_v<T>, "");
        (void)(alloc);
        P *ret = mal;
        mal += n;
        return ret;
    } else {
        (void)(mal);
        return static_cast<P *>(alloc.allocate(n));
    }
}

template <bool reserved, typename T>
void __toom22_mul_s_impl(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1,
                         size_t m,
                         std::conditional_t<reserved, T *, in_place_empty_t> mal) {
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
        T *tmp = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (4 * m));
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * m + 67));

        toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
        n -= 2 * m;
        src0 += 2 * m;
        dst += 2 * m;

        T cf = 0;

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
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * m + 67));

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

template <bool reserved, typename T>
void __noinline_mul_s_impl(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1,
                           size_t m,
                           std::conditional_t<reserved, T *, in_place_empty_t> mal) {
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
            T *tmp = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (4 * m));
            T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * m + 67));

            toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;
            dst += 2 * m;

            T cf = 0;

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
            T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * m + 67));

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
            T *tmp = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (4 * m));
            T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * m + 288));

            toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;
            dst += 2 * m;

            T cf = 0;

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
            T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * m + 288));
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

    T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * m + 288));
    toom44_mul_s(dst, src0, n, src1, m, stk);
    return;
}

extern template void __noinline_mul_s_impl<true, uint64_t>(uint64_t *WJR_RESTRICT dst,
                                                           const uint64_t *src0, size_t n,
                                                           const uint64_t *src1, size_t m,
                                                           uint64_t *mal);

extern template void
__noinline_mul_s_impl<false, uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                       size_t n, const uint64_t *src1, size_t m,
                                       in_place_empty_t mal);

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__mul_s_impl(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
             std::conditional_t<reserved, T *, in_place_empty_t> mal) {
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

template <__mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                  const T *src1, size_t m, T *stk) {
    return __mul_s_impl<mode, true>(dst, src0, n, src1, m, stk);
}

template <typename T>
WJR_INTRINSIC_INLINE void mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                const T *src1, size_t m) {
    return __mul_s_impl<__mul_mode::all, false>(dst, src0, n, src1, m, in_place_empty);
}

template <typename T>
WJR_INTRINSIC_INLINE void mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n,
                                const T *src1, size_t m, T *stk) {
    return __mul_s_impl<__mul_mode::all, true>(dst, src0, n, src1, m, stk);
}

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__inline_mul_n_impl(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n,
                    std::conditional_t<reserved, T *, in_place_empty_t> mal) {
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
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * n + 67));
        return toom22_mul_s(dst, src0, n, src1, n, stk);
    }

    T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
    return toom33_mul_s(dst, src0, n, src1, n, stk);
}

template <bool reserved, typename T>
void __noinline_mul_n_impl(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n,
                           std::conditional_t<reserved, T *, in_place_empty_t> mal) {
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
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * n + 67));
        return toom22_mul_s(dst, src0, n, src1, n, stk);
    }

    if (n < toom44_mul_threshold) {
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
        return toom33_mul_s(dst, src0, n, src1, n, stk);
    }

    T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
    return toom44_mul_s(dst, src0, n, src1, n, stk);
}

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__mul_n_impl(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n,
             std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return __sqr_impl<mode, reserved>(dst, src0, n, mal);
    }

    if constexpr (mode <= __mul_mode::toom33) {
        return __inline_mul_n_impl<mode, reserved, T>(dst, src0, src1, n, mal);
    } else {
        return __noinline_mul_n_impl<reserved>(dst, src0, src1, n, mal);
    }
}

template <__mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                  size_t n, T *stk) {
    return __mul_n_impl<mode, true>(dst, src0, src1, n, stk);
}

template <__mul_mode mode, typename T, T m0, T m1>
void __mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1, size_t n, T *stk,
             T &c_out, type_identity_t<T> cf0, type_identity_t<T> cf1,
             std::integral_constant<T, m0> x0, std::integral_constant<T, m1> x1) {
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

template <typename T>
WJR_INTRINSIC_INLINE void mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                size_t n) {
    return __mul_n_impl<__mul_mode::all, false>(dst, src0, src1, n, in_place_empty);
}

template <typename T>
WJR_INTRINSIC_INLINE void mul_n(T *WJR_RESTRICT dst, const T *src0, const T *src1,
                                size_t n, T *stk) {
    return __mul_n_impl<__mul_mode::all, true>(dst, src0, src1, n, stk);
}

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__inline_sqr_impl(T *WJR_RESTRICT dst, const T *src, size_t n,
                  std::conditional_t<reserved, T *, in_place_empty_t> mal) {
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
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * n + 67));
        return toom2_sqr(dst, src, n, stk);
    }

    T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
    return toom3_sqr(dst, src, n, stk);
}

template <bool reserved, typename T>
void __noinline_sqr_impl(T *WJR_RESTRICT dst, const T *src, size_t n,
                         std::conditional_t<reserved, T *, in_place_empty_t> mal) {
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
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (6 * n + 67));
        return toom2_sqr(dst, src, n, stk);
    }

    if (n < toom4_sqr_threshold) {
        T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
        return toom3_sqr(dst, src, n, stk);
    }

    T *stk = __mul_s_allocate<T>(mal, stkal, sizeof(T) * (9 * n + 288));
    return toom4_sqr(dst, src, n, stk);
}

template <__mul_mode mode, bool reserved, typename T>
WJR_INTRINSIC_INLINE void
__sqr_impl(T *WJR_RESTRICT dst, const T *src, size_t n,
           std::conditional_t<reserved, T *, in_place_empty_t> mal) {
    if constexpr (mode <= __mul_mode::toom33) {
        return __inline_sqr_impl<mode, reserved>(dst, src, n, mal);
    } else {
        return __noinline_sqr_impl<reserved>(dst, src, n, mal);
    }
}

template <__mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk) {
    return __sqr_impl<mode, true>(dst, src, n, stk);
}

template <__mul_mode mode, typename T, T m>
void __sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk, T &c_out,
           type_identity_t<T> cf, std::integral_constant<T, m>) {
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

    c_out += try_addmul_1(dst + n, src, n, 2 * cf, std::integral_constant<T, m2>{});
}

template <typename T>
WJR_INTRINSIC_INLINE void sqr(T *WJR_RESTRICT dst, const T *src, size_t n) {
    __sqr_impl<__mul_mode::all, false>(dst, src, n, in_place_empty);
}

template <typename T>
WJR_INTRINSIC_INLINE void sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk) {
    __sqr_impl<__mul_mode::all, true>(dst, src, n, stk);
}

template <typename T>
void fallback_basecase_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1,
                             size_t m) {
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
template <typename T>
void basecase_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1,
                    size_t m) {
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

// TODO : optimize
template <typename T>
void basecase_sqr(T *WJR_RESTRICT dst, const T *src, size_t n) {
#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR)
    return asm_basecase_sqr(dst, src, n);
#else
    return basecase_mul_s(dst, src, n, src, n);
#endif
}

template <typename T>
void toom22_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    // [u0(l), u1(rn)] (n)
    // [v0(l), v1(rm)] (m)

    // (u0 - u1) * (v0 - v1)

    WJR_ASSERT_ASSUME(m <= n && n < 2 * m);

    const size_t l = (n + 1) / 2;
    const size_t rn = n - l;
    const size_t rm = m - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);
    WJR_ASSERT_ASSUME(l - rn <= 1);

    const auto u0 = src0;
    const auto u1 = src0 + l;

    const auto v0 = src1;
    const auto v1 = src1 + l;

    const auto p0 = dst;
    const auto p1 = dst + l;
    const auto p2 = dst + l * 2;
    const auto p3 = dst + l * 3;

    const auto wp = stk;
    stk += l * 2;

    bool f = 0;

    do {
        ssize_t p;
        p = abs_subc_s(p0, u0, l, u1, rn);
        if (__fasts_is_negative(p)) {
            f ^= 1;
        } else if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        p = abs_subc_s(p1, v0, l, v1, rm);
        if (__fasts_is_negative(p)) {
            f ^= 1;
        } else if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        __mul_n<__mul_mode::toom22>(wp, p0, p1, l, stk);
        break;
    ZERO:
        set_n(wp, 0, l * 2);
        break;
    } while (0);

    __mul_n<__mul_mode::toom22>(p0, u0, v0, l, stk);
    __mul_s<__mul_mode::toom22>(p2, u1, rn, v1, rm, stk);

    T cf = 0, cf2 = 0;

    cf = addc_n(p2, p1, p2, l);
    cf2 = cf + addc_n(p1, p0, p2, l);
    cf += addc_sz(p2, p2, l, p3, rn + rm - l);

    if (!f) {
        cf -= subc_n(p1, p1, wp, l * 2);
    } else {
        cf += addc_n(p1, p1, wp, l * 2);
    }

    cf2 = addc_1(p2, p2, l, cf2);
    if (WJR_LIKELY(rn + rm != l)) {
        cf = addc_1(p3, p3, rn + rm - l, cf, cf2);
    }
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom2_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk) {
    const size_t l = (n + 1) / 2;
    const size_t rn = n - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(rn * 2 >= l);

    const auto u0 = src;
    const auto u1 = src + l;

    const auto p0 = dst;
    const auto p1 = dst + l;
    const auto p2 = dst + l * 2;
    const auto p3 = dst + l * 3;

    const auto wp = stk;
    stk += l * 2;

    do {
        ssize_t p = abs_subc_s(p0, u0, l, u1, rn);
        if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        __sqr<__mul_mode::toom22>(wp, p0, l, stk);
        break;
    ZERO:
        set_n(wp, 0, l * 2);
        break;
    } while (0);

    __sqr<__mul_mode::toom22>(p0, u0, l, stk);
    __sqr<__mul_mode::toom22>(p2, u1, rn, stk);

    T cf = 0, cf2 = 0;

    cf = addc_n(p2, p1, p2, l);
    cf2 = cf + addc_n(p1, p0, p2, l);
    cf += addc_sz(p2, p2, l, p3, rn * 2 - l);

    cf -= subc_n(p1, p1, wp, l * 2);

    cf2 = addc_1(p2, p2, l, cf2);
    if (WJR_LIKELY(rn * 2 != l)) {
        cf = addc_1(p3, p3, rn * 2 - l, cf, cf2);
    }
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom32_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    WJR_ASSERT_ASSUME(m + 2 <= n && n + 8 <= 3 * m);

    const size_t l = (2 * n >= 3 * m ? (n + 2) / 3 : (m + 1) / 2);
    const size_t rn = n - l * 2;
    const size_t rm = m - l;
    const size_t maxr = std::max(rn, rm);

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;

    const auto v0p = src1;
    const auto v1p = src1 + l;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = stk + l * 2;
    const auto w3p = dst + l * 3;

    stk += l * 4;

    T cf = 0;
    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0;
    bool neg0 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);

    // W3 = V0 + V1 : (non-negative) v(1)
    cf3 = addc_s(w3p, v0p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf3 <= 1);

    // W2 = W0 + U1 : (non-negative) u(1)
    cf2 = cf0 + addc_n(w2p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf2 <= 2);

    // W1 = W2 * W3 : (non-negative) f(1) = r1
    __mul_n<__mul_mode::toom33, T, 2, 1>(w1p, w2p, w3p, l, stk, cf1, cf2, cf3);

    // W0 = W0 - U1 : u(-1)
    {
        ssize_t p = abs_subc_n(w0p, w0p, u1p, l, cf0, cf0, 0);
        neg0 = __fasts_is_negative(p);
    }
    WJR_ASSERT_ASSUME(cf0 <= 1);

    // W3 = V0 - V1 : v(-1)
    {
        ssize_t p = abs_subc_s(w3p, v0p, l, v1p, rm);
        neg3 = __fasts_is_negative(p);
    }

    // W2 = W0 * W3 : f(-1) = r2
    neg0 ^= neg3;
    __mul_n<__mul_mode::toom33, T, 1, 0>(w2p, w0p, w3p, l, stk, cf2, cf0, 0);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    // W3 = U2 * V1 : (non-negative) f(inf) = r3
    if (rn >= rm) {
        __mul_s<__mul_mode::toom33>(w3p, u2p, rn, v1p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom33>(w3p, v1p, rm, u2p, rn, stk);
    }

    // W1 = (W1 - W2) >> 1 : (non-negative) (f(1) - f(-1)) / 2
    {
        if (!neg0) {
            cf1 = cf1 - cf2 - subc_n(w1p, w1p, w2p, l * 2);
        } else {
            cf1 = cf1 + cf2 + addc_n(w1p, w1p, w2p, l * 2);
        }

        rshift_n(w1p, w1p, l * 2, 1, cf1);
        cf1 >>= 1;
    }

    // W2 = (W1 + W2) - W0 : (non-negative) r2
    {
        if (!neg0) {
            cf2 = cf1 + cf2 + addc_n(w2p, w1p, w2p, l * 2);
        } else {
            cf2 = cf1 - cf2 - subc_n(w2p, w1p, w2p, l * 2);
        }

        cf2 -= subc_n(w2p, w2p, w0p, l * 2);
        if (l != maxr) {
            WJR_ASSERT(cf2 == 0);
            cf2 = w2p[l + maxr];
        }
    }

    // W1 = W1 - W3 : (non-negative) r1
    cf1 -= subc_s(w1p, w1p, l * 2, w3p, rn + rm);

    // W = W3*x^3+W2*x^2+W1*x+W0
    cf = addc_n(w0p + l, w0p + l, w1p, l);
    cf = addc_n(dst + l * 2, w1p + l, w2p, l, cf);
    cf = addc_n(w3p, w3p, w2p + l, maxr, cf);
    cf2 += addc_1(w3p, w3p, maxr, cf1);
    cf = addc_1(w3p + maxr, w3p + maxr, (rn + rm) - maxr, cf2, cf);
    WJR_ASSERT(cf == 0);
}

template <typename T>
struct toom_interpolation_5p_struct {
    bool neg1;
    T cf1;
    T cf2;
    T cf3;
};

template <typename T>
void toom_interpolation_5p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_5p_struct<T> &&flag) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(2);
     W4 = f(inf);
    */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const size_t maxr = std::max(rn, rm);

    const auto w0p = dst;
    const auto w2p = w0p + l * 2;
    const auto w3p = w1p + l * 2;
    const auto w4p = w0p + l * 4;

    T cf;
    auto [neg1, cf1, cf2, cf3] = flag;

    // W3 = (W3 - W1) / 3 : (non-negative) (f(2) - f(-1)) / 3
    {
        if (!neg1) {
            cf3 -= cf1 + subc_n(w3p, w3p, w1p, l * 2);
        } else {
            cf3 += cf1 + addc_n(w3p, w3p, w1p, l * 2);
        }

        divexact_by3(w3p, w3p, l * 2);
        cf3 /= 3;
    }

    // W1 = (W2 - W1) >> 1 : (non-negative) (f(1) - f(-1)) / 2
    {
        if (!neg1) {
            cf1 = cf2 - cf1 - subc_n(w1p, w2p, w1p, l * 2);
        } else {
            cf1 = cf2 + cf1 + addc_n(w1p, w2p, w1p, l * 2);
        }

        rshift_n(w1p, w1p, l * 2, 1, cf1);
        cf1 >>= 1;
    }

    // W2 = W2 - W0 : (non-negative) f(1) - f(0)
    cf2 -= subc_n(w2p, w2p, w0p, l * 2);

    // W3 = ((W3 - W2) >> 1) - (W4 << 1) : (non-negative) r3
    {
        cf3 -= cf2 + subc_n(w3p, w3p, w2p, l * 2);

        rshift_n(w3p, w3p, l * 2, 1, cf3);
        cf3 >>= 1;

        cf = submul_1(w3p, w4p, rn + rm, 2);
        if (l * 2 != (rn + rm)) {
            cf3 -= subc_1(w3p + rn + rm, w3p + rn + rm, l * 2 - (rn + rm), cf);
            if (l != maxr) {
                WJR_ASSERT(cf3 == 0);
                cf3 = w3p[l + maxr];
            }
        } else {
            cf3 -= cf;
        }
    }

    // W2 = W2 - W1 : (non-negative) f(1) / 2 - f(0) + f(-1) / 2
    cf2 -= cf1 + subc_n(w2p, w2p, w1p, l * 2);

    // W3 = W4 * x + W3 : r4 * x + r3
    cf = addc_n(w4p, w4p, w3p + l, maxr);
    cf = addc_1(w4p + maxr, w4p + maxr, rn + rm - maxr, cf3, cf);
    WJR_ASSERT(cf == 0);

    // W1 = W2 * x + W1 :
    cf = addc_n(w2p, w2p, w1p + l, l);
    cf2 += addc_1(w2p + l, w2p + l, l, cf1, cf);

    // W1 = W1 - W3 : // r2 * x + r1
    cf = subc_n(w1p, w1p, w3p, l);
    cf2 -= subc_s(w2p, w2p, l * 2, w4p, rn + rm, cf);

    // W  = W3*x^3 + W1*x + W0
    cf = addc_n(dst + l, dst + l, w1p, l);
    cf = addc_1(dst + l * 2, dst + l * 2, l, cf);
    cf = addc_n(dst + l * 3, dst + l * 3, w3p, l, cf);
    cf = addc_1(dst + l * 4, dst + l * 4, rn + rm, cf2, cf);
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom42_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(2);
     W4 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - U1 + U2 - U3) * (V0 - v1);
     W2 = (U0 + U1 + U2 + U3) * (V0 + V1);
     W3 = (U0 + 2U1 + 4U2 + 8U3) * (V0 + 2V1);
     W4 = U3 * V1;
    */

    /*
     7 add/sub + 3 addlsh

     Not implemented according to the following steps

     T0 = U0 + U2;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W3 = V0 - V1; v(-1)
     T2 = V0 + V1; v(1)
     W1 = T1 * W3; f(-1)
     W2 = T0 * T2; f(1)
     T0 = ((2U3 + U2) << 1 + U1) << 1 + U0; u(2)
     T2 = T0 + V1; v(2)
     W3 = T0 * T2; f(2)
     W0 = U0 * V0;
     W4 = U3 * V1;
    */

    WJR_ASSERT_ASSUME(3 * m + 5 <= 2 * n && n + 15 <= 4 * m);

    const size_t l = (n >= 2 * m ? (n + 3) / 4 : (m + 1) / 2);
    const size_t rn = n - l * 3;
    const size_t rm = m - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;
    const auto u3p = src0 + l * 3;

    const auto v0p = src1;
    const auto v1p = src1 + l;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;

    stk += l * 4;

    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0;
    bool neg2 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_n(w0p, u0p, u2p, l);

    // W4 = V0 + V1 : (non-negative) v(1)
    cf4 = addc_s(w4p, v0p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf4 <= 1);

    // W1 = U1 + U3 : (non-negative)
    cf1 = addc_s(w1p, u1p, l, u3p, rn);

    // W3 = W0 - W1 : u(-1)
    {
        ssize_t p = abs_subc_n(w3p, w0p, w1p, l, cf3, cf0, cf1);
        neg3 = __fasts_is_negative(p);
    }
    WJR_ASSERT_ASSUME(cf3 <= 1);

    // W2 = V0 - V1 : v(-1)
    {
        ssize_t p = abs_subc_s(w2p, v0p, l, v1p, rm);
        neg2 = __fasts_is_negative(p);
    }

    // W0 = W0 + W1 : (non-negative) u(1)
    cf0 += cf1 + addc_n(w0p, w0p, w1p, l);
    WJR_ASSERT_ASSUME(cf0 <= 3);

    // W1 = W3 * W2 : f(-1)
    neg2 ^= neg3;
    __mul_n<__mul_mode::toom33, T, 1, 0>(w1p, w3p, w2p, l, stk, cf1, cf3, 0);

    // W2 = W0 * W4 : (non-negative) f(1)
    __mul_n<__mul_mode::toom33, T, 3, 1>(w2p, w0p, w4p, l, stk, cf2, cf0, cf4);

    // W0 = U0 +(U1 +(U2 + U3<<1)<<1)<<1 : (non-negative) u(2)
    {
        cf0 = addlsh_n(w0p, u2p, u3p, rn, 1);
        if (l != rn) {
            cf0 = addc_1(w0p + rn, u2p + rn, l - rn, cf0);
        }

        cf0 += cf0 + addlsh_n(w0p, u1p, w0p, l, 1);
        cf0 += cf0 + addlsh_n(w0p, u0p, w0p, l, 1);
        WJR_ASSERT_ASSUME(cf0 <= 14);
    }

    // W4 = W4 + V1 : (non-negative) v(2)
    cf4 += addc_s(w4p, w4p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf4 <= 2);

    // W3 = W0 * W4 : (non-negative) f(2)
    __mul_n<__mul_mode::toom33, T, in_place_max, 2>(w3p, w0p, w4p, l, stk, cf3, cf0, cf4);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    // W4 = U3 * V1 : (non-negative) f(inf) = r4
    if (rn >= rm) {
        __mul_s<__mul_mode::toom33>(w4p, u3p, rn, v1p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom33>(w4p, v1p, rm, u3p, rn, stk);
    }

    return toom_interpolation_5p_s(dst, w1p, l, rn, rm,
                                   toom_interpolation_5p_struct<T>{neg2, cf1, cf2, cf3});
}

template <typename T>
void toom33_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    WJR_ASSERT_ASSUME(2 * n + 10 <= 3 * m);

    const size_t l = (n + 2) / 3;
    const size_t rn = n - l * 2;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;

    stk += l * 4;

    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0;
    bool neg2 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);
    // W4 = V0 + V2 : (non-negative)
    cf4 = addc_s(w4p, v0p, l, v2p, rm);

    // W3 = W0 - U1 : u(-1)
    {
        ssize_t p = abs_subc_n(w3p, w0p, u1p, l, cf3, cf0, 0);
        neg3 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf3 <= 1);
    }

    // W2 = W4 - V1 : v(-1)
    {
        ssize_t p = abs_subc_n(w2p, w4p, v1p, l, cf2, cf4, 0);
        neg2 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf2 <= 1);
    }

    // W0 = W0 + U1 : (non-negative) u(1)
    cf0 += addc_n(w0p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf0 <= 2);

    // W4 = W4 + V1 : (non-negative) v(1)
    cf4 += addc_n(w4p, w4p, v1p, l);
    WJR_ASSERT_ASSUME(cf4 <= 2);

    // W1 = W3 * W2 : f(-1)
    neg2 ^= neg3;
    __mul_n<__mul_mode::toom33, T, 1, 1>(w1p, w3p, w2p, l, stk, cf1, cf3, cf2);

    // W2 = W0 * W4 : (non-negative) f(1)
    __mul_n<__mul_mode::toom33, T, 2, 2>(w2p, w0p, w4p, l, stk, cf2, cf0, cf4);

    // W0 = (W0 + U2) << 1 - U0 : (non-negative) u(2)
    cf0 += addc_s(w0p, w0p, l, u2p, rn);
    cf0 += cf0 + rsblsh_n(w0p, u0p, w0p, l, 1);
    WJR_ASSERT_ASSUME(cf0 <= 6);

    // W4 = (W4 + V2) << 1 - V0 : (non-negative) v(2)
    cf4 += addc_s(w4p, w4p, l, v2p, rm);
    cf4 += cf4 + rsblsh_n(w4p, v0p, w4p, l, 1);
    WJR_ASSERT_ASSUME(cf4 <= 6);

    // W3 = W0 * W4 : (non-negative) f(2)
    __mul_n<__mul_mode::toom33>(w3p, w0p, w4p, l, stk, cf3, cf0, cf4);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    // W4 = U2 * V2 : (non-negative) f(inf) = r4
    __mul_s<__mul_mode::toom33>(w4p, u2p, rn, v2p, rm, stk);

    return toom_interpolation_5p_s(dst, w1p, l, rn, rm,
                                   toom_interpolation_5p_struct<T>{neg2, cf1, cf2, cf3});
}

template <typename T>
void toom3_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk) {
    WJR_ASSERT_ASSUME(10 <= n);

    const size_t l = (n + 2) / 3;
    const size_t rn = n - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);

    const auto u0p = src;
    const auto u1p = src + l;
    const auto u2p = src + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;

    stk += l * 4;

    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);

    // W3 = W0 - U1 : u(-1)
    {
        (void)abs_subc_n(w3p, w0p, u1p, l, cf3, cf0, 0);
        WJR_ASSERT_ASSUME(cf3 <= 1);
    }

    // W0 = W0 + U1 : (non-negative) u(1)
    cf0 += addc_n(w0p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf0 <= 2);

    // W1 = W3 * W3 : f(-1)
    __sqr<__mul_mode::toom33, T, 1>(w1p, w3p, l, stk, cf1, cf3);

    // W2 = W0 * W9 : (non-negative) f(1)
    __sqr<__mul_mode::toom33, T, 2>(w2p, w0p, l, stk, cf2, cf0);

    // W0 = (W0 + U2) << 1 - U0 : (non-negative) u(2)
    cf0 += addc_s(w0p, w0p, l, u2p, rn);
    cf0 += cf0 + rsblsh_n(w0p, u0p, w0p, l, 1);
    WJR_ASSERT_ASSUME(cf0 <= 6);

    // W3 = W0 * W0 : (non-negative) f(2)
    __sqr<__mul_mode::toom33>(w3p, w0p, l, stk, cf3, cf0);

    // W0 = U0 * U0 : (non-negative) f(0) = r0
    __sqr<__mul_mode::toom33>(w0p, u0p, l, stk);

    // W4 = U2 * U2 : (non-negative) f(inf) = r4
    __sqr<__mul_mode::toom33>(w4p, u2p, rn, stk);

    return toom_interpolation_5p_s(dst, w1p, l, rn, rn,
                                   toom_interpolation_5p_struct<T>{0, cf1, cf2, cf3});
}

template <typename T>
struct toom_interpolation_6p_struct {
    uint8_t neg1 : 1;
    uint8_t neg3 : 1;
    T cf1;
    T cf2;
    T cf3;
    T cf4;
};

template <typename T>
void toom_interpolation_6p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_6p_struct<T> &&flag) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(-2);
     W4 = f(2);
     W5 = f(inf)
    */

    /*
     11 add/sub + 4 shift + 2 divby3 + 1 submul

     W3 = (W3 - W4) >> 1;
     W4 = W4 + W3;
     W4 = W4 - W0;
     W1 = W2 - W1;
     W2 = W2 - W0;
     W3 = W3 + W1;
     W1 >>= 1;
     W3 = W3 / -6; => W3 = W3 / -2 / 3;
     W2 = W2 - W1;
     W3 = W3 - 4 * W5;
     W1 = W1 - W3;
     W4 = W4 >> 2;
     W4 = W4 - W2;
     W4 = W4 / 3;

     W4 = W5 * x + W4;
     W2 = W3 * x + W2;
     W2 = W2 - W4;
     W = W4 * x ^ 4 + W2 * x ^ 2 + W1 * x + W0;
    */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const size_t maxr = std::max(rn, rm);

    const auto w0p = dst;
    const auto w2p = w0p + l * 2;
    const auto w3p = w1p + l * 2;
    const auto w4p = w1p + l * 4;
    const auto w5p = w0p + l * 5;

    T cf;
    auto [neg1, neg3, cf1, cf2, cf3, cf4] = flag;

    // W3 = (W3 - W4) >> 1;
    {
        if (!neg3) {
            cf3 = cf4 - cf3 - subc_n(w3p, w4p, w3p, l * 2);
        } else {
            cf3 += cf4 + addc_n(w3p, w3p, w4p, l * 2);
        }

        (void)rshift_n(w3p, w3p, l * 2, 1, cf3);
        cf3 >>= 1;
    }

    // W4 = W4 + W3;
    cf4 -= cf3 + subc_n(w4p, w4p, w3p, l * 2);

    // W4 = W4 - W0;
    cf4 -= subc_n(w4p, w4p, w0p, l * 2);

    // W1 = W2 - W1;
    if (!neg1) {
        cf1 = cf2 - cf1 - subc_n(w1p, w2p, w1p, l * 2);
    } else {
        cf1 = cf2 + cf1 + addc_n(w1p, w2p, w1p, l * 2);
    }

    // W2 = W2 - W0;
    cf2 -= subc_n(w2p, w2p, w0p, l * 2);

    // W3 = W3 + W1;
    cf3 -= cf1 + subc_n(w3p, w3p, w1p, l * 2);

    // W1 >>= 1;
    (void)rshift_n(w1p, w1p, l * 2, 1, cf1);
    cf1 >>= 1;

    // W3 = W3 / -6; = > W3 = W3 / -2 / 3;
    {
        (void)rshift_n(w3p, w3p, l * 2, 1, cf3);
        divexact_by3(w3p, w3p, l * 2);
        cf3 /= 6;
    }

    // W2 = W2 - W1;
    cf2 -= cf1 + subc_n(w2p, w2p, w1p, l * 2);

    // W3 = W3 - 4 * W5;
    {
        cf = submul_1(w3p, w5p, rn + rm, 4);
        if (rn + rm != l * 2) {
            cf3 -= subc_1(w3p + rn + rm, w3p + rn + rm, l * 2 - (rn + rm), cf);
        } else {
            cf3 -= cf;
        }
    }

    // W1 = W1 - W3;
    cf1 -= cf3 + subc_n(w1p, w1p, w3p, l * 2);

    // W4 = W4 >> 2;
    rshift_n(w4p, w4p, l * 2, 2, cf4);
    cf4 >>= 2;

    // W4 = W4 - W2;
    cf4 -= cf2 + subc_n(w4p, w4p, w2p, l * 2);

    // W4 = W4 / 3;
    divexact_by3(w4p, w4p, l * 2);
    if (l != maxr) {
        cf4 = w4p[l + maxr];
    } else {
        cf4 /= 3;
    }

    // W4 = W5 * x + W4;
    cf = addc_n(w5p, w5p, w4p + l, maxr);
    cf = addc_1(w5p + maxr, w5p + maxr, rn + rm - maxr, cf4, cf);
    WJR_ASSERT(cf == 0);

    // W2 = W3 * x + W2;
    cf = addc_n(w3p, w3p, w2p + l, l);
    cf3 += addc_1(w3p + l, w3p + l, l, cf2, cf);

    // W2 = W2 - W4;
    cf = subc_n(w2p, w2p, w4p, l);
    cf3 -= subc_s(dst + l * 3, w3p, l * 2, w5p, rn + rm, cf);

    // W = W4 * x ^ 4 + W2 * x ^ 2 + W1 * x + W0;
    cf = addc_n(dst + l, dst + l, w1p, l * 2);
    cf = addc_1(dst + l * 3, dst + l * 3, l, cf1, cf);
    cf = addc_n(dst + l * 4, dst + l * 4, w4p, l, cf);
    cf = addc_1(dst + l * 5, dst + l * 5, rn + rm, cf3, cf);
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom43_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(-2);
     W4 = f(2);
     W5 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - U1 + U2 - U3) * (V0 - V1 + V2);
     W2 = (U0 + U1 + U2 + U3) * (V0 + V1 + V2);
     W3 = (U0 - 2U1 + 4U2 - 8U3) * (V0 - 2V2 + 4V3);
     W4 = (U0 + 2U1 + 4U2 + 8U3) * (V0 + 2V2 + 4V3);
     W5 = U3 * V2;
    */

    /*
        dst :
        |--- l * 2 ---|--- l * 2 ---|- l --|--- rn+rm ---|
        W0             W2            T2    W5
        |- l --|- l --|
        T0      T1
        stk :
        |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
        W1             W3            W4
     */

    /*
     11 add/sub + 4 addlsh/rsblsh + 1 shift

     W3 = U0 + U2;
     W4 = U1 + U3;
     T0 = W3 - W4; u(-1)
     T1 = W3 + W4; u(1)
     W3 = V0 + V2;
     W4 = W3 - V1; v(-1)
     W1 = T0 * W4; f(-1)
     W5 = W3 + V1; v(1)
     W2 = T1 * W5; f(1)
     W4 = (W4 + V2) << 1 - V0; v(-2)
     W5 = (W5 + V2) << 1 - V0; v(2)
     T0 = U0 + 4U2;
     T1 = (U1 + 4U3) << 1;
     T2 = T0 - T1; u(-2)
     W3 = T2 * W4; f(-2)
     T2 = T0 + T1; u(2)
     W4 = T2 * W5; f(2)
     W0 = U0 * V0;
     W5 = U3 * V2;
     */

    WJR_ASSERT_ASSUME(3 * m + 8 <= 3 * n && 2 * n + 18 <= 4 * m);

    const size_t l = (3 * n >= 4 * m ? (n + 3) / 4 : (m + 2) / 3);
    const size_t rn = n - l * 3;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = u0p + l;
    const auto u2p = u0p + l * 2;
    const auto u3p = u0p + l * 3;

    const auto v0p = src1;
    const auto v1p = v0p + l;
    const auto v2p = v0p + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = stk + l * 4;
    const auto w5p = w0p + l * 5;

    const auto t0p = w0p;
    const auto t1p = t0p + l;
    const auto t2p = w0p + l * 4;

    stk += l * 6;

    // Hand it over to the compiler to optimize reusable variables
    T cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    T cft0 = 0, cft1 = 0, cft2 = 0;
    // Reusable
    // not point to real W0...W5 or T0...T2
    bool neg0 = 0, neg1 = 0, neg2 = 0;

    //  W3 = U0 + U2;
    cf3 = addc_n(w3p, u0p, u2p, l);

    //  W4 = U1 + U3;
    cf4 = addc_s(w4p, u1p, l, u3p, rn);

    //  T0 = W3 - W4; u(-1)
    {
        ssize_t p = abs_subc_n(t0p, w3p, w4p, l, cft0, cf3, cf4);
        neg0 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cft0 <= 1);
    }

    //  T1 = W3 + W4; u(1)
    cft1 = cf3 + cf4 + addc_n(t1p, w3p, w4p, l);
    WJR_ASSERT_ASSUME(cft1 <= 3);

    //  W3 = V0 + V2;
    cf3 = addc_s(w3p, v0p, l, v2p, rm);

    //  W4 = W3 - V1; v(-1)
    {
        ssize_t p = abs_subc_n(w4p, w3p, v1p, l, cf4, cf3, 0);
        neg1 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf4 <= 1);
    }

    //  W1 = T0 * W4; f(-1)
    neg0 ^= neg1;
    __mul_n<__mul_mode::toom33, T, 1, 1>(w1p, t0p, w4p, l, stk, cf1, cft0, cf4);

    //  W5 = W3 + V1; v(1)
    cf5 = cf3 + addc_n(w5p, w3p, v1p, l);
    WJR_ASSERT_ASSUME(cf5 <= 2);

    //  W2 = T1 * W5; f(1)
    __mul_n<__mul_mode::toom33, T, 3, 2>(w2p, t1p, w5p, l, stk, cf2, cft1, cf5);

    //  W4 = (W4 + V2) << 1 - V0; v(-2)
    {
        if (!neg1) {
            cf4 += addc_s(w4p, w4p, l, v2p, rm);
        } else {
            WJR_ASSERT_ASSUME(cf4 == 0);
            ssize_t p = abs_subc_s(w4p, w4p, l, v2p, rm);
            neg1 = p > 0;
        }

        if (!neg1) {
            // W4 maybe less than V0
            // use lshfit + abs_sub instead of rsblsh_n
            cf4 += cf4 + lshift_n(w4p, w4p, l, 1);
            {
                ssize_t p = abs_subc_n(w4p, w4p, v0p, l, cf4, cf4, 0);
                neg1 = __fasts_is_negative(p);
            }
        } else {
            WJR_ASSERT_ASSUME(cf4 == 0);
            cf4 += addlsh_n(w4p, v0p, w4p, l, 1);
        }
        WJR_ASSERT_ASSUME(cf4 <= 4);
    }

    //  W5 = (W5 + V2) << 1 - V0; v(2)
    cf5 += addc_s(w5p, w5p, l, v2p, rm);
    cf5 += cf5 + rsblsh_n(w5p, v0p, w5p, l, 1);
    WJR_ASSERT_ASSUME(cf5 <= 6);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  T1 = (U1 + 4U3) << 1;
    cft1 = addlsh_n(t1p, u1p, u3p, rn, 2);
    if (l != rn) {
        cft1 = addc_1(t1p + rn, u1p + rn, l - rn, cft1);
    }
    cft1 += cft1 + lshift_n(t1p, t1p, l, 1);
    WJR_ASSERT_ASSUME(cft1 <= 9);

    //  T2 = T0 - T1; u(-2)
    {
        ssize_t p = abs_subc_n(t2p, t0p, t1p, l, cft2, cft0, cft1);
        neg2 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cft2 <= 9);
    }

    //  W3 = T2 * W4; f(-2)
    neg1 ^= neg2;
    __mul_n<__mul_mode::toom33>(w3p, t2p, w4p, l, stk, cf3, cft2, cf4);

    //  T2 = T0 + T1; u(2)
    cft2 = cft0 + cft1 + addc_n(t2p, t0p, t1p, l);
    WJR_ASSERT_ASSUME(cft2 <= 14);

    //  W4 = T2 * W5; f(2)
    __mul_n<__mul_mode::toom33>(w4p, t2p, w5p, l, stk, cf4, cft2, cf5);

    //  W0 = U0 * V0;
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    //  W5 = U3 * V2;
    if (rn >= rm) {
        __mul_s<__mul_mode::toom33>(w5p, u3p, rn, v2p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom33>(w5p, v2p, rm, u3p, rn, stk);
    }

    toom_interpolation_6p_s(
        dst, w1p, l, rn, rm,
        toom_interpolation_6p_struct<T>{neg0, neg1, cf1, cf2, cf3, cf4});
}

template <typename T>
struct toom_interpolation_7p_struct {
    uint8_t neg1 : 1;
    uint8_t neg3 : 1;
    T cf1;
    T cf2;
    T cf3;
    T cf4;
    T cf5;
};

template <typename T>
void toom_interpolation_7p_s(T *WJR_RESTRICT dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_7p_struct<T> &&flag) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);
    */

    /*
    [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   1, -2,  4, -8, 16,-32, 64
    W2   1,  1,  1,  1,  1,  1,  1
    W3   1, -1,  1, -1,  1, -1,  1
    W4   1,  2,  4,  8, 16, 32, 64
    W5  64, 32, 16,  8,  4,  2,  1
    W6   0,  0,  0,  0,  0,  0,  1
    ]

     W5 = W5 + W4
     W1 =(W4 - W1)/2
     W4 = W4 - W0
     W4 =(W4 - W1)/4 - W6*16
     W3 =(W2 - W3)/2
     W2 = W2 - W3

     [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   0,  2,  0,  8,  0, 32,  0
    W2   1,  0,  1,  0,  1,  0,  1
    W3   0,  1,  0,  1,  0,  1,  0
    W4   0,  0,  1,  0,  4,  0,  0
    W5  65, 34, 20, 16, 20, 34, 65
    W6   0,  0,  0,  0,  0,  0,  1
    ]

     W5 = W5 - W2*65      May be negative.
     W2 = W2 - W6 - W0
     W5 =(W5 + W2*45)/2   Now >= 0 again.
     W4 =(W4 - W2)/3
     W2 = W2 - W4

     [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   0,  2,  0,  8,  0, 32,  0
    W2   0,  0,  1,  0,  0,  0,  0
    W3   0,  1,  0,  1,  0,  1,  0
    W4   0,  0,  0,  0,  1,  0,  0
    W5   0, 17,  0,  8,  0, 17,  0
    W6   0,  0,  0,  0,  0,  0,  1
    ]

     W1 = W5 - W1         May be negative.
     W5 =(W5 - W3*8)/9
     W3 = W3 - W5
     W1 =(W1/15 + W5)/2   Now >= 0 again.
     W5 = W5 - W1

     [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   0,  1,  0,  0,  0,  0,  0
    W2   0,  0,  1,  0,  0,  0,  0
    W3   0,  0,  0,  1,  0,  0,  0
    W4   0,  0,  0,  0,  1,  0,  0
    W5   0,  0,  0,  0,  0,  1,  0
    W6   0,  0,  0,  0,  0,  0,  1
    ]

    */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const size_t maxr = std::max(rn, rm);

    const auto w0p = dst;
    const auto w2p = w0p + l * 2;
    const auto w3p = w1p + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = w1p + l * 4;
    const auto w6p = w0p + l * 6;

    T cf = 0;
    auto [neg1, neg3, cf1, cf2, cf3, cf4, cf5] = flag;

    //  W5 = W5 + W4
    cf5 += cf4 + addc_n(w5p, w5p, w4p, l * 2);

    //  W1 =(W4 - W1)/2
    if (!neg1) {
        WJR_ASSERT(cf4 >= cf1);
        cf1 = cf4 - cf1 - subc_n(w1p, w4p, w1p, l * 2);
    } else {
        cf1 += cf4 + addc_n(w1p, w4p, w1p, l * 2);
    }
    (void)rshift_n(w1p, w1p, l * 2, 1, cf1);
    cf1 >>= 1;

    //  W4 = W4 - W0
    cf4 -= subc_n(w4p, w4p, w0p, l * 2);

    //  W4 =(W4 - W1)/4 - W6*16
    cf4 -= cf1 + subc_n(w4p, w4p, w1p, l * 2);
    (void)rshift_n(w4p, w4p, l * 2, 2, cf4);
    cf4 >>= 2;
    cf = submul_1(w4p, w6p, rn + rm, 16);
    if (rn + rm != l * 2) {
        cf4 -= subc_1(w4p + rn + rm, w4p + rn + rm, l * 2 - (rn + rm), cf);
    } else {
        cf4 -= cf;
    }

    //  W3 =(W2 - W3)/2
    if (!neg3) {
        WJR_ASSERT(cf2 >= cf3);
        cf3 = cf2 - cf3 - subc_n(w3p, w2p, w3p, l * 2);
    } else {
        cf3 += cf2 + addc_n(w3p, w2p, w3p, l * 2);
    }
    (void)rshift_n(w3p, w3p, l * 2, 1, cf3);
    cf3 >>= 1;

    //  W2 = W2 - W3
    WJR_ASSERT(cf2 >= cf3);
    cf2 -= cf3 + subc_n(w2p, w2p, w3p, l * 2);

    //  W5 = W5 - W2*65      May be negative.
    cf5 -= cf2 * 65 + submul_1(w5p, w2p, l * 2, 65);

    //  W2 = W2 - W6 - W0
    cf2 -= subc_n(w2p, w2p, w0p, l * 2);
    cf2 -= subc_s(w2p, w2p, l * 2, w6p, rn + rm);

    //  W5 =(W5 + W2*45)/2   Now >= 0 again.
    cf5 += cf2 * 45 + addmul_1(w5p, w2p, l * 2, 45);
    (void)rshift_n(w5p, w5p, l * 2, 1, cf5);
    cf5 >>= 1;

    //  W4 =(W4 - W2)/3
    cf4 -= cf2 + subc_n(w4p, w4p, w2p, l * 2);
    divexact_by3(w4p, w4p, l * 2);
    cf4 /= 3;

    //  W2 = W2 - W4
    cf2 -= cf4 + subc_n(w2p, w2p, w4p, l * 2);

    //  W1 = W5 - W1         May be negative.
    {
        ssize_t p = abs_subc_n(w1p, w5p, w1p, l * 2, cf1, cf5, cf1);
        neg1 = __fasts_is_negative(p);
    }

    //  W5 =(W5 - W3*8)/9
    cf5 -= cf3 * 8 + submul_1(w5p, w3p, l * 2, 8);
    divexact_byc(w5p, w5p, l * 2, std::integral_constant<T, 9>{});
    cf5 /= 9;

    //  W3 = W3 - W5
    cf3 -= cf5 + subc_n(w3p, w3p, w5p, l * 2);

    //  W1 =(W1/15 + W5)/2   Now >= 0 again.
    divexact_by15(w1p, w1p, l * 2);
    cf1 /= 15;
    if (!neg1) {
        cf1 += cf5 + addc_n(w1p, w1p, w5p, l * 2);
    } else {
        cf1 = cf5 - cf1 - subc_n(w1p, w5p, w1p, l * 2);
    }
    rshift_n(w1p, w1p, l * 2, 1, cf1);
    cf1 >>= 1;

    //  W5 = W5 - W1
    cf5 -= cf1 + subc_n(w5p, w5p, w1p, l * 2);
    if (l != maxr) {
        cf5 = w5p[l + maxr];
    }

    cf = addc_n(dst + l, dst + l, w1p, l * 2);
    cf = addc_n(dst + l * 3, dst + l * 3, w3p, l, cf);
    cf2 += addc_1(dst + l * 3, dst + l * 3, l, cf1);
    cf = addc_n(dst + l * 4, dst + l * 4, w3p + l, l, cf);
    cf3 += addc_1(dst + l * 4, dst + l * 4, l, cf2);
    cf = addc_n(dst + l * 5, dst + l * 5, w5p, l, cf);
    cf4 += addc_1(dst + l * 5, dst + l * 5, l, cf3);
    cf = addc_n(dst + l * 6, dst + l * 6, w5p + l, maxr, cf);
    cf5 += addc_1(dst + l * 6, dst + l * 6, maxr, cf4);
    cf = addc_1(dst + l * 6 + maxr, dst + l * 6 + maxr, (rn + rm) - maxr, cf5, cf);
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom53_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - 2U1 + 4U2 + 8U3 - 16U4) * (V0 - 2V1 + 4V2);
     W2 = (U0 + U1 + U2 + U3 + U4) * (V0 + V1 + V2);
     W3 = (U0 - U1 + U2 - U3 + U4) * (V0 - V1 + V2);
     W4 = (U0 + 2U1 + 4U2 + 8U3 + 16U4) * (V0 + 2V1 + 4V2);
     W5 = (16U0 + 8U1 + 4U2 + 2U3 + U4) * (4V0 + 2V1 + V2);
     W6 = U4 * V2;
    */

    /*
          dst :
          |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|--- rn+rm ---|
          W0             W2            W4            W6
          |- l --|- l --|                           |- l --|
          T0      T1                                 T2
          stk :
          |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
          W1             W3            W5
     */

    /*
     11 add/sub + 11 addlsh/rsblsh + 1 shift

     T0 = U0 + U2 + U4;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W5 = V0 + V2;
     T2 = W5 + V1; v(1)
     W5 = W5 - V1; v(-1)
     W2 = T0 * T2; f(1)
     W3 = T1 * W5; f(-1)
     W5 = (W5 + V2) << 1 - V0; v(-2)
     T2 = W5 + 4V1; v(2)
     T1 = U0 + (U2 + 4U4) << 2;
     W1 = (U1 + 4U3) << 1;
     T0 = T1 + W1; u(2)
     T1 = T1 - W1; u(-2)
     W1 = T1 * W5; f(-2)
     W4 = T0 * T2; f(2)
     T0 = (((2U0 + U1) << 1 + U2) << 1 + U3) << 1 + U4; 16 * u(1/2)
     T1 = (2V0 + V1) << 1 + V2; 4 * v(1/2)
     W5 = T0 * T1; 64 * f(1/2)
     W0 = U0 * V0; f(0)
     W6 = U4 * V2; f(inf)
    */

    WJR_ASSERT_ASSUME(4 * m + 14 <= 3 * n && 2 * n + 28 <= 5 * m);

    const size_t l = 3 * n >= 5 * m ? (n + 4) / 5 : (m + 2) / 3;
    const size_t rn = n - l * 4;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;
    const auto u3p = src0 + l * 3;
    const auto u4p = src0 + l * 4;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = stk + l * 4;
    const auto w6p = w0p + l * 6;

    const auto t0p = w0p;
    const auto t1p = w0p + l;
    const auto t2p = w6p;

    stk += l * 6;

    T cf = 0;
    T cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    T cft0 = 0, cft1 = 0, cft2 = 0;
    bool neg0 = 0, neg1 = 0, neg2 = 0;

    //  T0 = U0 + U2 + U4;
    cft0 = addc_n(t0p, u0p, u2p, l);
    cft0 += addc_s(t0p, t0p, l, u4p, rn);

    //  T2 = U1 + U3;
    cft2 = addc_n(t2p, u1p, u3p, l);

    //  T1 = T0 - T2; u(-1)
    {
        ssize_t p = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        neg0 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cft1 <= 2);
    }

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  W5 = V0 + V2;
    cf5 = addc_s(w5p, v0p, l, v2p, rm);

    //  T2 = W5 + V1; v(1)
    cft2 = cf5 + addc_n(t2p, w5p, v1p, l);
    WJR_ASSERT_ASSUME(cft2 <= 2);

    //  W5 = W5 - V1; v(-1)
    {
        ssize_t p = abs_subc_n(w5p, w5p, v1p, l, cf5, cf5, 0);
        neg1 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf5 <= 1);
    }

    //  W2 = T0 * T2; f(1)
    __mul_n<__mul_mode::toom44, T, 4, 2>(w2p, t0p, t2p, l, stk, cf2, cft0, cft2);

    //  W3 = T1 * W5; f(-1)
    neg0 ^= neg1;
    __mul_n<__mul_mode::toom44, T, 2, 1>(w3p, t1p, w5p, l, stk, cf3, cft1, cf5);

    //  W5 = (W5 + V2) << 1 - V0; v(-2)
    if (!neg1) {
        cf5 += addc_s(w5p, w5p, l, v2p, rm);
    } else {
        WJR_ASSERT_ASSUME(cf5 == 0);
        ssize_t p = abs_subc_s(w5p, w5p, l, v2p, rm);
        neg1 = p > 0;
    }

    if (!neg1) {
        cf5 += cf5 + lshift_n(w5p, w5p, l, 1);
        {
            ssize_t p = abs_subc_n(w5p, w5p, v0p, l, cf5, cf5, 0);
            neg1 = __fasts_is_negative(p);
        }
    } else {
        WJR_ASSERT_ASSUME(cf5 == 0);
        cf5 += addlsh_n(w5p, v0p, w5p, l, 1);
    }
    WJR_ASSERT_ASSUME(cf5 <= 4);

    //  T2 = W5 + 4V1; v(2)
    if (!neg1) {
        cft2 = cf5 + addlsh_n(t2p, w5p, v1p, l, 2);
    } else {
        cft2 = rsblsh_n(t2p, w5p, v1p, l, 2) - cf5;
    }
    WJR_ASSERT_ASSUME(cft2 <= 6);

    //  T1 = U0 + (U2 + 4U4) << 2;
    cft1 = addlsh_n(t1p, u2p, u4p, rn, 2);
    if (l != rn) {
        cft1 = addc_1(t1p + rn, u2p + rn, l - rn, cft1);
    }
    cft1 = cft1 * 4 + addlsh_n(t1p, u0p, t1p, l, 2);

    //  W1 = (U1 + 4U3) << 1;
    cf1 = addlsh_n(w1p, u1p, u3p, l, 2);
    cf1 += cf1 + lshift_n(w1p, w1p, l, 1);

    //  T0 = T1 + W1; u(2)
    cft0 = cft1 + cf1 + addc_n(t0p, t1p, w1p, l);
    WJR_ASSERT_ASSUME(cft0 <= 30);

    //  T1 = T1 - W1; u(-2)
    {
        ssize_t p = abs_subc_n(t1p, t1p, w1p, l, cft1, cft1, cf1);
        neg2 = __fasts_is_negative(p);
    }
    WJR_ASSERT_ASSUME(cft1 <= 20);

    //  W1 = T1 * W5; f(-2)
    neg1 ^= neg2;
    __mul_n<__mul_mode::toom44, T, in_place_max, 4>(w1p, t1p, w5p, l, stk, cf1, cft1,
                                                    cf5);

    //  W4 = T0 * T2; f(2)
    __mul_n<__mul_mode::toom44, T, in_place_max, 6>(w4p, t0p, t2p, l, stk, cf4, cft0,
                                                    cft2);

    //  T0 = (((2U0 + U1) << 1 + U2) << 1 + U3) << 1 + U4; 16 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u3p, t0p, l, 1);
    cf = addlsh_n(t0p, u4p, t0p, rn, 1);
    if (l != rn) {
        cft0 += cft0 + lshift_n(t0p + rn, t0p + rn, l - rn, 1);
        cft0 += addc_1(t0p + rn, t0p + rn, l - rn, cf);
    } else {
        cft0 += cft0 + cf;
    }
    WJR_ASSERT_ASSUME(cft0 <= 30);

    //  T1 = (2V0 + V1) << 1 + V2; 4 * v(1/2)
    cft1 = addlsh_n(t1p, v1p, v0p, l, 1);
    cf = addlsh_n(t1p, v2p, t1p, rm, 1);
    if (l != rm) {
        cft1 += cft1 + lshift_n(t1p + rm, t1p + rm, l - rm, 1);
        cft1 += addc_1(t1p + rm, t1p + rm, l - rm, cf);
    } else {
        cft1 += cft1 + cf;
    }
    WJR_ASSERT_ASSUME(cft1 <= 6);

    //  W5 = T0 * T1; 64 * f(1/2)
    __mul_n<__mul_mode::toom44>(w5p, t0p, t1p, l, stk, cf5, cft0, cft1);

    //  W0 = U0 * V0; f(0)
    __mul_n<__mul_mode::toom44>(w0p, u0p, v0p, l, stk);

    //  W6 = U4 * V2; f(inf)
    if (rn >= rm) {
        __mul_s<__mul_mode::toom44>(w6p, u4p, rn, v2p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom44>(w6p, v2p, rm, u4p, rn, stk);
    }

    toom_interpolation_7p_s(
        dst, w1p, l, rn, rm,
        toom_interpolation_7p_struct<T>{neg1, neg0, cf1, cf2, cf3, cf4, cf5});
}

template <typename T>
void toom44_mul_s(T *WJR_RESTRICT dst, const T *src0, size_t n, const T *src1, size_t m,
                  T *stk) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - 2U1 + 4U2 + 8U3) * (V0 - 2V1 + 4V2 - 8V3);
     W2 = (U0 + U1 + U2 + U3) * (V0 + V1 + V2 + V3);
     W3 = (U0 - U1 + U2 - U3) * (V0 - V1 + V2 - V3);
     W4 = (U0 + 2U1 + 4U2 + 8U3) * (V0 + 2V1 + 4V2 + 8V3);
     W5 = (8U0 + 4U1 + 2U2 + U3) * (8V0 + 4V1 + 2V2 + V3);
     W6 = U3 * V3;
    */

    /*
         dst :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|--- rn+rm ---|
         W0             W2            W4            W6
         |- l --|- l --|                           |- l --|
         T0      T1                                 T2
         stk :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
         W1             W3            W5
    */

    /*
     12 add/sub + 10 addlsh + 2shift

     T0 = U0 + U2;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W1 = V0 + V2;
     T2 = V1 + V3;
     W4 = W1 + T2; v(1)
     W1 = W1 - T2; v(-1)
     W2 = T0 * W4; f(1)
     W3 = T1 * W1; f(-1)
     T0 = U0 + 4U2;
     T2 = (U1 + 4U3) << 1;
     T1 = T0 - T2; u(-2)
     T0 = T0 + T2; u(2)
     W5 = V0 + 4V2;
     T2 = (V1 + 4V3) << 1;
     W1 = W5 + T2; v(2)
     W5 = W5 - T2; v(-2)
     W4 = T0 * W1; f(2)
     W1 = T1 * W5; f(-2)
     T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
     T1 = ((2V0 + V1) << 1 + V2) << 1 + V3; 8 * v(1/2)
     W5 = T0 * T1; 64 * f(1/2)
     W0 = U0 * V0; f(0)
     W6 = U3 * V3; f(inf)
    */

    WJR_ASSERT_ASSUME(3 * n + 21 <= 4 * m);

    const size_t l = (n + 3) / 4;
    const size_t rn = n - l * 3;
    const size_t rm = m - l * 3;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;
    const auto u3p = src0 + l * 3;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;
    const auto v3p = src1 + l * 3;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = stk + l * 4;
    const auto w6p = w0p + l * 6;

    const auto t0p = w0p;
    const auto t1p = w0p + l;
    const auto t2p = w6p;

    stk += l * 6;

    T cf = 0;
    T cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    T cft0 = 0, cft1 = 0, cft2 = 0;
    bool neg0 = 0, neg1 = 0, neg2 = 0;

    //  T0 = U0 + U2;
    cft0 = addc_n(t0p, u0p, u2p, l);

    //  T2 = U1 + U3;
    cft2 = addc_s(t2p, u1p, l, u3p, rn);

    //  T1 = T0 - T2; u(-1)
    {
        ssize_t p = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        neg0 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cft1 <= 1);
    }

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 3);

    //  W1 = V0 + V2;
    cf1 = addc_n(w1p, v0p, v2p, l);

    //  T2 = V1 + V3;
    cft2 = addc_s(t2p, v1p, l, v3p, rm);

    //  W4 = W1 + T2; v(1)
    cf4 = cf1 + cft2 + addc_n(w4p, w1p, t2p, l);
    WJR_ASSERT_ASSUME(cf4 <= 3);

    //  W1 = W1 - T2; v(-1)
    {
        ssize_t p = abs_subc_n(w1p, w1p, t2p, l, cf1, cf1, cft2);
        neg1 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf1 <= 1);
    }

    //  W2 = T0 * W4; f(1)
    __mul_n<__mul_mode::toom44, T, 3, 3>(w2p, t0p, w4p, l, stk, cf2, cft0, cf4);

    //  W3 = T1 * W1; f(-1)
    neg0 ^= neg1;
    __mul_n<__mul_mode::toom44, T, 1, 1>(w3p, t1p, w1p, l, stk, cf3, cft1, cf1);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  T2 = (U1 + 4U3) << 1;
    cft2 = addlsh_n(t2p, u1p, u3p, rn, 2);
    if (l != rn) {
        cft2 = addc_1(t2p + rn, u1p + rn, l - rn, cft2);
    }
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT_ASSUME(cft2 <= 9);

    //  T1 = T0 - T2; u(-2)
    {
        ssize_t p = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        neg1 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cft1 <= 9);
    }

    //  T0 = T0 + T2; u(2)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  W5 = V0 + 4V2;
    cf5 = addlsh_n(w5p, v0p, v2p, l, 2);
    WJR_ASSERT_ASSUME(cf5 <= 4);

    //  T2 = (V1 + 4V3) << 1;
    cft2 = addlsh_n(t2p, v1p, v3p, rm, 2);
    if (l != rm) {
        cft2 = addc_1(t2p + rm, v1p + rm, l - rm, cft2);
    }
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT_ASSUME(cft2 <= 9);

    //  W1 = W5 + T2; v(2)
    cf1 = cf5 + cft2 + addc_n(w1p, w5p, t2p, l);
    WJR_ASSERT_ASSUME(cf1 <= 14);

    //  W5 = W5 - T2; v(-2)
    {
        ssize_t p = abs_subc_n(w5p, w5p, t2p, l, cf5, cf5, cft2);
        neg2 = __fasts_is_negative(p);
        WJR_ASSERT_ASSUME(cf5 <= 9);
    }

    //  W4 = T0 * W1; f(2)
    __mul_n<__mul_mode::toom44>(w4p, t0p, w1p, l, stk, cf4, cft0, cf1);

    //  W1 = T1 * W5; f(-2)
    neg1 ^= neg2;
    __mul_n<__mul_mode::toom44>(w1p, t1p, w5p, l, stk, cf1, cft1, cf5);

    //  T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    cf = addlsh_n(t0p, u3p, t0p, rn, 1);
    if (l != rn) {
        cft0 += cft0 + lshift_n(t0p + rn, t0p + rn, l - rn, 1);
        cft0 += addc_1(t0p + rn, t0p + rn, l - rn, cf);
    } else {
        cft0 += cft0 + cf;
    }
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  T1 = ((2V0 + V1) << 1 + V2) << 1 + V3; 8 * v(1/2)
    cft1 = addlsh_n(t1p, v1p, v0p, l, 1);
    cft1 += cft1 + addlsh_n(t1p, v2p, t1p, l, 1);
    cf = addlsh_n(t1p, v3p, t1p, rm, 1);
    if (l != rm) {
        cft1 += cft1 + lshift_n(t1p + rm, t1p + rm, l - rm, 1);
        cft1 += addc_1(t1p + rm, t1p + rm, l - rm, cf);
    } else {
        cft1 += cft1 + cf;
    }
    WJR_ASSERT_ASSUME(cft1 <= 14);

    //  W5 = T0 * T1; 64 * f(1/2)
    __mul_n<__mul_mode::toom44>(w5p, t0p, t1p, l, stk, cf5, cft0, cft1);

    //  W0 = U0 * V0; f(0)
    __mul_n<__mul_mode::toom44>(w0p, u0p, v0p, l, stk);

    //  W6 = U3 * V3; f(inf)
    if (rn >= rm) {
        __mul_s<__mul_mode::toom44>(w6p, u3p, rn, v3p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom44>(w6p, v3p, rm, u3p, rn, stk);
    }

    toom_interpolation_7p_s(
        dst, w1p, l, rn, rm,
        toom_interpolation_7p_struct<T>{neg1, neg0, cf1, cf2, cf3, cf4, cf5});
}

template <typename T>
void toom4_sqr(T *WJR_RESTRICT dst, const T *src, size_t n, T *stk) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);

     W0 = U0 ^ 2;
     W1 = (U0 - 2U1 + 4U2 + 8U3) ^ 2;
     W2 = (U0 + U1 + U2 + U3) ^ 2;
     W3 = (U0 - U1 + U2 - U3) ^ 2;
     W4 = (U0 + 2U1 + 4U2 + 8U3) ^ 2;
     W5 = (8U0 + 4U1 + 2U2 + U3) ^ 2;
     W6 = U3 ^ 2;
    */

    /*
         dst :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|--- rn+rm ---|
         W0             W2            W4            W6
         |- l --|- l --|                           |- l --|
         T0      T1                                 T2
         stk :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
         W1             W3            W5
    */

    /*
     12 add/sub + 10 addlsh + 2shift

     T0 = U0 + U2;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W2 = T0 ^ 2; f(1)
     W3 = T1 ^ 2; f(-1)
     T0 = U0 + 4U2;
     T2 = (U1 + 4U3) << 1;
     T1 = T0 - T2; u(-2)
     T0 = T0 + T2; u(2)
     W4 = T0 ^ 2; f(2)
     W1 = T1 ^ 2; f(-2)
     T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
     W5 = T0 ^ 2; 64 * f(1/2)
     W0 = U0 ^ 2; f(0)
     W6 = U3 ^ 2; f(inf)
    */

    WJR_ASSERT_ASSUME(21 <= n);

    const size_t l = (n + 3) / 4;
    const size_t rn = n - l * 3;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(rn * 2 >= l);

    const auto u0p = src;
    const auto u1p = src + l;
    const auto u2p = src + l * 2;
    const auto u3p = src + l * 3;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = stk + l * 4;
    const auto w6p = w0p + l * 6;

    const auto t0p = w0p;
    const auto t1p = w0p + l;
    const auto t2p = w6p;

    stk += l * 6;

    T cf = 0;
    T cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    T cft0 = 0, cft1 = 0, cft2 = 0;

    //  T0 = U0 + U2;
    cft0 = addc_n(t0p, u0p, u2p, l);

    //  T2 = U1 + U3;
    cft2 = addc_s(t2p, u1p, l, u3p, rn);

    //  T1 = T0 - T2; u(-1)
    {
        (void)abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        WJR_ASSERT_ASSUME(cft1 <= 1);
    }

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 3);

    //  W2 = T0 ^ 2; f(1)
    __sqr<__mul_mode::toom44, T, 3>(w2p, t0p, l, stk, cf2, cft0);

    //  W3 = T1 ^ 2; f(-1)
    __sqr<__mul_mode::toom44, T, 1>(w3p, t1p, l, stk, cf3, cft1);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  T2 = (U1 + 4U3) << 1;
    cft2 = addlsh_n(t2p, u1p, u3p, rn, 2);
    if (l != rn) {
        cft2 = addc_1(t2p + rn, u1p + rn, l - rn, cft2);
    }
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT_ASSUME(cft2 <= 9);

    //  T1 = T0 - T2; u(-2)
    {
        (void)abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        WJR_ASSERT_ASSUME(cft1 <= 9);
    }

    //  T0 = T0 + T2; u(2)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  W4 = T0 ^ 2; f(2)
    __sqr<__mul_mode::toom44>(w4p, t0p, l, stk, cf4, cft0);

    //  W1 = T1 * W5; f(-2)
    __sqr<__mul_mode::toom44>(w1p, t1p, l, stk, cf1, cft1);

    //  T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    cf = addlsh_n(t0p, u3p, t0p, rn, 1);
    if (l != rn) {
        cft0 += cft0 + lshift_n(t0p + rn, t0p + rn, l - rn, 1);
        cft0 += addc_1(t0p + rn, t0p + rn, l - rn, cf);
    } else {
        cft0 += cft0 + cf;
    }
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  W5 = T0 ^ 2; 64 * f(1/2)
    __sqr<__mul_mode::toom44>(w5p, t0p, l, stk, cf5, cft0);

    //  W0 = U0 * V0; f(0)
    __sqr<__mul_mode::toom44>(w0p, u0p, l, stk);

    //  W6 = U3 * V3; f(inf)
    __sqr<__mul_mode::toom44>(w6p, u3p, rn, stk);

    toom_interpolation_7p_s(
        dst, w1p, l, rn, rn,
        toom_interpolation_7p_struct<T>{0, 0, cf1, cf2, cf3, cf4, cf5});
}

} // namespace wjr

#endif // WJR_MATH_MUL_HPP__