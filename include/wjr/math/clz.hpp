#ifndef WJR_MATH_CLZ_HPP__
#define WJR_MATH_CLZ_HPP__

#include <wjr/math/popcount.hpp>

namespace wjr {

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int fallback_clz_impl(T x) {
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
    return popcount<T>(~x);
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
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int fallback_clz(T x) {
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
WJR_CONST WJR_INTRINSIC_INLINE int builtin_clz_impl(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 32) {
        return builtin_clz_impl(static_cast<uint32_t>(x)) - (32 - nd);
    } else {
        if constexpr (nd <= std::numeric_limits<unsigned int>::digits) {
            constexpr auto delta = std::numeric_limits<unsigned int>::digits - nd;
            return __builtin_clz(static_cast<unsigned int>(x)) - delta;
        } else if constexpr (nd <= std::numeric_limits<unsigned long>::digits) {
            constexpr auto delta = std::numeric_limits<unsigned long>::digits - nd;
            return __builtin_clzl(static_cast<unsigned long>(x)) - delta;
        } else if constexpr (nd <= std::numeric_limits<unsigned long long>::digits) {
            constexpr auto delta = std::numeric_limits<unsigned long long>::digits - nd;
            return __builtin_clzll(static_cast<unsigned long long>(x)) - delta;
        } else {
            static_assert(nd <= 64, "not supported yet");
        }
    }
}

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_clz(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if (WJR_UNLIKELY(x == 0)) {
        return nd;
    }

    return builtin_clz_impl(x);
}

#endif

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int clz_impl(T x) {
#if WJR_HAS_BUILTIN(CLZ)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_clz(x);
    }

    return builtin_clz(x);
#else
    return fallback_clz(x);
#endif
}

/**
 * @brief Fast count leading zeros
 *
 * @tparam T Must be an unsigned integral type
 */
template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int clz(T x) {
    int ret = clz_impl(x);
    WJR_ASSUME(0 <= ret && ret <= std::numeric_limits<T>::digits);
    return ret;
}

} // namespace wjr

#endif // WJR_MATH_CLZ_HPP__