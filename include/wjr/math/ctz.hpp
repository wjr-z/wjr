#ifndef WJR_MATH_CTZ_HPP__
#define WJR_MATH_CTZ_HPP__

#include <wjr/math/popcount.hpp>

namespace wjr {

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
int fallback_ctz_impl(T x) {

#if WJR_HAS_BUILTIN(POPCOUNT) && WJR_HAS_SIMD(POPCNT)
    return popcount<T>(lowbit(x) - 1);
#else
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 32) {
        return fallback_ctz_impl(static_cast<uint32_t>(x));
    } else {
        x = lowbit(x);

        if constexpr (nd <= 32) {
            return math_details::de_bruijn32.get(x);
        } else if constexpr (nd <= 64) {
            return math_details::de_bruijn64.get(x);
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }
#endif //
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
int fallback_ctz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

    return fallback_ctz_impl(x);
}

#if WJR_HAS_BUILTIN(__builtin_ctz)
#define WJR_HAS_BUILTIN_CTZ WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(CTZ)

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_INLINE)
int builtin_ctz_impl(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_CTZ(args)                                                   \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_CTZ_I_CALLER)
#define WJR_REGISTER_BUILTIN_CTZ_I(suffix, type)                                         \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        return __builtin_ctz##suffix(static_cast<type>(x));                              \
    } else
#define WJR_REGISTER_BUILTIN_CTZ_I_CALLER(args) WJR_REGISTER_BUILTIN_CTZ_I args

    if constexpr (nd < 32) {
        return builtin_ctz_impl(static_cast<uint32_t>(x));
    } else {
        WJR_REGISTER_BUILTIN_CTZ(
            ((, unsigned int), (l, unsigned long), (ll, unsigned long long))) {
            static_assert(nd <= 64, "not supported yet");
        }
    }

#undef WJR_REGISTER_BUILTIN_CTZ_I_CALLER
#undef WJR_REGISTER_BUILTIN_CTZ_I
#undef WJR_REGISTER_BUILTIN_CTZ
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_INLINE)
int builtin_ctz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

    return builtin_ctz_impl(x);
}

#endif

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
int ctz(T x) {
#if WJR_HAS_BUILTIN(CTZ)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_ctz(x);
    }

    return builtin_ctz(x);
#else
    return fallback_ctz(x);
#endif
}

} // namespace wjr

#endif // WJR_MATH_CTZ_HPP__