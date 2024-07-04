#ifndef WJR_X86_MATH_MUL_HPP__
#define WJR_X86_MATH_MUL_HPP__

#include <wjr/assert.hpp>
#include <wjr/x86/math/mul-impl.hpp>

#if WJR_HAS_BUILTIN(MSVC_UMUL128)
#include <wjr/x86/simd/intrin.hpp>
#endif

namespace wjr {

#if WJR_HAS_BUILTIN(UMUL128)

WJR_INTRINSIC_INLINE uint64_t builtin_umul128(uint64_t a, uint64_t b,
                                              uint64_t &hi) noexcept {
#if WJR_HAS_BUILTIN(INT128_UMUL128)
    const __uint128_t x = static_cast<__uint128_t>(a) * b;
    hi = x >> 64;
    return static_cast<uint64_t>(x);
#elif WJR_HAS_BUILTIN(ASM_UMUL128)
    uint64_t lo;
    asm("mul{q %3| %3}\n\t" : "=a,a"(lo), "=d,d"(hi) : "%a,r"(a), "r,a"(b) : "cc");
    return lo;
#else
    return _umul128(a, b, &hi);
#endif
}

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)

#if WJR_HAS_BUILTIN(ASM_MUL_1) == 1

inline uint64_t asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n,
                          uint64_t rdx) noexcept {
    size_t rcx = n / 8;
    uint64_t r8, r9, r10 = n, r11;

    const auto pdst = dst;
    const auto psrc = src;

    (void)(pdst);
    (void)(psrc);

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

        ".align 32\n\t"
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

        : [dst] "+&r"(dst), [src] "+&r"(src), [rcx] "+&c"(rcx), [r8] "=&r"(r8),
          [r9] "=&r"(r9), [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(rdx)
        : "cc", "memory");

    WJR_ASSERT_ASSUME(rcx == 0);
    WJR_ASSERT_ASSUME(dst == pdst + n);
    WJR_ASSERT_ASSUME(src == psrc + n);

    return r9;
}

#else

extern "C" WJR_MS_ABI uint64_t __wjr_asm_mul_1(uint64_t *dst, const uint64_t *src,
                                               size_t n, uint64_t rdx) noexcept;

WJR_INTRINSIC_INLINE uint64_t asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                        uint64_t rdx) noexcept {
    return __wjr_asm_mul_1(dst, src, n, rdx);
}

#endif

#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1) == 1

inline uint64_t asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                             uint64_t rdx) noexcept {
    size_t rcx = n / 8;
    uint64_t r8, r9, r10 = n, r11;

    const auto pdst = dst;
    const auto psrc = src;

    (void)(pdst);
    (void)(psrc);

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

        ".align 32\n\t"
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

        : [dst] "+&r"(dst), [src] "+&r"(src), [rcx] "+&c"(rcx), [r8] "=&r"(r8),
          [r9] "=&r"(r9), [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(rdx)
        : "cc", "memory");

    WJR_ASSERT_ASSUME(rcx == 0);
    WJR_ASSERT_ASSUME(dst == pdst + n);
    WJR_ASSERT_ASSUME(src == psrc + n);

    return r9;
}

#else

extern "C" WJR_MS_ABI uint64_t __wjr_asm_addmul_1(uint64_t *dst, const uint64_t *src,
                                                  size_t n, uint64_t rdx) noexcept;

WJR_INTRINSIC_INLINE uint64_t asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                           uint64_t rdx) noexcept {
    return __wjr_asm_addmul_1(dst, src, n, rdx);
}

#endif

#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S) == 1
extern void __wjr_asm_basecase_mul_s_impl(uint64_t *dst, const uint64_t *src0, size_t rdx,
                                          const uint64_t *src1, size_t m) noexcept;
#else
extern "C" WJR_MS_ABI void __wjr_asm_basecase_mul_s_impl(uint64_t *dst,
                                                         const uint64_t *src0, size_t rdx,
                                                         const uint64_t *src1,
                                                         size_t m) noexcept;
#endif

inline void asm_basecase_mul_s(uint64_t *dst, const uint64_t *src0, size_t n,
                               const uint64_t *src1, size_t m) noexcept {
    WJR_ASSERT(n >= m);
    WJR_ASSERT(m >= 1);
    __wjr_asm_basecase_mul_s_impl(dst, src0, n, src1, m);
}

#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR)

#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR) == 1
extern void __wjr_asm_basecase_sqr_impl(uint64_t *dst, const uint64_t *src,
                                        size_t rdx) noexcept;
#else
extern "C" WJR_MS_ABI void __wjr_asm_basecase_sqr_impl(uint64_t *dst, const uint64_t *src,
                                                       size_t rdx) noexcept;
#endif

inline void asm_basecase_sqr(uint64_t *dst, const uint64_t *src, size_t n) noexcept {
    WJR_ASSERT(n >= 1);
    __wjr_asm_basecase_sqr_impl(dst, src, n);
}

#endif

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1) == 1

// slower than asm_addmul_1
inline uint64_t asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n,
                             uint64_t rdx) noexcept {
    WJR_ASSERT(n != 0);

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

        ".align 32\n\t"
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

        : [dst] "+&r"(dst), [src] "+&r"(src), [rcx] "+&c"(rcx), [r8] "=&r"(r8),
          [r9] "=&r"(r9), [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(rdx)
        : "cc", "memory");

    WJR_ASSUME(rcx == 0);

    return r9;
}

#else

extern "C" WJR_MS_ABI uint64_t __wjr_asm_submul_1(uint64_t *dst, const uint64_t *src,
                                                  size_t n, uint64_t rdx) noexcept;

WJR_INTRINSIC_INLINE uint64_t asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                           uint64_t rdx) noexcept {
    return __wjr_asm_submul_1(dst, src, n, rdx);
}

#endif

#endif

#if WJR_HAS_BUILTIN(ASM_ADDLSH_N)
#define WJR_ADDSUB_I 1
#include <wjr/x86/math/gen_addrsblsh_n.hpp>
#endif

#if WJR_HAS_BUILTIN(ASM_RSBLSH_N)
#define WJR_ADDSUB_I 0
#include <wjr/x86/math/gen_addrsblsh_n.hpp>
#endif

} // namespace wjr

#endif // WJR_X86_MATH_MUL_HPP__