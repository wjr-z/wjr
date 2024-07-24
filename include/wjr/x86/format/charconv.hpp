#ifndef WJR_X86_FORMAT_CHARCONV_HPP__
#define WJR_X86_FORMAT_CHARCONV_HPP__

#include <wjr/format/charconv-impl.hpp>
#include <wjr/x86/simd/simd.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_SIMD(SSE4_1)
#define WJR_HAS_BUILTIN_TO_CHARS_UNROLL_8_FAST WJR_HAS_DEF

#define WJR_HAS_BUILTIN_FROM_CHARS_UNROLL_8_FAST WJR_HAS_DEF
#define WJR_HAS_BUILTIN_FROM_CHARS_UNROLL_16_FAST WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(TO_CHARS_UNROLL_8_FAST)

namespace to_chars_detail {

const static __m128i mul10p4 = simd_cast<uint32_t, __m128i_t>(3518437209);
const static __m128i mul10p4x = simd_cast<uint32_t, __m128i_t>(10000);
const static __m128i mul10p2 = sse::set1_epi16(5243);
const static __m128i mul10p2x = sse::set1_epi16(100);
const static __m128i mul10p1 = sse::set1_epi16((short)52429u);
const static __m128i mul10p1x = sse::set1_epi16(10);

const static __m128i shuf =
    sse::setr_epi8(0, 8, 4, 12, 2, 10, 6, 14, 1, 1, 1, 1, 1, 1, 1, 1);

} // namespace to_chars_detail

#endif

#if WJR_HAS_BUILTIN(TO_CHARS_UNROLL_8_FAST)

inline uint64_t builtin_to_chars_unroll_8_fast_10(uint32_t in) noexcept {

    __m128i x = simd_cast<uint32_t, __m128i_t>(in);
    __m128i q, r;

    q = _mm_mul_epu32(x, to_chars_detail::mul10p4);
    q = _mm_srli_epi64(q, 45);

    r = _mm_sub_epi32(x, _mm_mul_epu32(q, to_chars_detail::mul10p4x));
    x = _mm_packus_epi32(q, r);

    q = _mm_mulhi_epu16(x, to_chars_detail::mul10p2);
    q = _mm_srli_epi16(q, 3);

    r = _mm_sub_epi16(x, _mm_mullo_epi16(q, to_chars_detail::mul10p2x));
    x = _mm_packus_epi16(q, r);

    q = _mm_mulhi_epu16(x, to_chars_detail::mul10p1);
    q = _mm_srli_epi16(q, 3);

    r = _mm_sub_epi16(x, _mm_mullo_epi16(q, to_chars_detail::mul10p1x));
    x = _mm_packus_epi16(q, r);

    return simd_cast<__m128i_t, uint64_t>(sse::shuffle_epi8(x, to_chars_detail::shuf));
}

template <uint64_t Base>
uint64_t builtin_to_chars_unroll_8_fast(uint32_t in) noexcept {
    if constexpr (Base == 10) {
        return builtin_to_chars_unroll_8_fast_10(in);
    } else {
        static_assert(Base == 10, "");
    }
}

template <uint64_t Base>
void builtin_to_chars_unroll_8_fast(void *ptr, uint32_t in, char_converter_t) noexcept {
    const uint64_t x = builtin_to_chars_unroll_8_fast<Base>(in) + 0x3030303030303030ull;
    write_memory<uint64_t>(ptr, x);
}

template <uint64_t Base>
void builtin_to_chars_unroll_8_fast(void *ptr, uint32_t in, origin_converter_t) noexcept {
    const uint64_t x = builtin_to_chars_unroll_8_fast<Base>(in);
    write_memory<uint64_t>(ptr, x);
}

#endif

#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST) ||                                         \
    WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_16_FAST)

