#ifndef WJR_MATH_MUL_HPP__
#define WJR_MATH_MUL_HPP__

#include <wjr/math/add.hpp>
#include <wjr/math/shift.hpp>
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

    if ((WJR_BUILTIN_CONSTANT_P(a == 0) && a == 0) ||
        (WJR_BUILTIN_CONSTANT_P(b == 0) && b == 0)) {
        hi = 0;
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(a == 1) && a == 1) {
        hi = 0;
        return b;
    }

    if (WJR_BUILTIN_CONSTANT_P(b == 1) && b == 1) {
        hi = 0;
        return a;
    }

    constexpr auto max = std::numeric_limits<T>::max();

    if (WJR_BUILTIN_CONSTANT_P(a == max) && a == max) {
        hi = b == 0 ? 0 : b - 1;
        return -b;
    }

    if (WJR_BUILTIN_CONSTANT_P(b == max) && b == max) {
        hi = a == 0 ? 0 : a - 1;
        return -a;
    }

    if constexpr (nd < 64) {
        return fallback_mul(a, b, hi);
    } else {

#if WJR_HAS_BUILTIN(MUL64)
        if (is_constant_evaluated() ||
            (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b)) ||
            ((WJR_BUILTIN_CONSTANT_P(a) || WJR_BUILTIN_CONSTANT_P(is_power_of_two(a))) &&
             is_power_of_two(a)) ||
            ((WJR_BUILTIN_CONSTANT_P(b) || WJR_BUILTIN_CONSTANT_P(is_power_of_two(b))) &&
             is_power_of_two(b))) {
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
    T ret = 0;
    (void)mul(a, b, ret);
    return ret;
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
T mullo(T a, T b) {
    return a * b;
}

// TODO : optimize
template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul1(T *dst, const T *src0, size_t n, T src1) {
    T lo = 0, hi = 0;
    T c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src0[i], src1, hi);
        dst[i] = addc<T>(lo, c_in, 0u, c_in);
        c_in += hi;
    }

    return c_in;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T mul_1(T *dst, const T *src0, size_t n, T src1) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
#if WJR_HAS_BUILTIN(ASM_MUL_1)
    if (is_constant_evaluated()) {
        return fallback_mul1(dst, src0, n, src1);
    }

    return asm_mul_1(dst, src0, n, src1);
#else
    return fallback_mul1(dst, src0, n, src1);
#endif
}

// TODO : optimize
template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_addmul_1(T *dst, const T *src0, size_t n, T src1) {
    T lo = 0, hi = 0;
    T o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src0[i], src1, hi);
        lo = addc<T>(lo, c_in, 0u, c_in);
        dst[i] = addc<T>(lo, dst[i], 0u, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addmul_1(T *dst, const T *src0, size_t n, T src1) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
    if (is_constant_evaluated()) {
        return fallback_addmul_1(dst, src0, n, src1);
    }

    return asm_addmul_1(dst, src0, n, src1);
#else
    return fallback_addmul_1(dst, src0, n, src1);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_submul_1(T *dst, const T *src0, size_t n, T src1) {
    T lo = 0, hi = 0;
    T o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src0[i], src1, hi);
        lo = addc<T>(lo, c_in, 0u, c_in);
        dst[i] = subc<T>(dst[i], lo, 0u, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T submul_1(T *dst, const T *src0, size_t n, T src1) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)
    if (is_constant_evaluated()) {
        return fallback_submul_1(dst, src0, n, src1);
    }

    return asm_submul_1(dst, src0, n, src1);
#else
    return fallback_submul_1(dst, src0, n, src1);
#endif
}

// preview :

// native default threshold of toom-cook-2
// TODO : optimize threshold
#ifndef WJR_TOOM22_MUL_THRESHOLD
#define WJR_TOOM22_MUL_THRESHOLD 26
#endif

inline constexpr size_t toom22_mul_threshold = WJR_TOOM22_MUL_THRESHOLD;

template <typename T>
WJR_INLINE_CONSTEXPR void basecase_mul_s(T *dst, const T *src0, size_t n, const T *src1,
                                         size_t m);

template <typename T>
void toom22_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m);

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

