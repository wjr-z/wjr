#ifndef WJR_MATH_CLZ_HPP__
#define WJR_MATH_CLZ_HPP__

#include <wjr/math/ctz.hpp>

namespace wjr {

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
int fallback_clz_impl(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#if !(WJR_HAS_BUILTIN(POPCOUNT) && WJR_HAS_SIMD(POPCNT))
    if constexpr (nd >= 32) {
#endif
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);

        if constexpr (nd >= 16) {
            x |= (x >> 8);
        }

        if constexpr (nd >= 32) {
            x |= (x >> 16);
        }

        if constexpr (nd >= 64) {
            x |= (x >> 32);
        }
#if !(WJR_HAS_BUILTIN(POPCOUNT) && WJR_HAS_SIMD(POPCNT))
    }
#endif

#if WJR_HAS_BUILTIN(POPCOUNT) && WJR_HAS_SIMD(POPCNT)
    return popcount(~x);
#else
    if constexpr (nd < 32) {
        return fallback_clz_impl(static_cast<uint32_t>(x)) - (32 - nd);
    } else {
        ++x;

        if constexpr (nd <= 32) {
            return math_details::de_bruijn32.getr(x);
        } else if constexpr (nd <= 64) {
            return math_details::de_bruijn64.getr(x);
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }
#endif
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
int fallback_clz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

    return fallback_clz_impl(x);
}

#if WJR_HAS_BUILTIN(__builtin_clz)
#define WJR_HAS_BUILTIN_CLZ WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(CLZ)

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_INLINE)
int builtin_clz_impl(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_CLZ(args)                                                   \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_CLZ_I_CALLER)
#define WJR_REGISTER_BUILTIN_CLZ_I(suffix, type)                                         \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        constexpr auto delta = std::numeric_limits<type>::digits - nd;                   \
        return __builtin_clz##suffix(static_cast<type>(x)) - delta;                      \
    } else
#define WJR_REGISTER_BUILTIN_CLZ_I_CALLER(args) WJR_REGISTER_BUILTIN_CLZ_I args

    if constexpr (nd < 32) {
        return builtin_clz_impl(static_cast<uint32_t>(x)) - (32 - nd);
    } else {
        WJR_REGISTER_BUILTIN_CLZ(
            ((, unsigned int), (l, unsigned long), (ll, unsigned long long))) {
            static_assert(nd <= 64, "not supported yet");
        }
    }

#undef WJR_REGISTER_BUILTIN_CLZ_I_CALLER
#undef WJR_REGISTER_BUILTIN_CLZ_I
#undef WJR_REGISTER_BUILTIN_CLZ
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_INLINE)
int builtin_clz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

    return builtin_clz_impl(x);
}

#endif

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
int clz(T x) {
#if WJR_HAS_BUILTIN(CLZ)
    if (is_constant_evaluated() || is_constant_p(x)) {
        return fallback_clz(x);
    }

    return builtin_clz(x);
#else
    return fallback_clz(x);
#endif
}

} // namespace wjr

#endif // WJR_MATH_CLZ_HPP__