#pragma once
#ifndef __WJR_SIMD_SIMD_INTRIN_H
#define __WJR_SIMD_SIMD_INTRIN_H

#include <wjr/simd/simd_cast.h>
#include <wjr/type_traits.h>

_WJR_SIMD_BEGIN

#if WJR_SSE2

WJR_INTRINSIC_INLINE static __m128i mm_loadu_si16(void const* p);

WJR_INTRINSIC_INLINE static __m128i mm_loadu_si32(void const* p);

WJR_INTRINSIC_INLINE static __m128i mm_loadu_si64(void const* p);

#endif // WJR_SSE2

struct sse {
	using mask_type = uint16_t;
#if WJR_SSE
	using float_type = __m128;
#endif // WJR_SSE
#if WJR_SSE2
	using int_type = __m128i;
	using double_type = __m128d;
#endif // WJR_SSE2

	constexpr static size_t width();

	constexpr static mask_type mask();

#if WJR_SSE
	WJR_INTRINSIC_INLINE static mask_type movemask_ps(__m128 v);

	WJR_INTRINSIC_INLINE static void sfence();

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128 shuffle_ps(__m128 a, __m128 b);
#endif // WJR_SSE

#if WJR_SSE2

	WJR_INTRINSIC_INLINE static __m128i add_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i add_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i add_epi32(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i add_epi64(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int64_t);
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint64_t);

	WJR_INTRINSIC_INLINE static int8_t add_epi8(__m128i a);
	WJR_INTRINSIC_INLINE static int16_t add_epi16(__m128i a);
	WJR_INTRINSIC_INLINE static int32_t add_epi32(__m128i a);
	WJR_INTRINSIC_INLINE static int64_t add_epi64(__m128i a);

	WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m128i a);
	WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m128i a);
	WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m128i a);
	WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m128i a);

	WJR_INTRINSIC_INLINE static int8_t add(__m128i a, int8_t);
	WJR_INTRINSIC_INLINE static int16_t add(__m128i a, int16_t);
	WJR_INTRINSIC_INLINE static int32_t add(__m128i a, int32_t);
	WJR_INTRINSIC_INLINE static int64_t add(__m128i a, int64_t);
	WJR_INTRINSIC_INLINE static uint8_t add(__m128i a, uint8_t);
	WJR_INTRINSIC_INLINE static uint16_t add(__m128i a, uint16_t);
	WJR_INTRINSIC_INLINE static uint32_t add(__m128i a, uint32_t);
	WJR_INTRINSIC_INLINE static uint64_t add(__m128i a, uint64_t);

	WJR_INTRINSIC_INLINE static __m128i adds_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i adds_epi16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i adds_epu8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i adds_epu16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint16_t);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i alignr_epi16(__m128i a, __m128i b, int c);
	WJR_INTRINSIC_INLINE static __m128i alignr_epi32(__m128i a, __m128i b, int c);
	WJR_INTRINSIC_INLINE static __m128i alignr_epi64(__m128i a, __m128i b, int c);

	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int16_t);
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int32_t);
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int64_t);
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint32_t);
	WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint64_t);

	WJR_INTRINSIC_INLINE static __m128i And(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i AndNot(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i avg_epu8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i avg_epu16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint16_t);

	// notice that mask must be 0 or 255(every byte)
	WJR_INTRINSIC_INLINE static __m128i blendv_epi8(__m128i a, __m128i b, __m128i mask);
	WJR_INTRINSIC_INLINE static __m128i blendv_epi16(__m128i a, __m128i b, __m128i mask);
	WJR_INTRINSIC_INLINE static __m128i blendv_epi32(__m128i a, __m128i b, __m128i mask);

	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int8_t);
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int16_t);
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int32_t);
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint32_t);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i bslli(__m128i val);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i bsrli(__m128i val);

	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint32_t);

	WJR_INTRINSIC_INLINE static __m128i cmpge_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpge_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpge_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmpge_epu8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpge_epu16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpge_epu32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint32_t);

	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmpgt_epu8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epu16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpgt_epu32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint32_t);

	WJR_INTRINSIC_INLINE static __m128i cmple_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmple_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmple_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmple_epu8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmple_epu16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmple_epu32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint32_t);

	WJR_INTRINSIC_INLINE static __m128i cmplt_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmplt_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmplt_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmplt_epu8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmplt_epu16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmplt_epu32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint32_t);

	WJR_INTRINSIC_INLINE static __m128i cmpne_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpne_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i cmpne_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint32_t);

	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::equal_to<>, T);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::not_equal_to<>, T);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater<>, T);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater_equal<>, T);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less<>, T);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less_equal<>, T);

	WJR_INTRINSIC_INLINE static __m128i concat(uint64_t lo, uint64_t hi);

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi8(__m128i a);

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi16(__m128i a);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi32(__m128i a);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m128i a);

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, int8_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, int16_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, int32_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, int64_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, uint8_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, uint16_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m128i a, uint32_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, uint64_t);

	WJR_INTRINSIC_INLINE static uint64_t getlow(__m128i v);
	WJR_INTRINSIC_INLINE static uint64_t gethigh(__m128i v);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi16(__m128i a, int i);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int16_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint16_t);

	WJR_INTRINSIC_INLINE static void lfence();

	WJR_INTRINSIC_INLINE static __m128i load(const __m128i* ptr);
	WJR_INTRINSIC_INLINE static __m128i loadu(const __m128i* ptr);
	WJR_INTRINSIC_INLINE static __m128i loadu_si16(const void* ptr);
	WJR_INTRINSIC_INLINE static __m128i loadu_si32(const void* ptr);
	WJR_INTRINSIC_INLINE static __m128i loadu_si64(const void* ptr);

	template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m128i logical_and(__m128i a, __m128i b, T);

	template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m128i logical_not(__m128i v, T);

	template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m128i logical_or(__m128i a, __m128i b, T);

	WJR_INTRINSIC_INLINE static __m128i madd_epi16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static void maskmoveu(__m128i a, __m128i mask, char* mem_addr);

	WJR_INTRINSIC_INLINE static __m128i max_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i max_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i max_epi32(__m128i a, __m128i b);
	
	WJR_INTRINSIC_INLINE static __m128i max_epu8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i max_epu16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i max_epu32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint32_t);

	WJR_INTRINSIC_INLINE static int8_t max_epi8(__m128i a);
	WJR_INTRINSIC_INLINE static int16_t max_epi16(__m128i a);
	WJR_INTRINSIC_INLINE static int32_t max_epi32(__m128i a);
	
	WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m128i a);
	WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m128i a);
	WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m128i a);

	WJR_INTRINSIC_INLINE static int8_t max(__m128i a, int8_t);
	WJR_INTRINSIC_INLINE static int16_t max(__m128i a, int16_t);
	WJR_INTRINSIC_INLINE static int32_t max(__m128i a, int32_t);
	WJR_INTRINSIC_INLINE static uint8_t max(__m128i a, uint8_t);
	WJR_INTRINSIC_INLINE static uint16_t max(__m128i a, uint16_t);
	WJR_INTRINSIC_INLINE static uint32_t max(__m128i a, uint32_t);

	WJR_INTRINSIC_INLINE static void mfence();

	WJR_INTRINSIC_INLINE static __m128i min_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i min_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i min_epi32(__m128i a, __m128i b);
	
	WJR_INTRINSIC_INLINE static __m128i min_epu8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i min_epu16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i min_epu32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint32_t);

	WJR_INTRINSIC_INLINE static int8_t min_epi8(__m128i a);
	WJR_INTRINSIC_INLINE static int16_t min_epi16(__m128i a);
	WJR_INTRINSIC_INLINE static int32_t min_epi32(__m128i a);
	
	WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m128i a);
	WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m128i a);
	WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m128i a);

	WJR_INTRINSIC_INLINE static int8_t min(__m128i a, int8_t);
	WJR_INTRINSIC_INLINE static int16_t min(__m128i a, int16_t);
	WJR_INTRINSIC_INLINE static int32_t min(__m128i a, int32_t);
	
	WJR_INTRINSIC_INLINE static uint8_t min(__m128i a, uint8_t);
	WJR_INTRINSIC_INLINE static uint16_t min(__m128i a, uint16_t);
	WJR_INTRINSIC_INLINE static uint32_t min(__m128i a, uint32_t);

	WJR_INTRINSIC_INLINE static __m128i move_epi64(__m128i a);

	WJR_INTRINSIC_INLINE static mask_type movemask_epi8(__m128i a);
	WJR_INTRINSIC_INLINE static mask_type movemask_pd(__m128d v);

	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int8_t);
	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int32_t);
	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int64_t);
	
	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint8_t);
	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint32_t);
	WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint64_t);

	WJR_INTRINSIC_INLINE static __m128i mul_epu32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i mulhi_epi16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i mulhi_epu16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i mullo_epi16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i negate_epi8(__m128i a);
	WJR_INTRINSIC_INLINE static __m128i negate_epi16(__m128i a);
	WJR_INTRINSIC_INLINE static __m128i negate_epi32(__m128i a);
	WJR_INTRINSIC_INLINE static __m128i negate_epi64(__m128i a);

	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int8_t);
	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int16_t);
	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int32_t);
	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int64_t);
	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint32_t);
	WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint64_t);

	WJR_INTRINSIC_INLINE static __m128i Not(__m128i v);

	WJR_INTRINSIC_INLINE static __m128i Or(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i packs_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i packs_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i packus_epi16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i preloadu_si16(const void* ptr);
	WJR_INTRINSIC_INLINE static __m128i preloadu_si32(const void* ptr);
	WJR_INTRINSIC_INLINE static __m128i preloadu_si48(const void* ptr);
	WJR_INTRINSIC_INLINE static __m128i preloadu_si64(const void* ptr);
	WJR_INTRINSIC_INLINE static __m128i preloadu_si80(const void* ptr);
	WJR_INTRINSIC_INLINE static __m128i preloadu_si96(const void* ptr);
	WJR_INTRINSIC_INLINE static __m128i preloadu_si112(const void* ptr);
	WJR_INTRINSIC_INLINE static __m128i preloadu_si128(const void* ptr);

	WJR_INTRINSIC_INLINE static __m128i preloadu_si16x(const void* ptr, int n);

	WJR_INTRINSIC_INLINE static __m128i sad_epu8(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i zeros();
	WJR_INTRINSIC_INLINE static __m128i ones();

	WJR_INTRINSIC_INLINE static __m128i set_epi8(
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0);

	WJR_INTRINSIC_INLINE static __m128i set_epi16(
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0);
	WJR_INTRINSIC_INLINE static __m128i set_epi32(int e3, int e2, int e1, int e0);
	WJR_INTRINSIC_INLINE static __m128i set_epi64x(long long e1, long long e0);

	WJR_INTRINSIC_INLINE static __m128i setr_epi8(
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0);

	WJR_INTRINSIC_INLINE static __m128i setr_epi16(
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0);
	WJR_INTRINSIC_INLINE static __m128i setr_epi32(int e3, int e2, int e1, int e0);

	WJR_INTRINSIC_INLINE static __m128i set1_epi8(int8_t val);
	WJR_INTRINSIC_INLINE static __m128i set1_epi16(int16_t val);
	WJR_INTRINSIC_INLINE static __m128i set1_epi32(int32_t val);
	WJR_INTRINSIC_INLINE static __m128i set1_epi64(int64_t val);

	WJR_INTRINSIC_INLINE static __m128i set1(int8_t val, int8_t);
	WJR_INTRINSIC_INLINE static __m128i set1(int16_t val, int16_t);
	WJR_INTRINSIC_INLINE static __m128i set1(int32_t val, int32_t);
	WJR_INTRINSIC_INLINE static __m128i set1(int64_t val, int64_t);
	WJR_INTRINSIC_INLINE static __m128i set1(uint8_t val, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i set1(uint16_t val, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i set1(uint32_t val, uint32_t);
	WJR_INTRINSIC_INLINE static __m128i set1(uint64_t val, uint64_t);

	WJR_INTRINSIC_INLINE static __m128i setmin_epi8();
	WJR_INTRINSIC_INLINE static __m128i setmin_epi16();
	WJR_INTRINSIC_INLINE static __m128i setmin_epi32();

	WJR_INTRINSIC_INLINE static __m128i setmin(int8_t);
	WJR_INTRINSIC_INLINE static __m128i setmin(int16_t);
	WJR_INTRINSIC_INLINE static __m128i setmin(int32_t);
	WJR_INTRINSIC_INLINE static __m128i setmin(uint8_t);
	WJR_INTRINSIC_INLINE static __m128i setmin(uint16_t);
	WJR_INTRINSIC_INLINE static __m128i setmin(uint32_t);

	WJR_INTRINSIC_INLINE static __m128i setmax_epi8();
	WJR_INTRINSIC_INLINE static __m128i setmax_epi16();
	WJR_INTRINSIC_INLINE static __m128i setmax_epi32();

	WJR_INTRINSIC_INLINE static __m128i setmax(int8_t);
	WJR_INTRINSIC_INLINE static __m128i setmax(int16_t);
	WJR_INTRINSIC_INLINE static __m128i setmax(int32_t);
	WJR_INTRINSIC_INLINE static __m128i setmax(uint8_t);
	WJR_INTRINSIC_INLINE static __m128i setmax(uint16_t);
	WJR_INTRINSIC_INLINE static __m128i setmax(uint32_t);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i shuffle_epi32(__m128i v);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i shufflehi_epi16(__m128i v);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i shufflelo_epi16(__m128i v);

	WJR_INTRINSIC_INLINE static __m128i sll_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i sll_epi32(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i sll_epi64(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int64_t);
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint64_t);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i v);
	WJR_INTRINSIC_INLINE static __m128i slli_epi16(__m128i a, int imm8);
	WJR_INTRINSIC_INLINE static __m128i slli_epi32(__m128i a, int imm8);
	WJR_INTRINSIC_INLINE static __m128i slli_epi64(__m128i a, int imm8);

	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int16_t);
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int32_t);
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int64_t);
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint32_t);
	WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint64_t);

	WJR_INTRINSIC_INLINE static __m128i sra_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i sra_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int32_t);

	WJR_INTRINSIC_INLINE static __m128i srai_epi16(__m128i a, int imm8);
	WJR_INTRINSIC_INLINE static __m128i srai_epi32(__m128i a, int imm8);

	WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int16_t);
	WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int32_t);

	WJR_INTRINSIC_INLINE static __m128i srl_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i srl_epi32(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i srl_epi64(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int64_t);
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint64_t);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i v);
	WJR_INTRINSIC_INLINE static __m128i srli_epi16(__m128i a, int imm8);
	WJR_INTRINSIC_INLINE static __m128i srli_epi32(__m128i a, int imm8);
	WJR_INTRINSIC_INLINE static __m128i srli_epi64(__m128i a, int imm8);

	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int16_t);
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int32_t);
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int64_t);
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint32_t);
	WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint64_t);

	WJR_INTRINSIC_INLINE static void stream(__m128i* ptr, __m128i v);

	WJR_INTRINSIC_INLINE static void store(__m128i* ptr, __m128i val);
	WJR_INTRINSIC_INLINE static void storeu(__m128i* ptr, __m128i val);

	WJR_INTRINSIC_INLINE static __m128i sub_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i sub_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i sub_epi32(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i sub_epi64(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int64_t);
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint64_t);

	WJR_INTRINSIC_INLINE static __m128i subs_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i subs_epi16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i subs_epu8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i subs_epu16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint16_t);

	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi32(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i unpackhi_epi64(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int64_t);
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint64_t);

	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi32(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i unpacklo_epi64(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int64_t);
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint32_t);

	WJR_INTRINSIC_INLINE static __m128i Xor(__m128i a, __m128i b);

