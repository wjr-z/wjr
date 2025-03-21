#ifndef WJR_BIGINTEGER_DETAIL_CONVERT_HPP__
#define WJR_BIGINTEGER_DETAIL_CONVERT_HPP__

#include <wjr/format/charconv.hpp>

#include <wjr/biginteger/detail/div.hpp>
#include <wjr/biginteger/detail/precompute-chars-convert.hpp>
#include <wjr/memory/stack_allocator.hpp>

namespace wjr {

namespace {
inline constexpr size_t dc_bignum_to_chars_threshold = WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;
inline constexpr size_t dc_bignum_to_chars_precompute_threshold = WJR_DC_BIGNUM_TO_CHARS_THRESHOLD;

inline constexpr size_t dc_bignum_from_chars_threshold = WJR_DC_BIGNUM_FROM_CHARS_THRESHOLD;
inline constexpr size_t dc_bignum_from_chars_precompute_threshold =
    WJR_DC_BIGNUM_FROM_CHARS_PRECOMPUTE_THRESHOLD;

inline constexpr auto div2by1_divider_noshift_of_big_base_10 =
    div2by1_divider_noshift<uint64_t>(10'000'000'000'000'000'000ull, 15'581'492'618'384'294'730ull);

inline constexpr auto div3by2_divider_shift_of_big_base_10 = div3by2_divider<uint64_t>(
    1374799102801346560ull, 10842021724855044340ull, 12'938'764'603'223'852'203ull, 1);
} // namespace

template <typename Converter>
size_t __biginteger_to_chars_2_impl(uint8_t *first, const uint64_t *up, size_t n,
                                    Converter conv) noexcept;

extern template size_t
__biginteger_to_chars_2_impl<char_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                               char_converter_t conv) noexcept;
extern template size_t
__biginteger_to_chars_2_impl<origin_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                 origin_converter_t conv) noexcept;

template <typename Converter>
size_t __biginteger_to_chars_8_impl(uint8_t *first, const uint64_t *up, size_t n,
                                    Converter conv) noexcept;

extern template size_t
__biginteger_to_chars_8_impl<char_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                               char_converter_t conv) noexcept;

extern template size_t
__biginteger_to_chars_8_impl<origin_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                 origin_converter_t conv) noexcept;

template <typename Converter>
size_t __biginteger_to_chars_16_impl(uint8_t *first, const uint64_t *up, size_t n,
                                     Converter conv) noexcept;

extern template size_t
__biginteger_to_chars_16_impl<char_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                char_converter_t conv) noexcept;

extern template size_t
__biginteger_to_chars_16_impl<origin_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                  origin_converter_t conv) noexcept;

template <typename Converter>
WJR_ALL_NONNULL size_t __biginteger_to_chars_power_of_two_impl(uint8_t *first, const uint64_t *up,
                                                               size_t n, unsigned int base,
                                                               Converter conv) noexcept;

extern template size_t
__biginteger_to_chars_power_of_two_impl<char_converter_t>(uint8_t *first, const uint64_t *up,
                                                          size_t n, unsigned int base,
                                                          char_converter_t conv) noexcept;
extern template size_t
__biginteger_to_chars_power_of_two_impl<origin_converter_t>(uint8_t *first, const uint64_t *up,
                                                            size_t n, unsigned int base,
                                                            origin_converter_t conv) noexcept;

template <typename Converter>
WJR_ALL_NONNULL WJR_RETURNS_NONNULL uint8_t *
basecase_to_chars_10(uint8_t *buf, uint64_t *up, size_t n, Converter conv) noexcept;

extern template uint8_t *basecase_to_chars_10<char_converter_t>(uint8_t *buf, uint64_t *up,
                                                                size_t n,
                                                                char_converter_t conv) noexcept;
extern template uint8_t *basecase_to_chars_10<origin_converter_t>(uint8_t *buf, uint64_t *up,
                                                                  size_t n,
                                                                  origin_converter_t conv) noexcept;

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

    return copy_restrict(start, end, first);
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
                               Converter conv) noexcept;

extern template uint8_t *
__biginteger_basecase_to_chars<char_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                 unsigned int base, char_converter_t conv) noexcept;
extern template uint8_t *
__biginteger_basecase_to_chars<origin_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                   unsigned int base,
                                                   origin_converter_t conv) noexcept;

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
        auto *const __ptr = reinterpret_cast<uint8_t *>(stkal.template allocate<uint64_t>(SIZE));  \
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
                                              Converter conv) noexcept;

extern template WJR_ALL_NONNULL size_t basecase_from_chars_10<char_converter_t>(
    const uint8_t *first, size_t n, uint64_t *up, char_converter_t conv) noexcept;
extern template WJR_ALL_NONNULL size_t basecase_from_chars_10<origin_converter_t>(
    const uint8_t *first, size_t n, uint64_t *up, origin_converter_t conv) noexcept;

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
    auto *stk = stkal.template allocate<uint64_t>(un * 16 / 5 + 192);
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