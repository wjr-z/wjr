#include <wjr/simd/simd.h>

#if defined(__AVX2__)
#include <wjr/simd/memchr-avx2.h>
#include <wjr/simd/memrchr-avx2.h>
#include <wjr/simd/memcmp-avx2.h>
#include <wjr/simd/memrcmp-avx2.h>
#include <wjr/simd/memcmpeq-avx2.h>
#endif // __AVX2__

#if defined(__SSE2__)
#include <wjr/simd/memchr-sse2.h>
#include <wjr/simd/memrchr-sse2.h>
#include <wjr/simd/memcmp-sse2.h>
#include <wjr/simd/memrcmp-sse2.h>
#include <wjr/simd/memcmpeq-sse2.h>
#endif // __SSE2__

_WJR_SIMD_BEGIN

alignas(32) const uint32_t pdata::preload_mask[16] = {
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		0		  , 0		  , 0		  , 0		  ,
		0		  , 0		  , 0		  , 0
};

#if defined(__HAS_SIMD_MEMCHR)

template<typename T>
const T* __SIMD_memchr(const T* s, T val, size_t n) {
#if defined(__AVX2__)
	return avx2_memchr(s, val, n);
#else
	return sse2_memchr(s, val, n);
#endif
}

const uint8_t* memchr(const uint8_t* s, uint8_t val, size_t n){
	return __SIMD_memchr(s, val, n);
}

const uint16_t* memchr(const uint16_t* s, uint16_t val, size_t n) {
	return __SIMD_memchr(s, val, n);
}

const uint32_t* memchr(const uint32_t* s, uint32_t val, size_t n) {
	return __SIMD_memchr(s, val, n);
}

template<typename T>
const T* __SIMD_memrchr(const T* s, T val, size_t n) {
#if defined(__AVX2__)
	return avx2_memrchr(s, val, n);
#else
	return sse2_memrchr(s, val, n);
#endif
}

const uint8_t* memrchr(const uint8_t* s, uint8_t val, size_t n) {
	return __SIMD_memrchr(s, val, n);
}

const uint16_t* memrchr(const uint16_t* s, uint16_t val, size_t n) {
	return __SIMD_memrchr(s, val, n);
}

const uint32_t* memrchr(const uint32_t* s, uint32_t val, size_t n) {
	return __SIMD_memrchr(s, val, n);
}

#endif // __HAS_SIMD_MEMCHR

#if defined(__HAS_SIMD_MEMCMP)

const uint8_t* memcmp(const uint8_t* s0, const uint8_t* s1, size_t n) {
#if defined(__AVX2__)
	return avx2_memcmp(s0, s1, n);
#else
	return sse2_memcmp(s0, s1, n);
#endif 
}

const uint8_t* memrcmp(const uint8_t* s0, const uint8_t* s1, size_t n) {
#if defined(__AVX2__)
	return avx2_memrcmp(s0, s1, n);
#else
	return sse2_memrcmp(s0, s1, n);
#endif
}

#endif // __HAS_SIMD_MEMCMP

#if defined(__HAS_SIMD_MEMCMPEQ)

bool memcmpeq(const uint8_t* s0, const uint8_t* s1, size_t n) {
#if defined(__AVX2__)
	return avx2_memcmpeq(s0, s1, n);
#else
	return sse2_memcmpeq(s0, s1, n);
#endif
}

#endif // __HAS_SIMD_MEMCMPEQ

_WJR_SIMD_END