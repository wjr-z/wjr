#ifndef WJR_MATH_MUL_HPP__
#define WJR_MATH_MUL_HPP__

#include <wjr/math/div-impl.hpp>

#include <wjr/math/add.hpp>
#include <wjr/math/ctz.hpp>
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

WJR_INTRINSIC_CONSTEXPR_E uint64_t fallback_mul64(uint64_t a, uint64_t b, uint64_t &hi) {
    uint64_t ah = a >> 32;
    uint64_t al = a & 0xFFFFFFFF;
    uint64_t bh = b >> 32;
    uint64_t bl = b & 0xFFFFFFFF;

    uint64_t rh = ah * bh;
    uint64_t rm0 = ah * bl;
    uint64_t rm1 = al * bh;
    uint64_t rl = al * bl;

    __addc_128(rl, rh, rl, rh, rm0 << 32, rm0 >> 32);
    __addc_128(rl, rh, rl, rh, rm1 << 32, rm1 >> 32);

    hi = rh;
    return rl;
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
        return static_cast<T>(-b);
    }

    if (WJR_BUILTIN_CONSTANT_P(b == max) && b == max) {
        hi = a == 0 ? 0 : a - 1;
        return static_cast<T>(-a);
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
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR_E)
T fallback_mulhi(T a, T b) {
    T hi = 0;
    (void)fallback_mul(a, b, hi);
    return hi;
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T mulhi(T a, T b) {
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
WJR_INTRINSIC_CONSTEXPR T fallback_mul_1(T *dst, const T *src, size_t n, T ml) {
    T lo = 0, hi = 0;
    T c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        dst[i] = addc<T>(lo, c_in, 0u, c_in);
        c_in += hi;
    }

    return c_in;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T mul_1(T *dst, const T *src, size_t n, type_identity_t<T> ml) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 0) && ml == 0) {
        set_n(dst, 0, n);
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        if (src != dst) {
            std::copy(src, src + n, dst);
        }

        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(is_power_of_two(ml)) && is_power_of_two(ml)) {
        unsigned int k = ctz(ml);
        return lshift_n(dst, src, n, k);
    }

#if WJR_HAS_BUILTIN(ASM_MUL_1)
    if (is_constant_evaluated()) {
        return fallback_mul_1(dst, src, n, ml);
    }

    return asm_mul_1(dst, src, n, ml);
#else
    return fallback_mul_1(dst, src, n, ml);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_addmul_1(T *dst, const T *src, size_t n, T ml) {
    T lo = 0, hi = 0;
    T o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        lo = addc<T>(lo, c_in, 0u, c_in);
        dst[i] = addc<T>(lo, dst[i], 0u, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addmul_1(T *dst, const T *src, size_t n,
                                     type_identity_t<T> ml) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 0) && ml == 0) {
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        return addc_n(dst, dst, src, n);
    }

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_addmul_1(dst, src, n, ml);
        }

        return asm_addmul_1(dst, src, n, ml);
    } else {
        return fallback_addmul_1(dst, src, n, ml);
    }
#else
    return fallback_addmul_1(dst, src, n, ml);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_submul_1(T *dst, const T *src, size_t n, T ml) {
    T lo = 0, hi = 0;
    T o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = mul(src[i], ml, hi);
        lo = addc<T>(lo, c_in, 0u, c_in);
        dst[i] = subc<T>(dst[i], lo, 0u, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T submul_1(T *dst, const T *src, size_t n,
                                     type_identity_t<T> ml) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    if (WJR_BUILTIN_CONSTANT_P(ml == 0) && ml == 0) {
        return 0;
    }

    if (WJR_BUILTIN_CONSTANT_P(ml == 1) && ml == 1) {
        return subc_n(dst, dst, src, n);
    }

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)
    if (is_constant_evaluated()) {
        return fallback_submul_1(dst, src, n, ml);
    }

    return asm_submul_1(dst, src, n, ml);
#else
    return fallback_submul_1(dst, src, n, ml);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_addlsh_n(T *dst, const T *src0, const T *src1,
                                            size_t n, type_identity_t<T> cl) {
    T tcl = std::numeric_limits<T>::digits - cl;
    T lo = 0, hi = 0;
    T o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = src1[i] << cl;
        hi = src1[i] >> tcl;
        lo = addc<T>(lo, c_in, 0u, c_in);
        dst[i] = addc<T>(lo, src0[i], 0u, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

// dst = src0 + (src1 << cl)
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addlsh_n(T *dst, const T *src0, const T *src1, size_t n,
                                     type_identity_t<T> cl) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<T>::digits);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    if (WJR_UNLIKELY(cl == 0)) {
        return addc_n(dst, src0, src1, n);
    }

#if WJR_HAS_BUILTIN(ASM_ADDLSH_N)
    if (is_constant_evaluated()) {
        return fallback_addlsh_n(dst, src0, src1, n, cl);
    }

    return asm_addlsh_n(dst, src0, src1, n, cl);
#else

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
    if (WJR_UNLIKELY(dst == src0)) {
        return addmul_1(dst, src1, n, static_cast<T>(static_cast<T>(1u) << cl));
    }
#endif

    return fallback_addlsh_n(dst, src0, src1, n, cl);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_rsblsh_n(T *dst, const T *src0, const T *src1,
                                            size_t n, type_identity_t<T> cl) {
    T tcl = std::numeric_limits<T>::digits - cl;
    T lo = 0, hi = 0;
    T o_in = 0, c_in = 0;

    for (size_t i = 0; i < n; ++i) {
        lo = src1[i] << cl;
        hi = src1[i] >> tcl;
        lo = addc<T>(lo, c_in, 0u, c_in);
        dst[i] = subc<T>(lo, src0[i], 0u, o_in);
        c_in += hi + o_in;
    }

    return c_in;
}

// dst = (src1 << cl) - dst
template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T rsblsh_n(T *dst, const T *src0, const T *src1, size_t n,
                                     type_identity_t<T> cl) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_ASSUME(cl < std::numeric_limits<T>::digits);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src0, n));
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src1, n));

    if (WJR_UNLIKELY(cl == 0)) {
        return subc_n(dst, src1, src0, n);
    }

#if WJR_HAS_BUILTIN(ASM_RSBLSH_N)
    if (is_constant_evaluated()) {
        return fallback_rsblsh_n(dst, src0, src1, n, cl);
    }

    return asm_rsblsh_n(dst, src0, src1, n, cl);
#else
    return fallback_rsblsh_n(dst, src0, src1, n, cl);
#endif
}

