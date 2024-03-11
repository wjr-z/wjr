#ifndef WJR_MATH_CONVERT_HPP__
#define WJR_MATH_CONVERT_HPP__

#include <array>

#include <wjr/assert.hpp>
#include <wjr/math/clz.hpp>
#include <wjr/math/precompute-convert.hpp>

namespace wjr {

inline constexpr size_t dc_bignum_to_chars_threshold = WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;
inline constexpr size_t dc_bignum_to_chars_precompute_threshold =
    WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;

inline constexpr size_t dc_bignum_from_chars_threshold =
    WJR_DC_BIGNUM_FROM_CHARS_THRESHOLD;
inline constexpr size_t dc_bignum_from_chars_precompute_threshold =
    WJR_DC_BIGNUM_FROM_CHARS_PRECOMPUTE_THRESHOLD;

inline constexpr auto div2by1_divider_of_big_base_10 = div2by1_divider<uint64_t>(
    10'000'000'000'000'000'000ull, 15'581'492'618'384'294'730ull, 0);

struct char_converter_t {
private:
    static constexpr std::array<char, 36> to_table = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    static constexpr std::array<uint8_t, 256> from_table = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,
        0,  0,  0,  0,  0,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0,  0,  0,  0,  0,  0,  10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
        33, 34, 35, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};

public:
    WJR_CONST constexpr static char to(uint8_t x) { return to_table[x]; }
    WJR_CONST constexpr static uint8_t from(char x) { return from_table[x]; }
};

inline constexpr char_converter_t char_converter = {};

struct origin_converter_t {
    WJR_CONST constexpr static char to(uint8_t x) { return x; }
    WJR_CONST constexpr static uint8_t from(char x) { return x; }
};

inline constexpr origin_converter_t origin_converter = {};

// require operator() of Converter is constexpr
template <typename Converter, int Base, int Unroll>
class __char_converter_table_t {
    static constexpr int pw2 = Unroll == 2 ? Base * Base : Base * Base * Base * Base;

public:
    constexpr __char_converter_table_t() : table() {
        constexpr auto conv = Converter{};
        for (int i = 0, j = 0; i < pw2; ++i, j += Unroll) {
            int x = i;
            for (int k = Unroll - 1; ~k; --k) {
                table[j + k] = conv.to(x % Base);
                x /= Base;
            }
        }
    }

    WJR_CONST constexpr char operator[](unsigned int idx) const { return table[idx]; }

private:
    std::array<char, pw2 * Unroll> table;
};

template <typename Converter, int Base, int Unroll>
inline constexpr __char_converter_table_t<Converter, Base, Unroll>
    __char_converter_table = {};

