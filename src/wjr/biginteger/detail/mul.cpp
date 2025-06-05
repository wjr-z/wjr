#include <wjr/memory/copy.hpp>
#include <wjr/memory/stack_allocator.hpp>

#include "mul-helper.hpp"

namespace wjr {

namespace {
inline constexpr size_t toom22_mul_threshold = WJR_TOOM22_MUL_THRESHOLD;
inline constexpr size_t toom33_mul_threshold = WJR_TOOM33_MUL_THRESHOLD;
inline constexpr size_t toom44_mul_threshold = WJR_TOOM44_MUL_THRESHOLD;
inline constexpr size_t toom55_mul_threshold = WJR_TOOM55_MUL_THRESHOLD;
inline constexpr size_t toom32_to_toom43_mul_threshold = WJR_TOOM32_TO_TOOM43_MUL_THRESHOLD;
inline constexpr size_t toom32_to_toom53_mul_threshold = WJR_TOOM32_TO_TOOM53_MUL_THRESHOLD;
inline constexpr size_t toom42_to_toom53_mul_threshold = WJR_TOOM42_TO_TOOM53_MUL_THRESHOLD;
inline constexpr size_t toom42_to_toom63_mul_threshold = WJR_TOOM42_TO_TOOM63_MUL_THRESHOLD;

inline constexpr size_t toom2_sqr_threshold = WJR_TOOM2_SQR_THRESHOLD;
inline constexpr size_t toom3_sqr_threshold = WJR_TOOM3_SQR_THRESHOLD;
inline constexpr size_t toom4_sqr_threshold = WJR_TOOM4_SQR_THRESHOLD;
inline constexpr size_t toom5_sqr_threshold = WJR_TOOM5_SQR_THRESHOLD;

// todo: itch ...

constexpr size_t toom22_s_itch(size_t m) noexcept { return m * 4 + (m / 2) + 64; }
constexpr size_t toom22_n_itch(size_t n) noexcept { return n * 2 + 64; }

constexpr size_t toom33_s_itch(size_t m) noexcept { return m * 4 + (m / 2) + 64; }
constexpr size_t toom33_n_itch(size_t m) noexcept { return m * 2 + 64; }

constexpr size_t toom44_n_itch(size_t m) noexcept { return m * 2 + 64; }

constexpr size_t toom55_n_itch(size_t m) noexcept { return m * 3 + (m / 2) + 32; }

template <typename T>
WJR_INTRINSIC_INLINE uint64_t *__mul_s_allocate(T &al, WJR_MAYBE_UNUSED size_t n) noexcept {
    if constexpr (std::is_same_v<T, uint64_t *>) {
        return al;
    } else {
        return al.template allocate<uint64_t>(n);
    }
}

WJR_INTRINSIC_INLINE void __inline_mul_n_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                              const uint64_t *src1, size_t n,
                                              uint64_t *mal) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n * 2, src0, n));
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n * 2, src1, n));

    if (n < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, n);
    }

    uint64_t *stk = __mul_s_allocate(mal, toom22_n_itch(n));
    return toom22_mul_s(dst, src0, n, src1, n, stk);
}

WJR_INTRINSIC_INLINE void __inline_sqr_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                            size_t n, uint64_t *mal) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n * 2, src, n));

    if (n < toom2_sqr_threshold) {
        return basecase_sqr(dst, src, n);
    }

    uint64_t *stk = __mul_s_allocate(mal, toom22_n_itch(n));
    return toom2_sqr(dst, src, n, stk);
}

} // namespace

