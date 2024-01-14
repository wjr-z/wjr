// WJR_ADDSUB_I :
// 0 : SUB
// 1 : ADD

#ifndef WJR_ADDSUB_I
#error "abort"
#endif

#define WJR_ADDSUB WJR_PP_BOOL_IF(WJR_ADDSUB_I, ADDC, SUBC)
#define WJR_addcsubc WJR_PP_BOOL_IF(WJR_ADDSUB_I, addc, subc)
#define WJR_addsub WJR_PP_BOOL_IF(WJR_ADDSUB_I, add, sub)
#define WJR_adcsbb WJR_PP_BOOL_IF(WJR_ADDSUB_I, adc, sbb)

#if WJR_HAS_BUILTIN(WJR_PP_CONCAT(ASM_, WJR_ADDSUB))

template <typename T, typename U>
WJR_INTRINSIC_INLINE T WJR_PP_CONCAT(asm_, WJR_addcsubc)(T a, T b, U c_in, U &c_out) {
    static_assert(std::is_same_v<T, uint64_t>, "");

#if WJR_ADDSUB_I == 0
    if (WJR_BUILTIN_CONSTANT_P(c_in)) {
        if (c_in == 0) {
            asm("sub{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "=r"(c_in)
                : "ri"(b), "0"(a)
                : "cc");
            c_out = c_in;
            return a;
        } else {
            asm("stc\n\t"
                "sbb{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "=r"(c_in)
                : "ri"(b), "0"(a)
                : "cc");
            c_out = c_in;
            return a;
        }
    }

    asm("add{b $255, %b1| %b1, 255}\n\t"
        "sbb{q %2, %0| %0, %2}\n\t"
        "setb %b1"
        : "=r"(a), "+r"(c_in)
        : "ri"(b), "0"(a)
        : "cc");
    c_out = c_in;
    return a;
#else
    if (WJR_BUILTIN_CONSTANT_P(c_in)) {
        if (c_in == 0) {
            asm("add{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "=r"(c_in)
                : "ri"(b), "0"(a)
                : "cc");
            c_out = c_in;
            return a;
        } else {
            asm("stc\n\t"
                "adc{q"
                " %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "=r"(c_in)
                : "ri"(b), "0"(a)
                : "cc");
            c_out = c_in;
            return a;
        }
    }

    if (WJR_BUILTIN_CONSTANT_P(a)) {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "adc{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(b), "+r"(c_in)
            : "ri"(a), "0"(b)
            : "cc");
        c_out = c_in;
        return b;
    }

    asm("add{b $255, %b1| %b1, 255}\n\t"
        "adc{q %2, %0| %0, %2}\n\t"
        "setb %b1"
        : "=r"(a), "+r"(c_in)
        : "ri"(b), "0"(a)
        : "cc");
    c_out = c_in;
    return a;
#endif

#undef WJR_REGISTER_BUILTIN_ASM_ADDSUB_1
#undef WJR_REGISTER_BUILTIN_ASM_ADDSUB_0
}

#endif

#if WJR_HAS_BUILTIN(WJR_PP_CONCAT(ASM_, WJR_PP_CONCAT(WJR_ADDSUB, _N)))

