#ifndef WJR_MATH_DIV_HPP__
#define WJR_MATH_DIV_HPP__

#include <wjr/math/compare.hpp>
#include <wjr/math/div-impl.hpp>
#include <wjr/math/divider.hpp>

#if defined(WJR_X86)
#include <wjr/x86/math/div.hpp>
#endif

namespace wjr {

/*
 TODO :
 1. __div_constant_128
 2. __mod_constant_128
 3. __div_qr_constant_128
 4. optimize constant divisor of div_qr_1
 1, 2, 3, 4: constant numbers that can be divisible by (uint64_t)(-1),
*/

inline uint64_t
div128by64to64_noshift(uint64_t &rem, uint64_t lo, uint64_t hi,
                       const wjr::div2by1_divider_noshift<uint64_t> &divider) {
    uint64_t result = divider.divide(lo, hi);
    rem = hi;
    return result;
}

inline uint64_t div128by64to64_shift(uint64_t &rem, uint64_t lo, uint64_t hi,
                                     const wjr::div2by1_divider<uint64_t> &divider) {
    auto shift = divider.get_shift();
    hi = shld(hi, lo, shift);
    lo <<= shift;
    uint64_t result = divider.get_base().divide(lo, hi);
    rem = hi >> shift;
    return result;
}

inline uint64_t div128by64to64_impl(uint64_t &rem, uint64_t lo, uint64_t hi,
                                    const wjr::div2by1_divider<uint64_t> &divider) {
    if (divider.get_shift() == 0) {
        return div128by64to64_noshift(rem, lo, hi, divider);
    }

    return div128by64to64_shift(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                               const div2by1_divider<uint64_t> &divider) {
    return div128by64to64_impl(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi, uint64_t div) {
    return div128by64to64_impl(rem, lo, hi, wjr::div2by1_divider<uint64_t>(div));
}

inline std::pair<uint64_t, uint64_t>
div128by64to128_noshift(uint64_t &rem, uint64_t lo, uint64_t hi,
                        const div2by1_divider_noshift<uint64_t> &divider) {
    auto divisor = divider.get_divisor();
    uint64_t q0, q1 = 0;

    if (hi >= divisor) {
        q1 = 1;
        hi -= divisor;
    }

    q0 = divider.divide(lo, hi);
    rem = hi;
    return std::make_pair(q0, q1);
}

inline std::pair<uint64_t, uint64_t>
div128by64to128_shift(uint64_t &rem, uint64_t lo, uint64_t hi,
                      const div2by1_divider<uint64_t> &divider) {
    auto shift = divider.get_shift();
    uint64_t u0, u1, u2;
    uint64_t q0, q1;

    u2 = hi >> (64 - shift);
    u1 = shld(hi, lo, shift);
    u0 = lo << shift;

    const auto &div = divider.get_base();
    q1 = div.divide(u1, u2);
    q0 = div.divide(u0, u2);

    rem = u2 >> shift;
    return std::make_pair(q0, q1);
}

inline std::pair<uint64_t, uint64_t>
div128by64to128_impl(uint64_t &rem, uint64_t lo, uint64_t hi,
                     const div2by1_divider<uint64_t> &divider) {
    if (divider.get_shift() == 0) {
        return div128by64to128_noshift(rem, lo, hi, divider);
    }

    return div128by64to128_shift(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline std::pair<uint64_t, uint64_t>
div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                const div2by1_divider<uint64_t> &divider) {
    return div128by64to128_impl(rem, lo, hi, divider);
}

/*
 not optimize for divider that is power of 2,
 manually consider whether it needs to be optimized
*/
inline std::pair<uint64_t, uint64_t> div128by64to128(uint64_t &rem, uint64_t lo,
                                                     uint64_t hi, uint64_t div) {
    return div128by64to128_impl(rem, lo, hi, div2by1_divider<uint64_t>(div));
}

template <>
class div1by1_divider<uint64_t> {
public:
private:
};

// reference : https://ieeexplore.ieee.org/document/5487506
template <typename T>
WJR_CONSTEXPR20 T div_qr_1_noshift(T *dst, T &rem, const T *src, size_t n,
                                   const div2by1_divider_noshift<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));

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
WJR_CONSTEXPR20 T div_qr_1_shift(T *dst, T &rem, const T *src, size_t n,
                                 const div2by1_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(div.get_shift() != 0);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));

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
WJR_INTRINSIC_CONSTEXPR20 T div_qr_1_impl(T *dst, T &rem, const T *src, size_t n,
                                          const div2by1_divider<T> &div) {
    if (div.get_shift() == 0) {
        return div_qr_1_noshift(dst, rem, src, n, div);
    }

    return div_qr_1_shift(dst, rem, src, n, div);
}

// return high quotient limb
template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        const div2by1_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(div.is_zero_or_single_bit())) {
        unsigned int c = 63 - div.get_shift();
        rem = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    if (WJR_BUILTIN_CONSTANT_P(n == 2) && n == 2) {
        auto [ax, dx] = div128by64to128(rem, src[0], src[1], div);
        dst[0] = ax;
        dst[1] = dx;
        return;
    }

    dst[n - 1] = div_qr_1_impl(dst, rem, src, n, div);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        type_identity_t<T> div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(is_zero_or_single_bit(div))) {
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

    if (WJR_BUILTIN_CONSTANT_P(n == 2) && n == 2) {
        auto [ax, dx] = div128by64to128(rem, src[0], src[1], div);
        dst[0] = ax;
        dst[1] = dx;
        return;
    }

    dst[n - 1] = div_qr_1_impl(dst, rem, src, n, div2by1_divider<T>(div));
}

template <typename T>
WJR_CONSTEXPR20 T div_qr_2_noshift(T *dst, T *rem, const T *src, size_t n,
                                   const div3by2_divider_noshift<T> &div) {
    WJR_ASSERT_ASSUME(n >= 2);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n, rem, n));

    T divisor0 = div.get_divisor0();
    T divisor1 = div.get_divisor1();
    T value = div.get_value();

    T qh = 0;
    T u0, u1, u2;

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

