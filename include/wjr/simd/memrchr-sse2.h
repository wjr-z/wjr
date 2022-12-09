#pragma once
#ifndef __WJR_MEMRCHR_SSE2_H
#define __WJR_MEMRCHR_SSE2_H

#include <wjr/macro.h>

#if defined(__SSE2__)

_WJR_SIMD_BEGIN

const uint8_t* sse2_memrchr(const uint8_t* s, uint8_t val, size_t n);
const uint16_t* sse2_memrchr(const uint16_t* s, uint16_t val, size_t n);
const uint32_t* sse2_memrchr(const uint32_t* s, uint32_t val, size_t n);

_WJR_SIMD_END

#endif // __SSE2__

#endif // __WJR_MEMRCHR_SSE2_H