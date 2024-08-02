#ifndef WJR_MATH_SUB_HPP__
#define WJR_MATH_SUB_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/detail.hpp>
#include <wjr/math/replace.hpp>
#include <wjr/math/sub-impl.hpp>

#if defined(WJR_X86)
#include <wjr/x86/math/sub.hpp>
#endif

namespace wjr {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_subc(T a, T b, U c_in, U &c_out) noexcept {
    T ret = a - b;
    U c = ret > a;
    a = ret;
    ret -= c_in;
    c |= ret > a;
    c_out = c;
    return ret;
}

#if WJR_HAS_BUILTIN(__builtin_subc)
#define WJR_HAS_BUILTIN_SUBC WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T builtin_subc(T a, T b, U c_in, U &c_out) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_SUBC(suffix, type)                                          \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        type __c_out;                                                                    \
        T ret = __builtin_subc##suffix(a, b, static_cast<type>(c_in), &__c_out);         \
        c_out = static_cast<U>(__c_out);                                                 \
        return ret;                                                                      \
    } else

    WJR_REGISTER_BUILTIN_SUBC(b, unsigned char)
    WJR_REGISTER_BUILTIN_SUBC(s, unsigned short)
    WJR_REGISTER_BUILTIN_SUBC(, unsigned int)
    WJR_REGISTER_BUILTIN_SUBC(l, unsigned long)
    WJR_REGISTER_BUILTIN_SUBC(ll, unsigned long long) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_SUBC
}

#endif // WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U,
          WJR_REQUIRES_I(is_nonbool_unsigned_integral_v<T> &&is_unsigned_integral_v<U>)>
WJR_INTRINSIC_CONSTEXPR20 T subc(T a, T b, type_identity_t<U> c_in, U &c_out) noexcept {
    WJR_ASSERT_ASSUME_L2(c_in <= 1);

#if !WJR_HAS_BUILTIN(SUBC) && !WJR_HAS_BUILTIN(ASM_SUBC)
    return fallback_subc(a, b, c_in, c_out);
#else
    constexpr auto is_constant_or_zero = [](auto x) -> int {
        return WJR_BUILTIN_CONSTANT_P(x == 0) && x == 0 ? 2
               : WJR_BUILTIN_CONSTANT_P(x)              ? 1
                                                        : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_subc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return WJR_PP_BOOL_IF_NE(WJR_HAS_BUILTIN(ASM_SUBC), asm_subc,
                                 WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(SUBC), builtin_subc,
                                                fallback_subc))(a, b, c_in, c_out);
    } else {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(SUBC), builtin_subc,
                              fallback_subc)(a, b, c_in, c_out);
    }
#endif
}

/*
 Used for subc and then jump according to cc flag. Therefore, the types of c_in and
 c_out are limited to uint8_t, while the default c_in and c_out types of normal subc are
 the same as T, so that the high register is not cleared. Currently, GCC/Clang @=cccond
 cannot know that the high register is not cleared, so the performance is worse than the
 normal version when cc flag is not needed immediately.
*/
template <typename T, WJR_REQUIRES_I(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 T subc_cc(T a, T b, uint8_t c_in, uint8_t &c_out) noexcept {
    WJR_ASSERT_ASSUME_L2(c_in <= 1);

#if WJR_HAS_BUILTIN(ASM_SUBC_CC)
    constexpr auto is_constant_or_zero = [](auto x) -> int {
        return WJR_BUILTIN_CONSTANT_P(x == 0) && x == 0 ? 2
               : WJR_BUILTIN_CONSTANT_P(x)              ? 1
                                                        : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_subc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return asm_subc_cc(a, b, c_in, c_out);
    } else {
        return subc(a, b, c_in, c_out);
    }
#else
    return subc(a, b, c_in, c_out);
#endif
}

#if WJR_HAS_BUILTIN(__builtin_sub_overflow)
#define WJR_HAS_BUILTIN_SUB_OVERFLOW WJR_HAS_DEF
#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR20 bool fallback_sub_overflow(T a, T b, T &ret) noexcept {
    ret = a - b;
    return ret > a;
}

