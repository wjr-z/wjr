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

#if defined(_MSC_VER) && defined(WJR_X86)
    #define WJR_HAS_BUILTIN_MSVC_MULH64 WJR_HAS_DEF
#endif

#if defined(WJR_X86)
    #include <wjr/arch/x86/biginteger/detail/mul.hpp>
#endif

#if WJR_HAS_BUILTIN(MSVC_MULH64)
    #include <wjr/arch/x86/simd/intrin.hpp>
#endif

namespace wjr {

WJR_INTRINSIC_CONSTEXPR20 uint64_t fallback_mul_1(uint64_t *dst, const uint64_t *src, size_t n,
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

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
WJR_INTRINSIC_CONSTEXPR20 uint64_t mul_1(uint64_t *dst, const uint64_t *src, size_t n,
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

        const unsigned int c = ctz(ml);
        return lshift_n(dst, src, n, c);
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
WJR_INTRINSIC_CONSTEXPR20 uint64_t addlsh_n(uint64_t *dst, const uint64_t *src0,
                                            const uint64_t *src1, size_t n, uint64_t cl) noexcept;

// dst = (src1 << cl) - src0
WJR_INTRINSIC_CONSTEXPR20 uint64_t rsblsh_n(uint64_t *dst, const uint64_t *src0,
                                            const uint64_t *src1, size_t n, uint64_t cl) noexcept;

WJR_INTRINSIC_CONSTEXPR20 uint64_t fallback_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
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

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
WJR_INTRINSIC_CONSTEXPR20 uint64_t addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
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
    if (is_constant_evaluated()) {
        return fallback_addmul_1(dst, src, n, ml);
    }

    return asm_addmul_1(dst, src, n, ml);
#else
    return fallback_addmul_1(dst, src, n, ml);
#endif
}

WJR_INTRINSIC_CONSTEXPR20 uint64_t fallback_submul_1(uint64_t *dst, const uint64_t *src, size_t n,
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

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
WJR_INTRINSIC_CONSTEXPR20 uint64_t submul_1(uint64_t *dst, const uint64_t *src, size_t n,
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
    if (is_constant_evaluated()) {
        return fallback_submul_1(dst, src, n, ml);
    }

    return asm_submul_1(dst, src, n, ml);
#else
    return fallback_submul_1(dst, src, n, ml);
#endif
}

WJR_INTRINSIC_CONSTEXPR20 uint64_t fallback_addlsh_n(uint64_t *dst, const uint64_t *src0,
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

/*
dst = src0 + (src1 << cl);

require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
3. WJR_IS_SAME_OR_INCR_P(sdt, n, src1, n)
*/
WJR_INTRINSIC_CONSTEXPR20 uint64_t addlsh_n(uint64_t *dst, const uint64_t *src0,
                                            const uint64_t *src1, size_t n, uint64_t cl) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<uint64_t>::digits);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

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

WJR_INTRINSIC_CONSTEXPR20 uint64_t fallback_rsblsh_n(uint64_t *dst, const uint64_t *src0,
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

/*
dst = (src1 << cl) - src0;

require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
3. WJR_IS_SAME_OR_INCR_P(sdt, n, src1, n)
*/
WJR_INTRINSIC_CONSTEXPR20 uint64_t rsblsh_n(uint64_t *dst, const uint64_t *src0,
                                            const uint64_t *src1, size_t n, uint64_t cl) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<uint64_t>::digits);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

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

template <uint64_t maxn = UINT64_MAX>
WJR_INTRINSIC_CONSTEXPR20 uint64_t try_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
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

inline constexpr size_t toom22_mul_threshold = WJR_TOOM22_MUL_THRESHOLD;
inline constexpr size_t toom33_mul_threshold = WJR_TOOM33_MUL_THRESHOLD;

inline constexpr size_t toom2_sqr_threshold = WJR_TOOM2_SQR_THRESHOLD;
inline constexpr size_t toom3_sqr_threshold = WJR_TOOM3_SQR_THRESHOLD;

enum class __mul_mode : uint8_t {
    toom22 = 0x00,
    toom33 = 0x01,
    all = 0x03,
};

WJR_INTRINSIC_INLINE void mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                const uint64_t *src1, size_t m) noexcept;

WJR_INTRINSIC_INLINE void mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                const uint64_t *src1, size_t n) noexcept;

WJR_INTRINSIC_INLINE void basecase_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                         const uint64_t *src1, size_t m) noexcept;

WJR_INTRINSIC_INLINE void basecase_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                       size_t n) noexcept;

WJR_INTRINSIC_INLINE void sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n) noexcept;

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

template <size_t P>
using toom_interpolation_high_p_struct = std::array<uint64_t, P - 2>;

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

