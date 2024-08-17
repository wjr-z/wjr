#ifndef WJR_MATH_NOT_HPP__
#define WJR_MATH_NOT_HPP__

#include <wjr/type_traits.hpp>

#if defined(WJR_X86)
#include <wjr/arch/x86/math/not.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR void fallback_not_n(T *dst, const T *src, size_t n) noexcept {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = ~src[i];
    }
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 void not_n(T *dst, const T *src, size_t n) noexcept {
#if WJR_HAS_BUILTIN(COMPLEMENT_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_not_n(dst, src, n);
        }

        return builtin_not_n(dst, src, n);
    } else {
        return fallback_not_n(dst, src, n);
    }
#else
    return fallback_not_n(dst, src, n);
#endif
}

} // namespace wjr

#endif // WJR_MATH_NOT_HPP__