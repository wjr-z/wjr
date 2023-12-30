#ifndef WJR_MATH_MUL_HPP__
#define WJR_MATH_MUL_HPP__

#include <wjr/math/add.hpp>
#include <wjr/math/neg.hpp>

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul_(T a, T b, T &hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;
    using T2 = uint_t<nd * 2>;
    T2 x = static_cast<T2>(a) * b;
    hi = x >> nd;
    return static_cast<T>(x);
}

WJR_INTRINSIC_CONSTEXPR uint64_t fallback_mul64(uint64_t a, uint64_t b, uint64_t &hi) {
    uint64_t ah = a >> 32;
    uint64_t al = a & 0xFFFFFFFF;
    uint64_t bh = b >> 32;
    uint64_t bl = b & 0xFFFFFFFF;

    uint64_t rh = ah * bh;
    uint64_t rm0 = ah * bl;
    uint64_t rm1 = al * bh;
    uint64_t rl = al * bl;

    uint64_t t = rl + (rm0 << 32);
    uint64_t lo = t + (rm1 << 32);
    uint64_t c = t < rl;
    hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;
    return lo;
}

#define WJR_HAS_BUILTIN_MUL64 WJR_HAS_DEF

#if WJR_HAS_FEATURE(INT128)
#define WJR_HAS_BUILTIN_INT128_MUL64 WJR_HAS_DEF
#elif WJR_HAS_FEATURE(INLINE_ASM) && defined(WJR_X86) &&                                 \
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

template <typename T>
WJR_INTRINSIC_CONSTEXPR T mul(T a, T b, T &hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 64) {
        return fallback_mul_(a, b, hi);
    } else {

#if WJR_HAS_BUILTIN(MUL64)
        if (is_constant_evaluated() ||
            (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
            return fallback_mul64(a, b, hi);
        }

#if WJR_HAS_BUILTIN(ASM_MUL64)
        // mov b to rax, then mul a
        // instead of mov a to rax, mov b to register, then mul
        if (WJR_BUILTIN_CONSTANT_P(b)) {
            return builtin_mul64(b, a, hi);
        }
#endif

        return builtin_mul64(a, b, hi);
#else
        return fallback_mul64(a, b, hi);
#endif
    }
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
T fallback_mulhi_(T a, T b) {
    T hi = 0;
    (void)fallback_mul_(a, b, hi);
    return hi;
}

WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
uint64_t fallback_mulhi64(uint64_t a, uint64_t b) {
    uint64_t hi = 0;
    (void)fallback_mul64(a, b, hi);
    return hi;
}

#if WJR_HAS_BUILTIN(MUL64)

WJR_ATTRIBUTES(CONST, INTRINSIC_INLINE)
uint64_t builtin_mulhi64(uint64_t a, uint64_t b) {
    uint64_t hi;
    (void)builtin_mul64(a, b, hi);
    return hi;
}

#endif

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
T mulhi(T a, T b) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 64) {
        return fallback_mulhi_(a, b);
    } else {

#if WJR_HAS_BUILTIN(MUL64)
        if (is_constant_evaluated() ||
            (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
            return fallback_mulhi64(a, b);
        }

#if WJR_HAS_BUILTIN(ASM_MUL64)
        // mov b to rax, then mul a
        // instead of mov a to rax, mov b to register, then mul
        if (WJR_BUILTIN_CONSTANT_P(b)) {
            return builtin_mulhi64(b, a);
        }
#endif

        return builtin_mulhi64(a, b);
#else
        return fallback_mulhi64(a, b);
#endif
    }
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
T mullo(T a, T b) {
    return a * b;
}

#if WJR_HAS_SIMD(SIMD) && defined(__BMI2__) && defined(WJR_X86)
#define WJR_HAS_BUILTIN_MULX_U64 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(MULX_U64)

template <typename T>
WJR_INTRINSIC_INLINE T mulx(T a, T b, T &hi) {
    static_assert(sizeof(T) == 8, "");

#if defined(WJR_COMPILER_GCC)
    T lo;
    asm("mulx {%3, %0, %1|%1, %0, %3}" : "=r"(lo), "=r"(hi) : "%d"(a), "rm"(b));
    return lo;
#else
    unsigned long long hi_;
    unsigned long long lo = _mulx_u64(a, b, &hi_);
    hi = hi_;
    return lo;
#endif
}

#endif

// TODO : optimize
template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul_1(T *dst, const T *src0, size_t n, T src1,
                                         T c_in) {
    T lo, hi;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src0[i], src1, hi);
        dst[i] = addc<T>(lo, c_in, 0u, c_in);
        c_in += hi;
    }

    return c_in;
}

