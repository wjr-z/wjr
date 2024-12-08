#ifndef WJR_BIGINTEGER_DETAIL_CONVERT_HPP__
#define WJR_BIGINTEGER_DETAIL_CONVERT_HPP__

#include <wjr/format/charconv.hpp>

#include <wjr/biginteger/detail/div.hpp>
#include <wjr/biginteger/detail/precompute-chars-convert.hpp>
#include <wjr/memory/stack_allocator.hpp>

namespace wjr {

inline constexpr size_t dc_bignum_to_chars_threshold = WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;
inline constexpr size_t dc_bignum_to_chars_precompute_threshold = WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;

inline constexpr size_t dc_bignum_from_chars_threshold = WJR_DC_BIGNUM_FROM_CHARS_THRESHOLD;
inline constexpr size_t dc_bignum_from_chars_precompute_threshold =
    WJR_DC_BIGNUM_FROM_CHARS_PRECOMPUTE_THRESHOLD;

inline constexpr auto div2by1_divider_noshift_of_big_base_10 =
    div2by1_divider_noshift<uint64_t>(10'000'000'000'000'000'000ull, 15'581'492'618'384'294'730ull);

inline constexpr auto div3by2_divider_shift_of_big_base_10 = div3by2_divider<uint64_t>(
    1374799102801346560ull, 10842021724855044340ull, 12'938'764'603'223'852'203ull, 1);

template <typename Converter>
size_t __biginteger_to_chars_2_impl(uint8_t *first, const uint64_t *up, size_t n,
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
            __to_chars_unroll_8<2>(first - 8, x & 0xff, conv);
            first -= 8;
            x >>= 8;
        }

        ++up;
        --n;
    } while (n != 0);
    x = *up;

    (void)__unsigned_to_chars_backward_unchecked<2>(first, hbits, x, conv);
    return len;
}

