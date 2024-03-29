#ifndef WJR_MATH_ADD_HPP__
#define WJR_MATH_ADD_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/add-impl.hpp>
#include <wjr/math/replace.hpp>

#if defined(WJR_X86)
#include <wjr/x86/math/add.hpp>
#endif

namespace wjr {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_addc(T a, T b, U c_in, U &c_out) {
    T ret = a;
    ret += b;
    U c = ret < b;
    ret += c_in;
    c |= ret < c_in;
    c_out = c;
    return ret;
}

#if WJR_HAS_BUILTIN(__builtin_addc)
#define WJR_HAS_BUILTIN_ADDC WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ADDC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T builtin_addc(T a, T b, U c_in, U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ADDC(suffix, type)                                          \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        type __c_out;                                                                    \
        T ret = __builtin_addc##suffix(a, b, static_cast<type>(c_in), &__c_out);         \
        c_out = static_cast<U>(__c_out);                                                 \
        return ret;                                                                      \
    } else

    WJR_REGISTER_BUILTIN_ADDC(b, unsigned char)
    WJR_REGISTER_BUILTIN_ADDC(s, unsigned short)
    WJR_REGISTER_BUILTIN_ADDC(, unsigned int)
    WJR_REGISTER_BUILTIN_ADDC(l, unsigned long)
    WJR_REGISTER_BUILTIN_ADDC(ll, unsigned long long) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ADDC
}

#endif // WJR_HAS_BUILTIN(ADDC)

/**
 * @brief Add two numbers with carry-in, and return the result and carry-out
 *
 * @note The carry-in and carry-out are limited to 0 and 1
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral type.
 * the default type is the same as `T`
 * @param[in] c_in The carry-in flag.
 * @param[out] c_out The carry-out flag.
 */
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E T addc(T a, T b, type_identity_t<U> c_in, U &c_out) {
    WJR_ASSERT_ASSUME_L1(c_in <= 1);

#if !WJR_HAS_BUILTIN(ADDC) && !WJR_HAS_BUILTIN(ASM_ADDC)
    return fallback_addc(a, b, c_in, c_out);
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
        return fallback_addc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ASM_ADDC), asm_addc,
                              WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ADDC), builtin_addc,
                                             fallback_addc))(a, b, c_in, c_out);
    } else {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ADDC), builtin_addc,
                              fallback_addc)(a, b, c_in, c_out);
    }
#endif
}

/**
 * @brief Performs addition with carry-in and carry-out, optimized for subsequent
 * branching based on carry-out.
 *
 * @details This function, `addc_cc`, adds two numbers with a carry-in, and returns the
 * result and a carry-out. The carry-out (`c_out`) is optimized for subsequent code that
 * branches based on its value. For example, it can be used with jump instructions like
 * `je` or `jne`. This is in contrast to the `addc` function, which may use instructions
 * like `setc` or `test` for branching.
 *
 * @note The carry-in and carry-out are limited to 0 and 1
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral type.
 * @param[in] c_in The carry-in flag.
 * @param[out] c_out The carry-out flag.
 */
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E T addc_cc(T a, T b, uint8_t c_in, uint8_t &c_out) {
    WJR_ASSERT_ASSUME_L1(c_in <= 1);

#if WJR_HAS_BUILTIN(ASM_ADDC_CC)
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
        return fallback_addc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return asm_addc_cc(a, b, c_in, c_out);
    } else {
        return addc(a, b, c_in, c_out);
    }
#else
    return addc(a, b, c_in, c_out);
#endif
}

#if WJR_HAS_BUILTIN(__builtin_add_overflow)
#define WJR_HAS_BUILTIN_ADD_OVERFLOW WJR_HAS_DEF
#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E bool fallback_add_overflow(T a, T b, T &ret) {
    ret = a + b;
    return ret < a;
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E bool add_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret) {
#if WJR_HAS_BUILTIN(ADD_OVERFLOW)
    if (is_constant_evaluated() ||
        (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
        return fallback_add_overflow(a, b, ret);
    }

    return __builtin_add_overflow(a, b, &ret);
#else
    return fallback_add_overflow(a, b, ret);
#endif
}

/**
 * @brief Add biginteger(src0) and number with carry-in, and return the result(dst) and
 * carry-out.
 *
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral type.
 * @param[out] dst The result of the addition.
 * @param[in] src0 The biginteger to be added.
 * @param[in] n The number of elements in the biginteger.
 * @param[in] src1 The number to be added.
 * @param[in] c_in The carry-in flag.
 * @return The carry-out flag.
 */
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U addc_1(T *dst, const T *src0, size_t n,
                                   type_identity_t<T> src1, U c_in) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_ASSUME(c_in <= 1);

    uint8_t overflow = 0;
    dst[0] = addc_cc(src0[0], src1, c_in, overflow);

    if (overflow) {
        size_t idx = 1 + replace_find_not(dst + 1, src0 + 1, n - 1, -1, 0);

        if (WJR_UNLIKELY(idx == n)) {
            return static_cast<U>(1);
        }

        dst[idx] = src0[idx] + 1;

        dst += idx;
        src0 += idx;
        n -= idx;
    }

    if (src0 != dst) {
        std::copy(src0 + 1, src0 + n, dst + 1);
    }

    return static_cast<U>(0);
}

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U fallback_addc_n(T *dst, const T *src0, const T *src1, size_t n,
                                          U c_in) {
    size_t m = n / 4;

    for (size_t i = 0; i < m; ++i) {
        dst[0] = addc(src0[0], src1[0], c_in, c_in);
        dst[1] = addc(src0[1], src1[1], c_in, c_in);
        dst[2] = addc(src0[2], src1[2], c_in, c_in);
        dst[3] = addc(src0[3], src1[3], c_in, c_in);

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
        dst[-3] = addc(src0[-3], src1[-3], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 2: {
        dst[-2] = addc(src0[-2], src1[-2], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    case 1: {
        dst[-1] = addc(src0[-1], src1[-1], c_in, c_in);
        WJR_FALLTHROUGH;
    }
    default: {
        break;
    }
    }

    return c_in;
}

/**
 * @brief Add biginteger(src0) and biginteger(src1) with carry-in, and return the result
 * (dst) and carry-out.
 *
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral type.
 * @param[out] dst The result of the addition.
 * @param[in] src0 The biginteger to be added.
 * @param[in] src1 The biginteger to be added.
 * @param[in] n The number of elements in the biginteger.
 * @param[in] c_in The carry-in flag.
 * @return The carry-out flag.
 */
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U addc_n(T *dst, const T *src0, const T *src1, size_t n,
                                   U c_in) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

#if WJR_HAS_BUILTIN(ASM_ADDC_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_addc_n(dst, src0, src1, n, c_in);
        }

        return asm_addc_n(dst, src0, src1, n, c_in);
    } else {
        return fallback_addc_n(dst, src0, src1, n, c_in);
    }
#else
    return fallback_addc_n(dst, src0, src1, n, c_in);
#endif
}

