#ifndef WJR_BIGINTEGER_DETAIL_CONVERT_HPP__
#define WJR_BIGINTEGER_DETAIL_CONVERT_HPP__

#include <wjr/format/charconv.hpp>

#include <wjr/biginteger/detail/bignum-config.hpp>
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
WJR_ALL_NONNULL size_t _biginteger_to_chars_2_impl(uint8_t *first, const uint64_t *up, size_t n,
                                                   Converter conv) noexcept;

extern template size_t
_biginteger_to_chars_2_impl<char_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                              char_converter_t conv) noexcept;
extern template size_t
_biginteger_to_chars_2_impl<origin_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                origin_converter_t conv) noexcept;

template <typename Converter>
WJR_ALL_NONNULL size_t _biginteger_to_chars_8_impl(uint8_t *first, const uint64_t *up, size_t n,
                                                   Converter conv) noexcept;

extern template size_t
_biginteger_to_chars_8_impl<char_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                              char_converter_t conv) noexcept;

extern template size_t
_biginteger_to_chars_8_impl<origin_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                origin_converter_t conv) noexcept;

template <typename Converter>
WJR_ALL_NONNULL size_t _biginteger_to_chars_16_impl(uint8_t *first, const uint64_t *up, size_t n,
                                                    Converter conv) noexcept;

extern template size_t
_biginteger_to_chars_16_impl<char_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                               char_converter_t conv) noexcept;

extern template size_t
_biginteger_to_chars_16_impl<origin_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                 origin_converter_t conv) noexcept;

template <typename Converter>
WJR_ALL_NONNULL size_t _biginteger_to_chars_power_of_two_impl(uint8_t *first, const uint64_t *up,
                                                              size_t n, unsigned int base,
                                                              Converter conv) noexcept;

extern template size_t
_biginteger_to_chars_power_of_two_impl<char_converter_t>(uint8_t *first, const uint64_t *up,
                                                         size_t n, unsigned int base,
                                                         char_converter_t conv) noexcept;
extern template size_t
_biginteger_to_chars_power_of_two_impl<origin_converter_t>(uint8_t *first, const uint64_t *up,
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
    const size_t pn = pre->size;
    const size_t ps = pre->shift;

    WJR_ASSERT((pn + ps) * 5 >= n * 2);

    if (n < pn + ps || (n == pn + ps && reverse_compare_n(up + ps, pp, pn) < 0)) {
        return dc_to_chars(first, len, up, n, pre - 1, stk, conv);
    }

    const size_t pd = pre->digits_in_base;
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
_biginteger_basecase_to_chars(uint8_t *first, const uint64_t *up, size_t n, unsigned int base,
                              Converter conv) noexcept;

extern template uint8_t *
_biginteger_basecase_to_chars<char_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                unsigned int base, char_converter_t conv) noexcept;
extern template uint8_t *
_biginteger_basecase_to_chars<origin_converter_t>(uint8_t *first, const uint64_t *up, size_t n,
                                                  unsigned int base,
                                                  origin_converter_t conv) noexcept;

template <typename Converter>
uint8_t *_fast_biginteger_large_to_chars_impl(uint8_t *first, const uint64_t *up, size_t n,
                                              unsigned int base, Converter conv) noexcept {
    switch (base) {
    case 2: {
        return first + _biginteger_to_chars_2_impl(first, up, n, conv);
    }
    case 8: {
        return first + _biginteger_to_chars_8_impl(first, up, n, conv);
    }
    case 16: {
        return first + _biginteger_to_chars_16_impl(first, up, n, conv);
    }
    case 4:
    case 32: {
        return first + _biginteger_to_chars_power_of_two_impl(first, up, n, base, conv);
    }
    default: {
        break;
    }
    }

    return _biginteger_basecase_to_chars(first, up, n, base, conv);
}

