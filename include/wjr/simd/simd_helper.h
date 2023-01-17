#pragma once
#ifndef __WJR_SIMD_SIMD_HELPER_H
#define __WJR_SIMD_SIMD_HELPER_H

#include <limits>
#include <wjr/type_traits.h>

_WJR_SIMD_BEGIN

enum class simd_type : unsigned int {
	SCALAR,
	SSE,
	AVX
};

constexpr simd_type best_simd_type =
#if defined(__AVX__)
simd_type::AVX;
#elif defined(__SSE__)
simd_type::SSE;
#else
simd_type::SCALAR;
#endif

struct simd_integer {};
struct simd_float {};

struct simd_pre_data {
};

template<typename T, simd_type S>
class traits;

template<typename T>
using sse_traits_t = traits<T, simd_type::SSE>;
template<typename T>
using avx_traits_t = traits<T, simd_type::AVX>;

template<typename T>
struct get_traits;

template<typename T>
using get_traits_t = typename get_traits<T>::type;

#if defined(__SSE__)
template<>
struct get_traits<__m128> {
	using type = traits<void, simd_type::SSE>;
};
#endif // __SSE__
#if defined(__SSE2__)
template<>
struct get_traits<__m128i> {
	using type = traits<simd_integer, simd_type::SSE>;
};
template<>
struct get_traits<__m128d> {
	using type = traits<simd_float, simd_type::SSE>;
};
#endif // __SSE2__
#if defined(__AVX__)
template<>
struct get_traits<__m256> {
	using type = traits<void, simd_type::AVX>;
};
template<>
struct get_traits<__m256i> {
	using type = traits<simd_integer, simd_type::AVX>;
};
template<>
struct get_traits<__m256d> {
	using type = traits<simd_float, simd_type::AVX>;
};
#endif // __AVX__

#define __REGISTER_AVX_FROM_SSE(name, __wjr_a, __wjr_b)				\
	auto __wjr_a0 = getlow_si128(__wjr_a);							\
	auto __wjr_b0 = getlow_si128(__wjr_b);							\
	auto __wjr_a1 = gethigh_si128(__wjr_a);							\
	auto __wjr_b1 = gethigh_si128(__wjr_b);							\
	auto __wjr_r0 = name(__wjr_a0, __wjr_b0);						\
	auto __wjr_r1 = name(__wjr_a1, __wjr_b1);						\
	return insert_si128<1>(simd_cast<__m256i>(__wjr_r0), __wjr_r1);

template<typename T>
struct __simd_cast;

template<typename T>
constexpr __simd_cast<T> simd_cast;

#define __REGISTER_INTEGER_CAST_SIMD_FUNCTION_128_HELPER(IT, SUF)	\
WJR_HAS_SSE2_DEFINE(	                                            \
	WJR_INTRINSIC_INLINE __m128i operator()(IT v) const{	        \
		return _mm_cvtsi##SUF##_si128(v);					        \
	}	                                                            \
)

#define __REGISTER_INTEGER_CAST_SIMD_FUNCTION_256_HELPER(IT, SUF)	\
WJR_HAS_AVX_DEFINE(	                                                \
	WJR_INTRINSIC_INLINE __m256i operator()(IT v) const{	        \
		return simd_cast<__m256i>(simd_cast<__m128i>(v));	        \
	}	                                                            \
)

#define __REGISTER_INTEGER_CAST_SIMD_FUNCTION_HELPER(HELPER)	    \
	HELPER(int8_t, 32)	                                            \
	HELPER(uint8_t, 32)	                                            \
	HELPER(int16_t, 32)	                                            \
	HELPER(uint16_t, 32)	                                        \
	HELPER(int32_t, 32)	                                            \
	HELPER(uint32_t, 32)	                                        \
	HELPER(int64_t, 64)	                                            \
	HELPER(uint64_t, 64)

#define __REGISTER_INTEGER_CAST_SIMD_FUNCTION_128	                \
__REGISTER_INTEGER_CAST_SIMD_FUNCTION_HELPER(__REGISTER_INTEGER_CAST_SIMD_FUNCTION_128_HELPER)

#define __REGISTER_INTEGER_CAST_SIMD_FUNCTION_256	                \
__REGISTER_INTEGER_CAST_SIMD_FUNCTION_HELPER(__REGISTER_INTEGER_CAST_SIMD_FUNCTION_256_HELPER)
	
#if defined(__SSE__)
template<>
struct __simd_cast<__m128> {
#if defined(__SSE2__)
	WJR_INTRINSIC_INLINE __m128 operator()(__m128i v)const {
		return _mm_castsi128_ps(v);
	}
	WJR_INTRINSIC_INLINE __m128 operator()(__m128d v)const {
		return _mm_castpd_ps(v);
	}
#endif // __SSE2__
};
#endif // __SSE__

#if defined(__SSE2__)
template<>
struct __simd_cast<__m128i> {
	WJR_INTRINSIC_INLINE __m128i operator()(__m128 v)const {
		return _mm_castps_si128(v);
	}
	WJR_INTRINSIC_INLINE __m128i operator()(__m128d v)const {
		return _mm_castpd_si128(v);
	}
#if defined(__AVX__)
	WJR_INTRINSIC_INLINE __m128i operator()(__m256i v)const {
		return _mm256_castsi256_si128(v);
	}
#endif // __AVX__
	__REGISTER_INTEGER_CAST_SIMD_FUNCTION_128
};

template<>
struct __simd_cast<__m128d> {
	WJR_INTRINSIC_INLINE __m128d operator()(__m128 v)const {
		return _mm_castps_pd(v);
	}
	WJR_INTRINSIC_INLINE __m128d operator()(__m128i v)const {
		return _mm_castsi128_pd(v);
	}
};
#endif // __SSE2__

#if defined(__AVX__)
template<>
struct __simd_cast<__m256i> {
	WJR_INTRINSIC_INLINE __m256i operator()(__m128i v)const {
		return _mm256_castsi128_si256(v);
	}
	__REGISTER_INTEGER_CAST_SIMD_FUNCTION_256
};
#endif // __AVX__

#define __REGISTER_SIMD_CAST_INTEGER_FUNCTION(VT,SUF)			\
WJR_HAS_SSE2_DEFINE(	                                        \
WJR_INTRINSIC_INLINE VT operator()(__m128i v) const {	        \
	return _mm_cvtsi128_si##SUF(v);	                            \
}	                                                            \
)	                                                            \
WJR_HAS_AVX_DEFINE(	                                            \
WJR_INTRINSIC_INLINE VT operator()(__m256i v) const {	        \
	return _mm256_cvtsi256_si##SUF(v);	                        \
}	                                                            \
)
template<>
struct __simd_cast<int8_t> {
	__REGISTER_SIMD_CAST_INTEGER_FUNCTION(int8_t, 32);
};

template<>
struct __simd_cast<uint8_t> {
	__REGISTER_SIMD_CAST_INTEGER_FUNCTION(uint8_t, 32);
};

template<>
struct __simd_cast<int16_t> {
	__REGISTER_SIMD_CAST_INTEGER_FUNCTION(int16_t, 32);
};

template<>
struct __simd_cast<uint16_t> {
	__REGISTER_SIMD_CAST_INTEGER_FUNCTION(uint16_t, 32);
};

template<>
struct __simd_cast<int32_t> {
	__REGISTER_SIMD_CAST_INTEGER_FUNCTION(int32_t, 32);
};

template<>
struct __simd_cast<uint32_t> {
	__REGISTER_SIMD_CAST_INTEGER_FUNCTION(uint32_t, 32);
};


template<>
struct __simd_cast<uint64_t> {
#if defined(__SSE2__)
	WJR_INTRINSIC_INLINE uint64_t operator()(__m128i v)const {
		return _mm_cvtsi128_si64(v);
	}
#endif // __SSE2__
#if defined(__AVX__)
	WJR_INTRINSIC_INLINE uint64_t operator()(__m256i v)const {
		return simd_cast<uint64_t>(simd_cast<__m128i>(v));
	}
#endif // __AVX__
};

template<>
struct __simd_cast<int64_t> {
#if defined(__SSE2__)
	WJR_INTRINSIC_INLINE int64_t operator()(__m128i v)const {
		return simd_cast<uint64_t>(v);
	}
#endif // __SSE2__
#if defined(__AVX__)
	WJR_INTRINSIC_INLINE int64_t operator()(__m256i v)const {
		return simd_cast<uint64_t>(v);
	}
#endif // __AVX__
};

#undef __REGISTER_SIMD_CAST_FUNCTION

#if defined(__MMX__)
#endif // __MMX__

#if defined(__SSE__)

template<>
class traits<void, simd_type::SSE> {
public:
	using simd_type = __m128;

	constexpr static int width() { return 128; }

	WJR_INTRINSIC_INLINE static simd_type add_ps(simd_type a, simd_type b) { return _mm_add_ps(a, b); }
	WJR_INTRINSIC_INLINE static simd_type add_ss(simd_type a, simd_type b) { return _mm_add_ss(a, b); }

	WJR_INTRINSIC_INLINE static simd_type and_ps(simd_type a, simd_type b) { return _mm_and_ps(a, b); }
	WJR_INTRINSIC_INLINE static simd_type andnot_ps(simd_type a, simd_type b) { return _mm_andnot_ps(a, b); }

	WJR_INTRINSIC_INLINE static simd_type cmpeq_ps(simd_type a, simd_type b) { return _mm_cmpeq_ps(a, b); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq_ss(simd_type a, simd_type b) { return _mm_cmpeq_ss(a, b); }

	WJR_INTRINSIC_INLINE static simd_type cmpge_ps(simd_type a, simd_type b) { return _mm_cmpge_ps(a, b); }
	WJR_INTRINSIC_INLINE static simd_type cmpge_ss(simd_type a, simd_type b) { return _mm_cmpge_ss(a, b); }
};

#endif // __SSE__

#if defined(__SSE2__)

template<>
class traits<simd_integer, simd_type::SSE> {
public:
	using simd_type = __m128i;

	constexpr static int width() { return 128; }

	WJR_INTRINSIC_INLINE static simd_type set_all_zeros() { return _mm_setzero_si128(); }
	WJR_INTRINSIC_INLINE static simd_type set_all_ones() { return _mm_set1_epi32(-1); }

