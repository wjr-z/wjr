#pragma once
#ifndef __WJR_MEMRCMP_SSE2_H
#define __WJR_MEMRCMP_SSE2_H

#include <wjr/macro.h>

#if defined(__SSE2__)

_WJR_SIMD_BEGIN

const uint8_t* sse2_memrcmp(const uint8_t* s0, const uint8_t* s1, size_t n);

_WJR_SIMD_END

#endif

#endif // __WJR_MEMRCMP_SSE2_H