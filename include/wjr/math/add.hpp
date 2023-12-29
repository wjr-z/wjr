#ifndef WJR_MATH_ADD_HPP__
#define WJR_MATH_ADD_HPP__

#include <wjr/math/ctz.hpp>
#include <wjr/math/replace.hpp>

namespace wjr {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_addc(T a, T b, U c_in, U &c_out) {
    T ret = a;
    ret += b;
    U c = ret < b;
    ret += c_in;
    c |= ret < c_in;
    c_out = c;
    return ret;
}

#if WJR_HAS_BUILTIN(__builtin_addc)
#define WJR_HAS_BUILTIN_ADDC WJR_HAS_DEF
#endif

#if WJR_HAS_FEATURE(INLINE_ASM) && defined(WJR_X86) &&                                   \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_ADDC WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_ADDC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T asm_addc_1(T a, T b, U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ASM_ADDC(suffix, type)                                      \
    if constexpr (nd == std::numeric_limits<type>::digits) {                             \
        unsigned char cf;                                                                \
        asm("stc\n\t"                                                                    \
            "adc{" #suffix " %2, %0| %0, %2}\n\t"                                        \
            "setb %b1"                                                                   \
            : "=r"(a), "=r"(cf)                                                          \
            : "%r"(b), "0"(a)                                                            \
            : "cc");                                                                     \
        c_out = cf;                                                                      \
        return a;                                                                        \
    } else

    WJR_REGISTER_BUILTIN_ASM_ADDC(b, uint8_t)
    WJR_REGISTER_BUILTIN_ASM_ADDC(w, uint16_t)
    WJR_REGISTER_BUILTIN_ASM_ADDC(l, uint32_t)
    WJR_REGISTER_BUILTIN_ASM_ADDC(q, uint64_t) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ASM_ADDC
}

#endif // WJR_HAS_BUILTIN(ASM_ADDC)

#if WJR_HAS_BUILTIN(ADDC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T builtin_addc(T a, T b, U c_in, U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ADDC(suffix, type)                                          \
    if constexpr (nd <= std::numeric_limits<type>::digits) {                             \
        type __c_out;                                                                    \
        T ret = __builtin_addc##suffix(a, b, static_cast<type>(c_in), &__c_out);         \
        c_out = static_cast<U>(__c_out);                                                 \
        return ret;                                                                      \
    } else

    WJR_REGISTER_BUILTIN_ADDC(b, unsigned char)
    WJR_REGISTER_BUILTIN_ADDC(s, unsigned short)
    WJR_REGISTER_BUILTIN_ADDC(, unsigned int)
    WJR_REGISTER_BUILTIN_ADDC(l, unsigned long)
    WJR_REGISTER_BUILTIN_ADDC(ll, unsigned long long) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ADDC
}

#endif // WJR_HAS_BUILTIN(ADDC)

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR T addc(T a, T b, type_identity_t<U> c_in, U &c_out) {
    WJR_ASSERT_L(1, c_in == 0 || c_in == 1);
    WJR_ASSUME((c_in == 0 || c_in == 1));

#if !WJR_HAS_BUILTIN(ADDC) && !WJR_HAS_BUILTIN(ASM_ADDC)
    return fallback_addc(a, b, c_in, c_out);
#else
    constexpr auto is_constant_or_zero = [](const auto &x) -> int {
        return WJR_BUILTIN_CONSTANT_P(x) ? (x == 0 ? 2 : 1) : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when enabling
    // optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >=
         2)) {
        return fallback_addc(a, b, c_in, c_out);
    }

#if WJR_HAS_BUILTIN(ASM_ADDC)
    if (WJR_BUILTIN_CONSTANT_P(c_in == 1) && c_in == 1) {
        return asm_addc_1(a, b, c_out);
    }
#endif

#if WJR_HAS_BUILTIN(ADDC)
    return builtin_addc(a, b, c_in, c_out);
#else
    return fallback_addc(a, b, c_in, c_out);
#endif // WJR_HAS_BUILTIN(ADDC)

#endif
}

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR U addc_1(T *dst, const T *src0, size_t n, type_identity_t<T> src1,
                                 U c_in) {
    WJR_ASSUME(n >= 1);
    WJR_ASSUME(dst != nullptr);
    WJR_ASSUME(src0 != nullptr);

    dst[0] = addc(src0[0], src1, c_in, c_in);

    if (c_in) {
        size_t idx = 1 + replace_val(dst + 1, src0 + 1, n - 1, -1, 0);

        if (idx == n) {
            return static_cast<U>(1);
        }

        dst[idx] = src0[idx] + 1;

        dst += idx;
        src0 += idx;
        n -= idx;
    }

    if (src0 != dst) {
        std::copy(src0 + 1, src0 + n, dst + 1);
    }

    return static_cast<U>(0);
}