WJR_CONST WJR_INTRINSIC_CONSTEXPR size_t toom22_s_itch(size_t m) noexcept {
    return m * 4 + (m / 2) + 64;
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR20 size_t toom22_n_itch(size_t n) noexcept { return n * 2 + 64; }

WJR_CONST WJR_INTRINSIC_CONSTEXPR size_t toom33_s_itch(size_t m) noexcept {
    return m * 4 + (m / 2) + 64;
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR size_t toom33_n_itch(size_t m) noexcept { return m * 2 + 64; }

WJR_CONST WJR_INTRINSIC_CONSTEXPR size_t toom44_n_itch(size_t m) noexcept { return m * 2 + 64; }

WJR_CONST WJR_INTRINSIC_CONSTEXPR size_t toom55_n_itch(size_t m) noexcept {
    return m * 3 + (m / 2) + 32;
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR bool toom44_ok(size_t n, size_t m) noexcept {
    return 3 * n + 21 <= 4 * m;
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR bool toom55_ok(size_t n, size_t m) noexcept {
    return 4 * n + 36 <= 5 * m;
}

extern WJR_ALL_NONNULL void __noinline_mul_s_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                                  size_t n, const uint64_t *src1,
                                                  size_t m) noexcept;

WJR_INTRINSIC_INLINE void mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                                const uint64_t *src1, size_t m) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(n == m)) {
        return mul_n(dst, src0, src1, n);
    }

    return __noinline_mul_s_impl(dst, src0, n, src1, m);
}

template <typename T>
uint64_t *__mul_s_allocate(T &al, WJR_MAYBE_UNUSED size_t n) noexcept {
    if constexpr (std::is_same_v<T, uint64_t *>) {
        return al;
    } else {
        return static_cast<uint64_t *>(al.allocate(sizeof(uint64_t) * n));
    }
}

template <__mul_mode mode>
void __inline_mul_n_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, const uint64_t *src1,
                         size_t n, uint64_t *mal) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n * 2, src0, n));
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n * 2, src1, n));

    if (n < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, n);
    }

    if (mode <= __mul_mode::toom22 || n < toom33_mul_threshold) {
        uint64_t *stk = __mul_s_allocate(mal, toom22_n_itch(n));
        return toom22_mul_s(dst, src0, n, src1, n, stk);
    }

    uint64_t *stk = __mul_s_allocate(mal, toom33_n_itch(n));
    return toom33_mul_s(dst, src0, n, src1, n, stk);
}

extern WJR_ALL_NONNULL void __noinline_mul_n_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                                  const uint64_t *src1, size_t n) noexcept;

template <__mul_mode mode>
WJR_INTRINSIC_INLINE void __mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                  const uint64_t *src1, size_t n,
                                  WJR_MAYBE_UNUSED uint64_t *stk) noexcept {
    if constexpr (mode <= __mul_mode::toom33) {
        __inline_mul_n_impl<mode>(dst, src0, src1, n, stk);
    } else {
        mul_n(dst, src0, src1, n);
    }
}

template <__mul_mode mode, uint64_t m0 = UINT64_MAX, uint64_t m1 = UINT64_MAX>
void __mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, const uint64_t *src1, size_t n,
             uint64_t *stk, uint64_t &c_out, uint64_t cf0, uint64_t cf1) noexcept {
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

    c_out += try_addmul_1<m0>(dst + n, src1, n, cf0);
    c_out += try_addmul_1<m1>(dst + n, src0, n, cf1);
}

WJR_INTRINSIC_INLINE void mul_n(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                const uint64_t *src1, size_t n) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(src0 == src1)) {
        return sqr(dst, src0, n);
    }

    return __noinline_mul_n_impl(dst, src0, src1, n);
}

template <__mul_mode mode>
inline void __inline_sqr_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                              uint64_t *mal) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n * 2, src, n));

    if (n < toom2_sqr_threshold) {
        return basecase_sqr(dst, src, n);
    }

    if (mode <= __mul_mode::toom22 || n < toom3_sqr_threshold) {
        uint64_t *stk = __mul_s_allocate(mal, toom22_n_itch(n));
        return toom2_sqr(dst, src, n, stk);
    }

    uint64_t *stk = __mul_s_allocate(mal, toom33_n_itch(n));
    return toom3_sqr(dst, src, n, stk);
}

extern WJR_ALL_NONNULL void __noinline_sqr_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                                size_t n) noexcept;

template <__mul_mode mode>
void __sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
           WJR_MAYBE_UNUSED uint64_t *stk) noexcept {
    if constexpr (mode <= __mul_mode ::toom33) {
        __inline_sqr_impl<mode>(dst, src, n, stk);
    } else {
        sqr(dst, src, n);
    }
}

template <__mul_mode mode, uint64_t m = UINT64_MAX>
void __sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n, uint64_t *stk,
           uint64_t &c_out, uint64_t cf) noexcept {
    WJR_ASSERT_ASSUME(cf <= m);

    __sqr<mode>(dst, src, n, stk);

    if constexpr (m == 0) {
        c_out = 0;
    } else if constexpr (m == 1) {
        c_out = cf;
    } else {
        c_out = cf * cf;
    }

    constexpr auto m2 = in_range<uint32_t>(m) ? m * 2 : m;
    c_out += try_addmul_1<m2>(dst + n, src, n, 2 * cf);
}

WJR_INTRINSIC_INLINE void sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n) noexcept {
    return __noinline_sqr_impl(dst, src, n);
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
WJR_INTRINSIC_INLINE void basecase_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
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

WJR_INTRINSIC_INLINE void basecase_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                       size_t n) noexcept {
#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR)
    return asm_basecase_sqr(dst, src, n);
#else
    return basecase_mul_s(dst, src, n, src, n);
#endif
}

} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_MUL_HPP__