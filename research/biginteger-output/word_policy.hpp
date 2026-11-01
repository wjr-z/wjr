#include <wjr/biginteger/detail/convert.hpp>
namespace wjr {
template <typename Iter, typename Converter>
Iter _probe_word_fallback(Iter ptr, const uint64_t *up, size_t n, unsigned int base,
                          Converter conv) noexcept {
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
Iter _probe_word_driver(Iter first, const uint64_t *up, size_t n, unsigned int base,
                        Converter conv) noexcept {
    if (WJR_UNLIKELY(n == 1)) {
        return to_chars_unchecked(first, up[0], base, conv);
    }

    if constexpr (charconv_detail::_is_fast_convert_iterator_v<Iter>) {
        auto *const _first = reinterpret_cast<uint8_t *>(wjr::to_address(first));
        const auto _result = _fast_biginteger_large_to_chars_impl(_first, up, n, base, conv);
        return first + std::distance(_first, _result);
    } else {
        return _probe_word_fallback(first, up, n, base, conv);
    }
}

template <typename Iter, typename Converter = char_converter_t>
Iter _probe_word_public_to_chars(Iter first, const uint64_t *up, size_t n, unsigned int base = 10,
                                 Converter conv = {}) noexcept {
    WJR_ASSERT(base <= 36 && (is_zero_or_single_bit(base) || base == 10));
    WJR_ASSERT_ASSUME(n != 0);
    WJR_ASSERT_ASSUME(up[n - 1] != 0);

    return _probe_word_driver(first, up, n, base, conv);
}

} // namespace wjr
