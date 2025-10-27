#include <wjr/biginteger/detail/convert.hpp>
#include <wjr/biginteger/detail/mul.hpp>

namespace wjr {

namespace {
inline constexpr size_t bignum_to_chars_div_qr_2_threshold = 6;
}

template <typename Converter>
size_t _biginteger_to_chars_2_impl(uint8_t *first, const uint64_t *up, size_t n,
                                   Converter conv) noexcept {
    WJR_ASSERT_L2(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    const int pc = clz(x);
    const int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    const size_t len = hbits + 64 * n;
    first += len;

    do {
        x = *up;

        for (int i = 0; i < 8; ++i) {
            _to_chars_unroll_8<2>(first - 8, x & 0xff, conv);
            first -= 8;
            x >>= 8;
        }

        ++up;
        --n;
    } while (n != 0);
    x = *up;

    (void)_unsigned_to_chars_backward_unchecked<2>(first, hbits, x, conv);
    return len;
}

template size_t _biginteger_to_chars_2_impl<char_converter_t>(uint8_t *first, const uint64_t *up,
                                                              size_t n,
                                                              char_converter_t conv) noexcept;

template size_t _biginteger_to_chars_2_impl<origin_converter_t>(uint8_t *first, const uint64_t *up,
                                                                size_t n,
                                                                origin_converter_t conv) noexcept;

template <typename Converter>
size_t _biginteger_to_chars_8_impl(uint8_t *first, const uint64_t *up, size_t n,
                                   Converter conv) noexcept {
    WJR_ASSERT_L2(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    const int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);

    const size_t len = (hbits + 64 * n + 2) / 3;
    first += len;

    int rest = 0;
    unsigned int last = 0;

    do {
        x = *up;

        switch (rest) {
        case 0: {
            rest = 2;
            break;
        }
        case 2: {
            _to_chars_unroll_2<8>(first - 2, last | ((x & 0x03) << 4), conv);
            first -= 2;
            x >>= 2;
            rest = 4;
            break;
        }
        case 4: {
            _to_chars_unroll_2<8>(first - 2, last | ((x & 0x0f) << 2), conv);
            first -= 2;
            x >>= 4;
            rest = 0;
            break;
        }
        default: {
            WJR_UNREACHABLE();
            break;
        }
        }

        _to_chars_unroll_8<8>(first - 8, x & 0xff'ffff, conv);
        x >>= 24;
        _to_chars_unroll_8<8>(first - 16, x & 0xff'ffff, conv);
        x >>= 24;
        _to_chars_unroll_4<8>(first - 20, x & 0x0fff, conv);
        x >>= 12;
        first -= 20;

        last = x;

        ++up;
        --n;
    } while (n);
    x = *up;

    switch (rest) {
    case 0: {
        break;
    }
    case 2: {
        _to_chars_unroll_2<8>(first - 2, last | ((x & 0x03) << 4), conv);
        first -= 2;
        if (hbits <= 2) {
            goto DONE;
        }
        hbits -= 2;
        x >>= 2;
        break;
    }
    case 4: {
        if (WJR_UNLIKELY(hbits == 1)) {
            *--first = conv.template to<8>(x << 2 | last);
            goto DONE;
        }

        _to_chars_unroll_2<8>(first - 2, last | ((x & 0x0f) << 2), conv);
        first -= 2;
        if (hbits <= 4) {
            goto DONE;
        }
        hbits -= 4;
        x >>= 4;
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    if (WJR_LIKELY(hbits + 2 >= 12)) {
        do {
            _to_chars_unroll_4<8>(first - 4, x & 0x0fff, conv);
            first -= 4;
            x >>= 12;
            hbits -= 12;
        } while (WJR_LIKELY(hbits + 2 >= 12));
    }

    switch ((hbits + 2) / 3) {
    case 3: {
        *--first = conv.template to<8>(x & 0x07);
        x >>= 3;
        WJR_FALLTHROUGH;
    }
    case 2: {
        _to_chars_unroll_2<8>(first - 2, x, conv);
        break;
    }
    case 1: {
        *--first = conv.template to<8>(x);
        WJR_FALLTHROUGH;
    }
    case 0: {
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

DONE:
    return len;
}

template size_t _biginteger_to_chars_8_impl<char_converter_t>(uint8_t *first, const uint64_t *up,
                                                              size_t n,
                                                              char_converter_t conv) noexcept;

template size_t _biginteger_to_chars_8_impl<origin_converter_t>(uint8_t *first, const uint64_t *up,
                                                                size_t n,
                                                                origin_converter_t conv) noexcept;

template <typename Converter>
size_t _biginteger_to_chars_16_impl(uint8_t *first, const uint64_t *up, size_t n,
                                    Converter conv) noexcept {
    WJR_ASSERT_L2(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 2);

    uint64_t x = up[n - 1];
    --n;
    const int pc = clz(x);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    hbits = (hbits + 3) / 4;

    const size_t len = hbits + 16 * n;
    first += len;

    do {
        x = *up;

        _to_chars_unroll_8<16>(first - 8, x & 0xffff'ffff, conv);
        _to_chars_unroll_8<16>(first - 16, x >> 32, conv);
        first -= 16;

        ++up;
        --n;
    } while (n);
    x = *up;

    (void)_unsigned_to_chars_backward_unchecked<16>(first, hbits, x, conv);

    return len;
}

template size_t _biginteger_to_chars_16_impl<char_converter_t>(uint8_t *first, const uint64_t *up,
                                                               size_t n,
                                                               char_converter_t conv) noexcept;

template size_t _biginteger_to_chars_16_impl<origin_converter_t>(uint8_t *first, const uint64_t *up,
                                                                 size_t n,
                                                                 origin_converter_t conv) noexcept;

template <typename Converter>
size_t _biginteger_to_chars_power_of_two_impl(uint8_t *first, const uint64_t *up, size_t n,
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
_biginteger_to_chars_power_of_two_impl<char_converter_t>(uint8_t *first, const uint64_t *up,
                                                         size_t n, unsigned int base,
                                                         char_converter_t conv) noexcept;
template size_t
_biginteger_to_chars_power_of_two_impl<origin_converter_t>(uint8_t *first, const uint64_t *up,
                                                           size_t n, unsigned int base,
                                                           origin_converter_t conv) noexcept;

template <typename Converter>
uint8_t *basecase_to_chars_10(uint8_t *buf, uint64_t *up, size_t n, Converter conv) noexcept {
    while (n >= bignum_to_chars_div_qr_2_threshold) {
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

        _to_chars_unroll_16<10>(buf - 16, lo % 1'0000'0000'0000'0000, conv);
        lo /= 1'0000'0000'0000'0000;
        _to_chars_unroll_2<10>(buf - 18, lo % 100, conv);
        lo /= 100;
        buf[-19] = conv.template to<10>(lo);
        buf -= 19;

        _to_chars_unroll_16<10>(buf - 16, hi % 1'0000'0000'0000'0000, conv);
        hi /= 1'0000'0000'0000'0000;
        _to_chars_unroll_2<10>(buf - 18, hi % 100, conv);
        hi /= 100;
        buf[-19] = conv.template to<10>(hi);
        buf -= 19;
    }

    while (n != 1) {
        uint64_t q, rem;

        q = div_qr_1_noshift(up, rem, up, n, div2by1_divider_noshift_of_big_base_10);
        n -= q == 0;
        if (q != 0) {
            up[n - 1] = q;
        }

        _to_chars_unroll_16<10>(buf - 16, rem % 1'0000'0000'0000'0000, conv);
        rem /= 1'0000'0000'0000'0000;
        _to_chars_unroll_2<10>(buf - 18, rem % 100, conv);
        rem /= 100;
        buf[-19] = conv.template to<10>(rem);

        buf -= 19;
    }

    return _unsigned_to_chars_backward_unchecked<10>(buf, up[0], conv);
}

template uint8_t *basecase_to_chars_10<char_converter_t>(uint8_t *buf, uint64_t *up, size_t n,
                                                         char_converter_t conv) noexcept;
template uint8_t *basecase_to_chars_10<origin_converter_t>(uint8_t *buf, uint64_t *up, size_t n,
                                                           origin_converter_t conv) noexcept;

template <typename Converter>
uint8_t *_biginteger_basecase_to_chars(uint8_t *first, const uint64_t *up, size_t n,
                                       unsigned int base, Converter conv) noexcept {
    if (WJR_LIKELY(n < dc_bignum_to_chars_precompute_threshold)) {
        uint64_t upbuf[dc_bignum_to_chars_precompute_threshold];
        std::copy_n(up, n, upbuf);
        return basecase_to_chars(first, 0, upbuf, n, base, conv);
    }

    precompute_chars_convert_t pre[64 - 3];

    unique_stack_allocator stkal;
    auto *stk = stkal.template allocate<uint64_t>(n * 18 / 5 + 192);
    auto *const _up = stk;
    std::copy_n(up, n, _up);
    stk += n;
    auto *const first_pre = precompute_chars_convert(pre, n, base, stk);
    stk += n * 8 / 5 + 128;
    return dc_to_chars(first, 0, _up, n, first_pre, stk, conv);
}

template uint8_t *_biginteger_basecase_to_chars<char_converter_t>(uint8_t *first,
                                                                  const uint64_t *up, size_t n,
                                                                  unsigned int base,
                                                                  char_converter_t conv) noexcept;
template uint8_t *
_biginteger_basecase_to_chars<origin_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                  unsigned int base,
                                                  origin_converter_t conv) noexcept;

template <typename Converter>
size_t _biginteger_from_chars_8_impl(const uint8_t *first, size_t n, uint64_t *up,
                                     Converter conv) noexcept {
    size_t len = (n * 3 + 63) / 64;
    const size_t lbits = (64 * (len - 1)) / 3;
    size_t rest = (64 * (len - 1)) % 3;
    const size_t hbits = n - lbits - 1;

    uint64_t x = 0;
    up += len;
    size_t idx = len - 1;

    if (WJR_UNLIKELY(hbits == 0)) {
    } else {
        _unsigned_from_chars_unchecked<8>(first, first + hbits, x, conv);
        first += hbits;
    }

    uint64_t nx = conv.template from<8>(*first++);
    switch (rest) {
    case 0: {
        *--up = x << 3 | nx;
        x = 0;
        break;
    }
    case 1: {
        x = x << 2 | nx >> 1;
        if (WJR_UNLIKELY(x == 0)) {
            --len;
        }

        *--up = x;
        x = nx & 1;
        break;
    }
    case 2: {
        x = x << 1 | nx >> 2;
        if (WJR_UNLIKELY(x == 0)) {
            --len;
        }
        *--up = x;
        x = nx & 3;
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    if (idx) {
        do {
            for (int i = 0; i < 5; ++i) {
                auto x0 = conv.template from<8>(first[0]);
                auto x1 = conv.template from<8>(first[1]);
                auto x2 = conv.template from<8>(first[2]);
                auto x3 = conv.template from<8>(first[3]);

                x = x << 12 | x0 << 9 | x1 << 6 | x2 << 3 | x3;
                first += 4;
            }

            switch (rest) {
            case 0: {
                x = x << 3 | conv.template from<8>(*first++);
                nx = conv.template from<8>(*first++);
                x = x << 1 | nx >> 2;
                *--up = x;
                x = nx & 3;
                rest = 2;
                break;
            }
            case 1: {
                x = x << 3 | conv.template from<8>(*first++);
                *--up = x;
                x = 0;
                rest = 0;
                break;
            }
            case 2: {
                nx = conv.template from<8>(*first++);
                x = x << 2 | nx >> 1;
                *--up = x;
                x = nx & 1;
                rest = 1;
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }

        } while (WJR_LIKELY(--idx));
    }

    return len;
}

template size_t _biginteger_from_chars_8_impl<char_converter_t>(const uint8_t *first, size_t n,
                                                                uint64_t *up,
                                                                char_converter_t conv) noexcept;

template size_t _biginteger_from_chars_8_impl<origin_converter_t>(const uint8_t *first, size_t n,
                                                                  uint64_t *up,
                                                                  origin_converter_t conv) noexcept;

template <typename Converter>
size_t dc_from_chars(const uint8_t *first, size_t n, uint64_t *up, precompute_chars_convert_t *pre,
                     uint64_t *stk, Converter conv) noexcept {
    const size_t lo = pre->digits_in_base;
    if (n <= lo) {
        if (n < dc_bignum_from_chars_threshold) {
            return basecase_from_chars(first, n, up, pre->base, conv);
        } else {
            return dc_from_chars(first, n, up, pre - 1, stk, conv);
        }
    }

    const size_t hi = n - lo;
    size_t hn, ln;

    if (hi < dc_bignum_from_chars_threshold) {
        hn = basecase_from_chars(first, hi, stk, pre->base, conv);
    } else {
        hn = dc_from_chars(first, hi, stk, pre - (lo * 2 >= n), up, conv);
    }

    const size_t pn = pre->size;
    const size_t ps = pre->shift;

    if (WJR_LIKELY(hn != 0)) {
        if (pn >= hn) {
            mul_s(up + ps, pre->ptr, pn, stk, hn);
        } else {
            mul_s(up + ps, stk, hn, pre->ptr, pn);
        }
        set_n(up, 0, ps);
    }

    std::advance(first, hi);
    if (lo < dc_bignum_from_chars_threshold) {
        ln = basecase_from_chars(first, lo, stk, pre->base, conv);
    } else {
        ln = dc_from_chars(first, lo, stk, pre - (lo * 2 >= n), stk + pn + ps + 1, conv);
    }

    WJR_ASSERT(ps + pn + 1 >= ln);

    if (WJR_LIKELY(hn != 0)) {
        if (WJR_LIKELY(ln != 0)) {
            auto cf = addc_s(up, up, ps + pn + hn, stk, ln);
            WJR_ASSERT(cf == 0);
            (void)(cf);
        }

        n = ps + pn + hn;
        return n - (up[n - 1] == 0);
    }

    if (WJR_LIKELY(ln != 0)) {
        std::copy_n(stk, ln, up);
    }

    return ln;
}

template size_t dc_from_chars<char_converter_t>(const uint8_t *first, size_t n, uint64_t *up,
                                                precompute_chars_convert_t *pre, uint64_t *stk,
                                                char_converter_t conv) noexcept;

template size_t dc_from_chars<origin_converter_t>(const uint8_t *first, size_t n, uint64_t *up,
                                                  precompute_chars_convert_t *pre, uint64_t *stk,
                                                  origin_converter_t conv) noexcept;

template <typename Converter>
size_t basecase_from_chars_10(const uint8_t *first, size_t n, uint64_t *up,
                              Converter conv) noexcept {
    uint64_t x = 0;

    if (n <= 19) {
        _unsigned_from_chars_unchecked<10>(first, first + n, x, conv);
        up[0] = x;
        return up[0] != 0;
    }

    size_t m = (n - 1) % 19 + 1;

    _unsigned_from_chars_unchecked<10>(first, first + m, x, conv);
    up[0] = x;

    first += m;
    n -= m;

    m = up[0] != 0;

    do {
        x = 0;

        x = _from_chars_unroll_16<10>(first, conv);
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

template uint64_t *_basecase_basecase_from_chars<char_converter_t>(const uint8_t *first, size_t n,
                                                                   uint64_t *up, unsigned int base,
                                                                   char_converter_t) noexcept;
} // namespace wjr