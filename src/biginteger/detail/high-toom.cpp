#include <wjr/biginteger/detail/div.hpp>
#include <wjr/memory/stack_allocator.hpp>
#include <wjr/tuple.hpp>

#include "mul-helper.hpp"

namespace wjr {
namespace {

/**
 * @todo carry-flag in toom_interpolation struct's bits.
 */

struct toom_interpolation_5p_struct {
    bool neg1;
    uint64_t cf1;
    uint64_t cf2;
    uint64_t cf3;
};

struct toom_interpolation_6p_struct {
    uint8_t neg1 : 1;
    uint8_t neg3 : 1;
    uint64_t cf1;
    uint64_t cf2;
    uint64_t cf3;
    uint64_t cf4;
};

struct toom_interpolation_7p_struct {
    uint8_t neg1 : 1;
    uint8_t neg3 : 1;
    uint64_t cf1;
    uint64_t cf2;
    uint64_t cf3;
    uint64_t cf4;
    uint64_t cf5;
};

template <size_t P>
using toom_interpolation_high_p_struct = std::array<uint64_t, P - 2>;

WJR_INTRINSIC_INLINE void _sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                               WJR_MAYBE_UNUSED uint64_t *stk) noexcept {
    sqr(dst, src, n);
}

template <uint64_t m = UINT64_MAX>
WJR_INTRINSIC_INLINE void _sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n,
                               uint64_t *stk, uint64_t &c_out, uint64_t cf) noexcept {
    WJR_ASSERT_ASSUME(cf <= m);

    _sqr(dst, src, n, stk);

    if constexpr (m == 0) {
        c_out = 0;
    } else if constexpr (m == 1) {
        c_out = cf;
    } else {
        c_out = cf * cf;
    }

    constexpr auto m2 = in_range<uint32_t>(m) ? m * 2 : m;
    c_out += try_addmul_1<m2>(dst + n, src, n, 2 * cf);
}

} // namespace

