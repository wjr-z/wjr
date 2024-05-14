#include <wjr/math/div.hpp>
#include <wjr/math/mul.hpp>

namespace wjr {

template void __noinline_mul_s_impl<true>(uint64_t *WJR_RESTRICT dst,
                                          const uint64_t *src0, size_t n,
                                          const uint64_t *src1, size_t m, uint64_t *mal);

template void __noinline_mul_s_impl<false>(uint64_t *WJR_RESTRICT dst,
                                           const uint64_t *src0, size_t n,
                                           const uint64_t *src1, size_t m,
                                           in_place_empty_t mal);

void toom22_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                  const uint64_t *src1, size_t m, uint64_t *stk) {
    // [u0(l), u1(rn)] (n)
    // [v0(l), v1(rm)] (m)

    // (u0 - u1) * (v0 - v1)

    WJR_ASSERT_ASSUME(m <= n && n < 2 * m);

    const size_t l = (n + 1) / 2;
    const size_t rn = n - l;
    const size_t rm = m - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);
    WJR_ASSERT_ASSUME(l - rn <= 1);

    const auto u0 = src0;
    const auto u1 = src0 + l;

    const auto v0 = src1;
    const auto v1 = src1 + l;

    const auto p0 = dst;
    const auto p1 = dst + l;
    const auto p2 = dst + l * 2;
    const auto p3 = dst + l * 3;

    const auto wp = stk;
    stk += l * 2;

    bool f = 0;

    do {
        ssize_t p;
        p = abs_subc_s(p0, u0, l, u1, rn);
        if (p < 0) {
            f ^= 1;
        } else if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        p = abs_subc_s(p1, v0, l, v1, rm);
        if (p < 0) {
            f ^= 1;
        } else if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        __mul_n<__mul_mode::toom22>(wp, p0, p1, l, stk);
        break;
    ZERO:
        set_n(wp, 0, l * 2);
        break;
    } while (0);

    __mul_n<__mul_mode::toom22>(p0, u0, v0, l, stk);
    __mul_s<__mul_mode::toom22>(p2, u1, rn, v1, rm, stk);

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

void toom2_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n, uint64_t *stk) {
    const size_t l = (n + 1) / 2;
    const size_t rn = n - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(rn * 2 >= l);

    const auto u0 = src;
    const auto u1 = src + l;

    const auto p0 = dst;
    const auto p1 = dst + l;
    const auto p2 = dst + l * 2;
    const auto p3 = dst + l * 3;

    const auto wp = stk;
    stk += l * 2;

    do {
        ssize_t p = abs_subc_s(p0, u0, l, u1, rn);
        if (WJR_UNLIKELY(p == 0)) {
            goto ZERO;
        }

        __sqr<__mul_mode::toom22>(wp, p0, l, stk);
        break;
    ZERO:
        set_n(wp, 0, l * 2);
        break;
    } while (0);

    __sqr<__mul_mode::toom22>(p0, u0, l, stk);
    __sqr<__mul_mode::toom22>(p2, u1, rn, stk);

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

void toom32_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                  const uint64_t *src1, size_t m, uint64_t *stk) {
    WJR_ASSERT_ASSUME(m + 2 <= n && n + 8 <= 3 * m);

    const size_t l = (2 * n >= 3 * m ? (n + 2) / 3 : (m + 1) / 2);
    const size_t rn = n - l * 2;
    const size_t rm = m - l;
    const size_t maxr = std::max(rn, rm);

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;

    const auto v0p = src1;
    const auto v1p = src1 + l;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = stk + l * 2;
    const auto w3p = dst + l * 3;

    stk += l * 4;

    uint64_t cf = 0;
    uint64_t cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0;
    bool neg0 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);

    // W3 = V0 + V1 : (non-negative) v(1)
    cf3 = addc_s(w3p, v0p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf3 <= 1);

    // W2 = W0 + U1 : (non-negative) u(1)
    cf2 = cf0 + addc_n(w2p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf2 <= 2);

    // W1 = W2 * W3 : (non-negative) f(1) = r1
    __mul_n<__mul_mode::toom33, 2, 1>(w1p, w2p, w3p, l, stk, cf1, cf2, cf3);

    // W0 = W0 - U1 : u(-1)
    {
        ssize_t p = abs_subc_n(w0p, w0p, u1p, l, cf0, cf0, 0);
        neg0 = p < 0;
    }
    WJR_ASSERT_ASSUME(cf0 <= 1);

    // W3 = V0 - V1 : v(-1)
    {
        ssize_t p = abs_subc_s(w3p, v0p, l, v1p, rm);
        neg3 = p < 0;
    }

    // W2 = W0 * W3 : f(-1) = r2
    neg0 ^= neg3;
    __mul_n<__mul_mode::toom33, 1, 0>(w2p, w0p, w3p, l, stk, cf2, cf0, 0);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    // W3 = U2 * V1 : (non-negative) f(inf) = r3
    if (rn >= rm) {
        __mul_s<__mul_mode::toom33>(w3p, u2p, rn, v1p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom33>(w3p, v1p, rm, u2p, rn, stk);
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

void toom_interpolation_5p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                             size_t rn, size_t rm, toom_interpolation_5p_struct &&flag) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(2);
     W4 = f(inf);
    */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const size_t maxr = std::max(rn, rm);

    const auto w0p = dst;
    const auto w2p = w0p + l * 2;
    const auto w3p = w1p + l * 2;
    const auto w4p = w0p + l * 4;

    uint64_t cf;
    auto [neg1, cf1, cf2, cf3] = flag;

    // W3 = (W3 - W1) / 3 : (non-negative) (f(2) - f(-1)) / 3
    {
        if (!neg1) {
            cf3 -= cf1 + subc_n(w3p, w3p, w1p, l * 2);
        } else {
            cf3 += cf1 + addc_n(w3p, w3p, w1p, l * 2);
        }

        divexact_by3(w3p, w3p, l * 2);
        cf3 /= 3;
    }

    // W1 = (W2 - W1) >> 1 : (non-negative) (f(1) - f(-1)) / 2
    {
        if (!neg1) {
            cf1 = cf2 - cf1 - subc_n(w1p, w2p, w1p, l * 2);
        } else {
            cf1 = cf2 + cf1 + addc_n(w1p, w2p, w1p, l * 2);
        }

        rshift_n(w1p, w1p, l * 2, 1, cf1);
        cf1 >>= 1;
    }

    // W2 = W2 - W0 : (non-negative) f(1) - f(0)
    cf2 -= subc_n(w2p, w2p, w0p, l * 2);

    // W3 = ((W3 - W2) >> 1) - (W4 << 1) : (non-negative) r3
    {
        cf3 -= cf2 + subc_n(w3p, w3p, w2p, l * 2);

        rshift_n(w3p, w3p, l * 2, 1, cf3);
        cf3 >>= 1;

        cf = submul_1(w3p, w4p, rn + rm, 2);
        if (l * 2 != (rn + rm)) {
            cf3 -= subc_1(w3p + rn + rm, w3p + rn + rm, l * 2 - (rn + rm), cf);
            if (l != maxr) {
                WJR_ASSERT(cf3 == 0);
                cf3 = w3p[l + maxr];
            }
        } else {
            cf3 -= cf;
        }
    }

    // W2 = W2 - W1 : (non-negative) f(1) / 2 - f(0) + f(-1) / 2
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
    cf2 -= subc_s(w2p, w2p, l * 2, w4p, rn + rm, cf);

    // W  = W3*x^3 + W1*x + W0
    cf = addc_n(dst + l, dst + l, w1p, l);
    cf = addc_1(dst + l * 2, dst + l * 2, l, cf);
    cf = addc_n(dst + l * 3, dst + l * 3, w3p, l, cf);
    cf = addc_1(dst + l * 4, dst + l * 4, rn + rm, cf2, cf);
    WJR_ASSERT(cf == 0);
}

