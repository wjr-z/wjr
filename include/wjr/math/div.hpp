#ifndef WJR_MATH_DIV_HPP__
#define WJR_MATH_DIV_HPP__

#include <wjr/math/div-impl.hpp>
#include <wjr/math/divider.hpp>

#if defined(WJR_X86)
#include <wjr/x86/div.hpp>
#endif

namespace wjr {

// reference : https://ieeexplore.ieee.org/document/5487506

template <typename T>
WJR_NOINLINE WJR_CONSTEXPR20 T div_qr_1_without_shift(T *dst, T &rem, const T *src,
                                                      size_t n,
                                                      const div2by1_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));

    T divisor = div.get_divisor();
    T value = div.get_value();

    T qh = 0;
    T lo, hi;

    hi = src[n - 1];

    if (hi >= divisor) {
        hi -= divisor;
        qh = 1;
    }

    do {
        if (WJR_UNLIKELY(n == 1)) {
            break;
        }

        --n;

        do {
            lo = src[n - 1];
            dst[n - 1] = div.divide(divisor, value, lo, hi);
            --n;
        } while (WJR_LIKELY(n != 0));

    } while (0);

    rem = hi;
    return qh;
}

template <typename T>
WJR_NOINLINE WJR_CONSTEXPR20 T div_qr_1_with_shift(T *dst, T &rem, const T *src, size_t n,
                                                   const div2by1_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(div.get_shift() != 0);
    WJR_ASSERT(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));

    T divisor = div.get_divisor();
    T value = div.get_value();
    unsigned int shift = div.get_shift();

    T qh;
    T lo, hi;

    T rbp = src[n - 1];
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

template <typename T>
WJR_INTRINSIC_CONSTEXPR20 T fallback_div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                              const div2by1_divider<T> &div) {
    if (div.get_shift() == 0) {
        return div_qr_1_without_shift(dst, rem, src, n, div);
    }

    return div_qr_1_with_shift(dst, rem, src, n, div);
}

// return high quotient limb
template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        const div2by1_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(div.is_power_of_two())) {
        unsigned int c = 63 - div.get_shift();
        rem = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    dst[n - 1] = fallback_div_qr_1(dst, rem, src, n, div);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        type_identity_t<T> div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(is_power_of_two(div))) {
        unsigned int c = ctz(div);
        rem = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    if (WJR_UNLIKELY(n == 1)) {
        if (__has_high_bit(div)) {
            T tmp = src[0];
            if (tmp >= div) {
                rem = tmp - div;
                dst[0] = 1;
                return;
            }
            rem = tmp;
            dst[0] = 0;
            return;
        }

        T tmp = src[0];
        dst[0] = tmp / div;
        rem = tmp % div;
        return;
    }

    dst[n - 1] = fallback_div_qr_1(dst, rem, src, n, div2by1_divider<T>(div));
}

template <typename T>
WJR_NOINLINE WJR_CONSTEXPR20 T div_qr_2_without_shift(T *dst, T *rem, const T *src,
                                                      size_t n,
                                                      const div3by2_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 2);
    WJR_ASSERT(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));

    T divisor0 = div.get_divisor0();
    T divisor1 = div.get_divisor1();
    T value = div.get_value();

    T qh = 0;
    T u0, u1, u2;

    u2 = src[n - 1];
    u1 = src[n - 2];

    if (__less_equal_128(divisor0, divisor1, u1, u2)) {
        __subc_128(u1, u2, u1, u2, divisor0, divisor1);
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

template <typename T>
WJR_NOINLINE WJR_CONSTEXPR20 T div_qr_2_with_shift(T *dst, T *rem, const T *src, size_t n,
                                                   const div3by2_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 2);
    WJR_ASSERT(div.get_shift() != 0);
    WJR_ASSERT(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));

    T divisor0 = div.get_divisor0();
    T divisor1 = div.get_divisor1();
    T value = div.get_value();
    unsigned int shift = div.get_shift();

    T qh;
    T u0, u1, u2;
    T rbp;

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

