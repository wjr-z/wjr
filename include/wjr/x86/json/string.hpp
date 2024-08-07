#ifndef WJR_X86_JSON_STRING_HPP__
#define WJR_X86_JSON_STRING_HPP__

#include <wjr/x86/simd/simd.hpp>

#if WJR_HAS_SIMD(SSE2)
#define WJR_HAS_BUILTIN_JSON_PARSE_STRING WJR_HAS_DEF
#endif

#endif // WJR_X86_JSON_STRING_HPP__