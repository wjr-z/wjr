#ifndef WJR_MATH_XOR_HPP__
#define WJR_MATH_XOR_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::math {

template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void xor_n(T *dst, const T *src0, const T *src1, size_t n) noexcept {
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    for (size_t i = 0; i < n; ++i) {
        dst[i] = src0[i] ^ src1[i];
    }
}

template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void or_n(T *dst, const T *src0, const T *src1, size_t n) noexcept {
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    for (size_t i = 0; i < n; ++i) {
        dst[i] = src0[i] | src1[i];
    }
}

template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void and_n(T *dst, const T *src0, const T *src1, size_t n) noexcept {
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    for (size_t i = 0; i < n; ++i) {
        dst[i] = src0[i] & src1[i];
    }
}

template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void not_n(T *dst, const T *src, size_t n) noexcept {
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    for (size_t i = 0; i < n; ++i) {
        dst[i] = ~src[i];
    }
}

} // namespace wjr::math

#endif // WJR_MATH_XOR_HPP__