template <typename T, T maxn = std::numeric_limits<T>::max()>
WJR_INTRINSIC_CONSTEXPR_E T try_addmul_1(T *dst, const T *src, size_t n,
                                         type_identity_t<T> ml,
                                         std::integral_constant<T, maxn> = {}) {
    static_assert(std::is_same_v<T, uint64_t>, "only support uint64_t now");
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n, src, n));

    WJR_ASSERT_ASSUME(ml <= maxn);

    if constexpr (maxn == 0) {
        return 0;
    }

    if (ml == 0) {
        return 0;
    }

    if constexpr (maxn == 1) {
        return addc_n(dst, dst, src, n);
    } else {
        if (ml == 1) {
            return addc_n(dst, dst, src, n);
        }

        if constexpr (maxn == 2) {
            // addlsh is slightly faster than addmul
            return addlsh_n(dst, dst, src, n, 1u);
        }

        return addmul_1(dst, src, n, ml);
    }
}

// native default threshold of toom-cook-2
// TODO : optimize threshold
#ifndef WJR_TOOM22_MUL_THRESHOLD
#define WJR_TOOM22_MUL_THRESHOLD 26
#endif

#ifndef WJR_TOOM33_MUL_THRESHOLD
#define WJR_TOOM33_MUL_THRESHOLD 66
#endif

#ifndef WJR_TOOM32_TO_TOOM43_MUL_THRESHOLD
#define WJR_TOOM32_TO_TOOM43_MUL_THRESHOLD 69
#endif

inline constexpr size_t toom22_mul_threshold = WJR_TOOM22_MUL_THRESHOLD;
inline constexpr size_t toom33_mul_threshold = WJR_TOOM33_MUL_THRESHOLD;
inline constexpr size_t toom32_to_toom43_mul_threshold =
    WJR_TOOM32_TO_TOOM43_MUL_THRESHOLD;

template <typename T>
void mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m);

enum class __rec_mul_mode : unsigned int {
    toom22,
    toom33,
};

template <__rec_mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __rec_mul_s(T *dst, const T *src0, size_t n, const T *src1,
                                      size_t m, T *stk);

template <__rec_mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __rec_mul_n(T *dst, const T *src0, size_t n, const T *src1,
                                      size_t m, T *stk);

template <typename T>
void basecase_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m);

// l = ceil(n / 2)
// stk usage : l * 2
// recursive stk max usage : l * 4 + 64
template <typename T>
void toom22_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk);

// l = max(ceil(n / 3), ceil(m / 2))
// stk usage : l * 4
// recursive stk max usage : l * 6 + 64
template <typename T>
void toom32_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk);

// l = max(ceil(n / 4), ceil(m / 2))
// stk usage : l * 6
// recursive stk max usage : l * 8 + 67 ?
template <typename T>
void toom42_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk);

// l = ceil(n / 3)
// stk usage : l * 6
// recursive stk max usage : l * 9 + 288 ?
template <typename T>
void toom33_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk);

// l = max(ceil(n / 4),  ceil(m / 3))
// stk usage : l * 6
// recursive stk max usage : l * 9 + 288 ?
template <typename T>
void toom43_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk);

template <typename T>
void mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n + m, src0, n));
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n + m, src1, m));

    if (n < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, m);
    }

    if (m < toom22_mul_threshold) {
        if (n >= m + toom22_mul_threshold / 4) {
            return basecase_mul_s(dst, src0, n, src1, m);
        }
        // n < m + toom22_mul_threshold / 4 <= toom22_mul_threshold - 1 +
        // toom22_mul_threshold / 4. Only need to allocate temporary memory once.
        constexpr auto maxn = (toom22_mul_threshold - 1) + (toom22_mul_threshold / 4);
        constexpr auto maxl = (maxn + 1) / 2;
        T stk[2 * maxl];
        return toom22_mul_s(dst, src0, n, src1, m, stk);
    }

    if (m < toom33_mul_threshold) {
        unique_stack_allocator stkal(math_details::stack_alloc, std::in_place_index<2>);
        T *stk = static_cast<T *>(stkal.allocate(sizeof(T) * (6 * m + 67)));
        if (n >= 3 * m) {
            T *tmp = static_cast<T *>(stkal.allocate(sizeof(T) * (4 * m)));

            toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;
            dst += 2 * m;

            T cf = 0;

            while (n >= 3 * m) {
                toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
                n -= 2 * m;
                src0 += 2 * m;

                cf = addc_n(dst, dst, tmp, m, cf);
                std::copy(tmp + m, tmp + 3 * m, dst + m);
                cf = addc_1(dst + m, dst + m, 2 * m, 0u, cf);

                dst += 2 * m;
            }

            if (4 * n < 5 * m) {
                toom22_mul_s(tmp, src0, n, src1, m, stk);
            } else if (4 * n < 7 * m) {
                toom32_mul_s(tmp, src0, n, src1, m, stk);
            } else {
                toom42_mul_s(tmp, src0, n, src1, m, stk);
            }

            cf = addc_n(dst, dst, tmp, m, cf);
            std::copy(tmp + m, tmp + m + n, dst + m);
            cf = addc_1(dst + m, dst + m, n, 0u, cf);
            WJR_ASSERT(cf == 0);
        } else {
            if (4 * n < 5 * m) {
                toom22_mul_s(dst, src0, n, src1, m, stk);
            } else if (4 * n < 7 * m) {
                toom32_mul_s(dst, src0, n, src1, m, stk);
            } else {
                toom42_mul_s(dst, src0, n, src1, m, stk);
            }
        }

        return;
    }

    unique_stack_allocator stkal(math_details::stack_alloc, std::in_place_index<2>);
    T *stk = static_cast<T *>(stkal.allocate(sizeof(T) * (9 * m + 288)));
    if (n >= 3 * m) {
        T *tmp = static_cast<T *>(stkal.allocate(sizeof(T) * (4 * m)));

        toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
        n -= 2 * m;
        src0 += 2 * m;
        dst += 2 * m;

        T cf = 0;

        while (n >= 3 * m) {
            toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;

            cf = addc_n(dst, dst, tmp, m, cf);
            std::copy(tmp + m, tmp + 3 * m, dst + m);
            cf = addc_1(dst + m, dst + m, 2 * m, 0u, cf);

            dst += 2 * m;
        }

        if (6 * n < 7 * m) {
            toom33_mul_s(tmp, src0, n, src1, m, stk);
        } else if (2 * n < 3 * m) {
            if (m <= toom32_to_toom43_mul_threshold) {
                toom32_mul_s(tmp, src0, n, src1, m, stk);
            } else {
                toom43_mul_s(tmp, src0, n, src1, m, stk);
            }
        } else {
            if (4 * n < 7 * m) {
                toom32_mul_s(tmp, src0, n, src1, m, stk);
            } else {
                toom42_mul_s(tmp, src0, n, src1, m, stk);
            }
        }

        cf = addc_n(dst, dst, tmp, m, cf);
        std::copy(tmp + m, tmp + m + n, dst + m);
        cf = addc_1(dst + m, dst + m, n, 0u, cf);
        WJR_ASSERT(cf == 0);
    } else {
        if (6 * n < 7 * m) {
            toom33_mul_s(dst, src0, n, src1, m, stk);
        } else if (2 * n < 3 * m) {
            if (m <= toom32_to_toom43_mul_threshold) {
                toom32_mul_s(dst, src0, n, src1, m, stk);
            } else {
                toom43_mul_s(dst, src0, n, src1, m, stk);
            }
        } else {
            if (4 * n < 7 * m) {
                toom32_mul_s(dst, src0, n, src1, m, stk);
            } else {
                toom42_mul_s(dst, src0, n, src1, m, stk);
            }
        }
    }

    return;
}

