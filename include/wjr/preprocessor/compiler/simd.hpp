#ifndef WJR_PREPROCESSOR_COMPILER_SIMD_H__
#define WJR_PREPROCESSOR_COMPILER_SIMD_H__

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
/* GCC-compatible compiler, targeting ARM with NEON */
#include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)
/* GCC-compatible compiler, targeting ARM with WMMX */
#include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) &&                               \
    (defined(__VEC__) || defined(__ALTIVEC__))
/* XLC or GCC-compatible compiler, targeting PowerPC with VMX/VSX */
#include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)
/* GCC-compatible compiler, targeting PowerPC with SPE */
#include <spe.h>
#endif

#if defined(__AVX512VL__)
#define WJR_AVX512VL 1
#else
#define WJR_AVX512VL 0
#endif

#if defined(__AVX512BW__)
#define WJR_AVX512BW 1
#else
#define WJR_AVX512BW 0
#endif

#if defined(__AVX512DQ__)
#define WJR_AVX512DQ 1
#else
#define WJR_AVX512DQ 0
#endif

#if defined(__AVX512F__) || (WJR_AVX512VL && WJR_AVX512BW && WJR_AVX512DQ)
#define WJR_AVX512F 1
#else
#define WJR_AVX512F 0
#endif

#if defined(__AVX512__) || (WJR_AVX512F && WJR_AVX512BW && WJR_AVX512DQ)
#define WJR_AVX512 1
#else
#define WJR_AVX512 0
#endif

#if defined(__AVX2__) || (WJR_AVX512 || WJR_AVX512F)
#define WJR_AVX2 1
#else
#define WJR_AVX2 0
#endif

#if defined(__AVX__) || WJR_AVX2
#define WJR_AVX 1
#else
#define WJR_AVX 0
#endif

#if defined(__SSE4_2__) || WJR_AVX
#define WJR_SSE4_2 1
#else
#define WJR_SSE4_2 0
#endif

#if defined(__SSE4_1__) || WJR_SSE4_2
#define WJR_SSE4_1 1
#else
#define WJR_SSE4_1 0
#endif

#if defined(__SSSE3__) || WJR_SSE4_1
#define WJR_SSSE3 1
#else
#define WJR_SSSE3 0
#endif

#if defined(__SSE3__) || WJR_SSSE3
#define WJR_SSE3 1
#else
#define WJR_SSE3 0
#endif

#if defined(__SSE2__) || WJR_SSE3 || _M_IX86_FP >= 2 ||                        \
    (defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_X64)))
#define WJR_SSE2 1
#else
#define WJR_SSE2 0
#endif

#if defined(__SSE__) || WJR_SSE2 || _M_IX86_FP >= 1
#define WJR_SSE 1
#else
#define WJR_SSE 0
#endif

#if defined(__MMX__) || WJR_SSE
#define WJR_MMX 1
#else
#define WJR_MMX 0
#endif

#if defined(__XOP__)
#define WJR_XOP 1
#else
#define WJR_XOP 0
#endif

#endif // ! WJR_PREPROCESSOR_COMPILER_SIMD_H__
