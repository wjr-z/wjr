#ifndef __WJR_MMACRO_H
#define __WJR_MMACRO_H

#include <cassert>
#include <cstdio>
#include <type_traits>

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

#if defined(__has_builtin)
#define WJR_HAS_BUILTIN(x) __has_builtin(x)
#else
#define WJR_HAS_BUILTIN(x) 0
#endif

#if defined(__has_attribute)
#define WJR_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define WJR_HAS_ATTRIBUTE(x) 0
#endif

#if defined(WJR_CPP_20)
#define WJR_IS_CONSTANT_EVALUATED_BEGIN if(std::is_constant_evaluated()) {
#define WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN if(!std::is_constant_evaluated()) {
#define WJR_CONSTEXPR constexpr
#define WJR_CONSTEXPR20 constexpr
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated)
#define WJR_IS_CONSTANT_EVALUATED_BEGIN if(__builtin_is_constant_evaluated()){
#define WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN if(!__builtin_is_constant_evaluated()){
#define WJR_CONSTEXPR constexpr
#define WJR_CONSTEXPR20 inline
#else
#define WJR_IS_CONSTANT_EVALUATED_BEGIN if constexpr (false){
#define WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN if constexpr (true){
#define WJR_CONSTEXPR inline
#define WJR_CONSTEXPR20 inline
#endif

#define WJR_IS_CONSTANT_EVALUATED_END }
#define WJR_IS_NOT_CONSTANT_EVALUATED_END }

#if defined(__clang__)
#define WJR_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER) 
#define WJR_UNREACHABLE __assume(false)
#else
#define WJR_UNREACHABLE
#endif

#if defined(_MSC_VER)
#define WJR_FORCEINLINE __forceinline
#elif WJR_HAS_ATTRIBUTE(always_inline)
#define WJR_FORCEINLINE __inline__ __attribute__((always_inline))
#else
#define WJR_FORCEINLINE inline
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

#if defined(__x86_64__) || (defined(_M_IX86) || (defined(_M_X64) && !defined(_M_ARM64EC)))
#define WJR_X86_64
#elif defined(__arm__) || (defined(_M_ARM) || defined(_M_ARM64))
#define WJR_ARM
#endif

#if SIZE_MAX == 0xffffffffffffffffull
#define WJR_X64
#else
#define WJR_X86
#endif

#if WJR_HAS_BUILTIN(__builtin_expect) && !defined(WJR_CPP_20)
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

#if defined(__USE_THREADS)
#define USE_THREAD_LOCAL thread_local
#else
#define USE_THREAD_LOCAL
#endif

#ifndef _DEBUG
#define WDEBUG_LEVEL 0
#else
#define WDEBUG_LEVEL 3
#endif

#define __WASSERT_LEVEL_MESSAGE(LEVEL,expression, message, ...)						    \
    do{																					\
        if (unlikely(!(expression))) {													\
                fprintf(stderr,"DEBUG_LEVEL : %d\nAssertion failed: %s in %s : %d\n"	\
                "message : %s\n"                                                        \
                ,LEVEL, #expression,__FILE__,__LINE__, message);						\
                std::abort();															\
        }																				\
    }while(0)

#define __MSVC_VA_ARGS_EXPAND(x) x
#define WASSERT_LEVEL_MESSAGE(...) __MSVC_VA_ARGS_EXPAND(__WASSERT_LEVEL_MESSAGE(__VA_ARGS__, "no additional message"))
#define WASSERT_LEVEL_0(...) __MSVC_VA_ARGS_EXPAND(WASSERT_LEVEL_MESSAGE(0, __VA_ARGS__))

// WDEBUG_LEVEL		1
// The impact on program running time or space is only constant
// Some small bugs that are not easy to test under a small part or a small range of data
#if WDEBUG_LEVEL >= 1
#define WASSERT_LEVEL_1(...) __MSVC_VA_ARGS_EXPAND(WASSERT_LEVEL_MESSAGE(1, __VA_ARGS__))
#else
#define WASSERT_LEVEL_1(...)
#endif

// WDEBUG_LEVEL		2
// The impact on program running time or space is only linear or less
// Some bugs that may be found and fixed in small-scale tests
#if WDEBUG_LEVEL >= 2
#define WASSERT_LEVEL_2(...) __MSVC_VA_ARGS_EXPAND(WASSERT_LEVEL_MESSAGE(2, __VA_ARGS__))
#else
#define WASSERT_LEVEL_2(...)
#endif

// WDEBUG_LEVEL		3
// The impact on program running time or space is O(nlogn),O(n sqrt(n)),O(n^2) or less
// It can still run in a large range
#if WDEBUG_LEVEL >= 3
#define WASSERT_LEVEL_3(...) __MSVC_VA_ARGS_EXPAND(WASSERT_LEVEL_MESSAGE(3, __VA_ARGS__))
#else
#define WASSERT_LEVEL_3(...)
#endif

// WDEBUG_LEVEL		4
// no limit
// It can only be tested in a small range
#if WDEBUG_LEVEL >= 4
#define WASSERT_LEVEL_4(...) __MSVC_VA_ARGS_EXPAND(WASSERT_LEVEL_MESSAGE(4, __VA_ARGS__))
#else
#define WASSERT_LEVEL_4(...)
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

#define WJR_USE_LIBDIVIDE

#endif