#ifndef WJR_MATH_XOR_HPP__
#define WJR_MATH_XOR_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_xor_n(T *dst, const T *src0, const T *src1,
                                            size_t n) noexcept {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = src0[i] ^ src1[i];
    }
}

template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void xor_n(T *dst, const T *src0, const T *src1, size_t n) noexcept {
    return fallback_xor_n(dst, src0, src1, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_or_n(T *dst, const T *src0, const T *src1,
                                           size_t n) noexcept {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = src0[i] | src1[i];
    }
}

template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void or_n(T *dst, const T *src0, const T *src1, size_t n) noexcept {
    return fallback_or_n(dst, src0, src1, n);
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_and_n(T *dst, const T *src0, const T *src1,
                                            size_t n) noexcept {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = src0[i] & src1[i];
    }
}

template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void and_n(T *dst, const T *src0, const T *src1, size_t n) noexcept {
    return fallback_and_n(dst, src0, src1, n);
}

} // namespace wjr

#endif // WJR_MATH_XOR_HPP__