void toom42_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                  const uint64_t *src1, size_t m, uint64_t *stk) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(2);
     W4 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - U1 + U2 - U3) * (V0 - v1);
     W2 = (U0 + U1 + U2 + U3) * (V0 + V1);
     W3 = (U0 + 2U1 + 4U2 + 8U3) * (V0 + 2V1);
     W4 = U3 * V1;
    */

    /*
     7 add/sub + 3 addlsh

     Not implemented according to the following steps

     T0 = U0 + U2;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W3 = V0 - V1; v(-1)
     T2 = V0 + V1; v(1)
     W1 = T1 * W3; f(-1)
     W2 = T0 * T2; f(1)
     T0 = ((2U3 + U2) << 1 + U1) << 1 + U0; u(2)
     T2 = T0 + V1; v(2)
     W3 = T0 * T2; f(2)
     W0 = U0 * V0;
     W4 = U3 * V1;
    */

    WJR_ASSERT_ASSUME(3 * m + 5 <= 2 * n && n + 15 <= 4 * m);

    const size_t l = (n >= 2 * m ? (n + 3) / 4 : (m + 1) / 2);
    const size_t rn = n - l * 3;
    const size_t rm = m - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;
    const auto u3p = src0 + l * 3;

    const auto v0p = src1;
    const auto v1p = src1 + l;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;

    stk += l * 4;

    uint64_t cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0;
    bool neg2 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_n(w0p, u0p, u2p, l);

    // W4 = V0 + V1 : (non-negative) v(1)
    cf4 = addc_s(w4p, v0p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf4 <= 1);

    // W1 = U1 + U3 : (non-negative)
    cf1 = addc_s(w1p, u1p, l, u3p, rn);

    // W3 = W0 - W1 : u(-1)
    {
        ssize_t p = abs_subc_n(w3p, w0p, w1p, l, cf3, cf0, cf1);
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

    // W1 = W3 * W2 : f(-1)
    neg2 ^= neg3;
    __mul_n<__mul_mode::toom33, 1, 0>(w1p, w3p, w2p, l, stk, cf1, cf3, 0);

    // W2 = W0 * W4 : (non-negative) f(1)
    __mul_n<__mul_mode::toom33, 3, 1>(w2p, w0p, w4p, l, stk, cf2, cf0, cf4);

    // W0 = U0 +(U1 +(U2 + U3<<1)<<1)<<1 : (non-negative) u(2)
    {
        cf0 = addlsh_n(w0p, u2p, u3p, rn, 1);
        if (l != rn) {
            cf0 = addc_1(w0p + rn, u2p + rn, l - rn, cf0);
        }

        cf0 += cf0 + addlsh_n(w0p, u1p, w0p, l, 1);
        cf0 += cf0 + addlsh_n(w0p, u0p, w0p, l, 1);
        WJR_ASSERT_ASSUME(cf0 <= 14);
    }

    // W4 = W4 + V1 : (non-negative) v(2)
    cf4 += addc_s(w4p, w4p, l, v1p, rm);
    WJR_ASSERT_ASSUME(cf4 <= 2);

    // W3 = W0 * W4 : (non-negative) f(2)
    __mul_n<__mul_mode::toom33, in_place_max, 2>(w3p, w0p, w4p, l, stk, cf3, cf0, cf4);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    // W4 = U3 * V1 : (non-negative) f(inf) = r4
    if (rn >= rm) {
        __mul_s<__mul_mode::toom33>(w4p, u3p, rn, v1p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom33>(w4p, v1p, rm, u3p, rn, stk);
    }

    return toom_interpolation_5p_s(dst, w1p, l, rn, rm,
                                   toom_interpolation_5p_struct{neg2, cf1, cf2, cf3});
}

void toom33_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                  const uint64_t *src1, size_t m, uint64_t *stk) {
    WJR_ASSERT_ASSUME(2 * n + 10 <= 3 * m);

    const size_t l = (n + 2) / 3;
    const size_t rn = n - l * 2;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;

    stk += l * 4;

    uint64_t cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0;
    bool neg2 = 0, neg3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);
    // W4 = V0 + V2 : (non-negative)
    cf4 = addc_s(w4p, v0p, l, v2p, rm);

    // W3 = W0 - U1 : u(-1)
    {
        ssize_t p = abs_subc_n(w3p, w0p, u1p, l, cf3, cf0, 0);
        neg3 = p < 0;
        WJR_ASSERT_ASSUME(cf3 <= 1);
    }

    // W2 = W4 - V1 : v(-1)
    {
        ssize_t p = abs_subc_n(w2p, w4p, v1p, l, cf2, cf4, 0);
        neg2 = p < 0;
        WJR_ASSERT_ASSUME(cf2 <= 1);
    }

    // W0 = W0 + U1 : (non-negative) u(1)
    cf0 += addc_n(w0p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf0 <= 2);

    // W4 = W4 + V1 : (non-negative) v(1)
    cf4 += addc_n(w4p, w4p, v1p, l);
    WJR_ASSERT_ASSUME(cf4 <= 2);

    // W1 = W3 * W2 : f(-1)
    neg2 ^= neg3;
    __mul_n<__mul_mode::toom33, 1, 1>(w1p, w3p, w2p, l, stk, cf1, cf3, cf2);

    // W2 = W0 * W4 : (non-negative) f(1)
    __mul_n<__mul_mode::toom33, 2, 2>(w2p, w0p, w4p, l, stk, cf2, cf0, cf4);

    // W0 = (W0 + U2) << 1 - U0 : (non-negative) u(2)
    cf0 += addc_s(w0p, w0p, l, u2p, rn);
    cf0 += cf0 + rsblsh_n(w0p, u0p, w0p, l, 1);
    WJR_ASSERT_ASSUME(cf0 <= 6);

    // W4 = (W4 + V2) << 1 - V0 : (non-negative) v(2)
    cf4 += addc_s(w4p, w4p, l, v2p, rm);
    cf4 += cf4 + rsblsh_n(w4p, v0p, w4p, l, 1);
    WJR_ASSERT_ASSUME(cf4 <= 6);

    // W3 = W0 * W4 : (non-negative) f(2)
    __mul_n<__mul_mode::toom33>(w3p, w0p, w4p, l, stk, cf3, cf0, cf4);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    // W4 = U2 * V2 : (non-negative) f(inf) = r4
    __mul_s<__mul_mode::toom33>(w4p, u2p, rn, v2p, rm, stk);

    return toom_interpolation_5p_s(dst, w1p, l, rn, rm,
                                   toom_interpolation_5p_struct{neg2, cf1, cf2, cf3});
}

