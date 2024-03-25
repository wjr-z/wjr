#ifndef WJR_X86_MATH_CONVERT_HPP__
#define WJR_X86_MATH_CONVERT_HPP__

#include <wjr/math/convert-impl.hpp>
#include <wjr/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1) && WJR_HAS_SIMD(SIMD)
#define WJR_HAS_BUILTIN_TO_CHARS_UNROLL_4_FAST WJR_HAS_DEF
#define WJR_HAS_BUILTIN_TO_CHARS_UNROLL_8_FAST WJR_HAS_DEF

#define WJR_HAS_BUILTIN_FROM_CHARS_UNROLL_8_FAST WJR_HAS_DEF
#define WJR_HAS_BUILTIN_FROM_CHARS_UNROLL_16_FAST WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(TO_CHARS_UNROLL_4_FAST)

template <uint64_t Base>
uint32_t builtin_to_chars_unroll_4_fast(uint16_t in) {
    static_assert(Base == 10, "");

    __m128i mulp2 = sse::set1_epi16(5243);
    __m128i mulp2x = sse::set1_epi16(100);
    __m128i mulp1 = sse::set1_epi16((short)52429u);
    __m128i mulp1x = sse::set1_epi16(10);
    __m128i shuf = sse::setr_epi8(0, 8, 4, 12, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);

    __m128i x = simd_cast<uint32_t, __m128i_t>(in);

    // x[0: 15] = (x[0: 15] * 5243) >> 19;
    // x[64: 79] = (x[64: 79] * 5243) >> 19;
    __m128i q = _mm_mulhi_epu16(x, mulp2);
    q = _mm_srli_epi16(q, 3);

    __m128i r = _mm_sub_epi16(x, _mm_mullo_epi16(q, mulp2x));

    x = _mm_packus_epi16(q, r);

    // x[0: 15] = (x[0: 15] * 52429) >> 19;
    // x[32 : 47] = (x[32 : 47] * 52429) >> 19;
    // x[64 : 79] = (x[64 : 79] * 52429) >> 19;
    // x[96 : 111] = (x[96 : 111] * 52429) >> 19;

    q = _mm_mulhi_epu16(x, mulp1);
    q = _mm_srli_epi16(q, 3);

    r = _mm_sub_epi16(x, _mm_mullo_epi16(q, mulp1x));

    x = _mm_packus_epi16(q, r);
    return simd_cast<__m128i_t, uint32_t>(sse::shuffle_epi8(x, shuf));
}

template <uint64_t Base>
void builtin_to_chars_unroll_4_fast(void *ptr, uint32_t in, char_converter_t) {
    uint32_t x = builtin_to_chars_unroll_4_fast<Base>(in) + 0x30303030ull;

    write_memory<uint32_t>(ptr, x);
}

template <uint64_t Base>
void builtin_to_chars_unroll_4_fast(void *ptr, uint32_t in, origin_converter_t) {
    uint32_t x = builtin_to_chars_unroll_4_fast<Base>(in);

    write_memory<uint32_t>(ptr, x);
}

#endif

#if WJR_HAS_BUILTIN(TO_CHARS_UNROLL_8_FAST)

template <uint64_t Base>
uint64_t builtin_to_chars_unroll_8_fast(uint32_t in) {
    static_assert(Base == 10, "");

    __m128i mulp4 = simd_cast<uint32_t, __m128i_t>(3518437209);
    __m128i mulp4x = simd_cast<uint32_t, __m128i_t>(10000);
    __m128i mulp2 = sse::set1_epi16(5243);
    __m128i mulp2x = sse::set1_epi16(100);
    __m128i mulp1 = sse::set1_epi16((short)52429u);
    __m128i mulp1x = sse::set1_epi16(10);
    __m128i shuf = sse::setr_epi8(0, 8, 4, 12, 2, 10, 6, 14, 1, 1, 1, 1, 1, 1, 1, 1);

    __m128i x = simd_cast<uint32_t, __m128i_t>(in);

    // x[0: 15] = (x[0: 31] * 3518437209) >> 45;
    // x[16 : 127] = x[32 : 127]
    __m128i q = _mm_mul_epu32(x, mulp4);
    q = _mm_srli_epi64(q, 45);

    __m128i r = _mm_sub_epi32(x, _mm_mul_epu32(q, mulp4x));

    x = _mm_packus_epi32(q, r);

    // x[0: 15] = (x[0: 15] * 5243) >> 19;
    // x[64: 79] = (x[64: 79] * 5243) >> 19;
    q = _mm_mulhi_epu16(x, mulp2);
    q = _mm_srli_epi16(q, 3);

    r = _mm_sub_epi16(x, _mm_mullo_epi16(q, mulp2x));

    x = _mm_packus_epi16(q, r);

    // x[0: 15] = (x[0: 15] * 52429) >> 19;
    // x[32 : 47] = (x[32 : 47] * 52429) >> 19;
    // x[64 : 79] = (x[64 : 79] * 52429) >> 19;
    // x[96 : 111] = (x[96 : 111] * 52429) >> 19;

    q = _mm_mulhi_epu16(x, mulp1);
    q = _mm_srli_epi16(q, 3);

    r = _mm_sub_epi16(x, _mm_mullo_epi16(q, mulp1x));

    x = _mm_packus_epi16(q, r);

    return simd_cast<__m128i_t, uint64_t>(sse::shuffle_epi8(x, shuf));
}

template <uint64_t Base>
void builtin_to_chars_unroll_8_fast(void *ptr, uint32_t in, char_converter_t) {
    uint64_t x = builtin_to_chars_unroll_8_fast<Base>(in) + 0x3030303030303030ull;

    write_memory<uint64_t>(ptr, x);
}

template <uint64_t Base>
void builtin_to_chars_unroll_8_fast(void *ptr, uint32_t in, origin_converter_t) {
    uint64_t x = builtin_to_chars_unroll_8_fast<Base>(in);

    write_memory<uint64_t>(ptr, x);
}

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