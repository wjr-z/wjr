#ifndef WJR_X86_MUL_HPP__
#define WJR_X86_MUL_HPP__

#include <wjr/type_traits.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T mul(T a, T b, T &hi);

#define WJR_HAS_BUILTIN_MUL64 WJR_HAS_DEF

#if WJR_HAS_FEATURE(INT128)
#define WJR_HAS_BUILTIN_INT128_MUL64 WJR_HAS_DEF
#elif WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
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
    asm("mulx{q %3, %0, %1| %1, %0, %3}" : "=r"(lo), "=r"(hi) : "%d"(a), "r"(b));
    return lo;
#else
    unsigned long long hi_;
    unsigned long long lo = _mulx_u64(a, b, &hi_);
    hi = hi_;
    return lo;
#endif
}

#endif

#if WJR_HAS_BUILTIN(MULX_U64) && WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_MUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)

inline uint64_t asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t dx) {
    size_t cx = n / 8;
    uint64_t r8, r9, r10 = n, r11;

    const auto pdst = dst;
    const auto psrc = src;

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
        "inc %[cx]\n\t"
        "jmp .Lb2%=\n\t"

        ".Ll3%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -40(%[src]), %[src]| %[src], [%[src] - 40]}\n\t"
        "lea{q -40(%[dst]), %[dst]| %[dst], [%[dst] - 40]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb3%=\n\t"

        ".Ll4%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -32(%[src]), %[src]| %[src], [%[src] - 32]}\n\t"
        "lea{q -32(%[dst]), %[dst]| %[dst], [%[dst] - 32]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb4%=\n\t"

        ".Ll5%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -24(%[src]), %[src]| %[src], [%[src] - 24]}\n\t"
        "lea{q -24(%[dst]), %[dst]| %[dst], [%[dst] - 24]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb5%=\n\t"

        ".Ll6%=:\n\t"
        "mulx{q (%[src]), %[r10], %[r11]| %[r11], %[r10], [%[src]]}\n\t"
        "lea{q -16(%[src]), %[src]| %[src], [%[src] - 16]}\n\t"
        "lea{q -16(%[dst]), %[dst]| %[dst], [%[dst] - 16]}\n\t"
        "inc %[cx]\n\t"
        "jmp .Lb6%=\n\t"

        ".Ll7%=:\n\t"
        "mulx{q (%[src]), %[r8], %[r9]| %[r9], %[r8], [%[src]]}\n\t"
        "lea{q -8(%[src]), %[src]| %[src], [%[src] - 8]}\n\t"
        "lea{q -8(%[dst]), %[dst]| %[dst], [%[dst] - 8]}\n\t"
        "inc %[cx]\n\t"
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

        "dec %[cx]\n\t"
        "jne .Lloop%=\n\t"

        "adc{q %[cx], %[r9]| %[r9], %[cx]}\n\t"
        ".Ldone%=:\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"

        : [dst] "+&r"(dst), [src] "+&r"(src), [cx] "+&c"(cx), [r8] "=&r"(r8),
          [r9] "=&r"(r9), [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(dx)
        : "cc", "memory");

    WJR_ASSERT_ASSUME(cx == 0);
    WJR_ASSERT_ASSUME(dst == pdst + n);
    WJR_ASSERT_ASSUME(src == psrc + n);

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1) && defined(__ADX__)
#define WJR_HAS_BUILTIN_ASM_ADDMUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)

// TODO : optimize pipeline
inline uint64_t asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t dx) {
    size_t cx = n / 8;
    uint64_t r8, r9, r10 = n, r11;

    const auto pdst = dst;
    const auto psrc = src;

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
        "add{q -8(%[dst]), %[r8]| [%[dst] - 8], %[r8]}\n\t"
        "mov{q %[r8], -8(%[dst])| [%[dst] - 8], %[r8]}\n\t"
        "adc{q %[cx], %[r9]| %[r9], %[cx]}\n\t"
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
        "lea{q -1(%[cx]), %[cx]| %[cx], [%[cx] - 1]}\n\t"
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
        "adox{q %[cx], %[r9]| %[r9], %[cx]}\n\t"
        "adc{q %[cx], %[r9]| %[r9], %[cx]}\n\t"

        ".Ldone%=:"

        : [dst] "+&r"(dst), [src] "+&r"(src), [cx] "+&c"(cx), [r8] "=&r"(r8),
          [r9] "=&r"(r9), [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(dx)
        : "cc", "memory");

    WJR_ASSERT_ASSUME(cx == 0);
    WJR_ASSERT_ASSUME(dst == pdst + n);
    WJR_ASSERT_ASSUME(src == psrc + n);

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1) && WJR_HAS_BUILTIN(ASM_ADDMUL_1)
#define WJR_HAS_BUILTIN_ASM_BASECASE_MUL_S WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)

inline void asm_basecase_mul_s(uint64_t *dst, const uint64_t *src0, size_t n,
                               const uint64_t *src1, size_t m) {
    if (WJR_UNLIKELY(n == 1)) {
        WJR_ASSERT_ASSUME(m == 1);
        dst[0] = mul(*src0, *src1, dst[1]);
        return;
    }

    uint64_t r8, r9, r10, r11;
    uint64_t dx = n, cx;

    uint64_t ax;  // ax = n & 7
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

#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
#define WJR_HAS_BUILTIN_ASM_SUBMUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)

// slower than asm_addmul_1
inline uint64_t asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t dx) {
    WJR_ASSERT(n != 0);

    size_t cx = n / 8;
    uint64_t r8, r9, r10 = n & 7, r11;

    asm volatile(
        // set CF = 1, OF = 0
        "mov{b $255, %b[r11]| %b[r11], 255}\n\t"
        "add{b $1, %b[r11]| %b[r11], 1}\n\t"

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
        "adc{q (%[dst]), %[r8]| [%[dst]], %[r8]}\n\t"
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
        "lea{q -1(%[cx]), %[cx]| %[cx], [%[cx] - 1]}\n\t"
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
        "adox{q %[cx], %[r9]| %[r9], %[cx]}\n\t"
        "sbb{q $-1, %[r9]| %[r9], -1}\n\t"

        ".Ldone%=:"

        : [dst] "+&r"(dst), [src] "+&r"(src), [cx] "+&c"(cx), [r8] "=&r"(r8),
          [r9] "=&r"(r9), [r10] "+&r"(r10), [r11] "=&r"(r11)
        : "d"(dx)
        : "cc", "memory");

    WJR_ASSUME(cx == 0);

    return r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)
#define WJR_HAS_BUILTIN_ASM_ADDLSH_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_RSBLSH_N WJR_HAS_DEF

#define WJR_ADDSUB_I 1
#include <wjr/x86/gen_addrsblsh_n.hpp>

#define WJR_ADDSUB_I 0
#include <wjr/x86/gen_addrsblsh_n.hpp>

#endif

} // namespace wjr

#endif // WJR_X86_MUL_HPP__