namespace {
void __toom22_mul_s_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                         const uint64_t *src1, size_t m, uint64_t *mal) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n + m, src0, n));
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n + m, src1, m));

    if (m < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, m);
    }

    uint64_t *stk = __mul_s_allocate(mal, toom22_s_itch(m));

    if (n >= 3 * m) {
        unique_stack_allocator stkal;
        uint64_t *tmp = __mul_s_allocate(stkal, (4 * m));

        toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
        n -= 2 * m;
        src0 += 2 * m;
        dst += 2 * m;

        uint64_t cf = 0;

        while (n >= 3 * m) {
            toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;

            cf = addc_n(dst, dst, tmp, m, cf);
            copy_restrict(tmp + m, tmp + 3 * m, dst + m);
            cf = addc_1(dst + m, dst + m, 2 * m, 0, cf);

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
        copy_restrict(tmp + m, tmp + m + n, dst + m);
        cf = addc_1(dst + m, dst + m, n, 0, cf);
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
}

} // namespace

void __mul_s_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m) noexcept {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n + m, src0, n));
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n + m, src1, m));

    if (m < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, m);
    }

    unique_stack_allocator stkal;

    if (m < toom33_mul_threshold) {
        uint64_t *stk = __mul_s_allocate(stkal, toom22_s_itch(m));

        if (n >= 3 * m) {
            uint64_t *tmp = __mul_s_allocate(stkal, (4 * m));

            toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
            n -= 2 * m;
            src0 += 2 * m;
            dst += 2 * m;

            uint64_t cf = 0;

            while (n >= 3 * m) {
                toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
                n -= 2 * m;
                src0 += 2 * m;

                cf = addc_n(dst, dst, tmp, m, cf);
                copy_restrict(tmp + m, tmp + 3 * m, dst + m);
                cf = addc_1(dst + m, dst + m, 2 * m, 0, cf);

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
            copy_restrict(tmp + m, tmp + m + n, dst + m);
            cf = addc_1(dst + m, dst + m, n, 0, cf);
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

    do {
        if (m < toom44_mul_threshold)
            break;

        if (m < toom55_mul_threshold) {
            if (!toom44_ok(n, m))
                break;

            uint64_t *stk = __mul_s_allocate(stkal, toom44_n_itch(m));
            toom44_mul_s(dst, src0, n, src1, m, stk);
            return;
        }

        if (((n + m) >> 1) >= WJR_MUL_FFT_THRESHOLD && 3 * m >= WJR_MUL_FFT_THRESHOLD)
            goto FFT_MUL;

        if (!toom55_ok(n, m))
            break;

        uint64_t *stk = __mul_s_allocate(stkal, toom55_n_itch(m));
        toom55_mul_s(dst, src0, n, src1, m, stk);
        return;
    } while (0);

    if (2 * n >= 5 * m) {
        uint64_t *tmp = __mul_s_allocate(stkal, (4 * m));
        uint64_t *stk = __mul_s_allocate(stkal, toom22_s_itch(m));

        if (m < toom42_to_toom63_mul_threshold) {
            toom42_mul_s(dst, src0, 2 * m, src1, m, stk);
        } else {
            toom63_mul_s(dst, src0, 2 * m, src1, m, stk);
        }

        n -= 2 * m;
        src0 += 2 * m;
        dst += 2 * m;

        uint64_t cf = 0;

        while (2 * n >= 5 * m) {
            if (m < toom42_to_toom63_mul_threshold) {
                toom42_mul_s(tmp, src0, 2 * m, src1, m, stk);
            } else {
                toom63_mul_s(tmp, src0, 2 * m, src1, m, stk);
            }

            n -= 2 * m;
            src0 += 2 * m;

            cf = addc_n(dst, dst, tmp, m, cf);
            copy_restrict(tmp + m, tmp + 3 * m, dst + m);
            cf = addc_1(dst + m, dst + m, 2 * m, 0, cf);

            dst += 2 * m;
        }

        if (n >= m) {
            mul_s(tmp, src0, n, src1, m);
        } else {
            mul_s(tmp, src1, m, src0, n);
        }

        cf = addc_n(dst, dst, tmp, m, cf);
        copy_restrict(tmp + m, tmp + m + n, dst + m);
        cf = addc_1(dst + m, dst + m, n, 0, cf);
        WJR_ASSERT(cf == 0);
    } else {
        uint64_t *stk = __mul_s_allocate(stkal, toom33_s_itch(m));

        if (6 * n < 7 * m) {
            toom33_mul_s(dst, src0, n, src1, m, stk);
        } else if (2 * n < 3 * m) {
            if (m < toom32_to_toom43_mul_threshold) {
                toom32_mul_s(dst, src0, n, src1, m, stk);
            } else {
                toom43_mul_s(dst, src0, n, src1, m, stk);
            }
        } else if (6 * n < 11 * m) {
            if (4 * n < 7 * m) {
                if (m < toom32_to_toom53_mul_threshold) {
                    toom32_mul_s(dst, src0, n, src1, m, stk);
                } else {
                    toom53_mul_s(dst, src0, n, src1, m, stk);
                }
            } else {
                if (m < toom42_to_toom53_mul_threshold) {
                    toom42_mul_s(dst, src0, n, src1, m, stk);
                } else {
                    toom53_mul_s(dst, src0, n, src1, m, stk);
                }
            }
        } else {
            if (m < toom42_to_toom63_mul_threshold) {
                toom42_mul_s(dst, src0, n, src1, m, stk);
            } else {
                toom63_mul_s(dst, src0, n, src1, m, stk);
            }
        }
    }

    return;

FFT_MUL:

    if (n >= 8 * m) {
        uint64_t *tmp;

        /* The maximum tmp usage is for the mpn_mul result.  */
        tmp = stkal.template allocate<uint64_t>(9 * m >> 1);

        fft_mul(dst, src0, 3 * m, src1, m);
        n -= 3 * m;
        src0 += 3 * m;
        dst += 3 * m;

        uint64_t cf = 0;

        while (2 * n >= 7 * m) { /* n >= 3.5vn  */
            fft_mul(tmp, src0, 3 * m, src1, m);
            n -= 3 * m;
            src0 += 3 * m;

            cf = addc_n(dst, dst, tmp, m, cf);
            copy_restrict(tmp + m, tmp + 4 * m, dst + m);
            cf = addc_1(dst + m, dst + m, 3 * m, 0, cf);

            dst += 3 * m;
        }

        /* m / 2 <= n < 3.5vn */

        if (n < m)
            mul_s(tmp, src1, m, src0, n);
        else
            mul_s(tmp, src0, n, src1, m);

        cf = addc_n(dst, dst, tmp, m, cf);
        copy_restrict(tmp + m, tmp + m + n, dst + m);
        cf = addc_1(dst + m, dst + m, n, 0, cf);
        WJR_ASSERT(cf == 0);
    } else {
        fft_mul(dst, src0, n, src1, m);
    }
}

void __mul_n_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, const uint64_t *src1,
                  size_t n) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n * 2, src0, n));
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n * 2, src1, n));

    if (n < toom22_mul_threshold) {
        return basecase_mul_s(dst, src0, n, src1, n);
    }

    unique_stack_allocator stkal;

    if (n < toom33_mul_threshold) {
        uint64_t *stk = __mul_s_allocate(stkal, toom22_n_itch(n));
        return toom22_mul_s(dst, src0, n, src1, n, stk);
    }

    if (n < toom44_mul_threshold) {
        uint64_t *stk = __mul_s_allocate(stkal, toom33_n_itch(n));
        return toom33_mul_s(dst, src0, n, src1, n, stk);
    }

    if (n < toom55_mul_threshold) {
        uint64_t *stk = __mul_s_allocate(stkal, toom44_n_itch(n));
        return toom44_mul_s(dst, src0, n, src1, n, stk);
    }

    if (n < WJR_MUL_FFT_THRESHOLD) {
        uint64_t *stk = __mul_s_allocate(stkal, toom55_n_itch(n));
        return toom55_mul_s(dst, src0, n, src1, n, stk);
    }

    return fft_mul(dst, src0, n, src1, n);
}