#endif // WJR_SSE2

#if WJR_SSE3
	WJR_INTRINSIC_INLINE static __m128i lddqu(const __m128i* ptr);
#endif // WJR_SSE3

#if WJR_SSSE3

	WJR_INTRINSIC_INLINE static __m128i abs_epi8(__m128i val);
	WJR_INTRINSIC_INLINE static __m128i abs_epi16(__m128i val);
	WJR_INTRINSIC_INLINE static __m128i abs_epi32(__m128i val);

	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int8_t);
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int16_t);
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int32_t);
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint32_t);

	WJR_INTRINSIC_INLINE static __m128i shuffle_epi8(__m128i v, __m128i imm8);

	WJR_INTRINSIC_INLINE static __m128i sign_epi8(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i sign_epi16(__m128i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m128i sign_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int8_t);
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint32_t);

#endif // WJR_SSSE3

#if WJR_SSE4_1
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i blend_epi16(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmpeq_epi64(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int64_t);
	WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint64_t);

	WJR_INTRINSIC_INLINE static __m128i cmpgt_epi64(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int64_t);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi8(__m128i a, int i);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi32(__m128i a, int i);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert_epi64(__m128i a, int64_t i);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int8_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int32_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, int64_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint8_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint32_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, uint64_t);

	WJR_INTRINSIC_INLINE static __m128i minpos_epu16(__m128i a);

	WJR_INTRINSIC_INLINE static __m128i mul_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i mullo_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i packus_epi32(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m128i stream_load(__m128i* p);

	WJR_INTRINSIC_INLINE static int test_all_ones(__m128i a);

	WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a);

	WJR_INTRINSIC_INLINE static int test_mix_ones_zeros(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static int testc(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static int testnzc(__m128i a, __m128i b);

	WJR_INTRINSIC_INLINE static int testz(__m128i a, __m128i b);
#endif // WJR_SSE4_1

};

struct avx {
	using mask_type = uint32_t;
#if WJR_AVX
	using float_type = __m256;
	using int_type = __m256i;
	using double_type = __m256d;
#endif // WJR_AVX

	constexpr static size_t width();

	constexpr static mask_type mask();

#if WJR_AVX

	WJR_INTRINSIC_INLINE static __m256i concat(__m128i a, __m128i b);

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi32(__m256i v);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m256i v);

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m256i v, int32_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int64_t extract(__m256i v, int64_t);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m128i extract_si128(__m256i v);

	WJR_INTRINSIC_INLINE static __m128i getlow(__m256i a);

	WJR_INTRINSIC_INLINE static __m128i gethigh(__m256i a);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi8(__m256i v, int8_t i);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi16(__m256i v, int16_t i);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi32(__m256i v, int32_t i);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_epi64(__m256i v, int64_t i);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i insert_si128(__m256i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m256i load(const __m256i* p);
	WJR_INTRINSIC_INLINE static __m256i loadu(const __m256i* p);

	WJR_INTRINSIC_INLINE static __m256i ones();

	WJR_INTRINSIC_INLINE static __m256i preloadu_si16(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si32(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si48(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si64(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si80(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si96(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si112(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si128(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si144(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si160(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si176(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si192(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si208(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si224(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si240(const void* ptr);
	WJR_INTRINSIC_INLINE static __m256i preloadu_si256(const void* ptr);

	WJR_INTRINSIC_INLINE static __m256i preloadu_si16x(const void* ptr, int n);

	WJR_INTRINSIC_INLINE static __m256i set_epi8(
		char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
		char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0);
	
	WJR_INTRINSIC_INLINE static __m256i set_epi16(
		short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0);

	WJR_INTRINSIC_INLINE static __m256i set_epi32(
		int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0);

	WJR_INTRINSIC_INLINE static __m256i set_epi64x(
		long long e3, long long e2, long long e1, long long e0);

	WJR_INTRINSIC_INLINE static __m256i setr_epi8(
		char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
		char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
		char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
		char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0);
	
	WJR_INTRINSIC_INLINE static __m256i setr_epi16(
		short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
		short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0);

	WJR_INTRINSIC_INLINE static __m256i setr_epi32(
		int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0);
	
	WJR_INTRINSIC_INLINE static __m256i setr_epi64x(
		long long e3, long long e2, long long e1, long long e0);

	WJR_INTRINSIC_INLINE static __m256i set1_epi8(int8_t a);
	WJR_INTRINSIC_INLINE static __m256i set1_epi16(int16_t a);
	WJR_INTRINSIC_INLINE static __m256i set1_epi32(int32_t a);
	WJR_INTRINSIC_INLINE static __m256i set1_epi64(int64_t a);

	WJR_INTRINSIC_INLINE static __m256i set1(int8_t a, int8_t);
	WJR_INTRINSIC_INLINE static __m256i set1(int16_t a, int16_t);
	WJR_INTRINSIC_INLINE static __m256i set1(int32_t a, int32_t);
	WJR_INTRINSIC_INLINE static __m256i set1(int64_t a, int64_t);
	WJR_INTRINSIC_INLINE static __m256i set1(uint8_t a, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i set1(uint16_t a, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i set1(uint32_t a, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i set1(uint64_t a, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i setmin_epi8();
	WJR_INTRINSIC_INLINE static __m256i setmin_epi16();
	WJR_INTRINSIC_INLINE static __m256i setmin_epi32();
	WJR_INTRINSIC_INLINE static __m256i setmin_epi64();

	WJR_INTRINSIC_INLINE static __m256i setmin(int8_t);
	WJR_INTRINSIC_INLINE static __m256i setmin(int16_t);
	WJR_INTRINSIC_INLINE static __m256i setmin(int32_t);
	WJR_INTRINSIC_INLINE static __m256i setmin(int64_t);

	WJR_INTRINSIC_INLINE static __m256i setmax_epi8();
	WJR_INTRINSIC_INLINE static __m256i setmax_epi16();
	WJR_INTRINSIC_INLINE static __m256i setmax_epi32();
	WJR_INTRINSIC_INLINE static __m256i setmax_epi64();

	WJR_INTRINSIC_INLINE static __m256i setmax(int8_t);
	WJR_INTRINSIC_INLINE static __m256i setmax(int16_t);
	WJR_INTRINSIC_INLINE static __m256i setmax(int32_t);
	WJR_INTRINSIC_INLINE static __m256i setmax(int64_t);

	WJR_INTRINSIC_INLINE static void stream(__m256i* p, __m256i a);

	WJR_INTRINSIC_INLINE static void store(__m256i* p, const __m256i& a);
	WJR_INTRINSIC_INLINE static void storeu(__m256i* p, const __m256i& a);

	WJR_INTRINSIC_INLINE static int test_all_zeros(__m256i a);

	WJR_INTRINSIC_INLINE static int testc(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static int testnzc(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static int testz(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i zeros();
#endif // WJR_AVX

#if WJR_AVX2
	WJR_INTRINSIC_INLINE static __m256i And(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i AndNot(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i Or(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i Xor(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i Not(__m256i v);

	WJR_INTRINSIC_INLINE static __m256i abs_epi8(__m256i v);
	WJR_INTRINSIC_INLINE static __m256i abs_epi16(__m256i v);
	WJR_INTRINSIC_INLINE static __m256i abs_epi32(__m256i v);

	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int8_t);
	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int16_t);
	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int32_t);
	WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int64_t);

	WJR_INTRINSIC_INLINE static __m256i add_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i add_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i add_epi32(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i add_epi64(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int64_t);
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint64_t);

	WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m256i v);
	WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m256i v);
	WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m256i v);
	WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m256i v);
	
	WJR_INTRINSIC_INLINE static int8_t add_epi8(__m256i v);
	WJR_INTRINSIC_INLINE static int16_t add_epi16(__m256i v);
	WJR_INTRINSIC_INLINE static int32_t add_epi32(__m256i v);
	WJR_INTRINSIC_INLINE static int64_t add_epi64(__m256i v);

	WJR_INTRINSIC_INLINE static int8_t add(__m256i v, int8_t);
	WJR_INTRINSIC_INLINE static int16_t add(__m256i v, int16_t);
	WJR_INTRINSIC_INLINE static int32_t add(__m256i v, int32_t);
	WJR_INTRINSIC_INLINE static int64_t add(__m256i v, int64_t);
	WJR_INTRINSIC_INLINE static uint8_t add(__m256i v, uint8_t);
	WJR_INTRINSIC_INLINE static uint16_t add(__m256i v, uint16_t);
	WJR_INTRINSIC_INLINE static uint32_t add(__m256i v, uint32_t);
	WJR_INTRINSIC_INLINE static uint64_t add(__m256i v, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i adds_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i adds_epi16(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i adds_epu8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i adds_epu16(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint16_t);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i alignr_epi16(__m256i a, __m256i b, int c);
	WJR_INTRINSIC_INLINE static __m256i alignr_epi32(__m256i a, __m256i b, int c);
	WJR_INTRINSIC_INLINE static __m256i alignr_epi64(__m256i a, __m256i b, int c);

	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int16_t);
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int32_t);
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int64_t);
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i avg_epu8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i avg_epu16(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint16_t);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i blend_epi16(__m256i a, __m256i b);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i blend_epi32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i blendv_epi8(__m256i a, __m256i b, __m256i mask);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i bslli_epi128(__m256i a);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i bsrli_epi128(__m256i a);

	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi32(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpeq_epi64(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int64_t);
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i cmpge_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpge_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpge_epi32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i cmpge_epu8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpge_epu16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpge_epu32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint32_t);

	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi32(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epi64(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu32(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpgt_epu64(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int64_t);
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i cmple_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmple_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmple_epi32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i cmple_epu8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmple_epu16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmple_epu32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint32_t);

	WJR_INTRINSIC_INLINE static __m256i cmplt_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmplt_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmplt_epi32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i cmplt_epu8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmplt_epu16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmplt_epu32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint32_t);

	WJR_INTRINSIC_INLINE static __m256i cmpne_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpne_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i cmpne_epi32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint32_t);

	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::equal_to<>, T);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::not_equal_to<>, T);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater<>, T);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater_equal<>, T);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less<>, T);
	template<typename T>
	WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less_equal<>, T);

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi8(__m256i v);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract_epi16(__m256i v);

	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m256i v, int8_t);
	template<int imm8>
	WJR_INTRINSIC_INLINE static int extract(__m256i v, int16_t);

	WJR_INTRINSIC_INLINE static __m256i hadd_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i hadd_epi32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int32_t);

	WJR_INTRINSIC_INLINE static __m256i hadds_epi16(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i hsub_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i hsub_epi32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int32_t);

	WJR_INTRINSIC_INLINE static __m256i hsubs_epi16(__m256i a, __m256i b);

	template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m256i logical_and(__m256i a, __m256i b, T);

	template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m256i logical_not(__m256i v, T);

	template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
	WJR_INTRINSIC_INLINE static __m256i logical_or(__m256i a, __m256i b, T);

	WJR_INTRINSIC_INLINE static __m256i madd_epi16(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i max_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i max_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i max_epi32(__m256i a, __m256i b);
	
	WJR_INTRINSIC_INLINE static __m256i max_epu8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i max_epu16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i max_epu32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint32_t);

	WJR_INTRINSIC_INLINE static int8_t max_epi8(__m256i a);
	WJR_INTRINSIC_INLINE static int16_t max_epi16(__m256i a);
	WJR_INTRINSIC_INLINE static int32_t max_epi32(__m256i a);
	WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m256i a);
	WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m256i a);
	WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m256i a);

	WJR_INTRINSIC_INLINE static int8_t max(__m256i a, int8_t);
	WJR_INTRINSIC_INLINE static int16_t max(__m256i a, int16_t);
	WJR_INTRINSIC_INLINE static int32_t max(__m256i a, int32_t);
	
	WJR_INTRINSIC_INLINE static uint8_t max(__m256i a, uint8_t);
	WJR_INTRINSIC_INLINE static uint16_t max(__m256i a, uint16_t);
	WJR_INTRINSIC_INLINE static uint32_t max(__m256i a, uint32_t);

	WJR_INTRINSIC_INLINE static __m256i min_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i min_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i min_epi32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i min_epu8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i min_epu16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i min_epu32(__m256i a, __m256i b);
	
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint32_t);

	WJR_INTRINSIC_INLINE static int8_t min_epi8(__m256i a);
	WJR_INTRINSIC_INLINE static int16_t min_epi16(__m256i a);
	WJR_INTRINSIC_INLINE static int32_t min_epi32(__m256i a);
	
	WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m256i a);
	WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m256i a);
	WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m256i a);

	WJR_INTRINSIC_INLINE static int8_t min(__m256i a, int8_t);
	WJR_INTRINSIC_INLINE static int16_t min(__m256i a, int16_t);
	WJR_INTRINSIC_INLINE static int32_t min(__m256i a, int32_t);
	WJR_INTRINSIC_INLINE static uint8_t min(__m256i a, uint8_t);
	WJR_INTRINSIC_INLINE static uint16_t min(__m256i a, uint16_t);
	WJR_INTRINSIC_INLINE static uint32_t min(__m256i a, uint32_t);

	WJR_INTRINSIC_INLINE static mask_type movemask_epi8(__m256i a);

	WJR_INTRINSIC_INLINE static __m256i mul_epi32(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i mul_epu32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i mulhi_epi16(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i mulhi_epu16(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i mullo_epi16(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i packs_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i packs_epi32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i packus_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i packus_epi32(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i shuffle_epi8(__m256i a, __m256i b);
	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i shuffle_epi32(__m256i a);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i shufflehi_epi16(__m256i a);

	template<int imm8>
	WJR_INTRINSIC_INLINE static __m256i shufflelo_epi16(__m256i a);

	WJR_INTRINSIC_INLINE static __m256i sll_epi16(__m256i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m256i sll_epi32(__m256i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m256i sll_epi64(__m256i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int64_t);
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i slli_epi16(__m256i a, int imm8);
	WJR_INTRINSIC_INLINE static __m256i slli_epi32(__m256i a, int imm8);
	WJR_INTRINSIC_INLINE static __m256i slli_epi64(__m256i a, int imm8);

	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int16_t);
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int32_t);
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int64_t);
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i sra_epi16(__m256i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m256i sra_epi32(__m256i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int32_t);

	WJR_INTRINSIC_INLINE static __m256i srai_epi16(__m256i a, int imm8);
	WJR_INTRINSIC_INLINE static __m256i srai_epi32(__m256i a, int imm8);

	WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int16_t);
	WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int32_t);

	WJR_INTRINSIC_INLINE static __m256i stream_load(__m256i const* p);

	WJR_INTRINSIC_INLINE static __m256i srl_epi16(__m256i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m256i srl_epi32(__m256i a, __m128i b);
	WJR_INTRINSIC_INLINE static __m256i srl_epi64(__m256i a, __m128i b);

	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int64_t);
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i srli_epi16(__m256i a, int imm8);
	WJR_INTRINSIC_INLINE static __m256i srli_epi32(__m256i a, int imm8);
	WJR_INTRINSIC_INLINE static __m256i srli_epi64(__m256i a, int imm8);

	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int16_t);
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int32_t);
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int64_t);
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i sub_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i sub_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i sub_epi32(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i sub_epi64(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int64_t);
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i subs_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i subs_epi16(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i subs_epu8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i subs_epu16(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint16_t);

	WJR_INTRINSIC_INLINE static int test_all_ones(__m256i a);

	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi32(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i unpackhi_epi64(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int64_t);
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint32_t);
	WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint64_t);

	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi8(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi16(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi32(__m256i a, __m256i b);
	WJR_INTRINSIC_INLINE static __m256i unpacklo_epi64(__m256i a, __m256i b);

	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int8_t);
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int16_t);
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int32_t);
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int64_t);
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint8_t);
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint16_t);
	WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint32_t);
