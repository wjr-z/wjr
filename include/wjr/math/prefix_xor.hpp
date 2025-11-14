/**
 * @file math/prefix_xor.hpp
 * @brief Prefix XOR operation
 * @author wjr
 *
 * Computes the prefix XOR (parallel prefix) of bit patterns.
 * Also known as the "parity prefix" operation.
 */

#ifndef WJR_MATH_PREFIX_XOR_HPP__
#define WJR_MATH_PREFIX_XOR_HPP__

#include <wjr/type_traits.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/math/prefix_xor.hpp>
#endif

namespace wjr {

/// @private
template <typename T>
constexpr T fallback_prefix_xor(T x) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    static_assert(nd <= 64, "Type T has more than 64 bits");

    x ^= x << 1;
    x ^= x << 2;
    x ^= x << 4;
    if constexpr (nd > 8) {
        x ^= x << 8;
    }
    if constexpr (nd > 16) {
        x ^= x << 16;
    }
    if constexpr (nd > 32) {
        x ^= x << 32;
    }

    return x;
}

/**
 * @brief Compute prefix XOR
 *
 * Computes the prefix XOR (parallel prefix) where each bit in the result
 * is the XOR of all bits at and to the left of that position.
 * Example: 0b00001111 -> 0b00011110
 *
 * @tparam T Unsigned integral type
 * @param[in] x Input value
 * @return Prefix XOR result
 */
template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 T prefix_xor(T x) noexcept {
#if WJR_HAS_BUILTIN(PREFIX_XOR)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_prefix_xor(x);
    }

    return builtin_prefix_xor(x);
#else
    return fallback_prefix_xor(x);
#endif
}

} // namespace wjr

#endif // WJR_MATH_PREFIX_XOR_HPP__