namespace {
void toom_interpolation_5p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l, size_t rn,
                             size_t rm, toom_interpolation_5p_struct &&flag) noexcept {
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

    auto *const w0p = dst;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;

    uint64_t cf;
    auto [neg1, cf1, cf2, cf3] = flag;

    // W3 = (W3 - W1) / 3 : (non-negative) (f(2) - f(-1)) / 3
    {
        if (!neg1) {
            cf3 -= cf1 + subc_n(w3p, w3p, w1p, l * 2);
        } else {
            cf3 += cf1 + addc_n(w3p, w3p, w1p, l * 2);
        }

        divexact_byc(w3p, w3p, l * 2, 3_u64, 0);
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

void toom_interpolation_6p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l, size_t rn,
                             size_t rm, toom_interpolation_6p_struct &&flag) noexcept {
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

    WJR_ASSERT(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const size_t maxr = std::max(rn, rm);

    auto *const w0p = dst;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w1p + l * 4;
    auto *const w5p = w0p + l * 5;

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
        divexact_byc(w3p, w3p, l * 2, 6_u64, cf3);
        cf3 /= 6;
    }

    // W2 = W2 - W1;
    cf2 -= cf1 + subc_n(w2p, w2p, w1p, l * 2);

    // W3 = W3 - 4 * W5;
    WJR_SUBMUL_1_S(w3p, l * 2, w5p, rn + rm, cf3, 0, 4, cf3);

    // W1 = W1 - W3;
    cf1 -= cf3 + subc_n(w1p, w1p, w3p, l * 2);

    // W4 = W4 >> 2;
    rshift_n(w4p, w4p, l * 2, 2, cf4);
    cf4 >>= 2;

    // W4 = W4 - W2;
    cf4 -= cf2 + subc_n(w4p, w4p, w2p, l * 2);

    // W4 = W4 / 3;
    divexact_byc(w4p, w4p, l * 2, 3_u64, 0);
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

void toom_interpolation_7p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l, size_t rn,
                             size_t rm, toom_interpolation_7p_struct &&flag) noexcept {
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

    auto *const w0p = dst;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;
    auto *const w5p = w1p + l * 4;
    auto *const w6p = w0p + l * 6;

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

    //  W4 = (W4 - W1)/4 - W6*16
    cf4 -= cf1 + subc_n(w4p, w4p, w1p, l * 2);
    (void)rshift_n(w4p, w4p, l * 2, 2, cf4);
    cf4 >>= 2;
    WJR_SUBMUL_1_S(w4p, l * 2, w6p, rn + rm, cf4, 0, 16, cf4);

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
    divexact_byc(w4p, w4p, l * 2, 3_u64, 0);
    cf4 /= 3;

    //  W2 = W2 - W4
    cf2 -= cf4 + subc_n(w2p, w2p, w4p, l * 2);

    //  W1 = W5 - W1         May be negative.
    neg1 = abs_subc_n(w1p, w5p, w1p, l * 2, cf1, cf5, cf1) < 0;

    //  W5 =(W5 - W3*8)/9
    cf5 -= cf3 * 8 + submul_1(w5p, w3p, l * 2, 8);
    divexact_byc(w5p, w5p, l * 2, 9_u64, cf5);
    cf5 /= 9;

    //  W3 = W3 - W5
    cf3 -= cf5 + subc_n(w3p, w3p, w5p, l * 2);

    //  W1 =(W1/15 + W5)/2   Now >= 0 again.
    divexact_byc(w1p, w1p, l * 2, 15_u64, 0);
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

struct toom_eval_opposite_exp_args_k3 {
    using tuple_type =
        tuple<uint64_t *, uint64_t *, uint64_t *, const uint64_t *, size_t, size_t, unsigned int>;

    toom_eval_opposite_exp_args_k3(uint64_t *t0p, uint64_t *t1p, uint64_t *stk, const uint64_t *wp,
                                   size_t length, size_t rest, unsigned int exp) noexcept
        : input(t0p, t1p, stk, wp, length, rest, exp), cf(default_construct, default_construct) {}

    void set_exp(unsigned int exp) noexcept { input[6_zu] = exp; }

    tuple_type input;
    tuple<uint64_t, uint64_t> cf;
};

struct toom_eval_opposite_exp_args {
    using tuple_type = tuple<uint64_t *, uint64_t *, uint64_t *, const uint64_t *, size_t, size_t,
                             size_t, unsigned int>;

    toom_eval_opposite_exp_args(uint64_t *t0p, uint64_t *t1p, uint64_t *stk, const uint64_t *wp,
                                size_t length, size_t rest, size_t k, unsigned int exp) noexcept
        : input(t0p, t1p, stk, wp, length, rest, k, exp), cf(default_construct, default_construct) {
    }

    void set_exp(unsigned int exp) noexcept { input[7_zu] = exp; }

    tuple_type input;
    tuple<uint64_t, uint64_t> cf;
};

void toom_eval_2_exp(toom_eval_opposite_exp_args &args) noexcept {
    const auto [t0p, t1p, stk, wp, len, rest, k, exp] = args.input;
    auto &[cf0, cf1] = args.cf;
    WJR_ASSERT(k >= 4);
    WJR_ASSERT(exp * (k - 1) <= 60);

    cf0 = addlsh_n(t0p, wp, wp + len, len, exp);
    for (size_t i = 2; i < k - 1; ++i) {
        cf0 += addlsh_n(t0p, t0p, wp + len * i, len, exp * i);
    }

    WJR_ADDLSH_S(t0p, t0p, len, wp + len * (k - 1), rest, cf0, 0, exp * (k - 1), cf0);
}

WJR_NODISCARD bool toom_eval_opposite_2_exp(toom_eval_opposite_exp_args_k3 &args) noexcept {
    const auto [t0p, t1p, stk, wp, len, rest, exp] = args.input;
    auto &[cf0, cf1] = args.cf;
    WJR_ASSERT(exp * 2 <= 60);

    uint64_t cft1;

    // deal with odd position
    WJR_ADDLSH_S(t1p, wp, len, wp + len * 2, rest, 0, 0, exp * 2, cft1);

    // deal with even position
    uint64_t cfstk = lshift_n(stk, wp + len, len, exp);

    cf0 = cft1 + cfstk + addc_n(t0p, t1p, stk, len);
    return abs_subc_n(t1p, t1p, stk, len, cf1, cft1, cfstk) < 0;
}

WJR_NODISCARD bool toom_eval_opposite_half_exp(toom_eval_opposite_exp_args_k3 &args) noexcept {
    const auto [t0p, t1p, stk, wp, len, rest, exp] = args.input;
    auto &[cf0, cf1] = args.cf;
    WJR_ASSERT(exp * 2 <= 60);

    uint64_t cft1;

    // deal with odd position
    WJR_ADDLSH_NS(t1p, wp + len * 2, rest, wp, len, 0, 0, exp * 2, cft1);

    // deal with even position
    const uint64_t cfstk = lshift_n(stk, wp + len, len, exp);

    cf0 = cft1 + cfstk + addc_n(t0p, t1p, stk, len);
    return abs_subc_n(t1p, t1p, stk, len, cf1, cft1, cfstk) < 0;
}

WJR_NODISCARD bool toom_eval_opposite_2_exp(toom_eval_opposite_exp_args &args) noexcept {
    const auto [t0p, t1p, stk, wp, len, rest, k, exp] = args.input;
    auto &[cf0, cf1] = args.cf;
    WJR_ASSERT(k >= 4);
    WJR_ASSERT(exp * (k - 1) <= 60);

    uint64_t cft1;

    // deal with odd position
    cft1 = addlsh_n(t1p, wp, wp + len * 2, len, exp * 2);
    for (size_t i = 4; i < k - 1; i += 2) {
        cft1 += addlsh_n(t1p, t1p, wp + len * i, len, exp * i);
    }

    // deal with even position
    uint64_t cfstk = lshift_n(stk, wp + len, len, exp);
    for (size_t i = 3; i < k - 1; i += 2) {
        cfstk += addlsh_n(stk, stk, wp + len * i, len, exp * i);
    }

    if (k & 1) {
        WJR_ADDLSH_S(t1p, t1p, len, wp + len * (k - 1), rest, cft1, 0, exp * (k - 1), cft1);
    } else {
        WJR_ADDLSH_S(stk, stk, len, wp + len * (k - 1), rest, cfstk, 0, exp * (k - 1), cfstk);
    }

    cf0 = cft1 + cfstk + addc_n(t0p, t1p, stk, len);
    return abs_subc_n(t1p, t1p, stk, len, cf1, cft1, cfstk) < 0;
}

WJR_NODISCARD bool toom_eval_opposite_half_exp(toom_eval_opposite_exp_args &args) noexcept {
    const auto [t0p, t1p, stk, wp, len, rest, k, exp] = args.input;
    auto &[cf0, cf1] = args.cf;
    WJR_ASSERT(k >= 4);
    WJR_ASSERT(exp * (k - 1) <= 60);

    uint64_t cft1;

    // deal with odd position
    cft1 = addlsh_n(t1p, wp + len * 2, wp, len, exp * 2);
    for (size_t i = 4; i < k - 1; i += 2) {
        cft1 = (cft1 << (exp * 2)) + addlsh_n(t1p, wp + len * i, t1p, len, exp * 2);
    }

    // deal with even position

    uint64_t cfstk = addlsh_n(stk, wp + len * 3, wp + len, len, exp * 2);
    for (size_t i = 5; i < k - 1; i += 2) {
        cfstk = (cfstk << (exp * 2)) + addlsh_n(stk, wp + len * i, stk, len, exp * 2);
    }

    if (k & 1) {
        WJR_ADDLSH_NS(t1p, wp + len * (k - 1), rest, t1p, len, 0, cft1, exp * 2, cft1);
        cfstk = (cfstk << exp) + lshift_n(stk, stk, len, exp);
    } else {
        WJR_ADDLSH_NS(stk, wp + len * (k - 1), rest, stk, len, 0, cfstk, exp * 2, cfstk);
        cft1 = (cft1 << exp) + lshift_n(t1p, t1p, len, exp);
    }

    cf0 = cft1 + cfstk + addc_n(t0p, t1p, stk, len);
    return abs_subc_n(t1p, t1p, stk, len, cf1, cft1, cfstk) < 0;
}

/**
 * @details \n
 * A is postive, B's sign is `sign' \n
 * abs(A) >= abs(B) \n
 * A -> (A - B) / (2^sA) \n
 * B -> (A + B) / 2 \n
 *
 */
#define WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_ODD(A, B, n, sign, cfA, cfB, sA)                      \
    do {                                                                                           \
        if (!sign) {                                                                               \
            cfB += cfA + addc_n(B, A, B, n);                                                       \
        } else {                                                                                   \
            cfB = cfA - cfB - subc_n(B, A, B, n);                                                  \
        }                                                                                          \
                                                                                                   \
        (void)rshift_n(B, B, n, 1, cfB);                                                           \
        cfB >>= 1;                                                                                 \
                                                                                                   \
        cfA -= cfB + subc_n(A, A, B, n);                                                           \
        if constexpr (sA != 1) {                                                                   \
            (void)rshift_n(A, A, n, sA - 1, cfA);                                                  \
            cfA >>= sA - 1;                                                                        \
        }                                                                                          \
    } while (0)

/**
 * @details \n
 * A is postive, B's sign is `sign' \n
 * abs(A) >= abs(B) \n
 * A -> (A - B) / 2 \n
 * B -> (A + B) / (2^sB) \n
 *
 */
#define WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_EVEN(A, B, n, sign, cfA, cfB, sB)                     \
    do {                                                                                           \
        if (!sign) {                                                                               \
            cfB += cfA + addc_n(B, A, B, n);                                                       \
        } else {                                                                                   \
            cfB = cfA - cfB - subc_n(B, A, B, n);                                                  \
        }                                                                                          \
                                                                                                   \
        (void)rshift_n(B, B, n, 1, cfB);                                                           \
        cfB >>= 1;                                                                                 \
                                                                                                   \
        cfA -= cfB + subc_n(A, A, B, n);                                                           \
        if constexpr (sB != 1) {                                                                   \
            (void)rshift_n(B, B, n, sB - 1, cfB);                                                  \
            cfB >>= sB - 1;                                                                        \
        }                                                                                          \
    } while (0)

void toom_interpolation_even_4_solve(uint64_t *w0p, uint64_t *w2p, uint64_t *w4p, uint64_t *w6p,
                                     size_t n, uint64_t *tp) noexcept {
    /*
        part 0 :
        [
            1,      0,      0,      0       W0
            1,      1,      1,      1       W2
            1,      4,      16,     64      W4
            64,     16,     4,      1       W6
        ]

        W4 -= W2;
        W6 = 64*W2-W6;
        W2 -= W0;

        [
            1,      1,      1       W2
            3,      15,     63      W4
            48,     60,     63      W6
        ]

        W6 -= W4;
        W6 /= 45;
        W6 = W2 - W6;
        W4 /= 3;
        W4 -= W2;
        W4 /= 4;
        W4 -= 5*W6;
        W2 -= W4 + W6;

        [
            1,      0,      0,      W2
            0,      1,      0,      W4
            0,      0,      1,      W6
        ]
    */

    auto &cf2 = tp[1];
    auto &cf4 = tp[3];
    auto &cf6 = tp[5];

    // W4 -= W2;
    cf4 -= cf2 + subc_n(w4p, w4p, w2p, n);

    // W6 = 64 * W2 - W6;
    cf6 = rsblsh_n(w6p, w6p, w2p, n, 6) + 64 * cf2 - cf6;

    // W2 -= W0;
    cf2 -= subc_n(w2p, w2p, w0p, n);

    // W6 -= W4;
    cf6 -= cf4 + subc_n(w6p, w6p, w4p, n);

    // W6 /= 45;
    divexact_byc(w6p, w6p, n, 45_u64, cf6);
    cf6 /= 45;

    // W6 = W2 - W6;
    cf6 = cf2 - cf6 - subc_n(w6p, w2p, w6p, n);

    // W4 /= 3;
    divexact_byc(w4p, w4p, n, 3_u64, cf4);
    cf4 /= 3;

    // W4 -= W2;
    cf4 -= cf2 + subc_n(w4p, w4p, w2p, n);

    // W4 /= 4;
    (void)rshift_n(w4p, w4p, n, 2, cf4);
    cf4 >>= 2;

    // W4 -= 5 * W6;
    cf4 -= cf6 * 5 + submul_1(w4p, w6p, n, 5);

    // W2 -= W4 + W6;
    cf2 -= cf4 + subc_n(w2p, w2p, w4p, n);
    cf2 -= cf6 + subc_n(w2p, w2p, w6p, n);
}

// need to optimize
void toom_interpolation_opposite_4_solve(uint64_t *w1p, uint64_t *w3p, uint64_t *w5p, uint64_t *w7p,
                                         size_t n, uint64_t *tp) noexcept {
    /*
        [
            1,      1,      1,      1       W1
            0,      3,      15,     63      W3
            0,      3,      51,     819     W5
            0,      48,     60,     63      W7
        ]

        W5 = W5-W3;

        [
            1,      1,      1,      1       W1
            0,      3,      15,     63      W3
            0,      0,      36,     756     W5
            0,      48,     60,     63      W7
        ]

        W7 = 16*W3-W7;
        W7 -= W5;
        W7 = 4*W5-W7;
        W7 /= 2835;

        [
            1,      1,      1,      1       W1
            0,      3,      15,     63      W3
            0,      0,      36,     756     W5
            0,      0,      0,      1       W7
        ]

        W3 /= 3;
        W5 /= 36;
        W3 -= W5;
        W5 -= 21*W7;
        W3 -= 4*W5;
        W1 -= W3 + W5 + W7;

        [
            1,      0,      0,      0       W1
            0,      1,      0,      0       W3
            0,      0,      1,      0       W5
            0,      0,      0,      1       W7
        ]
     */

    auto &cf1 = tp[0];
    auto &cf3 = tp[2];
    auto &cf5 = tp[4];
    auto &cf7 = tp[6];

    // W5 = W5 - W3;
    cf5 -= cf3 + subc_n(w5p, w5p, w3p, n);

    // W7 = 16*W3-W7;
    cf7 = rsblsh_n(w7p, w7p, w3p, n, 4) + 16 * cf3 - cf7;

    // W7 -= W5;
    cf7 -= cf5 + subc_n(w7p, w7p, w5p, n);

    // W7 = 4 * W5 - W7;
    cf7 = rsblsh_n(w7p, w7p, w5p, n, 2) + 4 * cf5 - cf7;

    // W7 /= 2835;
    divexact_byc(w7p, w7p, n, 2835_u64, cf7);
    cf7 /= 2835;

    // W3 /= 3;
    divexact_byc(w3p, w3p, n, 3_u64, 0);
    cf3 /= 3;

    // W5 /= 36;
    divexact_byc(w5p, w5p, n, 36_u64, cf5);
    cf5 /= 36;

    // W3 -= W5;
    cf3 -= cf5 + subc_n(w3p, w3p, w5p, n);

    // W5 -= 21*W7;
    cf5 -= submul_1(w5p, w7p, n, 21) + 21 * cf7;

    // W3 -= 4*W5;
    cf3 -= submul_1(w3p, w5p, n, 4) + 4 * cf5;

    // W1 -= W3+W5+W7;
    cf1 -= cf3 + subc_n(w1p, w1p, w3p, n);
    cf1 -= cf5 + subc_n(w1p, w1p, w5p, n);
    cf1 -= cf7 + subc_n(w1p, w1p, w7p, n);
}

void toom_interpolation_8p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l, size_t rn,
                             size_t rm, toom_interpolation_high_p_struct<8> &&flag) noexcept {
    /*
    W0 = f(0);
    W1 = (f(1)-f(-1))/2;
    W2 = (f(1)+f(-1))/2;
    W3 = (f(2)-f(-2))/4;
    W4 = (f(2)+(f-2))/2;
    W5 = 128 * (f(1/2)-f(-1/2)) / 2;
    W6 = 128 * (f(1/2)+f(-1/2)) / 4;
    W7 = f(inf);
    */

    /*

    [
        1,      0,      0,      0,      0,      0,      0,      0
        0,      1,      0,      1,      0,      1,      0,      1
        1,      0,      1,      0,      1,      0,      1,      0
        0,      1,      0,      4,      0,      16,     0,      64
        1,      0,      4,      0,      16,     0,      64,     0
        0,      64,     0,      16,     0,      4,      0,      1
        64,     0,      16,     0,      4,      0,      1,      0
        0,      0,      0,      0,      0,      0,      0,      1
    ]

    seprate to two part :

    part 0 :
    [
        1,      0,      0,      0       W0
        1,      1,      1,      1       W2
        1,      4,      16,     64      W4
        64,     16,     4,      1       W6
    ]

    part 1 :
    [
        1,      1,      1,      1       W1
        1,      4,      16,     64      W3
        64,     16,     4,      1       W5
        0,      0,      0,      1       W7
    ]

    */

    /*
        part 0 :
        [
            1,      0,      0,      0       W0
            1,      1,      1,      1       W2
            1,      4,      16,     64      W4
            64,     16,     4,      1       W6
        ]

        goto toom_interpolation_even_4_solve;

        W4 -= W2;
        W6 = 64*W2-W6;
        W2 -= W0;

        [
            1,      1,      1       W2
            3,      15,     63      W4
            48,     60,     63      W6
        ]

        W6 -= W4;
        W6 /= 45;
        W6 = W2 - W6;
        W4 /= 3;
        W4 -= W2;
        W4 /= 4;
        W4 -= 5*W6;
        W2 -= W4 + W6;

        [
            1,      0,      0,      W2
            0,      1,      0,      W4
            0,      0,      1,      W6
        ]
    */

    /*
        part 1 :
        [
            1,      1,      1,      1       W1
            1,      4,      16,     64      W3
            64,     16,     4,      1       W5
            0,      0,      0,      1       W7
        ]

        W5 -= W1;
        W3 = 64*W1-W3;
        W1 -= W7;

        [
            1,      1,      1       W1
            63,     60,     48      W3
            63,     15,     3       W5
        ]

        W3 -= W5;
        W3 /= 45;
        W5 /= 3;
        W5 -= W1;
        W1 -= W3;
        W5 /= 4;
        W5 -= 5*W1;
        W3 -= W5;
        swap(W3, W5);

        [
            1,      0,      0       W1
            0,      1,      0       W3
            0,      0,      1       W5
        ]

     */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    auto *const w0p = dst;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;
    auto *const w5p = w1p + l * 4;
    auto *const w6p = w1p + l * 6;
    auto *const w7p = w0p + l * 7;

    auto &[cf1, cf2, cf3, cf4, cf5, cf6] = flag;

    // part 0 :

    toom_interpolation_even_4_solve(w0p, w2p, w4p, w6p, l * 2, flag.data());

    // part 1 :

    // W5 -= W1;
    cf5 -= cf1 + subc_n(w5p, w5p, w1p, l * 2);

    // W3 = 64 * W1 - W3;
    cf3 = rsblsh_n(w3p, w3p, w1p, l * 2, 6) + 64 * cf1 - cf3;

    // W1 -= W7;
    cf1 -= subc_s(w1p, w1p, l * 2, w7p, rn + rm);

    // W3 -= W5;
    cf3 -= cf5 + subc_n(w3p, w3p, w5p, l * 2);

    // W3 /= 45;
    divexact_byc(w3p, w3p, l * 2, 45_u64, cf3);
    cf3 /= 45;

    // W5 /= 3;
    divexact_byc(w5p, w5p, l * 2, 3_u64, cf5);
    cf5 /= 3;

    // W5 -= W1;
    cf5 -= cf1 + subc_n(w5p, w5p, w1p, l * 2);

    // W1 -= W3;
    cf1 -= cf3 + subc_n(w1p, w1p, w3p, l * 2);

    // W5 /= 4;
    (void)rshift_n(w5p, w5p, l * 2, 2, cf5);
    cf5 >>= 2;

    // W5 -= 5 * W1;
    cf5 -= cf1 * 5 + submul_1(w5p, w1p, l * 2, 5);

    // W3 -= W5;
    cf3 -= cf5 + subc_n(w3p, w3p, w5p, l * 2);

    // recombine :

    uint64_t cf;
    cf = addc_n(dst + l, dst + l, w1p, l * 2);
    (void)addc_1(dst + l * 3, dst + l * 3, l, cf1, cf);

    cf = addc_n(dst + l * 3, dst + l * 3, w5p, l);
    (void)addc_1(dst + l * 4, dst + l * 4, l, cf2, cf);
    cf = addc_n(dst + l * 4, dst + l * 4, w5p + l, l);
    (void)addc_1(dst + l * 5, dst + l * 5, l, cf5, cf);

    cf = addc_n(dst + l * 5, dst + l * 5, w3p, l);
    (void)addc_1(dst + l * 6, w6p, l, cf4, cf);
    cf = addc_n(dst + l * 6, dst + l * 6, w3p + l, l);
    (void)addc_1(dst + l * 7, dst + l * 7, l, cf3, cf);

    cf = addc_n(dst + l * 7, dst + l * 7, w6p + l, l);
    if (rn + rm != l) {
        cf = addc_1(dst + l * 8, dst + l * 8, (rn + rm) - l, cf6, cf);
    } else {
        cf += cf6;
    }
    WJR_ASSERT(cf == 0);
}

void toom_interpolation_9p_s(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l, size_t rn,
                             size_t rm, toom_interpolation_high_p_struct<9> &&flag) noexcept {
    /*
     W0 = f(0);
     W1 = (f(1)-f(-1))/2;
     W2 = (f(1)+f(-1))/2;
     W3 = (f(2)-f(-2))/4;
     W4 = (f(2)+(f-2))/2;
     W5 = f(4);
     W6 = 256 * (f(1/2)+f(-1/2)) / 2;
     W7 = 256 * (f(1/2)-f(-1/2)) / 4;
     W8 = f(inf);
    */

    /*

    [
        1,      0,      0,      0,      0,      0,      0,      0,      0
        0,      1,      0,      1,      0,      1,      0,      1,      0
        1,      0,      1,      0,      1,      0,      1,      0,      1
        0,      1,      0,      4,      0,      16,     0,      64,     0
        1,      0,      4,      0,      16,     0,      64,     0,      256
        1,      4,      16,     64,     256,    1024,   4096,   16384,  65536
        256,    0,      64,     0,      16,     0,      4,      0,      1
        0,      64,     0,      16,     0,      4,      0,      1,      0
        0,      0,      0,      0,      0,      0,      0,      0,      1
    ]

    seprate to two part :

    part 0 :
    [
        1,      0,      0,      0,      0       W0
        1,      1,      1,      1,      1       W2
        1,      4,      16,     64,     256     W4
        256,    64,     16,     4,      1       W6
        0,      0,      0,      0,      1       W8
    ]

    part 1 :
    [
        1,      1,      1,      1       W1
        1,      4,      16,     64      W3
        1,      16,     256,    4096    W5
        64,     16,     4,      1       W7
    ]

    */

    /*
         part 0 :
         [
             1,      0,      0,      0,      0       W0
             1,      1,      1,      1,      1       W2
             1,      4,      16,     64,     256     W4
             256,    64,     16,     4,      1       W6
             0,      0,      0,      0,      1       W8
         ]

         W2 -= W8;
         W4 -= 256*W8;
         W6 -= W8;
         W6 /= 4;

         goto toom_interpolation_even_4_solve
     */

    /*
        part 1 :
        [
            1,      1,      1,      1       W1
            1,      4,      16,     64      W3
            1,      16,     256,    4096    W5
            64,     16,     4,      1       W7
        ]

        W3 -= W1;
        W5 = (W5-W1)/5;
        W7 = (64*W1-W7)

        [
            1,      1,      1,      1       W1
            0,      3,      15,     63      W3
            0,      3,      51,     819     W5
            0,      48,     60,     63      W7
        ]

        goto toom_interpolation_opposite_4_solve;
     */

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    auto *const w0p = dst;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;
    auto *const w5p = w1p + l * 4;
    auto *const w6p = w0p + l * 6;
    auto *const w7p = w1p + l * 6;
    auto *const w8p = w0p + l * 8;

    auto &[cf1, cf2, cf3, cf4, cf5, cf6, cf7] = flag;

    // W2 -= W8
    cf2 -= subc_s(w2p, w2p, l * 2, w8p, rn + rm);

    // W4 -= 256*W8
    WJR_SUBMUL_1_S(w4p, l * 2, w8p, rn + rm, cf4, 0, 256, cf4);

    // W6 -= W8
    cf6 -= subc_s(w6p, w6p, l * 2, w8p, rn + rm);

    // W6 /= 4
    (void)rshift_n(w6p, w6p, l * 2, 2u, cf6);
    cf6 /= 4;

    toom_interpolation_even_4_solve(w0p, w2p, w4p, w6p, l * 2, flag.data());

    // W5 -= W0 + 16*W2 + 256*W4 + 4096*W6 + 65536*W8
    do {
        uint64_t *tmp = w1p + l * 8;
        uint64_t cft = 0;

        // tmp = 16*W8+W6;
        WJR_ADDLSH_S(tmp, w6p, l * 2, w8p, rn + rm, cf6, 0, 4, cft);

        // tmp = 16*tmp+W4;
        cft = addlsh_n(tmp, w4p, tmp, l * 2, 4) + 16 * cft + cf4;

        // tmp = 16*tmp+W2;
        cft = addlsh_n(tmp, w2p, tmp, l * 2, 4) + 16 * cft + cf2;

        // tmp = 16*tmp+W0;
        cft = addlsh_n(tmp, w0p, tmp, l * 2, 4) + 16 * cft;

        // W5 -= tmp
        cf5 -= cft + subc_n(w5p, w5p, tmp, l * 2);
    } while (0);

    // W5 /= 4
    (void)rshift_n(w5p, w5p, l * 2, 2, cf5);
    cf5 >>= 2;

    // W3 -= W1
    cf3 -= cf1 + subc_n(w3p, w3p, w1p, l * 2);

    // W5 = (W5-W1)/5
    {
        cf5 -= cf1 + subc_n(w5p, w5p, w1p, l * 2);
        divexact_byc(w5p, w5p, l * 2, 5_u64, 0);
        cf5 /= 5;
    }

    // W7 = (64*W1-W7)
    cf7 = rsblsh_n(w7p, w7p, w1p, l * 2, 6) + 64 * cf1 - cf7;

    toom_interpolation_opposite_4_solve(w1p, w3p, w5p, w7p, l * 2, flag.data());

    uint64_t cf;
    cf = addc_n(dst + l, dst + l, w1p, l * 2);
    (void)addc_1(dst + l * 3, dst + l * 3, l, cf1, cf);

    cf = addc_n(dst + l * 3, dst + l * 3, w3p, l);
    (void)addc_1(dst + l * 4, dst + l * 4, l, cf2, cf);
    cf = addc_n(dst + l * 4, dst + l * 4, w3p + l, l);
    (void)addc_1(dst + l * 5, dst + l * 5, l, cf3, cf);

    cf = addc_n(dst + l * 5, dst + l * 5, w5p, l);
    (void)addc_1(dst + l * 6, dst + l * 6, l, cf4, cf);
    cf = addc_n(dst + l * 6, dst + l * 6, w5p + l, l);
    (void)addc_1(dst + l * 7, dst + l * 7, l, cf5, cf);

    cf = addc_n(dst + l * 7, dst + l * 7, w7p, l);
    (void)addc_1(dst + l * 8, dst + l * 8, l, cf6, cf);
    cf = addc_n(dst + l * 8, dst + l * 8, w7p + l, l);
    if (rn + rm != l) {
        cf = addc_1(dst + l * 9, dst + l * 9, (rn + rm) - l, cf7, cf);
    } else {
        cf += cf7;
    }
    WJR_ASSERT(cf == 0);
}
} // namespace

