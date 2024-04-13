#ifndef WJR_JSON_LEXER_IMPL_HPP__
#define WJR_JSON_LEXER_IMPL_HPP__

#include <cstdint>

#include <wjr/span.hpp>

namespace wjr {

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
    alignas(16) uint32_t token_buf[127 + 7];
};

class lexer {
public:
    static constexpr uint32_t npos = -1;

    lexer(span<const char> sp) : lex(sp.begin(), sp.end()) {}

    lexer() = delete;
    lexer(const lexer &) = default;
    lexer &operator=(const lexer &) = default;
    ~lexer() = default;

    uint32_t next();
    bool next(uint32_t &);

private:
    WJR_NOINLINE bool read_token();

    basic_lexer lex;
};

} // namespace wjr

#endif // WJR_JSON_LEXER_IMPL_HPP__