#ifndef WJR_MATH_MUL_HPP__
#define WJR_MATH_MUL_HPP__

#include <wjr/math/add.hpp>

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
    asm volatile("mul{q %3|%3}\n\t"
                 : "=a,a"(lo), "=d,d"(hi)
                 : "%a,r"(a), "r,a"(b)
                 : "cc");
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
        if (is_constant_evaluated() || is_constant_p(a) && is_constant_p(b)) {
            return fallback_mul64(a, b, hi);
        }

#if WJR_HAS_BUILTIN(ASM_MUL64)
        // mov b to rax, then mul a
        // instead of mov a to rax, mov b to register, then mul
        if (is_constant_p(b)) {
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
    uint64_t lo, hi;
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
        if (is_constant_evaluated() || is_constant_p(a) && is_constant_p(b)) {
            return fallback_mulhi64(a, b);
        }

#if WJR_HAS_BUILTIN(ASM_MUL64)
        // mov b to rax, then mul a
        // instead of mov a to rax, mov b to register, then mul
        if (is_constant_p(b)) {
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
    asm volatile("mulx {%3, %0, %1|%1, %0, %3}" : "=r"(lo), "=r"(hi) : "%d"(a), "rm"(b));
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
        dst[i] = addc<T>(lo, 0, c_in, c_in);
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
                              uint64_t src1, uint64_t c_in) {
    size_t m = n / 8;

    if (WJR_LIKELY(m != 0)) {
        uint64_t t0, t1;
        uint64_t t2 = c_in;

        asm volatile("clc\n\t"
                     "asm_mul_1_loop%=:\n\t"

                     "mulx {(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0]]}\n\t"
                     "adc{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                     "mov{q %[t0], (%[dst])| [%[dst]], %[t0]}\n\t"
                     "mulx {8(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 8]}\n\t"
                     "adc{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                     "mov{q %[t0], 8(%[dst])| [%[dst] + 8], %[t0]}\n\t"

                     "mulx {16(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 16]}\n\t"
                     "adc{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                     "mov{q %[t0], 16(%[dst])| [%[dst] + 16], %[t0]}\n\t"
                     "mulx {24(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 24]}\n\t"
                     "adc{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                     "mov{q %[t0], 24(%[dst])| [%[dst] + 24], %[t0]}\n\t"

                     "mulx {32(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 32]}\n\t"
                     "adc{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                     "mov{q %[t0], 32(%[dst])| [%[dst] + 32], %[t0]}\n\t"
                     "mulx {40(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 40]}\n\t"
                     "adc{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                     "mov{q %[t0], 40(%[dst])| [%[dst] + 40], %[t0]}\n\t"

                     "mulx {48(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 48]}\n\t"
                     "adc{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
                     "mov{q %[t0], 48(%[dst])| [%[dst] + 48], %[t0]}\n\t"
                     "mulx {56(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 56]}\n\t"
                     "adc{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
                     "mov{q %[t0], 56(%[dst])| [%[dst] + 56], %[t0]}\n\t"

                     "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
                     "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"
                     "dec %[m]\n\t"
                     "jne asm_mul_1_loop%=\n\t"
                     "adc{q $0, %[t2]| %[t2], 0}"
                     : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+d"(src1), [m] "+r"(m),
                       [t0] "=r"(t0), [t1] "=r"(t1), [t2] "+r"(t2)
                     :
                     : "cc", "memory");

        c_in = t2;
    }

    n &= 7;

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    return fallback_mul_1(dst + m, src0 + m, n, src1, c_in);
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

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src0[i], src1, hi);
        dst[i] = addc<T>(lo, dst[i], c_in, c_in);
        c_in += hi;
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
    uint64_t t0;
    uint64_t t1 = 0;

    asm volatile(
        "and{q $7, %[n]| %[n], 7}\n\t"
        "lea{q -64(%[src0], %[n], 8), %[src0]| %[src0], [%[src0] - 64 + %[n] * 8]}\n\t"
        "lea{q -64(%[dst], %[n], 8), %[dst]| %[dst], [%[dst] - 64 + %[n] * 8]}\n\t"
        "jmp{q *asm_addmul_1_lookup%=(, %[n], 8)| QWORD PTR [asm_addmul_1_lookup%= + %[n] * 8]}\n\t"
        "asm_addmul_1_lookup%=:\n\t"
        ".quad case0%=\n\t"
        ".quad case1%=\n\t"
        ".quad case2%=\n\t"
        ".quad case3%=\n\t"
        ".quad case4%=\n\t"
        ".quad case5%=\n\t"
        ".quad case6%=\n\t"
        ".quad case7%=\n\t"

        "asm_addmul_1_loop%=:\n\t"

        "mulx {(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0]]}\n\t"
        "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
        "adox{q (%[dst]), %[t0]| %[t0], [%[dst]]}\n\t"
        "mov{q %[t0], (%[dst])| [%[dst]], %[t0]}\n\t"

        "case7%=:\n\t"
        "mulx {8(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 8]}\n\t"
        "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
        "adox{q 8(%[dst]), %[t0]| %[t0], [%[dst] + 8]}\n\t"
        "mov{q %[t0], 8(%[dst])| [%[dst] + 8], %[t0]}\n\t"

        "case6%=:\n\t"
        "mulx {16(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 16]}\n\t"
        "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
        "adox{q 16(%[dst]), %[t0]| %[t0], [%[dst] + 16]}\n\t"
        "mov{q %[t0], 16(%[dst])| [%[dst] + 16], %[t0]}\n\t"

        "case5%=:\n\t"
        "mulx {24(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 24]}\n\t"
        "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
        "adox{q 24(%[dst]), %[t0]| %[t0], [%[dst] + 24]}\n\t"
        "mov{q %[t0], 24(%[dst])| [%[dst] + 24], %[t0]}\n\t"

        "case4%=:\n\t"
        "mulx {32(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 32]}\n\t"
        "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
        "adox{q 32(%[dst]), %[t0]| %[t0], [%[dst] + 32]}\n\t"
        "mov{q %[t0], 32(%[dst])| [%[dst] + 32], %[t0]}\n\t"

        "case3%=:\n\t"
        "mulx {40(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 40]}\n\t"
        "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
        "adox{q 40(%[dst]), %[t0]| %[t0], [%[dst] + 40]}\n\t"
        "mov{q %[t0], 40(%[dst])| [%[dst] + 40], %[t0]}\n\t"

        "case2%=:\n\t"
        "mulx {48(%[src0]), %[t0], %[t1]|%[t1], %[t0], [%[src0] + 48]}\n\t"
        "adcx{q %[t2], %[t0]| %[t0], %[t2]}\n\t"
        "adox{q 48(%[dst]), %[t0]| %[t0], [%[dst] + 48]}\n\t"
        "mov{q %[t0], 48(%[dst])| [%[dst] + 48], %[t0]}\n\t"

        "case1%=:\n\t"
        "mulx {56(%[src0]), %[t0], %[t2]|%[t2], %[t0], [%[src0] + 56]}\n\t"
        "adcx{q %[t1], %[t0]| %[t0], %[t1]}\n\t"
        "adox{q 56(%[dst]), %[t0]| %[t0], [%[dst] + 56]}\n\t"
        "mov{q %[t0], 56(%[dst])| [%[dst] + 56], %[t0]}\n\t"

        "case0%=:\n\t"
        "lea{q 64(%[src0]), %[src0]| %[src0], [%[src0] + 64]}\n\t"
        "lea{q 64(%[dst]), %[dst]| %[dst], [%[dst] + 64]}\n\t"
        "jrcxz asm_addmul_1_loop_out%=\n\t"
        "lea{q -1(%[m]), %[m]| %[m], [%[m] - 1]}\n\t"
        "jmp asm_addmul_1_loop%=\n\t"
        
        "asm_addmul_1_loop_out%=:\n\t"
        "seto %b[t0]\n\t"
        "mov{zbl %b[t0], %k[t0]|zx %k[t0], %b[t0]}\n\t"
        "adc{q %[t0], %[t2]| %[t2], %[t0]}"
        : [dst] "+r"(dst), [src0] "+r"(src0), [src1] "+d"(src1), [m] "+c"(m),
          [t0] "=r"(t0), [t1] "+r"(t1), [t2] "+r"(t2), [n] "+r"(n)
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

// preview
template <typename T>
WJR_INTRINSIC_CONSTEXPR T mul_n_basecase(T *dst, const T *src0, size_t n, const T *src1,
                                         size_t m, type_identity_t<T> c_in) {
    WJR_ASSUME(n >= 1);
    WJR_ASSUME(m >= 1);

    if (n > m) {
        std::swap(src0, src1);
        std::swap(n, m);
    }

    c_in = mul_1(dst, src1, m, src0[0], c_in);
    for (size_t i = 1; i < n; ++i) {
        dst[m + i - 1] = c_in;
        c_in = addmul_1(dst + i, src1, m, src0[i], 0);
    }

    return c_in;
}

} // namespace wjr

#endif // WJR_MATH_MUL_HPP__