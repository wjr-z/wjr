#ifndef WJR_MATH_SHIFT_HPP__
#define WJR_MATH_SHIFT_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/math/shift.hpp>
#endif

namespace wjr {

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T shld(T hi, T lo, unsigned int c) noexcept {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return hi << c | lo >> (digits - c);
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T shrd(T lo, T hi, unsigned int c) noexcept {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return lo >> c | hi << (digits - c);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_lshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                            T lo) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[n - 1] >> (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[n - i - 1] = shld(src[n - i - 1], src[n - i - 2], c);
    }
    dst[0] = shld(src[0], lo, c);
    return ret;
}

/**
 * @pre  \n
 * 1. n >= 1
 * 2. WJR_IS_SAME_OR_DECR_P(dst, n, src, n)
 */
template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 T lshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                                   type_identity_t<T> lo = 0) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));
    WJR_ASSERT_L2(c < std::numeric_limits<T>::digits);

    if (WJR_UNLIKELY(c == 0)) {
        if (WJR_LIKELY(dst != src)) {
            std::copy_backward(src, src + n, dst + n);
        }

        return 0;
    }

#if WJR_HAS_BUILTIN(LSHIFT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_lshift_n(dst, src, n, c, lo);
        }

        return builtin_lshift_n(dst, src, n, c, lo);
    } else {
        return fallback_lshift_n(dst, src, n, c, lo);
    }
#else
    return fallback_lshift_n(dst, src, n, c, lo);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_rshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                            T hi) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[0] << (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[i] = shrd(src[i], src[i + 1], c);
    }
    dst[n - 1] = shrd(src[n - 1], hi, c);
    return ret;
}

/**
 * @pre  \n
 * 1. n >= 1
 * 2. WJR_IS_SAME_OR_DECR_P(dst, n, src, n)
 */
template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 T rshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                     type_identity_t<T> hi = 0) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));
    WJR_ASSERT_L2(c < std::numeric_limits<T>::digits);

    if (WJR_UNLIKELY(c == 0)) {
        if (WJR_LIKELY(dst != src)) {
            std::copy(src, src + n, dst);
        }

        return 0;
    }

#if WJR_HAS_BUILTIN(RSHIFT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_rshift_n(dst, src, n, c, hi);
        }

        return builtin_rshift_n(dst, src, n, c, hi);
    } else {
        return fallback_rshift_n(dst, src, n, c, hi);
    }
#else
    return fallback_rshift_n(dst, src, n, c, hi);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_lshiftc_n(T *dst, const T *src, size_t n, unsigned int c,
                                             T lo) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[n - 1] >> (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[n - i - 1] = ~shld(src[n - i - 1], src[n - i - 2], c);
    }
    dst[0] = ~shld(src[0], lo, c);
    return ret;
}

/**
 * @pre  \n
 * 1. n >= 1
 * 2. WJR_IS_SAME_OR_DECR_P(dst, n, src, n)
 */
template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 T lshiftc_n(T *dst, const T *src, size_t n, unsigned int c,
                                                    type_identity_t<T> lo = 0) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(c >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));
    WJR_ASSERT_L2(c < std::numeric_limits<T>::digits);

#if WJR_HAS_BUILTIN(LSHIFTC_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_lshiftc_n(dst, src, n, c, lo);
        }

        return builtin_lshift_n(dst, src, n, c, lo);
    } else {
        return fallback_lshiftc_n(dst, src, n, c, lo);
    }
#else
    return fallback_lshiftc_n(dst, src, n, c, lo);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_rshiftc_n(T *dst, const T *src, size_t n, unsigned int c,
                                             T hi) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[0] << (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[i] = ~shrd(src[i], src[i + 1], c);
    }
    dst[n - 1] = ~shrd(src[n - 1], hi, c);
    return ret;
}

/**
 * @pre  \n
 * 1. n >= 1
 * 2. WJR_IS_SAME_OR_DECR_P(dst, n, src, n)
 */
template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 T rshiftc_n(T *dst, const T *src, size_t n, unsigned int c,
                                      type_identity_t<T> hi = 0) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(c >= 1);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));
    WJR_ASSERT_L2(c < std::numeric_limits<T>::digits);

#if WJR_HAS_BUILTIN(RSHIFTC_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_rshiftc_n(dst, src, n, c, hi);
        }

        return builtin_rshift_n(dst, src, n, c, hi);
    } else {
        return fallback_rshiftc_n(dst, src, n, c, hi);
    }
#else
    return fallback_rshiftc_n(dst, src, n, c, hi);
#endif
}

} // namespace wjr

#endif // WJR_MATH_SHIFT_HPP__