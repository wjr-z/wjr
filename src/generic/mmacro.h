#ifndef __WJR_MMACRO_H
#define __WJR_MMACRO_H

#include <cassert>
#include <cstdio>

#if defined(__clang__) || defined(__GNUC__)
#define WJR_CPP_STANDARD __cplusplus
#elif defined(_MSC_VER)
#define WJR_CPP_STANDARD _MSVC_LANG
#endif

#if WJR_CPP_STANDARD >= 199711L
#define WJR_CPP_03
#endif
#if WJR_CPP_STANDARD >= 201103L
#define WJR_CPP_11
#endif
#if WJR_CPP_STANDARD >= 201402L
#define WJR_CPP_14
#endif
#if WJR_CPP_STANDARD >= 201703L
#define WJR_CPP_17
#endif
#if WJR_CPP_STANDARD >= 202002L
#define WJR_CPP_20
#endif

#if defined(WJR_CPP_20)
#define WJR_CONSTEXPR20 constexpr
#else
#define WJR_CONSTEXPR20 inline
#endif

#if defined(_MSC_VER)
#if !(defined(__AVX__) || defined(__AVX2__))
#if (defined(_M_AMD64) || defined(_M_X64))
#define __SSE2__
#elif _M_IX86_FP == 2
#define __SSE2__
#elif _M_IX86_FP == 1
#define __SSE__
#endif
#endif
#endif

#define __USE_THREADS

#ifndef is_little_endian
#if defined(__BYTE_ORDER__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define is_little_endian true
#else
#define is_little_endian false
#endif
#else
#define is_little_endian true
#endif
#endif

#if defined(__has_builtin)
#define whas_builtin(x) __has_builtin(x)
#else
#define whas_builtin(x) 0
#endif

#if whas_builtin(__builtin_expect) && !defined(WJR_CPP_20)
#ifndef likely
#define likely(expr) __builtin_expect(!!(expr), 1)
#endif
#ifndef unlikely
#define unlikely(expr) __builtin_expect(!!(expr),0)
#endif
#else

#ifndef likely
#define likely(expr) expr
#endif
#ifndef unlikely
#define unlikely(expr) expr
#endif
#endif

#ifdef __USE_THREADS
#define WJR_THREADS (true)
#else
#define WJR_THREADS (false)
#endif

#if WJR_THREADS
#define USE_THREAD_LOCAL thread_local
#else
#define USE_THREAD_LOCAL
#endif

#if defined(__clang__)
#define WJR_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER) 
#define WJR_UNREACHABLE __assume(false)
#else
#define WJR_UNREACHABLE
#endif

#if defined(_MSC_VER)
#define WJR_FORCEINLINE __forceinline
#elif defined(__GUNC__)
#define WJR_FORCEINLINE __inline__ __attribute__((always_inline))
#else
#define WJR_FORCEINLINE
#endif

#if defined(KB)
#undef KB
#endif
#define KB *(size_t(1) << 10)

#if defined(MB)
#undef MB
#endif
#define MB *((size_t)(1) << 20)

#if defined(GB)
#undef GB
#endif
#define GB *((size_t)(1) << 30)

#define CONNECT(A,B) A##B
#define STD_FUNCTION(FUNC) std::FUNC

#ifndef _DEBUG
#define WDEBUG_LEVEL 0
#else
#define WDEBUG_LEVEL 3
#endif

#define WASSERT_LEVEL_MESSAGE(LEVEL,expression)											\
    do{																					\
        if (unlikely(!(expression))) {													\
                fprintf(stderr,"Assertion failed: %s in %s : %d\nBUG_LEVEL : %d",		\
                #expression,__FILE__,__LINE__,LEVEL);									\
                std::abort();															\
        }																				\
    }while(0)

#define WASSERT_LEVEL_0(expression) WASSERT_LEVEL_MESSAGE(0, expression)

// WDEBUG_LEVEL		1
// The impact on program running time or space is only constant
// Some small bugs that are not easy to test under a small part or a small range of data
#if WDEBUG_LEVEL >= 1
#define WASSERT_LEVEL_1(expression)	WASSERT_LEVEL_MESSAGE(1,expression)
#else
#define WASSERT_LEVEL_1(expression)
#endif

// WDEBUG_LEVEL		2
// The impact on program running time or space is only linear or less
// Some bugs that may be found and fixed in small-scale tests
#if WDEBUG_LEVEL >= 2
#define WASSERT_LEVEL_2(expression)	WASSERT_LEVEL_MESSAGE(2,expression)
#else
#define WASSERT_LEVEL_2(expression)
#endif

// WDEBUG_LEVEL		3
// The impact on program running time or space is O(nlogn),O(n sqrt(n)),O(n^2) or less
// It can still run in a large range
#if WDEBUG_LEVEL >= 3
#define WASSERT_LEVEL_3(expression)	WASSERT_LEVEL_MESSAGE(3,expression)
#else
#define WASSERT_LEVEL_3(expression)
#endif

// WDEBUG_LEVEL		4
// no limit
// It can only be tested in a small range
#if WDEBUG_LEVEL >= 4
#define WASSERT_LEVEL_4(expression)	WASSERT_LEVEL_MESSAGE(4,expression)
#else
#define WASSERT_LEVEL_4(expression)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define WJR_GCC_STYLE_ASM
#endif

#if defined(__SSE__)
#include <xmmintrin.h>
#endif

#if defined(__SSE2__)
#include <emmintrin.h>
#endif

#if defined(__SSE3__)
#include <pmmintrin.h>
#endif

#if defined(__SSE4_1__)
#include <smmintrin.h>
#endif

#if defined(__SSE4_2__)
#include <nmmintrin.h>
#endif

#if defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__)
#include <immintrin.h>
#endif

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#endif