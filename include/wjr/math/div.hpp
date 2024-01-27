#ifndef WJR_MATH_DIV_HPP__
#define WJR_MATH_DIV_HPP__

#include <wjr/math/divider.hpp>

#if defined(WJR_X86)
#include <wjr/x86/div.hpp>
#endif

namespace wjr {

// reference : https://ieeexplore.ieee.org/document/5487506
template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR20 T fallback_divmod_1(T *dst, const T *src, size_t n,
                                    div2by1_divider<T> div) {
    auto shift = div.get_shift();
    uint64_t hi = 0, hi1 = 1;

    if (WJR_UNLIKELY(shift == 0)) {
        dst[n - 1] = div.divide_without_shift(src[n - 1], hi, hi1);
        --n;

        if (WJR_UNLIKELY(n == 0)) {
            return hi;
        }

        do {
            dst[n - 1] = div.divide_without_shift(src[n - 1], hi, hi1);
            --n;
        } while (WJR_LIKELY(n != 0));

        return hi;
    }

    uint64_t rbp = src[n - 1];
    --n;
    hi = rbp >> (64 - shift);
    hi1 = hi + 1;

    if (WJR_LIKELY(n != 0)) {
        do {
            dst[n] = div.divide_with_shift(src[n - 1], hi, hi1, rbp);
            --n;
        } while (WJR_LIKELY(n != 0));
    }

    dst[0] = div.divide_with_shift(0, hi, hi1, rbp);
    return hi >> shift;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T divmod_1(T *dst, const T *src, size_t n,
                                     div2by1_divider<T> div) {
    WJR_ASSERT(n != 0);

    if (WJR_UNLIKELY(div.is_power_of_two())) {
        unsigned int c = 63 - div.get_shift();
        T ret = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return ret;
    }

    if (WJR_BUILTIN_CONSTANT_P(div.get_divisor()) &&
        WJR_BUILTIN_CONSTANT_P(div.get_shift()) && WJR_UNLIKELY(n == 1)) {
        uint64_t divisor = div.get_divisor() >> div.get_shift();
        uint64_t tmp = src[0];
        dst[0] = tmp / divisor;
        return tmp % divisor;
    }

    return fallback_divmod_1(dst, src, n, div);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T divmod_1(T *dst, const T *src, size_t n,
                                     type_identity_t<T> div) {
    WJR_ASSERT(n != 0);

    if (WJR_UNLIKELY(is_power_of_two(div))) {
        unsigned int c = ctz(div);
        T ret = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return ret;
    }

    if (WJR_UNLIKELY(n == 1)) {
        uint64_t tmp = src[0];
        dst[0] = tmp / div;
        return tmp % div;
    }

    return fallback_divmod_1(dst, src, n, div2by1_divider<T>(div));
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
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

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
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

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR_E void divexact_by5(T *dst, const T *src, size_t n) {
    constexpr auto max = std::numeric_limits<T>::max();
    (void)divexact_dbm1c<T>(dst, src, n, max / 5, 0);
}

template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR_E void divexact_by15(T *dst, const T *src, size_t n) {
    constexpr auto max = std::numeric_limits<T>::max();
    (void)divexact_dbm1c<T>(dst, src, n, max / 15, 0);
}

// reference : ftp://ftp.risc.uni-linz.ac.at/pub/techreports/1992/92-35.ps.gz
// TODO : asm_divexact_1 (Low priority)
template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
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

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
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

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
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

} // namespace wjr

#endif // WJR_MATH_DIV_HPP__