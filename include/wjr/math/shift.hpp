#ifndef WJR_MATH_SHIFT_HPP__
#define WJR_MATH_SHIFT_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

#if defined(WJR_X86)
#include <wjr/x86/math/shift.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_shld(T hi, T lo, unsigned int c) {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return hi << c | lo >> (digits - c);
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T shld(T hi, T lo, unsigned int c) {
    return fallback_shld(hi, lo, c);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_shrd(T lo, T hi, unsigned int c) {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return lo >> c | hi << (digits - c);
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T shrd(T lo, T hi, unsigned int c) {
    return fallback_shrd(lo, hi, c);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_lshift_n(T *dst, const T *src, size_t n,
                                            unsigned int c, T lo) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[n - 1] >> (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[n - i - 1] = fallback_shld(src[n - i - 1], src[n - i - 2], c);
    }
    dst[0] = fallback_shld(src[0], lo, c);
    return ret;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_DECR_P(dst, n, src, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T lshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                     T lo = 0) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));

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
WJR_INTRINSIC_CONSTEXPR T fallback_rshift_n(T *dst, const T *src, size_t n,
                                            unsigned int c, T hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[0] << (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[i] = fallback_shrd(src[i], src[i + 1], c);
    }
    dst[n - 1] = fallback_shrd(src[n - 1], hi, c);
    return ret;
}

/*
require :
1. n >= 1
2. WJR_IS_SAME_OR_INCR_P(dst, n, src, n)
*/
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T rshift_n(T *dst, const T *src, size_t n, unsigned int c,
                                     T hi = 0) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

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

} // namespace wjr

#endif // WJR_MATH_SHIFT_HPP__