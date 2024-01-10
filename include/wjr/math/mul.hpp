#ifndef WJR_MATH_MUL_HPP__
#define WJR_MATH_MUL_HPP__

#include <wjr/math/add.hpp>
#include <wjr/math/sub.hpp>

#if defined(WJR_X86)
#include <wjr/x86/mul.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul(T a, T b, T &hi) {
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
    uint64_t c = t < rl;
    uint64_t lo = t + (rm1 << 32);
    c += lo < t;
    hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;
    return lo;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T mul(T a, T b, T &hi) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 64) {
        return fallback_mul(a, b, hi);
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
T fallback_mulhi(T a, T b) {
    T hi = 0;
    (void)fallback_mul(a, b, hi);
    return hi;
}

WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
uint64_t fallback_mulhi64(uint64_t a, uint64_t b) {
    uint64_t hi = 0;
    (void)fallback_mul64(a, b, hi);
    return hi;
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR_E)
T mulhi(T a, T b) {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 64) {
        return fallback_mulhi(a, b);
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

// TODO : optimize
template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul1(T *dst, const T *src0, size_t n, T src1, T c_in) {
    T lo, hi;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src0[i], src1, hi);
        dst[i] = addc<T>(lo, c_in, 0u, c_in);
        c_in += hi;
    }

    return c_in;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T mul_1(T *dst, const T *src0, size_t n, T src1,
                                  type_identity_t<T> c_in) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
#if WJR_HAS_BUILTIN(ASM_MUL_1)
    if (is_constant_evaluated()) {
        return fallback_mul1(dst, src0, n, src1, c_in);
    }

    return asm_mul_1(dst, src0, n, src1, c_in);
#else
    return fallback_mul1(dst, src0, n, src1, c_in);
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

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addmul_1(T *dst, const T *src0, size_t n, T src1,
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

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T submul_1(T *dst, const T *src0, size_t n, T src1,
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
WJR_INLINE_CONSTEXPR void basecase_mul_s(T *dst, const T *src0, size_t n, const T *src1,
                                         size_t m) {
    dst[n] = mul_1(dst, src0, n, src1[0], 0);
    for (size_t i = 1; i < m; ++i) {
        dst[i + n] = addmul_1(dst + i, src0, n, src1[i], 0);
    }
}

// native default threshold of toom-cook-2
// TODO : optimize threshold
#ifndef WJR_TOOM22_MUL_THRESHOLD
#define WJR_TOOM22_MUL_THRESHOLD 26
#endif

inline constexpr size_t toom22_mul_threshold = WJR_TOOM22_MUL_THRESHOLD;

template <typename T>
void mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m);

template <typename T>
void toom22_mul_s(T *dst, const T *src0, size_t n, const T *src1,
                                  size_t m) {
    WJR_ASSUME(n >= m);

    const size_t rn = n >> 1;
    const size_t l = n - rn;

    WJR_ASSUME(l >= rn);
    WJR_ASSUME(l - rn <= 1);

    const size_t rm = m - l;
    WJR_ASSUME(l >= rm);

    unique_stack_ptr ptr(math_details::stack_alloc, sizeof(T) * (l * 2));
    T *stk = (T *)ptr.get();

    // [u0(l), u1(rn)] (n)
    // [v0(l), v1(rm)] (m)

    // (u0 - u1) * (v0 - v1)

    auto u0 = src0;
    auto u1 = src0 + l;

    auto v0 = src1;
    auto v1 = src1 + l;

    const auto p0 = dst;
    const auto p1 = dst + l;
    const auto p2 = dst + l * 2;
    const auto p3 = dst + l * 3;
    const auto p3n = rn + rm - l;

    bool f = 0;

    do {
        int cc;
        cc = abs_subc_s(p0, u0, l, u1, rn);
        if (cc < 0) {
            f ^= 1;
        } else if (WJR_UNLIKELY(cc == 0)) {
            set_n(stk, 0u, l * 2);
            break;
        }

        cc = abs_subc_s(p1, v0, l, v1, rm);
        if (cc < 0) {
            f ^= 1;
        } else if (WJR_UNLIKELY(cc == 0)) {
            set_n(stk, 0u, l * 2);
            break;
        }

        mul_s(stk, p0, l, p1, l);
    } while (0);

    mul_s(p0, u0, l, v0, l);
    mul_s(p2, u1, rn, v1, rm);

    T cy = 0, cy2 = 0;

    cy = addc_n(p2, p1, p2, l, 0u);
    cy2 = cy + addc_n(p1, p0, p2, l, 0u);
    cy += addc_s(p2, p2, l, p3, p3n, 0u);

    if (!f) {
        cy -= subc_n(p1, p1, stk, l * 2, 0u);
    } else {
        cy += addc_n(p1, p1, stk, l * 2, 0u);
    }

    cy2 = addc_1(p2, p2, rn + rm, cy2, 0u);
    WJR_ASSERT(cy2 == 0);
    cy = addc_1(p3, p3, rn + rm - l, cy, 0u);
    WJR_ASSERT(cy == 0);
}

// preview : mul n x m
// TODO : ...

template <typename T>
void mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m) {
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n + m, src0, n));
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n + m, src1, m));

    if (n < m) {
        std::swap(n, m);
        std::swap(src0, src1);
    }

    WJR_ASSUME(m >= 1);

    do {
        if (m <= toom22_mul_threshold) {
            break;
        }

        if (m <= toom22_mul_threshold * 4) {
            if (5 * m <= 4 * n) {
                break;
            }
        }

        if (5 * m <= 3 * n) {
            break;
        }

        return toom22_mul_s(dst, src0, n, src1, m);
    } while (0);

    return basecase_mul_s(dst, src0, n, src1, m);
}

} // namespace wjr

#endif // WJR_MATH_MUL_HPP__