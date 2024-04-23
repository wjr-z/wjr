#ifndef WJR_X86_FORMAT_UTF_8_HPP__
#define WJR_X86_FORMAT_UTF_8_HPP__

#include <wjr/x86/simd/simd.hpp>

namespace wjr::utf8 {

#if WJR_HAS_SIMD(SSE2) && WJR_HAS_SIMD(X86_SIMD)
#define WJR_HAS_BUILTIN_IS_ASCII_16 WJR_HAS_DEF

#if WJR_HAS_SIMD(AVX2)
#define WJR_HAS_BUILTIN_IS_ASCII_32 WJR_HAS_DEF
#endif

#endif

#if WJR_HAS_BUILTIN(IS_ASCII_16)

WJR_PURE bool builtin_is_ascii_16(uint8_t *ptr) {
    return sse::test_all_ones(
        sse::cmplt_epi8(sse::loadu((__m128i *)(ptr)), sse::set1_epi8(0x80)));
}

#endif

#if WJR_HAS_BUILTIN(IS_ASCII_32)

WJR_PURE bool builtin_is_ascii_32(uint8_t *ptr) {
    return avx::test_all_ones(
        avx::cmplt_epi8(avx::loadu((__m256i *)(ptr)), avx::set1_epi8(0x80)));
}

#endif

} // namespace wjr::utf8

#endif // WJR_X86_FORMAT_UTF_8_HPP__