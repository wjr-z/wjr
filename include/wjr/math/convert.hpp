#ifndef WJR_MATH_CONVERT_HPP__
#define WJR_MATH_CONVERT_HPP__

#include <array>

#include <wjr/math/clz.hpp>
#include <wjr/math/div.hpp>

namespace wjr {

inline constexpr size_t dc_bignum_to_string_threshold = WJR_DC_BIGNUM_TO_STRING_THRESHOLD;

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

struct char_original_converter_t {
    WJR_CONST constexpr char operator()(unsigned int x) const { return x; }
};

inline constexpr char_original_converter_t char_original_converter = {};

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

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = hbits + 64 * n;
    first += len;

    constexpr auto unroll = [](auto &first, unsigned int v) {
        first[-1] = table[v * 4 + 3];
        first[-2] = table[v * 4 + 2];
        first[-3] = table[v * 4 + 1];
        first[-4] = table[v * 4];
        first -= 4;
    };

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

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = (hbits + 64 * n + 2) / 3;
    first += len;

    constexpr auto unroll = [](auto &first, unsigned int v) {
        first[-1] = table[v * 2 + 1];
        first[-2] = table[v * 2];
        first -= 2;
    };

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

    uint64_t x = up[n - 1];
    --n;
    int pc = clz(x);
    WJR_ASSUME(0 <= pc && pc <= 63);
    int hbits = 64 - pc;
    WJR_ASSUME(1 <= hbits && hbits <= 64);
    size_t len = (hbits + 3) / 4 + 16 * n;
    first += len;

    constexpr auto unroll = [](auto &first, unsigned int v) {
        first[-1] = table[v * 2 + 1];
        first[-2] = table[v * 2];
        first -= 2;
    };

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

inline char *bc_bignum_to_string_base10(char *first, unsigned int base, uint64_t *up,
                                        size_t n) {
    uint64_t q = 0;
    uint64_t rem = 0;
    do {
        q = div_qr_1_without_shift(up, rem, up, n, div2by1_divider_of_big_base_10);
        n -= q == 0;
        if (q != 0) {
            up[n - 1] = q;
        }
        for (int i = 0; i < 19; ++i) {
            *first++ = rem % 10;
            rem /= 10;
        }
    } while (n);

    return first;
}

inline char *bignum_to_string(char *first, unsigned int base, uint64_t *p, size_t n) {
    WJR_ASSERT_ASSUME(n >= 1);
    WJR_ASSERT(base != 0);
    WJR_ASSERT(base == 10 || is_power_of_two(base),
               "Only support simple case currently.");

    uint64_t up[WJR_DC_BIGNUM_TO_STRING_THRESHOLD];
    std::copy_n(p, n, up);
    return bc_bignum_to_string_base10(first, base, up, n);
}

} // namespace wjr

#endif // WJR_MATH_CONVERT_HPP__