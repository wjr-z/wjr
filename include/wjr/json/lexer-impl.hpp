#ifndef WJR_JSON_LEXER_IMPL_HPP__
#define WJR_JSON_LEXER_IMPL_HPP__

#include <cstdint>

#include <wjr/span.hpp>

namespace wjr::json {

struct lexer_iterator_struct {
    uint32_t *token_ptr = nullptr;

    uint64_t prev_in_string = 0;
    uint64_t prev_is_escape = 0;
    uint64_t prev_is_ws = 0;

    uint32_t idx = 0;
};

struct lexer_iterator_end {};

class lexer_iterator {
    static constexpr unsigned int token_buf_size = 64;

public:
private:
    lexer_iterator_struct lex;

    const char *first;
    const char *last;
    unsigned int count = 0;
    uint32_t token_buf[token_buf_size * 2 - 1];
};

struct basic_lexer {
    basic_lexer(const char *first, const char *last) : first(first), last(last) {}

    basic_lexer() = delete;
    basic_lexer(const basic_lexer &) = default;
    basic_lexer &operator=(const basic_lexer &) = default;
    ~basic_lexer() = default;

    const char *first;
    const char *last;

    uint64_t prev_in_string = 0;
    uint64_t prev_is_escape = 0;
    uint64_t prev_is_ws = 0;

    uint32_t idx = 0;
    uint32_t *token_first = nullptr;
    uint32_t *token_last = nullptr;
    alignas(16) uint32_t token_buf[64 * 2 - 1];
};

class lexer {
public:
    lexer(span<const char> sp) : lex(sp.begin(), sp.end()) {}

    lexer() = delete;
    lexer(const lexer &) = default;
    lexer &operator=(const lexer &) = default;
    ~lexer() = default;

    WJR_NODISCARD bool next(uint32_t &);

private:
    bool read_token();

    basic_lexer lex;
};

} // namespace wjr::json

#endif // WJR_JSON_LEXER_IMPL_HPP__