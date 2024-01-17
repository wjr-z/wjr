#ifndef WJR_X86_MUL_HPP__
#define WJR_X86_MUL_HPP__

#include <wjr/type_traits.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#define WJR_HAS_BUILTIN_MUL64 WJR_HAS_DEF

#if WJR_HAS_FEATURE(INT128)
#define WJR_HAS_BUILTIN_INT128_MUL64 WJR_HAS_DEF
#elif WJR_HAS_FEATURE(INLINE_ASM) &&                                                     \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_MUL64 WJR_HAS_DEF
#else
#undef WJR_HAS_BUILTIN_MUL64
#endif

#if WJR_HAS_BUILTIN(MUL64)

WJR_INTRINSIC_INLINE uint64_t builtin_mul64(uint64_t a, uint64_t b, uint64_t &hi) {
#if WJR_HAS_BUILTIN(INT128_MUL64)
    __uint128_t x = static_cast<__uint128_t>(a) * b;
    hi = x >> 64;
    return static_cast<uint64_t>(x);
#elif WJR_HAS_BUILTIN(ASM_MUL64)
    uint64_t lo;
    asm("mul{q %3| %3}\n\t" : "=a,a"(lo), "=d,d"(hi) : "%a,r"(a), "r,a"(b) : "cc");
    return lo;
#endif
}

#endif

#if WJR_HAS_SIMD(SIMD) && defined(__BMI2__)
#define WJR_HAS_BUILTIN_MULX_U64 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(MULX_U64)

template <typename T>
WJR_INTRINSIC_INLINE T mulx(T a, T b, T &hi) {
    static_assert(sizeof(T) == 8, "");

#if defined(WJR_COMPILER_GCC)
    T lo;
    asm("mulx {%3, %0, %1|%1, %0, %3}" : "=r"(lo), "=r"(hi) : "%d"(a), "r"(b));
    return lo;
#else
    unsigned long long hi_;
    unsigned long long lo = _mulx_u64(a, b, &hi_);
    hi = hi_;
    return lo;
#endif
}

#endif

#if WJR_HAS_BUILTIN(MULX_U64) && WJR_HAS_FEATURE(INLINE_ASM) &&                          \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_MUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)

WJR_INLINE uint64_t asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t dx) {
    size_t cx = n / 8;
    uint64_t r8, r9, r11;

    auto cdst = dst;
    auto csrc = src;

    asm volatile(
        "and{l $7, %k[r10]| %k[r10], 7}\n\t"
        "lea{q| %[r9], [rip +} .Lasm_mul_1_lookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r10] + %[r9]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"

        ".align 8\n\t"
        ".Lasm_mul_1_lookup%=:\n\t"
        ".long .Ll0%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll1%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll2%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll3%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll4%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll5%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll6%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll7%=-.Lasm_mul_1_lookup%=\n\t"
        ".align 16\n\t"

        ".Ld0%=:\n\t"
        "xor %k[r9], %k[r9]\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll0%=:\n\t"
        "jrcxz .Ld0%=\n\t"
        "mulx {(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -48(%[src]), %[src]| %[src], [%[src] - 48]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -40(%[src]), %[src]| %[src], [%[src] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -32(%[src]), %[src]| %[src], [%[src] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -24(%[src]), %[src]| %[src], [%[src] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld1%=:\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "jrcxz .Ld1%=\n\t"
        "lea{q 8(%[src]), %[src]| %[src], [%[src] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"

        ".align 32\n\t"
        ".Lasm_addmul_1_loop%=:\n\t"

        ".Lb1%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb0%=:\n\t"
        "mulx {8(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 8]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb7%=:\n\t"
        "mulx {16(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src] + 16]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb6%=:\n\t"
        "mulx {24(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 24]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb5%=:\n\t"
        "mulx {32(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src] + 32]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb4%=:\n\t"
        "mulx {40(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 40]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb3%=:\n\t"
        "mulx {48(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src] + 48]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"
        "adc{q %[r9], %[r10]| %[r10], %[r9]}\n\t"

        ".Lb2%=:\n\t"
        "mulx {56(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 56]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"
        "adc{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "dec %[cx]\n\t"
        "jne .Lasm_addmul_1_loop%=\n\t"

        "adc{q $0, %[r9]| %[r9], 0}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        ".Ldone%=:"

        : [dst] "+r"(dst), [src] "+r"(src), "+d"(dx), [cx] "+c"(cx), [r8] "=r"(r8),
          [r9] "=r"(r9), [r10] "+r"(n), [r11] "=r"(r11)
        :
        : "cc", "memory");

    // Instruct the compiler to perform more optimizations
    WJR_ASSUME(dst == cdst + n);
    WJR_ASSUME(src == csrc + n);

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1) && defined(__ADX__)
#define WJR_HAS_BUILTIN_ASM_ADDMUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)

