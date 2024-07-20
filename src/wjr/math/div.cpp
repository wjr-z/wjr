#include <wjr/math/div.hpp>
#include <wjr/math/stack_allocator.hpp>

namespace wjr {

namespace {
inline constexpr size_t dc_div_qr_threshold = WJR_DC_DIV_QR_THRESHOLD;
}

uint64_t div_qr_1_shift(uint64_t *dst, uint64_t &rem, const uint64_t *src, size_t n,
                        const div2by1_divider<uint64_t> &div) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(div.get_shift() != 0);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_DECR_P(dst, n - 1, src, n - 1));

    const uint64_t divisor = div.get_divisor();
    const uint64_t value = div.get_value();
    const auto shift = div.get_shift();

    uint64_t qh;
    uint64_t lo, hi;

    uint64_t rbp = src[n - 1];
    --n;
    hi = rbp >> (64 - shift);

    do {
        if (WJR_UNLIKELY(n == 0)) {
            qh = div.divide(divisor, value, rbp << shift, hi);
            break;
        }

        lo = src[n - 1];
        qh = div.divide(divisor, value, shld(rbp, lo, shift), hi);
        rbp = lo;
        --n;

        if (WJR_LIKELY(n != 0)) {
            do {
                lo = src[n - 1];
                dst[n] = div.divide(divisor, value, shld(rbp, lo, shift), hi);
                rbp = lo;
                --n;
            } while (WJR_LIKELY(n != 0));
        }

        dst[0] = div.divide(divisor, value, rbp << shift, hi);
    } while (0);

    rem = hi >> shift;
    return qh;
}

uint64_t div_qr_2_noshift(uint64_t *dst, uint64_t *rem, const uint64_t *src, size_t n,
                          const div3by2_divider_noshift<uint64_t> &div) noexcept {
    WJR_ASSERT_ASSUME(n >= 2);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_DECR_P(dst, n - 2, src, n - 2));
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n - 2, rem, 2));

    const uint64_t divisor0 = div.get_divisor0();
    const uint64_t divisor1 = div.get_divisor1();
    const uint64_t value = div.get_value();

    uint64_t qh = 0;
    uint64_t u0, u1, u2;

    u2 = src[n - 1];
    u1 = src[n - 2];

    if (__less_equal_128(divisor0, divisor1, u1, u2)) {
        __sub_128(u1, u2, u1, u2, divisor0, divisor1);
        qh = 1;
    }

    do {
        if (WJR_UNLIKELY(n == 2)) {
            break;
        }

        n -= 2;

        do {
            u0 = src[n - 1];
            dst[n - 1] = div.divide(divisor0, divisor1, value, u0, u1, u2);
            --n;
        } while (WJR_LIKELY(n != 0));

    } while (0);

    rem[0] = u1;
    rem[1] = u2;
    return qh;
}

uint64_t div_qr_2_shift(uint64_t *dst, uint64_t *rem, const uint64_t *src, size_t n,
                        const div3by2_divider<uint64_t> &div) noexcept {
    WJR_ASSERT_ASSUME(n >= 2);
    WJR_ASSERT(div.get_shift() != 0);
    WJR_ASSERT_L2(WJR_IS_SAME_OR_DECR_P(dst, n - 2, src, n - 2));
    WJR_ASSERT_L2(WJR_IS_SEPARATE_P(dst, n - 2, rem, 2));

    const uint64_t divisor0 = div.get_divisor0();
    const uint64_t divisor1 = div.get_divisor1();
    const uint64_t value = div.get_value();
    const auto shift = div.get_shift();

    uint64_t qh;
    uint64_t u0, u1, u2;
    uint64_t rbp;

    rbp = src[n - 2];
    u2 = src[n - 1];
    u1 = shld(u2, rbp, shift);
    u2 >>= (64 - shift);

    n -= 2;

    do {
        if (WJR_UNLIKELY(n == 0)) {
            qh = div.divide(divisor0, divisor1, value, rbp << shift, u1, u2);
            break;
        }

        u0 = src[n - 1];
        qh = div.divide(divisor0, divisor1, value, shld(rbp, u0, shift), u1, u2);
        rbp = u0;
        --n;

        if (WJR_LIKELY(n != 0)) {
            do {
                u0 = src[n - 1];
                dst[n] =
                    div.divide(divisor0, divisor1, value, shld(rbp, u0, shift), u1, u2);
                rbp = u0;
                --n;
            } while (WJR_LIKELY(n != 0));
        }

        dst[0] = div.divide(divisor0, divisor1, value, rbp << shift, u1, u2);
    } while (0);

    rem[0] = shrd(u1, u2, shift);
    rem[1] = u2 >> shift;
    return qh;
}

