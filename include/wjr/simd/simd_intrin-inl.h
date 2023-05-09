#ifndef __WJR_SIMD_SIMD_INTRIN_H
#error "This file should not be included directly. Include <wjr/simd/simd_intrin.h> instead."
#endif 

#include <wjr/math.h>

_WJR_BEGIN

#if WJR_SSE2

template<>
struct broadcast_fn<simd::__m128i_tag, uint8_t> {
	WJR_INTRINSIC_INLINE __m128i operator()(uint8_t v) const {
		return _mm_set1_epi8(v);
	}
};
template<>
struct broadcast_fn<simd::__m128i_tag, uint16_t> {
	WJR_INTRINSIC_INLINE __m128i operator()(uint16_t v) const {
		return _mm_set1_epi16(v);
	}
};
template<>
struct broadcast_fn<simd::__m128i_tag, uint32_t> {
	WJR_INTRINSIC_INLINE __m128i operator()(uint32_t v) const {
		return _mm_set1_epi32(v);
	}
};
template<>
struct broadcast_fn<simd::__m128i_tag, uint64_t> {
	WJR_INTRINSIC_INLINE __m128i operator()(uint64_t v) const {
		return _mm_set1_epi64x(v);
	}
};
template<>
struct broadcast_fn<simd::__m128i_tag, simd::__m128i_tag> {
	WJR_INTRINSIC_INLINE __m128i operator()(__m128i v) const {
		return v;
	}
};
#endif // WJR_SSE2

#if WJR_AVX
template<>
struct broadcast_fn<simd::__m256i_tag, uint8_t> {
	WJR_INTRINSIC_INLINE __m256i operator()(uint8_t v) const {
		return _mm256_set1_epi8(v);
	}
};
template<>
struct broadcast_fn<simd::__m256i_tag, uint16_t> {
	WJR_INTRINSIC_INLINE __m256i operator()(uint16_t v) const {
		return _mm256_set1_epi16(v);
	}
};
template<>
struct broadcast_fn<simd::__m256i_tag, uint32_t> {
	WJR_INTRINSIC_INLINE __m256i operator()(uint32_t v) const {
		return _mm256_set1_epi32(v);
	}
};
template<>
struct broadcast_fn<simd::__m256i_tag, uint64_t> {
	WJR_INTRINSIC_INLINE __m256i operator()(uint64_t v) const {
		return _mm256_set1_epi64x(v);
	}
};
template<>
struct broadcast_fn<simd::__m256i_tag, simd::__m256i_tag> {
	WJR_INTRINSIC_INLINE __m256i operator()(__m256i v) const {
		return v;
	}
};
template<>
struct broadcast_fn<simd::__m256i_tag, simd::__m128i_tag> {
	WJR_INTRINSIC_INLINE __m256i operator()(__m128i v) const {
#if WJR_AVX2
		return _mm256_broadcastsi128_si256(v);
#else
		return _mm256_insertf128_si256(_mm256_castsi128_si256(v), v, 1);
#endif // WJR_AVX2
	}
};

#endif // WJR_AVX

_WJR_END

_WJR_SIMD_BEGIN

#if WJR_SSE2

__m128i mm_loadu_si16(void const* p) {
	return simd_cast<__m128i_tag, uint16_t>(*reinterpret_cast<uint16_t const*>(p));
}

__m128i mm_loadu_si32(void const* p) {
	return simd_cast<__m128i_tag, uint32_t>(*reinterpret_cast<uint32_t const*>(p));
}

__m128i mm_loadu_si64(void const* p) {
	return simd_cast<__m128i_tag, uint64_t>(*reinterpret_cast<uint64_t const*>(p));
}

#endif // WJR_SSE2

/*------SSE------*/

constexpr size_t sse::width() { return 128; }

constexpr sse::mask_type sse::mask() { return 0xFFFF; }

#if WJR_SSE
sse::mask_type sse::movemask_ps(__m128 v) {
	return static_cast<sse::mask_type>(_mm_movemask_ps(v));
}

void sse::sfence() { return _mm_sfence(); }

template<int imm8>
__m128 sse::shuffle_ps(__m128 a, __m128 b) {
	static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
	return _mm_shuffle_ps(a, b, imm8);
}
#endif // WJR_SSE

#if WJR_SSE2

__m128i sse::add_epi8(__m128i a, __m128i b) { return _mm_add_epi8(a, b); }
__m128i sse::add_epi16(__m128i a, __m128i b) { return _mm_add_epi16(a, b); }
__m128i sse::add_epi32(__m128i a, __m128i b) { return _mm_add_epi32(a, b); }
__m128i sse::add_epi64(__m128i a, __m128i b) { return _mm_add_epi64(a, b); }

__m128i sse::add(__m128i a, __m128i b, int8_t) { return add_epi8(a, b); }
__m128i sse::add(__m128i a, __m128i b, int16_t) { return add_epi16(a, b); }
__m128i sse::add(__m128i a, __m128i b, int32_t) { return add_epi32(a, b); }
__m128i sse::add(__m128i a, __m128i b, int64_t) { return add_epi64(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint8_t) { return add_epi8(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint16_t) { return add_epi16(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint32_t) { return add_epi32(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint64_t) { return add_epi64(a, b); }

int8_t sse::add_epi8(__m128i a) { return static_cast<int8_t>(add_epu8(a)); }
int16_t sse::add_epi16(__m128i a) { return static_cast<int16_t>(add_epu16(a)); }
int32_t sse::add_epi32(__m128i a) { return static_cast<int32_t>(add_epu32(a)); }
int64_t sse::add_epi64(__m128i a) { return static_cast<int64_t>(add_epu64(a)); }

uint8_t sse::add_epu8(__m128i a) {
	auto b = shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a);
	a = add(a, b, uint8_t());
	b = zeros();
	a = sad_epu8(a, b);
	return simd_cast<uint8_t, __m128i_tag>(a);
}

uint16_t sse::add_epu16(__m128i a) {
	a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint16_t());
	a = add(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a), uint16_t());
	a = add(a, srli<2>(a), uint16_t());
	return simd_cast<uint16_t, __m128i_tag>(a);
}

uint32_t sse::add_epu32(__m128i a) {
	a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint32_t());
	a = add(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a), uint32_t());
	return simd_cast<uint32_t, __m128i_tag>(a);
}

uint64_t sse::add_epu64(__m128i a) {
	a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint64_t());
	return simd_cast<uint64_t, __m128i_tag>(a);
}

int8_t sse::add(__m128i a, int8_t) { return add_epi8(a); }
int16_t sse::add(__m128i a, int16_t) { return add_epi16(a); }
int32_t sse::add(__m128i a, int32_t) { return add_epi32(a); }
int64_t sse::add(__m128i a, int64_t) { return add_epi64(a); }
uint8_t sse::add(__m128i a, uint8_t) { return add_epu8(a); }
uint16_t sse::add(__m128i a, uint16_t) { return add_epu16(a); }
uint32_t sse::add(__m128i a, uint32_t) { return add_epu32(a); }
uint64_t sse::add(__m128i a, uint64_t) { return add_epu64(a); }

__m128i sse::adds_epi8(__m128i a, __m128i b) { return _mm_adds_epi8(a, b); }
__m128i sse::adds_epi16(__m128i a, __m128i b) { return _mm_adds_epi16(a, b); }

__m128i sse::adds_epu8(__m128i a, __m128i b) { return _mm_adds_epu8(a, b); }
__m128i sse::adds_epu16(__m128i a, __m128i b) { return _mm_adds_epu16(a, b); }

__m128i sse::adds(__m128i a, __m128i b, int8_t) { return adds_epi8(a, b); }
__m128i sse::adds(__m128i a, __m128i b, int16_t) { return adds_epi16(a, b); }
__m128i sse::adds(__m128i a, __m128i b, uint8_t) { return adds_epu8(a, b); }
__m128i sse::adds(__m128i a, __m128i b, uint16_t) { return adds_epu16(a, b); }

template<int imm8>
__m128i sse::alignr(__m128i a, __m128i b) {
	constexpr int s = imm8 & 0x1F;
#if WJR_SSSE3
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
#endif // WJR_SSSE3
}

__m128i sse::alignr_epi16(__m128i a, __m128i b, int c) {
	return Or(slli(a, 16 - c, uint16_t()), srli(b, c, uint16_t()));
}

__m128i sse::alignr_epi32(__m128i a, __m128i b, int c) {
	return Or(slli(a, 32 - c, uint32_t()), srli(b, c, uint32_t()));
}

__m128i sse::alignr_epi64(__m128i a, __m128i b, int c) {
	return Or(slli(a, 64 - c, uint64_t()), srli(b, c, uint64_t()));
}

__m128i sse::alignr(__m128i a, __m128i b, int c, int16_t) { return alignr_epi16(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, int32_t) { return alignr_epi32(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, int64_t) { return alignr_epi64(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, uint16_t) { return alignr_epi16(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, uint32_t) { return alignr_epi32(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, uint64_t) { return alignr_epi64(a, b, c); }

__m128i sse::And(__m128i a, __m128i b) { return _mm_and_si128(a, b); }

__m128i sse::AndNot(__m128i a, __m128i b) { return _mm_andnot_si128(a, b); }

__m128i sse::avg_epu8(__m128i a, __m128i b) { return _mm_avg_epu8(a, b); }
__m128i sse::avg_epu16(__m128i a, __m128i b) { return _mm_avg_epu16(a, b); }

__m128i sse::avg(__m128i a, __m128i b, int8_t) { return avg_epu8(a, b); }
__m128i sse::avg(__m128i a, __m128i b, int16_t) { return avg_epu16(a, b); }
__m128i sse::avg(__m128i a, __m128i b, uint8_t) { return avg_epu8(a, b); }
__m128i sse::avg(__m128i a, __m128i b, uint16_t) { return avg_epu16(a, b); }

// notice that mask must be 0 or 255(every byte)
__m128i sse::blendv_epi8(__m128i a, __m128i b, __m128i mask) {
#if WJR_SSE4_1
	return _mm_blendv_epi8(a, b, mask);
#elif defined(WJR_COMPILER_GCC)
	return ((~mask) & a) | (mask & b);
#else
	return Or(AndNot(mask, a), And(mask, b));
#endif // WJR_SSE4_1
}

__m128i sse::blendv_epi16(__m128i a, __m128i b, __m128i mask) {
	return blendv_epi8(b, a, logical_not(mask, uint16_t()));
}

__m128i sse::blendv_epi32(__m128i a, __m128i b, __m128i mask) {
	return blendv_epi8(b, a, logical_not(mask, uint32_t()));
}

__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int8_t) { return blendv_epi8(a, b, mask); }
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int16_t) { return blendv_epi16(a, b, mask); }
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int32_t) { return blendv_epi32(a, b, mask); }
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint8_t) { return blendv_epi8(a, b, mask); }
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint16_t) { return blendv_epi16(a, b, mask); }
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint32_t) { return blendv_epi32(a, b, mask); }

template<int imm8>
__m128i sse::bslli(__m128i val) { return _mm_bslli_si128(val, imm8); }

template<int imm8>
__m128i sse::bsrli(__m128i val) { return _mm_bsrli_si128(val, imm8); }

__m128i sse::cmpeq_epi8(__m128i a, __m128i b) { return _mm_cmpeq_epi8(a, b); }
__m128i sse::cmpeq_epi16(__m128i a, __m128i b) { return _mm_cmpeq_epi16(a, b); }
__m128i sse::cmpeq_epi32(__m128i a, __m128i b) { return _mm_cmpeq_epi32(a, b); }