namespace from_chars_detail {

/// @private
template <uint64_t Base>
inline constexpr uint64_t __base2 = Base * Base;

/// @private
template <uint64_t Base>
inline constexpr uint64_t __base4 = __base2<Base> * __base2<Base>;

/// @private
template <uint64_t Base>
inline constexpr uint64_t __base8 = __base4<Base> * __base4<Base>;

/// @private
template <uint64_t Base>
const static __m128i mulp1x = sse::setr_epi8(Base, 1, Base, 1, Base, 1, Base, 1, Base, 1,
                                             Base, 1, Base, 1, Base, 1);

/// @private
template <uint64_t Base>
const static __m128i mulp2x = sse::setr_epi16(__base2<Base>, 1, __base2<Base>, 1,
                                              __base2<Base>, 1, __base2<Base>, 1);

/// @private
template <uint64_t Base>
const static __m128i mulp4x = sse::setr_epi16(__base4<Base>, 1, __base4<Base>, 1,
                                              __base4<Base>, 1, __base4<Base>, 1);

/// @private
template <uint64_t Base>
const static __m128i baseu8 =
    sse::setr_epi8(Base, Base, Base, Base, Base, Base, Base, 0xff, 0xff, 0xff, 0xff, 0xff,
                   0xff, 0xff, 0xff, 0xff);

/// @private
static __m128i ascii = sse::set1_epi8(0x30);

} // namespace from_chars_detail

#endif

#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_8_FAST)

template <uint64_t Base>
uint32_t builtin_from_chars_unroll_8_fast(__m128i in) noexcept {
    const __m128i t1 = _mm_maddubs_epi16(in, from_chars_detail::mulp1x<Base>);
    const __m128i t2 = _mm_madd_epi16(t1, from_chars_detail::mulp2x<Base>);
    const __m128i t3 = _mm_packus_epi32(t2, t2);
    const __m128i t4 = _mm_madd_epi16(t3, from_chars_detail::mulp4x<Base>);

    return simd_cast<__m128i_t, uint32_t>(t4);
}

template <uint64_t Base>
uint32_t builtin_from_chars_unroll_8_fast(const void *ptr, char_converter_t) noexcept {
    static_assert(Base <= 10, "");
    const __m128i in = _mm_sub_epi8(sse::loadu_si64(ptr), from_chars_detail::ascii);
    return builtin_from_chars_unroll_8_fast<Base>(in);
}

template <uint64_t Base>
uint32_t builtin_from_chars_unroll_8_fast(const void *ptr, origin_converter_t) noexcept {
    static_assert(Base <= 10, "");
    const __m128i in = sse::loadu_si64(ptr);
    return builtin_from_chars_unroll_8_fast<Base>(in);
}

#endif

#if WJR_HAS_BUILTIN(FROM_CHARS_UNROLL_16_FAST)

template <uint64_t Base>
uint64_t builtin_from_chars_unroll_16_fast(__m128i in) noexcept {
    const __m128i t1 = _mm_maddubs_epi16(in, from_chars_detail::mulp1x<Base>);
    const __m128i t2 = _mm_madd_epi16(t1, from_chars_detail::mulp2x<Base>);
    const __m128i t3 = _mm_packus_epi32(t2, t2);
    const __m128i t4 = _mm_madd_epi16(t3, from_chars_detail::mulp4x<Base>);

    const uint64_t val = simd_cast<__m128i_t, uint64_t>(t4);
    const auto lo = static_cast<uint32_t>(val);
    const auto hi = static_cast<uint32_t>(val >> 32);

    return lo * from_chars_detail::__base8<Base> + hi;
}

template <uint64_t Base>
uint64_t builtin_from_chars_unroll_16_fast(const void *ptr, char_converter_t) noexcept {
    static_assert(Base <= 10, "");
    const __m128i in = _mm_sub_epi8(sse::loadu(ptr), from_chars_detail::ascii);
    return builtin_from_chars_unroll_16_fast<Base>(in);
}

template <uint64_t Base>
uint64_t builtin_from_chars_unroll_16_fast(const void *ptr, origin_converter_t) noexcept {
    static_assert(Base <= 10, "");
    const __m128i in = sse::loadu(ptr);
    return builtin_from_chars_unroll_16_fast<Base>(in);
}

#endif

} // namespace wjr

#endif // WJR_X86_FORMAT_CHARCONV_HPP__