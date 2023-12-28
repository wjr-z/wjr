#ifndef WJR_MATH_NEG_HPP__
#define WJR_MATH_NEG_HPP__

#include <wjr/math/sub.hpp>

namespace wjr {

template <typename T>
WJR_INLINE_CONSTEXPR void fallback_simd_neg_n(T *dst, const T *src, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = ~src[i];
    }
}

template <typename T>
WJR_INLINE_CONSTEXPR void simd_neg_n(T *dst, const T *src, size_t n) {
    return fallback_simd_neg_n(dst, src, n);
}

// return true if src is all zero
template <typename T>
WJR_INLINE_CONSTEXPR bool neg_n(T *dst, const T *src, size_t n) {
    size_t idx = replace_val(dst, src, n, 0, 0);

    if (idx == n) {
        return true;
    }

    dst[idx] = -src[idx];

    simd_neg_n(dst + idx + 1, src + idx + 1, n - idx - 1);
    return false;
}

} // namespace wjr

#endif // WJR_MATH_NEG_HPP__