template <typename T, WJR_REQUIRES_I(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 bool sub_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret) noexcept {
#if WJR_HAS_BUILTIN(SUB_OVERFLOW)
    if (is_constant_evaluated() ||
        (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
        return fallback_sub_overflow(a, b, ret);
    }

    return __builtin_sub_overflow(a, b, &ret);
#else
    return fallback_sub_overflow(a, b, ret);
#endif
}

template <typename U>
WJR_INTRINSIC_CONSTEXPR20 U __subc_1_impl(uint64_t *dst, const uint64_t *src0, size_t n,
                                          uint64_t src1, U c_in) noexcept {
    uint8_t overflow = 0;
    dst[0] = subc_cc(src0[0], src1, c_in, overflow);

    if (overflow) {
        size_t idx = 1 + replace_find_not(dst + 1, src0 + 1, n - 1, 0, UINT64_MAX);

        if (WJR_UNLIKELY(idx == n)) {
            return static_cast<U>(1);
        }

        dst[idx] = src0[idx] - 1;

        dst += idx;
        src0 += idx;
        n -= idx;
    }

    if (src0 != dst) {
        std::copy(src0 + 1, src0 + n, dst + 1);
    }

    return static_cast<U>(0);
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
*/
template <typename U, WJR_REQUIRES_I(is_unsigned_integral_v<U>)>
WJR_INTRINSIC_CONSTEXPR20 U subc_1(uint64_t *dst, const uint64_t *src0, size_t n,
                                   uint64_t src1, U c_in) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));

    if (WJR_BUILTIN_CONSTANT_P_TRUE(n == 1)) {
        uint8_t overflow = 0;
        dst[0] = subc_cc(src0[0], src1, c_in, overflow);
        return static_cast<U>(overflow);
    }

    return __subc_1_impl(dst, src0, n, src1, c_in);
}