template <typename Converter>
size_t __biginteger_to_chars_8_impl(uint8_t *first, const uint64_t *up, size_t n,
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
            __to_chars_unroll_2<8>(first - 2, last | ((x & 0x03) << 4), conv);
            first -= 2;
            x >>= 2;
            rest = 4;
            break;
        }
        case 4: {
            __to_chars_unroll_2<8>(first - 2, last | ((x & 0x0f) << 2), conv);
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

        __to_chars_unroll_8<8>(first - 8, x & 0xff'ffff, conv);
        x >>= 24;
        __to_chars_unroll_8<8>(first - 16, x & 0xff'ffff, conv);
        x >>= 24;
        __to_chars_unroll_4<8>(first - 20, x & 0x0fff, conv);
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
        __to_chars_unroll_2<8>(first - 2, last | ((x & 0x03) << 4), conv);
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

        __to_chars_unroll_2<8>(first - 2, last | ((x & 0x0f) << 2), conv);
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
            __to_chars_unroll_4<8>(first - 4, x & 0x0fff, conv);
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
        __to_chars_unroll_2<8>(first - 2, x, conv);
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

template <typename Converter>
size_t __biginteger_to_chars_16_impl(uint8_t *first, const uint64_t *up, size_t n,
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

        __to_chars_unroll_8<16>(first - 8, x & 0xffff'ffff, conv);
        __to_chars_unroll_8<16>(first - 16, x >> 32, conv);
        first -= 16;

        ++up;
        --n;
    } while (n);
    x = *up;

    (void)__unsigned_to_chars_backward_unchecked<16>(first, hbits, x, conv);

    return len;
}

template <typename Converter>
WJR_ALL_NONNULL size_t __biginteger_to_chars_power_of_two_impl(uint8_t *first, const uint64_t *up,
                                                               size_t n, unsigned int base,
                                                               Converter conv) noexcept {
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

extern template size_t
__biginteger_to_chars_power_of_two_impl<char_converter_t>(uint8_t *first, const uint64_t *up,
                                                          size_t n, unsigned int base,
                                                          char_converter_t conv) noexcept;

template <typename Converter>
WJR_ALL_NONNULL WJR_RETURNS_NONNULL uint8_t *
basecase_to_chars_10(uint8_t *buf, uint64_t *up, size_t n, Converter conv) noexcept {
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

extern template WJR_ALL_NONNULL WJR_RETURNS_NONNULL uint8_t *
basecase_to_chars_10<char_converter_t>(uint8_t *buf, uint64_t *up, size_t n,
                                       char_converter_t conv) noexcept;

template <typename Converter>
uint8_t *basecase_to_chars(uint8_t *first, size_t len, uint64_t *up, size_t n, unsigned int base,
                           Converter conv) noexcept {
    constexpr size_t buf_len = dc_bignum_to_chars_precompute_threshold * 64 * 7 / 11;
    uint8_t buf[buf_len];
    uint8_t *const end = buf + buf_len;
    uint8_t *start;

    if (WJR_LIKELY(base == 10)) {
        start = basecase_to_chars_10(end, up, n, conv);
    } else {
        start = end;
    }

    if (const size_t rlen = end - start; len > rlen) {
        first = std::fill_n(first, len - rlen, conv.template to<1>(0));
    }

    return std::copy(start, end, first);
}

template <typename Converter>
uint8_t *dc_to_chars(uint8_t *first, size_t len, uint64_t *up, size_t n,
                     precompute_chars_convert_t *pre, uint64_t *stk, Converter conv) noexcept {
    WJR_ASSERT_ASSUME(n >= 1);
    if (n < dc_bignum_to_chars_threshold) {
        return basecase_to_chars(first, len, up, n, pre->base, conv);
    }

    const auto *const pp = pre->ptr;
    const auto pn = pre->n;
    const auto ps = pre->shift;

    WJR_ASSERT((pn + ps) * 5 >= n * 2);

    if (n < pn + ps || (n == pn + ps && reverse_compare_n(up + ps, pp, pn) < 0)) {
        return dc_to_chars(first, len, up, n, pre - 1, stk, conv);
    }

    const auto pd = pre->digits_in_base;
    auto *qp = stk;

    div_qr_s(qp, up + ps, up + ps, n - ps, pp, pn);

    size_t qn = n - pn - ps;
    qn += qp[qn] != 0;

    if (len != 0) {
        len = len - pd;
    }

    pre -= qn * 2 <= n;

    first = dc_to_chars(first, len, qp, qn, pre, stk + qn, conv);
    first = dc_to_chars(first, pd, up, pn + ps, pre, stk, conv);
    return first;
}

template <typename Converter>
WJR_ALL_NONNULL WJR_RETURNS_NONNULL uint8_t *
__biginteger_basecase_to_chars(uint8_t *first, const uint64_t *up, size_t n, unsigned int base,
                               Converter conv) noexcept {
    if (WJR_LIKELY(n < dc_bignum_to_chars_precompute_threshold)) {
        uint64_t upbuf[dc_bignum_to_chars_precompute_threshold];
        std::copy_n(up, n, upbuf);
        return basecase_to_chars(first, 0, upbuf, n, base, conv);
    }

    precompute_chars_convert_t pre[64 - 3];

    unique_stack_allocator stkal;
    auto *stk = static_cast<uint64_t *>(stkal.allocate((n * 18 / 5 + 192) * sizeof(uint64_t)));
    auto *const __up = stk;
    std::copy_n(up, n, __up);
    stk += n;
    auto *const first_pre = precompute_chars_convert(pre, n, base, stk);
    stk += n * 8 / 5 + 128;
    return dc_to_chars(first, 0, __up, n, first_pre, stk, conv);
}

extern template uint8_t *
__biginteger_basecase_to_chars<char_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                 unsigned int base, char_converter_t conv) noexcept;

template <typename Converter>
uint8_t *__fast_biginteger_large_to_chars_impl(uint8_t *first, const uint64_t *up, size_t n,
                                               unsigned int base, Converter conv) noexcept {
    switch (base) {
    case 2: {
        return first + __biginteger_to_chars_2_impl(first, up, n, conv);
    }
    case 8: {
        return first + __biginteger_to_chars_8_impl(first, up, n, conv);
    }
    case 16: {
        return first + __biginteger_to_chars_16_impl(first, up, n, conv);
    }
    case 4:
    case 32: {
        return first + __biginteger_to_chars_power_of_two_impl(first, up, n, base, conv);
    }
    default: {
        break;
    }
    }

    return __biginteger_basecase_to_chars(first, up, n, base, conv);
}

template <typename Iter, typename Converter>
Iter __fallback_biginteger_large_to_chars_impl(Iter ptr, const uint64_t *up, size_t n,
                                               unsigned int base, Converter conv) noexcept {
#define WJR_BIGINTEGER_TO_CHARS_IMPL(BASE, NAME, TAIL, SIZE, CALL)                                 \
    constexpr auto __fast_container_inserter_v =                                                   \
        charconv_detail::is_fast_container_inserter_v<Iter>;                                       \
    if constexpr (__fast_container_inserter_v != 0) {                                              \
        auto &cont = get_inserter_container(ptr);                                                  \
        const auto __presize = cont.size();                                                        \
        if constexpr (__fast_container_inserter_v == 1) {                                          \
            resize(cont, __presize + SIZE);                                                        \
        } else {                                                                                   \
            append(cont, SIZE, default_construct);                                                 \
        }                                                                                          \
        auto *const __ptr = reinterpret_cast<uint8_t *>(wjr::to_address(cont.data())) + __presize; \
        const auto __size = NAME(__ptr, WJR_PP_QUEUE_EXPAND(CALL), conv) TAIL;                     \
        WJR_ASSERT((size_t)__size <= SIZE);                                                        \
        if constexpr (__fast_container_inserter_v == 1) {                                          \
            resize(cont, __presize + __size);                                                      \
        } else {                                                                                   \
            resize(cont, __presize + __size, default_construct);                                   \
        }                                                                                          \
                                                                                                   \
        return ptr;                                                                                \
    } else {                                                                                       \
        unique_stack_allocator stkal;                                                              \
        auto *const __ptr = static_cast<uint8_t *>(stkal.allocate(SIZE * sizeof(uint64_t)));       \
        const auto __size = NAME(__ptr, WJR_PP_QUEUE_EXPAND(CALL), conv) TAIL;                     \
                                                                                                   \
        return wjr::copy_n((charconv_detail::fast_buffer_t<Iter> *)__ptr, __size, ptr);            \
    }

    switch (base) {
    case 2: {
        const size_t capacity = 64 * n;
        WJR_BIGINTEGER_TO_CHARS_IMPL(2, __biginteger_to_chars_2_impl, , capacity, (up, n));
    }
    case 8: {
        const size_t capacity = (64 * n + 2) / 3;
        WJR_BIGINTEGER_TO_CHARS_IMPL(8, __biginteger_to_chars_8_impl, , capacity, (up, n));
    }
    case 16: {
        const size_t capacity = (64 * n + 3) / 4;
        WJR_BIGINTEGER_TO_CHARS_IMPL(16, __biginteger_to_chars_16_impl, , capacity, (up, n));
    }
    case 4:
    case 32: {
        const int bits = base == 4 ? 2 : 5;
        const size_t capacity = (64 * n + bits - 1) / bits;
        WJR_BIGINTEGER_TO_CHARS_IMPL(base, __biginteger_to_chars_power_of_two_impl, , capacity,
                                     (up, n, base));
    }
    default: {
        break;
    }
    }

    // todo : optimize memory usage
    const size_t capacity = ((64 * n) * 4 + 12) / 13;
    WJR_BIGINTEGER_TO_CHARS_IMPL(base, __biginteger_basecase_to_chars, -__ptr, capacity,
                                 (up, n, base));

#undef WJR_BIGINTEGER_TO_CHARS_IMPL
}

template <typename Iter, typename Converter>
Iter __biginteger_to_chars_impl(Iter first, const uint64_t *up, size_t n, unsigned int base,
                                Converter conv) noexcept {
    if (WJR_UNLIKELY(n == 1)) {
        return to_chars_unchecked(first, up[0], base, conv);
    }

    if constexpr (charconv_detail::__is_fast_convert_iterator_v<Iter>) {
        auto *const __first = reinterpret_cast<uint8_t *>(wjr::to_address(first));
        const auto __result = __fast_biginteger_large_to_chars_impl(__first, up, n, base, conv);
        return first + std::distance(__first, __result);
    } else {
        return __fallback_biginteger_large_to_chars_impl(first, up, n, base, conv);
    }
}

/**
 * @brief Convert a biginteger to a string by a given base.
 *
 * @tparam Iter Output iterator type
 * @param[out] first Output iterator
 * @param[in] up Pointer to the biginteger
 * @param[in] n Length of the biginteger
 * @param[in] base Base of the output string. Range: `[2, 36]`,
 * Only support 10 and power of two currently.
 * @return Output iterator after the conversion
 */
template <typename Iter, typename Converter = char_converter_t>
Iter biginteger_to_chars(Iter first, const uint64_t *up, size_t n, unsigned int base = 10,
                         Converter conv = {}) noexcept {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));
    WJR_ASSERT_ASSUME(n != 0);
    WJR_ASSERT_ASSUME(up[n - 1] != 0);

    return __biginteger_to_chars_impl(first, up, n, base, conv);
}

template <typename Converter>
size_t __biginteger_from_chars_2_impl(const uint8_t *first, size_t n, uint64_t *up,
                                      Converter conv) noexcept {
    const size_t hbits = (n - 1) % 64 + 1;
    const size_t len = (n - 1) / 64 + 1;

    uint64_t x = 0;
    up += len;

    __unsigned_from_chars_unchecked<2>(first, first + hbits, x, conv);
    first += hbits;

    *--up = x;

    size_t idx = len - 1;

    if (idx) {
        do {
            x = 0;

            for (int i = 0; i < 4; ++i) {
                x = (x << 16) + __from_chars_unroll_16<2>(first, conv);
                first += 16;
            }

            *--up = x;
        } while (WJR_LIKELY(--idx));
    }

    return len;
}

template <typename Converter>
size_t __biginteger_from_chars_8_impl(const uint8_t *first, size_t n, uint64_t *up,
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
        __unsigned_from_chars_unchecked<8>(first, first + hbits, x, conv);
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

template <typename Converter>
size_t __biginteger_from_chars_16_impl(const uint8_t *first, size_t n, uint64_t *up,
                                       Converter conv) noexcept {
    const size_t hbits = (n - 1) % 16 + 1;
    const size_t len = (n - 1) / 16 + 1;

    uint64_t x = 0;
    up += len;

    __unsigned_from_chars_unchecked<16>(first, first + hbits, x, conv);
    first += hbits;

    *--up = x;

    size_t idx = len - 1;

    if (idx) {
        do {
            x = 0;

            for (int i = 0; i < 4; ++i) {
                auto x0 = conv.template from<16>(first[0]);
                auto x1 = conv.template from<16>(first[1]);
                auto x2 = conv.template from<16>(first[2]);
                auto x3 = conv.template from<16>(first[3]);

                x = x << 16 | x0 << 12 | x1 << 8 | x2 << 4 | x3;
                first += 4;
            }

            *--up = x;
        } while (WJR_LIKELY(--idx));
    }

    return len;
}

template <typename Converter>
WJR_ALL_NONNULL size_t basecase_from_chars_10(const uint8_t *first, size_t n, uint64_t *up,
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

extern template WJR_ALL_NONNULL size_t basecase_from_chars_10<char_converter_t>(
    const uint8_t *first, size_t n, uint64_t *up, char_converter_t conv) noexcept;

template <typename Converter>
size_t basecase_from_chars(const uint8_t *first, size_t n, uint64_t *up, unsigned int base,
                           Converter conv) noexcept {
    if (base == 10) {
        return basecase_from_chars_10(first, n, up, conv);
    } else {
        WJR_UNREACHABLE();
    }
}

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

    const size_t ps = pre->shift;
    const size_t pn = pre->n;

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

template <typename Converter>
uint64_t *__basecase_basecase_from_chars(const uint8_t *first, size_t n, uint64_t *up,
                                         unsigned int base, Converter conv) noexcept {
    if (WJR_LIKELY(n < dc_bignum_from_chars_precompute_threshold)) {
        return up + basecase_from_chars(first, n, up, base, conv);
    }

    const auto per_digits = precompute_chars_convert_16n_ptr[base]->digits_in_one_base;

    precompute_chars_convert_t pre[64 - 3];

    unique_stack_allocator stkal;
    const size_t un = n / per_digits + 1;
    auto *stk = static_cast<uint64_t *>(stkal.allocate((un * 16 / 5 + 192) * sizeof(uint64_t)));
    auto *const first_pre = precompute_chars_convert(pre, un, base, stk);
    stk += un * 8 / 5 + 128;
    return up + dc_from_chars(first, n, up, first_pre, stk, conv);
}

template <typename Converter>
WJR_ALL_NONNULL WJR_RETURNS_NONNULL uint64_t *
__biginteger_from_chars_impl(const uint8_t *first, const uint8_t *last, uint64_t *up,
                             unsigned int base, Converter conv) noexcept {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));

    const size_t n = std::distance(first, last);

    if (is_zero_or_single_bit(base)) {
        switch (base) {
        case 2: {
            return up + __biginteger_from_chars_2_impl(first, n, up, conv);
        }
        case 8: {
            return up + __biginteger_from_chars_8_impl(first, n, up, conv);
        }
        case 16: {
            return up + __biginteger_from_chars_16_impl(first, n, up, conv);
        }
        default: {
            WJR_UNREACHABLE();
        }
        }
    }

    return __basecase_basecase_from_chars(first, n, up, base, conv);
}

extern template uint64_t *
__basecase_basecase_from_chars<char_converter_t>(const uint8_t *first, size_t n, uint64_t *up,
                                                 unsigned int base, char_converter_t) noexcept;

/**
 * @brief Convert a string to a biginteger by a given base.
 *
 * @tparam Iter Input iterator type
 * @param[in] first Input iterator
 * @param[in] last Input iterator
 * @param[out] up Pointer to the biginteger
 * @param[in] base Base of the input string. Range: `[2, 36]`,
 * Only support 10 and power of two currently.
 * @return Pointer after the conversion
 */
template <typename Iter, typename Converter = char_converter_t,
          WJR_REQUIRES(charconv_detail::__is_fast_convert_iterator_v<Iter>)>
uint64_t *biginteger_from_chars(Iter first, Iter last, uint64_t *up, unsigned int base = 10,
                                Converter conv = {}) noexcept {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));

    const auto *const __first = reinterpret_cast<const uint8_t *>(wjr::to_address(first));
    const auto *const __last = reinterpret_cast<const uint8_t *>(wjr::to_address(last));

    return __biginteger_from_chars_impl(__first, __last, up, base, conv);
}

} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_CONVERT_HPP__