template <typename T>
WJR_CONSTEXPR20 T div_qr_2_shift(T *dst, T *rem, const T *src, size_t n,
                                 const div3by2_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 2);
    WJR_ASSERT(div.get_shift() != 0);
    WJR_ASSERT_L1(WJR_IS_SAME_OR_DECR_P(dst, n, src, n));
    WJR_ASSERT_L1(WJR_IS_SEPARATE_P(dst, n, rem, n));

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
WJR_INTRINSIC_CONSTEXPR20 T div_qr_2_impl(T *dst, T *rem, const T *src, size_t n,
                                          const div3by2_divider<T> &div) {
    if (div.get_shift() == 0) {
        return div_qr_2_noshift(dst, rem, src, n, div);
    }

    return div_qr_2_shift(dst, rem, src, n, div);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(T *dst, T *rem, const T *src, size_t n,
                                        const div3by2_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 2);

    dst[n - 2] = div_qr_2_impl(dst, rem, src, n, div);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(T *dst, T *rem, const T *src, size_t n,
                                        const T *div) {
    WJR_ASSERT_ASSUME(n >= 2);

    dst[n - 2] = div_qr_2_impl(dst, rem, src, n, div3by2_divider<T>(div[0], div[1]));
}

// reference : GMP
// return qh;
template <typename T>
T sb_div_qr_s(T *dst, T *src, size_t n, const T *div, size_t m, T dinv) {
    using divider = div3by2_divider<T>;
    constexpr T mask = in_place_max;

    WJR_ASSERT_ASSUME(m > 2);
    WJR_ASSERT_ASSUME(n >= m);
    WJR_ASSERT(__has_high_bit(div[m - 1]));

    T qh;
    T n1, n0;
    T d1, d0;
    T cy;
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

extern template uint64_t sb_div_qr_s<uint64_t>(uint64_t *dst, uint64_t *src, size_t n,
                                               const uint64_t *div, size_t m,
                                               uint64_t dinv);

inline constexpr size_t dc_div_qr_threshold = WJR_DC_DIV_QR_THRESHOLD;

template <typename T>
T dc_div4by2_qr(T *dst, T *src, const T *div, size_t m, T dinv, T *stk) {
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
        subc_1(dst, dst, lo, 1);
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

    unique_stack_allocator stkal(math_details::stack_alloc);
    tp = static_cast<T *>(stkal.allocate(sizeof(T) * m));

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
                q = in_place_max;
                cy = submul_1(src - m, div - m, m, q);
            } else {
                q = wjr::div3by2_divider<T>::divide(d0, d1, dinv, src[-2], n0, n1);

                if (m > 2) {
                    T cy;
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
                qh = div_qr_2_noshift(dst, src - 2, src - 2, 4,
                                      div3by2_divider_noshift<T>(div[-2], div[-1], dinv));
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

extern template uint64_t dc_div_qr_s<uint64_t>(uint64_t *dst, uint64_t *src, size_t n,
                                               const uint64_t *div, size_t m,
                                               uint64_t dinv);

template <typename T>
void div_qr_s(T *dst, T *rem, const T *src, size_t n, const T *div, size_t m) {
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
        break;
    }
    }

    unsigned int adjust = src[n - 1] >= div[m - 1];
    if (n + adjust >= 2 * m) {
        T *sp;
        T *dp;

        dst[n - m] = 0;

        const auto shift = clz(div[m - 1]);
        const size_t alloc = n + 1 + (shift != 0 ? m : 0);
        unique_stack_allocator stkal(math_details::stack_alloc);
        auto stk = static_cast<T *>(stkal.allocate(sizeof(T) * alloc));
        sp = stk;

        if (shift != 0) {
            dp = stk + (n + 1);
            lshift_n(dp, div, m, shift);
            sp[n] = lshift_n(sp, src, n, shift);
        } else {
            dp = const_cast<T *>(div);
            std::copy_n(src, n, sp);
            sp[n] = 0;
        }

        n += adjust;

        const auto dinv = div3by2_divider<T>::reciprocal(dp[m - 2], dp[m - 1]);

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

    T *sp, *dp;
    size_t st;

    st = m - qn; // st = m - qn = 2 * m - n + adjust > 2 * adjust

    const auto shift = clz(div[m - 1]);

    const size_t alloc = 2 * qn + (shift != 0 ? qn : 0);
    unique_stack_allocator stkal(math_details::stack_alloc);
    auto stk = static_cast<T *>(stkal.allocate(sizeof(T) * alloc));
    sp = stk;

    if (shift != 0) {
        dp = stk + 2 * qn;
        lshift_n(dp, div + st, qn, shift, div[st - 1]);
        if (adjust) {
            sp[2 * qn - 1] =
                lshift_n(sp, src + n - 2 * qn + 1, 2 * qn - 1, shift, src[n - 2 * qn]);
        } else {
            lshift_n(sp, src + n - 2 * qn, 2 * qn, shift, src[n - 2 * qn - 1]);
        }
    } else {
        dp = const_cast<T *>(div + st);
        if (adjust) {
            std::copy_n(src + n - 2 * qn + 1, 2 * qn - 1, sp);
            sp[2 * qn - 1] = 0;
        } else {
            std::copy_n(src + n - 2 * qn, 2 * qn, sp);
        }
    }

    if (qn == 1) {
        const auto dinv = div2by1_divider<T>::reciprocal(dp[0]);
        auto hi = sp[1];
        dst[0] = div2by1_divider<T>::divide(dp[0], dinv, sp[0], hi);
        sp[0] = hi;
    } else if (qn == 2) {
        const auto lo = dp[0];
        const auto hi = dp[1];
        const auto dinv = div3by2_divider<T>::reciprocal(lo, hi);
        // maybe inlined ?
        div_qr_2_noshift(dst, sp, sp, 4, div3by2_divider_noshift<T>(lo, hi, dinv));
    } else {
        const auto lo = dp[qn - 2];
        const auto hi = dp[qn - 1];
        const auto dinv = div3by2_divider<T>::reciprocal(lo, hi);
        if (qn < dc_div_qr_threshold) {
            sb_div_qr_s(dst, sp, 2 * qn, dp, qn, dinv);
        } else {
            dc_div_qr_s(dst, sp, 2 * qn, dp, qn, dinv);
        }
    }

    WJR_ASSUME(st >= 1);

    auto stk2 = static_cast<T *>(stkal.allocate(sizeof(T) * m));
    auto rp = stk2;

    unsigned int cf;

    if (!shift) {
        if (qn >= st) {
            mul_s(rp, dst, qn, div, st);
        } else {
            mul_s(rp, div, st, dst, qn);
        }
        cf = subc_n(rem, src, rp, st);
    } else {
        constexpr auto digits = std::numeric_limits<T>::digits;
        T fix = rshift_n(sp, sp, qn, shift);
        T mask = (1ull << (digits - shift)) - 1;

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
        subc_1(dst, dst, qn, 1);
        cf -= addc_n(rem, rem, div, m);
    }

    return;
}

extern template void div_qr_s<uint64_t>(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                                        size_t n, const uint64_t *div, size_t m);

template <typename T>
WJR_CONSTEXPR_E T fallback_divexact_dbm1c(T *dst, const T *src, size_t n, T bd, T h) {
    T a = 0, p0 = 0, p1 = 0, cf = 0;

    // GCC can't optimize well
    WJR_UNROLL(4)
    for (size_t i = 0; i < n; i++) {
        a = src[i];
        p0 = mul(a, bd, p1);
        h = subc(h, p0, 0, cf);
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
    constexpr T max = in_place_max;
    (void)divexact_dbm1c<T>(dst, src, n, max / 3, 0);
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E void divexact_by5(T *dst, const T *src, size_t n) {
    constexpr T max = in_place_max;
    (void)divexact_dbm1c<T>(dst, src, n, max / 5, 0);
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E void divexact_by15(T *dst, const T *src, size_t n) {
    constexpr T max = in_place_max;
    (void)divexact_dbm1c<T>(dst, src, n, max / 15, 0);
}

template <typename T, T c, std::enable_if_t<std::is_same_v<T, uint64_t>, int>>
WJR_CONSTEXPR_E void divexact_byc(T *dst, const T *src, size_t n,
                                  std::integral_constant<T, c>) {

    // cost : divexact_dbm1c * 2 + shift * 1 <= divexact_1

    constexpr auto __is_fast = [](auto cr) {
        constexpr T r = get_place_index_v<remove_cvref_t<decltype(cr)>>;
        return c % r == 0 && is_zero_or_single_bit(c / r);
    };

    auto __resolve = [dst, n](auto cr) {
        constexpr T r = get_place_index_v<remove_cvref_t<decltype(cr)>>;
        constexpr auto p = fallback_ctz(c / r);
        if constexpr (p != 0) {
            rshift_n(dst, dst, n, p);
        } else {
            (void)(dst);
            (void)(n);
        }
    };

    if constexpr (__is_fast(std::in_place_index<1>)) {
        __resolve(std::in_place_index<1>);
    } else if constexpr (__is_fast(std::in_place_index<3>)) {
        divexact_by3(dst, src, n);
        __resolve(std::in_place_index<3>);
    } else if constexpr (__is_fast(std::in_place_index<5>)) {
        divexact_by5(dst, src, n);
        __resolve(std::in_place_index<5>);
    } else if constexpr (__is_fast(std::in_place_index<15>)) {
        divexact_by15(dst, src, n);
        __resolve(std::in_place_index<15>);
    } else if constexpr (__is_fast(std::in_place_index<9>)) {
        divexact_by3(dst, src, n);
        divexact_by3(dst, dst, n);
        __resolve(std::in_place_index<9>);
    } else if constexpr (__is_fast(std::in_place_index<25>)) {
        divexact_by5(dst, src, n);
        divexact_by5(dst, dst, n);
        __resolve(std::in_place_index<25>);
    } else if constexpr (__is_fast(std::in_place_index<45>)) {
        divexact_by3(dst, src, n);
        divexact_by15(dst, dst, n);
        __resolve(std::in_place_index<45>);
    } else if constexpr (__is_fast(std::in_place_index<75>)) {
        divexact_by5(dst, src, n);
        divexact_by15(dst, dst, n);
        __resolve(std::in_place_index<75>);
    } else if constexpr (__is_fast(std::in_place_index<225>)) {
        divexact_by15(dst, src, n);
        divexact_by15(dst, dst, n);
        __resolve(std::in_place_index<225>);
    } else {
        constexpr auto shift = fallback_ctz(c);
        using divider_t = divexact1_divider<T>;
        constexpr auto divisor = c >> shift;
        constexpr auto value = divider_t::reciprocal(divisor);
        constexpr auto divider = divider_t(divisor, value, shift);
        divexact_1(dst, src, n, divider);
    }
}

// reference : ftp://ftp.risc.uni-linz.ac.at/pub/techreports/1992/92-35.ps.gz
template <typename T>
WJR_CONSTEXPR_E void fallback_divexact_1_noshift(T *dst, const T *src, size_t n,
                                                 const divexact1_divider<T> &div) {
    uint64_t divisor = div.get_divisor();
    uint64_t value = div.get_value();

    uint64_t rdx = 0, r10 = 0;
    uint64_t cf = 0;
    size_t idx = 0;

    --n;

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

template <typename T>
WJR_CONSTEXPR_E void fallback_divexact_1_shift(T *dst, const T *src, size_t n,
                                               const divexact1_divider<T> &div) {
    uint64_t divisor = div.get_divisor();
    uint64_t value = div.get_value();
    unsigned int shift = div.get_shift();

    uint64_t rdx = 0, r10 = 0;
    uint64_t cf = 0;
    size_t idx = 0;

    --n;
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

template <typename T>
WJR_CONSTEXPR_E void fallback_divexact_1(T *dst, const T *src, size_t n,
                                         const divexact1_divider<T> &div) {
    if (div.get_shift() == 0) {
        return fallback_divexact_1_noshift(dst, src, n, div);
    }

    return fallback_divexact_1_shift(dst, src, n, div);
}

template <typename T,
          std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          const divexact1_divider<T> &div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(div.is_zero_or_single_bit())) {
        unsigned int c = div.get_shift();
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, div);
}

template <typename T, std::enable_if_t<is_nonbool_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          type_identity_t<T> div) {
    WJR_ASSERT_ASSUME(n >= 1);

    if (WJR_UNLIKELY(is_zero_or_single_bit(div))) {
        unsigned int c = ctz(div);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, divexact1_divider<T>(div));
}

} // namespace wjr

#endif // WJR_MATH_DIV_HPP__