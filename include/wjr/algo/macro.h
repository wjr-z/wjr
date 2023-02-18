#pragma once
#ifndef __WJR_ALGO_MACRO_H__
#define __WJR_ALGO_MACRO_H__

#include <wjr/simd/simd_intrin.h>
#include <wjr/vectorize.h>

#if defined(__SSE2__)
#define __HAS_FAST_MEMCHR
//#define __HAS_FAST_MEMCMP
//#define __HAS_FAST_MEMMIS
//#define __HAS_FAST_MEMCNT
#define __HAS_FAST_MEMSET
#endif // __SSE2__

_WJR_ALGO_BEGIN
_WJR_ALGO_END

#endif // __WJR_ALGO_MACRO_H__ 