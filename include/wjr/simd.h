#pragma once
#ifndef __WJR_SIMD_H
#define __WJR_SIMD_H

#include <wjr/type_traits.h>

_WJR_BEGIN

enum class __simd_type {
	SSE = 1,
	SSE2 = 2,
	SSE3 = 3,
	SSSE3 = 4,
	SSE4_1 = 5,
	SSE4_2 = 6,
	AVX = 7,
	AVX2 = 8
};

template<__simd_type S, typename T>
class simd_traits;

#if defined(__SSE__)
template<>
class simd_traits<__simd_type::SSE, void> {
public:
	using simd_type = __m128;
	using simd_int_type = __m128i;
	using simd_double_type = __m128d;
	constexpr static int bit = 128;
};
#endif

#if defined(__SSE2__)
template<>
class simd_traits<__simd_type::SSE2, void> 
	: public simd_traits<__simd_type::SSE, void> {
	using _Mybase = simd_traits<__simd_type::SSE, void>;
public:
	using simd_type = typename _Mybase::simd_type;
	using simd_int_type = typename _Mybase::simd_int_type;
	using simd_double_type = typename _Mybase::simd_double_type;
	constexpr static int bit = _Mybase::bit;

	static simd_int_type loadu_si(const simd_int_type* p) {
		return _mm_loadu_si128(p);
	}

	static int movemask_epi(simd_int_type v) {
		return _mm_movemask_epi8(v);
	}

	static int movemask_pd(simd_double_type v) {
		return _mm_movemask_pd(v);
	}

	static simd_int_type or_si(simd_int_type v1, simd_int_type v2) {
		return _mm_or_si128(v1, v2);
	}

	static simd_int_type and_si(simd_int_type v1, simd_int_type v2) {
		return _mm_and_si128(v1, v2);
	}

};

template<>
class simd_traits<__simd_type::SSE2, uint8_t> 
	: public simd_traits<__simd_type::SSE2, void> {
	using _Mybase = simd_traits<__simd_type::SSE2, void>;
public:
	using simd_type = typename _Mybase::simd_type;
	using simd_int_type = typename _Mybase::simd_int_type;
	using simd_double_type = typename _Mybase::simd_double_type;

	using scalar_type = uint8_t;

	constexpr static int bit = _Mybase::bit;
	constexpr static int width = bit / (sizeof(scalar_type) * 8);

	static simd_int_type set1_epi(scalar_type v) {
		return _mm_set1_epi8(v);
	}

	static simd_int_type cmpeq_epi(simd_int_type v1, simd_int_type v2) {
		return _mm_cmpeq_epi8(v1, v2);
	}
	
};

template<>
class simd_traits<__simd_type::SSE2, uint16_t>
	: public simd_traits<__simd_type::SSE2, void> {
	using _Mybase = simd_traits<__simd_type::SSE2, void>;
public:
	using simd_type = typename _Mybase::simd_type;
	using simd_int_type = typename _Mybase::simd_int_type;
	using simd_double_type = typename _Mybase::simd_double_type;

	using scalar_type = uint16_t;

	constexpr static int bit = _Mybase::bit;
	constexpr static int width = bit / (sizeof(scalar_type) * 8);

	static simd_int_type set1_epi(scalar_type v) {
		return _mm_set1_epi16(v);
	}

	static simd_int_type cmpeq_epi(simd_int_type v1, simd_int_type v2) {
		return _mm_cmpeq_epi16(v1, v2);
	}

};

template<>
class simd_traits<__simd_type::SSE2, uint32_t>
	: public simd_traits<__simd_type::SSE2, void> {
	using _Mybase = simd_traits<__simd_type::SSE2, void>;
public:
	using simd_type = typename _Mybase::simd_type;
	using simd_int_type = typename _Mybase::simd_int_type;
	using simd_double_type = typename _Mybase::simd_double_type;

	using scalar_type = uint32_t;

	constexpr static int bit = _Mybase::bit;
	constexpr static int width = bit / (sizeof(scalar_type) * 8);

	static simd_int_type set1_epi(scalar_type v) {
		return _mm_set1_epi32(v);
	}

	static simd_int_type cmpeq_epi(simd_int_type v1, simd_int_type v2) {
		return _mm_cmpeq_epi32(v1, v2);
	}

};

#endif

#if defined(__AVX__)
template<>
class simd_traits<__simd_type::AVX, void> {
public:
	using simd_type = __m256;
	using simd_int_type = __m256i;
	using simd_double_type = __m256d;
	constexpr static int bit = 256;
	
	static simd_int_type loadu_si(const simd_int_type* p) {
		return _mm256_loadu_si256(p);
	}
};
#endif

