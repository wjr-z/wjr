#ifndef WJR_MATH_SUB_HPP__
#define WJR_MATH_SUB_HPP__

#include <iostream>
#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_subc(T a, T b, U c_in, U &c_out) {
    T ret = a;
    U c = 0;
    c = ret < b;
    ret -= b;
    c |= ret < c_in;
    ret -= c_in;
    c_out = c;
    return ret;
}

#if WJR_HAS_BUILTIN(__builtin_subc) || WJR_HAS_CLANG(5, 0, 0)
#define WJR_HAS_BUILTIN_SUBC WJR_HAS_DEF
#endif

#if WJR_HAS_FEATURE(INLINE_ASM) && defined(WJR_X86) &&                                   \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_SUBC WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SUBC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T asm_subc_1(T a, T b, U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ASM_SUBC(args)                                              \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_ASM_SUBC_I_CALLER)
#define WJR_REGISTER_BUILTIN_ASM_SUBC_I(suffix, type)                                    \
    if constexpr (nd == std::numeric_limits<type>::digits) {                             \
        unsigned char cf = 0;                                                            \
        asm volatile("stc\n\t"                                                           \
                     "sbb{" #suffix " %2, %0| %0, %2}\n\t"                               \
                     "setb %b1\n\t"                                                      \
                     : "=r"(a), "=r"(cf)                                                 \
                     : "%r"(b), "0"(a)                                                   \
                     : "cc");                                                            \
        c_out = cf;                                                                      \
        return a;                                                                        \
    } else
#define WJR_REGISTER_BUILTIN_ASM_SUBC_I_CALLER(args) WJR_REGISTER_BUILTIN_ASM_SUBC_I args

    WJR_REGISTER_BUILTIN_ASM_SUBC(
        ((b, uint8_t), (w, uint16_t), (l, uint32_t), (q, uint64_t))) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ASM_SUBC_I_CALLER
#undef WJR_REGISTER_BUILTIN_ASM_SUBC_I
#undef WJR_REGISTER_BUILTIN_ASM_SUBC
}

#endif // WJR_HAS_BUILTIN(ASM_SUBC)

#if WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T builtin_subc(T a, T b, U c_in, U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_SUBC(args)                                                  \
    WJR_PP_TRANSFORM_PUT(args, WJR_REGISTER_BUILTIN_SUBC_I_CALLER)
#define WJR_REGISTER_BUILTIN_SUBC_I(suffix, type)                                        \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        type __c_out = 0;                                                                \
        T ret = __builtin_subc##suffix(a, b, static_cast<type>(c_in), &__c_out);         \
        c_out = static_cast<U>(__c_out);                                                 \
        return ret;                                                                      \
    } else
#define WJR_REGISTER_BUILTIN_SUBC_I_CALLER(args) WJR_REGISTER_BUILTIN_SUBC_I args

    WJR_REGISTER_BUILTIN_SUBC(((b, unsigned char), (s, unsigned short), (, unsigned int),
                               (l, unsigned long), (ll, unsigned long long))) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_SUBC_I_CALLER
#undef WJR_REGISTER_BUILTIN_SUBC_I
#undef WJR_REGISTER_BUILTIN_SUBC
}

#endif // WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T subc(T a, T b, type_identity_t<U> c_in, U &c_out) {
    WJR_ASSERT_L(1, c_in == 0 || c_in == 1);
    WJR_ASSUME((c_in == 0 || c_in == 1));

#if !WJR_HAS_BUILTIN(SUBC) && !WJR_HAS_BUILTIN(ASM_SUBC)
    return fallback_subc(a, b, c_in, c_out);
#else
    constexpr auto is_constant_or_zero = [](const auto &x) -> int {
        return is_constant_p(x) ? (x == 0 ? 2 : 1) : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_subc(a, b, c_in, c_out);
    }

#if WJR_HAS_BUILTIN(ASM_SUBC)
    if (is_constant_p(c_in) && c_in == 1) {
        return asm_subc_1(a, b, c_out);
    }
#endif

#if WJR_HAS_BUILTIN(SUBC)
    return builtin_subc(a, b, c_in, c_out);
#else
    return fallback_subc(a, b, c_in, c_out);
#endif // WJR_HAS_BUILTIN(SUBC)

#endif
}

template <size_t div, typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U subc_n_res(const T *src0, const T *src1, T *dst, U c_in,
                                     size_t n) {

    constexpr size_t mask = div - 1;

    n &= mask;

    if (WJR_UNLIKELY(n == 0)) {
        return c_in;
    }

    src0 += n;
    src1 += n;
    dst += n;

#define WJR_REGISTER_SUBC_RES_CASE_CALLER(idx)                                           \
    case idx: {                                                                          \
        dst[-idx] = subc(src0[-idx], src1[-idx], c_in, c_in);                            \
        WJR_FALLTHROUGH;                                                                 \
    }

#define WJR_REGISTER_SUBC_RES_SWITCH_CALLER(size)                                        \
    if constexpr (div == size) {                                                         \
        switch (n) {                                                                     \
            WJR_PP_TRANSFORM_PUT(                                                        \
                WJR_PP_QUEUE_TRANSFORM(                                                  \
                    WJR_PP_QUEUE_REVERSE((WJR_PP_IOTA(WJR_PP_DEC(size)))), WJR_PP_INC),  \
                WJR_REGISTER_SUBC_RES_CASE_CALLER)                                       \
        }                                                                                \
        return c_in;                                                                     \
    } else

    WJR_REGISTER_SUBC_RES_SWITCH_CALLER(2)
    WJR_REGISTER_SUBC_RES_SWITCH_CALLER(4) WJR_REGISTER_SUBC_RES_SWITCH_CALLER(8) {
        static_assert(div <= 8, "not support yet");
    }

#undef WJR_REGISTER_SUBC_RES_SWITCH_CALLER
#undef WJR_REGISTER_SUBC_RES_CASE_CALLER
} // namespace wjr

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U subc_n_impl(const T *src0, const T *src1, T *dst, U c_in,
                                      size_t n) {
    size_t m = n / 4;
    for (size_t i = 0; i < m; ++i) {
        dst[0] = subc(src0[0], src1[0], c_in, c_in);
        dst[1] = subc(src0[1], src1[1], c_in, c_in);
        dst[2] = subc(src0[2], src1[2], c_in, c_in);
        dst[3] = subc(src0[3], src1[3], c_in, c_in);

        src0 += 4;
        src1 += 4;
        dst += 4;
    }

    return subc_n_res<4>(src0, src1, dst, c_in, n);
}

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U subc_n(const T *src0, const T *src1, T *dst, U c_in, size_t n) {
    return subc_n_impl(src0, src1, dst, c_in, n);
}

} // namespace wjr

#endif // WJR_MATH_SUB_HPP__