	WJR_INTRINSIC_INLINE static simd_type set_epi8(
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8, 
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
		return _mm_set_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static simd_type set_epi16(
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) { 
		return _mm_set_epi16(e7, e6, e5, e4, e3, e2, e1, e0); 
	}
	WJR_INTRINSIC_INLINE static simd_type set_epi32(int e3, int e2, int e1, int e0) {
		return _mm_set_epi32(e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static simd_type set_epi64x(long long e1, long long e0) {
		return _mm_set_epi64x(e1, e0);
	}

	WJR_INTRINSIC_INLINE static simd_type setr_epi8(
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8, 
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
		return _mm_setr_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static simd_type setr_epi16(
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm_setr_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static simd_type setr_epi32(int e3, int e2, int e1, int e0) {
		return _mm_setr_epi32(e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static simd_type set1_epi8(int8_t val) { return _mm_set1_epi8(val); }
	WJR_INTRINSIC_INLINE static simd_type set1_epi16(int16_t val) { return _mm_set1_epi16(val); }
	WJR_INTRINSIC_INLINE static simd_type set1_epi32(int32_t val) { return _mm_set1_epi32(val); }
	WJR_INTRINSIC_INLINE static simd_type set1_epi64(int64_t val) { return _mm_set1_epi64x(val); }
	
	WJR_INTRINSIC_INLINE static simd_type set1(int8_t val, int8_t) { return set1_epi8(val); }
	WJR_INTRINSIC_INLINE static simd_type set1(int16_t val, int16_t) { return set1_epi16(val); }
	WJR_INTRINSIC_INLINE static simd_type set1(int32_t val, int32_t) { return set1_epi32(val); }
	WJR_INTRINSIC_INLINE static simd_type set1(int64_t val, int64_t) { return set1_epi64(val); }
	WJR_INTRINSIC_INLINE static simd_type set1(uint8_t val, uint8_t) { return set1_epi8(val); }
	WJR_INTRINSIC_INLINE static simd_type set1(uint16_t val, uint16_t) { return set1_epi16(val); }
	WJR_INTRINSIC_INLINE static simd_type set1(uint32_t val, uint32_t) { return set1_epi32(val); }
	WJR_INTRINSIC_INLINE static simd_type set1(uint64_t val, uint64_t) { return set1_epi64(val); }

	WJR_INTRINSIC_INLINE static simd_type setmin_epi8() { return set1_epi8(0x80u); }
	WJR_INTRINSIC_INLINE static simd_type setmin_epi16() { return set1_epi16(0x8000u); }
	WJR_INTRINSIC_INLINE static simd_type setmin_epi32() { return set1_epi32(0x80000000u); }

	WJR_INTRINSIC_INLINE static simd_type setmin(int8_t) { return setmin_epi8(); }
	WJR_INTRINSIC_INLINE static simd_type setmin(int16_t) { return setmin_epi16(); }
	WJR_INTRINSIC_INLINE static simd_type setmin(int32_t) { return setmin_epi32(); }
	WJR_INTRINSIC_INLINE static simd_type setmin(uint8_t) { return set1_epi32(0); }
	WJR_INTRINSIC_INLINE static simd_type setmin(uint16_t) { return set1_epi32(0); }
	WJR_INTRINSIC_INLINE static simd_type setmin(uint32_t) { return set1_epi32(0); }

	WJR_INTRINSIC_INLINE static simd_type setmax_epi8() { return set1_epi8(0x7F); }
	WJR_INTRINSIC_INLINE static simd_type setmax_epi16() { return set1_epi16(0x7FFF); }
	WJR_INTRINSIC_INLINE static simd_type setmax_epi32() { return set1_epi32(0x7FFFFFFF); }

	WJR_INTRINSIC_INLINE static simd_type setmax(int8_t) { return setmax_epi8(); }
	WJR_INTRINSIC_INLINE static simd_type setmax(int16_t) { return setmax_epi16(); }
	WJR_INTRINSIC_INLINE static simd_type setmax(int32_t) { return setmax_epi32(); }
	WJR_INTRINSIC_INLINE static simd_type setmax(uint8_t) { return set1_epi32(0xFFFFFFFF); }
	WJR_INTRINSIC_INLINE static simd_type setmax(uint16_t) { return set1_epi32(0xFFFFFFFF); }
	WJR_INTRINSIC_INLINE static simd_type setmax(uint32_t) { return set1_epi32(0xFFFFFFFF); }

	WJR_INTRINSIC_INLINE static simd_type load_all(const simd_type* ptr) { return _mm_load_si128(ptr); }
	WJR_INTRINSIC_INLINE static simd_type load(const simd_type* ptr) { return load_all(ptr); }
	WJR_INTRINSIC_INLINE static simd_type loadu_all(const simd_type* ptr) { return _mm_loadu_si128(ptr); }
	WJR_INTRINSIC_INLINE static simd_type loadu(const simd_type* ptr) { return loadu_all(ptr); }
	WJR_INTRINSIC_INLINE static simd_type loadu_si16(const void* ptr) { return _mm_loadu_si16(ptr); }
	WJR_INTRINSIC_INLINE static simd_type loadu_si32(const void* ptr) { return _mm_loadu_si32(ptr); }
	WJR_INTRINSIC_INLINE static simd_type loadu_si64(const void* ptr) { return _mm_loadu_si64(ptr); }
#if defined(__SSE3__)
	WJR_INTRINSIC_INLINE static simd_type lddqu_all(const simd_type* ptr) { return _mm_lddqu_si128(ptr); }
	WJR_INTRINSIC_INLINE static simd_type lddqu(const simd_type* ptr) { return lddqu_all(ptr); }
#endif // __SSE3__

	WJR_INTRINSIC_INLINE static void store_all(simd_type* ptr, simd_type val) { _mm_store_si128(ptr, val); }
	WJR_INTRINSIC_INLINE static void store(simd_type* ptr, simd_type val) { store_all(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu_all(simd_type* ptr, simd_type val) { _mm_storeu_si128(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu(simd_type* ptr, simd_type val) { storeu_all(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu_si16(void* ptr, simd_type val) { _mm_storeu_si16(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu_si32(void* ptr, simd_type val) { _mm_storeu_si32(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu_si64(void* ptr, simd_type val) { _mm_storeu_si64(ptr, val); }

	WJR_INTRINSIC_INLINE static simd_type And_all(simd_type lhs, simd_type rhs) { return _mm_and_si128(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type And(simd_type lhs, simd_type rhs) { return And_all(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type AndNot_all(simd_type lhs, simd_type rhs) { return _mm_andnot_si128(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type AndNot(simd_type lhs, simd_type rhs) { return AndNot_all(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type Or_all(simd_type lhs, simd_type rhs) { return _mm_or_si128(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type Or(simd_type lhs, simd_type rhs) { return Or_all(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type Xor_all(simd_type lhs, simd_type rhs) { return _mm_xor_si128(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type Xor(simd_type lhs, simd_type rhs) { return Xor_all(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type Not(simd_type v) { return Xor(v, set_all_ones()); }

#if defined(__SSSE3__)
	WJR_INTRINSIC_INLINE static simd_type abs_epi8(simd_type val) { return _mm_abs_epi8(val); }
	WJR_INTRINSIC_INLINE static simd_type abs_epi16(simd_type val) { return _mm_abs_epi16(val); }
	WJR_INTRINSIC_INLINE static simd_type abs_epi32(simd_type val) { return _mm_abs_epi32(val); }

	WJR_INTRINSIC_INLINE static simd_type abs(simd_type val, int8_t) { return abs_epi8(val); }
	WJR_INTRINSIC_INLINE static simd_type abs(simd_type val, int16_t) { return abs_epi16(val); }
	WJR_INTRINSIC_INLINE static simd_type abs(simd_type val, int32_t) { return abs_epi32(val); }
	WJR_INTRINSIC_INLINE static simd_type abs(simd_type val, uint8_t) { return val; }
	WJR_INTRINSIC_INLINE static simd_type abs(simd_type val, uint16_t) { return val; }
	WJR_INTRINSIC_INLINE static simd_type abs(simd_type val, uint32_t) { return val; }
#endif // __SSSE3__

	WJR_INTRINSIC_INLINE static simd_type add_epi8(simd_type lhs, simd_type rhs) { return _mm_add_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add_epi16(simd_type lhs, simd_type rhs) { return _mm_add_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add_epi32(simd_type lhs, simd_type rhs) { return _mm_add_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add_epi64(simd_type lhs, simd_type rhs) { return _mm_add_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, int8_t) { return add_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, int16_t) { return add_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, int32_t) { return add_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, int64_t) { return add_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint8_t) { return add_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint16_t) { return add_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint32_t) { return add_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint64_t) { return add_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static uint8_t add_epu8(simd_type a) {
		auto b = shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a);
		a = add_epi8(a, b);
		b = set_all_zeros();
		a = sad_epu8(a, b);
		return simd_cast<uint8_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint16_t add_epu16(simd_type a) {
		a = add_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = add_epi16(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a));
		a = add_epi16(a, srli_all<2>(a));
		return simd_cast<uint16_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint32_t add_epu32(simd_type a) {
		a = add_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = add_epi32(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a));
		return simd_cast<uint32_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint64_t add_epu64(simd_type a) {
		a = add_epi64(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		return simd_cast<uint64_t>(a);
	}
	WJR_INTRINSIC_INLINE static int8_t add_epi8(simd_type a) { return add_epu8(a); }
	WJR_INTRINSIC_INLINE static int16_t add_epi16(simd_type a) { return add_epu16(a); }
	WJR_INTRINSIC_INLINE static int32_t add_epi32(simd_type a) { return add_epu32(a); }
	WJR_INTRINSIC_INLINE static int64_t add_epi64(simd_type a) { return add_epu64(a); }

	WJR_INTRINSIC_INLINE static int8_t add(simd_type a, int8_t) { return add_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t add(simd_type a, int16_t) { return add_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t add(simd_type a, int32_t) { return add_epi32(a); }
	WJR_INTRINSIC_INLINE static int64_t add(simd_type a, int64_t) { return add_epi64(a); }
	WJR_INTRINSIC_INLINE static uint8_t add(simd_type a, uint8_t) { return add_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t add(simd_type a, uint16_t) { return add_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t add(simd_type a, uint32_t) { return add_epu32(a); }
	WJR_INTRINSIC_INLINE static uint64_t add(simd_type a, uint64_t) { return add_epu64(a); }

	WJR_INTRINSIC_INLINE static simd_type adds_epi8(simd_type lhs, simd_type rhs) { return _mm_adds_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type adds_epi16(simd_type lhs, simd_type rhs) { return _mm_adds_epi16(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type adds_epu8(simd_type lhs, simd_type rhs) { return _mm_adds_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type adds_epu16(simd_type lhs, simd_type rhs) { return _mm_adds_epu16(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type adds(simd_type lhs, simd_type rhs, int8_t) { return adds_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type adds(simd_type lhs, simd_type rhs, int16_t) { return adds_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type adds(simd_type lhs, simd_type rhs, uint8_t) { return adds_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type adds(simd_type lhs, simd_type rhs, uint16_t) { return adds_epu16(lhs, rhs); }

#if defined(__SSSE3__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type alignr_epi8(simd_type lhs, simd_type rhs) { 
		return _mm_alignr_epi8(lhs, rhs, imm8); 
	}
#endif // __SSSE3__

	WJR_INTRINSIC_INLINE static simd_type avg_epu8(simd_type lhs, simd_type rhs) { return _mm_avg_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type avg_epu16(simd_type lhs, simd_type rhs) { return _mm_avg_epu16(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type avg(simd_type lhs, simd_type rhs, int8_t) { return avg_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type avg(simd_type lhs, simd_type rhs, int16_t) { return avg_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type avg(simd_type lhs, simd_type rhs, uint8_t) { return avg_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type avg(simd_type lhs, simd_type rhs, uint16_t) { return avg_epu16(lhs, rhs); }

#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type blend_epi16(simd_type lhs, simd_type rhs) { 
		return _mm_blend_epi16(lhs, rhs, imm8); 
	}

	WJR_INTRINSIC_INLINE static simd_type blendv_epi8(simd_type lhs, simd_type rhs, simd_type mask) {
		return _mm_blendv_epi8(lhs, rhs, mask);
	}
#else
	WJR_INTRINSIC_INLINE static simd_type blendv_epi8(simd_type lhs, simd_type rhs, simd_type mask) {
		return Or(AndNot(mask, lhs), And(mask, rhs));
	}
#endif // __SSE4_1__

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type bslli_epi128(simd_type val) { return _mm_bslli_si128(val, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type bsrli_epi128(simd_type val) { return _mm_bsrli_si128(val, imm8); }

	WJR_INTRINSIC_INLINE static simd_type cmpeq_epi8(simd_type lhs, simd_type rhs) { return _mm_cmpeq_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq_epi16(simd_type lhs, simd_type rhs) { return _mm_cmpeq_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq_epi32(simd_type lhs, simd_type rhs) { return _mm_cmpeq_epi32(lhs, rhs); }
#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static simd_type cmpeq_epi64(simd_type lhs, simd_type rhs) { return _mm_cmpeq_epi64(lhs, rhs); }
#endif // __SSE4_1__

	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int8_t) { return cmpeq_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int16_t) { return cmpeq_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int32_t) { return cmpeq_epi32(lhs, rhs); }
#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int64_t) { return cmpeq_epi64(lhs, rhs); }
#endif // __SSE4_1__
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint8_t) { return cmpeq_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint16_t) { return cmpeq_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint32_t) { return cmpeq_epi32(lhs, rhs); }
#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint64_t) { return cmpeq_epi64(lhs, rhs); }
#endif // __SSE4_1__

	WJR_INTRINSIC_INLINE static simd_type cmpge_epi8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi8(rhs, lhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmpge_epi16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi16(rhs, lhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmpge_epi32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi32(rhs, lhs));
	}

	WJR_INTRINSIC_INLINE static simd_type cmpge_epu8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu8(rhs, lhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmpge_epu16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu16(rhs, lhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmpge_epu32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu32(rhs, lhs));
	}

	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, int8_t) { return cmpge_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, int16_t) { return cmpge_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, int32_t) { return cmpge_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, uint8_t) { return cmpge_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, uint16_t) { return cmpge_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, uint32_t) { return cmpge_epu32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmpgt_epi8(simd_type lhs, simd_type rhs) { return _mm_cmpgt_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epi16(simd_type lhs, simd_type rhs) { return _mm_cmpgt_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epi32(simd_type lhs, simd_type rhs) { return _mm_cmpgt_epi32(lhs, rhs); }
#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epi64(simd_type lhs, simd_type rhs) { return _mm_cmpgt_epi64(lhs, rhs); }
#endif // __SSE4_1__

	WJR_INTRINSIC_INLINE static simd_type cmpgt_epu8(simd_type lhs, simd_type rhs) {
#if defined(__XOP__)
		return _mm_comgt_epu8(lhs, rhs);
#else
		return cmpgt_epi8(Xor(lhs, setmin_epi8()), Xor(rhs, setmin_epi8()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epu16(simd_type lhs, simd_type rhs) {
#if defined(__XOP__)
		return _mm_comgt_epu16(lhs, rhs);
#else
		return cmpgt_epi16(Xor(lhs, setmin_epi16()), Xor(rhs, setmin_epi16()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epu32(simd_type lhs, simd_type rhs) {
#if defined(__XOP__)
		return _mm_comgt_epu32(lhs, rhs);
#else
		return cmpgt_epi32(Xor(lhs, setmin_epi32()), Xor(rhs, setmin_epi32()));
#endif // __XOP__
	}

	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int8_t) { return cmpgt_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int16_t) { return cmpgt_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int32_t) { return cmpgt_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint8_t) { return cmpgt_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint16_t) { return cmpgt_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint32_t) { return cmpgt_epu32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmple_epi8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi8(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmple_epi16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi16(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmple_epi32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi32(lhs, rhs));
	}

	WJR_INTRINSIC_INLINE static simd_type cmple_epu8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu8(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmple_epu16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu16(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmple_epu32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu32(lhs, rhs));
	}

	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, int8_t) { return cmple_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, int16_t) { return cmple_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, int32_t) { return cmple_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, uint8_t) { return cmple_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, uint16_t) { return cmple_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, uint32_t) { return cmple_epu32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmplt_epi8(simd_type lhs, simd_type rhs) { return _mm_cmplt_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt_epi16(simd_type lhs, simd_type rhs) { return _mm_cmplt_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt_epi32(simd_type lhs, simd_type rhs) { return _mm_cmplt_epi32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmplt_epu8(simd_type lhs, simd_type rhs) {
		return cmpgt_epu8(rhs, lhs);
	}
	WJR_INTRINSIC_INLINE static simd_type cmplt_epu16(simd_type lhs, simd_type rhs) {
		return cmpgt_epu16(rhs, lhs);
	}
	WJR_INTRINSIC_INLINE static simd_type cmplt_epu32(simd_type lhs, simd_type rhs) {
		return cmpgt_epu32(rhs, lhs);
	}

	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int8_t) { return cmplt_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int16_t) { return cmplt_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int32_t) { return cmplt_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint8_t) { return cmplt_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint16_t) { return cmplt_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint32_t) { return cmplt_epu32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmpne_epi8(simd_type lhs, simd_type rhs) { return Not(cmpeq_epi8(lhs, rhs)); }
	WJR_INTRINSIC_INLINE static simd_type cmpne_epi16(simd_type lhs, simd_type rhs) { return Not(cmpeq_epi16(lhs, rhs)); }
	WJR_INTRINSIC_INLINE static simd_type cmpne_epi32(simd_type lhs, simd_type rhs) { return Not(cmpeq_epi32(lhs, rhs)); }

	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, int8_t) { return cmpne_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, int16_t) { return cmpne_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, int32_t) { return cmpne_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, uint8_t) { return cmpne_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, uint16_t) { return cmpne_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, uint32_t) { return cmpne_epi32(lhs, rhs); }

	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::equal_to<>, T) {
		return cmpeq(lhs, rhs, T{});
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::not_equal_to<>, T) {
		return cmpne(lhs, rhs, T{});
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::greater<>, T) {
		return cmpgt(lhs, rhs, T{});
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::greater_equal<>, T) {
		return cmpge(lhs, rhs, T{});
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::less<>, T) {
		return cmplt(lhs, rhs, T{});
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::less_equal<>, T) {
		return cmple(lhs, rhs, T{});
	}

#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi8(simd_type lhs) {
		return _mm_extract_epi8(lhs, imm8);
	}
#endif // __SSE4_1__
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi16(simd_type lhs) {
		return _mm_extract_epi16(lhs, imm8);
	}
#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi32(simd_type lhs) {
		return _mm_extract_epi32(lhs, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract_epi64(simd_type lhs) {
		return _mm_extract_epi64(lhs, imm8);
	}
#endif // __SSE4_1__

#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(simd_type lhs, int8_t) { return extract_epi8<imm8>(lhs); }
#endif // __SSE4_1__
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(simd_type lhs, int16_t) { return extract_epi16<imm8>(lhs); }
#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(simd_type lhs, int32_t) { return extract_epi32<imm8>(lhs); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(simd_type lhs, int64_t) { return extract_epi64<imm8>(lhs); }
#endif // __SSE4_1__
#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(simd_type lhs, uint8_t) { return extract_epi8<imm8>(lhs); }
#endif // __SSE4_1__
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(simd_type lhs, uint16_t) { return extract_epi16<imm8>(lhs); }
#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(simd_type lhs, uint32_t) { return extract_epi32<imm8>(lhs); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(simd_type lhs, uint64_t) { return extract_epi64<imm8>(lhs); }
#endif // __SSE4_1__

#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert_epi8(simd_type lhs, int i) {
		return _mm_insert_epi8(lhs, i, imm8);
	}
#endif // __SSE4_1__
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert_epi16(simd_type lhs, int i) {
		return _mm_insert_epi16(lhs, i, imm8);
	}
#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert_epi32(simd_type lhs, int i) {
		return _mm_insert_epi32(lhs, i, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert_epi64(simd_type lhs, int64_t i) {
		return _mm_insert_epi64(lhs, i, imm8);
	}
#endif // __SSE4_1__

#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert(simd_type lhs, int i, int8_t) { return insert_epi8<imm8>(lhs, i); }
#endif // __SSE4_1__
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert(simd_type lhs, int i, int16_t) { return insert_epi16<imm8>(lhs, i); }
#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert(simd_type lhs, int i, int32_t) { return insert_epi32<imm8>(lhs, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert(simd_type lhs, int64_t i, int64_t) { return insert_epi64<imm8>(lhs, i); }
#endif // __SSE4_1__
#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert(simd_type lhs, int i, uint8_t) { return insert_epi8<imm8>(lhs, i); }
#endif // __SSE4_1__
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert(simd_type lhs, int i, uint16_t) { return insert_epi16<imm8>(lhs, i); }
#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert(simd_type lhs, int i, uint32_t) { return insert_epi32<imm8>(lhs, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert(simd_type lhs, int64_t i, uint64_t) { return insert_epi64<imm8>(lhs, i); }
#endif // __SSE4_1__

	WJR_INTRINSIC_INLINE static simd_type madd_epi16(simd_type lhs, simd_type rhs) {
		return _mm_madd_epi16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static void maskmoveu_all(simd_type lhs, simd_type mask, char* mem_addr) {
		return _mm_maskmoveu_si128(lhs, mask, mem_addr);
	}
	WJR_INTRINSIC_INLINE static void maskmoveu(simd_type lhs, simd_type mask, char* mem_addr) {
		return maskmoveu_all(lhs, mask, mem_addr);
	}

	WJR_INTRINSIC_INLINE static simd_type max_epi16(simd_type lhs, simd_type rhs) {
		return _mm_max_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type max_epu8(simd_type lhs, simd_type rhs) {
		return _mm_max_epu8(lhs, rhs);
	}
#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static simd_type max_epi8(simd_type lhs, simd_type rhs) {
		return _mm_max_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type max_epi32(simd_type lhs, simd_type rhs) {
		return _mm_max_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type max_epu16(simd_type lhs, simd_type rhs) {
		return _mm_max_epu16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type max_epu32(simd_type lhs, simd_type rhs) {
		return _mm_max_epu32(lhs, rhs);
	}

#else
	WJR_INTRINSIC_INLINE static simd_type max_epi8(simd_type lhs, simd_type rhs) {
		return blendv_epi8(rhs, lhs, cmpgt_epi8(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type max_epi32(simd_type lhs, simd_type rhs) {
		return blendv_epi8(rhs, lhs, cmpgt_epi32(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type max_epu16(simd_type lhs, simd_type rhs) {
		return blendv_epi8(rhs, lhs, cmpgt_epu16(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type max_epu32(simd_type lhs, simd_type rhs) {
		return blendv_epi8(rhs, lhs, cmpgt_epu32(lhs, rhs));
	}
#endif // __SSE4_1__
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, int8_t) { return max_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, int16_t) { return max_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, int32_t) { return max_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint8_t) { return max_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint16_t) { return max_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint32_t) { return max_epu32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static int8_t max_epi8(simd_type a) {
		return 0x7fu ^ min_epu8(Xor(a, set1_epi8(0x7fu)));
	}
	WJR_INTRINSIC_INLINE static int16_t max_epi16(simd_type a) {
#if defined(__SSE4_1__)
		return 0x7fffu ^ min_epu16(Xor(a, set1_epi16(0x7fffu)));
#else
		a = max_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<int16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static int32_t max_epi32(simd_type a) {
		a = max_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<int32_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint8_t max_epu8(simd_type a) {
#if defined(__SSE4_1__)
		return 0xffu ^ min_epu8(Xor(a, set_all_ones()));
#else
		a = max_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		auto X = simd_cast<uint32_t>(a);
		return std::max((uint8_t)X, (uint8_t)(X >> 8));
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint16_t max_epu16(simd_type a) {
#if defined(__SSE4_1__)
		return 0xffffu ^ min_epu16(Xor(a, set_all_ones()));
#else
		a = max_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<uint16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint32_t max_epu32(simd_type a) {
		a = max_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<uint32_t>(a);
	}

	WJR_INTRINSIC_INLINE static int8_t max(simd_type a, int8_t) { return max_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t max(simd_type a, int16_t) { return max_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t max(simd_type a, int32_t) { return max_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t max(simd_type a, uint8_t) { return max_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t max(simd_type a, uint16_t) { return max_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t max(simd_type a, uint32_t) { return max_epu32(a); }

	WJR_INTRINSIC_INLINE static simd_type min_epi16(simd_type lhs, simd_type rhs) {
		return _mm_min_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type min_epu8(simd_type lhs, simd_type rhs) {
		return _mm_min_epu8(lhs, rhs);
	}
#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static simd_type min_epi8(simd_type lhs, simd_type rhs) {
		return _mm_min_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type min_epi32(simd_type lhs, simd_type rhs) {
		return _mm_min_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type min_epu16(simd_type lhs, simd_type rhs) {
		return _mm_min_epu16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type min_epu32(simd_type lhs, simd_type rhs) {
		return _mm_min_epu32(lhs, rhs);
	}
#else
	WJR_INTRINSIC_INLINE static simd_type min_epi8(simd_type lhs, simd_type rhs) {
		return blendv_epi8(lhs, rhs, cmpgt_epi8(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type min_epi32(simd_type lhs, simd_type rhs) {
		return blendv_epi8(lhs, rhs, cmpgt_epi32(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type min_epu16(simd_type lhs, simd_type rhs) {
		return blendv_epi8(lhs, rhs, cmpgt_epu16(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type min_epu32(simd_type lhs, simd_type rhs) {
		return blendv_epi8(lhs, rhs, cmpgt_epu32(lhs, rhs));
	}
#endif // __SSE4_1__
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, int8_t) { return min_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, int16_t) { return min_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, int32_t) { return min_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint8_t) { return min_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint16_t) { return min_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint32_t) { return min_epu32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static int8_t min_epi8(simd_type a) {
		return 0x80u ^ min_epu8(Xor(a, setmin_epi8()));
	}
	WJR_INTRINSIC_INLINE static int16_t min_epi16(simd_type a) {
#if defined(__SSE4_1__)
		return 0x8000u ^ min_epu16(Xor(a, setmin_epi16()));
#else
		a = min_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<int16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static int32_t min_epi32(simd_type a) {
		a = min_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<int32_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint8_t min_epu8(simd_type a) {
#if defined(__SSE4_1__)
		a = min_epu8(a, srli_epi16(a, 8));
		a = minpos_epu16(a);
		return simd_cast<uint8_t>(a);
#else
		a = min_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		auto X = simd_cast<uint32_t>(a);
		return std::min((uint8_t)X, (uint8_t)(X >> 8));
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint16_t min_epu16(simd_type a) {
#if defined(__SSE4_1__)
		return simd_cast<uint16_t>(minpos_epu16(a));
#else
		a = min_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<uint16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint32_t min_epu32(simd_type a) {
		a = min_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<uint32_t>(a);
	}

	WJR_INTRINSIC_INLINE static int8_t min(simd_type a, int8_t) { return min_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t min(simd_type a, int16_t) { return min_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t min(simd_type a, int32_t) { return min_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t min(simd_type a, uint8_t) { return min_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t min(simd_type a, uint16_t) { return min_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t min(simd_type a, uint32_t) { return min_epu32(a); }

#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static simd_type minpos_epu16(simd_type a) { return _mm_minpos_epu16(a); }
#endif // __SSE4_1__

	WJR_INTRINSIC_INLINE static simd_type move_epi64(simd_type lhs) {
		return _mm_move_epi64(lhs);
	}

	WJR_INTRINSIC_INLINE static int movemask_epi8(simd_type lhs) {
		return _mm_movemask_epi8(lhs);
	}

#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static simd_type mul_epi32(simd_type lhs, simd_type rhs) {
		return _mm_mul_epi32(lhs, rhs);
	}
#endif // __SSE4_1__
	WJR_INTRINSIC_INLINE static simd_type mul_epu32(simd_type lhs, simd_type rhs) {
		return _mm_mul_epu32(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type mulhi_epi16(simd_type lhs, simd_type rhs) {
		return _mm_mulhi_epi16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type mulhi_epu16(simd_type lhs, simd_type rhs) {
		return _mm_mulhi_epu16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type mullo_epi16(simd_type lhs, simd_type rhs) {
		return _mm_mullo_epi16(lhs, rhs);
	}
#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static simd_type mullo_epi32(simd_type lhs, simd_type rhs) {
		return _mm_mullo_epi32(lhs, rhs);
	}
#endif // __SSE4_1__

	WJR_INTRINSIC_INLINE static simd_type packs_epi16(simd_type lhs, simd_type rhs) {
		return _mm_packs_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type packs_epi32(simd_type lhs, simd_type rhs) {
		return _mm_packs_epi32(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type packus_epi16(simd_type lhs, simd_type rhs) {
		return _mm_packus_epi16(lhs, rhs);
	}
#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static simd_type packus_epi32(simd_type lhs, simd_type rhs) {
		return _mm_packus_epi32(lhs, rhs);
	}
#endif // __SSE4_1__

	WJR_INTRINSIC_INLINE static simd_type sad_epu8(simd_type lhs, simd_type rhs) {
		return _mm_sad_epu8(lhs, rhs);
	}

#if defined(__SSSE3__)
	WJR_INTRINSIC_INLINE static simd_type shuffle_epi8(simd_type v, simd_type imm8) {
		return _mm_shuffle_epi8(v, imm8);
	}
#endif // __SSSE3__
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type shuffle_epi32(simd_type v) {
		return _mm_shuffle_epi32(v, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type shufflehi_epi16(simd_type v) {
		return _mm_shufflehi_epi16(v, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type shufflelo_epi16(simd_type v) {
		return _mm_shufflelo_epi16(v, imm8);
	}

#if defined(__SSSE3__)
	WJR_INTRINSIC_INLINE static simd_type sign_epi8(simd_type lhs, simd_type rhs) {
		return _mm_sign_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sign_epi16(simd_type lhs, simd_type rhs) {
		return _mm_sign_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sign_epi32(simd_type lhs, simd_type rhs) {
		return _mm_sign_epi32(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type sign(simd_type lhs, simd_type rhs, int8_t) { return sign_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sign(simd_type lhs, simd_type rhs, int16_t) { return sign_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sign(simd_type lhs, simd_type rhs, int32_t) { return sign_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sign(simd_type lhs, simd_type rhs, uint8_t) { return sign_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sign(simd_type lhs, simd_type rhs, uint16_t) { return sign_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sign(simd_type lhs, simd_type rhs, uint32_t) { return sign_epi32(lhs, rhs); }
#endif // __SSSE3__

	WJR_INTRINSIC_INLINE static simd_type sll_epi16(simd_type lhs, simd_type rhs) {
		return _mm_sll_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sll_epi32(simd_type lhs, simd_type rhs) {
		return _mm_sll_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sll_epi64(simd_type lhs, simd_type rhs) {
		return _mm_sll_epi64(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, simd_type rhs, int16_t) { return sll_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, simd_type rhs, int32_t) { return sll_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, simd_type rhs, int64_t) { return sll_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, simd_type rhs, uint16_t) { return sll_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, simd_type rhs, uint32_t) { return sll_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, simd_type rhs, uint64_t) { return sll_epi64(lhs, rhs); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type slli_all(simd_type v) {
		return _mm_slli_si128(v, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type v) {
		return slli_all<imm8>(v);
	}
	WJR_INTRINSIC_INLINE static simd_type slli_epi16(simd_type lhs, int imm8) {
		return _mm_slli_epi16(lhs, imm8);
	}
	WJR_INTRINSIC_INLINE static simd_type slli_epi32(simd_type lhs, int imm8) {
		return _mm_slli_epi32(lhs, imm8);
	}
	WJR_INTRINSIC_INLINE static simd_type slli_epi64(simd_type lhs, int imm8) {
		return _mm_slli_epi64(lhs, imm8);
	}

	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, int16_t) { return slli_epi16(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, int32_t) { return slli_epi32(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, int64_t) { return slli_epi64(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, uint16_t) { return slli_epi16(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, uint32_t) { return slli_epi32(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, uint64_t) { return slli_epi64(lhs, imm8); }

	WJR_INTRINSIC_INLINE static simd_type sra_epi16(simd_type lhs, simd_type rhs) {
		return _mm_sra_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sra_epi32(simd_type lhs, simd_type rhs) {
		return _mm_sra_epi32(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type sra(simd_type lhs, simd_type rhs, int16_t) { return sra_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sra(simd_type lhs, simd_type rhs, int32_t) { return sra_epi32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type srai_epi16(simd_type lhs, int imm8) {
		return _mm_srai_epi16(lhs, imm8);
	}
	WJR_INTRINSIC_INLINE static simd_type srai_epi32(simd_type lhs, int imm8) {
		return _mm_srai_epi32(lhs, imm8);
	}

	WJR_INTRINSIC_INLINE static simd_type srai(simd_type lhs, int imm8, int16_t) { return srai_epi16(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srai(simd_type lhs, int imm8, int32_t) { return srai_epi32(lhs, imm8); }

	WJR_INTRINSIC_INLINE static simd_type srl_epi16(simd_type lhs, simd_type rhs) {
		return _mm_srl_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type srl_epi32(simd_type lhs, simd_type rhs) {
		return _mm_srl_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type srl_epi64(simd_type lhs, simd_type rhs) {
		return _mm_srl_epi64(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, simd_type rhs, int16_t) { return srl_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, simd_type rhs, int32_t) { return srl_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, simd_type rhs, int64_t) { return srl_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, simd_type rhs, uint16_t) { return srl_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, simd_type rhs, uint32_t) { return srl_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, simd_type rhs, uint64_t) { return srl_epi64(lhs, rhs); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type srli_all(simd_type v) {
		return _mm_srli_si128(v, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type v) {
		return srli_all<imm8>(v);
	}
	WJR_INTRINSIC_INLINE static simd_type srli_epi16(simd_type lhs, int imm8) {
		return _mm_srli_epi16(lhs, imm8);
	}
	WJR_INTRINSIC_INLINE static simd_type srli_epi32(simd_type lhs, int imm8) {
		return _mm_srli_epi32(lhs, imm8);
	}
	WJR_INTRINSIC_INLINE static simd_type srli_epi64(simd_type lhs, int imm8) {
		return _mm_srli_epi64(lhs, imm8);
	}

	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, int16_t) { return srli_epi16(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, int32_t) { return srli_epi32(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, int64_t) { return srli_epi64(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, uint16_t) { return srli_epi16(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, uint32_t) { return srli_epi32(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, uint64_t) { return srli_epi64(lhs, imm8); }

	WJR_INTRINSIC_INLINE static simd_type sub_epi8(simd_type lhs, simd_type rhs) {
		return _mm_sub_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sub_epi16(simd_type lhs, simd_type rhs) {
		return _mm_sub_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sub_epi32(simd_type lhs, simd_type rhs) {
		return _mm_sub_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sub_epi64(simd_type lhs, simd_type rhs) {
		return _mm_sub_epi64(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int8_t) { return sub_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int16_t) { return sub_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int32_t) { return sub_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int64_t) { return sub_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint8_t) { return sub_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint16_t) { return sub_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint32_t) { return sub_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint64_t) { return sub_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type subs_epi8(simd_type lhs, simd_type rhs) {
		return _mm_subs_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type subs_epi16(simd_type lhs, simd_type rhs) {
		return _mm_subs_epi16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type subs_epu8(simd_type lhs, simd_type rhs) {
		return _mm_subs_epu8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type subs_epu16(simd_type lhs, simd_type rhs) {
		return _mm_subs_epu16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type subs(simd_type lhs, simd_type rhs, int8_t) { return subs_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type subs(simd_type lhs, simd_type rhs, int16_t) { return subs_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type subs(simd_type lhs, simd_type rhs, uint8_t) { return subs_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type subs(simd_type lhs, simd_type rhs, uint16_t) { return subs_epu16(lhs, rhs); }

#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static int test_all_ones(simd_type lhs) {
		return _mm_test_all_ones(lhs);
	}

	WJR_INTRINSIC_INLINE static int test_all_zeros(simd_type lhs, simd_type rhs) {
		return _mm_test_all_zeros(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static int test_all_zeros(simd_type a) {
		return _mm_test_all_zeros(a, a);
	}

	WJR_INTRINSIC_INLINE static int test_mix_ones_zeros(simd_type lhs, simd_type rhs) {
		return _mm_test_mix_ones_zeros(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static int testc(simd_type lhs, simd_type rhs) {
		return _mm_testc_si128(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static int testnzc(simd_type lhs, simd_type rhs) {
		return _mm_testnzc_si128(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static int testz(simd_type lhs, simd_type rhs) {
		return _mm_testz_si128(lhs, rhs);
	}
#endif // __SSE4_1__

	WJR_INTRINSIC_INLINE static simd_type unpackhi_epi8(simd_type lhs, simd_type rhs) {
		return _mm_unpackhi_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpackhi_epi16(simd_type lhs, simd_type rhs) {
		return _mm_unpackhi_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpackhi_epi32(simd_type lhs, simd_type rhs) {
		return _mm_unpackhi_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpackhi_epi64(simd_type lhs, simd_type rhs) {
		return _mm_unpackhi_epi64(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int8_t) { return unpackhi_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int16_t) { return unpackhi_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int32_t) { return unpackhi_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int64_t) { return unpackhi_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint8_t) { return unpackhi_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint16_t) { return unpackhi_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint32_t) { return unpackhi_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint64_t) { return unpackhi_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type unpacklo_epi8(simd_type lhs, simd_type rhs) {
		return _mm_unpacklo_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpacklo_epi16(simd_type lhs, simd_type rhs) {
		return _mm_unpacklo_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpacklo_epi32(simd_type lhs, simd_type rhs) {
		return _mm_unpacklo_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpacklo_epi64(simd_type lhs, simd_type rhs) {
		return _mm_unpacklo_epi64(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int8_t) { return unpacklo_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int16_t) { return unpacklo_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int32_t) { return unpacklo_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int64_t) { return unpacklo_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint8_t) { return unpacklo_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint16_t) { return unpacklo_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint32_t) { return unpacklo_epi32(lhs, rhs); }

	// extern :

	WJR_INTRINSIC_INLINE static simd_type preload_si16(const void* ptr) { return loadu_si16(ptr); }
	WJR_INTRINSIC_INLINE static simd_type preload_si32(const void* ptr) { return loadu_si32(ptr); }
	WJR_INTRINSIC_INLINE static simd_type preload_si48(const void* ptr) {
		return insert_epi16<2>(preload_si32(ptr), reinterpret_cast<const uint16_t*>(ptr)[2]);
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si64(const void* ptr) { return loadu_si64(ptr); }
	WJR_INTRINSIC_INLINE static simd_type preload_si80(const void* ptr) {
		return insert_epi16<4>(preload_si64(ptr), reinterpret_cast<const uint16_t*>(ptr)[4]);
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si96(const void* ptr) {
#if defined(__SSE4_1__)
		return insert_epi32<2>(preload_si64(ptr), reinterpret_cast<const uint32_t*>(ptr)[2]);
#else
		return insert_epi16<5>(preload_si80(ptr), reinterpret_cast<const uint16_t*>(ptr)[5]);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si112(const void* ptr) {
		return insert_epi16<6>(preload_si96(ptr), reinterpret_cast<const uint16_t*>(ptr)[6]);
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si128(const void* ptr) { return loadu_all((simd_type*)ptr); }

	WJR_INTRINSIC_INLINE static simd_type preload_si16x(const void* ptr, int n) {
		// preload_si(n * 16)
		switch (n) {
		case 0:return set_all_zeros();
		case 1:return preload_si16(ptr);
		case 2:return preload_si32(ptr);
		case 3:return preload_si48(ptr);
		case 4:return preload_si64(ptr);
		case 5:return preload_si80(ptr);
		case 6:return preload_si96(ptr);
		case 7:return preload_si112(ptr);
		default:return preload_si128(ptr);
		}
	}

};

#define __SIMD_REGISTER_FUNCTION(name, T)					\
template<typename...Args>									\
WJR_INTRINSIC_INLINE static decltype(auto) name(Args&&...args) {	\
	return base::name(std::forward<Args>(args)...,T{});		\
}

#define __SIMD_REGISTER_TEMPLATE_INT_FUNCTION(name, T)		                \
template<int imm8, typename...Args>							                \
WJR_INTRINSIC_INLINE static decltype(auto) name(Args&&...args) {	                \
	return base::template name<imm8>(std::forward<Args>(args)...,T{});		\
}

#define __SIMD_REGISTER_ALL_COMMON_FUNCTIONS(T)						\
using value_type = T;												\
																	\
constexpr static int size(){ return base::width() /					\
	(sizeof(T) * 8); }												\
																	\
__SIMD_REGISTER_FUNCTION(setmin, T)									\
__SIMD_REGISTER_FUNCTION(setmax, T)									\
__SIMD_REGISTER_FUNCTION(set1, T)									\
																	\
WJR_HAS_SSSE3_DEFINE(__SIMD_REGISTER_FUNCTION(abs, T))				\
__SIMD_REGISTER_FUNCTION(add, T)									\
__SIMD_REGISTER_FUNCTION(adds, T)									\
__SIMD_REGISTER_FUNCTION(avg, T)									\
__SIMD_REGISTER_FUNCTION(cmpeq, T)									\
__SIMD_REGISTER_FUNCTION(cmpge, T)									\
__SIMD_REGISTER_FUNCTION(cmpgt, T)									\
__SIMD_REGISTER_FUNCTION(cmple, T)									\
__SIMD_REGISTER_FUNCTION(cmplt, T)									\
__SIMD_REGISTER_FUNCTION(cmpne, T)									\
__SIMD_REGISTER_FUNCTION(cmp, T)									\
__SIMD_REGISTER_TEMPLATE_INT_FUNCTION(extract, T)					\
__SIMD_REGISTER_TEMPLATE_INT_FUNCTION(insert, T)					\
__SIMD_REGISTER_FUNCTION(max, T)									\
__SIMD_REGISTER_FUNCTION(min, T)									\
WJR_HAS_SSSE3_DEFINE(__SIMD_REGISTER_FUNCTION(sign, T))				\
__SIMD_REGISTER_FUNCTION(sll, T)									\
__SIMD_REGISTER_FUNCTION(slli, T)									\
__SIMD_REGISTER_FUNCTION(sra, T)									\
__SIMD_REGISTER_FUNCTION(srai, T)									\
__SIMD_REGISTER_FUNCTION(srl, T)									\
__SIMD_REGISTER_FUNCTION(srli, T)									\
__SIMD_REGISTER_FUNCTION(sub, T)									\
__SIMD_REGISTER_FUNCTION(subs, T)									\
__SIMD_REGISTER_FUNCTION(unpackhi, T)								\
__SIMD_REGISTER_FUNCTION(unpacklo, T)								

template<>
class traits<int8_t, simd_type::SSE>
	: public traits<simd_integer, simd_type::SSE> {
	using base = traits<simd_integer, simd::simd_type::SSE>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(int8_t);
};

template<>
class traits<uint8_t, simd_type::SSE>
	: public traits<simd_integer, simd_type::SSE> {
	using base = traits<simd_integer, simd::simd_type::SSE>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(uint8_t);
};

template<>
class traits<int16_t, simd_type::SSE>
	: public traits<simd_integer, simd_type::SSE> {
	using base = traits<simd_integer, simd::simd_type::SSE>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(int16_t);
};

template<>
class traits<uint16_t, simd_type::SSE>
	: public traits<simd_integer, simd_type::SSE> {
	using base = traits<simd_integer, simd::simd_type::SSE>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(uint16_t);
};

template<>
class traits<int32_t, simd_type::SSE>
	: public traits<simd_integer, simd_type::SSE> {
	using base = traits<simd_integer, simd::simd_type::SSE>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(int32_t);
};

template<>
class traits<uint32_t, simd_type::SSE>
	: public traits<simd_integer, simd_type::SSE> {
	using base = traits<simd_integer, simd::simd_type::SSE>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(uint32_t);
};

#undef __SIMD_REGISTER_FUNCTION
#undef __SIMD_REGISTER_TEMPLATE_INT_FUNCTION
#undef __SIMD_REGISTER_ALL_COMMON_FUNCTIONS

#endif // __SSE2__

#if defined(__AVX__)

template<>
class traits<simd_integer, simd_type::AVX> {
public:
	using simd_type = __m256i;

	constexpr static int width() { return 256; }

	WJR_INTRINSIC_INLINE static simd_type set_all_zeros() { return _mm256_setzero_si256(); }
	WJR_INTRINSIC_INLINE static simd_type set_all_ones() { return _mm256_set1_epi32(-1); }

	WJR_INTRINSIC_INLINE static simd_type zextsi128_si256(__m128i v) { return _mm256_zextsi128_si256(v); }

	WJR_INTRINSIC_INLINE static simd_type set_epi8(
		char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
		char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
		return _mm256_set_epi8(
			e31, e30, e29, e28, e27, e26, e25, e24,
			e23, e22, e21, e20, e19, e18, e17, e16,
			e15, e14, e13, e12, e11, e10, e9, e8,
			e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static simd_type set_epi16(
		short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm256_set_epi16(
			e15, e14, e13, e12, e11, e10, e9, e8,
			e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static simd_type set_epi32(
		int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
		return _mm256_set_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static simd_type set_epi64x(
		long long e3, long long e2, long long e1, long long e0) {
		return _mm256_set_epi64x(e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static simd_type setr_epi8(
		char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
		char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
		return _mm256_setr_epi8(
			e31, e30, e29, e28, e27, e26, e25, e24,
			e23, e22, e21, e20, e19, e18, e17, e16,
			e15, e14, e13, e12, e11, e10, e9, e8,
			e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static simd_type setr_epi16(
		short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm256_setr_epi16(
			e15, e14, e13, e12, e11, e10, e9, e8,
			e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static simd_type setr_epi32(
		int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
		return _mm256_setr_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static simd_type setr_epi64x(
		long long e3, long long e2, long long e1, long long e0) {
		return _mm256_setr_epi64x(e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static simd_type set1_epi8(int8_t a) { return _mm256_set1_epi8(a); }
	WJR_INTRINSIC_INLINE static simd_type set1_epi16(int16_t a) { return _mm256_set1_epi16(a); }
	WJR_INTRINSIC_INLINE static simd_type set1_epi32(int32_t a) { return _mm256_set1_epi32(a); }
	WJR_INTRINSIC_INLINE static simd_type set1_epi64(int64_t a) { return _mm256_set1_epi64x(a); }

	WJR_INTRINSIC_INLINE static simd_type set1(int8_t a, int8_t) { return set1_epi8(a); }
	WJR_INTRINSIC_INLINE static simd_type set1(int16_t a, int16_t) { return set1_epi16(a); }
	WJR_INTRINSIC_INLINE static simd_type set1(int32_t a, int32_t) { return set1_epi32(a); }
	WJR_INTRINSIC_INLINE static simd_type set1(int64_t a, int64_t) { return set1_epi64(a); }
	WJR_INTRINSIC_INLINE static simd_type set1(uint8_t a, uint8_t) { return set1_epi8(a); }
	WJR_INTRINSIC_INLINE static simd_type set1(uint16_t a, uint16_t) { return set1_epi16(a); }
	WJR_INTRINSIC_INLINE static simd_type set1(uint32_t a, uint32_t) { return set1_epi32(a); }
	WJR_INTRINSIC_INLINE static simd_type set1(uint64_t a, uint64_t) { return set1_epi64(a); }

	WJR_INTRINSIC_INLINE static simd_type setmin_epi8() { return set1_epi8(0x80u); }
	WJR_INTRINSIC_INLINE static simd_type setmin_epi16() { return set1_epi16(0x8000u); }
	WJR_INTRINSIC_INLINE static simd_type setmin_epi32() { return set1_epi32(0x80000000u); }
	WJR_INTRINSIC_INLINE static simd_type setmin_epi64() { return set1_epi64(0x8000000000000000ull); }

	WJR_INTRINSIC_INLINE static simd_type setmin(int8_t) { return setmin_epi8(); }
	WJR_INTRINSIC_INLINE static simd_type setmin(int16_t) { return setmin_epi16(); }
	WJR_INTRINSIC_INLINE static simd_type setmin(int32_t) { return setmin_epi32(); }
	WJR_INTRINSIC_INLINE static simd_type setmin(int64_t) { return setmin_epi64(); }

	WJR_INTRINSIC_INLINE static simd_type setmax_epi8() { return set1_epi8(0x7f); }
	WJR_INTRINSIC_INLINE static simd_type setmax_epi16() { return set1_epi16(0x7fff); }
	WJR_INTRINSIC_INLINE static simd_type setmax_epi32() { return set1_epi32(0x7fffffff); }
	WJR_INTRINSIC_INLINE static simd_type setmax_epi64() { return set1_epi64(0x7fffffffffffffff); }

	WJR_INTRINSIC_INLINE static simd_type setmax(int8_t) { return setmax_epi8(); }
	WJR_INTRINSIC_INLINE static simd_type setmax(int16_t) { return setmax_epi16(); }
	WJR_INTRINSIC_INLINE static simd_type setmax(int32_t) { return setmax_epi32(); }
	WJR_INTRINSIC_INLINE static simd_type setmax(int64_t) { return setmax_epi64(); }

	WJR_INTRINSIC_INLINE static simd_type load_all(const simd_type* p) { return _mm256_load_si256(p); }
	WJR_INTRINSIC_INLINE static simd_type load(const simd_type* p) { return load_all(p); }
	WJR_INTRINSIC_INLINE static simd_type loadu_all(const simd_type* p) { return _mm256_loadu_si256(p); }
	WJR_INTRINSIC_INLINE static simd_type loadu(const simd_type* p) { return loadu_all(p); }

	WJR_INTRINSIC_INLINE static void store_all(simd_type* p, const simd_type& a) { _mm256_store_si256(p, a); }
	WJR_INTRINSIC_INLINE static void store(simd_type* p, const simd_type& a) { store_all(p, a); }
	WJR_INTRINSIC_INLINE static void storeu_all(simd_type* p, const simd_type& a) { _mm256_storeu_si256(p, a); }
	WJR_INTRINSIC_INLINE static void storeu(simd_type* p, const simd_type& a) { storeu_all(p, a); }

#if defined(__AVX2__)
	WJR_INTRINSIC_INLINE static simd_type And_all(simd_type lhs, simd_type rhs) {
		return _mm256_and_si256(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type And(simd_type lhs, simd_type rhs) { return And_all(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type AndNot_all(simd_type lhs, simd_type rhs) {
		return _mm256_andnot_si256(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type AndNot(simd_type lhs, simd_type rhs) { return AndNot_all(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type Or_all(simd_type lhs, simd_type rhs) { return _mm256_or_si256(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type Or(simd_type lhs, simd_type rhs) { return Or_all(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type Xor_all(simd_type lhs, simd_type rhs) { return _mm256_xor_si256(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type Xor(simd_type lhs, simd_type rhs) { return Xor_all(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type Not(simd_type v) { return _mm256_xor_si256(v, set_all_ones()); }

	WJR_INTRINSIC_INLINE static simd_type abs_epi8(simd_type v) { return _mm256_abs_epi8(v); }
	WJR_INTRINSIC_INLINE static simd_type abs_epi16(simd_type v) { return _mm256_abs_epi16(v); }
	WJR_INTRINSIC_INLINE static simd_type abs_epi32(simd_type v) { return _mm256_abs_epi32(v); }

	WJR_INTRINSIC_INLINE static simd_type abs(simd_type v, int8_t) { return abs_epi8(v); }
	WJR_INTRINSIC_INLINE static simd_type abs(simd_type v, int16_t) { return abs_epi16(v); }
	WJR_INTRINSIC_INLINE static simd_type abs(simd_type v, int32_t) { return abs_epi32(v); }
	WJR_INTRINSIC_INLINE static simd_type abs(simd_type v, int64_t) { return abs_epi32(v); }

	WJR_INTRINSIC_INLINE static simd_type add_epi8(simd_type lhs, simd_type rhs) { return _mm256_add_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add_epi16(simd_type lhs, simd_type rhs) { return _mm256_add_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add_epi32(simd_type lhs, simd_type rhs) { return _mm256_add_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add_epi64(simd_type lhs, simd_type rhs) { return _mm256_add_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, int8_t) { return add_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, int16_t) { return add_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, int32_t) { return add_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, int64_t) { return add_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint8_t) { return add_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint16_t) { return add_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint32_t) { return add_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint64_t) { return add_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static uint8_t add_epu8(simd_type v) {
		return sse_traits_t<simd_integer>::add_epu8(sse_traits_t<simd_integer>::add_epi8( getlow_si128(v), gethigh_si128(v)));
	}
	WJR_INTRINSIC_INLINE static uint16_t add_epu16(simd_type v) {
		return sse_traits_t<simd_integer>::add_epu16(sse_traits_t<simd_integer>::add_epi16(getlow_si128(v), gethigh_si128(v)));
	}
	WJR_INTRINSIC_INLINE static uint32_t add_epu32(simd_type v) {
		return sse_traits_t<simd_integer>::add_epu32(sse_traits_t<simd_integer>::add_epi32(getlow_si128(v), gethigh_si128(v)));
	}
	WJR_INTRINSIC_INLINE static uint64_t add_epu64(simd_type v) {
		return sse_traits_t<simd_integer>::add_epu64(sse_traits_t<simd_integer>::add_epi64(getlow_si128(v), gethigh_si128(v)));
	}
	WJR_INTRINSIC_INLINE static int8_t add_epi8(simd_type v) { return add_epu8(v); }
	WJR_INTRINSIC_INLINE static int16_t add_epi16(simd_type v) { return add_epu16(v); }
	WJR_INTRINSIC_INLINE static int32_t add_epi32(simd_type v) { return add_epu32(v); }
	WJR_INTRINSIC_INLINE static int64_t add_epi64(simd_type v) { return add_epu64(v); }
	
	WJR_INTRINSIC_INLINE static int8_t add(simd_type v, int8_t) { return add_epi8(v); }
	WJR_INTRINSIC_INLINE static int16_t add(simd_type v, int16_t) { return add_epi16(v); }
	WJR_INTRINSIC_INLINE static int32_t add(simd_type v, int32_t) { return add_epi32(v); }
	WJR_INTRINSIC_INLINE static int64_t add(simd_type v, int64_t) { return add_epi64(v); }
	WJR_INTRINSIC_INLINE static uint8_t add(simd_type v, uint8_t) { return add_epu8(v); }
	WJR_INTRINSIC_INLINE static uint16_t add(simd_type v, uint16_t) { return add_epu16(v); }
	WJR_INTRINSIC_INLINE static uint32_t add(simd_type v, uint32_t) { return add_epu32(v); }
	WJR_INTRINSIC_INLINE static uint64_t add(simd_type v, uint64_t) { return add_epu64(v); }

	WJR_INTRINSIC_INLINE static simd_type adds_epi8(simd_type lhs, simd_type rhs) { return _mm256_adds_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type adds_epi16(simd_type lhs, simd_type rhs) { return _mm256_adds_epi16(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type adds_epu8(simd_type lhs, simd_type rhs) { return _mm256_adds_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type adds_epu16(simd_type lhs, simd_type rhs) { return _mm256_adds_epu16(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type adds(simd_type lhs, simd_type rhs, int8_t) { return adds_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type adds(simd_type lhs, simd_type rhs, int16_t) { return adds_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type adds(simd_type lhs, simd_type rhs, uint8_t) { return adds_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type adds(simd_type lhs, simd_type rhs, uint16_t) { return adds_epu16(lhs, rhs); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type alignr(simd_type lhs, simd_type rhs) { return _mm256_alignr_epi8(lhs, rhs, imm8); }

	WJR_INTRINSIC_INLINE static simd_type avg_epu8(simd_type lhs, simd_type rhs) { return _mm256_avg_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type avg_epu16(simd_type lhs, simd_type rhs) { return _mm256_avg_epu16(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type avg(simd_type lhs, simd_type rhs, int8_t) { return avg_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type avg(simd_type lhs, simd_type rhs, int16_t) { return avg_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type avg(simd_type lhs, simd_type rhs, uint8_t) { return avg_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type avg(simd_type lhs, simd_type rhs, uint16_t) { return avg_epu16(lhs, rhs); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type blend_epi16(simd_type lhs, simd_type rhs) { 
		return _mm256_blend_epi16(lhs, rhs, imm8); 
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type blend_epi32(simd_type lhs, simd_type rhs) { 
		return _mm256_blend_epi32(lhs, rhs, imm8); 
	}

	WJR_INTRINSIC_INLINE static simd_type blendv_epi8(simd_type lhs, simd_type rhs, simd_type mask) { 
		return _mm256_blendv_epi8(lhs, rhs, mask); 
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type bslli_epi128(simd_type lhs) { return _mm256_bslli_epi128(lhs, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type bsrli_epi128(simd_type lhs) { return _mm256_bsrli_epi128(lhs, imm8); }

	WJR_INTRINSIC_INLINE static simd_type cmpeq_epi8(simd_type lhs, simd_type rhs) { return _mm256_cmpeq_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq_epi16(simd_type lhs, simd_type rhs) { return _mm256_cmpeq_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq_epi32(simd_type lhs, simd_type rhs) { return _mm256_cmpeq_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq_epi64(simd_type lhs, simd_type rhs) { return _mm256_cmpeq_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int8_t) { return cmpeq_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int16_t) { return cmpeq_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int32_t) { return cmpeq_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int64_t) { return cmpeq_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint8_t) { return cmpeq_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint16_t) { return cmpeq_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint32_t) { return cmpeq_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint64_t) { return cmpeq_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmpge_epi8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi8(rhs, lhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmpge_epi16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi16(rhs, lhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmpge_epi32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi32(rhs, lhs));
	}

	WJR_INTRINSIC_INLINE static simd_type cmpge_epu8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu8(rhs, lhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmpge_epu16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu16(rhs, lhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmpge_epu32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu32(rhs, lhs));
	}

	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, int8_t) { return cmpge_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, int16_t) { return cmpge_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, int32_t) { return cmpge_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, uint8_t) { return cmpge_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, uint16_t) { return cmpge_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, uint32_t) { return cmpge_epu32(lhs, rhs); }
	
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epi8(simd_type lhs, simd_type rhs) { return _mm256_cmpgt_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epi16(simd_type lhs, simd_type rhs) { return _mm256_cmpgt_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epi32(simd_type lhs, simd_type rhs) { return _mm256_cmpgt_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epi64(simd_type lhs, simd_type rhs) { return _mm256_cmpgt_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmpgt_epu8(simd_type lhs, simd_type rhs) {
#if defined(__XOP__)
		__REGISTER_AVX_FROM_SSE(sse_traits_t<simd_integer>::cmpgt_epu8, lhs, rhs);
#else
		return cmpgt_epi8(Xor(lhs, setmin_epi8()), Xor(rhs, setmin_epi8()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epu16(simd_type lhs, simd_type rhs) {
#if defined(__XOP__)
		__REGISTER_AVX_FROM_SSE(sse_traits_t<simd_integer>::cmpgt_epu16, lhs, rhs);
#else
		return cmpgt_epi16(Xor(lhs, setmin_epi16()), Xor(rhs, setmin_epi16()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epu32(simd_type lhs, simd_type rhs) {
#if defined(__XOP__)
		__REGISTER_AVX_FROM_SSE(sse_traits_t<simd_integer>::cmpgt_epu32, lhs, rhs);
#else
		return cmpgt_epi32(Xor(lhs, setmin_epi32()), Xor(rhs, setmin_epi32()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static simd_type cmpgt_epu64(simd_type lhs, simd_type rhs) {
#if defined(__XOP__)
		__REGISTER_AVX_FROM_SSE(sse_traits_t<simd_integer>::cmpgt_epu64, lhs, rhs);
#else
		return cmpgt_epi64(Xor(lhs, setmin_epi64()), Xor(rhs, setmin_epi64()));
#endif // __XOP__
	}

	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int8_t) { return cmpgt_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int16_t) { return cmpgt_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int32_t) { return cmpgt_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int64_t) { return cmpgt_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint8_t) { return cmpgt_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint16_t) { return cmpgt_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint32_t) { return cmpgt_epu32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint64_t) { return cmpgt_epu64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmple_epi8(simd_type lhs, simd_type rhs) { 
		return Not(cmpgt_epi8(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmple_epi16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi16(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmple_epi32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi32(lhs, rhs));
	}
	
	WJR_INTRINSIC_INLINE static simd_type cmple_epu8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu8(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmple_epu16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu16(lhs, rhs));
	}
	WJR_INTRINSIC_INLINE static simd_type cmple_epu32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu32(lhs, rhs));
	}

	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, int8_t) { return cmple_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, int16_t) { return cmple_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, int32_t) { return cmple_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, uint8_t) { return cmple_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, uint16_t) { return cmple_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, uint32_t) { return cmple_epu32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmplt_epi8(simd_type lhs, simd_type rhs) { return cmpgt_epi8(rhs, lhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt_epi16(simd_type lhs, simd_type rhs) { return cmpgt_epi16(rhs, lhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt_epi32(simd_type lhs, simd_type rhs) { return cmpgt_epi32(rhs, lhs); }

	WJR_INTRINSIC_INLINE static simd_type cmplt_epu8(simd_type lhs, simd_type rhs) {
		return cmpgt_epu8(rhs, lhs);
	}
	WJR_INTRINSIC_INLINE static simd_type cmplt_epu16(simd_type lhs, simd_type rhs) {
		return cmpgt_epu16(rhs, lhs);
	}
	WJR_INTRINSIC_INLINE static simd_type cmplt_epu32(simd_type lhs, simd_type rhs) {
		return cmpgt_epu32(rhs, lhs);
	}

	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int8_t) { return cmplt_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int16_t) { return cmplt_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int32_t) { return cmplt_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint8_t) { return cmplt_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint16_t) { return cmplt_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint32_t) { return cmplt_epu32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type cmpne_epi8(simd_type lhs, simd_type rhs) { return Not(cmpeq_epi8(lhs, rhs)); }
	WJR_INTRINSIC_INLINE static simd_type cmpne_epi16(simd_type lhs, simd_type rhs) { return Not(cmpeq_epi16(lhs, rhs)); }
	WJR_INTRINSIC_INLINE static simd_type cmpne_epi32(simd_type lhs, simd_type rhs) { return Not(cmpeq_epi32(lhs, rhs)); }

	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, int8_t) { return cmpne_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, int16_t) { return cmpne_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, int32_t) { return cmpne_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, uint8_t) { return cmpne_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, uint16_t) { return cmpne_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type cmpne(simd_type lhs, simd_type rhs, uint32_t) { return cmpne_epi32(lhs, rhs); }

	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::equal_to<>, T) {
		return cmpeq(lhs, rhs, T{});
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::not_equal_to<>, T) {
		return cmpne(lhs, rhs, T{});
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::greater<>, T) {
		return cmpgt(lhs, rhs, T{});
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::greater_equal<>, T) {
		return cmpge(lhs, rhs, T{});
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::less<>, T) {
		return cmplt(lhs, rhs, T{});
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cmp(simd_type lhs, simd_type rhs, std::less_equal<>, T) {
		return cmple(lhs, rhs, T{});
	}

	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cvtepu8(__m128i v);
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cvtepu16(__m128i v);
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cvtepu32(__m128i v);

	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cvtepu(__m128i v, int8_t) { return cvtepu8<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cvtepu(__m128i v, int16_t) { return cvtepu16<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cvtepu(__m128i v, int32_t) { return cvtepu32<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cvtepu(__m128i v, uint8_t) { return cvtepu8<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cvtepu(__m128i v, uint16_t) { return cvtepu16<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static simd_type cvtepu(__m128i v, uint32_t) { return cvtepu32<T>(v); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi8(simd_type v) { return _mm256_extract_epi8(v, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi16(simd_type v) { return _mm256_extract_epi16(v, imm8); }
#endif // __AVX2__
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi32(simd_type v) { return _mm256_extract_epi32(v, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract_epi64(simd_type v) { return _mm256_extract_epi64(v, imm8); }

#if defined(__AVX2__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(simd_type v, int8_t) { return extract_epi8<imm8>(v); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(simd_type v, int16_t) { return extract_epi16<imm8>(v); }
#endif // __AVX2__
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(simd_type v, int32_t) { return extract_epi32<imm8>(v); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(simd_type v, int64_t) { return extract_epi64<imm8>(v); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i extract_si128(simd_type v) { 
#if defined(__AVX2__)
		return _mm256_extracti128_si256(v, imm8);
#else
		return _mm256_extractf128_si256(v, imm8);
#endif // __AVX2__
	}

#if defined(__AVX2__)
	WJR_INTRINSIC_INLINE static simd_type hadd_epi16(simd_type lhs, simd_type rhs) { return _mm256_hadd_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type hadd_epi32(simd_type lhs, simd_type rhs) { return _mm256_hadd_epi32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type hadd(simd_type lhs, simd_type rhs, int16_t) { return hadd_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type hadd(simd_type lhs, simd_type rhs, int32_t) { return hadd_epi32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type hadds_epi16(simd_type lhs, simd_type rhs) { return _mm256_hadds_epi16(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type hsub_epi16(simd_type lhs, simd_type rhs) { return _mm256_hsub_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type hsub_epi32(simd_type lhs, simd_type rhs) { return _mm256_hsub_epi32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type hsub(simd_type lhs, simd_type rhs, int16_t) { return hsub_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type hsub(simd_type lhs, simd_type rhs, int32_t) { return hsub_epi32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type hsubs_epi16(simd_type lhs, simd_type rhs) { return _mm256_hsubs_epi16(lhs, rhs); }

#endif // __AVX2__

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert_epi8(simd_type v, int8_t i) { return _mm256_insert_epi8(v, i, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert_epi16(simd_type v, int16_t i) { return _mm256_insert_epi16(v, i, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert_epi32(simd_type v, int32_t i) { return _mm256_insert_epi32(v, i, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert_epi64(simd_type v, int64_t i) { return _mm256_insert_epi64(v, i, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type insert_si128(simd_type a, __m128i b) { 
#if defined(__AVX2__)
		return _mm256_inserti128_si256(a, b, imm8);
#else
		return _mm256_insertf128_si256(a, b, imm8);
#endif // __AVX2__
	}

#if defined(__AVX2__)
	WJR_INTRINSIC_INLINE static simd_type madd_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_madd_epi16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type max_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_max_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type max_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_max_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type max_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_max_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type max_epu8(simd_type lhs, simd_type rhs) {
		return _mm256_max_epu8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type max_epu16(simd_type lhs, simd_type rhs) {
		return _mm256_max_epu16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type max_epu32(simd_type lhs, simd_type rhs) {
		return _mm256_max_epu32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, int8_t) { return max_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, int16_t) { return max_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint8_t) { return max_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint16_t) { return max_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, int32_t) { return max_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint32_t) { return max_epu32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static int8_t max_epi8(simd_type a){
		return sse_traits_t<simd_integer>::max_epi8(sse_traits_t<simd_integer>::max_epi8(getlow_si128(a), gethigh_si128(a)));
	}
	WJR_INTRINSIC_INLINE static int16_t max_epi16(simd_type a) {
		return sse_traits_t<simd_integer>::max_epi16(sse_traits_t<simd_integer>::max_epi16(getlow_si128(a), gethigh_si128(a)));
	}
	WJR_INTRINSIC_INLINE static int32_t max_epi32(simd_type a) {
		return sse_traits_t<simd_integer>::max_epi32(sse_traits_t<simd_integer>::max_epi32(getlow_si128(a), gethigh_si128(a)));
	}
	WJR_INTRINSIC_INLINE static uint8_t max_epu8(simd_type a) {
		return sse_traits_t<simd_integer>::max_epu8(sse_traits_t<simd_integer>::max_epu8(getlow_si128(a), gethigh_si128(a)));
	}
	WJR_INTRINSIC_INLINE static uint16_t max_epu16(simd_type a) {
		return sse_traits_t<simd_integer>::max_epu16(sse_traits_t<simd_integer>::max_epu16(getlow_si128(a), gethigh_si128(a)));
	}
	WJR_INTRINSIC_INLINE static uint32_t max_epu32(simd_type a) {
		return sse_traits_t<simd_integer>::max_epu32(sse_traits_t<simd_integer>::max_epu32(getlow_si128(a), gethigh_si128(a)));
	}

	WJR_INTRINSIC_INLINE static int8_t max(simd_type a, int8_t) { return max_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t max(simd_type a, int16_t) { return max_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t max(simd_type a, int32_t) { return max_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t max(simd_type a, uint8_t) { return max_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t max(simd_type a, uint16_t) { return max_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t max(simd_type a, uint32_t) { return max_epu32(a); }

	WJR_INTRINSIC_INLINE static simd_type min_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_min_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type min_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_min_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type min_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_min_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type min_epu8(simd_type lhs, simd_type rhs) {
		return _mm256_min_epu8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type min_epu16(simd_type lhs, simd_type rhs) {
		return _mm256_min_epu16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type min_epu32(simd_type lhs, simd_type rhs) {
		return _mm256_min_epu32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, int8_t) { return min_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, int16_t) { return min_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint8_t) { return min_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint16_t) { return min_epu16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, int32_t) { return min_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint32_t) { return min_epu32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static int8_t min_epi8(simd_type a) {
		return sse_traits_t<simd_integer>::min_epi8(sse_traits_t<simd_integer>::min_epi8(getlow_si128(a), gethigh_si128(a)));
	}
	WJR_INTRINSIC_INLINE static int16_t min_epi16(simd_type a) {
		return sse_traits_t<simd_integer>::min_epi16(sse_traits_t<simd_integer>::min_epi16(getlow_si128(a), gethigh_si128(a)));
	}
	WJR_INTRINSIC_INLINE static int32_t min_epi32(simd_type a) {
		return sse_traits_t<simd_integer>::min_epi32(sse_traits_t<simd_integer>::min_epi32(getlow_si128(a), gethigh_si128(a)));
	}
	WJR_INTRINSIC_INLINE static uint8_t min_epu8(simd_type a) {
		return sse_traits_t<simd_integer>::min_epu8(sse_traits_t<simd_integer>::min_epu8(getlow_si128(a), gethigh_si128(a)));
	}
	WJR_INTRINSIC_INLINE static uint16_t min_epu16(simd_type a) {
		return sse_traits_t<simd_integer>::min_epu16(sse_traits_t<simd_integer>::min_epu16(getlow_si128(a), gethigh_si128(a)));
	}
	WJR_INTRINSIC_INLINE static uint32_t min_epu32(simd_type a) {
		return sse_traits_t<simd_integer>::min_epu32(sse_traits_t<simd_integer>::min_epu32(getlow_si128(a), gethigh_si128(a)));
	}

	WJR_INTRINSIC_INLINE static int8_t min(simd_type a, int8_t) { return min_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t min(simd_type a, int16_t) { return min_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t min(simd_type a, int32_t) { return min_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t min(simd_type a, uint8_t) { return min_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t min(simd_type a, uint16_t) { return min_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t min(simd_type a, uint32_t) { return min_epu32(a); }

	WJR_INTRINSIC_INLINE static int movemask_epi8(simd_type lhs) {
		return _mm256_movemask_epi8(lhs);
	}

	WJR_INTRINSIC_INLINE static simd_type mul_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_mul_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type mul_epu32(simd_type lhs, simd_type rhs) {
		return _mm256_mul_epu32(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type mulhi_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_mulhi_epi16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type mulhi_epu16(simd_type lhs, simd_type rhs) {
		return _mm256_mulhi_epu16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type mullo_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_mullo_epi16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type packs_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_packs_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type packs_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_packs_epi32(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type packus_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_packus_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type packus_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_packus_epi32(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type shuffle_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_shuffle_epi8(lhs, rhs);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type shuffle_epi32(simd_type a) {
		return _mm256_shuffle_epi32(a, imm8);
	}
	
	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type shufflehi_epi16(simd_type a) {
		return _mm256_shufflehi_epi16(a, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static simd_type shufflelo_epi16(simd_type a) {
		return _mm256_shufflelo_epi16(a, imm8);
	}

	WJR_INTRINSIC_INLINE static simd_type sll_epi16(simd_type lhs, __m128i rhs) {
		return _mm256_sll_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sll_epi32(simd_type lhs, __m128i rhs) {
		return _mm256_sll_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sll_epi64(simd_type lhs, __m128i rhs) {
		return _mm256_sll_epi64(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, __m128i rhs, int16_t) { return sll_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, __m128i rhs, int32_t) { return sll_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, __m128i rhs, int64_t) { return sll_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, __m128i rhs, uint16_t) { return sll_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, __m128i rhs, uint32_t) { return sll_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sll(simd_type lhs, __m128i rhs, uint64_t) { return sll_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type slli_epi16(simd_type lhs, int imm8) {
		return _mm256_slli_epi16(lhs, imm8);
	}
	WJR_INTRINSIC_INLINE static simd_type slli_epi32(simd_type lhs, int imm8) {
		return _mm256_slli_epi32(lhs, imm8);
	}
	WJR_INTRINSIC_INLINE static simd_type slli_epi64(simd_type lhs, int imm8) {
		return _mm256_slli_epi64(lhs, imm8);
	}

	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, int16_t) { return slli_epi16(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, int32_t) { return slli_epi32(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, int64_t) { return slli_epi64(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, uint16_t) { return slli_epi16(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, uint32_t) { return slli_epi32(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type slli(simd_type lhs, int imm8, uint64_t) { return slli_epi64(lhs, imm8); }

	WJR_INTRINSIC_INLINE static simd_type sra_epi16(simd_type lhs, __m128i rhs) {
		return _mm256_sra_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sra_epi32(simd_type lhs, __m128i rhs) {
		return _mm256_sra_epi32(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type sra(simd_type lhs, __m128i rhs, int16_t) { return sra_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sra(simd_type lhs, __m128i rhs, int32_t) { return sra_epi32(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type srai_epi16(simd_type lhs, int imm8) {
		return _mm256_srai_epi16(lhs, imm8);
	}
	WJR_INTRINSIC_INLINE static simd_type srai_epi32(simd_type lhs, int imm8) {
		return _mm256_srai_epi32(lhs, imm8);
	}

	WJR_INTRINSIC_INLINE static simd_type srai(simd_type lhs, int imm8, int16_t) { return srai_epi16(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srai(simd_type lhs, int imm8, int32_t) { return srai_epi32(lhs, imm8); }

	WJR_INTRINSIC_INLINE static simd_type srl_epi16(simd_type lhs, __m128i rhs) {
		return _mm256_srl_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type srl_epi32(simd_type lhs, __m128i rhs) {
		return _mm256_srl_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type srl_epi64(simd_type lhs, __m128i rhs) {
		return _mm256_srl_epi64(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, __m128i rhs, int16_t) { return srl_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, __m128i rhs, int32_t) { return srl_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, __m128i rhs, int64_t) { return srl_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, __m128i rhs, uint16_t) { return srl_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, __m128i rhs, uint32_t) { return srl_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type srl(simd_type lhs, __m128i rhs, uint64_t) { return srl_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type srli_epi16(simd_type lhs, int imm8) {
		return _mm256_srli_epi16(lhs, imm8);
	}
	WJR_INTRINSIC_INLINE static simd_type srli_epi32(simd_type lhs, int imm8) {
		return _mm256_srli_epi32(lhs, imm8);
	}
	WJR_INTRINSIC_INLINE static simd_type srli_epi64(simd_type lhs, int imm8) {
		return _mm256_srli_epi64(lhs, imm8);
	}

	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, int16_t) { return srli_epi16(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, int32_t) { return srli_epi32(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, int64_t) { return srli_epi64(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, uint16_t) { return srli_epi16(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, uint32_t) { return srli_epi32(lhs, imm8); }
	WJR_INTRINSIC_INLINE static simd_type srli(simd_type lhs, int imm8, uint64_t) { return srli_epi64(lhs, imm8); }

	WJR_INTRINSIC_INLINE static simd_type sub_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_sub_epi8(lhs, rhs);
	} 
	WJR_INTRINSIC_INLINE static simd_type sub_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_sub_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sub_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_sub_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type sub_epi64(simd_type lhs, simd_type rhs) {
		return _mm256_sub_epi64(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int8_t) { return sub_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int16_t) { return sub_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int32_t) { return sub_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int64_t) { return sub_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint8_t) { return sub_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint16_t) { return sub_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint32_t) { return sub_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint64_t) { return sub_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type subs_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_subs_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type subs_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_subs_epi16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type subs_epu8(simd_type lhs, simd_type rhs) {
		return _mm256_subs_epu8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type subs_epu16(simd_type lhs, simd_type rhs) {
		return _mm256_subs_epu16(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type subs(simd_type lhs, simd_type rhs, int8_t) { return subs_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type subs(simd_type lhs, simd_type rhs, int16_t) { return subs_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type subs(simd_type lhs, simd_type rhs, uint8_t) { return subs_epu8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type subs(simd_type lhs, simd_type rhs, uint16_t) { return subs_epu16(lhs, rhs); }

#endif // __AVX2__

	WJR_INTRINSIC_INLINE static int test_all_ones(simd_type a) {
		return testc(a, cmpeq_epi32(a, a));
	}

	WJR_INTRINSIC_INLINE static int test_all_zeros(simd_type a) {
		return testz(a, a);
	}

	WJR_INTRINSIC_INLINE static int testc(simd_type lhs, simd_type rhs) {
		return _mm256_testc_si256(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static int testnzc(simd_type lhs, simd_type rhs) {
		return _mm256_testnzc_si256(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static int testz(simd_type lhs, simd_type rhs) {
		return _mm256_testz_si256(lhs, rhs);
	}

#if defined(__AVX2__)

	WJR_INTRINSIC_INLINE static simd_type unpackhi_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_unpackhi_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpackhi_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_unpackhi_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpackhi_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_unpackhi_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpackhi_epi64(simd_type lhs, simd_type rhs) {
		return _mm256_unpackhi_epi64(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int8_t) { return unpackhi_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int16_t) { return unpackhi_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int32_t) { return unpackhi_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int64_t) { return unpackhi_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint8_t) { return unpackhi_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint16_t) { return unpackhi_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint32_t) { return unpackhi_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint64_t) { return unpackhi_epi64(lhs, rhs); }

	WJR_INTRINSIC_INLINE static simd_type unpacklo_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_unpacklo_epi8(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpacklo_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_unpacklo_epi16(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpacklo_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_unpacklo_epi32(lhs, rhs);
	}
	WJR_INTRINSIC_INLINE static simd_type unpacklo_epi64(simd_type lhs, simd_type rhs) {
		return _mm256_unpacklo_epi64(lhs, rhs);
	}

	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int8_t) { return unpacklo_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int16_t) { return unpacklo_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int32_t) { return unpacklo_epi32(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int64_t) { return unpacklo_epi64(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint8_t) { return unpacklo_epi8(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint16_t) { return unpacklo_epi16(lhs, rhs); }
	WJR_INTRINSIC_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint32_t) { return unpacklo_epi32(lhs, rhs); }
#endif // __AVX2__

	// extern :

	WJR_INTRINSIC_INLINE static simd_type connect(__m128i a, __m128i b) {
		return insert_si128<1>(simd_cast<__m256i>(a), b);
	}

	WJR_INTRINSIC_INLINE static simd_type preload_si16(const void* ptr) {
		return zextsi128_si256(sse_traits_t<simd_integer>::preload_si16(ptr));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si32(const void* ptr) {
		return zextsi128_si256(sse_traits_t<simd_integer>::preload_si32(ptr));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si48(const void* ptr) {
		return zextsi128_si256(sse_traits_t<simd_integer>::preload_si48(ptr));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si64(const void* ptr) {
		return zextsi128_si256(sse_traits_t<simd_integer>::preload_si64(ptr));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si80(const void* ptr) {
		return zextsi128_si256(sse_traits_t<simd_integer>::preload_si80(ptr));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si96(const void* ptr) {
		return zextsi128_si256(sse_traits_t<simd_integer>::preload_si96(ptr));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si112(const void* ptr) {
		return zextsi128_si256(sse_traits_t<simd_integer>::preload_si112(ptr));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si128(const void* ptr) {
		return zextsi128_si256(sse_traits_t<simd_integer>::preload_si128(ptr));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si144(const void* ptr) {
		return connect(sse_traits_t<simd_integer>::preload_si128(ptr), 
			sse_traits_t<simd_integer>::preload_si16((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si160(const void* ptr) {
		return connect(sse_traits_t<simd_integer>::preload_si128(ptr),
			sse_traits_t<simd_integer>::preload_si32((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si176(const void* ptr) {
		return connect(sse_traits_t<simd_integer>::preload_si128(ptr),
			sse_traits_t<simd_integer>::preload_si48((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si192(const void* ptr) {
		return connect(sse_traits_t<simd_integer>::preload_si128(ptr),
			sse_traits_t<simd_integer>::preload_si64((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si208(const void* ptr) {
		return connect(sse_traits_t<simd_integer>::preload_si128(ptr),
			sse_traits_t<simd_integer>::preload_si80((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si224(const void* ptr) {
		return connect(sse_traits_t<simd_integer>::preload_si128(ptr),
			sse_traits_t<simd_integer>::preload_si96((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si240(const void* ptr) {
		return connect(sse_traits_t<simd_integer>::preload_si128(ptr),
			sse_traits_t<simd_integer>::preload_si112((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static simd_type preload_si256(const void* ptr) {
		return loadu_all((const simd_type*)ptr);
	}

	WJR_INTRINSIC_INLINE static simd_type preload_si16x(const void* ptr, int n) {
		switch (n) {
		case 0:return set_all_zeros();
		case 1:return preload_si16(ptr);
		case 2:return preload_si32(ptr);
		case 3:return preload_si48(ptr);
		case 4:return preload_si64(ptr);
		case 5:return preload_si80(ptr);
		case 6:return preload_si96(ptr);
		case 7:return preload_si112(ptr);
		case 8:return preload_si128(ptr);
		case 9:return preload_si144(ptr);
		case 10:return preload_si160(ptr);
		case 11:return preload_si176(ptr);
		case 12:return preload_si192(ptr);
		case 13:return preload_si208(ptr);
		case 14:return preload_si224(ptr);
		case 15:return preload_si240(ptr);
		default:return preload_si256(ptr);
		}
	}

	WJR_INTRINSIC_INLINE static __m128i getlow_si128(simd_type a) {
		return simd_cast<__m128i>(a);
	}
	
	WJR_INTRINSIC_INLINE static __m128i gethigh_si128(simd_type a) {
		return extract_si128<1>(a);
	}

};

template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type 
	avx_traits_t<simd_integer>::cvtepu8<int16_t>(__m128i a) {
	return _mm256_cvtepu8_epi16(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type 
	avx_traits_t<simd_integer>::cvtepu8<int32_t>(__m128i a) {
	return _mm256_cvtepu8_epi32(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type
	avx_traits_t<simd_integer>::cvtepu8<int64_t>(__m128i a) {
	return _mm256_cvtepu8_epi64(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type
avx_traits_t<simd_integer>::cvtepu8<uint16_t>(__m128i a) {
	return cvtepu8<int16_t>(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type
avx_traits_t<simd_integer>::cvtepu8<uint32_t>(__m128i a) {
	return cvtepu8<int32_t>(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type
avx_traits_t<simd_integer>::cvtepu8<uint64_t>(__m128i a) {
	return cvtepu8<int64_t>(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type
avx_traits_t<simd_integer>::cvtepu16<int32_t>(__m128i a) {
	return _mm256_cvtepu16_epi32(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type
avx_traits_t<simd_integer>::cvtepu16<int64_t>(__m128i a) {
	return _mm256_cvtepu16_epi64(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type
avx_traits_t<simd_integer>::cvtepu16<uint32_t>(__m128i a) {
	return cvtepu16<int32_t>(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type
avx_traits_t<simd_integer>::cvtepu16<uint64_t>(__m128i a) {
	return cvtepu16<int64_t>(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type
avx_traits_t<simd_integer>::cvtepu32<int64_t>(__m128i a) {
	return _mm256_cvtepu32_epi64(a);
}
template<>
WJR_INTRINSIC_INLINE typename avx_traits_t<simd_integer>::simd_type
avx_traits_t<simd_integer>::cvtepu32<uint64_t>(__m128i a) {
	return cvtepu32<int64_t>(a);
}

#define __SIMD_REGISTER_FUNCTION(name, T)					\
template<typename...Args>									\
WJR_INTRINSIC_INLINE static decltype(auto) name(Args&&...args) {	\
	return base::name(std::forward<Args>(args)...,T{});		\
}

#define __SIMD_REGISTER_TEMPLATE_INT_FUNCTION(name, T)		                \
template<int imm8, typename...Args>							                \
WJR_INTRINSIC_INLINE static decltype(auto) name(Args&&...args) {	                \
	return base::template name<imm8>(std::forward<Args>(args)...,T{});		\
}

#define __SIMD_REGISTER_TEMPLATE_TYPE_FUNCTION(name, T)						\
template<typename X, typename...Args>										\
WJR_INTRINSIC_INLINE static decltype(auto) name(Args&&...args) {	                \
	return base::template name<X>(std::forward<Args>(args)...,T{});			\
}

#define __SIMD_REGISTER_ALL_COMMON_FUNCTIONS(T)						\
using value_type = T;												\
																	\
constexpr static int size(){ return base::width() /					\
	(sizeof(T) * 8); }												\
																	\
__SIMD_REGISTER_FUNCTION(setmin, T)									\
__SIMD_REGISTER_FUNCTION(setmax, T)									\
__SIMD_REGISTER_FUNCTION(set1, T)									\
																	\
__SIMD_REGISTER_FUNCTION(add, T)									\
__SIMD_REGISTER_FUNCTION(adds, T)									\
__SIMD_REGISTER_FUNCTION(avg, T)									\
__SIMD_REGISTER_FUNCTION(cmpeq, T)									\
__SIMD_REGISTER_FUNCTION(cmpge, T)									\
__SIMD_REGISTER_FUNCTION(cmpgt, T)									\
__SIMD_REGISTER_FUNCTION(cmple, T)									\
__SIMD_REGISTER_FUNCTION(cmplt, T)									\
__SIMD_REGISTER_FUNCTION(cmpne, T)									\
__SIMD_REGISTER_FUNCTION(cmp, T)									\
__SIMD_REGISTER_TEMPLATE_TYPE_FUNCTION(cvtepu, T)					\
__SIMD_REGISTER_TEMPLATE_INT_FUNCTION(extract, T)					\
__SIMD_REGISTER_FUNCTION(hadd, T)									\
__SIMD_REGISTER_FUNCTION(max, T)									\
__SIMD_REGISTER_FUNCTION(min, T)									\
__SIMD_REGISTER_FUNCTION(sll, T)									\
__SIMD_REGISTER_FUNCTION(slli, T)									\
__SIMD_REGISTER_FUNCTION(sra, T)									\
__SIMD_REGISTER_FUNCTION(srai, T)									\
__SIMD_REGISTER_FUNCTION(srl, T)									\
__SIMD_REGISTER_FUNCTION(srli, T)									\
__SIMD_REGISTER_FUNCTION(sub, T)									\
__SIMD_REGISTER_FUNCTION(subs, T)									\
__SIMD_REGISTER_FUNCTION(unpackhi, T)								\
__SIMD_REGISTER_FUNCTION(unpacklo, T)								

template<>
class traits<int8_t, simd_type::AVX>
	: public traits<simd_integer, simd_type::AVX> {
	using base = traits<simd_integer, simd::simd_type::AVX>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(int8_t);
};

template<>
class traits<uint8_t, simd_type::AVX>
	: public traits<simd_integer, simd_type::AVX> {
	using base = traits<simd_integer, simd::simd_type::AVX>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(uint8_t);
};

template<>
class traits<int16_t, simd_type::AVX>
	: public traits<simd_integer, simd_type::AVX> {
	using base = traits<simd_integer, simd::simd_type::AVX>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(int16_t);
};

template<>
class traits<uint16_t, simd_type::AVX>
	: public traits<simd_integer, simd_type::AVX> {
	using base = traits<simd_integer, simd::simd_type::AVX>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(uint16_t);
};

template<>
class traits<int32_t, simd_type::AVX>
	: public traits<simd_integer, simd_type::AVX> {
	using base = traits<simd_integer, simd::simd_type::AVX>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(int32_t);
};

template<>
class traits<uint32_t, simd_type::AVX>
	: public traits<simd_integer, simd_type::AVX> {
	using base = traits<simd_integer, simd::simd_type::AVX>;
public:

	__SIMD_REGISTER_ALL_COMMON_FUNCTIONS(uint32_t);
};

#undef __SIMD_REGISTER_FUNCTION
#undef __SIMD_REGISTER_TEMPLATE_TYPE_FUNCTION
#undef __SIMD_REGISTER_TEMPLATE_INT_FUNCTION
#undef __SIMD_REGISTER_ALL_COMMON_FUNCTIONS

#endif // __AVX__

_WJR_SIMD_END

#endif // __WJR_SIMD_SIMD_HELPER_H