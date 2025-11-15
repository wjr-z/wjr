/**
 * @file replace.hpp
 * @brief Array find-and-replace operations
 * @author wjr
 *
 * Provides optimized operations that find and replace patterns in arrays.
 * Useful for implementing increment/decrement with carry propagation.
 */

#ifndef WJR_MATH_REPLACE_HPP__
#define WJR_MATH_REPLACE_HPP__

#include <wjr/math/find.hpp>
#include <wjr/math/set.hpp>

namespace wjr {

/**
 * @brief Find first non-matching element and replace preceding elements
 *
 * Finds the first position not equal to 'from' and replaces [0, ret) with 'to'.
 * Useful for carry/borrow propagation (e.g., increment replaces segment of -1 with 0).
 *
 * @tparam T Integral type
 * @param[out] dst Destination array
 * @param[in] src Source array
 * @param[in] n Array size
 * @param[in] from Value to search for
 * @param[in] to Replacement value
 * @return size_t Index of first non-matching element
 *
 * @todo Optimization: merge builtin_find_not_n and builtin_set_n
 */
template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t replace_find_not(T *dst, const T *src, size_t n,
                                                  type_identity_t<T> from,
                                                  type_identity_t<T> to) noexcept {
    const size_t ret = find_not_n(src, from, n);
    if (WJR_UNLIKELY(ret != 0) && WJR_LIKELY(dst != src || from != to)) {
        set_n(dst, to, ret);
    }

    return ret;
}

/**
 * @brief Reverse-order find and replace operation
 *
 * Like replace_find_not but searches from high to low. Finds the last position
 * not equal to 'from' and replaces [ret, n) with 'to'.
 *
 * @tparam T Integral type
 * @param[out] dst Destination array
 * @param[in] src Source array
 * @param[in] n Array size
 * @param[in] from Value to search for
 * @param[in] to Replacement value
 * @return size_t Index of last non-matching element
 */
template <typename T>
WJR_INTRINSIC_CONSTEXPR size_t reverse_replace_find_not(T *dst, const T *src, size_t n,
                                                          type_identity_t<T> from,
                                                          type_identity_t<T> to) noexcept {
    const size_t ret = reverse_find_not_n(src, from, n);
    if (WJR_UNLIKELY(ret != n) && WJR_LIKELY(dst != src || from != to)) {
        set_n(dst + ret, to, n - ret);
    }

    return ret;
}

} // namespace wjr

#endif // WJR_MATH_REPLACE_HPP__