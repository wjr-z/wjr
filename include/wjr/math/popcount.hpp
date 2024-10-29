#ifndef WJR_MATH_POPCOUNT_HPP__
#define WJR_MATH_POPCOUNT_HPP__

#include <wjr/math/detail.hpp>

#if WJR_HAS_SIMD(POPCNT)

    #if WJR_HAS_BUILTIN(__builtin_popcount)
        #define WJR_HAS_BUILTIN_POPCOUNT WJR_HAS_DEF
    #elif defined(_MSC_VER)
        #define WJR_HAS_BUILTIN_POPCOUNT WJR_HAS_DEF_VAR(2)
    #endif

    #if WJR_HAS_BUILTIN(POPCOUNT) == 2
        #include <wjr/arch/x86/simd/intrin.hpp>
    #endif

#endif

namespace wjr {

#if !WJR_HAS_BUILTIN(POPCOUNT)

namespace math_detail {

template <typename T, T seed>
class de_bruijn {
public:
    constexpr static uint8_t digits = std::numeric_limits<T>::digits;
    constexpr static uint8_t mv = digits == 32 ? 27 : 58;
    constexpr de_bruijn() noexcept : lookup(), lookupr() { initialize(); }

    constexpr int get(T idx) const noexcept { return lookup[(idx * seed) >> mv]; }
    constexpr int getr(T idx) const noexcept { return lookupr[(idx * seed) >> mv]; }

private:
    constexpr void initialize() noexcept {
        for (uint8_t i = 0; i < digits; ++i) {
            const auto idx = (seed << i) >> mv;
            lookup[idx] = i;
            lookupr[idx] = i == 0 ? 0u : digits - i;
        }
    }

    uint8_t lookup[digits];
    uint8_t lookupr[digits];
};

inline constexpr de_bruijn<uint32_t, 0x077C'B531> de_bruijn32 = {};
inline constexpr de_bruijn<uint64_t, 0x03f7'9d71'b4ca'8b09> de_bruijn64 = {};

} // namespace math_detail

#endif

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR int fallback_popcount(T x) noexcept {
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

#if WJR_HAS_BUILTIN(POPCOUNT)

template <typename T>
WJR_CONST WJR_INTRINSIC_INLINE int builtin_popcount(T x) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    #if WJR_HAS_BUILTIN(POPCOUNT) == 1
    if constexpr (nd < 32) {
        return builtin_popcount(static_cast<uint32_t>(x));
    } else {
        if constexpr (nd <= std::numeric_limits<unsigned int>::digits) {
            return __builtin_popcount(x);
        } else if constexpr (nd <= std::numeric_limits<unsigned long>::digits) {
            return __builtin_popcountl(x);
        } else if constexpr (nd <= std::numeric_limits<unsigned long long>::digits) {
            return __builtin_popcountll(x);
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }
    #else
    if constexpr (nd < 32) {
        return builtin_popcount(static_cast<uint32_t>(x));
    } else {
        if constexpr (nd <= 32) {
            return static_cast<int>(__popcnt(x));
        } else if constexpr (nd <= 64) {
            return static_cast<int>(__popcnt64(x));
        } else {
            static_assert(nd <= 64, "not support yet");
        }
    }

    #endif // WJR_HAS_BUILTIN(POPCOUNT)
}

#endif

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int popcount_impl(T x) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(is_zero_or_single_bit(x))) {
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
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int popcount(T x) noexcept {
    const int ret = popcount_impl(x);
    WJR_ASSUME(0 <= ret && ret <= std::numeric_limits<T>::digits);
    return ret;
}

} // namespace wjr

#endif // WJR_MATH_POPCOUNT_HPP__