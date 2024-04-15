#ifndef WJR_X86_JSON_LEXER_HPP__
#define WJR_X86_JSON_LEXER_HPP__

#include <wjr/json/lexer-impl.hpp>
#include <wjr/x86/simd/simd.hpp>

namespace wjr::json {

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_JSON_READ_TOKEN_BUFFER WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(JSON_READ_TOKEN_BUFFER)
extern bool builtin_read_token_buffer(basic_lexer &lex) noexcept;
#endif

} // namespace wjr::json

#endif // WJR_X86_JSON_LEXER_HPP__