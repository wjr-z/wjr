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

        // UB
        ".Ld0%=:\n\t"
        "xor %k[r9], %k[r9]\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll0%=:\n\t"
        "jrcxz .Ld0%=\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
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
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
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
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
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

        // UB
        ".Ld0%=:\n\t"
        "xor %k[r9], %k[r9]\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll0%=:\n\t"
        "jrcxz .Ld0%=\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
        "lea{q -48(%[src]), %[src]| %[src], [%[src] - 48]}\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -40(%[src]), %[src]| %[src], [%[src] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
        "lea{q -32(%[src]), %[src]| %[src], [%[src] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -24(%[src]), %[src]| %[src], [%[src] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
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
        "adox %[cx], %[r9]\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
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
WJR_INLINE uint64_t asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                 uint64_t dx) {
    size_t cx = n / 8;
    n &= 7;
    uint64_t r8, r9, r11;

    auto cdst = dst;
    auto csrc = src;

    asm volatile(
        // set CF = 1, OF = 0
        "mov{b $255, %b[r11]| %b[r11], 255}\n\t"
        "add{b $1, %b[r11]| %b[r11], 1}\n\t"

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

        // UB
        ".Ld0%=:\n\t"
        "xor %k[r9], %k[r9]\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll0%=:\n\t"
        "jrcxz .Ld0%=\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
        "not %[r10]\n\t"
        "jmp .Lb0%=\n\t"

        ".Ll2%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
        "lea{q -48(%[src]), %[src]| %[src], [%[src] - 48]}\n\t"
        "not %[r10]\n\t"
        "lea{q -48(%[dst]), %[dst]| %[dst], [%[dst] - 48]}\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -40(%[src]), %[src]| %[src], [%[src] - 40]}\n\t"
        "not %[r8]\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
        "lea{q -32(%[src]), %[src]| %[src], [%[src] - 32]}\n\t"
        "not %[r10]\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -24(%[src]), %[src]| %[src], [%[src] - 24]}\n\t"
        "not %[r8]\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx {(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src]]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "not %[r10]\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "not %[r8]\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "jmp .Lb7%=\n\t"

        ".Ld1%=:\n\t"
        "adc{q (%[dst]), %[r8]| [%[dst]], %[r8]}\n\t"
        "sbb{q $-1, %[r9]| %[r9], -1}\n\t"
        "mov{q %[r8], (%[dst])| [%[dst]], %[r8]}\n\t"
        "jmp .Ldone%=\n\t"

        ".Ll1%=:\n\t"
        "mulx {(%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "not %[r8]\n\t"
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
        "not %[r10]\n\t"

        ".Lb0%=:\n\t"
        "mulx {8(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 8]}\n\t"
        "adcx{q (%[dst]), %[r10]| %[r10], [%[dst]]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "mov{q %[r10], (%[dst])| [%[dst]], %[r10]}\n\t"
        "not %[r8]\n\t"

        "lea{q -1(%[cx]), %[cx]| %[cx], [%[cx] - 1]}\n\t"

        ".Lb7%=:\n\t"
        "mulx {16(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src] + 16]}\n\t"
        "adcx{q 8(%[dst]), %[r8]| %[r8], [%[dst] + 8]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 8(%[dst])| [%[dst] + 8], %[r8]}\n\t"
        "not %[r10]\n\t"

        ".Lb6%=:\n\t"
        "mulx {24(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 24]}\n\t"
        "adcx{q 16(%[dst]), %[r10]| %[r10], [%[dst] + 16]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "mov{q %[r10], 16(%[dst])| [%[dst] + 16], %[r10]}\n\t"
        "not %[r8]\n\t"

        ".Lb5%=:\n\t"
        "mulx {32(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src] + 32]}\n\t"
        "adcx{q 24(%[dst]), %[r8]| %[r8], [%[dst] + 24]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 24(%[dst])| [%[dst] + 24], %[r8]}\n\t"
        "not %[r10]\n\t"

        ".Lb4%=:\n\t"
        "mulx {40(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 40]}\n\t"
        "adcx{q 32(%[dst]), %[r10]| %[r10], [%[dst] + 32]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "mov{q %[r10], 32(%[dst])| [%[dst] + 32], %[r10]}\n\t"
        "not %[r8]\n\t"

        ".Lb3%=:\n\t"
        "mulx {48(%[src]), %[r10], %[r11]|%[r11], %[r10], [%[src] + 48]}\n\t"
        "adcx{q 40(%[dst]), %[r8]| %[r8], [%[dst] + 40]}\n\t"
        "adox{q %[r9], %[r10]| %[r10], %[r9]}\n\t"
        "mov{q %[r8], 40(%[dst])| [%[dst] + 40], %[r8]}\n\t"
        "not %[r10]\n\t"

        ".Lb2%=:\n\t"
        "mulx {56(%[src]), %[r8], %[r9]|%[r9], %[r8], [%[src] + 56]}\n\t"
        "adcx{q 48(%[dst]), %[r10]| %[r10], [%[dst] + 48]}\n\t"
        "adox{q %[r11], %[r8]| %[r8], %[r11]}\n\t"
        "mov{q %[r10], 48(%[dst])| [%[dst] + 48], %[r10]}\n\t"
        "not %[r8]\n\t"

        "lea{q 64(%[src]), %[src]| %[src], [%[src] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

        "jrcxz .Lloop_out%=\n\t"
        "jmp .Lasm_addmul_1_loop%=\n\t"
        ".Lloop_out%=:\n\t"

        "adcx{q -8(%[dst]), %[r8]| %[r8], [%[dst] - 8]}\n\t"
        "adox{q %[cx], %[r9]| %[r9], %[cx]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "sbb{q $-1, %[r9]| %[r9], -1}\n\t"

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

} // namespace wjr

#endif // WJR_X86_MUL_HPP__