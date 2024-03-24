#ifndef WJR_X86_MATH_CONVERT_HPP__
#define WJR_X86_MATH_CONVERT_HPP__

#include <wjr/math/convert-impl.hpp>
#include <wjr/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_FROM_CHARS_UNROLL_8_FAST WJR_HAS_DEF
#define WJR_HAS_BUILTIN_FROM_CHARS_UNROLL_16_FAST WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)

template <uint64_t Base>
uint32_t builtin_from_chars_unroll_8_fast(__m128i in) {
    uint64_t Base2 = Base * Base;
    uint64_t Base4 = Base2 * Base2;

    __m128i mul1 = sse::setr_epi8(Base, 1, Base, 1, Base, 1, Base, 1, Base, 1, Base, 1,
                                  Base, 1, Base, 1);
    __m128i mul2 = sse::setr_epi16(Base2, 1, Base2, 1, Base2, 1, Base2, 1);
    __m128i mul3 = sse::setr_epi16(Base4, 1, Base4, 1, Base4, 1, Base4, 1);

    __m128i t1 = _mm_maddubs_epi16(in, mul1);
    __m128i t2 = _mm_madd_epi16(t1, mul2);
    __m128i t3 = _mm_packus_epi32(t2, t2);
    __m128i t4 = _mm_madd_epi16(t3, mul3);

    return simd_cast<__m128i_t, uint32_t>(t4);
}

template <uint64_t Base>
uint32_t builtin_from_chars_unroll_8_fast(const void *ptr, char_converter_t) {
    __m128i assci = sse::set1_epi8(0x30);
    __m128i in = _mm_sub_epi8(sse::loadu_si64(ptr), assci);
    return builtin_from_chars_unroll_8_fast<Base>(in);
}

template <uint64_t Base>
uint32_t builtin_from_chars_unroll_8_fast(const void *ptr, origin_converter_t) {
    return builtin_from_chars_unroll_8_fast<Base>(sse::loadu_si64(ptr));
}

#endif

#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_16_FAST)

template <uint64_t Base>
uint64_t builtin_from_chars_unroll_16_fast(__m128i in) {
    uint64_t Base2 = Base * Base;
    uint64_t Base4 = Base2 * Base2;
    uint64_t Base8 = Base4 * Base4;

    __m128i mul1 = sse::setr_epi8(Base, 1, Base, 1, Base, 1, Base, 1, Base, 1, Base, 1,
                                  Base, 1, Base, 1);
    __m128i mul2 = sse::setr_epi16(Base2, 1, Base2, 1, Base2, 1, Base2, 1);
    __m128i mul3 = sse::setr_epi16(Base4, 1, Base4, 1, Base4, 1, Base4, 1);

    __m128i t1 = _mm_maddubs_epi16(in, mul1);
    __m128i t2 = _mm_madd_epi16(t1, mul2);
    __m128i t3 = _mm_packus_epi32(t2, t2);
    __m128i t4 = _mm_madd_epi16(t3, mul3);

    uint64_t val = simd_cast<__m128i_t, uint64_t>(t4);
    uint32_t lo = val;
    uint32_t hi = val >> 32;

    return lo * Base8 + hi;
}

template <uint64_t Base>
uint64_t builtin_from_chars_unroll_16_fast(const void *ptr, char_converter_t) {
    __m128i assci = sse::set1_epi8(0x30);
    __m128i in = _mm_sub_epi8(sse::loadu((__m128i *)(ptr)), assci);
    return builtin_from_chars_unroll_16_fast<Base>(in);
}

template <uint64_t Base>
uint64_t builtin_from_chars_unroll_16_fast(const void *ptr, origin_converter_t) {
    return builtin_from_chars_unroll_16_fast<Base>(sse::loadu((__m128i *)(ptr)));
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_CONVERT_HPP__