#pragma once
#ifndef __WJR_MMACRO_H
#define __WJR_MMACRO_H

#include <cassert>
#include <cstddef>
#include <type_traits>
#include <cstdint>
#include <utility>

#if defined(__clang__)
#define WJR_COMPILER_CLANG
#elif defined(__GNUC__)
#define WJR_COMPILER_GCC
#elif defined(_MSC_VER)
#define WJR_COMPILER_MSVC
#endif

#if defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC)
#define WJR_CPP_STANDARD __cplusplus
#elif defined(WJR_COMPILER_MSVC)
#define WJR_CPP_STANDARD _MSVC_LANG
#endif

#define WJR_OVERLAP_P(p1, s1, p2, s2) \
	((p1) + (s1) > (p2) && (p2) + (s2) > (p1))

#define WJR_OVERLAP_P_N(p1, p2, n)	\
	WJR_OVERLAP_P(p1, n, p2, n)

#define WJR_SAME_OR_SEPARATE_P(p1, s1, p2, s2)	\
	((p1) == (p2) || !WJR_OVERLAP_P(p1, s1, p2, s2))

#define WJR_SAME_OR_SEPARATE_P_N(p1, p2, n)	\
	WJR_SAME_OR_SEPARATE_P(p1, n, p2, n)

#define WJR_PRE_OR_SEPARATE_P(p1, s1, p2, s2) \
	((p1) <= (p2) || !WJR_OVERLAP_P(p1, s1, p2, s2))

#define WJR_PRE_OR_SEPARATE_P_N(p1, p2, n)	\
	WJR_PRE_OR_SEPARATE_P(p1, n, p2, n)

#define WJR_POST_OR_SEPARATE_P(p1, s1, p2, s2) \
	((p1) >= (p2) || !WJR_OVERLAP_P(p1, s1, p2, s2))

#define WJR_POST_OR_SEPARATE_P_N(p1, p2, n)	\
	WJR_POST_OR_SEPARATE_P(p1, n, p2, n)

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

#if defined(__GNUC__)
#define WJR_HAS_GCC(major, minor, patchlevel)	            \
((__GNUC__ > (major)) ||					                \
	(__GNUC__ == (major) && __GNUC_MINOR__ > (minor)) ||	\
	(__GNUC__ == (major) && __GNUC_MINOR__ == (minor) && __GNUC_PATCHLEVEL__ >= (patchlevel)))
#else
#define WJR_HAS_GCC(major, minor, patchlevel) 0
#endif // __GNUC__

#if defined(__clang__)
#define WJR_HAS_CLANG(major, minor, patchlevel)	                    \
((__clang_major__ > (major)) ||					                    \
	(__clang_major__ == (major) && __clang_minor__ > (minor)) ||	\
	(__clang_major__ == (major) && __clang_minor__ == (minor) && __clang_patchlevel__ >= (patchlevel)))
#else
#define WJR_HAS_CLANG(major, minor, patchlevel) 0
#endif

#if defined(_MSC_VER)
#define WJR_HAS_MSVC(minor, level)	\
(_MSC_VER >= (minor) * 100 + (level))
#else
#define WJR_HAS_MSVC(minor, level) 0
#endif

#if (defined(WJR_COMPILER_GCC) && !WJR_HAS_GCC(7, 1, 0)) ||	\
	(defined(WJR_COMPILER_CLANG) && !WJR_HAS_CLANG(5, 0, 0))	
#error "GCC 7.1.0 or Clang 5.0.0 or later is required"
#endif 

#if (defined(WJR_COMPILER_GCC) && WJR_HAS_GCC(10, 1, 0)) ||	\
	(defined(WJR_COMPILER_CLANG) && WJR_HAS_CLANG(10, 0, 0))	|| \
	(!defined(WJR_COMPILER_GCC) && !defined(WJR_COMPILER_CLANG) && defined(__has_builtin))
#define WJR_HAS_BUILTIN(x) __has_builtin(x)
#else
#define WJR_HAS_BUILTIN(x) 0
#endif 

#if defined(__has_attribute)
#define WJR_HAS_ATTRIBUTE(x) __has_attribute(x)
#elif defined(__has_cpp_attribute)
#define WJR_HAS_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define WJR_HAS_ATTRIBUTE(x) 0
#endif

#if defined(__has_include)
#define WJR_HAS_INCLUDE(x) __has_include(x)
#else
#define WJR_HAS_INCLUDE(x) 0
#endif // __has_include

