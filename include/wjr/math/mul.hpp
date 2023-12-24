#ifndef WJR_MATH_MUL_HPP__
#define WJR_MATH_MUL_HPP__

#include <wjr/math/add.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul_(T a, T b, T &hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    using T2 = uint_t<nd * 2>;
    T2 x = static_cast<T2>(a) * b;
    hi = x >> nd;
    return static_cast<T>(x);
}

WJR_INTRINSIC_CONSTEXPR uint64_t fallback_mul64(uint64_t a, uint64_t b, uint64_t &hi) {
    uint64_t ah = a >> 32;
    uint64_t al = a & 0xFFFFFFFF;
    uint64_t bh = b >> 32;
    uint64_t bl = b & 0xFFFFFFFF;

    uint64_t rh = ah * bh;
    uint64_t rm0 = ah * bl;
    uint64_t rm1 = al * bh;
    uint64_t rl = al * bl;

    uint64_t t = rl + (rm0 << 32);
    uint64_t lo = t + (rm1 << 32);
    uint64_t c = t < rl;
    hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;
    return lo;
}

#define WJR_HAS_BUILTIN_MUL64 WJR_HAS_DEF

#if WJR_HAS_FEATURE(INT128)
#define WJR_HAS_BUILTIN_INT128_MUL64 WJR_HAS_DEF
#elif WJR_HAS_FEATURE(INLINE_ASM) && defined(WJR_X86) &&                                 \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_MUL64 WJR_HAS_DEF
#else
#undef WJR_HAS_BUILTIN_MUL64
#endif

#if WJR_HAS_BUILTIN(MUL64)

WJR_INTRINSIC_INLINE uint64_t builtin_mul64(uint64_t a, uint64_t b, uint64_t &hi) {
#if WJR_HAS_BUILTIN(INT128_MUL64)
    __uint128_t x = static_cast<__uint128_t>(a) * b;
    hi = x >> 64;
    return static_cast<uint64_t>(x);
#elif WJR_HAS_BUILTIN(ASM_MUL64)
    uint64_t lo = 0;
    asm volatile("mul{q %3|%3}\n\t"
                 : "=a,a"(lo), "=d,d"(hi)
                 : "%a,r"(a), "r,a"(b)
                 : "cc");
    return lo;
#endif
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR T mul(T a, T b, T &hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 64) {
        return fallback_mul_(a, b, hi);
    } else {

#if WJR_HAS_BUILTIN(MUL64)
        if (is_constant_evaluated() || is_constant_p(a) && is_constant_p(b)) {
            return fallback_mul64(a, b, hi);
        }

#if WJR_HAS_BUILTIN(ASM_MUL64)
        // mov b to rax, then mul a
        // instead of mov a to rax, mov b to register, then mul
        if (is_constant_p(b)) {
            return builtin_mul64(b, a, hi);
        }
#endif

        return builtin_mul64(a, b, hi);
#else
        return fallback_mul64(a, b, hi);
#endif
    }
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
T fallback_mulhi_(T a, T b) {
    T hi = 0;
    (void)fallback_mul_(a, b, hi);
    return hi;
}

WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
uint64_t fallback_mulhi64(uint64_t a, uint64_t b) {
    uint64_t hi = 0;
    (void)fallback_mul64(a, b, hi);
    return hi;
}

#if WJR_HAS_BUILTIN(MUL64)
#define WJR_HAS_BUILTIN_MULHI64 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(INT128_MUL64)
#define WJR_HAS_BUILTIN_INT128_MULHI64 WJR_HAS_DEF
#elif WJR_HAS_BUILTIN(ASM_MUL64)
#define WJR_HAS_BUILTIN_ASM_MULHI64 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(MULHI64)

WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
uint64_t builtin_mulhi64(uint64_t a, uint64_t b) {
#if WJR_HAS_BUILTIN(INT128_MUL64)
    return static_cast<uint64_t>((static_cast<__uint128_t>(a) * b) >> 64);
#elif WJR_HAS_BUILTIN(ASM_MUL64)
    uint64_t lo = 0;
    uint64_t hi = 0;
    asm volatile("mul{q %3|%3}\n\t"
                 : "=a,a"(lo), "=d,d"(hi)
                 : "%a,r"(a), "r,a"(b)
                 : "cc");
    return hi;
#endif
}

#endif

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
T mulhi(T a, T b) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 64) {
        return fallback_mulhi_(a, b);
    } else {

#if WJR_HAS_BUILTIN(MUL64)
        if (is_constant_evaluated() || is_constant_p(a) && is_constant_p(b)) {
            return fallback_mulhi64(a, b);
        }

#if WJR_HAS_BUILTIN(ASM_MUL64)
        // mov b to rax, then mul a
        // instead of mov a to rax, mov b to register, then mul
        if (is_constant_p(b)) {
            return builtin_mulhi64(b, a);
        }
#endif

        return builtin_mulhi64(a, b);
#else
        return fallback_mulhi64(a, b);
#endif
    }
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
T mullo(T a, T b) {
    return a * b;
}

} // namespace wjr

#endif // WJR_MATH_MUL_HPP__