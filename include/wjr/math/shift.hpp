#ifndef WJR_MATH_SHIFT_HPP__
#define WJR_MATH_SHIFT_HPP__

#include <wjr/simd/simd.hpp>

#if defined(WJR_X86)
#include <wjr/x86/shift.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_shld(T hi, T lo, unsigned int c) {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return hi << c | lo >> (digits - c);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T shld(T hi, T lo, unsigned int c) {
#if WJR_HAS_BUILTIN(ASM_SHLD)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(c == 0) && c == 0)) {
        return fallback_shld(hi, lo, c);
    }

    return asm_shld(hi, lo, c);
#else
    return fallback_shld(hi, lo, c);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_shrd(T lo, T hi, unsigned int c) {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return lo >> c | hi << (digits - c);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T shrd(T lo, T hi, unsigned int c) {
#if WJR_HAS_BUILTIN(ASM_SHRD)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(c == 0) && c == 0)) {
        return fallback_shrd(lo, hi, c);
    }

    return asm_shrd(lo, hi, c);
#else
    return fallback_shrd(lo, hi, c);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_lshift_n(T *dst, const T *src, size_t n,
                                            unsigned int c) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[n - 1] >> (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[n - i - 1] = fallback_shld(src[n - i - 1], src[n - i - 2], c);
    }
    dst[0] = src[0] << c;
    return ret;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T lshift_n(T *dst, const T *src, size_t n, unsigned int c) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_BUILTIN_CONSTANT_P(c % nd == 0) && c % nd == 0) {
        if (WJR_LIKELY(dst != src)) {
            std::copy_backward(src, src + n, dst);
        }

        return 0;
    }

#if WJR_HAS_BUILTIN(LSHIFT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_lshift_n(dst, src, n, c);
        }

        return builtin_lshift_n(dst, src, n, c);
    } else {
        return fallback_lshift_n(dst, src, n, c);
    }
#else
    return fallback_lshift_n(dst, src, n, c);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_rshift_n(T *dst, const T *src, size_t n,
                                            unsigned int c) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T ret = src[0] << (nd - c);
    for (size_t i = 0; i < n - 1; ++i) {
        dst[i] = fallback_shrd(src[i], src[i + 1], c);
    }
    dst[n - 1] = src[n - 1] >> c;
    return ret;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T rshift_n(T *dst, const T *src, size_t n, unsigned int c) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_BUILTIN_CONSTANT_P(c % nd == 0) && c % nd == 0) {
        if (WJR_LIKELY(dst != src)) {
            std::copy(src, src + n, dst);
        }

        return 0;
    }

#if WJR_HAS_BUILTIN(RSHIFT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_rshift_n(dst, src, n, c);
        }

        return builtin_rshift_n(dst, src, n, c);
    } else {
        return fallback_rshift_n(dst, src, n, c);
    }
#else
    return fallback_rshift_n(dst, src, n, c);
#endif
}

} // namespace wjr

#endif // WJR_MATH_SHIFT_HPP__