template <typename T>
WJR_INLINE_CONSTEXPR void basecase_mul_s(T *dst, const T *src0, size_t n, const T *src1,
                                         size_t m) {
    dst[n] = mul_1(dst, src0, n, src1[0]);
    for (size_t i = 1; i < m; ++i) {
        dst[i + n] = addmul_1(dst + i, src0, n, src1[i]);
    }
}

template <typename T>
void toom22_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m) {
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
        ptrdiff_t cc;
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

    T cf = 0, cf2 = 0;

    cf = addc_n(p2, p1, p2, l, 0u);
    cf2 = cf + addc_n(p1, p0, p2, l, 0u);
    cf += addc_s(p2, p2, l, p3, p3n, 0u);

    if (!f) {
        cf -= subc_n(p1, p1, stk, l * 2, 0u);
    } else {
        cf += addc_n(p1, p1, stk, l * 2, 0u);
    }

    cf2 = addc_1(p2, p2, rn + rm, cf2, 0u);
    WJR_ASSERT(cf2 == 0);
    cf = addc_1(p3, p3, rn + rm - l, cf, 0u);
    WJR_ASSERT(cf == 0);
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR_E void divexact_by3(T *dst, const T *src, size_t n);

template <typename T>
void toom33_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m) {
    WJR_ASSUME(n >= m);

    const size_t l = (n + 2) / 3;
    const size_t rn = n - l * 2;
    const size_t rm = m - l * 2;

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;

    unique_stack_ptr ptr(math_details::stack_alloc, sizeof(T) * ((l + 1) * 12));
    T *stk = (T *)ptr.get();

    auto w0p = stk;
    auto w1p = stk + (l + 1) * 2;
    auto w2p = stk + (l + 1) * 4;
    auto w3p = stk + (l + 1) * 6;
    auto w4p = stk + (l + 1) * 8;
    auto w5p = stk + (l + 1) * 10;

    T cf0, cf1, cf2, cf3, cf4, cf5;
    bool neg0, neg1, neg2, neg3, neg4, neg5;

    // W0 = U0 + U2
    cf0 = addc_s(w0p, u0p, l, u2p, rn, 0u);
    // W4 = V0 + V2
    cf4 = addc_s(w4p, v0p, l, v2p, rm, 0u);

    // W3 = W0 - U1
    if (cf0) {
        neg3 = 0;
        cf3 = cf0 - subc_n(w3p, stk, u1p, l, 0u);
    } else {
        cf3 = 0;
        ptrdiff_t p = abs_subc_n(w3p, stk, u1p, l);
        neg3 = p < 0;
    }

    // W2 = W4 - V1
    if (cf1) {
        neg2 = 0;
        cf2 = cf4 - subc_n(w2p, w4p, v1p, l, 0u);
    } else {
        cf2 = 0;
        ptrdiff_t p = abs_subc_n(w2p, w4p, v1p, l);
        neg2 = p < 0;
    }

    // W0 = W0 + U1
    cf0 += addc_n(w0p, w0p, u1p, l, 0u);

    // W4 = W4 + V1
    cf4 += addc_n(w4p, w4p, v1p, l, 0u);

    // W1 = W3 * W2
    w3p[l] = cf3;
    w2p[l] = cf2;
    cf1 = 0;
    neg1 = neg3 ^ neg2;
    mul_s(w1p, w3p, l + 1, w2p, l + 1);

    // W2 = W0 * W4
    w0p[l] = cf0;
    w4p[l] = cf4;
    cf2 = 0;
    neg2 = neg0 ^ neg4;
    mul_s(w2p, w0p, l + 1, w4p, l + 1);

    // W0 = (W0 + U2) << 1 - U0
    cf0 += addc_s(w0p, w0p, l, u2p, rn, 0u);
    cf0 += lshift_n(w0p, w0p, l, 1u);
    if (cf0) {
        neg0 = 0;
        cf0 -= subc_n(w0p, w0p, u0p, l, 0u);
    } else {
        cf0 = 0;
        ptrdiff_t p = abs_subc_n(w0p, w0p, u0p, l);
        neg0 = p < 0;
    }

    // W4 = (W4 + V2) << 1 - V0
    cf4 += addc_s(w4p, w4p, l, v2p, rm, 0u);
    cf4 += lshift_n(w4p, w4p, l, 1u);
    if (cf4) {
        neg4 = 0;
        cf4 -= subc_n(w4p, w4p, v0p, l, 0u);
    } else {
        cf4 = 0;
        ptrdiff_t p = abs_subc_n(w4p, w4p, v0p, l);
        neg4 = p < 0;
    }

    // W3 = W0 * W4
    w0p[l] = cf0;
    w4p[l] = cf4;
    cf3 = 0;
    neg3 = neg0 ^ neg4;
    mul_s(w3p, w0p, l + 1, w4p, l + 1);

    // W0 = U0 * V0
    cf0 = 0;
    neg0 = 0;
    mul_s(w0p, u0p, l, v0p, l);

    // W4 = U2 * V2
    cf4 = 0;
    neg4 = 0;
    mul_s(w4p, u2p, rn, v2p, rm);

    // W3 = (W3 - W1) / 3
    {
        if (neg3 == neg1) {
            ptrdiff_t p = abs_subc_n(w3p, w3p, w1p, (l + 1) * 2);
            neg3 ^= p < 0;
        } else {
            cf3 = addc_n(w3p, w3p, w1p, (l + 1) * 2, 0u);
        }

        cf3 = 0;
        divexact_by3(w3p, w3p, (l + 1) * 2);
    }

    // W1 = (W2 - W1) >> 1
    {
        if (neg2 == neg1) {
            ptrdiff_t p = abs_subc_n(w1p, w2p, w1p, (l + 1) * 2);
            neg1 ^= p < 0;
        } else {
            neg1 ^= 1;
            cf1 = addc_n(w1p, w2p, w1p, (l + 1) * 2, 0u);
        }

        rshift_n(w1p, w1p, (l + 1) * 2, 1u);
        if (cf1) {
            w1p[(l + 1) * 2 - 1] |= (cf1) << 63;
            cf1 >>= 1;
        }
    }

    // W2 = W2 - W0
    {
        if (neg2 == neg0) {
            ptrdiff_t p = abs_subc_n(w2p, w2p, w0p, (l + 1) * 2);
            neg2 ^= p < 0;
        } else {
            cf2 = addc_n(w2p, w2p, w0p, (l + 1) * 2, 0u);
        }
    }

    // W3 = (W3 - W2) >> 1 - W4 << 1
    {
        if (neg3 == neg2) {
            if (cf2) {
                neg3 ^= 1;
                cf3 = cf2 - subc_n(w3p, w2p, w3p, (l + 1) * 2, 0u);
            } else {
                cf3 = 0;
                ptrdiff_t p = abs_subc_n(w3p, w3p, w2p, (l + 1) * 2);
                neg3 ^= p < 0;
            }
        } else {
            cf3 = cf2 + addc_n(w3p, w3p, w2p, (l + 1) * 2, 0u);
        }

        (void)rshift_n(w3p, w3p, (l + 1) * 2, 1u);
        if (cf3) {
            w3p[(l + 1) * 2 - 1] |= cf3 << 63;
            cf3 >>= 1;
        }

        neg5 = 0;
        cf5 = lshift_n(w5p, w4p, (l + 1) * 2, 1u);

        if (neg3 == neg5) {
            if (cf3 > cf5) {
                cf3 = cf3 - cf5;
                cf3 -= subc_n(w3p, w3p, w5p, (l + 1) * 2, 0u);
            } else if (cf3 < cf5) {
                cf3 = cf5 - cf3;
                neg3 ^= 1;
                cf3 -= subc_n(w3p, w5p, w3p, (l + 1) * 2, 0u);
            } else {
                cf3 = 0;
                ptrdiff_t p = abs_subc_n(w3p, w3p, w5p, (l + 1) * 2);
                neg3 ^= p < 0;
            }
        } else {
            cf3 += cf5;
            cf3 += addc_n(w3p, w3p, w5p, (l + 1) * 2, 0u);
        }
    }

    // W2 = W2 - W1
    {
        if (neg2 == neg1) {
            if (cf2) {
                cf2 = subc_n(w2p, w2p, w1p, (l + 1) * 2, 0u);
            } else {
                ptrdiff_t p = abs_subc_n(w2p, w2p, w1p, (l + 1) * 2);
                neg2 ^= p < 0;
            }
        } else {
            cf2 += addc_n(w2p, w2p, w1p, (l + 1) * 2, 0u);
        }
    }

    // W3 = W4 * x + W3
}

} // namespace wjr

#endif // WJR_MATH_MUL_HPP__