#if WJR_HAS_INCLUDE(<execution>)
#define WJR_HAS_EXECUTION
#endif // WJR_HAS_INCLUDE(<execution>)

#ifndef WJR_NODISCARD
#ifndef __has_cpp_attribute
#define WJR_NODISCARD
#elif __has_cpp_attribute(nodiscard) >= 201603L // TRANSITION, VSO#939899 (need toolset update)
#define WJR_NODISCARD [[nodiscard]]
#else
#define WJR_NODISCARD
#endif
#endif

#ifdef WJR_CPP_20
#define WJR_EXPLICIT(expression) explicit(expression)
#define WJR_EXPLICIT20(expreesion) explicit(expression)
#else
#define WJR_EXPLICIT(expression) explicit
#define WJR_EXPLICIT20(expression)
#endif

#if defined(_MSC_VER)
#define WJR_FORCEINLINE __forceinline
#elif WJR_HAS_ATTRIBUTE(always_inline)
#define WJR_FORCEINLINE __attribute__((always_inline))
#else
#define WJR_FORCEINLINE
#endif

#if defined(WJR_FORCEINLINE)
#define WJR_INTRINSIC_INLINE inline WJR_FORCEINLINE
#else
#define WJR_INTRINSIC_INLINE inline
#endif

#if defined(WJR_CPP_20)
#define WJR_CONSTEXPR20 constexpr
#define WJR_INTRINSIC_CONSTEXPR20 WJR_INTRINSIC_INLINE constexpr
#define WJR_CONSTEVAL20 consteval
#else
#define WJR_CONSTEXPR20 inline
#define WJR_INTRINSIC_CONSTEXPR20 WJR_INTRINSIC_INLINE
#define WJR_CONSTEVAL20 constexpr
#endif

#define WJR_INTRINSIC_CONSTEXPR WJR_INTRINSIC_INLINE constexpr

#if defined(_MSC_VER)
#if !(defined(__AVX__) || defined(__AVX2__))
#if (defined(_M_AMD64) || defined(_M_X64))
#ifndef __SSE2__
#define __SSE2__
#endif // __SSE2__
#elif _M_IX86_FP == 2
#ifndef __SSE2__
#define __SSE2__
#endif // __SSE2__
#elif _M_IX86_FP == 1
#ifndef __SSE__
#define __SSE__
#endif // __SSE__
#endif
#endif
#endif

#if defined(__x86_64__) || (defined(_M_IX86) || (defined(_M_X64) && !defined(_M_ARM64EC)))
#define WJR_X86_64
#elif defined(__arm__) || (defined(_M_ARM) || defined(_M_ARM64))
#define WJR_ARM
#endif

#if defined(WJR_ARM)
#error "ARM is not supported"
#endif

#if SIZE_MAX == 0xffffffffffffffffull
#define WJR_X64
#else
#define WJR_X86
#endif

#if defined(WJR_X64)
#define WJR_BYTE_WIDTH 8
#else
#define WJR_BYTE_WIDTH 4
#endif 

#define WJR_CONCAT(x, y) x##y
#define WJR_MACRO_CONCAT(x, y) WJR_CONCAT(x, y)

#define WJR_EXPAND(x) x

#define __WJR_DEFINE_0(x)
#define __WJR_DEFINE_1(x) x
#define WJR_DEFINE(x, y) __WJR_DEFINE_##y (x)

#define WJR_STR(x) #x
#define WJR_MACRO_STR(x) WJR_STR(x)

#define WJR_MACRO_NULL(...)

#define WJR_MACRO_LABEL(NAME) __wjr_label_##NAME

// judge if i can use inline asm
#if defined(WJR_X86_64) && (defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG))
#define WJR_INLINE_ASM
#if defined(WJR_COMPILER_GCC)
#define WJR_BETTER_INLINE_ASM
#endif // WJR_COMPILER_GCC
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

#if defined(WJR_COMPILER_MSVC)
#include <intrin.h>
#elif defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG)
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
#define _WJR_ASM_BEGIN _WJR_BEGIN namespace masm{
#define _WJR_ASM_END } _WJR_END
#define _WJR_ALGO_BEGIN _WJR_BEGIN namespace algo{
#define _WJR_ALGO_END } _WJR_END
#define _WJR_MP_BEGIN _WJR_BEGIN namespace mp{
#define _WJR_MP_END } _WJR_END
#define _WJR_LITERALS_BEGIN _WJR_BEGIN namespace literals{
#define _WJR_LITERALS_END } _WJR_END

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