template <__rec_mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __rec_mul_s(T *dst, const T *src0, size_t n, const T *src1,
                                      size_t m, T *stk) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n + m, src0, n));
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n + m, src1, m));

    if (n < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, m);
    }

    if (m < toom22_mul_threshold) {
        if (n >= m + toom22_mul_threshold / 4) {
            return basecase_mul_s(dst, src0, n, src1, m);
        }
        return toom22_mul_s(dst, src0, n, src1, m, stk);
    }

    if (mode <= __rec_mul_mode::toom22 || m < toom33_mul_threshold) {
        if (n >= 3 * m) {
            T *tmp = stk;
            stk += 4 * m;

            toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src1 += 2 * m;
            dst += 2 * m;

            T cf = 0;

            while (n >= 3 * m) {
                toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
                n -= 2 * m;
                src1 += 2 * m;

                cf = addc_n(dst, dst, tmp, m, cf);
                std::copy(tmp + m, tmp + 3 * m, dst + m);
                cf = addc_1(dst + m, dst + m, 2 * m, 0u, cf);

                dst += 2 * m;
            }

            if (4 * n < 5 * m) {
                toom22_mul_s(tmp, src0, n, src1, m, stk);
            } else if (4 * n < 7 * m) {
                toom32_mul_s(tmp, src0, n, src1, m, stk);
            } else {
                toom42_mul_s(tmp, src0, n, src1, m, stk);
            }

            cf = addc_n(dst, dst, tmp, m, cf);
            std::copy(tmp + m, tmp + m + n, dst + m);
            cf = addc_1(dst + m, dst + m, n, 0u, cf);
            WJR_ASSERT(cf == 0);
        } else {
            if (4 * n < 5 * m) {
                toom22_mul_s(dst, src0, n, src1, m, stk);
            } else if (4 * n < 7 * m) {
                toom32_mul_s(dst, src0, n, src1, m, stk);
            } else {
                toom42_mul_s(dst, src0, n, src1, m, stk);
            }
        }

        return;
    }

    if (n >= 3 * m) {
        T *tmp = stk;
        stk += 4 * m;

        toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
        n -= 2 * m;
        src1 += 2 * m;
        dst += 2 * m;

        T cf = 0;

        while (n >= 3 * m) {
            toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src1 += 2 * m;

            cf = addc_n(dst, dst, tmp, m, cf);
            std::copy(tmp + m, tmp + 3 * m, dst + m);
            cf = addc_1(dst + m, dst + m, 2 * m, 0u, cf);

            dst += 2 * m;
        }

        if (4 * n < 5 * m) {
            toom33_mul_s(tmp, src0, n, src1, m, stk);
        } else if (4 * n < 7 * m) {
            toom32_mul_s(tmp, src0, n, src1, m, stk);
        } else {
            toom42_mul_s(tmp, src0, n, src1, m, stk);
        }

        cf = addc_n(dst, dst, tmp, m, cf);
        std::copy(tmp + m, tmp + m + n, dst + m);
        cf = addc_1(dst + m, dst + m, n, 0u, cf);
        WJR_ASSERT(cf == 0);
    } else {
        if (4 * n < 5 * m) {
            toom33_mul_s(dst, src0, n, src1, m, stk);
        } else if (4 * n < 7 * m) {
            toom32_mul_s(dst, src0, n, src1, m, stk);
        } else {
            toom42_mul_s(dst, src0, n, src1, m, stk);
        }
    }

    return;
}

template <__rec_mul_mode mode, typename T>
WJR_INTRINSIC_INLINE void __rec_mul_n(T *dst, const T *src0, const T *src1, size_t n,
                                      T *stk) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n * 2, src0, n));
    WJR_ASSERT(WJR_IS_SAME_OR_INCR_P(dst, n * 2, src1, n));

    if (n < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, n);
    }

    if (mode <= __rec_mul_mode::toom22 || n < toom33_mul_threshold) {
        toom22_mul_s(dst, src0, n, src1, n, stk);
        return;
    }

    toom33_mul_s(dst, src0, n, src1, n, stk);
    return;
}

template <typename T>
void basecase_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m) {
    WJR_ASSERT_ASSUME(m >= 1);

    dst[n] = mul_1(dst, src0, n, src1[0]);
    for (size_t i = 1; i < m; ++i) {
        ++dst;
        dst[n] = addmul_1(dst, src0, n, src1[i]);
    }
}