#endif // WJR_AVX2

};

#define WJR_SIMD_WIDTH(simd_t) simd_t::width()
#define WJR_SIMD_VALUE_SIZE(ptr) sizeof(decltype(*(ptr)))
#define WJR_SIMD_OFFSET(simd_t, ptr) (WJR_SIMD_WIDTH(simd_t) / (8 * WJR_SIMD_VALUE_SIZE(ptr)))
#define WJR_SIMD_ALIGNMENT(simd_t, ptr) (WJR_SIMD_WIDTH(simd_t) / (WJR_SIMD_VALUE_SIZE(ptr)))
#define WJR_SIMD_UINTPTR(ptr) reinterpret_cast<uintptr_t>(ptr)

#define WJR_SIMD_SUFFIX_0 _FORWARD
#define WJR_SIMD_SUFFIX_1 _BACKWARD

// use this to control if is forward of backward
//#define WJR_SIMD_IS_BACKWARD 0

#define WJR_SIMD_SUFFIX WJR_MACRO_CONCAT(WJR_SIMD_SUFFIX_, WJR_SIMD_IS_BACKWARD)
#define _WJR_SIMD_USE_SUFFIX(MACRO, SUFFIX) WJR_MACRO_CONCAT(MACRO, SUFFIX)
#define WJR_SIMD_USE_SUFFIX(MACRO) _WJR_SIMD_USE_SUFFIX(MACRO, WJR_SIMD_SUFFIX)

