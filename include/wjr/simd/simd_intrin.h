#pragma once
#ifndef __WJR_SIMD_SIMD_INTRIN_H
#define __WJR_SIMD_SIMD_INTRIN_H

#include <wjr/math.h>
#include <wjr/simd/simd_cast.h>

_WJR_SIMD_BEGIN

/*
* width()
* and(simd_type, simd_type)
* andnot(simd_type, simd_type)
* or(simd_type, simd_type)
* xor(simd_type, simd_type)
* not(simd_type)
* logical_and(simd_type, simd_type)
* logical_or(simd_type, simd_type)
* logical_not(simd_type)
* add(simd_type, simd_type) 
* add(simd_type) 
* sub(simd_type, simd_type)
* min(simd_type, simd_type)
* min(simd_type)
* max(simd_type, simd_type)
* max(simd_type)
* blendv(simd_type, simd_type, simd_type)
* cmpeq(simd_type, simd_type)
* cmplt(simd_type, simd_type)
* cmple
* cmpgt
* cmpge
* cmpneq
* alignl
* alignr
*/

struct sse {
	constexpr static int width() { return 128; }

#if defined(__SSE__)
	WJR_INTRINSIC_INLINE static int movemask_ps(__m128 v) {
		return _mm_movemask_ps(v);
	}
	
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128 shuffle_ps(__m128 a, __m128 b) {
		static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
		return _mm_shuffle_ps(a, b, imm8);
	}
#endif // __SSE__

#if defined(__SSE2__)

