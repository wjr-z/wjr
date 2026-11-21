/**
 * @file set.hpp
 * @author wjr
 * @brief This is a small size optimized memset, but it also support uint16_t/uint32_t/uint64_t.
 * @details Only uint16_t/uint32_t/uint64_t on x86-64 platform is fully optimized currently.
 * @version 0.1
 * @date 2025-01-19
 *
 * @todo 1. Using builtin_memset on small size, and then fallback to my own large_builtin_memset or
 * std::memset.
 * 2. Support stream load/store to optimize.
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef WJR_MATH_SET_HPP__
#define WJR_MATH_SET_HPP__

#include <cstring>
#include <wjr/concept/integral.hpp>

#include <wjr/assert.hpp>
#include <wjr/math/broadcast.hpp>
#include <wjr/type_traits.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/math/set.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_set_n(T *dst, T val, size_t n) noexcept {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = val;
    }
}

/**
 * @brief Fill array with value (optimized memset)
 *
 * Fills n elements of dst with val. Supports non-byte values like 0x01020304
 * that can't be optimized by std::memset. Optimized for uint16_t/uint32_t/uint64_t.
 *
 * @tparam T Unsigned integral type
 * @param[out] dst Destination array
 * @param[in] val Value to fill
 * @param[in] n Number of elements to fill
 *
 */
template <nonbool_unsigned_integral T>
WJR_INTRINSIC_CONSTEXPR20 void set_n(T *dst, type_identity_t<T> val, size_t n) noexcept {
#if WJR_HAS_BUILTIN(SET_N)
    if constexpr (set_detail::simd_element<T>) {
        if (!is_constant_evaluated()) {
            return builtin_set_n(dst, val, n);
        }
    }
#endif
    return fallback_set_n(dst, val, n);
}

} // namespace wjr

#endif // WJR_MATH_SET_HPP__