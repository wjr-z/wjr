#ifndef WJR_JSON_LEXER_IMPL_HPP
#define WJR_JSON_LEXER_IMPL_HPP

#include <wjr/span.hpp>

namespace wjr::json {

struct forward_lexer_storage {
    forward_lexer_storage(span<const char> input) noexcept
        : first(input.data()), last(input.data() + input.size()) {}

    const char *first;
    const char *last;

    uint64_t prev_in_string = 0;
    uint64_t prev_is_escape = 0;
    uint64_t prev_is_ws = ~0ull;

    uint32_t idx = 0;
};

struct dynamic_lexer_storage {
    dynamic_lexer_storage(span<const char> input) noexcept
        : first(input.data()), last(input.data() + input.size()) {}

    const char *first;
    const char *last;
};

template <uint32_t token_buf_size>
class basic_lexer {
    static_assert(((token_buf_size & (token_buf_size - 1)) == 0 &&
                   token_buf_size <= 65536) ||
                      token_buf_size == (uint32_t)in_place_max,
                  "token_buf_size must be a power of 2");

    constexpr static bool __is_dynamic = token_buf_size == (uint32_t)in_place_max;
    using storage_type =
        std::conditional_t<__is_dynamic, dynamic_lexer_storage, forward_lexer_storage>;

public:
    constexpr basic_lexer(span<const char> input) noexcept : m_storage(input) {}

    basic_lexer() = delete;
    constexpr basic_lexer(const basic_lexer &) = delete;
    constexpr basic_lexer(basic_lexer &&) = default;
    constexpr basic_lexer &operator=(const basic_lexer &) = delete;
    constexpr basic_lexer &operator=(basic_lexer &&) = default;
    ~basic_lexer() = default;

    /**
     * @brief read tokens
     *
     * @details Read at least token_buf_size tokens from the input.
     * token_buf' size must be at least token_buf_size * 2 - 1.
     *
     * @return return the number of tokens read.
     *
     */

    uint32_t read(uint32_t *token_buf) noexcept;

    WJR_PURE const char *end() const noexcept { return m_storage.last; }

private:
    storage_type m_storage;
};

using dynamic_lexer = basic_lexer<in_place_max>;

template <uint32_t token_buf_size>
using forward_lexer = basic_lexer<token_buf_size>;

namespace lexer_details {
inline uint64_t calc_backslash(uint64_t B) {
    uint64_t maybe_escaped = B << 1;

    uint64_t maybe_escaped_and_odd_bits = maybe_escaped | 0xAAAAAAAAAAAAAAAAULL;
    uint64_t even_series_codes_and_odd_bits = maybe_escaped_and_odd_bits - B;

    return even_series_codes_and_odd_bits ^ 0xAAAAAAAAAAAAAAAAULL;
}
} // namespace lexer_details

} // namespace wjr::json

#endif // WJR_JSON_LEXER_IMPL_HPP