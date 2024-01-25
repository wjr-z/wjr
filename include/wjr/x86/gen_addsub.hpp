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
// GCC seems to have some optimization issues
#if defined(WJR_COMPILER_GCC)
            asm("sub{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
                : "ri"(b), "0"(a)
                : "cc");
            c_out = c_in;
            return a;
#else
            c_out = a < b;
            a -= b;
            return a;
#endif
        } else {
            c_in = 0;
            asm("stc\n\t"
                "sbb{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
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
            a += b;
            c_out = a < b;
            return a;
        } else {
            c_in = 0;
            asm("stc\n\t"
                "adc{q"
                " %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
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

    if (WJR_BUILTIN_CONSTANT_P(n)) {
        if (n == 0) {
            return c_in;
        }

        if (n == 1) {
            dst[0] = WJR_PP_CONCAT(asm_, WJR_addcsubc)(src0[0], src1[0], c_in, c_in);
            return c_in;
        }
    }

    size_t m = n / 8;
    n &= 7;

    T r8 = c_in, r9, r11;

    const auto cdst = dst;
    const auto csrc0 = src0;
    const auto csrc1 = src1;

    asm volatile(
        "add{b $255, %b[r8]| %b[r8], 255}\n\t"
        "lea{q| %[r9], [rip +} .Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r9] + %[r10]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"
        
        ".align 8\n\t"
        ".Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=:\n\t"
        ".long .Ll0%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
        ".long .Ll1%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
        ".long .Ll2%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
        ".long .Ll3%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
        ".long .Ll4%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
        ".long .Ll5%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
        ".long .Ll6%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
        ".long .Ll7%=-.Lasm_" WJR_PP_STR(WJR_addcsubc) "_n_lookup%=\n\t"
        ".align 16\n\t"
        
        ".Ld0%=:\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll0%=:\n\t"
        "jrcxz .Ld0%=\n\t"
        "mov{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r11]| %[r11], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ld1%=:\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mov{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "jrcxz .Ld1%=\n\t"
        "mov{q 8(%[src0]), %[r9]| %[r9], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "lea{q 8(%[src0]), %[src0]| %[src0], [%[src0] + 8]}\n\t"
        "lea{q 8(%[src1]), %[src1]| %[src1], [%[src1] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"
        "jmp .Lb1%=\n\t"

        ".Ll3%=:\n\t"
        "mov{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r8]| %[r8], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "lea{q -40(%[src0]), %[src0]| %[src0], [%[src0] - 40]}\n\t"
        "lea{q -40(%[src1]), %[src1]| %[src1], [%[src1] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "inc %[m]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mov{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r11]| %[r11], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "lea{q -32(%[src0]), %[src0]| %[src0], [%[src0] - 32]}\n\t"
        "lea{q -32(%[src1]), %[src1]| %[src1], [%[src1] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[m]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mov{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r9]| %[r9], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "lea{q -24(%[src0]), %[src0]| %[src0], [%[src0] - 24]}\n\t"
        "lea{q -24(%[src1]), %[src1]| %[src1], [%[src1] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[m]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mov{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r10]| %[r10], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "lea{q -16(%[src0]), %[src0]| %[src0], [%[src0] - 16]}\n\t"
        "lea{q -16(%[src1]), %[src1]| %[src1], [%[src1] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[m]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mov{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r8]| %[r8], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "lea{q -8(%[src0]), %[src0]| %[src0], [%[src0] - 8]}\n\t"
        "lea{q -8(%[src1]), %[src1]| %[src1], [%[src1] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[m]\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld2%=:\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src1]), %[r10]| %[r10], [%[src1] + 8]}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "mov{q %[r10], 8(%[dst])| [%[dst] + 8], %[r10]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll2%=:\n\t"
        "mov{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r10]| %[r10], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "jrcxz .Ld2%=\n\t"
        "lea{q 16(%[src0]), %[src0]| %[src0], [%[src0] + 16]}\n\t"
        "lea{q 16(%[src1]), %[src1]| %[src1], [%[src1] + 16]}\n\t"
        "lea{q 16(%[dst]), %[dst]| %[dst], [%[dst] + 16]}\n\t"

        ".align 32\n\t"
        ".Lwjr_asm_" WJR_PP_STR(WJR_addcsubc) "_n_loop%=:\n\t"

        ".Lb2%=:\n\t"
        "mov{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src1]), %[r10]| %[r10], [%[src1] - 8]}\n\t"
        "mov{q %[r8], -16(%[dst])| [%[dst] - 16], %[r8]}\n\t"

        ".Lb1%=:\n\t"
        "mov{q 8(%[src0]), %[r11]| %[r11], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Lb0%=:\n\t"
        "mov{q 16(%[src0]), %[r8]| %[r8], [%[src0] + 16]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src1]), %[r11]| %[r11], [%[src1] + 8]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Lb7%=:\n\t"
        "mov{q 24(%[src0]), %[r10]| %[r10], [%[src0] + 24]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 16(%[src1]), %[r8]| %[r8], [%[src1] + 16]}\n\t"
        "mov{q %[r11], 8(%[dst])| [%[dst] + 8], %[r11]}\n\t"

        ".Lb6%=:\n\t"
        "mov{q 32(%[src0]), %[r9]| %[r9], [%[src0] + 32]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 24(%[src1]), %[r10]| %[r10], [%[src1] + 24]}\n\t"
        "mov{q %[r8], 16(%[dst])| [%[dst] + 16], %[r8]}\n\t"

        ".Lb5%=:\n\t"
        "mov{q 40(%[src0]), %[r11]| %[r11], [%[src0] + 40]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 32(%[src1]), %[r9]| %[r9], [%[src1] + 32]}\n\t"
        "mov{q %[r10], 24(%[dst])| [%[dst] + 24], %[r10]}\n\t"

        ".Lb4%=:\n\t"
        "mov{q 48(%[src0]), %[r8]| %[r8], [%[src0] + 48]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 40(%[src1]), %[r11]| %[r11], [%[src1] + 40]}\n\t"
        "mov{q %[r9], 32(%[dst])| [%[dst] + 32], %[r9]}\n\t"

        ".Lb3%=:\n\t"
        "mov{q 56(%[src0]), %[r10]| %[r10], [%[src0] + 56]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 48(%[src1]), %[r8]| %[r8], [%[src1] + 48]}\n\t"
        "mov{q %[r11], 40(%[dst])| [%[dst] + 40], %[r11]}\n\t"

        // TODO : optimize pipeline
        "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
        "lea{q 64(%[src1]), %[src1]| %[src1], [%[src1] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"
        "dec %[m]\n\t"
        
        "jne .Lwjr_asm_" WJR_PP_STR(WJR_addcsubc) "_n_loop%=\n\t"

        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src1]), %[r10]| %[r10], [%[src1] - 8]}\n\t"
        "mov{q %[r8], -16(%[dst])| [%[dst] - 16], %[r8]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Ldone%=:\n\t"
        "mov %k[m], %k[r9]\n\t"
        "adc{l %k[m], %k[r9]| %k[r9], %k[m]}"

        : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+r"(src1), [m] "+c"(m), 
          [r8] "+r"(r8), [r9] "=r"(r9), [r10] "+r"(n), [r11] "=r"(r11)
        :
        : "cc", "memory");

    WJR_ASSUME(dst == cdst + n);
    WJR_ASSUME(src0 == csrc0 + n);
    WJR_ASSUME(src1 == csrc1 + n);

    return r9;
}

#endif

#undef WJR_adcsbb
#undef WJR_addsub
#undef WJR_addcsubc
#undef WJR_ADDSUB

#undef WJR_ADDSUB_I