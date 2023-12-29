#ifndef WJR_MATH_SHIFT_HPP__
#define WJR_MATH_SHIFT_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_shld(T hi, T lo, unsigned int c) {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return hi << c | lo >> (digits - c);
}

#if WJR_HAS_FEATURE(INLINE_ASM) && defined(WJR_X86) &&                                   \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_SHLD WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SHLD)

template <typename T>
WJR_INTRINSIC_CONSTEXPR T asm_shld(T hi, T lo, unsigned int c) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ASM_SHLD(args)                                              \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_ASM_SHLD_I_CALLER)
#define WJR_REGISTER_BUILTIN_ASM_SHLD_I(suffix, type)                                    \
    if constexpr (nd == std::numeric_limits<type>::digits) {                             \
        asm("shld{" #suffix " %b2, %1, %0| %0, %1, %2 }"                                 \
            : "+r"(hi)                                                                   \
            : "r"(lo), "ci"(c)                                                           \
            : "cc");                                                                     \
        return hi;                                                                       \
    } else
#define WJR_REGISTER_BUILTIN_ASM_SHLD_I_CALLER(args) WJR_REGISTER_BUILTIN_ASM_SHLD_I args

    WJR_REGISTER_BUILTIN_ASM_ADDC(
        ((b, uint8_t), (w, uint16_t), (l, uint32_t), (q, uint64_t))) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ASM_SHLD_I_CALLER
#undef WJR_REGISTER_BUILTIN_ASM_SHLD_I
#undef WJR_REGISTER_BUILTIN_ASM_SHLD
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR T shld(T hi, T lo, unsigned int c) {

#if WJR_HAS_BUILTIN(ASM_SHLD)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(c == 0) && c == 0)) {
        return fallback_shld(hi, lo, c);
    }

    return asm_shld(hi, lo, c);
#else
    return fallback_shld(hi, lo, c);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_shrd(T lo, T hi, unsigned int c) {
    constexpr auto digits = std::numeric_limits<T>::digits;
    return lo >> c | hi << (digits - c);
}

#if WJR_HAS_FEATURE(INLINE_ASM) && defined(WJR_X86) &&                                   \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_SHLD WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SHLD)

template <typename T>
WJR_INTRINSIC_CONSTEXPR T asm_shrd(T lo, T hi, unsigned int c) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ASM_SHLD(args)                                              \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_ASM_SHLD_I_CALLER)
#define WJR_REGISTER_BUILTIN_ASM_SHLD_I(suffix, type)                                    \
    if constexpr (nd == std::numeric_limits<type>::digits) {                             \
        asm("shrd{" #suffix " %b2, %1, %0| %0, %1, %2 }"                                 \
            : "+r"(lo)                                                                   \
            : "r"(hi), "ci"(c)                                                           \
            : "cc");                                                                     \
        return lo;                                                                       \
    } else
#define WJR_REGISTER_BUILTIN_ASM_SHLD_I_CALLER(args) WJR_REGISTER_BUILTIN_ASM_SHLD_I args

    WJR_REGISTER_BUILTIN_ASM_ADDC(
        ((b, uint8_t), (w, uint16_t), (l, uint32_t), (q, uint64_t))) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ASM_SHLD_I_CALLER
#undef WJR_REGISTER_BUILTIN_ASM_SHLD_I
#undef WJR_REGISTER_BUILTIN_ASM_SHLD
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR T shrd(T lo, T hi, unsigned int c) {

#if WJR_HAS_BUILTIN(ASM_SHLD)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(c == 0) && c == 0)) {
        return fallback_shrd(lo, hi, c);
    }

    return asm_shrd(lo, hi, c);
#else
    return fallback_shrd(lo, hi, c);
#endif
}

} // namespace wjr

#endif // WJR_MATH_SHIFT_HPP__