void __sqr_impl(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n * 2, src, n));

    if (n < toom2_sqr_threshold) {
        return basecase_sqr(dst, src, n);
    }

    unique_stack_allocator stkal;

    if (n < toom3_sqr_threshold) {
        uint64_t *stk = __mul_s_allocate(stkal, toom22_n_itch(n));
        return toom2_sqr(dst, src, n, stk);
    }

    if (n < toom4_sqr_threshold) {
        uint64_t *stk = __mul_s_allocate(stkal, toom33_n_itch(n));
        return toom3_sqr(dst, src, n, stk);
    }

    if (n < toom5_sqr_threshold) {
        uint64_t *stk = __mul_s_allocate(stkal, toom44_n_itch(n));
        return toom4_sqr(dst, src, n, stk);
    }

    if (n < WJR_SQR_FFT_THRESHOLD) {
        uint64_t *stk = __mul_s_allocate(stkal, toom55_n_itch(n));
        return toom5_sqr(dst, src, n, stk);
    }

    return fft_mul(dst, src, n, src, n);
}

void toom22_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m, uint64_t *stk) noexcept {
    // [u0(l), u1(rn)] (n)
    // [v0(l), v1(rm)] (m)

    // (u0 - u1) * (v0 - v1)

    WJR_ASSERT(m <= n && n < 2 * m);

    const size_t l = (n + 1) / 2;
    const size_t rn = n - l;
    const size_t rm = m - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);
    WJR_ASSERT_ASSUME(l - rn <= 1);

    const auto *const u0 = src0;
    const auto *const u1 = src0 + l;

    const auto *const v0 = src1;
    const auto *const v1 = src1 + l;

    auto *const p0 = dst;
    auto *const p1 = dst + l;
    auto *const p2 = dst + l * 2;
    auto *const p3 = dst + l * 3;

    auto *const wp = stk;
    stk += l * 2;

    bool f = false;

    do {
        if (const ssize_t p = abs_subc_s1(p0, u0, l, u1, rn); p < 0) {
            f ^= 1;
        } else if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        if (const ssize_t p = abs_subc_s(p1, v0, l, v1, rm); p < 0) {
            f ^= 1;
        } else if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        __inline_mul_n_impl(wp, p0, p1, l, stk);
        break;
    ZERO:
        set_n(wp, 0, l * 2);
        break;
    } while (0);

    __inline_mul_n_impl(p0, u0, v0, l, stk);
    __toom22_mul_s_impl(p2, u1, rn, v1, rm, stk);

    uint64_t cf = 0, cf2 = 0;

    cf = addc_n(p2, p1, p2, l);
    cf2 = cf + addc_n(p1, p0, p2, l);
    cf += addc_sz(p2, p2, l, p3, rn + rm - l);

    if (!f) {
        cf -= subc_n(p1, p1, wp, l * 2);
    } else {
        cf += addc_n(p1, p1, wp, l * 2);
    }

    cf2 = addc_1(p2, p2, l, cf2);
    if (WJR_LIKELY(rn + rm != l)) {
        cf = addc_1(p3, p3, rn + rm - l, cf, cf2);
    }
    WJR_ASSERT(cf == 0);
}

