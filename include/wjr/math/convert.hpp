#ifndef WJR_MATH_CONVERT_HPP__
#define WJR_MATH_CONVERT_HPP__

#include <array>

#include <wjr/math/clz.hpp>
#include <wjr/math/precompute-convert.hpp>

namespace wjr {

inline constexpr size_t dc_bignum_to_chars_threshold = WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;

inline constexpr auto div2by1_divider_of_big_base_10 = div2by1_divider<uint64_t>(
    10'000'000'000'000'000'000ull, 15'581'492'618'384'294'730ull, 0);

struct char_converter_t {
private:
    static constexpr char table[36] = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
                                       '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
                                       'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
                                       'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

public:
    WJR_CONST constexpr char operator()(unsigned int x) const { return table[x]; }
};

inline constexpr char_converter_t char_converter = {};

struct origin_converter_t {
    WJR_CONST constexpr char operator()(unsigned int x) const { return x; }
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
                table[j + k] = conv(x % Base);
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
        *--first = conv(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 2: {
        *--first = conv(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 1: {
        *--first = conv(x & 1);
        x >>= 1;
        WJR_FALLTHROUGH;
    }
    case 0: {
        *--first = conv(x);
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
            *--first = conv(x << 2 | last);
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
        *--first = conv(x);
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
        *--first = conv(x);
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
                *--first = conv(val);
            } else {
                rest = 64;
            }

            WJR_ASSERT_ASSUME(rest >= per_bit);

            do {
                *--first = conv(x & mask);
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
        *--first = conv(val);
        rest += hbits - fix;
        if (WJR_UNLIKELY(rest == 0)) {
            goto DONE;
        }
    } else {
        rest = hbits;
    }

    do {
        *--first = conv(x & mask);
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
        *--buf = conv(val);
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

    uint64_t q = 0;
    uint64_t rem = 0;
    do {
        if (WJR_UNLIKELY(n == 1)) {
            return __backward_to_chars_10(buf, up[0], conv);
        }

        q = div_qr_1_without_shift(up, rem, up, n, div2by1_divider_of_big_base_10);
        n -= q == 0;
        if (q != 0) {
            up[n - 1] = q;
        }

        for (int i = 0; i < 9; ++i) {
            unroll(buf, rem % 100);
            rem /= 100;
        }

        *--buf = conv(rem);
    } while (n);

    return buf;
}

template <typename Iter, typename Converter>
Iter basecase_to_chars(Iter first, size_t len, unsigned int base, uint64_t *up, size_t n,
                       Converter conv) {
    constexpr size_t buf_len = dc_bignum_to_chars_threshold * 64 * 7 / 11;
    char buf[buf_len];
    char *start;

    if (WJR_LIKELY(base == 10)) {
        start = basecase_to_chars_10(buf + buf_len, up, n, conv);
    } else {
        start = buf + buf_len;
    }

    size_t rlen = buf + buf_len - start;
    if (len > rlen) {
        first = std::fill_n(first, len - rlen, conv(0));
    }

    return std::copy(start, buf + buf_len, first);
}

template <typename Iter, typename Converter>
Iter dc_to_chars(Iter first, size_t len, uint64_t *up, size_t n,
                 precompute_to_chars_t *pre, uint64_t *stk, Converter conv) {
    WJR_ASSERT_ASSUME(n >= 1);
    if (n < dc_bignum_to_chars_threshold) {
        return basecase_to_chars(first, len, pre->base, up, n, conv);
    } else {
        const auto pp = pre->ptr;
        const auto pn = pre->n;

        if (n < pn || (n == pn && reverse_compare_n(up, pp, n) < 0)) {
            return dc_to_chars(first, len, up, n, pre - 1, stk, conv);
        }

        const auto pd = pre->digits_in_base;
        auto qp = stk;
        auto rp = up;

        div_qr_s(qp, rp, up, n, pp, pn);

        size_t qn = n - pn;
        qn += qp[qn] != 0;

        if (len != 0) {
            len = len - pd;
        }

        first = dc_to_chars(first, len, qp, qn, pre - 1, stk + qn, conv);
        first = dc_to_chars(first, pd, rp, pn, pre - 1, stk, conv);
        return first;
    }
}

template <
    typename Iter, typename Converter = char_converter_t,
    std::enable_if_t<
        std::disjunction_v<std::is_same<remove_cvref_t<Converter>, char_converter_t>,
                           std::is_same<remove_cvref_t<Converter>, origin_converter_t>>,
        int> = 0>
Iter to_chars(Iter first, unsigned int base, uint64_t *up, size_t n,
              Converter conv = {}) {
    WJR_ASSERT(base <= 36 && is_power_of_two(base) || base == 10, "Not support yet.");

    if (is_power_of_two(base)) {
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

    if (WJR_LIKELY(n < dc_bignum_to_chars_threshold)) {
        uint64_t upbuf[dc_bignum_to_chars_threshold];
        std::copy_n(up, n, upbuf);
        return basecase_to_chars(first, 0, base, upbuf, n, conv);
    }

    precompute_to_chars_t pre[64 - 3];

    unique_stack_allocator stkal(math_details::stack_alloc, std::in_place_index<1>);
    auto stk = static_cast<uint64_t *>(stkal.allocate(sizeof(uint64_t) * (3 * n + 192)));
    auto __up = stk;
    std::copy_n(up, n, __up);
    stk += n;
    auto table_mem = stk;
    stk += n + 128;
    auto mpre = precompute_to_chars(pre, base, n, table_mem);
    return dc_to_chars(first, 0, __up, n, mpre, stk, conv);
}

} // namespace wjr

#endif // WJR_MATH_CONVERT_HPP__