uint64_t sb_div_qr_s(uint64_t *dst, uint64_t *src, size_t n, const uint64_t *div,
                     size_t m, uint64_t dinv) noexcept {
    using divider = div3by2_divider<uint64_t>;
    constexpr uint64_t mask = in_place_max;

    WJR_ASSERT_ASSUME(m > 2);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT(__has_high_bit(div[m - 1]));

    uint64_t n1, n0;
    uint64_t d1, d0;
    uint64_t cy;
    uint64_t q;

    src += n;

    const uint64_t qh = reverse_compare_n(src - m, div, m) >= 0;
    if (qh != 0) {
        (void)subc_n(src - m, src - m, div, m);
    }

    dst += n - m;

    m -= 2;
    d1 = div[m + 1];
    d0 = div[m + 0];

    src -= 2;

    n1 = src[1];

    for (size_t i = n - (m + 2); i > 0; i--) {
        src--;
        if (WJR_UNLIKELY(n1 == d1) && src[1] == d0) {
            q = mask;
            submul_1(src - m, div, m + 2, q);
            n1 = src[1];
        } else {
            n0 = src[1];
            q = divider::divide(d0, d1, dinv, src[0], n0, n1);

            cy = submul_1(src - m, div, m, q);
            cy = __subc_cc_128(n0, n1, n0, n1, cy, 0, 0);
            src[0] = n0;

            if (WJR_UNLIKELY(cy != 0)) {
                n1 += d1 + addc_n(src - m, src - m, div, m + 1);
                q--;
            }
        }

        *--dst = q;
    }

    src[1] = n1;

    return qh;
}

uint64_t dc_div4by2_qr(uint64_t *dst, uint64_t *src, const uint64_t *div, size_t m,
                       uint64_t dinv, uint64_t *stk) noexcept {
    size_t lo, hi;
    uint64_t cy, qh, ql;

    lo = m >> 1; /* floor(n/2) */
    hi = m - lo; /* ceil(n/2) */

    if (hi < dc_div_qr_threshold) {
        qh = sb_div_qr_s(dst + lo, src + 2 * lo, 2 * hi, div + lo, hi, dinv);
    } else {
        qh = dc_div4by2_qr(dst + lo, src + 2 * lo, div + lo, hi, dinv, stk);
    }

    mul_s(stk, dst + lo, hi, div, lo);

    cy = subc_n(src + lo, src + lo, stk, m);
    if (qh != 0) {
        cy += subc_n(src + m, src + m, div, lo);
    }

    while (cy != 0) {
        qh -= subc_1(dst + lo, dst + lo, hi, 1);
        cy -= addc_n(src + lo, src + lo, div, m);
    }

    if (lo < dc_div_qr_threshold) {
        ql = sb_div_qr_s(dst, src + hi, 2 * lo, div + hi, lo, dinv);
    } else {
        ql = dc_div4by2_qr(dst, src + hi, div + hi, lo, dinv, stk);
    }

    mul_s(stk, div, hi, dst, lo);

    cy = subc_n(src, src, stk, m);
    if (ql != 0) {
        cy += subc_n(src + lo, src + lo, div, hi);
    }

    while (cy != 0) {
        (void)subc_1(dst, dst, lo, 1);
        cy -= addc_n(src, src, div, m);
    }

    return qh;
}

