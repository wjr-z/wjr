#ifndef WJR_MATH_SET_HPP__
#define WJR_MATH_SET_HPP__

#include <cstring>

#include <wjr/math/broadcast.hpp>
#include <wjr/type_traits.hpp>

#if defined(WJR_X86)
#include <wjr/x86/set.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_set_n(T *dst, T val, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = val;
    }
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR void set_n(T *dst, type_identity_t<T> val, size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(n >= 256) && n >= 256) {
        uint8_t u8 = val;
        if (WJR_UNLIKELY((broadcast<uint8_t, T>(u8) == val))) {
            ::memset(dst, u8, sizeof(T) * n);
            return;
        }
    }
#if WJR_HAS_BUILTIN(SET_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_set_n(dst, val, n);
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