void toom2_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n, uint64_t *stk) noexcept {
    const size_t l = (n + 1) / 2;
    const size_t rn = n - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(rn * 2 >= l);

    const auto *const u0 = src;
    const auto *const u1 = src + l;

    auto *const p0 = dst;
    auto *const p1 = dst + l;
    auto *const p2 = dst + l * 2;
    auto *const p3 = dst + l * 3;

    auto *const wp = stk;
    stk += l * 2;

    do {
        if (const ssize_t p = abs_subc_s1(p0, u0, l, u1, rn); WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        __inline_sqr_impl(wp, p0, l, stk);
        break;
    ZERO:
        set_n(wp, 0, l * 2);
        break;
    } while (0);

    __inline_sqr_impl(p0, u0, l, stk);
    __inline_sqr_impl(p2, u1, rn, stk);

    uint64_t cf = 0, cf2 = 0;

    cf = addc_n(p2, p1, p2, l);
    cf2 = cf + addc_n(p1, p0, p2, l);
    cf += addc_sz(p2, p2, l, p3, rn * 2 - l);

    cf -= subc_n(p1, p1, wp, l * 2);

    cf2 = addc_1(p2, p2, l, cf2);
    if (WJR_LIKELY(rn * 2 != l)) {
        cf = addc_1(p3, p3, rn * 2 - l, cf, cf2);
    }
    WJR_ASSERT(cf == 0);
}

void toom32_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m, uint64_t *stk) noexcept {
    WJR_ASSERT(m + 2 <= n && n + 8 <= 3 * m);

    const size_t l = (2 * n >= 3 * m ? (n + 2) / 3 : (m + 1) / 2);
    const size_t rn = n - l * 2;
    const size_t rm = m - l;
    const size_t maxr = std::max(rn, rm);

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto *const u0p = src0;
    const auto *const u1p = src0 + l;
    const auto *const u2p = src0 + l * 2;

    const auto *const v0p = src1;
    const auto *const v1p = src1 + l;

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w1p + l * 2;
    auto *const w3p = dst + l * 3;

    stk += l * 4;

    uint64_t cf = 0;
    uint64_t cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0;
    bool neg0 = false, neg3 = false;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);

    // W3 = V0 + V1 : (non-negative) v(1)
    cf3 = addc_s(w3p, v0p, l, v1p, rm);
    WJR_ASSERT(cf3 <= 1);

    // W2 = W0 + U1 : (non-negative) u(1)
    cf2 = cf0 + addc_n(w2p, w0p, u1p, l);
    WJR_ASSERT(cf2 <= 2);

    // W1 = W2 * W3 : (non-negative) f(1) = r1
    __mul_n<2, 1>(w1p, w2p, w3p, l, stk, cf1, cf2, cf3);

    // W0 = W0 - U1 : u(-1)
    neg0 = abs_subc_n(w0p, w0p, u1p, l, cf0, cf0, 0) < 0;
    WJR_ASSERT(cf0 <= 1);

    // W3 = V0 - V1 : v(-1)
    neg3 = abs_subc_s(w3p, v0p, l, v1p, rm) < 0;

    // W2 = W0 * W3 : f(-1) = r2
    neg0 ^= neg3;
    __mul_n<1, 0>(w2p, w0p, w3p, l, stk, cf2, cf0, 0);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    __mul_n(w0p, u0p, v0p, l, stk);

    // W3 = U2 * V1 : (non-negative) f(inf) = r3
    if (rn >= rm) {
        mul_s(w3p, u2p, rn, v1p, rm);
    } else {
        mul_s(w3p, v1p, rm, u2p, rn);
    }

    // W1 = (W1 - W2) >> 1 : (non-negative) (f(1) - f(-1)) / 2
    {
        if (!neg0) {
            cf1 = cf1 - cf2 - subc_n(w1p, w1p, w2p, l * 2);
        } else {
            cf1 = cf1 + cf2 + addc_n(w1p, w1p, w2p, l * 2);
        }

        rshift_n(w1p, w1p, l * 2, 1, cf1);
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
    cf = addc_n(w0p + l, w0p + l, w1p, l);
    cf = addc_n(dst + l * 2, w1p + l, w2p, l, cf);
    cf = addc_n(w3p, w3p, w2p + l, maxr, cf);
    cf2 += addc_1(w3p, w3p, maxr, cf1);
    cf = addc_1(w3p + maxr, w3p + maxr, (rn + rm) - maxr, cf2, cf);
    WJR_ASSERT(cf == 0);
}
} // namespace wjr