template <typename Iter, typename Converter>
Iter _fallback_biginteger_large_to_chars_impl(Iter ptr, const uint64_t *up, size_t n,
                                              unsigned int base, Converter conv) noexcept {
#define WJR_BIGINTEGER_TO_CHARS_IMPL(BASE, NAME, TAIL, SIZE, CALL)                                 \
    constexpr auto _fast_container_inserter_v =                                                    \
        charconv_detail::is_fast_container_inserter_v<Iter>;                                       \
    if constexpr (_fast_container_inserter_v != 0) {                                               \
        auto &cont = get_inserter_container(ptr);                                                  \
        const auto _presize = cont.size();                                                         \
        if constexpr (_fast_container_inserter_v == 1) {                                           \
            resize(cont, _presize + SIZE);                                                         \
        } else {                                                                                   \
            append(cont, SIZE, default_construct);                                                 \
        }                                                                                          \
        auto *const _ptr = reinterpret_cast<uint8_t *>(wjr::to_address(cont.data())) + _presize;   \
        const auto _size = NAME(_ptr, WJR_PP_QUEUE_EXPAND(CALL), conv) TAIL;                       \
        WJR_ASSERT((size_t)_size <= SIZE);                                                         \
        if constexpr (_fast_container_inserter_v == 1) {                                           \
            resize(cont, _presize + _size);                                                        \
        } else {                                                                                   \
            resize(cont, _presize + _size, default_construct);                                     \
        }                                                                                          \
                                                                                                   \
        return ptr;                                                                                \
    } else {                                                                                       \
        unique_stack_allocator stkal;                                                              \
        auto *const _ptr = reinterpret_cast<uint8_t *>(stkal.template allocate<uint64_t>(SIZE));   \
        const auto _size = NAME(_ptr, WJR_PP_QUEUE_EXPAND(CALL), conv) TAIL;                       \
                                                                                                   \
        return wjr::copy_n((charconv_detail::fast_buffer_t<Iter> *)_ptr, _size, ptr);              \
    }

    switch (base) {
    case 2: {
        const size_t capacity = 64 * n;
        WJR_BIGINTEGER_TO_CHARS_IMPL(2, _biginteger_to_chars_2_impl, , capacity, (up, n));
    }
    case 8: {
        const size_t capacity = (64 * n + 2) / 3;
        WJR_BIGINTEGER_TO_CHARS_IMPL(8, _biginteger_to_chars_8_impl, , capacity, (up, n));
    }
    case 16: {
        const size_t capacity = (64 * n + 3) / 4;
        WJR_BIGINTEGER_TO_CHARS_IMPL(16, _biginteger_to_chars_16_impl, , capacity, (up, n));
    }
    case 4:
    case 32: {
        const int bits = base == 4 ? 2 : 5;
        const size_t capacity = (64 * n + bits - 1) / bits;
        WJR_BIGINTEGER_TO_CHARS_IMPL(base, _biginteger_to_chars_power_of_two_impl, , capacity,
                                     (up, n, base));
    }
    default: {
        break;
    }
    }

    // todo : optimize memory usage
    const size_t capacity = ((64 * n) * 4 + 12) / 13;
    WJR_BIGINTEGER_TO_CHARS_IMPL(base, _biginteger_basecase_to_chars, -_ptr, capacity,
                                 (up, n, base));

#undef WJR_BIGINTEGER_TO_CHARS_IMPL
}