#define WJR_SIMD_LEFT_PTR_N_FORWARD(ptr, N) (ptr)
#define WJR_SIMD_RIGHT_PTR_N_FORWARD(ptr, N) (ptr + (N))
#define WJR_SIMD_LEFT_PTR_N_BACKWARD(ptr, N) (ptr - (N))
#define WJR_SIMD_RIGHT_PTR_N_BACKWARD(ptr, N) (ptr)

#define WJR_SIMD_LEFT_PTR_N(ptr, N) WJR_SIMD_USE_SUFFIX(WJR_SIMD_LEFT_PTR_N)(ptr, N)
#define WJR_SIMD_RIGHT_PTR_N(ptr, N) WJR_SIMD_USE_SUFFIX(WJR_SIMD_RIGHT_PTR_N)(ptr, N)

#define WJR_SIMD_PTR(ptr) WJR_SIMD_LEFT_PTR_N(ptr, 1)

#define WJR_SIMD_LEFT_PTR(simd_t, ptr) WJR_SIMD_LEFT_PTR_N(ptr, WJR_SIMD_OFFSET(simd_t, ptr))
#define WJR_SIMD_RIGHT_PTR(simd_t, ptr) WJR_SIMD_RIGHT_PTR_N(ptr, WJR_SIMD_OFFSET(simd_t, ptr))

