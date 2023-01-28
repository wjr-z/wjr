#pragma once
#ifndef __WJR_SIMD_SIMD_CAST_H__
#define __WJR_SIMD_SIMD_CAST_H__

#include <wjr/macro.h>

_WJR_SIMD_BEGIN

template<typename T, typename U>
WJR_INTRINSIC_INLINE static T simd_cast(U);

#if defined(__SSE__)
#endif // __SSE__

#if defined(__SSE2__)

template<>
WJR_INTRINSIC_INLINE static __m128 simd_cast<__m128>(__m128i v) {
	return _mm_castsi128_ps(v);
}
template<>
WJR_INTRINSIC_INLINE static __m128i simd_cast<__m128i>(__m128 v) {
	return _mm_castps_si128(v);
}
template<>
WJR_INTRINSIC_INLINE static __m128d simd_cast<__m128d>(__m128 v) {
	return _mm_castps_pd(v);
}
template<>
WJR_INTRINSIC_INLINE static __m128 simd_cast<__m128>(__m128d v) {
	return _mm_castpd_ps(v);
}
template<>
WJR_INTRINSIC_INLINE static __m128i simd_cast<__m128i>(__m128d v) {
	return _mm_castpd_si128(v);
}
template<>
WJR_INTRINSIC_INLINE static __m128d simd_cast<__m128d>(__m128i v) {
	return _mm_castsi128_pd(v);
}

#define __WJR_REGISTER_SIMD_CAST_WITH_INTEGER(IT, SUF)	        \
template<>	                                                    \
WJR_INTRINSIC_INLINE static IT simd_cast<IT>(__m128i v){	    \
	return static_cast<IT>(_mm_cvtsi128_si##SUF(v));	        \
}	                                                            \
template<>	                                                    \
WJR_INTRINSIC_INLINE static __m128i simd_cast<__m128i>(IT v){	\
	return _mm_cvtsi##SUF##_si128(v);	                        \
}

__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int8_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int16_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int32_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int64_t, 64);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint8_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint16_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint32_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint64_t, 64);

#undef __WJR_REGISTER_SIMD_CAST_WITH_INTEGER

#endif // __SSE2__

#if defined(__AVX__)

template<>
WJR_INTRINSIC_INLINE static __m256 simd_cast<__m256>(__m256i v) {
	return _mm256_castsi256_ps(v);
}
template<>
WJR_INTRINSIC_INLINE static __m256i simd_cast<__m256i>(__m256 v) {
	return _mm256_castps_si256(v);
}
template<>
WJR_INTRINSIC_INLINE static __m256d simd_cast<__m256d>(__m256 v) {
	return _mm256_castps_pd(v);
}
template<>
WJR_INTRINSIC_INLINE static __m256 simd_cast<__m256>(__m256d v) {
	return _mm256_castpd_ps(v);
}
template<>
WJR_INTRINSIC_INLINE static __m256i simd_cast<__m256i>(__m256d v) {
	return _mm256_castpd_si256(v);
}
template<>
WJR_INTRINSIC_INLINE static __m256d simd_cast<__m256d>(__m256i v) {
	return _mm256_castsi256_pd(v);
}

template<>
WJR_INTRINSIC_INLINE static __m256i simd_cast<__m256i>(__m128i v) {
	return _mm256_castsi128_si256(v);
}

template<>
WJR_INTRINSIC_INLINE static __m128i simd_cast<__m128i>(__m256i v) {
	return _mm256_castsi256_si128(v);
}

#define __WJR_REGISTER_SIMD_CAST_WITH_INTEGER(IT, SUF)	            \
template<>	                                                        \
WJR_INTRINSIC_INLINE static IT simd_cast<IT>(__m256i v){	        \
	return simd_cast<IT>(simd_cast<__m128i>(v));					\
}	                                                                \
template<>	                                                        \
WJR_INTRINSIC_INLINE static __m256i simd_cast<__m256i>(IT v){	    \
	return simd_cast<__m256i>(simd_cast<__m128i>(v));				\
}

__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int8_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int16_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int32_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(int64_t, 64);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint8_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint16_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint32_t, 32);
__WJR_REGISTER_SIMD_CAST_WITH_INTEGER(uint64_t, 64);

#undef __WJR_REGISTER_SIMD_CAST_WITH_INTEGER

#endif // __AVX__

_WJR_SIMD_END

#endif // __WJR_SIMD_SIMD_CAST_H__