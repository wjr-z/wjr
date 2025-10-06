/**
 * @file math/bitwise.hpp
 * @brief Array-wise bitwise operations
 * @author wjr
 *
 * Provides vectorized bitwise operations (XOR, OR, AND, NOT) on arrays.
 */

#ifndef WJR_MATH_BITWISE_HPP__
#define WJR_MATH_BITWISE_HPP__

#include <wjr/assert.hpp>
#include <wjr/type_traits.hpp>

namespace wjr::math {

/**
 * @brief Array-wise XOR operation
 *
 * Computes dst[i] = src0[i] ^ src1[i] for i in [0, n).
 *
 * @tparam T Unsigned integral type
 * @param[out] dst Destination array
 * @param[in] src0 First source array
 * @param[in] src1 Second source array
 * @param[in] n Number of elements
 */
template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void xor_n(T *dst, const T *src0, const T *src1, size_t n) noexcept {
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    for (size_t i = 0; i < n; ++i) {
        dst[i] = src0[i] ^ src1[i];
    }
}

/**
 * @brief Array-wise OR operation
 *
 * Computes dst[i] = src0[i] | src1[i] for i in [0, n).
 *
 * @tparam T Unsigned integral type
 * @param[out] dst Destination array
 * @param[in] src0 First source array
 * @param[in] src1 Second source array
 * @param[in] n Number of elements
 */
template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void or_n(T *dst, const T *src0, const T *src1, size_t n) noexcept {
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    for (size_t i = 0; i < n; ++i) {
        dst[i] = src0[i] | src1[i];
    }
}

/**
 * @brief Array-wise AND operation
 *
 * Computes dst[i] = src0[i] & src1[i] for i in [0, n).
 *
 * @tparam T Unsigned integral type
 * @param[out] dst Destination array
 * @param[in] src0 First source array
 * @param[in] src1 Second source array
 * @param[in] n Number of elements
 */
template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void and_n(T *dst, const T *src0, const T *src1, size_t n) noexcept {
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    for (size_t i = 0; i < n; ++i) {
        dst[i] = src0[i] & src1[i];
    }
}

/**
 * @brief Array-wise NOT operation
 *
 * Computes dst[i] = ~src[i] for i in [0, n).
 *
 * @tparam T Unsigned integral type
 * @param[out] dst Destination array
 * @param[in] src Source array
 * @param[in] n Number of elements
 */
template <typename T, std::enable_if_t<(is_nonbool_unsigned_integral_v<T>), int> = 0>
WJR_INTRINSIC_CONSTEXPR void not_n(T *dst, const T *src, size_t n) noexcept {
    WJR_ASSERT_L2(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    for (size_t i = 0; i < n; ++i) {
        dst[i] = ~src[i];
    }
}

} // namespace wjr::math

#endif // WJR_MATH_BITWISE_HPP__