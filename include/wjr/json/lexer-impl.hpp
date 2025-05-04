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
    constexpr static uint32_t big_mask = static_cast<uint32_t>(1) << 31;

public:
    using size_type = uint32_t;

    lexer() = delete;
    lexer(const lexer &) = delete;
    lexer(lexer &&) = default;
    lexer &operator=(const lexer &) = delete;
    lexer &operator=(lexer &&) = default;
    ~lexer() = default;

    constexpr lexer(std::string_view input) noexcept
        : first(input.data()), last(input.data() + input.size()) {}

    /**
     * @brief read tokens
     *
     * @return return the number of tokens read.
     *
     */
    uint32_t read(uint32_t *token_buf, size_type token_buf_size) noexcept;

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

extern WJR_ALL_NONNULL WJR_RETURNS_NONNULL char *minify(char *dst, const char *first,
                                                        const char *last) noexcept;

} // namespace wjr::json

#endif // WJR_JSON_LEXER_IMPL_HPP__