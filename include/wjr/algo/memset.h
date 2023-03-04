#pragma once
#ifndef __WJR_ALGO_MEMSET_H
#define __WJR_ALGO_MEMSET_H

#include <wjr/macro.h>

#if defined(_WJR_FAST_MEMSET)

_WJR_ALGO_BEGIN

extern void __memset(uint8_t*, uint8_t, size_t);
extern void __memset(uint16_t*, uint16_t, size_t);
extern void __memset(uint32_t*, uint32_t, size_t);
#if defined(WJR_X86_64)
extern void __memset(uint64_t*, uint64_t, size_t);
#endif

_WJR_ALGO_END

#endif // _WJR_FAST_MEMSET

#endif // __WJR_ALGO_MEMSET_H