// do nothing
#define WJR_SIMD_INIT_PTR_FORWARD(ptr, n) 
#define WJR_SIMD_INIT_PTR_BACKWARD(ptr, n) (ptr += n)

#define WJR_SIMD_INIT_PTR(ptr, n) WJR_SIMD_USE_SUFFIX(WJR_SIMD_INIT_PTR)(ptr, n)

#define WJR_SIMD_ADD_PTR_FORWARD(ptr, n) (ptr + (n))
#define WJR_SIMD_SUB_PTR_FORWARD(ptr, n) (ptr - (n))
#define WJR_SIMD_ADD_PTR_BACKWARD(ptr, n) WJR_SIMD_SUB_PTR_FORWARD(ptr, n)
#define WJR_SIMD_SUB_PTR_BACKWARD(ptr, n) WJR_SIMD_ADD_PTR_FORWARD(ptr, n)

#define WJR_SIMD_ADD_PTR(ptr, n) WJR_SIMD_USE_SUFFIX(WJR_SIMD_ADD_PTR)(ptr, n)
#define WJR_SIMD_SUB_PTR(ptr, n) WJR_SIMD_USE_SUFFIX(WJR_SIMD_SUB_PTR)(ptr, n)

#define WJR_SIMD_INC_PTR_FORWARD(ptr, n)	(ptr += n)
#define WJR_SIMD_DEC_PTR_FORWARD(ptr, n)	(ptr -= n)
#define WJR_SIMD_INC_PTR_BACKWARD(ptr, n)	WJR_SIMD_DEC_PTR_FORWARD(ptr, n)
#define WJR_SIMD_DEC_PTR_BACKWARD(ptr, n)	WJR_SIMD_INC_PTR_FORWARD(ptr, n)

