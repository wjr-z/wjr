#pragma once
#ifndef __WJR_MEMCHR_AVX2_H
#define __WJR_MEMCHR_AVX2_H

#include <wjr/macro.h>

#if defined(__AVX2__)

_WJR_SIMD_BEGIN

const uint8_t* avx2_memchr(const uint8_t* s, uint8_t val, size_t n);
const uint16_t* avx2_memchr(const uint16_t* s, uint16_t val, size_t n);
const uint32_t* avx2_memchr(const uint32_t* s, uint32_t val, size_t n);

_WJR_SIMD_END

#endif // __AVX2__

#endif // __WJR_MEMCHR_AVX2_H