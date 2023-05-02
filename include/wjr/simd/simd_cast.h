#pragma once
#ifndef __WJR_SIMD_SIMD_CAST_H
#define __WJR_SIMD_SIMD_CAST_H

#include <wjr/macro.h>

_WJR_SIMD_BEGIN

struct lo {};
struct hi {};

template<typename T, typename U>
struct simd_cast_fn;

template<typename T, typename U>
inline constexpr simd_cast_fn<T, U> simd_cast;

#define __REGISTER_BASIC_CAST(FROM, TO, func)											                    \
template<>	                                                                                                \
struct simd_cast_fn<TO##_tag, FROM##_tag>{	                                                                \
	TO operator()(FROM v) const{	                                                                        \
		return func(v);	                                                                                    \
	}	                                                                                                    \
};

#define __REGISTER_INTEGER1_CAST(FROM, TO, func1, func2)					                                \
template<>	                                                                                                \
struct simd_cast_fn<TO##_tag, FROM>{	                                                                    \
	TO operator()(FROM v) const{		                                                                    \
		return func1(v);	                                                                                \
	}	                                                                                                    \
};                                                                                                          \
template<>	                                                                                                \
struct simd_cast_fn<FROM, TO##_tag>{	                                                                    \
	FROM operator()(TO v) const{		                                                                    \
		return static_cast<FROM>(func2(v));	                                                                \
	}	                                                                                                    \
};              

template<typename T>
struct __simd_wrapper;

template<typename T>
using __simd_wrapper_t = typename __simd_wrapper<T>::type;

#if WJR_SSE

struct __m128_tag { using type = __m128; };

template<>
struct __simd_wrapper<__m128> {
	using type = __m128_tag;
};

#endif // WJR_SSE

#if WJR_SSE2

struct __m128i_tag { using type = __m128i; };
struct __m128d_tag { using type = __m128d; };

template<>
struct __simd_wrapper<__m128i> {
	using type = __m128i_tag;
};

template<>
struct __simd_wrapper<__m128d> {
	using type = __m128d_tag;
};

__REGISTER_BASIC_CAST(__m128, __m128i, _mm_castps_si128);
__REGISTER_BASIC_CAST(__m128, __m128d, _mm_castps_pd);
__REGISTER_BASIC_CAST(__m128i, __m128, _mm_castsi128_ps);
__REGISTER_BASIC_CAST(__m128i, __m128d, _mm_castsi128_pd);
__REGISTER_BASIC_CAST(__m128d, __m128, _mm_castpd_ps);
__REGISTER_BASIC_CAST(__m128d, __m128i, _mm_castpd_si128);

__REGISTER_INTEGER1_CAST(int8_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(int16_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(int32_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(int64_t, __m128i, _mm_cvtsi64_si128, _mm_cvtsi128_si64);

__REGISTER_INTEGER1_CAST(uint8_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(uint16_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(uint32_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(uint64_t, __m128i, _mm_cvtsi64_si128, _mm_cvtsi128_si64);

#endif // WJR_SSE2

#if WJR_AVX
struct __m256_tag { using type = __m256; };
struct __m256i_tag { using type = __m256i; };
struct __m256d_tag { using type = __m256d; };

template<>
struct __simd_wrapper<__m256> {
	using type = __m256_tag;
};

template<>
struct __simd_wrapper<__m256i> {
	using type = __m256i_tag;
};

template<>
struct __simd_wrapper<__m256d> {
	using type = __m256d_tag;
};

__REGISTER_BASIC_CAST(__m256, __m256i, _mm256_castps_si256);
__REGISTER_BASIC_CAST(__m256, __m256d, _mm256_castps_pd);
__REGISTER_BASIC_CAST(__m256i, __m256, _mm256_castsi256_ps);
__REGISTER_BASIC_CAST(__m256i, __m256d, _mm256_castsi256_pd);
__REGISTER_BASIC_CAST(__m256d, __m256, _mm256_castpd_ps);
__REGISTER_BASIC_CAST(__m256d, __m256i, _mm256_castpd_si256);
__REGISTER_BASIC_CAST(__m128i, __m256i, _mm256_castsi128_si256);
__REGISTER_BASIC_CAST(__m256i, __m128i, _mm256_castsi256_si128);

#define __AVX_CVT2_INT32(x) simd_cast<uint32_t, __m128i_tag>(simd_cast<__m128i_tag, __m256i_tag>(x))
#define __AVX_CVT2_INT64(x) simd_cast<uint64_t, __m128i_tag>(simd_cast<__m128i_tag, __m256i_tag>(x))

#define __INT32_CVT2_AVX(x) simd_cast<__m256i_tag, __m128i_tag>(simd_cast<__m128i_tag, uint32_t>(x))
#define __INT64_CVT2_AVX(x) simd_cast<__m256i_tag, __m128i_tag>(simd_cast<__m128i_tag, uint64_t>(x))

__REGISTER_INTEGER1_CAST(int8_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(int16_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(int32_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(int64_t, __m256i, __INT64_CVT2_AVX, __AVX_CVT2_INT64);

__REGISTER_INTEGER1_CAST(uint8_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(uint16_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(uint32_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(uint64_t, __m256i, __INT64_CVT2_AVX, __AVX_CVT2_INT64);

#undef __AVX_CVT2_INT32
#undef __AVX_CVT2_INT64

#undef __INT32_CVT2_AVX
#undef __INT64_CVT2_AVX

#endif // WJR_AVX

#undef __REGISTER_INTEGER2_CAST
#undef __REGISTER_INTEGER1_CAST
#undef __REGISTER_BASIC_CAST

_WJR_SIMD_END

#endif // __WJR_SIMD_SIMD_CAST_H