#define WJR_SIMD_INC_PTR(ptr, n) WJR_SIMD_USE_SUFFIX(WJR_SIMD_INC_PTR)(ptr, n)
#define WJR_SIMD_DEC_PTR(ptr, n) WJR_SIMD_USE_SUFFIX(WJR_SIMD_DEC_PTR)(ptr, n)

#define WJR_SIMD_CHECK_ALIGN(W) static_assert(wjr::has_single_bit(W), "invalid align");

#define WJR_SIMD_FLOOR_ALIGN_OFFSET_FORWARD(ptr, W) (WJR_SIMD_UINTPTR(ptr) & ((W) - 1))
#define WJR_SIMD_CEIL_ALIGN_OFFSET_FORWARD(ptr, W) ((-WJR_SIMD_UINTPTR(ptr)) & ((W) - 1))
#define WJR_SIMD_FLOOR_ALIGN_OFFSET_BACKWARD(ptr, W) WJR_SIMD_CEIL_ALIGN_OFFSET_FORWARD(ptr, W)
#define WJR_SIMD_CEIL_ALIGN_OFFSET_BACKWARD(ptr, W) WJR_SIMD_FLOOR_ALIGN_OFFSET_FORWARD(ptr, W)

#define WJR_SIMD_FLOOR_ALIGN_OFFSET(ptr, W) WJR_SIMD_USE_SUFFIX(WJR_SIMD_FLOOR_ALIGN_OFFSET)(ptr, W)
#define WJR_SIMD_CEIL_ALIGN_OFFSET(ptr, W) WJR_SIMD_USE_SUFFIX(WJR_SIMD_CEIL_ALIGN_OFFSET)(ptr, W)

#define WJR_SIMD_FLOOR_ALIGN_PTR_FORWARD(ptr, W) reinterpret_cast<decltype(ptr)>(WJR_SIMD_UINTPTR(ptr) & (~((W) - 1)))
#define WJR_SIMD_CEIL_ALIGN_PTR_FORWARD(ptr, W) reinterpret_cast<decltype(ptr)>((WJR_SIMD_UINTPTR(ptr) + (W) - 1) & (~((W) - 1)))
#define WJR_SIMD_FLOOR_ALIGN_PTR_BACKWARD(ptr, W) WJR_SIMD_CEIL_ALIGN_PTR_FORWARD(ptr, W)
#define WJR_SIMD_CEIL_ALIGN_PTR_BACKWARD(ptr, W) WJR_SIMD_FLOOR_ALIGN_PTR_FORWARD(ptr, W)

#define WJR_SIMD_FLOOR_ALIGN_PTR(ptr, W) WJR_SIMD_USE_SUFFIX(WJR_SIMD_FLOOR_ALIGN_PTR)(ptr, W)
#define WJR_SIMD_CEIL_ALIGN_PTR(ptr, W) WJR_SIMD_USE_SUFFIX(WJR_SIMD_CEIL_ALIGN_PTR)(ptr, W)

