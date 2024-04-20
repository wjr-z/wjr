#ifndef WJR_X86_JSON_PARSER_HPP__
#define WJR_X86_JSON_PARSER_HPP__

#include <wjr/x86/simd/simd.hpp>

namespace wjr::json {

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(X86_SIMD)
#define WJR_HAS_BUILTIN_JSON_CHECK_8_CHARACTERS WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(JSON_CHECK_8_CHARACTERS)

// str[i] >= 32 && str[i] != 127 && str[i] != '\\'
WJR_INTRINSIC_INLINE bool check_8_characters(const char* p) {
    using simd = sse;
    using simd_int = typename simd::int_type;
    auto x = simd::loadu((simd_int*)p);
}

#endif 

} // namespace wjr

#endif // WJR_X86_JSON_PARSER_HPP__