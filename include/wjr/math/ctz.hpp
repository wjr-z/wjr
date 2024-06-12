#ifndef WJR_MATH_CTZ_HPP__
#define WJR_MATH_CTZ_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/popcount.hpp>

namespace wjr {

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int fallback_ctz_impl(T x) noexcept {
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
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int fallback_ctz(T x) noexcept {
    return fallback_ctz_impl(x);
}

#if WJR_HAS_BUILTIN(__builtin_ctz)
#define WJR_HAS_BUILTIN_CTZ WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(CTZ)

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_ctz_impl(T x) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 32) {
        return builtin_ctz_impl(static_cast<uint32_t>(x));
    } else {
        if constexpr (nd <= std::numeric_limits<unsigned int>::digits) {
            return __builtin_ctz(static_cast<unsigned int>(x));
        } else if constexpr (nd <= std::numeric_limits<unsigned long>::digits) {
            return __builtin_ctzl(static_cast<unsigned long>(x));
        } else if constexpr (nd <= std::numeric_limits<unsigned long long>::digits) {
            return __builtin_ctzll(static_cast<unsigned long long>(x));
        } else {
            static_assert(nd <= 64, "not supported yet");
        }
    }
}

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_ctz(T x) noexcept {
    return builtin_ctz_impl(x);
}

#endif

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int ctz_impl(T x) noexcept {
#if WJR_HAS_BUILTIN(CTZ)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_ctz(x);
    }

    return builtin_ctz(x);
#else
    return fallback_ctz(x);
#endif
}

/**
 * @brief Fast count trailing zeros
 *
 * @details Very fast even on non-optimized platforms by using a De Bruijn sequence. \n
 * Try __builtin_clz if available, otherwise fallback to a portable implementation. \n
 * In fallback_clz, use popcount and lowbit if POPCOUNT and POPCNT are available, make
 * sure popcount is fast. \n
 * Then use De Bruijn sequence, just a bit slower than popcount + lowbit.
 *
 * @tparam T Must be an unsigned integral type
 */
template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int ctz(T x) noexcept {
    WJR_ASSERT_ASSUME_L2(x != 0);
    const int ret = ctz_impl(x);
    WJR_ASSUME(0 <= ret && ret < std::numeric_limits<T>::digits);
    return ret;
}

} // namespace wjr

#endif // WJR_MATH_CTZ_HPP__