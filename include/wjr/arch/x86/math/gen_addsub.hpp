// WJR_ADDSUB_I :
// 0 : SUB
// 1 : ADD

#include <wjr/assert.hpp>

#ifndef WJR_ADDSUB_I
    #error "abort"
#endif

#if WJR_ADDSUB_I == 1
    #define WJR_addcsubc addc
    #define WJR_adcsbb adc
    #define __WJR_TEST_ASSEMBLY ASM_ADDC_N
#else
    #define WJR_addcsubc subc
    #define WJR_adcsbb sbb
    #define __WJR_TEST_ASSEMBLY ASM_SUBC_N
#endif

#if WJR_HAS_BUILTIN(__WJR_TEST_ASSEMBLY) == 1

inline uint64_t WJR_PP_CONCAT(__wjr_asm_, WJR_PP_CONCAT(WJR_addcsubc, _n_impl))(
    uint64_t *dst, const uint64_t *src0, const uint64_t *src1, size_t n,
    uint64_t c_in) noexcept {
    size_t rcx = n / 8;
    uint64_t r8 = c_in, r9, r10 = n & 7, r11;

    asm volatile(
        "add{b $255, %b[r8]| %b[r8], 255}\n\t"
        "lea{q| %[r9], [rip +} .Llookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r9] + %[r10]]}\n\t"
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
        "inc %[rcx]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mov{q (%[src0]), %[r9]| %[r9], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r11]| %[r11], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r9]| %[r9], [%[src1]]}\n\t"
        "lea{q -32(%[src0]), %[src0]| %[src0], [%[src0] - 32]}\n\t"
        "lea{q -32(%[src1]), %[src1]| %[src1], [%[src1] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mov{q (%[src0]), %[r10]| %[r10], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r9]| %[r9], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r10]| %[r10], [%[src1]]}\n\t"
        "lea{q -24(%[src0]), %[src0]| %[src0], [%[src0] - 24]}\n\t"
        "lea{q -24(%[src1]), %[src1]| %[src1], [%[src1] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mov{q (%[src0]), %[r8]| %[r8], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r10]| %[r10], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r8]| %[r8], [%[src1]]}\n\t"
        "lea{q -16(%[src0]), %[src0]| %[src0], [%[src0] - 16]}\n\t"
        "lea{q -16(%[src1]), %[src1]| %[src1], [%[src1] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[rcx]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mov{q (%[src0]), %[r11]| %[r11], [%[src0]]}\n\t"
        "mov{q 8(%[src0]), %[r8]| %[r8], [%[src0] + 8]}\n\t"
        WJR_PP_STR(WJR_adcsbb) "{q (%[src1]), %[r11]| %[r11], [%[src1]]}\n\t"
        "lea{q -8(%[src0]), %[src0]| %[src0], [%[src0] - 8]}\n\t"
        "lea{q -8(%[src1]), %[src1]| %[src1], [%[src1] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[rcx]\n\t"
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
        ".Lloop%=:\n\t"

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
        "dec %[rcx]\n\t"
        
        "jne .Lloop%=\n\t"

        WJR_PP_STR(WJR_adcsbb) "{q -8(%[src1]), %[r10]| %[r10], [%[src1] - 8]}\n\t"
        "mov{q %[r8], -16(%[dst])| [%[dst] - 16], %[r8]}\n\t"
        "mov{q %[r10], -8(%[dst])| [%[dst] - 8], %[r10]}\n\t"

        ".Ldone%=:\n\t"
        "mov{l %k[rcx], %k[r9]| %k[r9], %k[rcx]}\n\t"
        "adc{l %k[rcx], %k[r9]| %k[r9], %k[rcx]}"

        : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+r"(src1), [rcx] "+c"(rcx), 
          [r8] "+r"(r8), [r9] "=&r"(r9), [r10] "+r"(r10), [r11] "=&r"(r11)
        :
        : "cc", "memory");

    WJR_ASSERT_ASSUME(rcx == 0);
    WJR_ASSERT_ASSUME(r9 <= 1);

    return r9;
}

#else
extern "C" WJR_MS_ABI uint64_t WJR_PP_CONCAT(
    __wjr_asm_, WJR_PP_CONCAT(WJR_addcsubc, _n_impl))(uint64_t *dst, const uint64_t *src0,
                                                      const uint64_t *src1, size_t n,
                                                      uint64_t c_in) noexcept;
#endif

WJR_INTRINSIC_INLINE uint64_t WJR_PP_CONCAT(asm_, WJR_PP_CONCAT(WJR_addcsubc, _n))(
    uint64_t *dst, const uint64_t *src0, const uint64_t *src1, size_t n,
    uint64_t c_in) noexcept {
    if (WJR_BUILTIN_CONSTANT_P(n)) {
        if (n == 1) {
            dst[0] = WJR_PP_CONCAT(asm_, WJR_addcsubc)(src0[0], src1[0], c_in, c_in);
            return c_in;
        }
    }

    return WJR_PP_CONCAT(__wjr_asm_, WJR_PP_CONCAT(WJR_addcsubc, _n_impl))(dst, src0,
                                                                           src1, n, c_in);
}

#undef __WJR_TEST_ASSEMBLY
#undef WJR_adcsbb
#undef WJR_addcsubc

#undef WJR_ADDSUB_I