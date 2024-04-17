#ifndef WJR_JSON_LEXER_IMPL_HPP__
#define WJR_JSON_LEXER_IMPL_HPP__

#include <wjr/span.hpp>

namespace wjr::json {

struct forward_lexer_reader_storage {
    forward_lexer_reader_storage(span<const char> input) noexcept
        : first(input.data()), last(input.data() + input.size()) {}

    const char *first;
    const char *last;

    uint64_t prev_in_string = 0;
    uint64_t prev_is_escape = 0;
    uint64_t prev_is_ws = 0;

    uint32_t idx = 0;
};

struct dynamic_lexer_reader_storage {
    dynamic_lexer_reader_storage(span<const char> input) noexcept
        : first(input.data()), last(input.data() + input.size()) {}

    const char *first;
    const char *last;
};

template <uint32_t token_buf_size>
class basic_lexer_reader {
    static_assert(((token_buf_size & (token_buf_size - 1)) == 0 &&
                   token_buf_size <= 65536) ||
                      token_buf_size == (uint32_t)in_place_max,
                  "token_buf_size must be a power of 2");

    constexpr static bool __is_dynamic = token_buf_size == (uint32_t)in_place_max;
    using storage_type = std::conditional_t<__is_dynamic, dynamic_lexer_reader_storage,
                                            forward_lexer_reader_storage>;

public:
    constexpr basic_lexer_reader(span<const char> input) noexcept : m_storage(input) {}

    basic_lexer_reader() = delete;
    constexpr basic_lexer_reader(const basic_lexer_reader &) = delete;
    constexpr basic_lexer_reader(basic_lexer_reader &&) = default;
    constexpr basic_lexer_reader &operator=(const basic_lexer_reader &) = delete;
    constexpr basic_lexer_reader &operator=(basic_lexer_reader &&) = default;
    ~basic_lexer_reader() = default;

    /**
     * @brief read tokens
     *
     * @details Read at least token_buf_size tokens from the input.
     * token_buf' size must be at least token_buf_size * 2 - 1.
     *
     * @return return the number of tokens read.
     *
     */

    inline uint32_t read(uint32_t *token_buf) noexcept;

private:
    WJR_NOINLINE uint32_t read_buf(uint32_t *token_buf) noexcept;

    storage_type m_storage;
};

using dynamic_lexer_reader = basic_lexer_reader<in_place_max>;

template <uint32_t token_buf_size>
using forward_lexer_reader = basic_lexer_reader<token_buf_size>;

} // namespace wjr::json

#endif // WJR_JSON_LEXER_IMPL_HPP__