__m128i sse::cmpeq(__m128i a, __m128i b, int8_t) { return cmpeq_epi8(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, int16_t) { return cmpeq_epi16(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, int32_t) { return cmpeq_epi32(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint8_t) { return cmpeq_epi8(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint16_t) { return cmpeq_epi16(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint32_t) { return cmpeq_epi32(a, b); }

__m128i sse::cmpge_epi8(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comge_epi8(a, b);
#elif WJR_SSE4_1
	return cmpeq(min(a, b, int8_t()), b, uint8_t());
#else
	return Not(cmpgt(b, a, int8_t()));
#endif // WJR_XOP
}

__m128i sse::cmpge_epi16(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comge_epi16(a, b);
#else
	return cmpeq(min(a, b, int16_t()), b, uint16_t());
#endif // WJR_XOP
}

__m128i sse::cmpge_epi32(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comge_epi32(a, b);
#elif WJR_SSE4_1
	return cmpeq(min(a, b, int32_t()), b, uint32_t());
#else
	return Not(cmpgt(b, a, int32_t()));
#endif // WJR_XOP
}

__m128i sse::cmpge_epu8(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comge_epu8(a, b);
#else
	return cmpeq(min(a, b, uint8_t()), b, uint8_t());
#endif // WJR_XOP
}

__m128i sse::cmpge_epu16(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comge_epu16(a, b);
#elif WJR_SSE4_1
	return cmpeq(min(a, b, uint16_t()), b, uint16_t());
#else
	return logical_not(subs(b, a, uint16_t()), uint16_t());
#endif // WJR_XOP
}

__m128i sse::cmpge_epu32(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comge_epu32(a, b);
#elif WJR_SSE4_1
	return cmpeq(min(a, b, uint32_t()), b, uint32_t());
#else
	return Not(cmpgt(b, a, uint32_t()));
#endif // WJR_XOP
}

__m128i sse::cmpge(__m128i a, __m128i b, int8_t) { return cmpge_epi8(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, int16_t) { return cmpge_epi16(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, int32_t) { return cmpge_epi32(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint8_t) { return cmpge_epu8(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint16_t) { return cmpge_epu16(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint32_t) { return cmpge_epu32(a, b); }

__m128i sse::cmpgt_epi8(__m128i a, __m128i b) { return _mm_cmpgt_epi8(a, b); }
__m128i sse::cmpgt_epi16(__m128i a, __m128i b) { return _mm_cmpgt_epi16(a, b); }
__m128i sse::cmpgt_epi32(__m128i a, __m128i b) { return _mm_cmpgt_epi32(a, b); }

__m128i sse::cmpgt_epu8(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comgt_epu8(a, b);
#else
	return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
#endif // WJR_XOP
}

__m128i sse::cmpgt_epu16(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comgt_epu16(a, b);
#else
	return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
#endif // WJR_XOP
}

__m128i sse::cmpgt_epu32(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comgt_epu32(a, b);
#else
	return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
#endif // WJR_XOP
}

__m128i sse::cmpgt(__m128i a, __m128i b, int8_t) { return cmpgt_epi8(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, int16_t) { return cmpgt_epi16(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, int32_t) { return cmpgt_epi32(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint8_t) { return cmpgt_epu8(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint16_t) { return cmpgt_epu16(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint32_t) { return cmpgt_epu32(a, b); }

__m128i sse::cmple_epi8(__m128i a, __m128i b) { return cmpge_epi8(b, a); }
__m128i sse::cmple_epi16(__m128i a, __m128i b) { return cmpge_epi16(b, a); }
__m128i sse::cmple_epi32(__m128i a, __m128i b) { return cmpge_epi32(b, a); }

__m128i sse::cmple_epu8(__m128i a, __m128i b) { return cmpge_epu8(b, a); }
__m128i sse::cmple_epu16(__m128i a, __m128i b) { return cmpge_epu16(b, a); }
__m128i sse::cmple_epu32(__m128i a, __m128i b) { return cmpge_epu32(b, a); }

__m128i sse::cmple(__m128i a, __m128i b, int8_t) { return cmple_epi8(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, int16_t) { return cmple_epi16(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, int32_t) { return cmple_epi32(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint8_t) { return cmple_epu8(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint16_t) { return cmple_epu16(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint32_t) { return cmple_epu32(a, b); }

__m128i sse::cmplt_epi8(__m128i a, __m128i b) { return _mm_cmplt_epi8(a, b); }
__m128i sse::cmplt_epi16(__m128i a, __m128i b) { return _mm_cmplt_epi16(a, b); }
__m128i sse::cmplt_epi32(__m128i a, __m128i b) { return _mm_cmplt_epi32(a, b); }

__m128i sse::cmplt_epu8(__m128i a, __m128i b) { return cmpgt_epu8(b, a); }
__m128i sse::cmplt_epu16(__m128i a, __m128i b) { return cmpgt_epu16(b, a); }
__m128i sse::cmplt_epu32(__m128i a, __m128i b) { return cmpgt_epu32(b, a); }

__m128i sse::cmplt(__m128i a, __m128i b, int8_t) { return cmplt_epi8(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, int16_t) { return cmplt_epi16(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, int32_t) { return cmplt_epi32(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint8_t) { return cmplt_epu8(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint16_t) { return cmplt_epu16(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint32_t) { return cmplt_epu32(a, b); }

__m128i sse::cmpne_epi8(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comneq_epi8(a, b);
#else
	return Not(cmpeq_epi8(a, b));
#endif
}

__m128i sse::cmpne_epi16(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comneq_epi16(a, b);
#else
	return Not(cmpeq_epi16(a, b));
#endif
}

__m128i sse::cmpne_epi32(__m128i a, __m128i b) {
#if WJR_XOP
	return _mm_comneq_epi32(a, b);
#else
	return Not(cmpeq_epi32(a, b));
#endif
}

__m128i sse::cmpne(__m128i a, __m128i b, int8_t) { return cmpne_epi8(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, int16_t) { return cmpne_epi16(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, int32_t) { return cmpne_epi32(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint8_t) { return cmpne_epi8(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint16_t) { return cmpne_epi16(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint32_t) { return cmpne_epi32(a, b); }

template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::equal_to<>, T) { return cmpeq(a, b, T()); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::not_equal_to<>, T) { return cmpne(a, b, T()); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::greater<>, T) { return cmpgt(a, b, T()); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::greater_equal<>, T) { return cmpge(a, b, T()); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::less<>, T) { return cmplt(a, b, T()); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::less_equal<>, T) { return cmple(a, b, T()); }

__m128i sse::concat(uint64_t lo, uint64_t hi) { return set_epi64x(hi, lo); }

template<int imm8>
int sse::extract_epi8(__m128i a) {
	static_assert(imm8 >= 0 && imm8 < 16, "imm8 must be in range [0, 15]");
#if WJR_SSE4_1
	return _mm_extract_epi8(a, imm8);
#else
	if constexpr (imm8 & 1) {
		return extract_epi16<(imm8 >> 1)>(a) >> 8;
	}
	else {
		return extract_epi16 <(imm8 >> 1)>(a) & 0xff;
	}
#endif // WJR_SSE4_1
}

template<int imm8>
int sse::extract_epi16(__m128i a) {
	static_assert(imm8 >= 0 && imm8 < 8, "imm8 must be in range [0, 7]");
	return _mm_extract_epi16(a, imm8);
}

template<int imm8>
int sse::extract_epi32(__m128i a) {
	static_assert(imm8 >= 0 && imm8 < 4, "imm8 must be in range [0, 3]");
#if WJR_SSE4_1
	return _mm_extract_epi32(a, imm8);
#else
	if constexpr (imm8 == 0) {
		return simd_cast<uint32_t, __m128i_tag>(a);
	}
	else if constexpr (imm8 == 1) {
		return static_cast<uint32_t>(simd_cast<uint64_t, __m128i_tag>(a) >> 32);
	}
	else if constexpr (imm8 == 2) {
		return simd_cast<uint32_t, __m128i_tag>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	}
	else {
		return simd_cast<uint32_t, __m128i_tag>(shuffle_epi32<_MM_SHUFFLE(3, 3, 3, 3)>(a));
	}
#endif // WJR_SSE4_1
}

template<int imm8>
int64_t sse::extract_epi64(__m128i a) {
	static_assert(imm8 >= 0 && imm8 < 2, "imm8 must be in range [0, 1]");
#if WJR_SSE4_1
	return _mm_extract_epi64(a, imm8);
#else
	if constexpr (imm8 == 0) {
		return simd_cast<uint64_t, __m128i_tag>(a);
	}
	else {
		return simd_cast<uint64_t, __m128i_tag>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	}
#endif // WJR_SSE4_1
}

template<int imm8>
int sse::extract(__m128i a, int8_t) { return extract_epi8<imm8>(a); }
template<int imm8>
int sse::extract(__m128i a, int16_t) { return extract_epi16<imm8>(a); }
template<int imm8>
int sse::extract(__m128i a, int32_t) { return extract_epi32<imm8>(a); }
template<int imm8>
int64_t sse::extract(__m128i a, int64_t) { return extract_epi64<imm8>(a); }
template<int imm8>
int sse::extract(__m128i a, uint8_t) { return extract_epi8<imm8>(a); }
template<int imm8>
int sse::extract(__m128i a, uint16_t) { return extract_epi16<imm8>(a); }
template<int imm8>
int sse::extract(__m128i a, uint32_t) { return extract_epi32<imm8>(a); }
template<int imm8>
int64_t sse::extract(__m128i a, uint64_t) { return extract_epi64<imm8>(a); }

uint64_t sse::getlow(__m128i v) { return simd_cast<uint64_t, __m128i_tag>(v); }
uint64_t sse::gethigh(__m128i v) { return extract_epi64<1>(v); }

template<int imm8>
__m128i sse::insert_epi16(__m128i a, int i) {
	return _mm_insert_epi16(a, i, imm8);
}

template<int imm8>
__m128i sse::insert(__m128i a, int i, int16_t) { return insert_epi16<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int i, uint16_t) { return insert_epi16<imm8>(a, i); }

void sse::lfence() { _mm_lfence(); }

__m128i sse::load(const __m128i* ptr) { return _mm_load_si128(ptr); }
__m128i sse::loadu(const __m128i* ptr) { return _mm_loadu_si128(ptr); }
__m128i sse::loadu_si16(const void* ptr) { return simd::mm_loadu_si16(ptr); }
__m128i sse::loadu_si32(const void* ptr) { return simd::mm_loadu_si32(ptr); }
__m128i sse::loadu_si64(const void* ptr) { return simd::mm_loadu_si64(ptr); }

template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
	uint8_t, uint16_t, uint32_t, uint64_t>, int>>
	__m128i sse::logical_and(__m128i a, __m128i b, T) {
	return Not(Or(logical_not(a, T()), logical_not(b, T())));
}

template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
	uint8_t, uint16_t, uint32_t, uint64_t>, int>>
	__m128i sse::logical_not(__m128i v, T) {
	auto Zero = zeros();
	return cmpeq(v, Zero, T());
}

template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
	uint8_t, uint16_t, uint32_t, uint64_t>, int>>
	__m128i sse::logical_or(__m128i a, __m128i b, T) {
	return Not(logical_not(Or(a, b), T()));
}

__m128i sse::madd_epi16(__m128i a, __m128i b) {
	return _mm_madd_epi16(a, b);
}

void sse::maskmoveu(__m128i a, __m128i mask, char* mem_addr) {
	return _mm_maskmoveu_si128(a, mask, mem_addr);
}

__m128i sse::max_epi8(__m128i a, __m128i b) {
#if WJR_SSE4_1
	return _mm_max_epi8(a, b);
#else
	return blendv_epi8(b, a, cmpgt_epi8(a, b));
#endif // WJR_SSE4_1
}

__m128i sse::max_epi16(__m128i a, __m128i b) { return _mm_max_epi16(a, b); }

__m128i sse::max_epi32(__m128i a, __m128i b) {
#if WJR_SSE4_1
	return _mm_max_epi32(a, b);
#else
	return blendv_epi8(b, a, cmpgt_epi32(a, b));
#endif // WJR_SSE4_1
}

__m128i sse::max_epu8(__m128i a, __m128i b) { return _mm_max_epu8(a, b); }

__m128i sse::max_epu16(__m128i a, __m128i b) {
#if WJR_SSE4_1
	return _mm_max_epu16(a, b);
#else
	return add(subs_epu16(b, a), a, uint16_t());
#endif // WJR_SSE4_1
}

__m128i sse::max_epu32(__m128i a, __m128i b) {
#if WJR_SSE4_1
	return _mm_max_epu32(a, b);
#else
	return blendv_epi8(b, a, cmpgt_epu32(a, b));
#endif // WJR_SSE4_1
}

__m128i sse::max(__m128i a, __m128i b, int8_t) { return max_epi8(a, b); }
__m128i sse::max(__m128i a, __m128i b, int16_t) { return max_epi16(a, b); }
__m128i sse::max(__m128i a, __m128i b, int32_t) { return max_epi32(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint8_t) { return max_epu8(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint16_t) { return max_epu16(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint32_t) { return max_epu32(a, b); }

int8_t sse::max_epi8(__m128i a) {
	return 0x7fu ^ min_epu8(Xor(a, set1_epi8(0x7fu)));
}

int16_t sse::max_epi16(__m128i a) {
#if WJR_SSE4_1
	return 0x7fffu ^ min_epu16(Xor(a, set1_epi16(0x7fffu)));
#else
	a = max_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
	a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
	return simd_cast<int16_t, __m128i_tag>(a);
#endif // WJR_SSE4_1
}

int32_t sse::max_epi32(__m128i a) {
	a = max_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	a = max_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
	return simd_cast<int32_t, __m128i_tag>(a);
}

uint8_t sse::max_epu8(__m128i a) {
#if WJR_SSE4_1
	return 0xffu ^ min_epu8(Xor(a, ones()));
#else
	a = max_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
	a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
	auto X = simd_cast<uint32_t, __m128i_tag>(a);
	return std::max((uint8_t)X, (uint8_t)(X >> 8));
#endif // WJR_SSE4_1
}

uint16_t sse::max_epu16(__m128i a) {
#if WJR_SSE4_1
	return 0xffffu ^ min_epu16(Xor(a, ones()));
#else
	a = max_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
	a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
	return simd_cast<uint16_t, __m128i_tag>(a);
#endif // WJR_SSE4_1
}

uint32_t sse::max_epu32(__m128i a) {
	a = max_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	a = max_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
	return simd_cast<uint32_t, __m128i_tag>(a);
}

int8_t sse::max(__m128i a, int8_t) { return max_epi8(a); }
int16_t sse::max(__m128i a, int16_t) { return max_epi16(a); }
int32_t sse::max(__m128i a, int32_t) { return max_epi32(a); }
uint8_t sse::max(__m128i a, uint8_t) { return max_epu8(a); }
uint16_t sse::max(__m128i a, uint16_t) { return max_epu16(a); }
uint32_t sse::max(__m128i a, uint32_t) { return max_epu32(a); }

void sse::mfence() { _mm_mfence(); }

__m128i sse::min_epi8(__m128i a, __m128i b) {
#if WJR_SSE4_1
	return _mm_min_epi8(a, b);
#else
	return blendv_epi8(a, b, cmpgt_epi8(a, b));
#endif // WJR_SSE4_1
}

__m128i sse::min_epi16(__m128i a, __m128i b) { return _mm_min_epi16(a, b); }

__m128i sse::min_epi32(__m128i a, __m128i b) {
#if WJR_SSE4_1
	return _mm_min_epi32(a, b);
#else
	return blendv_epi8(a, b, cmpgt_epi32(a, b));
#endif // WJR_SSE4_1
}

__m128i sse::min_epu8(__m128i a, __m128i b) { return _mm_min_epu8(a, b); }

__m128i sse::min_epu16(__m128i a, __m128i b) {
#if WJR_SSE4_1
	return _mm_min_epu16(a, b);
#else
	return blendv_epi8(a, b, cmpgt_epu16(a, b));
#endif // WJR_SSE4_1
}

__m128i sse::min_epu32(__m128i a, __m128i b) {
#if WJR_SSE4_1
	return _mm_min_epu32(a, b);
#else
	return blendv_epi8(a, b, cmpgt_epu32(a, b));
#endif // WJR_SSE4_1
}

__m128i sse::min(__m128i a, __m128i b, int8_t) { return min_epi8(a, b); }
__m128i sse::min(__m128i a, __m128i b, int16_t) { return min_epi16(a, b); }
__m128i sse::min(__m128i a, __m128i b, int32_t) { return min_epi32(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint8_t) { return min_epu8(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint16_t) { return min_epu16(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint32_t) { return min_epu32(a, b); }

int8_t sse::min_epi8(__m128i a) {
	return 0x80u ^ min_epu8(Xor(a, setmin_epi8()));
}

int16_t sse::min_epi16(__m128i a) {
#if WJR_SSE4_1
	return 0x8000u ^ min_epu16(Xor(a, setmin_epi16()));
#else
	a = min_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
	a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
	return simd_cast<int16_t, __m128i_tag>(a);
#endif // WJR_SSE4_1
}

int32_t sse::min_epi32(__m128i a) {
	a = min_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	a = min_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
	return simd_cast<int32_t, __m128i_tag>(a);
}

uint8_t sse::min_epu8(__m128i a) {
#if WJR_SSE4_1
	a = min_epu8(a, srli_epi16(a, 8));
	a = _mm_minpos_epu16(a);
	return simd_cast<uint8_t, __m128i_tag>(a);
#else
	a = min_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
	a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
	auto X = simd_cast<uint32_t, __m128i_tag>(a);
	return std::min((uint8_t)X, (uint8_t)(X >> 8));
#endif // WJR_SSE4_1
}

uint16_t sse::min_epu16(__m128i a) {
#if WJR_SSE4_1
	return simd_cast<uint16_t, __m128i_tag>(_mm_minpos_epu16(a));
#else
	a = min_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
	a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
	return simd_cast<uint16_t, __m128i_tag>(a);
#endif // WJR_SSE4_1
}

uint32_t sse::min_epu32(__m128i a) {
	a = min_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
	a = min_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
	return simd_cast<uint32_t, __m128i_tag>(a);
}

int8_t sse::min(__m128i a, int8_t) { return min_epi8(a); }
int16_t sse::min(__m128i a, int16_t) { return min_epi16(a); }
int32_t sse::min(__m128i a, int32_t) { return min_epi32(a); }
uint8_t sse::min(__m128i a, uint8_t) { return min_epu8(a); }
uint16_t sse::min(__m128i a, uint16_t) { return min_epu16(a); }
uint32_t sse::min(__m128i a, uint32_t) { return min_epu32(a); }

__m128i sse::move_epi64(__m128i a) { return _mm_move_epi64(a); }

sse::mask_type sse::movemask_epi8(__m128i a) { return static_cast<mask_type>(_mm_movemask_epi8(a)); }
sse::mask_type sse::movemask_pd(__m128d v) { return static_cast<mask_type>(_mm_movemask_pd(v)); }

sse::mask_type sse::movemask(__m128i v, int8_t) { return movemask_epi8(v); }
sse::mask_type sse::movemask(__m128i v, int32_t) { return movemask_ps(simd_cast<__m128_tag, __m128i_tag>(v)); }
sse::mask_type sse::movemask(__m128i v, int64_t) { return movemask_pd(simd_cast<__m128d_tag, __m128i_tag>(v)); }
sse::mask_type sse::movemask(__m128i v, uint8_t) { return movemask(v, int8_t()); }
sse::mask_type sse::movemask(__m128i v, uint32_t) { return movemask(v, int32_t()); }
sse::mask_type sse::movemask(__m128i v, uint64_t) { return movemask(v, int64_t()); }

__m128i sse::mul_epu32(__m128i a, __m128i b) { return _mm_mul_epu32(a, b); }

__m128i sse::mulhi_epi16(__m128i a, __m128i b) { return _mm_mulhi_epi16(a, b); }

__m128i sse::mulhi_epu16(__m128i a, __m128i b) { return _mm_mulhi_epu16(a, b); }

__m128i sse::mullo_epi16(__m128i a, __m128i b) { return _mm_mullo_epi16(a, b); }

__m128i sse::negate_epi8(__m128i a) {
#if WJR_SSSE3
	return sign_epi8(a, ones());
#else
	return sub_epi8(zeros(), a);
#endif // WJR_SSSE3
}

__m128i sse::negate_epi16(__m128i a) {
#if WJR_SSSE3
	return sign_epi16(a, ones());
#else
	return sub_epi16(zeros(), a);
#endif // WJR_SSSE3
}

__m128i sse::negate_epi32(__m128i a) {
#if WJR_SSSE3
	return sign_epi32(a, ones());
#else
	return sub_epi32(zeros(), a);
#endif // WJR_SSSE3
}

__m128i sse::negate_epi64(__m128i a) {
	return sub_epi64(zeros(), a);
}

__m128i sse::negate(__m128i a, int8_t) { return negate_epi8(a); }
__m128i sse::negate(__m128i a, int16_t) { return negate_epi16(a); }
__m128i sse::negate(__m128i a, int32_t) { return negate_epi32(a); }
__m128i sse::negate(__m128i a, int64_t) { return negate_epi64(a); }
__m128i sse::negate(__m128i a, uint8_t) { return negate_epi8(a); }
__m128i sse::negate(__m128i a, uint16_t) { return negate_epi16(a); }
__m128i sse::negate(__m128i a, uint32_t) { return negate_epi32(a); }
__m128i sse::negate(__m128i a, uint64_t) { return negate_epi64(a); }

__m128i sse::Not(__m128i v) { return Xor(v, ones()); }

__m128i sse::Or(__m128i a, __m128i b) { return _mm_or_si128(a, b); }

__m128i sse::packs_epi16(__m128i a, __m128i b) { return _mm_packs_epi16(a, b); }
__m128i sse::packs_epi32(__m128i a, __m128i b) { return _mm_packs_epi32(a, b); }

__m128i sse::packus_epi16(__m128i a, __m128i b) { return _mm_packus_epi16(a, b); }

__m128i sse::preloadu_si16(const void* ptr) { return loadu_si16(ptr); }
__m128i sse::preloadu_si32(const void* ptr) { return loadu_si32(ptr); }

__m128i sse::preloadu_si48(const void* ptr) {
	return insert_epi16<2>(preloadu_si32(ptr), reinterpret_cast<const uint16_t*>(ptr)[2]);
}

__m128i sse::preloadu_si64(const void* ptr) { return loadu_si64(ptr); }

__m128i sse::preloadu_si80(const void* ptr) {
	return insert_epi16<4>(preloadu_si64(ptr), reinterpret_cast<const uint16_t*>(ptr)[4]);
}

__m128i sse::preloadu_si96(const void* ptr) {
#if WJR_SSE4_1
	return insert_epi32<2>(preloadu_si64(ptr), reinterpret_cast<const uint32_t*>(ptr)[2]);
#else
	return insert_epi16<5>(preloadu_si80(ptr), reinterpret_cast<const uint16_t*>(ptr)[5]);
#endif // WJR_SSE4_1
}

__m128i sse::preloadu_si112(const void* ptr) {
	return insert_epi16<6>(preloadu_si96(ptr), reinterpret_cast<const uint16_t*>(ptr)[6]);
}

__m128i sse::preloadu_si128(const void* ptr) { return loadu((__m128i*)ptr); }

__m128i sse::preloadu_si16x(const void* ptr, int n) {
	// preloadu_si(n * 16)
	switch (n) {
	case 0:return zeros();
	case 1:return preloadu_si16(ptr);
	case 2:return preloadu_si32(ptr);
	case 3:return preloadu_si48(ptr);
	case 4:return preloadu_si64(ptr);
	case 5:return preloadu_si80(ptr);
	case 6:return preloadu_si96(ptr);
	case 7:return preloadu_si112(ptr);
	default:return preloadu_si128(ptr);
	}
}

__m128i sse::sad_epu8(__m128i a, __m128i b) { return _mm_sad_epu8(a, b); }

__m128i sse::zeros() { return _mm_setzero_si128(); }
__m128i sse::ones() { return _mm_set1_epi32(-1); }

__m128i sse::set_epi8(
	char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
	char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
	return _mm_set_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}

__m128i sse::set_epi16(
	short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
	return _mm_set_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
}
__m128i sse::set_epi32(int e3, int e2, int e1, int e0) {
	return _mm_set_epi32(e3, e2, e1, e0);
}
__m128i sse::set_epi64x(long long e1, long long e0) {
	return _mm_set_epi64x(e1, e0);
}

__m128i sse::setr_epi8(
	char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
	char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
	return _mm_setr_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}

__m128i sse::setr_epi16(
	short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
	return _mm_setr_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
}
__m128i sse::setr_epi32(int e3, int e2, int e1, int e0) {
	return _mm_setr_epi32(e3, e2, e1, e0);
}

__m128i sse::set1_epi8(int8_t val) { return _mm_set1_epi8(val); }
__m128i sse::set1_epi16(int16_t val) { return _mm_set1_epi16(val); }
__m128i sse::set1_epi32(int32_t val) { return _mm_set1_epi32(val); }
__m128i sse::set1_epi64(int64_t val) { return _mm_set1_epi64x(val); }

__m128i sse::set1(int8_t val, int8_t) { return set1_epi8(val); }
__m128i sse::set1(int16_t val, int16_t) { return set1_epi16(val); }
__m128i sse::set1(int32_t val, int32_t) { return set1_epi32(val); }
__m128i sse::set1(int64_t val, int64_t) { return set1_epi64(val); }
__m128i sse::set1(uint8_t val, uint8_t) { return set1_epi8(val); }
__m128i sse::set1(uint16_t val, uint16_t) { return set1_epi16(val); }
__m128i sse::set1(uint32_t val, uint32_t) { return set1_epi32(val); }
__m128i sse::set1(uint64_t val, uint64_t) { return set1_epi64(val); }

__m128i sse::setmin_epi8() { return set1_epi8(0x80u); }
__m128i sse::setmin_epi16() { return set1_epi16(0x8000u); }
__m128i sse::setmin_epi32() { return set1_epi32(0x80000000u); }

__m128i sse::setmin(int8_t) { return setmin_epi8(); }
__m128i sse::setmin(int16_t) { return setmin_epi16(); }
__m128i sse::setmin(int32_t) { return setmin_epi32(); }
__m128i sse::setmin(uint8_t) { return set1_epi32(0); }
__m128i sse::setmin(uint16_t) { return set1_epi32(0); }
__m128i sse::setmin(uint32_t) { return set1_epi32(0); }

__m128i sse::setmax_epi8() { return set1_epi8(0x7F); }
__m128i sse::setmax_epi16() { return set1_epi16(0x7FFF); }
__m128i sse::setmax_epi32() { return set1_epi32(0x7FFFFFFF); }

__m128i sse::setmax(int8_t) { return setmax_epi8(); }
__m128i sse::setmax(int16_t) { return setmax_epi16(); }
__m128i sse::setmax(int32_t) { return setmax_epi32(); }
__m128i sse::setmax(uint8_t) { return set1_epi32(0xFFFFFFFF); }
__m128i sse::setmax(uint16_t) { return set1_epi32(0xFFFFFFFF); }
__m128i sse::setmax(uint32_t) { return set1_epi32(0xFFFFFFFF); }

template<int imm>
__m128i sse::shl(__m128i a) {
	if constexpr (imm >= 64) {
		a = slli<8>(a);
		a = slli_epi64(a, imm - 64);
		return a;
	}
	else {
		auto b = slli_epi64(a, imm);
		auto c = slli<8>(a);
		c = srli_epi64(c, 64 - imm);
		return Or(b, c);
	}
}

template<int imm>
__m128i sse::shr(__m128i a) {
	if constexpr (imm >= 64) {
		a = srli<8>(a);
		a = srli_epi64(a, imm - 64);
		return a;
	}
	else {
		auto b = srli_epi64(a, imm);
		auto c = srli<8>(a);
		c = slli_epi64(c, 64 - imm);
		return Or(b, c);
	}
}

template<int imm8>
__m128i sse::shuffle_epi32(__m128i v) {
	static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
	return _mm_shuffle_epi32(v, imm8);
}

template<int imm8>
__m128i sse::shufflehi_epi16(__m128i v) {
	return _mm_shufflehi_epi16(v, imm8);
}

template<int imm8>
__m128i sse::shufflelo_epi16(__m128i v) {
	return _mm_shufflelo_epi16(v, imm8);
}

__m128i sse::sll_epi16(__m128i a, __m128i b) {
	return _mm_sll_epi16(a, b);
}
__m128i sse::sll_epi32(__m128i a, __m128i b) {
	return _mm_sll_epi32(a, b);
}
__m128i sse::sll_epi64(__m128i a, __m128i b) {
	return _mm_sll_epi64(a, b);
}

__m128i sse::sll(__m128i a, __m128i b, int16_t) { return sll_epi16(a, b); }
__m128i sse::sll(__m128i a, __m128i b, int32_t) { return sll_epi32(a, b); }
__m128i sse::sll(__m128i a, __m128i b, int64_t) { return sll_epi64(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint16_t) { return sll_epi16(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint32_t) { return sll_epi32(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint64_t) { return sll_epi64(a, b); }

template<int imm8>
__m128i sse::slli(__m128i v) {
	return _mm_slli_si128(v, imm8);
}
__m128i sse::slli_epi16(__m128i a, int imm8) {
	return _mm_slli_epi16(a, imm8);
}
__m128i sse::slli_epi32(__m128i a, int imm8) {
	return _mm_slli_epi32(a, imm8);
}
__m128i sse::slli_epi64(__m128i a, int imm8) {
	return _mm_slli_epi64(a, imm8);
}

__m128i sse::slli(__m128i a, int imm8, int16_t) { return slli_epi16(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, int32_t) { return slli_epi32(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, int64_t) { return slli_epi64(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint16_t) { return slli_epi16(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint32_t) { return slli_epi32(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint64_t) { return slli_epi64(a, imm8); }

__m128i sse::sra_epi16(__m128i a, __m128i b) { return _mm_sra_epi16(a, b); }
__m128i sse::sra_epi32(__m128i a, __m128i b) { return _mm_sra_epi32(a, b); }

__m128i sse::sra(__m128i a, __m128i b, int16_t) { return sra_epi16(a, b); }
__m128i sse::sra(__m128i a, __m128i b, int32_t) { return sra_epi32(a, b); }

__m128i sse::srai_epi16(__m128i a, int imm8) { return _mm_srai_epi16(a, imm8); }
__m128i sse::srai_epi32(__m128i a, int imm8) { return _mm_srai_epi32(a, imm8); }

__m128i sse::srai(__m128i a, int imm8, int16_t) { return srai_epi16(a, imm8); }
__m128i sse::srai(__m128i a, int imm8, int32_t) { return srai_epi32(a, imm8); }

__m128i sse::srl_epi16(__m128i a, __m128i b) { return _mm_srl_epi16(a, b); }
__m128i sse::srl_epi32(__m128i a, __m128i b) { return _mm_srl_epi32(a, b); }
__m128i sse::srl_epi64(__m128i a, __m128i b) { return _mm_srl_epi64(a, b); }

__m128i sse::srl(__m128i a, __m128i b, int16_t) { return srl_epi16(a, b); }
__m128i sse::srl(__m128i a, __m128i b, int32_t) { return srl_epi32(a, b); }
__m128i sse::srl(__m128i a, __m128i b, int64_t) { return srl_epi64(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint16_t) { return srl_epi16(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint32_t) { return srl_epi32(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint64_t) { return srl_epi64(a, b); }

template<int imm8>
__m128i sse::srli(__m128i v) {
	return _mm_srli_si128(v, imm8);
}
__m128i sse::srli_epi16(__m128i a, int imm8) {
	return _mm_srli_epi16(a, imm8);
}
__m128i sse::srli_epi32(__m128i a, int imm8) {
	return _mm_srli_epi32(a, imm8);
}
__m128i sse::srli_epi64(__m128i a, int imm8) {
	return _mm_srli_epi64(a, imm8);
}

__m128i sse::srli(__m128i a, int imm8, int16_t) { return srli_epi16(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, int32_t) { return srli_epi32(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, int64_t) { return srli_epi64(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint16_t) { return srli_epi16(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint32_t) { return srli_epi32(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint64_t) { return srli_epi64(a, imm8); }

void sse::stream(__m128i* ptr, __m128i v) { _mm_stream_si128(ptr, v); }

void sse::store(__m128i* ptr, __m128i val) { _mm_store_si128(ptr, val); }
void sse::storeu(__m128i* ptr, __m128i val) { _mm_storeu_si128(ptr, val); }

__m128i sse::sub_epi8(__m128i a, __m128i b) { return _mm_sub_epi8(a, b); }
__m128i sse::sub_epi16(__m128i a, __m128i b) { return _mm_sub_epi16(a, b); }
__m128i sse::sub_epi32(__m128i a, __m128i b) { return _mm_sub_epi32(a, b); }
__m128i sse::sub_epi64(__m128i a, __m128i b) { return _mm_sub_epi64(a, b); }

__m128i sse::sub(__m128i a, __m128i b, int8_t) { return sub_epi8(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int16_t) { return sub_epi16(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int32_t) { return sub_epi32(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int64_t) { return sub_epi64(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint8_t) { return sub_epi8(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint16_t) { return sub_epi16(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint32_t) { return sub_epi32(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint64_t) { return sub_epi64(a, b); }

__m128i sse::subs_epi8(__m128i a, __m128i b) { return _mm_subs_epi8(a, b); }
__m128i sse::subs_epi16(__m128i a, __m128i b) { return _mm_subs_epi16(a, b); }

__m128i sse::subs_epu8(__m128i a, __m128i b) { return _mm_subs_epu8(a, b); }
__m128i sse::subs_epu16(__m128i a, __m128i b) { return _mm_subs_epu16(a, b); }

__m128i sse::subs(__m128i a, __m128i b, int8_t) { return subs_epi8(a, b); }
__m128i sse::subs(__m128i a, __m128i b, int16_t) { return subs_epi16(a, b); }
__m128i sse::subs(__m128i a, __m128i b, uint8_t) { return subs_epu8(a, b); }
__m128i sse::subs(__m128i a, __m128i b, uint16_t) { return subs_epu16(a, b); }

__m128i sse::unpackhi_epi8(__m128i a, __m128i b) { return _mm_unpackhi_epi8(a, b); }
__m128i sse::unpackhi_epi16(__m128i a, __m128i b) { return _mm_unpackhi_epi16(a, b); }
__m128i sse::unpackhi_epi32(__m128i a, __m128i b) { return _mm_unpackhi_epi32(a, b); }
__m128i sse::unpackhi_epi64(__m128i a, __m128i b) { return _mm_unpackhi_epi64(a, b); }

__m128i sse::unpackhi(__m128i a, __m128i b, int8_t) { return unpackhi_epi8(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int16_t) { return unpackhi_epi16(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int32_t) { return unpackhi_epi32(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int64_t) { return unpackhi_epi64(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint8_t) { return unpackhi_epi8(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint16_t) { return unpackhi_epi16(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint32_t) { return unpackhi_epi32(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint64_t) { return unpackhi_epi64(a, b); }

__m128i sse::unpacklo_epi8(__m128i a, __m128i b) { return _mm_unpacklo_epi8(a, b); }
__m128i sse::unpacklo_epi16(__m128i a, __m128i b) { return _mm_unpacklo_epi16(a, b); }
__m128i sse::unpacklo_epi32(__m128i a, __m128i b) { return _mm_unpacklo_epi32(a, b); }
__m128i sse::unpacklo_epi64(__m128i a, __m128i b) { return _mm_unpacklo_epi64(a, b); }

__m128i sse::unpacklo(__m128i a, __m128i b, int8_t) { return unpacklo_epi8(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int16_t) { return unpacklo_epi16(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int32_t) { return unpacklo_epi32(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int64_t) { return unpacklo_epi64(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint8_t) { return unpacklo_epi8(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint16_t) { return unpacklo_epi16(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint32_t) { return unpacklo_epi32(a, b); }

__m128i sse::Xor(__m128i a, __m128i b) { return _mm_xor_si128(a, b); }

#endif // WJR_SSE2

#if WJR_SSE3
__m128i sse::lddqu(const __m128i* ptr) { return _mm_lddqu_si128(ptr); }
#endif // WJR_SSE3

#if WJR_SSSE3

__m128i sse::abs_epi8(__m128i val) { return _mm_abs_epi8(val); }
__m128i sse::abs_epi16(__m128i val) { return _mm_abs_epi16(val); }
__m128i sse::abs_epi32(__m128i val) { return _mm_abs_epi32(val); }

__m128i sse::abs(__m128i val, int8_t) { return abs_epi8(val); }
__m128i sse::abs(__m128i val, int16_t) { return abs_epi16(val); }
__m128i sse::abs(__m128i val, int32_t) { return abs_epi32(val); }
__m128i sse::abs(__m128i val, uint8_t) { return val; }
__m128i sse::abs(__m128i val, uint16_t) { return val; }
__m128i sse::abs(__m128i val, uint32_t) { return val; }

__m128i sse::shuffle_epi8(__m128i v, __m128i imm8) { return _mm_shuffle_epi8(v, imm8); }

__m128i sse::sign_epi8(__m128i a, __m128i b) { return _mm_sign_epi8(a, b); }
__m128i sse::sign_epi16(__m128i a, __m128i b) { return _mm_sign_epi16(a, b); }
__m128i sse::sign_epi32(__m128i a, __m128i b) { return _mm_sign_epi32(a, b); }

__m128i sse::sign(__m128i a, __m128i b, int8_t) { return sign_epi8(a, b); }
__m128i sse::sign(__m128i a, __m128i b, int16_t) { return sign_epi16(a, b); }
__m128i sse::sign(__m128i a, __m128i b, int32_t) { return sign_epi32(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint8_t) { return sign_epi8(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint16_t) { return sign_epi16(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint32_t) { return sign_epi32(a, b); }

#endif // WJR_SSSE3

#if WJR_SSE4_1
template<int imm8>
__m128i sse::blend_epi16(__m128i a, __m128i b) {
	return _mm_blend_epi16(a, b, imm8);
}

__m128i sse::cmpeq_epi64(__m128i a, __m128i b) { return _mm_cmpeq_epi64(a, b); }

__m128i sse::cmpeq(__m128i a, __m128i b, int64_t) { return cmpeq_epi64(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint64_t) { return cmpeq_epi64(a, b); }

__m128i sse::cmpgt_epi64(__m128i a, __m128i b) { return _mm_cmpgt_epi64(a, b); }

__m128i sse::cmpgt(__m128i a, __m128i b, int64_t) { return cmpgt_epi64(a, b); }

template<int imm8>
__m128i sse::insert_epi8(__m128i a, int i) {
	return _mm_insert_epi8(a, i, imm8);
}

template<int imm8>
__m128i sse::insert_epi32(__m128i a, int i) {
	return _mm_insert_epi32(a, i, imm8);
}

template<int imm8>
__m128i sse::insert_epi64(__m128i a, int64_t i) {
	return _mm_insert_epi64(a, i, imm8);
}

template<int imm8>
__m128i sse::insert(__m128i a, int i, int8_t) { return insert_epi8<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int i, int32_t) { return insert_epi32<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int64_t i, int64_t) { return insert_epi64<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int i, uint8_t) { return insert_epi8<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int i, uint32_t) { return insert_epi32<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int64_t i, uint64_t) { return insert_epi64<imm8>(a, i); }

__m128i sse::minpos_epu16(__m128i a) { return _mm_minpos_epu16(a); }

__m128i sse::mul_epi32(__m128i a, __m128i b) { return _mm_mul_epi32(a, b); }

__m128i sse::mullo_epi32(__m128i a, __m128i b) { return _mm_mullo_epi32(a, b); }

__m128i sse::packus_epi32(__m128i a, __m128i b) { return _mm_packus_epi32(a, b); }

__m128i sse::stream_load(__m128i* p) { return _mm_stream_load_si128(p); }

int sse::test_all_ones(__m128i a) { return _mm_test_all_ones(a); }

int sse::test_all_zeros(__m128i a, __m128i b) { return _mm_test_all_zeros(a, b); }

int sse::test_all_zeros(__m128i a) { return _mm_test_all_zeros(a, a); }

int sse::test_mix_ones_zeros(__m128i a, __m128i b) { return _mm_test_mix_ones_zeros(a, b); }

int sse::testc(__m128i a, __m128i b) { return _mm_testc_si128(a, b); }

int sse::testnzc(__m128i a, __m128i b) { return _mm_testnzc_si128(a, b); }

int sse::testz(__m128i a, __m128i b) {
	return _mm_testz_si128(a, b);
}
#endif // WJR_SSE4_1

/*------AVX------*/

constexpr size_t avx::width() { return 256; }

constexpr avx::mask_type avx::mask() { return 0xffffffff; }

#if WJR_AVX

__m256i avx::concat(__m128i a, __m128i b) {
	return insert_si128<1>(simd_cast<__m256i_tag, __m128i_tag>(a), b);
}

template<int imm8>
int avx::extract_epi32(__m256i v) { return _mm256_extract_epi32(v, imm8); }
template<int imm8>
int64_t avx::extract_epi64(__m256i v) { return _mm256_extract_epi64(v, imm8); }

template<int imm8>
int avx::extract(__m256i v, int32_t) { return extract_epi32<imm8>(v); }
template<int imm8>
int64_t avx::extract(__m256i v, int64_t) { return extract_epi64<imm8>(v); }

template<int imm8>
__m128i avx::extract_si128(__m256i v) {
#if WJR_AVX2
	return _mm256_extracti128_si256(v, imm8);
#else
	return _mm256_extractf128_si256(v, imm8);
#endif // WJR_AVX2
}

__m128i avx::getlow(__m256i a) {
	return simd_cast<__m128i_tag, __m256i_tag>(a);
}

__m128i avx::gethigh(__m256i a) {
	return extract_si128<1>(a);
}

template<int imm8>
__m256i avx::insert_epi8(__m256i v, int8_t i) { return _mm256_insert_epi8(v, i, imm8); }
template<int imm8>
__m256i avx::insert_epi16(__m256i v, int16_t i) { return _mm256_insert_epi16(v, i, imm8); }
template<int imm8>
__m256i avx::insert_epi32(__m256i v, int32_t i) { return _mm256_insert_epi32(v, i, imm8); }
template<int imm8>
__m256i avx::insert_epi64(__m256i v, int64_t i) { return _mm256_insert_epi64(v, i, imm8); }

template<int imm8>
__m256i avx::insert_si128(__m256i a, __m128i b) {
#if WJR_AVX2
	return _mm256_inserti128_si256(a, b, imm8);
#else
	return _mm256_insertf128_si256(a, b, imm8);
#endif // WJR_AVX2
}

__m256i avx::load(const __m256i* p) { return _mm256_load_si256(p); }
__m256i avx::loadu(const __m256i* p) { return _mm256_loadu_si256(p); }

__m256i avx::ones() { return _mm256_set1_epi32(-1); }

__m256i avx::preloadu_si16(const void* ptr) {
	return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si16(ptr));
}

__m256i avx::preloadu_si32(const void* ptr) {
	return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si32(ptr));
}

__m256i avx::preloadu_si48(const void* ptr) {
	return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si48(ptr));
}

__m256i avx::preloadu_si64(const void* ptr) {
	return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si64(ptr));
}

__m256i avx::preloadu_si80(const void* ptr) {
	return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si80(ptr));
}

__m256i avx::preloadu_si96(const void* ptr) {
	return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si96(ptr));
}

__m256i avx::preloadu_si112(const void* ptr) {
	return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si112(ptr));
}

__m256i avx::preloadu_si128(const void* ptr) {
	return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si128(ptr));
}

__m256i avx::preloadu_si144(const void* ptr) {
	return concat(sse::preloadu_si128(ptr),
		sse::preloadu_si16((const char*)ptr + 16));
}

__m256i avx::preloadu_si160(const void* ptr) {
	return concat(sse::preloadu_si128(ptr),
		sse::preloadu_si32((const char*)ptr + 16));
}

__m256i avx::preloadu_si176(const void* ptr) {
	return concat(sse::preloadu_si128(ptr),
		sse::preloadu_si48((const char*)ptr + 16));
}

__m256i avx::preloadu_si192(const void* ptr) {
	return concat(sse::preloadu_si128(ptr),
		sse::preloadu_si64((const char*)ptr + 16));
}

__m256i avx::preloadu_si208(const void* ptr) {
	return concat(sse::preloadu_si128(ptr),
		sse::preloadu_si80((const char*)ptr + 16));
}

__m256i avx::preloadu_si224(const void* ptr) {
	return concat(sse::preloadu_si128(ptr),
		sse::preloadu_si96((const char*)ptr + 16));
}
__m256i avx::preloadu_si240(const void* ptr) {
	return concat(sse::preloadu_si128(ptr),
		sse::preloadu_si112((const char*)ptr + 16));
}

__m256i avx::preloadu_si256(const void* ptr) {
	return loadu((const __m256i*)ptr);
}

__m256i avx::preloadu_si16x(const void* ptr, int n) {
	switch (n) {
	case 0:return zeros();
	case 1:return preloadu_si16(ptr);
	case 2:return preloadu_si32(ptr);
	case 3:return preloadu_si48(ptr);
	case 4:return preloadu_si64(ptr);
	case 5:return preloadu_si80(ptr);
	case 6:return preloadu_si96(ptr);
	case 7:return preloadu_si112(ptr);
	case 8:return preloadu_si128(ptr);
	case 9:return preloadu_si144(ptr);
	case 10:return preloadu_si160(ptr);
	case 11:return preloadu_si176(ptr);
	case 12:return preloadu_si192(ptr);
	case 13:return preloadu_si208(ptr);
	case 14:return preloadu_si224(ptr);
	case 15:return preloadu_si240(ptr);
	default:return preloadu_si256(ptr);
	}
}

__m256i avx::set_epi8(
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

__m256i avx::set_epi16(
	short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
	short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
	return _mm256_set_epi16(
		e15, e14, e13, e12, e11, e10, e9, e8,
		e7, e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::set_epi32(
	int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
	return _mm256_set_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::set_epi64x(
	long long e3, long long e2, long long e1, long long e0) {
	return _mm256_set_epi64x(e3, e2, e1, e0);
}

__m256i avx::setr_epi8(
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

__m256i avx::setr_epi16(
	short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
	short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
	return _mm256_setr_epi16(
		e15, e14, e13, e12, e11, e10, e9, e8,
		e7, e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::setr_epi32(
	int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
	return _mm256_setr_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
}

__m256i avx::setr_epi64x(
	long long e3, long long e2, long long e1, long long e0) {
	return _mm256_setr_epi64x(e3, e2, e1, e0);
}

__m256i avx::set1_epi8(int8_t a) { return _mm256_set1_epi8(a); }
__m256i avx::set1_epi16(int16_t a) { return _mm256_set1_epi16(a); }
__m256i avx::set1_epi32(int32_t a) { return _mm256_set1_epi32(a); }
__m256i avx::set1_epi64(int64_t a) { return _mm256_set1_epi64x(a); }

__m256i avx::set1(int8_t a, int8_t) { return set1_epi8(a); }
__m256i avx::set1(int16_t a, int16_t) { return set1_epi16(a); }
__m256i avx::set1(int32_t a, int32_t) { return set1_epi32(a); }
__m256i avx::set1(int64_t a, int64_t) { return set1_epi64(a); }
__m256i avx::set1(uint8_t a, uint8_t) { return set1_epi8(a); }
__m256i avx::set1(uint16_t a, uint16_t) { return set1_epi16(a); }
__m256i avx::set1(uint32_t a, uint32_t) { return set1_epi32(a); }
__m256i avx::set1(uint64_t a, uint64_t) { return set1_epi64(a); }

__m256i avx::setmin_epi8() { return set1_epi8(0x80u); }
__m256i avx::setmin_epi16() { return set1_epi16(0x8000u); }
__m256i avx::setmin_epi32() { return set1_epi32(0x80000000u); }
__m256i avx::setmin_epi64() { return set1_epi64(0x8000000000000000ull); }

__m256i avx::setmin(int8_t) { return setmin_epi8(); }
__m256i avx::setmin(int16_t) { return setmin_epi16(); }
__m256i avx::setmin(int32_t) { return setmin_epi32(); }
__m256i avx::setmin(int64_t) { return setmin_epi64(); }

__m256i avx::setmax_epi8() { return set1_epi8(0x7f); }
__m256i avx::setmax_epi16() { return set1_epi16(0x7fff); }
__m256i avx::setmax_epi32() { return set1_epi32(0x7fffffff); }
__m256i avx::setmax_epi64() { return set1_epi64(0x7fffffffffffffff); }

__m256i avx::setmax(int8_t) { return setmax_epi8(); }
__m256i avx::setmax(int16_t) { return setmax_epi16(); }
__m256i avx::setmax(int32_t) { return setmax_epi32(); }
__m256i avx::setmax(int64_t) { return setmax_epi64(); }

void avx::stream(__m256i* p, __m256i a) { _mm256_stream_si256(p, a); }

void avx::store(__m256i* p, const __m256i& a) { _mm256_store_si256(p, a); }
void avx::storeu(__m256i* p, const __m256i& a) { _mm256_storeu_si256(p, a); }

int avx::test_all_zeros(__m256i a) { return testz(a, a); }

int avx::testc(__m256i a, __m256i b) { return _mm256_testc_si256(a, b); }

int avx::testnzc(__m256i a, __m256i b) { return _mm256_testnzc_si256(a, b); }

int avx::testz(__m256i a, __m256i b) { return _mm256_testz_si256(a, b); }

__m256i avx::zeros() { return _mm256_setzero_si256(); }

#endif // WJR_AVX

#if WJR_AVX2
__m256i avx::And(__m256i a, __m256i b) { return _mm256_and_si256(a, b); }

__m256i avx::AndNot(__m256i a, __m256i b) { return _mm256_andnot_si256(a, b); }

__m256i avx::Or(__m256i a, __m256i b) { return _mm256_or_si256(a, b); }

__m256i avx::Xor(__m256i a, __m256i b) { return _mm256_xor_si256(a, b); }

__m256i avx::Not(__m256i v) { return _mm256_xor_si256(v, ones()); }

__m256i avx::abs_epi8(__m256i v) { return _mm256_abs_epi8(v); }
__m256i avx::abs_epi16(__m256i v) { return _mm256_abs_epi16(v); }
__m256i avx::abs_epi32(__m256i v) { return _mm256_abs_epi32(v); }

__m256i avx::abs(__m256i v, int8_t) { return abs_epi8(v); }
__m256i avx::abs(__m256i v, int16_t) { return abs_epi16(v); }
__m256i avx::abs(__m256i v, int32_t) { return abs_epi32(v); }
__m256i avx::abs(__m256i v, int64_t) { return abs_epi32(v); }

__m256i avx::add_epi8(__m256i a, __m256i b) { return _mm256_add_epi8(a, b); }
__m256i avx::add_epi16(__m256i a, __m256i b) { return _mm256_add_epi16(a, b); }
__m256i avx::add_epi32(__m256i a, __m256i b) { return _mm256_add_epi32(a, b); }
__m256i avx::add_epi64(__m256i a, __m256i b) { return _mm256_add_epi64(a, b); }

__m256i avx::add(__m256i a, __m256i b, int8_t) { return add_epi8(a, b); }
__m256i avx::add(__m256i a, __m256i b, int16_t) { return add_epi16(a, b); }
__m256i avx::add(__m256i a, __m256i b, int32_t) { return add_epi32(a, b); }
__m256i avx::add(__m256i a, __m256i b, int64_t) { return add_epi64(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint8_t) { return add_epi8(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint16_t) { return add_epi16(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint32_t) { return add_epi32(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint64_t) { return add_epi64(a, b); }

uint8_t avx::add_epu8(__m256i v) {
	return sse::add_epu8(sse::add_epi8(getlow(v), gethigh(v)));
}

uint16_t avx::add_epu16(__m256i v) {
	return sse::add_epu16(sse::add_epi16(getlow(v), gethigh(v)));
}

uint32_t avx::add_epu32(__m256i v) {
	return sse::add_epu32(sse::add_epi32(getlow(v), gethigh(v)));
}

uint64_t avx::add_epu64(__m256i v) {
	return sse::add_epu64(sse::add_epi64(getlow(v), gethigh(v)));
}

int8_t avx::add_epi8(__m256i v) { return add_epu8(v); }
int16_t avx::add_epi16(__m256i v) { return add_epu16(v); }
int32_t avx::add_epi32(__m256i v) { return add_epu32(v); }
int64_t avx::add_epi64(__m256i v) { return add_epu64(v); }

int8_t avx::add(__m256i v, int8_t) { return add_epi8(v); }
int16_t avx::add(__m256i v, int16_t) { return add_epi16(v); }
int32_t avx::add(__m256i v, int32_t) { return add_epi32(v); }
int64_t avx::add(__m256i v, int64_t) { return add_epi64(v); }
uint8_t avx::add(__m256i v, uint8_t) { return add_epu8(v); }
uint16_t avx::add(__m256i v, uint16_t) { return add_epu16(v); }
uint32_t avx::add(__m256i v, uint32_t) { return add_epu32(v); }
uint64_t avx::add(__m256i v, uint64_t) { return add_epu64(v); }

__m256i avx::adds_epi8(__m256i a, __m256i b) { return _mm256_adds_epi8(a, b); }
__m256i avx::adds_epi16(__m256i a, __m256i b) { return _mm256_adds_epi16(a, b); }

__m256i avx::adds_epu8(__m256i a, __m256i b) { return _mm256_adds_epu8(a, b); }
__m256i avx::adds_epu16(__m256i a, __m256i b) { return _mm256_adds_epu16(a, b); }

__m256i avx::adds(__m256i a, __m256i b, int8_t) { return adds_epi8(a, b); }
__m256i avx::adds(__m256i a, __m256i b, int16_t) { return adds_epi16(a, b); }
__m256i avx::adds(__m256i a, __m256i b, uint8_t) { return adds_epu8(a, b); }
__m256i avx::adds(__m256i a, __m256i b, uint16_t) { return adds_epu16(a, b); }

template<int imm8>
__m256i avx::alignr(__m256i a, __m256i b) { return _mm256_alignr_epi8(a, b, imm8); }

__m256i avx::alignr_epi16(__m256i a, __m256i b, int c) {
	return Or(slli_epi16(a, 16 - c), srli_epi16(b, c));
}

__m256i avx::alignr_epi32(__m256i a, __m256i b, int c) {
	return Or(slli_epi32(a, 32 - c), srli_epi32(b, c));
}

__m256i avx::alignr_epi64(__m256i a, __m256i b, int c) {
	return Or(slli_epi64(a, 64 - c), srli_epi64(b, c));
}

__m256i avx::alignr(__m256i a, __m256i b, int c, int16_t) {
	return alignr_epi16(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, int32_t) {
	return alignr_epi32(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, int64_t) {
	return alignr_epi64(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, uint16_t) {
	return alignr_epi16(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, uint32_t) {
	return alignr_epi32(a, b, c);
}

__m256i avx::alignr(__m256i a, __m256i b, int c, uint64_t) {
	return alignr_epi64(a, b, c);
}

__m256i avx::avg_epu8(__m256i a, __m256i b) { return _mm256_avg_epu8(a, b); }
__m256i avx::avg_epu16(__m256i a, __m256i b) { return _mm256_avg_epu16(a, b); }

__m256i avx::avg(__m256i a, __m256i b, int8_t) { return avg_epu8(a, b); }
__m256i avx::avg(__m256i a, __m256i b, int16_t) { return avg_epu16(a, b); }
__m256i avx::avg(__m256i a, __m256i b, uint8_t) { return avg_epu8(a, b); }
__m256i avx::avg(__m256i a, __m256i b, uint16_t) { return avg_epu16(a, b); }

template<int imm8>
__m256i avx::blend_epi16(__m256i a, __m256i b) { return _mm256_blend_epi16(a, b, imm8); }

template<int imm8>
__m256i avx::blend_epi32(__m256i a, __m256i b) { return _mm256_blend_epi32(a, b, imm8); }

__m256i avx::blendv_epi8(__m256i a, __m256i b, __m256i mask) {
	return _mm256_blendv_epi8(a, b, mask);
}

template<int imm8>
__m256i avx::bslli_epi128(__m256i a) { return _mm256_bslli_epi128(a, imm8); }

template<int imm8>
__m256i avx::bsrli_epi128(__m256i a) { return _mm256_bsrli_epi128(a, imm8); }

__m256i avx::cmpeq_epi8(__m256i a, __m256i b) { return _mm256_cmpeq_epi8(a, b); }
__m256i avx::cmpeq_epi16(__m256i a, __m256i b) { return _mm256_cmpeq_epi16(a, b); }
__m256i avx::cmpeq_epi32(__m256i a, __m256i b) { return _mm256_cmpeq_epi32(a, b); }
__m256i avx::cmpeq_epi64(__m256i a, __m256i b) { return _mm256_cmpeq_epi64(a, b); }

__m256i avx::cmpeq(__m256i a, __m256i b, int8_t) { return cmpeq_epi8(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int16_t) { return cmpeq_epi16(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int32_t) { return cmpeq_epi32(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int64_t) { return cmpeq_epi64(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint8_t) { return cmpeq_epi8(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint16_t) { return cmpeq_epi16(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint32_t) { return cmpeq_epi32(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint64_t) { return cmpeq_epi64(a, b); }

__m256i avx::cmpge_epi8(__m256i a, __m256i b) {
	return cmpeq(min(a, b, int8_t()), b, uint8_t());
}

__m256i avx::cmpge_epi16(__m256i a, __m256i b) {
	return cmpeq(min(a, b, int16_t()), b, uint16_t());
}

__m256i avx::cmpge_epi32(__m256i a, __m256i b) {
	return cmpeq(min(a, b, int32_t()), b, uint8_t());
}

__m256i avx::cmpge_epu8(__m256i a, __m256i b) {
	return cmpeq(min(a, b, uint8_t()), b, uint8_t());
}

__m256i avx::cmpge_epu16(__m256i a, __m256i b) {
	return cmpeq(min(a, b, uint16_t()), b, uint16_t());
}

__m256i avx::cmpge_epu32(__m256i a, __m256i b) {
	return cmpeq(min(a, b, uint32_t()), b, uint32_t());
}

__m256i avx::cmpge(__m256i a, __m256i b, int8_t) { return cmpge_epi8(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, int16_t) { return cmpge_epi16(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, int32_t) { return cmpge_epi32(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint8_t) { return cmpge_epu8(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint16_t) { return cmpge_epu16(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint32_t) { return cmpge_epu32(a, b); }

__m256i avx::cmpgt_epi8(__m256i a, __m256i b) { return _mm256_cmpgt_epi8(a, b); }
__m256i avx::cmpgt_epi16(__m256i a, __m256i b) { return _mm256_cmpgt_epi16(a, b); }
__m256i avx::cmpgt_epi32(__m256i a, __m256i b) { return _mm256_cmpgt_epi32(a, b); }
__m256i avx::cmpgt_epi64(__m256i a, __m256i b) { return _mm256_cmpgt_epi64(a, b); }

__m256i avx::cmpgt_epu8(__m256i a, __m256i b) {
	return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
}

__m256i avx::cmpgt_epu16(__m256i a, __m256i b) {
	return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
}

__m256i avx::cmpgt_epu32(__m256i a, __m256i b) {
	return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
}

__m256i avx::cmpgt_epu64(__m256i a, __m256i b) {
	return cmpgt_epi64(Xor(a, setmin_epi64()), Xor(b, setmin_epi64()));
}

__m256i avx::cmpgt(__m256i a, __m256i b, int8_t) { return cmpgt_epi8(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int16_t) { return cmpgt_epi16(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int32_t) { return cmpgt_epi32(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int64_t) { return cmpgt_epi64(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint8_t) { return cmpgt_epu8(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint16_t) { return cmpgt_epu16(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint32_t) { return cmpgt_epu32(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint64_t) { return cmpgt_epu64(a, b); }

__m256i avx::cmple_epi8(__m256i a, __m256i b) {
	return cmpge_epi8(b, a);
}

__m256i avx::cmple_epi16(__m256i a, __m256i b) {
	return cmpge_epi16(b, a);
}

__m256i avx::cmple_epi32(__m256i a, __m256i b) {
	return cmpge_epi32(b, a);
}

__m256i avx::cmple_epu8(__m256i a, __m256i b) {
	return cmpge_epu8(b, a);
}

__m256i avx::cmple_epu16(__m256i a, __m256i b) {
	return cmpge_epu16(b, a);
}

__m256i avx::cmple_epu32(__m256i a, __m256i b) {
	return cmpge_epu32(b, a);
}

__m256i avx::cmple(__m256i a, __m256i b, int8_t) { return cmple_epi8(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, int16_t) { return cmple_epi16(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, int32_t) { return cmple_epi32(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint8_t) { return cmple_epu8(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint16_t) { return cmple_epu16(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint32_t) { return cmple_epu32(a, b); }

__m256i avx::cmplt_epi8(__m256i a, __m256i b) { return cmpgt_epi8(b, a); }
__m256i avx::cmplt_epi16(__m256i a, __m256i b) { return cmpgt_epi16(b, a); }
__m256i avx::cmplt_epi32(__m256i a, __m256i b) { return cmpgt_epi32(b, a); }

__m256i avx::cmplt_epu8(__m256i a, __m256i b) {
	return cmpgt_epu8(b, a);
}
__m256i avx::cmplt_epu16(__m256i a, __m256i b) {
	return cmpgt_epu16(b, a);
}
__m256i avx::cmplt_epu32(__m256i a, __m256i b) {
	return cmpgt_epu32(b, a);
}

__m256i avx::cmplt(__m256i a, __m256i b, int8_t) { return cmplt_epi8(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, int16_t) { return cmplt_epi16(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, int32_t) { return cmplt_epi32(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint8_t) { return cmplt_epu8(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint16_t) { return cmplt_epu16(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint32_t) { return cmplt_epu32(a, b); }

__m256i avx::cmpne_epi8(__m256i a, __m256i b) { return Not(cmpeq_epi8(a, b)); }
__m256i avx::cmpne_epi16(__m256i a, __m256i b) { return Not(cmpeq_epi16(a, b)); }
__m256i avx::cmpne_epi32(__m256i a, __m256i b) { return Not(cmpeq_epi32(a, b)); }

__m256i avx::cmpne(__m256i a, __m256i b, int8_t) { return cmpne_epi8(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, int16_t) { return cmpne_epi16(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, int32_t) { return cmpne_epi32(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint8_t) { return cmpne_epi8(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint16_t) { return cmpne_epi16(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint32_t) { return cmpne_epi32(a, b); }

template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::equal_to<>, T) {
	return cmpeq(a, b, T());
}
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::not_equal_to<>, T) {
	return cmpne(a, b, T());
}
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::greater<>, T) {
	return cmpgt(a, b, T());
}
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::greater_equal<>, T) {
	return cmpge(a, b, T());
}
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::less<>, T) {
	return cmplt(a, b, T());
}
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::less_equal<>, T) {
	return cmple(a, b, T());
}

template<int imm8>
int avx::extract_epi8(__m256i v) { return _mm256_extract_epi8(v, imm8); }
template<int imm8>
int avx::extract_epi16(__m256i v) { return _mm256_extract_epi16(v, imm8); }

template<int imm8>
int avx::extract(__m256i v, int8_t) { return extract_epi8<imm8>(v); }
template<int imm8>
int avx::extract(__m256i v, int16_t) { return extract_epi16<imm8>(v); }

__m256i avx::hadd_epi16(__m256i a, __m256i b) { return _mm256_hadd_epi16(a, b); }
__m256i avx::hadd_epi32(__m256i a, __m256i b) { return _mm256_hadd_epi32(a, b); }

__m256i avx::hadd(__m256i a, __m256i b, int16_t) { return hadd_epi16(a, b); }
__m256i avx::hadd(__m256i a, __m256i b, int32_t) { return hadd_epi32(a, b); }

__m256i avx::hadds_epi16(__m256i a, __m256i b) { return _mm256_hadds_epi16(a, b); }

__m256i avx::hsub_epi16(__m256i a, __m256i b) { return _mm256_hsub_epi16(a, b); }
__m256i avx::hsub_epi32(__m256i a, __m256i b) { return _mm256_hsub_epi32(a, b); }

__m256i avx::hsub(__m256i a, __m256i b, int16_t) { return hsub_epi16(a, b); }
__m256i avx::hsub(__m256i a, __m256i b, int32_t) { return hsub_epi32(a, b); }

__m256i avx::hsubs_epi16(__m256i a, __m256i b) { return _mm256_hsubs_epi16(a, b); }

template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
	uint8_t, uint16_t, uint32_t, uint64_t>, int>>
	__m256i avx::logical_and(__m256i a, __m256i b, T) {
	return Not(Or(logical_not(a, T()), logical_not(b, T())));
}

template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
	uint8_t, uint16_t, uint32_t, uint64_t>, int>>
	__m256i avx::logical_not(__m256i v, T) {
	auto Zero = zeros();
	return cmpeq(v, Zero, T());
}

template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
	uint8_t, uint16_t, uint32_t, uint64_t>, int>>
	__m256i avx::logical_or(__m256i a, __m256i b, T) {
	return Not(logical_not(Or(a, b), T()));
}

__m256i avx::madd_epi16(__m256i a, __m256i b) { return _mm256_madd_epi16(a, b); }

__m256i avx::max_epi8(__m256i a, __m256i b) { return _mm256_max_epi8(a, b); }
__m256i avx::max_epi16(__m256i a, __m256i b) { return _mm256_max_epi16(a, b); }
__m256i avx::max_epi32(__m256i a, __m256i b) { return _mm256_max_epi32(a, b); }

__m256i avx::max_epu8(__m256i a, __m256i b) { return _mm256_max_epu8(a, b); }
__m256i avx::max_epu16(__m256i a, __m256i b) { return _mm256_max_epu16(a, b); }
__m256i avx::max_epu32(__m256i a, __m256i b) { return _mm256_max_epu32(a, b); }

__m256i avx::max(__m256i a, __m256i b, int8_t) { return max_epi8(a, b); }
__m256i avx::max(__m256i a, __m256i b, int16_t) { return max_epi16(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint8_t) { return max_epu8(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint16_t) { return max_epu16(a, b); }
__m256i avx::max(__m256i a, __m256i b, int32_t) { return max_epi32(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint32_t) { return max_epu32(a, b); }

int8_t avx::max_epi8(__m256i a) {
	return sse::max_epi8(sse::max_epi8(getlow(a), gethigh(a)));
}

int16_t avx::max_epi16(__m256i a) {
	return sse::max_epi16(sse::max_epi16(getlow(a), gethigh(a)));
}

int32_t avx::max_epi32(__m256i a) {
	return sse::max_epi32(sse::max_epi32(getlow(a), gethigh(a)));
}

uint8_t avx::max_epu8(__m256i a) {
	return sse::max_epu8(sse::max_epu8(getlow(a), gethigh(a)));
}

uint16_t avx::max_epu16(__m256i a) {
	return sse::max_epu16(sse::max_epu16(getlow(a), gethigh(a)));
}

uint32_t avx::max_epu32(__m256i a) {
	return sse::max_epu32(sse::max_epu32(getlow(a), gethigh(a)));
}

int8_t avx::max(__m256i a, int8_t) { return max_epi8(a); }
int16_t avx::max(__m256i a, int16_t) { return max_epi16(a); }
int32_t avx::max(__m256i a, int32_t) { return max_epi32(a); }
uint8_t avx::max(__m256i a, uint8_t) { return max_epu8(a); }
uint16_t avx::max(__m256i a, uint16_t) { return max_epu16(a); }
uint32_t avx::max(__m256i a, uint32_t) { return max_epu32(a); }

__m256i avx::min_epi8(__m256i a, __m256i b) { return _mm256_min_epi8(a, b); }
__m256i avx::min_epi16(__m256i a, __m256i b) { return _mm256_min_epi16(a, b); }
__m256i avx::min_epi32(__m256i a, __m256i b) { return _mm256_min_epi32(a, b); }

__m256i avx::min_epu8(__m256i a, __m256i b) { return _mm256_min_epu8(a, b); }
__m256i avx::min_epu16(__m256i a, __m256i b) { return _mm256_min_epu16(a, b); }
__m256i avx::min_epu32(__m256i a, __m256i b) { return _mm256_min_epu32(a, b); }

__m256i avx::min(__m256i a, __m256i b, int8_t) { return min_epi8(a, b); }
__m256i avx::min(__m256i a, __m256i b, int16_t) { return min_epi16(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint8_t) { return min_epu8(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint16_t) { return min_epu16(a, b); }
__m256i avx::min(__m256i a, __m256i b, int32_t) { return min_epi32(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint32_t) { return min_epu32(a, b); }

int8_t avx::min_epi8(__m256i a) {
	return sse::min_epi8(sse::min_epi8(getlow(a), gethigh(a)));
}

int16_t avx::min_epi16(__m256i a) {
	return sse::min_epi16(sse::min_epi16(getlow(a), gethigh(a)));
}

int32_t avx::min_epi32(__m256i a) {
	return sse::min_epi32(sse::min_epi32(getlow(a), gethigh(a)));
}

uint8_t avx::min_epu8(__m256i a) {
	return sse::min_epu8(sse::min_epu8(getlow(a), gethigh(a)));
}

uint16_t avx::min_epu16(__m256i a) {
	return sse::min_epu16(sse::min_epu16(getlow(a), gethigh(a)));
}

uint32_t avx::min_epu32(__m256i a) {
	return sse::min_epu32(sse::min_epu32(getlow(a), gethigh(a)));
}

int8_t avx::min(__m256i a, int8_t) { return min_epi8(a); }
int16_t avx::min(__m256i a, int16_t) { return min_epi16(a); }
int32_t avx::min(__m256i a, int32_t) { return min_epi32(a); }
uint8_t avx::min(__m256i a, uint8_t) { return min_epu8(a); }
uint16_t avx::min(__m256i a, uint16_t) { return min_epu16(a); }
uint32_t avx::min(__m256i a, uint32_t) { return min_epu32(a); }

avx::mask_type avx::movemask_epi8(__m256i a) { return _mm256_movemask_epi8(a); }

__m256i avx::mul_epi32(__m256i a, __m256i b) { return _mm256_mul_epi32(a, b); }
__m256i avx::mul_epu32(__m256i a, __m256i b) { return _mm256_mul_epu32(a, b); }

__m256i avx::mulhi_epi16(__m256i a, __m256i b) { return _mm256_mulhi_epi16(a, b); }

__m256i avx::mulhi_epu16(__m256i a, __m256i b) { return _mm256_mulhi_epu16(a, b); }

__m256i avx::mullo_epi16(__m256i a, __m256i b) { return _mm256_mullo_epi16(a, b); }

__m256i avx::packs_epi16(__m256i a, __m256i b) { return _mm256_packs_epi16(a, b); }
__m256i avx::packs_epi32(__m256i a, __m256i b) { return _mm256_packs_epi32(a, b); }

__m256i avx::packus_epi16(__m256i a, __m256i b) { return _mm256_packus_epi16(a, b); }
__m256i avx::packus_epi32(__m256i a, __m256i b) { return _mm256_packus_epi32(a, b); }

template<int imm>
__m256i avx::shl(__m256i a) {
	if constexpr (imm >= 64 * 3) {
		a = slli<8 * 3>(a);
		a = slli_epi64(a, imm - 64 * 3);
		return a;
	}
	else if constexpr (imm >= 64 * 2) {
		a = slli<8 * 2>(a);
		constexpr auto I = imm - 64 * 2;
		auto b = slli_epi64(a, I);
		auto c = slli<8>(a);
		c = srli_epi64(c, 64 - I);
		return Or(b, c);
	}
	else if constexpr (imm >= 64) {
		a = slli<8>(a);
		constexpr auto I = imm - 64;
		auto b = slli_epi64(a, I);
		auto c = slli<8>(a);
		c = srli_epi64(c, 64 - I);
		return Or(b, c);
	}
	else {
		auto b = slli_epi64(a, imm);
		auto c = slli<8>(a);
		c = srli_epi64(c, 64 - imm);
		return Or(b, c);
	}
}

template<int imm>
__m256i avx::shr(__m256i a) {
	if constexpr (imm >= 64 * 3) {
		a = srli<8 * 3>(a);
		a = srli_epi64(a, imm - 64 * 3);
		return a;
	}
	else if constexpr (imm >= 64 * 2) {
		a = srli<8 * 2>(a);
		constexpr auto I = imm - 64 * 2;
		auto b = srli_epi64(a, I);
		auto c = srli<8>(a);
		c = slli_epi64(c, 64 - I);
		return Or(b, c);
	}
	else if constexpr (imm >= 64) {
		a = srli<8>(a);
		constexpr auto I = imm - 64;
		auto b = srli_epi64(a, I);
		auto c = srli<8>(a);
		c = slli_epi64(c, 64 - I);
		return Or(b, c);
	}
	else {
		auto b = srli_epi64(a, imm);
		auto c = srli<8>(a);
		c = slli_epi64(c, 64 - imm);
		return Or(b, c);
	}
}

__m256i avx::shuffle_epi8(__m256i a, __m256i b) { return _mm256_shuffle_epi8(a, b); }

template<int imm8>
__m256i avx::shuffle_epi32(__m256i a) {
	return _mm256_shuffle_epi32(a, imm8);
}

template<int imm8>
__m256i avx::shufflehi_epi16(__m256i a) {
	return _mm256_shufflehi_epi16(a, imm8);
}

template<int imm8>
__m256i avx::shufflelo_epi16(__m256i a) {
	return _mm256_shufflelo_epi16(a, imm8);
}

__m256i avx::sll_epi16(__m256i a, __m128i b) { return _mm256_sll_epi16(a, b); }
__m256i avx::sll_epi32(__m256i a, __m128i b) { return _mm256_sll_epi32(a, b); }
__m256i avx::sll_epi64(__m256i a, __m128i b) { return _mm256_sll_epi64(a, b); }

__m256i avx::sll(__m256i a, __m128i b, int16_t) { return sll_epi16(a, b); }
__m256i avx::sll(__m256i a, __m128i b, int32_t) { return sll_epi32(a, b); }
__m256i avx::sll(__m256i a, __m128i b, int64_t) { return sll_epi64(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint16_t) { return sll_epi16(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint32_t) { return sll_epi32(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint64_t) { return sll_epi64(a, b); }

template<int imm8>
__m256i avx::slli(__m256i a) {
	return _mm256_slli_si256(a, imm8);
}
__m256i avx::slli_epi16(__m256i a, int imm8) { return _mm256_slli_epi16(a, imm8); }
__m256i avx::slli_epi32(__m256i a, int imm8) { return _mm256_slli_epi32(a, imm8); }
__m256i avx::slli_epi64(__m256i a, int imm8) { return _mm256_slli_epi64(a, imm8); }

__m256i avx::slli(__m256i a, int imm8, int16_t) { return slli_epi16(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, int32_t) { return slli_epi32(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, int64_t) { return slli_epi64(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint16_t) { return slli_epi16(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint32_t) { return slli_epi32(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint64_t) { return slli_epi64(a, imm8); }

__m256i avx::sra_epi16(__m256i a, __m128i b) { return _mm256_sra_epi16(a, b); }
__m256i avx::sra_epi32(__m256i a, __m128i b) { return _mm256_sra_epi32(a, b); }

__m256i avx::sra(__m256i a, __m128i b, int16_t) { return sra_epi16(a, b); }
__m256i avx::sra(__m256i a, __m128i b, int32_t) { return sra_epi32(a, b); }

__m256i avx::srai_epi16(__m256i a, int imm8) { return _mm256_srai_epi16(a, imm8); }
__m256i avx::srai_epi32(__m256i a, int imm8) { return _mm256_srai_epi32(a, imm8); }

__m256i avx::srai(__m256i a, int imm8, int16_t) { return srai_epi16(a, imm8); }
__m256i avx::srai(__m256i a, int imm8, int32_t) { return srai_epi32(a, imm8); }

__m256i avx::stream_load(__m256i const* p) { return _mm256_stream_load_si256(p); }

__m256i avx::srl_epi16(__m256i a, __m128i b) { return _mm256_srl_epi16(a, b); }
__m256i avx::srl_epi32(__m256i a, __m128i b) { return _mm256_srl_epi32(a, b); }
__m256i avx::srl_epi64(__m256i a, __m128i b) { return _mm256_srl_epi64(a, b); }

__m256i avx::srl(__m256i a, __m128i b, int16_t) { return srl_epi16(a, b); }
__m256i avx::srl(__m256i a, __m128i b, int32_t) { return srl_epi32(a, b); }
__m256i avx::srl(__m256i a, __m128i b, int64_t) { return srl_epi64(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint16_t) { return srl_epi16(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint32_t) { return srl_epi32(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint64_t) { return srl_epi64(a, b); }

template<int imm8>
__m256i avx::srli(__m256i a) {
	return _mm256_srli_si256(a, imm8);
}
__m256i avx::srli_epi16(__m256i a, int imm8) { return _mm256_srli_epi16(a, imm8); }
__m256i avx::srli_epi32(__m256i a, int imm8) { return _mm256_srli_epi32(a, imm8); }
__m256i avx::srli_epi64(__m256i a, int imm8) { return _mm256_srli_epi64(a, imm8); }

__m256i avx::srli(__m256i a, int imm8, int16_t) { return srli_epi16(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, int32_t) { return srli_epi32(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, int64_t) { return srli_epi64(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint16_t) { return srli_epi16(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint32_t) { return srli_epi32(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint64_t) { return srli_epi64(a, imm8); }

__m256i avx::sub_epi8(__m256i a, __m256i b) { return _mm256_sub_epi8(a, b); }
__m256i avx::sub_epi16(__m256i a, __m256i b) { return _mm256_sub_epi16(a, b); }
__m256i avx::sub_epi32(__m256i a, __m256i b) { return _mm256_sub_epi32(a, b); }
__m256i avx::sub_epi64(__m256i a, __m256i b) { return _mm256_sub_epi64(a, b); }

__m256i avx::sub(__m256i a, __m256i b, int8_t) { return sub_epi8(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int16_t) { return sub_epi16(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int32_t) { return sub_epi32(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int64_t) { return sub_epi64(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint8_t) { return sub_epi8(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint16_t) { return sub_epi16(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint32_t) { return sub_epi32(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint64_t) { return sub_epi64(a, b); }

__m256i avx::subs_epi8(__m256i a, __m256i b) { return _mm256_subs_epi8(a, b); }
__m256i avx::subs_epi16(__m256i a, __m256i b) { return _mm256_subs_epi16(a, b); }

__m256i avx::subs_epu8(__m256i a, __m256i b) { return _mm256_subs_epu8(a, b); }
__m256i avx::subs_epu16(__m256i a, __m256i b) { return _mm256_subs_epu16(a, b); }

__m256i avx::subs(__m256i a, __m256i b, int8_t) { return subs_epi8(a, b); }
__m256i avx::subs(__m256i a, __m256i b, int16_t) { return subs_epi16(a, b); }
__m256i avx::subs(__m256i a, __m256i b, uint8_t) { return subs_epu8(a, b); }
__m256i avx::subs(__m256i a, __m256i b, uint16_t) { return subs_epu16(a, b); }

int avx::test_all_ones(__m256i a) { return testc(a, cmpeq_epi32(a, a)); }

__m256i avx::unpackhi_epi8(__m256i a, __m256i b) { return _mm256_unpackhi_epi8(a, b); }
__m256i avx::unpackhi_epi16(__m256i a, __m256i b) { return _mm256_unpackhi_epi16(a, b); }
__m256i avx::unpackhi_epi32(__m256i a, __m256i b) { return _mm256_unpackhi_epi32(a, b); }
__m256i avx::unpackhi_epi64(__m256i a, __m256i b) { return _mm256_unpackhi_epi64(a, b); }

__m256i avx::unpackhi(__m256i a, __m256i b, int8_t) { return unpackhi_epi8(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int16_t) { return unpackhi_epi16(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int32_t) { return unpackhi_epi32(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int64_t) { return unpackhi_epi64(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint8_t) { return unpackhi_epi8(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint16_t) { return unpackhi_epi16(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint32_t) { return unpackhi_epi32(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint64_t) { return unpackhi_epi64(a, b); }

__m256i avx::unpacklo_epi8(__m256i a, __m256i b) { return _mm256_unpacklo_epi8(a, b); }
__m256i avx::unpacklo_epi16(__m256i a, __m256i b) { return _mm256_unpacklo_epi16(a, b); }
__m256i avx::unpacklo_epi32(__m256i a, __m256i b) { return _mm256_unpacklo_epi32(a, b); }
__m256i avx::unpacklo_epi64(__m256i a, __m256i b) { return _mm256_unpacklo_epi64(a, b); }

__m256i avx::unpacklo(__m256i a, __m256i b, int8_t) { return unpacklo_epi8(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int16_t) { return unpacklo_epi16(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int32_t) { return unpacklo_epi32(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int64_t) { return unpacklo_epi64(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint8_t) { return unpacklo_epi8(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint16_t) { return unpacklo_epi16(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint32_t) { return unpacklo_epi32(a, b); }

#endif // WJR_AVX2

_WJR_SIMD_END