// WJR_ADDSUB_I : 
// 0 : SUB
// 1 : ADD

#define WJR_ADDSUB WJR_PP_BOOL_IF(WJR_ADDSUB_I, ADDC, SUBC)
#define WJR_addcsubc WJR_PP_BOOL_IF(WJR_ADDSUB_I, addc, subc)
#define WJR_addsub WJR_PP_BOOL_IF(WJR_ADDSUB_I, add, sub)
#define WJR_adcsbb WJR_PP_BOOL_IF(WJR_ADDSUB_I, adc, sbb)

#if WJR_HAS_BUILTIN(WJR_PP_CONCAT(ASM_, WJR_PP_CONCAT(WJR_ADDSUB, _1)))

template <typename T, typename U>
WJR_INTRINSIC_INLINE T WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addcsubc, _1))(T a, T b,
                                                                          U c_in,
                                                                          U &c_out) {
    constexpr auto nd = std::numeric_limits<T>::digits;

#define WJR_REGISTER_BUILTIN_ASM_ADDSUB_IMPL(STR, suffix)                                \
    asm(STR "{" #suffix " %2, %0| %0, %2}\n\t"                                           \
        "setb %b1"                                                                       \
        : "=r"(a), "+r"(c_in)                                                            \
        : "%r"(b), "0"(a)                                                                \
        : "cc");                                                                         \
    c_out = static_cast<unsigned char>(c_in);                                            \
    return a

#define WJR_REGISTER_BUILTIN_ASM_ADDSUB(suffix, type)                                    \
    if constexpr (nd == std::numeric_limits<type>::digits) {                             \
        if (WJR_BUILTIN_CONSTANT_P(c_in)) {                                              \
            if (c_in == 0) {                                                             \
                WJR_REGISTER_BUILTIN_ASM_ADDSUB_IMPL(WJR_PP_STR(WJR_addsub), suffix);    \
            } else {                                                                     \
                WJR_REGISTER_BUILTIN_ASM_ADDSUB_IMPL("stc\n\t"                           \
                WJR_PP_STR(WJR_adcsbb), suffix);                                         \
            }                                                                            \
        }                                                                                \
        WJR_REGISTER_BUILTIN_ASM_ADDSUB_IMPL(                                            \
            "add{b $255, %b1| %b1, 255}\n\t" WJR_PP_STR(WJR_adcsbb), suffix);            \
                                                                                         \
    } else

    WJR_REGISTER_BUILTIN_ASM_ADDSUB(b, uint8_t)
    WJR_REGISTER_BUILTIN_ASM_ADDSUB(w, uint16_t)
    WJR_REGISTER_BUILTIN_ASM_ADDSUB(l, uint32_t)
    WJR_REGISTER_BUILTIN_ASM_ADDSUB(q, uint64_t) {
        static_assert(nd <= 64, "not supported yet");
    }

#undef WJR_REGISTER_BUILTIN_ASM_ADDSUB
#undef WJR_REGISTER_BUILTIN_ASM_ADDSUB_IMPL
}

#endif

#if WJR_HAS_BUILTIN(WJR_PP_CONCAT(ASM_, WJR_PP_CONCAT(WJR_ADDSUB, _N)))

template <typename T, typename U>
WJR_INLINE U WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addcsubc, _n))(T *dst, const T *src0,
                                                                const T *src1, size_t n,
                                                                U c_in) {
    if (WJR_BUILTIN_CONSTANT_P(n == 0) && n == 0) {
        return c_in;
    }

    if (WJR_BUILTIN_CONSTANT_P(n)) {
        if (n == 1) {
            dst[0] = WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addcsubc, _1))(src0[0], src1[0], c_in, c_in);
            return c_in;
        }
    }

    constexpr auto nd = std::numeric_limits<T>::digits;
    T t0 = c_in;