template <typename Iter, typename Converter>
Iter _biginteger_to_chars_impl(Iter first, const uint64_t *up, size_t n, unsigned int base,
                               Converter conv) noexcept {
    if (WJR_UNLIKELY(n == 1)) {
        return to_chars_unchecked(first, up[0], base, conv);
    }

    if constexpr (charconv_detail::_is_fast_convert_iterator_v<Iter>) {
        auto *const _first = reinterpret_cast<uint8_t *>(wjr::to_address(first));
        const auto _result = _fast_biginteger_large_to_chars_impl(_first, up, n, base, conv);
        return first + std::distance(_first, _result);
    } else {
        return _fallback_biginteger_large_to_chars_impl(first, up, n, base, conv);
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

    return _biginteger_to_chars_impl(first, up, n, base, conv);
}

template <typename Converter>
size_t _biginteger_from_chars_2_impl(const uint8_t *first, size_t n, uint64_t *up,
                                     Converter conv) noexcept {
    const size_t hbits = (n - 1) % 64 + 1;
    const size_t len = (n - 1) / 64 + 1;

    uint64_t x = 0;
    up += len;

    _unsigned_from_chars_unchecked<2>(first, first + hbits, x, conv);
    first += hbits;

    *--up = x;

    size_t idx = len - 1;

    if (idx) {
        do {
            x = 0;

            for (int i = 0; i < 4; ++i) {
                x = (x << 16) + _from_chars_unroll_16<2>(first, conv);
                first += 16;
            }

            *--up = x;
        } while (WJR_LIKELY(--idx));
    }

    return len;
}

template <typename Converter>
WJR_ALL_NONNULL size_t _biginteger_from_chars_8_impl(const uint8_t *first, size_t n, uint64_t *up,
                                                     Converter conv) noexcept;

extern template size_t
_biginteger_from_chars_8_impl<char_converter_t>(const uint8_t *first, size_t n, uint64_t *up,
                                                char_converter_t conv) noexcept;

extern template size_t
_biginteger_from_chars_8_impl<origin_converter_t>(const uint8_t *first, size_t n, uint64_t *up,
                                                  origin_converter_t conv) noexcept;

template <typename Converter>
size_t _biginteger_from_chars_16_impl(const uint8_t *first, size_t n, uint64_t *up,
                                      Converter conv) noexcept {
    const size_t hbits = (n - 1) % 16 + 1;
    const size_t len = (n - 1) / 16 + 1;

    uint64_t x = 0;
    up += len;

    _unsigned_from_chars_unchecked<16>(first, first + hbits, x, conv);
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
WJR_ALL_NONNULL size_t dc_from_chars(const uint8_t *first, size_t n, uint64_t *up,
                                     precompute_chars_convert_t *pre, uint64_t *stk,
                                     Converter conv) noexcept;

extern template size_t dc_from_chars<char_converter_t>(const uint8_t *first, size_t n, uint64_t *up,
                                                       precompute_chars_convert_t *pre,
                                                       uint64_t *stk,
                                                       char_converter_t conv) noexcept;

extern template size_t dc_from_chars<origin_converter_t>(const uint8_t *first, size_t n,
                                                         uint64_t *up,
                                                         precompute_chars_convert_t *pre,
                                                         uint64_t *stk,
                                                         origin_converter_t conv) noexcept;

template <typename Converter>
uint64_t *_basecase_basecase_from_chars(const uint8_t *first, size_t n, uint64_t *up,
                                        unsigned int base, Converter conv) noexcept {
    if (WJR_LIKELY(n < dc_bignum_from_chars_precompute_threshold)) {
        return up + basecase_from_chars(first, n, up, base, conv);
    }

    const auto per_digits = precompute_chars_convert_digits_in_one_base[base];

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
_biginteger_from_chars_impl(const uint8_t *first, const uint8_t *last, uint64_t *up,
                            unsigned int base, Converter conv) noexcept {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));

    const size_t n = std::distance(first, last);

    if (is_zero_or_single_bit(base)) {
        switch (base) {
        case 2: {
            return up + _biginteger_from_chars_2_impl(first, n, up, conv);
        }
        case 8: {
            return up + _biginteger_from_chars_8_impl(first, n, up, conv);
        }
        case 16: {
            return up + _biginteger_from_chars_16_impl(first, n, up, conv);
        }
        default: {
            WJR_UNREACHABLE();
        }
        }
    }

    return _basecase_basecase_from_chars(first, n, up, base, conv);
}

extern template uint64_t *
_basecase_basecase_from_chars<char_converter_t>(const uint8_t *first, size_t n, uint64_t *up,
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
          WJR_REQUIRES(charconv_detail::_is_fast_convert_iterator_v<Iter>)>
uint64_t *biginteger_from_chars(Iter first, Iter last, uint64_t *up, unsigned int base = 10,
                                Converter conv = {}) noexcept {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));

    const auto *const _first = reinterpret_cast<const uint8_t *>(wjr::to_address(first));
    const auto *const _last = reinterpret_cast<const uint8_t *>(wjr::to_address(last));

    return _biginteger_from_chars_impl(_first, _last, up, base, conv);
}

} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_CONVERT_HPP__