void toom3_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n, uint64_t *stk) {
    WJR_ASSERT_ASSUME(10 <= n);

    const size_t l = (n + 2) / 3;
    const size_t rn = n - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);

    const auto u0p = src;
    const auto u1p = src + l;
    const auto u2p = src + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;

    stk += l * 4;

    uint64_t cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);

    // W3 = W0 - U1 : u(-1)
    {
        (void)abs_subc_n(w3p, w0p, u1p, l, cf3, cf0, 0);
        WJR_ASSERT_ASSUME(cf3 <= 1);
    }

    // W0 = W0 + U1 : (non-negative) u(1)
    cf0 += addc_n(w0p, w0p, u1p, l);
    WJR_ASSERT_ASSUME(cf0 <= 2);

    // W1 = W3 * W3 : f(-1)
    __sqr<__mul_mode::toom33, 1>(w1p, w3p, l, stk, cf1, cf3);

    // W2 = W0 * W9 : (non-negative) f(1)
    __sqr<__mul_mode::toom33, 2>(w2p, w0p, l, stk, cf2, cf0);

    // W0 = (W0 + U2) << 1 - U0 : (non-negative) u(2)
    cf0 += addc_s(w0p, w0p, l, u2p, rn);
    cf0 += cf0 + rsblsh_n(w0p, u0p, w0p, l, 1);
    WJR_ASSERT_ASSUME(cf0 <= 6);

    // W3 = W0 * W0 : (non-negative) f(2)
    __sqr<__mul_mode::toom33>(w3p, w0p, l, stk, cf3, cf0);

    // W0 = U0 * U0 : (non-negative) f(0) = r0
    __sqr<__mul_mode::toom33>(w0p, u0p, l, stk);

    // W4 = U2 * U2 : (non-negative) f(inf) = r4
    __sqr<__mul_mode::toom33>(w4p, u2p, rn, stk);

    return toom_interpolation_5p_s(dst, w1p, l, rn, rn,
                                   toom_interpolation_5p_struct{0, cf1, cf2, cf3});
}

