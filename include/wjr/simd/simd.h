#pragma once
#ifndef __WJR_SIMD_H
#define __WJR_SIMD_H

#include <wjr/math.h>

_WJR_SIMD_BEGIN

enum class simd_type : unsigned int {
	SCALAR,
	SSE,
	AVX
};

#if defined(__AVX__)
#define __HAS_SIMD_AVX
#define __HAS_SIMD_AVXI
#define __HAS_SIMD_AVXD
#endif // __AVX__

#if defined(__SSE__)
#define __HAS_SIMD_SSE
#if defined(__SSE2__)
#define __HAS_SIMD_SSEI
#define __HAS_SIMD_SSED
#endif // __SSE2__
#endif // __SSE__

constexpr simd_type best_simd_type =
#if defined(__HAS_SIMD_AVX)
simd_type::AVX;
#elif defined(__HAS_SIMD_SSE)
simd_type::SSE;
#else
simd_type::SCALAR;
#endif

#if defined(WJR_FORCEINLINE)
#define __SIMD_INLINE inline WJR_FORCEINLINE
#else
#define __SIMD_INLINE inline
#endif

struct simd_integer {};
struct simd_float {};

struct pdata {
	alignas(32) static const uint32_t preload_mask[16];
};

template<typename T, simd_type S>
class traits;

template<typename T>
struct get_traits;

template<typename T>
using get_traits_t = typename get_traits<T>::type;

template<>
struct get_traits<__m128> {
	using type = traits<void, simd_type::SSE>;
};

template<>
struct get_traits<__m128i> {
	using type = traits<simd_integer, simd_type::SSE>;
};

template<>
struct get_traits<__m128d> {
	using type = traits<simd_float, simd_type::SSE>;
};

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

template<typename T>
struct __simd_cast;

template<typename T>
constexpr __simd_cast<T> simd_cast;

#define __REGISTER_SIMD_CAST_FUNCTION(VT,SUF)			\
WJR_HAS_SSE2_DEFINE(	                                \
__SIMD_INLINE VT operator()(__m128i v) const {	        \
	return _mm_cvtsi128_si##SUF(v);	                    \
}	                                                    \
)	                                                    \
WJR_HAS_AVX_DEFINE(	                                    \
__SIMD_INLINE VT operator()(__m256i v) const {	        \
	return _mm256_cvtsi256_si##SUF(v);	                \
}	                                                    \
)

template<>
struct __simd_cast<int8_t> {
	__REGISTER_SIMD_CAST_FUNCTION(int8_t, 32);
};

template<>
struct __simd_cast<uint8_t> {
	__REGISTER_SIMD_CAST_FUNCTION(uint8_t, 32);
};

template<>
struct __simd_cast<int16_t> {
	__REGISTER_SIMD_CAST_FUNCTION(int16_t, 32);
};

template<>
struct __simd_cast<uint16_t> {
	__REGISTER_SIMD_CAST_FUNCTION(uint16_t, 32);
};

template<>
struct __simd_cast<int32_t> {
	__REGISTER_SIMD_CAST_FUNCTION(int32_t, 32);
};

template<>
struct __simd_cast<uint32_t> {
	__REGISTER_SIMD_CAST_FUNCTION(uint32_t, 32);
};

template<>
struct __simd_cast<int64_t> {
	__REGISTER_SIMD_CAST_FUNCTION(int64_t, 64);
};

template<>
struct __simd_cast<uint64_t> {
	__REGISTER_SIMD_CAST_FUNCTION(uint64_t, 64);
};

#undef __REGISTER_SIMD_CAST_FUNCTION

#if defined(__MMX__)
#endif // __MMX__

#if defined(__SSE__)
#endif // __SSE__

#if defined(__SSE2__)

template<>
class traits<simd_integer, simd_type::SSE> {
public:
	using simd_type = __m128i;

	constexpr static int width() { return 128; }

	__SIMD_INLINE static simd_type set_all_zeros() { return _mm_setzero_si128(); }
	__SIMD_INLINE static simd_type set_all_ones() { return _mm_set1_epi32(-1); }

	__SIMD_INLINE static simd_type set1_epi8(int8_t val) {
		return _mm_set1_epi8(val);
	}
	__SIMD_INLINE static simd_type set1_epi16(int16_t val) {
		return _mm_set1_epi16(val);
	}
	__SIMD_INLINE static simd_type set1_epi32(int32_t val) {
		return _mm_set1_epi32(val);
	}
	__SIMD_INLINE static simd_type set1_epi64(int64_t val) {
		return _mm_set1_epi64x(val);
	}

	__SIMD_INLINE static simd_type set1(int8_t val, int8_t) { return set1_epi8(val); }
	__SIMD_INLINE static simd_type set1(int16_t val, int16_t) { return set1_epi16(val); }
	__SIMD_INLINE static simd_type set1(int32_t val, int32_t) { return set1_epi32(val); }
	__SIMD_INLINE static simd_type set1(int64_t val, int64_t) { return set1_epi64(val); }
	__SIMD_INLINE static simd_type set1(uint8_t val, uint8_t) { return set1_epi8(val); }
	__SIMD_INLINE static simd_type set1(uint16_t val, uint16_t) { return set1_epi16(val); }
	__SIMD_INLINE static simd_type set1(uint32_t val, uint32_t) { return set1_epi32(val); }
	__SIMD_INLINE static simd_type set1(uint64_t val, uint64_t) { return set1_epi64(val); }

	__SIMD_INLINE static simd_type setmin_epi8() { return set1_epi8(-0x80); }
	__SIMD_INLINE static simd_type setmin_epi16() { return set1_epi16(-0x8000); }
	__SIMD_INLINE static simd_type setmin_epi32() { return set1_epi32(-0x7fffffff - 1); }

	__SIMD_INLINE static simd_type setmin(int8_t) { return setmin_epi8(); }
	__SIMD_INLINE static simd_type setmin(int16_t) { return setmin_epi16(); }
	__SIMD_INLINE static simd_type setmin(int32_t) { return setmin_epi32(); }

	__SIMD_INLINE static simd_type setmax_epi8() { return set1_epi8(0x7F); }
	__SIMD_INLINE static simd_type setmax_epi16() { return set1_epi16(0x7FFF); }
	__SIMD_INLINE static simd_type setmax_epi32() { return set1_epi32(0x7FFFFFFF); }

	__SIMD_INLINE static simd_type setmax(int8_t) { return setmax_epi8(); }
	__SIMD_INLINE static simd_type setmax(int16_t) { return setmax_epi16(); }
	__SIMD_INLINE static simd_type setmax(int32_t) { return setmax_epi32(); }

	__SIMD_INLINE static simd_type load_all(const simd_type* ptr) { return _mm_load_si128(ptr); }
	__SIMD_INLINE static simd_type load(const simd_type* ptr) { return load_all(ptr); }
	__SIMD_INLINE static simd_type loadu_all(const simd_type* ptr) { return _mm_loadu_si128(ptr); }
	__SIMD_INLINE static simd_type loadu(const simd_type* ptr) { return loadu_all(ptr); }
	__SIMD_INLINE static simd_type loadu_si16(const void* ptr) { return _mm_loadu_si16(ptr); }
	__SIMD_INLINE static simd_type loadu_si32(const void* ptr) { return _mm_loadu_si32(ptr); }
	__SIMD_INLINE static simd_type loadu_si64(const void* ptr) { return _mm_loadu_si64(ptr); }
#if defined(__SSE3__)
	__SIMD_INLINE static simd_type lddqu_all(const simd_type* ptr) { return _mm_lddqu_si128(ptr); }
	__SIMD_INLINE static simd_type lddqu(const simd_type* ptr) { return lddqu_all(ptr); }
#endif // __SSE3__

	__SIMD_INLINE static void store_all(simd_type* ptr, simd_type val) { _mm_store_si128(ptr, val); }
	__SIMD_INLINE static void store(simd_type* ptr, simd_type val) { store_all(ptr, val); }
	__SIMD_INLINE static void storeu_all(simd_type* ptr, simd_type val) { _mm_storeu_si128(ptr, val); }
	__SIMD_INLINE static void storeu(simd_type* ptr, simd_type val) { storeu_all(ptr, val); }
	__SIMD_INLINE static void storeu_si16(void* ptr, simd_type val) { _mm_storeu_si16(ptr, val); }
	__SIMD_INLINE static void storeu_si32(void* ptr, simd_type val) { _mm_storeu_si32(ptr, val); }
	__SIMD_INLINE static void storeu_si64(void* ptr, simd_type val) { _mm_storeu_si64(ptr, val); }