void toom42_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m, uint64_t *stk) noexcept {
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

    WJR_ASSERT(3 * m + 5 <= 2 * n && n + 15 <= 4 * m);

    const size_t l = (n >= 2 * m ? (n + 3) / 4 : (m + 1) / 2);
    const size_t rn = n - l * 3;
    const size_t rm = m - l;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto *const u0p = src0;
    const auto *const u1p = src0 + l;
    const auto *const u2p = src0 + l * 2;
    const auto *const u3p = src0 + l * 3;

    const auto *const v0p = src1;
    const auto *const v1p = src1 + l;

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;

    stk += l * 4;

    uint64_t cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0;
    bool neg2 = false, neg3 = false;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_n(w0p, u0p, u2p, l);

    // W4 = V0 + V1 : (non-negative) v(1)
    cf4 = addc_s(w4p, v0p, l, v1p, rm);
    WJR_ASSERT(cf4 <= 1);

    // W1 = U1 + U3 : (non-negative)
    cf1 = addc_s(w1p, u1p, l, u3p, rn);

    // W3 = W0 - W1 : u(-1)
    neg3 = abs_subc_n(w3p, w0p, w1p, l, cf3, cf0, cf1) < 0;
    WJR_ASSERT(cf3 <= 1);

    // W2 = V0 - V1 : v(-1)
    neg2 = abs_subc_s(w2p, v0p, l, v1p, rm) < 0;

    // W0 = W0 + W1 : (non-negative) u(1)
    cf0 += cf1 + addc_n(w0p, w0p, w1p, l);
    WJR_ASSERT(cf0 <= 3);

    // W1 = W3 * W2 : f(-1)
    neg2 ^= neg3;
    _mul_n<1, 0>(w1p, w3p, w2p, l, stk, cf1, cf3, 0);

    // W2 = W0 * W4 : (non-negative) f(1)
    _mul_n<3, 1>(w2p, w0p, w4p, l, stk, cf2, cf0, cf4);

    // W0 = U0 +(U1 +(U2 + U3<<1)<<1)<<1 : (non-negative) u(2)
    {
        WJR_ADDLSH_S(w0p, u2p, l, u3p, rn, 0, 0, 1, cf0);
        cf0 += cf0 + addlsh_n(w0p, u1p, w0p, l, 1);
        cf0 += cf0 + addlsh_n(w0p, u0p, w0p, l, 1);
        WJR_ASSERT(cf0 <= 14);
    }

    // W4 = W4 + V1 : (non-negative) v(2)
    cf4 += addc_s(w4p, w4p, l, v1p, rm);
    WJR_ASSERT(cf4 <= 2);

    // W3 = W0 * W4 : (non-negative) f(2)
    _mul_n<in_place_max, 2>(w3p, w0p, w4p, l, stk, cf3, cf0, cf4);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    _mul_n(w0p, u0p, v0p, l, stk);

    // W4 = U3 * V1 : (non-negative) f(inf) = r4
    if (rn >= rm) {
        mul_s(w4p, u3p, rn, v1p, rm);
    } else {
        mul_s(w4p, v1p, rm, u3p, rn);
    }

    return toom_interpolation_5p_s(dst, w1p, l, rn, rm,
                                   toom_interpolation_5p_struct{neg2, cf1, cf2, cf3});
}