template <size_t div, typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U addc_n_res(T *dst, const T *src0, const T *src1, size_t n,
                                     U c_in) {

    constexpr size_t mask = div - 1;

    n &= mask;

    if (WJR_UNLIKELY(n == 0)) {
        return c_in;
    }

    dst += n;
    src0 += n;
    src1 += n;

#define WJR_REGISTER_ADDC_RES_CASE_CALLER(idx)                                           \
    case idx: {                                                                          \
        dst[-idx] = addc(src0[-idx], src1[-idx], c_in, c_in);                            \
        WJR_FALLTHROUGH;                                                                 \
    }

#define WJR_REGISTER_ADDC_RES_SWITCH_CALLER(size)                                        \
    if constexpr (div == size) {                                                         \
        switch (n) {                                                                     \
            WJR_PP_TRANSFORM_PUT(                                                        \
                WJR_PP_QUEUE_TRANSFORM(                                                  \
                    WJR_PP_QUEUE_REVERSE((WJR_PP_IOTA(WJR_PP_DEC(size)))), WJR_PP_INC),  \
                WJR_REGISTER_ADDC_RES_CASE_CALLER)                                       \
        }                                                                                \
        return c_in;                                                                     \
    } else

    WJR_REGISTER_ADDC_RES_SWITCH_CALLER(2)
    WJR_REGISTER_ADDC_RES_SWITCH_CALLER(4) WJR_REGISTER_ADDC_RES_SWITCH_CALLER(8) {
        static_assert(div <= 8, "not support yet");
    }

#undef WJR_REGISTER_ADDC_RES_CASE_CALLER
#undef WJR_REGISTER_ADDC_RES_SWITCH_CALLER
}

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR U fallback_addc_n(T *dst, const T *src0, const T *src1, size_t n,
                                          U c_in) {
    size_t m = n / 4;

    for (size_t i = 0; i < m; ++i) {
        dst[0] = addc(src0[0], src1[0], c_in, c_in);
        dst[1] = addc(src0[1], src1[1], c_in, c_in);
        dst[2] = addc(src0[2], src1[2], c_in, c_in);
        dst[3] = addc(src0[3], src1[3], c_in, c_in);

        dst += 4;
        src0 += 4;
        src1 += 4;
    }

    return addc_n_res<4>(dst, src0, src1, n, c_in);
}

#if WJR_HAS_BUILTIN(ASM_ADDC)
#define WJR_HAS_BUILTIN_ASM_ADDC_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_ADDC_N)

template <typename T, typename U>
WJR_INLINE U asm_addc_n(T *dst, const T *src0, const T *src1, size_t n, U c_in) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T t0 = c_in;