uint64_t dc_div_qr_s(uint64_t *dst, uint64_t *src, size_t n, const uint64_t *div,
                     size_t m, uint64_t dinv) noexcept {
    WJR_ASSERT(m >= 6);
    WJR_ASSERT(n - m >= 3);
    WJR_ASSERT(__has_high_bit(div[m - 1]));

    size_t qn;
    uint64_t qh, cy;

    unique_stack_allocator stkal(math_detail::stack_alloc);
    const auto tp = static_cast<uint64_t *>(stkal.allocate(sizeof(uint64_t) * m));

    qn = n - m;
    dst += qn;
    src += n;
    div += m;

    if (qn > m) {
        /* Reduce qn mod m without division, optimizing small operations.  */
        if (qn >= 8 * m) {
            qn %= m;
            if (qn == 0) {
                qn = m;
            }
        } else {
            do {
                qn -= m;
            } while (qn > m);
        }

        dst -= qn; /* point at low limb of next quotient block */
        src -= qn; /* point in the middle of partial remainder */

        /* Perform the typically smaller block first.  */
        if (qn == 1) {
            uint64_t q, n1, n0, d1, d0;

            /* Handle qh up front, for simplicity. */
            qh = reverse_compare_n(src - m + 1, div - m, m) >= 0;
            if (qh) {
                (void)subc_n(src - m + 1, src - m + 1, div - m, m);
            }

            /* A single iteration of schoolbook: One 3/2 division,
               followed by the bignum update and adjustment. */
            n1 = src[0];
            n0 = src[-1];
            d1 = div[-1];
            d0 = div[-2];

            WJR_ASSERT(n1 < d1 || (n1 == d1 && n0 <= d0));

            if (WJR_UNLIKELY(n1 == d1) && n0 == d0) {
                q = in_place_max;
                cy = submul_1(src - m, div - m, m, q);
            } else {
                q = wjr::div3by2_divider<uint64_t>::divide(d0, d1, dinv, src[-2], n0, n1);

                if (m > 2) {
                    cy = submul_1(src - m, div - m, m - 2, q);
                    cy = __subc_cc_128(n0, n1, n0, n1, cy, 0, 0);
                    src[-2] = n0;

                    if (WJR_UNLIKELY(cy != 0)) {
                        n1 += d1 + addc_n(src - m, src - m, div - m, m - 1);
                        qh -= (q == 0);
                        --q;
                    }
                } else {
                    src[-2] = n0;
                }

                src[-1] = n1;
            }
            dst[0] = q;
        } else {
            /* Do a 2qn / qn division */
            if (qn == 2) {
                qh = div_qr_2_noshift(
                    dst, src - 2, src - 2, 4,
                    div3by2_divider_noshift<uint64_t>(div[-2], div[-1], dinv));
            } else if (qn < dc_div_qr_threshold) {
                qh = sb_div_qr_s(dst, src - qn, 2 * qn, div - qn, qn, dinv);
            } else {
                qh = dc_div4by2_qr(dst, src - qn, div - qn, qn, dinv, tp);
            }

            if (qn != m) {
                if (qn > m - qn) {
                    mul_s(tp, dst, qn, div - m, m - qn);
                } else {
                    mul_s(tp, div - m, m - qn, dst, qn);
                }

                cy = subc_n(src - m, src - m, tp, m);
                if (qh != 0) {
                    cy += subc_n(src - m + qn, src - m + qn, div - m, m - qn);
                }

                while (cy != 0) {
                    qh -= subc_1(dst, dst, qn, 1);
                    cy -= addc_n(src - m, src - m, div - m, m);
                }
            }
        }

        qn = n - m - qn;
        do {
            dst -= m;
            src -= m;
            dc_div4by2_qr(dst, src - m, div - m, m, dinv, tp);
            qn -= m;
        } while (qn > 0);
    } else {
        dst -= qn; /* point at low limb of next quotient block */
        src -= qn; /* point in the middle of partial remainder */

        if (qn < dc_div_qr_threshold) {
            qh = sb_div_qr_s(dst, src - qn, 2 * qn, div - qn, qn, dinv);
        } else {
            qh = dc_div4by2_qr(dst, src - qn, div - qn, qn, dinv, tp);
        }

        if (qn != m) {
            if (qn > m - qn) {
                mul_s(tp, dst, qn, div - m, m - qn);
            } else {
                mul_s(tp, div - m, m - qn, dst, qn);
            }

            cy = subc_n(src - m, src - m, tp, m);
            if (qh != 0) {
                cy += subc_n(src - m + qn, src - m + qn, div - m, m - qn);
            }

            while (cy != 0) {
                qh -= subc_1(dst, dst, qn, 1);
                cy -= addc_n(src - m, src - m, div - m, m);
            }
        }
    }

    return qh;
}

