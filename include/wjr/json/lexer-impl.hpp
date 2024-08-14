#ifndef WJR_JSON_LEXER_IMPL_HPP__
#define WJR_JSON_LEXER_IMPL_HPP__

#include <wjr/crtp/class_base.hpp>
#include <wjr/span.hpp>

namespace wjr::json {

namespace lexer_detail {

WJR_CONST WJR_INLINE_CONSTEXPR uint64_t calc_backslash(uint64_t B) noexcept {
    uint64_t maybe_escaped = B << 1;

    uint64_t maybe_escaped_and_odd_bits = maybe_escaped | 0xAAAAAAAAAAAAAAAAULL;
    uint64_t even_series_codes_and_odd_bits = maybe_escaped_and_odd_bits - B;

    return even_series_codes_and_odd_bits ^ 0xAAAAAAAAAAAAAAAAULL;
}

} // namespace lexer_detail

class lexer {
public:
    using size_type = uint32_t;

    lexer() = delete;
    lexer(const lexer &) = delete;
    lexer(lexer &&) = default;
    lexer &operator=(const lexer &) = delete;
    lexer &operator=(lexer &&) = default;
    ~lexer() = default;

    constexpr lexer(span<const char> input) noexcept
        : first(input.data()), last(input.data() + input.size()) {}

    class result_type {

    public:
        constexpr static uint32_t mask = static_cast<uint32_t>(1) << 31;

        WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(result_type);

        constexpr result_type(uint32_t result) noexcept : m_result(result) {}
        constexpr uint32_t get() const noexcept { return m_result & (mask - 1); }
        constexpr bool done() const noexcept { return (m_result & mask) != 0; }

    private:
        uint32_t m_result;
    };

    /**
     * @brief read tokens
     *
     * @details Read at least token_buf_size tokens from the input.
     * token_buf' size must be at least token_buf_size * 2 - 1.
     *
     * @return return the number of tokens read.
     *
     */
    result_type read(uint32_t *token_buf, size_type token_buf_size) noexcept;

    constexpr const char *begin() const noexcept { return first; }
    constexpr const char *end() const noexcept { return last; }

private:
    const char *first;
    const char *last;

    uint64_t prev_in_string = 0;
    uint64_t prev_is_escape = 0;
    uint64_t prev_is_ws = ~0ull;
    uint32_t idx = 0;
};

extern char *minify(char *dst, const char *first, const char *last) noexcept;

} // namespace wjr::json

#endif // WJR_JSON_LEXER_IMPL_HPP__