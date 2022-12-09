#pragma once
#ifndef __WJR_MMACRO_H
#define __WJR_MMACRO_H

#include <cassert>
#include <cstddef>
#include <type_traits>
#include <cstdint>

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

#ifndef WJR_NODISCARD
#ifndef __has_cpp_attribute
#define WJR_NODISCARD
#elif __has_cpp_attribute(nodiscard) >= 201603L // TRANSITION, VSO#939899 (need toolset update)
#define WJR_NODISCARD [[nodiscard]]
#else
#define WJR_NODISCARD
#endif
#endif

#ifdef WJR_CPP20
#define WJR_EXPLICIT(expression) explicit(expression)
#define WJR_EXPLICIT20(expreesion) explicit(expression)
#else
#define WJR_EXPLICIT(expression) explicit
#define WJR_EXPLICIT20(expression)
#endif

#if defined(_MSC_VER)
#define WJR_FORCEINLINE __forceinline
#elif WJR_HAS_ATTRIBUTE(always_inline)
#define WJR_FORCEINLINE __inline__ __attribute__((always_inline))
#else
#define WJR_FORCEINLINE inline
#endif

#if defined(WJR_CPP_20)
#define WJR_IS_CONSTANT_EVALUATED_BEGIN if(std::is_constant_evaluated()) {
#define WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN if(!std::is_constant_evaluated()) {
#define WJR_CONSTEXPR constexpr
#define WJR_CONSTEXPR20 constexpr
#define WJR_CONSTEVAL20 consteval
#define WJR_CONSTEXPR_FORCEINLINE constexpr WJR_FORCEINLINE
#define WJR_CONSTEXPR20_FORCEINLINE constexpr WJR_FORCEINLINE
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated)
#define WJR_IS_CONSTANT_EVALUATED_BEGIN if(__builtin_is_constant_evaluated()){
#define WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN if(!__builtin_is_constant_evaluated()){
#define WJR_CONSTEXPR constexpr
#define WJR_CONSTEXPR20 inline
#define WJR_CONSTEVAL20 constexpr
#define WJR_CONSTEXPR_FORCEINLINE constexpr WJR_FORCEINLINE
#define WJR_CONSTEXPR20_FORCEINLINE WJR_FORCEINLINE
#else
#define WJR_IS_CONSTANT_EVALUATED_BEGIN if constexpr (false){
#define WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN if constexpr (true){
#define WJR_CONSTEXPR inline
#define WJR_CONSTEXPR20 inline
#define WJR_CONSTEVAL20 constexpr
#define WJR_CONSTEXPR_FORCEINLINE WJR_FORCEINLINE
#define WJR_CONSTEXPR20_FORCEINLINE WJR_FORCEINLINE
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

#define WJR_FALLTHROUGH [[fallthrough]]

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

#if defined(WJR_CPP_20)
#define WJR_LIKELY [[likely]]
#else
#define WJR_LIKELY
#endif

#define WJR_CONCAT(x, y) x##y

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

#define MSVC_VA_ARGS_EXPAND(x) x
#define WASSERT_LEVEL_MESSAGE(...) MSVC_VA_ARGS_EXPAND(__WASSERT_LEVEL_MESSAGE(__VA_ARGS__, "no additional message"))
#define WASSERT_LEVEL_0(...) MSVC_VA_ARGS_EXPAND(WASSERT_LEVEL_MESSAGE(0, __VA_ARGS__))

// WDEBUG_LEVEL		1
// The impact on program running time or space is only constant
// Some small bugs that are not easy to test under a small part or a small range of data
#if WDEBUG_LEVEL >= 1
#define WASSERT_LEVEL_1(...) MSVC_VA_ARGS_EXPAND(WASSERT_LEVEL_MESSAGE(1, __VA_ARGS__))
#else
#define WASSERT_LEVEL_1(...)
#endif

// WDEBUG_LEVEL		2
// The impact on program running time or space is only linear or less
// Some bugs that may be found and fixed in small-scale tests
#if WDEBUG_LEVEL >= 2
#define WASSERT_LEVEL_2(...) MSVC_VA_ARGS_EXPAND(WASSERT_LEVEL_MESSAGE(2, __VA_ARGS__))
#else
#define WASSERT_LEVEL_2(...)
#endif

// WDEBUG_LEVEL		3
// The impact on program running time or space is O(nlogn),O(n sqrt(n)),O(n^2) or less
// It can still run in a large range
#if WDEBUG_LEVEL >= 3
#define WASSERT_LEVEL_3(...) MSVC_VA_ARGS_EXPAND(WASSERT_LEVEL_MESSAGE(3, __VA_ARGS__))
#else
#define WASSERT_LEVEL_3(...)
#endif

// WDEBUG_LEVEL		4
// no limit
// It can only be tested in a small range
#if WDEBUG_LEVEL >= 4
#define WASSERT_LEVEL_4(...) MSVC_VA_ARGS_EXPAND(WASSERT_LEVEL_MESSAGE(4, __VA_ARGS__))
#else
#define WASSERT_LEVEL_4(...)
#endif

#if defined(__GNUC__) || defined(__clang__)
#define WJR_GCC_STYLE_ASM
#endif

#define __AVX512F__INSTRSET 10
#define __AVX512__INSTRSET 10
#define __AVX2__INSTRSET 9
#define __AVX__INSTRSET 8
#define __SSE4_2__INSTRSET 7
#define __SSE4_1__INSTRSET 6
#define __SSSE3__INSTRSET 5
#define __SSE3__INSTRSET 4
#define __SSE2__INSTRSET 3
#define __SSE__INSTRSET 2
#define __MMX__INSTRSET 1
#define __SCALAR__INSTRSET 0