	WJR_INTRINSIC_INLINE static __m128i add_epi8(__m128i a, __m128i b) { return _mm_add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add_epi16(__m128i a, __m128i b) { return _mm_add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add_epi32(__m128i a, __m128i b) { return _mm_add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add_epi64(__m128i a, __m128i b) { return _mm_add_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int8_t) { return add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int16_t) { return add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int32_t) { return add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int64_t) { return add_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint8_t) { return add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint16_t) { return add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint32_t) { return add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint64_t) { return add_epi64(a, b); }

	WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m128i a) {
		auto b = shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a);
		a = add_epi8(a, b);
		b = zeros();
		a = sad_epu8(a, b);
		return simd_cast<uint8_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m128i a) {
		a = add_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = add_epi16(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a));
		a = add_epi16(a, srli<2>(a));
		return simd_cast<uint16_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m128i a) {
		a = add_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = add_epi32(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a));
		return simd_cast<uint32_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m128i a) {
		a = add_epi64(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		return simd_cast<uint64_t>(a);
	}
	WJR_INTRINSIC_INLINE static int8_t add_epi8(__m128i a) { return add_epu8(a); }
	WJR_INTRINSIC_INLINE static int16_t add_epi16(__m128i a) { return add_epu16(a); }
	WJR_INTRINSIC_INLINE static int32_t add_epi32(__m128i a) { return add_epu32(a); }
	WJR_INTRINSIC_INLINE static int64_t add_epi64(__m128i a) { return add_epu64(a); }

	WJR_INTRINSIC_INLINE static int8_t add(__m128i a, int8_t) { return add_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t add(__m128i a, int16_t) { return add_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t add(__m128i a, int32_t) { return add_epi32(a); }
	WJR_INTRINSIC_INLINE static int64_t add(__m128i a, int64_t) { return add_epi64(a); }
	WJR_INTRINSIC_INLINE static uint8_t add(__m128i a, uint8_t) { return add_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t add(__m128i a, uint16_t) { return add_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t add(__m128i a, uint32_t) { return add_epu32(a); }
	WJR_INTRINSIC_INLINE static uint64_t add(__m128i a, uint64_t) { return add_epu64(a); }

	WJR_INTRINSIC_INLINE static __m128i adds_epi8(__m128i a, __m128i b) { return _mm_adds_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i adds_epi16(__m128i a, __m128i b) { return _mm_adds_epi16(a, b); }

	WJR_INTRINSIC_INLINE static __m128i adds_epu8(__m128i a, __m128i b) { return _mm_adds_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i adds_epu16(__m128i a, __m128i b) { return _mm_adds_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int8_t) { return adds_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int16_t) { return adds_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint8_t) { return adds_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint16_t) { return adds_epu16(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b) {
		constexpr int s = imm8 & 0x1F;
#if defined(__SSSE3__)
		return _mm_alignr_epi8(a, b, s);
#else
		if constexpr (s == 0) {
			return b;
		}
		if constexpr (s == 16) {
			return a;
		}
		if constexpr (s < 16) {
			return Or(slli<16 - s>(a), srli<s>(b));
		}
		return srli<s - 16>(a);
#endif // __SSSE3__
	}

	WJR_INTRINSIC_INLINE static __m128i alignl_epi16(__m128i a, __m128i b, int c) {
		return Or(slli_epi16(a, c), srli_epi16(b, 16 - c));
	}
	WJR_INTRINSIC_INLINE static __m128i alignl_epi32(__m128i a, __m128i b, int c) {
		return Or(slli_epi32(a, c), srli_epi32(b, 32 - c));
	}
	WJR_INTRINSIC_INLINE static __m128i alignl_epi64(__m128i a, __m128i b, int c) {
		return Or(slli_epi64(a, c), srli_epi64(b, 64 - c));
	}

	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, int16_t) {
		return alignl_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, int32_t) {
		return alignl_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, int64_t) {
		return alignl_epi64(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, uint16_t) {
		return alignl_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, uint32_t) {
		return alignl_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignl(__m128i a, __m128i b, int c, uint64_t) {
		return alignl_epi64(a, b, c);
	}
	
	WJR_INTRINSIC_INLINE static __m128i alignr_epi16(__m128i a, __m128i b, int c) {
		return Or(slli_epi16(a, 16 - c), srli_epi16(b, c));
	}
	WJR_INTRINSIC_INLINE static __m128i alignr_epi32(__m128i a, __m128i b, int c) {
		return Or(slli_epi32(a, 32 - c), srli_epi32(b, c));
	}
	WJR_INTRINSIC_INLINE static __m128i alignr_epi64(__m128i a, __m128i b, int c) {
		return Or(slli_epi64(a, 64 - c), srli_epi64(b, c));
	}

	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int16_t) {
		return alignr_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int32_t) {
		return alignr_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int64_t) {
		return alignr_epi64(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint16_t) {
		return alignr_epi16(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint32_t) {
		return alignr_epi32(a, b, c);
	}
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint64_t) {
		return alignr_epi64(a, b, c);
	}

	WJR_INTRINSIC_INLINE static __m128i And(__m128i a, __m128i b) { return _mm_and_si128(a, b); }

	WJR_INTRINSIC_INLINE static __m128i AndNot(__m128i a, __m128i b) { return _mm_andnot_si128(a, b); }

	WJR_INTRINSIC_INLINE static __m128i avg_epu8(__m128i a, __m128i b) { return _mm_avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i avg_epu16(__m128i a, __m128i b) { return _mm_avg_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int8_t) { return avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int16_t) { return avg_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint8_t) { return avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint16_t) { return avg_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m128i blendv_epi8(__m128i a, __m128i b, __m128i mask) {
#if defined(__SSE4_1__)
		return _mm_blendv_epi8(a, b, mask);
#else
		return Or(AndNot(mask, a), And(mask, b));
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static __m128i blendv_epi16(__m128i a, __m128i b, __m128i mask) {
		return blendv_epi8(b, a, logical_not(mask, uint16_t()));
	}
	WJR_INTRINSIC_INLINE static __m128i blendv_epi32(__m128i a, __m128i b, __m128i mask) {
		return blendv_epi8(b, a, logical_not(mask, uint32_t()));
	}

	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int8_t) {
		return blendv_epi8(a, b, mask);
	}
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int16_t) {
		return blendv_epi16(a, b, mask);
	}
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int32_t) {
		return blendv_epi32(a, b, mask);
	}
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint8_t) {
		return blendv_epi8(a, b, mask);
	}
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint16_t) {
		return blendv_epi16(a, b, mask);
	}
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint32_t) {
		return blendv_epi32(a, b, mask);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i bslli(__m128i val) { return _mm_bslli_si128(val, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i bsrli(__m128i val) { return _mm_bsrli_si128(val, imm8); }

	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi8(__m128i a, __m128i b) { return _mm_cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi16(__m128i a, __m128i b) { return _mm_cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi32(__m128i a, __m128i b) { return _mm_cmpeq_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int8_t) { return cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int16_t) { return cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int32_t) { return cmpeq_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint8_t) { return cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint16_t) { return cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint32_t) { return cmpeq_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpge_epi8(__m128i a, __m128i b) {
		return Not(cmpgt_epi8(b, a));
	}
	WJR_INTRINSIC_INLINE static __m128i cmpge_epi16(__m128i a, __m128i b) {
		return Not(cmpgt_epi16(b, a));
	}
	WJR_INTRINSIC_INLINE static __m128i cmpge_epi32(__m128i a, __m128i b) {
		return Not(cmpgt_epi32(b, a));
	}

	WJR_INTRINSIC_INLINE static __m128i cmpge_epu8(__m128i a, __m128i b) {
		return Not(cmpgt_epu8(b, a));
	}
	WJR_INTRINSIC_INLINE static __m128i cmpge_epu16(__m128i a, __m128i b) {
		return Not(cmpgt_epu16(b, a));
	}
	WJR_INTRINSIC_INLINE static __m128i cmpge_epu32(__m128i a, __m128i b) {
		return Not(cmpgt_epu32(b, a));
	}

	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int8_t) { return cmpge_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int16_t) { return cmpge_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int32_t) { return cmpge_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint8_t) { return cmpge_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint16_t) { return cmpge_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint32_t) { return cmpge_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi8(__m128i a, __m128i b) { return _mm_cmpgt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi16(__m128i a, __m128i b) { return _mm_cmpgt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi32(__m128i a, __m128i b) { return _mm_cmpgt_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpgt_epu8(__m128i a, __m128i b) {
#if defined(__XOP__)
		return _mm_comgt_epu8(a, b);
#else
		return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epu16(__m128i a, __m128i b) {
#if defined(__XOP__)
		return _mm_comgt_epu16(a, b);
#else
		return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epu32(__m128i a, __m128i b) {
#if defined(__XOP__)
		return _mm_comgt_epu32(a, b);
#else
		return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
#endif // __XOP__
	}

	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int8_t) { return cmpgt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int16_t) { return cmpgt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int32_t) { return cmpgt_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint8_t) { return cmpgt_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint16_t) { return cmpgt_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint32_t) { return cmpgt_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmple_epi8(__m128i a, __m128i b) {
		return Not(cmpgt_epi8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i cmple_epi16(__m128i a, __m128i b) {
		return Not(cmpgt_epi16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i cmple_epi32(__m128i a, __m128i b) {
		return Not(cmpgt_epi32(a, b));
	}

	WJR_INTRINSIC_INLINE static __m128i cmple_epu8(__m128i a, __m128i b) {
		return Not(cmpgt_epu8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i cmple_epu16(__m128i a, __m128i b) {
		return Not(cmpgt_epu16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i cmple_epu32(__m128i a, __m128i b) {
		return Not(cmpgt_epu32(a, b));
	}

	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int8_t) { return cmple_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int16_t) { return cmple_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int32_t) { return cmple_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint8_t) { return cmple_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint16_t) { return cmple_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint32_t) { return cmple_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmplt_epi8(__m128i a, __m128i b) { return _mm_cmplt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt_epi16(__m128i a, __m128i b) { return _mm_cmplt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt_epi32(__m128i a, __m128i b) { return _mm_cmplt_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmplt_epu8(__m128i a, __m128i b) {
		return cmpgt_epu8(b, a);
	}
	WJR_INTRINSIC_INLINE static __m128i cmplt_epu16(__m128i a, __m128i b) {
		return cmpgt_epu16(b, a);
	}
	WJR_INTRINSIC_INLINE static __m128i cmplt_epu32(__m128i a, __m128i b) {
		return cmpgt_epu32(b, a);
	}

	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int8_t) { return cmplt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int16_t) { return cmplt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int32_t) { return cmplt_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint8_t) { return cmplt_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint16_t) { return cmplt_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint32_t) { return cmplt_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpne_epi8(__m128i a, __m128i b) { return Not(cmpeq_epi8(a, b)); }
	WJR_INTRINSIC_INLINE static __m128i cmpne_epi16(__m128i a, __m128i b) { return Not(cmpeq_epi16(a, b)); }
	WJR_INTRINSIC_INLINE static __m128i cmpne_epi32(__m128i a, __m128i b) { return Not(cmpeq_epi32(a, b)); }

	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int8_t) { return cmpne_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int16_t) { return cmpne_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int32_t) { return cmpne_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint8_t) { return cmpne_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint16_t) { return cmpne_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint32_t) { return cmpne_epi32(a, b); }

	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::equal_to<>, T) {
		return cmpeq(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::not_equal_to<>, T) {
		return cmpne(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater<>, T) {
		return cmpgt(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater_equal<>, T) {
		return cmpge(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less<>, T) {
		return cmplt(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less_equal<>, T) {
		return cmple(a, b, T());
	}

	WJR_INTRINSIC_INLINE static __m128i concat(uint64_t lo, uint64_t hi) {
		return set_epi64x(hi, lo);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi8(__m128i a) {
		static_assert(imm8 >= 0 && imm8 < 16, "imm8 must be in range [0, 15]");
#if defined(__SSE4_1__)
		return _mm_extract_epi8(a, imm8);
#else
		if constexpr (imm8 & 1) {
			return (extract_epi16(a, imm8 >> 1) >> 8;
		}
		else {
			return extract_epi16(a, imm8 >> 1) & 0xff;
		}
#endif // __SSE4_1__
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi16(__m128i a) {
		static_assert(imm8 >= 0 && imm8 < 8, "imm8 must be in range [0, 7]");
		return _mm_extract_epi16(a, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi32(__m128i a) {
		static_assert(imm8 >= 0 && imm8 < 4, "imm8 must be in range [0, 3]");
#if defined(__SSE4_1__)
		return _mm_extract_epi32(a, imm8);
#else
		if constexpr (imm8 == 0) {
			return simd_cast<uint32_t>(a);
		}
		else if constexpr (imm8 == 1) {
			return static_cast<uint32_t>(simd_cast<uint64_t>(a) >> 32);
		}
		else if constexpr (imm8 == 2) {
			return simd_cast<uint32_t>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		}
		else {
			return simd_cast<uint32_t>(shuffle_epi32<_MM_SHUFFLE(3, 3, 3, 3)>(a));
		}
#endif // __SSE4_1__
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m128i a) {
		static_assert(imm8 >= 0 && imm8 < 2, "imm8 must be in range [0, 1]");
#if defined(__SSE4_1__)
		return _mm_extract_epi64(a, imm8);
#else
		if constexpr (imm8 == 0) {
			return simd_cast<uint64_t>(a);
		}
		else {
			return simd_cast<uint64_t>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		}
#endif // __SSE4_1__
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, int8_t) { return extract_epi8<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, int16_t) { return extract_epi16<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, int32_t) { return extract_epi32<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, int64_t) { return extract_epi64<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, uint8_t) { return extract_epi8<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, uint16_t) { return extract_epi16<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, uint32_t) { return extract_epi32<imm8>(a); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, uint64_t) { return extract_epi64<imm8>(a); }

	WJR_INTRINSIC_INLINE static uint64_t getlow(__m128i v) {
		return simd_cast<uint64_t>(v);
	}
	WJR_INTRINSIC_INLINE static uint64_t gethigh(__m128i v) {
		return extract_epi64<1>(v);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi16(__m128i a, int i) {
		return _mm_insert_epi16(a, i, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int16_t) { return insert_epi16<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint16_t) { return insert_epi16<imm8>(a, i); }

	WJR_INTRINSIC_INLINE static __m128i load(const __m128i* ptr) { return _mm_load_si128(ptr); }
	WJR_INTRINSIC_INLINE static __m128i loadu(const __m128i* ptr) { return _mm_loadu_si128(ptr); }
	WJR_INTRINSIC_INLINE static __m128i loadu_si16(const void* ptr) { return _mm_loadu_si16(ptr); }
	WJR_INTRINSIC_INLINE static __m128i loadu_si32(const void* ptr) { return _mm_loadu_si32(ptr); }
	WJR_INTRINSIC_INLINE static __m128i loadu_si64(const void* ptr) { return _mm_loadu_si64(ptr); }

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m128i logical_and(__m128i a, __m128i b, T) {
		return Not(Or(logical_not(a, T()), logical_not(b, T())));
	}

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m128i logical_not(__m128i v, T) {
		auto Zero = zeros();
		return cmpeq(v, Zero, T());
	}

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m128i logical_or(__m128i a, __m128i b, T) {
		return Not(logical_not(Or(a, b)));
	}

	WJR_INTRINSIC_INLINE static __m128i madd_epi16(__m128i a, __m128i b) {
		return _mm_madd_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static void maskmoveu(__m128i a, __m128i mask, char* mem_addr) {
		return _mm_maskmoveu_si128(a, mask, mem_addr);
	}

	WJR_INTRINSIC_INLINE static __m128i max_epi16(__m128i a, __m128i b) {
		return _mm_max_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i max_epu8(__m128i a, __m128i b) {
		return _mm_max_epu8(a, b);
	}

#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static __m128i max_epi8(__m128i a, __m128i b) {
		return _mm_max_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i max_epi32(__m128i a, __m128i b) {
		return _mm_max_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i max_epu16(__m128i a, __m128i b) {
		return _mm_max_epu16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i max_epu32(__m128i a, __m128i b) {
		return _mm_max_epu32(a, b);
	}
#else
	WJR_INTRINSIC_INLINE static __m128i max_epi8(__m128i a, __m128i b) {
		return blendv_epi8(b, a, cmpgt_epi8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i max_epi32(__m128i a, __m128i b) {
		return blendv_epi8(b, a, cmpgt_epi32(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i max_epu16(__m128i a, __m128i b) {
		return blendv_epi8(b, a, cmpgt_epu16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i max_epu32(__m128i a, __m128i b) {
		return blendv_epi8(b, a, cmpgt_epu32(a, b));
	}
#endif // __SSE4_1__

	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int8_t) { return max_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int16_t) { return max_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int32_t) { return max_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint8_t) { return max_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint16_t) { return max_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint32_t) { return max_epu32(a, b); }

	WJR_INTRINSIC_INLINE static int8_t max_epi8(__m128i a) {
		return 0x7fu ^ min_epu8(Xor(a, set1_epi8(0x7fu)));
	}

	WJR_INTRINSIC_INLINE static int16_t max_epi16(__m128i a) {
#if defined(__SSE4_1__)
		return 0x7fffu ^ min_epu16(Xor(a, set1_epi16(0x7fffu)));
#else
		a = max_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<int16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static int32_t max_epi32(__m128i a) {
		a = max_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<int32_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m128i a) {
#if defined(__SSE4_1__)
		return 0xffu ^ min_epu8(Xor(a, ones()));
#else
		a = max_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		auto X = simd_cast<uint32_t>(a);
		return std::max((uint8_t)X, (uint8_t)(X >> 8));
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m128i a) {
#if defined(__SSE4_1__)
		return 0xffffu ^ min_epu16(Xor(a, ones()));
#else
		a = max_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<uint16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m128i a) {
		a = max_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = max_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<uint32_t>(a);
	}

	WJR_INTRINSIC_INLINE static int8_t max(__m128i a, int8_t) { return max_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t max(__m128i a, int16_t) { return max_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t max(__m128i a, int32_t) { return max_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t max(__m128i a, uint8_t) { return max_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t max(__m128i a, uint16_t) { return max_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t max(__m128i a, uint32_t) { return max_epu32(a); }

	WJR_INTRINSIC_INLINE static __m128i min_epi16(__m128i a, __m128i b) {
		return _mm_min_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i min_epu8(__m128i a, __m128i b) {
		return _mm_min_epu8(a, b);
	}

#if defined(__SSE4_1__)
	WJR_INTRINSIC_INLINE static __m128i min_epi8(__m128i a, __m128i b) {
		return _mm_min_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i min_epi32(__m128i a, __m128i b) {
		return _mm_min_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i min_epu16(__m128i a, __m128i b) {
		return _mm_min_epu16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i min_epu32(__m128i a, __m128i b) {
		return _mm_min_epu32(a, b);
	}
#else
	WJR_INTRINSIC_INLINE static __m128i min_epi8(__m128i a, __m128i b) {
		return blendv_epi8(a, b, cmpgt_epi8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i min_epi32(__m128i a, __m128i b) {
		return blendv_epi8(a, b, cmpgt_epi32(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i min_epu16(__m128i a, __m128i b) {
		return blendv_epi8(a, b, cmpgt_epu16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m128i min_epu32(__m128i a, __m128i b) {
		return blendv_epi8(a, b, cmpgt_epu32(a, b));
	}
#endif // __SSE4_1__
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int8_t) { return min_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int16_t) { return min_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int32_t) { return min_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint8_t) { return min_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint16_t) { return min_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint32_t) { return min_epu32(a, b); }

	WJR_INTRINSIC_INLINE static int8_t min_epi8(__m128i a) {
		return 0x80u ^ min_epu8(Xor(a, setmin_epi8()));
	}
	WJR_INTRINSIC_INLINE static int16_t min_epi16(__m128i a) {
#if defined(__SSE4_1__)
		return 0x8000u ^ min_epu16(Xor(a, setmin_epi16()));
#else
		a = min_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<int16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static int32_t min_epi32(__m128i a) {
		a = min_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<int32_t>(a);
	}
	WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m128i a) {
#if defined(__SSE4_1__)
		a = min_epu8(a, srli_epi16(a, 8));
		a = _mm_minpos_epu16(a);
		return simd_cast<uint8_t>(a);
#else
		a = min_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		auto X = simd_cast<uint32_t>(a);
		return std::min((uint8_t)X, (uint8_t)(X >> 8));
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m128i a) {
#if defined(__SSE4_1__)
		return simd_cast<uint16_t>(_mm_minpos_epu16(a));
#else
		a = min_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
		return simd_cast<uint16_t>(a);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m128i a) {
		a = min_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
		a = min_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
		return simd_cast<uint32_t>(a);
	}

	WJR_INTRINSIC_INLINE static int8_t min(__m128i a, int8_t) { return min_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t min(__m128i a, int16_t) { return min_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t min(__m128i a, int32_t) { return min_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t min(__m128i a, uint8_t) { return min_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t min(__m128i a, uint16_t) { return min_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t min(__m128i a, uint32_t) { return min_epu32(a); }

	WJR_INTRINSIC_INLINE static __m128i move_epi64(__m128i a) {
		return _mm_move_epi64(a);
	}

	WJR_INTRINSIC_INLINE static int movemask_epi8(__m128i a) {
		return _mm_movemask_epi8(a);
	}
	WJR_INTRINSIC_INLINE static int movemask_pd(__m128d v) {
		return _mm_movemask_pd(v);
	}

	WJR_INTRINSIC_INLINE static int movemask(__m128i v, int8_t) { return movemask_epi8(v); }
	WJR_INTRINSIC_INLINE static int movemask(__m128i v, int32_t) { return movemask_ps(simd_cast<__m128>(v)); }
	WJR_INTRINSIC_INLINE static int movemask(__m128i v, int64_t) { return movemask_pd(simd_cast<__m128d>(v)); }
	WJR_INTRINSIC_INLINE static int movemask(__m128i v, uint8_t) { return movemask(v, int8_t()); }
	WJR_INTRINSIC_INLINE static int movemask(__m128i v, uint32_t) { return movemask(v, int32_t()); }
	WJR_INTRINSIC_INLINE static int movemask(__m128i v, uint64_t) { return movemask(v, int64_t()); }

	WJR_INTRINSIC_INLINE static __m128i mul_epu32(__m128i a, __m128i b) {
		return _mm_mul_epu32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i mulhi_epi16(__m128i a, __m128i b) {
		return _mm_mulhi_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i mulhi_epu16(__m128i a, __m128i b) {
		return _mm_mulhi_epu16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i mullo_epi16(__m128i a, __m128i b) {
		return _mm_mullo_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i Not(__m128i v) { return Xor(v, ones()); }

	WJR_INTRINSIC_INLINE static __m128i Or(__m128i a, __m128i b) { return _mm_or_si128(a, b); }

	WJR_INTRINSIC_INLINE static __m128i packs_epi16(__m128i a, __m128i b) {
		return _mm_packs_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i packs_epi32(__m128i a, __m128i b) {
		return _mm_packs_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i packus_epi16(__m128i a, __m128i b) {
		return _mm_packus_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i preload_si16(const void* ptr) { return loadu_si16(ptr); }
	WJR_INTRINSIC_INLINE static __m128i preload_si32(const void* ptr) { return loadu_si32(ptr); }
	WJR_INTRINSIC_INLINE static __m128i preload_si48(const void* ptr) {
		return insert_epi16<2>(preload_si32(ptr), reinterpret_cast<const uint16_t*>(ptr)[2]);
	}
	WJR_INTRINSIC_INLINE static __m128i preload_si64(const void* ptr) { return loadu_si64(ptr); }
	WJR_INTRINSIC_INLINE static __m128i preload_si80(const void* ptr) {
		return insert_epi16<4>(preload_si64(ptr), reinterpret_cast<const uint16_t*>(ptr)[4]);
	}
	WJR_INTRINSIC_INLINE static __m128i preload_si96(const void* ptr) {
#if defined(__SSE4_1__)
		return insert_epi32<2>(preload_si64(ptr), reinterpret_cast<const uint32_t*>(ptr)[2]);
#else
		return insert_epi16<5>(preload_si80(ptr), reinterpret_cast<const uint16_t*>(ptr)[5]);
#endif // __SSE4_1__
	}
	WJR_INTRINSIC_INLINE static __m128i preload_si112(const void* ptr) {
		return insert_epi16<6>(preload_si96(ptr), reinterpret_cast<const uint16_t*>(ptr)[6]);
	}
	WJR_INTRINSIC_INLINE static __m128i preload_si128(const void* ptr) { return loadu((__m128i*)ptr); }

	WJR_INTRINSIC_INLINE static __m128i preload_si16x(const void* ptr, int n) {
		// preload_si(n * 16)
		switch (n) {
		case 0:return zeros();
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

	WJR_INTRINSIC_INLINE static __m128i sad_epu8(__m128i a, __m128i b) {
		return _mm_sad_epu8(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i zeros() { return _mm_setzero_si128(); }
	WJR_INTRINSIC_INLINE static __m128i ones() { return _mm_set1_epi32(-1); }

	WJR_INTRINSIC_INLINE static __m128i set_epi8(
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
		return _mm_set_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m128i set_epi16(
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm_set_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m128i set_epi32(int e3, int e2, int e1, int e0) {
		return _mm_set_epi32(e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m128i set_epi64x(long long e1, long long e0) {
		return _mm_set_epi64x(e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m128i setr_epi8(
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
		return _mm_setr_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m128i setr_epi16(
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm_setr_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m128i setr_epi32(int e3, int e2, int e1, int e0) {
		return _mm_setr_epi32(e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m128i set1_epi8(int8_t val) { return _mm_set1_epi8(val); }
	WJR_INTRINSIC_INLINE static __m128i set1_epi16(int16_t val) { return _mm_set1_epi16(val); }
	WJR_INTRINSIC_INLINE static __m128i set1_epi32(int32_t val) { return _mm_set1_epi32(val); }
	WJR_INTRINSIC_INLINE static __m128i set1_epi64(int64_t val) { return _mm_set1_epi64x(val); }

	WJR_INTRINSIC_INLINE static __m128i set1(int8_t val, int8_t) { return set1_epi8(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(int16_t val, int16_t) { return set1_epi16(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(int32_t val, int32_t) { return set1_epi32(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(int64_t val, int64_t) { return set1_epi64(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(uint8_t val, uint8_t) { return set1_epi8(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(uint16_t val, uint16_t) { return set1_epi16(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(uint32_t val, uint32_t) { return set1_epi32(val); }
	WJR_INTRINSIC_INLINE static __m128i set1(uint64_t val, uint64_t) { return set1_epi64(val); }

	WJR_INTRINSIC_INLINE static __m128i setmin_epi8() { return set1_epi8(0x80u); }
	WJR_INTRINSIC_INLINE static __m128i setmin_epi16() { return set1_epi16(0x8000u); }
	WJR_INTRINSIC_INLINE static __m128i setmin_epi32() { return set1_epi32(0x80000000u); }

	WJR_INTRINSIC_INLINE static __m128i setmin(int8_t) { return setmin_epi8(); }
	WJR_INTRINSIC_INLINE static __m128i setmin(int16_t) { return setmin_epi16(); }
	WJR_INTRINSIC_INLINE static __m128i setmin(int32_t) { return setmin_epi32(); }
	WJR_INTRINSIC_INLINE static __m128i setmin(uint8_t) { return set1_epi32(0); }
	WJR_INTRINSIC_INLINE static __m128i setmin(uint16_t) { return set1_epi32(0); }
	WJR_INTRINSIC_INLINE static __m128i setmin(uint32_t) { return set1_epi32(0); }

	WJR_INTRINSIC_INLINE static __m128i setmax_epi8() { return set1_epi8(0x7F); }
	WJR_INTRINSIC_INLINE static __m128i setmax_epi16() { return set1_epi16(0x7FFF); }
	WJR_INTRINSIC_INLINE static __m128i setmax_epi32() { return set1_epi32(0x7FFFFFFF); }

	WJR_INTRINSIC_INLINE static __m128i setmax(int8_t) { return setmax_epi8(); }
	WJR_INTRINSIC_INLINE static __m128i setmax(int16_t) { return setmax_epi16(); }
	WJR_INTRINSIC_INLINE static __m128i setmax(int32_t) { return setmax_epi32(); }
	WJR_INTRINSIC_INLINE static __m128i setmax(uint8_t) { return set1_epi32(0xFFFFFFFF); }
	WJR_INTRINSIC_INLINE static __m128i setmax(uint16_t) { return set1_epi32(0xFFFFFFFF); }
	WJR_INTRINSIC_INLINE static __m128i setmax(uint32_t) { return set1_epi32(0xFFFFFFFF); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i shuffle_epi32(__m128i v) {
		static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
		return _mm_shuffle_epi32(v, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i shufflehi_epi16(__m128i v) {
		return _mm_shufflehi_epi16(v, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i shufflelo_epi16(__m128i v) {
		return _mm_shufflelo_epi16(v, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i sll_epi16(__m128i a, __m128i b) {
		return _mm_sll_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sll_epi32(__m128i a, __m128i b) {
		return _mm_sll_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sll_epi64(__m128i a, __m128i b) {
		return _mm_sll_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int16_t) { return sll_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int32_t) { return sll_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int64_t) { return sll_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint16_t) { return sll_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint32_t) { return sll_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint64_t) { return sll_epi64(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i v) {
		return _mm_slli_si128(v, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i slli_epi16(__m128i a, int imm8) {
		return _mm_slli_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i slli_epi32(__m128i a, int imm8) {
		return _mm_slli_epi32(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i slli_epi64(__m128i a, int imm8) {
		return _mm_slli_epi64(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int16_t) { return slli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int32_t) { return slli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int64_t) { return slli_epi64(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint16_t) { return slli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint32_t) { return slli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint64_t) { return slli_epi64(a, imm8); }

	WJR_INTRINSIC_INLINE static __m128i sra_epi16(__m128i a, __m128i b) {
		return _mm_sra_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sra_epi32(__m128i a, __m128i b) {
		return _mm_sra_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int16_t) { return sra_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int32_t) { return sra_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i srai_epi16(__m128i a, int imm8) {
		return _mm_srai_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i srai_epi32(__m128i a, int imm8) {
		return _mm_srai_epi32(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int16_t) { return srai_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int32_t) { return srai_epi32(a, imm8); }

	WJR_INTRINSIC_INLINE static __m128i srl_epi16(__m128i a, __m128i b) {
		return _mm_srl_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i srl_epi32(__m128i a, __m128i b) {
		return _mm_srl_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i srl_epi64(__m128i a, __m128i b) {
		return _mm_srl_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int16_t) { return srl_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int32_t) { return srl_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int64_t) { return srl_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint16_t) { return srl_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint32_t) { return srl_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint64_t) { return srl_epi64(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i v) {
		return _mm_srli_si128(v, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i srli_epi16(__m128i a, int imm8) {
		return _mm_srli_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i srli_epi32(__m128i a, int imm8) {
		return _mm_srli_epi32(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m128i srli_epi64(__m128i a, int imm8) {
		return _mm_srli_epi64(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int16_t) { return srli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int32_t) { return srli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int64_t) { return srli_epi64(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint16_t) { return srli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint32_t) { return srli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint64_t) { return srli_epi64(a, imm8); }

	WJR_INTRINSIC_INLINE static void store(__m128i* ptr, __m128i val) { _mm_store_si128(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu(__m128i* ptr, __m128i val) { _mm_storeu_si128(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu_si16(void* ptr, __m128i val) { _mm_storeu_si16(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu_si32(void* ptr, __m128i val) { _mm_storeu_si32(ptr, val); }
	WJR_INTRINSIC_INLINE static void storeu_si64(void* ptr, __m128i val) { _mm_storeu_si64(ptr, val); }

	WJR_INTRINSIC_INLINE static __m128i sub_epi8(__m128i a, __m128i b) {
		return _mm_sub_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sub_epi16(__m128i a, __m128i b) {
		return _mm_sub_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sub_epi32(__m128i a, __m128i b) {
		return _mm_sub_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sub_epi64(__m128i a, __m128i b) {
		return _mm_sub_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int8_t) { return sub_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int16_t) { return sub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int32_t) { return sub_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int64_t) { return sub_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint8_t) { return sub_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint16_t) { return sub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint32_t) { return sub_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint64_t) { return sub_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i subs_epi8(__m128i a, __m128i b) {
		return _mm_subs_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i subs_epi16(__m128i a, __m128i b) {
		return _mm_subs_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i subs_epu8(__m128i a, __m128i b) {
		return _mm_subs_epu8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i subs_epu16(__m128i a, __m128i b) {
		return _mm_subs_epu16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int8_t) { return subs_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int16_t) { return subs_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint8_t) { return subs_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint16_t) { return subs_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi8(__m128i a, __m128i b) {
		return _mm_unpackhi_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi16(__m128i a, __m128i b) {
		return _mm_unpackhi_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi32(__m128i a, __m128i b) {
		return _mm_unpackhi_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi64(__m128i a, __m128i b) {
		return _mm_unpackhi_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int8_t) { return unpackhi_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int16_t) { return unpackhi_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int32_t) { return unpackhi_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int64_t) { return unpackhi_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint8_t) { return unpackhi_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint16_t) { return unpackhi_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint32_t) { return unpackhi_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint64_t) { return unpackhi_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi8(__m128i a, __m128i b) {
		return _mm_unpacklo_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi16(__m128i a, __m128i b) {
		return _mm_unpacklo_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi32(__m128i a, __m128i b) {
		return _mm_unpacklo_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi64(__m128i a, __m128i b) {
		return _mm_unpacklo_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int8_t) { return unpacklo_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int16_t) { return unpacklo_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int32_t) { return unpacklo_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int64_t) { return unpacklo_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint8_t) { return unpacklo_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint16_t) { return unpacklo_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint32_t) { return unpacklo_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m128i Xor(__m128i a, __m128i b) { return _mm_xor_si128(a, b); }

#endif // __SSE2__

#if defined(__SSE3__)
	WJR_INTRINSIC_INLINE static __m128i lddqu(const __m128i* ptr) { return _mm_lddqu_si128(ptr); }
#endif // __SSE3__

#if defined(__SSSE3__)

	WJR_INTRINSIC_INLINE static __m128i abs_epi8(__m128i val) { return _mm_abs_epi8(val); }
	WJR_INTRINSIC_INLINE static __m128i abs_epi16(__m128i val) { return _mm_abs_epi16(val); }
	WJR_INTRINSIC_INLINE static __m128i abs_epi32(__m128i val) { return _mm_abs_epi32(val); }

	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int8_t) { return abs_epi8(val); }
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int16_t) { return abs_epi16(val); }
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int32_t) { return abs_epi32(val); }
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint8_t) { return val; }
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint16_t) { return val; }
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint32_t) { return val; }

	WJR_INTRINSIC_INLINE static __m128i shuffle_epi8(__m128i v, __m128i imm8) {
		return _mm_shuffle_epi8(v, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i sign_epi8(__m128i a, __m128i b) {
		return _mm_sign_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sign_epi16(__m128i a, __m128i b) {
		return _mm_sign_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m128i sign_epi32(__m128i a, __m128i b) {
		return _mm_sign_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int8_t) { return sign_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int16_t) { return sign_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int32_t) { return sign_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint8_t) { return sign_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint16_t) { return sign_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint32_t) { return sign_epi32(a, b); }

#endif // __SSSE3__

#if defined(__SSE4_1__)
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i blend_epi16(__m128i a, __m128i b) {
		return _mm_blend_epi16(a, b, imm8);
	}

	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi64(__m128i a, __m128i b) { return _mm_cmpeq_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int64_t) { return cmpeq_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint64_t) { return cmpeq_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi64(__m128i a, __m128i b) { return _mm_cmpgt_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int64_t) { return cmpgt_epi64(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi8(__m128i a, int i) {
		return _mm_insert_epi8(a, i, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi32(__m128i a, int i) {
		return _mm_insert_epi32(a, i, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi64(__m128i a, int64_t i) {
		return _mm_insert_epi64(a, i, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int8_t) { return insert_epi8<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int32_t) { return insert_epi32<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, int64_t) { return insert_epi64<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint8_t) { return insert_epi8<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint32_t) { return insert_epi32<imm8>(a, i); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, uint64_t) { return insert_epi64<imm8>(a, i); }

	WJR_INTRINSIC_INLINE static __m128i minpos_epu16(__m128i a) { return _mm_minpos_epu16(a); }

	WJR_INTRINSIC_INLINE static __m128i mul_epi32(__m128i a, __m128i b) {
		return _mm_mul_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m128i mullo_epi32(__m128i a, __m128i b) {
		return _mm_mullo_epi32(a, b);
	}


	WJR_INTRINSIC_INLINE static __m128i packus_epi32(__m128i a, __m128i b) {
		return _mm_packus_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static int test_all_ones(__m128i a) {
		return _mm_test_all_ones(a);
	}

	WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a, __m128i b) {
		return _mm_test_all_zeros(a, b);
	}

	WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a) {
		return _mm_test_all_zeros(a, a);
	}

	WJR_INTRINSIC_INLINE static int test_mix_ones_zeros(__m128i a, __m128i b) {
		return _mm_test_mix_ones_zeros(a, b);
	}

	WJR_INTRINSIC_INLINE static int testc(__m128i a, __m128i b) {
		return _mm_testc_si128(a, b);
	}

	WJR_INTRINSIC_INLINE static int testnzc(__m128i a, __m128i b) {
		return _mm_testnzc_si128(a, b);
	}

	WJR_INTRINSIC_INLINE static int testz(__m128i a, __m128i b) {
		return _mm_testz_si128(a, b);
	}
#endif // __SSE4_1__

};
struct avx {
	constexpr static int width() { return 256; }

#if defined(__AVX__)

	WJR_INTRINSIC_INLINE static __m256i concat(__m128i a, __m128i b) {
		return insert_si128<1>(simd_cast<__m256i>(a), b);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi32(__m256i v) { return _mm256_extract_epi32(v, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m256i v) { return _mm256_extract_epi64(v, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m256i v, int32_t) { return extract_epi32<imm8>(v); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(__m256i v, int64_t) { return extract_epi64<imm8>(v); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i extract_si128(__m256i v) {
#if defined(__AVX2__)
		return _mm256_extracti128_si256(v, imm8);
#else
		return _mm256_extractf128_si256(v, imm8);
#endif // __AVX2__
	}

	WJR_INTRINSIC_INLINE static __m128i getlow(__m256i a) {
		return simd_cast<__m128i>(a);
	}

	WJR_INTRINSIC_INLINE static __m128i gethigh(__m256i a) {
		return extract_si128<1>(a);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi8(__m256i v, int8_t i) { return _mm256_insert_epi8(v, i, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi16(__m256i v, int16_t i) { return _mm256_insert_epi16(v, i, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi32(__m256i v, int32_t i) { return _mm256_insert_epi32(v, i, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi64(__m256i v, int64_t i) { return _mm256_insert_epi64(v, i, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_si128(__m256i a, __m128i b) {
#if defined(__AVX2__)
		return _mm256_inserti128_si256(a, b, imm8);
#else
		return _mm256_insertf128_si256(a, b, imm8);
#endif // __AVX2__
	}

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m256i logical_and(__m256i a, __m256i b, T) {
		return Not(Or(logical_not(a, T()), logical_not(b, T())));
	}

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m256i logical_not(__m256i v, T) {
		auto Zero = zeros();
		return cmpeq(v, Zero, T());
	}

	template<typename T, std::enable_if_t<wjr::is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m256i logical_or(__m256i a, __m256i b, T) {
		return Not(logical_not(Or(a, b)));
	}

	WJR_INTRINSIC_INLINE static __m256i zeros() { return _mm256_setzero_si256(); }
	WJR_INTRINSIC_INLINE static __m256i ones() { return _mm256_set1_epi32(-1); }

	WJR_INTRINSIC_INLINE static __m256i set_epi8(
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
	WJR_INTRINSIC_INLINE static __m256i set_epi16(
		short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm256_set_epi16(
			e15, e14, e13, e12, e11, e10, e9, e8,
			e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m256i set_epi32(
		int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
		return _mm256_set_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m256i set_epi64x(
		long long e3, long long e2, long long e1, long long e0) {
		return _mm256_set_epi64x(e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m256i setr_epi8(
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
	WJR_INTRINSIC_INLINE static __m256i setr_epi16(
		short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
		return _mm256_setr_epi16(
			e15, e14, e13, e12, e11, e10, e9, e8,
			e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m256i setr_epi32(
		int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
		return _mm256_setr_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
	}
	WJR_INTRINSIC_INLINE static __m256i setr_epi64x(
		long long e3, long long e2, long long e1, long long e0) {
		return _mm256_setr_epi64x(e3, e2, e1, e0);
	}

	WJR_INTRINSIC_INLINE static __m256i set1_epi8(int8_t a) { return _mm256_set1_epi8(a); }
	WJR_INTRINSIC_INLINE static __m256i set1_epi16(int16_t a) { return _mm256_set1_epi16(a); }
	WJR_INTRINSIC_INLINE static __m256i set1_epi32(int32_t a) { return _mm256_set1_epi32(a); }
	WJR_INTRINSIC_INLINE static __m256i set1_epi64(int64_t a) { return _mm256_set1_epi64x(a); }

	WJR_INTRINSIC_INLINE static __m256i set1(int8_t a, int8_t) { return set1_epi8(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(int16_t a, int16_t) { return set1_epi16(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(int32_t a, int32_t) { return set1_epi32(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(int64_t a, int64_t) { return set1_epi64(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(uint8_t a, uint8_t) { return set1_epi8(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(uint16_t a, uint16_t) { return set1_epi16(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(uint32_t a, uint32_t) { return set1_epi32(a); }
	WJR_INTRINSIC_INLINE static __m256i set1(uint64_t a, uint64_t) { return set1_epi64(a); }

	WJR_INTRINSIC_INLINE static __m256i setmin_epi8() { return set1_epi8(0x80u); }
	WJR_INTRINSIC_INLINE static __m256i setmin_epi16() { return set1_epi16(0x8000u); }
	WJR_INTRINSIC_INLINE static __m256i setmin_epi32() { return set1_epi32(0x80000000u); }
	WJR_INTRINSIC_INLINE static __m256i setmin_epi64() { return set1_epi64(0x8000000000000000ull); }

	WJR_INTRINSIC_INLINE static __m256i setmin(int8_t) { return setmin_epi8(); }
	WJR_INTRINSIC_INLINE static __m256i setmin(int16_t) { return setmin_epi16(); }
	WJR_INTRINSIC_INLINE static __m256i setmin(int32_t) { return setmin_epi32(); }
	WJR_INTRINSIC_INLINE static __m256i setmin(int64_t) { return setmin_epi64(); }

	WJR_INTRINSIC_INLINE static __m256i setmax_epi8() { return set1_epi8(0x7f); }
	WJR_INTRINSIC_INLINE static __m256i setmax_epi16() { return set1_epi16(0x7fff); }
	WJR_INTRINSIC_INLINE static __m256i setmax_epi32() { return set1_epi32(0x7fffffff); }
	WJR_INTRINSIC_INLINE static __m256i setmax_epi64() { return set1_epi64(0x7fffffffffffffff); }

	WJR_INTRINSIC_INLINE static __m256i setmax(int8_t) { return setmax_epi8(); }
	WJR_INTRINSIC_INLINE static __m256i setmax(int16_t) { return setmax_epi16(); }
	WJR_INTRINSIC_INLINE static __m256i setmax(int32_t) { return setmax_epi32(); }
	WJR_INTRINSIC_INLINE static __m256i setmax(int64_t) { return setmax_epi64(); }

	WJR_INTRINSIC_INLINE static __m256i zextsi128_si256(__m128i v) { return _mm256_zextsi128_si256(v); }

	WJR_INTRINSIC_INLINE static __m256i load(const __m256i* p) { return _mm256_load_si256(p); }
	WJR_INTRINSIC_INLINE static __m256i loadu(const __m256i* p) { return _mm256_loadu_si256(p); }

	WJR_INTRINSIC_INLINE static __m256i preload_si16(const void* ptr) {
		return zextsi128_si256(sse::preload_si16(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si32(const void* ptr) {
		return zextsi128_si256(sse::preload_si32(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si48(const void* ptr) {
		return zextsi128_si256(sse::preload_si48(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si64(const void* ptr) {
		return zextsi128_si256(sse::preload_si64(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si80(const void* ptr) {
		return zextsi128_si256(sse::preload_si80(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si96(const void* ptr) {
		return zextsi128_si256(sse::preload_si96(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si112(const void* ptr) {
		return zextsi128_si256(sse::preload_si112(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si128(const void* ptr) {
		return zextsi128_si256(sse::preload_si128(ptr));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si144(const void* ptr) {
		return concat(sse::preload_si128(ptr),
			sse::preload_si16((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si160(const void* ptr) {
		return concat(sse::preload_si128(ptr),
			sse::preload_si32((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si176(const void* ptr) {
		return concat(sse::preload_si128(ptr),
			sse::preload_si48((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si192(const void* ptr) {
		return concat(sse::preload_si128(ptr),
			sse::preload_si64((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si208(const void* ptr) {
		return concat(sse::preload_si128(ptr),
			sse::preload_si80((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si224(const void* ptr) {
		return concat(sse::preload_si128(ptr),
			sse::preload_si96((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si240(const void* ptr) {
		return concat(sse::preload_si128(ptr),
			sse::preload_si112((const char*)ptr + 16));
	}
	WJR_INTRINSIC_INLINE static __m256i preload_si256(const void* ptr) {
		return loadu((const __m256i*)ptr);
	}

	WJR_INTRINSIC_INLINE static __m256i preload_si16x(const void* ptr, int n) {
		switch (n) {
		case 0:return zeros();
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

	WJR_INTRINSIC_INLINE static void store(__m256i* p, const __m256i& a) { _mm256_store_si256(p, a); }
	WJR_INTRINSIC_INLINE static void storeu(__m256i* p, const __m256i& a) { _mm256_storeu_si256(p, a); }

	WJR_INTRINSIC_INLINE static int test_all_ones(__m256i a) {
		return testc(a, cmpeq_epi32(a, a));
	}

	WJR_INTRINSIC_INLINE static int test_all_zeros(__m256i a) {
		return testz(a, a);
	}

	WJR_INTRINSIC_INLINE static int testc(__m256i a, __m256i b) {
		return _mm256_testc_si256(a, b);
	}

	WJR_INTRINSIC_INLINE static int testnzc(__m256i a, __m256i b) {
		return _mm256_testnzc_si256(a, b);
	}

	WJR_INTRINSIC_INLINE static int testz(__m256i a, __m256i b) {
		return _mm256_testz_si256(a, b);
	}

#endif // __AVX__

#if defined(__AVX2__)
	WJR_INTRINSIC_INLINE static __m256i And(__m256i a, __m256i b) {
		return _mm256_and_si256(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i AndNot(__m256i a, __m256i b) {
		return _mm256_andnot_si256(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i Or(__m256i a, __m256i b) { return _mm256_or_si256(a, b); }

	WJR_INTRINSIC_INLINE static __m256i Xor(__m256i a, __m256i b) { return _mm256_xor_si256(a, b); }

	WJR_INTRINSIC_INLINE static __m256i Not(__m256i v) { return _mm256_xor_si256(v, ones()); }

	WJR_INTRINSIC_INLINE static __m256i abs_epi8(__m256i v) { return _mm256_abs_epi8(v); }
	WJR_INTRINSIC_INLINE static __m256i abs_epi16(__m256i v) { return _mm256_abs_epi16(v); }
	WJR_INTRINSIC_INLINE static __m256i abs_epi32(__m256i v) { return _mm256_abs_epi32(v); }

	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int8_t) { return abs_epi8(v); }
	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int16_t) { return abs_epi16(v); }
	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int32_t) { return abs_epi32(v); }
	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int64_t) { return abs_epi32(v); }

	WJR_INTRINSIC_INLINE static __m256i add_epi8(__m256i a, __m256i b) { return _mm256_add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add_epi16(__m256i a, __m256i b) { return _mm256_add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add_epi32(__m256i a, __m256i b) { return _mm256_add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add_epi64(__m256i a, __m256i b) { return _mm256_add_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int8_t) { return add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int16_t) { return add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int32_t) { return add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int64_t) { return add_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint8_t) { return add_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint16_t) { return add_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint32_t) { return add_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint64_t) { return add_epi64(a, b); }

	WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m256i v) {
		return sse::add_epu8(sse::add_epi8(getlow(v), gethigh(v)));
	}
	WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m256i v) {
		return sse::add_epu16(sse::add_epi16(getlow(v), gethigh(v)));
	}
	WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m256i v) {
		return sse::add_epu32(sse::add_epi32(getlow(v), gethigh(v)));
	}
	WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m256i v) {
		return sse::add_epu64(sse::add_epi64(getlow(v), gethigh(v)));
	}
	WJR_INTRINSIC_INLINE static int8_t add_epi8(__m256i v) { return add_epu8(v); }
	WJR_INTRINSIC_INLINE static int16_t add_epi16(__m256i v) { return add_epu16(v); }
	WJR_INTRINSIC_INLINE static int32_t add_epi32(__m256i v) { return add_epu32(v); }
	WJR_INTRINSIC_INLINE static int64_t add_epi64(__m256i v) { return add_epu64(v); }

	WJR_INTRINSIC_INLINE static int8_t add(__m256i v, int8_t) { return add_epi8(v); }
	WJR_INTRINSIC_INLINE static int16_t add(__m256i v, int16_t) { return add_epi16(v); }
	WJR_INTRINSIC_INLINE static int32_t add(__m256i v, int32_t) { return add_epi32(v); }
	WJR_INTRINSIC_INLINE static int64_t add(__m256i v, int64_t) { return add_epi64(v); }
	WJR_INTRINSIC_INLINE static uint8_t add(__m256i v, uint8_t) { return add_epu8(v); }
	WJR_INTRINSIC_INLINE static uint16_t add(__m256i v, uint16_t) { return add_epu16(v); }
	WJR_INTRINSIC_INLINE static uint32_t add(__m256i v, uint32_t) { return add_epu32(v); }
	WJR_INTRINSIC_INLINE static uint64_t add(__m256i v, uint64_t) { return add_epu64(v); }

	WJR_INTRINSIC_INLINE static __m256i adds_epi8(__m256i a, __m256i b) { return _mm256_adds_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i adds_epi16(__m256i a, __m256i b) { return _mm256_adds_epi16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i adds_epu8(__m256i a, __m256i b) { return _mm256_adds_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i adds_epu16(__m256i a, __m256i b) { return _mm256_adds_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int8_t) { return adds_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int16_t) { return adds_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint8_t) { return adds_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint16_t) { return adds_epu16(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b) { return _mm256_alignr_epi8(a, b, imm8); }

	WJR_INTRINSIC_INLINE static __m256i avg_epu8(__m256i a, __m256i b) { return _mm256_avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i avg_epu16(__m256i a, __m256i b) { return _mm256_avg_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int8_t) { return avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int16_t) { return avg_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint8_t) { return avg_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint16_t) { return avg_epu16(a, b); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i blend_epi16(__m256i a, __m256i b) {
		return _mm256_blend_epi16(a, b, imm8);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i blend_epi32(__m256i a, __m256i b) {
		return _mm256_blend_epi32(a, b, imm8);
	}

	WJR_INTRINSIC_INLINE static __m256i blendv_epi8(__m256i a, __m256i b, __m256i mask) {
		return _mm256_blendv_epi8(a, b, mask);
	}
	
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i bslli_epi128(__m256i a) { return _mm256_bslli_epi128(a, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i bsrli_epi128(__m256i a) { return _mm256_bsrli_epi128(a, imm8); }

	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi8(__m256i a, __m256i b) { return _mm256_cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi16(__m256i a, __m256i b) { return _mm256_cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi32(__m256i a, __m256i b) { return _mm256_cmpeq_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi64(__m256i a, __m256i b) { return _mm256_cmpeq_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int8_t) { return cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int16_t) { return cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int32_t) { return cmpeq_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int64_t) { return cmpeq_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint8_t) { return cmpeq_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint16_t) { return cmpeq_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint32_t) { return cmpeq_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint64_t) { return cmpeq_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmpge_epi8(__m256i a, __m256i b) {
		return Not(cmpgt_epi8(b, a));
	}
	WJR_INTRINSIC_INLINE static __m256i cmpge_epi16(__m256i a, __m256i b) {
		return Not(cmpgt_epi16(b, a));
	}
	WJR_INTRINSIC_INLINE static __m256i cmpge_epi32(__m256i a, __m256i b) {
		return Not(cmpgt_epi32(b, a));
	}

	WJR_INTRINSIC_INLINE static __m256i cmpge_epu8(__m256i a, __m256i b) {
		return Not(cmpgt_epu8(b, a));
	}
	WJR_INTRINSIC_INLINE static __m256i cmpge_epu16(__m256i a, __m256i b) {
		return Not(cmpgt_epu16(b, a));
	}
	WJR_INTRINSIC_INLINE static __m256i cmpge_epu32(__m256i a, __m256i b) {
		return Not(cmpgt_epu32(b, a));
	}

	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int8_t) { return cmpge_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int16_t) { return cmpge_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int32_t) { return cmpge_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint8_t) { return cmpge_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint16_t) { return cmpge_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint32_t) { return cmpge_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi8(__m256i a, __m256i b) { return _mm256_cmpgt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi16(__m256i a, __m256i b) { return _mm256_cmpgt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi32(__m256i a, __m256i b) { return _mm256_cmpgt_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi64(__m256i a, __m256i b) { return _mm256_cmpgt_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu8(__m256i a, __m256i b) {
#if defined(__XOP__)
		__REGISTER_AVX_FROM_SSE(sse::cmpgt_epu8, a, b);
#else
		return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu16(__m256i a, __m256i b) {
#if defined(__XOP__)
		__REGISTER_AVX_FROM_SSE(sse::cmpgt_epu16, a, b);
#else
		return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu32(__m256i a, __m256i b) {
#if defined(__XOP__)
		__REGISTER_AVX_FROM_SSE(sse::cmpgt_epu32, a, b);
#else
		return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
#endif // __XOP__
	}
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu64(__m256i a, __m256i b) {
#if defined(__XOP__)
		__REGISTER_AVX_FROM_SSE(sse::cmpgt_epu64, a, b);
#else
		return cmpgt_epi64(Xor(a, setmin_epi64()), Xor(b, setmin_epi64()));
#endif // __XOP__
	}

	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int8_t) { return cmpgt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int16_t) { return cmpgt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int32_t) { return cmpgt_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int64_t) { return cmpgt_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint8_t) { return cmpgt_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint16_t) { return cmpgt_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint32_t) { return cmpgt_epu32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint64_t) { return cmpgt_epu64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmple_epi8(__m256i a, __m256i b) {
		return Not(cmpgt_epi8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m256i cmple_epi16(__m256i a, __m256i b) {
		return Not(cmpgt_epi16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m256i cmple_epi32(__m256i a, __m256i b) {
		return Not(cmpgt_epi32(a, b));
	}

	WJR_INTRINSIC_INLINE static __m256i cmple_epu8(__m256i a, __m256i b) {
		return Not(cmpgt_epu8(a, b));
	}
	WJR_INTRINSIC_INLINE static __m256i cmple_epu16(__m256i a, __m256i b) {
		return Not(cmpgt_epu16(a, b));
	}
	WJR_INTRINSIC_INLINE static __m256i cmple_epu32(__m256i a, __m256i b) {
		return Not(cmpgt_epu32(a, b));
	}

	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int8_t) { return cmple_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int16_t) { return cmple_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int32_t) { return cmple_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint8_t) { return cmple_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint16_t) { return cmple_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint32_t) { return cmple_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmplt_epi8(__m256i a, __m256i b) { return cmpgt_epi8(b, a); }
	WJR_INTRINSIC_INLINE static __m256i cmplt_epi16(__m256i a, __m256i b) { return cmpgt_epi16(b, a); }
	WJR_INTRINSIC_INLINE static __m256i cmplt_epi32(__m256i a, __m256i b) { return cmpgt_epi32(b, a); }

	WJR_INTRINSIC_INLINE static __m256i cmplt_epu8(__m256i a, __m256i b) {
		return cmpgt_epu8(b, a);
	}
	WJR_INTRINSIC_INLINE static __m256i cmplt_epu16(__m256i a, __m256i b) {
		return cmpgt_epu16(b, a);
	}
	WJR_INTRINSIC_INLINE static __m256i cmplt_epu32(__m256i a, __m256i b) {
		return cmpgt_epu32(b, a);
	}

	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int8_t) { return cmplt_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int16_t) { return cmplt_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int32_t) { return cmplt_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint8_t) { return cmplt_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint16_t) { return cmplt_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint32_t) { return cmplt_epu32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i cmpne_epi8(__m256i a, __m256i b) { return Not(cmpeq_epi8(a, b)); }
	WJR_INTRINSIC_INLINE static __m256i cmpne_epi16(__m256i a, __m256i b) { return Not(cmpeq_epi16(a, b)); }
	WJR_INTRINSIC_INLINE static __m256i cmpne_epi32(__m256i a, __m256i b) { return Not(cmpeq_epi32(a, b)); }

	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int8_t) { return cmpne_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int16_t) { return cmpne_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int32_t) { return cmpne_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint8_t) { return cmpne_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint16_t) { return cmpne_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint32_t) { return cmpne_epi32(a, b); }

	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::equal_to<>, T) {
		return cmpeq(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::not_equal_to<>, T) {
		return cmpne(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater<>, T) {
		return cmpgt(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater_equal<>, T) {
		return cmpge(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less<>, T) {
		return cmplt(a, b, T());
	}
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less_equal<>, T) {
		return cmple(a, b, T());
	}

	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu8(__m128i v);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu16(__m128i v);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu32(__m128i v);

	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, int8_t) { return cvtepu8<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, int16_t) { return cvtepu16<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, int32_t) { return cvtepu32<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, uint8_t) { return cvtepu8<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, uint16_t) { return cvtepu16<T>(v); }
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cvtepu(__m128i v, uint32_t) { return cvtepu32<T>(v); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi8(__m256i v) { return _mm256_extract_epi8(v, imm8); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi16(__m256i v) { return _mm256_extract_epi16(v, imm8); }

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m256i v, int8_t) { return extract_epi8<imm8>(v); }
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m256i v, int16_t) { return extract_epi16<imm8>(v); }

	WJR_INTRINSIC_INLINE static __m256i hadd_epi16(__m256i a, __m256i b) { return _mm256_hadd_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i hadd_epi32(__m256i a, __m256i b) { return _mm256_hadd_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int16_t) { return hadd_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int32_t) { return hadd_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i hadds_epi16(__m256i a, __m256i b) { return _mm256_hadds_epi16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i hsub_epi16(__m256i a, __m256i b) { return _mm256_hsub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i hsub_epi32(__m256i a, __m256i b) { return _mm256_hsub_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int16_t) { return hsub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int32_t) { return hsub_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i hsubs_epi16(__m256i a, __m256i b) { return _mm256_hsubs_epi16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i madd_epi16(__m256i a, __m256i b) {
		return _mm256_madd_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i max_epi8(__m256i a, __m256i b) {
		return _mm256_max_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max_epi16(__m256i a, __m256i b) {
		return _mm256_max_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max_epi32(__m256i a, __m256i b) {
		return _mm256_max_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max_epu8(__m256i a, __m256i b) {
		return _mm256_max_epu8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max_epu16(__m256i a, __m256i b) {
		return _mm256_max_epu16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max_epu32(__m256i a, __m256i b) {
		return _mm256_max_epu32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int8_t) { return max_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int16_t) { return max_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint8_t) { return max_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint16_t) { return max_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int32_t) { return max_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint32_t) { return max_epu32(a, b); }

	WJR_INTRINSIC_INLINE static int8_t max_epi8(__m256i a) {
		return sse::max_epi8(sse::max_epi8(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static int16_t max_epi16(__m256i a) {
		return sse::max_epi16(sse::max_epi16(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static int32_t max_epi32(__m256i a) {
		return sse::max_epi32(sse::max_epi32(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m256i a) {
		return sse::max_epu8(sse::max_epu8(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m256i a) {
		return sse::max_epu16(sse::max_epu16(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m256i a) {
		return sse::max_epu32(sse::max_epu32(getlow(a), gethigh(a)));
	}

	WJR_INTRINSIC_INLINE static int8_t max(__m256i a, int8_t) { return max_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t max(__m256i a, int16_t) { return max_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t max(__m256i a, int32_t) { return max_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t max(__m256i a, uint8_t) { return max_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t max(__m256i a, uint16_t) { return max_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t max(__m256i a, uint32_t) { return max_epu32(a); }

	WJR_INTRINSIC_INLINE static __m256i min_epi8(__m256i a, __m256i b) {
		return _mm256_min_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min_epi16(__m256i a, __m256i b) {
		return _mm256_min_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min_epi32(__m256i a, __m256i b) {
		return _mm256_min_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min_epu8(__m256i a, __m256i b) {
		return _mm256_min_epu8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min_epu16(__m256i a, __m256i b) {
		return _mm256_min_epu16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min_epu32(__m256i a, __m256i b) {
		return _mm256_min_epu32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int8_t) { return min_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int16_t) { return min_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint8_t) { return min_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint16_t) { return min_epu16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int32_t) { return min_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint32_t) { return min_epu32(a, b); }

	WJR_INTRINSIC_INLINE static int8_t min_epi8(__m256i a) {
		return sse::min_epi8(sse::min_epi8(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static int16_t min_epi16(__m256i a) {
		return sse::min_epi16(sse::min_epi16(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static int32_t min_epi32(__m256i a) {
		return sse::min_epi32(sse::min_epi32(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m256i a) {
		return sse::min_epu8(sse::min_epu8(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m256i a) {
		return sse::min_epu16(sse::min_epu16(getlow(a), gethigh(a)));
	}
	WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m256i a) {
		return sse::min_epu32(sse::min_epu32(getlow(a), gethigh(a)));
	}

	WJR_INTRINSIC_INLINE static int8_t min(__m256i a, int8_t) { return min_epi8(a); }
	WJR_INTRINSIC_INLINE static int16_t min(__m256i a, int16_t) { return min_epi16(a); }
	WJR_INTRINSIC_INLINE static int32_t min(__m256i a, int32_t) { return min_epi32(a); }
	WJR_INTRINSIC_INLINE static uint8_t min(__m256i a, uint8_t) { return min_epu8(a); }
	WJR_INTRINSIC_INLINE static uint16_t min(__m256i a, uint16_t) { return min_epu16(a); }
	WJR_INTRINSIC_INLINE static uint32_t min(__m256i a, uint32_t) { return min_epu32(a); }

	WJR_INTRINSIC_INLINE static int movemask_epi8(__m256i a) {
		return _mm256_movemask_epi8(a);
	}

	WJR_INTRINSIC_INLINE static __m256i mul_epi32(__m256i a, __m256i b) {
		return _mm256_mul_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i mul_epu32(__m256i a, __m256i b) {
		return _mm256_mul_epu32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i mulhi_epi16(__m256i a, __m256i b) {
		return _mm256_mulhi_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i mulhi_epu16(__m256i a, __m256i b) {
		return _mm256_mulhi_epu16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i mullo_epi16(__m256i a, __m256i b) {
		return _mm256_mullo_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i packs_epi16(__m256i a, __m256i b) {
		return _mm256_packs_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i packs_epi32(__m256i a, __m256i b) {
		return _mm256_packs_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i packus_epi16(__m256i a, __m256i b) {
		return _mm256_packus_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i packus_epi32(__m256i a, __m256i b) {
		return _mm256_packus_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i shuffle_epi8(__m256i a, __m256i b) {
		return _mm256_shuffle_epi8(a, b);
	}
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i shuffle_epi32(__m256i a) {
		return _mm256_shuffle_epi32(a, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i shufflehi_epi16(__m256i a) {
		return _mm256_shufflehi_epi16(a, imm8);
	}

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i shufflelo_epi16(__m256i a) {
		return _mm256_shufflelo_epi16(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m256i sll_epi16(__m256i a, __m128i b) {
		return _mm256_sll_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sll_epi32(__m256i a, __m128i b) {
		return _mm256_sll_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sll_epi64(__m256i a, __m128i b) {
		return _mm256_sll_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int16_t) { return sll_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int32_t) { return sll_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int64_t) { return sll_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint16_t) { return sll_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint32_t) { return sll_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint64_t) { return sll_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i slli_epi16(__m256i a, int imm8) {
		return _mm256_slli_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m256i slli_epi32(__m256i a, int imm8) {
		return _mm256_slli_epi32(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m256i slli_epi64(__m256i a, int imm8) {
		return _mm256_slli_epi64(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int16_t) { return slli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int32_t) { return slli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int64_t) { return slli_epi64(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint16_t) { return slli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint32_t) { return slli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint64_t) { return slli_epi64(a, imm8); }

	WJR_INTRINSIC_INLINE static __m256i sra_epi16(__m256i a, __m128i b) {
		return _mm256_sra_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sra_epi32(__m256i a, __m128i b) {
		return _mm256_sra_epi32(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int16_t) { return sra_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int32_t) { return sra_epi32(a, b); }

	WJR_INTRINSIC_INLINE static __m256i srai_epi16(__m256i a, int imm8) {
		return _mm256_srai_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m256i srai_epi32(__m256i a, int imm8) {
		return _mm256_srai_epi32(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int16_t) { return srai_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int32_t) { return srai_epi32(a, imm8); }

	WJR_INTRINSIC_INLINE static __m256i srl_epi16(__m256i a, __m128i b) {
		return _mm256_srl_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i srl_epi32(__m256i a, __m128i b) {
		return _mm256_srl_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i srl_epi64(__m256i a, __m128i b) {
		return _mm256_srl_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int16_t) { return srl_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int32_t) { return srl_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int64_t) { return srl_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint16_t) { return srl_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint32_t) { return srl_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint64_t) { return srl_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i srli_epi16(__m256i a, int imm8) {
		return _mm256_srli_epi16(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m256i srli_epi32(__m256i a, int imm8) {
		return _mm256_srli_epi32(a, imm8);
	}
	WJR_INTRINSIC_INLINE static __m256i srli_epi64(__m256i a, int imm8) {
		return _mm256_srli_epi64(a, imm8);
	}

	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int16_t) { return srli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int32_t) { return srli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int64_t) { return srli_epi64(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint16_t) { return srli_epi16(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint32_t) { return srli_epi32(a, imm8); }
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint64_t) { return srli_epi64(a, imm8); }

	WJR_INTRINSIC_INLINE static __m256i sub_epi8(__m256i a, __m256i b) {
		return _mm256_sub_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sub_epi16(__m256i a, __m256i b) {
		return _mm256_sub_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sub_epi32(__m256i a, __m256i b) {
		return _mm256_sub_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i sub_epi64(__m256i a, __m256i b) {
		return _mm256_sub_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int8_t) { return sub_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int16_t) { return sub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int32_t) { return sub_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int64_t) { return sub_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint8_t) { return sub_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint16_t) { return sub_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint32_t) { return sub_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint64_t) { return sub_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i subs_epi8(__m256i a, __m256i b) {
		return _mm256_subs_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i subs_epi16(__m256i a, __m256i b) {
		return _mm256_subs_epi16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i subs_epu8(__m256i a, __m256i b) {
		return _mm256_subs_epu8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i subs_epu16(__m256i a, __m256i b) {
		return _mm256_subs_epu16(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int8_t) { return subs_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int16_t) { return subs_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint8_t) { return subs_epu8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint16_t) { return subs_epu16(a, b); }

	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi8(__m256i a, __m256i b) {
		return _mm256_unpackhi_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi16(__m256i a, __m256i b) {
		return _mm256_unpackhi_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi32(__m256i a, __m256i b) {
		return _mm256_unpackhi_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi64(__m256i a, __m256i b) {
		return _mm256_unpackhi_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int8_t) { return unpackhi_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int16_t) { return unpackhi_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int32_t) { return unpackhi_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int64_t) { return unpackhi_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint8_t) { return unpackhi_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint16_t) { return unpackhi_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint32_t) { return unpackhi_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint64_t) { return unpackhi_epi64(a, b); }

	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi8(__m256i a, __m256i b) {
		return _mm256_unpacklo_epi8(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi16(__m256i a, __m256i b) {
		return _mm256_unpacklo_epi16(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi32(__m256i a, __m256i b) {
		return _mm256_unpacklo_epi32(a, b);
	}
	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi64(__m256i a, __m256i b) {
		return _mm256_unpacklo_epi64(a, b);
	}

	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int8_t) { return unpacklo_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int16_t) { return unpacklo_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int32_t) { return unpacklo_epi32(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int64_t) { return unpacklo_epi64(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint8_t) { return unpacklo_epi8(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint16_t) { return unpacklo_epi16(a, b); }
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint32_t) { return unpacklo_epi32(a, b); }
#endif // __AVX2__

};

_WJR_SIMD_END

#endif // __WJR_SIMD_SIMD_INTRIN_H