#define WJR_REGISTER_ASM_ADDSUB_N(type, suffix, offset0, offset1, offset2, offset3,      \
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
                     "lea{q| %[t2], [rip +} .Lasm_"                                      \
                     WJR_PP_STR(WJR_addcsubc) "_n_lookup%={(%%rip), %[t2]|]}\n\t"        \
                     "movs{lq (%[t2], %[t1], 4), %[t1]|xd %[t1], DWORD PTR [%[t2] + "    \
                     "%[t1] * 4]}\n\t"                                                   \
                     "lea{q (%[t2], %[t1], 1), %[t1]| %[t1], [%[t2] + %[t1]]}\n\t"       \
                     "jmp{q *%[t1]| %[t1]}\n\t"                                          \
                                                                                         \
                     ".align 4\n\t"                                                      \
                     ".Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=:\n\t"                \
                     ".long .Lcase0%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t" \
                     ".long .Lcase1%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t" \
                     ".long .Lcase2%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t" \
                     ".long .Lcase3%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t" \
                     ".long .Lcase4%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t" \
                     ".long .Lcase5%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t" \
                     ".long .Lcase6%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t" \
                     ".long .Lcase7%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t" \
                                                                                         \
                     ".Lwjr_asm_" WJR_PP_STR(WJR_addcsubc) "_n_loop%=:\n\t"              \
                     "lea{" #suffix " " #offset8                                         \
                     "(%[src0]), %[src0]| %[src0], [%[src0] + " #offset8 "]}\n\t"        \
                     "lea{" #suffix " " #offset8                                         \
                     "(%[src1]), %[src1]| %[src1], [%[src1] + " #offset8 "]}\n\t"        \
                     "lea{" #suffix " " #offset8                                         \
                     "(%[dst]), %[dst]| %[dst], [%[dst] + " #offset8 "]}\n\t"            \
                                                                                         \
                     "mov{" #suffix " " #offset0                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset0 "]}\n\t"            \
                     WJR_PP_STR(WJR_adcsbb) "{" #suffix " " #offset0                     \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset0 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset0 "(%[dst])| [%[dst] + " #offset0  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase7%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset1                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset1 "]}\n\t"            \
                     WJR_PP_STR(WJR_adcsbb) "{" #suffix " " #offset1                     \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset1 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset1 "(%[dst])| [%[dst] + " #offset1  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase6%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset2                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset2 "]}\n\t"            \
                     WJR_PP_STR(WJR_adcsbb) "{" #suffix " " #offset2                     \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset2 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset2 "(%[dst])| [%[dst] + " #offset2  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase5%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset3                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset3 "]}\n\t"            \
                     WJR_PP_STR(WJR_adcsbb) "{" #suffix " " #offset3                     \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset3 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset3 "(%[dst])| [%[dst] + " #offset3  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase4%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset4                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset4 "]}\n\t"            \
                     WJR_PP_STR(WJR_adcsbb) "{" #suffix " " #offset4                     \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset4 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset4 "(%[dst])| [%[dst] + " #offset4  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase3%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset5                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset5 "]}\n\t"            \
                     WJR_PP_STR(WJR_adcsbb) "{" #suffix " " #offset5                     \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset5 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset5 "(%[dst])| [%[dst] + " #offset5  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase2%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset6                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset6 "]}\n\t"            \
                     WJR_PP_STR(WJR_adcsbb) "{" #suffix " " #offset6                     \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset6 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset6 "(%[dst])| [%[dst] + " #offset6  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase1%=:\n\t"                                                    \
                     "mov{" #suffix " " #offset7                                         \
                     "(%[src0]), %[t0]| %[t0], [%[src0] + " #offset7 "]}\n\t"            \
                     WJR_PP_STR(WJR_adcsbb) "{" #suffix " " #offset7                     \
                     "(%[src1]), %[t0]| %[t0], [%[src1] + " #offset7 "]}\n\t"            \
                     "mov{" #suffix " %[t0], " #offset7 "(%[dst])| [%[dst] + " #offset7  \
                     "], %[t0]}\n\t"                                                     \
                                                                                         \
                     ".Lcase0%=:\n\t"                                                    \
                     "dec %[m]\n\t"                                                      \
                     "jne .Lwjr_asm_" WJR_PP_STR(WJR_addcsubc) "_n_loop%=\n\t"           \
                     "setb %b[t0]"                                                       \
                                                                                         \
                     : [dst] "+r"(dst), [src0] "+%r"(src0), [src1] "+r"(src1),           \
                       [m] "+r"(m), [t0] "+r"(t0), [t1] "+r"(t1), [t2] "=r"(t2)          \
                     :                                                                   \
                     : "cc", "memory");                                                  \
    } else

    WJR_REGISTER_ASM_ADDSUB_N(uint8_t, b, 0, 1, 2, 3, 4, 5, 6, 7, 8)
    WJR_REGISTER_ASM_ADDSUB_N(uint16_t, w, 0, 2, 4, 6, 8, 10, 12, 14, 16)
    WJR_REGISTER_ASM_ADDSUB_N(uint32_t, l, 0, 4, 8, 12, 16, 20, 24, 28, 32)
    WJR_REGISTER_ASM_ADDSUB_N(uint64_t, q, 0, 8, 16, 24, 32, 40, 48, 56, 64) {
        static_assert(nd <= 64, "not support yet");
    }

#undef WJR_REGISTER_ASM_ADDSUB_N

    return static_cast<unsigned char>(t0);
}

#endif

#undef WJR_adcsbb
#undef WJR_addsub
#undef WJR_addcsubc
#undef WJR_ADDSUB

#undef WJR_ADDSUB_I