void toom33_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m, uint64_t *stk) noexcept {
    WJR_ASSERT(2 * n + 10 <= 3 * m);

    const size_t l = (n + 2) / 3;
    const size_t rn = n - l * 2;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto *const u0p = src0;
    const auto *const u1p = src0 + l;
    const auto *const u2p = src0 + l * 2;

    const auto *const v0p = src1;
    const auto *const v1p = src1 + l;
    const auto *const v2p = src1 + l * 2;

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;

    stk += l * 4;

    uint64_t cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0;
    bool neg2 = false, neg3 = false;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);
    // W4 = V0 + V2 : (non-negative)
    cf4 = addc_s(w4p, v0p, l, v2p, rm);

    // W3 = W0 - U1 : u(-1)
    neg3 = abs_subc_n(w3p, w0p, u1p, l, cf3, cf0, 0) < 0;
    WJR_ASSERT(cf3 <= 1);

    // W2 = W4 - V1 : v(-1)
    neg2 = abs_subc_n(w2p, w4p, v1p, l, cf2, cf4, 0) < 0;
    WJR_ASSERT(cf2 <= 1);

    // W0 = W0 + U1 : (non-negative) u(1)
    cf0 += addc_n(w0p, w0p, u1p, l);
    WJR_ASSERT(cf0 <= 2);

    // W4 = W4 + V1 : (non-negative) v(1)
    cf4 += addc_n(w4p, w4p, v1p, l);
    WJR_ASSERT(cf4 <= 2);

    // W1 = W3 * W2 : f(-1)
    neg2 ^= neg3;
    _mul_n<1, 1>(w1p, w3p, w2p, l, stk, cf1, cf3, cf2);

    // W2 = W0 * W4 : (non-negative) f(1)
    _mul_n<2, 2>(w2p, w0p, w4p, l, stk, cf2, cf0, cf4);

    // W0 = (W0 + U2) << 1 - U0 : (non-negative) u(2)
    cf0 += addc_s(w0p, w0p, l, u2p, rn);
    cf0 += cf0 + rsblsh_n(w0p, u0p, w0p, l, 1);
    WJR_ASSERT(cf0 <= 6);

    // W4 = (W4 + V2) << 1 - V0 : (non-negative) v(2)
    cf4 += addc_s(w4p, w4p, l, v2p, rm);
    cf4 += cf4 + rsblsh_n(w4p, v0p, w4p, l, 1);
    WJR_ASSERT(cf4 <= 6);

    // W3 = W0 * W4 : (non-negative) f(2)
    _mul_n(w3p, w0p, w4p, l, stk, cf3, cf0, cf4);

    // W0 = U0 * V0 : (non-negative) f(0) = r0
    _mul_n(w0p, u0p, v0p, l, stk);

    // W4 = U2 * V2 : (non-negative) f(inf) = r4
    mul_s(w4p, u2p, rn, v2p, rm);

    return toom_interpolation_5p_s(dst, w1p, l, rn, rm,
                                   toom_interpolation_5p_struct{neg2, cf1, cf2, cf3});
}