#if WJR_HAS_BUILTIN(MULX_U64) && WJR_HAS_FEATURE(INLINE_ASM) && defined(WJR_X86) &&      \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_MUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)

WJR_INLINE uint64_t asm_mul_1(uint64_t *dst, const uint64_t *src0, size_t n,
                              uint64_t src1, uint64_t t2) {
    size_t m = n / 8 + 1;
    n &= 7;
    dst += n - 8;
    src0 += n - 8;

    uint64_t t0 = n;
    uint64_t t1;
    uint64_t t3;

    asm volatile("xor %[t1], %[t1]\n\t"
                 "lea{q .Lasm_addmul_1_lookup%=(%%rip), %[t3]| %[t3], "
                 "[rip+.Lasm_addmul_1_lookup%=]}\n\t"
                 "movs{lq (%[t3], %[t0], 4), %[t0]|xd %[t0], DWORD PTR [%[t3] + "
                 "%[t0] * "
                 "4]}\n\t"
                 "lea{q (%[t3], %[t0], 1), %[t0]| %[t0], [%[t0] + %[t3]]}\n\t"
                 "jmp{q *%[t0]| %[t0]}\n\t"

                 ".align 4\n\t"
                 ".Lasm_addmul_1_lookup%=:\n\t"
                 ".long .Lcase0%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase1%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase2%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase3%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase4%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase5%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase6%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase7%=-.Lasm_addmul_1_lookup%=\n\t"

                 ".Lasm_addmul_1_loop%=:\n\t"

                 "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
                 "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"

                 "mulx {(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0]]}\n\t"
                 "adc{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                 "mov{q %[t0], (%[dst])| [%[dst]], %[t0]}\n\t"

                 ".Lcase7%=:\n\t"
                 "mulx {8(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 8]}\n\t"
                 "adc{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                 "mov{q %[t0], 8(%[dst])| [%[dst] + 8], %[t0]}\n\t"

                 ".Lcase6%=:\n\t"
                 "mulx {16(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 16]}\n\t"
                 "adc{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                 "mov{q %[t0], 16(%[dst])| [%[dst] + 16], %[t0]}\n\t"

                 ".Lcase5%=:\n\t"
                 "mulx {24(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 24]}\n\t"
                 "adc{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                 "mov{q %[t0], 24(%[dst])| [%[dst] + 24], %[t0]}\n\t"

                 ".Lcase4%=:\n\t"
                 "mulx {32(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 32]}\n\t"
                 "adc{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                 "mov{q %[t0], 32(%[dst])| [%[dst] + 32], %[t0]}\n\t"

                 ".Lcase3%=:\n\t"
                 "mulx {40(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 40]}\n\t"
                 "adc{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                 "mov{q %[t0], 40(%[dst])| [%[dst] + 40], %[t0]}\n\t"

                 ".Lcase2%=:\n\t"
                 "mulx {48(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 48]}\n\t"
                 "adc{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                 "mov{q %[t0], 48(%[dst])| [%[dst] + 48], %[t0]}\n\t"

                 ".Lcase1%=:\n\t"
                 "mulx {56(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 56]}\n\t"
                 "adc{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                 "mov{q %[t0], 56(%[dst])| [%[dst] + 56], %[t0]}\n\t"

                 ".Lcase0%=:\n\t"
                 "dec %[m]\n\t"
                 "jne .Lasm_addmul_1_loop%=\n\t"
                 "adc{q $0, %[t2]| %[t2], 0}"
                 : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+d"(src1), [m] "+c"(m),
                   [t0] "+r"(t0), [t1] "=r"(t1), [t2] "+r"(t2), [t3] "=r"(t3)
                 :
                 : "cc", "memory");

    return t2;
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR T mul_1(T *dst, const T *src0, size_t n, T src1,
                                type_identity_t<T> c_in) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
#if WJR_HAS_BUILTIN(ASM_MUL_1)
    if (is_constant_evaluated()) {
        return fallback_mul_1(dst, src0, n, src1, c_in);
    }

    return asm_mul_1(dst, src0, n, src1, c_in);
#else
    return fallback_mul_1(dst, src0, n, src1, c_in);
#endif
}

