#include <wjr/arch/x86/biginteger/detail/mul.hpp>

namespace wjr {

#if WJR_HAS_BUILTIN(ASM_MUL_1) == 1

uint64_t __wjr_asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t rdx) noexcept {
    size_t rcx = n / 8;
    uint64_t r8, r9, r10 = n, r11;

    asm volatile(
        "and{l $7, %k[r10]| %k[r10], 7}\n\t"
        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + "
        "%[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r10] + %[r9]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"

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
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -48(%[src]), %[src]| %[src], [%[src] - 48]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -40(%[src]), %[src]| %[src], [%[src] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -32(%[src]), %[src]| %[src], [%[src] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -24(%[src]), %[src]| %[src], [%[src] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld1%=:\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 8(%[src]), %[src]| %[src], [%[src] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"
        "jrcxz .Ld1%=\n\t"

        ".align 16\n\t"
        ".Lloop%=:\n\t"

        ".Lb1%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb0%=:\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb7%=:\n\t"
        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb6%=:\n\t"
        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb5%=:\n\t"
        "mulx{q 32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 32]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb4%=:\n\t"
        "mulx{q 40(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 40]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb3%=:\n\t"
        "mulx{q 48(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 48]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb2%=:\n\t"
        "mulx{q 56(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 56]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "dec %[rcx]\n\t"
        "jne .Lloop%=\n\t"

        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        ".Ldone%=:\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        : [dst] "+&r"(dst), [src] "+&r"(src), [rcx] "+&c"(rcx), [r8] "=&r"(r8), [r9] "=&r"(r9),
          [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(rdx)
        : "cc", "memory");

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1) == 1

uint64_t __wjr_asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t rdx) noexcept {
    size_t rcx = n / 8;
    uint64_t r8, r9, r10 = n, r11;

    asm volatile(
        "and{l $7, %k[r10]| %k[r10], 7}\n\t"
        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + "
        "%[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r10] + %[r9]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"

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
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q 16(%[src]), %[src]| %[src], [%[src] + 16]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 24(%[src]), %[src]| %[src], [%[src] + 24]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q 32(%[src]), %[src]| %[src], [%[src] + 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 40(%[src]), %[src]| %[src], [%[src] + 40]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld1%=:\n\t"
        "add{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 8(%[src]), %[src]| %[src], [%[src] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"
        "jrcxz .Ld1%=\n\t"

        ".align 16\n\t"
        ".Lloop%=:\n\t"

        ".Lb1%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        ".Lb0%=:\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        ".Lb7%=:\n\t"
        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        ".Lb6%=:\n\t"
        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        ".Lb5%=:\n\t"
        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        ".Lb4%=:\n\t"
        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        ".Lb3%=:\n\t"
        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        ".Lb2%=:\n\t"
        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Lloop_out%=\n\t"
        "jmp .Lloop%=\n\t"
        ".Lloop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"

        ".Ldone%=:"

        : [dst] "+&r"(dst), [src] "+&r"(src), [rcx] "+&c"(rcx), [r8] "=&r"(r8), [r9] "=&r"(r9),
          [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(rdx)
        : "cc", "memory");

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1) == 1

uint64_t __wjr_asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t rdx) noexcept {
    size_t rcx = n / 8;
    uint64_t r8, r9, r10 = static_cast<uint32_t>(n), r11;

    asm volatile(
        // set CF = 1, OF = 0
        "and{l $7, %k[r10]| %k[r10], 7}\n\t"
        "stc\n\t"

        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[r10] * "
        "4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r10] + %[r9]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"

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
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q 16(%[src]), %[src]| %[src], [%[src] + 16]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 24(%[src]), %[src]| %[src], [%[src] + 24]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q 32(%[src]), %[src]| %[src], [%[src] + 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q 40(%[src]), %[src]| %[src], [%[src] + 40]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld1%=:\n\t"
        "adc{q (%[dst]), %[r8]| %[r8], [%[dst]]}\n\t"
        "sbb{q $-1, %[r9]| %[r9], -1}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "not %[r8]\n\t"
        "jrcxz .Ld1%=\n\t"
        "lea{q 8(%[src]), %[src]| %[src], [%[src] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"

        ".align 16\n\t"
        ".Lloop%=:\n\t"

        ".Lb1%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        ".Lb0%=:\n\t"
        "not %[r10]\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        ".Lb7%=:\n\t"
        "not %[r8]\n\t"
        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        ".Lb6%=:\n\t"
        "not %[r10]\n\t"
        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        ".Lb5%=:\n\t"
        "not %[r8]\n\t"
        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        ".Lb4%=:\n\t"
        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "not %[r10]\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        ".Lb3%=:\n\t"
        "not %[r8]\n\t"
        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        ".Lb2%=:\n\t"
        "not %[r10]\n\t"
        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "not %[r8]\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Lloop_out%=\n\t"
        "jmp .Lloop%=\n\t"
        ".Lloop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "sbb{q $-1, %[r9]| %[r9], -1}\n\t"

        ".Ldone%=:"

        : [dst] "+&r"(dst), [src] "+&r"(src), [rcx] "+&c"(rcx), [r8] "=&r"(r8), [r9] "=&r"(r9),
          [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(rdx)
        : "cc", "memory");

    WJR_ASSUME(rcx == 0);

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDLSH_N) == 1
    #define WJR_ADDSUB_I 1
    #include "gen_addrsblsh_n.hpp"
#endif

#if WJR_HAS_BUILTIN(ASM_RSBLSH_N) == 1
    #define WJR_ADDSUB_I 0
    #include "gen_addrsblsh_n.hpp"
#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S) == 1

void __wjr_asm_basecase_mul_s_impl(uint64_t *dst, const uint64_t *src0, size_t rdx,
                                   const uint64_t *src1, size_t m) noexcept {
    uint64_t r8, r9, r10, r11;
    uint64_t rax, rcx; // rax = rdx & 7

    if (WJR_LIKELY(rdx <= 2)) {
        if (WJR_LIKELY(rdx != 2)) {
            rdx = src1[0];
            asm volatile("mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
                         : [r8] "=r"(r8), [r9] "=r"(r9)
                         : "d"(rdx), [src0] "r"(src0)
                         : "memory");
            dst[0] = r8;
            dst[1] = r9;
            return;
        }

        rdx = src1[0];
        asm volatile(
            "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
            "mulx{q 8(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] + 8]}\n\t"
            : [r8] "=&r"(r8), [r9] "=&r"(r9), [r10] "=r"(r10), [r11] "=r"(r11)
            : "d"(rdx), [src0] "r"(src0)
            : "memory");

        if (WJR_LIKELY(m == 1)) {
            asm volatile(
                "add{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
                "adc{q $0, %[r11]| %[r11], 0}\n\t"
                "mov{q %[r8], (%[dst])| [%[dst] ], %[r8]}\n\t"
                "mov{q %[r10], 8(%[dst])| [%[dst] + 8], %[r10]}\n\t"
                "mov{q %[r11], 16(%[dst])| [%[dst] + 16], %[r11]}\n\t"
                : [r10] "+&r"(r10), [r11] "+&r"(r11)
                : [r8] "r"(r8), [r9] "r"(r9), [dst] "r"(dst), [src0] "r"(src0), [src1] "r"(src1)
                : "cc", "memory");

            return;
        }

        asm volatile(
            "add{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
            "adc{q $0, %[r11]| %[r11], 0}\n\t"
            "mov{q 8(%[src1]), %[rdx]| %[rdx], [%[src1] + 8]}\n\t"
            "mov{q %[r8], (%[dst])| [%[dst] ], %[r8]}\n\t"
            "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
            "mulx{q 8(%[src0]), %[rax], %[rdx]| %[rdx], %[rax], [%[src0] + 8]}\n\t"
            "add{q %[r9], %[rax]| %[rax], %[r9]}\n\t"
            "adc{q $0, %[rdx]| %[rdx], 0}\n\t"
            "add{q %[r10], %[r8]| %[r8], %[r10]}\n\t"
            "adc{q %[r11], %[rax]| %[rax], %[r11]}\n\t"
            "adc{q $0, %[rdx]| %[rdx], 0}\n\t"
            "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"
            "mov{q %[rax], 16(%[dst])| [%[dst] + 16], %[rax]}\n\t"
            "mov{q %[rdx], 24(%[dst])| [%[dst] + 24], %[rdx]}\n\t"
            : [r8] "+&r"(r8), [r9] "+&r"(r9), [r10] "+&r"(r10), [r11] "+&r"(r11), [rdx] "=&d"(rdx),
              [rax] "=&r"(rax)
            : [dst] "r"(dst), [src0] "r"(src0), [src1] "r"(src1)
            : "cc", "memory");

        return;
    }

    uint64_t adj; // adj = ((n + 2) & 7) - (n + 2)
    uint64_t rbp; // rbp = n / 8

    asm volatile(
        "lea{q 2(%[rdx]), %[adj]| %[adj], [%[rdx] + 2]}\n\t"
        "mov{q %[rdx], %[rbp]| %[rbp], %[rdx]}\n\t"
        "mov{l %k[rdx], %k[rax]| %k[rax], %k[rdx]}\n\t"
        "and{q $0xfffffffffffffff8, %[adj]| %[adj], 0xfffffffffffffff8}\n\t"
        "shr{q $3, %[rbp]| %[rbp], 3}\n\t"
        "neg %[adj]\n\t"
        "and{l $7, %k[rax]| %k[rax], 7}\n\t"
        "mov{q %[rbp], %[rcx]| %[rcx], %[rbp]}\n\t"

        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "mov{q (%[src1]), %[rdx]| %[rdx], [%[src1]]}\n\t"
        "movs{lq (%[r9], %[rax], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[rax] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r9]| %[r9], [%[r9] + %[r10]]}\n\t"
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
        "mulx{q (%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx{q (%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0]]}\n\t"
        "lea{q -48(%[src0]), %[src0]| %[src0], [%[src0] - 48]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
        "lea{q -40(%[src0]), %[src0]| %[src0], [%[src0] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx{q (%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0]]}\n\t"
        "lea{q -32(%[src0]), %[src0]| %[src0], [%[src0] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
        "lea{q -24(%[src0]), %[src0]| %[src0], [%[src0] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx{q (%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0]]}\n\t"
        "lea{q -16(%[src0]), %[src0]| %[src0], [%[src0] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
        "lea{q -8(%[src0]), %[src0]| %[src0], [%[src0] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb7%=\n\t"

        ".Ll1%=:\n\t"
        "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
        "lea{q 8(%[src0]), %[src0]| %[src0], [%[src0] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"

        ".align 16\n\t"
        ".Lloop%=:\n\t"

        ".Lb1%=:\n\t"
        "mulx{q (%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0]]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb0%=:\n\t"
        "mulx{q 8(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] + 8]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb7%=:\n\t"
        "mulx{q 16(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] + 16]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb6%=:\n\t"
        "mulx{q 24(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] + 24]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb5%=:\n\t"
        "mulx{q 32(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] + 32]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb4%=:\n\t"
        "mulx{q 40(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] + 40]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb3%=:\n\t"
        "mulx{q 48(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] + 48]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb2%=:\n\t"
        "mulx{q 56(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] + 56]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "dec %[rcx]\n\t"
        "jne .Lloop%=\n\t"

        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        "dec %[m]\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"
        "jz .LDone%=\n\t"

        "lea{q (%[src0], %[adj], 8), %[src0]| %[src0], [%[src0] + %[adj] * 8]}\n\t"
        "mov{q %[rbp], %[rcx]| %[rcx], %[rbp]}\n\t"
        "mov{q 8(%[src1]), %[rdx]| %[rdx], [%[src1] + 8]}\n\t"
        "lea{q 8(%[src1]), %[src1]| %[src1], [%[src1] + 8]}\n\t"

        "lea{q| %[r9], [rip +} .LLookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[rax], 4), %[rax]|xd %[rax], DWORD PTR [%[r9] + %[rax] * 4]}\n\t"
        "lea{q (%[r9], %[rax], 1), %[rax]| %[rax], [%[r9] + %[rax]]}\n\t"
        "jmp{q *%[rax]| %[rax]}\n\t"

        ".align 8\n\t"
        ".LLookup%=:\n\t"
        ".long .LL0%=-.LLookup%=\n\t"
        ".long .LL1%=-.LLookup%=\n\t"
        ".long .LL2%=-.LLookup%=\n\t"
        ".long .LL3%=-.LLookup%=\n\t"
        ".long .LL4%=-.LLookup%=\n\t"
        ".long .LL5%=-.LLookup%=\n\t"
        ".long .LL6%=-.LLookup%=\n\t"
        ".long .LL7%=-.LLookup%=\n\t"
        ".align 16\n\t"

        ".LL0%=:\n\t"
        "mulx{q (%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0]]}\n\t"
        "lea{q 8(%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8 + 8]}\n\t"
        "jmp .LB0%=\n\t"

        ".LL2%=:\n\t"
        "mulx{q -16(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] - 16]}\n\t"
        "lea{q -56(%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8 - 56]}\n\t"
        "jmp .LB2%=\n\t"

        ".LL3%=:\n\t"
        "mulx{q -24(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] - 24]}\n\t"
        "lea{q -56(%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8 - 56]}\n\t"
        "jmp .LB3%=\n\t"

        ".LL4%=:\n\t"
        "mulx{q -32(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] - 32]}\n\t"
        "lea{q -56(%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8 - 56]}\n\t"
        "jmp .LB4%=\n\t"

        ".LL5%=:\n\t"
        "mulx{q -40(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] - 40]}\n\t"
        "lea{q -56(%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8 - 56]}\n\t"
        "jmp .LB5%=\n\t"

        ".LL6%=:\n\t"
        "mulx{q 16(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] + 16]}\n\t"
        "lea{q 8(%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8 + 8]}\n\t"
        "jmp .LB6%=\n\t"

        ".LL7%=:\n\t"
        "mulx{q 8(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] + 8]}\n\t"
        "lea{q 8(%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8 + 8]}\n\t"
        "jmp .LB7%=\n\t"

        ".LL1%=:\n\t"
        "mulx{q -8(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] - 8]}\n\t"
        "lea{q 8(%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8 + 8]}\n\t"

        ".align 16\n\t"
        ".LLoop%=:\n\t"

        ".LB1%=:\n\t"
        "mulx{q (%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        ".LB0%=:\n\t"
        "mulx{q 8(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        ".LB7%=:\n\t"
        "mulx{q 16(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        ".LB6%=:\n\t"
        "mulx{q 24(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] + 24]}\n\t"
        "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        ".LB5%=:\n\t"
        "mulx{q -32(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        ".LB4%=:\n\t"
        "mulx{q -24(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        ".LB3%=:\n\t"
        "mulx{q -16(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        ".LB2%=:\n\t"
        "mulx{q -8(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .LLoop_out%=\n\t"
        "jmp .LLoop%=\n\t"
        ".LLoop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"

        "dec %[m]\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"
        "jz .LDone%=\n\t"

        "lea{q (%[src0], %[adj], 8), %[src0]| %[src0], [%[src0] + %[adj] * 8]}\n\t"
        "mov{q %[rbp], %[rcx]| %[rcx], %[rbp]}\n\t"
        "mov{q 8(%[src1]), %[rdx]| %[rdx], [%[src1] + 8]}\n\t"
        "lea{q 8(%[src1]), %[src1]| %[src1], [%[src1] + 8]}\n\t"
        "jmp{q *%[rax]| %[rax]}\n\t"

        ".LDone%=:"

        : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+r"(src1), [m] "+r"(m), [rdx] "+d"(rdx),
          [rcx] "=c"(rcx), [r8] "=r"(r8), [r9] "=r"(r9), [r10] "=r"(r10), [r11] "=r"(r11),
          [rax] "=r"(rax), [adj] "=r"(adj), [rbp] "=r"(rbp)
        :
        : "cc", "memory");
}

#endif // WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)

#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR) == 1

// Local testing is slower than GMP by 2% to 3%
void __wjr_asm_basecase_sqr_impl(uint64_t *dst, const uint64_t *src, size_t rdx) noexcept {
    uint64_t r8, r9, r10, r11;
    uint64_t rax, rcx;

    if (WJR_LIKELY(rdx <= 2)) {
        if (WJR_LIKELY(rdx != 2)) {
            rdx = src[0];
            asm volatile("mulx{q %[rdx], %[r8], %[r9]| %[r9], %[r8], %[rdx]}\n\t"
                         : [r8] "=r"(r8), [r9] "=r"(r9)
                         : [rdx] "d"(rdx)
                         : "memory");
            dst[0] = r8;
            dst[1] = r9;
            return;
        }

        rdx = src[0];
        asm volatile(
            "mulx{q %[rdx], %[r8], %[r9]| %[r9], %[r8], %[rdx]}\n\t"
            "mov{q 8(%[src]), %[rcx]| %[rcx], [%[src] + 8]}\n\t"
            "mulx{q %[rcx], %[r10], %[r11]| %[r11], %[r10], %[rcx]}\n\t"
            "mov{q %[rcx], %[rdx]| %[rdx], %[rcx]}\n\t"
            "mulx{q %[rdx], %[rax], %[rdx]| %[rdx], %[rax], %[rdx]}\n\t"
            "add{q %[r10], %[r10]| %[r10], %[r10]}\n\t"
            "adc{q %[r11], %[r11]| %[r11], %[r11]}\n\t"
            "adc{q $0, %[rdx]| %[rdx], 0}\n\t"
            "add{q %[r10], %[r9]| %[r9], %[r10]}\n\t"
            "adc{q %[r11], %[rax]| %[rax], %[r11]}\n\t"
            "adc{q $0, %[rdx]| %[rdx], 0}\n\t"
            "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
            "mov{q %[r9], 8(%[dst])| [%[dst] + 8], %[r9]}\n\t"
            "mov{q %[rax], 16(%[dst])| [%[dst] + 16], %[rax]}\n\t"
            "mov{q %[rdx], 24(%[dst])| [%[dst] + 24], %[rdx]}\n\t"
            : [r8] "=&r"(r8), [r9] "=&r"(r9), [r10] "=&r"(r10), [r11] "=&r"(r11), [rax] "=&r"(rax),
              [rdx] "+&d"(rdx), [rcx] "=&r"(rcx)
            : [dst] "r"(dst), [src] "r"(src)
            : "cc", "memory");

        return;
    }

    uint64_t rbp;
    uint64_t adj;

    asm volatile(
        "lea{q -0x1(%[rdx]), %[rbp]| %[rbp], [%[rdx] - 1]}\n\t"

        "mov{l %k[rbp], %k[r8]| %k[r8], %k[rbp]}\n\t"
        "mov{q %[rbp], %[rax]| %[rax], %[rbp]}\n\t"
        "shr{q $3, %[rax]| %[rax], 3}\n\t"
        "and{l $7, %k[r8]| %k[r8], 7}\n\t"
        "mov{l %k[r8], %k[adj]| %k[adj], %k[r8]}\n\t"

        "mov{q %[rax], %[rcx]| %[rcx], %[rax]}\n\t"

        "mov{q (%[src]), %[rdx]| %[rdx], [%[src]]}\n\t"
        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r8], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[r8] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r9]| %[r9], [%[r9] + %[r10]]}\n\t"
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
        "mulx{q 8(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 8]}\n\t"
        "lea{q 0x8(%[src]), %[src]| %[src], [%[src] + 8]}\n\t"
        "lea{q 0x8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx{q 8(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 8]}\n\t"
        "lea{q -40(%[src]), %[src]| %[src], [%[src] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -32(%[src]), %[src]| %[src], [%[src] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx{q 8(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 8]}\n\t"
        "lea{q -24(%[src]), %[src]| %[src], [%[src] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx{q 8(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 8]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb7%=\n\t"

        ".Ll1%=:\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q 16(%[src]), %[src]| %[src], [%[src] + 16]}\n\t"
        "lea{q 16(%[dst]), %[dst]| %[dst], [%[dst] + 16]}\n\t"

        ".align 16\n\t"
        ".Lloop%=:\n\t"

        ".Lb1%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb0%=:\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb7%=:\n\t"
        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb6%=:\n\t"
        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb5%=:\n\t"
        "mulx{q 32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 32]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb4%=:\n\t"
        "mulx{q 40(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 40]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb3%=:\n\t"
        "mulx{q 48(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 48]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb2%=:\n\t"
        "mulx{q 56(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 56]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "dec %[rcx]\n\t"
        "jne .Lloop%=\n\t"

        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        "lea{q| %[r9], [rip +} .Lslookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[adj], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[adj] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r9]| %[r9], [%[r9] + %[r10]]}\n\t"
        "jmp{q *%[r9]| %[r9]}\n\t"

        // (n - 1) & 7
        ".align 8\n\t"
        ".Lslookup%=:\n\t"
        ".long .Ljs0%=-.Lslookup%=\n\t"
        ".long .Ljs1%=-.Lslookup%=\n\t"
        ".long .Ljs2%=-.Lslookup%=\n\t"
        ".long .Ljs3%=-.Lslookup%=\n\t"
        ".long .Ljs4%=-.Lslookup%=\n\t"
        ".long .Ljs5%=-.Lslookup%=\n\t"
        ".long .Ljs6%=-.Lslookup%=\n\t"
        ".long .Ljs7%=-.Lslookup%=\n\t"
        ".align 16\n\t"

        ".Ljs0%=:\n\t"
        "lea{q -2(%[rbp]), %[adj]| %[adj], [%[rbp] - 2]}\n\t"
        "not %[adj]\n\t"
        "jmp .Ls7%=\n\t"

        ".Ljs1%=:\n\t"
        "lea{q -3(%[rbp]), %[adj]| %[adj], [%[rbp] - 3]}\n\t"
        "not %[adj]\n\t"
        "jmp .Ls0%=\n\t"

        ".Ljs2%=:\n\t"
        "lea{q 4(%[rbp]), %[adj]| %[adj], [%[rbp] + 4]}\n\t"
        "not %[adj]\n\t"
        "jmp .Ls1%=\n\t"

        ".Ljs3%=:\n\t"
        "lea{q 3(%[rbp]), %[adj]| %[adj], [%[rbp] + 3]}\n\t"
        "not %[adj]\n\t"
        "jmp .Ls2%=\n\t"

        ".Ljs4%=:\n\t"
        "lea{q 2(%[rbp]), %[adj]| %[adj], [%[rbp] + 2]}\n\t"
        "not %[adj]\n\t"
        "jmp .Ls3%=\n\t"

        ".Ljs5%=:\n\t"
        "lea{q 1(%[rbp]), %[adj]| %[adj], [%[rbp] + 1]}\n\t"
        "not %[adj]\n\t"
        "jmp .Ls4%=\n\t"

        ".Ljs6%=:\n\t"
        "lea{q (%[rbp]), %[adj]| %[adj], [%[rbp]]}\n\t"
        "not %[adj]\n\t"
        "jmp .Ls5%=\n\t"

        ".Ljs7%=:\n\t"
        "lea{q -1(%[rbp]), %[adj]| %[adj], [%[rbp] - 1]}\n\t"
        "not %[adj]\n\t"
        "jmp .Ls6%=\n\t"

        ".align 16\n\t"
        ".Lsloop%=:\n\t"

        "lea{q 8(%[adj]), %[adj]| %[adj], [%[adj] + 8]}\n\t"

        ".Ls0%=:\n\t"

        "lea{q -8(%[src], %[adj], 8), %[src]| %[src], [%[src] + %[adj] * 8 - 8]}\n\t"
        "lea{q (%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8]}\n\t"
        "mov{q -8(%[src]), %[rdx]| %[rdx], [%[src] - 8]}\n\t"
        "mov{q %[rax], %[rcx]| %[rcx], %[rax]}\n\t"

        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "jmp .Ls0b0%=\n\t"

        ".align 16\n\t"
        ".Ls0loop%=:\n\t"

        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        ".Ls0b0%=:\n\t"
        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Ls0loop_out%=\n\t"
        "jmp .Ls0loop%=\n\t"
        ".Ls0loop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Ls7%=:\n\t"

        "lea{q -1(%[rax]), %[rcx]| %[rcx], [%[rax] - 1]}\n\t"
        "lea{q -1(%[rax]), %[rax]| %[rax], [%[rax] - 1]}\n\t"
        "lea{q -8(%[src], %[adj], 8), %[src]| %[src], [%[src] + %[adj] * 8 - 8]}\n\t"
        "lea{q (%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8]}\n\t"
        "mov{q (%[src]), %[rdx]| %[rdx], [%[src]]}\n\t"

        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "jmp .Ls7b7%=\n\t"

        ".align 16\n\t"
        ".Ls7loop%=:\n\t"

        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        ".Ls7b7%=:\n\t"
        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Ls7loop_out%=\n\t"
        "jmp .Ls7loop%=\n\t"
        ".Ls7loop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Ls6%=:\n\t"

        "lea{q -8(%[src], %[adj], 8), %[src]| %[src], [%[src] + %[adj] * 8 - 8]}\n\t"
        "lea{q (%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8]}\n\t"
        "mov{q 8(%[src]), %[rdx]| %[rdx], [%[src] + 8]}\n\t"
        "mov{q %[rax], %[rcx]| %[rcx], %[rax]}\n\t"

        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "jmp .Ls6b6%=\n\t"

        ".align 16\n\t"
        ".Ls6loop%=:\n\t"

        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        ".Ls6b6%=:\n\t"
        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Ls6loop_out%=\n\t"
        "jmp .Ls6loop%=\n\t"
        ".Ls6loop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Ls5%=:\n\t"

        "lea{q 56(%[src], %[adj], 8), %[src]| %[src], [%[src] + %[adj] * 8 + 56]}\n\t"
        "lea{q (%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8]}\n\t"
        "mov{q -48(%[src]), %[rdx]| %[rdx], [%[src] - 48]}\n\t"
        "mov{q %[rax], %[rcx]| %[rcx], %[rax]}\n\t"

        "mulx{q -40(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 40]}\n\t"
        "jmp .Ls5b5%=\n\t"

        ".align 16\n\t"
        ".Ls5loop%=:\n\t"

        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        ".Ls5b5%=:\n\t"
        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Ls5loop_out%=\n\t"
        "jmp .Ls5loop%=\n\t"
        ".Ls5loop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Ls4%=:\n\t"

        "lea{q 56(%[src], %[adj], 8), %[src]| %[src], [%[src] + %[adj] * 8 + 56]}\n\t"
        "lea{q (%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8]}\n\t"
        "mov{q -40(%[src]), %[rdx]| %[rdx], [%[src] - 40]}\n\t"
        "mov{q %[rax], %[rcx]| %[rcx], %[rax]}\n\t"

        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "jmp .Ls4b4%=\n\t"

        ".align 16\n\t"
        ".Ls4loop%=:\n\t"

        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        ".Ls4b4%=:\n\t"
        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Ls4loop_out%=\n\t"
        "jmp .Ls4loop%=\n\t"
        ".Ls4loop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Ls3%=:\n\t"

        "lea{q 56(%[src], %[adj], 8), %[src]| %[src], [%[src] + %[adj] * 8 + 56]}\n\t"
        "lea{q (%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8]}\n\t"
        "mov{q -32(%[src]), %[rdx]| %[rdx], [%[src] - 32]}\n\t"
        "mov{q %[rax], %[rcx]| %[rcx], %[rax]}\n\t"

        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "jmp .Ls3b3%=\n\t"

        ".align 16\n\t"
        ".Ls3loop%=:\n\t"

        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        ".Ls3b3%=:\n\t"
        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Ls3loop_out%=\n\t"
        "jmp .Ls3loop%=\n\t"
        ".Ls3loop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Ls2%=:\n\t"

        "lea{q 56(%[src], %[adj], 8), %[src]| %[src], [%[src] + %[adj] * 8 + 56]}\n\t"
        "lea{q (%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8]}\n\t"
        "mov{q -24(%[src]), %[rdx]| %[rdx], [%[src] - 24]}\n\t"
        "mov{q %[rax], %[rcx]| %[rcx], %[rax]}\n\t"

        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "jmp .Ls2b2%=\n\t"

        ".align 16\n\t"
        ".Ls2loop%=:\n\t"

        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        ".Ls2b2%=:\n\t"
        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Ls2loop_out%=\n\t"
        "jmp .Ls2loop%=\n\t"
        ".Ls2loop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Ls1%=:\n\t"

        "lea{q 56(%[src], %[adj], 8), %[src]| %[src], [%[src] + %[adj] * 8 + 56]}\n\t"
        "lea{q 64(%[dst], %[adj], 8), %[dst]| %[dst], [%[dst] + %[adj] * 8 + 64]}\n\t"
        "mov{q -16(%[src]), %[rdx]| %[rdx], [%[src] - 16]}\n\t"
        "mov{q %[rax], %[rcx]| %[rcx], %[rax]}\n\t"

        "jmp .Ls1l1%=\n\t"

        ".Ls1d1%=:\n\t"
        "add{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"
        "jmp .Ls1done%=\n\t"

        ".Ls1l1%=:\n\t"
        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "jrcxz .Ls1d1%=\n\t"

        ".align 16\n\t"
        ".Ls1loop%=:\n\t"

        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        "mulx{q 8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        "mulx{q 16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        "mulx{q 24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] + 24]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        "mulx{q -32(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        "mulx{q -24(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 24]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        "mulx{q -16(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src] - 16]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        "mulx{q -8(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src] - 8]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Ls1loop_out%=\n\t"
        "jmp .Ls1loop%=\n\t"
        ".Ls1loop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"

        ".Ls1done%=:\n\t"

        "test %[rax], %[rax]\n\t"
        "jne .Lsloop%=\n\t"

        // dst' = dst + 2 * n - 2
        // src' = src + n

        "mov{q %[rbp], %[rcx]| %[rcx], %[rbp]}\n\t"
        "neg %[rbp]\n\t"
        "mov{l %k[rcx], %k[r10]| %k[r10], %k[rcx]}\n\t"

        "lea{q -8(%[src], %[rbp], 8), %[src]| %[src], [%[src] + %[rbp] * 8 - 8]}\n\t"
        "shl{q $0x4, %[rbp]| %[rbp], 4}\n\t"
        "add{q %[rbp], %[dst]| %[dst], %[rbp]}\n\t"

        "shr{q $0x3, %[rcx]| %[rcx], 3}\n\t"
        "and{l $0x7, %k[r10]| %k[r10], 7}\n\t"

        "mov{q (%[src]), %[rdx]| %[rdx], [%[src]]}\n\t"
        "lea{q| %[r9], [rip +} .Llslookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r9]| %[r9], [%[r9] + %[r10]]}\n\t"
        "jmp{q *%[r9]| %[r9]}\n\t"

        ".align 8\n\t"
        ".Llslookup%=:\n\t"
        ".long .Llsl0%=-.Llslookup%=\n\t"
        ".long .Llsl1%=-.Llslookup%=\n\t"
        ".long .Llsl2%=-.Llslookup%=\n\t"
        ".long .Llsl3%=-.Llslookup%=\n\t"
        ".long .Llsl4%=-.Llslookup%=\n\t"
        ".long .Llsl5%=-.Llslookup%=\n\t"
        ".long .Llsl6%=-.Llslookup%=\n\t"
        ".long .Llsl7%=-.Llslookup%=\n\t"
        ".align 16\n\t"

        ".Llsl0%=:\n\t"
        "mulx{q %[rdx], %[r10], %[r9]| %[r9], %[r10], %[rdx]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "mov{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r8], %[r8]| %[r8], %[r8]}\n\t"
        "jmp .Llsb0%=\n\t"

        ".Llsl2%=:\n\t"
        "mulx{q %[rdx], %[r10], %[r9]| %[r9], %[r10], %[rdx]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "mov{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r8], %[r8]| %[r8], %[r8]}\n\t"
        "lea{q -48(%[src]), %[src]| %[src], [%[src] - 48]}\n\t"
        "lea{q -96(%[dst]), %[dst]| %[dst], [%[dst] - 96]}\n\t"
        "jmp .Llsb2%=\n\t"

        ".Llsl3%=:\n\t"
        "mulx{q %[rdx], %[r10], %[r11]| %[r11], %[r10], %[rdx]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "mov{q 8(%[dst]), %[rbp]| %[rbp], [%[dst] + 8]}\n\t"
        "adox{q %[rbp], %[rbp]| %[rbp], %[rbp]}\n\t"
        "lea{q -40(%[src]), %[src]| %[src], [%[src] - 40]}\n\t"
        "lea{q -80(%[dst]), %[dst]| %[dst], [%[dst] - 80]}\n\t"
        "jmp .Llsb3%=\n\t"

        ".Llsl4%=:\n\t"
        "mulx{q %[rdx], %[r10], %[r9]| %[r9], %[r10], %[rdx]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "mov{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r8], %[r8]| %[r8], %[r8]}\n\t"
        "lea{q -32(%[src]), %[src]| %[src], [%[src] - 32]}\n\t"
        "lea{q -64(%[dst]), %[dst]| %[dst], [%[dst] - 64]}\n\t"
        "jmp .Llsb4%=\n\t"

        ".Llsl5%=:\n\t"
        "mulx{q %[rdx], %[r10], %[r11]| %[r11], %[r10], %[rdx]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "mov{q 8(%[dst]), %[rbp]| %[rbp], [%[dst] + 8]}\n\t"
        "adox{q %[rbp], %[rbp]| %[rbp], %[rbp]}\n\t"
        "lea{q -24(%[src]), %[src]| %[src], [%[src] - 24]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "jmp .Llsb5%=\n\t"

        ".Llsl6%=:\n\t"
        "mulx{q %[rdx], %[r10], %[r9]| %[r9], %[r10], %[rdx]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "mov{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r8], %[r8]| %[r8], %[r8]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Llsb6%=\n\t"

        ".Llsl7%=:\n\t"
        "mulx{q %[rdx], %[r10], %[r11]| %[r11], %[r10], %[rdx]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "mov{q 8(%[dst]), %[rbp]| %[rbp], [%[dst] + 8]}\n\t"
        "adox{q %[rbp], %[rbp]| %[rbp], %[rbp]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "jmp .Llsb7%=\n\t"

        ".Llsl1%=:\n\t"
        "mulx{q %[rdx], %[r10], %[r11]| %[r11], %[r10], %[rdx]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "mov{q 8(%[dst]), %[rbp]| %[rbp], [%[dst] + 8]}\n\t"
        "adox{q %[rbp], %[rbp]| %[rbp], %[rbp]}\n\t"
        "lea{q 8(%[src]), %[src]| %[src], [%[src] + 8]}\n\t"
        "lea{q 16(%[dst]), %[dst]| %[dst], [%[dst] + 16]}\n\t"

        ".align 16\n\t"
        ".Llsloop%=:\n\t"

        ".Llsb1%=:\n\t"
        "mov{q (%[src]), %[rdx]| %[rdx], [%[src]]}\n\t"
        "mulx{q %[rdx], %[r10], %[r9]| %[r9], %[r10], %[rdx]}\n\t"
        "mov{q (%[dst]), %[rax]| %[rax], [%[dst]]}\n\t"
        "mov{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[rax], %[rax]| %[rax], %[rax]}\n\t"
        "adcx{q %[rbp], %[r11]| %[r11], %[rbp]}\n\t"
        "adox{q %[r8], %[r8]| %[r8], %[r8]}\n\t"
        "adcx{q %[rax], %[r10]| %[r10], %[rax]}\n\t"
        "mov{q %[r11], -8(%[dst])| [%[dst] - 8], %[r11]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        ".Llsb0%=:\n\t"
        "mov{q 8(%[src]), %[rdx]| %[rdx], [%[src] + 8]}\n\t"
        "mulx{q %[rdx], %[r10], %[r11]| %[r11], %[r10], %[rdx]}\n\t"
        "mov{q 16(%[dst]), %[rax]| %[rax], [%[dst] + 16]}\n\t"
        "mov{q 24(%[dst]), %[rbp]| %[rbp], [%[dst] + 24]}\n\t"
        "adox{q %[rax], %[rax]| %[rax], %[rax]}\n\t"
        "adcx{q %[r8], %[r9]| %[r9], %[r8]}\n\t"
        "lea{q -1(%[rcx]), %[rcx]| %[rcx], [%[rcx] - 1]}\n\t"
        "adox{q %[rbp], %[rbp]| %[rbp], %[rbp]}\n\t"
        "adcx{q %[rax], %[r10]| %[r10], %[rax]}\n\t"
        "mov{q %[r9], 8(%[dst])| [%[dst] + 8], %[r9]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        ".Llsb7%=:\n\t"
        "mov{q 16(%[src]), %[rdx]| %[rdx], [%[src] + 16]}\n\t"
        "mulx{q %[rdx], %[r10], %[r9]| %[r9], %[r10], %[rdx]}\n\t"
        "mov{q 32(%[dst]), %[rax]| %[rax], [%[dst] + 32]}\n\t"
        "mov{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[rax], %[rax]| %[rax], %[rax]}\n\t"
        "adcx{q %[rbp], %[r11]| %[r11], %[rbp]}\n\t"
        "adox{q %[r8], %[r8]| %[r8], %[r8]}\n\t"
        "adcx{q %[rax], %[r10]| %[r10], %[rax]}\n\t"
        "mov{q %[r11], 24(%[dst])| [%[dst] + 24], %[r11]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        ".Llsb6%=:\n\t"
        "mov{q 24(%[src]), %[rdx]| %[rdx], [%[src] + 24]}\n\t"
        "mulx{q %[rdx], %[r10], %[r11]| %[r11], %[r10], %[rdx]}\n\t"
        "mov{q 48(%[dst]), %[rax]| %[rax], [%[dst] + 48]}\n\t"
        "mov{q 56(%[dst]), %[rbp]| %[rbp], [%[dst] + 56]}\n\t"
        "adox{q %[rax], %[rax]| %[rax], %[rax]}\n\t"
        "adcx{q %[r8], %[r9]| %[r9], %[r8]}\n\t"
        "adox{q %[rbp], %[rbp]| %[rbp], %[rbp]}\n\t"
        "adcx{q %[rax], %[r10]| %[r10], %[rax]}\n\t"
        "mov{q %[r9], 40(%[dst])| [%[dst] + 40], %[r9]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        ".Llsb5%=:\n\t"
        "mov{q 32(%[src]), %[rdx]| %[rdx], [%[src] + 32]}\n\t"
        "mulx{q %[rdx], %[r10], %[r9]| %[r9], %[r10], %[rdx]}\n\t"
        "mov{q 64(%[dst]), %[rax]| %[rax], [%[dst] + 64]}\n\t"
        "mov{q 72(%[dst]), %[r8]| %[r8], [%[dst] + 72]}\n\t"
        "adox{q %[rax], %[rax]| %[rax], %[rax]}\n\t"
        "adcx{q %[rbp], %[r11]| %[r11], %[rbp]}\n\t"
        "adox{q %[r8], %[r8]| %[r8], %[r8]}\n\t"
        "adcx{q %[rax], %[r10]| %[r10], %[rax]}\n\t"
        "mov{q %[r11], 56(%[dst])| [%[dst] + 56], %[r11]}\n\t"
        "mov{q %[r10], 64(%[dst])| [%[dst] + 64], %[r10]}\n\t"

        ".Llsb4%=:\n\t"
        "mov{q 40(%[src]), %[rdx]| %[rdx], [%[src] + 40]}\n\t"
        "mulx{q %[rdx], %[r10], %[r11]| %[r11], %[r10], %[rdx]}\n\t"
        "mov{q 80(%[dst]), %[rax]| %[rax], [%[dst] + 80]}\n\t"
        "mov{q 88(%[dst]), %[rbp]| %[rbp], [%[dst] + 88]}\n\t"
        "adox{q %[rax], %[rax]| %[rax], %[rax]}\n\t"
        "adcx{q %[r8], %[r9]| %[r9], %[r8]}\n\t"
        "adox{q %[rbp], %[rbp]| %[rbp], %[rbp]}\n\t"
        "adcx{q %[rax], %[r10]| %[r10], %[rax]}\n\t"
        "mov{q %[r9], 72(%[dst])| [%[dst] + 72], %[r9]}\n\t"
        "mov{q %[r10], 80(%[dst])| [%[dst] + 80], %[r10]}\n\t"

        ".Llsb3%=:\n\t"
        "mov{q 48(%[src]), %[rdx]| %[rdx], [%[src] + 48]}\n\t"
        "mulx{q %[rdx], %[r10], %[r9]| %[r9], %[r10], %[rdx]}\n\t"
        "mov{q 96(%[dst]), %[rax]| %[rax], [%[dst] + 96]}\n\t"
        "mov{q 104(%[dst]), %[r8]| %[r8], [%[dst] + 104]}\n\t"
        "adox{q %[rax], %[rax]| %[rax], %[rax]}\n\t"
        "adcx{q %[rbp], %[r11]| %[r11], %[rbp]}\n\t"
        "adox{q %[r8], %[r8]| %[r8], %[r8]}\n\t"
        "adcx{q %[rax], %[r10]| %[r10], %[rax]}\n\t"
        "mov{q %[r11], 88(%[dst])| [%[dst] + 88], %[r11]}\n\t"
        "mov{q %[r10], 96(%[dst])| [%[dst] + 96], %[r10]}\n\t"

        ".Llsb2%=:\n\t"
        "mov{q 56(%[src]), %[rdx]| %[rdx], [%[src] + 56]}\n\t"
        "mulx{q %[rdx], %[r10], %[r11]| %[r11], %[r10], %[rdx]}\n\t"
        "mov{q 112(%[dst]), %[rax]| %[rax], [%[dst] + 112]}\n\t"
        "mov{q 120(%[dst]), %[rbp]| %[rbp], [%[dst] + 120]}\n\t"
        "adox{q %[rax], %[rax]| %[rax], %[rax]}\n\t"
        "adcx{q %[r8], %[r9]| %[r9], %[r8]}\n\t"
        "adox{q %[rbp], %[rbp]| %[rbp], %[rbp]}\n\t"
        "adcx{q %[rax], %[r10]| %[r10], %[rax]}\n\t"
        "mov{q %[r9], 104(%[dst])| [%[dst] + 104], %[r9]}\n\t"
        "mov{q %[r10], 112(%[dst])| [%[dst] + 112], %[r10]}\n\t"

        "lea{q 128(%[dst]), %[dst]| %[dst], [%[dst] + 128]}\n\t"
        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"

        "jrcxz .Llsloop_out%=\n\t"
        "jmp .Llsloop%=\n\t"
        ".Llsloop_out%=:\n\t"

        "mov{q (%[src]), %[rdx]| %[rdx], [%[src]]}\n\t"
        "mulx{q %[rdx], %[r10], %[r9]| %[r9], %[r10], %[rdx]}\n\t"
        "mov{q (%[dst]), %[rax]| %[rax], [%[dst]]}\n\t"
        "adox{q %[rax], %[rax]| %[rax], %[rax]}\n\t"
        "adcx{q %[rbp], %[r11]| %[r11], %[rbp]}\n\t"
        "adcx{q %[rax], %[r10]| %[r10], %[rax]}\n\t"
        "adox{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"
        "adc{q %[rcx], %[r9]| %[r9], %[rcx]}\n\t"

        "mov{q %[r11], -8(%[dst])| [%[dst] - 8], %[r11]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "mov{q %[r9], 8(%[dst])| [%[dst] + 8], %[r9]}"

        : [dst] "+r"(dst), [src] "+r"(src), [rcx] "=c"(rcx), [rdx] "+d"(rdx), [r8] "=r"(r8),
          [r9] "=r"(r9), [r10] "=r"(r10), [r11] "=r"(r11), [rax] "=r"(rax), [rbp] "=r"(rbp),
          [adj] "=r"(adj)
        :
        : "cc", "memory");
}

#endif

} // namespace wjr