template <typename T>
WJR_INTRINSIC_CONSTEXPR20 T fallback_div_qr_2(T *dst, T *rem, const T *src, size_t n,
                                              const div3by2_divider<T> &div) {
    if (div.get_shift() == 0) {
        return div_qr_2_without_shift(dst, rem, src, n, div);
    }

    return div_qr_2_with_shift(dst, rem, src, n, div);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(T *dst, T *rem, const T *src, size_t n,
                                        const div3by2_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 2);

    dst[n - 2] = fallback_div_qr_2(dst, rem, src, n, div);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(T *dst, T *rem, const T *src, size_t n, T *div) {
    WJR_ASSERT_ASSUME(n >= 2);

    dst[n - 2] = fallback_div_qr_2(dst, rem, src, n, div3by2_divider<T>(div[0], div[1]));
}

// reference : GMP
// return qh;
template <typename T>
WJR_NOINLINE WJR_CONSTEXPR20 T sb_div_qr_s(T *dst, T *src, size_t n, const T *div,
                                           size_t m, T dinv) {
    using divider = div3by2_divider<T>;
    constexpr T mask = std::numeric_limits<T>::max();

    WJR_ASSERT_ASSUME(m > 2);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT(__has_high_bit(div[m - 1]));

    T qh;
    T n1, n0;
    T d1, d0;
    T cy, cy1;
    T q;

    src += n;

    qh = reverse_compare_n(src - m, div, m) >= 0;
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

            n0 = subc(n0, cy, 0u, cy1);
            n1 = subc(n1, cy1, 0u, cy);
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

#ifndef WJR_DC_DIV_QR_THRESHOLD
#define WJR_DC_DIV_QR_THRESHOLD (WJR_TOOM22_MUL_THRESHOLD * 2)
#endif // WJR_DC_DIV_QR_THRESHOLD

inline constexpr size_t dc_div_qr_threshold = WJR_DC_DIV_QR_THRESHOLD;

template <typename T>
WJR_NOINLINE T dc_div4by2_qr(T *dst, T *src, const T *div, size_t m, T dinv, T *stk) {
    size_t lo, hi;
    T cy, qh, ql;

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
        qh -= subc_1(dst + lo, dst + lo, hi, 1u);
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
        subc_1(dst, dst, lo, 1u);
        cy -= addc_n(src, src, div, m);
    }

    return qh;
}

template <typename T>
T dc_div_qr_s(T *dst, T *src, size_t n, const T *div, size_t m, T dinv) {
    WJR_ASSERT(m >= 6);
    WJR_ASSERT(n - m >= 3);
    WJR_ASSERT(__has_high_bit(div[m - 1]));

    size_t qn;
    T qh, cy;
    T *tp;

    unique_stack_ptr ptr(math_details::stack_alloc, sizeof(T) * m);
    tp = static_cast<T *>(ptr.get());

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
            T q, n1, n0, d1, d0;

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
                q = std::numeric_limits<T>::max();
                cy = submul_1(src - m, div - m, m, q);
            } else {
                q = wjr::div3by2_divider<T>::divide(d0, d1, dinv, src[-2], n0, n1);

                if (m > 2) {
                    T cy, cy1;
                    cy = submul_1(src - m, div - m, m - 2, q);

                    n0 = subc(n0, cy, 0u, cy1);
                    n1 = subc(n1, cy1, 0u, cy);

                    src[-2] = n0;

                    if (WJR_UNLIKELY(cy != 0)) {
                        n1 += d1 + addc_n(src - m, src - m, div - m, m - 1);
                        qh -= (q == 0);
                        q = (q - 1);
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
                qh = div_qr_2_without_shift(
                    dst, src - 2, src - 2, 4,
                    div3by2_divider<T>(div[-2], div[-1], dinv, 0u));
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

template <typename T>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_s(T *dst, T *rem, const T *src, size_t n, T *div,
                                        size_t m) {
    WJR_ASSERT_ASSUME(m >= 1);
    WJR_ASSERT_ASSUME(n >= m);

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
        WJR_UNREACHABLE();
        break;
    }
    }
}

template <typename T>
WJR_CONSTEXPR_E T fallback_divexact_dbm1c(T *dst, const T *src, size_t n, T bd, T h) {
    T a = 0, p0 = 0, p1 = 0, cf = 0;

    // GCC can't optimize well
    WJR_UNROLL(4)
    for (size_t i = 0; i < n; i++) {
        a = src[i];
        p0 = mul(a, bd, p1);
        h = subc(h, p0, 0u, cf);
        dst[i] = h;
        h -= p1 + cf;
    }

    return h;
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E T divexact_dbm1c(T *dst, const T *src, size_t n, T bd, T h) {
#if WJR_HAS_BUILTIN(ASM_DIVEXACT_DBM1C)
    if (is_constant_evaluated()) {
        return fallback_divexact_dbm1c(dst, src, n, bd, h);
    }

    return asm_divexact_dbm1c(dst, src, n, bd, h);
#else
    return fallback_divexact_dbm1c(dst, src, n, bd, h);
#endif
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E void divexact_by3(T *dst, const T *src, size_t n) {
    constexpr auto max = std::numeric_limits<T>::max();
    (void)divexact_dbm1c<T>(dst, src, n, max / 3, 0);
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E void divexact_by5(T *dst, const T *src, size_t n) {
    constexpr auto max = std::numeric_limits<T>::max();
    (void)divexact_dbm1c<T>(dst, src, n, max / 5, 0);
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E void divexact_by15(T *dst, const T *src, size_t n) {
    constexpr auto max = std::numeric_limits<T>::max();
    (void)divexact_dbm1c<T>(dst, src, n, max / 15, 0);
}

// reference : ftp://ftp.risc.uni-linz.ac.at/pub/techreports/1992/92-35.ps.gz
// TODO : (Low priority)
template <typename T>
WJR_NOINLINE WJR_CONSTEXPR_E void fallback_divexact_1(T *dst, const T *src, size_t n,
                                                      divexact1_divider<T> div) {
    uint64_t divisor = div.get_divisor();
    uint64_t value = div.get_value();
    unsigned int shift = div.get_shift();

    uint64_t rdx = 0, r10 = 0;
    unsigned char cf = 0;
    size_t idx = 0;

    --n;

    if (WJR_UNLIKELY(shift == 0)) {

        if (WJR_LIKELY(n != 0)) {
            do {
                r10 = src[idx];
                r10 = subc(r10, rdx, cf, cf);
                r10 = mullo(r10, value);
                dst[idx] = r10;
                ++idx;
                rdx = mulhi(r10, divisor);
            } while (WJR_LIKELY(idx != n));
        }

        r10 = src[n];
        r10 -= rdx + cf;
        r10 = mullo(r10, value);
        dst[n] = r10;
        return;
    }

    r10 = src[0];

    if (WJR_LIKELY(n != 0)) {
        do {
            uint64_t r11 = src[idx + 1];
            r10 = shrd(r10, r11, shift);
            r10 = subc(r10, rdx, cf, cf);
            r10 = mullo(r10, value);
            dst[idx] = r10;
            ++idx;
            rdx = mulhi(r10, divisor);
            r10 = r11;
        } while (WJR_LIKELY(idx != n));
    }

    r10 = r10 >> shift;
    r10 -= rdx + cf;
    r10 = mullo(r10, value);
    dst[n] = r10;
    return;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          divexact1_divider<T> div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(div.is_power_of_two())) {
        unsigned int c = div.get_shift();
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, div);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          type_identity_t<T> div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(is_power_of_two(div))) {
        unsigned int c = ctz(div);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, divexact1_divider<T>(div));
}

} // namespace wjr

#endif // WJR_MATH_DIV_HPP__