// TODO : optimize
template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_addmul_1(T *dst, const T *src0, size_t n, T src1,
                                            T c_in) {
    T lo, hi;
    T o_in;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src0[i], src1, hi);
        lo = addc<T>(lo, c_in, 0u, c_in);
        dst[i] = addc<T>(lo, dst[i], 0u, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

#if WJR_HAS_BUILTIN(ASM_MUL_1) && defined(__ADX__)
#define WJR_HAS_BUILTIN_ASM_ADDMUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)

WJR_INLINE uint64_t asm_addmul_1(uint64_t *dst, const uint64_t *src0, size_t n,
                                 uint64_t src1, uint64_t t2) {
    size_t m = n / 8;
    n &= 7;
    dst += n - 8;
    src0 += n - 8;

    uint64_t t0 = n;
    uint64_t t1;
    uint64_t t3;

    asm volatile("xor %[t1], %[t1]\n\t"

                 "lea{q .Lasm_addmul_1_lookup%=(%%rip), %[t3]| %[t3], "
                 "[rip+.Lasm_addmul_1_lookup%=]}\n\t"
                 "movs{lq (%[t3], %[t0], 4), %[t0]|xd %[t0], DWORD PTR [%[t3] + "
                 "%[t0] * "
                 "4]}\n\t"
                 "lea{q (%[t3], %[t0], 1), %[t0]| %[t0], [%[t0] + %[t3]]}\n\t"
                 "jmp{q *%[t0]| %[t0]}\n\t"

                 ".align 4\n\t"
                 ".Lasm_addmul_1_lookup%=:\n\t"
                 ".long .Lcase0%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase1%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase2%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase3%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase4%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase5%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase6%=-.Lasm_addmul_1_lookup%=\n\t"
                 ".long .Lcase7%=-.Lasm_addmul_1_lookup%=\n\t"

                 ".Lasm_addmul_1_loop%=:\n\t"

                 "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
                 "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"
                 "lea{q -1(%[m]), %[m]| %[m], [%[m] - 1]}\n\t"

                 "mulx {(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0]]}\n\t"
                 "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                 "adox{q (%[dst]), %[t0]| %[t0], [%[dst]]}\n\t"
                 "mov{q %[t0], (%[dst])| [%[dst]], %[t0]}\n\t"

                 ".Lcase7%=:\n\t"
                 "mulx {8(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 8]}\n\t"
                 "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                 "adox{q 8(%[dst]), %[t0]| %[t0], [%[dst] + 8]}\n\t"
                 "mov{q %[t0], 8(%[dst])| [%[dst] + 8], %[t0]}\n\t"

                 ".Lcase6%=:\n\t"
                 "mulx {16(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 16]}\n\t"
                 "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                 "adox{q 16(%[dst]), %[t0]| %[t0], [%[dst] + 16]}\n\t"
                 "mov{q %[t0], 16(%[dst])| [%[dst] + 16], %[t0]}\n\t"

                 ".Lcase5%=:\n\t"
                 "mulx {24(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 24]}\n\t"
                 "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                 "adox{q 24(%[dst]), %[t0]| %[t0], [%[dst] + 24]}\n\t"
                 "mov{q %[t0], 24(%[dst])| [%[dst] + 24], %[t0]}\n\t"

                 ".Lcase4%=:\n\t"
                 "mulx {32(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 32]}\n\t"
                 "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                 "adox{q 32(%[dst]), %[t0]| %[t0], [%[dst] + 32]}\n\t"
                 "mov{q %[t0], 32(%[dst])| [%[dst] + 32], %[t0]}\n\t"

                 ".Lcase3%=:\n\t"
                 "mulx {40(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 40]}\n\t"
                 "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                 "adox{q 40(%[dst]), %[t0]| %[t0], [%[dst] + 40]}\n\t"
                 "mov{q %[t0], 40(%[dst])| [%[dst] + 40], %[t0]}\n\t"

                 ".Lcase2%=:\n\t"
                 "mulx {48(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 48]}\n\t"
                 "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                 "adox{q 48(%[dst]), %[t0]| %[t0], [%[dst] + 48]}\n\t"
                 "mov{q %[t0], 48(%[dst])| [%[dst] + 48], %[t0]}\n\t"

                 ".Lcase1%=:\n\t"
                 "mulx {56(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 56]}\n\t"
                 "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                 "adox{q 56(%[dst]), %[t0]| %[t0], [%[dst] + 56]}\n\t"
                 "mov{q %[t0], 56(%[dst])| [%[dst] + 56], %[t0]}\n\t"

                 ".Lcase0%=:\n\t"
                 "jrcxz .Lasm_addmul_1_loop_out%=\n\t"
                 "jmp .Lasm_addmul_1_loop%=\n\t"

                 ".Lasm_addmul_1_loop_out%=:\n\t"
                 "seto %b[t0]\n\t"
                 "mov{zbl %b[t0], %k[t0]|zx %k[t0], %b[t0]}\n\t"
                 "adc{q %[t0], %[t2]| %[t2], %[t0]}"
                 : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+d"(src1), [m] "+c"(m),
                   [t0] "+r"(t0), [t1] "=r"(t1), [t2] "+r"(t2), [t3] "=r"(t3)
                 :
                 : "cc", "memory");

    return t2;
}

#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR T addmul_1(T *dst, const T *src0, size_t n, T src1,
                                   type_identity_t<T> c_in) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
    if (is_constant_evaluated()) {
        return fallback_addmul_1(dst, src0, n, src1, c_in);
    }

    return asm_addmul_1(dst, src0, n, src1, c_in);