void toom_interpolation_6p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                             size_t rn, size_t rm, toom_interpolation_6p_struct &&flag) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(-2);
     W4 = f(2);
     W5 = f(inf)
    */

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

     W4 = W5 * x + W4;
     W2 = W3 * x + W2;
     W2 = W2 - W4;
     W = W4 * x ^ 4 + W2 * x ^ 2 + W1 * x + W0;
    */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const size_t maxr = std::max(rn, rm);

    const auto w0p = dst;
    const auto w2p = w0p + l * 2;
    const auto w3p = w1p + l * 2;
    const auto w4p = w1p + l * 4;
    const auto w5p = w0p + l * 5;

    uint64_t cf;
    auto [neg1, neg3, cf1, cf2, cf3, cf4] = flag;

    // W3 = (W3 - W4) >> 1;
    {
        if (!neg3) {
            cf3 = cf4 - cf3 - subc_n(w3p, w4p, w3p, l * 2);
        } else {
            cf3 += cf4 + addc_n(w3p, w3p, w4p, l * 2);
        }

        (void)rshift_n(w3p, w3p, l * 2, 1, cf3);
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
    (void)rshift_n(w1p, w1p, l * 2, 1, cf1);
    cf1 >>= 1;

    // W3 = W3 / -6; = > W3 = W3 / -2 / 3;
    {
        (void)rshift_n(w3p, w3p, l * 2, 1, cf3);
        divexact_by3(w3p, w3p, l * 2);
        cf3 /= 6;
    }

    // W2 = W2 - W1;
    cf2 -= cf1 + subc_n(w2p, w2p, w1p, l * 2);

    // W3 = W3 - 4 * W5;
    {
        cf = submul_1(w3p, w5p, rn + rm, 4);
        if (rn + rm != l * 2) {
            cf3 -= subc_1(w3p + rn + rm, w3p + rn + rm, l * 2 - (rn + rm), cf);
        } else {
            cf3 -= cf;
        }
    }

    // W1 = W1 - W3;
    cf1 -= cf3 + subc_n(w1p, w1p, w3p, l * 2);

    // W4 = W4 >> 2;
    rshift_n(w4p, w4p, l * 2, 2, cf4);
    cf4 >>= 2;

    // W4 = W4 - W2;
    cf4 -= cf2 + subc_n(w4p, w4p, w2p, l * 2);

    // W4 = W4 / 3;
    divexact_by3(w4p, w4p, l * 2);
    if (l != maxr) {
        cf4 = w4p[l + maxr];
    } else {
        cf4 /= 3;
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
    cf = addc_n(dst + l, dst + l, w1p, l * 2);
    cf = addc_1(dst + l * 3, dst + l * 3, l, cf1, cf);
    cf = addc_n(dst + l * 4, dst + l * 4, w4p, l, cf);
    cf = addc_1(dst + l * 5, dst + l * 5, rn + rm, cf3, cf);
    WJR_ASSERT(cf == 0);
}

void toom43_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                  const uint64_t *src1, size_t m, uint64_t *stk) {
    /*
     W0 = f(0);
     W1 = f(-1);
     W2 = f(1);
     W3 = f(-2);
     W4 = f(2);
     W5 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - U1 + U2 - U3) * (V0 - V1 + V2);
     W2 = (U0 + U1 + U2 + U3) * (V0 + V1 + V2);
     W3 = (U0 - 2U1 + 4U2 - 8U3) * (V0 - 2V2 + 4V3);
     W4 = (U0 + 2U1 + 4U2 + 8U3) * (V0 + 2V2 + 4V3);
     W5 = U3 * V2;
    */

    /*
        dst :
        |--- l * 2 ---|--- l * 2 ---|- l --|--- rn+rm ---|
        W0             W2            T2    W5
        |- l --|- l --|
        T0      T1
        stk :
        |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
        W1             W3            W4
     */

    /*
     11 add/sub + 4 addlsh/rsblsh + 1 shift

     W3 = U0 + U2;
     W4 = U1 + U3;
     T0 = W3 - W4; u(-1)
     T1 = W3 + W4; u(1)
     W3 = V0 + V2;
     W4 = W3 - V1; v(-1)
     W1 = T0 * W4; f(-1)
     W5 = W3 + V1; v(1)
     W2 = T1 * W5; f(1)
     W4 = (W4 + V2) << 1 - V0; v(-2)
     W5 = (W5 + V2) << 1 - V0; v(2)
     T0 = U0 + 4U2;
     T1 = (U1 + 4U3) << 1;
     T2 = T0 - T1; u(-2)
     W3 = T2 * W4; f(-2)
     T2 = T0 + T1; u(2)
     W4 = T2 * W5; f(2)
     W0 = U0 * V0;
     W5 = U3 * V2;
     */

    WJR_ASSERT_ASSUME(3 * m + 8 <= 3 * n && 2 * n + 18 <= 4 * m);

    const size_t l = (3 * n >= 4 * m ? (n + 3) / 4 : (m + 2) / 3);
    const size_t rn = n - l * 3;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = u0p + l;
    const auto u2p = u0p + l * 2;
    const auto u3p = u0p + l * 3;

    const auto v0p = src1;
    const auto v1p = v0p + l;
    const auto v2p = v0p + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = stk + l * 4;
    const auto w5p = w0p + l * 5;

    const auto t0p = w0p;
    const auto t1p = t0p + l;
    const auto t2p = w0p + l * 4;

    stk += l * 6;

    // Hand it over to the compiler to optimize reusable variables
    uint64_t cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    uint64_t cft0 = 0, cft1 = 0, cft2 = 0;
    // Reusable
    // not point to real W0...W5 or T0...T2
    bool neg0 = 0, neg1 = 0, neg2 = 0;

    //  W3 = U0 + U2;
    cf3 = addc_n(w3p, u0p, u2p, l);

    //  W4 = U1 + U3;
    cf4 = addc_s(w4p, u1p, l, u3p, rn);

    //  T0 = W3 - W4; u(-1)
    {
        ssize_t p = abs_subc_n(t0p, w3p, w4p, l, cft0, cf3, cf4);
        neg0 = p < 0;
        WJR_ASSERT_ASSUME(cft0 <= 1);
    }

    //  T1 = W3 + W4; u(1)
    cft1 = cf3 + cf4 + addc_n(t1p, w3p, w4p, l);
    WJR_ASSERT_ASSUME(cft1 <= 3);

    //  W3 = V0 + V2;
    cf3 = addc_s(w3p, v0p, l, v2p, rm);

    //  W4 = W3 - V1; v(-1)
    {
        ssize_t p = abs_subc_n(w4p, w3p, v1p, l, cf4, cf3, 0);
        neg1 = p < 0;
        WJR_ASSERT_ASSUME(cf4 <= 1);
    }

    //  W1 = T0 * W4; f(-1)
    neg0 ^= neg1;
    __mul_n<__mul_mode::toom33, 1, 1>(w1p, t0p, w4p, l, stk, cf1, cft0, cf4);

    //  W5 = W3 + V1; v(1)
    cf5 = cf3 + addc_n(w5p, w3p, v1p, l);
    WJR_ASSERT_ASSUME(cf5 <= 2);

    //  W2 = T1 * W5; f(1)
    __mul_n<__mul_mode::toom33, 3, 2>(w2p, t1p, w5p, l, stk, cf2, cft1, cf5);

    //  W4 = (W4 + V2) << 1 - V0; v(-2)
    {
        if (!neg1) {
            cf4 += addc_s(w4p, w4p, l, v2p, rm);
        } else {
            WJR_ASSERT_ASSUME(cf4 == 0);
            ssize_t p = abs_subc_s(w4p, w4p, l, v2p, rm);
            neg1 = p > 0;
        }

        if (!neg1) {
            // W4 maybe less than V0
            // use lshfit + abs_sub instead of rsblsh_n
            cf4 += cf4 + lshift_n(w4p, w4p, l, 1);
            {
                ssize_t p = abs_subc_n(w4p, w4p, v0p, l, cf4, cf4, 0);
                neg1 = p < 0;
            }
        } else {
            WJR_ASSERT_ASSUME(cf4 == 0);
            cf4 += addlsh_n(w4p, v0p, w4p, l, 1);
        }
        WJR_ASSERT_ASSUME(cf4 <= 4);
    }

    //  W5 = (W5 + V2) << 1 - V0; v(2)
    cf5 += addc_s(w5p, w5p, l, v2p, rm);
    cf5 += cf5 + rsblsh_n(w5p, v0p, w5p, l, 1);
    WJR_ASSERT_ASSUME(cf5 <= 6);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  T1 = (U1 + 4U3) << 1;
    cft1 = addlsh_n(t1p, u1p, u3p, rn, 2);
    if (l != rn) {
        cft1 = addc_1(t1p + rn, u1p + rn, l - rn, cft1);
    }
    cft1 += cft1 + lshift_n(t1p, t1p, l, 1);
    WJR_ASSERT_ASSUME(cft1 <= 9);

    //  T2 = T0 - T1; u(-2)
    {
        ssize_t p = abs_subc_n(t2p, t0p, t1p, l, cft2, cft0, cft1);
        neg2 = p < 0;
        WJR_ASSERT_ASSUME(cft2 <= 9);
    }

    //  W3 = T2 * W4; f(-2)
    neg1 ^= neg2;
    __mul_n<__mul_mode::toom33>(w3p, t2p, w4p, l, stk, cf3, cft2, cf4);

    //  T2 = T0 + T1; u(2)
    cft2 = cft0 + cft1 + addc_n(t2p, t0p, t1p, l);
    WJR_ASSERT_ASSUME(cft2 <= 14);

    //  W4 = T2 * W5; f(2)
    __mul_n<__mul_mode::toom33>(w4p, t2p, w5p, l, stk, cf4, cft2, cf5);

    //  W0 = U0 * V0;
    __mul_n<__mul_mode::toom33>(w0p, u0p, v0p, l, stk);

    //  W5 = U3 * V2;
    if (rn >= rm) {
        __mul_s<__mul_mode::toom33>(w5p, u3p, rn, v2p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom33>(w5p, v2p, rm, u3p, rn, stk);
    }

    toom_interpolation_6p_s(dst, w1p, l, rn, rm,
                            toom_interpolation_6p_struct{neg0, neg1, cf1, cf2, cf3, cf4});
}

void toom_interpolation_7p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                             size_t rn, size_t rm, toom_interpolation_7p_struct &&flag) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);
    */

    /*
    [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   1, -2,  4, -8, 16,-32, 64
    W2   1,  1,  1,  1,  1,  1,  1
    W3   1, -1,  1, -1,  1, -1,  1
    W4   1,  2,  4,  8, 16, 32, 64
    W5  64, 32, 16,  8,  4,  2,  1
    W6   0,  0,  0,  0,  0,  0,  1
    ]

     W5 = W5 + W4
     W1 =(W4 - W1)/2
     W4 = W4 - W0
     W4 =(W4 - W1)/4 - W6*16
     W3 =(W2 - W3)/2
     W2 = W2 - W3

     [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   0,  2,  0,  8,  0, 32,  0
    W2   1,  0,  1,  0,  1,  0,  1
    W3   0,  1,  0,  1,  0,  1,  0
    W4   0,  0,  1,  0,  4,  0,  0
    W5  65, 34, 20, 16, 20, 34, 65
    W6   0,  0,  0,  0,  0,  0,  1
    ]

     W5 = W5 - W2*65      May be negative.
     W2 = W2 - W6 - W0
     W5 =(W5 + W2*45)/2   Now >= 0 again.
     W4 =(W4 - W2)/3
     W2 = W2 - W4

     [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   0,  2,  0,  8,  0, 32,  0
    W2   0,  0,  1,  0,  0,  0,  0
    W3   0,  1,  0,  1,  0,  1,  0
    W4   0,  0,  0,  0,  1,  0,  0
    W5   0, 17,  0,  8,  0, 17,  0
    W6   0,  0,  0,  0,  0,  0,  1
    ]

     W1 = W5 - W1         May be negative.
     W5 =(W5 - W3*8)/9
     W3 = W3 - W5
     W1 =(W1/15 + W5)/2   Now >= 0 again.
     W5 = W5 - W1

     [
    W0   1,  0,  0,  0,  0,  0,  0
    W1   0,  1,  0,  0,  0,  0,  0
    W2   0,  0,  1,  0,  0,  0,  0
    W3   0,  0,  0,  1,  0,  0,  0
    W4   0,  0,  0,  0,  1,  0,  0
    W5   0,  0,  0,  0,  0,  1,  0
    W6   0,  0,  0,  0,  0,  0,  1
    ]

    */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const size_t maxr = std::max(rn, rm);

    const auto w0p = dst;
    const auto w2p = w0p + l * 2;
    const auto w3p = w1p + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = w1p + l * 4;
    const auto w6p = w0p + l * 6;

    uint64_t cf = 0;
    auto [neg1, neg3, cf1, cf2, cf3, cf4, cf5] = flag;

    //  W5 = W5 + W4
    cf5 += cf4 + addc_n(w5p, w5p, w4p, l * 2);

    //  W1 =(W4 - W1)/2
    if (!neg1) {
        WJR_ASSERT(cf4 >= cf1);
        cf1 = cf4 - cf1 - subc_n(w1p, w4p, w1p, l * 2);
    } else {
        cf1 += cf4 + addc_n(w1p, w4p, w1p, l * 2);
    }
    (void)rshift_n(w1p, w1p, l * 2, 1, cf1);
    cf1 >>= 1;

    //  W4 = W4 - W0
    cf4 -= subc_n(w4p, w4p, w0p, l * 2);

    //  W4 =(W4 - W1)/4 - W6*16
    cf4 -= cf1 + subc_n(w4p, w4p, w1p, l * 2);
    (void)rshift_n(w4p, w4p, l * 2, 2, cf4);
    cf4 >>= 2;
    cf = submul_1(w4p, w6p, rn + rm, 16);
    if (rn + rm != l * 2) {
        cf4 -= subc_1(w4p + rn + rm, w4p + rn + rm, l * 2 - (rn + rm), cf);
    } else {
        cf4 -= cf;
    }

    //  W3 =(W2 - W3)/2
    if (!neg3) {
        WJR_ASSERT(cf2 >= cf3);
        cf3 = cf2 - cf3 - subc_n(w3p, w2p, w3p, l * 2);
    } else {
        cf3 += cf2 + addc_n(w3p, w2p, w3p, l * 2);
    }
    (void)rshift_n(w3p, w3p, l * 2, 1, cf3);
    cf3 >>= 1;

    //  W2 = W2 - W3
    WJR_ASSERT(cf2 >= cf3);
    cf2 -= cf3 + subc_n(w2p, w2p, w3p, l * 2);

    //  W5 = W5 - W2*65      May be negative.
    cf5 -= cf2 * 65 + submul_1(w5p, w2p, l * 2, 65);

    //  W2 = W2 - W6 - W0
    cf2 -= subc_n(w2p, w2p, w0p, l * 2);
    cf2 -= subc_s(w2p, w2p, l * 2, w6p, rn + rm);

    //  W5 =(W5 + W2*45)/2   Now >= 0 again.
    cf5 += cf2 * 45 + addmul_1(w5p, w2p, l * 2, 45);
    (void)rshift_n(w5p, w5p, l * 2, 1, cf5);
    cf5 >>= 1;

    //  W4 =(W4 - W2)/3
    cf4 -= cf2 + subc_n(w4p, w4p, w2p, l * 2);
    divexact_by3(w4p, w4p, l * 2);
    cf4 /= 3;

    //  W2 = W2 - W4
    cf2 -= cf4 + subc_n(w2p, w2p, w4p, l * 2);

    //  W1 = W5 - W1         May be negative.
    {
        ssize_t p = abs_subc_n(w1p, w5p, w1p, l * 2, cf1, cf5, cf1);
        neg1 = p < 0;
    }

    //  W5 =(W5 - W3*8)/9
    cf5 -= cf3 * 8 + submul_1(w5p, w3p, l * 2, 8);
    divexact_byc(w5p, w5p, l * 2, std::integral_constant<uint64_t, 9>{});
    cf5 /= 9;

    //  W3 = W3 - W5
    cf3 -= cf5 + subc_n(w3p, w3p, w5p, l * 2);

    //  W1 =(W1/15 + W5)/2   Now >= 0 again.
    divexact_by15(w1p, w1p, l * 2);
    cf1 /= 15;
    if (!neg1) {
        cf1 += cf5 + addc_n(w1p, w1p, w5p, l * 2);
    } else {
        cf1 = cf5 - cf1 - subc_n(w1p, w5p, w1p, l * 2);
    }
    rshift_n(w1p, w1p, l * 2, 1, cf1);
    cf1 >>= 1;

    //  W5 = W5 - W1
    cf5 -= cf1 + subc_n(w5p, w5p, w1p, l * 2);
    if (l != maxr) {
        cf5 = w5p[l + maxr];
    }

    cf = addc_n(dst + l, dst + l, w1p, l * 2);
    cf = addc_n(dst + l * 3, dst + l * 3, w3p, l, cf);
    cf2 += addc_1(dst + l * 3, dst + l * 3, l, cf1);
    cf = addc_n(dst + l * 4, dst + l * 4, w3p + l, l, cf);
    cf3 += addc_1(dst + l * 4, dst + l * 4, l, cf2);
    cf = addc_n(dst + l * 5, dst + l * 5, w5p, l, cf);
    cf4 += addc_1(dst + l * 5, dst + l * 5, l, cf3);
    cf = addc_n(dst + l * 6, dst + l * 6, w5p + l, maxr, cf);
    cf5 += addc_1(dst + l * 6, dst + l * 6, maxr, cf4);
    cf = addc_1(dst + l * 6 + maxr, dst + l * 6 + maxr, (rn + rm) - maxr, cf5, cf);
    WJR_ASSERT(cf == 0);
}