template <typename T>
void toom22_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk) {
    // [u0(l), u1(rn)] (n)
    // [v0(l), v1(rm)] (m)

    // (u0 - u1) * (v0 - v1)

    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT_ASSUME(2 * m > n);

    const size_t l = (n + 1) / 2;
    const size_t rn = n - l;
    const size_t rm = m - l;

    auto u0 = src0;
    auto u1 = src0 + l;

    auto v0 = src1;
    auto v1 = src1 + l;

    const auto p0 = dst;
    const auto p1 = dst + l;
    const auto p2 = dst + l * 2;
    const auto p3 = dst + l * 3;

    auto wp = stk;
    stk += l * 2;

    bool f = 0;

    do {
        ssize_t p;
        p = abs_subc_s(p0, u0, l, u1, rn);
        if (p < 0) {
            f ^= 1;
        } else if (WJR_UNLIKELY(p == 0)) {
            set_n(wp, 0u, l * 2);
            break;
        }

        p = abs_subc_s(p1, v0, l, v1, rm);
        if (p < 0) {
            f ^= 1;
        } else if (WJR_UNLIKELY(p == 0)) {
            set_n(wp, 0u, l * 2);
            break;
        }

        __rec_mul_n<__rec_mul_mode::toom22>(wp, p0, p1, l, stk);
    } while (0);

    __rec_mul_n<__rec_mul_mode::toom22>(p0, u0, v0, l, stk);
    __rec_mul_s<__rec_mul_mode::toom22>(p2, u1, rn, v1, rm, stk);

    T cf = 0, cf2 = 0;

    cf = addc_n(p2, p1, p2, l);
    cf2 = cf + addc_n(p1, p0, p2, l);
    cf += addc_s(p2, p2, l, p3, rn + rm - l);

    if (!f) {
        cf -= subc_n(p1, p1, wp, l * 2);
    } else {
        cf += addc_n(p1, p1, wp, l * 2);
    }

    cf2 = addc_1(p2, p2, l, cf2);
    cf = addc_1(p3, p3, rn + rm - l, cf, cf2);
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom32_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk) {
    WJR_ASSERT_ASSUME(m + 2 <= n);
    WJR_ASSERT(n + 6 <= 3 * m);

    const size_t l = (2 * n >= 3 * m ? (n + 2) / 3 : (m + 1) / 2);
    const size_t rn = n - l * 2;
    const size_t rm = m - l;
    const size_t maxr = std::max(rn, rm);

    WJR_ASSERT(0 < rn && rn <= l);
    WJR_ASSERT(0 < rm && rm <= l);
    WJR_ASSERT(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;

    const auto v0p = src1;
    const auto v1p = src1 + l;

    auto w0p = dst;
    auto w1p = stk;
    auto w2p = stk + (l * 2);
    auto w3p = dst + l * 3;

    stk += l * 4;

    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0;
    bool neg0 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);
    // W3 = V0 + V1 : (non-negative) v(1)
    cf3 = addc_s(w3p, v0p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf3 <= 1);
    // W2 = W0 + U1 : (non-negative) u(1)
    cf2 = cf0 + addc_n(w2p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf2 <= 2);

    // W1 = W2 * W3 : (non-negative) r(1) = r1
    __rec_mul_n<__rec_mul_mode::toom22>(w1p, w2p, w3p, l, stk);
    cf1 = cf2 * cf3;
    cf1 += try_addmul_1(w1p + l, w3p, l, cf2, std::integral_constant<T, 2>{});
    cf1 += try_addmul_1(w1p + l, w2p, l, cf3, std::integral_constant<T, 1>{});

    // W0 = W0 - U1 : u(-1)
    if (cf0) {
        cf0 -= subc_n(w0p, w0p, u1p, l);
    } else {
        ssize_t p = abs_subc_n(w0p, w0p, u1p, l);
        neg0 = p < 0;
    }
    WJR_ASSERT_ASSUME(cf0 <= 1);

    // W3 = V0 - V1 : v(-1)
    {
        ssize_t p = abs_subc_s(w3p, v0p, l, v1p, rm);
        neg3 = p < 0;
    }

    // W2 = W0 * W3 : r(-1) = r2
    neg0 ^= neg3;
    __rec_mul_n<__rec_mul_mode::toom22>(w2p, w0p, w3p, l, stk);
    cf2 = 0;
    cf2 += try_addmul_1(w2p + l, w3p, l, cf0, std::integral_constant<T, 1>{});

    // W0 = U0 * V0 : (non-negative) r(0) = r0
    __rec_mul_n<__rec_mul_mode::toom22>(w0p, u0p, v0p, l, stk);

    // W3 = U2 * V1 : (non-negative) r(inf) = r3
    if (maxr == rn) {
        __rec_mul_s<__rec_mul_mode::toom22>(w3p, u2p, rn, v1p, rm, stk);
    } else {
        __rec_mul_s<__rec_mul_mode::toom22>(w3p, v1p, rm, u2p, rn, stk);
    }

    // W1 = (W1 - W2) >> 1 : (non-negative) (r(1) - r(-1)) / 2
    {
        if (!neg0) {
            cf1 = cf1 - cf2 - subc_n(w1p, w1p, w2p, l * 2);
        } else {
            cf1 = cf1 + cf2 + addc_n(w1p, w1p, w2p, l * 2);
        }

        rshift_n(w1p, w1p, l * 2, 1u, cf1);
        cf1 >>= 1;
    }

    // W2 = (W1 + W2) - W0 : (non-negative) r2
    {
        if (!neg0) {
            cf2 = cf1 + cf2 + addc_n(w2p, w1p, w2p, l * 2);
        } else {
            cf2 = cf1 - cf2 - subc_n(w2p, w1p, w2p, l * 2);
        }

        cf2 -= subc_n(w2p, w2p, w0p, l * 2);
        if (l != maxr) {
            WJR_ASSERT(cf2 == 0);
            cf2 = w2p[l + maxr];
        }
    }

    // W1 = W1 - W3 : (non-negative) r1
    cf1 -= subc_s(w1p, w1p, l * 2, w3p, rn + rm);

    // W = W3*x^3+W2*x^2+W1*x+W0
    cf0 = addc_n(w0p + l, w0p + l, w1p, l);
    cf0 = addc_n(dst + l * 2, w1p + l, w2p, l, cf0);
    cf0 = addc_n(w3p, w3p, w2p + l, maxr, cf0);
    cf2 += addc_1(w3p, w3p, maxr, cf1);
    cf0 = addc_1(w3p + maxr, w3p + maxr, (rn + rm) - maxr, cf2, cf0);
    WJR_ASSERT(cf0 == 0);
}

template <typename T>
struct toom_interpolation_5p_struct {
    bool neg1;
    T cf1;
    T cf2;
    T cf3;
};

