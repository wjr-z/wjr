#ifndef WJR_MATH_NOT_HPP
#define WJR_MATH_NOT_HPP

#include <wjr/type_traits.hpp>

#if defined(WJR_X86)
#include <wjr/x86/math/complement.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_complement_n(T *dst, const T *src, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = ~src[i];
    }
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR_E void complement_n(T *dst, const T *src, size_t n) {
#if WJR_HAS_BUILTIN(COMPLEMENT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_complement_n(dst, src, n);
        }

        return builtin_complement_n(dst, src, n);
    } else {
        return fallback_complement_n(dst, src, n);
    }
#else
    return fallback_complement_n(dst, src, n);
#endif
}

} // namespace wjr

#endif // WJR_MATH_NOT_HPP