void toom53_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                  const uint64_t *src1, size_t m, uint64_t *stk) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - 2U1 + 4U2 + 8U3 - 16U4) * (V0 - 2V1 + 4V2);
     W2 = (U0 + U1 + U2 + U3 + U4) * (V0 + V1 + V2);
     W3 = (U0 - U1 + U2 - U3 + U4) * (V0 - V1 + V2);
     W4 = (U0 + 2U1 + 4U2 + 8U3 + 16U4) * (V0 + 2V1 + 4V2);
     W5 = (16U0 + 8U1 + 4U2 + 2U3 + U4) * (4V0 + 2V1 + V2);
     W6 = U4 * V2;
    */

    /*
          dst :
          |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|--- rn+rm ---|
          W0             W2            W4            W6
          |- l --|- l --|                           |- l --|
          T0      T1                                 T2
          stk :
          |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
          W1             W3            W5
     */

    /*
     11 add/sub + 11 addlsh/rsblsh + 1 shift

     T0 = U0 + U2 + U4;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W5 = V0 + V2;
     T2 = W5 + V1; v(1)
     W5 = W5 - V1; v(-1)
     W2 = T0 * T2; f(1)
     W3 = T1 * W5; f(-1)
     W5 = (W5 + V2) << 1 - V0; v(-2)
     T2 = W5 + 4V1; v(2)
     T1 = U0 + (U2 + 4U4) << 2;
     W1 = (U1 + 4U3) << 1;
     T0 = T1 + W1; u(2)
     T1 = T1 - W1; u(-2)
     W1 = T1 * W5; f(-2)
     W4 = T0 * T2; f(2)
     T0 = (((2U0 + U1) << 1 + U2) << 1 + U3) << 1 + U4; 16 * u(1/2)
     T1 = (2V0 + V1) << 1 + V2; 4 * v(1/2)
     W5 = T0 * T1; 64 * f(1/2)
     W0 = U0 * V0; f(0)
     W6 = U4 * V2; f(inf)
    */

    WJR_ASSERT_ASSUME(4 * m + 14 <= 3 * n && 2 * n + 28 <= 5 * m);

    const size_t l = 3 * n >= 5 * m ? (n + 4) / 5 : (m + 2) / 3;
    const size_t rn = n - l * 4;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;
    const auto u3p = src0 + l * 3;
    const auto u4p = src0 + l * 4;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = stk + l * 4;
    const auto w6p = w0p + l * 6;

    const auto t0p = w0p;
    const auto t1p = w0p + l;
    const auto t2p = w6p;

    stk += l * 6;

    uint64_t cf = 0;
    uint64_t cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    uint64_t cft0 = 0, cft1 = 0, cft2 = 0;
    bool neg0 = 0, neg1 = 0, neg2 = 0;

    //  T0 = U0 + U2 + U4;
    cft0 = addc_n(t0p, u0p, u2p, l);
    cft0 += addc_s(t0p, t0p, l, u4p, rn);

    //  T2 = U1 + U3;
    cft2 = addc_n(t2p, u1p, u3p, l);

    //  T1 = T0 - T2; u(-1)
    {
        ssize_t p = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        neg0 = p < 0;
        WJR_ASSERT_ASSUME(cft1 <= 2);
    }

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  W5 = V0 + V2;
    cf5 = addc_s(w5p, v0p, l, v2p, rm);

    //  T2 = W5 + V1; v(1)
    cft2 = cf5 + addc_n(t2p, w5p, v1p, l);
    WJR_ASSERT_ASSUME(cft2 <= 2);

    //  W5 = W5 - V1; v(-1)
    {
        ssize_t p = abs_subc_n(w5p, w5p, v1p, l, cf5, cf5, 0);
        neg1 = p < 0;
        WJR_ASSERT_ASSUME(cf5 <= 1);
    }

    //  W2 = T0 * T2; f(1)
    __mul_n<__mul_mode::toom44, 4, 2>(w2p, t0p, t2p, l, stk, cf2, cft0, cft2);

    //  W3 = T1 * W5; f(-1)
    neg0 ^= neg1;
    __mul_n<__mul_mode::toom44, 2, 1>(w3p, t1p, w5p, l, stk, cf3, cft1, cf5);

    //  W5 = (W5 + V2) << 1 - V0; v(-2)
    if (!neg1) {
        cf5 += addc_s(w5p, w5p, l, v2p, rm);
    } else {
        WJR_ASSERT_ASSUME(cf5 == 0);
        ssize_t p = abs_subc_s(w5p, w5p, l, v2p, rm);
        neg1 = p > 0;
    }

    if (!neg1) {
        cf5 += cf5 + lshift_n(w5p, w5p, l, 1);
        {
            ssize_t p = abs_subc_n(w5p, w5p, v0p, l, cf5, cf5, 0);
            neg1 = p < 0;
        }
    } else {
        WJR_ASSERT_ASSUME(cf5 == 0);
        cf5 += addlsh_n(w5p, v0p, w5p, l, 1);
    }
    WJR_ASSERT_ASSUME(cf5 <= 4);

    //  T2 = W5 + 4V1; v(2)
    if (!neg1) {
        cft2 = cf5 + addlsh_n(t2p, w5p, v1p, l, 2);
    } else {
        cft2 = rsblsh_n(t2p, w5p, v1p, l, 2) - cf5;
    }
    WJR_ASSERT_ASSUME(cft2 <= 6);

    //  T1 = U0 + (U2 + 4U4) << 2;
    cft1 = addlsh_n(t1p, u2p, u4p, rn, 2);
    if (l != rn) {
        cft1 = addc_1(t1p + rn, u2p + rn, l - rn, cft1);
    }
    cft1 = cft1 * 4 + addlsh_n(t1p, u0p, t1p, l, 2);

    //  W1 = (U1 + 4U3) << 1;
    cf1 = addlsh_n(w1p, u1p, u3p, l, 2);
    cf1 += cf1 + lshift_n(w1p, w1p, l, 1);

    //  T0 = T1 + W1; u(2)
    cft0 = cft1 + cf1 + addc_n(t0p, t1p, w1p, l);
    WJR_ASSERT_ASSUME(cft0 <= 30);

    //  T1 = T1 - W1; u(-2)
    {
        ssize_t p = abs_subc_n(t1p, t1p, w1p, l, cft1, cft1, cf1);
        neg2 = p < 0;
    }
    WJR_ASSERT_ASSUME(cft1 <= 20);

    //  W1 = T1 * W5; f(-2)
    neg1 ^= neg2;
    __mul_n<__mul_mode::toom44, in_place_max, 4>(w1p, t1p, w5p, l, stk, cf1, cft1, cf5);

    //  W4 = T0 * T2; f(2)
    __mul_n<__mul_mode::toom44, in_place_max, 6>(w4p, t0p, t2p, l, stk, cf4, cft0, cft2);

    //  T0 = (((2U0 + U1) << 1 + U2) << 1 + U3) << 1 + U4; 16 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u3p, t0p, l, 1);
    cf = addlsh_n(t0p, u4p, t0p, rn, 1);
    if (l != rn) {
        cft0 += cft0 + lshift_n(t0p + rn, t0p + rn, l - rn, 1);
        cft0 += addc_1(t0p + rn, t0p + rn, l - rn, cf);
    } else {
        cft0 += cft0 + cf;
    }
    WJR_ASSERT_ASSUME(cft0 <= 30);

    //  T1 = (2V0 + V1) << 1 + V2; 4 * v(1/2)
    cft1 = addlsh_n(t1p, v1p, v0p, l, 1);
    cf = addlsh_n(t1p, v2p, t1p, rm, 1);
    if (l != rm) {
        cft1 += cft1 + lshift_n(t1p + rm, t1p + rm, l - rm, 1);
        cft1 += addc_1(t1p + rm, t1p + rm, l - rm, cf);
    } else {
        cft1 += cft1 + cf;
    }
    WJR_ASSERT_ASSUME(cft1 <= 6);

    //  W5 = T0 * T1; 64 * f(1/2)
    __mul_n<__mul_mode::toom44>(w5p, t0p, t1p, l, stk, cf5, cft0, cft1);

    //  W0 = U0 * V0; f(0)
    __mul_n<__mul_mode::toom44>(w0p, u0p, v0p, l, stk);

    //  W6 = U4 * V2; f(inf)
    if (rn >= rm) {
        __mul_s<__mul_mode::toom44>(w6p, u4p, rn, v2p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom44>(w6p, v2p, rm, u4p, rn, stk);
    }

    toom_interpolation_7p_s(
        dst, w1p, l, rn, rm,
        toom_interpolation_7p_struct{neg1, neg0, cf1, cf2, cf3, cf4, cf5});
}

