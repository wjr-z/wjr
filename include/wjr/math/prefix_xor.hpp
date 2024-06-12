#ifndef WJR_MATH_PREFIX_XOR_HPP__
#define WJR_MATH_PREFIX_XOR_HPP__

#include <wjr/type_traits.hpp>

#if defined(WJR_X86)
#include <wjr/x86/math/prefix_xor.hpp>
#endif

namespace wjr {

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

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR_E T prefix_xor(T x) noexcept {
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