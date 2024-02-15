// WJR_ADDSUB_I :
// 0 : SUB
// 1 : ADD

#ifndef WJR_ADDSUB_I
#error "abort"
#endif

#define WJR_addsub WJR_PP_BOOL_IF(WJR_ADDSUB_I, add, rsb)
#define WJR_adcsbb WJR_PP_BOOL_IF(WJR_ADDSUB_I, adc, sbb)

inline uint64_t WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addsub, lsh_n))(
    uint64_t *dst, const uint64_t *src0, const uint64_t *src1, size_t n, uint64_t cl) {
    WJR_ASSERT(cl != 0);

    size_t cx = n / 8;
    uint64_t tcl = 64 - cl;
    uint64_t r8, r9 = n, r10;

    asm volatile(
        "and{l $7, %k[r9]| %k[r9], 7}\n\t"
        "lea{q| %[r8], [rip +} .Llookup%={(%%rip), %[r8]|]}\n\t"
        "movs{lq (%[r8], %[r9], 4), %[r9]|xd %[r9], DWORD PTR [%[r8] + "
        "%[r9] * 4]}\n\t"
        "lea{q (%[r8], %[r9], 1), %[r9]| %[r9], [%[r9] + %[r8]]}\n\t"
        "jmp{q *%[r9]| %[r9]}\n\t"

        ".align 8\n\t"
        ".Llookup%=:\n\t"
        ".long .Ll0%=-.Llookup%=\n\t"
        ".long .Ll1%=-.Llookup%=\n\t"
        ".long .Ll2%=-.Llookup%=\n\t"
        ".long .Ll3%=-.Llookup%=\n\t"
        ".long .Ll4%=-.Llookup%=\n\t"
        ".long .Ll5%=-.Llookup%=\n\t"
        ".long .Ll6%=-.Llookup%=\n\t"
        ".long .Ll7%=-.Llookup%=\n\t"
        ".align 16\n\t"

        ".Ll0%=:\n\t"
        "mov{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "xor %k[r9], %k[r9]\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mov{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "xor %k[r9], %k[r9]\n\t"
        "lea{q -48(%[src0]), %[src0]| %[src0], [%[src0] - 48]}\n\t"
        "lea{q -48(%[src1]), %[src1]| %[src1], [%[src1] - 48]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mov{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "xor %k[r10], %k[r10]\n\t"
        "lea{q -40(%[src0]), %[src0]| %[src0], [%[src0] - 40]}\n\t"
        "lea{q -40(%[src1]), %[src1]| %[src1], [%[src1] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mov{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "xor %k[r9], %k[r9]\n\t"
        "lea{q -32(%[src0]), %[src0]| %[src0], [%[src0] - 32]}\n\t"
        "lea{q -32(%[src1]), %[src1]| %[src1], [%[src1] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mov{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "xor %k[r10], %k[r10]\n\t"
        "lea{q -24(%[src0]), %[src0]| %[src0], [%[src0] - 24]}\n\t"
        "lea{q -24(%[src1]), %[src1]| %[src1], [%[src1] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mov{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "xor %k[r9], %k[r9]\n\t"
        "lea{q -16(%[src0]), %[src0]| %[src0], [%[src0] - 16]}\n\t"
        "lea{q -16(%[src1]), %[src1]| %[src1], [%[src1] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mov{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "xor %k[r10], %k[r10]\n\t"
        "lea{q -8(%[src0]), %[src0]| %[src0], [%[src0] - 8]}\n\t"
        "lea{q -8(%[src1]), %[src1]| %[src1], [%[src1] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld1%=:\n\t"
        "shlx{q %[cl], %[r9], %[r8]| %[r8], %[r9], %[cl]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r8]| [%[src0]], %[r8]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9], %[r9], %[tcl]}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q %[cx], %[r9]| %[r9], %[cx]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mov{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "xor %k[r10], %k[r10]\n\t"
        "jrcxz .Ld1%=\n\t"
        "lea{q 8(%[src0]), %[src0]| %[src0], [%[src0] + 8]}\n\t"
        "lea{q 8(%[src1]), %[src1]| %[src1], [%[src1] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"

        ".align 32\n\t"
        ".Lloop%=:\n\t"

        ".Lb1%=:\n\t"
        "shlx{q %[cl], %[r9], %[r8]| %[r8], %[r9], %[cl]}\n\t"
        "lea{q (%[r10], %[r8]), %[r8]| %[r8], [%[r10] + %[r8]]}\n\t"
        "mov{q (%[src1]), %[r10]| [%[src1]], %[r10]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src0]), %[r8]| [%[src0] - 8], %[r8]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9],%[r9], %[tcl]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        ".Lb0%=:\n\t"
        "shlx{q %[cl], %[r10], %[r8]| %[r8], %[r10], %[cl]}\n\t"
        "lea{q (%[r9], %[r8]), %[r8]| %[r8], [%[r9] + %[r8]]}\n\t"
        "mov{q 8(%[src1]), %[r9]| [%[src1] + 8], %[r9]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src0]), %[r8]| [%[src0]], %[r8]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10],%[r10], %[tcl]}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"

        "lea{q -1(%[cx]), %[cx]| %[cx], [%[cx] - 1]}\n\t"

        ".Lb7%=:\n\t"
        "shlx{q %[cl], %[r9], %[r8]| %[r8], %[r9], %[cl]}\n\t"
        "lea{q (%[r10], %[r8]), %[r8]| %[r8], [%[r10] + %[r8]]}\n\t"
        "mov{q 16(%[src1]), %[r10]| [%[src1] + 16], %[r10]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 8(%[src0]), %[r8]| [%[src0] + 8], %[r8]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9],%[r9], %[tcl]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        ".Lb6%=:\n\t"
        "shlx{q %[cl], %[r10], %[r8]| %[r8], %[r10], %[cl]}\n\t"
        "lea{q (%[r9], %[r8]), %[r8]| %[r8], [%[r9] + %[r8]]}\n\t"
        "mov{q 24(%[src1]), %[r9]| [%[src1] + 24], %[r9]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 16(%[src0]), %[r8]| [%[src0] + 16], %[r8]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10],%[r10], %[tcl]}\n\t"
        "mov{q %[r8], 16(%[dst])| [%[dst] + 16], %[r8]}\n\t"

        ".Lb5%=:\n\t"
        "shlx{q %[cl], %[r9], %[r8]| %[r8], %[r9], %[cl]}\n\t"
        "lea{q (%[r10], %[r8]), %[r8]| %[r8], [%[r10] + %[r8]]}\n\t"
        "mov{q 32(%[src1]), %[r10]| [%[src1] + 32], %[r10]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 24(%[src0]), %[r8]| [%[src0] + 24], %[r8]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9],%[r9], %[tcl]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        ".Lb4%=:\n\t"
        "shlx{q %[cl], %[r10], %[r8]| %[r8], %[r10], %[cl]}\n\t"
        "lea{q (%[r9], %[r8]), %[r8]| %[r8], [%[r9] + %[r8]]}\n\t"
        "mov{q 40(%[src1]), %[r9]| [%[src1] + 40], %[r9]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 32(%[src0]), %[r8]| [%[src0] + 32], %[r8]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10],%[r10], %[tcl]}\n\t"
        "mov{q %[r8], 32(%[dst])| [%[dst] + 32], %[r8]}\n\t"

        ".Lb3%=:\n\t"
        "shlx{q %[cl], %[r9], %[r8]| %[r8], %[r9], %[cl]}\n\t"
        "lea{q (%[r10], %[r8]), %[r8]| %[r8], [%[r10] + %[r8]]}\n\t"
        "mov{q 48(%[src1]), %[r10]| [%[src1] + 48], %[r10]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 40(%[src0]), %[r8]| [%[src0] + 40], %[r8]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9],%[r9], %[tcl]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        ".Lb2%=:\n\t"
        "shlx{q %[cl], %[r10], %[r8]| %[r8], %[r10], %[cl]}\n\t"
        "lea{q (%[r9], %[r8]), %[r8]| %[r8], [%[r9] + %[r8]]}\n\t"
        "mov{q 56(%[src1]), %[r9]| [%[src1] + 56], %[r9]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q 48(%[src0]), %[r8]| [%[src0]+ 48], %[r8]}\n\t"
        "shrx{q %[tcl], %[r10], %[r10]| %[r10],%[r10], %[tcl]}\n\t"
        "mov{q %[r8], 48(%[dst])| [%[dst] + 48], %[r8]}\n\t"

        "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
        "lea{q 64(%[src1]), %[src1]| %[src1], [%[src1] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Lloop_out%=\n\t"
        "jmp .Lloop%=\n\t"
        ".Lloop_out%=:\n\t"

        "shlx{q %[cl], %[r9], %[r8]| %[r8], %[r9], %[cl]}\n\t"
        "lea{q (%[r10], %[r8]), %[r8]| %[r8], [%[r10] + %[r8]]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src0]), %[r8]| [%[src0] - 8], %[r8]}\n\t"
        "shrx{q %[tcl], %[r9], %[r9]| %[r9],%[r9], %[tcl]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q %[cx], %[r9]| %[r9], %[cx]}\n\t"

        ".Ldone%=:"

        : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+r"(src1), [cx] "+c"(cx),
          [cl] "+r"(cl), [tcl] "+r"(tcl), [r8] "=r"(r8), [r9] "+r"(r9),
          [r10] "=r"(r10)
        :
        : "cc", "memory");

    WJR_ASSUME(cx == 0);

    return r9;
}

#undef WJR_adcsbb
#undef WJR_addcsubc

#undef WJR_ADDSUB_I