void toom44_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n,
                  const uint64_t *src1, size_t m, uint64_t *stk) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);

     W0 = U0 * V0;
     W1 = (U0 - 2U1 + 4U2 + 8U3) * (V0 - 2V1 + 4V2 - 8V3);
     W2 = (U0 + U1 + U2 + U3) * (V0 + V1 + V2 + V3);
     W3 = (U0 - U1 + U2 - U3) * (V0 - V1 + V2 - V3);
     W4 = (U0 + 2U1 + 4U2 + 8U3) * (V0 + 2V1 + 4V2 + 8V3);
     W5 = (8U0 + 4U1 + 2U2 + U3) * (8V0 + 4V1 + 2V2 + V3);
     W6 = U3 * V3;
    */

    /*
         dst :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|--- rn+rm ---|
         W0             W2            W4            W6
         |- l --|- l --|                           |- l --|
         T0      T1                                 T2
         stk :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
         W1             W3            W5
    */

    /*
     12 add/sub + 10 addlsh + 2shift

     T0 = U0 + U2;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W1 = V0 + V2;
     T2 = V1 + V3;
     W4 = W1 + T2; v(1)
     W1 = W1 - T2; v(-1)
     W2 = T0 * W4; f(1)
     W3 = T1 * W1; f(-1)
     T0 = U0 + 4U2;
     T2 = (U1 + 4U3) << 1;
     T1 = T0 - T2; u(-2)
     T0 = T0 + T2; u(2)
     W5 = V0 + 4V2;
     T2 = (V1 + 4V3) << 1;
     W1 = W5 + T2; v(2)
     W5 = W5 - T2; v(-2)
     W4 = T0 * W1; f(2)
     W1 = T1 * W5; f(-2)
     T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
     T1 = ((2V0 + V1) << 1 + V2) << 1 + V3; 8 * v(1/2)
     W5 = T0 * T1; 64 * f(1/2)
     W0 = U0 * V0; f(0)
     W6 = U3 * V3; f(inf)
    */

    WJR_ASSERT_ASSUME(3 * n + 21 <= 4 * m);

    const size_t l = (n + 3) / 4;
    const size_t rn = n - l * 3;
    const size_t rm = m - l * 3;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto u0p = src0;
    const auto u1p = src0 + l;
    const auto u2p = src0 + l * 2;
    const auto u3p = src0 + l * 3;

    const auto v0p = src1;
    const auto v1p = src1 + l;
    const auto v2p = src1 + l * 2;
    const auto v3p = src1 + l * 3;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = stk + l * 4;
    const auto w6p = w0p + l * 6;

    const auto t0p = w0p;
    const auto t1p = w0p + l;
    const auto t2p = w6p;

    stk += l * 6;

    uint64_t cf = 0;
    uint64_t cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    uint64_t cft0 = 0, cft1 = 0, cft2 = 0;
    bool neg0 = 0, neg1 = 0, neg2 = 0;

    //  T0 = U0 + U2;
    cft0 = addc_n(t0p, u0p, u2p, l);

    //  T2 = U1 + U3;
    cft2 = addc_s(t2p, u1p, l, u3p, rn);

    //  T1 = T0 - T2; u(-1)
    {
        ssize_t p = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        neg0 = p < 0;
        WJR_ASSERT_ASSUME(cft1 <= 1);
    }

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 3);

    //  W1 = V0 + V2;
    cf1 = addc_n(w1p, v0p, v2p, l);

    //  T2 = V1 + V3;
    cft2 = addc_s(t2p, v1p, l, v3p, rm);

    //  W4 = W1 + T2; v(1)
    cf4 = cf1 + cft2 + addc_n(w4p, w1p, t2p, l);
    WJR_ASSERT_ASSUME(cf4 <= 3);

    //  W1 = W1 - T2; v(-1)
    {
        ssize_t p = abs_subc_n(w1p, w1p, t2p, l, cf1, cf1, cft2);
        neg1 = p < 0;
        WJR_ASSERT_ASSUME(cf1 <= 1);
    }

    //  W2 = T0 * W4; f(1)
    __mul_n<__mul_mode::toom44, 3, 3>(w2p, t0p, w4p, l, stk, cf2, cft0, cf4);

    //  W3 = T1 * W1; f(-1)
    neg0 ^= neg1;
    __mul_n<__mul_mode::toom44, 1, 1>(w3p, t1p, w1p, l, stk, cf3, cft1, cf1);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  T2 = (U1 + 4U3) << 1;
    cft2 = addlsh_n(t2p, u1p, u3p, rn, 2);
    if (l != rn) {
        cft2 = addc_1(t2p + rn, u1p + rn, l - rn, cft2);
    }
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT_ASSUME(cft2 <= 9);

    //  T1 = T0 - T2; u(-2)
    {
        ssize_t p = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        neg1 = p < 0;
        WJR_ASSERT_ASSUME(cft1 <= 9);
    }

    //  T0 = T0 + T2; u(2)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  W5 = V0 + 4V2;
    cf5 = addlsh_n(w5p, v0p, v2p, l, 2);
    WJR_ASSERT_ASSUME(cf5 <= 4);

    //  T2 = (V1 + 4V3) << 1;
    cft2 = addlsh_n(t2p, v1p, v3p, rm, 2);
    if (l != rm) {
        cft2 = addc_1(t2p + rm, v1p + rm, l - rm, cft2);
    }
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT_ASSUME(cft2 <= 9);

    //  W1 = W5 + T2; v(2)
    cf1 = cf5 + cft2 + addc_n(w1p, w5p, t2p, l);
    WJR_ASSERT_ASSUME(cf1 <= 14);

    //  W5 = W5 - T2; v(-2)
    {
        ssize_t p = abs_subc_n(w5p, w5p, t2p, l, cf5, cf5, cft2);
        neg2 = p < 0;
        WJR_ASSERT_ASSUME(cf5 <= 9);
    }

    //  W4 = T0 * W1; f(2)
    __mul_n<__mul_mode::toom44>(w4p, t0p, w1p, l, stk, cf4, cft0, cf1);

    //  W1 = T1 * W5; f(-2)
    neg1 ^= neg2;
    __mul_n<__mul_mode::toom44>(w1p, t1p, w5p, l, stk, cf1, cft1, cf5);

    //  T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    cf = addlsh_n(t0p, u3p, t0p, rn, 1);
    if (l != rn) {
        cft0 += cft0 + lshift_n(t0p + rn, t0p + rn, l - rn, 1);
        cft0 += addc_1(t0p + rn, t0p + rn, l - rn, cf);
    } else {
        cft0 += cft0 + cf;
    }
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  T1 = ((2V0 + V1) << 1 + V2) << 1 + V3; 8 * v(1/2)
    cft1 = addlsh_n(t1p, v1p, v0p, l, 1);
    cft1 += cft1 + addlsh_n(t1p, v2p, t1p, l, 1);
    cf = addlsh_n(t1p, v3p, t1p, rm, 1);
    if (l != rm) {
        cft1 += cft1 + lshift_n(t1p + rm, t1p + rm, l - rm, 1);
        cft1 += addc_1(t1p + rm, t1p + rm, l - rm, cf);
    } else {
        cft1 += cft1 + cf;
    }
    WJR_ASSERT_ASSUME(cft1 <= 14);

    //  W5 = T0 * T1; 64 * f(1/2)
    __mul_n<__mul_mode::toom44>(w5p, t0p, t1p, l, stk, cf5, cft0, cft1);

    //  W0 = U0 * V0; f(0)
    __mul_n<__mul_mode::toom44>(w0p, u0p, v0p, l, stk);

    //  W6 = U3 * V3; f(inf)
    if (rn >= rm) {
        __mul_s<__mul_mode::toom44>(w6p, u3p, rn, v3p, rm, stk);
    } else {
        __mul_s<__mul_mode::toom44>(w6p, v3p, rm, u3p, rn, stk);
    }

    toom_interpolation_7p_s(
        dst, w1p, l, rn, rm,
        toom_interpolation_7p_struct{neg1, neg0, cf1, cf2, cf3, cf4, cf5});
}