template <typename T>
void toom_interpolation_5p_s(T *dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_5p_struct<T> &&flag) {
    // r(0) r(-1) r(1) r(2) r(inf)

    const size_t maxr = std::max(rn, rm);

    auto w0p = dst;
    auto w2p = w1p + l * 2;
    auto w3p = w1p + l * 4;
    auto w4p = dst + l * 4;

    auto [neg1, cf1, cf2, cf3] = flag;

    T cf;

    // W3 = (W3 - W1) / 3 : (non-negative) (r(2) - r(-1)) / 3
    {
        if (!neg1) {
            cf3 -= cf1 + subc_n(w3p, w3p, w1p, l * 2);
        } else {
            cf3 += cf1 + addc_n(w3p, w3p, w1p, l * 2);
        }

        cf3 /= 3;
        divexact_by3(w3p, w3p, l * 2);
    }

    // W1 = (W2 - W1) >> 1 : (non-negative) (r(1) - r(-1)) / 2
    {
        if (!neg1) {
            cf1 = cf2 - cf1 - subc_n(w1p, w2p, w1p, l * 2);
        } else {
            cf1 = cf2 + cf1 + addc_n(w1p, w2p, w1p, l * 2);
        }

        rshift_n(w1p, w1p, l * 2, 1u, cf1);
        cf1 >>= 1;
    }

    // W2 = W2 - W0 : (non-negative) r(1) - r(0)
    cf2 -= subc_n(w2p, w2p, w0p, l * 2);

    // W3 = ((W3 - W2) >> 1) - (W4 << 1) : (non-negative) r3
    {
        cf3 -= cf2 + subc_n(w3p, w3p, w2p, l * 2);
        if (maxr != l) {
            WJR_ASSERT(cf3 == 0);
            cf3 = w3p[l + maxr];
        }

        rshift_n(w3p, w3p, l + maxr, 1u, cf3);
        cf3 >>= 1;

        cf = submul_1(w3p, w4p, rn + rm, 2u);
        if ((l + maxr) != (rn + rm)) {
            cf3 -= subc_1(w3p + rn + rm, w3p + rn + rm, (l + maxr) - (rn + rm), cf);
        } else {
            cf3 -= cf;
        }
    }

    // W2 = W2 - W1 : (non-negative) r(1) / 2 - r(0) + r(-1) / 2
    cf2 -= cf1 + subc_n(w2p, w2p, w1p, l * 2);

    // W3 = W4 * x + W3 : r4 * x + r3
    cf = addc_n(w4p, w4p, w3p + l, maxr);
    cf = addc_1(w4p + maxr, w4p + maxr, rn + rm - maxr, cf3, cf);
    WJR_ASSERT(cf == 0);

    // W1 = W2 * x + W1 :
    cf = addc_n(w2p, w2p, w1p + l, l);
    cf2 += addc_1(w2p + l, w2p + l, l, cf1, cf);

    // W1 = W1 - W3 : // r2 * x + r1
    cf = subc_n(w1p, w1p, w3p, l);
    cf2 -= subc_s(dst + l * 2, w2p, l * 2, w4p, rn + rm, cf);

    // W  = W3*x^3 + W1*x + W0
    cf = addc_n(dst + l, dst + l, w1p, l);
    cf = addc_1(dst + l * 2, dst + l * 2, l, 0u, cf);
    cf = addc_n(dst + l * 3, dst + l * 3, w3p, l, cf);
    cf = addc_1(dst + l * 4, dst + l * 4, rn + rm, cf2, cf);
    WJR_ASSERT(cf == 0);
}

