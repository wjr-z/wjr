#include <wjr/biginteger/detail/convert.hpp>

namespace wjr {

template <typename Converter>
size_t __biginteger_to_chars_power_of_two_impl(uint8_t *first, const uint64_t *up, size_t n,
                                               unsigned int base, Converter conv) noexcept {
    WJR_ASSERT_L2(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    const int bits = ctz(base);
    const unsigned int mask = (1u << bits) - 1;

    uint64_t x = up[n - 1];
    --n;
    const int pc = clz(x);
    const int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    const size_t len = (hbits + 64 * n + bits - 1) / bits;
    first += len;

    int rest = 0;
    unsigned int last = 0;

    do {
        x = *up;

        if (rest) {
            int fix = bits - rest;
            unsigned int val = ((x & ((1u << fix) - 1)) << rest) | last;
            x >>= fix;
            rest = 64 - fix;
            *--first = conv.to(val);
        } else {
            rest = 64;
        }

        do {
            *--first = conv.to(x & mask);
            x >>= bits;
            rest -= bits;
        } while (rest >= bits);

        last = x;

        ++up;
        --n;
    } while (n);
    x = *up;

    WJR_ASSERT_ASSUME(rest < bits);

    if (WJR_UNLIKELY(rest != 0)) {
        int fix = bits - rest;
        unsigned int val = ((x & ((1u << fix) - 1)) << rest) | last;
        x >>= fix;
        *--first = conv.to(val);
        rest = hbits - fix;
        if (WJR_UNLIKELY(rest == 0)) {
            goto DONE;
        }
    } else {
        rest = hbits;
    }

    do {
        *--first = conv.to(x & mask);
        x >>= bits;
        rest -= bits;
    } while (WJR_LIKELY(rest > 0));

DONE:
    return len;
}

template size_t
__biginteger_to_chars_power_of_two_impl<char_converter_t>(uint8_t *first, const uint64_t *up,
                                                          size_t n, unsigned int base,
                                                          char_converter_t conv) noexcept;
template size_t
__biginteger_to_chars_power_of_two_impl<origin_converter_t>(uint8_t *first, const uint64_t *up,
                                                            size_t n, unsigned int base,
                                                            origin_converter_t conv) noexcept;

template <typename Converter>
uint8_t *basecase_to_chars_10(uint8_t *buf, uint64_t *up, size_t n, Converter conv) noexcept {
    constexpr size_t div_qr_2_threshold = 6;
    if (n >= div_qr_2_threshold) {
        do {
            uint64_t q;
            uint64_t rem[2];
            q = div_qr_2_shift(up, rem, up, n, div3by2_divider_shift_of_big_base_10);
            if (q != 0) {
                up[n - 2] = q;
                n -= 1;
            } else {
                n -= 2;
            }

            uint64_t lo, hi;
            hi = div128by64to64_noshift(lo, rem[0], rem[1], div2by1_divider_noshift_of_big_base_10);

            __to_chars_unroll_16<10>(buf - 16, lo % 1'0000'0000'0000'0000, conv);
            lo /= 1'0000'0000'0000'0000;
            __to_chars_unroll_2<10>(buf - 18, lo % 100, conv);
            lo /= 100;
            buf[-19] = conv.template to<10>(lo);
            buf -= 19;

            __to_chars_unroll_16<10>(buf - 16, hi % 1'0000'0000'0000'0000, conv);
            hi /= 1'0000'0000'0000'0000;
            __to_chars_unroll_2<10>(buf - 18, hi % 100, conv);
            hi /= 100;
            buf[-19] = conv.template to<10>(hi);
            buf -= 19;
        } while (n >= div_qr_2_threshold);
    }

    do {
        if (WJR_UNLIKELY(n == 1)) {
            return __unsigned_to_chars_backward_unchecked<10>(buf, up[0], conv);
        }

        uint64_t q, rem;

        q = div_qr_1_noshift(up, rem, up, n, div2by1_divider_noshift_of_big_base_10);
        n -= q == 0;
        if (q != 0) {
            up[n - 1] = q;
        }

        __to_chars_unroll_16<10>(buf - 16, rem % 1'0000'0000'0000'0000, conv);
        rem /= 1'0000'0000'0000'0000;
        __to_chars_unroll_2<10>(buf - 18, rem % 100, conv);
        rem /= 100;
        buf[-19] = conv.template to<10>(rem);

        buf -= 19;
    } while (n);

    WJR_UNREACHABLE();
}

template uint8_t *basecase_to_chars_10<char_converter_t>(uint8_t *buf, uint64_t *up, size_t n,
                                                         char_converter_t conv) noexcept;
template uint8_t *basecase_to_chars_10<origin_converter_t>(uint8_t *buf, uint64_t *up, size_t n,
                                                           origin_converter_t conv) noexcept;

template <typename Converter>
uint8_t *__biginteger_basecase_to_chars(uint8_t *first, const uint64_t *up, size_t n,
                                        unsigned int base, Converter conv) noexcept {
    if (WJR_LIKELY(n < dc_bignum_to_chars_precompute_threshold)) {
        uint64_t upbuf[dc_bignum_to_chars_precompute_threshold];
        std::copy_n(up, n, upbuf);
        return basecase_to_chars(first, 0, upbuf, n, base, conv);
    }

    precompute_chars_convert_t pre[64 - 3];

    unique_stack_allocator stkal;
    auto *stk = stkal.template allocate<uint64_t>(n * 18 / 5 + 192);
    auto *const __up = stk;
    std::copy_n(up, n, __up);
    stk += n;
    auto *const first_pre = precompute_chars_convert(pre, n, base, stk);
    stk += n * 8 / 5 + 128;
    return dc_to_chars(first, 0, __up, n, first_pre, stk, conv);
}

template uint8_t *__biginteger_basecase_to_chars<char_converter_t>(uint8_t *first,
                                                                   const uint64_t *up, size_t n,
                                                                   unsigned int base,
                                                                   char_converter_t conv) noexcept;
template uint8_t *
__biginteger_basecase_to_chars<origin_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                   unsigned int base,
                                                   origin_converter_t conv) noexcept;

template <typename Converter>
size_t basecase_from_chars_10(const uint8_t *first, size_t n, uint64_t *up,
                              Converter conv) noexcept {
    uint64_t x = 0;

    if (n <= 19) {
        __unsigned_from_chars_unchecked<10>(first, first + n, x, conv);
        up[0] = x;
        return up[0] != 0;
    }

    size_t m = (n - 1) % 19 + 1;

    __unsigned_from_chars_unchecked<10>(first, first + m, x, conv);
    up[0] = x;

    first += m;
    n -= m;

    m = up[0] != 0;

    do {
        x = 0;

        x = __from_chars_unroll_16<10>(first, conv);
        first += 16;

        x = x * 10 + conv.template from<10>(*first++);
        x = x * 10 + conv.template from<10>(*first++);
        x = x * 10 + conv.template from<10>(*first++);

        uint64_t cf;

        if (WJR_LIKELY(m == 0)) {
            cf = x;
        } else {
            cf = mul_1(up, up, m, div2by1_divider_noshift_of_big_base_10.get_divisor());
            cf += addc_1(up, up, m, x);
        }

        if (WJR_LIKELY(cf != 0)) {
            up[m++] = cf;
        }

        n -= 19;
    } while (WJR_LIKELY(n != 0));

    return m;
}

template WJR_ALL_NONNULL size_t basecase_from_chars_10<char_converter_t>(
    const uint8_t *first, size_t n, uint64_t *up, char_converter_t conv) noexcept;
template WJR_ALL_NONNULL size_t basecase_from_chars_10<origin_converter_t>(
    const uint8_t *first, size_t n, uint64_t *up, origin_converter_t conv) noexcept;

template uint64_t *__basecase_basecase_from_chars<char_converter_t>(const uint8_t *first, size_t n,
                                                                    uint64_t *up, unsigned int base,
                                                                    char_converter_t) noexcept;
} // namespace wjr