void toom3_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n, uint64_t *stk) noexcept {
    WJR_ASSERT(10 <= n);

    const size_t l = (n + 2) / 3;
    const size_t rn = n - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);

    const auto *const u0p = src;
    const auto *const u1p = src + l;
    const auto *const u2p = src + l * 2;

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;

    stk += l * 4;

    uint64_t cf0 = 0, cf1 = 0, cf2 = 0, cf3 = 0;

    // W0 = U0 + U2 : (non-negative)
    cf0 = addc_s(w0p, u0p, l, u2p, rn);

    // W3 = W0 - U1 : u(-1)
    {
        (void)abs_subc_n(w3p, w0p, u1p, l, cf3, cf0, 0);
        WJR_ASSERT(cf3 <= 1);
    }

    // W0 = W0 + U1 : (non-negative) u(1)
    cf0 += addc_n(w0p, w0p, u1p, l);
    WJR_ASSERT(cf0 <= 2);

    // W1 = W3 * W3 : f(-1)
    _sqr<1>(w1p, w3p, l, stk, cf1, cf3);

    // W2 = W0 * W9 : (non-negative) f(1)
    _sqr<2>(w2p, w0p, l, stk, cf2, cf0);

    // W0 = (W0 + U2) << 1 - U0 : (non-negative) u(2)
    cf0 += addc_s(w0p, w0p, l, u2p, rn);
    cf0 += cf0 + rsblsh_n(w0p, u0p, w0p, l, 1);
    WJR_ASSERT(cf0 <= 6);

    // W3 = W0 * W0 : (non-negative) f(2)
    _sqr(w3p, w0p, l, stk, cf3, cf0);

    // W0 = U0 * U0 : (non-negative) f(0) = r0
    _sqr(w0p, u0p, l, stk);

    // W4 = U2 * U2 : (non-negative) f(inf) = r4
    _sqr(w4p, u2p, rn, stk);

    return toom_interpolation_5p_s(dst, w1p, l, rn, rn,
                                   toom_interpolation_5p_struct{0, cf1, cf2, cf3});
}

void toom43_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m, uint64_t *stk) noexcept {
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

    WJR_ASSERT(3 * m + 8 <= 3 * n && 2 * n + 18 <= 4 * m);

    const size_t l = (3 * n >= 4 * m ? (n + 3) / 4 : (m + 2) / 3);
    const size_t rn = n - l * 3;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto *const u0p = src0;
    const auto *const u1p = u0p + l;
    const auto *const u2p = u0p + l * 2;
    const auto *const u3p = u0p + l * 3;

    const auto *const v0p = src1;
    const auto *const v1p = v0p + l;
    const auto *const v2p = v0p + l * 2;

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w1p + l * 4;
    auto *const w5p = w0p + l * 5;

    auto *const t0p = w0p;
    auto *const t1p = t0p + l;
    auto *const t2p = w0p + l * 4;

    stk += l * 6;

    // Hand it over to the compiler to optimize reusable variables
    uint64_t cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    uint64_t cft0 = 0, cft1 = 0, cft2 = 0;
    // Reusable
    // not point to real W0...W5 or T0...T2
    bool neg0 = false, neg1 = false, neg2 = false;

    //  W3 = U0 + U2;
    cf3 = addc_n(w3p, u0p, u2p, l);

    //  W4 = U1 + U3;
    cf4 = addc_s(w4p, u1p, l, u3p, rn);

    //  T0 = W3 - W4; u(-1)
    neg0 = abs_subc_n(t0p, w3p, w4p, l, cft0, cf3, cf4) < 0;
    WJR_ASSERT(cft0 <= 1);

    //  T1 = W3 + W4; u(1)
    cft1 = cf3 + cf4 + addc_n(t1p, w3p, w4p, l);
    WJR_ASSERT(cft1 <= 3);

    //  W3 = V0 + V2;
    cf3 = addc_s(w3p, v0p, l, v2p, rm);

    //  W4 = W3 - V1; v(-1)
    neg1 = abs_subc_n(w4p, w3p, v1p, l, cf4, cf3, 0) < 0;
    WJR_ASSERT(cf4 <= 1);

    //  W1 = T0 * W4; f(-1)
    neg0 ^= neg1;
    _mul_n<1, 1>(w1p, t0p, w4p, l, stk, cf1, cft0, cf4);

    //  W5 = W3 + V1; v(1)
    cf5 = cf3 + addc_n(w5p, w3p, v1p, l);
    WJR_ASSERT(cf5 <= 2);

    //  W2 = T1 * W5; f(1)
    _mul_n<3, 2>(w2p, t1p, w5p, l, stk, cf2, cft1, cf5);

    //  W4 = (W4 + V2) << 1 - V0; v(-2)
    {
        if (!neg1) {
            cf4 += addc_s(w4p, w4p, l, v2p, rm);
        } else {
            WJR_ASSERT(cf4 == 0);
            neg1 = abs_subc_s(w4p, w4p, l, v2p, rm) > 0;
        }

        if (!neg1) {
            // W4 maybe less than V0
            // use lshfit + abs_sub instead of rsblsh_n
            cf4 += cf4 + lshift_n(w4p, w4p, l, 1);
            neg1 = abs_subc_n(w4p, w4p, v0p, l, cf4, cf4, 0) < 0;
        } else {
            WJR_ASSERT(cf4 == 0);
            cf4 += addlsh_n(w4p, v0p, w4p, l, 1);
        }
        WJR_ASSERT(cf4 <= 4);
    }

    //  W5 = (W5 + V2) << 1 - V0; v(2)
    cf5 += addc_s(w5p, w5p, l, v2p, rm);
    cf5 += cf5 + rsblsh_n(w5p, v0p, w5p, l, 1);
    WJR_ASSERT(cf5 <= 6);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT(cft0 <= 4);

    //  T1 = (U1 + 4U3) << 1;
    WJR_ADDLSH_S(t1p, u1p, l, u3p, rn, 0, 0, 2, cft1);
    cft1 += cft1 + lshift_n(t1p, t1p, l, 1);
    WJR_ASSERT(cft1 <= 9);

    //  T2 = T0 - T1; u(-2)
    neg2 = abs_subc_n(t2p, t0p, t1p, l, cft2, cft0, cft1) < 0;
    WJR_ASSERT(cft2 <= 9);

    //  W3 = T2 * W4; f(-2)
    neg1 ^= neg2;
    _mul_n(w3p, t2p, w4p, l, stk, cf3, cft2, cf4);

    //  T2 = T0 + T1; u(2)
    cft2 = cft0 + cft1 + addc_n(t2p, t0p, t1p, l);
    WJR_ASSERT(cft2 <= 14);

    //  W4 = T2 * W5; f(2)
    _mul_n(w4p, t2p, w5p, l, stk, cf4, cft2, cf5);

    //  W0 = U0 * V0;
    _mul_n(w0p, u0p, v0p, l, stk);

    //  W5 = U3 * V2;
    if (rn >= rm) {
        mul_s(w5p, u3p, rn, v2p, rm);
    } else {
        mul_s(w5p, v2p, rm, u3p, rn);
    }

    toom_interpolation_6p_s(dst, w1p, l, rn, rm,
                            toom_interpolation_6p_struct{neg0, neg1, cf1, cf2, cf3, cf4});
}