	__SIMD_INLINE static simd_type And_all(simd_type lhs, simd_type rhs) { return _mm_and_si128(lhs, rhs); }
	__SIMD_INLINE static simd_type And(simd_type lhs, simd_type rhs) { return And_all(lhs, rhs); }

	__SIMD_INLINE static simd_type AndNot_all(simd_type lhs, simd_type rhs) { return _mm_andnot_si128(lhs, rhs); }
	__SIMD_INLINE static simd_type AndNot(simd_type lhs, simd_type rhs) { return AndNot_all(lhs, rhs); }

	__SIMD_INLINE static simd_type Or_all(simd_type lhs, simd_type rhs) { return _mm_or_si128(lhs, rhs); }
	__SIMD_INLINE static simd_type Or(simd_type lhs, simd_type rhs) { return Or_all(lhs, rhs); }

	__SIMD_INLINE static simd_type Xor_all(simd_type lhs, simd_type rhs) { return _mm_xor_si128(lhs, rhs); }
	__SIMD_INLINE static simd_type Xor(simd_type lhs, simd_type rhs) { return Xor_all(lhs, rhs); }

	__SIMD_INLINE static simd_type Not(simd_type v) { return Xor(v, set_all_ones()); }

#if defined(__SSSE3__)
	__SIMD_INLINE static simd_type abs_epi8(simd_type val) { return _mm_abs_epi8(val); }
	__SIMD_INLINE static simd_type abs_epi16(simd_type val) { return _mm_abs_epi16(val); }
	__SIMD_INLINE static simd_type abs_epi32(simd_type val) { return _mm_abs_epi32(val); }

	__SIMD_INLINE static simd_type abs(simd_type val, int8_t) { return abs_epi8(val); }
	__SIMD_INLINE static simd_type abs(simd_type val, int16_t) { return abs_epi16(val); }
	__SIMD_INLINE static simd_type abs(simd_type val, int32_t) { return abs_epi32(val); }
	__SIMD_INLINE static simd_type abs(simd_type val, uint8_t) { return val; }
	__SIMD_INLINE static simd_type abs(simd_type val, uint16_t) { return val; }
	__SIMD_INLINE static simd_type abs(simd_type val, uint32_t) { return val; }
#endif // __SSSE3__

	__SIMD_INLINE static simd_type add_epi8(simd_type lhs, simd_type rhs) { return _mm_add_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type add_epi16(simd_type lhs, simd_type rhs) { return _mm_add_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type add_epi32(simd_type lhs, simd_type rhs) { return _mm_add_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type add_epi64(simd_type lhs, simd_type rhs) { return _mm_add_epi64(lhs, rhs); }

	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, int8_t) { return add_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, int16_t) { return add_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, int32_t) { return add_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, int64_t) { return add_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint8_t) { return add_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint16_t) { return add_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint32_t) { return add_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint64_t) { return add_epi64(lhs, rhs); }

	__SIMD_INLINE static uint8_t add_epi8(simd_type a) {
		auto b = shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a);
		a = add_epi8(a, b);
		b = set_all_zeros();
		a = sad_epu8(a, b);
		return simd_cast<uint8_t>(a);
	}
	__SIMD_INLINE static uint16_t add_epi16(simd_type a) {
		a = add_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = add_epi16(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a));
		a = add_epi16(a, srli_all<2>(a));
		return simd_cast<uint16_t>(a);
	}
	__SIMD_INLINE static uint32_t add_epi32(simd_type a) {
		a = add_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = add_epi32(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a));
		return simd_cast<uint32_t>(a);
	}

	__SIMD_INLINE static int8_t add(simd_type a, int8_t) { return add_epi8(a); }
	__SIMD_INLINE static int16_t add(simd_type a, int16_t) { return add_epi16(a); }
	__SIMD_INLINE static int32_t add(simd_type a, int32_t) { return add_epi32(a); }
	__SIMD_INLINE static uint8_t add(simd_type a, uint8_t) { return add_epi8(a); }
	__SIMD_INLINE static uint16_t add(simd_type a, uint16_t) { return add_epi16(a); }
	__SIMD_INLINE static uint32_t add(simd_type a, uint32_t) { return add_epi32(a); }

	__SIMD_INLINE static simd_type adds_epi8(simd_type lhs, simd_type rhs) { return _mm_adds_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type adds_epi16(simd_type lhs, simd_type rhs) { return _mm_adds_epi16(lhs, rhs); }

	__SIMD_INLINE static simd_type adds_epu8(simd_type lhs, simd_type rhs) { return _mm_adds_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type adds_epu16(simd_type lhs, simd_type rhs) { return _mm_adds_epu16(lhs, rhs); }

	__SIMD_INLINE static simd_type adds(simd_type lhs, simd_type rhs, int8_t) { return adds_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type adds(simd_type lhs, simd_type rhs, int16_t) { return adds_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type adds(simd_type lhs, simd_type rhs, uint8_t) { return adds_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type adds(simd_type lhs, simd_type rhs, uint16_t) { return adds_epu16(lhs, rhs); }

	__SIMD_INLINE static simd_type avg_epu8(simd_type lhs, simd_type rhs) { return _mm_avg_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type avg_epu16(simd_type lhs, simd_type rhs) { return _mm_avg_epu16(lhs, rhs); }

	__SIMD_INLINE static simd_type avg(simd_type lhs, simd_type rhs, int8_t) { return avg_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type avg(simd_type lhs, simd_type rhs, int16_t) { return avg_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type avg(simd_type lhs, simd_type rhs, uint8_t) { return avg_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type avg(simd_type lhs, simd_type rhs, uint16_t) { return avg_epu16(lhs, rhs); }

#if defined(__SSE4_1__)
	__SIMD_INLINE static simd_type blendv_epi8(simd_type lhs, simd_type rhs, simd_type mask) {
		return _mm_blendv_epi8(lhs, rhs, mask);
	}
#else
	__SIMD_INLINE static simd_type blendv_epi8(simd_type lhs, simd_type rhs, simd_type mask) {
		return Or(AndNot(mask, lhs), And(mask, rhs));
	}
#endif // __SSE4_1__

	template<int imm8>
	__SIMD_INLINE static simd_type bslli_epi128(simd_type val) { return _mm_bslli_si128(val, imm8); }

	template<int imm8>
	__SIMD_INLINE static simd_type bsrli_epi128(simd_type val) { return _mm_bsrli_si128(val, imm8); }

	__SIMD_INLINE static simd_type cmpeq_epi8(simd_type lhs, simd_type rhs) { return _mm_cmpeq_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq_epi16(simd_type lhs, simd_type rhs) { return _mm_cmpeq_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq_epi32(simd_type lhs, simd_type rhs) { return _mm_cmpeq_epi32(lhs, rhs); }
#if defined(__SSE4_1__)
	__SIMD_INLINE static simd_type cmpeq_epi64(simd_type lhs, simd_type rhs) { return _mm_cmpeq_epi64(lhs, rhs); }
#endif // __SSE4_1__

	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int8_t) { return cmpeq_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int16_t) { return cmpeq_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int32_t) { return cmpeq_epi32(lhs, rhs); }
#if defined(__SSE4_1__)
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int64_t) { return cmpeq_epi64(lhs, rhs); }
#endif // __SSE4_1__
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint8_t) { return cmpeq_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint16_t) { return cmpeq_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint32_t) { return cmpeq_epi32(lhs, rhs); }
#if defined(__SSE4_1__)
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint64_t) { return cmpeq_epi64(lhs, rhs); }
#endif // __SSE4_1__

	__SIMD_INLINE static simd_type cmpge_epi8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi8(rhs, lhs));
	}
	__SIMD_INLINE static simd_type cmpge_epi16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi16(rhs, lhs));
	}
	__SIMD_INLINE static simd_type cmpge_epi32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi32(rhs, lhs));
	}

	__SIMD_INLINE static simd_type cmpge_epu8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu8(rhs, lhs));
	}
	__SIMD_INLINE static simd_type cmpge_epu16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu16(rhs, lhs));
	}
	__SIMD_INLINE static simd_type cmpge_epu32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu32(rhs, lhs));
	}

	__SIMD_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, int8_t) { return cmpge_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, int16_t) { return cmpge_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, int32_t) { return cmpge_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, uint8_t) { return cmpge_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, uint16_t) { return cmpge_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpge(simd_type lhs, simd_type rhs, uint32_t) { return cmpge_epu32(lhs, rhs); }

	__SIMD_INLINE static simd_type cmpgt_epi8(simd_type lhs, simd_type rhs) { return _mm_cmpgt_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt_epi16(simd_type lhs, simd_type rhs) { return _mm_cmpgt_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt_epi32(simd_type lhs, simd_type rhs) { return _mm_cmpgt_epi32(lhs, rhs); }
#if defined(__SSE4_1__)
	__SIMD_INLINE static simd_type cmpgt_epi64(simd_type lhs, simd_type rhs) { return _mm_cmpgt_epi64(lhs, rhs); }
#endif // __SSE4_1__

	__SIMD_INLINE static simd_type cmpgt_epu8(simd_type lhs, simd_type rhs) {
#if defined(__XOP__)	
		return _mm_comgt_epu8(lhs, rhs);
#else
		return cmpgt_epi8(Xor(lhs, setmin_epi8()), Xor(rhs, setmin_epi8()));
#endif // __XOP__
	}
	__SIMD_INLINE static simd_type cmpgt_epu16(simd_type lhs, simd_type rhs) {
#if defined(__XOP__) || defined(_mm_comgt_epu16)
		return _mm_comgt_epu16(lhs, rhs);
#else
		return cmpgt_epi16(Xor(lhs, setmin_epi16()), Xor(rhs, setmin_epi16()));
#endif // __XOP__
	}
	__SIMD_INLINE static simd_type cmpgt_epu32(simd_type lhs, simd_type rhs) {
#if defined(__XOP__) || defined(_mm_comgt_epu32)
		return _mm_comgt_epu32(lhs, rhs);
#else
		return cmpgt_epi32(Xor(lhs, setmin_epi32()), Xor(rhs, setmin_epi32()));
#endif // __XOP__
	}

	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int8_t) { return cmpgt_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int16_t) { return cmpgt_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int32_t) { return cmpgt_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint8_t) { return cmpgt_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint16_t) { return cmpgt_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint32_t) { return cmpgt_epu32(lhs, rhs); }

	__SIMD_INLINE static simd_type cmple_epi8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi8(lhs, rhs));
	}
	__SIMD_INLINE static simd_type cmple_epi16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi16(lhs, rhs));
	}
	__SIMD_INLINE static simd_type cmple_epi32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epi32(lhs, rhs));
	}

	__SIMD_INLINE static simd_type cmple_epu8(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu8(lhs, rhs));
	}
	__SIMD_INLINE static simd_type cmple_epu16(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu16(lhs, rhs));
	}
	__SIMD_INLINE static simd_type cmple_epu32(simd_type lhs, simd_type rhs) {
		return Not(cmpgt_epu32(lhs, rhs));
	}

	__SIMD_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, int8_t) { return cmple_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, int16_t) { return cmple_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, int32_t) { return cmple_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, uint8_t) { return cmple_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, uint16_t) { return cmple_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmple(simd_type lhs, simd_type rhs, uint32_t) { return cmple_epu32(lhs, rhs); }

	__SIMD_INLINE static simd_type cmplt_epi8(simd_type lhs, simd_type rhs) { return _mm_cmplt_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt_epi16(simd_type lhs, simd_type rhs) { return _mm_cmplt_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt_epi32(simd_type lhs, simd_type rhs) { return _mm_cmplt_epi32(lhs, rhs); }

	__SIMD_INLINE static simd_type cmplt_epu8(simd_type lhs, simd_type rhs) {
		return cmplt_epi8(Xor(lhs, setmin_epi8()), Xor(rhs, setmin_epi8()));
	}
	__SIMD_INLINE static simd_type cmplt_epu16(simd_type lhs, simd_type rhs) {
		return cmplt_epi16(Xor(lhs, setmin_epi16()), Xor(rhs, setmin_epi16()));
	}
	__SIMD_INLINE static simd_type cmplt_epu32(simd_type lhs, simd_type rhs) {
		return cmplt_epi32(Xor(lhs, setmin_epi32()), Xor(rhs, setmin_epi32()));
	}

	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int8_t) { return cmplt_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int16_t) { return cmplt_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int32_t) { return cmplt_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint8_t) { return cmplt_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint16_t) { return cmplt_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint32_t) { return cmplt_epu32(lhs, rhs); }

#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static int extract_epi8(simd_type lhs) {
		return _mm_extract_epi8(lhs, imm8);
	}
#endif // __SSE4_1__
	template<int imm8>
	__SIMD_INLINE static int extract_epi16(simd_type lhs) {
		return _mm_extract_epi16(lhs, imm8);
	}
#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static int extract_epi32(simd_type lhs) {
		return _mm_extract_epi32(lhs, imm8);
	}
	template<int imm8>
	__SIMD_INLINE static int64_t extract_epi64(simd_type lhs) {
		return _mm_extract_epi64(lhs, imm8);
	}
