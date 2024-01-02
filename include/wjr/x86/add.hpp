#ifndef WJR_X86_ADD_HPP__
#define WJR_X86_ADD_HPP__

#include <wjr/type_traits.hpp>

#if !defined(WJR_X86)
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_FEATURE(INLINE_ASM) &&                                                       \
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

#if WJR_HAS_BUILTIN(ASM_ADDC)
#define WJR_HAS_BUILTIN_ASM_ADDC_N WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_ADDC_N)

template <typename T, typename U>
WJR_INLINE U asm_addc_n(T *dst, const T *src0, const T *src1, size_t n, U c_in) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    T t0 = c_in;

#define WJR_REGISTER_ASM_ADDC_N(type, suffix, offset0, offset1, offset2, offset3,        \
                                offset4, offset5, offset6, offset7, offset8)             \
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
                     "lea{q| %[t2], [rip +} .Lasm_addc_n_lookup%={(%%rip), %[t2]|]}\n\t" \
                     "movs{lq (%[t2], %[t1], 4), %[t1]|xd %[t1], DWORD PTR [%[t2] + "    \
                     "%[t1] * "                                                          \
                     "4]}\n\t"                                                           \
                     "lea{q (%[t2], %[t1], 1), %[t1]| %[t1], [%[t2] + %[t1]]}\n\t"       \
                     "jmp{q *%[t1]| %[t1]}\n\t"                                          \
                                                                                         \
                     ".align 4\n\t"                                                      \
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

} // namespace wjr

#endif // WJR_X86_ADD_HPP__