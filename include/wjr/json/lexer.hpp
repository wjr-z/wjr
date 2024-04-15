#ifndef WJR_JSON_LEXER_HPP__
#define WJR_JSON_LEXER_HPP__

#include <wjr/json/lexer-impl.hpp>
#include <wjr/span.hpp>

#if defined(WJR_X86)
#include <wjr/x86/json/lexer.hpp>
#endif

namespace wjr::json {

bool lexer::next(uint32_t &value) {
    if (WJR_LIKELY(lex.token_first != lex.token_last)) {
        value = *lex.token_first++;
        return true;
    }

    if (WJR_UNLIKELY(!read_token())) {
        return false;
    }

    value = *lex.token_first++;
    return true;
}

inline void fallback_read_token_buffer(basic_lexer &lex) { (void)lex; }

inline bool read_token_buffer(basic_lexer &lex) {
#if !WJR_HAS_BUILTIN(JSON_READ_TOKEN_BUFFER)
    fallback_read_token_buffer(lex);
#else
    return builtin_read_token_buffer(lex);
#endif
}

bool lexer::read_token() {
    if (WJR_UNLIKELY(lex.first == lex.last)) {
        return false;
    }

    lex.token_first = lex.token_last = lex.token_buf;
    return read_token_buffer(lex);
}

} // namespace wjr::json

#endif // WJR_JSON_LEXER_HPP__