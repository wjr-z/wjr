/**
 * @file align.hpp
 * @brief Memory alignment utilities
 * @author wjr
 *
 * Provides functions for aligning memory addresses and checking alignment.
 * These utilities are essential for optimizing memory access patterns and
 * ensuring proper alignment for SIMD operations.
 */

#ifndef WJR_MEMORY_ALIGN_HPP__
#define WJR_MEMORY_ALIGN_HPP__

#include <cstdlib>
#include <new>

#include <wjr/assert.hpp>
#include <wjr/math/bit.hpp>

namespace wjr {

/**
 * @brief Align value down to the nearest multiple of alignment
 *
 * Rounds down the input value to the largest multiple of alignment that
 * is less than or equal to n.
 *
 * @tparam T Unsigned integral type
 * @param[in] n Value to align
 * @param[in] alignment Alignment boundary (must be power of 2)
 * @return T Aligned value
 */
template <nonbool_unsigned_integral T>
WJR_CONST constexpr T align_down(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(has_single_bit(alignment));
    return n & (-alignment);
}

/**
 * @brief Align value up to the nearest multiple of alignment
 *
 * Rounds up the input value to the smallest multiple of alignment that
 * is greater than or equal to n.
 *
 * @tparam T Unsigned integral type
 * @param[in] n Value to align
 * @param[in] alignment Alignment boundary (must be power of 2)
 * @return T Aligned value
 */
template <nonbool_unsigned_integral T>
WJR_CONST constexpr T align_up(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(has_single_bit(alignment));
    return (n + alignment - 1) & (-alignment);
}

template <nonbool_unsigned_integral T>
WJR_CONST constexpr T align_down_offset(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(has_single_bit(alignment));
    return n & (alignment - 1);
}

template <nonbool_unsigned_integral T>
WJR_CONST constexpr T align_up_offset(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(has_single_bit(alignment));
    return (-n) & (alignment - 1);
}

/**
 * @brief Check if value is aligned to the specified boundary
 *
 * Tests whether the input value is an exact multiple of the alignment.
 *
 * @tparam T Unsigned integral type
 * @param[in] n Value to check
 * @param[in] alignment Alignment boundary (must be power of 2)
 * @return bool True if n is aligned, false otherwise
 */
template <nonbool_unsigned_integral T>
WJR_CONST constexpr bool is_aligned(T n, type_identity_t<T> alignment) noexcept {
    return align_down_offset(n, alignment) == 0;
}

using std::assume_aligned;

namespace mem {
#if defined(__STDCPP_DEFAULT_NEW_ALIGNMENT__)
inline constexpr size_t stdcpp_default_new_alignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
#else
inline constexpr size_t stdcpp_default_new_alignment = 0;
#endif

inline constexpr size_t default_new_alignment = std::max<size_t>(
    sizeof(void *), std::max<size_t>(alignof(max_align_t), stdcpp_default_new_alignment));

template <typename T>
inline constexpr size_t _new_alignof = std::max<size_t>(alignof(T), default_new_alignment);

} // namespace mem

} // namespace wjr

#endif // WJR_MEMORY_ALIGN_HPP__