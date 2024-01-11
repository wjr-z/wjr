#ifndef WJR_MATH_DIV_HPP__
#define WJR_MATH_DIV_HPP__

#include <wjr/math/divider.hpp>

namespace wjr {

// reference : https://ieeexplore.ieee.org/document/5487506
template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR_E T fallback_divmod_1(T *dst, const T *src, size_t n,
                                    div2by1_divider<T> div) {
    uint64_t divisor = div.divisor();
    uint64_t value = div.value();
    unsigned int shift = div.shift();

    uint64_t rax = 0, rdx = 0, rbp = 0, r10 = 0, r11 = 0, r13 = 0;

    if (WJR_UNLIKELY(shift == 0)) {
        rbp = src[n - 1];

        rax = rbp;
        rbp -= divisor;

        bool f = rbp < rax;
        rax = f ? rbp : rax;
        r13 = f;

        if (WJR_VERY_UNLIKELY(rax >= divisor, 0.97)) {
            WJR_FORCE_BRANCH_BARRIER();
            rax -= divisor;
            ++r13;
        }

        dst[n - 1] = r13;
        --n;

        if (WJR_UNLIKELY(n == 0)) {
            return rax;
        }

        do {
            rbp = src[n - 1];
            r11 = rax + 1;

            rax = mul(rax, value, rdx);
            rax += rbp;
            rdx += r11 + (rax < rbp);

            r11 = rax;
            r13 = rdx;
            rdx = mullo(rdx, divisor);
            rbp -= rdx;

            rax = divisor;
            rax += rbp;

            bool f = rbp < r11;
            rax = f ? rbp : rax;
            r13 += -1 + f;

            if (WJR_VERY_UNLIKELY(rax >= divisor, 0.97)) {
                WJR_FORCE_BRANCH_BARRIER();
                rax -= divisor;
                ++r13;
            }

            dst[n - 1] = r13;
            --n;
        } while (WJR_LIKELY(n != 0));

        return rax;
    }

    --n;
    rbp = src[n];
    rax = rbp >> (64 - shift);

    if (WJR_LIKELY(n != 0)) {
        do {
            r10 = src[n - 1];
            r11 = rax + 1;
            rbp = shld(rbp, r10, shift);

            rax = mul(rax, value, rdx);
            rax += rbp;
            rdx += r11 + (rax < rbp);

            r11 = rax;
            r13 = rdx;
            rdx = mullo(rdx, divisor);
            rbp -= rdx;

            rax = divisor;
            rax += rbp;

            bool f = rbp < r11;
            rax = f ? rbp : rax;
            r13 += -1 + f;

            if (WJR_VERY_UNLIKELY(rax >= divisor, 0.97)) {
                WJR_FORCE_BRANCH_BARRIER();
                rax -= divisor;
                ++r13;
            }

            dst[n] = r13;
            --n;
            rbp = r10;
        } while (WJR_LIKELY(n != 0));
    }

    r11 = rax + 1;
    rbp = rbp << shift;

    rax = mul(rax, value, rdx);
    rax += rbp;
    rdx += r11 + (rax < rbp);

    r11 = rax;
    r13 = rdx;
    rdx = mullo(rdx, divisor);
    rbp -= rdx;

    rax = divisor;
    rax += rbp;

    bool f = rbp < r11;
    rax = f ? rbp : rax;
    r13 += -1 + f;

    if (WJR_VERY_UNLIKELY(rax >= divisor, 0.97)) {
        WJR_FORCE_BRANCH_BARRIER();
        rax -= divisor;
        ++r13;
    }

    dst[0] = r13;

    return rax >> shift;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E T divmod_1(T *dst, const T *src, size_t n,
                                     div2by1_divider<T> div) {
    WJR_ASSERT(n != 0);
    WJR_ASSUME(n != 0);

    if (WJR_UNLIKELY(div.is_power_of_two())) {
        unsigned int c = 63 - div.shift();
        T ret = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return ret;
    }

    if (WJR_BUILTIN_CONSTANT_P(div.divisor()) && WJR_BUILTIN_CONSTANT_P(div.shift()) &&
        WJR_UNLIKELY(n == 1)) {
        uint64_t divisor = div.divisor() >> div.shift();
        uint64_t tmp = src[0];
        dst[0] = tmp / divisor;
        return tmp % divisor;
    }

    return fallback_divmod_1(dst, src, n, div);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E T divmod_1(T *dst, const T *src, size_t n,
                                     type_identity_t<T> div) {
    WJR_ASSERT(n != 0);
    WJR_ASSUME(n != 0);

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

// reference : ftp://ftp.risc.uni-linz.ac.at/pub/techreports/1992/92-35.ps.gz
// TODO : asm_divexact_1 (Low priority)
template <typename T, std::enable_if_t<std::is_same_v<T, uint64_t>, int> = 0>
WJR_CONSTEXPR_E void fallback_divexact_1(T *dst, const T *src, size_t n,
                                         divexact1_divider<T> div) {
    uint64_t divisor = div.divisor();
    uint64_t value = div.value();
    unsigned int shift = div.shift();

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
        r10 = subc(r10, rdx, cf, cf);
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
    r10 = subc(r10, rdx, cf, cf);
    r10 = mullo(r10, value);
    dst[n] = r10;
    return;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          divexact1_divider<T> div) {
    WJR_ASSERT(n != 0);
    WJR_ASSUME(n != 0);

    if (WJR_UNLIKELY(div.is_power_of_two())) {
        unsigned int c = div.shift();
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, div);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          type_identity_t<T> div) {
    WJR_ASSERT(n != 0);
    WJR_ASSUME(n != 0);

    if (WJR_UNLIKELY(is_power_of_two(div))) {
        unsigned int c = ctz(div);
        (void)rshift_n(dst, src, n, c);
        return;
    }

    return fallback_divexact_1(dst, src, n, divexact1_divider<T>(div));
}

} // namespace wjr

#endif // WJR_MATH_DIV_HPP__