#define WJR_SIMD_FIRST_ZERO_FORWARD(x) wjr::countr_one(x)
#define WJR_SIMD_LAST_ZERO_FORWARD(x) wjr::countl_one(x)
#define WJR_SIMD_FIRST_ONE_FORWARD(x) wjr::countr_zero(x)
#define WJR_SIMD_LAST_ONE_FORWARD(x) wjr::countl_zero(x)
#define WJR_SIMD_FIRST_ZERO_BACKWARD(x) WJR_SIMD_LAST_ZERO_FORWARD(x)
#define WJR_SIMD_LAST_ZERO_BACKWARD(x) WJR_SIMD_FIRST_ZERO_FORWARD(x)
#define WJR_SIMD_FIRST_ONE_BACKWARD(x) WJR_SIMD_LAST_ONE_FORWARD(x)
#define WJR_SIMD_LAST_ONE_BACKWARD(x) WJR_SIMD_FIRST_ONE_FORWARD(x)

#define WJR_SIMD_FIRST_ZERO(x) WJR_SIMD_USE_SUFFIX(WJR_SIMD_FIRST_ZERO)(x)
#define WJR_SIMD_LAST_ZERO(x) WJR_SIMD_USE_SUFFIX(WJR_SIMD_LAST_ZERO)(x)
#define WJR_SIMD_FIRST_ONE(x) WJR_SIMD_USE_SUFFIX(WJR_SIMD_FIRST_ONE)(x)
#define WJR_SIMD_LAST_ONE(x) WJR_SIMD_USE_SUFFIX(WJR_SIMD_LAST_ONE)(x)

#define _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, FUNC, SUFFIX) \
_WJR_SIMD_USE_SUFFIX(WJR_SIMD_ADD_PTR, SUFFIX)(ptr, FUNC(x) / WJR_SIMD_VALUE_SIZE(ptr))

#define WJR_SIMD_FIRST_ZERO_PTR_FORWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_FIRST_ZERO, WJR_SIMD_FORWARD_SUFFIX)
#define WJR_SIMD_LAST_ZERO_PTR_FORWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_LAST_ZERO, WJR_SIMD_FORWARD_SUFFIX)
#define WJR_SIMD_FIRST_ONE_PTR_FORWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_FIRST_ONE, WJR_SIMD_FORWARD_SUFFIX)
#define WJR_SIMD_LAST_ONE_PTR_FORWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_LAST_ONE, WJR_SIMD_FORWARD_SUFFIX)

#define WJR_SIMD_FIRST_ZERO_PTR_BACKWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_FIRST_ZERO, WJR_SIMD_BACKWARD_SUFFIX)
#define WJR_SIMD_LAST_ZERO_PTR_BACKWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_LAST_ZERO, WJR_SIMD_BACKWARD_SUFFIX)
#define WJR_SIMD_FIRST_ONE_PTR_BACKWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_FIRST_ONE, WJR_SIMD_BACKWARD_SUFFIX)
#define WJR_SIMD_LAST_ONE_PTR_BACKWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_LAST_ONE, WJR_SIMD_BACKWARD_SUFFIX)

#define WJR_SIMD_FIRST_ZERO_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_FIRST_ZERO(x) / WJR_SIMD_VALUE_SIZE(ptr))
#define WJR_SIMD_LAST_ZERO_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_LAST_ZERO(x) / WJR_SIMD_VALUE_SIZE(ptr))
#define WJR_SIMD_FIRST_ONE_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_FIRST_ONE(x) / WJR_SIMD_VALUE_SIZE(ptr))
#define WJR_SIMD_LAST_ONE_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_LAST_ONE(x) / WJR_SIMD_VALUE_SIZE(ptr))

#define _WJR_SIMD_LOAD(simd_t, PREFIX, ptr, LOAD, J)	                                                                        \
	auto PREFIX = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr)));
#define _WJR_SIMD_LOAD2(simd_t, PREFIX, ptr0, ptr1, LOAD, J)	                                                                \
	auto PREFIX##0 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr0)));	                        \
	auto PREFIX##1 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr1)));
#define _WJR_SIMD_LOAD4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3 , LOAD, J)	                                                    \
	auto PREFIX##0 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr0)));	                        \
	auto PREFIX##1 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr1)));	                        \
	auto PREFIX##2 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr2)));	                        \
	auto PREFIX##3 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr3)));

#define WJR_SIMD_LOADU(simd_t, PREFIX, ptr) _WJR_SIMD_LOAD(simd_t, PREFIX, ptr, loadu, WJR_SIMD_LEFT_PTR)
#define WJR_SIMD_LOADU2(simd_t, PREFIX, ptr0, ptr1)             \
_WJR_SIMD_LOAD2(simd_t, PREFIX, ptr0, ptr1, loadu, WJR_SIMD_LEFT_PTR)
#define WJR_SIMD_LOADU4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3) \
_WJR_SIMD_LOAD4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3, loadu, WJR_SIMD_LEFT_PTR)

#define WJR_SIMD_LOAD(simd_t, PREFIX, ptr)	_WJR_SIMD_LOAD(simd_t, PREFIX, ptr, load, WJR_SIMD_LEFT_PTR)
#define WJR_SIMD_LOAD2(simd_t, PREFIX, ptr0, ptr1)              \
_WJR_SIMD_LOAD2(simd_t, PREFIX, ptr0, ptr1, load, WJR_SIMD_LEFT_PTR)
#define WJR_SIMD_LOAD4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3)  \
_WJR_SIMD_LOAD4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3, load, WJR_SIMD_LEFT_PTR)

_WJR_SIMD_END

#include <wjr/simd/simd_intrin-inl.h>

#endif // __WJR_SIMD_SIMD_INTRIN_H