template <typename T, typename U>
WJR_INLINE U WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addcsubc, _n))(T *dst, const T *src0,
                                                                  const T *src1, size_t n,
                                                                  U c_in) {
    static_assert(std::is_same_v<T, uint64_t>, "");

    if (WJR_BUILTIN_CONSTANT_P(n == 0) && n == 0) {
        return c_in;
    }

    if (WJR_BUILTIN_CONSTANT_P(n)) {
        if (n == 1) {
            dst[0] = WJR_PP_CONCAT(asm_, WJR_addcsubc)(src0[0], src1[0], c_in, c_in);
            return c_in;
        }
    }

    T t0 = c_in;

    size_t m = n / 8 + 1;
    n &= 7;
    src0 += n - 8;
    src1 += n - 8;
    dst += n - 8;
    asm volatile("add{b $255, %b[t0]| %b[t0], 255}\n\t"
                     "lea{q| %[t0], [rip +} .Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%={(%%rip), %[t0]|]}\n\t"
                     "movs{lq (%[t0], %[n], 4), %[n]|xd %[n], DWORD PTR [%[t0] + %[n] * 4]}\n\t"
                     "lea{q (%[t0], %[n], 1), %[n]| %[n], [%[t0] + %[n]]}\n\t"
                     "jmp{q *%[n]| %[n]}\n\t"
                     ".align 8\n\t"
                     ".Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=:\n\t"
                     ".long .Lcase0%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
                     ".long .Lcase1%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
                     ".long .Lcase2%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
                     ".long .Lcase3%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
                     ".long .Lcase4%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
                     ".long .Lcase5%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
                     ".long .Lcase6%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
                     ".long .Lcase7%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
                     ".align 16\n\t"

                     ".Lwjr_asm_" WJR_PP_STR(WJR_addcsubc) "_n_loop%=:\n\t"

                     "mov{q (%[src0]), %[t0]| %[t0], [%[src0]]}\n\t"
                     WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[t0]| %[t0], [%[src1]]}\n\t"
                     "mov{q %[t0], (%[dst])| [%[dst]], %[t0]}\n\t"

                     ".Lcase7%=:\n\t"
                     "mov{q 8(%[src0]), %[t0]| %[t0], [%[src0] + 8]}\n\t"
                     WJR_PP_STR(WJR_adcsbb) "{q 8(%[src1]), %[t0]| %[t0], [%[src1] + 8]}\n\t"
                     "mov{q %[t0], 8(%[dst])| [%[dst] + 8], %[t0]}\n\t"

                     ".Lcase6%=:\n\t"
                     "mov{q 16(%[src0]), %[t0]| %[t0], [%[src0] + 16]}\n\t"
                     WJR_PP_STR(WJR_adcsbb) "{q 16(%[src1]), %[t0]| %[t0], [%[src1] + 16]}\n\t"
                     "mov{q %[t0], 16(%[dst])| [%[dst] + 16], %[t0]}\n\t"

                     ".Lcase5%=:\n\t"
                     "mov{q 24(%[src0]), %[t0]| %[t0], [%[src0] + 24]}\n\t"
                     WJR_PP_STR(WJR_adcsbb) "{q 24(%[src1]), %[t0]| %[t0], [%[src1] + 24]}\n\t"
                     "mov{q %[t0], 24(%[dst])| [%[dst] + 24], %[t0]}\n\t"

                     ".Lcase4%=:\n\t"
                     "mov{q 32(%[src0]), %[t0]| %[t0], [%[src0] + 32]}\n\t"
                     WJR_PP_STR(WJR_adcsbb) "{q 32(%[src1]), %[t0]| %[t0], [%[src1] + 32]}\n\t"
                     "mov{q %[t0], 32(%[dst])| [%[dst] + 32], %[t0]}\n\t"

                     ".Lcase3%=:\n\t"
                     "mov{q 40(%[src0]), %[t0]| %[t0], [%[src0] + 40]}\n\t"
                     WJR_PP_STR(WJR_adcsbb) "{q 40(%[src1]), %[t0]| %[t0], [%[src1] + 40]}\n\t"
                     "mov{q %[t0], 40(%[dst])| [%[dst] + 40], %[t0]}\n\t"

                     ".Lcase2%=:\n\t"
                     "mov{q 48(%[src0]), %[t0]| %[t0], [%[src0] + 48]}\n\t"
                     WJR_PP_STR(WJR_adcsbb) "{q 48(%[src1]), %[t0]| %[t0], [%[src1] + 48]}\n\t"
                     "mov{q %[t0], 48(%[dst])| [%[dst] + 48], %[t0]}\n\t"

                     ".Lcase1%=:\n\t"
                     "mov{q 56(%[src0]), %[t0]| %[t0], [%[src0] + 56]}\n\t"
                     WJR_PP_STR(WJR_adcsbb) "{q 56(%[src1]), %[t0]| %[t0], [%[src1] + 56]}\n\t"
                     "mov{q %[t0], 56(%[dst])| [%[dst] + 56], %[t0]}\n\t"

                     ".Lcase0%=:\n\t"

                     "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
                     "lea{q 64(%[src1]), %[src1]| %[src1], [%[src1] + 64]}\n\t"
                     "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"
                     "dec %[m]\n\t"
                     "jne .Lwjr_asm_" WJR_PP_STR(WJR_addcsubc) "_n_loop%=\n\t"
                     "setb %b[t0]"

                     : [dst] "+r"(dst), [src0] "+%r"(src0), [src1] "+r"(src1),
                       [m] "+r"(m), [t0] "+r"(t0), [n] "+r"(n)
                     :
                     : "cc", "memory");

    return static_cast<unsigned char>(t0);
}

#endif

#undef WJR_adcsbb
#undef WJR_addsub
#undef WJR_addcsubc
#undef WJR_ADDSUB

#undef WJR_ADDSUB_I