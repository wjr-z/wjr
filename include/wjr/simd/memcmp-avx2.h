#pragma once
#ifndef __WJR_MEMCMP_AVX2_H
#define __WJR_MEMCMP_AVX2_H

#include <wjr/macro.h>

#if defined(__AVX2__)

_WJR_SIMD_BEGIN

const uint8_t* avx2_memcmp(const uint8_t* s0, const uint8_t* s1, size_t n);

_WJR_SIMD_END

#endif // __AVX2__

#endif // __WJR_MEMCMP_AVX2_H