#pragma once
#ifndef __WJR_MEMCMPEQ_SSE2_H
#define __WJR_MEMCMPEQ_SSE2_H

#include <wjr/macro.h>

#if defined(__SSE2__)

_WJR_SIMD_BEGIN

bool sse2_memcmpeq(const uint8_t* s0, const uint8_t* s1, size_t n);

_WJR_SIMD_END

#endif // __SSE2__

#endif // __WJR_MEMCMPEQ_SSE2_H