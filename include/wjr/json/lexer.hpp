#ifndef WJR_JSON_LEXER_HPP__
#define WJR_JSON_LEXER_HPP__

#include <wjr/json/lexer-impl.hpp>
#include <wjr/span.hpp>

#if defined(WJR_X86)
#include <wjr/x86/json/lexer.hpp>
#endif

namespace wjr::json {

template <uint32_t token_buf_size>
inline uint32_t basic_lexer_reader<token_buf_size>::read(uint32_t *token_buf) noexcept {
    if (WJR_UNLIKELY(m_storage.first == m_storage.last)) {
        return 0;
    }

    return read_buf(token_buf);
}

#if !WJR_HAS_BUILTIN(JSON_LEXER_READER_READ)
template <uint32_t token_buf_size>
uint32_t basic_lexer_reader<token_buf_size>::read_buf(uint32_t *token_buf) noexcept {
    return 0;
}
#endif

} // namespace wjr::json

#endif // WJR_JSON_LEXER_HPP__