// TODO : optimize pipeline
WJR_INLINE uint64_t asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                 uint64_t dx) {
    size_t cx = n / 8;
    uint64_t r8, r9, r11;

    auto cdst = dst;
    auto csrc = src;

    asm volatile(
        "and{l $7, %k[r10]| %k[r10], 7}\n\t"
        "lea{q| %[r9], [rip +} .Lasm_mul_1_lookup%={(%%rip), %[r9]|]}\n\t"
        "movs{lq (%[r9], %[r10], 4), %[r10]|xd %[r10], DWORD PTR [%[r9] + %[r10] * 4]}\n\t"
        "lea{q (%[r9], %[r10], 1), %[r10]| %[r10], [%[r10] + %[r9]]}\n\t"
        "jmp{q *%[r10]| %[r10]}\n\t"

        ".align 8\n\t"
        ".Lasm_mul_1_lookup%=:\n\t"
        ".long .Ll0%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll1%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll2%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll3%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll4%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll5%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll6%=-.Lasm_mul_1_lookup%=\n\t"
        ".long .Ll7%=-.Lasm_mul_1_lookup%=\n\t"
        ".align 16\n\t"

        ".Ld0%=:\n\t"
        "xor %k[r9], %k[r9]\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll0%=:\n\t"
        "jrcxz .Ld0%=\n\t"
        "mulx {(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -48(%[src]), %[src]| %[src], [%[src] - 48]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -40(%[src]), %[src]| %[src], [%[src] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -32(%[src]), %[src]| %[src], [%[src] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -24(%[src]), %[src]| %[src], [%[src] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld1%=:\n\t"
        "add{q (%[dst]), %[r8]| [%[dst]], %[r8]}\n\t"
        "adc{q $0, %[r9]| %[r9], 0}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "jrcxz .Ld1%=\n\t"
        "lea{q 8(%[src]), %[src]| %[src], [%[src] + 8]}\n\t"
        "lea{q 8(%[dst]), %[dst]| %[dst], [%[dst] + 8]}\n\t"

        ".align 32\n\t"
        ".Lasm_addmul_1_loop%=:\n\t"

        ".Lb1%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        ".Lb0%=:\n\t"
        "mulx {8(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 8]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"

        "lea{q -1(%[cx]), %[cx]| %[cx], [%[cx] - 1]}\n\t"

        ".Lb7%=:\n\t"
        "mulx {16(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"

        ".Lb6%=:\n\t"
        "mulx {24(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 24]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"

        ".Lb5%=:\n\t"
        "mulx {32(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src] + 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"

        ".Lb4%=:\n\t"
        "mulx {40(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 40]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"

        ".Lb3%=:\n\t"
        "mulx {48(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src] + 48]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"

        ".Lb2%=:\n\t"
        "mulx {56(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 56]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"

        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Lloop_out%=\n\t"
        "jmp .Lasm_addmul_1_loop%=\n\t"
        ".Lloop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adox %[cx], %[r9]\n\t"
        "adc %[cx], %[r9]\n\t"

        ".Ldone%=:"

        : [dst] "+r"(dst), [src] "+r"(src), "+d"(dx), [cx] "+c"(cx), [r8] "=r"(r8),
          [r9] "=r"(r9), [r10] "+r"(n), [r11] "=r"(r11)
        :
        : "cc", "memory");

    WJR_ASSUME(dst == cdst + n);
    WJR_ASSUME(src == csrc + n);

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1) && WJR_HAS_BUILTIN(ASM_ADDMUL_1)
#define WJR_HAS_BUILTIN_ASM_BASECASE_MUL_S WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)

// TODO : low priority

#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
#define WJR_HAS_BUILTIN_ASM_SUBMUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)

