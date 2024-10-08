#ifndef WJR_MATH_SET_HPP__
#define WJR_MATH_SET_HPP__

#include <cstring>

#include <wjr/math/broadcast.hpp>
#include <wjr/type_traits.hpp>

#if defined(WJR_X86)
    #include <wjr/arch/x86/math/set.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_set_n(T *dst, T val, size_t n) noexcept {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = val;
    }
}

/// @todo Tests for T = uint16_t/uint32_t when using builtin_set_n.
template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 void set_n(T *dst, type_identity_t<T> val, size_t n) noexcept {
#if WJR_HAS_BUILTIN(SET_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_set_n(dst, val, n);
        }

        if (WJR_BUILTIN_CONSTANT_P(val) &&
            broadcast<uint8_t, T>(static_cast<uint8_t>(val)) == val) {
            if (WJR_UNLIKELY(n >= 2048 / sizeof(T))) {
                std::memset(dst, static_cast<uint8_t>(val), n * sizeof(T));
                return;
            }
        }

        return builtin_set_n(dst, val, n);
    } else {
        return fallback_set_n(dst, val, n);
    }
#else
    return fallback_set_n(dst, val, n);
#endif
}

} // namespace wjr

#endif // WJR_MATH_SET_HPP__