template <typename U>
WJR_INTRINSIC_CONSTEXPR U fallback_subc_n(uint64_t *dst, const uint64_t *src0,
                                          const uint64_t *src1, size_t n,
                                          U c_in) noexcept {
    size_t m = n / 4;

    for (size_t i = 0; i < m; ++i) {
        dst[0] = subc(src0[0], src1[0], c_in, c_in);
        dst[1] = subc(src0[1], src1[1], c_in, c_in);
        dst[2] = subc(src0[2], src1[2], c_in, c_in);
        dst[3] = subc(src0[3], src1[3], c_in, c_in);

        dst += 4;
        src0 += 4;
        src1 += 4;
    }

    n &= 3;
    if (WJR_UNLIKELY(n == 0)) {
        return c_in;
    }

    dst += n;
    src0 += n;
    src1 += n;

    switch (n) {
    case 3: {
        dst[-3] = subc(src0[-3], src1[-3], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 2: {
        dst[-2] = subc(src0[-2], src1[-2], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 1: {
        dst[-1] = subc(src0[-1], src1[-1], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    default: {
        break;
    }
    }

    return c_in;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
3. WJR_IS_SAME_OR_INCR_P(dst, n, src1, n)
*/
template <typename U, WJR_REQUIRES_I(is_unsigned_integral_v<U>)>
WJR_INTRINSIC_CONSTEXPR20 U subc_n(uint64_t *dst, const uint64_t *src0,
                                   const uint64_t *src1, size_t n, U c_in) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

#if WJR_HAS_BUILTIN(ASM_SUBC_N)
    if (is_constant_evaluated()) {
        return fallback_subc_n(dst, src0, src1, n, c_in);
    }

    return asm_subc_n(dst, src0, src1, n, c_in);
#else
    return fallback_subc_n(dst, src0, src1, n, c_in);
#endif
}

/*
require :
1. m >= 1
2. n >= m
3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
*/
template <typename U, WJR_REQUIRES_I(is_unsigned_integral_v<U>)>
WJR_INTRINSIC_CONSTEXPR20 U subc_s(uint64_t *dst, const uint64_t *src0, size_t n,
                                   const uint64_t *src1, size_t m, U c_in) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    c_in = subc_n(dst, src0, src1, m, c_in);

    if (n != m) {
        c_in = subc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

/*
require :
1. n >= 0
2. n >= m
3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
*/
template <typename U, WJR_REQUIRES_I(is_unsigned_integral_v<U>)>
WJR_INTRINSIC_CONSTEXPR20 U subc_sz(uint64_t *dst, const uint64_t *src0, size_t n,
                                    const uint64_t *src1, size_t m, U c_in) noexcept {
    WJR_ASSERT_ASSUME(n >= m);

    if (WJR_LIKELY(m != 0)) {
        c_in = subc_n(dst, src0, src1, m, c_in);
    }

    if (n != m) {
        c_in = subc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_n(uint64_t *dst, const uint64_t *src0,
                                             const uint64_t *src1, size_t n) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_SEPARATE_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_SEPARATE_P(dst, n, src1, n));

    size_t idx = reverse_find_not_n(src0, src1, n);

    if (WJR_UNLIKELY(idx != n)) {
        set_n(dst + idx, 0, n - idx);

        if (WJR_UNLIKELY(idx == 0)) {
            return 0;
        }
    }

    WJR_ASSUME(idx >= 1);

    uint64_t hi = 0;
    WJR_ASSUME(src0[idx - 1] != src1[idx - 1]);
    const bool overflow = sub_overflow(src0[idx - 1], src1[idx - 1], hi);

    if (overflow) {
        std::swap(src0, src1);
        hi = -hi;
    }

    do {
        if (WJR_UNLIKELY(idx == 1)) {
            dst[0] = hi;
            break;
        }

        hi -= subc_n(dst, src0, src1, idx - 1);
        dst[idx - 1] = hi;
    } while (0);

    return overflow ? -1 : 1;
}

WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_n_pos(uint64_t *dst, const uint64_t *src0,
                                                 const uint64_t *src1,
                                                 size_t n) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_SEPARATE_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_SEPARATE_P(dst, n, src1, n));

    size_t idx = reverse_find_not_n(src0, src1, n);

    if (WJR_UNLIKELY(idx != n)) {
        set_n(dst + idx, 0, n - idx);

        if (WJR_UNLIKELY(idx == 0)) {
            return 0;
        }
    }

    WJR_ASSUME(idx >= 1);

    uint64_t hi = 0;
    WJR_ASSUME(src0[idx - 1] != src1[idx - 1]);
    const bool overflow = sub_overflow(src0[idx - 1], src1[idx - 1], hi);

    if (overflow) {
        std::swap(src0, src1);
        hi = -hi;
    }

    ssize_t ret = __fasts_from_unsigned(idx);
    WJR_ASSUME(ret >= 1);

    do {
        if (WJR_UNLIKELY(idx == 1)) {
            dst[0] = hi;
            break;
        }

        WJR_ASSUME(ret >= 2);

        hi -= subc_n(dst, src0, src1, idx - 1);

        if (WJR_LIKELY(hi != 0)) {
            dst[idx - 1] = hi;
        } else {
            --ret;
        }
    } while (0);

    WJR_ASSUME(ret > 0);
    return overflow ? -ret : ret;
}

WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_s1(uint64_t *dst, const uint64_t *src0,
                                              size_t n, const uint64_t *src1,
                                              size_t m) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    if (WJR_BUILTIN_CONSTANT_P_TRUE(n == m)) {
        return abs_subc_n(dst, src0, src1, m);
    }

    WJR_ASSERT(n - m <= 1);

    do {
        if (n == m) {
            break;
        }

        if (WJR_UNLIKELY(src0[m] == 0)) {
            dst[m] = 0;
            break;
        }

        uint64_t hi = src0[m];
        hi -= subc_n(dst, src0, src1, m);
        dst[m] = hi;
        return 1;
    } while (0);

    return abs_subc_n(dst, src0, src1, m);
}

WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_s(uint64_t *dst, const uint64_t *src0,
                                             size_t n, const uint64_t *src1,
                                             size_t m) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    if (WJR_BUILTIN_CONSTANT_P_TRUE(n == m)) {
        return abs_subc_n(dst, src0, src1, m);
    }

    if (WJR_BUILTIN_CONSTANT_P_TRUE(n - m <= 1)) {
        return abs_subc_s1(dst, src0, n, src1, m);
    }

    size_t idx = reverse_replace_find_not(dst + m, src0 + m, n - m, 0, 0);

    if (WJR_UNLIKELY(idx == 0)) {
        return abs_subc_n(dst, src0, src1, m);
    }

    (void)subc_s(dst, src0, m + idx, src1, m);
    return 1;
}

WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_s_pos(uint64_t *dst, const uint64_t *src0,
                                                 size_t n, const uint64_t *src1,
                                                 size_t m) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    if (WJR_BUILTIN_CONSTANT_P_TRUE(n == m)) {
        return abs_subc_n_pos(dst, src0, src1, m);
    }

    if (WJR_BUILTIN_CONSTANT_P_TRUE(n - m <= 1)) {
        do {
            if (n == m) {
                break;
            }

            if (WJR_UNLIKELY(src0[m] == 0)) {
                dst[m] = 0;
                break;
            }

            uint64_t hi = src0[m];
            hi -= subc_n(dst, src0, src1, m);
            ssize_t ret = __fasts_from_unsigned(m + 1);

            if (WJR_LIKELY(hi != 0)) {
                dst[m] = hi;
            } else {
                --ret;
            }

            WJR_ASSUME(ret > 0);
            return ret;
        } while (0);

        return abs_subc_n_pos(dst, src0, src1, m);
    }

    size_t idx = reverse_replace_find_not(dst + m, src0 + m, n - m, 0, 0);

    if (WJR_UNLIKELY(idx == 0)) {
        return abs_subc_n_pos(dst, src0, src1, m);
    }

    (void)subc_s(dst, src0, m + idx, src1, m);
    uint64_t ret = __fasts_from_unsigned(m + idx);
    WJR_ASSUME(ret >= 2);
    ret -= dst[m + idx - 1] == 0;
    WJR_ASSUME(ret >= 1);
    return ret;
}

// just like abs_subc_n.
template <typename U, WJR_REQUIRES_I(is_unsigned_integral_v<U>)>
WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_n(uint64_t *dst, const uint64_t *src0,
                                             const uint64_t *src1, size_t n, U &c_out,
                                             type_identity_t<U> cf0,
                                             type_identity_t<U> cf1) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    if (cf0 != cf1) {
        ssize_t ret = __fasts_from_unsigned(n);
        U cf = 0;
        if (cf0 < cf1) {
            std::swap(src0, src1);
            ret = __fasts_negate(ret);
            cf = cf1 - cf0;
        } else {
            cf = cf0 - cf1;
        }

        c_out = cf - subc_n(dst, src0, src1, n);
        return ret;
    } else {
        c_out = 0;
        return abs_subc_n(dst, src0, src1, n);
    }
}

WJR_INTRINSIC_CONSTEXPR void __fallback_sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1,
                                                uint64_t hi1) noexcept {
    uint64_t __al = lo0 - lo1;
    ah = hi0 - hi1 - (__al > lo0);
    al = __al;
}

