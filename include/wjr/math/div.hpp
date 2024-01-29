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
WJR_CONSTEXPR20 void fallback_div_qr_1_without_shift(T *dst, T &rem, const T *src,
                                                     size_t n,
                                                     const div2by1_divider<T> &div) {
    WJR_ASSERT(div.get_shift() == 0);
    WJR_ASSERT(n >= 1);

    T divisor = div.get_divisor();

    T lo, hi, hi1;

    hi = src[n - 1];

    if (hi >= divisor) {
        hi -= divisor;
        dst[n - 1] = 1;
    } else {
        dst[n - 1] = 0;
    }

    if (WJR_UNLIKELY(!(--n))) {
        rem = hi;
        return;
    }

    hi1 = hi + 1;

    do {
        lo = src[n - 1];
        dst[n - 1] = div.divide_without_shift(lo, hi, hi1);
    } while (--n);

    rem = hi;
    return;
}

template <typename T>
WJR_CONSTEXPR20 void fallback_div_qr_1_with_shift(T *dst, T &rem, const T *src, size_t n,
                                                  const div2by1_divider<T> &div) {
    WJR_ASSERT(div.get_shift() != 0);
    WJR_ASSERT(n >= 1);

    unsigned int shift = div.get_shift();

    T lo, hi, hi1;

    T rbp = src[n - 1];
    --n;
    hi = rbp >> (64 - shift);
    hi1 = hi + 1;

    if (WJR_LIKELY(n != 0)) {
        do {
            lo = src[n - 1];
            dst[n] = div.divide_without_shift(shld(rbp, lo, shift), hi, hi1);
            rbp = lo;
            --n;
        } while (WJR_LIKELY(n != 0));
    }

    dst[0] = div.divide_without_shift(rbp << shift, hi, hi1);
    rem = hi >> shift;

    return;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR20 void fallback_div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                                 const div2by1_divider<T> &div) {
    if (WJR_UNLIKELY(div.get_shift() == 0)) {
        return fallback_div_qr_1_without_shift(dst, rem, src, n, div);
    }

    return fallback_div_qr_1_with_shift(dst, rem, src, n, div);
}

// return high quotient limb
template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        const div2by1_divider<T> &div) {
    WJR_ASSERT(n != 0);

    if (WJR_UNLIKELY(div.is_power_of_two())) {
        unsigned int c = 63 - div.get_shift();
        rem = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    if (WJR_BUILTIN_CONSTANT_P(div.get_divisor()) &&
        WJR_BUILTIN_CONSTANT_P(div.get_shift()) && WJR_UNLIKELY(n == 1)) {
        T divisor = div.get_divisor() >> div.get_shift();
        T tmp = src[0];
        dst[0] = tmp / divisor;
        rem = tmp % divisor;
        return;
    }

    return fallback_div_qr_1(dst, rem, src, n, div);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 T div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                     type_identity_t<T> div) {
    WJR_ASSERT(n != 0);

    if (WJR_UNLIKELY(is_power_of_two(div))) {
        unsigned int c = ctz(div);
        rem = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    if (WJR_UNLIKELY(n == 1)) {
        if (div >> (std::numeric_limits<T>::digits - 1)) {
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

    return fallback_div_qr_1(dst, rem, src, n, div2by1_divider<T>(div));
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
// TODO : asm_divexact_1 (Low priority)
template <typename T>
WJR_CONSTEXPR_E void fallback_divexact_1(T *dst, const T *src, size_t n,
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

    uint64_t r11 = 0;
    r10 = src[0];

    if (WJR_LIKELY(n != 0)) {
        do {
            r11 = src[idx + 1];
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
    WJR_ASSERT(n != 0);

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
    WJR_ASSERT(n != 0);

    if (WJR_UNLIKELY(is_power_of_two(div))) {
        unsigned int c = ctz(div);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, divexact1_divider<T>(div));
}

template <typename T>
WJR_CONSTEXPR20 T fallback_divmod_2(T *dst, const T *src, size_t n,
                                    const div3by2_divider<T> &div) {}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 T divmod_2(T *dst, const T *src, size_t n,
                                     const div3by2_divider<T> &div) {}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_INTRINSIC_CONSTEXPR20 T divmod_2(T *dst, const T *src, size_t n,
                                     type_identity_t<T> d0, type_identity_t<T> d1) {
    return divmod_2(dst, src, n, div3by2_divider<T>(d0, d1));
}

} // namespace wjr

#endif // WJR_MATH_DIV_HPP__