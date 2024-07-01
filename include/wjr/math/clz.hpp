#ifndef WJR_MATH_CLZ_HPP__
#define WJR_MATH_CLZ_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/popcount.hpp>

namespace wjr {

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int fallback_clz(T x) noexcept {
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
        return fallback_clz(static_cast<uint32_t>(x)) - (32 - nd);
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

#if WJR_HAS_BUILTIN(__builtin_clz)
#define WJR_HAS_BUILTIN_CLZ WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(CLZ)

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_clz(T x) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 32) {
        return builtin_clz(static_cast<uint32_t>(x)) - (32 - nd);
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

#endif

#if !WJR_HAS_BUILTIN(CLZ)

#if defined(WJR_MSVC)
#define WJR_HAS_BUILTIN_MSVC_CLZ WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(MSVC_CLZ)

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_msvc_clz(T x) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 32) {
        return builtin_msvc_clz(static_cast<uint32_t>(x)) - (32 - nd);
    } else {
        if constexpr (nd == 32) {
            unsigned long result;
            if (_BitScanReverse(&result, x)) {
                return 31 - result;
            }
            return 0;
        } else {
            unsigned long result;
            if (_BitScanReverse64(&result, x)) {
                return 63 - result;
            }
            return 0;
        }
    }
}

#endif

#endif

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int clz_impl(T x) noexcept {
#if WJR_HAS_BUILTIN(CLZ) || WJR_HAS_BUILTIN(MSVC_CLZ)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_clz(x);
    }

#if WJR_HAS_BUILTIN(CLZ)
    return builtin_clz(x);
#else
    return builtin_msvc_clz(x);
#endif
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
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int clz(T x) noexcept {
    WJR_ASSERT_ASSUME_L2(x != 0);
    const int ret = clz_impl(x);
    WJR_ASSUME(0 <= ret && ret < std::numeric_limits<T>::digits);
    return ret;
}

} // namespace wjr

#endif // WJR_MATH_CLZ_HPP__