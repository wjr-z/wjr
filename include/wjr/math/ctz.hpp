/**
 * @file ctz.hpp
 * @brief Count trailing zeros operations
 * @author wjr
 *
 * Provides functions to count the number of trailing zero bits in integers.
 * Uses compiler intrinsics when available for optimal performance.
 */

#ifndef WJR_MATH_CTZ_HPP__
#define WJR_MATH_CTZ_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/popcount.hpp>

#if WJR_HAS_BUILTIN(__builtin_ctz)
    #define WJR_HAS_BUILTIN_CTZ WJR_HAS_DEF
#elif defined(_MSC_VER) && defined(WJR_ARCH_X86)
    #define WJR_HAS_BUILTIN_CTZ WJR_HAS_SIMD_DEF
#endif

#if WJR_HAS_BUILTIN(CTZ) == WJR_SIMD_DEF_VAR
    #include <wjr/arch/x86/simd/intrin.hpp>
#endif

#if WJR_HAS_BUILTIN(CTZ) || !WJR_HAS_BUILTIN(POPCOUNT)
    #define WJR_CTZ_NOT_FULL
#endif

namespace wjr {

namespace constant {
template <typename T>
WJR_CONST constexpr int ctz(T x) noexcept {
    return constant::popcount<T>(lowbit(x) - 1);
}
} // namespace constant

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int fallback_ctz(T x) noexcept {
#if WJR_HAS_BUILTIN(POPCOUNT)
    return popcount<T>(lowbit(x) - 1);
#else
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 32) {
        return fallback_ctz(static_cast<uint32_t>(x));
    } else {
        x = lowbit(x);

        if constexpr (nd <= 32) {
            return math_detail::de_bruijn32.get(x);
        } else if constexpr (nd <= 64) {
            return math_detail::de_bruijn64.get(x);
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }
#endif //
}

#if WJR_HAS_BUILTIN(CTZ)

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_ctz(T x) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 32) {
        return builtin_ctz(static_cast<uint32_t>(x));
    } else {
    #if WJR_HAS_BUILTIN(CTZ) == 1
        if constexpr (nd <= std::numeric_limits<unsigned int>::digits) {
            return __builtin_ctz(static_cast<unsigned int>(x));
        } else if constexpr (nd <= std::numeric_limits<unsigned long>::digits) {
            return __builtin_ctzl(static_cast<unsigned long>(x));
        } else if constexpr (nd <= std::numeric_limits<unsigned long long>::digits) {
            return __builtin_ctzll(static_cast<unsigned long long>(x));
        } else {
            static_assert(nd <= 64, "not supported yet");
        }
    #else
        if constexpr (nd == 32) {
            unsigned long result;
            (void)_BitScanForward(&result, x);
            return result;
        } else {
            unsigned long result;
            (void)_BitScanForward64(&result, x);
            return result;
        }
    #endif
    }
}

#endif

/**
 * @brief Fast count trailing zeros
 *
 * @details Very fast even on non-optimized platforms by using a De Bruijn
 * sequence. \n Try __builtin_clz if available, otherwise fallback to a portable
 * implementation. \n In fallback_clz, use popcount and lowbit if POPCOUNT and
 * POPCNT are available, make sure popcount is fast. \n Then use De Bruijn
 * sequence, just a bit slower than popcount + lowbit.
 *
 * @tparam T Must be an unsigned integral type
 */
template <nonbool_unsigned_integral T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int ctz(T x) noexcept {
    int ans;
#if WJR_HAS_BUILTIN(CTZ)
    if (is_constant_evaluated()) {
        ans = fallback_ctz(x);
    } else {
        ans = builtin_ctz(x);
    }
#else
    ans = fallback_ctz(x);
#endif

#if defined(WJR_CTZ_NOT_FULL)
    WJR_ASSUME(ans >= 0 && ans < std::numeric_limits<T>::digits);
#else
    WJR_ASSUME(ans >= 0 && ans <= std::numeric_limits<T>::digits);
#endif

    return ans;
}

} // namespace wjr

#endif // WJR_MATH_CTZ_HPP__