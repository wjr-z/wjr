#ifndef WJR_MATH_CTZ_HPP__
#define WJR_MATH_CTZ_HPP__

#include <wjr/math/popcount.hpp>

namespace wjr {

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
int fallback_ctz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

#if WJR_HAS_BUILTIN(POPCOUNT) && defined(__POPCNT__)
    return popcount((x & -x) - 1);
#else
    if constexpr (nd < 32) {
        return fallback_ctz(static_cast<uint32_t>(x));
    } else {
        if (!is_constant_p(is_power_of_two(x))) {
            x &= -x;
        }

        if constexpr (nd <= 32) {
            return math_details::de_bruijn32_lookup[(x * math_details::de_bruijn32) >>
                                                    27];
        } else if constexpr (nd <= 64) {
            return math_details::de_bruijn64_lookup[(x * math_details::de_bruijn64) >>
                                                    58];
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }
#endif //
}

#if WJR_HAS_BUILTIN(__builtin_ctz)
#define WJR_HAS_BUILTIN_CTZ WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(CTZ)

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_INLINE)
int builtin_ctz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

#define WJR_REGISTER_BUILTIN_CTZ(args)                                                   \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_CTZ_I_CALLER)
#define WJR_REGISTER_BUILTIN_CTZ_I(suffix, type)                                         \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        constexpr auto delta = std::numeric_limits<type>::digits;                        \
        return __builtin_ctz##suffix(static_cast<type>(x)) - delta;                      \
    } else
#define WJR_REGISTER_BUILTIN_CTZ_I_CALLER(args) WJR_REGISTER_BUILTIN_CTZ_I args

    if constexpr (nd < 32) {
        return builtin_ctz(static_cast<uint32_t>(x));
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

#endif

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_INLINE)
int ctz(T x) {
#if WJR_HAS_BUILTIN(CTZ)
    if (is_constant_evaluated() || is_constant_p(x)) {
        return fallback_ctz(x);
    }

    return builtin_ctz(x);
#else
    return fallback_ctz(x);
#endif
}

} // namespace wjr

#endif // WJR_MATH_CTZ_HPP__