void toom53_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m, uint64_t *stk) noexcept {
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

    WJR_ASSERT(4 * m + 14 <= 3 * n && 2 * n + 28 <= 5 * m);

    const size_t l = 3 * n >= 5 * m ? (n + 4) / 5 : (m + 2) / 3;
    const size_t rn = n - l * 4;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto *const u0p = src0;
    const auto *const u1p = src0 + l;
    const auto *const u2p = src0 + l * 2;
    const auto *const u3p = src0 + l * 3;
    const auto *const u4p = src0 + l * 4;

    const auto *const v0p = src1;
    const auto *const v1p = src1 + l;
    const auto *const v2p = src1 + l * 2;

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;
    auto *const w5p = w1p + l * 4;
    auto *const w6p = w0p + l * 6;

    auto *const t0p = w0p;
    auto *const t1p = w0p + l;
    auto *const t2p = w6p;

    stk += l * 6;

    uint64_t cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    uint64_t cft0 = 0, cft1 = 0, cft2 = 0;
    bool neg0 = false, neg1 = false, neg2 = false;

    //  T0 = U0 + U2 + U4;
    cft0 = addc_n(t0p, u0p, u2p, l);
    cft0 += addc_s(t0p, t0p, l, u4p, rn);

    //  T2 = U1 + U3;
    cft2 = addc_n(t2p, u1p, u3p, l);

    //  T1 = T0 - T2; u(-1)
    neg0 = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2) < 0;
    WJR_ASSERT(cft1 <= 2);

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT(cft0 <= 4);

    //  W5 = V0 + V2;
    cf5 = addc_s(w5p, v0p, l, v2p, rm);

    //  T2 = W5 + V1; v(1)
    cft2 = cf5 + addc_n(t2p, w5p, v1p, l);
    WJR_ASSERT(cft2 <= 2);

    //  W5 = W5 - V1; v(-1)
    neg1 = abs_subc_n(w5p, w5p, v1p, l, cf5, cf5, 0) < 0;
    WJR_ASSERT(cf5 <= 1);

    //  W2 = T0 * T2; f(1)
    _mul_n<in_place_max, 2>(w2p, t0p, t2p, l, stk, cf2, cft0, cft2);

    //  W3 = T1 * W5; f(-1)
    neg0 ^= neg1;
    _mul_n<2, 1>(w3p, t1p, w5p, l, stk, cf3, cft1, cf5);

    //  W5 = (W5 + V2) << 1 - V0; v(-2)
    if (!neg1) {
        cf5 += addc_s(w5p, w5p, l, v2p, rm);
    } else {
        WJR_ASSERT(cf5 == 0);
        neg1 = abs_subc_s(w5p, w5p, l, v2p, rm) > 0;
    }

    if (!neg1) {
        cf5 += cf5 + lshift_n(w5p, w5p, l, 1);
        neg1 = abs_subc_n(w5p, w5p, v0p, l, cf5, cf5, 0) < 0;
    } else {
        WJR_ASSERT(cf5 == 0);
        cf5 += addlsh_n(w5p, v0p, w5p, l, 1);
    }
    WJR_ASSERT(cf5 <= 4);

    //  T2 = W5 + 4V1; v(2)
    if (!neg1) {
        cft2 = cf5 + addlsh_n(t2p, w5p, v1p, l, 2);
    } else {
        cft2 = rsblsh_n(t2p, w5p, v1p, l, 2) - cf5;
    }
    WJR_ASSERT(cft2 <= 6);

    //  T1 = U0 + (U2 + 4U4) << 2;
    WJR_ADDLSH_S(t1p, u2p, l, u4p, rn, 0, 0, 2, cft1);
    cft1 = cft1 * 4 + addlsh_n(t1p, u0p, t1p, l, 2);

    //  W1 = (U1 + 4U3) << 1;
    cf1 = addlsh_n(w1p, u1p, u3p, l, 2);
    cf1 += cf1 + lshift_n(w1p, w1p, l, 1);

    //  T0 = T1 + W1; u(2)
    cft0 = cft1 + cf1 + addc_n(t0p, t1p, w1p, l);
    WJR_ASSERT(cft0 <= 30);

    //  T1 = T1 - W1; u(-2)
    neg2 = abs_subc_n(t1p, t1p, w1p, l, cft1, cft1, cf1) < 0;
    WJR_ASSERT(cft1 <= 20);

    //  W1 = T1 * W5; f(-2)
    neg1 ^= neg2;
    _mul_n(w1p, t1p, w5p, l, stk, cf1, cft1, cf5);

    //  W4 = T0 * T2; f(2)
    _mul_n(w4p, t0p, t2p, l, stk, cf4, cft0, cft2);

    //  T0 = (((2U0 + U1) << 1 + U2) << 1 + U3) << 1 + U4; 16 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u3p, t0p, l, 1);
    WJR_ADDLSH_NS(t0p, u4p, rn, t0p, l, 0, cft0, 1, cft0);
    WJR_ASSERT(cft0 <= 30);

    //  T1 = (2V0 + V1) << 1 + V2; 4 * v(1/2)
    cft1 = addlsh_n(t1p, v1p, v0p, l, 1);
    WJR_ADDLSH_NS(t1p, v2p, rm, t1p, l, 0, cft1, 1, cft1);
    WJR_ASSERT(cft1 <= 6);

    //  W5 = T0 * T1; 64 * f(1/2)
    _mul_n(w5p, t0p, t1p, l, stk, cf5, cft0, cft1);

    //  W0 = U0 * V0; f(0)
    _mul_n(w0p, u0p, v0p, l, stk);

    //  W6 = U4 * V2; f(inf)
    if (rn >= rm) {
        mul_s(w6p, u4p, rn, v2p, rm);
    } else {
        mul_s(w6p, v2p, rm, u4p, rn);
    }

    toom_interpolation_7p_s(dst, w1p, l, rn, rm,
                            toom_interpolation_7p_struct{neg1, neg0, cf1, cf2, cf3, cf4, cf5});
}