#endif // __SSE4_1__

#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static int extract(simd_type lhs, int8_t) { return extract_epi8<imm8>(lhs); }
#endif // __SSE4_1__
	template<int imm8>
	__SIMD_INLINE static int extract(simd_type lhs, int16_t) { return extract_epi16<imm8>(lhs); }
#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static int extract(simd_type lhs, int32_t) { return extract_epi32<imm8>(lhs); }
	template<int imm8>
	__SIMD_INLINE static int64_t extract(simd_type lhs, int64_t) { return extract_epi64<imm8>(lhs); }
#endif // __SSE4_1__
#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static int extract(simd_type lhs, uint8_t) { return extract_epi8<imm8>(lhs); }
#endif // __SSE4_1__
	template<int imm8>
	__SIMD_INLINE static int extract(simd_type lhs, uint16_t) { return extract_epi16<imm8>(lhs); }
#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static int extract(simd_type lhs, uint32_t) { return extract_epi32<imm8>(lhs); }
	template<int imm8>
	__SIMD_INLINE static int64_t extract(simd_type lhs, uint64_t) { return extract_epi64<imm8>(lhs); }
#endif // __SSE4_1__

#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static simd_type insert_epi8(simd_type lhs, int i) {
		return _mm_insert_epi8(lhs, i, imm8);
	}
#endif // __SSE4_1__
	template<int imm8>
	__SIMD_INLINE static simd_type insert_epi16(simd_type lhs, int i) {
		return _mm_insert_epi16(lhs, i, imm8);
	}
#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static simd_type insert_epi32(simd_type lhs, int i) {
		return _mm_insert_epi32(lhs, i, imm8);
	}
	template<int imm8>
	__SIMD_INLINE static simd_type insert_epi64(simd_type lhs, int64_t i) {
		return _mm_insert_epi64(lhs, i, imm8);
	}
#endif // __SSE4_1__

#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static simd_type insert(simd_type lhs, int i, int8_t) { return insert_epi8<imm8>(lhs, i); }
#endif // __SSE4_1__
	template<int imm8>
	__SIMD_INLINE static simd_type insert(simd_type lhs, int i, int16_t) { return insert_epi16<imm8>(lhs, i); }
#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static simd_type insert(simd_type lhs, int i, int32_t) { return insert_epi32<imm8>(lhs, i); }
	template<int imm8>
	__SIMD_INLINE static simd_type insert(simd_type lhs, int64_t i, int64_t) { return insert_epi64<imm8>(lhs, i); }
#endif // __SSE4_1__
#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static simd_type insert(simd_type lhs, int i, uint8_t) { return insert_epi8<imm8>(lhs, i); }
#endif // __SSE4_1__
	template<int imm8>
	__SIMD_INLINE static simd_type insert(simd_type lhs, int i, uint16_t) { return insert_epi16<imm8>(lhs, i); }
#if defined(__SSE4_1__)
	template<int imm8>
	__SIMD_INLINE static simd_type insert(simd_type lhs, int i, uint32_t) { return insert_epi32<imm8>(lhs, i); }
	template<int imm8>
	__SIMD_INLINE static simd_type insert(simd_type lhs, int64_t i, uint64_t) { return insert_epi64<imm8>(lhs, i); }