void __div_qr_s_impl(uint64_t *dst, uint64_t *rem, const uint64_t *src, size_t n,
                     const uint64_t *div, size_t m) noexcept {
    switch (m) {
    case 0: {
        WJR_UNREACHABLE();
        break;
    }
    case 1: {
        return div_qr_1(dst, rem[0], src, n, div[0]);
    }
    case 2: {
        return div_qr_2(dst, rem, src, n, div);
    }
    default: {
        break;
    }
    }

    unsigned int adjust = src[n - 1] >= div[m - 1];
    if (n + adjust >= 2 * m) {
        uint64_t *sp;
        uint64_t *dp;

        dst[n - m] = 0;

        const auto shift = clz(div[m - 1]);
        const size_t alloc = n + 1 + (shift != 0 ? m : 0);
        unique_stack_allocator stkal(math_detail::stack_alloc);
        auto stk = static_cast<uint64_t *>(stkal.allocate(sizeof(uint64_t) * alloc));
        sp = stk;

        if (shift != 0) {
            dp = stk + (n + 1);
            (void)lshift_n(dp, div, m, shift);
            sp[n] = lshift_n(sp, src, n, shift);
        } else {
            dp = const_cast<uint64_t *>(div);
            std::copy_n(src, n, sp);
            sp[n] = 0;
        }

        n += adjust;

        const auto dinv = div3by2_divider<uint64_t>::reciprocal(dp[m - 2], dp[m - 1]);

        if (m < dc_div_qr_threshold) {
            sb_div_qr_s(dst, sp, n, dp, m, dinv);
        } else {
            dc_div_qr_s(dst, sp, n, dp, m, dinv);
        }

        rshift_n(rem, sp, m, shift);
        return;
    }

    // 2 * m > n + adjust

    size_t qn = n - m;
    dst[qn] = 0;
    qn += adjust;

    if (qn == 0) {
        std::copy_n(src, m, rem);
        return;
    }

    uint64_t *sp, *dp;
    size_t st;

    st = m - qn; // st = m - qn = 2 * m - n + adjust > 2 * adjust

    const auto shift = clz(div[m - 1]);

    const size_t alloc = 2 * qn + (shift != 0 ? qn : 0);
    unique_stack_allocator stkal(math_detail::stack_alloc);
    auto stk = static_cast<uint64_t *>(stkal.allocate(sizeof(uint64_t) * alloc));
    sp = stk;

    if (shift != 0) {
        dp = stk + 2 * qn;
        (void)lshift_n(dp, div + st, qn, shift, div[st - 1]);
        if (adjust) {
            sp[2 * qn - 1] =
                lshift_n(sp, src + n - 2 * qn + 1, 2 * qn - 1, shift, src[n - 2 * qn]);
        } else {
            (void)lshift_n(sp, src + n - 2 * qn, 2 * qn, shift, src[n - 2 * qn - 1]);
        }
    } else {
        dp = const_cast<uint64_t *>(div + st);
        if (adjust) {
            std::copy_n(src + n - 2 * qn + 1, 2 * qn - 1, sp);
            sp[2 * qn - 1] = 0;
        } else {
            std::copy_n(src + n - 2 * qn, 2 * qn, sp);
        }
    }

    if (qn == 1) {
        const auto dinv = div2by1_divider<uint64_t>::reciprocal(dp[0]);
        auto hi = sp[1];
        dst[0] = div2by1_divider<uint64_t>::divide(dp[0], dinv, sp[0], hi);
        sp[0] = hi;
    } else if (qn == 2) {
        const auto lo = dp[0];
        const auto hi = dp[1];
        const auto dinv = div3by2_divider<uint64_t>::reciprocal(lo, hi);
        // maybe inlined ?
        div_qr_2_noshift(dst, sp, sp, 4, div3by2_divider_noshift<uint64_t>(lo, hi, dinv));
    } else {
        const auto lo = dp[qn - 2];
        const auto hi = dp[qn - 1];
        const auto dinv = div3by2_divider<uint64_t>::reciprocal(lo, hi);
        if (qn < dc_div_qr_threshold) {
            sb_div_qr_s(dst, sp, 2 * qn, dp, qn, dinv);
        } else {
            dc_div_qr_s(dst, sp, 2 * qn, dp, qn, dinv);
        }
    }

    WJR_ASSUME(st >= 1);

    const auto rp = static_cast<uint64_t *>(stkal.allocate(sizeof(uint64_t) * m));

    size_t cf;

    if (!shift) {
        if (qn >= st) {
            mul_s(rp, dst, qn, div, st);
        } else {
            mul_s(rp, div, st, dst, qn);
        }
        cf = subc_n(rem, src, rp, st);
    } else {
        constexpr auto digits = std::numeric_limits<uint64_t>::digits;
        const uint64_t fix = rshift_n(sp, sp, qn, shift);
        const uint64_t mask = (1ull << (digits - shift)) - 1;

        if (st != 1) {
            if (qn >= st - 1) {
                mul_s(rp, dst, qn, div, st - 1);
            } else {
                mul_s(rp, div, st - 1, dst, qn);
            }
            rp[m - 1] = addmul_1(rp + st - 1, dst, qn, div[st - 1] & mask);
            cf = subc_n(rem, src, rp, st - 1);
            rem[st - 1] = subc((src[st - 1] & mask) | fix, rp[st - 1], cf, cf);
        } else {
            rp[m - 1] = mul_1(rp, dst, qn, div[0] & mask);
            rem[0] = subc((src[0] & mask) | fix, rp[0], 0, cf);
        }
    }

    cf = subc_n(rem + st, sp, rp + st, qn, cf);

    while (cf != 0) {
        (void)subc_1(dst, dst, qn, 1);
        cf -= addc_n(rem, rem, div, m);
    }

    return;
}

} // namespace wjr