void toom44_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m, uint64_t *stk) noexcept {
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

    WJR_ASSERT(toom44_ok(n, m));

    const size_t l = (n + 3) / 4;
    const size_t rn = n - l * 3;
    const size_t rm = m - l * 3;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto *const u0p = src0;
    const auto *const u1p = src0 + l;
    const auto *const u2p = src0 + l * 2;
    const auto *const u3p = src0 + l * 3;

    const auto *const v0p = src1;
    const auto *const v1p = src1 + l;
    const auto *const v2p = src1 + l * 2;
    const auto *const v3p = src1 + l * 3;

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;
    auto *const w5p = w1p + l * 4;
    auto *const w6p = w0p + l * 6;

    auto *const t0p = w0p;
    auto *const t1p = w0p + l;
    auto *const t2p = w6p;

    stk += l * 6;

    uint64_t cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    uint64_t cft0 = 0, cft1 = 0, cft2 = 0;
    bool neg0 = false, neg1 = false, neg2 = false;

    //  T0 = U0 + U2;
    cft0 = addc_n(t0p, u0p, u2p, l);

    //  T2 = U1 + U3;
    cft2 = addc_s(t2p, u1p, l, u3p, rn);

    //  T1 = T0 - T2; u(-1)
    neg0 = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2) < 0;
    WJR_ASSERT(cft1 <= 1);

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT(cft0 <= 3);

    //  W1 = V0 + V2;
    cf1 = addc_n(w1p, v0p, v2p, l);

    //  T2 = V1 + V3;
    cft2 = addc_s(t2p, v1p, l, v3p, rm);

    //  W4 = W1 + T2; v(1)
    cf4 = cf1 + cft2 + addc_n(w4p, w1p, t2p, l);
    WJR_ASSERT(cf4 <= 3);

    //  W1 = W1 - T2; v(-1)
    neg1 = abs_subc_n(w1p, w1p, t2p, l, cf1, cf1, cft2) < 0;
    WJR_ASSERT(cf1 <= 1);

    //  W2 = T0 * W4; f(1)
    _mul_n<3, 3>(w2p, t0p, w4p, l, stk, cf2, cft0, cf4);

    //  W3 = T1 * W1; f(-1)
    neg0 ^= neg1;
    _mul_n<1, 1>(w3p, t1p, w1p, l, stk, cf3, cft1, cf1);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT(cft0 <= 4);

    //  T2 = (U1 + 4U3) << 1;
    WJR_ADDLSH_S(t2p, u1p, l, u3p, rn, 0, 0, 2, cft2);
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT(cft2 <= 9);

    //  T1 = T0 - T2; u(-2)
    neg1 = abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2) < 0;
    WJR_ASSERT(cft1 <= 9);

    //  T0 = T0 + T2; u(2)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT(cft0 <= 14);

    //  W5 = V0 + 4V2;
    cf5 = addlsh_n(w5p, v0p, v2p, l, 2);
    WJR_ASSERT(cf5 <= 4);

    //  T2 = (V1 + 4V3) << 1;
    WJR_ADDLSH_S(t2p, v1p, l, v3p, rm, 0, 0, 2, cft2);
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT(cft2 <= 9);

    //  W1 = W5 + T2; v(2)
    cf1 = cf5 + cft2 + addc_n(w1p, w5p, t2p, l);
    WJR_ASSERT(cf1 <= 14);

    //  W5 = W5 - T2; v(-2)
    neg2 = abs_subc_n(w5p, w5p, t2p, l, cf5, cf5, cft2) < 0;
    WJR_ASSERT(cf5 <= 9);

    //  W4 = T0 * W1; f(2)
    _mul_n(w4p, t0p, w1p, l, stk, cf4, cft0, cf1);

    //  W1 = T1 * W5; f(-2)
    neg1 ^= neg2;
    _mul_n(w1p, t1p, w5p, l, stk, cf1, cft1, cf5);

    //  T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    WJR_ADDLSH_NS(t0p, u3p, rn, t0p, l, 0, cft0, 1, cft0);
    WJR_ASSERT(cft0 <= 14);

    //  T1 = ((2V0 + V1) << 1 + V2) << 1 + V3; 8 * v(1/2)
    cft1 = addlsh_n(t1p, v1p, v0p, l, 1);
    cft1 += cft1 + addlsh_n(t1p, v2p, t1p, l, 1);
    WJR_ADDLSH_NS(t1p, v3p, rm, t1p, l, 0, cft1, 1, cft1);
    WJR_ASSERT(cft1 <= 14);

    //  W5 = T0 * T1; 64 * f(1/2)
    _mul_n(w5p, t0p, t1p, l, stk, cf5, cft0, cft1);

    //  W0 = U0 * V0; f(0)
    _mul_n(w0p, u0p, v0p, l, stk);

    //  W6 = U3 * V3; f(inf)
    if (rn >= rm) {
        mul_s(w6p, u3p, rn, v3p, rm);
    } else {
        mul_s(w6p, v3p, rm, u3p, rn);
    }

    toom_interpolation_7p_s(dst, w1p, l, rn, rm,
                            toom_interpolation_7p_struct{neg1, neg0, cf1, cf2, cf3, cf4, cf5});
}

void toom4_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n, uint64_t *stk) noexcept {
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

    WJR_ASSERT(21 <= n);

    const size_t l = (n + 3) / 4;
    const size_t rn = n - l * 3;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(rn * 2 >= l);

    const auto *const u0p = src;
    const auto *const u1p = src + l;
    const auto *const u2p = src + l * 2;
    const auto *const u3p = src + l * 3;

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;
    auto *const w5p = w1p + l * 4;
    auto *const w6p = w0p + l * 6;

    auto *const t0p = w0p;
    auto *const t1p = w0p + l;
    auto *const t2p = w6p;

    stk += l * 6;

    uint64_t cf1 = 0, cf2 = 0, cf3 = 0, cf4 = 0, cf5 = 0;
    uint64_t cft0 = 0, cft1 = 0, cft2 = 0;

    //  T0 = U0 + U2;
    cft0 = addc_n(t0p, u0p, u2p, l);

    //  T2 = U1 + U3;
    cft2 = addc_s(t2p, u1p, l, u3p, rn);

    //  T1 = T0 - T2; u(-1)
    {
        (void)abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        WJR_ASSERT(cft1 <= 1);
    }

    //  T0 = T0 + T2; u(1)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT(cft0 <= 3);

    //  W2 = T0 ^ 2; f(1)
    _sqr<3>(w2p, t0p, l, stk, cf2, cft0);

    //  W3 = T1 ^ 2; f(-1)
    _sqr<1>(w3p, t1p, l, stk, cf3, cft1);

    //  T0 = U0 + 4U2;
    cft0 = addlsh_n(t0p, u0p, u2p, l, 2);
    WJR_ASSERT(cft0 <= 4);

    //  T2 = (U1 + 4U3) << 1;
    WJR_ADDLSH_S(t2p, u1p, l, u3p, rn, 0, 0, 2, cft2);
    cft2 += cft2 + lshift_n(t2p, t2p, l, 1);
    WJR_ASSERT(cft2 <= 9);

    //  T1 = T0 - T2; u(-2)
    {
        (void)abs_subc_n(t1p, t0p, t2p, l, cft1, cft0, cft2);
        WJR_ASSERT(cft1 <= 9);
    }

    //  T0 = T0 + T2; u(2)
    cft0 += cft2 + addc_n(t0p, t0p, t2p, l);
    WJR_ASSERT(cft0 <= 14);

    //  W4 = T0 ^ 2; f(2)
    _sqr(w4p, t0p, l, stk, cf4, cft0);

    //  W1 = T1 * W5; f(-2)
    _sqr(w1p, t1p, l, stk, cf1, cft1);

    //  T0 = ((2U0 + U1) << 1 + U2) << 1 + U3; 8 * u(1/2)
    cft0 = addlsh_n(t0p, u1p, u0p, l, 1);
    cft0 += cft0 + addlsh_n(t0p, u2p, t0p, l, 1);
    WJR_ADDLSH_NS(t0p, u3p, rn, t0p, l, 0, cft0, 1, cft0);
    WJR_ASSERT(cft0 <= 14);

    //  W5 = T0 ^ 2; 64 * f(1/2)
    _sqr(w5p, t0p, l, stk, cf5, cft0);

    //  W0 = U0 * V0; f(0)
    _sqr(w0p, u0p, l, stk);

    //  W6 = U3 * V3; f(inf)
    _sqr(w6p, u3p, rn, stk);

    toom_interpolation_7p_s(dst, w1p, l, rn, rn,
                            toom_interpolation_7p_struct{0, 0, cf1, cf2, cf3, cf4, cf5});
}