#if defined ( __AVX512VL__ ) && defined ( __AVX512BW__ ) && defined ( __AVX512DQ__ )
#ifndef __AVX512F__
#define __AVX512F__
#endif
#ifndef __AVX512__
#define __AVX512__
#endif
#ifndef WJR_INSTRSET
#define WJR_INSTRSET __AVX512F__INSTRSET
#endif
#endif

#if defined(__AVX512__) || defined(__AVX512F__)
#ifndef __AVX2__
#define __AVX2__
#endif
#ifndef WJR_INSTRSET
#define WJR_INSTRSET __AVX2__INSTRSET
#endif
#endif

#if defined(__AVX2__)
#ifndef __AVX__
#define __AVX__
#endif
#ifndef WJR_INSTRSET
#define WJR_INSTRSET __AVX__INSTRSET
#endif
#endif

#if defined(__AVX__)
#ifndef __SSE4_2__
#define __SSE4_2__
#endif
#ifndef WJR_INSTRSET
#define WJR_INSTRSET __SSE4_2__INSTRSET
#endif
#endif

#if defined(__SSE4_2__)
#ifndef __SSE4_1__
#define __SSE4_1__
#endif
#ifndef WJR_INSTRSET
#define WJR_INSTRSET __SSE4_1__INSTRSET
#endif
#endif

#if defined(__SSE4_1__)
#ifndef __SSSE3__
#define __SSSE3__
#endif
#ifndef WJR_INSTRSET
#define WJR_INSTRSET __SSSE3__INSTRSET
#endif
#endif

#if defined(__SSSE3__)
#ifndef __SSE3__
#define __SSE3__
#endif
#ifndef WJR_INSTRSET
#define WJR_INSTRSET __SSE3__INSTRSET
#endif
#endif

#if defined(__SSE3__)
#ifndef __SSE2__
#define __SSE2__
#endif
#ifndef WJR_INSTRSET
#define WJR_INSTRSET __SSE2__INSTRSET
#endif
#endif

#if defined(__SSE2__)
#ifndef __SSE__
#define __SSE__
#endif
#ifndef WJR_INSTRSET
#define WJR_INSTRSET __SSE__INSTRSET
#endif
#endif

#if defined(__SSE__)
#ifndef __MMX__
#define __MMX__
#endif
#ifndef WJR_INSTRSET
#define WJR_INSTRSET __MMX__INSTRSET
#endif
#endif

#ifndef WJR_INSTRSET
#define WJR_INSTRSET __SCALAR__INSTRSET
#endif

#if defined(__MMX__)
#define WJR_HAS_MMX_DEFINE(X) X
#else
#define WJR_HAS_MMX_DEFINE(X)
#endif // __MMX__

#if defined(__SSE__)
#define WJR_HAS_SSE_DEFINE(X) X
#else
#define WJR_HAS_SSE_DEFINE(X)
#endif // __SSE__

#if defined(__SSE2__)
#define WJR_HAS_SSE2_DEFINE(X) X
#else
#define WJR_HAS_SSE2_DEFINE(X)
#endif // __SSE2__

#if defined(__SSE3__)
#define WJR_HAS_SSE3_DEFINE(X) X
#else
#define WJR_HAS_SSE3_DEFINE(X)
#endif // __SSE3__

#if defined(__SSSE3__)
#define WJR_HAS_SSSE3_DEFINE(X) X
#else
#define WJR_HAS_SSSE3_DEFINE(X)
#endif // __SSSE3__

#if defined(__SSE4_1__)
#define WJR_HAS_SSE4_1_DEFINE(X) X
#else
#define WJR_HAS_SSE4_1_DEFINE(X)
#endif // __SSE4_1__

#if defined(__SSE4_2__)
#define WJR_HAS_SSE4_2_DEFINE(X) X
#else
#define WJR_HAS_SSE4_2_DEFINE(X)
#endif // __SSE4_2__

#if defined(__AVX__)
#define WJR_HAS_AVX_DEFINE(X) X
#else
#define WJR_HAS_AVX_DEFINE(X)
#endif // __AVX__

#if defined(__AVX2__)
#define WJR_HAS_AVX2_DEFINE(X) X
#else
#define WJR_HAS_AVX2_DEFINE(X)
#endif // __AVX2__

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GUNC__) || defined(__clang__)
#include <x86intrin.h>
#else
#if defined(__AVX__)
#include <immintrin.h>
#elif defined(__SSE4_2__)
#include <nmmintrin.h>
#elif defined(__SSE4_1__)
#include <smmintrin.h>
#elif defined(__SSSE3__)
#include <tmmintrin.h>
#elif defined(__SSE3__)
#include <pmmintrin.h>
#elif defined(__SSE2__)
#include <emmintrin.h>
#elif defined(__SSE__)
#include <xmmintrin.h>
#endif
#endif

#define _WJR_BEGIN namespace wjr{
#define _WJR_END }
#define _WJR_SIMD_BEGIN _WJR_BEGIN namespace simd{
#define _WJR_SIMD_END } _WJR_END

#ifndef _WJR_NOEXCEPTION
#define _WJR_TRY try{
#define _WJR_CATCH(x) }catch(x){
#define _WJR_CATCH_ALL }catch(...){
#define _WJR_END_CATCH }
#define _WJR_THROW(x) throw x
#define _WJR_RELEASE throw
#else
#define _WJR_TRY { if constexpr(true){
#define _WJR_CATCH(x) } if constexpr(false){
#define _WJR_CATCH_ALL } if constexpr(false){
#define _WJR_END_CATCH }}
#define _WJR_THROW(x) 
#define _WJR_RELEASE
#endif

#endif