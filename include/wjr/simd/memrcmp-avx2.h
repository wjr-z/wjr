#pragma once
#ifndef __WJR_MEMRCMP_AVX2_H
#define __WJR_MEMRCMP_AVX2_H

#include <wjr/macro.h>

#if defined(__AVX2__)

_WJR_SIMD_BEGIN

const uint8_t* avx2_memrcmp(const uint8_t* s0, const uint8_t* s1, size_t n);

_WJR_SIMD_END

#endif // __AVX2__

#endif // __WJR_SIMD_MEMCHR_AVX2_H__