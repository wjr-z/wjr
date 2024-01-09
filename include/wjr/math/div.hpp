#ifndef WJR_MATH_DIV_HPP__
#define WJR_MATH_DIV_HPP__

#include <wjr/math/divider.hpp>

namespace wjr {

template <typename T>
constexpr T fallback_divmod_1(T *dst, const T *src, size_t n, div2by1_divider<T> div) {
    WJR_ASSERT(n != 0);
    WJR_ASSUME(n != 0);

    uint64_t divisor = div.divisor();
    uint64_t value = div.value();
    uint64_t shift = div.shift();

    uint64_t u1 = 0;
    uint64_t u0 = 0;
    uint64_t q1 = 0;
    uint64_t q0 = 0;
    uint64_t tmp = 0;
    bool f = false;

    if (WJR_UNLIKELY(shift == 0)) {
        u0 = src[n - 1];

        q0 = u0;
        q1 = 1;

        u1 = u0 - divisor;
        tmp = u1 + divisor;

        f = u1 < q0;
        u1 = f ? u1 : tmp;
        q1 += -1 + f;

        if (WJR_VERY_UNLIKELY(u1 >= divisor, 0.999)) {
            ++q1;
            u1 -= divisor;
        }

        dst[n - 1] = q1;
        --n;

        if (WJR_UNLIKELY(n == 0)) {
            return u1;
        }

        do {
            u0 = src[n - 1];
            q0 = mul(u1, value, q1);

            q0 += u0;
            q1 += u1 + 1 + (q0 < u0);

            u1 = u0 - mullo(q1, divisor);
            tmp = u1 + divisor;
            f = u1 < q0;
            u1 = f ? u1 : tmp;
            q1 += -1 + f;

            if (WJR_VERY_UNLIKELY(u1 >= divisor, 0.999)) {
                ++q1;
                u1 -= divisor;
            }

            dst[n - 1] = q1;
            --n;
        } while (WJR_LIKELY(n != 0));

        return u1;
    }

    --n;
    uint64_t tmp2 = src[n];
    u1 = tmp2 >> (64 - shift);

    if (WJR_LIKELY(n != 0)) {
        do {
            tmp = src[n - 1];
            u0 = shld(tmp2, tmp, shift);
            tmp2 = tmp;
            q0 = mul(u1, value, q1);

            q0 += u0;
            q1 += u1 + 1 + (q0 < u0);

            u1 = u0 - mullo(q1, divisor);
            tmp = u1 + divisor;
            f = u1 < q0;
            u1 = f ? u1 : tmp;
            q1 += -1 + f;

            if (WJR_VERY_UNLIKELY(u1 >= divisor, 0.999)) {
                ++q1;
                u1 -= divisor;
            }

            dst[n] = q1;

            --n;
        } while (WJR_LIKELY(n != 0));
    }

    u0 = tmp2 << shift;
    q0 = mul(u1, value, q1);

    q0 += u0;
    q1 += u1 + 1 + (q0 < u0);

    u1 = u0 - mullo(q1, divisor);
    tmp = u1 + divisor;
    f = u1 < q0;
    u1 = f ? u1 : tmp;
    q1 += -1 + f;

    if (WJR_VERY_UNLIKELY(u1 >= divisor, 0.999)) {
        ++q1;
        u1 -= divisor;
    }

    dst[0] = q1;

    return u1 >> shift;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
constexpr T divmod_1(T *dst, const T *src, size_t n, div2by1_divider<T> div) {
    if (WJR_UNLIKELY(div.is_power_of_two())) {
        unsigned int c = 63 - div.shift();
        T ret = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return ret;
    }

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    return fallback_divmod_1(dst, src, n, div);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
constexpr T divmod_1(T *dst, const T *src, size_t n, T div) {
    if (WJR_UNLIKELY(is_power_of_two(div))) {
        unsigned int c = ctz(div);
        T ret = src[0] & ((1ull << c) - 1);
        (void)rshift_n(dst, src, n, c);
        return ret;
    }

    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_UNLIKELY(n == 1)) {
        dst[0] = src[0] / div;
        return src[0] % div;
    }

    return fallback_divmod_1(dst, src, n, div2by1_divider<T>(div));
}

} // namespace wjr

#endif // WJR_MATH_DIV_HPP__