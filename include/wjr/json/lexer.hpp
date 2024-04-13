#ifndef WJR_JSON_LEXER_HPP__
#define WJR_JSON_LEXER_HPP__

#include <wjr/json/lexer-impl.hpp>
#include <wjr/span.hpp>

#if defined(WJR_X86)
#include <wjr/x86/json/lexer.hpp>
#endif

namespace wjr {

uint32_t lexer::next() {
    if (WJR_LIKELY(lex.token_first != lex.token_last)) {
        return *lex.token_first++;
    }

    if (WJR_UNLIKELY(!read_token())) {
        return npos;
    }

    return *lex.token_first++;
}

bool lexer::next(uint32_t &value) {
    value = next();
    return value != npos;
}

inline void fallback_read_token_mask(basic_lexer &lex) { (void)lex; }

inline void read_token_mask(basic_lexer &lex) {
#if !WJR_HAS_BUILTIN(LEXER_READ_TOKEN_MASK)
    fallback_read_token_mask(lex);
#else
    lexer_details::builtin_read_token_mask(lex);
#endif
}

bool lexer::read_token() {
    if (WJR_UNLIKELY(lex.first == lex.last)) {
        return false;
    }

    lex.token_first = lex.token_last = lex.token_buf;

    do {
        read_token_mask(lex);
    } while (WJR_LIKELY(lex.first != lex.last &&
                        std::distance(lex.token_buf, lex.token_last) < 64));

    return lex.token_first != lex.token_last;
}

} // namespace wjr

#endif // WJR_JSON_LEXER_HPP__