template <typename Iter, typename Converter>
Iter to_chars_2(Iter first, uint64_t *up, size_t n, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 1);

    constexpr auto &table = __char_converter_table<Converter, 2, 4>;
    constexpr auto unroll = [](auto &first, unsigned int v) {
        first[-1] = table[v * 4 + 3];
        first[-2] = table[v * 4 + 2];
        first[-3] = table[v * 4 + 1];
        first[-4] = table[v * 4];
        first -= 4;
    };

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = hbits + 64 * n;
    first += len;

    if (n) {
        do {
            x = *up;
            for (int i = 0; i < 16; ++i) {
                unroll(first, x & 0x0f);
                x >>= 4;
            }
            ++up;
            --n;
        } while (n);
        x = *up;
    }

    if (WJR_LIKELY(hbits >= 4)) {
        do {
            unroll(first, x & 0x0f);
            x >>= 4;
            hbits -= 4;
        } while (WJR_LIKELY(hbits >= 4));
    }

    switch (hbits) {
    case 3: {
        *--first = conv.to(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 2: {
        *--first = conv.to(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 1: {
        *--first = conv.to(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 0: {
        *--first = conv.to(x);
        break;
    }
    default: {
        WJR_UNREACHABLE();
        break;
    }
    }

    return first + len;
}

template <typename Iter, typename Converter>
Iter to_chars_8(Iter first, uint64_t *up, size_t n, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 1);

    constexpr auto &table = __char_converter_table<Converter, 8, 2>;
    constexpr auto unroll = [](auto &first, unsigned int v) {
        first[-1] = table[v * 2 + 1];
        first[-2] = table[v * 2];
        first -= 2;
    };

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = (hbits + 64 * n + 2) / 3;
    first += len;

    int rest = 0;
    unsigned int last = 0;

    if (n) {

        do {
            x = *up;

            switch (rest) {
            case 0: {
                rest = 2;
                break;
            }
            case 2: {
                unroll(first, last | ((x & 0x03) << 4));
                x >>= 2;
                rest = 4;
                break;
            }
            case 4: {
                unroll(first, last | ((x & 0x0f) << 2));
                x >>= 4;
                rest = 0;
                break;
            }
            default: {
                WJR_UNREACHABLE();
                break;
            }
            }

            for (int i = 0; i < 10; ++i) {
                unroll(first, x & 0x3f);
                x >>= 6;
            }

            last = x;

            ++up;
            --n;
        } while (n);
        x = *up;
    }

    switch (rest) {
    case 0: {
        break;
    }
    case 2: {
        unroll(first, last | ((x & 0x03) << 4));
        if (hbits <= 2) {
            goto DONE;
        }
        hbits -= 2;
        x >>= 2;
        break;
    }
    case 4: {
        if (WJR_UNLIKELY(hbits == 1)) {
            *--first = conv.to(x << 2 | last);
            goto DONE;
        }

        unroll(first, last | ((x & 0x0f) << 2));
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

    if (WJR_LIKELY(hbits >= 6)) {
        do {
            unroll(first, x & 0x3f);
            x >>= 6;
            hbits -= 6;
        } while (WJR_LIKELY(hbits >= 6));
    }

    if (hbits >= 3) {
        unroll(first, x);
    } else {
        *--first = conv.to(x);
    }

DONE:
    return first + len;
}

template <typename Iter, typename Converter>
Iter to_chars_16(Iter first, uint64_t *up, size_t n, Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 1);

    constexpr auto &table = __char_converter_table<Converter, 16, 2>;
    constexpr auto unroll = [](auto &first, unsigned int v) {
        first[-1] = table[v * 2 + 1];
        first[-2] = table[v * 2];
        first -= 2;
    };

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = (hbits + 3) / 4 + 16 * n;
    first += len;

    if (n) {

        do {
            x = *up;

            for (int i = 0; i < 8; ++i) {
                unroll(first, x & 0xff);
                x >>= 8;
            }

            ++up;
            --n;
        } while (n);
        x = *up;
    }

    if (WJR_LIKELY(hbits >= 8)) {
        do {
            unroll(first, x & 0xff);
            x >>= 8;
            hbits -= 8;
        } while (WJR_LIKELY(hbits >= 8));
    }

    if (hbits >= 4) {
        unroll(first, x);
    } else {
        *--first = conv.to(x);
    }

DONE:
    return first + len;
}

template <typename Iter, typename Converter>
Iter to_chars_power_of_two(Iter first, unsigned int Base, uint64_t *up, size_t n,
                           Converter conv) {
    WJR_ASSERT(up[n - 1] != 0);
    WJR_ASSERT_ASSUME(n >= 1);

    int per_bit = ctz(Base);
    unsigned int mask = (1u << per_bit) - 1;

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = (hbits + 64 * n + per_bit - 1) / per_bit;
    first += len;

    int rest = 0;
    unsigned int last = 0;

    if (n) {
        do {
            x = *up;

            if (rest) {
                int fix = per_bit - rest;
                unsigned int val = (x & ((1u << fix) - 1)) | last;
                x >>= fix;
                rest += 64 - fix;
                *--first = conv.to(val);
            } else {
                rest = 64;
            }

            do {
                *--first = conv.to(x & mask);
                x >>= per_bit;
                rest -= per_bit;
            } while (rest >= per_bit);

            last = x;

            ++up;
            --n;
        } while (n);
        x = *up;
    }

    WJR_ASSERT_ASSUME(rest < per_bit);

    if (WJR_UNLIKELY(rest != 0)) {
        int fix = per_bit - rest;
        unsigned int val = (x & ((1u << fix) - 1)) | last;
        x >>= fix;
        *--first = conv.to(val);
        rest += hbits - fix;
        if (WJR_UNLIKELY(rest == 0)) {
            goto DONE;
        }
    } else {
        rest = hbits;
    }

    do {
        *--first = conv.to(x & mask);
        x >>= per_bit;
        rest -= per_bit;
    } while (WJR_LIKELY(rest > 0));

DONE:
    return first + len;
}

template <typename Converter>
char *__backward_to_chars_10(char *buf, uint64_t val, Converter conv) {
    constexpr auto &table = __char_converter_table<Converter, 10, 2>;
    constexpr auto unroll = [](auto &first, unsigned int v) {
        first[-1] = table[v * 2 + 1];
        first[-2] = table[v * 2];
        first -= 2;
    };

    if (WJR_LIKELY(val >= 100)) {
        do {
            unroll(buf, val % 100);
            val /= 100;
        } while (WJR_LIKELY(val >= 100));
    }

    if (val >= 10) {
        unroll(buf, val);
    } else {
        *--buf = conv.to(val);
    }

    return buf;
}

template <typename Converter>
char *basecase_to_chars_10(char *buf, uint64_t *up, size_t n, Converter conv) {
    constexpr auto &table = __char_converter_table<Converter, 10, 2>;
    constexpr auto unroll = [](auto &first, unsigned int v) {
        first[-1] = table[v * 2 + 1];
        first[-2] = table[v * 2];
        first -= 2;
    };

    do {
        if (WJR_UNLIKELY(n == 1)) {
            return __backward_to_chars_10(buf, up[0], conv);
        }

        uint64_t q, rem;

        q = div_qr_1_without_shift(up, rem, up, n, div2by1_divider_of_big_base_10);
        n -= q == 0;
        if (q != 0) {
            up[n - 1] = q;
        }

        for (int i = 0; i < 9; ++i) {
            unroll(buf, rem % 100);
            rem /= 100;
        }

        *--buf = conv.to(rem);
    } while (n);

    return buf;
}

template <typename Iter, typename Converter>
Iter basecase_to_chars(Iter first, size_t len, uint64_t *up, size_t n, unsigned int base,
                       Converter conv) {
    constexpr size_t buf_len = dc_bignum_to_chars_precompute_threshold * 64 * 7 / 11;
    char buf[buf_len];
    char *start;

    if (WJR_LIKELY(base == 10)) {
        start = basecase_to_chars_10(buf + buf_len, up, n, conv);
    } else {
        start = buf + buf_len;
    }

    size_t rlen = buf + buf_len - start;
    if (len > rlen) {
        first = std::fill_n(first, len - rlen, conv.to(0));
    }

    return std::copy(start, buf + buf_len, first);
}

template <typename Iter, typename Converter>
Iter dc_to_chars(Iter first, size_t len, uint64_t *up, size_t n,
                 precompute_to_chars_t *pre, uint64_t *stk, Converter conv) {
    WJR_ASSERT_ASSUME(n >= 1);
    if (n < dc_bignum_to_chars_threshold) {
        return basecase_to_chars(first, len, up, n, pre->base, conv);
    } else {
        const auto pp = pre->ptr;
        const auto pn = pre->n;
        const auto ps = pre->shift;

        if (n < pn + ps || (n == pn + ps && reverse_compare_n(up + ps, pp, pn) < 0)) {
            return dc_to_chars(first, len, up, n, pre - 1, stk, conv);
        }

        const auto pd = pre->digits_in_base;
        auto qp = stk;

        div_qr_s(qp, up + ps, up + ps, n - ps, pp, pn);

        size_t qn = n - pn - ps;
        qn += qp[qn] != 0;

        if (len != 0) {
            len = len - pd;
        }

        first = dc_to_chars(first, len, qp, qn, pre - 1, stk + qn, conv);
        first = dc_to_chars(first, pd, up, pn + ps, pre - 1, stk, conv);
        return first;
    }
}

template <typename Iter, typename Converter = char_converter_t,
          std::enable_if_t<is_any_of_v<remove_cvref_t<Converter>, char_converter_t,
                                       origin_converter_t>,
                           int> = 0>
Iter to_chars(Iter first, uint64_t *up, size_t n, unsigned int base = 10,
              Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10),
               "Not support yet.");

    if (is_zero_or_single_bit(base)) {
        switch (base) {
        case 2: {
            return to_chars_2(first, up, n, conv);
        }
        case 8: {
            return to_chars_8(first, up, n, conv);
        }
        case 16: {
            return to_chars_16(first, up, n, conv);
        }
        default: {
            return to_chars_power_of_two(first, base, up, n, conv);
        }
        }
    }

    if (WJR_UNLIKELY(n == 1)) {
        constexpr size_t buf_len = 41;
        char buf[buf_len];
        char *start;
        if (WJR_LIKELY(base == 10)) {
            start = __backward_to_chars_10(buf + buf_len, up[0], conv);
        } else {
            start = buf + buf_len;
        }
        return std::copy(start, buf + buf_len, first);
    }

    if (WJR_LIKELY(n < dc_bignum_to_chars_precompute_threshold)) {
        uint64_t upbuf[dc_bignum_to_chars_precompute_threshold];
        std::copy_n(up, n, upbuf);
        return basecase_to_chars(first, 0, upbuf, n, base, conv);
    }

    precompute_to_chars_t pre[64 - 3];

    unique_stack_allocator stkal(math_details::stack_alloc, std::in_place_index<1>);
    auto stk = static_cast<uint64_t *>(stkal.allocate(sizeof(uint64_t) * (n * 3 + 192)));
    auto __up = stk;
    std::copy_n(up, n, __up);
    stk += n;
    auto table_mem = stk;
    stk += n + 128;
    auto mpre = precompute_to_chars(pre, n, base, table_mem);
    return dc_to_chars(first, 0, __up, n, mpre, stk, conv);
}

template <typename Iter, typename Converter>
uint64_t *from_chars_2(Iter first, size_t n, uint64_t *up, Converter conv) {
    return up;
}

template <typename Iter, typename Converter>
uint64_t *from_chars_8(Iter first, size_t n, uint64_t *up, Converter conv) {
    return up;
}

template <typename Iter, typename Converter>
uint64_t *from_chars_16(Iter first, size_t n, uint64_t *up, Converter conv) {
    return up;
}

template <typename Iter, typename Converter>
void __from_chars_10(Iter first, size_t n, uint64_t &x, Converter conv) {
    x = 0;

    if (n >= 4) {
        do {
            auto x0 = conv.from(first[0]);
            auto x1 = conv.from(first[1]);
            auto x2 = conv.from(first[2]);
            auto x3 = conv.from(first[3]);

            x = x * 10000 + x0 * 1000 + x1 * 100 + x2 * 10 + x3;

            first += 4;
            n -= 4;
        } while (WJR_LIKELY(n >= 4));
    }

    switch (n) {
    case 3: {
        x = x * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 2: {
        x = x * 10 + conv.from(*first++);
        WJR_FALLTHROUGH;
    }
    case 1: {
        x = x * 10 + conv.from(*first++);
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
}

template <typename Iter, typename Converter>
size_t basecase_from_chars_10(Iter first, size_t n, uint64_t *up, Converter conv) {
    if (n <= 19) {
        __from_chars_10(first, n, up[0], conv);
        return up[0] != 0;
    }

    size_t m = (n - 1) % 19 + 1;

    __from_chars_10(first, m, up[0], conv);
    first += m;
    n -= m;

    m = up[0] != 0;

    do {
        uint64_t x = 0;
        int i = 19;

        do {
            auto x0 = conv.from(first[0]);
            auto x1 = conv.from(first[1]);
            auto x2 = conv.from(first[2]);
            auto x3 = conv.from(first[3]);

            x = x * 10000 + x0 * 1000 + x1 * 100 + x2 * 10 + x3;

            first += 4;
            i -= 4;
        } while (WJR_LIKELY(i >= 4));

        do {
            auto x0 = conv.from(first[0]);
            auto x1 = conv.from(first[1]);
            auto x2 = conv.from(first[2]);

            x = x * 1000 + x0 * 100 + x1 * 10 + x2;

            first += 3;
        } while (0);

        uint64_t cf;

        if (WJR_LIKELY(m == 0)) {
            cf = x;
        } else {
            cf = mul_1(up, up, m, div2by1_divider_of_big_base_10.get_divisor());
            cf += addc_1(up, up, m, x);
        }

        if (WJR_LIKELY(cf != 0)) {
            up[m++] = cf;
        }

        n -= 19;
    } while (WJR_LIKELY(n != 0));

    return m;
}

template <typename Iter, typename Converter>
size_t basecase_from_chars(Iter first, size_t n, uint64_t *up, unsigned int base,
                           Converter conv) {
    if (base == 10) {
        return basecase_from_chars_10(first, n, up, conv);
    } else {
        return 0;
    }
}

template <typename Iter, typename Converter>
size_t dc_from_chars(Iter first, size_t n, uint64_t *up, precompute_to_chars_t *pre,
                     uint64_t *stk, Converter conv) {
    size_t lo = pre->digits_in_base;
    if (n <= lo) {
        if (n < dc_bignum_from_chars_threshold) {
            return basecase_from_chars(first, n, up, pre->base, conv);
        } else {
            return dc_from_chars(first, n, up, pre - 1, stk, conv);
        }
    }

    size_t hi = n - lo;
    size_t hn, ln;

    if (hi < dc_bignum_from_chars_threshold) {
        hn = basecase_from_chars(first, hi, stk, pre->base, conv);
    } else {
        hn = dc_from_chars(first, hi, stk, pre - 1, up, conv);
    }

    size_t ps = pre->shift;
    size_t pn = pre->n;

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
        ln = dc_from_chars(first, lo, stk, pre - 1, stk + pn + ps + 1, conv);
    }

    WJR_ASSERT(ps + pn >= ln);

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

template <typename Iter, typename Converter = char_converter_t,
          std::enable_if_t<is_any_of_v<remove_cvref_t<Converter>, char_converter_t,
                                       origin_converter_t>,
                           int> = 0>
uint64_t *from_chars(Iter first, Iter last, uint64_t *up, unsigned int base = 10,
                     Converter conv = {}) {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10),
               "Not support yet.");

    if (is_zero_or_single_bit(base)) {
        return up;
    }

    size_t n = std::distance(first, last);

    if (WJR_LIKELY(n < dc_bignum_from_chars_precompute_threshold)) {
        return up + basecase_from_chars(first, n, up, base, conv);
    }

    const auto per_digits = precompute_to_chars_16n_ptr[base]->digits_in_one_base;

    precompute_to_chars_t pre[64 - 3];

    unique_stack_allocator stkal(math_details::stack_alloc, std::in_place_index<1>);
    size_t un = n / per_digits + 1;
    auto stk = static_cast<uint64_t *>(stkal.allocate(n * 2 + 192));
    auto table_mem = stk;
    stk += n + 128;

    auto mpre = precompute_to_chars(pre, un, base, table_mem);

    return up + dc_from_chars(first, n, up, mpre, stk, conv);
}

} // namespace wjr

#endif // WJR_MATH_CONVERT_HPP__