#else
    return fallback_addmul_1(dst, src0, n, src1, c_in);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_submul_1(T *dst, const T *src0, size_t n, T src1,
                                            T c_in) {
    T lo, hi;
    T o_in;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src0[i], src1, hi);
        lo = addc<T>(lo, c_in, 0u, c_in);
        dst[i] = subc<T>(dst[i], lo, 0u, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
#define WJR_HAS_BUILTIN_ASM_SUBMUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)

// slower than asm_addmul_1
// TODO : optimize
WJR_INLINE uint64_t asm_submul_1(uint64_t *dst, const uint64_t *src0, size_t n,
                                 uint64_t src1, uint64_t t2) {
    size_t m = n / 8;
    n &= 7;
    dst += n - 8;
    src0 += n - 8;

    uint64_t t0 = n;
    uint64_t t1;
    uint64_t t3;

    asm volatile("xor %[t1], %[t1]\n\t"
                 "mov{q $127, %[t3]| %[t3], 127}\n\t"
                 // set CF = 0, OF = 1
                 "add{b $1, %b[t3]| %b[t3], 1}\n\t"

                 "lea{q .Lasm_submul_1_lookup%=(%%rip), %[t3]| %[t3], "
                 "[rip+.Lasm_submul_1_lookup%=]}\n\t"
                 "movs{lq (%[t3], %[t0], 4), %[t0]|xd %[t0], DWORD PTR [%[t3] + "
                 "%[t0] * "
                 "4]}\n\t"
                 "lea{q (%[t3], %[t0], 1), %[t0]| %[t0], [%[t0] + %[t3]]}\n\t"
                 "jmp{q *%[t0]| %[t0]}\n\t"

                 ".align 4\n\t"
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

template <typename T>
WJR_INTRINSIC_CONSTEXPR T submul_1(T *dst, const T *src0, size_t n, T src1,
                                   type_identity_t<T> c_in) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)
    if (is_constant_evaluated()) {
        return fallback_submul_1(dst, src0, n, src1, c_in);
    }

    return asm_submul_1(dst, src0, n, src1, c_in);
#else
    return fallback_submul_1(dst, src0, n, src1, c_in);
#endif
}

template <typename T>
WJR_INLINE_CONSTEXPR void basecase_mul(T *dst, const T *src0, size_t n, const T *src1,
                                       size_t m) {
    dst[n] = mul_1(dst, src0, n, src1[0], 0);
    for (size_t i = 1; i < m; ++i) {
        dst[i + n] = addmul_1(dst + i, src0, n, src1[i], 0);
    }
}

template <typename T, typename StackAllocator>
WJR_INLINE_CONSTEXPR20 void mul(T *dst, const T *src0, size_t n, const T *src1, size_t m,
                                StackAllocator &al);

template <typename T, typename StackAllocator>
WJR_INLINE_CONSTEXPR20 void toom22_mul(T *dst, const T *src0, size_t n, const T *src1,
                                       size_t m, StackAllocator &al) {

    size_t mid = n >> 1;
    size_t rn = n - mid;
    size_t rm = m - mid;
    size_t tm = std::max(mid, rm);

    unique_stack_ptr ptr(al, sizeof(T) * (rn + tm));
    T *stk = (T *)ptr.get();

    auto a = src0;
    auto b = src0 + mid;

    auto c = src1;
    auto d = src1 + mid;

    bool f = 0;
    // 0 : (A-B)*(C-D)
    // 1 : (A-B)*(D-C)

    T cc;

    cc = subc(dst, b, rn, a, mid, 0u);
    if (cc) {
        bool g = neg_n(dst, dst, rn);
        WJR_ASSERT(!g);
        f ^= 1;
    }

    if (rm >= mid) {
        cc = subc(dst + rn, d, rm, c, mid, 0u);
        if (cc) {
            bool g = neg_n(dst + rn, dst + rn, rm);
            WJR_ASSERT(!g);
            f ^= 1;
        }

    } else {
        f ^= 1;
        cc = subc(dst + rn, c, mid, d, rm, 0u);
        if (cc) {
            bool g = neg_n(dst + rn, dst + rn, mid);
            WJR_ASSERT(!g);
            f ^= 1;
        }
    }

    mul(stk, dst, rn, dst + rn, tm, al);
    mul(dst, a, mid, c, mid, al);
    mul(dst + mid * 2, b, rn, d, rm, al);

    // stk : rn + tm

    T c2 = 0;

    if (!f) {
        c2 += subc(stk, stk, rn + tm, dst, mid * 2, 0u);
        c2 += subc(stk, stk, rn + tm, dst + mid * 2, rn + rm, 0u);

        c2 -= subc_n(dst + mid, dst + mid, stk, rn + tm, 0u);
    } else {
        c2 += addc(stk, stk, rn + tm, dst, mid * 2, 0u);
        c2 += addc(stk, stk, rn + tm, dst + mid * 2, rn + rm, 0u);

        c2 += addc_n(dst + mid, dst + mid, stk, rn + tm, 0u);
    }

    if (c2) {
        WJR_ASSERT(c2 <= 2);
        c2 = addc_1(dst + mid + rn + tm, dst + mid + rn + tm, m - tm, c2, 0u);
        WJR_ASSERT(c2 == 0);
    }
}

// native default threshold of toom-cook-2
// TODO : optimize threshold
#if !defined(WJR_TOOM2_MUL_THRESHOLD)
#define WJR_TOOM2_MUL_THRESHOLD 26
#endif

inline constexpr size_t toom2_mul_threshold = WJR_TOOM2_MUL_THRESHOLD;

struct mul_threshold {
    size_t toom22_threshold;
    size_t toom33_threshold;
};

void mul_optimize_threshold() {}

// preview : mul n x m
// TODO : ...

template <typename T, typename StackAllocator>
WJR_INLINE_CONSTEXPR20 void mul(T *dst, const T *src0, size_t n, const T *src1, size_t m,
                                StackAllocator &al) {
    if (n < m) {
        std::swap(n, m);
        std::swap(src0, src1);
    }

    WJR_ASSUME(m >= 1);

    if (m <= toom2_mul_threshold) {
        return basecase_mul(dst, src0, n, src1, m);
    }

    if (m <= toom2_mul_threshold * 4) {
        if (5 * m <= 4 * n) {
            return basecase_mul(dst, src0, n, src1, m);
        }

        return toom22_mul(dst, src0, n, src1, m, al);
    }

    if (5 * m <= 3 * n) {
        return basecase_mul(dst, src0, n, src1, m);
    }

    return toom22_mul(dst, src0, n, src1, m, al);
}

} // namespace wjr

#endif // WJR_MATH_MUL_HPP__