/*
require :
1. m >= 1
2. n >= m
3. WJR_IS_SAME_OR_INCR_P(dst, n, src0, n)
4. WJR_IS_SAME_OR_INCR_P(dst, m, src1, m)
*/
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U addc_s(T *dst, const T *src0, size_t n, const T *src1,
                                   size_t m, U c_in) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

    c_in = addc_n(dst, src0, src1, m, c_in);

    if (n != m) {
        c_in = addc_1(dst + m, src0 + m, n - m, 0, c_in);
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
template <
    typename T, typename U,
    std::enable_if_t<is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int>>
WJR_INTRINSIC_CONSTEXPR_E U addc_sz(T *dst, const T *src0, size_t n, const T *src1,
                                    size_t m, U c_in) {
    WJR_ASSERT_ASSUME(n >= m);

    if (WJR_LIKELY(m != 0)) {
        c_in = addc_n(dst, src0, src1, m, c_in);
    }

    if (n != m) {
        c_in = addc_1(dst + m, src0 + m, n - m, 0, c_in);
    }

    return c_in;
}

WJR_INTRINSIC_CONSTEXPR void __fallback_add_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1,
                                                uint64_t hi1) {
    uint64_t __al = lo0 + lo1;
    ah = hi0 + hi1 + (__al < lo0);
    al = __al;
}

#if WJR_HAS_FEATURE(FAST_INT128_ADDSUB)
#define WJR_HAS_BUILTIN___BUILTIN_ADD_128 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(__BUILTIN_ADD_128)

WJR_INTRINSIC_INLINE void __builtin_add_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                            uint64_t hi0, uint64_t lo1, uint64_t hi1) {
    const auto x0 = static_cast<__uint128_t>(hi0) << 64 | lo0;
    const auto x1 = static_cast<__uint128_t>(hi1) << 64 | lo1;
    x0 += x1;

    al = x0;
    ah = x0 >> 64;
}

#endif

// <ah, al> = <hi0, lo0> + <hi1, lo1>
WJR_INTRINSIC_CONSTEXPR_E void __add_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                         uint64_t hi0, uint64_t lo1, uint64_t hi1) {
#if WJR_HAS_BUILTIN(__BUILTIN_ADD_128) || WJR_HAS_BUILTIN(__ASM_ADD_128)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(lo0 == 0) && lo0 == 0) ||
        (WJR_BUILTIN_CONSTANT_P(lo1 == 0) && lo1 == 0)) {
        return __fallback_add_128(al, ah, lo0, hi0, lo1, hi1);
    }

    return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(__BUILTIN_ADD_128), __builtin_add_128,
                          __asm_add_128)(al, ah, lo0, hi0, lo1, hi1);
#else
    return __fallback_add_128(al, ah, lo0, hi0, lo1, hi1);
#endif
}

WJR_INTRINSIC_CONSTEXPR_E uint64_t __fallback_addc_128(uint64_t &al, uint64_t &ah,
                                                       uint64_t lo0, uint64_t hi0,
                                                       uint64_t lo1, uint64_t hi1,
                                                       uint64_t c_in) {
    al = addc(lo0, lo1, c_in, c_in);
    ah = addc(hi0, hi1, c_in, c_in);
    return c_in;
}

// return c_out
WJR_INTRINSIC_CONSTEXPR_E uint64_t __addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                              uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                              uint64_t c_in) {
#if WJR_HAS_BUILTIN(__ASM_ADDC_128)
    if (is_constant_evaluated()) {
        return __fallback_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
    }

    return __asm_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return __fallback_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#endif
}

WJR_INTRINSIC_CONSTEXPR_E uint8_t __addc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                                uint8_t c_in) {
#if WJR_HAS_BUILTIN(__ASM_ADDC_CC_128)
    if (is_constant_evaluated()) {
        return __fallback_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
    }

    return __asm_addc_cc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return __addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#endif
}

} // namespace wjr

#endif // WJR_MATH_ADD_HPP__