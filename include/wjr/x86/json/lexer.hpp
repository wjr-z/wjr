#ifndef WJR_X86_JSON_LEXER_HPP__
#define WJR_X86_JSON_LEXER_HPP__

#include <wjr/x86/simd/simd.hpp>

#if WJR_HAS_SIMD(SSSE3)
#define WJR_HAS_BUILTIN_JSON_LEXER_READER_READ_BUF WJR_HAS_DEF
#endif

#endif // WJR_X86_JSON_LEXER_HPP__