#define WJR_REGISTER_ASM_ADDC_N(type, suffix, offset0, offset1, offset2, offset3,        \
                                offset4, offset5, offset6, offset7, offset8)                      \
    if constexpr (nd == std::numeric_limits<type>::digits) {                             \
        size_t m = n / 8 + 1;                                                            \
        n &= 7;                                                                          \
        src0 += n - 8;                                                                   \
        src1 += n - 8;                                                                   \
        dst += n - 8;                                                                    \
        uint64_t t1;                                                                     \
        uint64_t t2;                                                                     \
        t1 = n;                                                                          \
        asm volatile("add{b $255, %b[t0]| %b[t0], 255}\n\t"                              \
                     "lea{q .Lasm_addc_n_lookup%=(%%rip), %[t2]| %[t2], "                \
                     "[rip+.Lasm_addc_n_lookup%=]}\n\t"                                  \
                     "movs{lq (%[t2], %[t1], 4), %[t1]|xd %[t1], DWORD PTR [%[t2] + "    \
                     "%[t1] * "                                                          \
                     "4]}\n\t"                                                           \
                     "lea{q (%[t2], %[t1], 1), %[t1]| %[t1], [%[t2] + %[t1]]}\n\t"       \
                     "jmp{q *%[t1]| %[t1]}\n\t"                                          \
                                                                                         \
                     ".align 8\n\t"                                                      \
                     ".Lasm_addc_n_lookup%=:\n\t"                                        \
                     ".long .Lcase0%=-.Lasm_addc_n_lookup%=\n\t"                         \
                     ".long .Lcase1%=-.Lasm_addc_n_lookup%=\n\t"                         \
                     ".long .Lcase2%=-.Lasm_addc_n_lookup%=\n\t"                         \
                     ".long .Lcase3%=-.Lasm_addc_n_lookup%=\n\t"                         \
                     ".long .Lcase4%=-.Lasm_addc_n_lookup%=\n\t"                         \
                     ".long .Lcase5%=-.Lasm_addc_n_lookup%=\n\t"                         \
                     ".long .Lcase6%=-.Lasm_addc_n_lookup%=\n\t"                         \
                     ".long .Lcase7%=-.Lasm_addc_n_lookup%=\n\t"                         \
                                                                                         \
                     ".Lwjr_asm_addc_n_loop%=:\n\t"                                      \
                     "lea{" #suffix " " #offset8                                         \
                     "(%[src0]), %[src0]| %[src0], [%[src0] + " #offset8 "]}\n\t"        \
                     "lea{" #suffix " " #offset8                                         \
                     "(%[src1]), %[src1]| %[src1], [%[src1] + " #offset8 "]}\n\t"        \
                     "lea{" #suffix " " #offset8                                         \
                     "(%[dst]), %[dst]| %[dst], [%[dst] + " #offset8 "]}\n\t"            \
                                                                                         \
                     "mov{" #suffix " " #offset0                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset0 "]}\n\t"            \
                     "adc{" #suffix " " #offset0                                         \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset0 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset0 "(%[dst])| [%[dst] + " #offset0  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase7%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset1                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset1 "]}\n\t"            \
                     "adc{" #suffix " " #offset1                                         \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset1 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset1 "(%[dst])| [%[dst] + " #offset1  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase6%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset2                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset2 "]}\n\t"            \
                     "adc{" #suffix " " #offset2                                         \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset2 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset2 "(%[dst])| [%[dst] + " #offset2  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase5%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset3                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset3 "]}\n\t"            \
                     "adc{" #suffix " " #offset3                                         \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset3 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset3 "(%[dst])| [%[dst] + " #offset3  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase4%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset4                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset4 "]}\n\t"            \
                     "adc{" #suffix " " #offset4                                         \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset4 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset4 "(%[dst])| [%[dst] + " #offset4  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase3%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset5                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset5 "]}\n\t"            \
                     "adc{" #suffix " " #offset5                                         \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset5 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset5 "(%[dst])| [%[dst] + " #offset5  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase2%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset6                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset6 "]}\n\t"            \
                     "adc{" #suffix " " #offset6                                         \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset6 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset6 "(%[dst])| [%[dst] + " #offset6  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase1%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset7                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset7 "]}\n\t"            \
                     "adc{" #suffix " " #offset7                                         \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset7 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset7 "(%[dst])| [%[dst] + " #offset7  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase0%=:\n\t"                                                    \
                     "dec %[m]\n\t"                                                      \
                     "jne .Lwjr_asm_addc_n_loop%=\n\t"                                   \
                     "setb %b[t0]"                                                       \
                                                                                         \
                     : [dst] "+r"(dst), [src0] "+%r"(src0), [src1] "+r"(src1),           \
                       [m] "+r"(m), [t0] "+r"(t0), [t1] "+r"(t1), [t2] "=r"(t2)          \
                     :                                                                   \
                     : "cc", "memory");                                                  \
    } else

    WJR_REGISTER_ASM_ADDC_N(uint8_t, b, 0, 1, 2, 3, 4, 5, 6, 7, 8)
    WJR_REGISTER_ASM_ADDC_N(uint16_t, w, 0, 2, 4, 6, 8, 10, 12, 14, 16)
    WJR_REGISTER_ASM_ADDC_N(uint32_t, l, 0, 4, 8, 12, 16, 20, 24, 28, 32)
    WJR_REGISTER_ASM_ADDC_N(uint64_t, q, 0, 8, 16, 24, 32, 40, 48, 56, 64) {
        static_assert(nd <= 64, "not support yet");
    }

#undef WJR_REGISTER_ASM_ADDC_N

    return static_cast<unsigned char>(t0);
}

#endif

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR U addc_n(T *dst, const T *src0, const T *src1, size_t n, U c_in) {
#if WJR_HAS_BUILTIN(ASM_ADDC_N)
    if (is_constant_evaluated()) {
        return fallback_addc_n(dst, src0, src1, n, c_in);
    }

    return asm_addc_n(dst, src0, src1, n, c_in);
#else
    return fallback_addc_n(dst, src0, src1, n, c_in);
#endif
}

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR U addc(T *dst, const T *src0, size_t n, const T *src1, size_t m,
                               U c_in) {
    WJR_ASSUME(n >= m);
    WJR_ASSERT(n >= m);

    c_in = addc_n(dst, src0, src1, m, c_in);

    if (n != m) {
        c_in = addc_1(dst + m, src0 + m, n - m, 0u, c_in);
    }

    return c_in;
}

} // namespace wjr

#endif // WJR_MATH_ADD_HPP__