#if defined(__AVX2__)
template<>
class simd_traits<__simd_type::AVX2, void>
	: public simd_traits<__simd_type::AVX, void> {
	using _Mybase = simd_traits<__simd_type::AVX, void>;
public:
	using simd_type = typename _Mybase::simd_type;
	using simd_int_type = typename _Mybase::simd_int_type;
	using simd_double_type = typename _Mybase::simd_double_type;

	constexpr static int bit = _Mybase::bit;
	constexpr static uint32_t __size_mask_pre[16] = {
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000
	};
	constexpr static uint32_t __size_mask_suf[16] = {
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
	};

	static int movemask_epi(simd_int_type v) {
		return _mm256_movemask_epi8(v);
	}
	
	// get top n 32 bit mask
	static simd_int_type mask32_si256_pre(int n) {
		return _mm256_loadu_si256(reinterpret_cast<const simd_int_type*>(__size_mask_pre + 8 - n));
	}

	// get bottom n 32 bit mask
	static simd_int_type mask32_si256_suf(int n) {
		return _mm256_loadu_si256(reinterpret_cast<const simd_int_type*>(__size_mask_suf + n));
	}

	static simd_int_type maskload_epi32(const int* p, simd_int_type mask) {
		return _mm256_maskload_epi32(p, mask);
	}

	static simd_int_type or_si(simd_int_type v1, simd_int_type v2) {
		return _mm256_or_si256(v1, v2);
	}

	static simd_int_type and_si(simd_int_type v1, simd_int_type v2) {
		return _mm256_and_si256(v1, v2);
	}

};

template<>
class simd_traits<__simd_type::AVX2, uint8_t>
	: public simd_traits<__simd_type::AVX2, void> {
	using _Mybase = simd_traits<__simd_type::AVX2, void>;
public:
	using simd_type = typename _Mybase::simd_type;
	using simd_int_type = typename _Mybase::simd_int_type;
	using simd_double_type = typename _Mybase::simd_double_type;

	using scalar_type = uint8_t;

	constexpr static int bit = _Mybase::bit;
	constexpr static int width = bit / (sizeof(scalar_type) * 8);

	static simd_int_type set1_epi(scalar_type v) {
		return _mm256_set1_epi8(v);
	}

	static simd_int_type cmpeq_epi(simd_int_type v1, simd_int_type v2) {
		return _mm256_cmpeq_epi8(v1, v2);
	}

};

template<>
class simd_traits<__simd_type::AVX2, uint16_t>
	: public simd_traits<__simd_type::AVX2, void> {
	using _Mybase = simd_traits<__simd_type::AVX2, void>;
public:
	using simd_type = typename _Mybase::simd_type;
	using simd_int_type = typename _Mybase::simd_int_type;
	using simd_double_type = typename _Mybase::simd_double_type;

	using scalar_type = uint16_t;

	constexpr static int bit = _Mybase::bit;
	constexpr static int width = bit / (sizeof(scalar_type) * 8);

	static simd_int_type set1_epi(scalar_type v) {
		return _mm256_set1_epi16(v);
	}

	static simd_int_type cmpeq_epi(simd_int_type v1, simd_int_type v2) {
		return _mm256_cmpeq_epi16(v1, v2);
	}

};

template<>
class simd_traits<__simd_type::AVX2, uint32_t>
	: public simd_traits<__simd_type::AVX2, void> {
	using _Mybase = simd_traits<__simd_type::AVX2, void>;
public:
	using simd_type = typename _Mybase::simd_type;
	using simd_int_type = typename _Mybase::simd_int_type;
	using simd_double_type = typename _Mybase::simd_double_type;
	
	using scalar_type = uint32_t;

	constexpr static int bit = _Mybase::bit;
	constexpr static int width = bit / (sizeof(scalar_type) * 8);

	static simd_int_type set1_epi(scalar_type v) {
		return _mm256_set1_epi32(v);
	}

	static simd_int_type cmpeq_epi(simd_int_type v1, simd_int_type v2) {
		return _mm256_cmpeq_epi32(v1, v2);
	}

};

#endif

#if defined(__AVX2__) || defined(__SSE2__)
#define __HAS_SIMD_MEMCHR
const uint8_t* simd_memchr(const uint8_t* s, const uint8_t* e, uint8_t val);
const uint16_t* simd_memchr(const uint16_t* s, const uint16_t* e, uint16_t val);
const uint32_t* simd_memchr(const uint32_t* s, const uint32_t* e, uint32_t val);
const uint8_t* simd_memrchr(const uint8_t* s, const uint8_t* e, uint8_t val);
const uint16_t* simd_memrchr(const uint16_t* s, const uint16_t* e, uint16_t val);
const uint32_t* simd_memrchr(const uint32_t* s, const uint32_t* e, uint32_t val);
#endif

#if defined(__AVX2__) || defined(__SSE2__)
#define __HAS_SIMD_MEMCMP
const uint8_t* simd_memcmp(const uint8_t* s1, const uint8_t* e1, const uint8_t* s2);
#endif

_WJR_END

#endif // __WJR_SIMD_H