#endif // __SSE4_1__

	__SIMD_INLINE static simd_type madd_epi16(simd_type lhs, simd_type rhs) {
		return _mm_madd_epi16(lhs, rhs);
	}

	__SIMD_INLINE static void maskmoveu_all(simd_type lhs, simd_type mask, char* mem_addr) {
		return _mm_maskmoveu_si128(lhs, mask, mem_addr);
	}
	__SIMD_INLINE static void maskmoveu(simd_type lhs, simd_type mask, char* mem_addr) {
		return maskmoveu_all(lhs, mask, mem_addr);
	}

	__SIMD_INLINE static simd_type max_epi16(simd_type lhs, simd_type rhs) {
		return _mm_max_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type max_epu8(simd_type lhs, simd_type rhs) {
		return _mm_max_epu8(lhs, rhs);
	}
#if defined(__SSE4_1__)
	__SIMD_INLINE static simd_type max_epi8(simd_type lhs, simd_type rhs) {
		return _mm_max_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type max_epi32(simd_type lhs, simd_type rhs) {
		return _mm_max_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type max_epu16(simd_type lhs, simd_type rhs) {
		return _mm_max_epu16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type max_epu32(simd_type lhs, simd_type rhs) {
		return _mm_max_epu32(lhs, rhs);
	}

#else
	__SIMD_INLINE static simd_type max_epi8(simd_type lhs, simd_type rhs) {
		return blendv(rhs, lhs, cmpgt_epi8(lhs, rhs));
	}
	__SIMD_INLINE static simd_type max_epi32(simd_type lhs, simd_type rhs) {
		return blendv(rhs, lhs, cmpgt_epi32(lhs, rhs));
	}
	__SIMD_INLINE static simd_type max_epu16(simd_type lhs, simd_type rhs) {
		return blendv(rhs, lhs, cmpgt_epu16(lhs, rhs));
	}
	__SIMD_INLINE static simd_type max_epu32(simd_type lhs, simd_type rhs) {
		return blendv(rhs, lhs, cmpgt_epu32(lhs, rhs));
	}
#endif
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, int8_t) { return max_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, int16_t) { return max_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, int32_t) { return max_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint8_t) { return max_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint16_t) { return max_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint32_t) { return max_epu32(lhs, rhs); }

	__SIMD_INLINE static int8_t max_epi8(simd_type a) {
		a = max_epi8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epi8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epi8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		auto X = simd_cast<uint32_t>(a);
		return std::max((int8_t)X, (int8_t)(X >> 8));
	}
	__SIMD_INLINE static int16_t max_epi16(simd_type a) {
		a = max_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<int16_t>(a);
	}
	__SIMD_INLINE static int32_t max_epi32(simd_type a) {
		a = max_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<int32_t>(a);
	}
	__SIMD_INLINE static uint8_t max_epu8(simd_type a) {
		a = max_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		auto X = simd_cast<uint32_t>(a);
		return std::max((uint8_t)X, (uint8_t)(X >> 8));
	}
	__SIMD_INLINE static uint16_t max_epu16(simd_type a) {
		a = max_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<uint16_t>(a);
	}
	__SIMD_INLINE static uint32_t max_epu32(simd_type a) {
		a = max_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<uint32_t>(a);
	}

	__SIMD_INLINE static int8_t max(simd_type a, int8_t) { return max_epi8(a); }
	__SIMD_INLINE static int16_t max(simd_type a, int16_t) { return max_epi16(a); }
	__SIMD_INLINE static int32_t max(simd_type a, int32_t) { return max_epi32(a); }
	__SIMD_INLINE static uint8_t max(simd_type a, uint8_t) { return max_epu8(a); }
	__SIMD_INLINE static uint16_t max(simd_type a, uint16_t) { return max_epu16(a); }
	__SIMD_INLINE static uint32_t max(simd_type a, uint32_t) { return max_epu32(a); }

	__SIMD_INLINE static simd_type min_epi16(simd_type lhs, simd_type rhs) {
		return _mm_min_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type min_epu8(simd_type lhs, simd_type rhs) {
		return _mm_min_epu8(lhs, rhs);
	}
#if defined(__SSE4_1__)
	__SIMD_INLINE static simd_type min_epi8(simd_type lhs, simd_type rhs) {
		return _mm_min_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type min_epi32(simd_type lhs, simd_type rhs) {
		return _mm_min_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type min_epu16(simd_type lhs, simd_type rhs) {
		return _mm_min_epu16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type min_epu32(simd_type lhs, simd_type rhs) {
		return _mm_min_epu32(lhs, rhs);
	}
#else
	__SIMD_INLINE static simd_type min_epi8(simd_type lhs, simd_type rhs) {
		return blendv(lhs, rhs, cmpgt_epi8(lhs, rhs));
	}
	__SIMD_INLINE static simd_type min_epi32(simd_type lhs, simd_type rhs) {
		return blendv(lhs, rhs, cmpgt_epi32(lhs, rhs));
	}
	__SIMD_INLINE static simd_type min_epu16(simd_type lhs, simd_type rhs) {
		return blendv(lhs, rhs, cmpgt_epu16(lhs, rhs));
	}
	__SIMD_INLINE static simd_type min_epu32(simd_type lhs, simd_type rhs) {
		return blendv(lhs, rhs, cmpgt_epu32(lhs, rhs));
	}
#endif // __SSE4_1__
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, int8_t) { return min_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, int16_t) { return min_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, int32_t) { return min_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint8_t) { return min_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint16_t) { return min_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint32_t) { return min_epu32(lhs, rhs); }

	__SIMD_INLINE static int8_t min_epi8(simd_type a) {
		a = min_epi8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epi8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epi8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		auto X = simd_cast<uint32_t>(a);
		return std::min((int8_t)X, (int8_t)(X >> 8));
	}
	__SIMD_INLINE static int16_t min_epi16(simd_type a) {
		a = min_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<int16_t>(a);
	}
	__SIMD_INLINE static int32_t min_epi32(simd_type a) {
		a = min_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<int32_t>(a);
	}
	__SIMD_INLINE static uint8_t min_epu8(simd_type a) {
		a = min_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		auto X = simd_cast<uint32_t>(a);
		return std::min((uint8_t)X, (uint8_t)(X >> 8));
	}
	__SIMD_INLINE static uint16_t min_epu16(simd_type a) {
		a = min_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<uint16_t>(a);
	}
	__SIMD_INLINE static uint32_t min_epu32(simd_type a) {
		a = min_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<uint32_t>(a);
	}

	__SIMD_INLINE static int8_t min(simd_type a, int8_t) { return min_epi8(a); }
	__SIMD_INLINE static int16_t min(simd_type a, int16_t) { return min_epi16(a); }
	__SIMD_INLINE static int32_t min(simd_type a, int32_t) { return min_epi32(a); }
	__SIMD_INLINE static uint8_t min(simd_type a, uint8_t) { return min_epu8(a); }
	__SIMD_INLINE static uint16_t min(simd_type a, uint16_t) { return min_epu16(a); }
	__SIMD_INLINE static uint32_t min(simd_type a, uint32_t) { return min_epu32(a); }

	__SIMD_INLINE static simd_type move_epi64(simd_type lhs) {
		return _mm_move_epi64(lhs);
	}

	__SIMD_INLINE static int movemask_epi8(simd_type lhs) {
		return _mm_movemask_epi8(lhs);
	}

#if defined(__SSE4_1__)
	__SIMD_INLINE static simd_type mul_epi32(simd_type lhs, simd_type rhs) {
		return _mm_mul_epi32(lhs, rhs);
	}
#endif // __SSE4_1__
	__SIMD_INLINE static simd_type mul_epu32(simd_type lhs, simd_type rhs) {
		return _mm_mul_epu32(lhs, rhs);
	}

	__SIMD_INLINE static simd_type mulhi_epi16(simd_type lhs, simd_type rhs) {
		return _mm_mulhi_epi16(lhs, rhs);
	}

	__SIMD_INLINE static simd_type mulhi_epu16(simd_type lhs, simd_type rhs) {
		return _mm_mulhi_epu16(lhs, rhs);
	}

	__SIMD_INLINE static simd_type mullo_epi16(simd_type lhs, simd_type rhs) {
		return _mm_mullo_epi16(lhs, rhs);
	}
#if defined(__SSE4_1__)
	__SIMD_INLINE static simd_type mullo_epi32(simd_type lhs, simd_type rhs) {
		return _mm_mullo_epi32(lhs, rhs);
	}
#endif // __SSE4_1__

	__SIMD_INLINE static simd_type packs_epi16(simd_type lhs, simd_type rhs) {
		return _mm_packs_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type packs_epi32(simd_type lhs, simd_type rhs) {
		return _mm_packs_epi32(lhs, rhs);
	}

	__SIMD_INLINE static simd_type packus_epi16(simd_type lhs, simd_type rhs) {
		return _mm_packus_epi16(lhs, rhs);
	}
#if defined(__SSE4_1__)
	__SIMD_INLINE static simd_type packus_epi32(simd_type lhs, simd_type rhs) {
		return _mm_packus_epi32(lhs, rhs);
	}
#endif // __SSE4_1__

	__SIMD_INLINE static simd_type sad_epu8(simd_type lhs, simd_type rhs) {
		return _mm_sad_epu8(lhs, rhs);
	}

#if defined(__SSSE3__)
	template<int imm8>
	__SIMD_INLINE static simd_type shuffle_epi8(simd_type v) {
		return _mm_shuffle_epi8(v, imm8);
	}
#endif // __SSSE3__
	template<int imm8>
	__SIMD_INLINE static simd_type shuffle_epi32(simd_type v) {
		return _mm_shuffle_epi32(v, imm8);
	}

	template<int imm8>
	__SIMD_INLINE static simd_type shufflehi_epi16(simd_type v) {
		return _mm_shufflehi_epi16(v, imm8);
	}

	template<int imm8>
	__SIMD_INLINE static simd_type shufflelo_epi16(simd_type v) {
		return _mm_shufflelo_epi16(v, imm8);
	}

	__SIMD_INLINE static simd_type sign_epi8(simd_type lhs, simd_type rhs) {
		return _mm_sign_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sign_epi16(simd_type lhs, simd_type rhs) {
		return _mm_sign_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sign_epi32(simd_type lhs, simd_type rhs) {
		return _mm_sign_epi32(lhs, rhs);
	}

#if defined(__SSSE3__)
	__SIMD_INLINE static simd_type sign(simd_type lhs, simd_type rhs, int8_t) { return sign_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type sign(simd_type lhs, simd_type rhs, int16_t) { return sign_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sign(simd_type lhs, simd_type rhs, int32_t) { return sign_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type sign(simd_type lhs, simd_type rhs, uint8_t) { return sign_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type sign(simd_type lhs, simd_type rhs, uint16_t) { return sign_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sign(simd_type lhs, simd_type rhs, uint32_t) { return sign_epi32(lhs, rhs); }
#endif // __SSSE3__

	__SIMD_INLINE static simd_type sll_epi16(simd_type lhs, simd_type rhs) {
		return _mm_sll_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sll_epi32(simd_type lhs, simd_type rhs) {
		return _mm_sll_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sll_epi64(simd_type lhs, simd_type rhs) {
		return _mm_sll_epi64(lhs, rhs);
	}

	__SIMD_INLINE static simd_type sll(simd_type lhs, simd_type rhs, int16_t) { return sll_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sll(simd_type lhs, simd_type rhs, int32_t) { return sll_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type sll(simd_type lhs, simd_type rhs, int64_t) { return sll_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type sll(simd_type lhs, simd_type rhs, uint16_t) { return sll_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sll(simd_type lhs, simd_type rhs, uint32_t) { return sll_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type sll(simd_type lhs, simd_type rhs, uint64_t) { return sll_epi64(lhs, rhs); }

	template<int imm8>
	__SIMD_INLINE static simd_type slli_all(simd_type v) {
		return _mm_slli_si128(v, imm8);
	}
	template<int imm8>
	__SIMD_INLINE static simd_type slli(simd_type v) {
		return slli_all(v);
	}
	__SIMD_INLINE static simd_type slli_epi16(simd_type lhs, int imm8) {
		return _mm_slli_epi16(lhs, imm8);
	}
	__SIMD_INLINE static simd_type slli_epi32(simd_type lhs, int imm8) {
		return _mm_slli_epi32(lhs, imm8);
	}
	__SIMD_INLINE static simd_type slli_epi64(simd_type lhs, int imm8) {
		return _mm_slli_epi64(lhs, imm8);
	}

	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, int16_t) { return slli_epi16(lhs, imm8); }
	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, int32_t) { return slli_epi32(lhs, imm8); }
	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, int64_t) { return slli_epi64(lhs, imm8); }
	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, uint16_t) { return slli_epi16(lhs, imm8); }
	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, uint32_t) { return slli_epi32(lhs, imm8); }
	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, uint64_t) { return slli_epi64(lhs, imm8); }

	__SIMD_INLINE static simd_type sra_epi16(simd_type lhs, simd_type rhs) {
		return _mm_sra_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sra_epi32(simd_type lhs, simd_type rhs) {
		return _mm_sra_epi32(lhs, rhs);
	}

	__SIMD_INLINE static simd_type sra(simd_type lhs, simd_type rhs, int16_t) { return sra_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sra(simd_type lhs, simd_type rhs, int32_t) { return sra_epi32(lhs, rhs); }

	__SIMD_INLINE static simd_type srai_epi16(simd_type lhs, int imm8) {
		return _mm_srai_epi16(lhs, imm8);
	}
	__SIMD_INLINE static simd_type srai_epi32(simd_type lhs, int imm8) {
		return _mm_srai_epi32(lhs, imm8);
	}

	__SIMD_INLINE static simd_type srai(simd_type lhs, int imm8, int16_t) { return srai_epi16(lhs, imm8); }
	__SIMD_INLINE static simd_type srai(simd_type lhs, int imm8, int32_t) { return srai_epi32(lhs, imm8); }

	__SIMD_INLINE static simd_type srl_epi16(simd_type lhs, simd_type rhs) {
		return _mm_srl_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type srl_epi32(simd_type lhs, simd_type rhs) {
		return _mm_srl_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type srl_epi64(simd_type lhs, simd_type rhs) {
		return _mm_srl_epi64(lhs, rhs);
	}

	__SIMD_INLINE static simd_type srl(simd_type lhs, simd_type rhs, int16_t) { return srl_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type srl(simd_type lhs, simd_type rhs, int32_t) { return srl_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type srl(simd_type lhs, simd_type rhs, int64_t) { return srl_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type srl(simd_type lhs, simd_type rhs, uint16_t) { return srl_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type srl(simd_type lhs, simd_type rhs, uint32_t) { return srl_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type srl(simd_type lhs, simd_type rhs, uint64_t) { return srl_epi64(lhs, rhs); }

	template<int imm8>
	__SIMD_INLINE static simd_type srli_all(simd_type v) {
		return _mm_srli_si128(v, imm8);
	}
	template<int imm8>
	__SIMD_INLINE static simd_type srli(simd_type v) {
		return srli_all<imm8>(v);
	}
	__SIMD_INLINE static simd_type srli_epi16(simd_type lhs, int imm8) {
		return _mm_srli_epi16(lhs, imm8);
	}
	__SIMD_INLINE static simd_type srli_epi32(simd_type lhs, int imm8) {
		return _mm_srli_epi32(lhs, imm8);
	}
	__SIMD_INLINE static simd_type srli_epi64(simd_type lhs, int imm8) {
		return _mm_srli_epi64(lhs, imm8);
	}

	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, int16_t) { return srli_epi16(lhs, imm8); }
	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, int32_t) { return srli_epi32(lhs, imm8); }
	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, int64_t) { return srli_epi64(lhs, imm8); }
	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, uint16_t) { return srli_epi16(lhs, imm8); }
	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, uint32_t) { return srli_epi32(lhs, imm8); }
	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, uint64_t) { return srli_epi64(lhs, imm8); }

	__SIMD_INLINE static simd_type sub_epi8(simd_type lhs, simd_type rhs) {
		return _mm_sub_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sub_epi16(simd_type lhs, simd_type rhs) {
		return _mm_sub_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sub_epi32(simd_type lhs, simd_type rhs) {
		return _mm_sub_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sub_epi64(simd_type lhs, simd_type rhs) {
		return _mm_sub_epi64(lhs, rhs);
	}

	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int8_t) { return sub_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int16_t) { return sub_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int32_t) { return sub_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int64_t) { return sub_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint8_t) { return sub_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint16_t) { return sub_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint32_t) { return sub_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint64_t) { return sub_epi64(lhs, rhs); }

	__SIMD_INLINE static simd_type subs_epi8(simd_type lhs, simd_type rhs) {
		return _mm_subs_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type subs_epi16(simd_type lhs, simd_type rhs) {
		return _mm_subs_epi16(lhs, rhs);
	}

	__SIMD_INLINE static simd_type subs_epu8(simd_type lhs, simd_type rhs) {
		return _mm_subs_epu8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type subs_epu16(simd_type lhs, simd_type rhs) {
		return _mm_subs_epu16(lhs, rhs);
	}

	__SIMD_INLINE static simd_type subs(simd_type lhs, simd_type rhs, int8_t) { return subs_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type subs(simd_type lhs, simd_type rhs, int16_t) { return subs_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type subs(simd_type lhs, simd_type rhs, uint8_t) { return subs_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type subs(simd_type lhs, simd_type rhs, uint16_t) { return subs_epu16(lhs, rhs); }

#if defined(__SSE4_1__)
	__SIMD_INLINE static int test_all_ones(simd_type lhs) {
		return _mm_test_all_ones(lhs);
	}

	__SIMD_INLINE static int test_all_zeros(simd_type lhs, simd_type rhs) {
		return _mm_test_all_zeros(lhs, rhs);
	}

	__SIMD_INLINE static int test_mix_ones_zeros(simd_type lhs, simd_type rhs) {
		return _mm_test_mix_ones_zeros(lhs, rhs);
	}

	__SIMD_INLINE static int testc_si128(simd_type lhs, simd_type rhs) {
		return _mm_testc_si128(lhs, rhs);
	}

	__SIMD_INLINE static int testnzc_si128(simd_type lhs, simd_type rhs) {
		return _mm_testnzc_si128(lhs, rhs);
	}

	__SIMD_INLINE static int testz_si128(simd_type lhs, simd_type rhs) {
		return _mm_testz_si128(lhs, rhs);
	}
#endif // __SSE4_1__

	__SIMD_INLINE static simd_type unpackhi_epi8(simd_type lhs, simd_type rhs) {
		return _mm_unpackhi_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpackhi_epi16(simd_type lhs, simd_type rhs) {
		return _mm_unpackhi_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpackhi_epi32(simd_type lhs, simd_type rhs) {
		return _mm_unpackhi_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpackhi_epi64(simd_type lhs, simd_type rhs) {
		return _mm_unpackhi_epi64(lhs, rhs);
	}

	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int8_t) { return unpackhi_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int16_t) { return unpackhi_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int32_t) { return unpackhi_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int64_t) { return unpackhi_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint8_t) { return unpackhi_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint16_t) { return unpackhi_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint32_t) { return unpackhi_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint64_t) { return unpackhi_epi64(lhs, rhs); }

	__SIMD_INLINE static simd_type unpacklo_epi8(simd_type lhs, simd_type rhs) {
		return _mm_unpacklo_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpacklo_epi16(simd_type lhs, simd_type rhs) {
		return _mm_unpacklo_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpacklo_epi32(simd_type lhs, simd_type rhs) {
		return _mm_unpacklo_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpacklo_epi64(simd_type lhs, simd_type rhs) {
		return _mm_unpacklo_epi64(lhs, rhs);
	}

	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int8_t) { return unpacklo_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int16_t) { return unpacklo_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int32_t) { return unpacklo_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int64_t) { return unpacklo_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint8_t) { return unpacklo_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint16_t) { return unpacklo_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint32_t) { return unpacklo_epi32(lhs, rhs); }

};

#define __SIMD_REGISTER_FUNCTION(name, T)					\
template<typename...Args>									\
__SIMD_INLINE static decltype(auto) name(Args&&...args) {	\
	return base::name(std::forward<Args>(args)...,T{});		\
}

#define __SIMD_REGISTER_TEMPLATE_INT_FUNCTION(name, T)		                \
template<int imm8, typename...Args>							                \
__SIMD_INLINE static decltype(auto) name(Args&&...args) {	                \
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
__SIMD_REGISTER_FUNCTION(abs, T)									\
__SIMD_REGISTER_FUNCTION(add, T)									\
__SIMD_REGISTER_FUNCTION(adds, T)									\
__SIMD_REGISTER_FUNCTION(avg, T)									\
__SIMD_REGISTER_FUNCTION(cmpeq, T)									\
__SIMD_REGISTER_FUNCTION(cmpgt, T)									\
__SIMD_REGISTER_FUNCTION(cmplt, T)									\
__SIMD_REGISTER_TEMPLATE_INT_FUNCTION(extract, T)					\
__SIMD_REGISTER_TEMPLATE_INT_FUNCTION(insert, T)					\
__SIMD_REGISTER_FUNCTION(max, T)									\
__SIMD_REGISTER_FUNCTION(min, T)									\
__SIMD_REGISTER_FUNCTION(sign, T)									\
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
#undef __SIMD_REGISTER_ALL_COMMON_FUNCTIONS

#endif // __SSE2__

#if defined(__AVX__)

template<>
class traits<simd_integer, simd_type::AVX> {
public:
	using simd_type = __m256i;

	constexpr static int width() { return 256; }

	__SIMD_INLINE static simd_type setzero() { return _mm256_setzero_si256(); }
	__SIMD_INLINE static simd_type setallone() { return _mm256_set1_epi32(-1); }

	__SIMD_INLINE static simd_type set1_epi8(int8_t a) { return _mm256_set1_epi8(a); }
	__SIMD_INLINE static simd_type set1_epi16(int16_t a) { return _mm256_set1_epi16(a); }
	__SIMD_INLINE static simd_type set1_epi32(int32_t a) { return _mm256_set1_epi32(a); }

	__SIMD_INLINE static simd_type set1(int8_t a, int8_t) { return set1_epi8(a); }
	__SIMD_INLINE static simd_type set1(int16_t a, int16_t) { return set1_epi16(a); }
	__SIMD_INLINE static simd_type set1(int32_t a, int32_t) { return set1_epi32(a); }
	__SIMD_INLINE static simd_type set1(uint8_t a, uint8_t) { return set1_epi8(a); }
	__SIMD_INLINE static simd_type set1(uint16_t a, uint16_t) { return set1_epi16(a); }
	__SIMD_INLINE static simd_type set1(uint32_t a, uint32_t) { return set1_epi32(a); }

	__SIMD_INLINE static simd_type setmin_epi8() { return set1_epi8(-0x80); }
	__SIMD_INLINE static simd_type setmin_epi16() { return set1_epi16(-0x8000); }
	__SIMD_INLINE static simd_type setmin_epi32() { return set1_epi32(-0x7fffffff - 1); }

	__SIMD_INLINE static simd_type setmin(int8_t) { return setmin_epi8(); }
	__SIMD_INLINE static simd_type setmin(int16_t) { return setmin_epi16(); }
	__SIMD_INLINE static simd_type setmin(int32_t) { return setmin_epi32(); }

	__SIMD_INLINE static simd_type setmax_epi8() { return set1_epi8(0x7f); }
	__SIMD_INLINE static simd_type setmax_epi16() { return set1_epi16(0x7fff); }
	__SIMD_INLINE static simd_type setmax_epi32() { return set1_epi32(0x7fffffff); }

	__SIMD_INLINE static simd_type setmax(int8_t) { return setmax_epi8(); }
	__SIMD_INLINE static simd_type setmax(int16_t) { return setmax_epi16(); }
	__SIMD_INLINE static simd_type setmax(int32_t) { return setmax_epi32(); }

	__SIMD_INLINE static simd_type load_all(const simd_type* p) { return _mm256_load_si256(p); }
	__SIMD_INLINE static simd_type load(const simd_type* p) { return load_all(p); }
	__SIMD_INLINE static simd_type loadu_all(const simd_type* p) { return _mm256_loadu_si256(p); }
	__SIMD_INLINE static simd_type loadu(const simd_type* p) { return loadu_all(p); }

	__SIMD_INLINE static void store_all(simd_type* p, const simd_type& a) { _mm256_store_si256(p, a); }
	__SIMD_INLINE static void store(simd_type* p, const simd_type& a) { store_all(p, a); }
	__SIMD_INLINE static void storeu_all(simd_type* p, const simd_type& a) { _mm256_storeu_si256(p, a); }
	__SIMD_INLINE static void storeu(simd_type* p, const simd_type& a) { storeu_all(p, a); }

#if defined(__AVX2__)
	__SIMD_INLINE static simd_type And_all(simd_type lhs, simd_type rhs) {
		return _mm256_and_si256(lhs, rhs);
	}
	__SIMD_INLINE static simd_type And(simd_type lhs, simd_type rhs) { return And_all(lhs, rhs); }

	__SIMD_INLINE static simd_type AndNot_all(simd_type lhs, simd_type rhs) {
		return _mm256_andnot_si256(lhs, rhs);
	}
	__SIMD_INLINE static simd_type AndNot(simd_type lhs, simd_type rhs) { return AndNot_all(lhs, rhs); }

	__SIMD_INLINE static simd_type Or_all(simd_type lhs, simd_type rhs) { return _mm256_or_si256(lhs, rhs); }
	__SIMD_INLINE static simd_type Or(simd_type lhs, simd_type rhs) { return Or_all(lhs, rhs); }

	__SIMD_INLINE static simd_type Xor_all(simd_type lhs, simd_type rhs) { return _mm256_xor_si256(lhs, rhs); }
	__SIMD_INLINE static simd_type Xor(simd_type lhs, simd_type rhs) { return Xor_all(lhs, rhs); }

	__SIMD_INLINE static simd_type add_epi8(simd_type lhs, simd_type rhs) { return _mm256_add_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type add_epi16(simd_type lhs, simd_type rhs) { return _mm256_add_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type add_epi32(simd_type lhs, simd_type rhs) { return _mm256_add_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type add_epi64(simd_type lhs, simd_type rhs) { return _mm256_add_epi64(lhs, rhs); }

	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, int8_t) { return add_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, int16_t) { return add_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, int32_t) { return add_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, int64_t) { return add_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint8_t) { return add_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint16_t) { return add_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint32_t) { return add_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type add(simd_type lhs, simd_type rhs, uint64_t) { return add_epi64(lhs, rhs); }

	__SIMD_INLINE static simd_type adds_epi8(simd_type lhs, simd_type rhs) { return _mm256_adds_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type adds_epi16(simd_type lhs, simd_type rhs) { return _mm256_adds_epi16(lhs, rhs); }

	__SIMD_INLINE static simd_type adds_epu8(simd_type lhs, simd_type rhs) { return _mm256_adds_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type adds_epu16(simd_type lhs, simd_type rhs) { return _mm256_adds_epu16(lhs, rhs); }

	__SIMD_INLINE static simd_type adds(simd_type lhs, simd_type rhs, int8_t) { return adds_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type adds(simd_type lhs, simd_type rhs, int16_t) { return adds_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type adds(simd_type lhs, simd_type rhs, uint8_t) { return adds_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type adds(simd_type lhs, simd_type rhs, uint16_t) { return adds_epu16(lhs, rhs); }

	__SIMD_INLINE static simd_type avg_epu8(simd_type lhs, simd_type rhs) { return _mm256_avg_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type avg_epu16(simd_type lhs, simd_type rhs) { return _mm256_avg_epu16(lhs, rhs); }

	__SIMD_INLINE static simd_type avg(simd_type lhs, simd_type rhs, int8_t) { return avg_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type avg(simd_type lhs, simd_type rhs, int16_t) { return avg_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type avg(simd_type lhs, simd_type rhs, uint8_t) { return avg_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type avg(simd_type lhs, simd_type rhs, uint16_t) { return avg_epu16(lhs, rhs); }

	__SIMD_INLINE static simd_type cmpeq_epi8(simd_type lhs, simd_type rhs) { return _mm256_cmpeq_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq_epi16(simd_type lhs, simd_type rhs) { return _mm256_cmpeq_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq_epi32(simd_type lhs, simd_type rhs) { return _mm256_cmpeq_epi32(lhs, rhs); }

	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int8_t) { return cmpeq_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int16_t) { return cmpeq_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, int32_t) { return cmpeq_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint8_t) { return cmpeq_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint16_t) { return cmpeq_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpeq(simd_type lhs, simd_type rhs, uint32_t) { return cmpeq_epi32(lhs, rhs); }

	__SIMD_INLINE static simd_type cmpgt_epi8(simd_type lhs, simd_type rhs) { return _mm256_cmpgt_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt_epi16(simd_type lhs, simd_type rhs) { return _mm256_cmpgt_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt_epi32(simd_type lhs, simd_type rhs) { return _mm256_cmpgt_epi32(lhs, rhs); }

	__SIMD_INLINE static simd_type cmpgt_epu8(simd_type lhs, simd_type rhs) {
		return cmpgt_epi8(Xor(lhs, setmin_epi8()), Xor(rhs, setmin_epi8()));
	}
	__SIMD_INLINE static simd_type cmpgt_epu16(simd_type lhs, simd_type rhs) {
		return cmpgt_epi16(Xor(lhs, setmin_epi16()), Xor(rhs, setmin_epi16()));
	}
	__SIMD_INLINE static simd_type cmpgt_epu32(simd_type lhs, simd_type rhs) {
		return cmpgt_epi32(Xor(lhs, setmin_epi32()), Xor(rhs, setmin_epi32()));
	}

	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int8_t) { return cmpgt_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int16_t) { return cmpgt_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, int32_t) { return cmpgt_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint8_t) { return cmpgt_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint16_t) { return cmpgt_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmpgt(simd_type lhs, simd_type rhs, uint32_t) { return cmpgt_epu32(lhs, rhs); }

	__SIMD_INLINE static simd_type cmplt_epi8(simd_type lhs, simd_type rhs) { return cmpgt_epi8(rhs, lhs); }
	__SIMD_INLINE static simd_type cmplt_epi16(simd_type lhs, simd_type rhs) { return cmpgt_epi16(rhs, lhs); }
	__SIMD_INLINE static simd_type cmplt_epi32(simd_type lhs, simd_type rhs) { return cmpgt_epi32(rhs, lhs); }

	__SIMD_INLINE static simd_type cmplt_epu8(simd_type lhs, simd_type rhs) {
		return cmplt_epi8(Xor(lhs, setmin_epi8()), Xor(rhs, setmin_epi8()));
	}
	__SIMD_INLINE static simd_type cmplt_epu16(simd_type lhs, simd_type rhs) {
		return cmplt_epi16(Xor(lhs, setmin_epi16()), Xor(rhs, setmin_epi16()));
	}
	__SIMD_INLINE static simd_type cmplt_epu32(simd_type lhs, simd_type rhs) {
		return cmplt_epi32(Xor(lhs, setmin_epi32()), Xor(rhs, setmin_epi32()));
	}

	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int8_t) { return cmplt_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int16_t) { return cmplt_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, int32_t) { return cmplt_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint8_t) { return cmplt_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint16_t) { return cmplt_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type cmplt(simd_type lhs, simd_type rhs, uint32_t) { return cmplt_epu32(lhs, rhs); }

	__SIMD_INLINE static simd_type madd_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_madd_epi16(lhs, rhs);
	}

	__SIMD_INLINE static simd_type max_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_max_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type max_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_max_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type max_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_max_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type max_epu8(simd_type lhs, simd_type rhs) {
		return _mm256_max_epu8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type max_epu16(simd_type lhs, simd_type rhs) {
		return _mm256_max_epu16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type max_epu32(simd_type lhs, simd_type rhs) {
		return _mm256_max_epu32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, int8_t) { return max_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, int16_t) { return max_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint8_t) { return max_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint16_t) { return max_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, int32_t) { return max_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type max(simd_type lhs, simd_type rhs, uint32_t) { return max_epu32(lhs, rhs); }

	__SIMD_INLINE static simd_type min_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_min_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type min_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_min_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type min_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_min_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type min_epu8(simd_type lhs, simd_type rhs) {
		return _mm256_min_epu8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type min_epu16(simd_type lhs, simd_type rhs) {
		return _mm256_min_epu16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type min_epu32(simd_type lhs, simd_type rhs) {
		return _mm256_min_epu32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, int8_t) { return min_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, int16_t) { return min_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint8_t) { return min_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint16_t) { return min_epu16(lhs, rhs); }
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, int32_t) { return min_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type min(simd_type lhs, simd_type rhs, uint32_t) { return min_epu32(lhs, rhs); }

	__SIMD_INLINE static int movemask_epi8(simd_type lhs) {
		return _mm256_movemask_epi8(lhs);
	}

	__SIMD_INLINE static simd_type mul_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_mul_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type mul_epu32(simd_type lhs, simd_type rhs) {
		return _mm256_mul_epu32(lhs, rhs);
	}

	__SIMD_INLINE static simd_type mulhi_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_mulhi_epi16(lhs, rhs);
	}

	__SIMD_INLINE static simd_type mulhi_epu16(simd_type lhs, simd_type rhs) {
		return _mm256_mulhi_epu16(lhs, rhs);
	}

	__SIMD_INLINE static simd_type mullo_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_mullo_epi16(lhs, rhs);
	}

	__SIMD_INLINE static simd_type packs_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_packs_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type packs_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_packs_epi32(lhs, rhs);
	}

	__SIMD_INLINE static simd_type packus_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_packus_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type packus_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_packus_epi32(lhs, rhs);
	}

	__SIMD_INLINE static simd_type sll_epi16(simd_type lhs, __m128i rhs) {
		return _mm256_sll_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sll_epi32(simd_type lhs, __m128i rhs) {
		return _mm256_sll_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sll_epi64(simd_type lhs, __m128i rhs) {
		return _mm256_sll_epi64(lhs, rhs);
	}

	__SIMD_INLINE static simd_type sll(simd_type lhs, __m128i rhs, int16_t) { return sll_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sll(simd_type lhs, __m128i rhs, int32_t) { return sll_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type sll(simd_type lhs, __m128i rhs, int64_t) { return sll_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type sll(simd_type lhs, __m128i rhs, uint16_t) { return sll_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sll(simd_type lhs, __m128i rhs, uint32_t) { return sll_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type sll(simd_type lhs, __m128i rhs, uint64_t) { return sll_epi64(lhs, rhs); }

	__SIMD_INLINE static simd_type slli_epi16(simd_type lhs, int imm8) {
		return _mm256_slli_epi16(lhs, imm8);
	}
	__SIMD_INLINE static simd_type slli_epi32(simd_type lhs, int imm8) {
		return _mm256_slli_epi32(lhs, imm8);
	}
	__SIMD_INLINE static simd_type slli_epi64(simd_type lhs, int imm8) {
		return _mm256_slli_epi64(lhs, imm8);
	}

	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, int16_t) { return slli_epi16(lhs, imm8); }
	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, int32_t) { return slli_epi32(lhs, imm8); }
	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, int64_t) { return slli_epi64(lhs, imm8); }
	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, uint16_t) { return slli_epi16(lhs, imm8); }
	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, uint32_t) { return slli_epi32(lhs, imm8); }
	__SIMD_INLINE static simd_type slli(simd_type lhs, int imm8, uint64_t) { return slli_epi64(lhs, imm8); }

	__SIMD_INLINE static simd_type sra_epi16(simd_type lhs, __m128i rhs) {
		return _mm256_sra_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sra_epi32(simd_type lhs, __m128i rhs) {
		return _mm256_sra_epi32(lhs, rhs);
	}

	__SIMD_INLINE static simd_type sra(simd_type lhs, __m128i rhs, int16_t) { return sra_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sra(simd_type lhs, __m128i rhs, int32_t) { return sra_epi32(lhs, rhs); }

	__SIMD_INLINE static simd_type srai_epi16(simd_type lhs, int imm8) {
		return _mm256_srai_epi16(lhs, imm8);
	}
	__SIMD_INLINE static simd_type srai_epi32(simd_type lhs, int imm8) {
		return _mm256_srai_epi32(lhs, imm8);
	}

	__SIMD_INLINE static simd_type srai(simd_type lhs, int imm8, int16_t) { return srai_epi16(lhs, imm8); }
	__SIMD_INLINE static simd_type srai(simd_type lhs, int imm8, int32_t) { return srai_epi32(lhs, imm8); }

	__SIMD_INLINE static simd_type srl_epi16(simd_type lhs, __m128i rhs) {
		return _mm256_srl_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type srl_epi32(simd_type lhs, __m128i rhs) {
		return _mm256_srl_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type srl_epi64(simd_type lhs, __m128i rhs) {
		return _mm256_srl_epi64(lhs, rhs);
	}

	__SIMD_INLINE static simd_type srl(simd_type lhs, __m128i rhs, int16_t) { return srl_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type srl(simd_type lhs, __m128i rhs, int32_t) { return srl_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type srl(simd_type lhs, __m128i rhs, int64_t) { return srl_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type srl(simd_type lhs, __m128i rhs, uint16_t) { return srl_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type srl(simd_type lhs, __m128i rhs, uint32_t) { return srl_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type srl(simd_type lhs, __m128i rhs, uint64_t) { return srl_epi64(lhs, rhs); }

	__SIMD_INLINE static simd_type srli_epi16(simd_type lhs, int imm8) {
		return _mm256_srli_epi16(lhs, imm8);
	}
	__SIMD_INLINE static simd_type srli_epi32(simd_type lhs, int imm8) {
		return _mm256_srli_epi32(lhs, imm8);
	}
	__SIMD_INLINE static simd_type srli_epi64(simd_type lhs, int imm8) {
		return _mm256_srli_epi64(lhs, imm8);
	}

	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, int16_t) { return srli_epi16(lhs, imm8); }
	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, int32_t) { return srli_epi32(lhs, imm8); }
	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, int64_t) { return srli_epi64(lhs, imm8); }
	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, uint16_t) { return srli_epi16(lhs, imm8); }
	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, uint32_t) { return srli_epi32(lhs, imm8); }
	__SIMD_INLINE static simd_type srli(simd_type lhs, int imm8, uint64_t) { return srli_epi64(lhs, imm8); }

	__SIMD_INLINE static simd_type sub_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_sub_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sub_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_sub_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sub_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_sub_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type sub_epi64(simd_type lhs, simd_type rhs) {
		return _mm256_sub_epi64(lhs, rhs);
	}

	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int8_t) { return sub_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int16_t) { return sub_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int32_t) { return sub_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, int64_t) { return sub_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint8_t) { return sub_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint16_t) { return sub_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint32_t) { return sub_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type sub(simd_type lhs, simd_type rhs, uint64_t) { return sub_epi64(lhs, rhs); }

	__SIMD_INLINE static simd_type subs_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_subs_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type subs_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_subs_epi16(lhs, rhs);
	}

	__SIMD_INLINE static simd_type subs_epu8(simd_type lhs, simd_type rhs) {
		return _mm256_subs_epu8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type subs_epu16(simd_type lhs, simd_type rhs) {
		return _mm256_subs_epu16(lhs, rhs);
	}

	__SIMD_INLINE static simd_type subs(simd_type lhs, simd_type rhs, int8_t) { return subs_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type subs(simd_type lhs, simd_type rhs, int16_t) { return subs_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type subs(simd_type lhs, simd_type rhs, uint8_t) { return subs_epu8(lhs, rhs); }
	__SIMD_INLINE static simd_type subs(simd_type lhs, simd_type rhs, uint16_t) { return subs_epu16(lhs, rhs); }

	__SIMD_INLINE static simd_type unpackhi_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_unpackhi_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpackhi_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_unpackhi_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpackhi_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_unpackhi_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpackhi_epi64(simd_type lhs, simd_type rhs) {
		return _mm256_unpackhi_epi64(lhs, rhs);
	}

	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int8_t) { return unpackhi_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int16_t) { return unpackhi_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int32_t) { return unpackhi_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, int64_t) { return unpackhi_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint8_t) { return unpackhi_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint16_t) { return unpackhi_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint32_t) { return unpackhi_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type unpackhi(simd_type lhs, simd_type rhs, uint64_t) { return unpackhi_epi64(lhs, rhs); }

	__SIMD_INLINE static simd_type unpacklo_epi8(simd_type lhs, simd_type rhs) {
		return _mm256_unpacklo_epi8(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpacklo_epi16(simd_type lhs, simd_type rhs) {
		return _mm256_unpacklo_epi16(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpacklo_epi32(simd_type lhs, simd_type rhs) {
		return _mm256_unpacklo_epi32(lhs, rhs);
	}
	__SIMD_INLINE static simd_type unpacklo_epi64(simd_type lhs, simd_type rhs) {
		return _mm256_unpacklo_epi64(lhs, rhs);
	}

	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int8_t) { return unpacklo_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int16_t) { return unpacklo_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int32_t) { return unpacklo_epi32(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, int64_t) { return unpacklo_epi64(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint8_t) { return unpacklo_epi8(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint16_t) { return unpacklo_epi16(lhs, rhs); }
	__SIMD_INLINE static simd_type unpacklo(simd_type lhs, simd_type rhs, uint32_t) { return unpacklo_epi32(lhs, rhs); }
#endif // __AVX2__

#if defined(__AVX2__)
	__SIMD_INLINE static simd_type preloadmask_si32(size_t n) {
		return loadu(reinterpret_cast<const simd_type*>(pdata::preload_mask + 8 - n));
	}

	__SIMD_INLINE static simd_type maskload_si32(const void* ptr, simd_type mask) {
		return _mm256_maskload_epi32(reinterpret_cast<const int*>(ptr), mask);
	}

	__SIMD_INLINE static simd_type preload_si32(const void* ptr, size_t n) {
		return maskload_si32(ptr, preloadmask_si32(n));
	}
#endif // __AVX2__

};

#define __SIMD_REGISTER_FUNCTION(name, T)					\
template<typename...Args>									\
__SIMD_INLINE static decltype(auto) name(Args&&...args) {	\
	return base::name(std::forward<Args>(args)...,T{});		\
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
__SIMD_REGISTER_FUNCTION(cmpgt, T)									\
__SIMD_REGISTER_FUNCTION(cmplt, T)									\
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
#undef __SIMD_REGISTER_ALL_COMMON_FUNCTIONS

#endif // __AVX__

#if defined(__AVX2__) || defined(__SSE2__)
#define __HAS_SIMD_MEMCHR
const uint8_t* memchr(const uint8_t* s, uint8_t val, size_t n);
const uint16_t* memchr(const uint16_t* s, uint16_t val, size_t n);
const uint32_t* memchr(const uint32_t* s, uint32_t val, size_t n);
const uint8_t* memrchr(const uint8_t* s, uint8_t val, size_t n);
const uint16_t* memrchr(const uint16_t* s, uint16_t val, size_t n);
const uint32_t* memrchr(const uint32_t* s, uint32_t val, size_t n);
#endif // __AVX2__ || __SSE2__

#if defined(__AVX2__) || defined(__SSE2__)
#define __HAS_SIMD_MEMCMP
const uint8_t* memcmp(const uint8_t* s0, const uint8_t* s1, size_t n);
const uint8_t* memrcmp(const uint8_t* s0, const uint8_t* s1, size_t n);
#endif // __AVX2__ || __SSE2__

#if defined(__AVX2__) || defined(__SSE2__)
#define __HAS_SIMD_MEMCMPEQ
bool memcmpeq(const uint8_t* s0, const uint8_t* s1, size_t n);
#endif // __AVX2__ || __SSE2__

_WJR_SIMD_END

#endif // __WJR_SIMD_H