#if WJR_HAS_FEATURE(FAST_INT128_ADDSUB)
#define WJR_HAS_BUILTIN___BUILTIN_SUB_128 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(__BUILTIN_SUBC_128)

WJR_INTRINSIC_INLINE void __builtin_sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                            uint64_t hi0, uint64_t lo1,
                                            uint64_t hi1) noexcept {
    const auto x0 = static_cast<__uint128_t>(hi0) << 64 | lo0;
    const auto x1 = static_cast<__uint128_t>(hi1) << 64 | lo1;
    x0 -= x1;

    al = x0;
    ah = x0 >> 64;
}

#endif

// <ah, al> = <hi0, lo0> - <hi1, lo1>
WJR_INTRINSIC_CONSTEXPR20 void __sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                         uint64_t hi0, uint64_t lo1,
                                         uint64_t hi1) noexcept {
#if WJR_HAS_BUILTIN(__BUILTIN_SUB_128) || WJR_HAS_BUILTIN(__ASM_SUB_128)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P_TRUE(lo0 == 0) ||
        WJR_BUILTIN_CONSTANT_P_TRUE(lo1 == 0) || WJR_BUILTIN_CONSTANT_P(lo0 + hi0)) {
        return __fallback_sub_128(al, ah, lo0, hi0, lo1, hi1);
    }

    return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(__BUILTIN_SUB_128), __builtin_sub_128,
                          __asm_sub_128)(al, ah, lo0, hi0, lo1, hi1);
#else
    return __fallback_sub_128(al, ah, lo0, hi0, lo1, hi1);
#endif
}

WJR_INTRINSIC_CONSTEXPR20 uint64_t __fallback_subc_128(uint64_t &al, uint64_t &ah,
                                                       uint64_t lo0, uint64_t hi0,
                                                       uint64_t lo1, uint64_t hi1,
                                                       uint64_t c_in) noexcept {
    al = subc(lo0, lo1, c_in, c_in);
    ah = subc(hi0, hi1, c_in, c_in);
    return c_in;
}

// return c_out
WJR_INTRINSIC_CONSTEXPR20 uint64_t __subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                              uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                              uint64_t c_in) noexcept {
#if WJR_HAS_BUILTIN(__ASM_SUBC_128)
    if (is_constant_evaluated()) {
        return __fallback_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
    }

    return __asm_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return __fallback_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#endif
}

WJR_INTRINSIC_CONSTEXPR20 uint8_t __subc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                                uint8_t c_in) noexcept {
#if WJR_HAS_BUILTIN(__ASM_SUBC_CC_128)
    if (is_constant_evaluated()) {
        return __fallback_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
    }

    return __asm_subc_cc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return fast_cast<uint8_t>(__subc_128(al, ah, lo0, hi0, lo1, hi1, c_in));
#endif
}

} // namespace wjr

#endif // WJR_MATH_SUB_HPP__