template <typename T>
void toom42_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk) {
    WJR_ASSERT_ASSUME(n >= m);

    const size_t l = (n >= 2 * m ? (n + 3) / 4 : (m + 1) / 2);
    const size_t rn = n - l * 3;
    const size_t rm = m - l;

    WJR_ASSERT(0 < rn && rn <= l);
    WJR_ASSERT(0 < rm && rm <= l);
    WJR_ASSERT(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;
    const auto u3p = src0 + l * 3;

    const auto v0p = src1;
    const auto v1p = src1 + l;

    auto w0p = dst;
    auto w1p = stk;
    auto w2p = stk + l * 2;
    auto w3p = stk + l * 4;
    auto w4p = dst + l * 4;

    stk += l * 6;

    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0;
    bool neg2 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_n(w0p, u0p, u2p, l);
    // W4 = V0 + V1 : (non-negative) v(1)
    cf4 = addc_s(w4p, v0p, l, v1p, rm);
    // W1 = U1 + U3 : (non-negative)
    cf1 = addc_s(w1p, u1p, l, u3p, rn);

    // W3 = W0 - W1 : u(-1)
    if (cf0 != cf1) {
        cf3 = 1;
        if (cf0) {
            cf3 -= subc_n(w3p, w0p, w1p, l);
        } else {
            neg3 = 1;
            cf3 -= subc_n(w3p, w1p, w0p, l);
        }
    } else {
        ssize_t p = abs_subc_n(w3p, w0p, w1p, l);
        neg3 = p < 0;
    }
    WJR_ASSERT_ASSUME(cf3 <= 1);

    // W2 = V0 - V1 : v(-1)
    {
        ssize_t p = abs_subc_s(w2p, v0p, l, v1p, rm);
        neg2 = p < 0;
    }

    // W0 = W0 + W1 : (non-negative) u(1)
    cf0 += cf1 + addc_n(w0p, w0p, w1p, l);
    WJR_ASSERT_ASSUME(cf0 <= 3);

    // W1 = W3 * W2 : r(-1)
    neg2 ^= neg3;
    __rec_mul_n<__rec_mul_mode::toom22>(w1p, w3p, w2p, l, stk);
    cf1 = 0;
    cf1 += try_addmul_1(w1p + l, w2p, l, cf3, std::integral_constant<T, 1>{});

    // W2 = W0 * W4 : (non-negative) r(1)
    __rec_mul_n<__rec_mul_mode::toom22>(w2p, w0p, w4p, l, stk);
    cf2 = cf0 * cf4;
    cf2 += try_addmul_1(w2p + l, w4p, l, cf0);
    cf2 += try_addmul_1(w2p + l, w0p, l, cf4, std::integral_constant<T, 1>{});

    // W0 = U0 +(U1 +(U2 + U3<<1)<<1)<<1 : (non-negative) u(2)
    {
        cf0 = addlsh_n(w0p, u2p, u3p, rn, 1u);
        if (l != rn) {
            cf0 = addc_1(w0p + rn, u2p + rn, l - rn, cf0);
        }

        cf0 += cf0 + addlsh_n(w0p, u1p, w0p, l, 1u);
        cf0 += cf0 + addlsh_n(w0p, u0p, w0p, l, 1u);
        WJR_ASSERT(cf0 <= 14);
    }

    // W4 = W4 + V1 : (non-negative) v(2)
    cf4 += addc_s(w4p, w4p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf4 <= 2);

    // W3 = W0 * W4 : (non-negative) r(2)
    __rec_mul_n<__rec_mul_mode::toom22>(w3p, w0p, w4p, l, stk);
    cf3 = cf0 * cf4;
    cf3 += try_addmul_1(w3p + l, w4p, l, cf0);
    cf3 += try_addmul_1(w3p + l, w0p, l, cf4, std::integral_constant<T, 2>{});

    // W0 = U0 * V0 : (non-negative) r(0) = r0
    __rec_mul_n<__rec_mul_mode::toom22>(w0p, u0p, v0p, l, stk);

    // W4 = U3 * V1 : (non-negative) r(inf) = r4
    if (rn >= rm) {
        __rec_mul_s<__rec_mul_mode::toom22>(w4p, u3p, rn, v1p, rm, stk);
    } else {
        __rec_mul_s<__rec_mul_mode::toom22>(w4p, v1p, rm, u3p, rn, stk);
    }

    return toom_interpolation_5p_s(dst, w1p, l, rn, rm,
                                   toom_interpolation_5p_struct<T>{neg2, cf1, cf2, cf3});
}

template <typename T>
void toom33_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk) {
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT(3 * m > 2 * n);

    const size_t l = (n + 2) / 3;
    const size_t rn = n - l * 2;
    const size_t rm = m - l * 2;

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;

    auto w0p = dst;
    auto w1p = stk;
    auto w2p = stk + l * 2;
    auto w3p = stk + l * 4;
    auto w4p = dst + l * 4;

    stk += l * 6;

    T cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0;
    bool neg2 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);
    // W4 = V0 + V2 : (non-negative)
    cf4 = addc_s(w4p, v0p, l, v2p, rm);

    // W3 = W0 - U1 : u(-1)
    if (cf0) {
        cf3 = cf0 - subc_n(w3p, w0p, u1p, l);
    } else {
        ssize_t p = abs_subc_n(w3p, w0p, u1p, l);
        neg3 = p < 0;
    }
    WJR_ASSERT_ASSUME(cf3 <= 1);

    // W2 = W4 - V1 : v(-1)
    if (cf4) {
        cf2 = cf4 - subc_n(w2p, w4p, v1p, l);
    } else {
        ssize_t p = abs_subc_n(w2p, w4p, v1p, l);
        neg2 = p < 0;
    }
    WJR_ASSERT_ASSUME(cf2 <= 1);

    // W0 = W0 + U1 : (non-negative) u(1)
    cf0 += addc_n(w0p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf0 <= 2);

    // W4 = W4 + V1 : (non-negative) v(1)
    cf4 += addc_n(w4p, w4p, v1p, l);
    WJR_ASSERT_ASSUME(cf4 <= 2);

    // W1 = W3 * W2 : r(-1)
    neg2 ^= neg3;
    __rec_mul_n<__rec_mul_mode::toom33>(w1p, w3p, w2p, l, stk);
    cf1 = cf2 && cf3;
    cf1 += try_addmul_1(w1p + l, w3p, l, cf2, std::integral_constant<T, 1>{});
    cf1 += try_addmul_1(w1p + l, w2p, l, cf3, std::integral_constant<T, 1>{});

    // W2 = W0 * W4 : (non-negative) r(1)
    __rec_mul_n<__rec_mul_mode::toom33>(w2p, w0p, w4p, l, stk);
    cf2 = cf0 * cf4;
    cf2 += try_addmul_1(w2p + l, w4p, l, cf0, std::integral_constant<T, 2>{});
    cf2 += try_addmul_1(w2p + l, w0p, l, cf4, std::integral_constant<T, 2>{});

    // W0 = (W0 + U2) << 1 - U0 : (non-negative) u(2)
    cf0 += addc_s(w0p, w0p, l, u2p, rn);
    WJR_ASSERT(cf0 <= 3);
    cf0 += cf0 + rsblsh_n(w0p, u0p, w0p, l, 1u);
    WJR_ASSERT(cf0 <= 6);

    // W4 = (W4 + V2) << 1 - V0 : (non-negative) v(2)
    cf4 += addc_s(w4p, w4p, l, v2p, rm);
    WJR_ASSERT(cf4 <= 3);
    cf4 += cf4 + rsblsh_n(w4p, v0p, w4p, l, 1u);
    WJR_ASSERT(cf4 <= 6);

    // W3 = W0 * W4 : (non-negative) r(2)
    __rec_mul_n<__rec_mul_mode::toom33>(w3p, w0p, w4p, l, stk);
    cf3 = cf0 * cf4;
    cf3 += try_addmul_1(w3p + l, w4p, l, cf0);
    cf3 += try_addmul_1(w3p + l, w0p, l, cf4);

    // W0 = U0 * V0 : (non-negative) r(0) = r0
    __rec_mul_n<__rec_mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    // W4 = U2 * V2 : (non-negative) r(inf) = r4
    __rec_mul_s<__rec_mul_mode::toom33>(w4p, u2p, rn, v2p, rm, stk);

    return toom_interpolation_5p_s(dst, w1p, l, rn, rm,
                                   toom_interpolation_5p_struct<T>{neg2, cf1, cf2, cf3});
}

template <typename T>
struct toom_interpolation_6p_struct {
    bool neg1;
    bool neg3;
    T cf1;
    T cf2;
    T cf3;
    T cf4;
};