void toom63_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m, uint64_t *stk) noexcept {
    /*
    W0 = f(0);
    W1 = (f(1)-f(-1))/2;
    W2 = (f(1)+f(-1))/2;
    W3 = (f(2)-f(-2))/4;
    W4 = (f(2)+(f-2))/2;
    W5 = 128 * (f(1/2)-f(-1/2)) / 2;
    W6 = 128 * (f(1/2)+f(-1/2)) / 4;
    W7 = f(inf);
    */

    const size_t l = n >= 2 * m ? (n + 5) / 6 : (m + 2) / 3;
    const size_t rn = n - l * 5;
    const size_t rm = m - l * 2;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;
    auto *const w5p = w1p + l * 4;
    auto *const w6p = w1p + l * 6;
    auto *const w7p = w0p + l * 7;

    auto *const t0p = w0p + l;
    auto *const t1p = w1p + l * 8;
    auto *const t2p = w1p + l * 9;

    stk += l * 10;

    toom_interpolation_high_p_struct<8> flag;
    bool neg;

    // f(1),f(-1)
    toom_eval_opposite_exp_args args0(w0p, t1p, t0p, src0, l, rn, 6, 0);
    toom_eval_opposite_exp_args_k3 args1(w7p, t2p, t0p, src1, l, rm, 0);

    neg = toom_eval_opposite_2_exp(args0);
    neg ^= toom_eval_opposite_2_exp(args1);

    _mul_n(w1p, w0p, w7p, l, stk, flag[0], args0.cf[0_zu], args1.cf[0_zu]);
    _mul_n(w2p, t1p, t2p, l, stk, flag[1], args0.cf[1_zu], args1.cf[1_zu]);

    WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_ODD(w1p, w2p, l * 2, neg, flag[0], flag[1], 1);

    // f(2),f(-2)
    args0.set_exp(1);
    args1.set_exp(1);

    neg = toom_eval_opposite_2_exp(args0);
    neg ^= toom_eval_opposite_2_exp(args1);

    _mul_n(w3p, w0p, w7p, l, stk, flag[2], args0.cf[0_zu], args1.cf[0_zu]);
    _mul_n(w4p, t1p, t2p, l, stk, flag[3], args0.cf[1_zu], args1.cf[1_zu]);

    WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_ODD(w3p, w4p, l * 2, neg, flag[2], flag[3], 2);

    // f(1/2),f(-1/2)
    neg = toom_eval_opposite_half_exp(args0);
    neg ^= toom_eval_opposite_half_exp(args1);

    _mul_n(w5p, w0p, w7p, l, stk, flag[4], args0.cf[0_zu], args1.cf[0_zu]);
    _mul_n(w6p, t1p, t2p, l, stk, flag[5], args0.cf[1_zu], args1.cf[1_zu]);

    WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_EVEN(w5p, w6p, l * 2, neg, flag[4], flag[5], 2);

    const auto *const u0p = src0;
    const auto *const u5p = src0 + l * 5;

    const auto *const v0p = src1;
    const auto *const v2p = src1 + l * 2;

    // f(0)
    _mul_n(w0p, u0p, v0p, l, stk);

    // f(inf)
    if (rn >= rm) {
        mul_s(w7p, u5p, rn, v2p, rm);
    } else {
        mul_s(w7p, v2p, rm, u5p, rn);
    }

    toom_interpolation_8p_s(dst, w1p, l, rn, rm, std::move(flag));
}

void toom55_mul_s(uint64_t *WJR_RESTRICT dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                  size_t m, uint64_t *stk) noexcept {
    WJR_ASSERT(toom55_ok(n, m));

    const size_t l = (n + 4) / 5;
    const size_t rn = n - l * 4;
    const size_t rm = m - l * 4;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(0 < rm && rm <= l);
    WJR_ASSERT_ASSUME(rn + rm >= l);

    const auto *const u0p = src0;
    const auto *const u4p = src0 + l * 4;

    const auto *const v0p = src1;
    const auto *const v4p = src1 + l * 4;

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;
    auto *const w5p = w1p + l * 4;
    auto *const w6p = w0p + l * 6;
    auto *const w7p = w1p + l * 6;
    auto *const w8p = w0p + l * 8;

    auto *const t0p = w0p + l;
    auto *const t1p = w1p + l * 8;
    auto *const t2p = w1p + l * 9;

    stk += l * 10;

    toom_interpolation_high_p_struct<9> flag;
    bool neg;

    // f(1),f(-1)
    toom_eval_opposite_exp_args args0(w0p, w8p, t0p, src0, l, rn, 5, 0);
    toom_eval_opposite_exp_args args1(t1p, t2p, t0p, src1, l, rm, 5, 0);

    neg = toom_eval_opposite_2_exp(args0);
    neg ^= toom_eval_opposite_2_exp(args1);

    _mul_n(w1p, w0p, t1p, l, stk, flag[0], args0.cf[0_zu], args1.cf[0_zu]);
    _mul_n(w2p, w8p, t2p, l, stk, flag[1], args0.cf[1_zu], args1.cf[1_zu]);

    WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_ODD(w1p, w2p, l * 2, neg, flag[0], flag[1], 1);

    // f(2),f(-2)
    args0.set_exp(1);
    args1.set_exp(1);

    neg = toom_eval_opposite_2_exp(args0);
    neg ^= toom_eval_opposite_2_exp(args1);

    _mul_n(w3p, w0p, t1p, l, stk, flag[2], args0.cf[0_zu], args1.cf[0_zu]);
    _mul_n(w4p, w8p, t2p, l, stk, flag[3], args0.cf[1_zu], args1.cf[1_zu]);

    WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_ODD(w3p, w4p, l * 2, neg, flag[2], flag[3], 2);

    // f(1/2),f(-1/2)
    neg = toom_eval_opposite_half_exp(args0);
    neg ^= toom_eval_opposite_half_exp(args1);

    _mul_n(w7p, w0p, t1p, l, stk, flag[6], args0.cf[0_zu], args1.cf[0_zu]);
    _mul_n(w6p, w8p, t2p, l, stk, flag[5], args0.cf[1_zu], args1.cf[1_zu]);

    WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_ODD(w7p, w6p, l * 2, neg, flag[6], flag[5], 2);

    // f(4)
    args0.set_exp(2);
    args1.set_exp(2);

    toom_eval_2_exp(args0);
    toom_eval_2_exp(args1);

    _mul_n(w5p, w0p, t1p, l, stk, flag[4], args0.cf[0_zu], args1.cf[0_zu]);

    // f(0)
    _mul_n(w0p, u0p, v0p, l, stk);

    // f(inf)
    mul_s(w8p, u4p, rn, v4p, rm);

    toom_interpolation_9p_s(dst, w1p, l, rn, rm, std::move(flag));
}

void toom5_sqr(uint64_t *WJR_RESTRICT dst, const uint64_t *src, size_t n, uint64_t *stk) noexcept {
    const size_t l = (n + 4) / 5;
    const size_t rn = n - l * 4;

    WJR_ASSERT_ASSUME(0 < rn && rn <= l);
    WJR_ASSERT_ASSUME(rn * 2 >= l);

    const auto *const u0p = src;
    const auto *const u4p = src + l * 4;

    auto *const w0p = dst;
    auto *const w1p = stk;
    auto *const w2p = w0p + l * 2;
    auto *const w3p = w1p + l * 2;
    auto *const w4p = w0p + l * 4;
    auto *const w5p = w1p + l * 4;
    auto *const w6p = w0p + l * 6;
    auto *const w7p = w1p + l * 6;
    auto *const w8p = w0p + l * 8;

    auto *const t0p = w0p + l;

    stk += l * 10;

    toom_interpolation_high_p_struct<9> flag;

    // f(1),f(-1)
    toom_eval_opposite_exp_args args(w0p, w8p, t0p, src, l, rn, 5, 0);

    (void)toom_eval_opposite_2_exp(args);

    _sqr(w1p, w0p, l, stk, flag[0], args.cf[0_zu]);
    _sqr(w2p, w8p, l, stk, flag[1], args.cf[1_zu]);

    WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_ODD(w1p, w2p, l * 2, false, flag[0], flag[1], 1);

    // f(2),f(-2)
    args.set_exp(1);

    (void)toom_eval_opposite_2_exp(args);

    _sqr(w3p, w0p, l, stk, flag[2], args.cf[0_zu]);
    _sqr(w4p, w8p, l, stk, flag[3], args.cf[1_zu]);

    WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_ODD(w3p, w4p, l * 2, false, flag[2], flag[3], 2);

    // f(1/2),f(-1/2)
    (void)toom_eval_opposite_half_exp(args);

    _sqr(w7p, w0p, l, stk, flag[6], args.cf[0_zu]);
    _sqr(w6p, w8p, l, stk, flag[5], args.cf[1_zu]);

    WJR_TOOM_EVAL_OPPOSITE_INTERPOLATION_ODD(w7p, w6p, l * 2, false, flag[6], flag[5], 2);

    // f(4)
    args.set_exp(2);

    toom_eval_2_exp(args);

    _sqr(w5p, w0p, l, stk, flag[4], args.cf[0_zu]);

    // f(0)
    _sqr(w0p, u0p, l, stk);

    // f(inf)
    _sqr(w8p, u4p, rn, stk);

    toom_interpolation_9p_s(dst, w1p, l, rn, rn, std::move(flag));
}

} // namespace wjr