void toom4_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n, uint64_t *stk) {
    /*
     W0 = f(0);
     W1 = f(-2);
     W2 = f(1);
     W3 = f(-1);
     W4 = f(2);
     W5 = 64 * f(1/2);
     W6 = f(inf);

     W0 = U0 ^ 2;
     W1 = (U0 - 2U1 + 4U2 + 8U3) ^ 2;
     W2 = (U0 + U1 + U2 + U3) ^ 2;
     W3 = (U0 - U1 + U2 - U3) ^ 2;
     W4 = (U0 + 2U1 + 4U2 + 8U3) ^ 2;
     W5 = (8U0 + 4U1 + 2U2 + U3) ^ 2;
     W6 = U3 ^ 2;
    */

    /*
         dst :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|--- rn+rm ---|
         W0             W2            W4            W6
         |- l --|- l --|                           |- l --|
         T0      T1                                 T2
         stk :
         |--- l * 2 ---|--- l * 2 ---|--- l * 2 ---|
         W1             W3            W5
    */

    /*
     12 add/sub + 10 addlsh + 2shift

     T0 = U0 + U2;
     T2 = U1 + U3;
     T1 = T0 - T2; u(-1)
     T0 = T0 + T2; u(1)
     W2 = T0 ^ 2; f(1)
     W3 = T1 ^ 2; f(-1)
     T0 = U0 + 4U2;
     T2 = (U1 + 4U3) << 1;
     T1 = T0 - T2; u(-2)
     T0 = T0 + T2; u(2)
     W4 = T0 ^ 2; f(2)
     W1 = T1 ^ 2; f(-2)
     T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
     W5 = T0 ^ 2; 64 * f(1/2)
     W0 = U0 ^ 2; f(0)
     W6 = U3 ^ 2; f(inf)
    */

    WJR_ASSERT_ASSUME(21 <= n);

    const size_t l = (n + 3) / 4;
    const size_t rn = n - l * 3;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(rn * 2 >= l);

    const auto u0p = src;
    const auto u1p = src + l;
    const auto u2p = src + l * 2;
    const auto u3p = src + l * 3;

    const auto w0p = dst;
    const auto w1p = stk;
    const auto w2p = w0p + l * 2;
    const auto w3p = stk + l * 2;
    const auto w4p = w0p + l * 4;
    const auto w5p = stk + l * 4;
    const auto w6p = w0p + l * 6;

    const auto t0p = w0p;
    const auto t1p = w0p + l;
    const auto t2p = w6p;

    stk += l * 6;

    uint64_t cf = 0;
    uint64_t cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    uint64_t cft0 = 0, cft1 = 0, cft2 = 0;

    //  T0 = U0 + U2;
    cft0 = addc_n(t0p, u0p, u2p, l);

    //  T2 = U1 + U3;
    cft2 = addc_s(t2p, u1p, l, u3p, rn);

    //  T1 = T0 - T2; u(-1)
    {
        (void)abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        WJR_ASSERT_ASSUME(cft1 <= 1);
    }

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 3);

    //  W2 = T0 ^ 2; f(1)
    __sqr<__mul_mode::toom44, 3>(w2p, t0p, l, stk, cf2, cft0);

    //  W3 = T1 ^ 2; f(-1)
    __sqr<__mul_mode::toom44, 1>(w3p, t1p, l, stk, cf3, cft1);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT_ASSUME(cft0 <= 4);

    //  T2 = (U1 + 4U3) << 1;
    cft2 = addlsh_n(t2p, u1p, u3p, rn, 2);
    if (l != rn) {
        cft2 = addc_1(t2p + rn, u1p + rn, l - rn, cft2);
    }
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT_ASSUME(cft2 <= 9);

    //  T1 = T0 - T2; u(-2)
    {
        (void)abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        WJR_ASSERT_ASSUME(cft1 <= 9);
    }

    //  T0 = T0 + T2; u(2)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  W4 = T0 ^ 2; f(2)
    __sqr<__mul_mode::toom44>(w4p, t0p, l, stk, cf4, cft0);

    //  W1 = T1 * W5; f(-2)
    __sqr<__mul_mode::toom44>(w1p, t1p, l, stk, cf1, cft1);

    //  T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    cf = addlsh_n(t0p, u3p, t0p, rn, 1);
    if (l != rn) {
        cft0 += cft0 + lshift_n(t0p + rn, t0p + rn, l - rn, 1);
        cft0 += addc_1(t0p + rn, t0p + rn, l - rn, cf);
    } else {
        cft0 += cft0 + cf;
    }
    WJR_ASSERT_ASSUME(cft0 <= 14);

    //  W5 = T0 ^ 2; 64 * f(1/2)
    __sqr<__mul_mode::toom44>(w5p, t0p, l, stk, cf5, cft0);

    //  W0 = U0 * V0; f(0)
    __sqr<__mul_mode::toom44>(w0p, u0p, l, stk);

    //  W6 = U3 * V3; f(inf)
    __sqr<__mul_mode::toom44>(w6p, u3p, rn, stk);

    toom_interpolation_7p_s(dst, w1p, l, rn, rn,
                            toom_interpolation_7p_struct{0, 0, cf1, cf2, cf3, cf4, cf5});
}

} // namespace wjr