// slower than asm_addmul_1
// TODO : optimize
WJR_INLINE uint64_t asm_submul_1(uint64_t *dst, const uint64_t *src0, size_t n,
                                 uint64_t src1) {
    size_t m = n / 8;
    n &= 7;
    dst += n - 8;
    src0 += n - 8;

    uint64_t t0 = n;
    uint64_t t1;
    uint64_t t2 = 0;
    uint64_t t3;

    asm volatile(
        "xor %k[t1], %k[t1]\n\t"
        "mov{q $127, %[t3]| %[t3], 127}\n\t"
        // set CF = 0, OF = 1
        "add{b $1, %b[t3]| %b[t3], 1}\n\t"

        "lea{q| %[t3], [rip +} .Lasm_submul_1_lookup%={(%%rip), %[t3]|]}\n\t"
        "movs{lq (%[t3], %[t0], 4), %[t0]|xd %[t0], DWORD PTR [%[t3] + "
        "%[t0] * "
        "4]}\n\t"
        "lea{q (%[t3], %[t0], 1), %[t0]| %[t0], [%[t0] + %[t3]]}\n\t"
        "jmp{q *%[t0]| %[t0]}\n\t"

        ".align 8\n\t"
        ".Lasm_submul_1_lookup%=:\n\t"
        ".long .Lcase0%=-.Lasm_submul_1_lookup%=\n\t"
        ".long .Lcase1%=-.Lasm_submul_1_lookup%=\n\t"
        ".long .Lcase2%=-.Lasm_submul_1_lookup%=\n\t"
        ".long .Lcase3%=-.Lasm_submul_1_lookup%=\n\t"
        ".long .Lcase4%=-.Lasm_submul_1_lookup%=\n\t"
        ".long .Lcase5%=-.Lasm_submul_1_lookup%=\n\t"
        ".long .Lcase6%=-.Lasm_submul_1_lookup%=\n\t"
        ".long .Lcase7%=-.Lasm_submul_1_lookup%=\n\t"

        ".Lasm_submul_1_loop%=:\n\t"

        "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"
        "lea{q -1(%[m]), %[m]| %[m], [%[m] - 1]}\n\t"

        "mulx {(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0]]}\n\t"
        "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
        "not %[t0]\n\t"
        "adox{q (%[dst]), %[t0]| %[t0], [%[dst]]}\n\t"
        "mov{q %[t0], (%[dst])| [%[dst]], %[t0]}\n\t"

        ".Lcase7%=:\n\t"
        "mulx {8(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 8]}\n\t"
        "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
        "not %[t0]\n\t"
        "adox{q 8(%[dst]), %[t0]| %[t0], [%[dst] + 8]}\n\t"
        "mov{q %[t0], 8(%[dst])| [%[dst] + 8], %[t0]}\n\t"

        ".Lcase6%=:\n\t"
        "mulx {16(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 16]}\n\t"
        "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
        "not %[t0]\n\t"
        "adox{q 16(%[dst]), %[t0]| %[t0], [%[dst] + 16]}\n\t"
        "mov{q %[t0], 16(%[dst])| [%[dst] + 16], %[t0]}\n\t"

        ".Lcase5%=:\n\t"
        "mulx {24(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 24]}\n\t"
        "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
        "not %[t0]\n\t"
        "adox{q 24(%[dst]), %[t0]| %[t0], [%[dst] + 24]}\n\t"
        "mov{q %[t0], 24(%[dst])| [%[dst] + 24], %[t0]}\n\t"

        ".Lcase4%=:\n\t"
        "mulx {32(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 32]}\n\t"
        "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
        "not %[t0]\n\t"
        "adox{q 32(%[dst]), %[t0]| %[t0], [%[dst] + 32]}\n\t"
        "mov{q %[t0], 32(%[dst])| [%[dst] + 32], %[t0]}\n\t"

        ".Lcase3%=:\n\t"
        "mulx {40(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 40]}\n\t"
        "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
        "not %[t0]\n\t"
        "adox{q 40(%[dst]), %[t0]| %[t0], [%[dst] + 40]}\n\t"
        "mov{q %[t0], 40(%[dst])| [%[dst] + 40], %[t0]}\n\t"

        ".Lcase2%=:\n\t"
        "mulx {48(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 48]}\n\t"
        "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
        "not %[t0]\n\t"
        "adox{q 48(%[dst]), %[t0]| %[t0], [%[dst] + 48]}\n\t"
        "mov{q %[t0], 48(%[dst])| [%[dst] + 48], %[t0]}\n\t"

        ".Lcase1%=:\n\t"
        "mulx {56(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 56]}\n\t"
        "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
        "not %[t0]\n\t"
        "adox{q 56(%[dst]), %[t0]| %[t0], [%[dst] + 56]}\n\t"
        "mov{q %[t0], 56(%[dst])| [%[dst] + 56], %[t0]}\n\t"

        ".Lcase0%=:\n\t"
        "jrcxz .Lasm_submul_1_loop_out%=\n\t"
        "jmp .Lasm_submul_1_loop%=\n\t"

        ".Lasm_submul_1_loop_out%=:\n\t"
        "seto %b[t0]\n\t"
        "mov{zbl %b[t0], %k[t0]|zx %k[t0], %b[t0]}\n\t"
        "adc{q $1, %[t2]| %[t2], 1}\n\t"
        "sub{q %[t0], %[t2]| %[t2], %[t0]}"
        : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+d"(src1), [m] "+c"(m),
          [t0] "+r"(t0), [t1] "=r"(t1), [t2] "+r"(t2), [t3] "=r"(t3)
        :
        : "cc", "memory");

    return t2;
}

#endif

} // namespace wjr

#endif // WJR_X86_MUL_HPP__