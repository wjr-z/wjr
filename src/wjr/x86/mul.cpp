#include <cstddef>
#include <cstdint>

#include <wjr/preprocessor.hpp>

namespace wjr {

void __asm_basecase_mul_s_impl(uint64_t *dst, const uint64_t *src0, size_t dx,
                               const uint64_t *src1, size_t m) {

    uint64_t r8, r9, r10, r11;
    uint64_t cx;
    uint64_t ax; // ax = dx & 7

#if !WJR_HAS_FEATURE(INLINE_ASM_GOTO_OUTPUT)
    if (WJR_UNLIKELY(dx == 1)) {
        dx = src1[0];
        asm volatile("mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
                     : [r8] "=r"(r8), [r9] "=r"(r9)
                     : "d"(dx), [src0] "r"(src0)
                     : "memory");
        dst[0] = r8;
        dst[1] = r9;
        return;
    }
#else
    asm volatile goto(
        "cmp{q $2, %[dx]| %[dx], 2}\n\t"
        "ja %l[LARGE]\n\t"
        "mov{q (%[src1]), %[dx]| %[dx], [%[src1]]}\n\t"
        "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
        "je %l[N2]"
        : [r8] "=&r"(r8), [r9] "=&r"(r9), [dx] "+&r"(dx)
        : [src0] "r"(src0), [src1] "r"(src1)
        : "cc", "memory"
        : LARGE, N2);

    dst[0] = r8;
    dst[1] = r9;

    return;

N2:
    asm volatile("mulx{q 8(%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0] + 8]}\n\t"
                 : [r10] "=r"(r10), [r11] "=r"(r11)
                 : "d"(dx), [src0] "r"(src0)
                 : "memory");

    if (WJR_LIKELY(m == 1)) {
        asm volatile(
            "add{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
            "adc{q $0x0, %[r11]| %[r11], 0}\n\t"
            "mov{q %[r8], (%[dst])| [%[dst] ], %[r8]}\n\t"
            "mov{q %[r10], 8(%[dst])| [%[dst] + 8], %[r10]}\n\t"
            "mov{q %[r11], 16(%[dst])| [%[dst] + 16], %[r11]}\n\t"
            : [r10] "+&r"(r10), [r11] "+&r"(r11)
            :
            [r8] "r"(r8), [r9] "r"(r9), [dst] "r"(dst), [src0] "r"(src0), [src1] "r"(src1)
            : "cc", "memory");

        return;
    }

    asm volatile(
        "add{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "adc{q $0x0, %[r11]| %[r11], 0}\n\t"
        "mov{q 8(%[src1]), %[dx]| %[dx], [%[src1] + 8]}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst] ], %[r8]}\n\t"
        "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
        "mulx{q 8(%[src0]), %[ax], %[dx]| %[dx], %[ax], [%[src0] + 8]}\n\t"
        "add{q %[r9], %[ax]| %[ax], %[r9]}\n\t"
        "adc{q $0x0, %[dx]| %[dx], 0}\n\t"
        "add{q %[r10], %[r8]| %[r8], %[r10]}\n\t"
        "adc{q %[r11], %[ax]| %[ax], %[r11]}\n\t"
        "adc{q $0x0, %[dx]| %[dx], 0}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"
        "mov{q %[ax], 16(%[dst])| [%[dst] + 16], %[ax]}\n\t"
        "mov{q %[dx], 24(%[dst])| [%[dst] + 24], %[dx]}\n\t"
        : [r8] "+&r"(r8), [r9] "+&r"(r9), [r10] "+&r"(r10), [r11] "+&r"(r11),
          [dx] "=&d"(dx), [ax] "=&r"(ax)
        : [dst] "r"(dst), [src0] "r"(src0), [src1] "r"(src1)
        : "cc", "memory");

    return;

LARGE:
#endif

    uint64_t adj; // adj = ((n + 2) & 7) - (n + 2)
    uint64_t rbp; // rbp = n / 8

    asm volatile(
        "lea{q 2(%[dx]), %[adj]| %[adj], [%[dx] + 2]}\n\t"
        "mov{q %[dx], %[rbp]| %[rbp], %[dx]}\n\t"
        "mov{l %k[dx], %k[ax]| %k[ax], %k[dx]}\n\t"
        "and{q $0xfffffffffffffff8, %[adj]| %[adj], 0xfffffffffffffff8}\n\t"
        "shr{q $3, %[rbp]| %[rbp], 3}\n\t"
        "neg %[adj]\n\t"
        "and{l $0x7, %k[ax]| %k[ax], 7}\n\t"
        "mov{q %[rbp], %[cx]| %[cx], %[rbp]}\n\t"

        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "mov{q (%[src1]), %[dx]| %[dx], [%[src1]]}\n\t"
        "movs{lq (%[r9], %[ax], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[ax] * 4]}\n\t"
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
        "inc %[cx]\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
        "lea{q -40(%[src0]), %[src0]| %[src0], [%[src0] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx{q (%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0]]}\n\t"
        "lea{q -32(%[src0]), %[src0]| %[src0], [%[src0] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
        "lea{q -24(%[src0]), %[src0]| %[src0], [%[src0] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx{q (%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0]]}\n\t"
        "lea{q -16(%[src0]), %[src0]| %[src0], [%[src0] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
        "lea{q -8(%[src0]), %[src0]| %[src0], [%[src0] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb7%=\n\t"

        ".Ll1%=:\n\t"
        "mulx{q (%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0]]}\n\t"
        "lea{q 8(%[src0]), %[src0]| %[src0], [%[src0] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"

        ".align 32\n\t"
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

        "dec %[cx]\n\t"
        "jne .Lloop%=\n\t"

        "adc{q %[cx], %[r9]| %[r9], %[cx]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        "dec %[m]\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"
        "jz .LDone%=\n\t"

        "lea{q (%[src0], %[adj], 8), %[src0]| %[src0], [%[src0] + %[adj] * 8]}\n\t"
        "mov{q %[rbp], %[cx]| %[cx], %[rbp]}\n\t"
        "mov{q 8(%[src1]), %[dx]| %[dx], [%[src1] + 8]}\n\t"
        "lea{q 8(%[src1]), %[src1]| %[src1], [%[src1] + 8]}\n\t"

        "lea{q| %[r9], [rip +} .LLookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[ax], 4), %[ax]|xd %[ax], DWORD PTR [%[r9] + %[ax] * 4]}\n\t"
        "lea{q (%[r9], %[ax], 1), %[ax]| %[ax], [%[r9] + %[ax]]}\n\t"
        "jmp{q *%[ax]| %[ax]}\n\t"

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

        ".align 32\n\t"
        ".LLoop%=:\n\t"

        ".LB1%=:\n\t"
        "mulx{q (%[src0]), %[r10], %[r11]| %[r11], %[r10], [%[src0]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        ".LB0%=:\n\t"
        "mulx{q 8(%[src0]), %[r8], %[r9]| %[r9], %[r8], [%[src0] + 8]}\n\t"
        "lea{q -1(%[cx]), %[cx]| %[cx], [%[cx] - 1]}\n\t"
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
        "adox{q %[cx], %[r9]| %[r9], %[cx]}\n\t"
        "adc{q %[cx], %[r9]| %[r9], %[cx]}\n\t"

        "dec %[m]\n\t"
        "mov{q %[r9], (%[dst])| [%[dst]], %[r9]}\n\t"
        "jz .LDone%=\n\t"

        "lea{q (%[src0], %[adj], 8), %[src0]| %[src0], [%[src0] + %[adj] * 8]}\n\t"
        "mov{q %[rbp], %[cx]| %[cx], %[rbp]}\n\t"
        "mov{q 8(%[src1]), %[dx]| %[dx], [%[src1] + 8]}\n\t"
        "lea{q 8(%[src1]), %[src1]| %[src1], [%[src1] + 8]}\n\t"
        "jmp{q *%[ax]| %[ax]}\n\t"

        ".LDone%=:"

        : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+r"(src1), [m] "+r"(m),
          [dx] "+d"(dx), [cx] "=c"(cx), [r8] "=r"(r8), [r9] "=r"(r9), [r10] "=r"(r10),
          [r11] "=r"(r11), [ax] "=r"(ax), [adj] "=r"(adj), [rbp] "=r"(rbp)
        :
        : "cc", "memory");

    return;
}

} // namespace wjr