template <typename T>
void toom_interpolation_6p_s(T *dst, T *w1p, size_t l, size_t rn, size_t rm,
                             toom_interpolation_6p_struct<T> &&flag) {
    // W0 = r(0);
    // W1 = r(-1);
    // W2 = r(1);
    // W3 = r(-2);
    // W4 = r(2);
    // W5 = r(inf)

    /*
     11 add/sub + 4 shift + 2 divby3 + 1 submul
     W3 = (W3 - W4) >> 1;
     W4 = W4 + W3;
     W4 = W4 - W0;
     W1 = W2 - W1;
     W2 = W2 - W0;
     W3 = W3 + W1;
     W1 >>= 1;
     W3 = W3 / -6; => W3 = W3 / -2 / 3;
     W2 = W2 - W1;
     W3 = W3 - 4 * W5;
     W1 = W1 - W3;
     W4 = W4 >> 2;
     W4 = W4 - W2;
     W4 = W4 / 3;
     // W3 = W3 - W5;
     // W2 = W2 - W4;
     W4 = W5 * x + W4;
     W2 = W3 * x + W2;
     W2 = W2 - W4;
     W = W4 * x ^ 4 + W2 * x ^ 2 + W1 * x + W0;
    */

    const size_t maxr = std::max(rn, rm);

    auto w0p = dst;
    auto w2p = w0p + l * 2;
    auto w3p = w1p + l * 2;
    auto w4p = w1p + l * 4;
    auto w5p = w0p + l * 5;

    auto [neg1, neg3, cf1, cf2, cf3, cf4] = flag;
    T cf;

    // W3 = (W3 - W4) >> 1;
    {
        if (!neg3) {
            cf3 = cf4 - cf3 - subc_n(w3p, w4p, w3p, l * 2);
        } else {
            cf3 += cf4 + addc_n(w3p, w3p, w4p, l * 2);
        }

        (void)rshift_n(w3p, w3p, l * 2, 1u, cf3);
        cf3 >>= 1;
    }

    // W4 = W4 + W3;
    cf4 -= cf3 + subc_n(w4p, w4p, w3p, l * 2);

    // W4 = W4 - W0;
    cf4 -= subc_n(w4p, w4p, w0p, l * 2);

    // W1 = W2 - W1;
    if (!neg1) {
        cf1 = cf2 - cf1 - subc_n(w1p, w2p, w1p, l * 2);
    } else {
        cf1 = cf2 + cf1 + addc_n(w1p, w2p, w1p, l * 2);
    }

    // W2 = W2 - W0;
    cf2 -= subc_n(w2p, w2p, w0p, l * 2);

    // W3 = W3 + W1;
    cf3 -= cf1 + subc_n(w3p, w3p, w1p, l * 2);

    // W1 >>= 1;
    (void)rshift_n(w1p, w1p, l * 2, 1u, cf1);
    cf1 >>= 1;

    // W3 = W3 / -6; = > W3 = W3 / -2 / 3;
    {
        (void)rshift_n(w3p, w3p, l * 2, 1u, cf3);
        divexact_by3(w3p, w3p, l * 2);
        cf3 /= 6;
    }

    // W2 = W2 - W1;
    cf2 -= cf1 + subc_n(w2p, w2p, w1p, l * 2);

    // W3 = W3 - 4 * W5;
    {
        cf = submul_1(w3p, w5p, rn + rm, 4u);
        if (rn + rm != l * 2) {
            cf3 -= subc_1(w3p + rn + rm, w3p + rn + rm, l * 2 - (rn + rm), cf);
        } else {
            cf3 -= cf;
        }
    }

    // W1 = W1 - W3;
    cf1 -= cf3 + subc_n(w1p, w1p, w3p, l * 2);

    // W4 = W4 >> 2;
    rshift_n(w4p, w4p, l * 2, 2u, cf4);
    cf4 >>= 2;

    // W4 = W4 - W2;
    cf4 -= cf2 + subc_n(w4p, w4p, w2p, l * 2);

    // W4 = W4 / 3;
    divexact_by3(w4p, w4p, l * 2);
    cf4 /= 3;
    if (maxr != l) {
        cf4 = w4p[l + maxr];
    }

    // W4 = W5 * x + W4;
    cf = addc_n(w5p, w5p, w4p + l, maxr);
    cf = addc_1(w5p + maxr, w5p + maxr, rn + rm - maxr, cf4, cf);
    WJR_ASSERT(cf == 0);

    // W2 = W3 * x + W2;
    cf = addc_n(w3p, w3p, w2p + l, l);
    cf3 += addc_1(w3p + l, w3p + l, l, cf2, cf);

    // W2 = W2 - W4;
    cf = subc_n(w2p, w2p, w4p, l);
    cf3 -= subc_s(dst + l * 3, w3p, l * 2, w5p, rn + rm, cf);

    // W = W4 * x ^ 4 + W2 * x ^ 2 + W1 * x + W0;
    cf = addc_n(dst + l, dst + l, w1p, l);
    cf = addc_n(dst + l * 2, dst + l * 2, w1p + l, l, cf);
    cf = addc_1(dst + l * 3, dst + l * 3, l, cf1, cf);
    cf = addc_n(dst + l * 4, dst + l * 4, w4p, l, cf);
    cf = addc_1(dst + l * 5, dst + l * 5, rn + rm, cf3, cf);
    WJR_ASSERT(cf == 0);
}

// TODO : TOOM-COOK 3.5/4/4.5

