#ifndef WJR_MATH_POPCOUNT_HPP
#define WJR_MATH_POPCOUNT_HPP

#include <wjr/math/details.hpp>

namespace wjr {

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR int fallback_popcount(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    if constexpr (nd < 32) {
        return fallback_popcount(static_cast<uint32_t>(x));
    } else {
        if constexpr (nd == 32) {
            x -= (x >> 1) & 0x5555'5555;
            x = (x & 0x3333'3333) + ((x >> 2) & 0x3333'3333);
            x = (x + (x >> 4)) & 0x0f0f'0f0f;
            return (x * 0x0101'0101) >> 24;
        } else {
            x -= (x >> 1) & 0x5555'5555'5555'5555;
            x = (x & 0x3333'3333'3333'3333) + ((x >> 2) & 0x3333'3333'3333'3333);
            x = (x + (x >> 4)) & 0x0f0f'0f0f'0f0f'0f0f;
            return (x * 0x0101'0101'0101'0101) >> 56;
        }
    }
}

#if WJR_HAS_BUILTIN(__builtin_popcount)
#define WJR_HAS_BUILTIN_POPCOUNT WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(POPCOUNT)

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_popcount(T x) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    if constexpr (nd < 32) {
        return builtin_popcount(static_cast<uint32_t>(x));
    } else {
        if constexpr (nd <= std::numeric_limits<unsigned int>::digits) {
            return __builtin_popcount(x);
        } else if constexpr (nd <= std::numeric_limits<unsigned long>::digits) {
            return __builtin_popcountl(x);
        }
        if constexpr (nd <= std::numeric_limits<unsigned long long>::digits) {
            return __builtin_popcountll(x);
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }
}

#endif // WJR_HAS_BUILTIN(POPCOUNT)

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int popcount_impl(T x) {
    if (WJR_BUILTIN_CONSTANT_P(is_zero_or_single_bit(x)) && is_zero_or_single_bit(x)) {
        return x != 0;
    }

#if WJR_HAS_BUILTIN(POPCOUNT)
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(x)) {
        return fallback_popcount(x);
    }

    return builtin_popcount(x);
#else
    return fallback_popcount(x);
#endif
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int popcount(T x) {
    int ret = popcount_impl(x);
    WJR_ASSUME(0 <= ret && ret <= std::numeric_limits<T>::digits);
    return ret;
}

} // namespace wjr

#endif // WJR_MATH_POPCOUNT_HPP