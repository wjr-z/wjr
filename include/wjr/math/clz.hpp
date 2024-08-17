#ifndef WJR_MATH_CLZ_HPP__
#define WJR_MATH_CLZ_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/popcount.hpp>

#if WJR_HAS_BUILTIN(__builtin_clz)
    #define WJR_HAS_BUILTIN_CLZ WJR_HAS_DEF
#elif defined(WJR_MSVC) && defined(WJR_X86)
    #define WJR_HAS_BUILTIN_CLZ WJR_HAS_DEF_VAR(2)
#endif

#if WJR_HAS_BUILTIN(CLZ) == 2
    #include <wjr/arch/x86/simd/intrin.hpp>
#endif

namespace wjr {

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR int constexpr_clz(T x) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

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

    return fallback_popcount(~x);
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int fallback_clz(T x) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

#if !WJR_HAS_BUILTIN(POPCOUNT)
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
#if !WJR_HAS_BUILTIN(POPCOUNT)
    }
#endif

#if WJR_HAS_BUILTIN(POPCOUNT)
    return popcount<T>(~x);
#else
    if constexpr (nd < 32) {
        return fallback_clz(static_cast<uint32_t>(x)) - (32 - nd);
    } else {
        ++x;

        if constexpr (nd <= 32) {
            return math_detail::de_bruijn32.getr(x);
        } else if constexpr (nd <= 64) {
            return math_detail::de_bruijn64.getr(x);
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }
#endif
}

#if WJR_HAS_BUILTIN(CLZ)

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_clz(T x) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    if constexpr (nd < 32) {
        return builtin_clz(static_cast<uint32_t>(x)) - (32 - nd);
    } else {
    #if WJR_HAS_BUILTIN(CLZ) == 1
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
    #else
        if constexpr (nd == 32) {
            unsigned long result;
            (void)_BitScanReverse(&result, x);
            return 31 - result;
        } else {
            unsigned long result;
            (void)_BitScanReverse64(&result, x);
            return 63 - result;
        }
    #endif
    }
}

#endif

/**
 * @brief Fast count leading zeros
 *
 * @tparam T Must be an unsigned integral type
 */
template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int clz(T x) noexcept {
#if WJR_HAS_BUILTIN(CLZ)
    if (is_constant_evaluated()) {
        return fallback_clz(x);
    }

    return builtin_clz(x);
#else
    return fallback_clz(x);
#endif
}

} // namespace wjr

#endif // WJR_MATH_CLZ_HPP__