template <typename T>
void toom43_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk) {

    /*
     W0 = U0 * V0;
     W1 = (U0 - U1 + U2 - U3) * (V0 - V1 + V2);
     W2 = (U0 + U1 + U2 + U3) * (V0 + V1 + V2);
     W3 = (U0 - 2U1 + 4U2 - 8U3) * (V0 - 2V2 + 4V3);
     W4 = (U0 + 2U1 + 4U2 + 8U3) * (V0 + 2V2 + 4V3);
     W5 = U3 * V2;
    */

    /*
        dst :
        |--- 2 * l ---|--- 2 * l ---|- l --|--- 2 * l ---|
        W0             W2            T2    W5
        |- l --|- l --|
        T0      T1
        stk :
        |--- 2 * l ---|--- 2 * l ---|--- 2 * l ---|
        W1             W3            W4
     */

    /*
     11 add/sub + 2 rsblsh + 2 addmul + 1 shift

     W3 = U0 + U2;
     W4 = U1 + U3;
     T0 = W3 - W4; u(-1)
     T1 = W3 + W4; u(1)
     W3 = V0 + V2;
     W4 = W3 - V1; v(-1)
     W1 = T0 * W4; r(-1)
     W5 = W3 + V1; v(1)
     W2 = T1 * W5; r(1)
     W4 = (W4 + V2) << 1 - V0; v(-2)
     W5 = (W5 + V2) << 1 - V0; v(2)
     T0 = U0 + 4U2;
     T1 = (U1 + 4U3) << 1;
     T2 = T0 - T1; u(-2)
     W3 = T2 * W4; r(-2)
     T2 = T0 + T1; u(2)
     W4 = T2 * W5; r(2)
     W0 = U0 * V0;
     W5 = U3 * V2;
     */

    const size_t l = (3 * n >= 4 * m ? (n + 3) / 4 : (m + 2) / 3);
    const size_t rn = n - l * 3;
    const size_t rm = m - l * 2;

    WJR_ASSERT(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = u0p + l;
    const auto u2p = u0p + l * 2;
    const auto u3p = u0p + l * 3;

    const auto v0p = src1;
    const auto v1p = v0p + l;
    const auto v2p = v0p + l * 2;

    auto w0p = dst;
    auto w1p = stk;
    auto w2p = w0p + l * 2;
    auto w3p = stk + l * 2;
    auto w4p = stk + l * 4;
    auto w5p = w0p + l * 5;

    auto t0p = w0p;
    auto t1p = t0p + l;
    auto t2p = w0p + l * 4;

    stk += l * 6;

    // Hand it over to the compiler to optimize reusable variables
    T cf = 0;
    T cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    T cft0 = 0, cft1 = 0, cft2 = 0;
    // Reusable
    // not point to real W0...W5 or T0...T2
    bool neg0 = 0, neg1 = 0, neg2 = 0;

    //  W3 = U0 + U2;
    cf3 = addc_n(w3p, u0p, u2p, l);

    //  W4 = U1 + U3;
    cf4 = addc_s(w4p, u1p, l, u3p, rn);

    //  T0 = W3 - W4; u(-1)
    {
        if (cf3 != cf4) {
            cft0 = 1;
            if (cf3) {
                cft0 -= subc_n(t0p, w3p, w4p, l);
            } else {
                cft0 -= subc_n(t0p, w4p, w3p, l);
                neg0 = 1;
            }
        } else {
            ssize_t p = abs_subc_n(t0p, w3p, w4p, l);
            neg0 = p < 0;
        }
        WJR_ASSERT(cft0 <= 1);
    }

    //  T1 = W3 + W4; u(1)
    cft1 = cf3 + cf4 + addc_n(t1p, w3p, w4p, l);
    WJR_ASSERT(cft1 <= 3);

    //  W3 = V0 + V2;
    cf3 = addc_s(w3p, v0p, l, v2p, rm);

    //  W4 = W3 - V1; v(-1)
    if (cf3) {
        cf4 = cf3 - subc_n(w4p, w3p, v1p, l);
    } else {
        cf4 = 0;
        ssize_t p = abs_subc_n(w4p, w3p, v1p, l);
        neg1 = p < 0;
    }
    WJR_ASSERT(cf4 <= 1);

    //  W1 = T0 * W4; r(-1)
    neg0 ^= neg1;
    __rec_mul_n<__rec_mul_mode::toom33>(w1p, t0p, w4p, l, stk);
    cf1 = cft0 * cf4;
    cf1 += try_addmul_1(w1p + l, w4p, l, cft0, std::integral_constant<T, 1>{});
    cf1 += try_addmul_1(w1p + l, t0p, l, cf4, std::integral_constant<T, 1>{});

    //  W5 = W3 + V1; v(1)
    cf5 = cf3 + addc_n(w5p, w3p, v1p, l);
    WJR_ASSERT(cf5 <= 2);

    //  W2 = T1 * W5; r(1)
    __rec_mul_n<__rec_mul_mode::toom33>(w2p, t1p, w5p, l, stk);
    cf2 = cft1 * cf5;
    cf2 += try_addmul_1(w2p + l, w5p, l, cft1, std::integral_constant<T, 3>{});
    cf2 += try_addmul_1(w2p + l, t1p, l, cf5, std::integral_constant<T, 2>{});

    //  W4 = (W4 + V2) << 1 - V0; v(-2)
    {
        if (!neg1) {
            cf4 += addc_s(w4p, w4p, l, v2p, rm);
        } else {
            WJR_ASSERT(cf4 == 0);
            ssize_t p = abs_subc_s(w4p, w4p, l, v2p, rm);
            neg1 = p > 0;
        }

        if (!neg1) {
            // W4 maybe less than V0
            // use lshfit + compare sub instead of rsblsh_n
            cf4 += cf4 + lshift_n(w4p, w4p, l, 1u);
            if (cf4) {
                cf4 -= subc_n(w4p, w4p, v0p, l);
            } else {
                ssize_t p = abs_subc_n(w4p, w4p, v0p, l);
                neg1 = p < 0;
            }
        } else {
            cf4 += cf4 + addlsh_n(w4p, v0p, w4p, l, 1u);
        }
    }
    WJR_ASSERT(cf4 <= 4);

    //  W5 = (W5 + V2) << 1 - V0; v(2)
    cf5 += addc_s(w5p, w5p, l, v2p, rm);
    cf5 += cf5 + rsblsh_n(w5p, v0p, w5p, l, 1u);
    WJR_ASSERT(cf5 <= 6);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2u);
    WJR_ASSERT(cft0 <= 4);

    //  T1 = (U1 + 4U3) << 1;
    cft1 = addlsh_n(t1p, u1p, u3p, rn, 2u);
    if (l != rn) {
        cft1 = addc_1(t1p + rn, u1p + rn, l - rn, cft1);
    }
    cft1 += cft1 + lshift_n(t1p, t1p, l, 1u);
    WJR_ASSERT(cft1 <= 9);

    //  T2 = T0 - T1; u(-2)
    if (cft0 != cft1) {
        if (cft0 >= cft1) {
            cft2 = cft0 - cft1 - subc_n(t2p, t0p, t1p, l);
        } else {
            cft2 = cft1 - cft0 - subc_n(t2p, t1p, t0p, l);
            neg2 = 1;
        }
    } else {
        cft2 = 0;
        ssize_t p = abs_subc_n(t2p, t0p, t1p, l);
        neg2 = p < 0;
    }
    WJR_ASSERT(cft2 <= 9);

    //  W3 = T2 * W4; r(-2)
    neg1 ^= neg2;
    __rec_mul_n<__rec_mul_mode::toom33>(w3p, t2p, w4p, l, stk);
    cf3 = cft2 * cf4;
    cf3 += try_addmul_1(w3p + l, w4p, l, cft2);
    cf3 += try_addmul_1(w3p + l, t2p, l, cf4);

    //  T2 = T0 + T1; u(2)
    cft2 = cft0 + cft1 + addc_n(t2p, t0p, t1p, l);
    WJR_ASSERT(cft2 <= 14);

    //  W4 = T2 * W5; r(2)
    __rec_mul_n<__rec_mul_mode::toom33>(w4p, t2p, w5p, l, stk);
    cf4 = cft2 * cf5;
    cf4 += try_addmul_1(w4p + l, w5p, l, cft2);
    cf4 += try_addmul_1(w4p + l, t2p, l, cf5);

    //  W0 = U0 * V0;
    __rec_mul_n<__rec_mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    //  W5 = U3 * V2;
    if (rn >= rm) {
        __rec_mul_s<__rec_mul_mode::toom33>(w5p, u3p, rn, v2p, rm, stk);
    } else {
        __rec_mul_s<__rec_mul_mode::toom33>(w5p, v2p, rm, u3p, rn, stk);
    }

    toom_interpolation_6p_s(
        dst, w1p, l, rn, rm,
        toom_interpolation_6p_struct<T>{neg0, neg1, cf1, cf2, cf3, cf4});
}

template <typename T>
void toom52_mul_s(T *dst, const T *src0, size_t n, const T *src1, size_t m, T *stk) {}

} // namespace wjr

#endif // WJR_MATH_MUL_HPP__