#pragma once
#ifndef __WJR_STRING_H
#define __WJR_STRING_H
#include <array>
#include <string_view>
#include <iostream>
#include <cctype>
#include <locale>
#include <cstring>
#include <charconv>
#include <cmath>
#pragma once
#ifndef __WJR_VECTOR_H
#define __WJR_VECTOR_H
#pragma push_macro("new")
#undef new
#include <stdexcept>
#pragma once
#ifndef __WJR_COMPRESSED_PAIR_H
#define __WJR_COMPRESSED_PAIR_H
#pragma once
#ifndef __WJR_TYPE_TRAITS_H
#define __WJR_TYPE_TRAITS_H
#include <functional>
#include <limits>
#pragma once
#ifndef __WJR_CPUINFO_H
#define __WJR_CPUINFO_H
#pragma once
#ifndef __WJR_MMACRO_H
#define __WJR_MMACRO_H
#include <cstdint>
#include <cassert>
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
#elif (defined(__GNUC__) || defined(__xlC__)) && (defined(__VEC__) || defined(__ALTIVEC__))
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
#if defined(__SSE2__) || WJR_SSE3 || _M_IX86_FP >= 2 || \
(defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_X64)))
#define WJR_SSE2 1
#else
#define WJR_SSE2 0
#endif
#if defined(__SSE__) || WJR_SSE2 ||_M_IX86_FP >= 1
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
#if defined(__pnacl__) || defined(__CLR_VER)
#define WJR_VM
#endif
#if (defined(_M_IX86) || defined(__i386__)) && !defined(WJR_VM)
#define WJR_X86_32
#endif
#if (defined(_M_X64) || defined(__x86_64__)) && !defined(WJR_VM)
#define WJR_X86_64
#endif
#if defined(WJR_X86_32) || defined(WJR_X86_64)
#define WJR_X86
#endif
#if (defined(__arm__) || defined(_M_ARM))
#define WJR_ARM
#endif
#if defined(__aarch64__)
#define WJR_AARCH64
#endif
#if defined(__powerpc64__)
#define WJR_PPC64
#endif
#if !defined(WJR_X86)
#error "ARM is not supported"
#endif
#if defined(__clang__)
#define WJR_COMPILER_CLANG
#elif defined(__GNUC__)
#define WJR_COMPILER_GCC
#elif defined(_MSC_VER)
#define WJR_COMPILER_MSVC
#endif
#if defined(_MSC_VER)
#define WJR_MSVC
#endif // _MSC_VER
#if defined(WJR_X86) && (defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG))
#define WJR_INLINE_ASM
#if defined(WJR_COMPILER_GCC)
#define WJR_BETTER_INLINE_ASM
#endif // WJR_COMPILER_GCC
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
#if defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC)
#define WJR_CPP_STANDARD __cplusplus
#elif defined(WJR_COMPILER_MSVC)
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
#if defined(__cpp_char8_t)
#define WJR_CHAR8_T
#endif // __cpp_char8_t
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
defined(__BIG_ENDIAN__) || \
defined(__ARMEB__) || \
defined(__THUMBEB__) || \
defined(__AARCH64EB__) || \
defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
#define WJR_BIG_ENDIAN
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
defined(__LITTLE_ENDIAN__) || \
defined(__ARMEL__) || \
defined(__THUMBEL__) || \
defined(__AARCH64EL__) || \
defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
#define WJR_LITTLE_ENDIAN
#endif
#if WJR_SSE2
#define _WJR_FAST_MEMCHR
#define _WJR_FAST_MEMEQ
#define _WJR_FAST_MEMMIS
#define _WJR_FAST_MEMSET
#define _WJR_FAST_MEMSKIPW
#endif // WJR_SSE2
#if defined(NWJR_FAST_MEMCHR)
#undef _WJR_FAST_MEMCHR
#endif
#if defined(NWJR_FAST_MEMCMP)
#undef _WJR_FAST_MEMCMP
#endif
#if defined(NWJR_FAST_MEMMIS)
#undef _WJR_FAST_MEMMIS
#endif
#if defined(NWJR_FAST_MEMCNT)
#undef _WJR_FAST_MEMCNT
#endif
#if defined(NWJR_FAST_MEMSET)
#udnef _WJR_FAST_MEMSET
#endif
#if defined(NWJR_FAST_MEMCPY)
#endif
#if defined(WJR_INLINE_ASM) || defined(WJR_COMPILER_MSVC)
#define _WJR_FAST_REP
#endif
#define _WJR_CPUINFO
#if defined(NWJR_CPUINFO)
#undef _WJR_CPUINFO
#endif // NWJR_CPUINFO
#if defined(WJR_X86_64)
#if defined(__i386__) || defined(_M_IX86) || defined(_X86_)
#define CPU_INTEL
#elif defined(_M_AMD64)
#define CPU_AMD
#endif
#else
#define CPU_UNKNOWN
#endif
#if defined(WJR_INTEL)
#define _WJR_ENHANCED_REP
#endif
#if defined(NWJR_ENHANCED_REP)
#undef _WJR_ENHANCED_REP
#endif
#if defined(WJR_MAX_CACHE_SIZE) || defined(_WJR_CPUINFO)
#define _WJR_NON_TEMPORARY
#endif
#if defined(NWJR_NON_TEMPORARY)
#undef _WJR_NON_TEMPORARY
#endif
#if defined(_DEBUG) || defined(WJR_ENABLE_TESTING)
#define _WJR_EXCEPTION
#endif
#if defined(NWJR_EXCEPTION)
#undef _WJR_EXCEPTION
#endif // NWJR_EXCEPTION
#ifdef _WJR_EXCEPTION
#define _WJR_TRY try{
#define _WJR_CATCH(x) }catch(x){
#define _WJR_CATCH_ALL }catch(...){
#define _WJR_END_CATCH }
#define _WJR_THROW(x) throw x
#define _WJR_RELEASE throw
#define WJR_NOEXCEPT
#else
#define _WJR_TRY { if constexpr(true){
#define _WJR_CATCH(x) } if constexpr(false){
#define _WJR_CATCH_ALL } if constexpr(false){
#define _WJR_END_CATCH }}
#define _WJR_THROW(x)
#define _WJR_RELEASE
#define WJR_NOEXCEPT noexcept
#endif
#if (defined(WJR_COMPILER_GCC) && WJR_HAS_GCC(10, 1, 0)) ||	\
(defined(WJR_COMPILER_CLANG) && WJR_HAS_CLANG(10, 0, 0))	|| \
(!defined(WJR_COMPILER_GCC) && !defined(WJR_COMPILER_CLANG) && defined(__has_builtin))
#define WJR_HAS_BUILTIN(x) __has_builtin(x)
#else
#define WJR_HAS_BUILTIN(x) 0
#endif
#if defined(__has_include)
#define WJR_HAS_INCLUDE(x) __has_include(x)
#else
#define WJR_HAS_INCLUDE(x) 0
#endif // __has_include
#if WJR_HAS_INCLUDE(<execution>)
#endif // WJR_HAS_INCLUDE(<execution>)
#if defined(__has_attribute)
#define WJR_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define WJR_HAS_ATTRIBUTE(x) 0
#endif
#if defined(__has_cpp_attribute)
#define WJR_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define WJR_HAS_CPP_ATTRIBUTE(x) 0
#endif
#if WJR_HAS_CPP_ATTRIBUTE(fallthrough)
#define WJR_FALLTHROUGH [[fallthrough]]
#elif WJR_HAS_ATTRIBUTE(fallthrough)
#define WJR_FALLTHROUGH __attribute__((fallthrough))
#elif defined(_MSC_VER) && defined(__fallthrough)
#define WJR_FALLTHROUGH __fallthrough
#else
#define WJR_FALLTHROUGH
#endif
#if WJR_HAS_CPP_ATTRIBUTE(noreturn)
#define WJR_NORETURN [[noreturn]]
#elif WJR_HAS_ATTRIBUTE(noreturn)
#define WJR_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define WJR_NORETURN __declspec(noreturn)
#else
#define WJR_NORETURN
#endif
#if WJR_HAS_CPP_ATTRIBUTE(nodiscard)
#define WJR_NODISCARD [[nodiscard]]
#elif WJR_HAS_ATTRIBUTE(nodiscard)
#define WJR_NODISCARD __attribute__((nodiscard))
#elif defined(_MSC_VER)
#define WJR_NODISCARD _Check_return_
#else
#define WJR_NODISCARD
#endif
#if WJR_HAS_CPP_ATTRIBUTE(deprecated)
#define WJR_DEPRECATED [[deprecated]]
#elif WJR_HAS_ATTRIBUTE(deprecated)
#define WJR_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define WJR_DEPRECATED __declspec(deprecated)
#else
#define WJR_DEPRECATED
#endif
#if WJR_HAS_CPP_ATTRIBUTE(maybe_unused)
#define WJR_MAYBE_UNUSED [[maybe_unused]]
#elif WJR_HAS_ATTRIBUTE(maybe_unused)
#define WJR_MAYBE_UNUSED __attribute__((maybe_unused))
#elif defined(_MSC_VER)
#define WJR_MAYBE_UNUSED
#else
#define WJR_MAYBE_UNUSED
#endif
#if WJR_HAS_ATTRIBUTE(always_inline)
#define WJR_FORCEINLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define WJR_FORCEINLINE __forceinline
#else
#define WJR_FORCEINLINE
#endif
#if WJR_HAS_ATTRIBUTE(noinline)
#define WJR_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define WJR_NOINLINE __declspec(noinline)
#else
#define WJR_NOINLINE
#endif
#if WJR_HAS_ATTRIBUTE(hot)
#define WJR_HOT __attribute__((hot))
#elif defined(_MSC_VER)
#define WJR_HOT
#else
#define WJR_HOT
#endif
#if WJR_HAS_ATTRIBUTE(cold)
#define WJR_COLD __attribute__((cold))
#elif defined(_MSC_VER)
#define WJR_COLD
#else
#define WJR_COLD
#endif
#if defined(__cpp_lib_unreachable)
#define WJR_UNREACHABLE std::unreachable()
#elif WJR_HAS_BUILTIN(__builtin_unreachable) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
#define WJR_UNREACHABLE __builtin_unreachable()
#elif defined(WJR_COMPILER_MSVC)
#define WJR_UNREACHABLE __assume(0)
#else
#define WJR_UNREACHABLE
#endif
#if WJR_HAS_BUILTIN(__builtin_assume)
#define WJR_ASSUME(x) __builtin_assume(x)
#elif defined(WJR_COMPILER_MSVC)
#define WJR_ASSUME(x) __assume(x)
#else
#define WJR_ASSUME(x)	    \
do{                         \
if (!(x)) {	            \
WJR_UNREACHABLE;	\
}	                    \
} while (0)
#endif
#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
#define WJR_LIKELY(expr) __builtin_expect(!!(expr), true)
#else
#define WJR_LIKELY(expr) (expr)
#endif
#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
#define WJR_UNLIKELY(expr) __builtin_expect(!!(expr), false)
#else
#define WJR_UNLIKELY(expr) (expr)
#endif
#ifdef WJR_CPP_20
#define WJR_EXPLICIT(expression) explicit(expression)
#define WJR_EXPLICIT20(expreesion) explicit(expression)
#else
#define WJR_EXPLICIT(expression) explicit
#define WJR_EXPLICIT20(expression)
#endif
#if defined(WJR_FORCEINLINE)
#define WJR_INTRINSIC_INLINE inline WJR_FORCEINLINE
#else
#define WJR_INTRINSIC_INLINE inline
#endif
#if WJR_HAS_ATTRIBUTE(pure)
#define WJR_PURE __attribute__((pure))
#else
#define WJR_PURE
#endif
#if WJR_HAS_ATTRIBUTE(const)
#define WJR_CONST __attribute__((const))
#else
#define WJR_CONST
#endif
#if WJR_HAS_ATTRIBUTE(malloc)
#define WJR_MALLOC __attribute__((malloc))
#else
#define WJR_MALLOC
#endif
#define WJR_INLINE inline
#define WJR_CONSTEXPR constexpr
#if defined(__cpp_lib_is_constant_evaluated) || WJR_HAS_BUILTIN(__builtin_is_constant_evaluated) \
|| WJR_HAS_GCC(9,1,0) || WJR_HAS_CLANG(9,0,0)
#define WJR_ENABLE_CONSTEXPR 1
#else
#define WJR_ENABLE_CONSTEXPR 0
#endif
#if WJR_ENABLE_CONSTEXPR
#define WJR_E_CONSTEXPR constexpr
#else
#define WJR_E_CONSTEXPR
#endif
#if defined(WJR_CPP_20)
#define WJR_CONSTEXPR20 constexpr
#else
#define WJR_CONSTEXPR20
#endif
#define WJR_INTRINSIC_CONSTEXPR WJR_INTRINSIC_INLINE constexpr
#define WJR_INTRINSIC_CONSTEXPR20 WJR_INTRINSIC_INLINE WJR_CONSTEXPR20
#define WJR_INTRINSIC_E_CONSTEXPR WJR_INTRINSIC_INLINE WJR_E_CONSTEXPR
#define WJR_INLINE_CONSTEXPR inline constexpr
#define WJR_INLINE_CONSTEXPR20 inline WJR_CONSTEXPR20
#define WJR_INLINE_E_CONSTEXPR inline WJR_E_CONSTEXPR
#define _WJR_ATTRIBUTE(x) WJR_##x
#define WJR_ATTRIBUTE(...) WJR_EXPAND(WJR_MACRO_CALL(_WJR_ATTRIBUTE, , __VA_ARGS__))
#if defined(__SIZEOF_INT128__)
#define WJR_HAS_INT128 1
#if !(defined(__clang__) && defined(LIBDIVIDE_VC))
#define WJR_HAS_INT128_DIV 1
#endif
#else
#define WJR_HAS_INT128 0
#endif
#define _WJR_BEGIN namespace wjr{
#define _WJR_END }
#define _WJR_SIMD_BEGIN _WJR_BEGIN namespace simd{
#define _WJR_SIMD_END } _WJR_END
#define _WJR_ASM_BEGIN _WJR_BEGIN namespace masm{
#define _WJR_ASM_END } _WJR_END
#define _WJR_ALGO_BEGIN _WJR_BEGIN namespace algo{
#define _WJR_ALGO_END } _WJR_END
#define _WJR_RANGES_BEGIN _WJR_BEGIN namespace ranges{
#define _WJR_RANGES_END } _WJR_END
#define _WJR_LITERALS_BEGIN _WJR_BEGIN namespace literals{
#define _WJR_LITERALS_END } _WJR_END
#define _WJR_ENCODE_BEGIN _WJR_BEGIN namespace encode{
#define _WJR_ENCODE_END } _WJR_END
#define _WJR_PARSE_BEGIN _WJR_BEGIN namespace parse{
#define _WJR_PARSE_END } _WJR_END
#define WJR_MACRO_NULL(...)
#define WJR_MACRO_LABEL(NAME) __wjr_label_##NAME
#if defined(WJR_MSVC)
#define WJR_COUNTER __COUNTER__
#else
#define WJR_COUNTER __LINE__
#endif
#define _WJR_CONCAT(x, y) x##y
#define WJR_MACRO_CONCAT(x, y) _WJR_CONCAT(x, y)
#define WJR_EXPAND(x) x
#define __WJR_DEFINE_0(x)
#define __WJR_DEFINE_1(x) x
#define WJR_DEFINE(x, y) __WJR_DEFINE_##y (x)
#define _WJR_STR(x) #x
#define WJR_MACRO_STR(x) _WJR_STR(x)
#define WJR_MACRO_RP(M, x) M x
#define _WJR_MACRO_ESC(...) __VA_ARGS__
#define WJR_MACRO_ESC(x) WJR_MACRO_RP(_WJR_MACRO_ESC, x)
#define WJR_MACRO_IOTA(N) WJR_MACRO_IOTA_##N
#define WJR_MACRO_IOTA_0
#define WJR_MACRO_IOTA_1 0
#define WJR_MACRO_IOTA_2 WJR_MACRO_IOTA_1, 1
#define WJR_MACRO_IOTA_3 WJR_MACRO_IOTA_2, 2
#define WJR_MACRO_IOTA_4 WJR_MACRO_IOTA_3, 3
#define WJR_MACRO_IOTA_5 WJR_MACRO_IOTA_4, 4
#define WJR_MACRO_IOTA_6 WJR_MACRO_IOTA_5, 5
#define WJR_MACRO_IOTA_7 WJR_MACRO_IOTA_6, 6
#define WJR_MACRO_IOTA_8 WJR_MACRO_IOTA_7, 7
#define WJR_MACRO_IOTA_9 WJR_MACRO_IOTA_8, 8
#define WJR_MACRO_IOTA_10 WJR_MACRO_IOTA_9, 9
#define WJR_MACRO_IOTA_11 WJR_MACRO_IOTA_10, 10
#define WJR_MACRO_IOTA_12 WJR_MACRO_IOTA_11, 11
#define WJR_MACRO_IOTA_13 WJR_MACRO_IOTA_12, 12
#define WJR_MACRO_IOTA_14 WJR_MACRO_IOTA_13, 13
#define WJR_MACRO_IOTA_15 WJR_MACRO_IOTA_14, 14
#define WJR_MACRO_IOTA_16 WJR_MACRO_IOTA_15, 15
#define WJR_MACRO_IOTA_17 WJR_MACRO_IOTA_16, 16
#define WJR_MACRO_IOTA_18 WJR_MACRO_IOTA_17, 17
#define WJR_MACRO_IOTA_19 WJR_MACRO_IOTA_18, 18
#define WJR_MACRO_IOTA_20 WJR_MACRO_IOTA_19, 19
#define WJR_MACRO_IOTA_21 WJR_MACRO_IOTA_20, 20
#define WJR_MACRO_IOTA_22 WJR_MACRO_IOTA_21, 21
#define WJR_MACRO_IOTA_23 WJR_MACRO_IOTA_22, 22
#define WJR_MACRO_IOTA_24 WJR_MACRO_IOTA_23, 23
#define WJR_MACRO_IOTA_25 WJR_MACRO_IOTA_24, 24
#define WJR_MACRO_IOTA_26 WJR_MACRO_IOTA_25, 25
#define WJR_MACRO_IOTA_27 WJR_MACRO_IOTA_26, 26
#define WJR_MACRO_IOTA_28 WJR_MACRO_IOTA_27, 27
#define WJR_MACRO_IOTA_29 WJR_MACRO_IOTA_28, 28
#define WJR_MACRO_IOTA_30 WJR_MACRO_IOTA_29, 29
#define WJR_MACRO_IOTA_31 WJR_MACRO_IOTA_30, 30
#define WJR_MACRO_IOTA_32 WJR_MACRO_IOTA_31, 31
#define WJR_MACRO_IOTA_33 WJR_MACRO_IOTA_32, 32
#define WJR_MACRO_IOTA_34 WJR_MACRO_IOTA_33, 33
#define WJR_MACRO_IOTA_35 WJR_MACRO_IOTA_34, 34
#define WJR_MACRO_IOTA_36 WJR_MACRO_IOTA_35, 35
#define WJR_MACRO_IOTA_37 WJR_MACRO_IOTA_36, 36
#define WJR_MACRO_IOTA_38 WJR_MACRO_IOTA_37, 37
#define WJR_MACRO_IOTA_39 WJR_MACRO_IOTA_38, 38
#define WJR_MACRO_IOTA_40 WJR_MACRO_IOTA_39, 39
#define WJR_MACRO_IOTA_41 WJR_MACRO_IOTA_40, 40
#define WJR_MACRO_IOTA_42 WJR_MACRO_IOTA_41, 41
#define WJR_MACRO_IOTA_43 WJR_MACRO_IOTA_42, 42
#define WJR_MACRO_IOTA_44 WJR_MACRO_IOTA_43, 43
#define WJR_MACRO_IOTA_45 WJR_MACRO_IOTA_44, 44
#define WJR_MACRO_IOTA_46 WJR_MACRO_IOTA_45, 45
#define WJR_MACRO_IOTA_47 WJR_MACRO_IOTA_46, 46
#define WJR_MACRO_IOTA_48 WJR_MACRO_IOTA_47, 47
#define WJR_MACRO_IOTA_49 WJR_MACRO_IOTA_48, 48
#define WJR_MACRO_IOTA_50 WJR_MACRO_IOTA_49, 49
#define WJR_MACRO_IOTA_51 WJR_MACRO_IOTA_50, 50
#define WJR_MACRO_IOTA_52 WJR_MACRO_IOTA_51, 51
#define WJR_MACRO_IOTA_53 WJR_MACRO_IOTA_52, 52
#define WJR_MACRO_IOTA_54 WJR_MACRO_IOTA_53, 53
#define WJR_MACRO_IOTA_55 WJR_MACRO_IOTA_54, 54
#define WJR_MACRO_IOTA_56 WJR_MACRO_IOTA_55, 55
#define WJR_MACRO_IOTA_57 WJR_MACRO_IOTA_56, 56
#define WJR_MACRO_IOTA_58 WJR_MACRO_IOTA_57, 57
#define WJR_MACRO_IOTA_59 WJR_MACRO_IOTA_58, 58
#define WJR_MACRO_IOTA_60 WJR_MACRO_IOTA_59, 59
#define WJR_MACRO_IOTA_61 WJR_MACRO_IOTA_60, 60
#define WJR_MACRO_IOTA_62 WJR_MACRO_IOTA_61, 61
#define WJR_MACRO_IOTA_63 WJR_MACRO_IOTA_62, 62
#define WJR_MACRO_IOTA_64 WJR_MACRO_IOTA_63, 63
#define WJR_MACRO_ARG_LEN(...) WJR_EXPAND(WJR_MACRO_ARG_LEN_(0, ##__VA_ARGS__ , WJR_MACRO_ARG_LEN_RSEQ_N()))
#define WJR_MACRO_ARG_LEN_RSEQ_N()                                                   \
64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48,                  \
47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32,                      \
31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,                      \
15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define WJR_MACRO_ARG_LEN_(...) WJR_EXPAND(WJR_MACRO_ARG_LEN_N(__VA_ARGS__))
#define WJR_MACRO_ARG_LEN_N(	                                                     \
_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16,               \
_17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32,      \
_33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,      \
_49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, \
N, ...) N
#define WJR_MACRO_CALL(Func, OP, ...) WJR_EXPAND(WJR_MACRO_CALL_(Func, OP, WJR_MACRO_ARG_LEN(__VA_ARGS__), __VA_ARGS__))
#define WJR_MACRO_CALL_(Func, OP, N, ...) WJR_EXPAND(WJR_MACRO_CALL_N(Func, OP, N, __VA_ARGS__))
#define WJR_MACRO_CALL_N(Func, OP, N, ...) WJR_EXPAND(WJR_MACRO_CALL_##N(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_1(Func, OP, x) Func(x)
#define WJR_MACRO_CALL_2(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_1(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_3(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_2(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_4(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_3(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_5(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_4(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_6(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_5(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_7(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_6(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_8(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_7(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_9(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_8(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_10(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_9(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_11(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_10(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_12(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_11(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_13(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_12(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_14(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_13(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_15(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_14(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_16(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_15(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_17(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_16(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_18(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_17(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_19(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_18(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_20(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_19(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_21(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_20(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_22(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_21(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_23(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_22(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_24(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_23(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_25(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_24(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_26(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_25(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_27(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_26(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_28(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_27(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_29(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_28(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_30(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_29(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_31(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_30(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_32(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_31(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_33(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_32(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_34(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_33(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_35(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_34(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_36(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_35(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_37(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_36(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_38(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_37(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_39(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_38(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_40(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_39(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_41(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_40(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_42(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_41(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_43(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_42(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_44(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_43(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_45(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_44(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_46(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_45(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_47(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_46(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_48(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_47(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_49(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_48(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_50(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_49(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_51(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_50(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_52(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_51(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_53(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_52(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_54(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_53(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_55(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_54(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_56(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_55(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_57(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_56(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_58(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_57(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_59(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_58(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_60(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_59(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_61(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_60(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_62(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_61(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_63(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_62(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_64(Func, OP, x, ...) Func(x) OP WJR_EXPAND(WJR_MACRO_CALL_63(Func, OP, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA(Func,  ...) WJR_EXPAND(WJR_MACRO_CALL_COMMA_(Func,  WJR_MACRO_ARG_LEN(__VA_ARGS__), __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_(Func,  N, ...) WJR_EXPAND(WJR_MACRO_CALL_COMMA_N(Func,  N, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_N(Func,  N, ...) WJR_EXPAND(WJR_MACRO_CALL_COMMA_##N(Func,  __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_1(Func,  x) Func(x)
#define WJR_MACRO_CALL_COMMA_2(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_1(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_3(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_2(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_4(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_3(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_5(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_4(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_6(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_5(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_7(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_6(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_8(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_7(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_9(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_8(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_10(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_9(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_11(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_10(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_12(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_11(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_13(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_12(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_14(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_13(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_15(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_14(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_16(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_15(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_17(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_16(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_18(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_17(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_19(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_18(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_20(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_19(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_21(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_20(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_22(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_21(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_23(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_22(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_24(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_23(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_25(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_24(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_26(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_25(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_27(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_26(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_28(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_27(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_29(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_28(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_30(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_29(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_31(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_30(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_32(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_31(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_33(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_32(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_34(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_33(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_35(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_34(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_36(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_35(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_37(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_36(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_38(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_37(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_39(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_38(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_40(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_39(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_41(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_40(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_42(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_41(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_43(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_42(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_44(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_43(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_45(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_44(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_46(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_45(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_47(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_46(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_48(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_47(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_49(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_48(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_50(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_49(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_51(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_50(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_52(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_51(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_53(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_52(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_54(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_53(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_55(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_54(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_56(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_55(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_57(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_56(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_58(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_57(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_59(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_58(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_60(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_59(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_61(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_60(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_62(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_61(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_63(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_62(Func, __VA_ARGS__))
#define WJR_MACRO_CALL_COMMA_64(Func, x, ...) Func(x) , WJR_EXPAND(WJR_MACRO_CALL_COMMA_63(Func, __VA_ARGS__))
#define WJR_MACRO_PREFIX(N, ...) WJR_EXPAND(WJR_MACRO_CALL_COMMA_N(WJR_EXPAND, N, __VA_ARGS__))
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
#define WJR_REGISTER_HAS_MEMBER_FUNCTION(FUNC, NAME)							\
template<typename Enable, typename T, typename...Args>							\
struct __has_member_function_##NAME : std::false_type {};						\
template<typename T, typename...Args>											\
struct __has_member_function_##NAME <std::void_t<decltype(						\
std::declval<T>(). FUNC (std::declval<Args>()...))>, T, Args...>			\
: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_member_function_##NAME :												\
__has_member_function_##NAME<void, T, Args...> {};							\
template<typename T, typename...Args>											\
constexpr bool has_member_function_##NAME##_v =									\
has_member_function_##NAME<T, Args...>::value;
#define WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(FUNC, NAME)						\
template<typename Enable, typename T, typename...Args>							\
struct __has_static_member_function_##NAME : std::false_type {};				\
template<typename T, typename...Args>											\
struct __has_static_member_function_##NAME <std::void_t<decltype(				\
T:: FUNC (std::declval<Args>()...))>, T, Args...>							\
: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_static_member_function_##NAME :										\
__has_static_member_function_##NAME<void, T, Args...> {};					\
template<typename T, typename...Args>											\
constexpr bool has_static_member_function_##NAME##_v =							\
has_static_member_function_##NAME<T, Args...>::value;
#define WJR_REGISTER_HAS_GLOBAL_FUNCTION(FUNC, NAME)							\
template<typename Enable, typename...Args>										\
struct __has_global_function_##NAME : std::false_type {};						\
template<typename...Args>														\
struct __has_global_function_##NAME <std::void_t<decltype(						\
FUNC (std::declval<Args>()...))>, Args...>									\
: std::true_type {};														\
template<typename...Args>														\
struct has_global_function_##NAME :												\
__has_global_function_##NAME<void, Args...> {};								\
template<typename...Args>														\
constexpr bool has_global_function_##NAME##_v =									\
has_global_function_##NAME<Args...>::value;
#define WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(OP, NAME)						\
template<typename Enable, typename T, typename U>								\
struct __has_global_binary_operator_##NAME : std::false_type {};				\
template<typename T, typename U>												\
struct __has_global_binary_operator_##NAME <std::void_t<decltype(				\
std::declval<T>() OP std::declval<U>())>, T, U>								\
: std::true_type {};														\
template<typename T, typename U>												\
struct has_global_binary_operator_##NAME :										\
__has_global_binary_operator_##NAME<void, T, U> {};							\
template<typename T, typename U>												\
constexpr bool has_global_binary_operator_##NAME##_v =							\
has_global_binary_operator_##NAME<T, U>::value;								\
template<typename T, typename U>												\
struct has_global_binary_operator<T, U, std:: NAME<>> :							\
has_global_binary_operator_##NAME<T, U> {};
#define WJR_REGISTER_HAS_GLOBAL_UNARY_OPERATOR(OP, NAME)						\
template<typename Enable, typename T>											\
struct __has_global_unary_operator_##NAME : std::false_type {};					\
template<typename T>															\
struct __has_global_unary_operator_##NAME <std::void_t<decltype(				\
OP std::declval<T>())>, T>													\
: std::true_type {};														\
template<typename T>															\
struct has_global_unary_operator_##NAME :										\
__has_global_unary_operator_##NAME<void, T> {};								\
template<typename T>															\
constexpr bool has_global_unary_operator_##NAME##_v =							\
has_global_unary_operator_##NAME<T>::value;
#define WJR_REGISTER_HAS_STD_INVOKE												\
template<typename Enable, typename T, typename...Args>							\
struct __has_std_invoke : std::false_type {};					                \
template<typename T, typename...Args>											\
struct __has_std_invoke <std::void_t<std::invoke_result_t<			            \
T, Args...>>, T, Args...>													\
: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_std_invoke :											                \
__has_std_invoke<void, T, Args...> {};						                \
template<typename T, typename...Args>											\
constexpr bool has_std_invoke##_v =								                \
has_std_invoke<T, Args...>::value;
#define WJR_REGISTER_HAS_TYPE(TYPE, NAME)										\
template<typename Enable, typename T>							                \
struct __has_type_##NAME : std::false_type {};				                    \
template<typename T>											                \
struct __has_type_##NAME <std::void_t<decltype(				                    \
std::declval<typename T::TYPE>())>, T>							            \
: std::true_type {};														\
template<typename T>											                \
struct has_type_##NAME :										                \
__has_static_member_function_##NAME<void, T> {};					        \
template<typename T>											                \
constexpr bool has_type_##NAME##_v =							                \
has_type_##NAME<T>::value;
#define WJR_REGISTER_HAS_TYPE_ARGS(TYPE, NAME)									\
template<typename Enable, typename T, typename...Args>							\
struct __has_type_##NAME : std::false_type {};				                    \
template<typename T, typename...Args>											\
struct __has_type_##NAME <std::void_t<decltype(				                    \
std::declval<typename T::template TYPE<Args...>>())>, T, Args...>			\
: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_type_##NAME :										                \
__has_type_##NAME<void, T, Args...> {};					                    \
template<typename T, typename...Args>											\
constexpr bool has_type_##NAME##_v =							                \
has_type_##NAME<T, Args...>::value;
#endif

#if defined(_WJR_CPUINFO)
#include <cpuinfo_x86.h>
#endif // _WJR_CPUINFO
_WJR_BEGIN
#if defined(_WJR_CPUINFO)
extern const cpu_features::X86Info cpuinfo;
extern const cpu_features::CacheInfo cacheinfo;
extern const cpu_features::X86Microarchitecture microarchitecture;
extern const size_t __max_cache_size;
extern const bool __is_intel;
extern const bool __is_amd;
inline bool is_sse() {
return WJR_SSE || cpuinfo.features.sse;
}
inline bool is_sse2() {
return WJR_SSE2 || cpuinfo.features.sse2;
}
inline bool is_sse3() {
return WJR_SSE3 || cpuinfo.features.sse3;
}
inline bool is_ssse3() {
return WJR_SSSE3 || cpuinfo.features.ssse3;
}
inline bool is_sse4_1() {
return WJR_SSE4_1 || cpuinfo.features.sse4_1;
}
inline bool is_sse4_2() {
return WJR_SSE4_2 || cpuinfo.features.sse4_2;
}
inline bool is_avx() {
return WJR_AVX || cpuinfo.features.avx;
}
inline bool is_avx2() {
return WJR_AVX2 || cpuinfo.features.avx2;
}
inline bool is_popcnt() {
return cpuinfo.features.popcnt;
}
#endif // _WJR_CPUINFO
#if defined(_WJR_NON_TEMPORARY)
inline size_t max_cache_size() {
#if defined(WJR_MAX_CACHE_SIZE)
return WJR_MAX_CACHE_SIZE;
#else
return __max_cache_size;
#endif
}
#endif // _WJR_NON_TEMPORARY
inline bool is_intel() {
#if defined(WJR_INTEL)
return true;
#elif defined(_WJR_CPUINFO)
return __is_intel;
#else
return false;
#endif
}
inline bool is_amd() {
#if defined(WJR_AMD)
return true;
#elif defined(_WJR_CPUINFO)
return __is_amd;
#else
return false;
#endif
}
inline bool is_enhanced_rep() {
#if defined(_WJR_ENHANCED_REP)
return true;
#else
return is_intel();
#endif
}
_WJR_END
#endif // __WJR_CPUINFO_H
#include <stdio.h>
_WJR_BEGIN
#if defined(_WJR_CPUINFO)
const cpu_features::X86Info cpuinfo = cpu_features::GetX86Info();
const cpu_features::CacheInfo cacheinfo = cpu_features::GetX86CacheInfo();
const cpu_features::X86Microarchitecture microarchitecture = cpu_features::GetX86Microarchitecture(&cpuinfo);
const size_t __max_cache_size = []() {
size_t max_size = 0;
for (int i = 0; i < cacheinfo.size; ++i) {
if (cacheinfo.levels[i].cache_type != cpu_features::CacheType::CPU_FEATURE_CACHE_NULL) {
max_size = max_size < cacheinfo.levels[i].cache_size ?
cacheinfo.levels[i].cache_size : max_size;
}
}
return max_size;
}();
const bool __is_intel = []() {
return microarchitecture >= cpu_features::INTEL_80486 && microarchitecture <= cpu_features::INTEL_NETBURST;
}();
const bool __is_amd = []() {
return microarchitecture >= cpu_features::AMD_HAMMER && microarchitecture <= cpu_features::AMD_ZEN3;
}();
#endif // _WJR_CPUINFO
_WJR_END


namespace std {
template <typename _Ty = void>
struct plus_assign {
WJR_NODISCARD constexpr _Ty& operator()(_Ty& _Left, const _Ty& _Right) const {
return _Left += _Right;
}
};
template <>
struct plus_assign<> {
template<typename _Ty1, typename _Ty2>
WJR_NODISCARD constexpr decltype(auto) operator()(_Ty1& _Left, const _Ty2& _Right) const {
return _Left += _Right;
}
};
template <typename _Ty = void>
struct minus_assign {
WJR_NODISCARD constexpr _Ty& operator()(_Ty& _Left, const _Ty& _Right) const {
return _Left -= _Right;
}
};
template <>
struct minus_assign<> {
template<typename _Ty1, typename _Ty2>
WJR_NODISCARD constexpr decltype(auto) operator()(_Ty1& _Left, const _Ty2& _Right) const {
return _Left -= _Right;
}
};
}
_WJR_BEGIN
WJR_INTRINSIC_CONSTEXPR bool is_constant_evaluated() noexcept {
#if defined(__cpp_lib_is_constant_evaluated)
return std::is_constant_evaluated();
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated) || WJR_HAS_GCC(9,1,0) || WJR_HAS_CLANG(9,0,0)
return __builtin_is_constant_evaluated();
#else
return false;
#endif
}
template<typename T>
WJR_INTRINSIC_CONSTEXPR bool is_constant_p(T x) noexcept {
#if WJR_HAS_BUILTIN(__builtin_constant_p) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
return __builtin_constant_p(x);
#else
(void)(x);
return is_constant_evaluated();
#endif
}
template<typename T>
WJR_INTRINSIC_CONSTEXPR void assume_no_add_overflow(T a, T b) noexcept {
constexpr T _Max = std::numeric_limits<T>::max();
WJR_ASSUME(a <= _Max - b);
}
template<typename T>
WJR_INTRINSIC_CONSTEXPR void assume_no_sub_overflow(T a, T b) noexcept {
WJR_ASSUME(a >= b);
}
struct disable_tag {};
struct default_construct_tag {};
struct value_construct_tag {};
struct extend_tag {};
struct reserve_tag {};
struct defer_tag {};
struct adopt_tag {};
struct unreachable_tag {};
template<typename T, typename U, typename _Pred>
struct has_global_binary_operator : std::false_type {};
template<typename T, typename U, typename _Pred>
inline constexpr bool has_global_binary_operator_v = has_global_binary_operator<T, U, _Pred>::value;
WJR_REGISTER_HAS_MEMBER_FUNCTION(operator[], subscript_operator);
WJR_REGISTER_HAS_MEMBER_FUNCTION(operator->, point_to_operator);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(+, plus);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(+=, plus_assign);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(-, minus);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(-=, minus_assign);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(&, bit_and);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(| , bit_or);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(^, bit_xor);
WJR_REGISTER_HAS_GLOBAL_UNARY_OPERATOR(~, bit_not);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(== , equal_to);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(!= , not_equal_to);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(> , greater);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(>= , greater_equal);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(< , less);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(<= , less_equal);
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(min, min);
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(max, max);
template<typename T>
using aligned_storage_for_t = std::aligned_storage_t<sizeof(T), alignof(T)>;
template<typename T, typename...Args>
struct is_any_of : std::disjunction<std::is_same<T, Args>...> {};
template<typename T, typename...Args>
inline constexpr bool is_any_of_v = is_any_of<T, Args...>::value;
template<size_t i, size_t..._Index>
struct is_any_index_of : std::disjunction<std::bool_constant<i == _Index>...> {};
template<size_t i, size_t..._Index>
inline constexpr bool is_any_index_of_v = is_any_index_of<i, _Index...>::value;
template<typename T>
using remove_ref_t = std::remove_reference_t<T>;
template<typename T>
using remove_cref_t = std::remove_const_t<remove_ref_t<T>>;
template<typename T>
using remove_cvref_t = std::remove_cv_t<remove_ref_t<T>>;
#define WJR_PRIMITIVE_TYPE(x) ::wjr::remove_cvref_t<decltype(x)>
template<typename T>
using add_lref_t = std::add_lvalue_reference_t<T>;
template<typename T>
using add_rref_t = std::add_rvalue_reference_t<T>;
template<typename T>
using add_cvref_t = add_lref_t<std::add_cv_t<T>>;
template<typename T>
using add_cref_t = add_lref_t<std::add_const_t<T>>;
template<typename T>
struct is_in_place_type : std::false_type {};
template<template<typename...>typename C, typename T>
struct is_in_place_type<C<T>> : std::is_same<C<T>, std::in_place_type_t<T>> {};
template<typename T>
inline constexpr bool is_in_place_type_v = is_in_place_type<T>::value;
template<typename T>
struct is_in_place_index : std::false_type {};
template<template<size_t...>typename C, size_t I>
struct is_in_place_index<C<I>> : std::is_same<C<I>, std::in_place_index_t<I>> {};
template<typename T>
inline constexpr bool is_in_place_index_v = is_in_place_index<T>::value;
template<typename T>
struct is_in_place : std::disjunction<is_in_place_type<T>, is_in_place_index<T>> {};
template<typename T>
inline constexpr bool is_in_place_v = is_in_place<T>::value;
template<typename T>
struct is_standard_comparator :
is_any_of<remove_cvref_t<T>,
std::less<>,
std::less_equal<>,
std::equal_to<>,
std::not_equal_to<>,
std::greater<>,
std::greater_equal<>
> {};
template<typename T>
inline constexpr bool is_standard_comparator_v = is_standard_comparator<T>::value;
template<typename T>
struct unrefwrap {
using type = T;
};
template<typename T>
struct unrefwrap<std::reference_wrapper<T>> {
using type = T&;
};
template<typename T>
using unrefwrap_t = typename unrefwrap<T>::type;
template<typename T>
using iter_cat_t = typename std::iterator_traits<T>::iterator_category;
template<typename T>
using iter_diff_t = typename std::iterator_traits<T>::difference_type;
template<typename T>
using iter_val_t = typename std::iterator_traits<T>::value_type;
template<typename T>
using iter_ref_t = typename std::iterator_traits<T>::reference;
template<typename T, typename = void>
struct is_iterator : std::false_type {};
template<typename T>
struct is_iterator<T, std::void_t<iter_cat_t<T>>> : std::true_type {};
template<typename T>
inline constexpr bool is_iterator_v = is_iterator<T>::value;
template<typename T>
struct is_input_iterator : std::is_convertible<iter_cat_t<T>, std::input_iterator_tag> {};
template<typename T>
inline constexpr bool is_input_iterator_v = is_input_iterator<T>::value;
template<typename T>
struct is_output_iterator : std::is_convertible<iter_cat_t<T>, std::output_iterator_tag> {};
template<typename T>
inline constexpr bool is_output_iterator_v = is_output_iterator<T>::value;
template<typename T>
struct is_forward_iterator : std::is_convertible<iter_cat_t<T>, std::forward_iterator_tag> {};
template<typename T>
inline constexpr bool is_forward_iterator_v = is_forward_iterator<T>::value;
template<typename T>
struct is_bidir_iterator : std::is_convertible<iter_cat_t<T>, std::bidirectional_iterator_tag> {};
template<typename T>
inline constexpr bool is_bidir_iterator_v = is_bidir_iterator<T>::value;
template<typename T>
struct is_random_iterator : std::is_convertible<iter_cat_t<T>, std::random_access_iterator_tag> {};
template<typename T>
inline constexpr bool is_random_iterator_v = is_random_iterator<T>::value;
template<typename _Iter, typename = void>
struct _is_contiguous_iter_helper : std::false_type {};
template<typename _Iter>
struct _is_contiguous_iter_helper<_Iter, typename _Iter::is_contiguous_iter> : std::true_type {};
template<typename T>
struct _is_contiguous_iter_helper<T*, void> : std::true_type {};
#if defined(WJR_CPP_20)
template<typename T>
struct is_contiguous_iter : std::bool_constant<std::contiguous_iterator<T> || _is_contiguous_iter_helper<T>::value> {};
#else
template<typename T>
struct is_contiguous_iter : _is_contiguous_iter_helper<T> {};
#endif
template<typename _Iter>
struct is_contiguous_iter<std::reverse_iterator<_Iter>> : is_contiguous_iter<_Iter> {};
template<typename T>
inline constexpr bool is_contiguous_iter_v = is_contiguous_iter<T>::value;
template<size_t n>
struct __uint_helper{};
template<>
struct __uint_helper<8> {
using type = unsigned char;
};
template<>
struct __uint_helper<16> {
using type = unsigned short;
};
template<>
struct __uint_helper<32> {
using type = std::conditional_t<sizeof(unsigned int) == 4, unsigned int, unsigned long>;
};
template<>
struct __uint_helper<64> {
using type = unsigned long long;
};
template<size_t n>
using uint_t = typename __uint_helper<n>::type;
template<size_t n>
struct __int_helper {};
template<>
struct __int_helper<8> {
using type = signed char;
};
template<>
struct __int_helper<16> {
using type = signed short;
};
template<>
struct __int_helper<32> {
using type = std::conditional_t<sizeof(int) == 4, int, long>;
};
template<>
struct __int_helper<64> {
using type = long long;
};
template<size_t n>
using int_t = typename __int_helper<n>::type;
using int8_t = int_t<8>;
using int16_t = int_t<16>;
using int32_t = int_t<32>;
using int64_t = int_t<64>;
using uint8_t = uint_t<8>;
using uint16_t = uint_t<16>;
using uint32_t = uint_t<32>;
using uint64_t = uint_t<64>;
using intptr_t = int_t<sizeof(void*) * 8>;
using uintptr_t = uint_t<sizeof(void*) * 8>;
using std_int8_t = std::int8_t;
using std_int16_t = std::int16_t;
using std_int32_t = std::int32_t;
using std_int64_t = std::int64_t;
using std_uint8_t = std::uint8_t;
using std_uint16_t = std::uint16_t;
using std_uint32_t = std::uint32_t;
using std_uint64_t = std::uint64_t;
using std_intptr_t = std::intptr_t;
using std_uintptr_t = std::uintptr_t;
template<size_t n, bool __s>
using int_or_uint_t = std::conditional_t<__s, int_t<n>, uint_t<n>>;
template<typename T>
struct make_integral {
static_assert(std::is_integral_v<T>, "T must be an integral type");
using type = int_or_uint_t<sizeof(T) * 8, std::is_signed_v<T>>;
};
template<typename T>
using make_integral_t = typename make_integral<T>::type;
template<typename T>
constexpr make_integral_t<T> make_integral_v(T t) {
return static_cast<make_integral_t<T>>(t);
}
template<typename T>
constexpr std::make_unsigned_t<T> make_unsigned_v(T t) {
return static_cast<std::make_unsigned_t<T>>(t);
}
template<typename T, typename U, typename _Pred>
struct is_comparable : std::conjunction<
has_global_binary_operator<T, U, _Pred>,
has_global_binary_operator<U, T, _Pred>
> {};
template<typename T, typename U, typename _Pred>
constexpr bool is_comparable_v = is_comparable<T, U, _Pred>::value;
template<typename T, typename U, bool = sizeof(T) == sizeof(U) && std::is_integral_v<T> && std::is_integral_v<U>>
inline constexpr bool __is_memory_comparable_helper_v =
std::is_same_v<T, bool> || std::is_same_v<U, bool> || static_cast<T>(-1) == static_cast<U>(-1);
template<typename T, typename U>
inline constexpr bool __is_memory_comparable_helper_v<T, U, false> = false;
template<typename T, typename U, typename _Pred>
struct is_memory_comparable : std::conjunction<
is_standard_comparator<_Pred>,
is_comparable<T, U, _Pred>,
std::bool_constant<__is_memory_comparable_helper_v<T, U>>
> {};
template<typename T, typename U, typename _Pred>
inline constexpr bool is_memory_comparable_v = is_memory_comparable<T, U, _Pred>::value;
enum class ipmc_result {
none,
all,
exit
};
template<typename T, typename U, typename _Pred,
std::enable_if_t<is_any_of_v<_Pred, std::equal_to<>, std::not_equal_to<>>, int> = 0>
constexpr ipmc_result is_possible_memory_comparable(const U& v, _Pred) {
static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "T and U must be integral types");
using nt = T;
using nu = U;
using cat = std::common_type_t<nt, nu>;
auto _Val = static_cast<cat>(v);
constexpr auto __nt_min = static_cast<cat>(std::numeric_limits<nt>::min());
constexpr auto __nt_max = static_cast<cat>(std::numeric_limits<nt>::max());
if constexpr (std::is_signed_v<nt> && std::is_unsigned_v<cat>) {
constexpr auto __nt_negone = static_cast<cat>(static_cast<nt>(-1));
static_assert(__nt_negone == std::numeric_limits<cat>::max(), "error");
static_assert(__nt_min <= __nt_negone, "error");
static_assert(__nt_max < __nt_min, "error");
if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
return ((__nt_min <= _Val && _Val <= __nt_negone)
|| _Val <= __nt_max) ? ipmc_result::exit : ipmc_result::none;
}
else {
return (__nt_max < _Val && _Val < __nt_min) ?
ipmc_result::all : ipmc_result::exit;
}
}
else {
static_assert(__nt_min <= __nt_max, "error");
if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
return (__nt_min <= _Val && _Val <= __nt_max) ?
ipmc_result::exit : ipmc_result::none;
}
else {
return (__nt_max < _Val || _Val < __nt_min) ?
ipmc_result::all : ipmc_result::exit;
}
}
}
template<typename T, size_t threshold>
struct _Auto_variable_helper {
using type = std::conditional_t<sizeof(T) <= threshold, T, const T&>;
};
template<typename T, size_t threshold =
#if WJR_AVX
32
#elif WJR_SSE
16
#else
8
#endif
>
using auto_var_t = typename _Auto_variable_helper<T, threshold>::type;
template<typename T>
struct ref_wrapper {
using type = T;
};
template<typename T>
struct ref_wrapper<T&> {
using type = std::reference_wrapper<T>;
};
template<typename T>
struct ref_wrapper<T&&> {
using type = T;
};
template<typename T>
using ref_wrapper_t = typename ref_wrapper<T>::type;
template<typename...Args>
struct is_integrals : std::conjunction<std::is_integral<Args>...> {};
template<typename...Args>
inline constexpr bool is_integrals_v = is_integrals<Args...>::value;
template<typename...Args>
struct is_floating_points : std::conjunction<std::is_floating_point<Args>...> {};
template<typename...Args>
inline constexpr bool is_floating_points_v = is_floating_points<Args...>::value;
template<typename...Args>
struct is_arithmetics : std::conjunction<std::is_arithmetic<Args>...> {};
template<typename...Args>
inline constexpr bool is_arithmetics_v = is_arithmetics<Args...>::value;
template<typename T>
struct is_unsigned_integral : std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {};
template<typename T>
inline constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;
template<typename T>
struct is_signed_integral : std::conjunction<std::is_integral<T>, std::is_signed<T>> {};
template<typename T>
inline constexpr bool is_signed_integral_v = is_signed_integral<T>::value;
template<typename T>
struct is_standard_numer : std::conjunction<std::is_arithmetic<T>,
std::negation<std::is_same<std::remove_cv_t<T>, bool>>> {};
template<typename T>
inline constexpr bool is_standard_numer_v = is_standard_numer<T>::value;
template<typename T>
struct is_reverse_iterator : std::false_type {};
template<typename _Iter>
struct is_reverse_iterator<std::reverse_iterator<_Iter>> : std::negation<is_reverse_iterator<_Iter>> {};
template<typename T>
inline constexpr bool is_reverse_iterator_v = is_reverse_iterator<T>::value;
template<typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
constexpr auto enum_cast(T t) noexcept {
return static_cast<std::underlying_type_t<T>>(t);
}
namespace enum_ops {
template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T> operator|(T lhs, T rhs) noexcept {
return static_cast<T>(enum_cast(lhs) | enum_cast(rhs));
}
template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T> operator&(T lhs, T rhs) noexcept {
return static_cast<T>(enum_cast(lhs) & enum_cast(rhs));
}
template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T> operator^(T lhs, T rhs) noexcept {
return static_cast<T>(enum_cast(lhs) ^ enum_cast(rhs));
}
template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T> operator~(T t) noexcept {
return static_cast<T>(~enum_cast(t));
}
template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T&> operator|=(T& lhs, T rhs) noexcept {
return lhs = lhs | rhs;
}
template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T&> operator&=(T& lhs, T rhs) noexcept {
return lhs = lhs & rhs;
}
template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T&> operator^=(T& lhs, T rhs) noexcept {
return lhs = lhs ^ rhs;
}
}
class __is_little_endian_helper {
constexpr static std::uint32_t	u4 = 1;
constexpr static std::uint8_t	u1 = static_cast<const std::uint8_t&>(u4);
public:
constexpr static bool value = u1 != 0;
};
enum class endian {
little = 0,
big = 1,
native = __is_little_endian_helper::value ? little : big
};
template<typename T>
constexpr void* voidify(const T* ptr) {
return const_cast<void*>(static_cast<const volatile void*>(ptr));
}
namespace __toAddressHelper {
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(to_address, to_address);
}
template<typename T>
constexpr auto to_address(T* p) noexcept {
static_assert(!std::is_function_v<T>, "");
return p;
}
template<typename T>
constexpr auto to_address(const T& p) noexcept {
if constexpr (__toAddressHelper::has_static_member_function_to_address_v<std::pointer_traits<T>, T>) {
return std::pointer_traits<T>::to_address(p);
}
else {
return wjr::to_address(p.operator->());
}
}
template<typename T>
constexpr auto get_address(T* p) noexcept {
return p;
}
template<typename T>
constexpr auto get_address(const T& p) noexcept {
if constexpr (__toAddressHelper::has_static_member_function_to_address_v<std::pointer_traits<T>, T>) {
return std::pointer_traits<T>::to_address(p);
}
else if constexpr (wjr::has_member_function_point_to_operator_v<add_cref_t<T>>) {
return wjr::get_address(p.operator->());
}
else {
return std::addressof(*std::forward<T>(p));
}
}
template<typename iter>
using iter_address_t = std::add_pointer_t<remove_ref_t<iter_ref_t<iter>>>;
template<typename T>
struct type_identity {
using type = T;
};
template<typename T>
using type_identity_t = typename type_identity<T>::type;
template<typename T, typename = void>
struct _Is_default_convertible : std::false_type {};
template<typename T>
void __testDefaultConvertible(const T&);
template<typename T>
struct _Is_default_convertible<T, std::void_t<decltype(__testDefaultConvertible<T>({}))>> : std::true_type{};
template<typename T>
using is_default_convertible = _Is_default_convertible<T>;
template<typename T>
inline constexpr bool is_default_convertible_v = is_default_convertible<T>::value;
template<typename T, typename U, typename = void>
struct _Is_swappable_with : std::false_type {};
template<typename T, typename U>
struct _Is_swappable_with<T, U, std::void_t<decltype(std::swap(std::declval<T>(), std::declval<U>()))>>
: std::true_type {};
template<typename T, typename U>
struct is_swappable_with :
std::conjunction<_Is_swappable_with<T, U>, _Is_swappable_with<U, T>> {};
template<typename T, typename U>
inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;
template<typename T>
struct is_swappable : is_swappable_with<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<T>> {};
template<typename T>
inline constexpr bool is_swappable_v = is_swappable<T>::value;
template<typename T, typename U>
struct _Is_nothrow_swappable_with : std::bool_constant<noexcept(std::swap(std::declval<T>(), std::declval<U>()))
&& noexcept(std::swap(std::declval<U>(), std::declval<T>()))> {};
template<typename T, typename U>
struct is_nothrow_swappable_with :
std::conjunction<is_swappable_with<T, U>, _Is_nothrow_swappable_with<T, U>> {};
template<typename T, typename U>
inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;
template<typename T>
struct is_nothrow_swappable :
is_nothrow_swappable_with<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<T>> {};
template<typename T>
inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
struct __make_iter_wrapper : public std::tuple<iter, iter>{
using _Mybase = std::tuple<iter, iter>;
using _Mybase::_Mybase;
using _Mybase::operator=;
constexpr iter begin() const noexcept { return std::get<0>(*this); }
constexpr iter end() const noexcept { return std::get<1>(*this); }
constexpr decltype(auto) size() const noexcept {
return std::distance(begin(), end());
}
};
template<typename iter>
constexpr __make_iter_wrapper<iter> make_iter_wrapper(iter first, iter last) {
return { first, last };
}
template<typename T>
using alloc_ptr_t = typename std::allocator_traits<T>::pointer;
template<typename T>
using alloc_value_t = typename std::allocator_traits<T>::value_type;
template<typename T>
using alloc_size_t = typename std::allocator_traits<T>::size_type;
template<typename T>
using alloc_diff_t = typename std::allocator_traits<T>::difference_type;
template<typename T>
struct is_default_allocator : std::false_type {};
template<typename T>
struct is_default_allocator<std::allocator<T>> : std::true_type {};
template<typename T>
inline constexpr bool is_default_allocator_v = is_default_allocator<T>::value;
WJR_REGISTER_HAS_MEMBER_FUNCTION(destroy, destroy);
WJR_REGISTER_HAS_MEMBER_FUNCTION(construct, construct);
template<typename Alloc, typename Iter, typename...Args>
struct is_default_allocator_construct : std::disjunction<is_default_allocator<Alloc>,
std::negation<has_member_function_construct<Alloc, iter_address_t<Iter>, Args...>>> {};
template<typename Alloc, typename Iter, typename...Args>
inline constexpr bool is_default_allocator_construct_v = is_default_allocator_construct<Alloc, Iter, Args...>::value;
template<typename Alloc, typename Iter>
struct is_default_allocator_destroy : std::disjunction<is_default_allocator<Alloc>,
std::negation<has_member_function_destroy<Alloc, iter_address_t<Iter>>>> {};
template<typename Alloc, typename Iter>
constexpr bool is_default_allocator_destroy_v = is_default_allocator_destroy<Alloc, Iter>::value;
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::begin, std_begin);
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::cbegin, std_cbegin);
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::end, std_end);
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::cend, std_cend);
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::rbegin, std_rbegin);
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::crbegin, std_crbegin);
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::rend, std_rend);
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::crend, std_crend);
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::data, std_data);
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::size, std_size);
WJR_REGISTER_HAS_GLOBAL_FUNCTION(std::swap, std_swap);
template<typename _Container>
constexpr auto size(const _Container& c){
if constexpr (has_global_function_std_size_v<_Container>) {
return std::size(c);
}
else {
return std::distance(std::begin(c), std::end(c));
}
}
template<typename _Ty, size_t _Size>
constexpr size_t size(const _Ty(&)[_Size]) noexcept {
return _Size;
}
_WJR_END
#endif

_WJR_BEGIN
template<size_t index, typename T>
struct _Pair_wrapper1 {
template<typename _Ty = T, std::enable_if_t<std::is_default_constructible_v<_Ty>, int> = 0>
constexpr _Pair_wrapper1() noexcept(std::is_nothrow_default_constructible_v<_Ty>)
: val() {}
template<typename _Ty = T, std::enable_if_t<std::is_copy_constructible_v<_Ty>, int> = 0>
constexpr _Pair_wrapper1(const _Ty& other) noexcept(std::is_nothrow_copy_constructible_v<_Ty>)
: val(other) {}
template<typename _Ty = T, std::enable_if_t<std::is_move_constructible_v<_Ty>, int> = 0>
constexpr _Pair_wrapper1(_Ty&& other) noexcept(std::is_nothrow_move_constructible_v<_Ty>)
: val(std::forward<_Ty>(other)) {}
template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
constexpr _Pair_wrapper1(Args&&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
: val(std::forward<Args>(args)...) {}
_Pair_wrapper1(const _Pair_wrapper1&) = default;
_Pair_wrapper1(_Pair_wrapper1&&) noexcept = default;
_Pair_wrapper1& operator=(const _Pair_wrapper1&) = default;
_Pair_wrapper1& operator=(_Pair_wrapper1&&) noexcept = default;
constexpr T& value() noexcept { return val; }
constexpr const T& value() const noexcept { return val; }
private:
T val;
};
template<size_t index, typename T>
struct _Pair_wrapper2 : private T{
using _Mybase = T;
template<typename _Ty = T, std::enable_if_t<std::is_default_constructible_v<_Ty>, int> = 0>
constexpr _Pair_wrapper2() noexcept(std::is_nothrow_default_constructible_v<_Ty>)
: _Mybase() {}
template<typename _Ty = T, std::enable_if_t<std::is_copy_constructible_v<_Ty>, int> = 0>
constexpr _Pair_wrapper2(const _Ty& other) noexcept(std::is_nothrow_copy_constructible_v<_Ty>)
: _Mybase(other) {}
template<typename _Ty = T, std::enable_if_t<std::is_move_constructible_v<_Ty>, int> = 0>
constexpr _Pair_wrapper2(_Ty&& other) noexcept(std::is_nothrow_move_constructible_v<_Ty>)
: _Mybase(std::forward<_Ty>(other)) {}
template<typename...Args, std::enable_if_t<std::is_constructible_v<T, Args...>, int> = 0>
constexpr _Pair_wrapper2(Args&&...args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
: _Mybase(std::forward<Args>(args)...) {}
_Pair_wrapper2(const _Pair_wrapper2&) = default;
_Pair_wrapper2(_Pair_wrapper2&&) noexcept= default;
_Pair_wrapper2& operator=(const _Pair_wrapper2&) = default;
_Pair_wrapper2& operator=(_Pair_wrapper2&&) noexcept = default;
constexpr T& value() noexcept { return *this; }
constexpr const T& value() const noexcept { return *this; }
};
template<size_t index, typename T>
using _Pair_wrapper = std::conditional_t<
std::conjunction_v<std::is_class<T>, std::is_empty<T>, std::negation<std::is_final<T>>>,
_Pair_wrapper2<index, T>,
_Pair_wrapper1<index, T>
>;
template<typename T, typename U>
class compressed_pair : private _Pair_wrapper<0, T>, private _Pair_wrapper<1, U> {
template<typename _Ty, typename _Uy>
using _Is_default_constructible = std::conjunction <
std::is_default_constructible<_Ty>,
std::is_default_constructible<_Uy>>;
template<typename _Ty, typename _Uy>
using _Is_default_convertible = std::conjunction <
is_default_convertible<_Ty>,
is_default_convertible<_Uy>>;
template<typename _Ty, typename _Uy>
using _Is_copy_constructible = std::conjunction <
std::is_copy_constructible<_Ty>,
std::is_copy_constructible<_Uy>>;
template<typename _Ty, typename _Uy, typename _Vty, typename _Wuy>
using _Is_convertible = std::conjunction <
std::is_convertible<_Vty, _Ty>,
std::is_convertible<_Wuy, _Uy>>;
template<typename _Ty, typename _Uy, typename _Vty, typename _Wuy>
using _Is_constructible = std::conjunction<
std::is_constructible<_Ty, _Vty>,
std::is_constructible<_Uy, _Wuy>>;
public:
using _Mybase1 = _Pair_wrapper<0, T>;
using _Mybase2 = _Pair_wrapper<1, U>;
using first_type = T;
using second_type = U;
template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
std::conjunction_v<_Is_default_constructible<_Ty, _Uy>,
_Is_default_convertible<_Ty, _Uy>>, bool> = true>
constexpr compressed_pair() noexcept(std::conjunction_v<std::is_nothrow_default_constructible<_Ty>,
std::is_nothrow_default_constructible<_Uy>>)
: _Mybase1(), _Mybase2() {}
template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
std::conjunction_v<_Is_default_constructible<_Ty, _Uy>,
std::negation<_Is_default_convertible<_Ty, _Uy>>>, bool> = false>
constexpr explicit compressed_pair() noexcept(std::conjunction_v<std::is_nothrow_default_constructible<_Ty>,
std::is_nothrow_default_constructible<_Uy>>)
: _Mybase1(), _Mybase2() {}
template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
std::conjunction_v<_Is_copy_constructible<_Ty, _Uy>,
_Is_convertible<_Ty, _Uy, const _Ty&, const _Uy&>>, bool> = true>
constexpr compressed_pair(const T& _First, const U& _Second)
noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<_Ty>, std::is_nothrow_copy_constructible<_Uy>>)
: _Mybase1(_First), _Mybase2(_Second) {}
template<typename _Ty = T, typename _Uy = U, std::enable_if_t<
std::conjunction_v<_Is_copy_constructible<_Ty, _Uy>,
std::negation<_Is_convertible<_Ty, _Uy, const _Ty&, const _Uy&>>>, bool> = false>
constexpr explicit compressed_pair(const T& _First, const U& _Second)
noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<_Ty>, std::is_nothrow_copy_constructible<_Uy>>)
: _Mybase1(_First), _Mybase2(_Second) {}
template<typename _Other1, typename _Other2, std::enable_if_t<
std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
_Is_convertible<T, U, _Other1, _Other2>>, bool> = true>
constexpr compressed_pair(_Other1&& _First, _Other2&& _Second)
noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
: _Mybase1(std::forward<_Other1>(_First)), _Mybase2(std::forward<_Other2>(_Second)) {}
template<typename _Other1, typename _Other2, std::enable_if_t<
std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
std::negation<_Is_convertible<T, U, _Other1, _Other2>>>, bool> = false>
constexpr explicit compressed_pair(_Other1&& _First, _Other2&& _Second)
noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
: _Mybase1(std::forward<_Other1>(_First)), _Mybase2(std::forward<_Other2>(_Second)) {}
compressed_pair(const compressed_pair&) = default;
compressed_pair(compressed_pair&&) noexcept = default;
template<typename _Other1, typename _Other2, std::enable_if_t<
std::conjunction_v<_Is_constructible<T, U, const _Other1&, const _Other2&>,
_Is_convertible<T, U, const _Other1&, const _Other2&>>, bool> = true>
constexpr compressed_pair(const compressed_pair<_Other1, _Other2>& other)
noexcept(std::conjunction_v<std::is_nothrow_constructible<T, const _Other1&>, std::is_nothrow_constructible<U, const _Other2&>>)
: _Mybase1(other.first()), _Mybase2(other.second()) {}
template<typename _Other1, typename _Other2, std::enable_if_t<
std::conjunction_v<_Is_constructible<T, U, const _Other1&, const _Other2&>,
std::negation<_Is_convertible<T, U, const _Other1&, const _Other2&>>>, bool> = false>
constexpr explicit compressed_pair(const compressed_pair<_Other1, _Other2>& other)
noexcept(std::conjunction_v<std::is_nothrow_constructible<T, const _Other1&>, std::is_nothrow_constructible<U, const _Other2&>>)
: _Mybase1(other.first()), _Mybase2(other.second()) {}
template<typename _Other1, typename _Other2, std::enable_if_t<
std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
_Is_convertible<T, U, _Other1, _Other2>>, bool> = true>
constexpr compressed_pair(compressed_pair<_Other1, _Other2>&& other)
noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
: _Mybase1(std::forward<_Other1>(other.first())), _Mybase2(std::forward<_Other2>(other.second())) {}
template<typename _Other1, typename _Other2, std::enable_if_t<
std::conjunction_v<_Is_constructible<T, U, _Other1, _Other2>,
std::negation<_Is_convertible<T, U, _Other1, _Other2>>>, bool> = false>
constexpr explicit compressed_pair(compressed_pair<_Other1, _Other2>&& other)
noexcept(std::conjunction_v<std::is_nothrow_constructible<T, _Other1>, std::is_nothrow_constructible<U, _Other2>>)
: _Mybase1(std::forward<_Other1>(other.first())), _Mybase2(std::forward<_Other2>(other.second())) {}
template<typename _Tuple1, typename _Tuple2, size_t...N1, size_t...N2>
constexpr compressed_pair(_Tuple1& tp1, _Tuple2& tp2, std::index_sequence<N1...>, std::index_sequence<N2...>)
: _Mybase1(std::get<N1>(std::move(tp1))...), _Mybase2(std::get<N2>(std::move(tp2))...) {}
template<typename...Args1, typename...Args2>
constexpr compressed_pair(std::piecewise_construct_t, std::tuple<Args1...> tp1, std::tuple<Args2...> tp2)
: compressed_pair(tp1, tp2, std::index_sequence_for<Args1...>{}, std::index_sequence_for<Args2...>{}) {}
constexpr compressed_pair&
operator=(std::conditional_t<std::conjunction_v<std::is_copy_assignable<T>,
std::is_copy_assignable<U>>,const compressed_pair&, const wjr::disable_tag&> other)
noexcept(std::conjunction_v<std::is_nothrow_copy_assignable<T>, std::is_nothrow_copy_assignable<U>>) {
first() = other.first();
second() = other.second();
return *this;
}
constexpr compressed_pair&
operator=(std::conditional_t<std::conjunction_v<std::is_move_assignable<T>,
std::is_move_assignable<U>>, compressed_pair&&, wjr::disable_tag&&> other)
noexcept(std::conjunction_v<std::is_nothrow_move_assignable<T>, std::is_nothrow_move_assignable<U>>) {
first() = std::forward<T>(other.first());
second() = std::forward<U>(other.second());
return *this;
}
template<typename _Other1, typename _Other2, std::enable_if_t<
std::conjunction_v<std::negation<std::is_same<compressed_pair, compressed_pair<_Other1, _Other2>>>,
std::is_assignable<T&, const _Other1&>,
std::is_assignable<U&, const _Other2&>>, int> = 0>
constexpr compressed_pair& operator=(const compressed_pair<_Other1, _Other2>& other)
noexcept(std::conjunction_v<std::is_nothrow_assignable<T&, const _Other1&>,
std::is_nothrow_assignable<U&, const _Other2&>>) {
first() = other.first();
second() = other.second();
return *this;
}
template<typename _Other1, typename _Other2, std::enable_if_t<
std::conjunction_v<std::negation<std::is_same<compressed_pair, compressed_pair<_Other1, _Other2>>>,
std::is_assignable<T&, _Other1>,
std::is_assignable<U&, _Other2>>, int> = 0>
constexpr compressed_pair& operator=(compressed_pair<_Other1, _Other2>&& other)
noexcept(std::conjunction_v<std::is_nothrow_assignable<T&, _Other1>,
std::is_nothrow_assignable<U&, _Other2>>) {
first() = std::forward<_Other1>(other.first());
second() = std::forward<_Other2>(other.second());
return *this;
}
constexpr void swap(compressed_pair& other)
noexcept(std::conjunction_v<is_nothrow_swappable<T>, is_nothrow_swappable<U>>) {
using std::swap;
swap(first(), other.first());
swap(second(), other.second());
}
constexpr T& first() noexcept { return _Mybase1::value(); }
constexpr const T& first() const noexcept { return _Mybase1::value(); }
constexpr U& second() noexcept { return _Mybase2::value(); }
constexpr const U& second() const noexcept { return _Mybase2::value(); }
};
#ifdef WJR_CPP17
template<typename T, typename U>
compressed_pair(T, U)->compressed_pair<T, U>;
#endif
template<typename T, typename U>
WJR_NODISCARD constexpr bool operator==(const compressed_pair<T, U>& lhs, const compressed_pair<T, U>& rhs) {
return lhs.first() == rhs.first() && lhs.second() == rhs.second();
}
template<typename T, typename U>
WJR_NODISCARD constexpr bool operator!=(const compressed_pair<T, U>& lhs, const compressed_pair<T, U>& rhs) {
return !(lhs == rhs);
}
template<typename T, typename U>
WJR_NODISCARD constexpr bool operator<(const compressed_pair<T, U>& lhs, const compressed_pair<T, U>& rhs) {
return lhs.first() < rhs.first() || (!(rhs.first() < lhs.first()) && lhs.second() < rhs.second());
}
template<typename T, typename U>
WJR_NODISCARD constexpr bool operator>(const compressed_pair<T, U>& lhs, const compressed_pair<T, U>& rhs) {
return rhs < lhs;
}
template<typename T, typename U>
WJR_NODISCARD constexpr bool operator<=(const compressed_pair<T, U>& lhs, const compressed_pair<T, U>& rhs) {
return !(rhs < lhs);
}
template<typename T, typename U>
WJR_NODISCARD constexpr bool operator>=(const compressed_pair<T, U>& lhs, const compressed_pair<T, U>& rhs) {
return !(lhs < rhs);
}
template<typename T, typename U>
WJR_NODISCARD constexpr compressed_pair<unrefwrap_t<T>, unrefwrap_t<U>> make_pair(T&& t, U&& u)
noexcept(std::conjunction_v<std::is_nothrow_constructible<unrefwrap_t<T>, T>,
std::is_nothrow_constructible<unrefwrap_t<U>, U>>) {
return compressed_pair<unrefwrap_t<T>, unrefwrap_t<U>>(std::forward<T>(t), std::forward<U>(u));
}
_WJR_END
namespace std {
template<typename T, typename U>
using _Wjr_pair = wjr::compressed_pair<T, U>;
template<typename T, typename U, enable_if_t<conjunction_v<wjr::is_swappable<T>, wjr::is_swappable<U>>, int> = 0>
void swap(_Wjr_pair<T, U>& lhs, _Wjr_pair<T, U>& rhs)
noexcept(noexcept(lhs.swap(rhs))) {
lhs.swap(rhs);
}
template<typename T, typename U>
struct tuple_size<_Wjr_pair<T, U>> : integral_constant<size_t, 2> {};
template<size_t I, typename T, typename U>
struct tuple_element<I, _Wjr_pair<T, U>> {
static_assert(I < 2, "tuple_element index out of bounds");
using type = conditional_t<I == 0, T, U>;
};
template<typename _Ret, typename _Pair>
WJR_NODISCARD constexpr _Ret& _Wjr_pair_get(_Pair& p, integral_constant<size_t, 0>) noexcept {
return p.first();
}
template<typename _Ret, typename _Pair>
WJR_NODISCARD constexpr _Ret& _Wjr_pair_get(_Pair& p, integral_constant<size_t, 1>) noexcept {
return p.second();
}
template<size_t I, typename T, typename U>
WJR_NODISCARD constexpr tuple_element_t<I, _Wjr_pair<T, U>>& get(_Wjr_pair<T, U>& p) noexcept {
using _Ret = tuple_element_t<I, _Wjr_pair<T, U>>&;
return _Wjr_pair_get<_Ret>(p, integral_constant<size_t, I>{});
}
template<typename T, typename U>
WJR_NODISCARD constexpr T& get(_Wjr_pair<T, U>& p) noexcept {
return get<0>(p);
}
template<typename U, typename T>
WJR_NODISCARD constexpr U& get(_Wjr_pair<T, U>& p) noexcept {
return get<1>(p);
}
template<size_t I, typename T, typename U>
WJR_NODISCARD constexpr const tuple_element_t<I, _Wjr_pair<T, U>>& get(const _Wjr_pair<T, U>& p) noexcept {
using _Ret = const tuple_element_t<I, _Wjr_pair<T, U>>&;
return _Wjr_pair_get<_Ret>(p, integral_constant<size_t, I>{});
}
template<typename T, typename U>
WJR_NODISCARD constexpr const T& get(const _Wjr_pair<T, U>& p) noexcept {
return get<0>(p);
}
template<typename U, typename T>
WJR_NODISCARD constexpr const U& get(const _Wjr_pair<T, U>& p) noexcept {
return get<1>(p);
}
template <size_t I, typename T, typename U>
WJR_NODISCARD constexpr tuple_element_t<I, _Wjr_pair<T, U>>&& get(
_Wjr_pair<T, U>&& p) noexcept {
using _RRtype = tuple_element_t<I, _Wjr_pair<T, U>>&&;
return forward<_RRtype>(get<I>(p));
}
template <typename T, typename U>
WJR_NODISCARD constexpr T&& get(_Wjr_pair<T, U>&& p) noexcept {
return get<0>(move(p));
}
template <typename U, typename T>
WJR_NODISCARD constexpr U&& get(_Wjr_pair<T, U>&& p) noexcept {
return get<1>(move(p));
}
template <size_t I, typename T, typename U>
WJR_NODISCARD constexpr const tuple_element_t<I, _Wjr_pair<T, U>>&& get(
const _Wjr_pair<T, U>&& p) noexcept {
using _RRtype = const tuple_element_t<I, _Wjr_pair<T, U>>&&;
return forward<_RRtype>(get<I>(p));
}
template <typename T, typename U>
WJR_NODISCARD constexpr const T&& get(
const _Wjr_pair<T, U>&& p) noexcept {
return get<0>(move(p));
}
template <typename U, typename T>
WJR_NODISCARD constexpr const U&& get(
const _Wjr_pair<T, U>&& p) noexcept {
return get<1>(move(p));
}
}
#endif // __WJR_COMPRESSED_PAIR_H

#pragma once
#ifndef __WJR_ALGORITHM_H
#define __WJR_ALGORITHM_H
_WJR_BEGIN
template<typename _Iter, typename _Ty, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) _Iter do_find(_Iter _First, _Iter _Last, const _Ty& _val, _Pred pred);
template<typename _Iter, typename _Ty>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) _Iter do_find(_Iter _First, _Iter _Last, const _Ty& _val);
template<typename _Iter, typename _Pr>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) _Iter do_find_if(_Iter _First, _Iter _Last, _Pr _Pred);
template<typename _Iter, typename _Pr>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) _Iter do_find_if_not(_Iter _First, _Iter _Last, _Pr _Pred);
template<typename _Iter, typename _Ty>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) typename std::iterator_traits<_Iter>::difference_type
do_count(_Iter _First, _Iter _Last, const _Ty& _Val);
template<typename _Iter, typename _Pr>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) typename std::iterator_traits<_Iter>::difference_type
do_count_if(_Iter _First, _Iter _Last, _Pr _Pred);
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) std::pair<_Iter1, _Iter2> do_mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred);
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) std::pair<_Iter1, _Iter2> do_mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) std::pair<_Iter1, _Iter2> do_mismatch(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2);
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) std::pair<_Iter1, _Iter2> do_mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred);
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_equal(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2);
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) int do_compare(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) int do_compare(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_lexicographical_compare(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_lexicographical_compare(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);
template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_fill(_Iter _First, _Iter _Last, const _Val& value);
template<typename _Iter, typename _Size, typename _Val>
WJR_CONSTEXPR20 _Iter do_fill_n(_Iter _First, _Size count, const _Val& value);
template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_copy(_Input _First1, _Input _Last1, _Output _First2);
template<typename _Input, typename _Size, typename _Output>
WJR_CONSTEXPR20 _Output do_copy_n(_Input _First1, _Size count, _Output _First2);
template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_copy_backward(_Input _First1, _Input _Last1, _Output _Last2);
template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_move(_Input _First1, _Input _Last1, _Output _First2);
template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_move_backward(_Input _First1, _Input _Last1, _Output _Last2);
template<typename _Iter, typename _Pred = std::equal_to<>>
class default_searcher;
template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter1 do_search(_Iter1 _First1, _Iter1 _Last1,
_Iter2 _First2, _Iter2 _Last2);
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 _Iter1 do_search(_Iter1 _First1, _Iter1 _Last1,
_Iter2 _First2, _Iter2 _Last2, _Pred pred);
template<typename _Iter, typename _Searcher>
WJR_CONSTEXPR20 _Iter do_search(_Iter _First, _Iter _Last, const _Searcher& _Srch);
template<typename _Iter, typename...Args,
std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int> = 0>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, Args&&... args);
template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int> = 0>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, default_construct_tag);
template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int> = 0>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, value_construct_tag);
template<typename Alloc, typename _Iter, typename...Args,
std::enable_if_t<!is_iterator_v<wjr::remove_cvref_t<Alloc>>, int> = 0>
WJR_CONSTEXPR20 void do_construct_at(Alloc& al, _Iter iter, Args&&...args);
template<typename _Iter>
WJR_CONSTEXPR20 void do_destroy_at(_Iter ptr);
template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_destroy_at(Alloc& al, _Iter iter);
template<typename _Iter>
WJR_CONSTEXPR20 void do_destroy(_Iter _First, _Iter _Last);
template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_destroy(Alloc& al, _Iter _First, _Iter _Last);
template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void do_destroy_n(_Iter _First, const _Diff n);
template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void do_destroy_n(Alloc& al, _Iter _First, _Diff n);
template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_default_construct(_Iter _First, _Iter _Last);
template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_default_construct(
Alloc& al, _Iter _First, _Iter _Last);
template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_default_construct_n(_Iter _First, _Diff n);
template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_default_construct_n(
Alloc& al, _Iter _First, _Diff n);
template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_value_construct(_Iter _First, _Iter _Last);
template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_value_construct(
Alloc& al, _Iter _First, _Iter _Last);
template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_value_construct_n(_Iter _First, _Diff n);
template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_value_construct_n(
Alloc& al, _Iter _First, _Diff n);
template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest);
template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy(
Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest);
template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy_n(_Iter1 _First, _Diff n, _Iter2 _Dest);
template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy_n(Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest);
template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, const _Val& val);
template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, default_construct_tag);
template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, value_construct_tag);
template<typename Alloc, typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_uninitialized_fill(Alloc& al, _Iter _First, _Iter _Last, const _Val& val);
template<typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff count, const _Val& val);
template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff n, default_construct_tag);
template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff n, value_construct_tag);
template<typename Alloc, typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(Alloc& al, _Iter _First, _Diff n, const _Val& val);
template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_move(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest);
template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_move(Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest);
template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_uninitialized_move_n(_Iter1 _First, _Diff n, _Iter2 _Dest);
template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_uninitialized_move_n(
Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest);
template<typename _Iter, typename _Func>
WJR_CONSTEXPR20 void do_for_each(_Iter _First, _Iter _Last, _Func fn);
template<typename _Iter, typename _SizeT, typename _Func>
WJR_CONSTEXPR20 void do_for_each_n(_Iter _First, _SizeT n, _Func fn);
template<typename Alloc>
class temporary_allocator_value;
#define __WJR_REGISTER_ALGO_FUNCTOR(name)	                                \
struct name##_fn{	                                                        \
template<typename...Args>	                                            \
WJR_INLINE_CONSTEXPR20 decltype(auto) operator()(Args&&...args) const{	\
return do_##name(std::forward<Args>(args)...);				        \
}			                                                            \
};	                                                                        \
inline constexpr name##_fn name;
__WJR_REGISTER_ALGO_FUNCTOR(find);
__WJR_REGISTER_ALGO_FUNCTOR(find_if);
__WJR_REGISTER_ALGO_FUNCTOR(find_if_not);
__WJR_REGISTER_ALGO_FUNCTOR(count);
__WJR_REGISTER_ALGO_FUNCTOR(count_if);
__WJR_REGISTER_ALGO_FUNCTOR(mismatch);
__WJR_REGISTER_ALGO_FUNCTOR(equal);
__WJR_REGISTER_ALGO_FUNCTOR(lexicographical_compare);
__WJR_REGISTER_ALGO_FUNCTOR(compare);
__WJR_REGISTER_ALGO_FUNCTOR(fill);
__WJR_REGISTER_ALGO_FUNCTOR(fill_n);
__WJR_REGISTER_ALGO_FUNCTOR(copy);
__WJR_REGISTER_ALGO_FUNCTOR(copy_n);
__WJR_REGISTER_ALGO_FUNCTOR(copy_backward);
__WJR_REGISTER_ALGO_FUNCTOR(move);
__WJR_REGISTER_ALGO_FUNCTOR(move_backward);
__WJR_REGISTER_ALGO_FUNCTOR(search);
__WJR_REGISTER_ALGO_FUNCTOR(construct_at);
__WJR_REGISTER_ALGO_FUNCTOR(destroy_at);
__WJR_REGISTER_ALGO_FUNCTOR(destroy);
__WJR_REGISTER_ALGO_FUNCTOR(destroy_n);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_default_construct);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_default_construct_n);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_value_construct);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_value_construct_n);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_copy);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_copy_n);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_fill);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_fill_n);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_move);
__WJR_REGISTER_ALGO_FUNCTOR(uninitialized_move_n);
__WJR_REGISTER_ALGO_FUNCTOR(for_each);
__WJR_REGISTER_ALGO_FUNCTOR(for_each_n);
#undef __WJR_REGISTER_ALGO_FUNCTOR
_WJR_END
#ifndef __WJR_ALGORITHM_H
#error  "This file should not be included directly. Include <wjr/algorithm.h> instead."
#endif //__WJR_ALGORITHM_H
#include <algorithm>
#include <memory>
#pragma once
#ifndef __WJR_ALGO_ALOG_H
#define __WJR_ALGO_ALOG_H
#ifndef __WJR_ALGO_MEM_ALL_H
#define __WJR_ALGO_MEM_ALL_H
#pragma once
#ifndef __WJR_SIMD_SIMD_INTRIN_H
#define __WJR_SIMD_SIMD_INTRIN_H
#pragma once
#ifndef __WJR_MATH_H
#define __WJR_MATH_H
#pragma once
#ifndef __WJR_ASM_ASM_H
#define __WJR_ASM_ASM_H
#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif
_WJR_ASM_BEGIN
const static int _WJR_LOG_TABLE[256] = {
0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
};
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) int __wjr_fallback_clz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;
if (WJR_LIKELY(x != 0)) {
int n = 0;
if constexpr (_Nd > 32) {
if (x >> 32) {
n += 32;
x >>= 32;
}
}
if constexpr (_Nd > 16) {
if (x >> 16) {
n += 16;
x >>= 16;
}
}
if constexpr (_Nd > 8) {
if (x >> 8) {
n += 8;
x >>= 8;
}
}
return  _Nd - (n + _WJR_LOG_TABLE[x]);
}
return _Nd;
}
#if WJR_HAS_BUILTIN(__builtin_clz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_builtin_clz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;
if (WJR_LIKELY(x != 0)) {
constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
if constexpr (_Nd <= _Nd_ui) {
constexpr auto __diff = _Nd_ui - _Nd;
return __builtin_clz(x) - __diff;
}
else if constexpr (_Nd <= _Nd_ul) {
constexpr auto __diff = _Nd_ul - _Nd;
return __builtin_clzl(x) - __diff;
}
#if defined(WJR_X86_64)
else if constexpr (_Nd <= _Nd_ull) {
constexpr auto __diff = _Nd_ull - _Nd;
return __builtin_clzll(x) - __diff;
}
#endif // WJR_X86_64
else {
static_assert(_Nd <= _Nd_ull, "countl_zero is not implemented for this type");
}
}
return _Nd;
}
#elif defined(WJR_COMPILER_MSVC)
#if defined(WJR_X86)
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_x86_64_avx2_clz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;
if constexpr (_Nd <= 16) {
return static_cast<int>(__lzcnt16(x) - (16 - _Nd));
}
else if constexpr (_Nd <= 32) {
return static_cast<int>(__lzcnt(x));
}
else {
#if defined(WJR_X86_32)
const auto xh = static_cast<unsigned int>(x >> 32);
const auto xl = static_cast<unsigned int>(x);
if (xh == 0) {
return 32 + static_cast<int>(__lzcnt(xl));
}
else {
return static_cast<int>(__lzcnt(xh));
}
#else
return static_cast<int>(__lzcnt64(x));
#endif
}
}
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_x86_64_normal_clz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;
unsigned long _Result;
if constexpr (_Nd <= 32) {
if (!_BitScanReverse(&_Result, x)) {
return _Nd;
}
}
else {
#if defined(WJR_X86_32)
const auto xh = static_cast<unsigned int>(x >> 32);
if (_BitScanReverse(&_Result, xh)) {
return static_cast<int>(31 - _Result);
}
const auto xl = static_cast<unsigned int>(x);
if (!_BitScanReverse(&_Result, xl)) {
return _Nd;
}
#else
if (!_BitScanReverse64(&_Result, x)) {
return _Nd;
}
#endif
}
return static_cast<int>(_Nd - 1 - _Result);
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_x86_64_clz(T x) noexcept {
#if WJR_AVX2
return __wjr_msvc_x86_64_avx2_clz(x);
#elif defined(_WJR_CPUINFO)
if (is_avx2()) {
return __wjr_msvc_x86_64_avx2_clz(x);
}
else {
return __wjr_msvc_x86_64_normal_clz(x);
}
#else
return __wjr_msvc_x86_64_normal_clz(x);
#endif // WJR_AVX2
}
#elif defined(WJR_ARM)
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_arm_clz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;
if (x == 0) {
return _Nd;
}
if constexpr (_Nd <= 32) {
return static_cast<int>(_CountLeadingZeros(x));
}
else {
return static_cast<int>(_CountLeadingZeros64(x));
}
}
#endif
#endif
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int clz(T x) noexcept {
if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_clz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
return __wjr_builtin_clz(x);
#elif defined(WJR_COMPILER_MSVC)
#if defined(WJR_X86)
return __wjr_msvc_x86_64_clz(x);
#elif defined(WJR_ARM)
return __wjr_msvc_arm_clz(x);
#endif
#endif
}
return __wjr_fallback_clz(x);
}
_WJR_ASM_END

#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif
_WJR_ASM_BEGIN
#if WJR_HAS_BUILTIN(__builtin_ctz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_builtin_ctz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;
if (WJR_LIKELY(x != 0)) {
constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
if constexpr (_Nd <= _Nd_ui) {
return __builtin_ctz(x);
}
else if constexpr (_Nd <= _Nd_ul) {
return __builtin_ctzl(x);
}
else if constexpr (_Nd <= _Nd_ull) {
return __builtin_ctzll(x);
}
else {
static_assert(_Nd <= _Nd_ull, "countr_zero is not implemented for this type");
}
}
return _Nd;
}
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
&& !defined(__INTEL_COMPILER)
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_x86_64_avx2_ctz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;
constexpr T _Max = std::numeric_limits<T>::max();
if constexpr (_Nd <= 32) {
return static_cast<int>(_tzcnt_u32(static_cast<unsigned int>(~_Max | x)));
}
else {
#if defined(WJR_X86_32)
const auto xh = static_cast<unsigned int>(x >> 32);
const auto xl = static_cast<unsigned int>(x);
if (xh == 0) {
return 32 + static_cast<int>(_tzcnt_u32(xl));
}
else {
return static_cast<int>(_tzcnt_u32(xh));
}
#else
return static_cast<int>(_tzcnt_u64(x));
#endif // WJR_X86_32
}
}
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_x86_64_normal_ctz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;
constexpr T _Max = std::numeric_limits<T>::max();
unsigned long _Result;
if constexpr (_Nd <= 32) {
if (!_BitScanForward(&_Result, x)) {
return _Nd;
}
}
else {
#if defined(WJR_X86_32)
const auto xl = static_cast<unsigned int>(x);
if (_BitScanForward(&_Result, xl)) {
return static_cast<int>(_Result);
}
const auto xh = static_cast<unsigned int>(x >> 32);
if (!_BitScanForward(&_Result, xh)) {
return _Nd;
}
else {
return static_cast<int>(32 + _Result);
}
#else
if (!_BitScanForward64(&_Result, x)) {
return _Nd;
}
#endif
}
return static_cast<int>(_Result);
}
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_x86_64_ctz(T x) noexcept {
#if WJR_AVX2
return __wjr_msvc_x86_64_avx2_ctz(x);
#elif defined(_WJR_CPUINFO)
if (is_avx2()) {
return __wjr_msvc_x86_64_avx2_ctz(x);
}
else {
return __wjr_msvc_x86_64_normal_ctz(x);
}
#else
return __wjr_msvc_x86_64_normal_ctz(x);
#endif // WJR_AVX2
}
#endif
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int ctz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;
if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_ctz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
return __wjr_builtin_ctz(x);
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
&& !defined(__INTEL_COMPILER)
return __wjr_msvc_x86_64_ctz(x);
#endif
}
return _Nd - clz(static_cast<T>(static_cast<T>(~x) & static_cast<T>(x - 1)));
}
_WJR_ASM_END

#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif
_WJR_ASM_BEGIN
template<typename _Ty>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) int __wjr_fallback_popcount(_Ty _Val) noexcept {
constexpr int _Digits = std::numeric_limits<_Ty>::digits;
#if defined(WJR_X86_32) || defined(WJR_ARM)
if constexpr (_Digits == 64) {
return _Popcount_fallback(static_cast<unsigned long>(_Val))
+ _Popcount_fallback(static_cast<unsigned long>(_Val >> 32));
}
#endif // defined(WJR_X86_32) || defined(WJR_ARM)
_Val = static_cast<_Ty>(_Val - ((_Val >> 1) & static_cast<_Ty>(0x5555'5555'5555'5555ull)));
_Val = static_cast<_Ty>((_Val & static_cast<_Ty>(0x3333'3333'3333'3333ull))
+ ((_Val >> 2) & static_cast<_Ty>(0x3333'3333'3333'3333ull)));
_Val = static_cast<_Ty>((_Val + (_Val >> 4)) & static_cast<_Ty>(0x0F0F'0F0F'0F0F'0F0Full));
_Val = static_cast<_Ty>(_Val * static_cast<_Ty>(0x0101'0101'0101'0101ull));
return static_cast<int>(_Val >> (_Digits - 8));
}
#if WJR_HAS_BUILTIN(__builtin_popcount) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_builtin_popcount(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;
constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
if constexpr (_Nd <= _Nd_ui) {
return __builtin_popcount(static_cast<unsigned>(x));
}
else if constexpr (_Nd <= _Nd_ul) {
return __builtin_popcountl(static_cast<unsigned long>(x));
}
else if constexpr (_Nd <= _Nd_ull) {
return __builtin_popcountll(static_cast<unsigned long long>(x));
}
else {
static_assert(_Nd <= _Nd_ull, "unsupported integer type");
}
}
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
&& !defined(__INTEL_COMPILER)
template <typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_x86_64_popcount(T x) noexcept {
constexpr int _Digits = std::numeric_limits<T>::digits;
if constexpr (_Digits <= 16) {
return static_cast<int>(__popcnt16(x));
}
else if constexpr (_Digits == 32) {
return static_cast<int>(__popcnt(x));
}
else {
#ifdef WJR_X86_32
return static_cast<int>(__popcnt(x >> 32) + __popcnt(static_cast<unsigned int>(x)));
#else
return static_cast<int>(__popcnt64(x));
#endif // WJR_X86_32
}
}
#endif
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int popcnt(T x) noexcept {
if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_popcount) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
return __wjr_builtin_popcount(x);
#elif defined(_MSC_VER) && defined(WJR_X86) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
&& !defined(__INTEL_COMPILER)
return __wjr_msvc_x86_64_popcount(x);
#endif
}
return __wjr_fallback_popcount(x);
}
_WJR_ASM_END

#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif
_WJR_ASM_BEGIN
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint8_t __wjr_fallback_bswap8(uint8_t x) {
return x;
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint16_t __wjr_fallback_bswap16(uint16_t x) {
return static_cast<uint16_t>(x >> 8) | static_cast<uint16_t>(x << 8);
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint32_t __wjr_fallback_bswap32(uint32_t x) {
return (x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | (x << 24);
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint64_t __wjr_fallback_bswap64(uint64_t x) {
return (x >> 56) | ((x >> 40) & 0xff00) | ((x >> 24) & 0xff0000) | ((x >> 8) & 0xff000000)
| ((x << 8) & 0xff00000000) | ((x << 24) & 0xff0000000000) | ((x << 40) & 0xff000000000000) | (x << 56);
}
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) T __wjr_fallback_bswap(T x) {
constexpr auto _Nd = std::numeric_limits<T>::digits;
if constexpr (_Nd <= 8) {
return __wjr_fallback_bswap8(x);
}
else if constexpr (_Nd <= 16) {
return __wjr_fallback_bswap16(x);
}
else if constexpr (_Nd <= 32) {
return __wjr_fallback_bswap32(x);
}
else if constexpr (_Nd <= 64) {
return __wjr_fallback_bswap64(x);
}
else {
static_assert(_Nd <= 64, "Unsupported type.");
}
}
#if WJR_HAS_BUILTIN(__builtin_bswap16) || WJR_HAS_GCC(10, 1, 0) || WJR_HAS_CLANG(15, 0, 0)
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) T __wjr_builtin_bswap(T x) {
constexpr auto _Nd = std::numeric_limits<T>::digits;
constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;
if constexpr (_Nd <= _Nd_ub) {
return x;
}
else if constexpr (_Nd <= _Nd_us) {
return __builtin_bswap16(x);
}
else if constexpr (_Nd <= _Nd_ui) {
return __builtin_bswap32(x);
}
else if constexpr (_Nd <= _Nd_ul) {
return __builtin_bswap64(x);
}
#if defined(WJR_X86_64)
else if constexpr (_Nd <= _Nd_ull) {
return __builtin_bswap64(x);
}
#endif // WJR_X86_64
else {
static_assert(_Nd <= _Nd_ull, "unsupported integer type");
}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) T __wjr_msvc_bswap(T x) {
constexpr auto _Nd = std::numeric_limits<T>::digits;
constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
if constexpr (_Nd <= 8) {
return x;
}
else if constexpr (_Nd <= 16) {
return _byteswap_ushort(x);
}
else if constexpr (_Nd <= 32) {
return _byteswap_ulong(x);
}
else if constexpr (_Nd <= 64) {
return _byteswap_uint64(x);
}
else {
static_assert(_Nd <= _Nd, "unsupported integer type");
}
}
#endif // WJR_HAS_BUILTIN(__builtin_bswap16) || WJR_HAS_CLANG(3, 2, 0)
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) T bswap(T x) {
if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_bswap16) || WJR_HAS_GCC(10, 1, 0) || WJR_HAS_CLANG(15, 0, 0)
return __wjr_builtin_bswap(x);
#elif defined(WJR_COMPILER_MSVC)
return __wjr_msvc_bswap(x);
#endif
}
return __wjr_fallback_bswap(x);
}
_WJR_ASM_END

#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif
_WJR_ASM_BEGIN
#if defined(_WJR_FAST_REP)
WJR_INTRINSIC_INLINE void rep_stos(uint8_t* s, uint8_t val, size_t n) {
#if defined(WJR_COMPILER_MSVC)
__stosb(reinterpret_cast<unsigned char*>(s), val, n);
#else
asm volatile("rep stosb" : "+D"(s), "+c"(n) : "a"(val) : "memory");
#endif
}
WJR_INTRINSIC_INLINE void rep_stos(uint16_t* s, uint16_t val, size_t n) {
#if defined(WJR_COMPILER_MSVC)
__stosw(reinterpret_cast<unsigned short*>(s), val, n);
#else
asm volatile("rep stosw" : "+D"(s), "+c"(n) : "a"(val) : "memory");
#endif
}
WJR_INTRINSIC_INLINE void rep_stos(uint32_t* s, uint32_t val, size_t n) {
#if defined(WJR_COMPILER_MSVC)
__stosd(reinterpret_cast<unsigned long*>(s), val, n);
#else
asm volatile("rep stosd" : "+D"(s), "+c"(n) : "a"(val) : "memory");
#endif
}
WJR_INTRINSIC_INLINE void rep_stos(uint64_t* s, uint64_t val, size_t n) {
#if defined(WJR_COMPILER_MSVC)
__stosq(reinterpret_cast<unsigned long long*>(s), val, n);
#else
asm volatile("rep stosq" : "+D"(s), "+c"(n) : "a"(val) : "memory");
#endif
}
#endif  // _WJR_FAST_REP
_WJR_ASM_END

#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif
_WJR_ASM_BEGIN
#if WJR_HAS_BUILTIN(__builtin_addc) || WJR_HAS_CLANG(5, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_builtin_adc(T a, T b, T carry_in, T& carry_out) {
constexpr auto _Nd = std::numeric_limits<T>::digits;
constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;
if constexpr (_Nd <= _Nd_ub) {
unsigned char CF = 0;
T ret = __builtin_addcb(a, b, carry_in, &CF);
carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_us) {
unsigned short CF = 0;
T ret = __builtin_addcs(a, b, carry_in, &CF);
carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_ui) {
unsigned int CF = 0;
T ret = __builtin_addc(a, b, carry_in, &CF);
carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_ul) {
unsigned long CF = 0;
T ret = __builtin_addcl(a, b, carry_in, &CF);
carry_out = CF;
return ret;
}
#if defined(WJR_X86_64)
else if constexpr (_Nd <= _Nd_ull) {
unsigned long long CF = 0;
T ret = __builtin_addcll(a, b, carry_in, &CF);
carry_out = CF;
return ret;
}
#endif // WJR_X86_64
else {
static_assert(_Nd <= _Nd_ull, "unsupported integer type");
}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_msvc_adc(T a, T b, T carry_in, T& carry_out) {
constexpr auto _Nd = std::numeric_limits<T>::digits;
constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;
if constexpr (_Nd <= _Nd_ub) {
unsigned char ret = 0;
carry_out = _addcarry_u8(carry_in, a, b, &ret);
return ret;
}
else if constexpr (_Nd <= _Nd_us) {
unsigned short ret = 0;
carry_out = _addcarry_u16(carry_in, a, b, &ret);
return ret;
}
else if constexpr (_Nd <= _Nd_ui) {
unsigned int ret = 0;
carry_out = _addcarry_u32(carry_in, a, b, &ret);
return ret;
}
#if defined(WJR_X86_64)
else if constexpr (_Nd <= _Nd_ull) {
unsigned long long ret = 0;
carry_out = _addcarry_u64(carry_in, a, b, &ret);
return ret;
}
#endif // WJR_X86_64
else {
static_assert(_Nd <= _Nd_ull, "unsupported integer type");
}
}
#endif
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(INTRINSIC_E_CONSTEXPR) T adc(T a, T b, T carry_in, T& carry_out) {
if (!wjr::is_constant_evaluated()) {
if (!((is_constant_p(a) && is_constant_p(b)) || (is_constant_p(carry_in) && carry_in == 0))) {
#if WJR_HAS_BUILTIN(__builtin_addc) || WJR_HAS_CLANG(5, 0, 0)
return __wjr_builtin_adc(a, b, carry_in, carry_out);
#elif defined(WJR_COMPILER_MSVC)
return __wjr_msvc_adc(a, b, carry_in, carry_out);
#elif defined(WJR_INLINE_ASM)
#if defined(WJR_BETTER_INLINE_ASM)
asm("add $255, %b0\n\t"
"adc %3, %2\n\t"
"setb %b1"
: "+r"(carry_in), "=rm"(carry_out), "+%r"(a)
: "rm"(b)
: "cc");
#else
asm("add $255, %b0\n\t"
"adc %3, %2\n\t"
"setb %b1"
: "+r"(carry_in), "=r"(carry_out), "+%r"(a)
: "r"(b)
: "cc");
return a;
#endif // WJR_BETTER_INLINE_ASM
#endif
}
}
T c = 0;
a += b;
c = a < b;
a += carry_in;
c |= a < carry_in;
carry_out = c;
return a;
}
_WJR_ASM_END

#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif
_WJR_ASM_BEGIN
#if WJR_HAS_BUILTIN(__builtin_subc) || WJR_HAS_CLANG(5, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_builtin_sbb(T a, T b, T carry_in, T& carry_out) {
constexpr auto _Nd = std::numeric_limits<T>::digits;
constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;
if constexpr (_Nd <= _Nd_ub) {
unsigned char CF = 0;
T ret = __builtin_subcb(a, b, carry_in, &CF);
carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_us) {
unsigned short CF = 0;
T ret = __builtin_subcs(a, b, carry_in, &CF);
carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_ui) {
unsigned int CF = 0;
T ret = __builtin_subc(a, b, carry_in, &CF);
carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_ul) {
unsigned long CF = 0;
T ret = __builtin_subcl(a, b, carry_in, &CF);
carry_out = CF;
return ret;
}
#if defined(WJR_X86_64)
else if constexpr (_Nd <= _Nd_ull) {
unsigned long long CF = 0;
T ret = __builtin_subcll(a, b, carry_in, &CF);
carry_out = CF;
return ret;
}
#endif // WJR_X86_64
else {
static_assert(_Nd <= _Nd_ull, "unsupported integer type");
}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_msvc_sbb(T a, T b, T carry_in, T& carry_out) {
constexpr auto _Nd = std::numeric_limits<T>::digits;
constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;
if constexpr (_Nd <= _Nd_ub) {
unsigned char ret = 0;
carry_out = _subborrow_u8(carry_in, a, b, &ret);
return ret;
}
else if constexpr (_Nd <= _Nd_us) {
unsigned short ret = 0;
carry_out = _subborrow_u16(carry_in, a, b, &ret);
return ret;
}
else if constexpr (_Nd <= _Nd_ui) {
unsigned int ret = 0;
carry_out = _subborrow_u32(carry_in, a, b, &ret);
return ret;
}
#if defined(WJR_X86_64)
else if constexpr (_Nd <= _Nd_ull) {
unsigned long long ret = 0;
carry_out = _subborrow_u64(carry_in, a, b, &ret);
return ret;
}
#endif // WJR_X86_64
else {
static_assert(_Nd <= _Nd_ull, "unsupported integer type");
}
}
#endif
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(INTRINSIC_E_CONSTEXPR) T sbb(T a, T b, T carry_in, T& carry_out) {
if (!wjr::is_constant_evaluated()) {
if (!((is_constant_p(a) && is_constant_p(b)) || (is_constant_p(carry_in) && carry_in == 0))) {
#if WJR_HAS_BUILTIN(__builtin_subc) || WJR_HAS_CLANG(5, 0, 0)
return __wjr_builtin_sbb(a, b, carry_in, carry_out);
#elif defined(WJR_COMPILER_MSVC)
return __wjr_msvc_sbb(a, b, carry_in, carry_out);
#elif defined(WJR_INLINE_ASM) // Clang does not need inline assembly
#if defined(WJR_BETTER_INLINE_ASM)
asm("add $255, %b0\n\t"
"sbb %3, %2\n\t"
"setb %b1"
: "+r"(carry_in), "=rm"(carry_out), "+r"(a)
: "rm"(b)
: "cc");
#else
asm("add $255, %b0\n\t"
"sbb %3, %2\n\t"
"setb %b1"
: "+r"(carry_in), "=r"(carry_out), "+r"(a)
: "r"(b)
: "cc");
return a;
#endif // WJR_BETTER_INLINE_ASM
#endif
}
}
T c = 0;
c = a < b;
a -= b;
c |= a < carry_in;
a -= carry_in;
carry_out = c;
return a;
}
_WJR_ASM_END

#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif
_WJR_ASM_BEGIN
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR20) uint64_t u64x64(uint64_t a, uint64_t b, uint64_t& hi) {
if (!wjr::is_constant_evaluated()) {
#if defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64)
return _umul128(a, b, &hi);
#elif defined(WJR_HAS_INT128)
unsigned __int128 r = (unsigned __int128)a * (unsigned __int128)b;
hi = (uint64_t)(r >> 64);
return (uint64_t)r;
#elif defined(WJR_INLINE_ASM)
asm volatile("mulq %[u1]" : [u1] "+d"(a) : "a"(b) : "cc");
hi = a;
return b;
#endif
}
uint64_t ah = a >> 32;
uint64_t al = a & 0xFFFFFFFF;
uint64_t bh = b >> 32;
uint64_t bl = b & 0xFFFFFFFF;
uint64_t rh = ah * bh;
uint64_t rm0 = ah * bl;
uint64_t rm1 = al * bh;
uint64_t rl = al * bl;
uint64_t t = rl + (rm0 << 32);
uint64_t lo = t + (rm1 << 32);
uint64_t c = t < rl;
hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;
return lo;
}
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) uint32_t u32x32(uint32_t a, uint32_t b, uint32_t& hi) {
uint64_t r = (uint64_t)a * (uint64_t)b;
hi = (uint32_t)(r >> 32);
return (uint32_t)r;
}
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) uint16_t u16x16(uint16_t a, uint16_t b, uint16_t& hi) {
uint32_t r = (uint32_t)a * (uint32_t)b;
hi = (uint16_t)(r >> 16);
return (uint16_t)r;
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint64_t u64x64lo(uint64_t a, uint64_t b) {
return a * b;
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint32_t u32x32hi(uint32_t a, uint32_t b) {
return (uint32_t)(((uint64_t)a * (uint64_t)b) >> 32);
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) uint64_t u64x64hi(uint64_t a, uint64_t b) {
if (!wjr::is_constant_evaluated()) {
#if defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64)
return __umulh(a, b);
#elif defined(WJR_HAS_INT128)
return (uint64_t)(((unsigned __int128)a * (unsigned __int128)b) >> 64);
#elif defined(WJR_INLINE_ASM)
asm volatile("mulq %[u1]" : [u1] "+d"(a) : "a"(b) : "cc");
return a;
#endif
}
uint32_t ah = a >> 32;
uint32_t al = a & 0xFFFFFFFF;
uint32_t bh = b >> 32;
uint32_t bl = b & 0xFFFFFFFF;
uint32_t albl_hi = u32x32hi(al, bl);
uint64_t albh = al * (uint64_t)bh;
uint64_t ahbl = ah * (uint64_t)bl;
uint64_t ahbh = ah * (uint64_t)bh;
uint64_t temp = ahbl + albl_hi;
uint64_t temp_lo = temp & 0xFFFFFFFF;
uint64_t temp_hi = temp >> 32;
return ahbh + temp_hi + ((temp_lo + albh) >> 32);
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint32_t u32x32lo(uint32_t a, uint32_t b) {
return a * b;
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint16_t u16x16hi(uint16_t a, uint16_t b) {
return (uint16_t)(((uint32_t)a * (uint32_t)b) >> 16);
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint16_t u16x16lo(uint16_t a, uint16_t b) {
return a * b;
}
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR20) uint64_t u128d64t64(uint64_t hi, uint64_t lo, uint64_t den, uint64_t& r) {
if (!wjr::is_constant_evaluated()) {
#if defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64)
return _udiv128(hi, lo, den, &r);
#elif defined(WJR_INLINE_ASM)
asm volatile("divq %[u2]" : [u2] "+d"(hi), "+a"(lo) : "r"(den) : "cc");
r = hi;
return lo;
#endif
}
extern uint64_t __large__u128d64t64(uint64_t hi, uint64_t lo, uint64_t den, uint64_t & r);
return __large__u128d64t64(hi, lo, den, r);
}
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR20) uint32_t u64d32t32(uint64_t x, uint32_t den, uint32_t& r) {
if (wjr::is_constant_evaluated()) {
#if defined(WJR_COMPILER_MSVC) && defined(WJR_X86)
return _udiv64(x, den, &r);
#elif defined(WJR_INLINE_ASM)
uint32_t hi = x >> 32;
uint32_t lo = x;
asm volatile("divl %[u2]" : [u2] "+d"(hi), "+a"(lo) : "r"(den) : "cc");
r = hi;
return lo;
#endif
}
uint32_t q = x / den;
r = x % den;
return q;
}
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) uint16_t u32d16t16(uint32_t x, uint16_t den, uint16_t& r) {
uint16_t q = x / den;
r = x % den;
return q;
}
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) uint8_t u16d8t8(uint16_t x, uint8_t den, uint8_t& r) {
uint8_t q = x / den;
r = x % den;
return q;
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) int64_t s64x64hi(int64_t a, int64_t b) {
if (!wjr::is_constant_evaluated()) {
#if defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64)
return __mulh(a, b);
#elif defined(WJR_HAS_INT128)
return (int64_t)(((__int128)a * (__int128)b) >> 64);
#elif defined(WJR_INLINE_ASM)
asm volatile("imulq %[u1]" : [u1] "+d"(a) : "a"(b) : "cc");
return a;
#endif
}
int32_t ah = a >> 32;
uint32_t al = a & 0xFFFFFFFF;
int32_t bh = b >> 32;
uint32_t bl = b & 0xFFFFFFFF;
uint32_t albl_hi = u32x32hi(al, bl);
int64_t t = ah * (int64_t)bl + albl_hi;
int64_t w1 = al * (int64_t)bh + (t & 0xFFFFFFFF);
return ah * (int64_t)bh + (t >> 32) + (w1 >> 32);
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint64_t s64x64lo(uint64_t a, uint64_t b) {
return a * b;
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint32_t s32x32hi(uint32_t a, uint32_t b) {
return (uint32_t)(((uint64_t)a * (uint64_t)b) >> 32);
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint32_t s32x32lo(uint32_t a, uint32_t b) {
return a * b;
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint16_t s16x16hi(uint16_t a, uint16_t b) {
return (uint16_t)(((uint32_t)a * (uint32_t)b) >> 16);
}
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) uint16_t s16x16lo(uint16_t a, uint16_t b) {
return a * b;
}
_WJR_ASM_END

#endif // __WJR_ASM_ASM_H
_WJR_ASM_BEGIN
uint64_t __large__u128d64t64(uint64_t hi, uint64_t lo, uint64_t den, uint64_t& r) {
constexpr uint64_t b = ((uint64_t)1 << 32);
if (hi >= den) {
r = ~0ull;
return ~0ull;
}
int shift = masm::clz(den);
den <<= shift;
hi <<= shift;
hi |= (lo >> (-shift & 63)) & (-(int64_t)shift >> 63);
lo <<= shift;
uint32_t num1 = (uint32_t)(lo >> 32);
uint32_t num0 = (uint32_t)(lo & 0xFFFFFFFFu);
uint32_t den1 = (uint32_t)(den >> 32);
uint32_t den0 = (uint32_t)(den & 0xFFFFFFFFu);
uint64_t qhat = hi / den1;
uint64_t rhat = hi % den1;
uint64_t c1 = qhat * den0;
uint64_t c2 = rhat * b + num1;
if (c1 > c2) qhat -= (c1 - c2 > den) ? 2 : 1;
uint32_t q1 = (uint32_t)qhat;
uint64_t rem = hi * b + num1 - q1 * den;
qhat = rem / den1;
rhat = rem % den1;
c1 = qhat * den0;
c2 = rhat * b + num0;
if (c1 > c2) qhat -= (c1 - c2 > den) ? 2 : 1;
uint32_t q0 = (uint32_t)qhat;
r = (rem * b + num0 - q0 * den) >> shift;
return ((uint64_t)q1 << 32) | q0;
}
_WJR_ASM_END


_WJR_BEGIN
#ifdef __cpp_lib_bit_cast
using std::bit_cast;
#else
template <
typename To,
typename From,
std::enable_if_t<
sizeof(From) == sizeof(To) && std::is_trivially_copyable<To>::value&&
std::is_trivially_copyable<From>::value,
int> = 0>
WJR_INTRINSIC_CONSTEXPR20 To bit_cast(const From & src) noexcept;
#endif
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
constexpr bool has_single_bit(T x) noexcept;
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countl_zero(T x) noexcept;
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countl_one(T x) noexcept;
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countr_zero(T x) noexcept;
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countr_one(T x) noexcept;
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int bit_width(T x) noexcept;
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) T bit_floor(T x) noexcept;
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) T bit_ceil(T x) noexcept;
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int popcount(T x) noexcept;
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(INTRINSIC_E_CONSTEXPR) T adc(T a, T b, T c, T& d) noexcept;
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(INTRINSIC_E_CONSTEXPR) T sbb(T a, T b, T c, T& d) noexcept;
template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T byteswap(T x) noexcept;
template<endian to = endian::native, typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T endian_convert(T x, endian from = endian::native) noexcept;
template<endian from, endian to, typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T endian_convert(T x) noexcept;
template<typename T, endian to = endian::native>
WJR_INTRINSIC_CONSTEXPR20 T read_bytes(const void* ptr) noexcept;
template< typename T, typename U >
constexpr bool cmp_equal(T t, U u) noexcept;
template< typename T, typename U >
constexpr bool cmp_not_equal(T t, U u) noexcept;
template< typename T, typename U >
constexpr bool cmp_less(T t, U u) noexcept;
template< typename T, typename U >
constexpr bool cmp_greater(T t, U u) noexcept;
template< typename T, typename U >
constexpr bool cmp_less_equal(T t, U u) noexcept;
template< typename T, typename U >
constexpr bool cmp_greater_equal(T t, U u) noexcept;
template< typename R, typename T>
constexpr bool in_range(T t) noexcept;
#define _WJR_USE_ASM_INT128(FUNC) using masm::FUNC;
WJR_MACRO_CALL(_WJR_USE_ASM_INT128, ,
u64x64,
u32x32,
u16x16,
u64x64hi,
u64x64lo,
u32x32hi,
u32x32lo,
u16x16hi,
u16x16lo,
u128d64t64,
u64d32t32,
u32d16t16,
u16d8t8,
s64x64hi,
s64x64lo,
s32x32hi,
s32x32lo,
s16x16hi,
s16x16lo
);
#undef _WJR_USE_ASM_INT128
template<typename T, std::enable_if_t<is_integrals_v<T>, int> = 0>
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) T mul(T a, T b, T& hi);
template<typename T, std::enable_if_t<is_integrals_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) T mulhi(T a, T b);
template<typename T, std::enable_if_t<is_integrals_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) T mullo(T a, T b);
template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE, CONSTEXPR) T power(T a, unsigned int b);
template<typename T, unsigned int base>
struct base_digits {
static_assert(std::is_integral_v<T>, "");
constexpr static unsigned int value = []() {
unsigned int ret = 0;
auto _Max = std::numeric_limits<T>::max();
while (_Max) {
++ret;
_Max /= base;
}
return ret;
}();
};
template<typename T, unsigned int base>
inline constexpr unsigned int base_digits_v = base_digits<T, base>::value;
template<unsigned int base, typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base_width(T a);
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base2_width(T a);
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base10_width(T a);
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base_width(unsigned int b, T a);
template<typename R, typename T>
struct broadcast_fn {};
template<typename R, typename T>
inline constexpr broadcast_fn<R, T> broadcast{};
_WJR_END
#ifndef __WJR_MATH_H
#error "This file should not be included directly. Include <wjr/math.h> instead."
#endif // !__WJR_MATH_H
#include <string.h>
#ifndef LIBDIVIDE_H
#define LIBDIVIDE_H
#define LIBDIVIDE_VERSION "5.0"
#define LIBDIVIDE_VERSION_MAJOR 5
#define LIBDIVIDE_VERSION_MINOR 0
#include <stdint.h>
#if !defined(__AVR__)
#include <stdio.h>
#include <stdlib.h>
#endif
#if defined(LIBDIVIDE_NEON)
#include <arm_neon.h>
#endif
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
#pragma warning(disable : 4204)
#define LIBDIVIDE_VC
#endif
#if defined(__SIZEOF_INT128__)
#define HAS_INT128_T
#if !(defined(__clang__) && defined(LIBDIVIDE_VC))
#define HAS_INT128_DIV
#endif
#endif
#if defined(WJR_X86_64)
#define LIBDIVIDE_X86_64
#endif
#if defined(__i386__)
#define LIBDIVIDE_i386
#endif
#if defined(WJR_INLINE_ASM)
#define LIBDIVIDE_GCC_STYLE_ASM
#endif
#if defined(__cplusplus) || defined(LIBDIVIDE_VC)
#define LIBDIVIDE_FUNCTION __FUNCTION__
#else
#define LIBDIVIDE_FUNCTION __func__
#endif
#ifdef __cplusplus
namespace wjr::libdivide {
#endif
#pragma pack(push, 1)
struct libdivide_u16_t {
uint16_t magic;
uint8_t more;
};
struct libdivide_s16_t {
int16_t magic;
uint8_t more;
};
struct libdivide_u32_t {
uint32_t magic;
uint8_t more;
};
struct libdivide_s32_t {
int32_t magic;
uint8_t more;
};
struct libdivide_u64_t {
uint64_t magic;
uint8_t more;
};
struct libdivide_s64_t {
int64_t magic;
uint8_t more;
};
struct libdivide_u16_branchfree_t {
uint16_t magic;
uint8_t more;
};
struct libdivide_s16_branchfree_t {
int16_t magic;
uint8_t more;
};
struct libdivide_u32_branchfree_t {
uint32_t magic;
uint8_t more;
};
struct libdivide_s32_branchfree_t {
int32_t magic;
uint8_t more;
};
struct libdivide_u64_branchfree_t {
uint64_t magic;
uint8_t more;
};
struct libdivide_s64_branchfree_t {
int64_t magic;
uint8_t more;
};
#pragma pack(pop)
enum {
LIBDIVIDE_16_SHIFT_MASK = 0x1F,
LIBDIVIDE_32_SHIFT_MASK = 0x1F,
LIBDIVIDE_64_SHIFT_MASK = 0x3F,
LIBDIVIDE_ADD_MARKER = 0x40,
LIBDIVIDE_NEGATIVE_DIVISOR = 0x80
};
static WJR_INTRINSIC_INLINE struct libdivide_s16_t libdivide_s16_gen(int16_t d);
static WJR_INTRINSIC_INLINE struct libdivide_u16_t libdivide_u16_gen(uint16_t d);
static WJR_INTRINSIC_INLINE struct libdivide_s32_t libdivide_s32_gen(int32_t d);
static WJR_INTRINSIC_INLINE struct libdivide_u32_t libdivide_u32_gen(uint32_t d);
static WJR_INTRINSIC_INLINE struct libdivide_s64_t libdivide_s64_gen(int64_t d);
static WJR_INTRINSIC_INLINE struct libdivide_u64_t libdivide_u64_gen(uint64_t d);
static WJR_INTRINSIC_INLINE struct libdivide_s16_branchfree_t libdivide_s16_branchfree_gen(int16_t d);
static WJR_INTRINSIC_INLINE struct libdivide_u16_branchfree_t libdivide_u16_branchfree_gen(uint16_t d);
static WJR_INTRINSIC_INLINE struct libdivide_s32_branchfree_t libdivide_s32_branchfree_gen(int32_t d);
static WJR_INTRINSIC_INLINE struct libdivide_u32_branchfree_t libdivide_u32_branchfree_gen(uint32_t d);
static WJR_INTRINSIC_INLINE struct libdivide_s64_branchfree_t libdivide_s64_branchfree_gen(int64_t d);
static WJR_INTRINSIC_INLINE struct libdivide_u64_branchfree_t libdivide_u64_branchfree_gen(uint64_t d);
static WJR_INTRINSIC_INLINE int16_t libdivide_s16_do_raw(
int16_t numer, int16_t magic, uint8_t more);
static WJR_INTRINSIC_INLINE int16_t libdivide_s16_do(
int16_t numer, const struct libdivide_s16_t* denom);
static WJR_INTRINSIC_INLINE uint16_t libdivide_u16_do_raw(
uint16_t numer, uint16_t magic, uint8_t more);
static WJR_INTRINSIC_INLINE uint16_t libdivide_u16_do(
uint16_t numer, const struct libdivide_u16_t* denom);
static WJR_INTRINSIC_INLINE int32_t libdivide_s32_do(
int32_t numer, const struct libdivide_s32_t* denom);
static WJR_INTRINSIC_INLINE uint32_t libdivide_u32_do(
uint32_t numer, const struct libdivide_u32_t* denom);
static WJR_INTRINSIC_INLINE int64_t libdivide_s64_do(
int64_t numer, const struct libdivide_s64_t* denom);
static WJR_INTRINSIC_INLINE uint64_t libdivide_u64_do(
uint64_t numer, const struct libdivide_u64_t* denom);
static WJR_INTRINSIC_INLINE int16_t libdivide_s16_branchfree_do(
int16_t numer, const struct libdivide_s16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE uint16_t libdivide_u16_branchfree_do(
uint16_t numer, const struct libdivide_u16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE int32_t libdivide_s32_branchfree_do(
int32_t numer, const struct libdivide_s32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE uint32_t libdivide_u32_branchfree_do(
uint32_t numer, const struct libdivide_u32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE int64_t libdivide_s64_branchfree_do(
int64_t numer, const struct libdivide_s64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE uint64_t libdivide_u64_branchfree_do(
uint64_t numer, const struct libdivide_u64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE int16_t libdivide_s16_recover(const struct libdivide_s16_t* denom);
static WJR_INTRINSIC_INLINE uint16_t libdivide_u16_recover(const struct libdivide_u16_t* denom);
static WJR_INTRINSIC_INLINE int32_t libdivide_s32_recover(const struct libdivide_s32_t* denom);
static WJR_INTRINSIC_INLINE uint32_t libdivide_u32_recover(const struct libdivide_u32_t* denom);
static WJR_INTRINSIC_INLINE int64_t libdivide_s64_recover(const struct libdivide_s64_t* denom);
static WJR_INTRINSIC_INLINE uint64_t libdivide_u64_recover(const struct libdivide_u64_t* denom);
static WJR_INTRINSIC_INLINE int16_t libdivide_s16_branchfree_recover(
const struct libdivide_s16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE uint16_t libdivide_u16_branchfree_recover(
const struct libdivide_u16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE int32_t libdivide_s32_branchfree_recover(
const struct libdivide_s32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE uint32_t libdivide_u32_branchfree_recover(
const struct libdivide_u32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE int64_t libdivide_s64_branchfree_recover(
const struct libdivide_s64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE uint64_t libdivide_u64_branchfree_recover(
const struct libdivide_u64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE void libdivide_u128_shift(
uint64_t* u1, uint64_t* u0, int32_t signed_shift) {
if (signed_shift > 0) {
uint32_t shift = signed_shift;
*u1 <<= shift;
*u1 |= *u0 >> (64 - shift);
*u0 <<= shift;
}
else if (signed_shift < 0) {
uint32_t shift = -signed_shift;
*u0 >>= shift;
*u0 |= *u1 << (64 - shift);
*u1 >>= shift;
}
}
static WJR_INTRINSIC_INLINE uint64_t libdivide_128_div_128_to_64(
uint64_t u_hi, uint64_t u_lo, uint64_t v_hi, uint64_t v_lo, uint64_t* r_hi, uint64_t* r_lo) {
#if defined(HAS_INT128_T) && defined(HAS_INT128_DIV)
__uint128_t ufull = u_hi;
__uint128_t vfull = v_hi;
ufull = (ufull << 64) | u_lo;
vfull = (vfull << 64) | v_lo;
uint64_t res = (uint64_t)(ufull / vfull);
__uint128_t remainder = ufull - (vfull * res);
*r_lo = (uint64_t)remainder;
*r_hi = (uint64_t)(remainder >> 64);
return res;
#else
typedef struct {
uint64_t hi;
uint64_t lo;
} u128_t;
u128_t u = { u_hi, u_lo };
u128_t v = { v_hi, v_lo };
if (v.hi == 0) {
*r_hi = 0;
return wjr::u128d64t64(u.hi, u.lo, v.lo, *r_lo);
}
uint32_t n = wjr::countl_zero(v.hi);
u128_t v1t = v;
libdivide_u128_shift(&v1t.hi, &v1t.lo, n);
uint64_t v1 = v1t.hi;  // i.e. v1 = v1t >> 64
u128_t u1 = u;
libdivide_u128_shift(&u1.hi, &u1.lo, -1);
uint64_t rem_ignored;
uint64_t q1 = wjr::u128d64t64(u1.hi, u1.lo, v1, rem_ignored);
u128_t q0 = { 0, q1 };
libdivide_u128_shift(&q0.hi, &q0.lo, n);
libdivide_u128_shift(&q0.hi, &q0.lo, -63);
if (q0.hi != 0 || q0.lo != 0) {
q0.hi -= (q0.lo == 0);  // borrow
q0.lo -= 1;
}
u128_t q0v = { 0, 0 };
q0v.hi = q0.hi * v.lo + q0.lo * v.hi + wjr::u64x64hi(q0.lo, v.lo);
q0v.lo = q0.lo * v.lo;
u128_t u_q0v = u;
u_q0v.hi -= q0v.hi + (u.lo < q0v.lo);  // second term is borrow
u_q0v.lo -= q0v.lo;
if ((u_q0v.hi > v.hi) || (u_q0v.hi == v.hi && u_q0v.lo >= v.lo)) {
q0.lo += 1;
q0.hi += (q0.lo == 0);  // carry
u_q0v.hi -= v.hi + (u_q0v.lo < v.lo);
u_q0v.lo -= v.lo;
}
*r_hi = u_q0v.hi;
*r_lo = u_q0v.lo;
return q0.lo;
#endif
}
static WJR_INTRINSIC_INLINE struct libdivide_u16_t libdivide_internal_u16_gen(
uint16_t d, int branchfree) {
struct libdivide_u16_t result;
uint8_t floor_log_2_d = (uint8_t)(15 - wjr::countl_zero(d));
if ((d & (d - 1)) == 0) {
result.magic = 0;
result.more = (uint8_t)(floor_log_2_d - (branchfree != 0));
}
else {
uint8_t more;
uint16_t rem, proposed_m;
proposed_m = wjr::u32d16t16((uint32_t)1 << (floor_log_2_d + 16), d, rem);
const uint16_t e = d - rem;
if (!branchfree && (e < ((uint16_t)1 << floor_log_2_d))) {
more = floor_log_2_d;
}
else {
proposed_m += proposed_m;
const uint16_t twice_rem = rem + rem;
if (twice_rem >= d || twice_rem < rem) proposed_m += 1;
more = floor_log_2_d | LIBDIVIDE_ADD_MARKER;
}
result.magic = 1 + proposed_m;
result.more = more;
}
return result;
}
struct libdivide_u16_t libdivide_u16_gen(uint16_t d) {
return libdivide_internal_u16_gen(d, 0);
}
struct libdivide_u16_branchfree_t libdivide_u16_branchfree_gen(uint16_t d) {
struct libdivide_u16_t tmp = libdivide_internal_u16_gen(d, 1);
struct libdivide_u16_branchfree_t ret = {
tmp.magic, (uint8_t)(tmp.more & LIBDIVIDE_16_SHIFT_MASK) };
return ret;
}
uint16_t libdivide_u16_do_raw(uint16_t numer, uint16_t magic, uint8_t more) {
if (!magic) {
return numer >> more;
}
else {
uint16_t q = wjr::u16x16hi(magic, numer);
if (more & LIBDIVIDE_ADD_MARKER) {
uint16_t t = ((numer - q) >> 1) + q;
return t >> (more & LIBDIVIDE_16_SHIFT_MASK);
}
else {
return q >> more;
}
}
}
uint16_t libdivide_u16_do(uint16_t numer, const struct libdivide_u16_t* denom) {
return libdivide_u16_do_raw(numer, denom->magic, denom->more);
}
uint16_t libdivide_u16_branchfree_do(
uint16_t numer, const struct libdivide_u16_branchfree_t* denom) {
uint16_t q = wjr::u16x16hi(denom->magic, numer);
uint16_t t = ((numer - q) >> 1) + q;
return t >> denom->more;
}
uint16_t libdivide_u16_recover(const struct libdivide_u16_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_16_SHIFT_MASK;
if (!denom->magic) {
return (uint16_t)1 << shift;
}
else if (!(more & LIBDIVIDE_ADD_MARKER)) {
uint16_t hi_dividend = (uint16_t)1 << shift;
uint16_t rem_ignored;
return 1 + wjr::u32d16t16((uint32_t)hi_dividend << 16, denom->magic, rem_ignored);
}
else {
uint32_t half_n = (uint32_t)1 << (16 + shift);
uint32_t d = ((uint32_t)1 << 16) | denom->magic;
uint16_t half_q = (uint16_t)(half_n / d);
uint32_t rem = half_n % d;
uint16_t full_q = half_q + half_q + ((rem << 1) >= d);
return full_q + 1;
}
}
uint16_t libdivide_u16_branchfree_recover(const struct libdivide_u16_branchfree_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_16_SHIFT_MASK;
if (!denom->magic) {
return (uint16_t)1 << (shift + 1);
}
else {
uint32_t half_n = (uint32_t)1 << (16 + shift);
uint32_t d = ((uint32_t)1 << 16) | denom->magic;
uint16_t half_q = (uint16_t)(half_n / d);
uint32_t rem = half_n % d;
uint16_t full_q = half_q + half_q + ((rem << 1) >= d);
return full_q + 1;
}
}
static WJR_INTRINSIC_INLINE struct libdivide_u32_t libdivide_internal_u32_gen(
uint32_t d, int branchfree) {
struct libdivide_u32_t result;
uint32_t floor_log_2_d = 31 - wjr::countl_zero(d);
if ((d & (d - 1)) == 0) {
result.magic = 0;
result.more = (uint8_t)(floor_log_2_d - (branchfree != 0));
}
else {
uint8_t more;
uint32_t rem, proposed_m;
proposed_m = wjr::u64d32t32((uint64_t)1 << (floor_log_2_d + 32), d, rem);
const uint32_t e = d - rem;
if (!branchfree && (e < ((uint32_t)1 << floor_log_2_d))) {
more = (uint8_t)floor_log_2_d;
}
else {
proposed_m += proposed_m;
const uint32_t twice_rem = rem + rem;
if (twice_rem >= d || twice_rem < rem) proposed_m += 1;
more = (uint8_t)(floor_log_2_d | LIBDIVIDE_ADD_MARKER);
}
result.magic = 1 + proposed_m;
result.more = more;
}
return result;
}
struct libdivide_u32_t libdivide_u32_gen(uint32_t d) {
return libdivide_internal_u32_gen(d, 0);
}
struct libdivide_u32_branchfree_t libdivide_u32_branchfree_gen(uint32_t d) {
struct libdivide_u32_t tmp = libdivide_internal_u32_gen(d, 1);
struct libdivide_u32_branchfree_t ret = {
tmp.magic, (uint8_t)(tmp.more & LIBDIVIDE_32_SHIFT_MASK) };
return ret;
}
uint32_t libdivide_u32_do(uint32_t numer, const struct libdivide_u32_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return numer >> more;
}
else {
uint32_t q = wjr::u32x32hi(denom->magic, numer);
if (more & LIBDIVIDE_ADD_MARKER) {
uint32_t t = ((numer - q) >> 1) + q;
return t >> (more & LIBDIVIDE_32_SHIFT_MASK);
}
else {
return q >> more;
}
}
}
uint32_t libdivide_u32_branchfree_do(
uint32_t numer, const struct libdivide_u32_branchfree_t* denom) {
uint32_t q = wjr::u32x32hi(denom->magic, numer);
uint32_t t = ((numer - q) >> 1) + q;
return t >> denom->more;
}
uint32_t libdivide_u32_recover(const struct libdivide_u32_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
if (!denom->magic) {
return (uint32_t)1 << shift;
}
else if (!(more & LIBDIVIDE_ADD_MARKER)) {
uint32_t hi_dividend = (uint32_t)1 << shift;
uint32_t rem_ignored;
return 1 + wjr::u64d32t32((uint64_t)hi_dividend << 32, denom->magic, rem_ignored);
}
else {
uint64_t half_n = (uint64_t)1 << (32 + shift);
uint64_t d = ((uint64_t)1 << 32) | denom->magic;
uint32_t half_q = (uint32_t)(half_n / d);
uint64_t rem = half_n % d;
uint32_t full_q = half_q + half_q + ((rem << 1) >= d);
return full_q + 1;
}
}
uint32_t libdivide_u32_branchfree_recover(const struct libdivide_u32_branchfree_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
if (!denom->magic) {
return (uint32_t)1 << (shift + 1);
}
else {
uint64_t half_n = (uint64_t)1 << (32 + shift);
uint64_t d = ((uint64_t)1 << 32) | denom->magic;
uint32_t half_q = (uint32_t)(half_n / d);
uint64_t rem = half_n % d;
uint32_t full_q = half_q + half_q + ((rem << 1) >= d);
return full_q + 1;
}
}
static WJR_INTRINSIC_INLINE struct libdivide_u64_t libdivide_internal_u64_gen(
uint64_t d, int branchfree) {
struct libdivide_u64_t result;
uint32_t floor_log_2_d = 63 - wjr::countl_zero(d);
if (has_single_bit(d)) {
result.magic = 0;
result.more = (uint8_t)(floor_log_2_d - (branchfree != 0));
}
else {
uint64_t proposed_m, rem;
uint8_t more;
proposed_m = wjr::u128d64t64((uint64_t)1 << floor_log_2_d, 0, d, rem);
const uint64_t e = d - rem;
if (!branchfree && e < ((uint64_t)1 << floor_log_2_d)) {
more = (uint8_t)floor_log_2_d;
}
else {
proposed_m += proposed_m;
const uint64_t twice_rem = rem + rem;
if (twice_rem >= d || twice_rem < rem) proposed_m += 1;
more = (uint8_t)(floor_log_2_d | LIBDIVIDE_ADD_MARKER);
}
result.magic = 1 + proposed_m;
result.more = more;
}
return result;
}
struct libdivide_u64_t libdivide_u64_gen(uint64_t d) {
return libdivide_internal_u64_gen(d, 0);
}
struct libdivide_u64_branchfree_t libdivide_u64_branchfree_gen(uint64_t d) {
struct libdivide_u64_t tmp = libdivide_internal_u64_gen(d, 1);
struct libdivide_u64_branchfree_t ret = {
tmp.magic, (uint8_t)(tmp.more & LIBDIVIDE_64_SHIFT_MASK) };
return ret;
}
uint64_t libdivide_u64_do(uint64_t numer, const struct libdivide_u64_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return numer >> more;
}
else {
uint64_t q = wjr::u64x64hi(denom->magic, numer);
if (more & LIBDIVIDE_ADD_MARKER) {
uint64_t t = ((numer - q) >> 1) + q;
return t >> (more & LIBDIVIDE_64_SHIFT_MASK);
}
else {
return q >> more;
}
}
}
uint64_t libdivide_u64_branchfree_do(
uint64_t numer, const struct libdivide_u64_branchfree_t* denom) {
uint64_t q = wjr::u64x64hi(denom->magic, numer);
uint64_t t = ((numer - q) >> 1) + q;
return t >> denom->more;
}
uint64_t libdivide_u64_recover(const struct libdivide_u64_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
if (!denom->magic) {
return (uint64_t)1 << shift;
}
else if (!(more & LIBDIVIDE_ADD_MARKER)) {
uint64_t hi_dividend = (uint64_t)1 << shift;
uint64_t rem_ignored;
return 1 + wjr::u128d64t64(hi_dividend, 0, denom->magic, rem_ignored);
}
else {
uint64_t half_n_hi = (uint64_t)1 << shift, half_n_lo = 0;
const uint64_t d_hi = 1, d_lo = denom->magic;
uint64_t r_hi, r_lo;
uint64_t half_q =
libdivide_128_div_128_to_64(half_n_hi, half_n_lo, d_hi, d_lo, &r_hi, &r_lo);
uint64_t dr_lo = r_lo + r_lo;
uint64_t dr_hi = r_hi + r_hi + (dr_lo < r_lo);  // last term is carry
int dr_exceeds_d = (dr_hi > d_hi) || (dr_hi == d_hi && dr_lo >= d_lo);
uint64_t full_q = half_q + half_q + (dr_exceeds_d ? 1 : 0);
return full_q + 1;
}
}
uint64_t libdivide_u64_branchfree_recover(const struct libdivide_u64_branchfree_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
if (!denom->magic) {
return (uint64_t)1 << (shift + 1);
}
else {
uint64_t half_n_hi = (uint64_t)1 << shift, half_n_lo = 0;
const uint64_t d_hi = 1, d_lo = denom->magic;
uint64_t r_hi, r_lo;
uint64_t half_q =
libdivide_128_div_128_to_64(half_n_hi, half_n_lo, d_hi, d_lo, &r_hi, &r_lo);
uint64_t dr_lo = r_lo + r_lo;
uint64_t dr_hi = r_hi + r_hi + (dr_lo < r_lo);  // last term is carry
int dr_exceeds_d = (dr_hi > d_hi) || (dr_hi == d_hi && dr_lo >= d_lo);
uint64_t full_q = half_q + half_q + (dr_exceeds_d ? 1 : 0);
return full_q + 1;
}
}
static WJR_INTRINSIC_INLINE struct libdivide_s16_t libdivide_internal_s16_gen(
int16_t d, int branchfree) {
struct libdivide_s16_t result;
uint16_t ud = (uint16_t)d;
uint16_t absD = (d < 0) ? -ud : ud;
uint16_t floor_log_2_d = 15 - wjr::countl_zero(absD);
if ((absD & (absD - 1)) == 0) {
result.magic = 0;
result.more = (uint8_t)(floor_log_2_d | (d < 0 ? LIBDIVIDE_NEGATIVE_DIVISOR : 0));
}
else {
uint8_t more;
uint16_t rem, proposed_m;
proposed_m = wjr::u32d16t16((uint32_t)(1 << (floor_log_2_d - 1 + 16)) , absD, rem);
const uint16_t e = absD - rem;
if (!branchfree && e < ((uint16_t)1 << floor_log_2_d)) {
more = (uint8_t)(floor_log_2_d - 1);
}
else {
proposed_m += proposed_m;
const uint16_t twice_rem = rem + rem;
if (twice_rem >= absD || twice_rem < rem) proposed_m += 1;
more = (uint8_t)(floor_log_2_d | LIBDIVIDE_ADD_MARKER);
}
proposed_m += 1;
int16_t magic = (int16_t)proposed_m;
if (d < 0) {
more |= LIBDIVIDE_NEGATIVE_DIVISOR;
if (!branchfree) {
magic = -magic;
}
}
result.more = more;
result.magic = magic;
}
return result;
}
struct libdivide_s16_t libdivide_s16_gen(int16_t d) {
return libdivide_internal_s16_gen(d, 0);
}
struct libdivide_s16_branchfree_t libdivide_s16_branchfree_gen(int16_t d) {
struct libdivide_s16_t tmp = libdivide_internal_s16_gen(d, 1);
struct libdivide_s16_branchfree_t result = { tmp.magic, tmp.more };
return result;
}
int16_t libdivide_s16_do_raw(int16_t numer, int16_t magic, uint8_t more) {
uint8_t shift = more & LIBDIVIDE_16_SHIFT_MASK;
if (!magic) {
uint16_t sign = (int8_t)more >> 7;
uint16_t mask = ((uint16_t)1 << shift) - 1;
uint16_t uq = numer + ((numer >> 15) & mask);
int16_t q = (int16_t)uq;
q >>= shift;
q = (q ^ sign) - sign;
return q;
}
else {
uint16_t uq = (uint16_t)wjr::s16x16hi(magic, numer);
if (more & LIBDIVIDE_ADD_MARKER) {
int16_t sign = (int8_t)more >> 7;
uq += ((uint16_t)numer ^ sign) - sign;
}
int16_t q = (int16_t)uq;
q >>= shift;
q += (q < 0);
return q;
}
}
int16_t libdivide_s16_do(int16_t numer, const struct libdivide_s16_t* denom) {
return libdivide_s16_do_raw(numer, denom->magic, denom->more);
}
int16_t libdivide_s16_branchfree_do(int16_t numer, const struct libdivide_s16_branchfree_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_16_SHIFT_MASK;
int16_t sign = (int8_t)more >> 7;
int16_t magic = denom->magic;
int16_t q = wjr::s16x16hi(magic, numer);
q += numer;
uint16_t is_power_of_2 = (magic == 0);
uint16_t q_sign = (uint16_t)(q >> 15);
q += q_sign & (((uint16_t)1 << shift) - is_power_of_2);
q >>= shift;
q = (q ^ sign) - sign;
return q;
}
int16_t libdivide_s16_recover(const struct libdivide_s16_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_16_SHIFT_MASK;
if (!denom->magic) {
uint16_t absD = (uint16_t)1 << shift;
if (more & LIBDIVIDE_NEGATIVE_DIVISOR) {
absD = -absD;
}
return (int16_t)absD;
}
else {
int negative_divisor = (more & LIBDIVIDE_NEGATIVE_DIVISOR);
int magic_was_negated = (more & LIBDIVIDE_ADD_MARKER) ? denom->magic > 0 : denom->magic < 0;
if (denom->magic == 0) {
int16_t result = (uint16_t)1 << shift;
return negative_divisor ? -result : result;
}
uint16_t d = (uint16_t)(magic_was_negated ? -denom->magic : denom->magic);
uint32_t n = (uint32_t)1 << (16 + shift);  // this shift cannot exceed 30
uint16_t q = (uint16_t)(n / d);
int16_t result = (int16_t)q;
result += 1;
return negative_divisor ? -result : result;
}
}
int16_t libdivide_s16_branchfree_recover(const struct libdivide_s16_branchfree_t* denom) {
return libdivide_s16_recover((const struct libdivide_s16_t*)denom);
}
static WJR_INTRINSIC_INLINE struct libdivide_s32_t libdivide_internal_s32_gen(
int32_t d, int branchfree) {
struct libdivide_s32_t result;
uint32_t ud = (uint32_t)d;
uint32_t absD = (d < 0) ? -ud : ud;
uint32_t floor_log_2_d = 31 - wjr::countl_zero(absD);
if ((absD & (absD - 1)) == 0) {
result.magic = 0;
result.more = (uint8_t)(floor_log_2_d | (d < 0 ? LIBDIVIDE_NEGATIVE_DIVISOR : 0));
}
else {
uint8_t more;
uint32_t rem, proposed_m;
proposed_m = wjr::u64d32t32((uint64_t)(1 << (floor_log_2_d - 1 + 16)),absD, rem);
const uint32_t e = absD - rem;
if (!branchfree && e < ((uint32_t)1 << floor_log_2_d)) {
more = (uint8_t)(floor_log_2_d - 1);
}
else {
proposed_m += proposed_m;
const uint32_t twice_rem = rem + rem;
if (twice_rem >= absD || twice_rem < rem) proposed_m += 1;
more = (uint8_t)(floor_log_2_d | LIBDIVIDE_ADD_MARKER);
}
proposed_m += 1;
int32_t magic = (int32_t)proposed_m;
if (d < 0) {
more |= LIBDIVIDE_NEGATIVE_DIVISOR;
if (!branchfree) {
magic = -magic;
}
}
result.more = more;
result.magic = magic;
}
return result;
}
struct libdivide_s32_t libdivide_s32_gen(int32_t d) {
return libdivide_internal_s32_gen(d, 0);
}
struct libdivide_s32_branchfree_t libdivide_s32_branchfree_gen(int32_t d) {
struct libdivide_s32_t tmp = libdivide_internal_s32_gen(d, 1);
struct libdivide_s32_branchfree_t result = { tmp.magic, tmp.more };
return result;
}
int32_t libdivide_s32_do(int32_t numer, const struct libdivide_s32_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
if (!denom->magic) {
uint32_t sign = (int8_t)more >> 7;
uint32_t mask = ((uint32_t)1 << shift) - 1;
uint32_t uq = numer + ((numer >> 31) & mask);
int32_t q = (int32_t)uq;
q >>= shift;
q = (q ^ sign) - sign;
return q;
}
else {
uint32_t uq = (uint32_t)wjr::s32x32hi(denom->magic, numer);
if (more & LIBDIVIDE_ADD_MARKER) {
int32_t sign = (int8_t)more >> 7;
uq += ((uint32_t)numer ^ sign) - sign;
}
int32_t q = (int32_t)uq;
q >>= shift;
q += (q < 0);
return q;
}
}
int32_t libdivide_s32_branchfree_do(int32_t numer, const struct libdivide_s32_branchfree_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
int32_t sign = (int8_t)more >> 7;
int32_t magic = denom->magic;
int32_t q = wjr::s32x32hi(magic, numer);
q += numer;
uint32_t is_power_of_2 = (magic == 0);
uint32_t q_sign = (uint32_t)(q >> 31);
q += q_sign & (((uint32_t)1 << shift) - is_power_of_2);
q >>= shift;
q = (q ^ sign) - sign;
return q;
}
int32_t libdivide_s32_recover(const struct libdivide_s32_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
if (!denom->magic) {
uint32_t absD = (uint32_t)1 << shift;
if (more & LIBDIVIDE_NEGATIVE_DIVISOR) {
absD = -absD;
}
return (int32_t)absD;
}
else {
int negative_divisor = (more & LIBDIVIDE_NEGATIVE_DIVISOR);
int magic_was_negated = (more & LIBDIVIDE_ADD_MARKER) ? denom->magic > 0 : denom->magic < 0;
if (denom->magic == 0) {
int32_t result = (uint32_t)1 << shift;
return negative_divisor ? -result : result;
}
uint32_t d = (uint32_t)(magic_was_negated ? -denom->magic : denom->magic);
uint64_t n = (uint64_t)1 << (32 + shift);  // this shift cannot exceed 30
uint32_t q = (uint32_t)(n / d);
int32_t result = (int32_t)q;
result += 1;
return negative_divisor ? -result : result;
}
}
int32_t libdivide_s32_branchfree_recover(const struct libdivide_s32_branchfree_t* denom) {
return libdivide_s32_recover((const struct libdivide_s32_t*)denom);
}
static WJR_INTRINSIC_INLINE struct libdivide_s64_t libdivide_internal_s64_gen(
int64_t d, int branchfree) {
struct libdivide_s64_t result;
uint64_t ud = (uint64_t)d;
uint64_t absD = (d < 0) ? -ud : ud;
uint32_t floor_log_2_d = 63 - wjr::countl_zero(absD);
if ((absD & (absD - 1)) == 0) {
result.magic = 0;
result.more = (uint8_t)(floor_log_2_d | (d < 0 ? LIBDIVIDE_NEGATIVE_DIVISOR : 0));
}
else {
uint8_t more;
uint64_t rem, proposed_m;
proposed_m = wjr::u128d64t64((uint64_t)1 << (floor_log_2_d - 1), 0, absD, rem);
const uint64_t e = absD - rem;
if (!branchfree && e < ((uint64_t)1 << floor_log_2_d)) {
more = (uint8_t)(floor_log_2_d - 1);
}
else {
proposed_m += proposed_m;
const uint64_t twice_rem = rem + rem;
if (twice_rem >= absD || twice_rem < rem) proposed_m += 1;
more = (uint8_t)(floor_log_2_d | LIBDIVIDE_ADD_MARKER);
}
proposed_m += 1;
int64_t magic = (int64_t)proposed_m;
if (d < 0) {
more |= LIBDIVIDE_NEGATIVE_DIVISOR;
if (!branchfree) {
magic = -magic;
}
}
result.more = more;
result.magic = magic;
}
return result;
}
struct libdivide_s64_t libdivide_s64_gen(int64_t d) {
return libdivide_internal_s64_gen(d, 0);
}
struct libdivide_s64_branchfree_t libdivide_s64_branchfree_gen(int64_t d) {
struct libdivide_s64_t tmp = libdivide_internal_s64_gen(d, 1);
struct libdivide_s64_branchfree_t ret = { tmp.magic, tmp.more };
return ret;
}
int64_t libdivide_s64_do(int64_t numer, const struct libdivide_s64_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
if (!denom->magic) {  // shift path
uint64_t mask = ((uint64_t)1 << shift) - 1;
uint64_t uq = numer + ((numer >> 63) & mask);
int64_t q = (int64_t)uq;
q >>= shift;
int64_t sign = (int8_t)more >> 7;
q = (q ^ sign) - sign;
return q;
}
else {
uint64_t uq = (uint64_t)wjr::s64x64hi(denom->magic, numer);
if (more & LIBDIVIDE_ADD_MARKER) {
int64_t sign = (int8_t)more >> 7;
uq += ((uint64_t)numer ^ sign) - sign;
}
int64_t q = (int64_t)uq;
q >>= shift;
q += (q < 0);
return q;
}
}
int64_t libdivide_s64_branchfree_do(int64_t numer, const struct libdivide_s64_branchfree_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
int64_t sign = (int8_t)more >> 7;
int64_t magic = denom->magic;
int64_t q = wjr::s64x64hi(magic, numer);
q += numer;
uint64_t is_power_of_2 = (magic == 0);
uint64_t q_sign = (uint64_t)(q >> 63);
q += q_sign & (((uint64_t)1 << shift) - is_power_of_2);
q >>= shift;
q = (q ^ sign) - sign;
return q;
}
int64_t libdivide_s64_recover(const struct libdivide_s64_t* denom) {
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
if (denom->magic == 0) {  // shift path
uint64_t absD = (uint64_t)1 << shift;
if (more & LIBDIVIDE_NEGATIVE_DIVISOR) {
absD = -absD;
}
return (int64_t)absD;
}
else {
int negative_divisor = (more & LIBDIVIDE_NEGATIVE_DIVISOR);
int magic_was_negated = (more & LIBDIVIDE_ADD_MARKER) ? denom->magic > 0 : denom->magic < 0;
uint64_t d = (uint64_t)(magic_was_negated ? -denom->magic : denom->magic);
uint64_t n_hi = (uint64_t)1 << shift, n_lo = 0;
uint64_t rem_ignored;
uint64_t q = wjr::u128d64t64(n_hi, n_lo, d, rem_ignored);
int64_t result = (int64_t)(q + 1);
if (negative_divisor) {
result = -result;
}
return result;
}
}
int64_t libdivide_s64_branchfree_recover(const struct libdivide_s64_branchfree_t* denom) {
return libdivide_s64_recover((const struct libdivide_s64_t*)denom);
}
#define SIMPLE_VECTOR_DIVISION(IntT, VecT, Algo) \
const size_t count = sizeof(VecT) / sizeof(IntT); \
union type_pun_vec { \
VecT vec; \
IntT arr[sizeof(VecT) / sizeof(IntT)]; \
}; \
union type_pun_vec result; \
union type_pun_vec input; \
input.vec = numers; \
for (size_t loop=0; loop<count; ++loop) { \
result.arr[loop] = libdivide_##Algo##_do(input.arr[loop], denom); \
} \
return result.vec;
#if defined(LIBDIVIDE_NEON)
static WJR_INTRINSIC_INLINE uint16x8_t libdivide_u16_do_vec128(
uint16x8_t numers, const struct libdivide_u16_t* denom);
static WJR_INTRINSIC_INLINE int16x8_t libdivide_s16_do_vec128(
int16x8_t numers, const struct libdivide_s16_t* denom);
static WJR_INTRINSIC_INLINE uint32x4_t libdivide_u32_do_vec128(
uint32x4_t numers, const struct libdivide_u32_t* denom);
static WJR_INTRINSIC_INLINE int32x4_t libdivide_s32_do_vec128(
int32x4_t numers, const struct libdivide_s32_t* denom);
static WJR_INTRINSIC_INLINE uint64x2_t libdivide_u64_do_vec128(
uint64x2_t numers, const struct libdivide_u64_t* denom);
static WJR_INTRINSIC_INLINE int64x2_t libdivide_s64_do_vec128(
int64x2_t numers, const struct libdivide_s64_t* denom);
static WJR_INTRINSIC_INLINE uint16x8_t libdivide_u16_branchfree_do_vec128(
uint16x8_t numers, const struct libdivide_u16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE int16x8_t libdivide_s16_branchfree_do_vec128(
int16x8_t numers, const struct libdivide_s16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE uint32x4_t libdivide_u32_branchfree_do_vec128(
uint32x4_t numers, const struct libdivide_u32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE int32x4_t libdivide_s32_branchfree_do_vec128(
int32x4_t numers, const struct libdivide_s32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE uint64x2_t libdivide_u64_branchfree_do_vec128(
uint64x2_t numers, const struct libdivide_u64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE int64x2_t libdivide_s64_branchfree_do_vec128(
int64x2_t numers, const struct libdivide_s64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE uint32x4_t libdivide_u32_neon_srl(uint32x4_t v, uint8_t amt) {
int32_t wamt = (int32_t)(amt);
return vshlq_u32(v, vdupq_n_s32(-wamt));
}
static WJR_INTRINSIC_INLINE uint64x2_t libdivide_u64_neon_srl(uint64x2_t v, uint8_t amt) {
int64_t wamt = (int64_t)(amt);
return vshlq_u64(v, vdupq_n_s64(-wamt));
}
static WJR_INTRINSIC_INLINE int32x4_t libdivide_s32_neon_sra(int32x4_t v, uint8_t amt) {
int32_t wamt = (int32_t)(amt);
return vshlq_s32(v, vdupq_n_s32(-wamt));
}
static WJR_INTRINSIC_INLINE int64x2_t libdivide_s64_neon_sra(int64x2_t v, uint8_t amt) {
int64_t wamt = (int64_t)(amt);
return vshlq_s64(v, vdupq_n_s64(-wamt));
}
static WJR_INTRINSIC_INLINE int64x2_t libdivide_s64_signbits(int64x2_t v) { return vshrq_n_s64(v, 63); }
static WJR_INTRINSIC_INLINE uint32x4_t libdivide_mullhi_u32_vec128(uint32x4_t a, uint32_t b) {
uint32x4_t w1 = vreinterpretq_u32_u64(vmull_n_u32(vget_low_u32(a), b));  // [_, x0, _, x1]
uint32x4_t w2 = vreinterpretq_u32_u64(vmull_high_n_u32(a, b));           //[_, x2, _, x3]
return vuzp2q_u32(w1, w2);                                               // [x0, x1, x2, x3]
}
static WJR_INTRINSIC_INLINE int32x4_t libdivide_mullhi_s32_vec128(int32x4_t a, int32_t b) {
int32x4_t w1 = vreinterpretq_s32_s64(vmull_n_s32(vget_low_s32(a), b));  // [_, x0, _, x1]
int32x4_t w2 = vreinterpretq_s32_s64(vmull_high_n_s32(a, b));           //[_, x2, _, x3]
return vuzp2q_s32(w1, w2);                                              // [x0, x1, x2, x3]
}
static WJR_INTRINSIC_INLINE uint64x2_t libdivide_mullhi_u64_vec128(uint64x2_t x, uint64_t sy) {
uint64x2_t y = vdupq_n_u64(sy);
uint32x2_t x0 = vmovn_u64(x);
uint32x2_t y0 = vmovn_u64(y);
uint32x2_t x1 = vshrn_n_u64(x, 32);
uint32x2_t y1 = vshrn_n_u64(y, 32);
uint64x2_t x0y0 = vmull_u32(x0, y0);
uint64x2_t x0y0_hi = vshrq_n_u64(x0y0, 32);
uint64x2_t temp = vmlal_u32(x0y0_hi, x1, y0);  // temp = x0y0_hi + x1*y0;
uint64x2_t temp_lo = vshrq_n_u64(vshlq_n_u64(temp, 32), 32);  // temp_lo = temp & 0xFFFFFFFF;
uint64x2_t temp_hi = vshrq_n_u64(temp, 32);                   // temp_hi = temp >> 32;
temp_lo = vmlal_u32(temp_lo, x0, y1);  // temp_lo += x0*y0
temp_lo = vshrq_n_u64(temp_lo, 32);    // temp_lo >>= 32
temp_hi = vmlal_u32(temp_hi, x1, y1);  // temp_hi += x1*y1
uint64x2_t result = vaddq_u64(temp_hi, temp_lo);
return result;
}
static WJR_INTRINSIC_INLINE int64x2_t libdivide_mullhi_s64_vec128(int64x2_t x, int64_t sy) {
int64x2_t p = vreinterpretq_s64_u64(
libdivide_mullhi_u64_vec128(vreinterpretq_u64_s64(x), (uint64_t)(sy)));
int64x2_t y = vdupq_n_s64(sy);
int64x2_t t1 = vandq_s64(libdivide_s64_signbits(x), y);
int64x2_t t2 = vandq_s64(libdivide_s64_signbits(y), x);
p = vsubq_s64(p, t1);
p = vsubq_s64(p, t2);
return p;
}
uint16x8_t libdivide_u16_do_vec128(uint16x8_t numers, const struct libdivide_u16_t* denom) {
SIMPLE_VECTOR_DIVISION(uint16_t, uint16x8_t, u16)
}
uint16x8_t libdivide_u16_branchfree_do_vec128(uint16x8_t numers, const struct libdivide_u16_branchfree_t* denom) {
SIMPLE_VECTOR_DIVISION(uint16_t, uint16x8_t, u16_branchfree)
}
uint32x4_t libdivide_u32_do_vec128(uint32x4_t numers, const struct libdivide_u32_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return libdivide_u32_neon_srl(numers, more);
}
else {
uint32x4_t q = libdivide_mullhi_u32_vec128(numers, denom->magic);
if (more & LIBDIVIDE_ADD_MARKER) {
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
uint32x4_t t = vaddq_u32(vhsubq_u32(numers, q), q);
return libdivide_u32_neon_srl(t, shift);
}
else {
return libdivide_u32_neon_srl(q, more);
}
}
}
uint32x4_t libdivide_u32_branchfree_do_vec128(
uint32x4_t numers, const struct libdivide_u32_branchfree_t* denom) {
uint32x4_t q = libdivide_mullhi_u32_vec128(numers, denom->magic);
uint32x4_t t = vaddq_u32(vhsubq_u32(numers, q), q);
return libdivide_u32_neon_srl(t, denom->more);
}
uint64x2_t libdivide_u64_do_vec128(uint64x2_t numers, const struct libdivide_u64_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return libdivide_u64_neon_srl(numers, more);
}
else {
uint64x2_t q = libdivide_mullhi_u64_vec128(numers, denom->magic);
if (more & LIBDIVIDE_ADD_MARKER) {
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
uint64x2_t t = vaddq_u64(vshrq_n_u64(vsubq_u64(numers, q), 1), q);
return libdivide_u64_neon_srl(t, shift);
}
else {
return libdivide_u64_neon_srl(q, more);
}
}
}
uint64x2_t libdivide_u64_branchfree_do_vec128(
uint64x2_t numers, const struct libdivide_u64_branchfree_t* denom) {
uint64x2_t q = libdivide_mullhi_u64_vec128(numers, denom->magic);
uint64x2_t t = vaddq_u64(vshrq_n_u64(vsubq_u64(numers, q), 1), q);
return libdivide_u64_neon_srl(t, denom->more);
}
int16x8_t libdivide_s16_do_vec128(int16x8_t numers, const struct libdivide_s16_t* denom) {
SIMPLE_VECTOR_DIVISION(int16_t, int16x8_t, s16)
}
int16x8_t libdivide_s16_branchfree_do_vec128(int16x8_t numers, const struct libdivide_s16_branchfree_t* denom) {
SIMPLE_VECTOR_DIVISION(int16_t, int16x8_t, s16_branchfree)
}
int32x4_t libdivide_s32_do_vec128(int32x4_t numers, const struct libdivide_s32_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
uint32_t mask = ((uint32_t)1 << shift) - 1;
int32x4_t roundToZeroTweak = vdupq_n_s32((int)mask);
int32x4_t q = vaddq_s32(numers, vandq_s32(vshrq_n_s32(numers, 31), roundToZeroTweak));
q = libdivide_s32_neon_sra(q, shift);
int32x4_t sign = vdupq_n_s32((int8_t)more >> 7);
q = vsubq_s32(veorq_s32(q, sign), sign);
return q;
}
else {
int32x4_t q = libdivide_mullhi_s32_vec128(numers, denom->magic);
if (more & LIBDIVIDE_ADD_MARKER) {
int32x4_t sign = vdupq_n_s32((int8_t)more >> 7);
q = vaddq_s32(q, vsubq_s32(veorq_s32(numers, sign), sign));
}
q = libdivide_s32_neon_sra(q, more & LIBDIVIDE_32_SHIFT_MASK);
q = vaddq_s32(
q, vreinterpretq_s32_u32(vshrq_n_u32(vreinterpretq_u32_s32(q), 31)));  // q += (q < 0)
return q;
}
}
int32x4_t libdivide_s32_branchfree_do_vec128(
int32x4_t numers, const struct libdivide_s32_branchfree_t* denom) {
int32_t magic = denom->magic;
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
int32x4_t sign = vdupq_n_s32((int8_t)more >> 7);
int32x4_t q = libdivide_mullhi_s32_vec128(numers, magic);
q = vaddq_s32(q, numers);  // q += numers
uint32_t is_power_of_2 = (magic == 0);
int32x4_t q_sign = vshrq_n_s32(q, 31);  // q_sign = q >> 31
int32x4_t mask = vdupq_n_s32(((uint32_t)1 << shift) - is_power_of_2);
q = vaddq_s32(q, vandq_s32(q_sign, mask));  // q = q + (q_sign & mask)
q = libdivide_s32_neon_sra(q, shift);       // q >>= shift
q = vsubq_s32(veorq_s32(q, sign), sign);    // q = (q ^ sign) - sign
return q;
}
int64x2_t libdivide_s64_do_vec128(int64x2_t numers, const struct libdivide_s64_t* denom) {
uint8_t more = denom->more;
int64_t magic = denom->magic;
if (magic == 0) {  // shift path
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
uint64_t mask = ((uint64_t)1 << shift) - 1;
int64x2_t roundToZeroTweak = vdupq_n_s64(mask);  // TODO: no need to sign extend
int64x2_t q =
vaddq_s64(numers, vandq_s64(libdivide_s64_signbits(numers), roundToZeroTweak));
q = libdivide_s64_neon_sra(q, shift);
int64x2_t sign = vreinterpretq_s64_s8(vdupq_n_s8((int8_t)more >> 7));
q = vsubq_s64(veorq_s64(q, sign), sign);
return q;
}
else {
int64x2_t q = libdivide_mullhi_s64_vec128(numers, magic);
if (more & LIBDIVIDE_ADD_MARKER) {
int64x2_t sign = vdupq_n_s64((int8_t)more >> 7);  // TODO: no need to widen
q = vaddq_s64(q, vsubq_s64(veorq_s64(numers, sign), sign));
}
q = libdivide_s64_neon_sra(q, more & LIBDIVIDE_64_SHIFT_MASK);
q = vaddq_s64(
q, vreinterpretq_s64_u64(vshrq_n_u64(vreinterpretq_u64_s64(q), 63)));  // q += (q < 0)
return q;
}
}
int64x2_t libdivide_s64_branchfree_do_vec128(
int64x2_t numers, const struct libdivide_s64_branchfree_t* denom) {
int64_t magic = denom->magic;
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
int64x2_t sign = vdupq_n_s64((int8_t)more >> 7);  // TODO: avoid sign extend
int64x2_t q = libdivide_mullhi_s64_vec128(numers, magic);
q = vaddq_s64(q, numers);  // q += numers
uint32_t is_power_of_2 = (magic == 0);
int64x2_t q_sign = libdivide_s64_signbits(q);  // q_sign = q >> 63
int64x2_t mask = vdupq_n_s64(((uint64_t)1 << shift) - is_power_of_2);
q = vaddq_s64(q, vandq_s64(q_sign, mask));  // q = q + (q_sign & mask)
q = libdivide_s64_neon_sra(q, shift);       // q >>= shift
q = vsubq_s64(veorq_s64(q, sign), sign);    // q = (q ^ sign) - sign
return q;
}
#endif
#if defined(LIBDIVIDE_AVX512)
static WJR_INTRINSIC_INLINE __m512i libdivide_u16_do_vec512(
__m512i numers, const struct libdivide_u16_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_s16_do_vec512(
__m512i numers, const struct libdivide_s16_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_u32_do_vec512(
__m512i numers, const struct libdivide_u32_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_s32_do_vec512(
__m512i numers, const struct libdivide_s32_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_u64_do_vec512(
__m512i numers, const struct libdivide_u64_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_s64_do_vec512(
__m512i numers, const struct libdivide_s64_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_u16_branchfree_do_vec512(
__m512i numers, const struct libdivide_u16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_s16_branchfree_do_vec512(
__m512i numers, const struct libdivide_s16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_u32_branchfree_do_vec512(
__m512i numers, const struct libdivide_u32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_s32_branchfree_do_vec512(
__m512i numers, const struct libdivide_s32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_u64_branchfree_do_vec512(
__m512i numers, const struct libdivide_u64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_s64_branchfree_do_vec512(
__m512i numers, const struct libdivide_s64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m512i libdivide_s64_signbits_vec512(__m512i v) {
;
return _mm512_srai_epi64(v, 63);
}
static WJR_INTRINSIC_INLINE __m512i libdivide_s64_shift_right_vec512(__m512i v, int amt) {
return _mm512_srai_epi64(v, amt);
}
static WJR_INTRINSIC_INLINE __m512i libdivide_mullhi_u32_vec512(__m512i a, __m512i b) {
__m512i hi_product_0Z2Z = _mm512_srli_epi64(_mm512_mul_epu32(a, b), 32);
__m512i a1X3X = _mm512_srli_epi64(a, 32);
__m512i mask = _mm512_set_epi32(-1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0);
__m512i hi_product_Z1Z3 = _mm512_and_si512(_mm512_mul_epu32(a1X3X, b), mask);
return _mm512_or_si512(hi_product_0Z2Z, hi_product_Z1Z3);
}
static WJR_INTRINSIC_INLINE __m512i libdivide_mullhi_s32_vec512(__m512i a, __m512i b) {
__m512i hi_product_0Z2Z = _mm512_srli_epi64(_mm512_mul_epi32(a, b), 32);
__m512i a1X3X = _mm512_srli_epi64(a, 32);
__m512i mask = _mm512_set_epi32(-1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0);
__m512i hi_product_Z1Z3 = _mm512_and_si512(_mm512_mul_epi32(a1X3X, b), mask);
return _mm512_or_si512(hi_product_0Z2Z, hi_product_Z1Z3);
}
static WJR_INTRINSIC_INLINE __m512i libdivide_mullhi_u64_vec512(__m512i x, __m512i y) {
__m512i x0y0 = _mm512_mul_epu32(x, y);
__m512i x0y0_hi = _mm512_srli_epi64(x0y0, 32);
__m512i x1 = _mm512_shuffle_epi32(x, (_MM_PERM_ENUM)_MM_SHUFFLE(3, 3, 1, 1));
__m512i y1 = _mm512_shuffle_epi32(y, (_MM_PERM_ENUM)_MM_SHUFFLE(3, 3, 1, 1));
__m512i x0y1 = _mm512_mul_epu32(x, y1);
__m512i x1y0 = _mm512_mul_epu32(x1, y);
__m512i x1y1 = _mm512_mul_epu32(x1, y1);
__m512i mask = _mm512_set1_epi64(0xFFFFFFFF);
__m512i temp = _mm512_add_epi64(x1y0, x0y0_hi);
__m512i temp_lo = _mm512_and_si512(temp, mask);
__m512i temp_hi = _mm512_srli_epi64(temp, 32);
temp_lo = _mm512_srli_epi64(_mm512_add_epi64(temp_lo, x0y1), 32);
temp_hi = _mm512_add_epi64(x1y1, temp_hi);
return _mm512_add_epi64(temp_lo, temp_hi);
}
static WJR_INTRINSIC_INLINE __m512i libdivide_mullhi_s64_vec512(__m512i x, __m512i y) {
__m512i p = libdivide_mullhi_u64_vec512(x, y);
__m512i t1 = _mm512_and_si512(libdivide_s64_signbits_vec512(x), y);
__m512i t2 = _mm512_and_si512(libdivide_s64_signbits_vec512(y), x);
p = _mm512_sub_epi64(p, t1);
p = _mm512_sub_epi64(p, t2);
return p;
}
__m512i libdivide_u16_do_vec512(__m512i numers, const struct libdivide_u16_t* denom) {
SIMPLE_VECTOR_DIVISION(uint16_t, __m512i, u16)
}
__m512i libdivide_u16_branchfree_do_vec512(__m512i numers, const struct libdivide_u16_branchfree_t* denom) {
SIMPLE_VECTOR_DIVISION(uint16_t, __m512i, u16_branchfree)
}
__m512i libdivide_u32_do_vec512(__m512i numers, const struct libdivide_u32_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return _mm512_srli_epi32(numers, more);
}
else {
__m512i q = libdivide_mullhi_u32_vec512(numers, _mm512_set1_epi32(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
uint32_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
__m512i t = _mm512_add_epi32(_mm512_srli_epi32(_mm512_sub_epi32(numers, q), 1), q);
return _mm512_srli_epi32(t, shift);
}
else {
return _mm512_srli_epi32(q, more);
}
}
}
__m512i libdivide_u32_branchfree_do_vec512(
__m512i numers, const struct libdivide_u32_branchfree_t* denom) {
__m512i q = libdivide_mullhi_u32_vec512(numers, _mm512_set1_epi32(denom->magic));
__m512i t = _mm512_add_epi32(_mm512_srli_epi32(_mm512_sub_epi32(numers, q), 1), q);
return _mm512_srli_epi32(t, denom->more);
}
__m512i libdivide_u64_do_vec512(__m512i numers, const struct libdivide_u64_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return _mm512_srli_epi64(numers, more);
}
else {
__m512i q = libdivide_mullhi_u64_vec512(numers, _mm512_set1_epi64(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
uint32_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
__m512i t = _mm512_add_epi64(_mm512_srli_epi64(_mm512_sub_epi64(numers, q), 1), q);
return _mm512_srli_epi64(t, shift);
}
else {
return _mm512_srli_epi64(q, more);
}
}
}
__m512i libdivide_u64_branchfree_do_vec512(
__m512i numers, const struct libdivide_u64_branchfree_t* denom) {
__m512i q = libdivide_mullhi_u64_vec512(numers, _mm512_set1_epi64(denom->magic));
__m512i t = _mm512_add_epi64(_mm512_srli_epi64(_mm512_sub_epi64(numers, q), 1), q);
return _mm512_srli_epi64(t, denom->more);
}
__m512i libdivide_s16_do_vec512(__m512i numers, const struct libdivide_s16_t* denom) {
SIMPLE_VECTOR_DIVISION(int16_t, __m512i, s16)
}
__m512i libdivide_s16_branchfree_do_vec512(__m512i numers, const struct libdivide_s16_branchfree_t* denom) {
SIMPLE_VECTOR_DIVISION(int16_t, __m512i, s16_branchfree)
}
__m512i libdivide_s32_do_vec512(__m512i numers, const struct libdivide_s32_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
uint32_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
uint32_t mask = ((uint32_t)1 << shift) - 1;
__m512i roundToZeroTweak = _mm512_set1_epi32(mask);
__m512i q = _mm512_add_epi32(
numers, _mm512_and_si512(_mm512_srai_epi32(numers, 31), roundToZeroTweak));
q = _mm512_srai_epi32(q, shift);
__m512i sign = _mm512_set1_epi32((int8_t)more >> 7);
q = _mm512_sub_epi32(_mm512_xor_si512(q, sign), sign);
return q;
}
else {
__m512i q = libdivide_mullhi_s32_vec512(numers, _mm512_set1_epi32(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
__m512i sign = _mm512_set1_epi32((int8_t)more >> 7);
q = _mm512_add_epi32(q, _mm512_sub_epi32(_mm512_xor_si512(numers, sign), sign));
}
q = _mm512_srai_epi32(q, more & LIBDIVIDE_32_SHIFT_MASK);
q = _mm512_add_epi32(q, _mm512_srli_epi32(q, 31));  // q += (q < 0)
return q;
}
}
__m512i libdivide_s32_branchfree_do_vec512(
__m512i numers, const struct libdivide_s32_branchfree_t* denom) {
int32_t magic = denom->magic;
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
__m512i sign = _mm512_set1_epi32((int8_t)more >> 7);
__m512i q = libdivide_mullhi_s32_vec512(numers, _mm512_set1_epi32(magic));
q = _mm512_add_epi32(q, numers);  // q += numers
uint32_t is_power_of_2 = (magic == 0);
__m512i q_sign = _mm512_srai_epi32(q, 31);  // q_sign = q >> 31
__m512i mask = _mm512_set1_epi32(((uint32_t)1 << shift) - is_power_of_2);
q = _mm512_add_epi32(q, _mm512_and_si512(q_sign, mask));  // q = q + (q_sign & mask)
q = _mm512_srai_epi32(q, shift);                          // q >>= shift
q = _mm512_sub_epi32(_mm512_xor_si512(q, sign), sign);    // q = (q ^ sign) - sign
return q;
}
__m512i libdivide_s64_do_vec512(__m512i numers, const struct libdivide_s64_t* denom) {
uint8_t more = denom->more;
int64_t magic = denom->magic;
if (magic == 0) {  // shift path
uint32_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
uint64_t mask = ((uint64_t)1 << shift) - 1;
__m512i roundToZeroTweak = _mm512_set1_epi64(mask);
__m512i q = _mm512_add_epi64(
numers, _mm512_and_si512(libdivide_s64_signbits_vec512(numers), roundToZeroTweak));
q = libdivide_s64_shift_right_vec512(q, shift);
__m512i sign = _mm512_set1_epi32((int8_t)more >> 7);
q = _mm512_sub_epi64(_mm512_xor_si512(q, sign), sign);
return q;
}
else {
__m512i q = libdivide_mullhi_s64_vec512(numers, _mm512_set1_epi64(magic));
if (more & LIBDIVIDE_ADD_MARKER) {
__m512i sign = _mm512_set1_epi32((int8_t)more >> 7);
q = _mm512_add_epi64(q, _mm512_sub_epi64(_mm512_xor_si512(numers, sign), sign));
}
q = libdivide_s64_shift_right_vec512(q, more & LIBDIVIDE_64_SHIFT_MASK);
q = _mm512_add_epi64(q, _mm512_srli_epi64(q, 63));  // q += (q < 0)
return q;
}
}
__m512i libdivide_s64_branchfree_do_vec512(
__m512i numers, const struct libdivide_s64_branchfree_t* denom) {
int64_t magic = denom->magic;
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
__m512i sign = _mm512_set1_epi32((int8_t)more >> 7);
__m512i q = libdivide_mullhi_s64_vec512(numers, _mm512_set1_epi64(magic));
q = _mm512_add_epi64(q, numers);  // q += numers
uint32_t is_power_of_2 = (magic == 0);
__m512i q_sign = libdivide_s64_signbits_vec512(q);  // q_sign = q >> 63
__m512i mask = _mm512_set1_epi64(((uint64_t)1 << shift) - is_power_of_2);
q = _mm512_add_epi64(q, _mm512_and_si512(q_sign, mask));  // q = q + (q_sign & mask)
q = libdivide_s64_shift_right_vec512(q, shift);           // q >>= shift
q = _mm512_sub_epi64(_mm512_xor_si512(q, sign), sign);    // q = (q ^ sign) - sign
return q;
}
#endif
#if defined(LIBDIVIDE_AVX2)
static WJR_INTRINSIC_INLINE __m256i libdivide_u16_do_vec256(
__m256i numers, const struct libdivide_u16_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_s16_do_vec256(
__m256i numers, const struct libdivide_s16_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_u32_do_vec256(
__m256i numers, const struct libdivide_u32_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_s32_do_vec256(
__m256i numers, const struct libdivide_s32_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_u64_do_vec256(
__m256i numers, const struct libdivide_u64_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_s64_do_vec256(
__m256i numers, const struct libdivide_s64_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_u16_branchfree_do_vec256(
__m256i numers, const struct libdivide_u16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_s16_branchfree_do_vec256(
__m256i numers, const struct libdivide_s16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_u32_branchfree_do_vec256(
__m256i numers, const struct libdivide_u32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_s32_branchfree_do_vec256(
__m256i numers, const struct libdivide_s32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_u64_branchfree_do_vec256(
__m256i numers, const struct libdivide_u64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_s64_branchfree_do_vec256(
__m256i numers, const struct libdivide_s64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m256i libdivide_s64_signbits_vec256(__m256i v) {
__m256i hiBitsDuped = _mm256_shuffle_epi32(v, _MM_SHUFFLE(3, 3, 1, 1));
__m256i signBits = _mm256_srai_epi32(hiBitsDuped, 31);
return signBits;
}
static WJR_INTRINSIC_INLINE __m256i libdivide_s64_shift_right_vec256(__m256i v, int amt) {
const int b = 64 - amt;
__m256i m = _mm256_set1_epi64x((uint64_t)1 << (b - 1));
__m256i x = _mm256_srli_epi64(v, amt);
__m256i result = _mm256_sub_epi64(_mm256_xor_si256(x, m), m);
return result;
}
static WJR_INTRINSIC_INLINE __m256i libdivide_mullhi_u32_vec256(__m256i a, __m256i b) {
__m256i hi_product_0Z2Z = _mm256_srli_epi64(_mm256_mul_epu32(a, b), 32);
__m256i a1X3X = _mm256_srli_epi64(a, 32);
__m256i mask = _mm256_set_epi32(-1, 0, -1, 0, -1, 0, -1, 0);
__m256i hi_product_Z1Z3 = _mm256_and_si256(_mm256_mul_epu32(a1X3X, b), mask);
return _mm256_or_si256(hi_product_0Z2Z, hi_product_Z1Z3);
}
static WJR_INTRINSIC_INLINE __m256i libdivide_mullhi_s32_vec256(__m256i a, __m256i b) {
__m256i hi_product_0Z2Z = _mm256_srli_epi64(_mm256_mul_epi32(a, b), 32);
__m256i a1X3X = _mm256_srli_epi64(a, 32);
__m256i mask = _mm256_set_epi32(-1, 0, -1, 0, -1, 0, -1, 0);
__m256i hi_product_Z1Z3 = _mm256_and_si256(_mm256_mul_epi32(a1X3X, b), mask);
return _mm256_or_si256(hi_product_0Z2Z, hi_product_Z1Z3);
}
static WJR_INTRINSIC_INLINE __m256i libdivide_mullhi_u64_vec256(__m256i x, __m256i y) {
__m256i x0y0 = _mm256_mul_epu32(x, y);
__m256i x0y0_hi = _mm256_srli_epi64(x0y0, 32);
__m256i x1 = _mm256_shuffle_epi32(x, _MM_SHUFFLE(3, 3, 1, 1));
__m256i y1 = _mm256_shuffle_epi32(y, _MM_SHUFFLE(3, 3, 1, 1));
__m256i x0y1 = _mm256_mul_epu32(x, y1);
__m256i x1y0 = _mm256_mul_epu32(x1, y);
__m256i x1y1 = _mm256_mul_epu32(x1, y1);
__m256i mask = _mm256_set1_epi64x(0xFFFFFFFF);
__m256i temp = _mm256_add_epi64(x1y0, x0y0_hi);
__m256i temp_lo = _mm256_and_si256(temp, mask);
__m256i temp_hi = _mm256_srli_epi64(temp, 32);
temp_lo = _mm256_srli_epi64(_mm256_add_epi64(temp_lo, x0y1), 32);
temp_hi = _mm256_add_epi64(x1y1, temp_hi);
return _mm256_add_epi64(temp_lo, temp_hi);
}
static WJR_INTRINSIC_INLINE __m256i libdivide_mullhi_s64_vec256(__m256i x, __m256i y) {
__m256i p = libdivide_mullhi_u64_vec256(x, y);
__m256i t1 = _mm256_and_si256(libdivide_s64_signbits_vec256(x), y);
__m256i t2 = _mm256_and_si256(libdivide_s64_signbits_vec256(y), x);
p = _mm256_sub_epi64(p, t1);
p = _mm256_sub_epi64(p, t2);
return p;
}
__m256i libdivide_u16_do_vec256(__m256i numers, const struct libdivide_u16_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return _mm256_srli_epi16(numers, more);
}
else {
__m256i q = _mm256_mulhi_epu16(numers, _mm256_set1_epi16(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
__m256i t = _mm256_adds_epu16(_mm256_srli_epi16(_mm256_subs_epu16(numers, q), 1), q);
return _mm256_srli_epi16(t, (more & LIBDIVIDE_16_SHIFT_MASK));
}
else {
return _mm256_srli_epi16(q, more);
}
}
}
__m256i libdivide_u16_branchfree_do_vec256(__m256i numers, const struct libdivide_u16_branchfree_t* denom) {
__m256i q = _mm256_mulhi_epu16(numers, _mm256_set1_epi16(denom->magic));
__m256i t = _mm256_adds_epu16(_mm256_srli_epi16(_mm256_subs_epu16(numers, q), 1), q);
return _mm256_srli_epi16(t, denom->more);
}
__m256i libdivide_u32_do_vec256(__m256i numers, const struct libdivide_u32_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return _mm256_srli_epi32(numers, more);
}
else {
__m256i q = libdivide_mullhi_u32_vec256(numers, _mm256_set1_epi32(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
uint32_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
__m256i t = _mm256_add_epi32(_mm256_srli_epi32(_mm256_sub_epi32(numers, q), 1), q);
return _mm256_srli_epi32(t, shift);
}
else {
return _mm256_srli_epi32(q, more);
}
}
}
__m256i libdivide_u32_branchfree_do_vec256(
__m256i numers, const struct libdivide_u32_branchfree_t* denom) {
__m256i q = libdivide_mullhi_u32_vec256(numers, _mm256_set1_epi32(denom->magic));
__m256i t = _mm256_add_epi32(_mm256_srli_epi32(_mm256_sub_epi32(numers, q), 1), q);
return _mm256_srli_epi32(t, denom->more);
}
__m256i libdivide_u64_do_vec256(__m256i numers, const struct libdivide_u64_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return _mm256_srli_epi64(numers, more);
}
else {
__m256i q = libdivide_mullhi_u64_vec256(numers, _mm256_set1_epi64x(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
uint32_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
__m256i t = _mm256_add_epi64(_mm256_srli_epi64(_mm256_sub_epi64(numers, q), 1), q);
return _mm256_srli_epi64(t, shift);
}
else {
return _mm256_srli_epi64(q, more);
}
}
}
__m256i libdivide_u64_branchfree_do_vec256(
__m256i numers, const struct libdivide_u64_branchfree_t* denom) {
__m256i q = libdivide_mullhi_u64_vec256(numers, _mm256_set1_epi64x(denom->magic));
__m256i t = _mm256_add_epi64(_mm256_srli_epi64(_mm256_sub_epi64(numers, q), 1), q);
return _mm256_srli_epi64(t, denom->more);
}
__m256i libdivide_s16_do_vec256(__m256i numers, const struct libdivide_s16_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
uint16_t shift = more & LIBDIVIDE_16_SHIFT_MASK;
uint16_t mask = ((uint16_t)1 << shift) - 1;
__m256i roundToZeroTweak = _mm256_set1_epi16(mask);
__m256i q = _mm256_add_epi16(
numers, _mm256_and_si256(_mm256_srai_epi16(numers, 15), roundToZeroTweak));
q = _mm256_srai_epi16(q, shift);
__m256i sign = _mm256_set1_epi16((int8_t)more >> 7);
q = _mm256_sub_epi16(_mm256_xor_si256(q, sign), sign);
return q;
}
else {
__m256i q = _mm256_mulhi_epi16(numers, _mm256_set1_epi16(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
__m256i sign = _mm256_set1_epi16((int8_t)more >> 7);
q = _mm256_add_epi16(q, _mm256_sub_epi16(_mm256_xor_si256(numers, sign), sign));
}
q = _mm256_srai_epi16(q, more & LIBDIVIDE_16_SHIFT_MASK);
q = _mm256_add_epi16(q, _mm256_srli_epi16(q, 15));  // q += (q < 0)
return q;
}
}
__m256i libdivide_s16_branchfree_do_vec256(__m256i numers, const struct libdivide_s16_branchfree_t* denom) {
int16_t magic = denom->magic;
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_16_SHIFT_MASK;
__m256i sign = _mm256_set1_epi16((int8_t)more >> 7);
__m256i q = _mm256_mulhi_epi16(numers, _mm256_set1_epi16(magic));
q = _mm256_add_epi16(q, numers);  // q += numers
uint16_t is_power_of_2 = (magic == 0);
__m256i q_sign = _mm256_srai_epi16(q, 15);  // q_sign = q >> 15
__m256i mask = _mm256_set1_epi16(((uint16_t)1 << shift) - is_power_of_2);
q = _mm256_add_epi16(q, _mm256_and_si256(q_sign, mask));  // q = q + (q_sign & mask)
q = _mm256_srai_epi16(q, shift);                          // q >>= shift
q = _mm256_sub_epi16(_mm256_xor_si256(q, sign), sign);    // q = (q ^ sign) - sign
return q;
}
__m256i libdivide_s32_do_vec256(__m256i numers, const struct libdivide_s32_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
uint32_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
uint32_t mask = ((uint32_t)1 << shift) - 1;
__m256i roundToZeroTweak = _mm256_set1_epi32(mask);
__m256i q = _mm256_add_epi32(
numers, _mm256_and_si256(_mm256_srai_epi32(numers, 31), roundToZeroTweak));
q = _mm256_srai_epi32(q, shift);
__m256i sign = _mm256_set1_epi32((int8_t)more >> 7);
q = _mm256_sub_epi32(_mm256_xor_si256(q, sign), sign);
return q;
}
else {
__m256i q = libdivide_mullhi_s32_vec256(numers, _mm256_set1_epi32(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
__m256i sign = _mm256_set1_epi32((int8_t)more >> 7);
q = _mm256_add_epi32(q, _mm256_sub_epi32(_mm256_xor_si256(numers, sign), sign));
}
q = _mm256_srai_epi32(q, more & LIBDIVIDE_32_SHIFT_MASK);
q = _mm256_add_epi32(q, _mm256_srli_epi32(q, 31));  // q += (q < 0)
return q;
}
}
__m256i libdivide_s32_branchfree_do_vec256(
__m256i numers, const struct libdivide_s32_branchfree_t* denom) {
int32_t magic = denom->magic;
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
__m256i sign = _mm256_set1_epi32((int8_t)more >> 7);
__m256i q = libdivide_mullhi_s32_vec256(numers, _mm256_set1_epi32(magic));
q = _mm256_add_epi32(q, numers);  // q += numers
uint32_t is_power_of_2 = (magic == 0);
__m256i q_sign = _mm256_srai_epi32(q, 31);  // q_sign = q >> 31
__m256i mask = _mm256_set1_epi32(((uint32_t)1 << shift) - is_power_of_2);
q = _mm256_add_epi32(q, _mm256_and_si256(q_sign, mask));  // q = q + (q_sign & mask)
q = _mm256_srai_epi32(q, shift);                          // q >>= shift
q = _mm256_sub_epi32(_mm256_xor_si256(q, sign), sign);    // q = (q ^ sign) - sign
return q;
}
__m256i libdivide_s64_do_vec256(__m256i numers, const struct libdivide_s64_t* denom) {
uint8_t more = denom->more;
int64_t magic = denom->magic;
if (magic == 0) {  // shift path
uint32_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
uint64_t mask = ((uint64_t)1 << shift) - 1;
__m256i roundToZeroTweak = _mm256_set1_epi64x(mask);
__m256i q = _mm256_add_epi64(
numers, _mm256_and_si256(libdivide_s64_signbits_vec256(numers), roundToZeroTweak));
q = libdivide_s64_shift_right_vec256(q, shift);
__m256i sign = _mm256_set1_epi32((int8_t)more >> 7);
q = _mm256_sub_epi64(_mm256_xor_si256(q, sign), sign);
return q;
}
else {
__m256i q = libdivide_mullhi_s64_vec256(numers, _mm256_set1_epi64x(magic));
if (more & LIBDIVIDE_ADD_MARKER) {
__m256i sign = _mm256_set1_epi32((int8_t)more >> 7);
q = _mm256_add_epi64(q, _mm256_sub_epi64(_mm256_xor_si256(numers, sign), sign));
}
q = libdivide_s64_shift_right_vec256(q, more & LIBDIVIDE_64_SHIFT_MASK);
q = _mm256_add_epi64(q, _mm256_srli_epi64(q, 63));  // q += (q < 0)
return q;
}
}
__m256i libdivide_s64_branchfree_do_vec256(
__m256i numers, const struct libdivide_s64_branchfree_t* denom) {
int64_t magic = denom->magic;
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
__m256i sign = _mm256_set1_epi32((int8_t)more >> 7);
__m256i q = libdivide_mullhi_s64_vec256(numers, _mm256_set1_epi64x(magic));
q = _mm256_add_epi64(q, numers);  // q += numers
uint32_t is_power_of_2 = (magic == 0);
__m256i q_sign = libdivide_s64_signbits_vec256(q);  // q_sign = q >> 63
__m256i mask = _mm256_set1_epi64x(((uint64_t)1 << shift) - is_power_of_2);
q = _mm256_add_epi64(q, _mm256_and_si256(q_sign, mask));  // q = q + (q_sign & mask)
q = libdivide_s64_shift_right_vec256(q, shift);           // q >>= shift
q = _mm256_sub_epi64(_mm256_xor_si256(q, sign), sign);    // q = (q ^ sign) - sign
return q;
}
#endif
#if defined(LIBDIVIDE_SSE2)
static WJR_INTRINSIC_INLINE __m128i libdivide_u16_do_vec128(
__m128i numers, const struct libdivide_u16_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_s16_do_vec128(
__m128i numers, const struct libdivide_s16_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_u32_do_vec128(
__m128i numers, const struct libdivide_u32_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_s32_do_vec128(
__m128i numers, const struct libdivide_s32_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_u64_do_vec128(
__m128i numers, const struct libdivide_u64_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_s64_do_vec128(
__m128i numers, const struct libdivide_s64_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_u16_branchfree_do_vec128(
__m128i numers, const struct libdivide_u16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_s16_branchfree_do_vec128(
__m128i numers, const struct libdivide_s16_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_u32_branchfree_do_vec128(
__m128i numers, const struct libdivide_u32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_s32_branchfree_do_vec128(
__m128i numers, const struct libdivide_s32_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_u64_branchfree_do_vec128(
__m128i numers, const struct libdivide_u64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_s64_branchfree_do_vec128(
__m128i numers, const struct libdivide_s64_branchfree_t* denom);
static WJR_INTRINSIC_INLINE __m128i libdivide_s64_signbits_vec128(__m128i v) {
__m128i hiBitsDuped = _mm_shuffle_epi32(v, _MM_SHUFFLE(3, 3, 1, 1));
__m128i signBits = _mm_srai_epi32(hiBitsDuped, 31);
return signBits;
}
static WJR_INTRINSIC_INLINE __m128i libdivide_s64_shift_right_vec128(__m128i v, int amt) {
const int b = 64 - amt;
__m128i m = _mm_set1_epi64x((uint64_t)1 << (b - 1));
__m128i x = _mm_srli_epi64(v, amt);
__m128i result = _mm_sub_epi64(_mm_xor_si128(x, m), m);
return result;
}
static WJR_INTRINSIC_INLINE __m128i libdivide_mullhi_u32_vec128(__m128i a, __m128i b) {
__m128i hi_product_0Z2Z = _mm_srli_epi64(_mm_mul_epu32(a, b), 32);
__m128i a1X3X = _mm_srli_epi64(a, 32);
__m128i mask = _mm_set_epi32(-1, 0, -1, 0);
__m128i hi_product_Z1Z3 = _mm_and_si128(_mm_mul_epu32(a1X3X, b), mask);
return _mm_or_si128(hi_product_0Z2Z, hi_product_Z1Z3);
}
static WJR_INTRINSIC_INLINE __m128i libdivide_mullhi_s32_vec128(__m128i a, __m128i b) {
__m128i p = libdivide_mullhi_u32_vec128(a, b);
__m128i t1 = _mm_and_si128(_mm_srai_epi32(a, 31), b);
__m128i t2 = _mm_and_si128(_mm_srai_epi32(b, 31), a);
p = _mm_sub_epi32(p, t1);
p = _mm_sub_epi32(p, t2);
return p;
}
static WJR_INTRINSIC_INLINE __m128i libdivide_mullhi_u64_vec128(__m128i x, __m128i y) {
__m128i x0y0 = _mm_mul_epu32(x, y);
__m128i x0y0_hi = _mm_srli_epi64(x0y0, 32);
__m128i x1 = _mm_shuffle_epi32(x, _MM_SHUFFLE(3, 3, 1, 1));
__m128i y1 = _mm_shuffle_epi32(y, _MM_SHUFFLE(3, 3, 1, 1));
__m128i x0y1 = _mm_mul_epu32(x, y1);
__m128i x1y0 = _mm_mul_epu32(x1, y);
__m128i x1y1 = _mm_mul_epu32(x1, y1);
__m128i mask = _mm_set1_epi64x(0xFFFFFFFF);
__m128i temp = _mm_add_epi64(x1y0, x0y0_hi);
__m128i temp_lo = _mm_and_si128(temp, mask);
__m128i temp_hi = _mm_srli_epi64(temp, 32);
temp_lo = _mm_srli_epi64(_mm_add_epi64(temp_lo, x0y1), 32);
temp_hi = _mm_add_epi64(x1y1, temp_hi);
return _mm_add_epi64(temp_lo, temp_hi);
}
static WJR_INTRINSIC_INLINE __m128i libdivide_mullhi_s64_vec128(__m128i x, __m128i y) {
__m128i p = libdivide_mullhi_u64_vec128(x, y);
__m128i t1 = _mm_and_si128(libdivide_s64_signbits_vec128(x), y);
__m128i t2 = _mm_and_si128(libdivide_s64_signbits_vec128(y), x);
p = _mm_sub_epi64(p, t1);
p = _mm_sub_epi64(p, t2);
return p;
}
__m128i libdivide_u16_do_vec128(__m128i numers, const struct libdivide_u16_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return _mm_srli_epi16(numers, more);
}
else {
__m128i q = _mm_mulhi_epu16(numers, _mm_set1_epi16(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
__m128i t = _mm_adds_epu16(_mm_srli_epi16(_mm_subs_epu16(numers, q), 1), q);
return _mm_srli_epi16(t, (more & LIBDIVIDE_16_SHIFT_MASK));
}
else {
return _mm_srli_epi16(q, more);
}
}
}
__m128i libdivide_u16_branchfree_do_vec128(__m128i numers, const struct libdivide_u16_branchfree_t* denom) {
__m128i q = _mm_mulhi_epu16(numers, _mm_set1_epi16(denom->magic));
__m128i t = _mm_adds_epu16(_mm_srli_epi16(_mm_subs_epu16(numers, q), 1), q);
return _mm_srli_epi16(t, denom->more);
}
__m128i libdivide_u32_do_vec128(__m128i numers, const struct libdivide_u32_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return _mm_srli_epi32(numers, more);
}
else {
__m128i q = libdivide_mullhi_u32_vec128(numers, _mm_set1_epi32(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
uint32_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
__m128i t = _mm_add_epi32(_mm_srli_epi32(_mm_sub_epi32(numers, q), 1), q);
return _mm_srli_epi32(t, shift);
}
else {
return _mm_srli_epi32(q, more);
}
}
}
__m128i libdivide_u32_branchfree_do_vec128(
__m128i numers, const struct libdivide_u32_branchfree_t* denom) {
__m128i q = libdivide_mullhi_u32_vec128(numers, _mm_set1_epi32(denom->magic));
__m128i t = _mm_add_epi32(_mm_srli_epi32(_mm_sub_epi32(numers, q), 1), q);
return _mm_srli_epi32(t, denom->more);
}
__m128i libdivide_u64_do_vec128(__m128i numers, const struct libdivide_u64_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
return _mm_srli_epi64(numers, more);
}
else {
__m128i q = libdivide_mullhi_u64_vec128(numers, _mm_set1_epi64x(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
uint32_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
__m128i t = _mm_add_epi64(_mm_srli_epi64(_mm_sub_epi64(numers, q), 1), q);
return _mm_srli_epi64(t, shift);
}
else {
return _mm_srli_epi64(q, more);
}
}
}
__m128i libdivide_u64_branchfree_do_vec128(
__m128i numers, const struct libdivide_u64_branchfree_t* denom) {
__m128i q = libdivide_mullhi_u64_vec128(numers, _mm_set1_epi64x(denom->magic));
__m128i t = _mm_add_epi64(_mm_srli_epi64(_mm_sub_epi64(numers, q), 1), q);
return _mm_srli_epi64(t, denom->more);
}
__m128i libdivide_s16_do_vec128(__m128i numers, const struct libdivide_s16_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
uint16_t shift = more & LIBDIVIDE_16_SHIFT_MASK;
uint16_t mask = ((uint16_t)1 << shift) - 1;
__m128i roundToZeroTweak = _mm_set1_epi16(mask);
__m128i q = _mm_add_epi16(
numers, _mm_and_si128(_mm_srai_epi16(numers, 15), roundToZeroTweak));
q = _mm_srai_epi16(q, shift);
__m128i sign = _mm_set1_epi16((int8_t)more >> 7);
q = _mm_sub_epi16(_mm_xor_si128(q, sign), sign);
return q;
}
else {
__m128i q = _mm_mulhi_epi16(numers, _mm_set1_epi16(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
__m128i sign = _mm_set1_epi16((int8_t)more >> 7);
q = _mm_add_epi16(q, _mm_sub_epi16(_mm_xor_si128(numers, sign), sign));
}
q = _mm_srai_epi16(q, more & LIBDIVIDE_16_SHIFT_MASK);
q = _mm_add_epi16(q, _mm_srli_epi16(q, 15));  // q += (q < 0)
return q;
}
}
__m128i libdivide_s16_branchfree_do_vec128(__m128i numers, const struct libdivide_s16_branchfree_t* denom) {
int16_t magic = denom->magic;
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_16_SHIFT_MASK;
__m128i sign = _mm_set1_epi16((int8_t)more >> 7);
__m128i q = _mm_mulhi_epi16(numers, _mm_set1_epi16(magic));
q = _mm_add_epi16(q, numers);  // q += numers
uint16_t is_power_of_2 = (magic == 0);
__m128i q_sign = _mm_srai_epi16(q, 15);  // q_sign = q >> 15
__m128i mask = _mm_set1_epi16(((uint16_t)1 << shift) - is_power_of_2);
q = _mm_add_epi16(q, _mm_and_si128(q_sign, mask));  // q = q + (q_sign & mask)
q = _mm_srai_epi16(q, shift);                          // q >>= shift
q = _mm_sub_epi16(_mm_xor_si128(q, sign), sign);    // q = (q ^ sign) - sign
return q;
}
__m128i libdivide_s32_do_vec128(__m128i numers, const struct libdivide_s32_t* denom) {
uint8_t more = denom->more;
if (!denom->magic) {
uint32_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
uint32_t mask = ((uint32_t)1 << shift) - 1;
__m128i roundToZeroTweak = _mm_set1_epi32(mask);
__m128i q =
_mm_add_epi32(numers, _mm_and_si128(_mm_srai_epi32(numers, 31), roundToZeroTweak));
q = _mm_srai_epi32(q, shift);
__m128i sign = _mm_set1_epi32((int8_t)more >> 7);
q = _mm_sub_epi32(_mm_xor_si128(q, sign), sign);
return q;
}
else {
__m128i q = libdivide_mullhi_s32_vec128(numers, _mm_set1_epi32(denom->magic));
if (more & LIBDIVIDE_ADD_MARKER) {
__m128i sign = _mm_set1_epi32((int8_t)more >> 7);
q = _mm_add_epi32(q, _mm_sub_epi32(_mm_xor_si128(numers, sign), sign));
}
q = _mm_srai_epi32(q, more & LIBDIVIDE_32_SHIFT_MASK);
q = _mm_add_epi32(q, _mm_srli_epi32(q, 31));  // q += (q < 0)
return q;
}
}
__m128i libdivide_s32_branchfree_do_vec128(
__m128i numers, const struct libdivide_s32_branchfree_t* denom) {
int32_t magic = denom->magic;
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_32_SHIFT_MASK;
__m128i sign = _mm_set1_epi32((int8_t)more >> 7);
__m128i q = libdivide_mullhi_s32_vec128(numers, _mm_set1_epi32(magic));
q = _mm_add_epi32(q, numers);  // q += numers
uint32_t is_power_of_2 = (magic == 0);
__m128i q_sign = _mm_srai_epi32(q, 31);  // q_sign = q >> 31
__m128i mask = _mm_set1_epi32(((uint32_t)1 << shift) - is_power_of_2);
q = _mm_add_epi32(q, _mm_and_si128(q_sign, mask));  // q = q + (q_sign & mask)
q = _mm_srai_epi32(q, shift);                       // q >>= shift
q = _mm_sub_epi32(_mm_xor_si128(q, sign), sign);    // q = (q ^ sign) - sign
return q;
}
__m128i libdivide_s64_do_vec128(__m128i numers, const struct libdivide_s64_t* denom) {
uint8_t more = denom->more;
int64_t magic = denom->magic;
if (magic == 0) {  // shift path
uint32_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
uint64_t mask = ((uint64_t)1 << shift) - 1;
__m128i roundToZeroTweak = _mm_set1_epi64x(mask);
__m128i q =
_mm_add_epi64(numers, _mm_and_si128(libdivide_s64_signbits_vec128(numers), roundToZeroTweak));
q = libdivide_s64_shift_right_vec128(q, shift);
__m128i sign = _mm_set1_epi32((int8_t)more >> 7);
q = _mm_sub_epi64(_mm_xor_si128(q, sign), sign);
return q;
}
else {
__m128i q = libdivide_mullhi_s64_vec128(numers, _mm_set1_epi64x(magic));
if (more & LIBDIVIDE_ADD_MARKER) {
__m128i sign = _mm_set1_epi32((int8_t)more >> 7);
q = _mm_add_epi64(q, _mm_sub_epi64(_mm_xor_si128(numers, sign), sign));
}
q = libdivide_s64_shift_right_vec128(q, more & LIBDIVIDE_64_SHIFT_MASK);
q = _mm_add_epi64(q, _mm_srli_epi64(q, 63));  // q += (q < 0)
return q;
}
}
__m128i libdivide_s64_branchfree_do_vec128(
__m128i numers, const struct libdivide_s64_branchfree_t* denom) {
int64_t magic = denom->magic;
uint8_t more = denom->more;
uint8_t shift = more & LIBDIVIDE_64_SHIFT_MASK;
__m128i sign = _mm_set1_epi32((int8_t)more >> 7);
__m128i q = libdivide_mullhi_s64_vec128(numers, _mm_set1_epi64x(magic));
q = _mm_add_epi64(q, numers);  // q += numers
uint32_t is_power_of_2 = (magic == 0);
__m128i q_sign = libdivide_s64_signbits_vec128(q);  // q_sign = q >> 63
__m128i mask = _mm_set1_epi64x(((uint64_t)1 << shift) - is_power_of_2);
q = _mm_add_epi64(q, _mm_and_si128(q_sign, mask));  // q = q + (q_sign & mask)
q = libdivide_s64_shift_right_vec128(q, shift);     // q >>= shift
q = _mm_sub_epi64(_mm_xor_si128(q, sign), sign);    // q = (q ^ sign) - sign
return q;
}
#endif
#ifdef __cplusplus
enum Branching {
BRANCHFULL,  // use branching algorithms
BRANCHFREE   // use branchfree algorithms
};
#if defined(LIBDIVIDE_NEON)
template <typename T>
struct NeonVecFor {};
template <>
struct NeonVecFor<uint16_t> {
typedef uint16x8_t type;
};
template <>
struct NeonVecFor<int16_t> {
typedef int16x8_t type;
};
template <>
struct NeonVecFor<uint32_t> {
typedef uint32x4_t type;
};
template <>
struct NeonVecFor<int32_t> {
typedef int32x4_t type;
};
template <>
struct NeonVecFor<uint64_t> {
typedef uint64x2_t type;
};
template <>
struct NeonVecFor<int64_t> {
typedef int64x2_t type;
};
#endif
#if defined(LIBDIVIDE_NEON)
#define LIBDIVIDE_DIVIDE_NEON(ALGO, INT_TYPE)                    \
WJR_INTRINSIC_INLINE typename NeonVecFor<INT_TYPE>::type divide( \
typename NeonVecFor<INT_TYPE>::type n) const {           \
return libdivide_##ALGO##_do_vec128(n, &denom);          \
}
#else
#define LIBDIVIDE_DIVIDE_NEON(ALGO, INT_TYPE)
#endif
#if defined(LIBDIVIDE_SSE2)
#define LIBDIVIDE_DIVIDE_SSE2(ALGO)                     \
WJR_INTRINSIC_INLINE __m128i divide(__m128i n) const {  \
return libdivide_##ALGO##_do_vec128(n, &denom); \
}
#else
#define LIBDIVIDE_DIVIDE_SSE2(ALGO)
#endif
#if defined(LIBDIVIDE_AVX2)
#define LIBDIVIDE_DIVIDE_AVX2(ALGO)                     \
WJR_INTRINSIC_INLINE __m256i divide(__m256i n) const {  \
return libdivide_##ALGO##_do_vec256(n, &denom); \
}
#else
#define LIBDIVIDE_DIVIDE_AVX2(ALGO)
#endif
#if defined(LIBDIVIDE_AVX512)
#define LIBDIVIDE_DIVIDE_AVX512(ALGO)                   \
WJR_INTRINSIC_INLINE __m512i divide(__m512i n) const {  \
return libdivide_##ALGO##_do_vec512(n, &denom); \
}
#else
#define LIBDIVIDE_DIVIDE_AVX512(ALGO)
#endif
#define DISPATCHER_GEN(T, ALGO)                                                       \
libdivide_##ALGO##_t denom;                                                       \
WJR_INTRINSIC_INLINE dispatcher() {}                                                  \
WJR_INTRINSIC_INLINE dispatcher(T d) : denom(libdivide_##ALGO##_gen(d)) {}            \
WJR_INTRINSIC_INLINE T divide(T n) const { return libdivide_##ALGO##_do(n, &denom); } \
WJR_INTRINSIC_INLINE T recover() const { return libdivide_##ALGO##_recover(&denom); } \
LIBDIVIDE_DIVIDE_NEON(ALGO, T)                                                    \
LIBDIVIDE_DIVIDE_SSE2(ALGO)                                                       \
LIBDIVIDE_DIVIDE_AVX2(ALGO)                                                       \
LIBDIVIDE_DIVIDE_AVX512(ALGO)
template <typename _IntT, Branching ALGO>
struct dispatcher {};
template <>
struct dispatcher<int16_t, BRANCHFULL> {
DISPATCHER_GEN(int16_t, s16)
};
template <>
struct dispatcher<int16_t, BRANCHFREE> {
DISPATCHER_GEN(int16_t, s16_branchfree)
};
template <>
struct dispatcher<uint16_t, BRANCHFULL> {
DISPATCHER_GEN(uint16_t, u16)
};
template <>
struct dispatcher<uint16_t, BRANCHFREE> {
DISPATCHER_GEN(uint16_t, u16_branchfree)
};
template <>
struct dispatcher<int32_t, BRANCHFULL> {
DISPATCHER_GEN(int32_t, s32)
};
template <>
struct dispatcher<int32_t, BRANCHFREE> {
DISPATCHER_GEN(int32_t, s32_branchfree)
};
template <>
struct dispatcher<uint32_t, BRANCHFULL> {
DISPATCHER_GEN(uint32_t, u32)
};
template <>
struct dispatcher<uint32_t, BRANCHFREE> {
DISPATCHER_GEN(uint32_t, u32_branchfree)
};
template <>
struct dispatcher<int64_t, BRANCHFULL> {
DISPATCHER_GEN(int64_t, s64)
};
template <>
struct dispatcher<int64_t, BRANCHFREE> {
DISPATCHER_GEN(int64_t, s64_branchfree)
};
template <>
struct dispatcher<uint64_t, BRANCHFULL> {
DISPATCHER_GEN(uint64_t, u64)
};
template <>
struct dispatcher<uint64_t, BRANCHFREE> {
DISPATCHER_GEN(uint64_t, u64_branchfree)
};
template <typename T, Branching ALGO = BRANCHFULL>
class divider {
private:
typedef dispatcher<T, ALGO> dispatcher_t;
public:
divider() = default;
WJR_INTRINSIC_INLINE divider(T d) : div(d) {}
WJR_INTRINSIC_INLINE T divide(T n) const { return div.divide(n); }
T recover() const { return div.recover(); }
bool operator==(const divider<T, ALGO>& other) const {
return div.denom.magic == other.denom.magic && div.denom.more == other.denom.more;
}
bool operator!=(const divider<T, ALGO>& other) const { return !(*this == other); }
#if defined(LIBDIVIDE_SSE2)
WJR_INTRINSIC_INLINE __m128i divide(__m128i n) const { return div.divide(n); }
#endif
#if defined(LIBDIVIDE_AVX2)
WJR_INTRINSIC_INLINE __m256i divide(__m256i n) const { return div.divide(n); }
#endif
#if defined(LIBDIVIDE_AVX512)
WJR_INTRINSIC_INLINE __m512i divide(__m512i n) const { return div.divide(n); }
#endif
#if defined(LIBDIVIDE_NEON)
WJR_INTRINSIC_INLINE typename NeonVecFor<T>::type divide(typename NeonVecFor<T>::type n) const {
return div.divide(n);
}
#endif
dispatcher_t div;
};
template <typename T, Branching ALGO>
WJR_INTRINSIC_INLINE T operator/(T n, const divider<T, ALGO>& div) {
return div.divide(n);
}
template <typename T, Branching ALGO>
WJR_INTRINSIC_INLINE T& operator/=(T& n, const divider<T, ALGO>& div) {
n = div.divide(n);
return n;
}
#if defined(LIBDIVIDE_SSE2)
template <typename T, Branching ALGO>
WJR_INTRINSIC_INLINE __m128i operator/(__m128i n, const divider<T, ALGO>& div) {
return div.divide(n);
}
template <typename T, Branching ALGO>
WJR_INTRINSIC_INLINE __m128i operator/=(__m128i& n, const divider<T, ALGO>& div) {
n = div.divide(n);
return n;
}
#endif
#if defined(LIBDIVIDE_AVX2)
template <typename T, Branching ALGO>
WJR_INTRINSIC_INLINE __m256i operator/(__m256i n, const divider<T, ALGO>& div) {
return div.divide(n);
}
template <typename T, Branching ALGO>
WJR_INTRINSIC_INLINE __m256i operator/=(__m256i& n, const divider<T, ALGO>& div) {
n = div.divide(n);
return n;
}
#endif
#if defined(LIBDIVIDE_AVX512)
template <typename T, Branching ALGO>
WJR_INTRINSIC_INLINE __m512i operator/(__m512i n, const divider<T, ALGO>& div) {
return div.divide(n);
}
template <typename T, Branching ALGO>
WJR_INTRINSIC_INLINE __m512i operator/=(__m512i& n, const divider<T, ALGO>& div) {
n = div.divide(n);
return n;
}
#endif
#if defined(LIBDIVIDE_NEON)
template <typename T, Branching ALGO>
WJR_INTRINSIC_INLINE typename NeonVecFor<T>::type operator/(typename NeonVecFor<T>::type n, const divider<T, ALGO>& div) {
return div.divide(n);
}
template <typename T, Branching ALGO>
WJR_INTRINSIC_INLINE typename NeonVecFor<T>::type operator/=(typename NeonVecFor<T>::type& n, const divider<T, ALGO>& div) {
n = div.divide(n);
return n;
}
#endif
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
template <typename T>
using branchfree_divider = divider<T, BRANCHFREE>;
#endif
}  // namespace wjr::libdivide
#endif  // __cplusplus
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#endif  // LIBDIVIDE_H

_WJR_BEGIN
#ifdef __cpp_lib_bit_cast
#else
template <
typename To,
typename From,
std::enable_if_t<
sizeof(From) == sizeof(To) && std::is_trivially_copyable<To>::value&&
std::is_trivially_copyable<From>::value,
int>>
WJR_INTRINSIC_CONSTEXPR20 To bit_cast(const From& src) noexcept {
static_assert(std::is_trivially_constructible_v<To>,
"This implementation additionally requires destination type to be trivially constructible");
To storage;
if constexpr (is_any_index_of_v<sizeof(From), 1, 2, 4, 8>) {
using value_type = uint_t<8 * sizeof(From)>;
*reinterpret_cast<value_type*>(&storage) = *reinterpret_cast<const value_type*>(&src);
}
else {
::memcpy(&storage, &src, sizeof(From));
}
return storage;
}
#endif
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
constexpr bool has_single_bit(T x) noexcept {
return x != 0 && (x & (x - 1)) == 0;
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countl_zero(T x) noexcept {
return wjr::masm::clz(x);
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countl_one(T x) noexcept {
return wjr::countl_zero(static_cast<T>(~x));
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countr_zero(T x) noexcept {
return wjr::masm::ctz(x);
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int countr_one(T x) noexcept {
return wjr::countr_zero(static_cast<T>(~x));
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int bit_width(T x) noexcept {
return std::numeric_limits<T>::digits - wjr::countl_zero(x);
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) T bit_floor(T x) noexcept {
if (x != 0) {
return static_cast<T>(T{ 1 } << (wjr::bit_width(x) - 1));
}
return T{ 0 };
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) T bit_ceil(T x) noexcept {
if (x <= 1) {
return T{ 1 };
}
return static_cast<T>(T{ 1 } << wjr::bit_width(x - 1));
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int popcount(T x) noexcept {
return wjr::masm::popcnt(x);
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_ATTRIBUTE(INTRINSIC_E_CONSTEXPR) T adc(T a, T b, T c, T& d) noexcept {
return wjr::masm::adc(a, b, c, &d);
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
WJR_ATTRIBUTE(INTRINSIC_E_CONSTEXPR) T sbb(T a, T b, T c, T& d) noexcept {
return wjr::masm::sbb(a, b, c, &d);
}
template<typename T, std::enable_if_t<is_standard_numer_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T byteswap(T x) noexcept {
using value_type = uint_t<8 * sizeof(T)>;
return bit_cast<T>(wjr::masm::bswap(bit_cast<value_type>(x)));
}
template<endian to, typename T, std::enable_if_t<is_standard_numer_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T endian_convert(T x, endian from) noexcept {
if (from != to) {
return byteswap(x);
}
return x;
}
template<endian from, endian to, typename T, std::enable_if_t<is_standard_numer_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR20) T endian_convert(T x) noexcept {
if constexpr (from != to) {
return byteswap(x);
}
return x;
}
template<typename T, endian to>
WJR_INTRINSIC_CONSTEXPR20 T read_bytes(const void* ptr) noexcept {
T value = T{};
memcpy(std::addressof(value), ptr, sizeof(T));
return endian_convert<endian::little, to>(value);
}
template< typename T, typename U >
constexpr bool cmp_equal(T t, U u) noexcept {
using UT = std::make_unsigned_t<T>;
using UU = std::make_unsigned_t<U>;
if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
return t == u;
}
else if constexpr (std::is_signed_v<T>) {
return t < 0 ? false : UT(t) == u;
}
else {
return u < 0 ? false : t == UU(u);
}
}
template< typename T, typename U >
constexpr bool cmp_not_equal(T t, U u) noexcept {
return !cmp_equal(t, u);
}
template< typename T, typename U >
constexpr bool cmp_less(T t, U u) noexcept {
using UT = std::make_unsigned_t<T>;
using UU = std::make_unsigned_t<U>;
if constexpr (std::is_signed_v<T> == std::is_signed_v<U>) {
return t < u;
}
else if constexpr (std::is_signed_v<T>) {
return t < 0 ? true : UT(t) < u;
}
else {
return u < 0 ? false : t < UU(u);
}
}
template< typename T, typename U >
constexpr bool cmp_greater(T t, U u) noexcept {
return wjr::cmp_less(u, t);
}
template< typename T, typename U >
constexpr bool cmp_less_equal(T t, U u) noexcept {
return !wjr::cmp_greater(t, u);
}
template< typename T, typename U >
constexpr bool cmp_greater_equal(T t, U u) noexcept {
return !wjr::cmp_less(t, u);
}
template< typename R, typename T>
constexpr bool in_range(T t) noexcept {
return wjr::cmp_greater_equal(t, std::numeric_limits<R>::min()) &&
wjr::cmp_less_equal(t, std::numeric_limits<R>::max());
}
template<typename T, std::enable_if_t<is_integrals_v<T>, int>>
WJR_ATTRIBUTE(INTRINSIC_CONSTEXPR) T mul(T a, T b, T& hi) {
if constexpr (sizeof(T) == 8) {
if constexpr (std::is_signed_v<T>) {
static_assert(std::is_same_v<T, int64_t>, "");
return s64x64(a, b, hi);
}
else {
static_assert(std::is_same_v<T, uint64_t>, "");
return u64x64(a, b, hi);
}
}
else {
static_assert(sizeof(T) <= 4, "");
using dT = int_or_uint_t<2 * sizeof(T), std::is_signed_v<T>>;
dT ret = dT(a) * dT(b);
hi = ret >> (8 * sizeof(T));
return T(ret);
}
}
template<typename T, std::enable_if_t<is_integrals_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) T mulhi(T a, T b) {
if constexpr (sizeof(T) == 8) {
if constexpr (std::is_signed_v<T>) {
static_assert(std::is_same_v<T, int64_t>, "");
return s64x64hi(a, b);
}
else {
static_assert(std::is_same_v<T, uint64_t>, "");
return u64x64hi(a, b);
}
}
else {
static_assert(sizeof(T) <= 4, "");
using dT = int_or_uint_t<2 * sizeof(T), std::is_signed_v<T>>;
dT ret = dT(a) * dT(b);
return T(ret >> (8 * sizeof(T)));
}
}
template<typename T, std::enable_if_t<is_integrals_v<T>, int>>
WJR_ATTRIBUTE(CONST, INTRINSIC_CONSTEXPR) T mullo(T a, T b) {
if constexpr (sizeof(T) == 8) {
if constexpr (std::is_signed_v<T>) {
static_assert(std::is_same_v<T, int64_t>, "");
return s64x64lo(a, b);
}
else {
static_assert(std::is_same_v<T, uint64_t>, "");
return u64x64lo(a, b);
}
}
else {
static_assert(sizeof(T) <= 4, "");
using dT = int_or_uint_t<2 * sizeof(T), std::is_signed_v<T>>;
dT ret = dT(a) * dT(b);
return T(ret);
}
}
template<typename T, std::enable_if_t<is_standard_numer_v<T>, int>>
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE, CONSTEXPR) T power(T a, unsigned int b) {
T ret = 1;
while (b) {
if (b & 1)ret *= a;
a *= a;
b >>= 1;
}
return ret;
}
template<unsigned int base, unsigned int SIZE>
struct __width_table {
constexpr static unsigned int max_size = SIZE;
static_assert(SIZE <= 2048, "");
constexpr __width_table() : m_table() {
m_table[0] = -1;
for (unsigned int i = 1; i < SIZE; ++i) {
m_table[i] = m_table[i / base] + 1;
}
}
inline constexpr uint8_t operator[](unsigned int idx) const { return m_table[idx]; }
private:
uint8_t m_table[SIZE];
};
template<unsigned int base>
struct __get_width_table_size {
constexpr static unsigned int value = 0;
};
template<unsigned int base>
static constexpr __width_table<base, __get_width_table_size<base>::value> __width_table_v;
template<>
struct __get_width_table_size<2> {
constexpr static unsigned int value = 512;
};
template<>
struct __get_width_table_size<3> {
constexpr static unsigned int value = 729;
};
template<>
struct __get_width_table_size<5> {
constexpr static unsigned int value = 625;
};
template<>
struct __get_width_table_size<6> {
constexpr static unsigned int value = 1296;
};
template<>
struct __get_width_table_size<7> {
constexpr static unsigned int value = 343;
};
template<>
struct __get_width_table_size<10> {
constexpr static unsigned int value = 1000;
};
template<>
struct __get_width_table_size<11> {
constexpr static unsigned int value = 121;
};
template<>
struct __get_width_table_size<12> {
constexpr static unsigned int value = 144;
};
template<>
struct __get_width_table_size<13> {
constexpr static unsigned int value = 169;
};
template<>
struct __get_width_table_size<14> {
constexpr static unsigned int value = 196;
};
template<>
struct __get_width_table_size<15> {
constexpr static unsigned int value = 225;
};
template<>
struct __get_width_table_size<17> {
constexpr static unsigned int value = 289;
};
template<>
struct __get_width_table_size<18> {
constexpr static unsigned int value = 324;
};
template<>
struct __get_width_table_size<19> {
constexpr static unsigned int value = 361;
};
template<>
struct __get_width_table_size<20> {
constexpr static unsigned int value = 400;
};
template<>
struct __get_width_table_size<21> {
constexpr static unsigned int value = 441;
};
template<>
struct __get_width_table_size<22> {
constexpr static unsigned int value = 484;
};
template<unsigned int base>
struct __get_width_table {
constexpr static unsigned int value = -1;
};
template<>
struct __get_width_table<2> {
constexpr static unsigned int value = 2;
};
template<>
struct __get_width_table<3> {
constexpr static unsigned int value = 3;
};
template<>
struct __get_width_table<4> {
constexpr static unsigned int value = 2;
};
template<>
struct __get_width_table<5> {
constexpr static unsigned int value = 5;
};
template<>
struct __get_width_table<6> {
constexpr static unsigned int value = 6;
};
template<>
struct __get_width_table<7> {
constexpr static unsigned int value = 7;
};
template<>
struct __get_width_table<8> {
constexpr static unsigned int value = 2;
};
template<>
struct __get_width_table<9> {
constexpr static unsigned int value = 3;
};
template<>
struct __get_width_table<10> {
constexpr static unsigned int value = 10;
};
template<>
struct __get_width_table<11> {
constexpr static unsigned int value = 11;
};
template<>
struct __get_width_table<12> {
constexpr static unsigned int value = 12;
};
template<>
struct __get_width_table<13> {
constexpr static unsigned int value = 13;
};
template<>
struct __get_width_table<14> {
constexpr static unsigned int value = 14;
};
template<>
struct __get_width_table<15> {
constexpr static unsigned int value = 15;
};
template<>
struct __get_width_table<16> {
constexpr static unsigned int value = 2;
};
template<>
struct __get_width_table<17> {
constexpr static unsigned int value = 17;
};
template<>
struct __get_width_table<18> {
constexpr static unsigned int value = 18;
};
template<>
struct __get_width_table<19> {
constexpr static unsigned int value = 19;
};
template<>
struct __get_width_table<20> {
constexpr static unsigned int value = 20;
};
template<>
struct __get_width_table<21> {
constexpr static unsigned int value = 21;
};
template<>
struct __get_width_table<22> {
constexpr static unsigned int value = 22;
};
template<>
struct __get_width_table<25> {
constexpr static unsigned int value = 5;
};
template<unsigned int base>
inline constexpr unsigned int __get_width_table_v = __get_width_table<base>::value;
template<unsigned int base, typename T, unsigned int digits>
inline constexpr unsigned int __width_depth() {
if constexpr (digits <= 2) {
return 1;
}
else {
constexpr auto mid_digits = (digits + 1) / 2;
constexpr auto __table_index = __get_width_table_v<base>;
if constexpr (__table_index != -1) {
constexpr auto __table_size = __get_width_table_size<__table_index>::value;
constexpr auto mid = power<T>(base, mid_digits);
if constexpr (mid <= __table_size) {
return 1;
}
else {
return 1 + __width_depth<base, T, mid_digits>();
}
}
else {
return 1 + __width_depth<base, T, mid_digits>();
}
}
}
template<unsigned int base, typename T, unsigned int digits>
inline constexpr bool __width_end_with_table() {
if constexpr (digits <= 2) {
return false;
}
else {
constexpr auto mid_digits = (digits + 1) / 2;
constexpr auto __table_index = __get_width_table_v<base>;
if constexpr (__table_index != -1) {
constexpr auto __table_size = __get_width_table_size<__table_index>::value;
constexpr auto mid = power<T>(base, mid_digits);
if constexpr (mid <= __table_size) {
return true;
}
else {
return __width_end_with_table<base, T, mid_digits>();
}
}
else {
return __width_end_with_table<base, T, mid_digits>();
}
}
}
#define __WIDTH_WORK_GEN(x)	                                \
if constexpr(depth >= x){	                                \
constexpr auto mid_digits##x = get_mid_digits(x);	    \
constexpr auto mid##x = power<T>(base, mid_digits##x);	\
if(a >= mid##x){	                                    \
ret += mid_digits##x;	                            \
a /= mid##x;		                                \
}	                                                    \
}
template<unsigned int base, typename T, unsigned int digits, unsigned int depth, bool use_table>
inline constexpr void __width(T a, unsigned int& ret) {
constexpr auto get_mid_digits = [](int x) {
unsigned int D = digits;
while (x) {
D = (D + 1) / 2;
--x;
}
return D;
};
if constexpr (depth <= 4) {
WJR_MACRO_CALL(__WIDTH_WORK_GEN, , 1, 2, 3, 4);
if constexpr (use_table) {
constexpr auto __mid_digits = get_mid_digits(depth);
constexpr auto __mid = power<T>(base, __mid_digits);
constexpr auto __table_index = __get_width_table_v<base>;
constexpr auto __table_size = __get_width_table_size<__table_index>::value;
constexpr auto p = []() {
unsigned int ret = 0;
unsigned int idx = base;
while (idx != 1) {
++ret;
idx /= __table_index;
}
return ret;
}();
ret += (__width_table_v<__table_index>[static_cast<unsigned int>(a)] + p - 1) / p;
}
}
else {
WJR_MACRO_CALL(__WIDTH_WORK_GEN, , 1, 2, 3, 4);
__width<base, T, get_mid_digits(4), depth - 4, use_table>(a, ret);
}
}
#undef __WIDTH_WORK_GEN
template<unsigned int base, typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
inline constexpr unsigned int base_width(T a) {
constexpr auto __digits = base_digits_v<T, base>;
constexpr auto __depth = __width_depth<base, T, __digits>();
constexpr auto __use_table = __width_end_with_table<base, T, __digits>();
if (WJR_UNLIKELY(!a)) return 0;
unsigned int ret = 1;
__width<base, T, __digits, __depth, __use_table>(a, ret);
return ret;
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
inline constexpr unsigned int base2_width(T a) {
return base_width<2>(a);
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
inline constexpr unsigned int base10_width(T a) {
return base_width<10>(a);
}
template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int>>
inline constexpr unsigned int base_width(unsigned int b, T a) {
switch (b) {
case 2: return base_width<2>(a);
case 3: return base_width<3>(a);
case 4: return base_width<4>(a);
case 5: return base_width<5>(a);
case 6: return base_width<6>(a);
case 7: return base_width<7>(a);
case 8: return base_width<8>(a);
case 9: return base_width<9>(a);
case 10: return base_width<10>(a);
case 16: return base_width<16>(a);
case 25: return base_width<25>(a);
default: {
WJR_ASSUME(b != 0 && b != 1);
unsigned int ret = 0;
while (a) {
++ret;
a /= b;
}
return ret;
}
}
}
template<>
struct broadcast_fn<uint8_t, uint8_t> {
WJR_INTRINSIC_CONSTEXPR uint8_t operator()(uint8_t x)const {
return x;
}
};
template<>
struct broadcast_fn<uint16_t, uint16_t> {
WJR_INTRINSIC_CONSTEXPR uint16_t operator()(uint16_t x)const {
return x;
}
};
template<>
struct broadcast_fn<uint32_t, uint32_t> {
WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint32_t x)const {
return x;
}
};
template<>
struct broadcast_fn<uint64_t, uint64_t> {
WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint64_t x)const {
return x;
}
};
template<>
struct broadcast_fn<uint16_t, uint8_t> {
WJR_INTRINSIC_CONSTEXPR uint16_t operator()(uint8_t x)const {
return static_cast<uint16_t>(static_cast<uint32_t>(x) | (static_cast<uint16_t>(x) << 8));
}
};
template<>
struct broadcast_fn<uint32_t, uint16_t> {
WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint16_t x)const {
return x | (static_cast<uint32_t>(x) << 16);
}
};
template<>
struct broadcast_fn<uint64_t, uint32_t> {
WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint32_t x)const {
return static_cast<uint64_t>(x) | (static_cast<uint64_t>(x) << 32);
}
};
template<>
struct broadcast_fn<uint32_t, uint8_t> {
WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint8_t x)const {
return x * static_cast<uint32_t>(0x01010101u);
}
};
template<>
struct broadcast_fn<uint64_t, uint16_t> {
WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint16_t x)const {
return x * static_cast<uint64_t>(0x0001000100010001ull);
}
};
template<>
struct broadcast_fn<uint64_t, uint8_t> {
WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint8_t x)const {
return x * static_cast<uint64_t>(0x0101010101010101ull);
}
};
_WJR_END

#endif // __WJR_MATH_H

#pragma once
#ifndef __WJR_SIMD_SIMD_CAST_H
#define __WJR_SIMD_SIMD_CAST_H
_WJR_SIMD_BEGIN
struct lo {};
struct hi {};
template<typename T, typename U>
struct simd_cast_fn;
template<typename T, typename U>
inline constexpr simd_cast_fn<T, U> simd_cast;
#define __REGISTER_BASIC_CAST(FROM, TO, func)											                    \
template<>	                                                                                                \
struct simd_cast_fn<TO##_tag, FROM##_tag>{	                                                                \
TO operator()(FROM v) const{	                                                                        \
return func(v);	                                                                                    \
}	                                                                                                    \
};
#define __REGISTER_INTEGER1_CAST(FROM, TO, func1, func2)					                                \
template<>	                                                                                                \
struct simd_cast_fn<TO##_tag, FROM>{	                                                                    \
TO operator()(FROM v) const{		                                                                    \
return func1(v);	                                                                                \
}	                                                                                                    \
};                                                                                                          \
template<>	                                                                                                \
struct simd_cast_fn<FROM, TO##_tag>{	                                                                    \
FROM operator()(TO v) const{		                                                                    \
return static_cast<FROM>(func2(v));	                                                                \
}	                                                                                                    \
};
template<typename T>
struct __simd_wrapper;
template<typename T>
using __simd_wrapper_t = typename __simd_wrapper<T>::type;
#if WJR_SSE
struct __m128_tag { using type = __m128; };
template<>
struct __simd_wrapper<__m128> {
using type = __m128_tag;
};
#endif // WJR_SSE
#if WJR_SSE2
struct __m128i_tag { using type = __m128i; };
struct __m128d_tag { using type = __m128d; };
template<>
struct __simd_wrapper<__m128i> {
using type = __m128i_tag;
};
template<>
struct __simd_wrapper<__m128d> {
using type = __m128d_tag;
};
__REGISTER_BASIC_CAST(__m128, __m128i, _mm_castps_si128);
__REGISTER_BASIC_CAST(__m128, __m128d, _mm_castps_pd);
__REGISTER_BASIC_CAST(__m128i, __m128, _mm_castsi128_ps);
__REGISTER_BASIC_CAST(__m128i, __m128d, _mm_castsi128_pd);
__REGISTER_BASIC_CAST(__m128d, __m128, _mm_castpd_ps);
__REGISTER_BASIC_CAST(__m128d, __m128i, _mm_castpd_si128);
__REGISTER_INTEGER1_CAST(int8_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(int16_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(int32_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(int64_t, __m128i, _mm_cvtsi64_si128, _mm_cvtsi128_si64);
__REGISTER_INTEGER1_CAST(uint8_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(uint16_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(uint32_t, __m128i, _mm_cvtsi32_si128, _mm_cvtsi128_si32);
__REGISTER_INTEGER1_CAST(uint64_t, __m128i, _mm_cvtsi64_si128, _mm_cvtsi128_si64);
#endif // WJR_SSE2
#if WJR_AVX
struct __m256_tag { using type = __m256; };
struct __m256i_tag { using type = __m256i; };
struct __m256d_tag { using type = __m256d; };
template<>
struct __simd_wrapper<__m256> {
using type = __m256_tag;
};
template<>
struct __simd_wrapper<__m256i> {
using type = __m256i_tag;
};
template<>
struct __simd_wrapper<__m256d> {
using type = __m256d_tag;
};
__REGISTER_BASIC_CAST(__m256, __m256i, _mm256_castps_si256);
__REGISTER_BASIC_CAST(__m256, __m256d, _mm256_castps_pd);
__REGISTER_BASIC_CAST(__m256i, __m256, _mm256_castsi256_ps);
__REGISTER_BASIC_CAST(__m256i, __m256d, _mm256_castsi256_pd);
__REGISTER_BASIC_CAST(__m256d, __m256, _mm256_castpd_ps);
__REGISTER_BASIC_CAST(__m256d, __m256i, _mm256_castpd_si256);
__REGISTER_BASIC_CAST(__m128i, __m256i, _mm256_castsi128_si256);
__REGISTER_BASIC_CAST(__m256i, __m128i, _mm256_castsi256_si128);
#define __AVX_CVT2_INT32(x) simd_cast<uint32_t, __m128i_tag>(simd_cast<__m128i_tag, __m256i_tag>(x))
#define __AVX_CVT2_INT64(x) simd_cast<uint64_t, __m128i_tag>(simd_cast<__m128i_tag, __m256i_tag>(x))
#define __INT32_CVT2_AVX(x) simd_cast<__m256i_tag, __m128i_tag>(simd_cast<__m128i_tag, uint32_t>(x))
#define __INT64_CVT2_AVX(x) simd_cast<__m256i_tag, __m128i_tag>(simd_cast<__m128i_tag, uint64_t>(x))
__REGISTER_INTEGER1_CAST(int8_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(int16_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(int32_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(int64_t, __m256i, __INT64_CVT2_AVX, __AVX_CVT2_INT64);
__REGISTER_INTEGER1_CAST(uint8_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(uint16_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(uint32_t, __m256i, __INT32_CVT2_AVX, __AVX_CVT2_INT32);
__REGISTER_INTEGER1_CAST(uint64_t, __m256i, __INT64_CVT2_AVX, __AVX_CVT2_INT64);
#undef __AVX_CVT2_INT32
#undef __AVX_CVT2_INT64
#undef __INT32_CVT2_AVX
#undef __INT64_CVT2_AVX
#endif // WJR_AVX
#undef __REGISTER_INTEGER2_CAST
#undef __REGISTER_INTEGER1_CAST
#undef __REGISTER_BASIC_CAST
_WJR_SIMD_END
#endif // __WJR_SIMD_SIMD_CAST_H

_WJR_SIMD_BEGIN
#if WJR_SSE2
WJR_INTRINSIC_INLINE static __m128i mm_loadu_si16(void const* p);
WJR_INTRINSIC_INLINE static __m128i mm_loadu_si32(void const* p);
WJR_INTRINSIC_INLINE static __m128i mm_loadu_si64(void const* p);
#endif // WJR_SSE2
struct sse {
using mask_type = uint16_t;
#if WJR_SSE
using float_type = __m128;
#endif // WJR_SSE
#if WJR_SSE2
using int_type = __m128i;
using double_type = __m128d;
#endif // WJR_SSE2
constexpr static size_t width();
constexpr static mask_type mask();
#if WJR_SSE
WJR_INTRINSIC_INLINE static mask_type movemask_ps(__m128 v);
WJR_INTRINSIC_INLINE static void sfence();
template<int imm8>
WJR_INTRINSIC_INLINE static __m128 shuffle_ps(__m128 a, __m128 b);
#endif // WJR_SSE
#if WJR_SSE2
WJR_INTRINSIC_INLINE static __m128i add_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i add_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i add_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i add_epi64(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, int64_t);
WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i add(__m128i a, __m128i b, uint64_t);
WJR_INTRINSIC_INLINE static int8_t add_epi8(__m128i a);
WJR_INTRINSIC_INLINE static int16_t add_epi16(__m128i a);
WJR_INTRINSIC_INLINE static int32_t add_epi32(__m128i a);
WJR_INTRINSIC_INLINE static int64_t add_epi64(__m128i a);
WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m128i a);
WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m128i a);
WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m128i a);
WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m128i a);
WJR_INTRINSIC_INLINE static int8_t add(__m128i a, int8_t);
WJR_INTRINSIC_INLINE static int16_t add(__m128i a, int16_t);
WJR_INTRINSIC_INLINE static int32_t add(__m128i a, int32_t);
WJR_INTRINSIC_INLINE static int64_t add(__m128i a, int64_t);
WJR_INTRINSIC_INLINE static uint8_t add(__m128i a, uint8_t);
WJR_INTRINSIC_INLINE static uint16_t add(__m128i a, uint16_t);
WJR_INTRINSIC_INLINE static uint32_t add(__m128i a, uint32_t);
WJR_INTRINSIC_INLINE static uint64_t add(__m128i a, uint64_t);
WJR_INTRINSIC_INLINE static __m128i adds_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i adds_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i adds_epu8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i adds_epu16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i adds(__m128i a, __m128i b, uint16_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i alignr_epi16(__m128i a, __m128i b, int c);
WJR_INTRINSIC_INLINE static __m128i alignr_epi32(__m128i a, __m128i b, int c);
WJR_INTRINSIC_INLINE static __m128i alignr_epi64(__m128i a, __m128i b, int c);
WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int16_t);
WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int32_t);
WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, int64_t);
WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint16_t);
WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint32_t);
WJR_INTRINSIC_INLINE static __m128i alignr(__m128i a, __m128i b, int c, uint64_t);
WJR_INTRINSIC_INLINE static __m128i And(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i AndNot(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i avg_epu8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i avg_epu16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i avg(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i blendv_epi8(__m128i a, __m128i b, __m128i mask);
WJR_INTRINSIC_INLINE static __m128i blendv_epi16(__m128i a, __m128i b, __m128i mask);
WJR_INTRINSIC_INLINE static __m128i blendv_epi32(__m128i a, __m128i b, __m128i mask);
WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int8_t);
WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int16_t);
WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, int32_t);
WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint8_t);
WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint16_t);
WJR_INTRINSIC_INLINE static __m128i blendv(__m128i a, __m128i b, __m128i mask, uint32_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i bslli(__m128i val);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i bsrli(__m128i val);
WJR_INTRINSIC_INLINE static __m128i cmpeq_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpeq_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpeq_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i cmpge_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpge_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpge_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpge_epu8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpge_epu16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpge_epu32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i cmpge(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i cmpgt_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpgt_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpgt_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpgt_epu8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpgt_epu16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpgt_epu32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i cmple_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmple_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmple_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmple_epu8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmple_epu16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmple_epu32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i cmple(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i cmplt_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmplt_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmplt_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmplt_epu8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmplt_epu16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmplt_epu32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i cmplt(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i cmpne_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpne_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpne_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i cmpne(__m128i a, __m128i b, uint32_t);
template<typename T>
WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::equal_to<>, T);
template<typename T>
WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::not_equal_to<>, T);
template<typename T>
WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater<>, T);
template<typename T>
WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::greater_equal<>, T);
template<typename T>
WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less<>, T);
template<typename T>
WJR_INTRINSIC_INLINE static __m128i cmp(__m128i a, __m128i b, std::less_equal<>, T);
WJR_INTRINSIC_INLINE static __m128i concat(uint64_t lo, uint64_t hi);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract_epi8(__m128i a);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract_epi16(__m128i a);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract_epi32(__m128i a);
template<int imm8>
WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m128i a);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract(__m128i a, int8_t);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract(__m128i a, int16_t);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract(__m128i a, int32_t);
template<int imm8>
WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, int64_t);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract(__m128i a, uint8_t);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract(__m128i a, uint16_t);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract(__m128i a, uint32_t);
template<int imm8>
WJR_INTRINSIC_INLINE static int64_t extract(__m128i a, uint64_t);
WJR_INTRINSIC_INLINE static uint64_t getlow(__m128i v);
WJR_INTRINSIC_INLINE static uint64_t gethigh(__m128i v);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert_epi16(__m128i a, int i);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int16_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint16_t);
WJR_INTRINSIC_INLINE static void lfence();
WJR_INTRINSIC_INLINE static __m128i load(const __m128i* ptr);
WJR_INTRINSIC_INLINE static __m128i loadu(const __m128i* ptr);
WJR_INTRINSIC_INLINE static __m128i loadu_si16(const void* ptr);
WJR_INTRINSIC_INLINE static __m128i loadu_si32(const void* ptr);
WJR_INTRINSIC_INLINE static __m128i loadu_si64(const void* ptr);
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
WJR_INTRINSIC_INLINE static __m128i logical_and(__m128i a, __m128i b, T);
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
WJR_INTRINSIC_INLINE static __m128i logical_not(__m128i v, T);
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
WJR_INTRINSIC_INLINE static __m128i logical_or(__m128i a, __m128i b, T);
WJR_INTRINSIC_INLINE static __m128i madd_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static void maskmoveu(__m128i a, __m128i mask, char* mem_addr);
WJR_INTRINSIC_INLINE static __m128i max_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i max_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i max_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i max_epu8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i max_epu16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i max_epu32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i max(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static int8_t max_epi8(__m128i a);
WJR_INTRINSIC_INLINE static int16_t max_epi16(__m128i a);
WJR_INTRINSIC_INLINE static int32_t max_epi32(__m128i a);
WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m128i a);
WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m128i a);
WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m128i a);
WJR_INTRINSIC_INLINE static int8_t max(__m128i a, int8_t);
WJR_INTRINSIC_INLINE static int16_t max(__m128i a, int16_t);
WJR_INTRINSIC_INLINE static int32_t max(__m128i a, int32_t);
WJR_INTRINSIC_INLINE static uint8_t max(__m128i a, uint8_t);
WJR_INTRINSIC_INLINE static uint16_t max(__m128i a, uint16_t);
WJR_INTRINSIC_INLINE static uint32_t max(__m128i a, uint32_t);
WJR_INTRINSIC_INLINE static void mfence();
WJR_INTRINSIC_INLINE static __m128i min_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i min_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i min_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i min_epu8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i min_epu16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i min_epu32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i min(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static int8_t min_epi8(__m128i a);
WJR_INTRINSIC_INLINE static int16_t min_epi16(__m128i a);
WJR_INTRINSIC_INLINE static int32_t min_epi32(__m128i a);
WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m128i a);
WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m128i a);
WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m128i a);
WJR_INTRINSIC_INLINE static int8_t min(__m128i a, int8_t);
WJR_INTRINSIC_INLINE static int16_t min(__m128i a, int16_t);
WJR_INTRINSIC_INLINE static int32_t min(__m128i a, int32_t);
WJR_INTRINSIC_INLINE static uint8_t min(__m128i a, uint8_t);
WJR_INTRINSIC_INLINE static uint16_t min(__m128i a, uint16_t);
WJR_INTRINSIC_INLINE static uint32_t min(__m128i a, uint32_t);
WJR_INTRINSIC_INLINE static __m128i move_epi64(__m128i a);
WJR_INTRINSIC_INLINE static mask_type movemask_epi8(__m128i a);
WJR_INTRINSIC_INLINE static mask_type movemask_pd(__m128d v);
WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int8_t);
WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int32_t);
WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, int64_t);
WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint8_t);
WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint32_t);
WJR_INTRINSIC_INLINE static mask_type movemask(__m128i v, uint64_t);
WJR_INTRINSIC_INLINE static __m128i mul_epu32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i mulhi_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i mulhi_epu16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i mullo_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i negate_epi8(__m128i a);
WJR_INTRINSIC_INLINE static __m128i negate_epi16(__m128i a);
WJR_INTRINSIC_INLINE static __m128i negate_epi32(__m128i a);
WJR_INTRINSIC_INLINE static __m128i negate_epi64(__m128i a);
WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int8_t);
WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int16_t);
WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int32_t);
WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, int64_t);
WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint8_t);
WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint16_t);
WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint32_t);
WJR_INTRINSIC_INLINE static __m128i negate(__m128i a, uint64_t);
WJR_INTRINSIC_INLINE static __m128i Not(__m128i v);
WJR_INTRINSIC_INLINE static __m128i Or(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i packs_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i packs_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i packus_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i preloadu_si16(const void* ptr);
WJR_INTRINSIC_INLINE static __m128i preloadu_si32(const void* ptr);
WJR_INTRINSIC_INLINE static __m128i preloadu_si48(const void* ptr);
WJR_INTRINSIC_INLINE static __m128i preloadu_si64(const void* ptr);
WJR_INTRINSIC_INLINE static __m128i preloadu_si80(const void* ptr);
WJR_INTRINSIC_INLINE static __m128i preloadu_si96(const void* ptr);
WJR_INTRINSIC_INLINE static __m128i preloadu_si112(const void* ptr);
WJR_INTRINSIC_INLINE static __m128i preloadu_si128(const void* ptr);
WJR_INTRINSIC_INLINE static __m128i preloadu_si16x(const void* ptr, int n);
WJR_INTRINSIC_INLINE static __m128i sad_epu8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i zeros();
WJR_INTRINSIC_INLINE static __m128i ones();
WJR_INTRINSIC_INLINE static __m128i set_epi8(
char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0);
WJR_INTRINSIC_INLINE static __m128i set_epi16(
short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0);
WJR_INTRINSIC_INLINE static __m128i set_epi32(int e3, int e2, int e1, int e0);
WJR_INTRINSIC_INLINE static __m128i set_epi64x(long long e1, long long e0);
WJR_INTRINSIC_INLINE static __m128i setr_epi8(
char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0);
WJR_INTRINSIC_INLINE static __m128i setr_epi16(
short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0);
WJR_INTRINSIC_INLINE static __m128i setr_epi32(int e3, int e2, int e1, int e0);
WJR_INTRINSIC_INLINE static __m128i set1_epi8(int8_t val);
WJR_INTRINSIC_INLINE static __m128i set1_epi16(int16_t val);
WJR_INTRINSIC_INLINE static __m128i set1_epi32(int32_t val);
WJR_INTRINSIC_INLINE static __m128i set1_epi64(int64_t val);
WJR_INTRINSIC_INLINE static __m128i set1(int8_t val, int8_t);
WJR_INTRINSIC_INLINE static __m128i set1(int16_t val, int16_t);
WJR_INTRINSIC_INLINE static __m128i set1(int32_t val, int32_t);
WJR_INTRINSIC_INLINE static __m128i set1(int64_t val, int64_t);
WJR_INTRINSIC_INLINE static __m128i set1(uint8_t val, uint8_t);
WJR_INTRINSIC_INLINE static __m128i set1(uint16_t val, uint16_t);
WJR_INTRINSIC_INLINE static __m128i set1(uint32_t val, uint32_t);
WJR_INTRINSIC_INLINE static __m128i set1(uint64_t val, uint64_t);
WJR_INTRINSIC_INLINE static __m128i setmin_epi8();
WJR_INTRINSIC_INLINE static __m128i setmin_epi16();
WJR_INTRINSIC_INLINE static __m128i setmin_epi32();
WJR_INTRINSIC_INLINE static __m128i setmin(int8_t);
WJR_INTRINSIC_INLINE static __m128i setmin(int16_t);
WJR_INTRINSIC_INLINE static __m128i setmin(int32_t);
WJR_INTRINSIC_INLINE static __m128i setmin(uint8_t);
WJR_INTRINSIC_INLINE static __m128i setmin(uint16_t);
WJR_INTRINSIC_INLINE static __m128i setmin(uint32_t);
WJR_INTRINSIC_INLINE static __m128i setmax_epi8();
WJR_INTRINSIC_INLINE static __m128i setmax_epi16();
WJR_INTRINSIC_INLINE static __m128i setmax_epi32();
WJR_INTRINSIC_INLINE static __m128i setmax(int8_t);
WJR_INTRINSIC_INLINE static __m128i setmax(int16_t);
WJR_INTRINSIC_INLINE static __m128i setmax(int32_t);
WJR_INTRINSIC_INLINE static __m128i setmax(uint8_t);
WJR_INTRINSIC_INLINE static __m128i setmax(uint16_t);
WJR_INTRINSIC_INLINE static __m128i setmax(uint32_t);
template<int imm>
WJR_INTRINSIC_INLINE static __m128i shl(__m128i a);
template<int imm>
WJR_INTRINSIC_INLINE static __m128i shr(__m128i b);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i shuffle_epi32(__m128i v);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i shufflehi_epi16(__m128i v);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i shufflelo_epi16(__m128i v);
WJR_INTRINSIC_INLINE static __m128i sll_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sll_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sll_epi64(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, int64_t);
WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i sll(__m128i a, __m128i b, uint64_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i slli(__m128i v);
WJR_INTRINSIC_INLINE static __m128i slli_epi16(__m128i a, int imm8);
WJR_INTRINSIC_INLINE static __m128i slli_epi32(__m128i a, int imm8);
WJR_INTRINSIC_INLINE static __m128i slli_epi64(__m128i a, int imm8);
WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int16_t);
WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int32_t);
WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, int64_t);
WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint16_t);
WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint32_t);
WJR_INTRINSIC_INLINE static __m128i slli(__m128i a, int imm8, uint64_t);
WJR_INTRINSIC_INLINE static __m128i sra_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sra_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i sra(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i srai_epi16(__m128i a, int imm8);
WJR_INTRINSIC_INLINE static __m128i srai_epi32(__m128i a, int imm8);
WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int16_t);
WJR_INTRINSIC_INLINE static __m128i srai(__m128i a, int imm8, int32_t);
WJR_INTRINSIC_INLINE static __m128i srl_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i srl_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i srl_epi64(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, int64_t);
WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i srl(__m128i a, __m128i b, uint64_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i srli(__m128i v);
WJR_INTRINSIC_INLINE static __m128i srli_epi16(__m128i a, int imm8);
WJR_INTRINSIC_INLINE static __m128i srli_epi32(__m128i a, int imm8);
WJR_INTRINSIC_INLINE static __m128i srli_epi64(__m128i a, int imm8);
WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int16_t);
WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int32_t);
WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, int64_t);
WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint16_t);
WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint32_t);
WJR_INTRINSIC_INLINE static __m128i srli(__m128i a, int imm8, uint64_t);
WJR_INTRINSIC_INLINE static void stream(__m128i* ptr, __m128i v);
WJR_INTRINSIC_INLINE static void store(__m128i* ptr, __m128i val);
WJR_INTRINSIC_INLINE static void storeu(__m128i* ptr, __m128i val);
WJR_INTRINSIC_INLINE static __m128i sub_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sub_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sub_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sub_epi64(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, int64_t);
WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i sub(__m128i a, __m128i b, uint64_t);
WJR_INTRINSIC_INLINE static __m128i subs_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i subs_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i subs_epu8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i subs_epu16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i subs(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i unpackhi_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i unpackhi_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i unpackhi_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i unpackhi_epi64(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, int64_t);
WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i unpackhi(__m128i a, __m128i b, uint64_t);
WJR_INTRINSIC_INLINE static __m128i unpacklo_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i unpacklo_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i unpacklo_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i unpacklo_epi64(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, int64_t);
WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i unpacklo(__m128i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m128i Xor(__m128i a, __m128i b);
#endif // WJR_SSE2
#if WJR_SSE3
WJR_INTRINSIC_INLINE static __m128i lddqu(const __m128i* ptr);
#endif // WJR_SSE3
#if WJR_SSSE3
WJR_INTRINSIC_INLINE static __m128i abs_epi8(__m128i val);
WJR_INTRINSIC_INLINE static __m128i abs_epi16(__m128i val);
WJR_INTRINSIC_INLINE static __m128i abs_epi32(__m128i val);
WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int8_t);
WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int16_t);
WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, int32_t);
WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint8_t);
WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint16_t);
WJR_INTRINSIC_INLINE static __m128i abs(__m128i val, uint32_t);
WJR_INTRINSIC_INLINE static __m128i shuffle_epi8(__m128i v, __m128i imm8);
WJR_INTRINSIC_INLINE static __m128i sign_epi8(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sign_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sign_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int8_t);
WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint8_t);
WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m128i sign(__m128i a, __m128i b, uint32_t);
#endif // WJR_SSSE3
#if WJR_SSE4_1
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i blend_epi16(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpeq_epi64(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, int64_t);
WJR_INTRINSIC_INLINE static __m128i cmpeq(__m128i a, __m128i b, uint64_t);
WJR_INTRINSIC_INLINE static __m128i cmpgt_epi64(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i cmpgt(__m128i a, __m128i b, int64_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert_epi8(__m128i a, int i);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert_epi32(__m128i a, int i);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert_epi64(__m128i a, int64_t i);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int8_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, int32_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, int64_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint8_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int i, uint32_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i insert(__m128i a, int64_t i, uint64_t);
WJR_INTRINSIC_INLINE static __m128i minpos_epu16(__m128i a);
WJR_INTRINSIC_INLINE static __m128i mul_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i mullo_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i packus_epi32(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static __m128i stream_load(__m128i* p);
WJR_INTRINSIC_INLINE static int test_all_ones(__m128i a);
WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static int test_all_zeros(__m128i a);
WJR_INTRINSIC_INLINE static int test_mix_ones_zeros(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static int testc(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static int testnzc(__m128i a, __m128i b);
WJR_INTRINSIC_INLINE static int testz(__m128i a, __m128i b);
#endif // WJR_SSE4_1
};
struct avx {
using mask_type = uint32_t;
#if WJR_AVX
using float_type = __m256;
using int_type = __m256i;
using double_type = __m256d;
#endif // WJR_AVX
constexpr static size_t width();
constexpr static mask_type mask();
#if WJR_AVX
WJR_INTRINSIC_INLINE static __m256i concat(__m128i a, __m128i b);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract_epi32(__m256i v);
template<int imm8>
WJR_INTRINSIC_INLINE static int64_t extract_epi64(__m256i v);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract(__m256i v, int32_t);
template<int imm8>
WJR_INTRINSIC_INLINE static int64_t extract(__m256i v, int64_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m128i extract_si128(__m256i v);
WJR_INTRINSIC_INLINE static __m128i getlow(__m256i a);
WJR_INTRINSIC_INLINE static __m128i gethigh(__m256i a);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i insert_epi8(__m256i v, int8_t i);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i insert_epi16(__m256i v, int16_t i);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i insert_epi32(__m256i v, int32_t i);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i insert_epi64(__m256i v, int64_t i);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i insert_si128(__m256i a, __m128i b);
WJR_INTRINSIC_INLINE static __m256i load(const __m256i* p);
WJR_INTRINSIC_INLINE static __m256i loadu(const __m256i* p);
WJR_INTRINSIC_INLINE static __m256i ones();
WJR_INTRINSIC_INLINE static __m256i preloadu_si16(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si32(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si48(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si64(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si80(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si96(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si112(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si128(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si144(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si160(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si176(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si192(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si208(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si224(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si240(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si256(const void* ptr);
WJR_INTRINSIC_INLINE static __m256i preloadu_si16x(const void* ptr, int n);
WJR_INTRINSIC_INLINE static __m256i set_epi8(
char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0);
WJR_INTRINSIC_INLINE static __m256i set_epi16(
short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0);
WJR_INTRINSIC_INLINE static __m256i set_epi32(
int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0);
WJR_INTRINSIC_INLINE static __m256i set_epi64x(
long long e3, long long e2, long long e1, long long e0);
WJR_INTRINSIC_INLINE static __m256i setr_epi8(
char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0);
WJR_INTRINSIC_INLINE static __m256i setr_epi16(
short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0);
WJR_INTRINSIC_INLINE static __m256i setr_epi32(
int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0);
WJR_INTRINSIC_INLINE static __m256i setr_epi64x(
long long e3, long long e2, long long e1, long long e0);
WJR_INTRINSIC_INLINE static __m256i set1_epi8(int8_t a);
WJR_INTRINSIC_INLINE static __m256i set1_epi16(int16_t a);
WJR_INTRINSIC_INLINE static __m256i set1_epi32(int32_t a);
WJR_INTRINSIC_INLINE static __m256i set1_epi64(int64_t a);
WJR_INTRINSIC_INLINE static __m256i set1(int8_t a, int8_t);
WJR_INTRINSIC_INLINE static __m256i set1(int16_t a, int16_t);
WJR_INTRINSIC_INLINE static __m256i set1(int32_t a, int32_t);
WJR_INTRINSIC_INLINE static __m256i set1(int64_t a, int64_t);
WJR_INTRINSIC_INLINE static __m256i set1(uint8_t a, uint8_t);
WJR_INTRINSIC_INLINE static __m256i set1(uint16_t a, uint16_t);
WJR_INTRINSIC_INLINE static __m256i set1(uint32_t a, uint32_t);
WJR_INTRINSIC_INLINE static __m256i set1(uint64_t a, uint64_t);
WJR_INTRINSIC_INLINE static __m256i setmin_epi8();
WJR_INTRINSIC_INLINE static __m256i setmin_epi16();
WJR_INTRINSIC_INLINE static __m256i setmin_epi32();
WJR_INTRINSIC_INLINE static __m256i setmin_epi64();
WJR_INTRINSIC_INLINE static __m256i setmin(int8_t);
WJR_INTRINSIC_INLINE static __m256i setmin(int16_t);
WJR_INTRINSIC_INLINE static __m256i setmin(int32_t);
WJR_INTRINSIC_INLINE static __m256i setmin(int64_t);
WJR_INTRINSIC_INLINE static __m256i setmax_epi8();
WJR_INTRINSIC_INLINE static __m256i setmax_epi16();
WJR_INTRINSIC_INLINE static __m256i setmax_epi32();
WJR_INTRINSIC_INLINE static __m256i setmax_epi64();
WJR_INTRINSIC_INLINE static __m256i setmax(int8_t);
WJR_INTRINSIC_INLINE static __m256i setmax(int16_t);
WJR_INTRINSIC_INLINE static __m256i setmax(int32_t);
WJR_INTRINSIC_INLINE static __m256i setmax(int64_t);
WJR_INTRINSIC_INLINE static void stream(__m256i* p, __m256i a);
WJR_INTRINSIC_INLINE static void store(__m256i* p, const __m256i& a);
WJR_INTRINSIC_INLINE static void storeu(__m256i* p, const __m256i& a);
WJR_INTRINSIC_INLINE static int test_all_zeros(__m256i a);
WJR_INTRINSIC_INLINE static int testc(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static int testnzc(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static int testz(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i zeros();
#endif // WJR_AVX
#if WJR_AVX2
WJR_INTRINSIC_INLINE static __m256i And(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i AndNot(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i Or(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i Xor(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i Not(__m256i v);
WJR_INTRINSIC_INLINE static __m256i abs_epi8(__m256i v);
WJR_INTRINSIC_INLINE static __m256i abs_epi16(__m256i v);
WJR_INTRINSIC_INLINE static __m256i abs_epi32(__m256i v);
WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int8_t);
WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int16_t);
WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int32_t);
WJR_INTRINSIC_INLINE static __m256i abs(__m256i v, int64_t);
WJR_INTRINSIC_INLINE static __m256i add_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i add_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i add_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i add_epi64(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, int64_t);
WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint32_t);
WJR_INTRINSIC_INLINE static __m256i add(__m256i a, __m256i b, uint64_t);
WJR_INTRINSIC_INLINE static uint8_t add_epu8(__m256i v);
WJR_INTRINSIC_INLINE static uint16_t add_epu16(__m256i v);
WJR_INTRINSIC_INLINE static uint32_t add_epu32(__m256i v);
WJR_INTRINSIC_INLINE static uint64_t add_epu64(__m256i v);
WJR_INTRINSIC_INLINE static int8_t add_epi8(__m256i v);
WJR_INTRINSIC_INLINE static int16_t add_epi16(__m256i v);
WJR_INTRINSIC_INLINE static int32_t add_epi32(__m256i v);
WJR_INTRINSIC_INLINE static int64_t add_epi64(__m256i v);
WJR_INTRINSIC_INLINE static int8_t add(__m256i v, int8_t);
WJR_INTRINSIC_INLINE static int16_t add(__m256i v, int16_t);
WJR_INTRINSIC_INLINE static int32_t add(__m256i v, int32_t);
WJR_INTRINSIC_INLINE static int64_t add(__m256i v, int64_t);
WJR_INTRINSIC_INLINE static uint8_t add(__m256i v, uint8_t);
WJR_INTRINSIC_INLINE static uint16_t add(__m256i v, uint16_t);
WJR_INTRINSIC_INLINE static uint32_t add(__m256i v, uint32_t);
WJR_INTRINSIC_INLINE static uint64_t add(__m256i v, uint64_t);
WJR_INTRINSIC_INLINE static __m256i adds_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i adds_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i adds_epu8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i adds_epu16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i adds(__m256i a, __m256i b, uint16_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i alignr_epi16(__m256i a, __m256i b, int c);
WJR_INTRINSIC_INLINE static __m256i alignr_epi32(__m256i a, __m256i b, int c);
WJR_INTRINSIC_INLINE static __m256i alignr_epi64(__m256i a, __m256i b, int c);
WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int16_t);
WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int32_t);
WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, int64_t);
WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint16_t);
WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint32_t);
WJR_INTRINSIC_INLINE static __m256i alignr(__m256i a, __m256i b, int c, uint64_t);
WJR_INTRINSIC_INLINE static __m256i avg_epu8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i avg_epu16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i avg(__m256i a, __m256i b, uint16_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i blend_epi16(__m256i a, __m256i b);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i blend_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i blendv_epi8(__m256i a, __m256i b, __m256i mask);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i bslli_epi128(__m256i a);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i bsrli_epi128(__m256i a);
WJR_INTRINSIC_INLINE static __m256i cmpeq_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpeq_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpeq_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpeq_epi64(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, int64_t);
WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint32_t);
WJR_INTRINSIC_INLINE static __m256i cmpeq(__m256i a, __m256i b, uint64_t);
WJR_INTRINSIC_INLINE static __m256i cmpge_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpge_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpge_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpge_epu8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpge_epu16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpge_epu32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i cmpge(__m256i a, __m256i b, uint32_t);
WJR_INTRINSIC_INLINE static __m256i cmpgt_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpgt_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpgt_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpgt_epi64(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpgt_epu8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpgt_epu16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpgt_epu32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpgt_epu64(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, int64_t);
WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint32_t);
WJR_INTRINSIC_INLINE static __m256i cmpgt(__m256i a, __m256i b, uint64_t);
WJR_INTRINSIC_INLINE static __m256i cmple_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmple_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmple_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmple_epu8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmple_epu16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmple_epu32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i cmple(__m256i a, __m256i b, uint32_t);
WJR_INTRINSIC_INLINE static __m256i cmplt_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmplt_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmplt_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmplt_epu8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmplt_epu16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmplt_epu32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i cmplt(__m256i a, __m256i b, uint32_t);
WJR_INTRINSIC_INLINE static __m256i cmpne_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpne_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpne_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i cmpne(__m256i a, __m256i b, uint32_t);
template<typename T>
WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::equal_to<>, T);
template<typename T>
WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::not_equal_to<>, T);
template<typename T>
WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater<>, T);
template<typename T>
WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::greater_equal<>, T);
template<typename T>
WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less<>, T);
template<typename T>
WJR_INTRINSIC_INLINE static __m256i cmp(__m256i a, __m256i b, std::less_equal<>, T);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract_epi8(__m256i v);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract_epi16(__m256i v);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract(__m256i v, int8_t);
template<int imm8>
WJR_INTRINSIC_INLINE static int extract(__m256i v, int16_t);
WJR_INTRINSIC_INLINE static __m256i hadd_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i hadd_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i hadd(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i hadds_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i hsub_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i hsub_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i hsub(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i hsubs_epi16(__m256i a, __m256i b);
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
WJR_INTRINSIC_INLINE static __m256i logical_and(__m256i a, __m256i b, T);
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
WJR_INTRINSIC_INLINE static __m256i logical_not(__m256i v, T);
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int> = 0>
WJR_INTRINSIC_INLINE static __m256i logical_or(__m256i a, __m256i b, T);
WJR_INTRINSIC_INLINE static __m256i madd_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i max_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i max_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i max_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i max_epu8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i max_epu16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i max_epu32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i max(__m256i a, __m256i b, uint32_t);
WJR_INTRINSIC_INLINE static int8_t max_epi8(__m256i a);
WJR_INTRINSIC_INLINE static int16_t max_epi16(__m256i a);
WJR_INTRINSIC_INLINE static int32_t max_epi32(__m256i a);
WJR_INTRINSIC_INLINE static uint8_t max_epu8(__m256i a);
WJR_INTRINSIC_INLINE static uint16_t max_epu16(__m256i a);
WJR_INTRINSIC_INLINE static uint32_t max_epu32(__m256i a);
WJR_INTRINSIC_INLINE static int8_t max(__m256i a, int8_t);
WJR_INTRINSIC_INLINE static int16_t max(__m256i a, int16_t);
WJR_INTRINSIC_INLINE static int32_t max(__m256i a, int32_t);
WJR_INTRINSIC_INLINE static uint8_t max(__m256i a, uint8_t);
WJR_INTRINSIC_INLINE static uint16_t max(__m256i a, uint16_t);
WJR_INTRINSIC_INLINE static uint32_t max(__m256i a, uint32_t);
WJR_INTRINSIC_INLINE static __m256i min_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i min_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i min_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i min_epu8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i min_epu16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i min_epu32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i min(__m256i a, __m256i b, uint32_t);
WJR_INTRINSIC_INLINE static int8_t min_epi8(__m256i a);
WJR_INTRINSIC_INLINE static int16_t min_epi16(__m256i a);
WJR_INTRINSIC_INLINE static int32_t min_epi32(__m256i a);
WJR_INTRINSIC_INLINE static uint8_t min_epu8(__m256i a);
WJR_INTRINSIC_INLINE static uint16_t min_epu16(__m256i a);
WJR_INTRINSIC_INLINE static uint32_t min_epu32(__m256i a);
WJR_INTRINSIC_INLINE static int8_t min(__m256i a, int8_t);
WJR_INTRINSIC_INLINE static int16_t min(__m256i a, int16_t);
WJR_INTRINSIC_INLINE static int32_t min(__m256i a, int32_t);
WJR_INTRINSIC_INLINE static uint8_t min(__m256i a, uint8_t);
WJR_INTRINSIC_INLINE static uint16_t min(__m256i a, uint16_t);
WJR_INTRINSIC_INLINE static uint32_t min(__m256i a, uint32_t);
WJR_INTRINSIC_INLINE static mask_type movemask_epi8(__m256i a);
WJR_INTRINSIC_INLINE static __m256i mul_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i mul_epu32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i mulhi_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i mulhi_epu16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i mullo_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i packs_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i packs_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i packus_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i packus_epi32(__m256i a, __m256i b);
template<int imm>
WJR_INTRINSIC_INLINE static __m256i shl(__m256i a);
template<int imm>
WJR_INTRINSIC_INLINE static __m256i shr(__m256i a);
WJR_INTRINSIC_INLINE static __m256i shuffle_epi8(__m256i a, __m256i b);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i shuffle_epi32(__m256i a);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i shufflehi_epi16(__m256i a);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i shufflelo_epi16(__m256i a);
WJR_INTRINSIC_INLINE static __m256i sll_epi16(__m256i a, __m128i b);
WJR_INTRINSIC_INLINE static __m256i sll_epi32(__m256i a, __m128i b);
WJR_INTRINSIC_INLINE static __m256i sll_epi64(__m256i a, __m128i b);
WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, int64_t);
WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m256i sll(__m256i a, __m128i b, uint64_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i slli(__m256i a);
WJR_INTRINSIC_INLINE static __m256i slli_epi16(__m256i a, int imm8);
WJR_INTRINSIC_INLINE static __m256i slli_epi32(__m256i a, int imm8);
WJR_INTRINSIC_INLINE static __m256i slli_epi64(__m256i a, int imm8);
WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int16_t);
WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int32_t);
WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, int64_t);
WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint16_t);
WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint32_t);
WJR_INTRINSIC_INLINE static __m256i slli(__m256i a, int imm8, uint64_t);
WJR_INTRINSIC_INLINE static __m256i sra_epi16(__m256i a, __m128i b);
WJR_INTRINSIC_INLINE static __m256i sra_epi32(__m256i a, __m128i b);
WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i sra(__m256i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i srai_epi16(__m256i a, int imm8);
WJR_INTRINSIC_INLINE static __m256i srai_epi32(__m256i a, int imm8);
WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int16_t);
WJR_INTRINSIC_INLINE static __m256i srai(__m256i a, int imm8, int32_t);
WJR_INTRINSIC_INLINE static __m256i stream_load(__m256i const* p);
WJR_INTRINSIC_INLINE static __m256i srl_epi16(__m256i a, __m128i b);
WJR_INTRINSIC_INLINE static __m256i srl_epi32(__m256i a, __m128i b);
WJR_INTRINSIC_INLINE static __m256i srl_epi64(__m256i a, __m128i b);
WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, int64_t);
WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint32_t);
WJR_INTRINSIC_INLINE static __m256i srl(__m256i a, __m128i b, uint64_t);
template<int imm8>
WJR_INTRINSIC_INLINE static __m256i srli(__m256i a);
WJR_INTRINSIC_INLINE static __m256i srli_epi16(__m256i a, int imm8);
WJR_INTRINSIC_INLINE static __m256i srli_epi32(__m256i a, int imm8);
WJR_INTRINSIC_INLINE static __m256i srli_epi64(__m256i a, int imm8);
WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int16_t);
WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int32_t);
WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, int64_t);
WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint16_t);
WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint32_t);
WJR_INTRINSIC_INLINE static __m256i srli(__m256i a, int imm8, uint64_t);
WJR_INTRINSIC_INLINE static __m256i sub_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i sub_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i sub_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i sub_epi64(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, int64_t);
WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint32_t);
WJR_INTRINSIC_INLINE static __m256i sub(__m256i a, __m256i b, uint64_t);
WJR_INTRINSIC_INLINE static __m256i subs_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i subs_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i subs_epu8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i subs_epu16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i subs(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static int test_all_ones(__m256i a);
WJR_INTRINSIC_INLINE static __m256i unpackhi_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i unpackhi_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i unpackhi_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i unpackhi_epi64(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, int64_t);
WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint32_t);
WJR_INTRINSIC_INLINE static __m256i unpackhi(__m256i a, __m256i b, uint64_t);
WJR_INTRINSIC_INLINE static __m256i unpacklo_epi8(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i unpacklo_epi16(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i unpacklo_epi32(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i unpacklo_epi64(__m256i a, __m256i b);
WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int8_t);
WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int16_t);
WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int32_t);
WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, int64_t);
WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint8_t);
WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint16_t);
WJR_INTRINSIC_INLINE static __m256i unpacklo(__m256i a, __m256i b, uint32_t);
#endif // WJR_AVX2
};
#define WJR_SIMD_WIDTH(simd_t) simd_t::width()
#define WJR_SIMD_VALUE_SIZE(ptr) sizeof(decltype(*(ptr)))
#define WJR_SIMD_OFFSET(simd_t, ptr) (WJR_SIMD_WIDTH(simd_t) / (8 * WJR_SIMD_VALUE_SIZE(ptr)))
#define WJR_SIMD_ALIGNMENT(simd_t, ptr) (WJR_SIMD_WIDTH(simd_t) / WJR_SIMD_VALUE_SIZE(ptr))
#define WJR_SIMD_UINTPTR(ptr) reinterpret_cast<uintptr_t>(ptr)
#define WJR_SIMD_SUFFIX_0 _FORWARD
#define WJR_SIMD_SUFFIX_1 _BACKWARD
#define WJR_SIMD_SUFFIX WJR_MACRO_CONCAT(WJR_SIMD_SUFFIX_, WJR_SIMD_IS_BACKWARD)
#define _WJR_SIMD_USE_SUFFIX(MACRO, SUFFIX) WJR_MACRO_CONCAT(MACRO, SUFFIX)
#define WJR_SIMD_USE_SUFFIX(MACRO) _WJR_SIMD_USE_SUFFIX(MACRO, WJR_SIMD_SUFFIX)
#define WJR_SIMD_LEFT_PTR_N_FORWARD(ptr, N) (ptr)
#define WJR_SIMD_RIGHT_PTR_N_FORWARD(ptr, N) (ptr + (N))
#define WJR_SIMD_LEFT_PTR_N_BACKWARD(ptr, N) (ptr - (N))
#define WJR_SIMD_RIGHT_PTR_N_BACKWARD(ptr, N) (ptr)
#define WJR_SIMD_LEFT_PTR_N(ptr, N) WJR_SIMD_USE_SUFFIX(WJR_SIMD_LEFT_PTR_N)(ptr, N)
#define WJR_SIMD_RIGHT_PTR_N(ptr, N) WJR_SIMD_USE_SUFFIX(WJR_SIMD_RIGHT_PTR_N)(ptr, N)
#define WJR_SIMD_PTR(ptr) WJR_SIMD_LEFT_PTR_N(ptr, 1)
#define WJR_SIMD_LEFT_PTR(simd_t, ptr) WJR_SIMD_LEFT_PTR_N(ptr, WJR_MACRO_ESC(WJR_SIMD_OFFSET(simd_t, ptr)))
#define WJR_SIMD_RIGHT_PTR(simd_t, ptr) WJR_SIMD_RIGHT_PTR_N(ptr, WJR_MACRO_ESC(WJR_SIMD_OFFSET(simd_t, ptr)))
#define WJR_SIMD_INIT_PTR_FORWARD(ptr, n)
#define WJR_SIMD_INIT_PTR_BACKWARD(ptr, n) (ptr += n)
#define WJR_SIMD_INIT_PTR(ptr, n) WJR_SIMD_USE_SUFFIX(WJR_SIMD_INIT_PTR)(ptr, n)
#define WJR_SIMD_ADD_PTR_FORWARD(ptr, n) (ptr + (n))
#define WJR_SIMD_SUB_PTR_FORWARD(ptr, n) (ptr - (n))
#define WJR_SIMD_ADD_PTR_BACKWARD(ptr, n) WJR_SIMD_SUB_PTR_FORWARD(ptr, n)
#define WJR_SIMD_SUB_PTR_BACKWARD(ptr, n) WJR_SIMD_ADD_PTR_FORWARD(ptr, n)
#define WJR_SIMD_ADD_PTR(ptr, n) WJR_SIMD_USE_SUFFIX(WJR_SIMD_ADD_PTR)(ptr, n)
#define WJR_SIMD_SUB_PTR(ptr, n) WJR_SIMD_USE_SUFFIX(WJR_SIMD_SUB_PTR)(ptr, n)
#define WJR_SIMD_INC_PTR_FORWARD(ptr, n)	(ptr += n)
#define WJR_SIMD_DEC_PTR_FORWARD(ptr, n)	(ptr -= n)
#define WJR_SIMD_INC_PTR_BACKWARD(ptr, n)	WJR_SIMD_DEC_PTR_FORWARD(ptr, n)
#define WJR_SIMD_DEC_PTR_BACKWARD(ptr, n)	WJR_SIMD_INC_PTR_FORWARD(ptr, n)
#define WJR_SIMD_INC_PTR(ptr, n) WJR_SIMD_USE_SUFFIX(WJR_SIMD_INC_PTR)(ptr, n)
#define WJR_SIMD_DEC_PTR(ptr, n) WJR_SIMD_USE_SUFFIX(WJR_SIMD_DEC_PTR)(ptr, n)
#define WJR_SIMD_CHECK_ALIGN(W) static_assert(wjr::has_single_bit(W), "invalid align");
#define WJR_SIMD_FLOOR_ALIGN_OFFSET_FORWARD(ptr, W) (WJR_SIMD_UINTPTR(ptr) & ((W) - 1))
#define WJR_SIMD_CEIL_ALIGN_OFFSET_FORWARD(ptr, W) (static_cast<uintptr_t>(0 - WJR_SIMD_UINTPTR(ptr)) & ((W) - 1))
#define WJR_SIMD_FLOOR_ALIGN_OFFSET_BACKWARD(ptr, W) WJR_SIMD_CEIL_ALIGN_OFFSET_FORWARD(ptr, W)
#define WJR_SIMD_CEIL_ALIGN_OFFSET_BACKWARD(ptr, W) WJR_SIMD_FLOOR_ALIGN_OFFSET_FORWARD(ptr, W)
#define WJR_SIMD_FLOOR_ALIGN_OFFSET(ptr, W) WJR_SIMD_USE_SUFFIX(WJR_SIMD_FLOOR_ALIGN_OFFSET)(ptr, W)
#define WJR_SIMD_CEIL_ALIGN_OFFSET(ptr, W) WJR_SIMD_USE_SUFFIX(WJR_SIMD_CEIL_ALIGN_OFFSET)(ptr, W)
#define WJR_SIMD_FLOOR_ALIGN_PTR_FORWARD(ptr, W) reinterpret_cast<decltype(ptr)>(WJR_SIMD_UINTPTR(ptr) & (~((W) - 1)))
#define WJR_SIMD_CEIL_ALIGN_PTR_FORWARD(ptr, W) reinterpret_cast<decltype(ptr)>((WJR_SIMD_UINTPTR(ptr) + (W) - 1) & (~((W) - 1)))
#define WJR_SIMD_FLOOR_ALIGN_PTR_BACKWARD(ptr, W) WJR_SIMD_CEIL_ALIGN_PTR_FORWARD(ptr, W)
#define WJR_SIMD_CEIL_ALIGN_PTR_BACKWARD(ptr, W) WJR_SIMD_FLOOR_ALIGN_PTR_FORWARD(ptr, W)
#define WJR_SIMD_FLOOR_ALIGN_PTR(ptr, W) WJR_SIMD_USE_SUFFIX(WJR_SIMD_FLOOR_ALIGN_PTR)(ptr, W)
#define WJR_SIMD_CEIL_ALIGN_PTR(ptr, W) WJR_SIMD_USE_SUFFIX(WJR_SIMD_CEIL_ALIGN_PTR)(ptr, W)
#define WJR_SIMD_FIRST_ZERO_FORWARD(x) wjr::countr_one(x)
#define WJR_SIMD_LAST_ZERO_FORWARD(x) wjr::countl_one(x)
#define WJR_SIMD_FIRST_ONE_FORWARD(x) wjr::countr_zero(x)
#define WJR_SIMD_LAST_ONE_FORWARD(x) wjr::countl_zero(x)
#define WJR_SIMD_FIRST_ZERO_BACKWARD(x) WJR_SIMD_LAST_ZERO_FORWARD(x)
#define WJR_SIMD_LAST_ZERO_BACKWARD(x) WJR_SIMD_FIRST_ZERO_FORWARD(x)
#define WJR_SIMD_FIRST_ONE_BACKWARD(x) WJR_SIMD_LAST_ONE_FORWARD(x)
#define WJR_SIMD_LAST_ONE_BACKWARD(x) WJR_SIMD_FIRST_ONE_FORWARD(x)
#define WJR_SIMD_FIRST_ZERO(x) WJR_SIMD_USE_SUFFIX(WJR_SIMD_FIRST_ZERO)(x)
#define WJR_SIMD_LAST_ZERO(x) WJR_SIMD_USE_SUFFIX(WJR_SIMD_LAST_ZERO)(x)
#define WJR_SIMD_FIRST_ONE(x) WJR_SIMD_USE_SUFFIX(WJR_SIMD_FIRST_ONE)(x)
#define WJR_SIMD_LAST_ONE(x) WJR_SIMD_USE_SUFFIX(WJR_SIMD_LAST_ONE)(x)
#define WJR_SIMD_FIRST_ZERO_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_FIRST_ZERO(x) / WJR_SIMD_VALUE_SIZE(ptr))
#define WJR_SIMD_LAST_ZERO_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_LAST_ZERO(x) / WJR_SIMD_VALUE_SIZE(ptr))
#define WJR_SIMD_FIRST_ONE_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_FIRST_ONE(x) / WJR_SIMD_VALUE_SIZE(ptr))
#define WJR_SIMD_LAST_ONE_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_LAST_ONE(x) / WJR_SIMD_VALUE_SIZE(ptr))
#define _WJR_SIMD_LOAD(simd_t, PREFIX, ptr, LOAD, J)	                                                                        \
auto PREFIX = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(WJR_MACRO_ESC(J(simd_t, ptr))))
#define _WJR_SIMD_LOAD2(simd_t, PREFIX, ptr0, ptr1, LOAD, J)	                                                                \
auto PREFIX##0 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(WJR_MACRO_ESC(J(simd_t, ptr0))));	        \
auto PREFIX##1 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(WJR_MACRO_ESC(J(simd_t, ptr1))))
#define _WJR_SIMD_LOAD4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3 , LOAD, J)	                                                    \
auto PREFIX##0 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(WJR_MACRO_ESC(J(simd_t, ptr0))));	        \
auto PREFIX##1 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(WJR_MACRO_ESC(J(simd_t, ptr1))));	        \
auto PREFIX##2 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(WJR_MACRO_ESC(J(simd_t, ptr2))));	        \
auto PREFIX##3 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(WJR_MACRO_ESC(J(simd_t, ptr3))))
#define WJR_SIMD_LOADU(simd_t, PREFIX, ptr) _WJR_SIMD_LOAD(simd_t, PREFIX, ptr, loadu, WJR_SIMD_LEFT_PTR)
#define WJR_SIMD_LOADU2(simd_t, PREFIX, ptr0, ptr1)             \
_WJR_SIMD_LOAD2(simd_t, PREFIX, ptr0, ptr1, loadu, WJR_SIMD_LEFT_PTR)
#define WJR_SIMD_LOADU4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3) \
_WJR_SIMD_LOAD4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3, loadu, WJR_SIMD_LEFT_PTR)
#define WJR_SIMD_LOAD(simd_t, PREFIX, ptr)	_WJR_SIMD_LOAD(simd_t, PREFIX, ptr, load, WJR_SIMD_LEFT_PTR)
#define WJR_SIMD_LOAD2(simd_t, PREFIX, ptr0, ptr1)              \
_WJR_SIMD_LOAD2(simd_t, PREFIX, ptr0, ptr1, load, WJR_SIMD_LEFT_PTR)
#define WJR_SIMD_LOAD4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3)  \
_WJR_SIMD_LOAD4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3, load, WJR_SIMD_LEFT_PTR)
_WJR_SIMD_END
#ifndef __WJR_SIMD_SIMD_INTRIN_H
#error "This file should not be included directly. Include <wjr/simd/simd_intrin.h> instead."
#endif
_WJR_BEGIN
#if WJR_SSE2
template<>
struct broadcast_fn<simd::__m128i_tag, uint8_t> {
WJR_INTRINSIC_INLINE __m128i operator()(uint8_t v) const {
return _mm_set1_epi8(v);
}
};
template<>
struct broadcast_fn<simd::__m128i_tag, uint16_t> {
WJR_INTRINSIC_INLINE __m128i operator()(uint16_t v) const {
return _mm_set1_epi16(v);
}
};
template<>
struct broadcast_fn<simd::__m128i_tag, uint32_t> {
WJR_INTRINSIC_INLINE __m128i operator()(uint32_t v) const {
return _mm_set1_epi32(v);
}
};
template<>
struct broadcast_fn<simd::__m128i_tag, uint64_t> {
WJR_INTRINSIC_INLINE __m128i operator()(uint64_t v) const {
return _mm_set1_epi64x(v);
}
};
template<>
struct broadcast_fn<simd::__m128i_tag, simd::__m128i_tag> {
WJR_INTRINSIC_INLINE __m128i operator()(__m128i v) const {
return v;
}
};
#endif // WJR_SSE2
#if WJR_AVX
template<>
struct broadcast_fn<simd::__m256i_tag, uint8_t> {
WJR_INTRINSIC_INLINE __m256i operator()(uint8_t v) const {
return _mm256_set1_epi8(v);
}
};
template<>
struct broadcast_fn<simd::__m256i_tag, uint16_t> {
WJR_INTRINSIC_INLINE __m256i operator()(uint16_t v) const {
return _mm256_set1_epi16(v);
}
};
template<>
struct broadcast_fn<simd::__m256i_tag, uint32_t> {
WJR_INTRINSIC_INLINE __m256i operator()(uint32_t v) const {
return _mm256_set1_epi32(v);
}
};
template<>
struct broadcast_fn<simd::__m256i_tag, uint64_t> {
WJR_INTRINSIC_INLINE __m256i operator()(uint64_t v) const {
return _mm256_set1_epi64x(v);
}
};
template<>
struct broadcast_fn<simd::__m256i_tag, simd::__m256i_tag> {
WJR_INTRINSIC_INLINE __m256i operator()(__m256i v) const {
return v;
}
};
template<>
struct broadcast_fn<simd::__m256i_tag, simd::__m128i_tag> {
WJR_INTRINSIC_INLINE __m256i operator()(__m128i v) const {
#if WJR_AVX2
return _mm256_broadcastsi128_si256(v);
#else
return _mm256_insertf128_si256(_mm256_castsi128_si256(v), v, 1);
#endif // WJR_AVX2
}
};
#endif // WJR_AVX
_WJR_END
_WJR_SIMD_BEGIN
#if WJR_SSE2
__m128i mm_loadu_si16(void const* p) {
return simd_cast<__m128i_tag, uint16_t>(*reinterpret_cast<uint16_t const*>(p));
}
__m128i mm_loadu_si32(void const* p) {
return simd_cast<__m128i_tag, uint32_t>(*reinterpret_cast<uint32_t const*>(p));
}
__m128i mm_loadu_si64(void const* p) {
return simd_cast<__m128i_tag, uint64_t>(*reinterpret_cast<uint64_t const*>(p));
}
#endif // WJR_SSE2
/*------SSE------*/
constexpr size_t sse::width() { return 128; }
constexpr sse::mask_type sse::mask() { return 0xFFFF; }
#if WJR_SSE
sse::mask_type sse::movemask_ps(__m128 v) {
return static_cast<sse::mask_type>(_mm_movemask_ps(v));
}
void sse::sfence() { return _mm_sfence(); }
template<int imm8>
__m128 sse::shuffle_ps(__m128 a, __m128 b) {
static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
return _mm_shuffle_ps(a, b, imm8);
}
#endif // WJR_SSE
#if WJR_SSE2
__m128i sse::add_epi8(__m128i a, __m128i b) { return _mm_add_epi8(a, b); }
__m128i sse::add_epi16(__m128i a, __m128i b) { return _mm_add_epi16(a, b); }
__m128i sse::add_epi32(__m128i a, __m128i b) { return _mm_add_epi32(a, b); }
__m128i sse::add_epi64(__m128i a, __m128i b) { return _mm_add_epi64(a, b); }
__m128i sse::add(__m128i a, __m128i b, int8_t) { return add_epi8(a, b); }
__m128i sse::add(__m128i a, __m128i b, int16_t) { return add_epi16(a, b); }
__m128i sse::add(__m128i a, __m128i b, int32_t) { return add_epi32(a, b); }
__m128i sse::add(__m128i a, __m128i b, int64_t) { return add_epi64(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint8_t) { return add_epi8(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint16_t) { return add_epi16(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint32_t) { return add_epi32(a, b); }
__m128i sse::add(__m128i a, __m128i b, uint64_t) { return add_epi64(a, b); }
int8_t sse::add_epi8(__m128i a) { return static_cast<int8_t>(add_epu8(a)); }
int16_t sse::add_epi16(__m128i a) { return static_cast<int16_t>(add_epu16(a)); }
int32_t sse::add_epi32(__m128i a) { return static_cast<int32_t>(add_epu32(a)); }
int64_t sse::add_epi64(__m128i a) { return static_cast<int64_t>(add_epu64(a)); }
uint8_t sse::add_epu8(__m128i a) {
auto b = shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a);
a = add(a, b, uint8_t());
b = zeros();
a = sad_epu8(a, b);
return simd_cast<uint8_t, __m128i_tag>(a);
}
uint16_t sse::add_epu16(__m128i a) {
a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint16_t());
a = add(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a), uint16_t());
a = add(a, srli<2>(a), uint16_t());
return simd_cast<uint16_t, __m128i_tag>(a);
}
uint32_t sse::add_epu32(__m128i a) {
a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint32_t());
a = add(a, shuffle_epi32<_MM_SHUFFLE(1, 1, 1, 1)>(a), uint32_t());
return simd_cast<uint32_t, __m128i_tag>(a);
}
uint64_t sse::add_epu64(__m128i a) {
a = add(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a), uint64_t());
return simd_cast<uint64_t, __m128i_tag>(a);
}
int8_t sse::add(__m128i a, int8_t) { return add_epi8(a); }
int16_t sse::add(__m128i a, int16_t) { return add_epi16(a); }
int32_t sse::add(__m128i a, int32_t) { return add_epi32(a); }
int64_t sse::add(__m128i a, int64_t) { return add_epi64(a); }
uint8_t sse::add(__m128i a, uint8_t) { return add_epu8(a); }
uint16_t sse::add(__m128i a, uint16_t) { return add_epu16(a); }
uint32_t sse::add(__m128i a, uint32_t) { return add_epu32(a); }
uint64_t sse::add(__m128i a, uint64_t) { return add_epu64(a); }
__m128i sse::adds_epi8(__m128i a, __m128i b) { return _mm_adds_epi8(a, b); }
__m128i sse::adds_epi16(__m128i a, __m128i b) { return _mm_adds_epi16(a, b); }
__m128i sse::adds_epu8(__m128i a, __m128i b) { return _mm_adds_epu8(a, b); }
__m128i sse::adds_epu16(__m128i a, __m128i b) { return _mm_adds_epu16(a, b); }
__m128i sse::adds(__m128i a, __m128i b, int8_t) { return adds_epi8(a, b); }
__m128i sse::adds(__m128i a, __m128i b, int16_t) { return adds_epi16(a, b); }
__m128i sse::adds(__m128i a, __m128i b, uint8_t) { return adds_epu8(a, b); }
__m128i sse::adds(__m128i a, __m128i b, uint16_t) { return adds_epu16(a, b); }
template<int imm8>
__m128i sse::alignr(__m128i a, __m128i b) {
constexpr int s = imm8 & 0x1F;
#if WJR_SSSE3
return _mm_alignr_epi8(a, b, s);
#else
if constexpr (s == 0) {
return b;
}
if constexpr (s == 16) {
return a;
}
if constexpr (s < 16) {
return Or(slli<16 - s>(a), srli<s>(b));
}
return srli<s - 16>(a);
#endif // WJR_SSSE3
}
__m128i sse::alignr_epi16(__m128i a, __m128i b, int c) {
return Or(slli(a, 16 - c, uint16_t()), srli(b, c, uint16_t()));
}
__m128i sse::alignr_epi32(__m128i a, __m128i b, int c) {
return Or(slli(a, 32 - c, uint32_t()), srli(b, c, uint32_t()));
}
__m128i sse::alignr_epi64(__m128i a, __m128i b, int c) {
return Or(slli(a, 64 - c, uint64_t()), srli(b, c, uint64_t()));
}
__m128i sse::alignr(__m128i a, __m128i b, int c, int16_t) { return alignr_epi16(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, int32_t) { return alignr_epi32(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, int64_t) { return alignr_epi64(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, uint16_t) { return alignr_epi16(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, uint32_t) { return alignr_epi32(a, b, c); }
__m128i sse::alignr(__m128i a, __m128i b, int c, uint64_t) { return alignr_epi64(a, b, c); }
__m128i sse::And(__m128i a, __m128i b) { return _mm_and_si128(a, b); }
__m128i sse::AndNot(__m128i a, __m128i b) { return _mm_andnot_si128(a, b); }
__m128i sse::avg_epu8(__m128i a, __m128i b) { return _mm_avg_epu8(a, b); }
__m128i sse::avg_epu16(__m128i a, __m128i b) { return _mm_avg_epu16(a, b); }
__m128i sse::avg(__m128i a, __m128i b, int8_t) { return avg_epu8(a, b); }
__m128i sse::avg(__m128i a, __m128i b, int16_t) { return avg_epu16(a, b); }
__m128i sse::avg(__m128i a, __m128i b, uint8_t) { return avg_epu8(a, b); }
__m128i sse::avg(__m128i a, __m128i b, uint16_t) { return avg_epu16(a, b); }
__m128i sse::blendv_epi8(__m128i a, __m128i b, __m128i mask) {
#if WJR_SSE4_1
return _mm_blendv_epi8(a, b, mask);
#elif defined(WJR_COMPILER_GCC)
return ((~mask) & a) | (mask & b);
#else
return Or(AndNot(mask, a), And(mask, b));
#endif // WJR_SSE4_1
}
__m128i sse::blendv_epi16(__m128i a, __m128i b, __m128i mask) {
return blendv_epi8(b, a, logical_not(mask, uint16_t()));
}
__m128i sse::blendv_epi32(__m128i a, __m128i b, __m128i mask) {
return blendv_epi8(b, a, logical_not(mask, uint32_t()));
}
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int8_t) { return blendv_epi8(a, b, mask); }
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int16_t) { return blendv_epi16(a, b, mask); }
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, int32_t) { return blendv_epi32(a, b, mask); }
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint8_t) { return blendv_epi8(a, b, mask); }
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint16_t) { return blendv_epi16(a, b, mask); }
__m128i sse::blendv(__m128i a, __m128i b, __m128i mask, uint32_t) { return blendv_epi32(a, b, mask); }
template<int imm8>
__m128i sse::bslli(__m128i val) { return _mm_bslli_si128(val, imm8); }
template<int imm8>
__m128i sse::bsrli(__m128i val) { return _mm_bsrli_si128(val, imm8); }
__m128i sse::cmpeq_epi8(__m128i a, __m128i b) { return _mm_cmpeq_epi8(a, b); }
__m128i sse::cmpeq_epi16(__m128i a, __m128i b) { return _mm_cmpeq_epi16(a, b); }
__m128i sse::cmpeq_epi32(__m128i a, __m128i b) { return _mm_cmpeq_epi32(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, int8_t) { return cmpeq_epi8(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, int16_t) { return cmpeq_epi16(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, int32_t) { return cmpeq_epi32(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint8_t) { return cmpeq_epi8(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint16_t) { return cmpeq_epi16(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint32_t) { return cmpeq_epi32(a, b); }
__m128i sse::cmpge_epi8(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comge_epi8(a, b);
#elif WJR_SSE4_1
return cmpeq(min(a, b, int8_t()), b, uint8_t());
#else
return Not(cmpgt(b, a, int8_t()));
#endif // WJR_XOP
}
__m128i sse::cmpge_epi16(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comge_epi16(a, b);
#else
return cmpeq(min(a, b, int16_t()), b, uint16_t());
#endif // WJR_XOP
}
__m128i sse::cmpge_epi32(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comge_epi32(a, b);
#elif WJR_SSE4_1
return cmpeq(min(a, b, int32_t()), b, uint32_t());
#else
return Not(cmpgt(b, a, int32_t()));
#endif // WJR_XOP
}
__m128i sse::cmpge_epu8(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comge_epu8(a, b);
#else
return cmpeq(min(a, b, uint8_t()), b, uint8_t());
#endif // WJR_XOP
}
__m128i sse::cmpge_epu16(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comge_epu16(a, b);
#elif WJR_SSE4_1
return cmpeq(min(a, b, uint16_t()), b, uint16_t());
#else
return logical_not(subs(b, a, uint16_t()), uint16_t());
#endif // WJR_XOP
}
__m128i sse::cmpge_epu32(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comge_epu32(a, b);
#elif WJR_SSE4_1
return cmpeq(min(a, b, uint32_t()), b, uint32_t());
#else
return Not(cmpgt(b, a, uint32_t()));
#endif // WJR_XOP
}
__m128i sse::cmpge(__m128i a, __m128i b, int8_t) { return cmpge_epi8(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, int16_t) { return cmpge_epi16(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, int32_t) { return cmpge_epi32(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint8_t) { return cmpge_epu8(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint16_t) { return cmpge_epu16(a, b); }
__m128i sse::cmpge(__m128i a, __m128i b, uint32_t) { return cmpge_epu32(a, b); }
__m128i sse::cmpgt_epi8(__m128i a, __m128i b) { return _mm_cmpgt_epi8(a, b); }
__m128i sse::cmpgt_epi16(__m128i a, __m128i b) { return _mm_cmpgt_epi16(a, b); }
__m128i sse::cmpgt_epi32(__m128i a, __m128i b) { return _mm_cmpgt_epi32(a, b); }
__m128i sse::cmpgt_epu8(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comgt_epu8(a, b);
#else
return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
#endif // WJR_XOP
}
__m128i sse::cmpgt_epu16(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comgt_epu16(a, b);
#else
return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
#endif // WJR_XOP
}
__m128i sse::cmpgt_epu32(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comgt_epu32(a, b);
#else
return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
#endif // WJR_XOP
}
__m128i sse::cmpgt(__m128i a, __m128i b, int8_t) { return cmpgt_epi8(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, int16_t) { return cmpgt_epi16(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, int32_t) { return cmpgt_epi32(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint8_t) { return cmpgt_epu8(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint16_t) { return cmpgt_epu16(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, uint32_t) { return cmpgt_epu32(a, b); }
__m128i sse::cmple_epi8(__m128i a, __m128i b) { return cmpge_epi8(b, a); }
__m128i sse::cmple_epi16(__m128i a, __m128i b) { return cmpge_epi16(b, a); }
__m128i sse::cmple_epi32(__m128i a, __m128i b) { return cmpge_epi32(b, a); }
__m128i sse::cmple_epu8(__m128i a, __m128i b) { return cmpge_epu8(b, a); }
__m128i sse::cmple_epu16(__m128i a, __m128i b) { return cmpge_epu16(b, a); }
__m128i sse::cmple_epu32(__m128i a, __m128i b) { return cmpge_epu32(b, a); }
__m128i sse::cmple(__m128i a, __m128i b, int8_t) { return cmple_epi8(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, int16_t) { return cmple_epi16(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, int32_t) { return cmple_epi32(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint8_t) { return cmple_epu8(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint16_t) { return cmple_epu16(a, b); }
__m128i sse::cmple(__m128i a, __m128i b, uint32_t) { return cmple_epu32(a, b); }
__m128i sse::cmplt_epi8(__m128i a, __m128i b) { return _mm_cmplt_epi8(a, b); }
__m128i sse::cmplt_epi16(__m128i a, __m128i b) { return _mm_cmplt_epi16(a, b); }
__m128i sse::cmplt_epi32(__m128i a, __m128i b) { return _mm_cmplt_epi32(a, b); }
__m128i sse::cmplt_epu8(__m128i a, __m128i b) { return cmpgt_epu8(b, a); }
__m128i sse::cmplt_epu16(__m128i a, __m128i b) { return cmpgt_epu16(b, a); }
__m128i sse::cmplt_epu32(__m128i a, __m128i b) { return cmpgt_epu32(b, a); }
__m128i sse::cmplt(__m128i a, __m128i b, int8_t) { return cmplt_epi8(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, int16_t) { return cmplt_epi16(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, int32_t) { return cmplt_epi32(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint8_t) { return cmplt_epu8(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint16_t) { return cmplt_epu16(a, b); }
__m128i sse::cmplt(__m128i a, __m128i b, uint32_t) { return cmplt_epu32(a, b); }
__m128i sse::cmpne_epi8(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comneq_epi8(a, b);
#else
return Not(cmpeq_epi8(a, b));
#endif
}
__m128i sse::cmpne_epi16(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comneq_epi16(a, b);
#else
return Not(cmpeq_epi16(a, b));
#endif
}
__m128i sse::cmpne_epi32(__m128i a, __m128i b) {
#if WJR_XOP
return _mm_comneq_epi32(a, b);
#else
return Not(cmpeq_epi32(a, b));
#endif
}
__m128i sse::cmpne(__m128i a, __m128i b, int8_t) { return cmpne_epi8(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, int16_t) { return cmpne_epi16(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, int32_t) { return cmpne_epi32(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint8_t) { return cmpne_epi8(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint16_t) { return cmpne_epi16(a, b); }
__m128i sse::cmpne(__m128i a, __m128i b, uint32_t) { return cmpne_epi32(a, b); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::equal_to<>, T) { return cmpeq(a, b, T()); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::not_equal_to<>, T) { return cmpne(a, b, T()); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::greater<>, T) { return cmpgt(a, b, T()); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::greater_equal<>, T) { return cmpge(a, b, T()); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::less<>, T) { return cmplt(a, b, T()); }
template<typename T>
__m128i sse::cmp(__m128i a, __m128i b, std::less_equal<>, T) { return cmple(a, b, T()); }
__m128i sse::concat(uint64_t lo, uint64_t hi) { return set_epi64x(hi, lo); }
template<int imm8>
int sse::extract_epi8(__m128i a) {
static_assert(imm8 >= 0 && imm8 < 16, "imm8 must be in range [0, 15]");
#if WJR_SSE4_1
return _mm_extract_epi8(a, imm8);
#else
if constexpr (imm8 & 1) {
return extract_epi16<(imm8 >> 1)>(a) >> 8;
}
else {
return extract_epi16 <(imm8 >> 1)>(a) & 0xff;
}
#endif // WJR_SSE4_1
}
template<int imm8>
int sse::extract_epi16(__m128i a) {
static_assert(imm8 >= 0 && imm8 < 8, "imm8 must be in range [0, 7]");
return _mm_extract_epi16(a, imm8);
}
template<int imm8>
int sse::extract_epi32(__m128i a) {
static_assert(imm8 >= 0 && imm8 < 4, "imm8 must be in range [0, 3]");
#if WJR_SSE4_1
return _mm_extract_epi32(a, imm8);
#else
if constexpr (imm8 == 0) {
return simd_cast<uint32_t, __m128i_tag>(a);
}
else if constexpr (imm8 == 1) {
return static_cast<uint32_t>(simd_cast<uint64_t, __m128i_tag>(a) >> 32);
}
else if constexpr (imm8 == 2) {
return simd_cast<uint32_t, __m128i_tag>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
}
else {
return simd_cast<uint32_t, __m128i_tag>(shuffle_epi32<_MM_SHUFFLE(3, 3, 3, 3)>(a));
}
#endif // WJR_SSE4_1
}
template<int imm8>
int64_t sse::extract_epi64(__m128i a) {
static_assert(imm8 >= 0 && imm8 < 2, "imm8 must be in range [0, 1]");
#if WJR_SSE4_1
return _mm_extract_epi64(a, imm8);
#else
if constexpr (imm8 == 0) {
return simd_cast<uint64_t, __m128i_tag>(a);
}
else {
return simd_cast<uint64_t, __m128i_tag>(shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
}
#endif // WJR_SSE4_1
}
template<int imm8>
int sse::extract(__m128i a, int8_t) { return extract_epi8<imm8>(a); }
template<int imm8>
int sse::extract(__m128i a, int16_t) { return extract_epi16<imm8>(a); }
template<int imm8>
int sse::extract(__m128i a, int32_t) { return extract_epi32<imm8>(a); }
template<int imm8>
int64_t sse::extract(__m128i a, int64_t) { return extract_epi64<imm8>(a); }
template<int imm8>
int sse::extract(__m128i a, uint8_t) { return extract_epi8<imm8>(a); }
template<int imm8>
int sse::extract(__m128i a, uint16_t) { return extract_epi16<imm8>(a); }
template<int imm8>
int sse::extract(__m128i a, uint32_t) { return extract_epi32<imm8>(a); }
template<int imm8>
int64_t sse::extract(__m128i a, uint64_t) { return extract_epi64<imm8>(a); }
uint64_t sse::getlow(__m128i v) { return simd_cast<uint64_t, __m128i_tag>(v); }
uint64_t sse::gethigh(__m128i v) { return extract_epi64<1>(v); }
template<int imm8>
__m128i sse::insert_epi16(__m128i a, int i) {
return _mm_insert_epi16(a, i, imm8);
}
template<int imm8>
__m128i sse::insert(__m128i a, int i, int16_t) { return insert_epi16<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int i, uint16_t) { return insert_epi16<imm8>(a, i); }
void sse::lfence() { _mm_lfence(); }
__m128i sse::load(const __m128i* ptr) { return _mm_load_si128(ptr); }
__m128i sse::loadu(const __m128i* ptr) { return _mm_loadu_si128(ptr); }
__m128i sse::loadu_si16(const void* ptr) { return simd::mm_loadu_si16(ptr); }
__m128i sse::loadu_si32(const void* ptr) { return simd::mm_loadu_si32(ptr); }
__m128i sse::loadu_si64(const void* ptr) { return simd::mm_loadu_si64(ptr); }
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int>>
__m128i sse::logical_and(__m128i a, __m128i b, T) {
return Not(Or(logical_not(a, T()), logical_not(b, T())));
}
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int>>
__m128i sse::logical_not(__m128i v, T) {
auto Zero = zeros();
return cmpeq(v, Zero, T());
}
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int>>
__m128i sse::logical_or(__m128i a, __m128i b, T) {
return Not(logical_not(Or(a, b), T()));
}
__m128i sse::madd_epi16(__m128i a, __m128i b) {
return _mm_madd_epi16(a, b);
}
void sse::maskmoveu(__m128i a, __m128i mask, char* mem_addr) {
return _mm_maskmoveu_si128(a, mask, mem_addr);
}
__m128i sse::max_epi8(__m128i a, __m128i b) {
#if WJR_SSE4_1
return _mm_max_epi8(a, b);
#else
return blendv_epi8(b, a, cmpgt_epi8(a, b));
#endif // WJR_SSE4_1
}
__m128i sse::max_epi16(__m128i a, __m128i b) { return _mm_max_epi16(a, b); }
__m128i sse::max_epi32(__m128i a, __m128i b) {
#if WJR_SSE4_1
return _mm_max_epi32(a, b);
#else
return blendv_epi8(b, a, cmpgt_epi32(a, b));
#endif // WJR_SSE4_1
}
__m128i sse::max_epu8(__m128i a, __m128i b) { return _mm_max_epu8(a, b); }
__m128i sse::max_epu16(__m128i a, __m128i b) {
#if WJR_SSE4_1
return _mm_max_epu16(a, b);
#else
return add(subs_epu16(b, a), a, uint16_t());
#endif // WJR_SSE4_1
}
__m128i sse::max_epu32(__m128i a, __m128i b) {
#if WJR_SSE4_1
return _mm_max_epu32(a, b);
#else
return blendv_epi8(b, a, cmpgt_epu32(a, b));
#endif // WJR_SSE4_1
}
__m128i sse::max(__m128i a, __m128i b, int8_t) { return max_epi8(a, b); }
__m128i sse::max(__m128i a, __m128i b, int16_t) { return max_epi16(a, b); }
__m128i sse::max(__m128i a, __m128i b, int32_t) { return max_epi32(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint8_t) { return max_epu8(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint16_t) { return max_epu16(a, b); }
__m128i sse::max(__m128i a, __m128i b, uint32_t) { return max_epu32(a, b); }
int8_t sse::max_epi8(__m128i a) {
return 0x7fu ^ min_epu8(Xor(a, set1_epi8(0x7fu)));
}
int16_t sse::max_epi16(__m128i a) {
#if WJR_SSE4_1
return 0x7fffu ^ min_epu16(Xor(a, set1_epi16(0x7fffu)));
#else
a = max_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
a = max_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
return simd_cast<int16_t, __m128i_tag>(a);
#endif // WJR_SSE4_1
}
int32_t sse::max_epi32(__m128i a) {
a = max_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
a = max_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
return simd_cast<int32_t, __m128i_tag>(a);
}
uint8_t sse::max_epu8(__m128i a) {
#if WJR_SSE4_1
return 0xffu ^ min_epu8(Xor(a, ones()));
#else
a = max_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
a = max_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
auto X = simd_cast<uint32_t, __m128i_tag>(a);
return std::max((uint8_t)X, (uint8_t)(X >> 8));
#endif // WJR_SSE4_1
}
uint16_t sse::max_epu16(__m128i a) {
#if WJR_SSE4_1
return 0xffffu ^ min_epu16(Xor(a, ones()));
#else
a = max_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
a = max_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
return simd_cast<uint16_t, __m128i_tag>(a);
#endif // WJR_SSE4_1
}
uint32_t sse::max_epu32(__m128i a) {
a = max_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
a = max_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
return simd_cast<uint32_t, __m128i_tag>(a);
}
int8_t sse::max(__m128i a, int8_t) { return max_epi8(a); }
int16_t sse::max(__m128i a, int16_t) { return max_epi16(a); }
int32_t sse::max(__m128i a, int32_t) { return max_epi32(a); }
uint8_t sse::max(__m128i a, uint8_t) { return max_epu8(a); }
uint16_t sse::max(__m128i a, uint16_t) { return max_epu16(a); }
uint32_t sse::max(__m128i a, uint32_t) { return max_epu32(a); }
void sse::mfence() { _mm_mfence(); }
__m128i sse::min_epi8(__m128i a, __m128i b) {
#if WJR_SSE4_1
return _mm_min_epi8(a, b);
#else
return blendv_epi8(a, b, cmpgt_epi8(a, b));
#endif // WJR_SSE4_1
}
__m128i sse::min_epi16(__m128i a, __m128i b) { return _mm_min_epi16(a, b); }
__m128i sse::min_epi32(__m128i a, __m128i b) {
#if WJR_SSE4_1
return _mm_min_epi32(a, b);
#else
return blendv_epi8(a, b, cmpgt_epi32(a, b));
#endif // WJR_SSE4_1
}
__m128i sse::min_epu8(__m128i a, __m128i b) { return _mm_min_epu8(a, b); }
__m128i sse::min_epu16(__m128i a, __m128i b) {
#if WJR_SSE4_1
return _mm_min_epu16(a, b);
#else
return blendv_epi8(a, b, cmpgt_epu16(a, b));
#endif // WJR_SSE4_1
}
__m128i sse::min_epu32(__m128i a, __m128i b) {
#if WJR_SSE4_1
return _mm_min_epu32(a, b);
#else
return blendv_epi8(a, b, cmpgt_epu32(a, b));
#endif // WJR_SSE4_1
}
__m128i sse::min(__m128i a, __m128i b, int8_t) { return min_epi8(a, b); }
__m128i sse::min(__m128i a, __m128i b, int16_t) { return min_epi16(a, b); }
__m128i sse::min(__m128i a, __m128i b, int32_t) { return min_epi32(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint8_t) { return min_epu8(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint16_t) { return min_epu16(a, b); }
__m128i sse::min(__m128i a, __m128i b, uint32_t) { return min_epu32(a, b); }
int8_t sse::min_epi8(__m128i a) {
return 0x80u ^ min_epu8(Xor(a, setmin_epi8()));
}
int16_t sse::min_epi16(__m128i a) {
#if WJR_SSE4_1
return 0x8000u ^ min_epu16(Xor(a, setmin_epi16()));
#else
a = min_epi16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
a = min_epi16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
return simd_cast<int16_t, __m128i_tag>(a);
#endif // WJR_SSE4_1
}
int32_t sse::min_epi32(__m128i a) {
a = min_epi32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
a = min_epi32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
return simd_cast<int32_t, __m128i_tag>(a);
}
uint8_t sse::min_epu8(__m128i a) {
#if WJR_SSE4_1
a = min_epu8(a, srli_epi16(a, 8));
a = _mm_minpos_epu16(a);
return simd_cast<uint8_t, __m128i_tag>(a);
#else
a = min_epu8(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
a = min_epu8(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
auto X = simd_cast<uint32_t, __m128i_tag>(a);
return std::min((uint8_t)X, (uint8_t)(X >> 8));
#endif // WJR_SSE4_1
}
uint16_t sse::min_epu16(__m128i a) {
#if WJR_SSE4_1
return simd_cast<uint16_t, __m128i_tag>(_mm_minpos_epu16(a));
#else
a = min_epu16(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
a = min_epu16(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 1, 0)>(a));
return simd_cast<uint16_t, __m128i_tag>(a);
#endif // WJR_SSE4_1
}
uint32_t sse::min_epu32(__m128i a) {
a = min_epu32(a, shuffle_epi32<_MM_SHUFFLE(3, 2, 3, 2)>(a));
a = min_epu32(a, shufflelo_epi16<_MM_SHUFFLE(1, 0, 3, 2)>(a));
return simd_cast<uint32_t, __m128i_tag>(a);
}
int8_t sse::min(__m128i a, int8_t) { return min_epi8(a); }
int16_t sse::min(__m128i a, int16_t) { return min_epi16(a); }
int32_t sse::min(__m128i a, int32_t) { return min_epi32(a); }
uint8_t sse::min(__m128i a, uint8_t) { return min_epu8(a); }
uint16_t sse::min(__m128i a, uint16_t) { return min_epu16(a); }
uint32_t sse::min(__m128i a, uint32_t) { return min_epu32(a); }
__m128i sse::move_epi64(__m128i a) { return _mm_move_epi64(a); }
sse::mask_type sse::movemask_epi8(__m128i a) { return _mm_movemask_epi8(a); }
sse::mask_type sse::movemask_pd(__m128d v) { return _mm_movemask_pd(v); }
sse::mask_type sse::movemask(__m128i v, int8_t) { return movemask_epi8(v); }
sse::mask_type sse::movemask(__m128i v, int32_t) { return movemask_ps(simd_cast<__m128_tag, __m128i_tag>(v)); }
sse::mask_type sse::movemask(__m128i v, int64_t) { return movemask_pd(simd_cast<__m128d_tag, __m128i_tag>(v)); }
sse::mask_type sse::movemask(__m128i v, uint8_t) { return movemask(v, int8_t()); }
sse::mask_type sse::movemask(__m128i v, uint32_t) { return movemask(v, int32_t()); }
sse::mask_type sse::movemask(__m128i v, uint64_t) { return movemask(v, int64_t()); }
__m128i sse::mul_epu32(__m128i a, __m128i b) { return _mm_mul_epu32(a, b); }
__m128i sse::mulhi_epi16(__m128i a, __m128i b) { return _mm_mulhi_epi16(a, b); }
__m128i sse::mulhi_epu16(__m128i a, __m128i b) { return _mm_mulhi_epu16(a, b); }
__m128i sse::mullo_epi16(__m128i a, __m128i b) { return _mm_mullo_epi16(a, b); }
__m128i sse::negate_epi8(__m128i a) {
#if WJR_SSSE3
return sign_epi8(a, ones());
#else
return sub_epi8(zeros(), a);
#endif // WJR_SSSE3
}
__m128i sse::negate_epi16(__m128i a) {
#if WJR_SSSE3
return sign_epi16(a, ones());
#else
return sub_epi16(zeros(), a);
#endif // WJR_SSSE3
}
__m128i sse::negate_epi32(__m128i a) {
#if WJR_SSSE3
return sign_epi32(a, ones());
#else
return sub_epi32(zeros(), a);
#endif // WJR_SSSE3
}
__m128i sse::negate_epi64(__m128i a) {
return sub_epi64(zeros(), a);
}
__m128i sse::negate(__m128i a, int8_t) { return negate_epi8(a); }
__m128i sse::negate(__m128i a, int16_t) { return negate_epi16(a); }
__m128i sse::negate(__m128i a, int32_t) { return negate_epi32(a); }
__m128i sse::negate(__m128i a, int64_t) { return negate_epi64(a); }
__m128i sse::negate(__m128i a, uint8_t) { return negate_epi8(a); }
__m128i sse::negate(__m128i a, uint16_t) { return negate_epi16(a); }
__m128i sse::negate(__m128i a, uint32_t) { return negate_epi32(a); }
__m128i sse::negate(__m128i a, uint64_t) { return negate_epi64(a); }
__m128i sse::Not(__m128i v) { return Xor(v, ones()); }
__m128i sse::Or(__m128i a, __m128i b) { return _mm_or_si128(a, b); }
__m128i sse::packs_epi16(__m128i a, __m128i b) { return _mm_packs_epi16(a, b); }
__m128i sse::packs_epi32(__m128i a, __m128i b) { return _mm_packs_epi32(a, b); }
__m128i sse::packus_epi16(__m128i a, __m128i b) { return _mm_packus_epi16(a, b); }
__m128i sse::preloadu_si16(const void* ptr) { return loadu_si16(ptr); }
__m128i sse::preloadu_si32(const void* ptr) { return loadu_si32(ptr); }
__m128i sse::preloadu_si48(const void* ptr) {
return insert_epi16<2>(preloadu_si32(ptr), reinterpret_cast<const uint16_t*>(ptr)[2]);
}
__m128i sse::preloadu_si64(const void* ptr) { return loadu_si64(ptr); }
__m128i sse::preloadu_si80(const void* ptr) {
return insert_epi16<4>(preloadu_si64(ptr), reinterpret_cast<const uint16_t*>(ptr)[4]);
}
__m128i sse::preloadu_si96(const void* ptr) {
#if WJR_SSE4_1
return insert_epi32<2>(preloadu_si64(ptr), reinterpret_cast<const uint32_t*>(ptr)[2]);
#else
return insert_epi16<5>(preloadu_si80(ptr), reinterpret_cast<const uint16_t*>(ptr)[5]);
#endif // WJR_SSE4_1
}
__m128i sse::preloadu_si112(const void* ptr) {
return insert_epi16<6>(preloadu_si96(ptr), reinterpret_cast<const uint16_t*>(ptr)[6]);
}
__m128i sse::preloadu_si128(const void* ptr) { return loadu((__m128i*)ptr); }
__m128i sse::preloadu_si16x(const void* ptr, int n) {
switch (n) {
case 0:return zeros();
case 1:return preloadu_si16(ptr);
case 2:return preloadu_si32(ptr);
case 3:return preloadu_si48(ptr);
case 4:return preloadu_si64(ptr);
case 5:return preloadu_si80(ptr);
case 6:return preloadu_si96(ptr);
case 7:return preloadu_si112(ptr);
default:return preloadu_si128(ptr);
}
}
__m128i sse::sad_epu8(__m128i a, __m128i b) { return _mm_sad_epu8(a, b); }
__m128i sse::zeros() { return _mm_setzero_si128(); }
__m128i sse::ones() { return _mm_set1_epi32(-1); }
__m128i sse::set_epi8(
char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
return _mm_set_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}
__m128i sse::set_epi16(
short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
return _mm_set_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
}
__m128i sse::set_epi32(int e3, int e2, int e1, int e0) {
return _mm_set_epi32(e3, e2, e1, e0);
}
__m128i sse::set_epi64x(long long e1, long long e0) {
return _mm_set_epi64x(e1, e0);
}
__m128i sse::setr_epi8(
char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
return _mm_setr_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0);
}
__m128i sse::setr_epi16(
short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
return _mm_setr_epi16(e7, e6, e5, e4, e3, e2, e1, e0);
}
__m128i sse::setr_epi32(int e3, int e2, int e1, int e0) {
return _mm_setr_epi32(e3, e2, e1, e0);
}
__m128i sse::set1_epi8(int8_t val) { return _mm_set1_epi8(val); }
__m128i sse::set1_epi16(int16_t val) { return _mm_set1_epi16(val); }
__m128i sse::set1_epi32(int32_t val) { return _mm_set1_epi32(val); }
__m128i sse::set1_epi64(int64_t val) { return _mm_set1_epi64x(val); }
__m128i sse::set1(int8_t val, int8_t) { return set1_epi8(val); }
__m128i sse::set1(int16_t val, int16_t) { return set1_epi16(val); }
__m128i sse::set1(int32_t val, int32_t) { return set1_epi32(val); }
__m128i sse::set1(int64_t val, int64_t) { return set1_epi64(val); }
__m128i sse::set1(uint8_t val, uint8_t) { return set1_epi8(val); }
__m128i sse::set1(uint16_t val, uint16_t) { return set1_epi16(val); }
__m128i sse::set1(uint32_t val, uint32_t) { return set1_epi32(val); }
__m128i sse::set1(uint64_t val, uint64_t) { return set1_epi64(val); }
__m128i sse::setmin_epi8() { return set1_epi8(0x80u); }
__m128i sse::setmin_epi16() { return set1_epi16(0x8000u); }
__m128i sse::setmin_epi32() { return set1_epi32(0x80000000u); }
__m128i sse::setmin(int8_t) { return setmin_epi8(); }
__m128i sse::setmin(int16_t) { return setmin_epi16(); }
__m128i sse::setmin(int32_t) { return setmin_epi32(); }
__m128i sse::setmin(uint8_t) { return set1_epi32(0); }
__m128i sse::setmin(uint16_t) { return set1_epi32(0); }
__m128i sse::setmin(uint32_t) { return set1_epi32(0); }
__m128i sse::setmax_epi8() { return set1_epi8(0x7F); }
__m128i sse::setmax_epi16() { return set1_epi16(0x7FFF); }
__m128i sse::setmax_epi32() { return set1_epi32(0x7FFFFFFF); }
__m128i sse::setmax(int8_t) { return setmax_epi8(); }
__m128i sse::setmax(int16_t) { return setmax_epi16(); }
__m128i sse::setmax(int32_t) { return setmax_epi32(); }
__m128i sse::setmax(uint8_t) { return set1_epi32(0xFFFFFFFF); }
__m128i sse::setmax(uint16_t) { return set1_epi32(0xFFFFFFFF); }
__m128i sse::setmax(uint32_t) { return set1_epi32(0xFFFFFFFF); }
template<int imm>
__m128i sse::shl(__m128i a) {
if constexpr (imm >= 64) {
a = slli<8>(a);
a = slli_epi64(a, imm - 64);
return a;
}
else {
auto b = slli_epi64(a, imm);
auto c = slli<8>(a);
c = srli_epi64(c, 64 - imm);
return Or(b, c);
}
}
template<int imm>
__m128i sse::shr(__m128i a) {
if constexpr (imm >= 64) {
a = srli<8>(a);
a = srli_epi64(a, imm - 64);
return a;
}
else {
auto b = srli_epi64(a, imm);
auto c = srli<8>(a);
c = slli_epi64(c, 64 - imm);
return Or(b, c);
}
}
template<int imm8>
__m128i sse::shuffle_epi32(__m128i v) {
static_assert(imm8 >= 0 && imm8 <= 255, "imm8 must be in range [0, 255]");
return _mm_shuffle_epi32(v, imm8);
}
template<int imm8>
__m128i sse::shufflehi_epi16(__m128i v) {
return _mm_shufflehi_epi16(v, imm8);
}
template<int imm8>
__m128i sse::shufflelo_epi16(__m128i v) {
return _mm_shufflelo_epi16(v, imm8);
}
__m128i sse::sll_epi16(__m128i a, __m128i b) {
return _mm_sll_epi16(a, b);
}
__m128i sse::sll_epi32(__m128i a, __m128i b) {
return _mm_sll_epi32(a, b);
}
__m128i sse::sll_epi64(__m128i a, __m128i b) {
return _mm_sll_epi64(a, b);
}
__m128i sse::sll(__m128i a, __m128i b, int16_t) { return sll_epi16(a, b); }
__m128i sse::sll(__m128i a, __m128i b, int32_t) { return sll_epi32(a, b); }
__m128i sse::sll(__m128i a, __m128i b, int64_t) { return sll_epi64(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint16_t) { return sll_epi16(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint32_t) { return sll_epi32(a, b); }
__m128i sse::sll(__m128i a, __m128i b, uint64_t) { return sll_epi64(a, b); }
template<int imm8>
__m128i sse::slli(__m128i v) {
return _mm_slli_si128(v, imm8);
}
__m128i sse::slli_epi16(__m128i a, int imm8) {
return _mm_slli_epi16(a, imm8);
}
__m128i sse::slli_epi32(__m128i a, int imm8) {
return _mm_slli_epi32(a, imm8);
}
__m128i sse::slli_epi64(__m128i a, int imm8) {
return _mm_slli_epi64(a, imm8);
}
__m128i sse::slli(__m128i a, int imm8, int16_t) { return slli_epi16(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, int32_t) { return slli_epi32(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, int64_t) { return slli_epi64(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint16_t) { return slli_epi16(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint32_t) { return slli_epi32(a, imm8); }
__m128i sse::slli(__m128i a, int imm8, uint64_t) { return slli_epi64(a, imm8); }
__m128i sse::sra_epi16(__m128i a, __m128i b) { return _mm_sra_epi16(a, b); }
__m128i sse::sra_epi32(__m128i a, __m128i b) { return _mm_sra_epi32(a, b); }
__m128i sse::sra(__m128i a, __m128i b, int16_t) { return sra_epi16(a, b); }
__m128i sse::sra(__m128i a, __m128i b, int32_t) { return sra_epi32(a, b); }
__m128i sse::srai_epi16(__m128i a, int imm8) { return _mm_srai_epi16(a, imm8); }
__m128i sse::srai_epi32(__m128i a, int imm8) { return _mm_srai_epi32(a, imm8); }
__m128i sse::srai(__m128i a, int imm8, int16_t) { return srai_epi16(a, imm8); }
__m128i sse::srai(__m128i a, int imm8, int32_t) { return srai_epi32(a, imm8); }
__m128i sse::srl_epi16(__m128i a, __m128i b) { return _mm_srl_epi16(a, b); }
__m128i sse::srl_epi32(__m128i a, __m128i b) { return _mm_srl_epi32(a, b); }
__m128i sse::srl_epi64(__m128i a, __m128i b) { return _mm_srl_epi64(a, b); }
__m128i sse::srl(__m128i a, __m128i b, int16_t) { return srl_epi16(a, b); }
__m128i sse::srl(__m128i a, __m128i b, int32_t) { return srl_epi32(a, b); }
__m128i sse::srl(__m128i a, __m128i b, int64_t) { return srl_epi64(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint16_t) { return srl_epi16(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint32_t) { return srl_epi32(a, b); }
__m128i sse::srl(__m128i a, __m128i b, uint64_t) { return srl_epi64(a, b); }
template<int imm8>
__m128i sse::srli(__m128i v) {
return _mm_srli_si128(v, imm8);
}
__m128i sse::srli_epi16(__m128i a, int imm8) {
return _mm_srli_epi16(a, imm8);
}
__m128i sse::srli_epi32(__m128i a, int imm8) {
return _mm_srli_epi32(a, imm8);
}
__m128i sse::srli_epi64(__m128i a, int imm8) {
return _mm_srli_epi64(a, imm8);
}
__m128i sse::srli(__m128i a, int imm8, int16_t) { return srli_epi16(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, int32_t) { return srli_epi32(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, int64_t) { return srli_epi64(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint16_t) { return srli_epi16(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint32_t) { return srli_epi32(a, imm8); }
__m128i sse::srli(__m128i a, int imm8, uint64_t) { return srli_epi64(a, imm8); }
void sse::stream(__m128i* ptr, __m128i v) { _mm_stream_si128(ptr, v); }
void sse::store(__m128i* ptr, __m128i val) { _mm_store_si128(ptr, val); }
void sse::storeu(__m128i* ptr, __m128i val) { _mm_storeu_si128(ptr, val); }
__m128i sse::sub_epi8(__m128i a, __m128i b) { return _mm_sub_epi8(a, b); }
__m128i sse::sub_epi16(__m128i a, __m128i b) { return _mm_sub_epi16(a, b); }
__m128i sse::sub_epi32(__m128i a, __m128i b) { return _mm_sub_epi32(a, b); }
__m128i sse::sub_epi64(__m128i a, __m128i b) { return _mm_sub_epi64(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int8_t) { return sub_epi8(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int16_t) { return sub_epi16(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int32_t) { return sub_epi32(a, b); }
__m128i sse::sub(__m128i a, __m128i b, int64_t) { return sub_epi64(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint8_t) { return sub_epi8(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint16_t) { return sub_epi16(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint32_t) { return sub_epi32(a, b); }
__m128i sse::sub(__m128i a, __m128i b, uint64_t) { return sub_epi64(a, b); }
__m128i sse::subs_epi8(__m128i a, __m128i b) { return _mm_subs_epi8(a, b); }
__m128i sse::subs_epi16(__m128i a, __m128i b) { return _mm_subs_epi16(a, b); }
__m128i sse::subs_epu8(__m128i a, __m128i b) { return _mm_subs_epu8(a, b); }
__m128i sse::subs_epu16(__m128i a, __m128i b) { return _mm_subs_epu16(a, b); }
__m128i sse::subs(__m128i a, __m128i b, int8_t) { return subs_epi8(a, b); }
__m128i sse::subs(__m128i a, __m128i b, int16_t) { return subs_epi16(a, b); }
__m128i sse::subs(__m128i a, __m128i b, uint8_t) { return subs_epu8(a, b); }
__m128i sse::subs(__m128i a, __m128i b, uint16_t) { return subs_epu16(a, b); }
__m128i sse::unpackhi_epi8(__m128i a, __m128i b) { return _mm_unpackhi_epi8(a, b); }
__m128i sse::unpackhi_epi16(__m128i a, __m128i b) { return _mm_unpackhi_epi16(a, b); }
__m128i sse::unpackhi_epi32(__m128i a, __m128i b) { return _mm_unpackhi_epi32(a, b); }
__m128i sse::unpackhi_epi64(__m128i a, __m128i b) { return _mm_unpackhi_epi64(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int8_t) { return unpackhi_epi8(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int16_t) { return unpackhi_epi16(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int32_t) { return unpackhi_epi32(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, int64_t) { return unpackhi_epi64(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint8_t) { return unpackhi_epi8(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint16_t) { return unpackhi_epi16(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint32_t) { return unpackhi_epi32(a, b); }
__m128i sse::unpackhi(__m128i a, __m128i b, uint64_t) { return unpackhi_epi64(a, b); }
__m128i sse::unpacklo_epi8(__m128i a, __m128i b) { return _mm_unpacklo_epi8(a, b); }
__m128i sse::unpacklo_epi16(__m128i a, __m128i b) { return _mm_unpacklo_epi16(a, b); }
__m128i sse::unpacklo_epi32(__m128i a, __m128i b) { return _mm_unpacklo_epi32(a, b); }
__m128i sse::unpacklo_epi64(__m128i a, __m128i b) { return _mm_unpacklo_epi64(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int8_t) { return unpacklo_epi8(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int16_t) { return unpacklo_epi16(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int32_t) { return unpacklo_epi32(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, int64_t) { return unpacklo_epi64(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint8_t) { return unpacklo_epi8(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint16_t) { return unpacklo_epi16(a, b); }
__m128i sse::unpacklo(__m128i a, __m128i b, uint32_t) { return unpacklo_epi32(a, b); }
__m128i sse::Xor(__m128i a, __m128i b) { return _mm_xor_si128(a, b); }
#endif // WJR_SSE2
#if WJR_SSE3
__m128i sse::lddqu(const __m128i* ptr) { return _mm_lddqu_si128(ptr); }
#endif // WJR_SSE3
#if WJR_SSSE3
__m128i sse::abs_epi8(__m128i val) { return _mm_abs_epi8(val); }
__m128i sse::abs_epi16(__m128i val) { return _mm_abs_epi16(val); }
__m128i sse::abs_epi32(__m128i val) { return _mm_abs_epi32(val); }
__m128i sse::abs(__m128i val, int8_t) { return abs_epi8(val); }
__m128i sse::abs(__m128i val, int16_t) { return abs_epi16(val); }
__m128i sse::abs(__m128i val, int32_t) { return abs_epi32(val); }
__m128i sse::abs(__m128i val, uint8_t) { return val; }
__m128i sse::abs(__m128i val, uint16_t) { return val; }
__m128i sse::abs(__m128i val, uint32_t) { return val; }
__m128i sse::shuffle_epi8(__m128i v, __m128i imm8) { return _mm_shuffle_epi8(v, imm8); }
__m128i sse::sign_epi8(__m128i a, __m128i b) { return _mm_sign_epi8(a, b); }
__m128i sse::sign_epi16(__m128i a, __m128i b) { return _mm_sign_epi16(a, b); }
__m128i sse::sign_epi32(__m128i a, __m128i b) { return _mm_sign_epi32(a, b); }
__m128i sse::sign(__m128i a, __m128i b, int8_t) { return sign_epi8(a, b); }
__m128i sse::sign(__m128i a, __m128i b, int16_t) { return sign_epi16(a, b); }
__m128i sse::sign(__m128i a, __m128i b, int32_t) { return sign_epi32(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint8_t) { return sign_epi8(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint16_t) { return sign_epi16(a, b); }
__m128i sse::sign(__m128i a, __m128i b, uint32_t) { return sign_epi32(a, b); }
#endif // WJR_SSSE3
#if WJR_SSE4_1
template<int imm8>
__m128i sse::blend_epi16(__m128i a, __m128i b) {
return _mm_blend_epi16(a, b, imm8);
}
__m128i sse::cmpeq_epi64(__m128i a, __m128i b) { return _mm_cmpeq_epi64(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, int64_t) { return cmpeq_epi64(a, b); }
__m128i sse::cmpeq(__m128i a, __m128i b, uint64_t) { return cmpeq_epi64(a, b); }
__m128i sse::cmpgt_epi64(__m128i a, __m128i b) { return _mm_cmpgt_epi64(a, b); }
__m128i sse::cmpgt(__m128i a, __m128i b, int64_t) { return cmpgt_epi64(a, b); }
template<int imm8>
__m128i sse::insert_epi8(__m128i a, int i) {
return _mm_insert_epi8(a, i, imm8);
}
template<int imm8>
__m128i sse::insert_epi32(__m128i a, int i) {
return _mm_insert_epi32(a, i, imm8);
}
template<int imm8>
__m128i sse::insert_epi64(__m128i a, int64_t i) {
return _mm_insert_epi64(a, i, imm8);
}
template<int imm8>
__m128i sse::insert(__m128i a, int i, int8_t) { return insert_epi8<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int i, int32_t) { return insert_epi32<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int64_t i, int64_t) { return insert_epi64<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int i, uint8_t) { return insert_epi8<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int i, uint32_t) { return insert_epi32<imm8>(a, i); }
template<int imm8>
__m128i sse::insert(__m128i a, int64_t i, uint64_t) { return insert_epi64<imm8>(a, i); }
__m128i sse::minpos_epu16(__m128i a) { return _mm_minpos_epu16(a); }
__m128i sse::mul_epi32(__m128i a, __m128i b) { return _mm_mul_epi32(a, b); }
__m128i sse::mullo_epi32(__m128i a, __m128i b) { return _mm_mullo_epi32(a, b); }
__m128i sse::packus_epi32(__m128i a, __m128i b) { return _mm_packus_epi32(a, b); }
__m128i sse::stream_load(__m128i* p) { return _mm_stream_load_si128(p); }
int sse::test_all_ones(__m128i a) { return _mm_test_all_ones(a); }
int sse::test_all_zeros(__m128i a, __m128i b) { return _mm_test_all_zeros(a, b); }
int sse::test_all_zeros(__m128i a) { return _mm_test_all_zeros(a, a); }
int sse::test_mix_ones_zeros(__m128i a, __m128i b) { return _mm_test_mix_ones_zeros(a, b); }
int sse::testc(__m128i a, __m128i b) { return _mm_testc_si128(a, b); }
int sse::testnzc(__m128i a, __m128i b) { return _mm_testnzc_si128(a, b); }
int sse::testz(__m128i a, __m128i b) {
return _mm_testz_si128(a, b);
}
#endif // WJR_SSE4_1
/*------AVX------*/
constexpr size_t avx::width() { return 256; }
constexpr avx::mask_type avx::mask() { return 0xffffffff; }
#if WJR_AVX
__m256i avx::concat(__m128i a, __m128i b) {
return insert_si128<1>(simd_cast<__m256i_tag, __m128i_tag>(a), b);
}
template<int imm8>
int avx::extract_epi32(__m256i v) { return _mm256_extract_epi32(v, imm8); }
template<int imm8>
int64_t avx::extract_epi64(__m256i v) { return _mm256_extract_epi64(v, imm8); }
template<int imm8>
int avx::extract(__m256i v, int32_t) { return extract_epi32<imm8>(v); }
template<int imm8>
int64_t avx::extract(__m256i v, int64_t) { return extract_epi64<imm8>(v); }
template<int imm8>
__m128i avx::extract_si128(__m256i v) {
#if WJR_AVX2
return _mm256_extracti128_si256(v, imm8);
#else
return _mm256_extractf128_si256(v, imm8);
#endif // WJR_AVX2
}
__m128i avx::getlow(__m256i a) {
return simd_cast<__m128i_tag, __m256i_tag>(a);
}
__m128i avx::gethigh(__m256i a) {
return extract_si128<1>(a);
}
template<int imm8>
__m256i avx::insert_epi8(__m256i v, int8_t i) { return _mm256_insert_epi8(v, i, imm8); }
template<int imm8>
__m256i avx::insert_epi16(__m256i v, int16_t i) { return _mm256_insert_epi16(v, i, imm8); }
template<int imm8>
__m256i avx::insert_epi32(__m256i v, int32_t i) { return _mm256_insert_epi32(v, i, imm8); }
template<int imm8>
__m256i avx::insert_epi64(__m256i v, int64_t i) { return _mm256_insert_epi64(v, i, imm8); }
template<int imm8>
__m256i avx::insert_si128(__m256i a, __m128i b) {
#if WJR_AVX2
return _mm256_inserti128_si256(a, b, imm8);
#else
return _mm256_insertf128_si256(a, b, imm8);
#endif // WJR_AVX2
}
__m256i avx::load(const __m256i* p) { return _mm256_load_si256(p); }
__m256i avx::loadu(const __m256i* p) { return _mm256_loadu_si256(p); }
__m256i avx::ones() { return _mm256_set1_epi32(-1); }
__m256i avx::preloadu_si16(const void* ptr) {
return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si16(ptr));
}
__m256i avx::preloadu_si32(const void* ptr) {
return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si32(ptr));
}
__m256i avx::preloadu_si48(const void* ptr) {
return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si48(ptr));
}
__m256i avx::preloadu_si64(const void* ptr) {
return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si64(ptr));
}
__m256i avx::preloadu_si80(const void* ptr) {
return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si80(ptr));
}
__m256i avx::preloadu_si96(const void* ptr) {
return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si96(ptr));
}
__m256i avx::preloadu_si112(const void* ptr) {
return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si112(ptr));
}
__m256i avx::preloadu_si128(const void* ptr) {
return simd_cast<__m256i_tag, __m128i_tag>(sse::preloadu_si128(ptr));
}
__m256i avx::preloadu_si144(const void* ptr) {
return concat(sse::preloadu_si128(ptr),
sse::preloadu_si16((const char*)ptr + 16));
}
__m256i avx::preloadu_si160(const void* ptr) {
return concat(sse::preloadu_si128(ptr),
sse::preloadu_si32((const char*)ptr + 16));
}
__m256i avx::preloadu_si176(const void* ptr) {
return concat(sse::preloadu_si128(ptr),
sse::preloadu_si48((const char*)ptr + 16));
}
__m256i avx::preloadu_si192(const void* ptr) {
return concat(sse::preloadu_si128(ptr),
sse::preloadu_si64((const char*)ptr + 16));
}
__m256i avx::preloadu_si208(const void* ptr) {
return concat(sse::preloadu_si128(ptr),
sse::preloadu_si80((const char*)ptr + 16));
}
__m256i avx::preloadu_si224(const void* ptr) {
return concat(sse::preloadu_si128(ptr),
sse::preloadu_si96((const char*)ptr + 16));
}
__m256i avx::preloadu_si240(const void* ptr) {
return concat(sse::preloadu_si128(ptr),
sse::preloadu_si112((const char*)ptr + 16));
}
__m256i avx::preloadu_si256(const void* ptr) {
return loadu((const __m256i*)ptr);
}
__m256i avx::preloadu_si16x(const void* ptr, int n) {
switch (n) {
case 0:return zeros();
case 1:return preloadu_si16(ptr);
case 2:return preloadu_si32(ptr);
case 3:return preloadu_si48(ptr);
case 4:return preloadu_si64(ptr);
case 5:return preloadu_si80(ptr);
case 6:return preloadu_si96(ptr);
case 7:return preloadu_si112(ptr);
case 8:return preloadu_si128(ptr);
case 9:return preloadu_si144(ptr);
case 10:return preloadu_si160(ptr);
case 11:return preloadu_si176(ptr);
case 12:return preloadu_si192(ptr);
case 13:return preloadu_si208(ptr);
case 14:return preloadu_si224(ptr);
case 15:return preloadu_si240(ptr);
default:return preloadu_si256(ptr);
}
}
__m256i avx::set_epi8(
char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
return _mm256_set_epi8(
e31, e30, e29, e28, e27, e26, e25, e24,
e23, e22, e21, e20, e19, e18, e17, e16,
e15, e14, e13, e12, e11, e10, e9, e8,
e7, e6, e5, e4, e3, e2, e1, e0);
}
__m256i avx::set_epi16(
short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
return _mm256_set_epi16(
e15, e14, e13, e12, e11, e10, e9, e8,
e7, e6, e5, e4, e3, e2, e1, e0);
}
__m256i avx::set_epi32(
int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
return _mm256_set_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
}
__m256i avx::set_epi64x(
long long e3, long long e2, long long e1, long long e0) {
return _mm256_set_epi64x(e3, e2, e1, e0);
}
__m256i avx::setr_epi8(
char e31, char e30, char e29, char e28, char e27, char e26, char e25, char e24,
char e23, char e22, char e21, char e20, char e19, char e18, char e17, char e16,
char e15, char e14, char e13, char e12, char e11, char e10, char e9, char e8,
char e7, char e6, char e5, char e4, char e3, char e2, char e1, char e0) {
return _mm256_setr_epi8(
e31, e30, e29, e28, e27, e26, e25, e24,
e23, e22, e21, e20, e19, e18, e17, e16,
e15, e14, e13, e12, e11, e10, e9, e8,
e7, e6, e5, e4, e3, e2, e1, e0);
}
__m256i avx::setr_epi16(
short e15, short e14, short e13, short e12, short e11, short e10, short e9, short e8,
short e7, short e6, short e5, short e4, short e3, short e2, short e1, short e0) {
return _mm256_setr_epi16(
e15, e14, e13, e12, e11, e10, e9, e8,
e7, e6, e5, e4, e3, e2, e1, e0);
}
__m256i avx::setr_epi32(
int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0) {
return _mm256_setr_epi32(e7, e6, e5, e4, e3, e2, e1, e0);
}
__m256i avx::setr_epi64x(
long long e3, long long e2, long long e1, long long e0) {
return _mm256_setr_epi64x(e3, e2, e1, e0);
}
__m256i avx::set1_epi8(int8_t a) { return _mm256_set1_epi8(a); }
__m256i avx::set1_epi16(int16_t a) { return _mm256_set1_epi16(a); }
__m256i avx::set1_epi32(int32_t a) { return _mm256_set1_epi32(a); }
__m256i avx::set1_epi64(int64_t a) { return _mm256_set1_epi64x(a); }
__m256i avx::set1(int8_t a, int8_t) { return set1_epi8(a); }
__m256i avx::set1(int16_t a, int16_t) { return set1_epi16(a); }
__m256i avx::set1(int32_t a, int32_t) { return set1_epi32(a); }
__m256i avx::set1(int64_t a, int64_t) { return set1_epi64(a); }
__m256i avx::set1(uint8_t a, uint8_t) { return set1_epi8(a); }
__m256i avx::set1(uint16_t a, uint16_t) { return set1_epi16(a); }
__m256i avx::set1(uint32_t a, uint32_t) { return set1_epi32(a); }
__m256i avx::set1(uint64_t a, uint64_t) { return set1_epi64(a); }
__m256i avx::setmin_epi8() { return set1_epi8(0x80u); }
__m256i avx::setmin_epi16() { return set1_epi16(0x8000u); }
__m256i avx::setmin_epi32() { return set1_epi32(0x80000000u); }
__m256i avx::setmin_epi64() { return set1_epi64(0x8000000000000000ull); }
__m256i avx::setmin(int8_t) { return setmin_epi8(); }
__m256i avx::setmin(int16_t) { return setmin_epi16(); }
__m256i avx::setmin(int32_t) { return setmin_epi32(); }
__m256i avx::setmin(int64_t) { return setmin_epi64(); }
__m256i avx::setmax_epi8() { return set1_epi8(0x7f); }
__m256i avx::setmax_epi16() { return set1_epi16(0x7fff); }
__m256i avx::setmax_epi32() { return set1_epi32(0x7fffffff); }
__m256i avx::setmax_epi64() { return set1_epi64(0x7fffffffffffffff); }
__m256i avx::setmax(int8_t) { return setmax_epi8(); }
__m256i avx::setmax(int16_t) { return setmax_epi16(); }
__m256i avx::setmax(int32_t) { return setmax_epi32(); }
__m256i avx::setmax(int64_t) { return setmax_epi64(); }
void avx::stream(__m256i* p, __m256i a) { _mm256_stream_si256(p, a); }
void avx::store(__m256i* p, const __m256i& a) { _mm256_store_si256(p, a); }
void avx::storeu(__m256i* p, const __m256i& a) { _mm256_storeu_si256(p, a); }
int avx::test_all_zeros(__m256i a) { return testz(a, a); }
int avx::testc(__m256i a, __m256i b) { return _mm256_testc_si256(a, b); }
int avx::testnzc(__m256i a, __m256i b) { return _mm256_testnzc_si256(a, b); }
int avx::testz(__m256i a, __m256i b) { return _mm256_testz_si256(a, b); }
__m256i avx::zeros() { return _mm256_setzero_si256(); }
#endif // WJR_AVX
#if WJR_AVX2
__m256i avx::And(__m256i a, __m256i b) { return _mm256_and_si256(a, b); }
__m256i avx::AndNot(__m256i a, __m256i b) { return _mm256_andnot_si256(a, b); }
__m256i avx::Or(__m256i a, __m256i b) { return _mm256_or_si256(a, b); }
__m256i avx::Xor(__m256i a, __m256i b) { return _mm256_xor_si256(a, b); }
__m256i avx::Not(__m256i v) { return _mm256_xor_si256(v, ones()); }
__m256i avx::abs_epi8(__m256i v) { return _mm256_abs_epi8(v); }
__m256i avx::abs_epi16(__m256i v) { return _mm256_abs_epi16(v); }
__m256i avx::abs_epi32(__m256i v) { return _mm256_abs_epi32(v); }
__m256i avx::abs(__m256i v, int8_t) { return abs_epi8(v); }
__m256i avx::abs(__m256i v, int16_t) { return abs_epi16(v); }
__m256i avx::abs(__m256i v, int32_t) { return abs_epi32(v); }
__m256i avx::abs(__m256i v, int64_t) { return abs_epi32(v); }
__m256i avx::add_epi8(__m256i a, __m256i b) { return _mm256_add_epi8(a, b); }
__m256i avx::add_epi16(__m256i a, __m256i b) { return _mm256_add_epi16(a, b); }
__m256i avx::add_epi32(__m256i a, __m256i b) { return _mm256_add_epi32(a, b); }
__m256i avx::add_epi64(__m256i a, __m256i b) { return _mm256_add_epi64(a, b); }
__m256i avx::add(__m256i a, __m256i b, int8_t) { return add_epi8(a, b); }
__m256i avx::add(__m256i a, __m256i b, int16_t) { return add_epi16(a, b); }
__m256i avx::add(__m256i a, __m256i b, int32_t) { return add_epi32(a, b); }
__m256i avx::add(__m256i a, __m256i b, int64_t) { return add_epi64(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint8_t) { return add_epi8(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint16_t) { return add_epi16(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint32_t) { return add_epi32(a, b); }
__m256i avx::add(__m256i a, __m256i b, uint64_t) { return add_epi64(a, b); }
uint8_t avx::add_epu8(__m256i v) {
return sse::add_epu8(sse::add_epi8(getlow(v), gethigh(v)));
}
uint16_t avx::add_epu16(__m256i v) {
return sse::add_epu16(sse::add_epi16(getlow(v), gethigh(v)));
}
uint32_t avx::add_epu32(__m256i v) {
return sse::add_epu32(sse::add_epi32(getlow(v), gethigh(v)));
}
uint64_t avx::add_epu64(__m256i v) {
return sse::add_epu64(sse::add_epi64(getlow(v), gethigh(v)));
}
int8_t avx::add_epi8(__m256i v) { return add_epu8(v); }
int16_t avx::add_epi16(__m256i v) { return add_epu16(v); }
int32_t avx::add_epi32(__m256i v) { return add_epu32(v); }
int64_t avx::add_epi64(__m256i v) { return add_epu64(v); }
int8_t avx::add(__m256i v, int8_t) { return add_epi8(v); }
int16_t avx::add(__m256i v, int16_t) { return add_epi16(v); }
int32_t avx::add(__m256i v, int32_t) { return add_epi32(v); }
int64_t avx::add(__m256i v, int64_t) { return add_epi64(v); }
uint8_t avx::add(__m256i v, uint8_t) { return add_epu8(v); }
uint16_t avx::add(__m256i v, uint16_t) { return add_epu16(v); }
uint32_t avx::add(__m256i v, uint32_t) { return add_epu32(v); }
uint64_t avx::add(__m256i v, uint64_t) { return add_epu64(v); }
__m256i avx::adds_epi8(__m256i a, __m256i b) { return _mm256_adds_epi8(a, b); }
__m256i avx::adds_epi16(__m256i a, __m256i b) { return _mm256_adds_epi16(a, b); }
__m256i avx::adds_epu8(__m256i a, __m256i b) { return _mm256_adds_epu8(a, b); }
__m256i avx::adds_epu16(__m256i a, __m256i b) { return _mm256_adds_epu16(a, b); }
__m256i avx::adds(__m256i a, __m256i b, int8_t) { return adds_epi8(a, b); }
__m256i avx::adds(__m256i a, __m256i b, int16_t) { return adds_epi16(a, b); }
__m256i avx::adds(__m256i a, __m256i b, uint8_t) { return adds_epu8(a, b); }
__m256i avx::adds(__m256i a, __m256i b, uint16_t) { return adds_epu16(a, b); }
template<int imm8>
__m256i avx::alignr(__m256i a, __m256i b) { return _mm256_alignr_epi8(a, b, imm8); }
__m256i avx::alignr_epi16(__m256i a, __m256i b, int c) {
return Or(slli_epi16(a, 16 - c), srli_epi16(b, c));
}
__m256i avx::alignr_epi32(__m256i a, __m256i b, int c) {
return Or(slli_epi32(a, 32 - c), srli_epi32(b, c));
}
__m256i avx::alignr_epi64(__m256i a, __m256i b, int c) {
return Or(slli_epi64(a, 64 - c), srli_epi64(b, c));
}
__m256i avx::alignr(__m256i a, __m256i b, int c, int16_t) {
return alignr_epi16(a, b, c);
}
__m256i avx::alignr(__m256i a, __m256i b, int c, int32_t) {
return alignr_epi32(a, b, c);
}
__m256i avx::alignr(__m256i a, __m256i b, int c, int64_t) {
return alignr_epi64(a, b, c);
}
__m256i avx::alignr(__m256i a, __m256i b, int c, uint16_t) {
return alignr_epi16(a, b, c);
}
__m256i avx::alignr(__m256i a, __m256i b, int c, uint32_t) {
return alignr_epi32(a, b, c);
}
__m256i avx::alignr(__m256i a, __m256i b, int c, uint64_t) {
return alignr_epi64(a, b, c);
}
__m256i avx::avg_epu8(__m256i a, __m256i b) { return _mm256_avg_epu8(a, b); }
__m256i avx::avg_epu16(__m256i a, __m256i b) { return _mm256_avg_epu16(a, b); }
__m256i avx::avg(__m256i a, __m256i b, int8_t) { return avg_epu8(a, b); }
__m256i avx::avg(__m256i a, __m256i b, int16_t) { return avg_epu16(a, b); }
__m256i avx::avg(__m256i a, __m256i b, uint8_t) { return avg_epu8(a, b); }
__m256i avx::avg(__m256i a, __m256i b, uint16_t) { return avg_epu16(a, b); }
template<int imm8>
__m256i avx::blend_epi16(__m256i a, __m256i b) { return _mm256_blend_epi16(a, b, imm8); }
template<int imm8>
__m256i avx::blend_epi32(__m256i a, __m256i b) { return _mm256_blend_epi32(a, b, imm8); }
__m256i avx::blendv_epi8(__m256i a, __m256i b, __m256i mask) {
return _mm256_blendv_epi8(a, b, mask);
}
template<int imm8>
__m256i avx::bslli_epi128(__m256i a) { return _mm256_bslli_epi128(a, imm8); }
template<int imm8>
__m256i avx::bsrli_epi128(__m256i a) { return _mm256_bsrli_epi128(a, imm8); }
__m256i avx::cmpeq_epi8(__m256i a, __m256i b) { return _mm256_cmpeq_epi8(a, b); }
__m256i avx::cmpeq_epi16(__m256i a, __m256i b) { return _mm256_cmpeq_epi16(a, b); }
__m256i avx::cmpeq_epi32(__m256i a, __m256i b) { return _mm256_cmpeq_epi32(a, b); }
__m256i avx::cmpeq_epi64(__m256i a, __m256i b) { return _mm256_cmpeq_epi64(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int8_t) { return cmpeq_epi8(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int16_t) { return cmpeq_epi16(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int32_t) { return cmpeq_epi32(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, int64_t) { return cmpeq_epi64(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint8_t) { return cmpeq_epi8(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint16_t) { return cmpeq_epi16(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint32_t) { return cmpeq_epi32(a, b); }
__m256i avx::cmpeq(__m256i a, __m256i b, uint64_t) { return cmpeq_epi64(a, b); }
__m256i avx::cmpge_epi8(__m256i a, __m256i b) {
return cmpeq(min(a, b, int8_t()), b, uint8_t());
}
__m256i avx::cmpge_epi16(__m256i a, __m256i b) {
return cmpeq(min(a, b, int16_t()), b, uint16_t());
}
__m256i avx::cmpge_epi32(__m256i a, __m256i b) {
return cmpeq(min(a, b, int32_t()), b, uint8_t());
}
__m256i avx::cmpge_epu8(__m256i a, __m256i b) {
return cmpeq(min(a, b, uint8_t()), b, uint8_t());
}
__m256i avx::cmpge_epu16(__m256i a, __m256i b) {
return cmpeq(min(a, b, uint16_t()), b, uint16_t());
}
__m256i avx::cmpge_epu32(__m256i a, __m256i b) {
return cmpeq(min(a, b, uint32_t()), b, uint32_t());
}
__m256i avx::cmpge(__m256i a, __m256i b, int8_t) { return cmpge_epi8(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, int16_t) { return cmpge_epi16(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, int32_t) { return cmpge_epi32(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint8_t) { return cmpge_epu8(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint16_t) { return cmpge_epu16(a, b); }
__m256i avx::cmpge(__m256i a, __m256i b, uint32_t) { return cmpge_epu32(a, b); }
__m256i avx::cmpgt_epi8(__m256i a, __m256i b) { return _mm256_cmpgt_epi8(a, b); }
__m256i avx::cmpgt_epi16(__m256i a, __m256i b) { return _mm256_cmpgt_epi16(a, b); }
__m256i avx::cmpgt_epi32(__m256i a, __m256i b) { return _mm256_cmpgt_epi32(a, b); }
__m256i avx::cmpgt_epi64(__m256i a, __m256i b) { return _mm256_cmpgt_epi64(a, b); }
__m256i avx::cmpgt_epu8(__m256i a, __m256i b) {
return cmpgt_epi8(Xor(a, setmin_epi8()), Xor(b, setmin_epi8()));
}
__m256i avx::cmpgt_epu16(__m256i a, __m256i b) {
return cmpgt_epi16(Xor(a, setmin_epi16()), Xor(b, setmin_epi16()));
}
__m256i avx::cmpgt_epu32(__m256i a, __m256i b) {
return cmpgt_epi32(Xor(a, setmin_epi32()), Xor(b, setmin_epi32()));
}
__m256i avx::cmpgt_epu64(__m256i a, __m256i b) {
return cmpgt_epi64(Xor(a, setmin_epi64()), Xor(b, setmin_epi64()));
}
__m256i avx::cmpgt(__m256i a, __m256i b, int8_t) { return cmpgt_epi8(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int16_t) { return cmpgt_epi16(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int32_t) { return cmpgt_epi32(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, int64_t) { return cmpgt_epi64(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint8_t) { return cmpgt_epu8(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint16_t) { return cmpgt_epu16(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint32_t) { return cmpgt_epu32(a, b); }
__m256i avx::cmpgt(__m256i a, __m256i b, uint64_t) { return cmpgt_epu64(a, b); }
__m256i avx::cmple_epi8(__m256i a, __m256i b) {
return cmpge_epi8(b, a);
}
__m256i avx::cmple_epi16(__m256i a, __m256i b) {
return cmpge_epi16(b, a);
}
__m256i avx::cmple_epi32(__m256i a, __m256i b) {
return cmpge_epi32(b, a);
}
__m256i avx::cmple_epu8(__m256i a, __m256i b) {
return cmpge_epu8(b, a);
}
__m256i avx::cmple_epu16(__m256i a, __m256i b) {
return cmpge_epu16(b, a);
}
__m256i avx::cmple_epu32(__m256i a, __m256i b) {
return cmpge_epu32(b, a);
}
__m256i avx::cmple(__m256i a, __m256i b, int8_t) { return cmple_epi8(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, int16_t) { return cmple_epi16(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, int32_t) { return cmple_epi32(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint8_t) { return cmple_epu8(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint16_t) { return cmple_epu16(a, b); }
__m256i avx::cmple(__m256i a, __m256i b, uint32_t) { return cmple_epu32(a, b); }
__m256i avx::cmplt_epi8(__m256i a, __m256i b) { return cmpgt_epi8(b, a); }
__m256i avx::cmplt_epi16(__m256i a, __m256i b) { return cmpgt_epi16(b, a); }
__m256i avx::cmplt_epi32(__m256i a, __m256i b) { return cmpgt_epi32(b, a); }
__m256i avx::cmplt_epu8(__m256i a, __m256i b) {
return cmpgt_epu8(b, a);
}
__m256i avx::cmplt_epu16(__m256i a, __m256i b) {
return cmpgt_epu16(b, a);
}
__m256i avx::cmplt_epu32(__m256i a, __m256i b) {
return cmpgt_epu32(b, a);
}
__m256i avx::cmplt(__m256i a, __m256i b, int8_t) { return cmplt_epi8(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, int16_t) { return cmplt_epi16(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, int32_t) { return cmplt_epi32(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint8_t) { return cmplt_epu8(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint16_t) { return cmplt_epu16(a, b); }
__m256i avx::cmplt(__m256i a, __m256i b, uint32_t) { return cmplt_epu32(a, b); }
__m256i avx::cmpne_epi8(__m256i a, __m256i b) { return Not(cmpeq_epi8(a, b)); }
__m256i avx::cmpne_epi16(__m256i a, __m256i b) { return Not(cmpeq_epi16(a, b)); }
__m256i avx::cmpne_epi32(__m256i a, __m256i b) { return Not(cmpeq_epi32(a, b)); }
__m256i avx::cmpne(__m256i a, __m256i b, int8_t) { return cmpne_epi8(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, int16_t) { return cmpne_epi16(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, int32_t) { return cmpne_epi32(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint8_t) { return cmpne_epi8(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint16_t) { return cmpne_epi16(a, b); }
__m256i avx::cmpne(__m256i a, __m256i b, uint32_t) { return cmpne_epi32(a, b); }
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::equal_to<>, T) {
return cmpeq(a, b, T());
}
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::not_equal_to<>, T) {
return cmpne(a, b, T());
}
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::greater<>, T) {
return cmpgt(a, b, T());
}
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::greater_equal<>, T) {
return cmpge(a, b, T());
}
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::less<>, T) {
return cmplt(a, b, T());
}
template<typename T>
__m256i avx::cmp(__m256i a, __m256i b, std::less_equal<>, T) {
return cmple(a, b, T());
}
template<int imm8>
int avx::extract_epi8(__m256i v) { return _mm256_extract_epi8(v, imm8); }
template<int imm8>
int avx::extract_epi16(__m256i v) { return _mm256_extract_epi16(v, imm8); }
template<int imm8>
int avx::extract(__m256i v, int8_t) { return extract_epi8<imm8>(v); }
template<int imm8>
int avx::extract(__m256i v, int16_t) { return extract_epi16<imm8>(v); }
__m256i avx::hadd_epi16(__m256i a, __m256i b) { return _mm256_hadd_epi16(a, b); }
__m256i avx::hadd_epi32(__m256i a, __m256i b) { return _mm256_hadd_epi32(a, b); }
__m256i avx::hadd(__m256i a, __m256i b, int16_t) { return hadd_epi16(a, b); }
__m256i avx::hadd(__m256i a, __m256i b, int32_t) { return hadd_epi32(a, b); }
__m256i avx::hadds_epi16(__m256i a, __m256i b) { return _mm256_hadds_epi16(a, b); }
__m256i avx::hsub_epi16(__m256i a, __m256i b) { return _mm256_hsub_epi16(a, b); }
__m256i avx::hsub_epi32(__m256i a, __m256i b) { return _mm256_hsub_epi32(a, b); }
__m256i avx::hsub(__m256i a, __m256i b, int16_t) { return hsub_epi16(a, b); }
__m256i avx::hsub(__m256i a, __m256i b, int32_t) { return hsub_epi32(a, b); }
__m256i avx::hsubs_epi16(__m256i a, __m256i b) { return _mm256_hsubs_epi16(a, b); }
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int>>
__m256i avx::logical_and(__m256i a, __m256i b, T) {
return Not(Or(logical_not(a, T()), logical_not(b, T())));
}
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int>>
__m256i avx::logical_not(__m256i v, T) {
auto Zero = zeros();
return cmpeq(v, Zero, T());
}
template<typename T, std::enable_if_t<is_any_of_v<T, int8_t, int16_t, int32_t, int64_t,
uint8_t, uint16_t, uint32_t, uint64_t>, int>>
__m256i avx::logical_or(__m256i a, __m256i b, T) {
return Not(logical_not(Or(a, b), T()));
}
__m256i avx::madd_epi16(__m256i a, __m256i b) { return _mm256_madd_epi16(a, b); }
__m256i avx::max_epi8(__m256i a, __m256i b) { return _mm256_max_epi8(a, b); }
__m256i avx::max_epi16(__m256i a, __m256i b) { return _mm256_max_epi16(a, b); }
__m256i avx::max_epi32(__m256i a, __m256i b) { return _mm256_max_epi32(a, b); }
__m256i avx::max_epu8(__m256i a, __m256i b) { return _mm256_max_epu8(a, b); }
__m256i avx::max_epu16(__m256i a, __m256i b) { return _mm256_max_epu16(a, b); }
__m256i avx::max_epu32(__m256i a, __m256i b) { return _mm256_max_epu32(a, b); }
__m256i avx::max(__m256i a, __m256i b, int8_t) { return max_epi8(a, b); }
__m256i avx::max(__m256i a, __m256i b, int16_t) { return max_epi16(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint8_t) { return max_epu8(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint16_t) { return max_epu16(a, b); }
__m256i avx::max(__m256i a, __m256i b, int32_t) { return max_epi32(a, b); }
__m256i avx::max(__m256i a, __m256i b, uint32_t) { return max_epu32(a, b); }
int8_t avx::max_epi8(__m256i a) {
return sse::max_epi8(sse::max_epi8(getlow(a), gethigh(a)));
}
int16_t avx::max_epi16(__m256i a) {
return sse::max_epi16(sse::max_epi16(getlow(a), gethigh(a)));
}
int32_t avx::max_epi32(__m256i a) {
return sse::max_epi32(sse::max_epi32(getlow(a), gethigh(a)));
}
uint8_t avx::max_epu8(__m256i a) {
return sse::max_epu8(sse::max_epu8(getlow(a), gethigh(a)));
}
uint16_t avx::max_epu16(__m256i a) {
return sse::max_epu16(sse::max_epu16(getlow(a), gethigh(a)));
}
uint32_t avx::max_epu32(__m256i a) {
return sse::max_epu32(sse::max_epu32(getlow(a), gethigh(a)));
}
int8_t avx::max(__m256i a, int8_t) { return max_epi8(a); }
int16_t avx::max(__m256i a, int16_t) { return max_epi16(a); }
int32_t avx::max(__m256i a, int32_t) { return max_epi32(a); }
uint8_t avx::max(__m256i a, uint8_t) { return max_epu8(a); }
uint16_t avx::max(__m256i a, uint16_t) { return max_epu16(a); }
uint32_t avx::max(__m256i a, uint32_t) { return max_epu32(a); }
__m256i avx::min_epi8(__m256i a, __m256i b) { return _mm256_min_epi8(a, b); }
__m256i avx::min_epi16(__m256i a, __m256i b) { return _mm256_min_epi16(a, b); }
__m256i avx::min_epi32(__m256i a, __m256i b) { return _mm256_min_epi32(a, b); }
__m256i avx::min_epu8(__m256i a, __m256i b) { return _mm256_min_epu8(a, b); }
__m256i avx::min_epu16(__m256i a, __m256i b) { return _mm256_min_epu16(a, b); }
__m256i avx::min_epu32(__m256i a, __m256i b) { return _mm256_min_epu32(a, b); }
__m256i avx::min(__m256i a, __m256i b, int8_t) { return min_epi8(a, b); }
__m256i avx::min(__m256i a, __m256i b, int16_t) { return min_epi16(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint8_t) { return min_epu8(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint16_t) { return min_epu16(a, b); }
__m256i avx::min(__m256i a, __m256i b, int32_t) { return min_epi32(a, b); }
__m256i avx::min(__m256i a, __m256i b, uint32_t) { return min_epu32(a, b); }
int8_t avx::min_epi8(__m256i a) {
return sse::min_epi8(sse::min_epi8(getlow(a), gethigh(a)));
}
int16_t avx::min_epi16(__m256i a) {
return sse::min_epi16(sse::min_epi16(getlow(a), gethigh(a)));
}
int32_t avx::min_epi32(__m256i a) {
return sse::min_epi32(sse::min_epi32(getlow(a), gethigh(a)));
}
uint8_t avx::min_epu8(__m256i a) {
return sse::min_epu8(sse::min_epu8(getlow(a), gethigh(a)));
}
uint16_t avx::min_epu16(__m256i a) {
return sse::min_epu16(sse::min_epu16(getlow(a), gethigh(a)));
}
uint32_t avx::min_epu32(__m256i a) {
return sse::min_epu32(sse::min_epu32(getlow(a), gethigh(a)));
}
int8_t avx::min(__m256i a, int8_t) { return min_epi8(a); }
int16_t avx::min(__m256i a, int16_t) { return min_epi16(a); }
int32_t avx::min(__m256i a, int32_t) { return min_epi32(a); }
uint8_t avx::min(__m256i a, uint8_t) { return min_epu8(a); }
uint16_t avx::min(__m256i a, uint16_t) { return min_epu16(a); }
uint32_t avx::min(__m256i a, uint32_t) { return min_epu32(a); }
avx::mask_type avx::movemask_epi8(__m256i a) { return _mm256_movemask_epi8(a); }
__m256i avx::mul_epi32(__m256i a, __m256i b) { return _mm256_mul_epi32(a, b); }
__m256i avx::mul_epu32(__m256i a, __m256i b) { return _mm256_mul_epu32(a, b); }
__m256i avx::mulhi_epi16(__m256i a, __m256i b) { return _mm256_mulhi_epi16(a, b); }
__m256i avx::mulhi_epu16(__m256i a, __m256i b) { return _mm256_mulhi_epu16(a, b); }
__m256i avx::mullo_epi16(__m256i a, __m256i b) { return _mm256_mullo_epi16(a, b); }
__m256i avx::packs_epi16(__m256i a, __m256i b) { return _mm256_packs_epi16(a, b); }
__m256i avx::packs_epi32(__m256i a, __m256i b) { return _mm256_packs_epi32(a, b); }
__m256i avx::packus_epi16(__m256i a, __m256i b) { return _mm256_packus_epi16(a, b); }
__m256i avx::packus_epi32(__m256i a, __m256i b) { return _mm256_packus_epi32(a, b); }
template<int imm>
__m256i avx::shl(__m256i a) {
if constexpr (imm >= 64 * 3) {
a = slli<8 * 3>(a);
a = slli_epi64(a, imm - 64 * 3);
return a;
}
else if constexpr (imm >= 64 * 2) {
a = slli<8 * 2>(a);
constexpr auto I = imm - 64 * 2;
auto b = slli_epi64(a, I);
auto c = slli<8>(a);
c = srli_epi64(c, 64 - I);
return Or(b, c);
}
else if constexpr (imm >= 64) {
a = slli<8>(a);
constexpr auto I = imm - 64;
auto b = slli_epi64(a, I);
auto c = slli<8>(a);
c = srli_epi64(c, 64 - I);
return Or(b, c);
}
else {
auto b = slli_epi64(a, imm);
auto c = slli<8>(a);
c = srli_epi64(c, 64 - imm);
return Or(b, c);
}
}
template<int imm>
__m256i avx::shr(__m256i a) {
if constexpr (imm >= 64 * 3) {
a = srli<8 * 3>(a);
a = srli_epi64(a, imm - 64 * 3);
return a;
}
else if constexpr (imm >= 64 * 2) {
a = srli<8 * 2>(a);
constexpr auto I = imm - 64 * 2;
auto b = srli_epi64(a, I);
auto c = srli<8>(a);
c = slli_epi64(c, 64 - I);
return Or(b, c);
}
else if constexpr (imm >= 64) {
a = srli<8>(a);
constexpr auto I = imm - 64;
auto b = srli_epi64(a, I);
auto c = srli<8>(a);
c = slli_epi64(c, 64 - I);
return Or(b, c);
}
else {
auto b = srli_epi64(a, imm);
auto c = srli<8>(a);
c = slli_epi64(c, 64 - imm);
return Or(b, c);
}
}
__m256i avx::shuffle_epi8(__m256i a, __m256i b) { return _mm256_shuffle_epi8(a, b); }
template<int imm8>
__m256i avx::shuffle_epi32(__m256i a) {
return _mm256_shuffle_epi32(a, imm8);
}
template<int imm8>
__m256i avx::shufflehi_epi16(__m256i a) {
return _mm256_shufflehi_epi16(a, imm8);
}
template<int imm8>
__m256i avx::shufflelo_epi16(__m256i a) {
return _mm256_shufflelo_epi16(a, imm8);
}
__m256i avx::sll_epi16(__m256i a, __m128i b) { return _mm256_sll_epi16(a, b); }
__m256i avx::sll_epi32(__m256i a, __m128i b) { return _mm256_sll_epi32(a, b); }
__m256i avx::sll_epi64(__m256i a, __m128i b) { return _mm256_sll_epi64(a, b); }
__m256i avx::sll(__m256i a, __m128i b, int16_t) { return sll_epi16(a, b); }
__m256i avx::sll(__m256i a, __m128i b, int32_t) { return sll_epi32(a, b); }
__m256i avx::sll(__m256i a, __m128i b, int64_t) { return sll_epi64(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint16_t) { return sll_epi16(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint32_t) { return sll_epi32(a, b); }
__m256i avx::sll(__m256i a, __m128i b, uint64_t) { return sll_epi64(a, b); }
template<int imm8>
__m256i avx::slli(__m256i a) {
return _mm256_slli_si256(a, imm8);
}
__m256i avx::slli_epi16(__m256i a, int imm8) { return _mm256_slli_epi16(a, imm8); }
__m256i avx::slli_epi32(__m256i a, int imm8) { return _mm256_slli_epi32(a, imm8); }
__m256i avx::slli_epi64(__m256i a, int imm8) { return _mm256_slli_epi64(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, int16_t) { return slli_epi16(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, int32_t) { return slli_epi32(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, int64_t) { return slli_epi64(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint16_t) { return slli_epi16(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint32_t) { return slli_epi32(a, imm8); }
__m256i avx::slli(__m256i a, int imm8, uint64_t) { return slli_epi64(a, imm8); }
__m256i avx::sra_epi16(__m256i a, __m128i b) { return _mm256_sra_epi16(a, b); }
__m256i avx::sra_epi32(__m256i a, __m128i b) { return _mm256_sra_epi32(a, b); }
__m256i avx::sra(__m256i a, __m128i b, int16_t) { return sra_epi16(a, b); }
__m256i avx::sra(__m256i a, __m128i b, int32_t) { return sra_epi32(a, b); }
__m256i avx::srai_epi16(__m256i a, int imm8) { return _mm256_srai_epi16(a, imm8); }
__m256i avx::srai_epi32(__m256i a, int imm8) { return _mm256_srai_epi32(a, imm8); }
__m256i avx::srai(__m256i a, int imm8, int16_t) { return srai_epi16(a, imm8); }
__m256i avx::srai(__m256i a, int imm8, int32_t) { return srai_epi32(a, imm8); }
__m256i avx::stream_load(__m256i const* p) { return _mm256_stream_load_si256(p); }
__m256i avx::srl_epi16(__m256i a, __m128i b) { return _mm256_srl_epi16(a, b); }
__m256i avx::srl_epi32(__m256i a, __m128i b) { return _mm256_srl_epi32(a, b); }
__m256i avx::srl_epi64(__m256i a, __m128i b) { return _mm256_srl_epi64(a, b); }
__m256i avx::srl(__m256i a, __m128i b, int16_t) { return srl_epi16(a, b); }
__m256i avx::srl(__m256i a, __m128i b, int32_t) { return srl_epi32(a, b); }
__m256i avx::srl(__m256i a, __m128i b, int64_t) { return srl_epi64(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint16_t) { return srl_epi16(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint32_t) { return srl_epi32(a, b); }
__m256i avx::srl(__m256i a, __m128i b, uint64_t) { return srl_epi64(a, b); }
template<int imm8>
__m256i avx::srli(__m256i a) {
return _mm256_srli_si256(a, imm8);
}
__m256i avx::srli_epi16(__m256i a, int imm8) { return _mm256_srli_epi16(a, imm8); }
__m256i avx::srli_epi32(__m256i a, int imm8) { return _mm256_srli_epi32(a, imm8); }
__m256i avx::srli_epi64(__m256i a, int imm8) { return _mm256_srli_epi64(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, int16_t) { return srli_epi16(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, int32_t) { return srli_epi32(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, int64_t) { return srli_epi64(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint16_t) { return srli_epi16(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint32_t) { return srli_epi32(a, imm8); }
__m256i avx::srli(__m256i a, int imm8, uint64_t) { return srli_epi64(a, imm8); }
__m256i avx::sub_epi8(__m256i a, __m256i b) { return _mm256_sub_epi8(a, b); }
__m256i avx::sub_epi16(__m256i a, __m256i b) { return _mm256_sub_epi16(a, b); }
__m256i avx::sub_epi32(__m256i a, __m256i b) { return _mm256_sub_epi32(a, b); }
__m256i avx::sub_epi64(__m256i a, __m256i b) { return _mm256_sub_epi64(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int8_t) { return sub_epi8(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int16_t) { return sub_epi16(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int32_t) { return sub_epi32(a, b); }
__m256i avx::sub(__m256i a, __m256i b, int64_t) { return sub_epi64(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint8_t) { return sub_epi8(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint16_t) { return sub_epi16(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint32_t) { return sub_epi32(a, b); }
__m256i avx::sub(__m256i a, __m256i b, uint64_t) { return sub_epi64(a, b); }
__m256i avx::subs_epi8(__m256i a, __m256i b) { return _mm256_subs_epi8(a, b); }
__m256i avx::subs_epi16(__m256i a, __m256i b) { return _mm256_subs_epi16(a, b); }
__m256i avx::subs_epu8(__m256i a, __m256i b) { return _mm256_subs_epu8(a, b); }
__m256i avx::subs_epu16(__m256i a, __m256i b) { return _mm256_subs_epu16(a, b); }
__m256i avx::subs(__m256i a, __m256i b, int8_t) { return subs_epi8(a, b); }
__m256i avx::subs(__m256i a, __m256i b, int16_t) { return subs_epi16(a, b); }
__m256i avx::subs(__m256i a, __m256i b, uint8_t) { return subs_epu8(a, b); }
__m256i avx::subs(__m256i a, __m256i b, uint16_t) { return subs_epu16(a, b); }
int avx::test_all_ones(__m256i a) { return testc(a, cmpeq_epi32(a, a)); }
__m256i avx::unpackhi_epi8(__m256i a, __m256i b) { return _mm256_unpackhi_epi8(a, b); }
__m256i avx::unpackhi_epi16(__m256i a, __m256i b) { return _mm256_unpackhi_epi16(a, b); }
__m256i avx::unpackhi_epi32(__m256i a, __m256i b) { return _mm256_unpackhi_epi32(a, b); }
__m256i avx::unpackhi_epi64(__m256i a, __m256i b) { return _mm256_unpackhi_epi64(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int8_t) { return unpackhi_epi8(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int16_t) { return unpackhi_epi16(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int32_t) { return unpackhi_epi32(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, int64_t) { return unpackhi_epi64(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint8_t) { return unpackhi_epi8(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint16_t) { return unpackhi_epi16(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint32_t) { return unpackhi_epi32(a, b); }
__m256i avx::unpackhi(__m256i a, __m256i b, uint64_t) { return unpackhi_epi64(a, b); }
__m256i avx::unpacklo_epi8(__m256i a, __m256i b) { return _mm256_unpacklo_epi8(a, b); }
__m256i avx::unpacklo_epi16(__m256i a, __m256i b) { return _mm256_unpacklo_epi16(a, b); }
__m256i avx::unpacklo_epi32(__m256i a, __m256i b) { return _mm256_unpacklo_epi32(a, b); }
__m256i avx::unpacklo_epi64(__m256i a, __m256i b) { return _mm256_unpacklo_epi64(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int8_t) { return unpacklo_epi8(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int16_t) { return unpacklo_epi16(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int32_t) { return unpacklo_epi32(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, int64_t) { return unpacklo_epi64(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint8_t) { return unpacklo_epi8(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint16_t) { return unpacklo_epi16(a, b); }
__m256i avx::unpacklo(__m256i a, __m256i b, uint32_t) { return unpacklo_epi32(a, b); }
#endif // WJR_AVX2
_WJR_SIMD_END

#endif // __WJR_SIMD_SIMD_INTRIN_H

#ifndef __WJR_ALGO_MEM_ALL_H
#error "This file should not be included directly. Include <wjr/algo/mem-all.h> instead."
#endif // __WJR_ALGO_MEM_ALL_H
#if defined(_WJR_FAST_MEMCHR)
#define __WJR_MEMCHR_ONE(st, q, ptr)												\
{	                                                                            \
auto r = st::cmp(x, q, pred, T());	                                        \
st::mask_type z = st::movemask_epi8(r);	                                    \
if(WJR_LIKELY(z != 0)) {	                                                    \
return WJR_SIMD_FIRST_ONE_PTR(ptr, z);	                                \
}	                                                                        \
}
#define __WJR_MEMCHR_FOUR(st, q, ptr0, ptr1, ptr2, ptr3)							\
{	                                                                            \
auto r0 = st::cmp(x0, q, pred, T());	                                    \
auto r1 = st::cmp(x1, q, pred, T());	                                    \
auto r2 = st::cmp(x2, q, pred, T());	                                    \
auto r3 = st::cmp(x3, q, pred, T());	                                    \
\
r3 = st::Or(st::Or(r0, r1), st::Or(r2, r3));	                            \
st::mask_type z = st::movemask_epi8(r3);	                                \
if(z != 0){	                                                                \
st::mask_type tmp = st::movemask_epi8(r0);	                            \
if(tmp != 0){	                                                        \
return WJR_SIMD_FIRST_ONE_PTR(ptr0, tmp);	                        \
}	                                                                    \
tmp = st::movemask_epi8(r1);	                                        \
if(tmp != 0){	                                                        \
return WJR_SIMD_FIRST_ONE_PTR(ptr1, tmp);	                        \
}	                                                                    \
tmp = st::movemask_epi8(r2);	                                        \
if(tmp != 0){	                                                        \
return WJR_SIMD_FIRST_ONE_PTR(ptr2, tmp);	                        \
}	                                                                    \
tmp = z;	                                                            \
return WJR_SIMD_FIRST_ONE_PTR(ptr3, tmp);	                            \
}	                                                                        \
}
#define WJR_SIMD_IS_BACKWARD 0
#define __WJR_MEMCHR_NAME __memchr
_WJR_ALGO_BEGIN
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) const T*
WJR_MACRO_CONCAT(__large, __WJR_MEMCHR_NAME)(const T* s, T val, size_t n, _Pred pred) noexcept {
constexpr size_t _Mysize = sizeof(T);
#if WJR_AVX2
using simd_t = simd::avx;
#else
using simd_t = simd::sse;
#endif // WJR_AVX2
constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
constexpr uintptr_t bound = width * _Mysize;
auto q = simd_t::set1(val, T());
{
const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
const auto negdelta = m - 32 / _Mysize;
#if WJR_AVX2
static_assert(std::is_same_v<simd_t, simd::avx>, "");
{
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s, delta));
__WJR_MEMCHR_ONE(simd::avx, q, WJR_SIMD_ADD_PTR(s, delta));
}
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s, negdelta - delta));
__WJR_MEMCHR_ONE(simd::avx, q, WJR_SIMD_ADD_PTR(s, negdelta - delta));
}
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s, negdelta));
__WJR_MEMCHR_ONE(simd::avx, q, WJR_SIMD_ADD_PTR(s, negdelta));
}
}
#else
static_assert(std::is_same_v<simd_t, simd::sse>, "");
{
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s, delta));
__WJR_MEMCHR_ONE(simd::sse, q, WJR_SIMD_ADD_PTR(s, delta));
}
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s, delta + 16 / _Mysize));
__WJR_MEMCHR_ONE(simd::sse, q, WJR_SIMD_ADD_PTR(s, delta + 16 / _Mysize));
}
}
{
WJR_SIMD_LOADU4(simd::sse, x,
WJR_SIMD_ADD_PTR(s, negdelta - delta),
WJR_SIMD_ADD_PTR(s, negdelta - delta + 16 / _Mysize),
WJR_SIMD_ADD_PTR(s, negdelta),
WJR_SIMD_ADD_PTR(s, negdelta + 16 / _Mysize));
__WJR_MEMCHR_FOUR(simd::sse, q,
WJR_SIMD_ADD_PTR(s, negdelta - delta),
WJR_SIMD_ADD_PTR(s, negdelta - delta + 16 / _Mysize),
WJR_SIMD_ADD_PTR(s, negdelta),
WJR_SIMD_ADD_PTR(s, negdelta + 16 / _Mysize));
}
#endif // WJR_AVX2
if (m == n) {
return WJR_SIMD_ADD_PTR(s, n);
}
}
if (n <= 128 / _Mysize + width * 4) {
WJR_MACRO_LABEL(unaligned_last_4vec) :
WJR_SIMD_INC_PTR(s, n);
WJR_SIMD_LOADU4(simd_t, x,
WJR_SIMD_SUB_PTR(s, width * 4),
WJR_SIMD_SUB_PTR(s, width * 3),
WJR_SIMD_SUB_PTR(s, width * 2),
WJR_SIMD_SUB_PTR(s, width));
__WJR_MEMCHR_FOUR(simd_t, q,
WJR_SIMD_SUB_PTR(s, width * 4),
WJR_SIMD_SUB_PTR(s, width * 3),
WJR_SIMD_SUB_PTR(s, width * 2),
WJR_SIMD_SUB_PTR(s, width));
return s;
}
WJR_SIMD_INC_PTR(s, 128 / _Mysize);
n -= 128 / _Mysize;
if (WJR_LIKELY(WJR_SIMD_UINTPTR(s) % _Mysize == 0)) {
const auto __align_s = WJR_SIMD_CEIL_ALIGN_OFFSET(s, bound);
if (is_constant_p(__align_s) && __align_s == 0) {
}
else {
WJR_SIMD_LOADU(simd_t, x, s);
__WJR_MEMCHR_ONE(simd_t, q, s);
WJR_SIMD_INC_PTR(s, __align_s / _Mysize);
n -= __align_s / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align);
}
}
do {
WJR_SIMD_LOAD4(simd_t, x,
s,
WJR_SIMD_ADD_PTR(s, width),
WJR_SIMD_ADD_PTR(s, width * 2),
WJR_SIMD_ADD_PTR(s, width * 3));
__WJR_MEMCHR_FOUR(simd_t, q,
s,
WJR_SIMD_ADD_PTR(s, width),
WJR_SIMD_ADD_PTR(s, width * 2),
WJR_SIMD_ADD_PTR(s, width * 3));
WJR_SIMD_INC_PTR(s, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align) :
WJR_SIMD_INC_PTR(s, n);
auto ptr0 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s, bound), width * 4);
WJR_SIMD_LOAD(simd_t, x0, ptr0);
WJR_SIMD_LOAD(simd_t, x1, WJR_SIMD_ADD_PTR(ptr0, width));
WJR_SIMD_LOAD(simd_t, x2, WJR_SIMD_ADD_PTR(ptr0, width * 2));
WJR_SIMD_LOADU(simd_t, x3, WJR_SIMD_SUB_PTR(s, width));
__WJR_MEMCHR_FOUR(simd_t, q,
ptr0,
WJR_SIMD_ADD_PTR(ptr0, width),
WJR_SIMD_ADD_PTR(ptr0, width * 2),
WJR_SIMD_SUB_PTR(s, width));
}
return s;
}
do {
WJR_SIMD_LOADU4(simd_t, x,
s,
WJR_SIMD_ADD_PTR(s, width),
WJR_SIMD_ADD_PTR(s, width * 2),
WJR_SIMD_ADD_PTR(s, width * 3));
__WJR_MEMCHR_FOUR(simd_t, q,
s,
WJR_SIMD_ADD_PTR(s, width),
WJR_SIMD_ADD_PTR(s, width * 2),
WJR_SIMD_ADD_PTR(s, width * 3));
WJR_SIMD_INC_PTR(s, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
goto WJR_MACRO_LABEL(unaligned_last_4vec);
}
return s;
}
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) const T* WJR_MACRO_CONCAT(__small, __WJR_MEMCHR_NAME)(
const T* s, T val, size_t n, _Pred pred) noexcept {
constexpr auto _Mysize = sizeof(T);
#if !WJR_SIMD_IS_BACKWARD
if constexpr (_Mysize == 8) {
return pred(*s, val) ? s : s + 1;
}
if constexpr (_Mysize == 2) {
if (n >= 4) {
auto A = *reinterpret_cast<const uint64_t*>(s);
auto B = *reinterpret_cast<const uint64_t*>(s + (n - 4u));
auto x = simd::sse::set_epi64x(B, A);
auto y = simd::sse::set1(val, T());
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == 0)return s + n;
auto i = wjr::countr_zero(z) / _Mysize;
auto q = i >= 4 ? i + n - 8 : i;
return s + q;
}
}
if constexpr (_Mysize == 1) {
if (n >= 4) {
auto delta = (n & 8) >> 1;
auto A = *reinterpret_cast<const uint32_t*>(s);
auto B = *reinterpret_cast<const uint32_t*>(s + delta);
auto C = *reinterpret_cast<const uint32_t*>(s + (n - 4u - delta));
auto D = *reinterpret_cast<const uint32_t*>(s + (n - 4u));
auto x = simd::sse::set_epi32(D, C, B, A);
auto y = simd::sse::set1(val, T());
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == 0) return s + n;
auto i = wjr::countr_zero(z);
auto q = i >= 8 ? i + (n - 12 - delta) : i;
return s + q;
}
}
if constexpr (_Mysize <= 4) {
if (pred(s[0], val)) return s;
if (n == 1) return s + 1;
const bool f = pred(s[n - 2], val);
const bool g = pred(s[n - 1], val);
const size_t i1 = g ? n - 1 : n;
const size_t i2 = f ? n - 2 : i1;
return s + i2;
}
#else
if constexpr (_Mysize == 8) {
return pred(s[-1], val) ? s : s - 1;
}
if constexpr (_Mysize == 2) {
if (n >= 4) {
auto A0 = *reinterpret_cast<const uint64_t*>(s - n);
auto B0 = *reinterpret_cast<const uint64_t*>(s - 4);
auto x = simd::sse::set_epi64x(B0, A0);
auto y = simd::sse::set1(val, T());
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == 0)return s - n;
auto i = wjr::countl_zero(z) / _Mysize;
auto q = i >= 4 ? i + (n - 8) : i;
return s - q;
}
}
if constexpr (_Mysize == 1) {
if (n >= 4) {
auto delta = (n & 8) >> 1;
auto A0 = *reinterpret_cast<const uint32_t*>(s - n);
auto B0 = *reinterpret_cast<const uint32_t*>(s - n + delta);
auto C0 = *reinterpret_cast<const uint32_t*>(s - 4 - delta);
auto D0 = *reinterpret_cast<const uint32_t*>(s - 4);
auto x = simd::sse::set_epi32(D0, C0, B0, A0);
auto y = simd::sse::set1(val, T());
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == 0) return s - n;
auto i = wjr::countl_zero(z);
auto q = i >= 8 ? i + (n - 12 - delta) : i;
return s - q;
}
}
if constexpr (_Mysize <= 4) {
if (pred(s[-1], val)) return s;
if (n == 1) return s - 1;
const bool f = pred(s[static_cast<size_t>(1 - n)], val);
const bool g = pred(s[static_cast<size_t>(0 - n)], val);
const size_t i1 = g ? 1 - n : -n;
const size_t i2 = f ? 2 - n : i1;
return s + i2;
}
#endif //
}
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* (__WJR_MEMCHR_NAME)(
const T* s, T val, size_t n, _Pred pred) noexcept {
constexpr size_t _Mysize = sizeof(T);
if (WJR_UNLIKELY(n == 0)) return s;
WJR_SIMD_INIT_PTR(s, n);
if (n >= 16 / _Mysize) {
auto qx = simd::sse::set1(val, T());
{
WJR_SIMD_LOADU(simd::sse, x, s);
__WJR_MEMCHR_ONE(simd::sse, qx, s);
}
auto m = n <= 32 / _Mysize ? (n - 16 / _Mysize) : (16 / _Mysize);
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s, m));
__WJR_MEMCHR_ONE(simd::sse, qx, WJR_SIMD_ADD_PTR(s, m));
}
if (n <= 32 / _Mysize) {
return WJR_SIMD_ADD_PTR(s, n);
}
return WJR_MACRO_CONCAT(__large, __WJR_MEMCHR_NAME)(s, val, n, pred);
}
return WJR_MACRO_CONCAT(__small, __WJR_MEMCHR_NAME)(s, val, n, pred);
}
_WJR_ALGO_END

#undef __WJR_MEMCHR_NAME
#undef WJR_SIMD_IS_BACKWARD
#define WJR_SIMD_IS_BACKWARD 1
#define __WJR_MEMCHR_NAME __memrchr
_WJR_ALGO_BEGIN
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) const T*
WJR_MACRO_CONCAT(__large, __WJR_MEMCHR_NAME)(const T* s, T val, size_t n, _Pred pred) noexcept {
constexpr size_t _Mysize = sizeof(T);
#if WJR_AVX2
using simd_t = simd::avx;
#else
using simd_t = simd::sse;
#endif // WJR_AVX2
constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
constexpr uintptr_t bound = width * _Mysize;
auto q = simd_t::set1(val, T());
{
const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
const auto negdelta = m - 32 / _Mysize;
#if WJR_AVX2
static_assert(std::is_same_v<simd_t, simd::avx>, "");
{
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s, delta));
__WJR_MEMCHR_ONE(simd::avx, q, WJR_SIMD_ADD_PTR(s, delta));
}
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s, negdelta - delta));
__WJR_MEMCHR_ONE(simd::avx, q, WJR_SIMD_ADD_PTR(s, negdelta - delta));
}
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s, negdelta));
__WJR_MEMCHR_ONE(simd::avx, q, WJR_SIMD_ADD_PTR(s, negdelta));
}
}
#else
static_assert(std::is_same_v<simd_t, simd::sse>, "");
{
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s, delta));
__WJR_MEMCHR_ONE(simd::sse, q, WJR_SIMD_ADD_PTR(s, delta));
}
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s, delta + 16 / _Mysize));
__WJR_MEMCHR_ONE(simd::sse, q, WJR_SIMD_ADD_PTR(s, delta + 16 / _Mysize));
}
}
{
WJR_SIMD_LOADU4(simd::sse, x,
WJR_SIMD_ADD_PTR(s, negdelta - delta),
WJR_SIMD_ADD_PTR(s, negdelta - delta + 16 / _Mysize),
WJR_SIMD_ADD_PTR(s, negdelta),
WJR_SIMD_ADD_PTR(s, negdelta + 16 / _Mysize));
__WJR_MEMCHR_FOUR(simd::sse, q,
WJR_SIMD_ADD_PTR(s, negdelta - delta),
WJR_SIMD_ADD_PTR(s, negdelta - delta + 16 / _Mysize),
WJR_SIMD_ADD_PTR(s, negdelta),
WJR_SIMD_ADD_PTR(s, negdelta + 16 / _Mysize));
}
#endif // WJR_AVX2
if (m == n) {
return WJR_SIMD_ADD_PTR(s, n);
}
}
if (n <= 128 / _Mysize + width * 4) {
WJR_MACRO_LABEL(unaligned_last_4vec) :
WJR_SIMD_INC_PTR(s, n);
WJR_SIMD_LOADU4(simd_t, x,
WJR_SIMD_SUB_PTR(s, width * 4),
WJR_SIMD_SUB_PTR(s, width * 3),
WJR_SIMD_SUB_PTR(s, width * 2),
WJR_SIMD_SUB_PTR(s, width));
__WJR_MEMCHR_FOUR(simd_t, q,
WJR_SIMD_SUB_PTR(s, width * 4),
WJR_SIMD_SUB_PTR(s, width * 3),
WJR_SIMD_SUB_PTR(s, width * 2),
WJR_SIMD_SUB_PTR(s, width));
return s;
}
WJR_SIMD_INC_PTR(s, 128 / _Mysize);
n -= 128 / _Mysize;
if (WJR_LIKELY(WJR_SIMD_UINTPTR(s) % _Mysize == 0)) {
const auto __align_s = WJR_SIMD_CEIL_ALIGN_OFFSET(s, bound);
if (is_constant_p(__align_s) && __align_s == 0) {
}
else {
WJR_SIMD_LOADU(simd_t, x, s);
__WJR_MEMCHR_ONE(simd_t, q, s);
WJR_SIMD_INC_PTR(s, __align_s / _Mysize);
n -= __align_s / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align);
}
}
do {
WJR_SIMD_LOAD4(simd_t, x,
s,
WJR_SIMD_ADD_PTR(s, width),
WJR_SIMD_ADD_PTR(s, width * 2),
WJR_SIMD_ADD_PTR(s, width * 3));
__WJR_MEMCHR_FOUR(simd_t, q,
s,
WJR_SIMD_ADD_PTR(s, width),
WJR_SIMD_ADD_PTR(s, width * 2),
WJR_SIMD_ADD_PTR(s, width * 3));
WJR_SIMD_INC_PTR(s, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align) :
WJR_SIMD_INC_PTR(s, n);
auto ptr0 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s, bound), width * 4);
WJR_SIMD_LOAD(simd_t, x0, ptr0);
WJR_SIMD_LOAD(simd_t, x1, WJR_SIMD_ADD_PTR(ptr0, width));
WJR_SIMD_LOAD(simd_t, x2, WJR_SIMD_ADD_PTR(ptr0, width * 2));
WJR_SIMD_LOADU(simd_t, x3, WJR_SIMD_SUB_PTR(s, width));
__WJR_MEMCHR_FOUR(simd_t, q,
ptr0,
WJR_SIMD_ADD_PTR(ptr0, width),
WJR_SIMD_ADD_PTR(ptr0, width * 2),
WJR_SIMD_SUB_PTR(s, width));
}
return s;
}
do {
WJR_SIMD_LOADU4(simd_t, x,
s,
WJR_SIMD_ADD_PTR(s, width),
WJR_SIMD_ADD_PTR(s, width * 2),
WJR_SIMD_ADD_PTR(s, width * 3));
__WJR_MEMCHR_FOUR(simd_t, q,
s,
WJR_SIMD_ADD_PTR(s, width),
WJR_SIMD_ADD_PTR(s, width * 2),
WJR_SIMD_ADD_PTR(s, width * 3));
WJR_SIMD_INC_PTR(s, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
goto WJR_MACRO_LABEL(unaligned_last_4vec);
}
return s;
}
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) const T* WJR_MACRO_CONCAT(__small, __WJR_MEMCHR_NAME)(
const T* s, T val, size_t n, _Pred pred) noexcept {
constexpr auto _Mysize = sizeof(T);
#if !WJR_SIMD_IS_BACKWARD
if constexpr (_Mysize == 8) {
return pred(*s, val) ? s : s + 1;
}
if constexpr (_Mysize == 2) {
if (n >= 4) {
auto A = *reinterpret_cast<const uint64_t*>(s);
auto B = *reinterpret_cast<const uint64_t*>(s + (n - 4u));
auto x = simd::sse::set_epi64x(B, A);
auto y = simd::sse::set1(val, T());
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == 0)return s + n;
auto i = wjr::countr_zero(z) / _Mysize;
auto q = i >= 4 ? i + n - 8 : i;
return s + q;
}
}
if constexpr (_Mysize == 1) {
if (n >= 4) {
auto delta = (n & 8) >> 1;
auto A = *reinterpret_cast<const uint32_t*>(s);
auto B = *reinterpret_cast<const uint32_t*>(s + delta);
auto C = *reinterpret_cast<const uint32_t*>(s + (n - 4u - delta));
auto D = *reinterpret_cast<const uint32_t*>(s + (n - 4u));
auto x = simd::sse::set_epi32(D, C, B, A);
auto y = simd::sse::set1(val, T());
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == 0) return s + n;
auto i = wjr::countr_zero(z);
auto q = i >= 8 ? i + (n - 12 - delta) : i;
return s + q;
}
}
if constexpr (_Mysize <= 4) {
if (pred(s[0], val)) return s;
if (n == 1) return s + 1;
const bool f = pred(s[n - 2], val);
const bool g = pred(s[n - 1], val);
const size_t i1 = g ? n - 1 : n;
const size_t i2 = f ? n - 2 : i1;
return s + i2;
}
#else
if constexpr (_Mysize == 8) {
return pred(s[-1], val) ? s : s - 1;
}
if constexpr (_Mysize == 2) {
if (n >= 4) {
auto A0 = *reinterpret_cast<const uint64_t*>(s - n);
auto B0 = *reinterpret_cast<const uint64_t*>(s - 4);
auto x = simd::sse::set_epi64x(B0, A0);
auto y = simd::sse::set1(val, T());
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == 0)return s - n;
auto i = wjr::countl_zero(z) / _Mysize;
auto q = i >= 4 ? i + (n - 8) : i;
return s - q;
}
}
if constexpr (_Mysize == 1) {
if (n >= 4) {
auto delta = (n & 8) >> 1;
auto A0 = *reinterpret_cast<const uint32_t*>(s - n);
auto B0 = *reinterpret_cast<const uint32_t*>(s - n + delta);
auto C0 = *reinterpret_cast<const uint32_t*>(s - 4 - delta);
auto D0 = *reinterpret_cast<const uint32_t*>(s - 4);
auto x = simd::sse::set_epi32(D0, C0, B0, A0);
auto y = simd::sse::set1(val, T());
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == 0) return s - n;
auto i = wjr::countl_zero(z);
auto q = i >= 8 ? i + (n - 12 - delta) : i;
return s - q;
}
}
if constexpr (_Mysize <= 4) {
if (pred(s[-1], val)) return s;
if (n == 1) return s - 1;
const bool f = pred(s[static_cast<size_t>(1 - n)], val);
const bool g = pred(s[static_cast<size_t>(0 - n)], val);
const size_t i1 = g ? 1 - n : -n;
const size_t i2 = f ? 2 - n : i1;
return s + i2;
}
#endif //
}
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* (__WJR_MEMCHR_NAME)(
const T* s, T val, size_t n, _Pred pred) noexcept {
constexpr size_t _Mysize = sizeof(T);
if (WJR_UNLIKELY(n == 0)) return s;
WJR_SIMD_INIT_PTR(s, n);
if (n >= 16 / _Mysize) {
auto qx = simd::sse::set1(val, T());
{
WJR_SIMD_LOADU(simd::sse, x, s);
__WJR_MEMCHR_ONE(simd::sse, qx, s);
}
auto m = n <= 32 / _Mysize ? (n - 16 / _Mysize) : (16 / _Mysize);
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s, m));
__WJR_MEMCHR_ONE(simd::sse, qx, WJR_SIMD_ADD_PTR(s, m));
}
if (n <= 32 / _Mysize) {
return WJR_SIMD_ADD_PTR(s, n);
}
return WJR_MACRO_CONCAT(__large, __WJR_MEMCHR_NAME)(s, val, n, pred);
}
return WJR_MACRO_CONCAT(__small, __WJR_MEMCHR_NAME)(s, val, n, pred);
}
_WJR_ALGO_END

#undef __WJR_MEMCHR_NAME
#undef WJR_SIMD_IS_BACKWARD
_WJR_ALGO_BEGIN
extern template const uint8_t* __large__memchr(
const uint8_t*, uint8_t, size_t, std::equal_to<>) noexcept;
extern template const uint8_t* __large__memrchr(
const uint8_t*, uint8_t, size_t, std::equal_to<>) noexcept;
extern template const uint8_t* __large__memchr(
const uint8_t*, uint8_t, size_t, std::not_equal_to<>) noexcept;
extern template const uint8_t* __large__memrchr(
const uint8_t*, uint8_t, size_t, std::not_equal_to<>) noexcept;
_WJR_ALGO_END
#undef __WJR_MEMCHR_ONE
#undef __WJR_MEMCHR_FOUR
#endif // _WJR_FAST_MEMCHR

#ifndef __WJR_ALGO_MEM_ALL_H
#error "This file should not be included directly. Include <wjr/algo/mem-all.h> instead."
#endif // __WJR_ALGO_MEM_ALL_H
#if defined(_WJR_FAST_MEMEQ)
#define __WJR_MEMEQ_ONE_NORMAL(st)														    \
{	                                                                                    \
auto r = st::cmp(x, y, pred, T());	                                                \
st::mask_type z = st::movemask_epi8(r);		                                        \
if(z != st::mask()){	                                                            \
return false;	                                                                \
}	                                                                                \
}
#define __WJR_MEMEQ_FOUR_NORMAL(st)													    \
{	                                                                                    \
auto r0 = st::cmp(x0, y0, pred, T());	                                            \
auto r1 = st::cmp(x1, y1, pred, T());	                                            \
auto r2 = st::cmp(x2, y2, pred, T());	                                            \
auto r3 = st::cmp(x3, y3, pred, T());	                                            \
\
r3 = st::And(st::And(r0, r1), st::And(r2, r3));	                                    \
st::mask_type z = st::movemask_epi8(r3);		                                    \
if(z != st::mask()){	                                                            \
return false;	                                                                \
}	                                                                                \
}
#define __WJR_MEMEQ_ONE(st) __WJR_MEMEQ_ONE_NORMAL(st)
#define __WJR_MEMEQ_FOUR(st) __WJR_MEMEQ_FOUR_NORMAL(st)
#if WJR_SSE4_1
#undef __WJR_MEMEQ_ONE
#undef __WJR_MEMEQ_FOUR
#define __WJR_MEMEQ_ONE(st)															\
if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                            \
auto r = st::Xor(x, y);	                                                        \
auto z = st::test_all_zeros(r);	                                                \
if(!z){	                                                                        \
return false;	                                                            \
}	                                                                            \
}	                                                                                \
else{	                                                                            \
__WJR_MEMEQ_ONE_NORMAL(st)														\
}
#define __WJR_MEMEQ_FOUR(st)															\
if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                            \
auto r0 = st::Xor(x0, y0);	                                                    \
auto r1 = st::Xor(x1, y1);	                                                    \
auto r2 = st::Xor(x2, y2);	                                                    \
auto r3 = st::Xor(x3, y3);	                                                    \
\
r3 = st::Or(st::Or(r0, r1), st::Or(r2, r3));	                                \
auto z = st::test_all_zeros(r3);	                                            \
if(!z){	                                                                        \
return false;	                                                            \
}	                                                                            \
}	                                                                                \
else{	                                                                            \
__WJR_MEMEQ_FOUR_NORMAL(st)													\
}
#endif // WJR_SSE4_1
_WJR_ALGO_BEGIN
#define WJR_SIMD_IS_BACKWARD 0
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) bool __large__memeq(const T * s0, const T * s1, size_t n, _Pred pred) noexcept {
constexpr size_t _Mysize = sizeof(T);
#if WJR_AVX2
using simd_t = simd::avx;
#else
using simd_t = simd::sse;
#endif // WJR_AVX2
using sint = typename simd_t::int_type;
constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
constexpr uintptr_t bound = width * _Mysize;
{
const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
const auto negdelta = m - 32 / _Mysize;
#if WJR_AVX2
{
WJR_SIMD_LOADU(simd::avx, x, s0 + delta);
WJR_SIMD_LOADU(simd::avx, y, s1 + delta);
__WJR_MEMEQ_ONE(simd::avx);
}
{
WJR_SIMD_LOADU(simd::avx, x, s0 + negdelta - delta);
WJR_SIMD_LOADU(simd::avx, y, s1 + negdelta - delta);
__WJR_MEMEQ_ONE(simd::avx);
}
{
WJR_SIMD_LOADU(simd::avx, x, s0 + negdelta);
WJR_SIMD_LOADU(simd::avx, y, s1 + negdelta);
__WJR_MEMEQ_ONE(simd::avx);
}
#else
{
WJR_SIMD_LOADU(simd::sse, x, s0 + delta);
WJR_SIMD_LOADU(simd::sse, y, s1 + delta);
__WJR_MEMEQ_ONE(simd::sse);
}
{
WJR_SIMD_LOADU(simd::sse, x, s0 + delta + 16 / _Mysize);
WJR_SIMD_LOADU(simd::sse, y, s1 + delta + 16 / _Mysize);
__WJR_MEMEQ_ONE(simd::sse);
}
{
WJR_SIMD_LOADU4(simd::sse, x,
s0 + negdelta - delta,
s0 + negdelta - delta + 16 / _Mysize,
s0 + negdelta,
s0 + negdelta + 16 / _Mysize);
WJR_SIMD_LOADU4(simd::sse, y,
s1 + negdelta - delta,
s1 + negdelta - delta + 16 / _Mysize,
s1 + negdelta,
s1 + negdelta + 16 / _Mysize);
__WJR_MEMEQ_FOUR(simd::sse);
}
#endif // WJR_AVX2
if (m == n) {
return true;
}
}
if (n <= 128 / _Mysize + width * 4) {
WJR_MACRO_LABEL(unaligned_last_4vec) :
s0 += n;
s1 += n;
WJR_SIMD_LOADU4(simd_t, x,
s0 - width * 4,
s0 - width * 3,
s0 - width * 2,
s0 - width);
WJR_SIMD_LOADU4(simd_t, y,
s1 - width * 4,
s1 - width * 3,
s1 - width * 2,
s1 - width);
__WJR_MEMEQ_FOUR(simd_t);
return true;
}
s0 += 128 / _Mysize;
s1 += 128 / _Mysize;
n -= 128 / _Mysize;
if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s0) % _Mysize == 0)) {
const auto off0 = reinterpret_cast<uintptr_t>(s0) % bound;
const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;
if (off0 != off1) {
if (is_constant_p(off0) && off0 == 0) {
}
else {
WJR_SIMD_LOADU(simd_t, x, s0);
WJR_SIMD_LOADU(simd_t, y, s1);
__WJR_MEMEQ_ONE(simd_t);
const auto __align_s = bound - off0;
s0 += __align_s / _Mysize;
s1 += __align_s / _Mysize;
n -= __align_s / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0);
}
}
do {
WJR_SIMD_LOAD4(simd_t, x, s0, s0 + width, s0 + width * 2, s0 + width * 3);
WJR_SIMD_LOADU4(simd_t, y, s1, s1 + width, s1 + width * 2, s1 + width * 3);
__WJR_MEMEQ_FOUR(simd_t);
s0 += width * 4;
s1 += width * 4;
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align_0) :
s0 += n;
s1 += n;
const auto ptr0 = reinterpret_cast<T*>(
(reinterpret_cast<uintptr_t>(s0 - width * 3)) & (~(bound - 1)));
const auto dif = s0 - ptr0;
const auto ptr1 = s1 - dif;
auto x0 = simd_t::load(reinterpret_cast<const sint*>(ptr0));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(ptr0 + width));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(ptr0 + width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
WJR_SIMD_LOADU4(simd_t, y, ptr1, ptr1 + width, ptr1 + width * 2, s1 - width);
__WJR_MEMEQ_FOUR(simd_t);
}
return true;
}
if (is_constant_p(off0) && off0 == 0) {
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));
__WJR_MEMEQ_ONE(simd_t);
const auto __align_s = bound - off0;
s0 += __align_s / _Mysize;
s1 += __align_s / _Mysize;
n -= __align_s / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0_1);
}
}
do {
WJR_SIMD_LOAD4(simd_t, x, s0, s0 + width, s0 + width * 2, s0 + width * 3);
WJR_SIMD_LOAD4(simd_t, y, s1, s1 + width, s1 + width * 2, s1 + width * 3);
__WJR_MEMEQ_FOUR(simd_t);
s0 += width * 4;
s1 += width * 4;
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align_0_1) :
s0 += n;
s1 += n;
auto ptr0 = reinterpret_cast<T*>(
(reinterpret_cast<uintptr_t>(s0 - width * 3)) & (~(bound - 1)));
const auto dif = s0 - ptr0;
const auto ptr1 = s1 - dif;
auto x0 = simd_t::load(reinterpret_cast<const sint*>(ptr0));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(ptr0 + width));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(ptr0 + width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
auto y0 = simd_t::load(reinterpret_cast<const sint*>(ptr1));
auto y1 = simd_t::load(reinterpret_cast<const sint*>(ptr1 + width));
auto y2 = simd_t::load(reinterpret_cast<const sint*>(ptr1 + width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));
__WJR_MEMEQ_FOUR(simd_t);
}
return true;
}
if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s1) % _Mysize == 0)) {
const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;
if (is_constant_p(off1) && off1 == 0) {
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));
__WJR_MEMEQ_ONE(simd_t);
const auto __align_s = bound - off1;
s0 += __align_s / _Mysize;
s1 += __align_s / _Mysize;
n -= __align_s / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_1);
}
}
do {
WJR_SIMD_LOADU4(simd_t, x, s0, s0 + width, s0 + width * 2, s0 + width * 3);
WJR_SIMD_LOAD4(simd_t, y, s1, s1 + width, s1 + width * 2, s1 + width * 3);
__WJR_MEMEQ_FOUR(simd_t);
s0 += width * 4;
s1 += width * 4;
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align_1) :
s0 += n;
s1 += n;
const auto ptr1 = reinterpret_cast<T*>(
(reinterpret_cast<uintptr_t>(s1 - width * 3)) & (~(bound - 1)));
const auto dif = s1 - ptr1;
const auto ptr0 = s0 - dif;
auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0 + width));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0 + width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
auto y0 = simd_t::load(reinterpret_cast<const sint*>(ptr1));
auto y1 = simd_t::load(reinterpret_cast<const sint*>(ptr1 + width));
auto y2 = simd_t::load(reinterpret_cast<const sint*>(ptr1 + width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));
__WJR_MEMEQ_FOUR(simd_t);
}
return true;
}
do {
WJR_SIMD_LOADU4(simd_t, x, s0, s0 + width, s0 + width * 2, s0 + width * 3);
WJR_SIMD_LOADU4(simd_t, y, s1, s1 + width, s1 + width * 2, s1 + width * 3);
__WJR_MEMEQ_FOUR(simd_t);
s0 += width * 4;
s1 += width * 4;
n -= width * 4;
} while (n >= width * 4);
if (n == 0) {
return true;
}
goto WJR_MACRO_LABEL(unaligned_last_4vec);
}
extern template bool __large__memeq(const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
extern template bool __large__memeq(const uint8_t*, const uint8_t*, size_t, std::not_equal_to<>) noexcept;
template<typename T, typename _Pred>
WJR_NODISCARD WJR_PURE inline bool __memeq(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {
constexpr size_t _Mysize = sizeof(T);
if (is_constant_p(n) && n <= 64 / _Mysize) {
return ::memcmp(s0, s1, n * _Mysize) == 0;
}
if (WJR_UNLIKELY(n == 0)) return true;
if (n >= 16 / _Mysize) {
{
WJR_SIMD_LOADU(simd::sse, x, s0);
WJR_SIMD_LOADU(simd::sse, y, s1);
__WJR_MEMEQ_ONE(simd::sse);
}
{
auto m = n <= 32 / _Mysize ? (n - 16 / _Mysize) : (16 / _Mysize);
{
WJR_SIMD_LOADU(simd::sse, x, s0 + m);
WJR_SIMD_LOADU(simd::sse, y, s1 + m);
__WJR_MEMEQ_ONE(simd::sse);
}
if (n <= 32 / _Mysize) {
return WJR_SIMD_ADD_PTR(s0, n);
}
}
return __large__memeq(s0, s1, n, pred);
}
if constexpr (_Mysize == 8) {
return pred(s0[0], s1[0]);
}
constexpr auto __need_byteswap = !wjr::is_any_of_v<_Pred,
std::equal_to<>,
std::not_equal_to<>
>;
if constexpr (_Mysize == 2) {
if (n >= 4) {
auto A0 = *reinterpret_cast<const uint64_t*>(s0);
auto B0 = *reinterpret_cast<const uint64_t*>(s0 + n - 4);
auto A1 = *reinterpret_cast<const uint64_t*>(s1);
auto B1 = *reinterpret_cast<const uint64_t*>(s1 + n - 4);
if constexpr (__need_byteswap) {
A0 = wjr::byteswap(A0);
B0 = wjr::byteswap(B0);
A1 = wjr::byteswap(A1);
B1 = wjr::byteswap(B1);
}
return pred(A0, A1) && pred(B0, B1);
}
}
if constexpr (_Mysize == 1) {
if (n >= 4) {
auto delta = (n & 8) >> 1;
auto A0 = *reinterpret_cast<const uint32_t*>(s0);
auto B0 = *reinterpret_cast<const uint32_t*>(s0 + delta);
auto C0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4 - delta);
auto D0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4);
auto A1 = *reinterpret_cast<const uint32_t*>(s1);
auto B1 = *reinterpret_cast<const uint32_t*>(s1 + delta);
auto C1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4 - delta);
auto D1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4);
if constexpr (__need_byteswap) {
A0 = wjr::byteswap(A0);
B0 = wjr::byteswap(B0);
C0 = wjr::byteswap(C0);
D0 = wjr::byteswap(D0);
A1 = wjr::byteswap(A1);
B1 = wjr::byteswap(B1);
C1 = wjr::byteswap(C1);
D1 = wjr::byteswap(D1);
}
return (pred(A0, A1) && pred(B0, B1)) && (pred(C0, C1) && pred(D0, D1));
}
}
if constexpr (_Mysize <= 4) {
if (!pred(s0[0], s1[0])) return false;
if (n == 1) return true;
auto A = *reinterpret_cast<const uint16_t*>(s0 + n - 2);
auto B = *reinterpret_cast<const uint16_t*>(s1 + n - 2);
if constexpr (__need_byteswap) {
A = wjr::byteswap(A);
B = wjr::byteswap(B);
}
return pred(A, B);
}
}
#undef WJR_SIMD_IS_BACKWARD
_WJR_ALGO_END
#undef __WJR_MEMEQ_ONE
#undef __WJR_MEMEQ_FOUR
#undef __WJR_MEMEQ_ONE_NORMAL
#undef __WJR_MEMEQ_FOUR_NORMAL
#endif // _WJR_FAST_MEMEQ

#ifndef __WJR_ALGO_MEM_ALL_H
#error "This file should not be included directly. Include <wjr/algo/mem-all.h> instead."
#endif // __WJR_ALGO_MEM_ALL_H
#if defined(_WJR_FAST_MEMMIS)
#define __WJR_MEMMIS_ONE(st, _s)																	\
{	                                                                                            \
auto r = st::cmp(x, y, pred, T());	                                                        \
st::mask_type z = st::movemask_epi8(r);		                                                \
if(WJR_LIKELY(z != st::mask())) {	                                                        \
return WJR_SIMD_FIRST_ZERO_PTR(_s, z);													\
}	                                                                                        \
}
#define __WJR_MEMMIS_FOUR(st, _s0, _s1, _s2, _s3)													\
{	                                                                                            \
auto r0 = st::cmp(x0, y0, pred, T());	                                                    \
auto r1 = st::cmp(x1, y1, pred, T());	                                                    \
auto r2 = st::cmp(x2, y2, pred, T());	                                                    \
auto r3 = st::cmp(x3, y3, pred, T());	                                                    \
\
r3 = st::And(st::And(r0, r1), st::And(r2, r3));	                                            \
st::mask_type z = st::movemask_epi8(r3);		                                            \
if(WJR_UNLIKELY(z != st::mask())) {	                                                        \
st::mask_type tmp = st::movemask_epi8(r0);	                                            \
if(tmp != st::mask()){	                                                                \
return WJR_SIMD_FIRST_ZERO_PTR(_s0, tmp);	                                        \
}	                                                                                    \
tmp = st::movemask_epi8(r1);	                                                        \
if(tmp != st::mask()){	                                                                \
return WJR_SIMD_FIRST_ZERO_PTR(_s1, tmp);	                                        \
}	                                                                                    \
tmp = st::movemask_epi8(r2);	                                                        \
if(tmp != st::mask()){	                                                                \
return WJR_SIMD_FIRST_ZERO_PTR(_s2, tmp);	                                        \
}	                                                                                    \
tmp = z;	                                                                            \
return WJR_SIMD_FIRST_ZERO_PTR(_s3, tmp);	                                            \
}	                                                                                        \
}
#define WJR_SIMD_IS_BACKWARD 0
#define __WJR_MEMMIS_NAME __memmis
_WJR_ALGO_BEGIN
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) const T*
WJR_MACRO_CONCAT(__large, __WJR_MEMMIS_NAME)(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {
constexpr size_t _Mysize = sizeof(T);
#if WJR_AVX2
using simd_t = simd::avx;
#else
using simd_t = simd::sse;
#endif // WJR_AVX2
constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
constexpr uintptr_t bound = width * _Mysize;
{
auto m = n <= 32 / _Mysize ? (n - 16 / _Mysize) : (16 / _Mysize);
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s0, m));
WJR_SIMD_LOADU(simd::sse, y, WJR_SIMD_ADD_PTR(s1, m));
__WJR_MEMMIS_ONE(simd::sse, WJR_SIMD_ADD_PTR(s0, m));
}
if (n <= 32 / _Mysize) {
return WJR_SIMD_ADD_PTR(s0, n);
}
}
{
const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
const auto negdelta = m - 32 / _Mysize;
#if WJR_AVX2
{
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s0, delta));
WJR_SIMD_LOADU(simd::avx, y, WJR_SIMD_ADD_PTR(s1, delta));
__WJR_MEMMIS_ONE(simd::avx, WJR_SIMD_ADD_PTR(s0, delta));
}
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s0, negdelta - delta));
WJR_SIMD_LOADU(simd::avx, y, WJR_SIMD_ADD_PTR(s1, negdelta - delta));
__WJR_MEMMIS_ONE(simd::avx, WJR_SIMD_ADD_PTR(s0, negdelta - delta));
}
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s0, negdelta));
WJR_SIMD_LOADU(simd::avx, y, WJR_SIMD_ADD_PTR(s1, negdelta));
__WJR_MEMMIS_ONE(simd::avx, WJR_SIMD_ADD_PTR(s0, negdelta));
}
}
#else
{
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s0, delta));
WJR_SIMD_LOADU(simd::sse, y, WJR_SIMD_ADD_PTR(s1, delta));
__WJR_MEMMIS_ONE(simd::sse, WJR_SIMD_ADD_PTR(s0, delta));
}
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s0, delta + 16 / _Mysize));
WJR_SIMD_LOADU(simd::sse, y, WJR_SIMD_ADD_PTR(s1, delta + 16 / _Mysize));
__WJR_MEMMIS_ONE(simd::sse, WJR_SIMD_ADD_PTR(s0, delta + 16 / _Mysize));
}
}
{
WJR_SIMD_LOADU4(simd::sse, x,
WJR_SIMD_ADD_PTR(s0, negdelta - delta),
WJR_SIMD_ADD_PTR(s0, negdelta - delta + 16 / _Mysize),
WJR_SIMD_ADD_PTR(s0, negdelta),
WJR_SIMD_ADD_PTR(s0, negdelta + 16 / _Mysize));
WJR_SIMD_LOADU4(simd::sse, y,
WJR_SIMD_ADD_PTR(s1, negdelta - delta),
WJR_SIMD_ADD_PTR(s1, negdelta - delta + 16 / _Mysize),
WJR_SIMD_ADD_PTR(s1, negdelta),
WJR_SIMD_ADD_PTR(s1, negdelta + 16 / _Mysize));
__WJR_MEMMIS_FOUR(simd::sse,
WJR_SIMD_ADD_PTR(s0, negdelta - delta),
WJR_SIMD_ADD_PTR(s0, negdelta - delta + 16 / _Mysize),
WJR_SIMD_ADD_PTR(s0, negdelta),
WJR_SIMD_ADD_PTR(s0, negdelta + 16 / _Mysize));
}
#endif // WJR_AVX2
if (m == n) {
return WJR_SIMD_ADD_PTR(s0, n);
}
}
if (n <= 128 / _Mysize + width * 4) {
WJR_MACRO_LABEL(unaligned_last_4vec) :
WJR_SIMD_INC_PTR(s0, n);
WJR_SIMD_INC_PTR(s1, n);
WJR_SIMD_LOADU4(simd_t, x,
WJR_SIMD_SUB_PTR(s0, width * 4),
WJR_SIMD_SUB_PTR(s0, width * 3),
WJR_SIMD_SUB_PTR(s0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
WJR_SIMD_LOADU4(simd_t, y,
WJR_SIMD_SUB_PTR(s1, width * 4),
WJR_SIMD_SUB_PTR(s1, width * 3),
WJR_SIMD_SUB_PTR(s1, width * 2),
WJR_SIMD_SUB_PTR(s1, width));
__WJR_MEMMIS_FOUR(simd_t,
WJR_SIMD_SUB_PTR(s0, width * 4),
WJR_SIMD_SUB_PTR(s0, width * 3),
WJR_SIMD_SUB_PTR(s0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
return s0;
}
WJR_SIMD_INC_PTR(s0, 128 / _Mysize);
WJR_SIMD_INC_PTR(s1, 128 / _Mysize);
n -= 128 / _Mysize;
if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s0) % _Mysize == 0)) {
const auto __align_s0 = WJR_SIMD_CEIL_ALIGN_OFFSET(s0, bound);
const auto __align_s1 = WJR_SIMD_CEIL_ALIGN_OFFSET(s1, bound);
if (__align_s0 != __align_s1) {
if (is_constant_p(__align_s0) && __align_s0 == 0) {
}
else {
WJR_SIMD_LOADU(simd_t, x, s0);
WJR_SIMD_LOADU(simd_t, y, s1);
__WJR_MEMMIS_ONE(simd_t, s0);
WJR_SIMD_INC_PTR(s0, __align_s0 / _Mysize);
WJR_SIMD_INC_PTR(s1, __align_s0 / _Mysize);
n -= __align_s0 / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0);
}
}
do {
WJR_SIMD_LOAD4(simd_t, x,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_LOADU4(simd_t, y,
s1,
WJR_SIMD_ADD_PTR(s1, width),
WJR_SIMD_ADD_PTR(s1, width * 2),
WJR_SIMD_ADD_PTR(s1, width * 3));
__WJR_MEMMIS_FOUR(simd_t,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_INC_PTR(s0, width * 4);
WJR_SIMD_INC_PTR(s1, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align_0) :
WJR_SIMD_INC_PTR(s0, n);
WJR_SIMD_INC_PTR(s1, n);
const auto ptr0 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s0, bound), width * 4);
const auto dif = s0 - ptr0;
const auto ptr1 = s1 - dif;
WJR_SIMD_LOAD(simd_t, x0, ptr0);
WJR_SIMD_LOAD(simd_t, x1, WJR_SIMD_ADD_PTR(ptr0, width));
WJR_SIMD_LOAD(simd_t, x2, WJR_SIMD_ADD_PTR(ptr0, width * 2));
WJR_SIMD_LOADU(simd_t, x3, WJR_SIMD_SUB_PTR(s0, width));
WJR_SIMD_LOADU4(simd_t, y,
ptr1,
WJR_SIMD_ADD_PTR(ptr1, width),
WJR_SIMD_ADD_PTR(ptr1, width * 2),
WJR_SIMD_SUB_PTR(s1, width));
__WJR_MEMMIS_FOUR(simd_t,
ptr0,
WJR_SIMD_ADD_PTR(ptr0, width),
WJR_SIMD_ADD_PTR(ptr0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
}
return s0;
}
if (is_constant_p(__align_s0) && __align_s0 == 0) {
}
else {
WJR_SIMD_LOADU(simd_t, x, s0);
WJR_SIMD_LOADU(simd_t, y, s1);
__WJR_MEMMIS_ONE(simd_t, s0);
WJR_SIMD_INC_PTR(s0, __align_s0 / _Mysize);
WJR_SIMD_INC_PTR(s1, __align_s0 / _Mysize);
n -= __align_s0 / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0_1);
}
}
do {
WJR_SIMD_LOAD4(simd_t, x,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_LOAD4(simd_t, y,
s1,
WJR_SIMD_ADD_PTR(s1, width),
WJR_SIMD_ADD_PTR(s1, width * 2),
WJR_SIMD_ADD_PTR(s1, width * 3));
__WJR_MEMMIS_FOUR(simd_t,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_INC_PTR(s0, width * 4);
WJR_SIMD_INC_PTR(s1, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align_0_1) :
WJR_SIMD_INC_PTR(s0, n);
WJR_SIMD_INC_PTR(s1, n);
const auto ptr0 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s0, bound), width * 4);
const auto dif = s0 - ptr0;
const auto ptr1 = s1 - dif;
WJR_SIMD_LOAD(simd_t, x0, ptr0);
WJR_SIMD_LOAD(simd_t, x1, WJR_SIMD_ADD_PTR(ptr0, width));
WJR_SIMD_LOAD(simd_t, x2, WJR_SIMD_ADD_PTR(ptr0, width * 2));
WJR_SIMD_LOADU(simd_t, x3, WJR_SIMD_SUB_PTR(s0, width));
WJR_SIMD_LOAD(simd_t, y0, ptr1);
WJR_SIMD_LOAD(simd_t, y1, WJR_SIMD_ADD_PTR(ptr1, width));
WJR_SIMD_LOAD(simd_t, y2, WJR_SIMD_ADD_PTR(ptr1, width * 2));
WJR_SIMD_LOADU(simd_t, y3, WJR_SIMD_SUB_PTR(s1, width));
__WJR_MEMMIS_FOUR(simd_t,
ptr0,
WJR_SIMD_ADD_PTR(ptr0, width),
WJR_SIMD_ADD_PTR(ptr0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
}
return s0;
}
if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s1) % _Mysize == 0)) {
const auto __align_s1 = WJR_SIMD_CEIL_ALIGN_OFFSET(s1, bound);
if (is_constant_p(__align_s1) && __align_s1 == 0) {
}
else {
WJR_SIMD_LOADU(simd_t, x, s0);
WJR_SIMD_LOADU(simd_t, y, s1);
__WJR_MEMMIS_ONE(simd_t, s0);
WJR_SIMD_INC_PTR(s0, __align_s1 / _Mysize);
WJR_SIMD_INC_PTR(s1, __align_s1 / _Mysize);
n -= __align_s1 / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_1);
}
}
do {
WJR_SIMD_LOADU4(simd_t, x,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_LOAD4(simd_t, y,
s1,
WJR_SIMD_ADD_PTR(s1, width),
WJR_SIMD_ADD_PTR(s1, width * 2),
WJR_SIMD_ADD_PTR(s1, width * 3));
__WJR_MEMMIS_FOUR(simd_t,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_INC_PTR(s0, width * 4);
WJR_SIMD_INC_PTR(s1, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align_1) :
WJR_SIMD_INC_PTR(s0, n);
WJR_SIMD_INC_PTR(s1, n);
const auto ptr1 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s1, bound), width * 4);
const auto dif = s1 - ptr1;
const auto ptr0 = s0 - dif;
WJR_SIMD_LOADU4(simd_t, x,
ptr0,
WJR_SIMD_ADD_PTR(ptr0, width),
WJR_SIMD_ADD_PTR(ptr0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
WJR_SIMD_LOAD(simd_t, y0, ptr1);
WJR_SIMD_LOAD(simd_t, y1, WJR_SIMD_ADD_PTR(ptr1, width));
WJR_SIMD_LOAD(simd_t, y2, WJR_SIMD_ADD_PTR(ptr1, width * 2));
WJR_SIMD_LOADU(simd_t, y3, WJR_SIMD_SUB_PTR(s1, width));
__WJR_MEMMIS_FOUR(simd_t,
ptr0,
WJR_SIMD_ADD_PTR(ptr0, width),
WJR_SIMD_ADD_PTR(ptr0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
}
return s0;
}
do {
WJR_SIMD_LOADU4(simd_t, x,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_LOADU4(simd_t, y,
s1,
WJR_SIMD_ADD_PTR(s1, width),
WJR_SIMD_ADD_PTR(s1, width * 2),
WJR_SIMD_ADD_PTR(s1, width * 3));
__WJR_MEMMIS_FOUR(simd_t,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_INC_PTR(s0, width * 4);
WJR_SIMD_INC_PTR(s1, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n == 0) {
return s0;
}
goto WJR_MACRO_LABEL(unaligned_last_4vec);
}
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) const T*
WJR_MACRO_CONCAT(__small, __WJR_MEMMIS_NAME)(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {
constexpr auto _Mysize = sizeof(T);
#if !WJR_SIMD_IS_BACKWARD
if constexpr (_Mysize == 8) {
return pred(s0[0], s1[0]) ? s0 + 1 : s0;
}
if constexpr (_Mysize == 2) {
if (n >= 4) {
auto A0 = *reinterpret_cast<const uint64_t*>(s0);
auto B0 = *reinterpret_cast<const uint64_t*>(s0 + n - 4);
auto A1 = *reinterpret_cast<const uint64_t*>(s1);
auto B1 = *reinterpret_cast<const uint64_t*>(s1 + n - 4);
auto x = simd::sse::set_epi64x(B0, A0);
auto y = simd::sse::set_epi64x(B1, A1);
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == simd::sse::mask())return s0 + n;
auto i = wjr::countr_one(z) / _Mysize;
auto q = i >= 4 ? i + (n - 8) : i;
return s0 + q;
}
}
if constexpr (_Mysize == 1) {
if (n >= 4) {
auto delta = (n & 8) >> 1;
auto A0 = *reinterpret_cast<const uint32_t*>(s0);
auto B0 = *reinterpret_cast<const uint32_t*>(s0 + delta);
auto C0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4 - delta);
auto D0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4);
auto A1 = *reinterpret_cast<const uint32_t*>(s1);
auto B1 = *reinterpret_cast<const uint32_t*>(s1 + delta);
auto C1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4 - delta);
auto D1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4);
auto x = simd::sse::set_epi32(D0, C0, B0, A0);
auto y = simd::sse::set_epi32(D1, C1, B1, A1);
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == simd::sse::mask()) return s0 + n;
auto i = wjr::countr_one(z);
auto q = i >= 8 ? i + (n - 12 - delta) : i;
return s0 + q;
}
}
if constexpr (_Mysize <= 4) {
if (!pred(s0[0], s1[0])) return s0;
if (n == 1) return s0 + 1;
const bool f = pred(s0[n - 2], s1[n - 2]);
const bool g = pred(s0[n - 1], s1[n - 1]);
const size_t i1 = g ? n : n - 1;
const size_t i2 = f ? i1 : n - 2;
return s0 + i2;
}
#else
if constexpr (_Mysize == 8) {
return pred(s0[-1], s1[-1]) ? s0 : s0 - 1;
}
if constexpr (_Mysize == 2) {
if (n >= 4) {
auto A0 = *reinterpret_cast<const uint64_t*>(s0 - n);
auto B0 = *reinterpret_cast<const uint64_t*>(s0 - 4);
auto A1 = *reinterpret_cast<const uint64_t*>(s1 - n);
auto B1 = *reinterpret_cast<const uint64_t*>(s1 - 4);
auto x = simd::sse::set_epi64x(B0, A0);
auto y = simd::sse::set_epi64x(B1, A1);
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == simd::sse::mask())return s0 - n;
auto i = wjr::countl_one(z) / _Mysize;
auto q = i >= 4 ? i + (n - 8) : i;
return s0 - q;
}
}
if constexpr (_Mysize == 1) {
if (n >= 4) {
const auto delta = (n & 8) >> 1;
auto A0 = *reinterpret_cast<const uint32_t*>(s0 - n);
auto B0 = *reinterpret_cast<const uint32_t*>(s0 - n + delta);
auto C0 = *reinterpret_cast<const uint32_t*>(s0 - 4 - delta);
auto D0 = *reinterpret_cast<const uint32_t*>(s0 - 4);
auto A1 = *reinterpret_cast<const uint32_t*>(s1 - n);
auto B1 = *reinterpret_cast<const uint32_t*>(s1 - n + delta);
auto C1 = *reinterpret_cast<const uint32_t*>(s1 - 4 - delta);
auto D1 = *reinterpret_cast<const uint32_t*>(s1 - 4);
auto x = simd::sse::set_epi32(D0, C0, B0, A0);
auto y = simd::sse::set_epi32(D1, C1, B1, A1);
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == simd::sse::mask()) return s0 - n;
auto i = wjr::countl_one(z);
auto q = i >= 8 ? i + (n - 12 - delta) : i;
return s0 - q;
}
}
if constexpr (_Mysize <= 4) {
if (!pred(s0[-1], s1[-1])) return s0;
if (n == 1) return s0 - 1;
const bool f = pred(s0[static_cast<size_t>(1 - n)], s1[static_cast<size_t>(1 - n)]);
const bool g = pred(s0[static_cast<size_t>(0 - n)], s1[static_cast<size_t>(0 - n)]);
const size_t i1 = g ? -n : 1 - n;
const size_t i2 = f ? i1 : 2 - n;
return s0 + i2;
}
#endif // WJR_SIMD_IS_BACKWARD
}
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T*
__WJR_MEMMIS_NAME(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {
constexpr size_t _Mysize = sizeof(T);
if (WJR_UNLIKELY(n == 0)) return s0;
WJR_SIMD_INIT_PTR(s0, n);
WJR_SIMD_INIT_PTR(s1, n);
if (n >= 16 / _Mysize) {
{
WJR_SIMD_LOADU(simd::sse, x, s0);
WJR_SIMD_LOADU(simd::sse, y, s1);
__WJR_MEMMIS_ONE(simd::sse, s0);
}
return WJR_MACRO_CONCAT(__large, __WJR_MEMMIS_NAME)(s0, s1, n, pred);
}
return WJR_MACRO_CONCAT(__small, __WJR_MEMMIS_NAME)(s0, s1, n, pred);
}
_WJR_ALGO_END

#undef __WJR_MEMMIS_NAME
#undef WJR_SIMD_IS_BACKWARD
#define WJR_SIMD_IS_BACKWARD 1
#define __WJR_MEMMIS_NAME __memrmis
_WJR_ALGO_BEGIN
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) const T*
WJR_MACRO_CONCAT(__large, __WJR_MEMMIS_NAME)(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {
constexpr size_t _Mysize = sizeof(T);
#if WJR_AVX2
using simd_t = simd::avx;
#else
using simd_t = simd::sse;
#endif // WJR_AVX2
constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
constexpr uintptr_t bound = width * _Mysize;
{
auto m = n <= 32 / _Mysize ? (n - 16 / _Mysize) : (16 / _Mysize);
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s0, m));
WJR_SIMD_LOADU(simd::sse, y, WJR_SIMD_ADD_PTR(s1, m));
__WJR_MEMMIS_ONE(simd::sse, WJR_SIMD_ADD_PTR(s0, m));
}
if (n <= 32 / _Mysize) {
return WJR_SIMD_ADD_PTR(s0, n);
}
}
{
const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
const auto negdelta = m - 32 / _Mysize;
#if WJR_AVX2
{
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s0, delta));
WJR_SIMD_LOADU(simd::avx, y, WJR_SIMD_ADD_PTR(s1, delta));
__WJR_MEMMIS_ONE(simd::avx, WJR_SIMD_ADD_PTR(s0, delta));
}
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s0, negdelta - delta));
WJR_SIMD_LOADU(simd::avx, y, WJR_SIMD_ADD_PTR(s1, negdelta - delta));
__WJR_MEMMIS_ONE(simd::avx, WJR_SIMD_ADD_PTR(s0, negdelta - delta));
}
{
WJR_SIMD_LOADU(simd::avx, x, WJR_SIMD_ADD_PTR(s0, negdelta));
WJR_SIMD_LOADU(simd::avx, y, WJR_SIMD_ADD_PTR(s1, negdelta));
__WJR_MEMMIS_ONE(simd::avx, WJR_SIMD_ADD_PTR(s0, negdelta));
}
}
#else
{
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s0, delta));
WJR_SIMD_LOADU(simd::sse, y, WJR_SIMD_ADD_PTR(s1, delta));
__WJR_MEMMIS_ONE(simd::sse, WJR_SIMD_ADD_PTR(s0, delta));
}
{
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s0, delta + 16 / _Mysize));
WJR_SIMD_LOADU(simd::sse, y, WJR_SIMD_ADD_PTR(s1, delta + 16 / _Mysize));
__WJR_MEMMIS_ONE(simd::sse, WJR_SIMD_ADD_PTR(s0, delta + 16 / _Mysize));
}
}
{
WJR_SIMD_LOADU4(simd::sse, x,
WJR_SIMD_ADD_PTR(s0, negdelta - delta),
WJR_SIMD_ADD_PTR(s0, negdelta - delta + 16 / _Mysize),
WJR_SIMD_ADD_PTR(s0, negdelta),
WJR_SIMD_ADD_PTR(s0, negdelta + 16 / _Mysize));
WJR_SIMD_LOADU4(simd::sse, y,
WJR_SIMD_ADD_PTR(s1, negdelta - delta),
WJR_SIMD_ADD_PTR(s1, negdelta - delta + 16 / _Mysize),
WJR_SIMD_ADD_PTR(s1, negdelta),
WJR_SIMD_ADD_PTR(s1, negdelta + 16 / _Mysize));
__WJR_MEMMIS_FOUR(simd::sse,
WJR_SIMD_ADD_PTR(s0, negdelta - delta),
WJR_SIMD_ADD_PTR(s0, negdelta - delta + 16 / _Mysize),
WJR_SIMD_ADD_PTR(s0, negdelta),
WJR_SIMD_ADD_PTR(s0, negdelta + 16 / _Mysize));
}
#endif // WJR_AVX2
if (m == n) {
return WJR_SIMD_ADD_PTR(s0, n);
}
}
if (n <= 128 / _Mysize + width * 4) {
WJR_MACRO_LABEL(unaligned_last_4vec) :
WJR_SIMD_INC_PTR(s0, n);
WJR_SIMD_INC_PTR(s1, n);
WJR_SIMD_LOADU4(simd_t, x,
WJR_SIMD_SUB_PTR(s0, width * 4),
WJR_SIMD_SUB_PTR(s0, width * 3),
WJR_SIMD_SUB_PTR(s0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
WJR_SIMD_LOADU4(simd_t, y,
WJR_SIMD_SUB_PTR(s1, width * 4),
WJR_SIMD_SUB_PTR(s1, width * 3),
WJR_SIMD_SUB_PTR(s1, width * 2),
WJR_SIMD_SUB_PTR(s1, width));
__WJR_MEMMIS_FOUR(simd_t,
WJR_SIMD_SUB_PTR(s0, width * 4),
WJR_SIMD_SUB_PTR(s0, width * 3),
WJR_SIMD_SUB_PTR(s0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
return s0;
}
WJR_SIMD_INC_PTR(s0, 128 / _Mysize);
WJR_SIMD_INC_PTR(s1, 128 / _Mysize);
n -= 128 / _Mysize;
if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s0) % _Mysize == 0)) {
const auto __align_s0 = WJR_SIMD_CEIL_ALIGN_OFFSET(s0, bound);
const auto __align_s1 = WJR_SIMD_CEIL_ALIGN_OFFSET(s1, bound);
if (__align_s0 != __align_s1) {
if (is_constant_p(__align_s0) && __align_s0 == 0) {
}
else {
WJR_SIMD_LOADU(simd_t, x, s0);
WJR_SIMD_LOADU(simd_t, y, s1);
__WJR_MEMMIS_ONE(simd_t, s0);
WJR_SIMD_INC_PTR(s0, __align_s0 / _Mysize);
WJR_SIMD_INC_PTR(s1, __align_s0 / _Mysize);
n -= __align_s0 / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0);
}
}
do {
WJR_SIMD_LOAD4(simd_t, x,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_LOADU4(simd_t, y,
s1,
WJR_SIMD_ADD_PTR(s1, width),
WJR_SIMD_ADD_PTR(s1, width * 2),
WJR_SIMD_ADD_PTR(s1, width * 3));
__WJR_MEMMIS_FOUR(simd_t,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_INC_PTR(s0, width * 4);
WJR_SIMD_INC_PTR(s1, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align_0) :
WJR_SIMD_INC_PTR(s0, n);
WJR_SIMD_INC_PTR(s1, n);
const auto ptr0 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s0, bound), width * 4);
const auto dif = s0 - ptr0;
const auto ptr1 = s1 - dif;
WJR_SIMD_LOAD(simd_t, x0, ptr0);
WJR_SIMD_LOAD(simd_t, x1, WJR_SIMD_ADD_PTR(ptr0, width));
WJR_SIMD_LOAD(simd_t, x2, WJR_SIMD_ADD_PTR(ptr0, width * 2));
WJR_SIMD_LOADU(simd_t, x3, WJR_SIMD_SUB_PTR(s0, width));
WJR_SIMD_LOADU4(simd_t, y,
ptr1,
WJR_SIMD_ADD_PTR(ptr1, width),
WJR_SIMD_ADD_PTR(ptr1, width * 2),
WJR_SIMD_SUB_PTR(s1, width));
__WJR_MEMMIS_FOUR(simd_t,
ptr0,
WJR_SIMD_ADD_PTR(ptr0, width),
WJR_SIMD_ADD_PTR(ptr0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
}
return s0;
}
if (is_constant_p(__align_s0) && __align_s0 == 0) {
}
else {
WJR_SIMD_LOADU(simd_t, x, s0);
WJR_SIMD_LOADU(simd_t, y, s1);
__WJR_MEMMIS_ONE(simd_t, s0);
WJR_SIMD_INC_PTR(s0, __align_s0 / _Mysize);
WJR_SIMD_INC_PTR(s1, __align_s0 / _Mysize);
n -= __align_s0 / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0_1);
}
}
do {
WJR_SIMD_LOAD4(simd_t, x,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_LOAD4(simd_t, y,
s1,
WJR_SIMD_ADD_PTR(s1, width),
WJR_SIMD_ADD_PTR(s1, width * 2),
WJR_SIMD_ADD_PTR(s1, width * 3));
__WJR_MEMMIS_FOUR(simd_t,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_INC_PTR(s0, width * 4);
WJR_SIMD_INC_PTR(s1, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align_0_1) :
WJR_SIMD_INC_PTR(s0, n);
WJR_SIMD_INC_PTR(s1, n);
const auto ptr0 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s0, bound), width * 4);
const auto dif = s0 - ptr0;
const auto ptr1 = s1 - dif;
WJR_SIMD_LOAD(simd_t, x0, ptr0);
WJR_SIMD_LOAD(simd_t, x1, WJR_SIMD_ADD_PTR(ptr0, width));
WJR_SIMD_LOAD(simd_t, x2, WJR_SIMD_ADD_PTR(ptr0, width * 2));
WJR_SIMD_LOADU(simd_t, x3, WJR_SIMD_SUB_PTR(s0, width));
WJR_SIMD_LOAD(simd_t, y0, ptr1);
WJR_SIMD_LOAD(simd_t, y1, WJR_SIMD_ADD_PTR(ptr1, width));
WJR_SIMD_LOAD(simd_t, y2, WJR_SIMD_ADD_PTR(ptr1, width * 2));
WJR_SIMD_LOADU(simd_t, y3, WJR_SIMD_SUB_PTR(s1, width));
__WJR_MEMMIS_FOUR(simd_t,
ptr0,
WJR_SIMD_ADD_PTR(ptr0, width),
WJR_SIMD_ADD_PTR(ptr0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
}
return s0;
}
if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s1) % _Mysize == 0)) {
const auto __align_s1 = WJR_SIMD_CEIL_ALIGN_OFFSET(s1, bound);
if (is_constant_p(__align_s1) && __align_s1 == 0) {
}
else {
WJR_SIMD_LOADU(simd_t, x, s0);
WJR_SIMD_LOADU(simd_t, y, s1);
__WJR_MEMMIS_ONE(simd_t, s0);
WJR_SIMD_INC_PTR(s0, __align_s1 / _Mysize);
WJR_SIMD_INC_PTR(s1, __align_s1 / _Mysize);
n -= __align_s1 / _Mysize;
if (WJR_UNLIKELY(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_1);
}
}
do {
WJR_SIMD_LOADU4(simd_t, x,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_LOAD4(simd_t, y,
s1,
WJR_SIMD_ADD_PTR(s1, width),
WJR_SIMD_ADD_PTR(s1, width * 2),
WJR_SIMD_ADD_PTR(s1, width * 3));
__WJR_MEMMIS_FOUR(simd_t,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_INC_PTR(s0, width * 4);
WJR_SIMD_INC_PTR(s1, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n != 0) {
WJR_MACRO_LABEL(last_solve_align_1) :
WJR_SIMD_INC_PTR(s0, n);
WJR_SIMD_INC_PTR(s1, n);
const auto ptr1 = WJR_SIMD_SUB_PTR(WJR_SIMD_CEIL_ALIGN_PTR(s1, bound), width * 4);
const auto dif = s1 - ptr1;
const auto ptr0 = s0 - dif;
WJR_SIMD_LOADU4(simd_t, x,
ptr0,
WJR_SIMD_ADD_PTR(ptr0, width),
WJR_SIMD_ADD_PTR(ptr0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
WJR_SIMD_LOAD(simd_t, y0, ptr1);
WJR_SIMD_LOAD(simd_t, y1, WJR_SIMD_ADD_PTR(ptr1, width));
WJR_SIMD_LOAD(simd_t, y2, WJR_SIMD_ADD_PTR(ptr1, width * 2));
WJR_SIMD_LOADU(simd_t, y3, WJR_SIMD_SUB_PTR(s1, width));
__WJR_MEMMIS_FOUR(simd_t,
ptr0,
WJR_SIMD_ADD_PTR(ptr0, width),
WJR_SIMD_ADD_PTR(ptr0, width * 2),
WJR_SIMD_SUB_PTR(s0, width));
}
return s0;
}
do {
WJR_SIMD_LOADU4(simd_t, x,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_LOADU4(simd_t, y,
s1,
WJR_SIMD_ADD_PTR(s1, width),
WJR_SIMD_ADD_PTR(s1, width * 2),
WJR_SIMD_ADD_PTR(s1, width * 3));
__WJR_MEMMIS_FOUR(simd_t,
s0,
WJR_SIMD_ADD_PTR(s0, width),
WJR_SIMD_ADD_PTR(s0, width * 2),
WJR_SIMD_ADD_PTR(s0, width * 3));
WJR_SIMD_INC_PTR(s0, width * 4);
WJR_SIMD_INC_PTR(s1, width * 4);
n -= width * 4;
} while (n >= width * 4);
if (n == 0) {
return s0;
}
goto WJR_MACRO_LABEL(unaligned_last_4vec);
}
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE) const T*
WJR_MACRO_CONCAT(__small, __WJR_MEMMIS_NAME)(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {
constexpr auto _Mysize = sizeof(T);
#if !WJR_SIMD_IS_BACKWARD
if constexpr (_Mysize == 8) {
return pred(s0[0], s1[0]) ? s0 + 1 : s0;
}
if constexpr (_Mysize == 2) {
if (n >= 4) {
auto A0 = *reinterpret_cast<const uint64_t*>(s0);
auto B0 = *reinterpret_cast<const uint64_t*>(s0 + n - 4);
auto A1 = *reinterpret_cast<const uint64_t*>(s1);
auto B1 = *reinterpret_cast<const uint64_t*>(s1 + n - 4);
auto x = simd::sse::set_epi64x(B0, A0);
auto y = simd::sse::set_epi64x(B1, A1);
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == simd::sse::mask())return s0 + n;
auto i = wjr::countr_one(z) / _Mysize;
auto q = i >= 4 ? i + (n - 8) : i;
return s0 + q;
}
}
if constexpr (_Mysize == 1) {
if (n >= 4) {
auto delta = (n & 8) >> 1;
auto A0 = *reinterpret_cast<const uint32_t*>(s0);
auto B0 = *reinterpret_cast<const uint32_t*>(s0 + delta);
auto C0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4 - delta);
auto D0 = *reinterpret_cast<const uint32_t*>(s0 + n - 4);
auto A1 = *reinterpret_cast<const uint32_t*>(s1);
auto B1 = *reinterpret_cast<const uint32_t*>(s1 + delta);
auto C1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4 - delta);
auto D1 = *reinterpret_cast<const uint32_t*>(s1 + n - 4);
auto x = simd::sse::set_epi32(D0, C0, B0, A0);
auto y = simd::sse::set_epi32(D1, C1, B1, A1);
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == simd::sse::mask()) return s0 + n;
auto i = wjr::countr_one(z);
auto q = i >= 8 ? i + (n - 12 - delta) : i;
return s0 + q;
}
}
if constexpr (_Mysize <= 4) {
if (!pred(s0[0], s1[0])) return s0;
if (n == 1) return s0 + 1;
const bool f = pred(s0[n - 2], s1[n - 2]);
const bool g = pred(s0[n - 1], s1[n - 1]);
const size_t i1 = g ? n : n - 1;
const size_t i2 = f ? i1 : n - 2;
return s0 + i2;
}
#else
if constexpr (_Mysize == 8) {
return pred(s0[-1], s1[-1]) ? s0 : s0 - 1;
}
if constexpr (_Mysize == 2) {
if (n >= 4) {
auto A0 = *reinterpret_cast<const uint64_t*>(s0 - n);
auto B0 = *reinterpret_cast<const uint64_t*>(s0 - 4);
auto A1 = *reinterpret_cast<const uint64_t*>(s1 - n);
auto B1 = *reinterpret_cast<const uint64_t*>(s1 - 4);
auto x = simd::sse::set_epi64x(B0, A0);
auto y = simd::sse::set_epi64x(B1, A1);
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == simd::sse::mask())return s0 - n;
auto i = wjr::countl_one(z) / _Mysize;
auto q = i >= 4 ? i + (n - 8) : i;
return s0 - q;
}
}
if constexpr (_Mysize == 1) {
if (n >= 4) {
const auto delta = (n & 8) >> 1;
auto A0 = *reinterpret_cast<const uint32_t*>(s0 - n);
auto B0 = *reinterpret_cast<const uint32_t*>(s0 - n + delta);
auto C0 = *reinterpret_cast<const uint32_t*>(s0 - 4 - delta);
auto D0 = *reinterpret_cast<const uint32_t*>(s0 - 4);
auto A1 = *reinterpret_cast<const uint32_t*>(s1 - n);
auto B1 = *reinterpret_cast<const uint32_t*>(s1 - n + delta);
auto C1 = *reinterpret_cast<const uint32_t*>(s1 - 4 - delta);
auto D1 = *reinterpret_cast<const uint32_t*>(s1 - 4);
auto x = simd::sse::set_epi32(D0, C0, B0, A0);
auto y = simd::sse::set_epi32(D1, C1, B1, A1);
auto r = simd::sse::cmp(x, y, pred, T());
uint16_t z = simd::sse::movemask_epi8(r);
if (z == simd::sse::mask()) return s0 - n;
auto i = wjr::countl_one(z);
auto q = i >= 8 ? i + (n - 12 - delta) : i;
return s0 - q;
}
}
if constexpr (_Mysize <= 4) {
if (!pred(s0[-1], s1[-1])) return s0;
if (n == 1) return s0 - 1;
const bool f = pred(s0[static_cast<size_t>(1 - n)], s1[static_cast<size_t>(1 - n)]);
const bool g = pred(s0[static_cast<size_t>(0 - n)], s1[static_cast<size_t>(0 - n)]);
const size_t i1 = g ? -n : 1 - n;
const size_t i2 = f ? i1 : 2 - n;
return s0 + i2;
}
#endif // WJR_SIMD_IS_BACKWARD
}
template<typename T, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T*
__WJR_MEMMIS_NAME(const T* s0, const T* s1, size_t n, _Pred pred) noexcept {
constexpr size_t _Mysize = sizeof(T);
if (WJR_UNLIKELY(n == 0)) return s0;
WJR_SIMD_INIT_PTR(s0, n);
WJR_SIMD_INIT_PTR(s1, n);
if (n >= 16 / _Mysize) {
{
WJR_SIMD_LOADU(simd::sse, x, s0);
WJR_SIMD_LOADU(simd::sse, y, s1);
__WJR_MEMMIS_ONE(simd::sse, s0);
}
return WJR_MACRO_CONCAT(__large, __WJR_MEMMIS_NAME)(s0, s1, n, pred);
}
return WJR_MACRO_CONCAT(__small, __WJR_MEMMIS_NAME)(s0, s1, n, pred);
}
_WJR_ALGO_END

#undef __WJR_MEMMIS_NAME
#undef WJR_SIMD_IS_BACKWARD
_WJR_ALGO_BEGIN
extern template const uint8_t* __large__memmis(
const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
extern template const uint8_t* __large__memrmis(
const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
_WJR_ALGO_END
#undef __WJR_MEMMIS_FOUR
#undef __WJR_MEMMIS_ONE
#endif // _WJR_FAST_MEMMIS

#ifndef __WJR_ALGO_MEM_ALL_H
#error "This file should not be included directly. Include <wjr/algo/mem-all.h> instead."
#endif // __WJR_ALGO_MEM_ALL_H
#if defined(_WJR_FAST_MEMSET)
#if WJR_AVX2
#define __WJR_ALIGN64byte(ptr)	                                    \
simd_t::storeu(reinterpret_cast<sint*>((ptr)), q);	            \
simd_t::storeu(reinterpret_cast<sint*>((ptr) + width), q);
#else
#define __WJR_ALIGN64byte(ptr)	                                    \
simd_t::storeu(reinterpret_cast<sint*>((ptr)), q);	            \
simd_t::storeu(reinterpret_cast<sint*>((ptr) + width), q);	    \
simd_t::storeu(reinterpret_cast<sint*>((ptr) + width * 2), q);	\
simd_t::storeu(reinterpret_cast<sint*>((ptr) + width * 3), q);
#endif // __WJR_MEMSET64byte
_WJR_ALGO_BEGIN
template<typename T>
void __memset(T* s, T val, size_t n) {
constexpr size_t _Mysize = sizeof(T);
#if WJR_AVX2
using simd_t = simd::avx;
#else
using simd_t = simd::sse;
#endif // WJR_AVX2
using sint = typename simd_t::int_type;
constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
constexpr uintptr_t bound = width * _Mysize;
static_assert(bound >= 16, "");
static_assert(std::is_same_v<T, make_integral_t<T>>, "");
if constexpr (_Mysize != 1) {
if (
is_constant_p(val) &&
broadcast<T, uint8_t>(static_cast<uint8_t>(val)) == val) {
return __memset(reinterpret_cast<uint8_t*>(s), static_cast<uint8_t>(val), n * _Mysize);
}
}
constexpr size_t __constant_threshold = 256 / _Mysize;
constexpr size_t __rep_align64byte_threshold = 1024 / _Mysize;
const bool __use_rep = wjr::is_enhanced_rep();
#if defined(_WJR_NON_TEMPORARY)
const size_t __nt_threshold = (wjr::max_cache_size() + wjr::max_cache_size() / 8) / _Mysize;
#endif // _WJR_NON_TEMPORARY
if (is_constant_p(n) && n <= __constant_threshold) {
for (size_t i = 0; i < n; ++i) {
s[i] = val;
}
return;
}
if (n >= 16 / _Mysize) {
auto qx = simd::sse::set1(val, T());
if (n <= 32 / _Mysize) {
simd::sse::storeu(reinterpret_cast<__m128i*>(s), qx);
simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (16 / _Mysize)), qx);
return;
}
#if WJR_AVX2
if (n < 128 / _Mysize) {
auto qy = broadcast<simd::__m256i_tag, simd::__m128i_tag>(qx);
auto delta = (n & (64 / _Mysize)) >> 1;
simd::avx::storeu(reinterpret_cast<__m256i*>(s), qy);
simd::avx::storeu(reinterpret_cast<__m256i*>(s + delta), qy);
simd::avx::storeu(reinterpret_cast<__m256i*>(s + n - (32 / _Mysize) - delta), qy);
simd::avx::storeu(reinterpret_cast<__m256i*>(s + n - (32 / _Mysize)), qy);
return;
}
#else
if (n < 128 / _Mysize) {
auto delta = (n & (64 / _Mysize)) >> 1;
simd::sse::storeu(reinterpret_cast<__m128i*>(s), qx);
simd::sse::storeu(reinterpret_cast<__m128i*>(s + (16 / _Mysize)), qx);
simd::sse::storeu(reinterpret_cast<__m128i*>(s + delta), qx);
simd::sse::storeu(reinterpret_cast<__m128i*>(s + delta + (16 / _Mysize)), qx);
simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (32 / _Mysize) - delta), qx);
simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (16 / _Mysize) - delta), qx);
simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (32 / _Mysize)), qx);
simd::sse::storeu(reinterpret_cast<__m128i*>(s + n - (16 / _Mysize)), qx);
return;
}
#endif // WJR_AVX2
auto q = broadcast<simd::__simd_wrapper_t<sint>, simd::__m128i_tag>(qx);
if (WJR_LIKELY(reinterpret_cast<uintptr_t>(s) % _Mysize == 0)) {
#if defined(_WJR_FAST_REP)
if (__use_rep) {
if (n < __rep_align64byte_threshold) {
auto u64v = broadcast<uint64_t, T>(val);
if (is_constant_p(reinterpret_cast<uintptr_t>(s) % 8) &&
reinterpret_cast<uintptr_t>(s) % 8 == 0) {
}
else {
*reinterpret_cast<uint64_t*>(s) = u64v;
auto __align_s = 8 - reinterpret_cast<uintptr_t>(s) % 8;
s += __align_s / _Mysize;
n -= __align_s / _Mysize;
}
*reinterpret_cast<uint64_t*>(s + n - 8 / _Mysize) = u64v;
n &= static_cast<size_t>(0 - 8 / _Mysize);
if constexpr (_Mysize == 1) {
wjr::masm::rep_stos(reinterpret_cast<uint8_t*>(s), val, n);
}
else {
n /= (8 / _Mysize);
wjr::masm::rep_stos(reinterpret_cast<uint64_t*>(s), u64v, n);
}
return;
}
if (is_constant_p(reinterpret_cast<uintptr_t>(s) % 64) &&
reinterpret_cast<uintptr_t>(s) % 64 == 0) {
}
else {
__WJR_ALIGN64byte(s);
auto __align_s = 64 - reinterpret_cast<uintptr_t>(s) % 64;
s += __align_s / _Mysize;
n -= __align_s / _Mysize;
}
#if defined(_WJR_NON_TEMPORARY)
if (n < __nt_threshold) {
#endif // _WJR_NON_TEMPORARY
__WJR_ALIGN64byte(s + n - (64 / _Mysize));
n = (n - 1u) & static_cast<size_t>(0 - (64 / _Mysize));
if constexpr (_Mysize == 1) {
wjr::masm::rep_stos(reinterpret_cast<uint8_t*>(s), val, n);
}
else {
n /= (8 / _Mysize);
auto u64v = broadcast<uint64_t, T>(val);
wjr::masm::rep_stos(reinterpret_cast<uint64_t*>(s), u64v, n);
}
return;
#if defined(_WJR_NON_TEMPORARY)
}
#endif // _WJR_NON_TEMPORARY
}
else {
#endif // _WJR_FAST_REP
#if defined(_WJR_NON_TEMPORARY)
if (n < __nt_threshold) {
#endif // _WJR_NON_TEMPORARY
if (is_constant_p(reinterpret_cast<uintptr_t>(s) % bound) &&
reinterpret_cast<uintptr_t>(s) % bound == 0) {
}
else {
simd_t::storeu(reinterpret_cast<sint*>(s), q);
auto __align_s = bound - reinterpret_cast<uintptr_t>(s) % bound;
s += __align_s / _Mysize;
n -= __align_s / _Mysize;
if (n < width * 4) goto WJR_MACRO_LABEL(last_store);
}
do {
simd_t::store(reinterpret_cast<sint*>(s), q);
simd_t::store(reinterpret_cast<sint*>(s + width), q);
simd_t::store(reinterpret_cast<sint*>(s + width * 2), q);
simd_t::store(reinterpret_cast<sint*>(s + width * 3), q);
s += width * 4;
n -= width * 4;
} while (n >= width * 4);
goto WJR_MACRO_LABEL(last_store);
#if defined(_WJR_NON_TEMPORARY)
}
#endif // _WJR_NON_TEMPORARY
#if defined(_WJR_FAST_REP)
}
#endif // _WJR_FAST_REP
if (is_constant_p(reinterpret_cast<uintptr_t>(s) % 64) &&
reinterpret_cast<uintptr_t>(s) % 64 == 0) {
}
else {
__WJR_ALIGN64byte(s);
auto __align_s = 64 - reinterpret_cast<uintptr_t>(s) % 64;
s += __align_s / _Mysize;
n -= __align_s / _Mysize;
}
do {
simd_t::stream(reinterpret_cast<sint*>(s), q);
simd_t::stream(reinterpret_cast<sint*>(s + width), q);
simd_t::stream(reinterpret_cast<sint*>(s + width * 2), q);
simd_t::stream(reinterpret_cast<sint*>(s + width * 3), q);
s += width * 4;
n -= width * 4;
} while (n >= width * 4);
simd::sse::sfence();
WJR_MACRO_LABEL(last_store) :
s += n;
auto ptr = reinterpret_cast<T*>(
(reinterpret_cast<uintptr_t>(s - width * 3)) & (~(bound - 1)));
simd_t::store(reinterpret_cast<sint*>(ptr), q);
simd_t::store(reinterpret_cast<sint*>(ptr + width), q);
simd_t::store(reinterpret_cast<sint*>(ptr + width * 2), q);
simd_t::storeu(reinterpret_cast<sint*>(s - width), q);
return;
}
do {
simd_t::storeu(reinterpret_cast<sint*>(s), q);
simd_t::storeu(reinterpret_cast<sint*>(s + width), q);
simd_t::storeu(reinterpret_cast<sint*>(s + width * 2), q);
simd_t::storeu(reinterpret_cast<sint*>(s + width * 3), q);
s += width * 4;
n -= width * 4;
} while (n >= width * 4);
s += n;
simd_t::storeu(reinterpret_cast<sint*>(s - width * 4), q);
simd_t::storeu(reinterpret_cast<sint*>(s - width * 3), q);
simd_t::storeu(reinterpret_cast<sint*>(s - width * 2), q);
simd_t::storeu(reinterpret_cast<sint*>(s - width), q);
return;
}
if constexpr (_Mysize == 8) {
if (n != 0) {
*s = val;
}
return;
}
if constexpr (_Mysize <= 4) {
if (n >= 4 / _Mysize) {
auto u32v = broadcast<uint32_t, T>(val);
auto delta = (n & (8 / _Mysize)) >> 1;
*reinterpret_cast<uint32_t*>(s) = u32v;
*reinterpret_cast<uint32_t*>(s + delta) = u32v;
if constexpr (_Mysize != 4) {
*reinterpret_cast<uint32_t*>(s + n - (4 / _Mysize) - delta) = u32v;
}
*reinterpret_cast<uint32_t*>(s + n - (4 / _Mysize)) = u32v;
return;
}
}
if constexpr (_Mysize == 4) {
return;
}
if constexpr (_Mysize == 2) {
if (n != 0) *s = val;
return;
}
if constexpr (_Mysize == 1) {
if (n != 0) {
*s = val;
if (n != 1) {
*reinterpret_cast<uint16_t*>(s + n - 2) = broadcast<uint16_t, uint8_t>(val);
}
}
return;
}
}
_WJR_ALGO_END
#undef __WJR_ALIGN64byte
#endif // _WJR_FAST_MEMSET

#ifndef __WJR_ALGO_MEM_ALL_H
#error "This file should not be included directly. Include <wjr/algo/mem-all.h> instead."
#endif // __WJR_ALGO_MEM_ALL_H
#if defined(_WJR_FAST_MEMSKIPW)
_WJR_ALGO_BEGIN
struct __memskipw_table {
constexpr __memskipw_table() : m_table() {
for (auto i = 0; i < 256; ++i) {
m_table[i] = (i >= 9 && i <= 13) || i == 32;
}
}
WJR_PURE constexpr bool operator[](char c) const {
return m_table[wjr::make_unsigned_v(c)];
}
private:
bool m_table[256];
};
constexpr static __memskipw_table __memskipw_table_instance = {};
#define WJR_SIMD_IS_BACKWARD 0
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const char* __memskipw(const char* s, const char* e) {
if (WJR_UNLIKELY(s == e)) return s;
if(WJR_LIKELY(!__memskipw_table_instance[*s])) return s;
++s;
if (WJR_LIKELY(s + 3 <= e)) {
if (!__memskipw_table_instance[*s]) return s;
if (!__memskipw_table_instance[s[1]]) return s + 1;
if (!__memskipw_table_instance[s[2]]) return s + 2;
s += 3;
if (WJR_LIKELY(s + 16 <= e)) {
const auto simd_32 = simd::sse::set1_epi8(32);
const auto simd_9 = simd::sse::set1_epi8(9);
const auto simd_5 = simd::sse::set1_epi8(4);
do {
WJR_SIMD_LOADU(simd::sse, x, s);
auto y = simd::sse::cmpeq(x, simd_32, uint8_t());
auto z = simd::sse::sub(x, simd_9, uint8_t());
z = simd::sse::cmple(z, simd_5, uint8_t());
auto r = simd::sse::movemask_epi8(simd::sse::Or(y, z));
if (WJR_LIKELY(r != simd::sse::mask())) {
return WJR_SIMD_FIRST_ZERO_PTR(s, r);
}
s += 16;
} while (s <= e);
}
}
for (; s < e && __memskipw_table_instance[*s]; ++s);
return s;
}
#undef WJR_SIMD_IS_BACKWARD
_WJR_ALGO_END
#endif // _WJR_FAST_MEMSKIPW

#endif // __WJR_ALGO_MEM_ALL_H
_WJR_ALGO_BEGIN
#if defined(_WJR_FAST_MEMCHR)
template const uint8_t* __large__memchr(
const uint8_t*, uint8_t, size_t, std::equal_to<>) noexcept;
template const uint8_t* __large__memrchr(
const uint8_t*, uint8_t, size_t, std::equal_to<>) noexcept;
template const uint8_t* __large__memchr(
const uint8_t*, uint8_t, size_t, std::not_equal_to<>) noexcept;
template const uint8_t* __large__memrchr(
const uint8_t*, uint8_t, size_t, std::not_equal_to<>) noexcept;
#endif // _WJR_FAST_MEMCHR
#if defined(_WJR_FAST_MEMEQ)
template bool __large__memeq(const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
template bool __large__memeq(const uint8_t*, const uint8_t*, size_t, std::not_equal_to<>) noexcept;
#endif // _WJR_FAST_MEMEQ
#if defined(_WJR_FAST_MEMMIS)
template const uint8_t* __large__memmis(
const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
template const uint8_t* __large__memrmis(
const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;
#endif // _WJR_FAST_MEMMIS
_WJR_ALGO_END


#pragma once
#ifndef __WJR_OPTIMIZER_H
#define __WJR_OPTIMIZER_H
_WJR_BEGIN
template<size_t N>
constexpr size_t _Get_max_bytes_num() {
#if defined(WJR_X86_64)
if constexpr (N % 8 == 0)return 8;
else
#endif
if constexpr (N % 4 == 0)return 4;
else if constexpr (N % 2 == 0)return 2;
else return 1;
}
template<size_t C, typename _Ty>
inline size_t _Get_bytes_num(const _Ty& val) {
constexpr size_t N = sizeof(_Ty);
constexpr size_t M = _Get_max_bytes_num<C>();
const auto ptr = reinterpret_cast<const uint8_t*>(&val);
if constexpr (N == C) {
if constexpr (N == 1) {
return 1u;
}
else if constexpr (N == 2) {
return (ptr[0] == ptr[1]) ? 1u : 2u;
}
else if constexpr (N == 3) {
return (*reinterpret_cast<const uint16_t*>(ptr) == *reinterpret_cast<const uint16_t*>(ptr + 1)) ? 1u : 0u;
}
else if constexpr (N == 4) {
auto x = *reinterpret_cast<const uint32_t*>(ptr);
return ((x >> 8) == (x & 0x00FFFFFF)) ? 1u : 4u;
}
else if constexpr (N == 5) {
return (*reinterpret_cast<const uint32_t*>(ptr) == *reinterpret_cast<const uint32_t*>(ptr + 1)) ? 1u : 0u;
}
else if constexpr (N == 6) {
return (*reinterpret_cast<const uint32_t*>(ptr) == *reinterpret_cast<const uint32_t*>(ptr + 2)) ? 2u : 6u;
}
else if constexpr (N == 7) {
auto x = *reinterpret_cast<const uint32_t*>(ptr);
auto y = *reinterpret_cast<const uint32_t*>(ptr + 4);
return ((x >> 8) == (x & 0x00FFFFFF)) && ((y >> 8) == (y & 0x00FFFFFF)) ? 1u : 0u;
}
else if constexpr (N == 8) {
auto x = *reinterpret_cast<const uint64_t*>(ptr);
return ((x >> 8) == (x & 0x00FFFFFFFFFFFFFF)) ? 1u : 8u;
}
else {
if (!is_constant_p(val)) {
return 0u;
}
else {
for (size_t i = 0; i < N - M; ++i) {
if (ptr[i] != ptr[i + M]) {
return 0u;
}
}
for (size_t i = 0; i < M - 1; ++i) {
if (ptr[i] != ptr[i + 1]) {
return M;
}
}
return 1;
}
}
}
else {
if constexpr (N <= 8) {
constexpr uint8_t zero[8] = { 0,0,0,0,0,0,0,0 };
return ::memcmp(ptr, zero, N) == 0 ? 1 : 0;
}
else {
if (!is_constant_p(val)) {
return 0;
}
else {
for (size_t i = 0; i < N; ++i) {
if (ptr[i] != 0) {
return 0;
}
}
return 1;
}
}
}
}
template<typename T, typename U,
bool =
std::is_same_v<bool, std::remove_reference_t<U>> >= std::is_same_v<bool, T> &&
is_arithmetics_v<T, std::remove_reference_t<U>> &&
!std::is_volatile_v<T> &&
!std::is_volatile_v<std::remove_reference_t<U>>
>
struct __is_byte_constructible {
constexpr static bool is_copy = sizeof(T) == sizeof(U);
constexpr static bool is_fill = true;
constexpr static T get(const U& val) {
return static_cast<T>(val);
}
};
template<typename T, bool =
std::is_trivially_copyable_v<T> &&
!std::is_volatile_v<T>>
struct __is_byte_copy_constructible_helper {
constexpr static bool is_copy = true;
constexpr static bool is_fill = !std::is_empty_v<T>;
constexpr static const T& get(const T& val){
return val;
}
};
template<typename T>
struct __is_byte_copy_constructible_helper<T, false> {
constexpr static bool is_copy = false;
constexpr static bool is_fill = false;
};
template<typename T, bool =
std::is_trivially_copyable_v<T> &&
!std::is_volatile_v<T>>
struct __is_byte_move_constructible_helper {
constexpr static bool is_copy = true;
constexpr static bool is_fill = !std::is_empty_v<T>;
constexpr static const T& get(const T& val) {
return val;
}
};
template<typename T>
struct __is_byte_move_constructible_helper<T, false> {
constexpr static bool is_copy = false;
constexpr static bool is_fill = false;
};
template<typename T, typename U>
struct __is_byte_constructible<T, U, false> {
constexpr static bool is_copy = false;
constexpr static bool is_fill = false;
};
template<typename T>
struct __is_byte_constructible<T, T, false> : __is_byte_copy_constructible_helper<T> {};
template<typename T>
struct __is_byte_constructible<T, const T, false> : __is_byte_copy_constructible_helper<T> {};
template<typename T>
struct __is_byte_constructible<T, T&, false> : __is_byte_copy_constructible_helper<T> {};
template<typename T>
struct __is_byte_constructible<T, const T&, false> : __is_byte_copy_constructible_helper<T> {};
template<typename T>
struct __is_byte_constructible<T, T&&, false> : __is_byte_move_constructible_helper<T> {};
template<typename T, typename U>
struct is_byte_constructible : __is_byte_constructible<std::remove_reference_t<T>, U> {};
template<typename T, typename U>
struct is_byte_assignable : __is_byte_constructible<std::remove_reference_t<T>, U> {};
_WJR_END
#endif // __WJR_OPTIMIZER_H

_WJR_ALGO_BEGIN
#if defined(_WJR_FAST_MEMCHR)
template<typename T, typename U, typename _Pred>
struct __has_fast_memchr : std::conjunction<
is_any_of<_Pred, std::equal_to<>, std::not_equal_to<>>,
is_comparable<T, U, _Pred>,
is_integrals<T, U>,
is_any_index_of<sizeof(T), 1, 2, 4>
> {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memchr_v = __has_fast_memchr<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred>
struct __has_fast_memrchr : __has_fast_memchr<T, U, _Pred> {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrchr_v = __has_fast_memrchr<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memchr_v<T, U, _Pred>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* memchr(
const T* s, U val, size_t n, _Pred pred) noexcept {
auto p = is_possible_memory_comparable<T>(val, pred);
if (p == ipmc_result::none) {
return s + n;
}
if (p == ipmc_result::all) {
return s;
}
using value_type = std::make_unsigned_t<make_integral_t<T>>;
auto __s = reinterpret_cast<const value_type*>(s);
auto __val = static_cast<value_type>(static_cast<T>(val));
return reinterpret_cast<const T*>(__memchr(__s, __val, n, pred));
}
template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memrchr_v<T, U, _Pred>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* memrchr(const T* s, U val, size_t n, _Pred pred) {
auto p = is_possible_memory_comparable<T>(val, pred);
if (p == ipmc_result::none) {
return s;
}
if (p == ipmc_result::all) {
return s + n;
}
using value_type = std::make_unsigned_t<make_integral_t<T>>;
auto __s = reinterpret_cast<const value_type*>(s);
auto __val = static_cast<value_type>(val);
return reinterpret_cast<const T*>(__memrchr(__s, __val, n, pred));
}
#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memchr : std::conjunction<
std::is_same<_Pred, std::equal_to<>>,
is_comparable<T, U, _Pred>,
is_integrals<T, U>,
is_any_index_of<sizeof(T), sizeof(char), sizeof(wchar_t)>
> {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memchr_v = __has_fast_memchr<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred>
struct __has_fast_memrchr : std::false_type {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrchr_v = __has_fast_memrchr<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memchr_v<T, U, _Pred>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* memchr(const T* s, U val, size_t n, _Pred pred) {
auto p = is_possible_memory_comparable<T>(val, pred);
if (p == ipmc_result::none) {
return s + n;
}
if (p == ipmc_result::all) {
return s;
}
if constexpr (sizeof(T) == sizeof(char)) {
using value_type = char;
auto __s = reinterpret_cast<const value_type*>(s);
auto __val = static_cast<value_type>(val);
auto pos = ::memchr(__s, __val, n);
if (pos == nullptr) {
return s + n;
}
return reinterpret_cast<const T*>(pos);
}
else {
using value_type = wchar_t;
auto __s = reinterpret_cast<const value_type*>(s);
auto __val = static_cast<value_type>(val);
auto pos = ::wmemchr(__s, __val, n);
if (pos == nullptr) {
return s + n;
}
return reinterpret_cast<const T*>(pos);
}
}
template<typename T, typename U, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* memrchr(const T* s, U val, size_t n, _Pred pred);
#endif // _WJR_FAST_MEMCHR
#if defined(_WJR_FAST_MEMEQ)
template<typename T, typename U, typename _Pred>
struct __has_fast_memeq : std::conjunction<
is_memory_comparable<T, U, _Pred>,
is_integrals<T, U>,
is_any_index_of<sizeof(T), 1, 2, 4>
> {};
template<typename T, typename U>
struct __has_fast_memeq<T, U, std::equal_to<>> : std::conjunction<
is_memory_comparable<T, U, std::equal_to<>>
> {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memeq_v = __has_fast_memeq<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memeq_v<T, U, _Pred>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) bool memeq(const T* s0, const U* s1, size_t n, _Pred pred) {
if constexpr (is_any_of_v<_Pred, std::equal_to<>>) {
using value_type = uint8_t;
auto __s0 = reinterpret_cast<const value_type*>(s0);
auto __s1 = reinterpret_cast<const value_type*>(s1);
return __memeq(__s0, __s1, n * sizeof(T), pred);
}
else {
using value_type = make_integral_t<T>;
auto __s0 = reinterpret_cast<const value_type*>(s0);
auto __s1 = reinterpret_cast<const value_type*>(s1);
return __memeq(__s0, __s1, n, pred);
}
}
#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memeq : std::false_type {};
template<typename T, typename U>
struct __has_fast_memeq<T, U, std::equal_to<>> : std::conjunction<
is_memory_comparable<T, U, std::equal_to<>>
> {};
template<typename T, typename U, typename _Pred>
inline constexpr bool __has_fast_memeq_v = __has_fast_memeq<T, U, _Pred>::type::value;
template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memeq_v<T, U, _Pred>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) bool memeq(const T* s0, const U* s1, size_t n, WJR_MAYBE_UNUSED _Pred pred) {
return ::memcmp(s0, s1, n * sizeof(T)) == 0;
}
#endif // _WJR_FAST_MEMEQ
#if defined(_WJR_FAST_MEMMIS)
template<typename T, typename U, typename _Pred>
struct __has_fast_memmis : std::conjunction<
is_memory_comparable<T, U, _Pred>,
is_integrals<T, U>,
is_any_index_of<sizeof(T), 1, 2, 4>
> {};
template<typename T, typename U>
struct __has_fast_memmis<T, U, std::equal_to<>> : std::conjunction<
is_memory_comparable<T, U, std::equal_to<>>
> {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memmis_v = __has_fast_memmis<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred>
struct __has_fast_memrmis : __has_fast_memmis<T, U, _Pred> {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrmis_v = __has_fast_memrmis<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred, std::enable_if_t<
__has_fast_memmis_v<T, U, _Pred>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* memmis(const T* s0, const U* s1, size_t n, _Pred pred) {
if constexpr (is_any_of_v<_Pred, std::equal_to<>>) {
using value_type = uint8_t;
auto __s0 = reinterpret_cast<const value_type*>(s0);
auto __s1 = reinterpret_cast<const value_type*>(s1);
auto __pos =
__memmis(__s0, __s1, n * sizeof(T), pred);
auto __dif = (__pos - __s0) / sizeof(T);
return s0 + __dif;
}
else {
using value_type = make_integral_t<T>;
auto __s0 = reinterpret_cast<const value_type*>(s0);
auto __s1 = reinterpret_cast<const value_type*>(s1);
return reinterpret_cast<const T*>(__memmis(__s0, __s1, n, pred));
}
}
template<typename T, typename U, typename _Pred, std::enable_if_t<
__has_fast_memrmis_v<T, U, _Pred>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* memrmis(const T* s0, const U* s1, size_t n, _Pred pred) {
if constexpr (is_any_of_v<_Pred, std::equal_to<>>) {
using value_type = uint8_t;
auto __s0 = reinterpret_cast<const value_type*>(s0);
auto __s1 = reinterpret_cast<const value_type*>(s1);
auto __pos =
__memrmis(__s0, __s1, n * sizeof(T), pred);
auto __e0 = __s0 + n * sizeof(T);
auto __dif = (__e0 - __pos) / sizeof(T);
return (s0 + n) - __dif;
}
else {
using value_type = make_integral_t<T>;
auto __s0 = reinterpret_cast<const value_type*>(s0);
auto __s1 = reinterpret_cast<const value_type*>(s1);
return reinterpret_cast<const T*>(__memrmis(__s0, __s1, n, pred));
}
}
#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memmis : std::false_type {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memmis_v = __has_fast_memmis<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred>
struct __has_fast_memrmis : __has_fast_memmis<T, U, _Pred> {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrmis_v = __has_fast_memrmis<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* memmis(const T* s0, const U* s1, size_t n, _Pred pred);
template<typename T, typename U, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* memrmis(const T* s0, const U* s1, size_t n, _Pred pred);
#endif // _WJR_FAST_MEMMIS
#if defined(_WJR_FAST_MEMMIS)
template<typename T, typename U, typename _Pred>
struct __has_fast_memcmp : std::false_type {};
template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::less<>> : std::conjunction<
is_memory_comparable<T, U, std::less<>>,
__has_fast_memmis<T, U, std::equal_to<>>
> {};
template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::greater<>> : __has_fast_memcmp<T, U, std::less<>> {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memcmp_v = __has_fast_memcmp<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred>
struct __has_fast_memrcmp : std::false_type {};
template<typename T, typename U>
struct __has_fast_memrcmp<T, U, std::less<>> : std::conjunction<
is_memory_comparable<T, U, std::less<>>,
__has_fast_memrmis<T, U, std::equal_to<>>
> {};
template<typename T, typename U>
struct __has_fast_memrcmp<T, U, std::greater<>> : __has_fast_memrcmp<T, U, std::less<>> {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrcmp_v = __has_fast_memrmis<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred, std::enable_if_t<
__has_fast_memcmp_v<T, U, _Pred>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) int memcmp(const T* s0, const U* s1, size_t n, _Pred) {
if constexpr (std::is_same_v<_Pred, std::greater<>>) {
return -wjr::algo::memcmp(s0, s1, n, std::less<>{});
}
else {
auto mis = wjr::algo::memmis(s0, s1, n, std::equal_to<>{});
if (mis != s0 + n) {
auto pos = mis - s0;
return s0[pos] < s1[pos] ? -1 : 1;
}
return 0;
}
}
template<typename T, typename U, typename _Pred, std::enable_if_t<
__has_fast_memrmis_v<T, U, _Pred>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) int memrcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
if constexpr (std::is_same_v<_Pred, std::greater<>>) {
return -wjr::algo::memrcmp(s0, s1, n, std::less<>{});
}
else {
auto mis = wjr::algo::memrmis(s0, s1, n, std::equal_to<>{});
if (mis != s0 + n) {
auto pos = mis - s0;
return s0[pos] < s1[pos] ? -1 : 1;
}
return 0;
}
}
#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memcmp : std::false_type {};
template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::less<>> : std::conjunction<
is_memory_comparable<T, U, std::less<>>,
std::conjunction<
is_memory_comparable<T, U, std::equal_to<>>,
std::bool_constant<sizeof(T) == 1>,
std::is_unsigned<T>,
std::is_unsigned<U>
>
> {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memcmp_v = __has_fast_memcmp<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred>
struct __has_fast_memrcmp : std::false_type {};
template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrcmp_v = __has_fast_memrcmp<T, U, _Pred>::value;
template<typename T, typename U, typename _Pred, std::enable_if_t<
__has_fast_memcmp_v<T, U, _Pred>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) int memcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
using value_type = uint8_t;
auto __s0 = reinterpret_cast<const value_type*>(s0);
auto __s1 = reinterpret_cast<const value_type*>(s1);
return ::memcmp(__s0, __s1, n);
}
template<typename T, typename U, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) int memrcmp(const T* s0, const U* s1, size_t n, _Pred pred);
#endif // _WJR_FAST_MEMCMP
#if defined(_WJR_FAST_MEMCNT)
#else
template<typename T, typename U>
struct __has_fast_memcnt : std::false_type {};
template<typename T, typename U>
constexpr bool __has_fast_memcnt_v = __has_fast_memcnt<T, U>::value;
#endif // _WJR_FAST_MEMCNT
template<template<typename X, typename Y> typename TEST, typename T, typename U>
struct __has_fast_memset_helper : std::conjunction<
std::bool_constant<TEST<T, U>::is_fill>
> {};
template<template<typename X, typename Y> typename TEST, typename T, typename U>
constexpr bool __has_fast_memset_helper_v = __has_fast_memset_helper<TEST, T, U>::value;
template<template<typename X, typename Y> typename TEST, typename T, typename U>
void __memset_helper(T* s, const U& val, size_t n) {
if (is_constant_p(n) && n <= 4 / sizeof(T)) {
std::fill_n(s, n, val);
return;
}
const auto& _Val = TEST<T, const U&>::get(val);
auto __bytes_num = _Get_bytes_num<sizeof(T)>(_Val);
#if defined(_WJR_FAST_MEMSET)
constexpr auto __max_bytes_num = _Get_max_bytes_num<sizeof(T)>();
if (__bytes_num == 0) {
std::fill_n(s, n, val);
return;
}
#else
constexpr auto __max_bytes_num = 1;
if (__bytes_num != 1) {
std::fill_n(s, n, val);
return;
}
#endif
using value_type = uint_t<__max_bytes_num * 8>;
auto __s = reinterpret_cast<value_type*>(s);
auto __val = *reinterpret_cast<const value_type*>(&_Val);
static_assert(std::is_same_v<decltype(__val), value_type>, "type mismatch");
#if defined(_WJR_FAST_MEMSET)
__memset(__s, __val, n * (sizeof(T) / __max_bytes_num));
#else
::memset(__s, __val, n * (sizeof(T) / __max_bytes_num));
#endif
}
template<typename T, typename U>
struct __has_fast_construct_memset : __has_fast_memset_helper<is_byte_constructible, T, U> {};
template<typename T, typename U>
constexpr bool __has_fast_construct_memset_v = __has_fast_construct_memset<T, U>::value;
template<typename T, typename U>
void construct_memset(T* s, const U& val, size_t n) {
__memset_helper<is_byte_constructible>(s, val, n);
}
template<typename T, typename U>
struct __has_fast_assign_memset : __has_fast_memset_helper<is_byte_assignable, T, U> {};
template<typename T, typename U>
constexpr bool __has_fast_assign_memset_v = __has_fast_assign_memset<T, U>::value;
template<typename T, typename U>
void assign_memset(T* s, const U& val, size_t n) {
__memset_helper<is_byte_assignable>(s, val, n);
}
template<template<typename X, typename Y> typename TEST, typename T, typename U>
struct __has_fast_memcpy_helper : std::conjunction <
std::bool_constant<TEST<T, U>::is_copy>
> {};
template<template<typename X, typename Y> typename TEST, typename T, typename U>
constexpr bool __has_fast_memcpy_helper_v = __has_fast_memcpy_helper<TEST, T, U>::value;
template<template<typename X, typename Y> typename TEST, typename T, typename U>
void __memcpy_helper(T* s, const U* t, size_t n) {
static_assert(sizeof(T) == sizeof(U), "type mismatch");
#if defined(_WJR_FAST_MEMCPY)
#else
::memcpy(s, t, n * sizeof(T));
#endif
}
template<template<typename X, typename Y> typename TEST, typename T, typename U>
void __memmove_helper(T* s, const U* t, size_t n) {
static_assert(sizeof(T) == sizeof(U), "type mismatch");
#if defined(_WJR_FAST_MEMCPY)
#else
::memmove(s, t, n * sizeof(T));
#endif
}
template<typename T, typename U>
struct __has_fast_construct_memcpy : __has_fast_memcpy_helper<is_byte_constructible, T, U> {};
template<typename T, typename U>
constexpr bool __has_fast_construct_memcpy_v = __has_fast_construct_memcpy<T, U>::value;
template<typename T, typename U>
void construct_memcpy(T* s, const U* t, size_t n) {
__memcpy_helper<is_byte_constructible>(s, t, n);
}
template<typename T, typename U>
void construct_memmove(T* s, const U* t, size_t n) {
__memmove_helper<is_byte_constructible>(s, t, n);
}
template<typename T, typename U>
struct __has_fast_assign_memcpy : __has_fast_memcpy_helper<is_byte_assignable, T, U> {};
template<typename T, typename U>
inline constexpr bool __has_fast_assign_memcpy_v = __has_fast_assign_memcpy<T, U>::value;
template<typename T, typename U>
void assign_memcpy(T* s, const U* t, size_t n) {
__memcpy_helper<is_byte_assignable>(s, t, n);
}
template<typename T, typename U>
void assign_memmove(T* s, const U* t, size_t n) {
__memmove_helper<is_byte_assignable>(s, t, n);
}
#if defined(_WJR_FAST_MEMSKIPW)
template<typename T>
struct __has_fast_memskipw : std::conjunction<
is_integrals<T>,
is_any_index_of<sizeof(T), 1>
> {};
template<typename T>
constexpr bool __has_fast_memskipw_v = __has_fast_memskipw<T>::value;
template<typename T, std::enable_if_t<__has_fast_memskipw_v<T>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* memskipw(const T* s, const T* e) {
using value_type = char;
return reinterpret_cast<const T*>(
__memskipw(reinterpret_cast<const value_type*>(s),
reinterpret_cast<const value_type*>(e)));
}
#else
template<typename T>
struct __has_fast_memskipw : std::false_type {};
template<typename T>
inline constexpr bool __has_fast_memskipw_v = __has_fast_memskipw<T>::value;
template<typename T>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const T* memskipw(const T* s, const T* e);
#endif // _WJR_FAST_MEMSKIPW
_WJR_ALGO_END
#endif // __WJR_ALGO_ALOG_H

_WJR_BEGIN
template<typename _Iter, typename _Val, typename _Pred,
typename _Iter_value = iter_val_t<_Iter>>
struct __has_fast_find : std::conjunction<
is_contiguous_iter<_Iter>,
std::conditional_t<
wjr::is_reverse_iterator_v<_Iter>,
algo::__has_fast_memrchr<_Iter_value, _Val, _Pred>,
algo::__has_fast_memchr<_Iter_value, _Val, _Pred>
>
> {};
template<typename _Iter, typename _Val, typename _Pred>
inline constexpr bool __has_fast_find_v = __has_fast_find<_Iter, _Val, _Pred>::value;
template<typename _Iter, typename _Ty, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) _Iter do_find(
_Iter _First, _Iter _Last, const _Ty& _Val, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_find_v<_Iter, _Ty, _Pred>) {
const auto n = std::distance(_First, _Last);
if constexpr (!wjr::is_reverse_iterator_v<_Iter>) {
const auto first = wjr::get_address(_First);
return _First + (algo::memchr(first, _Val, n, pred) - first);
}
else {
const auto first = wjr::get_address(_Last - 1);
return _Last - (algo::memrchr(first, _Val, n, pred) - first);
}
}
}
if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
return std::find(_First, _Last, _Val);
}
else {
for (; _First != _Last; ++_First) {
if (pred(*_First, _Val)) {
return _First;
}
}
return _Last;
}
}
template<typename _Iter, typename _Ty>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) _Iter do_find(_Iter _First, _Iter _Last, const _Ty& _Val) {
return wjr::do_find(_First, _Last, _Val, std::equal_to<>{});
}
template<typename _Iter, typename _Pr>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) _Iter do_find_if(_Iter _First, _Iter _Last, _Pr _Pred) {
return std::find_if(_First, _Last, _Pred);
}
template<typename _Iter, typename _Pr>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) _Iter do_find_if_not(_Iter _First, _Iter _Last, _Pr _Pred) {
return std::find_if_not(_First, _Last, _Pred);
}
template<typename _Iter, typename _Val,
typename _Iter_value = iter_val_t<_Iter>>
struct __has_fast_count : std::conjunction<
wjr::is_contiguous_iter<_Iter>,
algo::__has_fast_memcnt<_Iter_value, _Val>
> {};
template<typename _Iter, typename _Val>
inline constexpr bool __has_fast_count_v = __has_fast_count<_Iter, _Val>::value;
template<typename _Iter, typename _Ty>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) typename std::iterator_traits<_Iter>::difference_type
do_count(_Iter _First, _Iter _Last, const _Ty& _Val) {
if (!wjr::is_constant_evaluated()) {
#if defined(_WJR_FAST_MEMCNT)
if constexpr (__has_fast_count_v<_Iter, _Ty>) {
const auto n = _Last - _First;
if constexpr (!wjr::is_reverse_iterator_v<_Iter>) {
const auto first = wjr::get_address(_First);
return algo::memcnt(first, _Val, n);
}
else {
const auto first = wjr::get_address(_Last - 1);
return algo::memcnt(first, _Val, n);
}
}
#endif // _WJR_FAST_MEMCNT
}
return std::count(_First, _Last, _Val);
}
template<typename _Iter, typename _Pr>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) typename std::iterator_traits<_Iter>::difference_type
do_count_if(_Iter _First, _Iter _Last, _Pr _Pred) {
return std::count_if(_First, _Last, _Pred);
}
template<typename _Iter1, typename _Iter2, typename _Pred,
typename _Iter_value1 = iter_val_t<_Iter1>,
typename _Iter_value2 = iter_val_t<_Iter2>>
struct __has_fast_equal_helper : std::conjunction<
wjr::is_contiguous_iter<_Iter1>,
wjr::is_contiguous_iter<_Iter2>,
std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
algo::__has_fast_memeq<_Iter_value1, _Iter_value2, _Pred>
> {};
template<typename _Iter1, typename _Iter2, typename _Pred>
struct __has_fast_equal : std::bool_constant<__has_fast_equal_helper<_Iter1, _Iter2, _Pred>::value> {};
template<typename _Iter1, typename _Iter2, typename _Pred>
inline constexpr bool __has_fast_equal_v = __has_fast_equal<_Iter1, _Iter2, _Pred>::value;
template<typename _Iter1, typename _Iter2, typename _Pred,
typename _Iter_value1 = iter_val_t<_Iter1>,
typename _Iter_value2 = iter_val_t<_Iter2>>
struct __has_fast_mismatch : std::conjunction <
wjr::is_contiguous_iter<_Iter1>,
wjr::is_contiguous_iter<_Iter2>,
std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
std::conditional_t<
wjr::is_reverse_iterator_v<_Iter1>,
algo::__has_fast_memrmis<_Iter_value1, _Iter_value2, _Pred>,
algo::__has_fast_memmis<_Iter_value1, _Iter_value2, _Pred>
>
> {};
template<typename _Iter1, typename _Iter2, typename _Pred>
inline constexpr bool __has_fast_mismatch_v = __has_fast_mismatch<_Iter1, _Iter2, _Pred>::value;
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) std::pair<_Iter1, _Iter2> do_mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
const auto n = std::distance(_First1, _Last1);
if (WJR_UNLIKELY(n == 0)) { return std::make_pair(_First1, _First2); }
if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
const auto first1 = wjr::get_address(_First1);
const auto first2 = wjr::get_address(_First2);
auto pos = algo::memmis(first1, first2, n, pred) - first1;
return std::make_pair(_First1 + pos, _First2 + pos);
}
else {
const auto first1 = wjr::get_address(_Last1 - 1);
const auto _Last2 = _First2 + n;
const auto first2 = wjr::get_address(_Last2 - 1);
auto pos = (first1 + n) - algo::memrmis(first1, first2, n, pred);
return std::make_pair(_First1 + pos, _First2 + pos);
}
}
}
return std::mismatch(_First1, _Last1, _First2, pred);
}
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) std::pair<_Iter1, _Iter2> do_mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
const auto n = _Last1 - _First1;
const auto m = _Last2 - _First2;
const auto _M = n < m ? n : m;
return wjr::do_mismatch(_First1, _First1 + _M, _First2, pred);
}
}
return std::mismatch(_First1, _Last1, _First2, _Last2, pred);
}
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) std::pair<_Iter1, _Iter2> do_mismatch(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) {
return wjr::do_mismatch(_First1, _Last1, _First2, std::equal_to<>{});
}
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) std::pair<_Iter1, _Iter2> do_mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
return wjr::do_mismatch(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
}
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
const auto n = _Last1 - _First1;
if (WJR_UNLIKELY(n == 0)) { return true; }
if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
const auto first1 = wjr::get_address(_First1);
const auto first2 = wjr::get_address(_First2);
return algo::memeq(first1, first2, n, pred);
}
else {
const auto first1 = wjr::get_address(_Last1 - 1);
const auto _Last2 = _First2 + n;
const auto first2 = wjr::get_address(_Last2 - 1);
return algo::memeq(first1, first2, n, pred);
}
}
}
return std::equal(_First1, _Last1, _First2, pred);
}
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_equal(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
const auto n = std::distance(_First1, _Last1);
const auto m = std::distance(_First2, _Last2);
if (n != m) { return false; }
return wjr::do_equal(_First1, _Last1, _First2, pred);
}
}
return std::equal(_First1, _Last1, _First2, _Last2, pred);
}
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) {
return wjr::do_equal(_First1, _Last1, _First2, std::equal_to<>{});
}
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
return wjr::do_equal(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
}
template<typename _Iter1, typename _Iter2, typename _Pred,
typename _Iter_value1 = iter_val_t<_Iter1>,
typename _Iter_value2 = iter_val_t<_Iter2>>
struct __has_fast_compare : std::conjunction <
wjr::is_contiguous_iter<_Iter1>,
wjr::is_contiguous_iter<_Iter2>,
std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
std::conditional_t<
wjr::is_reverse_iterator_v<_Iter1>,
algo::__has_fast_memrcmp<_Iter_value1, _Iter_value2, _Pred>,
algo::__has_fast_memcmp<_Iter_value1, _Iter_value2, _Pred>
>
> {};
template<typename _Iter1, typename _Iter2, typename _Pred>
inline constexpr bool __has_fast_compare_v = __has_fast_compare<_Iter1, _Iter2, _Pred>::value;
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) int do_compare(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_compare_v<_Iter1, _Iter2, _Pred>) {
const auto n = std::distance(_First1, _Last1);
const auto m = std::distance(_First2, _Last2);
const auto cl = std::min(n, m);
if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
const auto first1 = wjr::get_address(_First1);
const auto first2 = wjr::get_address(_First2);
auto r = algo::memcmp(first1, first2, cl, pred);
if (r != 0) return r;
}
else {
const auto first1 = wjr::get_address(_Last1 - 1);
const auto _Last2 = _First2 + n;
const auto first2 = wjr::get_address(_Last2 - 1);
auto r = algo::memrcmp(first1, first2, cl, pred);
if (r != 0) return r;
}
return n < m ? -1 : (n > m ? 1 : 0);
}
}
for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, ++_First2) {
if (pred(*_First1, *_First2)) {
return -1;
}
if (pred(*_First2, *_First1)) {
return 1;
}
}
return _First1 == _Last1 ? (_First2 == _Last2 ? 0 : -1) : 1;
}
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) int do_compare(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
return wjr::do_compare(_First1, _Last1, _First2, _Last2, std::less<>{});
}
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_lexicographical_compare(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
return wjr::do_compare(_First1, _Last1, _First2, _Last2, pred) < 0;
}
template<typename _Iter1, typename _Iter2>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE, CONSTEXPR20) bool do_lexicographical_compare(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
return wjr::do_lexicographical_compare(_First1, _Last1, _First2, _Last2, std::less<>{});
}
template<typename _Iter, typename _Val,
typename _Iter_ref = iter_ref_t<_Iter>>
struct __has_fast_fill : std::conjunction<
is_contiguous_iter<_Iter>,
algo::__has_fast_assign_memset<remove_cref_t<_Iter_ref>, add_cref_t<_Val>>
> {};
template<typename _Iter, typename _Val>
inline constexpr bool __has_fast_fill_v = __has_fast_fill<_Iter, _Val>::value;
template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_fill(_Iter _First, _Iter _Last, const _Val& value) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_fill_v<_Iter, _Val>) {
const auto n = std::distance(_First, _Last);
if constexpr (!is_reverse_iterator_v<_Iter>) {
const auto first = wjr::get_address(_First);
algo::assign_memset(first, value, n);
}
else {
if (WJR_UNLIKELY(n == 0)) { return; }
const auto first = wjr::get_address(_Last - 1);
algo::assign_memset(first, value, n);
}
return;
}
}
return std::fill(_First, _Last, value);
}
template<typename _Iter, typename _Size, typename _Val>
WJR_CONSTEXPR20 _Iter do_fill_n(_Iter _First, _Size count, const _Val& value) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_fill_v<_Iter, _Val>) {
if (count <= 0) { return _First; }
wjr::fill(_First, _First + count, value);
return _First + count;
}
}
return std::fill_n(_First, count, value);
}
template<typename _Input, typename _Output,
typename _Input_ref = iter_ref_t<_Input>,
typename _Output_ref = iter_ref_t<_Output>>
struct __has_fast_copy : std::conjunction<
is_contiguous_iter<_Input>,
is_contiguous_iter<_Output>,
std::bool_constant<wjr::is_reverse_iterator_v<_Input> == wjr::is_reverse_iterator_v<_Output>>,
algo::__has_fast_assign_memcpy<remove_ref_t<_Output_ref>, _Input_ref>
> {};
template<typename _Input, typename _Output>
inline constexpr bool __has_fast_copy_v = __has_fast_copy<_Input, _Output>::value;
template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_copy(_Input _First1, _Input _Last1, _Output _First2) {
static_assert(!std::is_const_v<iter_ref_t<_Output>>, "Output iterator must not be const");
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_copy_v<_Input, _Output>) {
const auto n = std::distance(_First1, _Last1);
if (WJR_UNLIKELY(n == 0)) { return _First2; }
if constexpr (!wjr::is_reverse_iterator_v<_Input>) {
const auto first1 = wjr::get_address(_First1);
const auto first2 = wjr::get_address(_First2);
algo::assign_memcpy(first2, first1, n);
}
else {
const auto first1 = wjr::get_address(_Last1 - 1);
const auto _Last2 = _First2 + n;
const auto first2 = wjr::get_address(_Last2 - 1);
algo::assign_memmove(first2, first1, n);
}
return _First2 + n;
}
}
return std::copy(_First1, _Last1, _First2);
}
template<typename _Input, typename _Size, typename _Output>
WJR_CONSTEXPR20 _Output do_copy_n(_Input _First1, _Size count, _Output _First2) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_copy_v<_Input, _Output>) {
if (count <= 0) { return _First2; }
return wjr::copy(_First1, _First1 + count, _First2);
}
}
return std::copy_n(_First1, count, _First2);
}
template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_copy_backward(_Input _First1, _Input _Last1, _Output _Last2) {
return wjr::copy(std::make_reverse_iterator(_Last1),
std::make_reverse_iterator(_First1),
std::make_reverse_iterator(_Last2)).base();
}
template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_move(_Input _First1, _Input _Last1, _Output _First2) {
return wjr::copy(std::make_move_iterator(_First1), std::make_move_iterator(_Last1), _First2);
}
template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output do_move_backward(_Input _First1, _Input _Last1, _Output _Last2) {
return wjr::copy_backward(std::make_move_iterator(_First1), std::make_move_iterator(_Last1), _Last2);
}
template<typename _Iter, typename _Pred>
class default_searcher {
public:
WJR_CONSTEXPR20 default_searcher(_Iter _First, _Iter _Last, _Pred _Eq = _Pred())
: m_pair(std::move(_Eq), std::move(std::make_pair(_First, _Last))) {
}
template <typename _Iter2>
WJR_NODISCARD WJR_CONSTEXPR20 std::pair<_Iter2, _Iter2> operator()(
_Iter2 _First, _Iter2 _Last) const {
auto& pred = m_pair.first();
auto _Myfirst = m_pair.second().first;
auto _Mylast = m_pair.second().second;
if constexpr (is_random_iterator_v<_Iter> && is_random_iterator_v<_Iter2>) {
const auto n = std::distance(_First, _Last);
const auto m = std::distance(_Myfirst, _Mylast);
if (m == 0) {
return std::make_pair(_First, _First);
}
if (n < m) {
return std::make_pair(_Last, _Last);
}
const auto& _Val = *_Myfirst;
auto _Mid = _Last - (m - 1);
for (;; ++_First) {
_First = wjr::find(_First, _Mid, _Val, pred);
if (_First == _Mid) {
return std::make_pair(_Last, _Last);
}
if (wjr::equal(_Myfirst, _Mylast, _First, pred)) {
return std::make_pair(_First, _First + m);
}
}
}
else {
for (;; ++_First) { // loop until match or end of a sequence
auto _Mid = _First;
for (auto _Mid2 = _Myfirst;; ++_Mid, (void) ++_Mid2) {
if (_Mid2 == _Mylast) {
return std::make_pair(_First, _Mid);
}
if (_Mid == _Last) {
return std::make_pair(_Last, _Last);
}
if (!pred(*_Mid, *_Mid2)) {
break;
}
}
}
}
}
private:
compressed_pair<_Pred, std::pair<_Iter, _Iter>> m_pair;
};
template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter1 do_search(_Iter1 _First1, _Iter1 _Last1,
_Iter2 _First2, _Iter2 _Last2) {
return wjr::search(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
}
template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 _Iter1 do_search(_Iter1 _First1, _Iter1 _Last1,
_Iter2 _First2, _Iter2 _Last2, _Pred pred) {
return wjr::search(_First1, _Last1, wjr::default_searcher(_First2, _Last2, pred));
}
template<typename _Iter, typename _Searcher>
WJR_CONSTEXPR20 _Iter do_search(_Iter _First, _Iter _Last, const _Searcher& _Srch) {
return _Srch(_First, _Last).first;
}
template<typename _Iter, typename...Args,
std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int>>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, Args&&... args) {
#if defined(WJR_CPP_20)
std::construct_at(get_address(iter), std::forward<Args>(args)...);
#else
using value_type = iter_val_t<_Iter>;
(void)(::new (voidify(get_address(iter))) value_type(std::forward<Args>(args)...));
#endif // WJR_CPP_20
}
template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int>>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, default_construct_tag) {
using value_type = iter_val_t<_Iter>;
if (!is_constant_evaluated()) {
(void)(::new (voidify(get_address(iter))) value_type);
return;
}
if constexpr (std::is_trivially_default_constructible_v<value_type>) {
}
else {
construct_at(iter, value_construct_tag());
}
}
template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int>>
WJR_CONSTEXPR20 void do_construct_at(_Iter iter, value_construct_tag) {
wjr::construct_at(iter);
}
template<typename Alloc, typename _Iter, typename...Args,
std::enable_if_t<!is_iterator_v<wjr::remove_cvref_t<Alloc>>, int>>
WJR_CONSTEXPR20 void do_construct_at(Alloc& al, _Iter iter, Args&&...args) {
using pointer = iter_address_t<_Iter>;
if constexpr (is_default_allocator_construct_v<Alloc, pointer, Args...>) {
wjr::construct_at(iter, std::forward<Args>(args)...);
}
else {
if constexpr(sizeof...(Args) == 1){
if constexpr (is_any_of_v<default_construct_tag, Args...>
|| is_any_of_v<value_construct_tag, Args...>) {
return construct_at(al, iter);
}
}
std::allocator_traits<Alloc>::construct(al, get_address(iter), std::forward<Args>(args)...);
}
}
template<typename _Iter>
WJR_CONSTEXPR20 void do_destroy_at(_Iter ptr) {
using value_type = iter_val_t<_Iter>;
if constexpr (!std::is_trivially_destructible_v<value_type>) {
get_address(ptr)->~value_type();
}
}
template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_destroy_at(Alloc& al, _Iter iter) {
if constexpr (is_default_allocator_destroy_v<Alloc, _Iter>) {
wjr::destroy_at(iter);
}
else {
std::allocator_traits<Alloc>::destroy(al, get_address(iter));
}
}
template<typename _Iter>
WJR_CONSTEXPR20 void do_destroy(_Iter _First, _Iter _Last) {
using value_type = iter_val_t<_Iter>;
if constexpr (!std::is_trivially_destructible_v<value_type>) {
for (; _First != _Last; ++_First) {
wjr::destroy_at(_First);
}
}
}
template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_destroy(Alloc& al, _Iter _First, _Iter _Last) {
using value_type = iter_val_t<_Iter>;
if constexpr (!(is_default_allocator_destroy_v<Alloc, _Iter>
&& std::is_trivially_destructible_v<value_type>)) {
for (; _First != _Last; ++_First) {
wjr::destroy_at(al, _First);
}
}
}
template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void do_destroy_n(_Iter _First, const _Diff n) {
using value_type = iter_val_t<_Iter>;
if constexpr (!std::is_trivially_destructible_v<value_type>) {
for (; n > 0; ++_First, (void)--n) {
wjr::destroy_at(_First);
}
}
}
template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void do_destroy_n(Alloc& al, _Iter _First, _Diff n) {
using value_type = iter_val_t<_Iter>;
if constexpr (!(is_default_allocator_destroy_v<Alloc, _Iter> && std::is_trivially_destructible_v<value_type>)) {
for (; n > 0; ++_First, (void)--n) {
wjr::destroy_at(al, _First);
}
}
}
template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_default_construct(_Iter _First, _Iter _Last) {
if (!wjr::is_constant_evaluated()) {
return std::uninitialized_default_construct(_First, _Last);
}
using value_type = iter_val_t<_Iter>;
if constexpr (std::is_trivially_default_constructible_v<value_type>) {
}
else {
for (; _First != _Last; (void)++_First) {
wjr::construct_at(_First, default_construct_tag());
}
}
}
template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_default_construct(
Alloc& al, _Iter _First, _Iter _Last) {
if constexpr (is_default_allocator_construct_v<Alloc, _Iter, default_construct_tag>) {
wjr::uninitialized_default_construct(_First, _Last);
}
else {
for (; _First != _Last; (void)++_First) {
wjr::construct_at(al, _First, default_construct_tag());
}
}
}
template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_default_construct_n(_Iter _First, _Diff n) {
if (!wjr::is_constant_evaluated()) {
return std::uninitialized_default_construct_n(_First, n);
}
if (n <= 0) {
return _First;
}
using value_type = iter_val_t<_Iter>;
if constexpr(std::is_trivially_default_constructible_v<value_type>){
std::advance(_First, n);
}
else {
for (; n; ++_First, (void)--n) {
wjr::construct_at(_First, default_construct_tag());
}
}
return _First;
}
template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_default_construct_n(
Alloc& al, _Iter _First, _Diff n) {
if constexpr (is_default_allocator_construct_v<Alloc, _Iter, default_construct_tag>) {
return wjr::uninitialized_default_construct_n(_First, n);
}
else {
for (; n > 0; ++_First, (void)--n) {
wjr::construct_at(al, _First, default_construct_tag{});
}
return _First;
}
}
template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_value_construct(_Iter _First, _Iter _Last) {
if (!wjr::is_constant_evaluated()) {
return std::uninitialized_value_construct(_First, _Last);
}
for (; _First != _Last; ++_First) {
wjr::construct_at(_First, value_construct_tag());
}
}
template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_value_construct(
Alloc& al, _Iter _First, _Iter _Last) {
if constexpr (is_default_allocator_construct_v<Alloc, _Iter, value_construct_tag>) {
wjr::uninitialized_value_construct(_First, _Last);
}
else {
for (; _First != _Last; (void)++_First) {
wjr::construct_at(al, _First, value_construct_tag());
}
}
}
template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_value_construct_n(_Iter _First, _Diff n) {
if (!wjr::is_constant_evaluated()) {
return std::uninitialized_value_construct_n(_First, n);
}
if (n <= 0) {
return _First;
}
for (; n; ++_First, (void)--n) {
wjr::construct_at(_First, value_construct_tag());
}
return _First;
}
template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_value_construct_n(
Alloc& al, _Iter _First, _Diff n) {
if constexpr (is_default_allocator_construct_v<Alloc, _Iter, value_construct_tag>) {
return wjr::uninitialized_value_construct_n(_First, n);
}
else {
for (; n > 0; ++_First, (void)--n) {
wjr::construct_at(al, _First, value_construct_tag{});
}
return _First;
}
}
template<typename _Input, typename _Output,
typename _Input_ref = iter_ref_t<_Input>,
typename _Output_ref = iter_ref_t<_Output>>
struct __has_fast_uninitialized_copy : std::conjunction<
is_contiguous_iter<_Input>,
is_contiguous_iter<_Output>,
std::bool_constant<wjr::is_reverse_iterator_v<_Input> == wjr::is_reverse_iterator_v<_Output>>,
algo::__has_fast_construct_memcpy<remove_ref_t<_Output_ref>, _Input_ref>
> {};
template<typename _Input, typename _Output>
inline constexpr bool __has_fast_uninitialized_copy_v = __has_fast_uninitialized_copy<_Input, _Output>::value;
template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_uninitialized_copy_v<_Iter1, _Iter2>) {
const auto n = std::distance(_First, _Last);
if (WJR_UNLIKELY(n == 0)) { return _Dest; }
if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
const auto first1 = wjr::get_address(_First);
const auto first2 = wjr::get_address(_Dest);
algo::construct_memcpy(first2, first1, n);
}
else {
const auto first1 = wjr::get_address(_Last - 1);
const auto _Last2 = _Dest + n;
const auto first2 = wjr::get_address(_Last2 - 1);
algo::construct_memmove(first2, first1, n);
}
return _Dest + n;
}
return std::uninitialized_copy(_First, _Last, _Dest);
}
for (; _First != _Last; ++_First, (void)++_Dest) {
wjr::construct_at(_Dest, *_First);
}
return _Dest;
}
template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy(
Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
if constexpr (is_default_allocator_construct_v<Alloc, _Iter2, decltype(*_First)>) {
return wjr::do_uninitialized_copy(_First, _Last, _Dest);
}
else {
for (; _First != _Last; ++_Dest, (void)++_First) {
wjr::construct_at(al, _Dest, *_First);
}
}
}
template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy_n(_Iter1 _First, _Diff n, _Iter2 _Dest) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_uninitialized_copy_v<_Iter1, _Iter2>) {
if (n <= 0) { return _Dest; }
return wjr::do_uninitialized_copy(_First, _First + n, _Dest);
}
return std::uninitialized_copy_n(_First, n, _Dest);
}
if (n <= 0) {
return _Dest;
}
for (; n; ++_First, (void)++_Dest, (void)--n) {
wjr::construct_at(_Dest, *_First);
}
return _Dest;
}
template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_copy_n(Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest) {
if constexpr (is_default_allocator_construct_v<Alloc, _Iter2, decltype(*_First)>) {
return wjr::uninitialized_copy_n(_First, n, _Dest);
}
else {
for (; n > 0; ++_First, (void)++_Dest, (void)--n) {
wjr::construct_at(al, _Dest, *_First);
}
return _Dest;
}
}
template<typename _Iter, typename _Val,
typename _Iter_ref = iter_ref_t<_Iter>>
struct __has_fast_uninitialized_fill : std::conjunction<
is_contiguous_iter<_Iter>,
algo::__has_fast_construct_memset<remove_cref_t<_Iter_ref>, add_cref_t<_Val>>
> {};
template<typename _Iter, typename _Val>
inline constexpr bool __has_fast_uninitialized_fill_v = __has_fast_uninitialized_fill<_Iter, _Val>::value;
template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, const _Val& val) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_uninitialized_fill_v<_Iter, _Val>) {
const auto n = static_cast<size_t>(std::distance(_First, _Last));
if constexpr (!is_reverse_iterator_v<_Iter>) {
const auto first = wjr::get_address(_First);
algo::construct_memset(first, val, n);
}
else {
if (WJR_UNLIKELY(n == 0)) { return; }
const auto first = wjr::get_address(_Last - 1);
algo::construct_memset(first, val, n);
}
return;
}
return std::uninitialized_fill(_First, _Last, val);
}
for (; _First != _Last; ++_First) {
wjr::construct_at(_First, val);
}
}
template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, default_construct_tag) {
wjr::uninitialized_default_construct(_First, _Last);
}
template<typename _Iter>
WJR_CONSTEXPR20 void do_uninitialized_fill(_Iter _First, _Iter _Last, value_construct_tag) {
wjr::uninitialized_value_construct(_First, _Last);
}
template<typename Alloc, typename _Iter, typename _Val>
WJR_CONSTEXPR20 void do_uninitialized_fill(Alloc& al, _Iter _First, _Iter _Last, const _Val& val) {
if constexpr (is_default_allocator_construct_v<Alloc, _Iter, _Val>) {
wjr::uninitialized_fill(_First, _Last, val);
}
else {
for (; _First != _Last; (void)++_First) {
wjr::construct_at(al, _First, val);
}
}
}
template<typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff n, const _Val& val) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_uninitialized_fill_v<_Iter, _Val>) {
if (n <= 0) { return _First; }
wjr::uninitialized_fill(_First, _First + n, val);
return _First + n;
}
return std::uninitialized_fill_n(_First, n, val);
}
if (n <= 0) {
return _First;
}
for (; n; ++_First, (void)--n) {
wjr::construct_at(_First, val);
}
return _First;
}
template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff n, default_construct_tag) {
return wjr::uninitialized_default_construct_n(_First, n);
}
template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(_Iter _First, _Diff n, value_construct_tag) {
return wjr::uninitialized_value_construct_n(_First, n);
}
template<typename Alloc, typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter do_uninitialized_fill_n(Alloc& al, _Iter _First, _Diff n, const _Val& val) {
if constexpr (is_default_allocator_construct_v<Alloc, _Iter, _Val>) {
return wjr::uninitialized_fill_n(_First, n, val);
}
else {
for (; n > 0; (void)++_First, --n) {
wjr::construct_at(al, _First, val);
}
}
}
template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_move(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
return wjr::uninitialized_copy(
std::make_move_iterator(_First),
std::make_move_iterator(_Last),
_Dest
);
}
template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 do_uninitialized_move(Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
return wjr::uninitialized_copy(al,
std::make_move_iterator(_First),
std::make_move_iterator(_Last),
_Dest
);
}
template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_uninitialized_move_n(_Iter1 _First, _Diff n, _Iter2 _Dest) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_uninitialized_copy_v<_Iter1, std::move_iterator<_Iter1>>) {
if (n <= 0) return std::make_pair(_First, _Dest);
wjr::uninitialized_move(_First, _First + n, _Dest);
return std::make_pair(_First + n, _Dest + n);
}
return std::uninitialized_move_n(_First, n, _Dest);
}
if (n <= 0) {
return std::make_pair(_First, _Dest);
}
for (; n; ++_First, (void)++_Dest, (void)--n) {
wjr::construct_at(_Dest, std::move(*_First));
}
return std::make_pair(_First, _Dest);
}
template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> do_uninitialized_move_n(
Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest) {
if constexpr (is_default_allocator_construct_v<Alloc, _Iter1, decltype(std::move(*_First))>) {
return wjr::uninitialized_move_n(_First, n, _Dest);
}
else {
for (; n > 0; ++_First, (void)++_Dest, (void)--n) {
wjr::construct_at(al, _Dest, std::move(*_First));
}
return std::make_pair(_First, _Dest);
}
}
template<typename _Iter, typename _Func>
WJR_CONSTEXPR20 void do_for_each(_Iter _First, _Iter _Last, _Func fn) {
for (; _First != _Last; ++_First) {
fn(*_First);
}
}
template<typename _Iter, typename _SizeT, typename _Func>
WJR_CONSTEXPR20 void do_for_each_n(_Iter _First, _SizeT n, _Func fn) {
if (n <= 0) {
return;
}
for (; n; ++_First, (void)--n) {
fn(*_First);
}
}
template<typename Alloc>
class temporary_allocator_value {
public:
using value_type = alloc_value_t<Alloc>;
using traits = std::allocator_traits<Alloc>;
template<typename...Args>
constexpr explicit temporary_allocator_value(Alloc& al, Args&&...args) noexcept
: al(al) {
traits::construct(al, get_ptr(), std::forward<Args>(args)...);
}
temporary_allocator_value(const temporary_allocator_value&) = delete;
temporary_allocator_value& operator=(const temporary_allocator_value&) = delete;
~temporary_allocator_value() {
wjr::destroy_at(al, get_ptr());
}
constexpr value_type* get_ptr() {
return reinterpret_cast<value_type*>(std::addressof(vl));
}
constexpr const value_type* get_ptr() const {
return reinterpret_cast<const value_type*>(std::addressof(vl));
}
constexpr value_type& value() {
return *get_ptr();
}
constexpr const value_type& value() const {
return *get_ptr();
}
private:
Alloc& al;
std::aligned_storage_t<sizeof(value_type), alignof(value_type)> vl;
};
_WJR_END

#endif // __WJR_ALGORITHM_H

_WJR_BEGIN
/*
* vector_data ->
* 1.vector_data()
* 2.vector_data(_Al&al, const size_t _Newsize, const size_t _Newcapacity)
* 4.vector_data(const vector_data&) = delete
* 5.vector_data(vector_data&&) = delete
* 6.static void copyConstruct(_Al& al, const vector_data& _Src, vector_data& _Dest)
* 7.static void moveConstruct(_Al& al, vector_data&& _Src, vector_data& _Dest)
* 8.static void Swap(_Al& al, vector_data& _Left, vector_data& _Right) [noexcept] [optional]
* 9.static void Deallocate(_Al& al, vector_data& _Data)
* 10.static size_t getGrowthCapacity(const size_t _Oldcapacity, const size_t _Newsize) [optinal]
* 11.static void shrinkToFit(_Al&al, vector_data& _Data) [optional]
* 12.size_t size() const;
* 13.size_t capacity() const;
* 14.[const] _Ty* data() [const]
* 15.const _Ty* cdata() const
* 16.void set_size(const size_type);
* 17.void inc_size(const difference_type);
*/
template<typename T, typename Alloc>
struct vector_data {
using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
using _Alty_traits = std::allocator_traits<_Alty>;
using value_type = T;
using pointer = typename _Alty_traits::pointer;
using const_pointer = typename _Alty_traits::const_pointer;
using size_type = typename _Alty_traits::size_type;
using difference_type = typename _Alty_traits::difference_type;
WJR_INLINE_CONSTEXPR20 vector_data() = default;
vector_data(const vector_data&) = delete;
vector_data& operator=(const vector_data&) = delete;
WJR_CONSTEXPR20 ~vector_data() = default;
WJR_CONSTEXPR20 vector_data(
_Alty& al,
const size_type _Newsize,
const size_type _Newcapacity)
: _Myfirst(_Alty_traits::allocate(al, _Newcapacity)),
_Mylast(_Myfirst + _Newsize),
_Myend(_Myfirst + _Newcapacity) {
}
WJR_CONSTEXPR20 vector_data(
_Alty& al,
const size_type _Newsize,
const size_type _Newcapacity,
extend_tag)
: vector_data(al, _Newsize, _Newcapacity) {}
WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const vector_data& _Src, vector_data& _Dest) {
{
vector_data data(al, _Src.size(), _Src.size());
moveConstruct(al, std::move(data), _Dest);
}
wjr::uninitialized_copy(al, _Src._Myfirst, _Src._Mylast, _Dest._Myfirst);
}
WJR_INLINE_CONSTEXPR20 static void moveConstruct(_Alty&, vector_data&& _Src, vector_data& _Dest) noexcept {
_Dest._Myfirst = _Src._Myfirst;
_Dest._Mylast = _Src._Mylast;
_Dest._Myend = _Src._Myend;
_Src._Myfirst = _Src._Mylast = _Src._Myend = nullptr;
}
WJR_CONSTEXPR20 static void Deallocate(_Alty& al, vector_data& _Data) {
_Alty_traits::deallocate(al, _Data._Myfirst, _Data.capacity());
_Data._Myfirst = _Data._Mylast = _Data._Myend = nullptr;
}
WJR_CONSTEXPR20 static void Tidy(_Alty& al, vector_data& _Data) {
if (_Data._Myfirst != nullptr) {
wjr::destroy(al, _Data.data(), _Data.lastPtr());
Deallocate(al, _Data);
}
}
WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, vector_data& _Data) noexcept {
if (_Data._Mylast != _Data._Myend) {
auto _Olddata = _Data._Myfirst;
const auto _Oldsize = _Data.size();
if (_Oldsize == 0) {
Deallocate(al, _Data);
}
else {
vector_data _Newdata(al, _Oldsize, _Oldsize);
wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, _Newdata.data());
wjr::destroy_n(al, _Olddata, _Oldsize);
Deallocate(al, _Data);
moveConstruct(al, std::move(_Newdata), _Data);
}
}
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) size_type size() const noexcept {
return static_cast<size_type>(_Mylast - _Myfirst);
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) size_type capacity() const noexcept {
return static_cast<size_type>(_Myend - _Myfirst);
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer data() noexcept {
return _Myfirst;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer data() const noexcept {
return _Myfirst;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer lastPtr() noexcept {
return _Mylast;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer lastPtr() const noexcept {
return _Mylast;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer endPtr() noexcept {
return _Myend;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer endPtr() const noexcept {
return _Myend;
}
WJR_INLINE_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
_Mylast = _Myfirst + _Size;
WJR_ASSUME(size() == _Size);
}
WJR_INLINE_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
_Mylast += _Size;
}
pointer _Myfirst = nullptr;
pointer _Mylast = nullptr;
pointer _Myend = nullptr;
};
template<typename T, size_t N, typename Alloc>
struct vector_static_data {
using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
using _Alty_traits = std::allocator_traits<_Alty>;
using value_type = T;
using pointer = typename _Alty_traits::pointer;
using const_pointer = typename _Alty_traits::const_pointer;
using size_type = typename _Alty_traits::size_type;
using difference_type = typename _Alty_traits::difference_type;
static_assert(N > 0, "N must be greater than 0");
constexpr static size_t _MaxAlignment = std::max(alignof(T), alignof(size_type));
constexpr static size_t _MaxMemroy = (sizeof(T) * N + _MaxAlignment - 1) & (~(_MaxAlignment - 1));
constexpr static size_t max_capacity = _MaxMemroy / sizeof(T);
static_assert(max_capacity >= N, "");
WJR_INLINE_CONSTEXPR20 vector_static_data() {}
vector_static_data(const vector_static_data&) = delete;
vector_static_data& operator=(const vector_static_data&) = delete;
WJR_CONSTEXPR20 ~vector_static_data() = default;
WJR_CONSTEXPR20 static void _lengthError(WJR_MAYBE_UNUSED const size_type _Newcapacity){
#if defined(_WJR_EXCEPTION)
throw std::length_error("vector_static_data is too small to hold the requested data");
#endif // _WJR_EXCEPTION
WJR_UNREACHABLE;
}
WJR_CONSTEXPR20 vector_static_data(
_Alty&,
const size_type,
const size_type _Newcapacity,
extend_tag) {
_lengthError(_Newcapacity);
}
WJR_CONSTEXPR20 vector_static_data(
_Alty&,
const size_type _Newsize,
const size_type _Newcapacity) : _M_size(_Newsize) {
if (_Newcapacity > max_capacity) {
_lengthError(_Newcapacity);
}
}
WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const vector_static_data& _Src, vector_static_data& _Dest) {
wjr::uninitialized_copy_n(al, _Src.data(), _Src.size(), _Dest.data());
_Dest.set_size(_Src.size());
}
WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, vector_static_data&& _Src, vector_static_data& _Dest) {
auto n = _Src.size();
if (n != 0) {
wjr::uninitialized_move_n(al, _Src.data(), n, _Dest.data());
wjr::destroy_n(al, _Src.data(), n);
_Src.set_size(0);
_Dest.set_size(n);
}
}
WJR_INLINE_CONSTEXPR20 static void Deallocate(_Alty&, vector_static_data& _Data) noexcept {
_Data.set_size(0);
}
WJR_CONSTEXPR20 static void Tidy(_Alty& al, vector_static_data& _Data) {
wjr::destroy_n(al, _Data.data(), _Data.size());
Deallocate(al, _Data);
}
WJR_INLINE_CONSTEXPR20 static void shrinkToFit(_Alty&, vector_static_data&) {
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) size_type size() const noexcept {
return _M_size;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) size_type capacity() const noexcept {
return max_capacity;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer data() noexcept {
return reinterpret_cast<pointer>(_M_storage);
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer data() const noexcept {
return reinterpret_cast<const_pointer>(_M_storage);
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer lastPtr() noexcept {
return data() + size();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer lastPtr() const noexcept {
return data() + size();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer endPtr() noexcept {
return data() + capacity();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer endPtr() const noexcept {
return data() + capacity();
}
WJR_INLINE_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
_M_size = _Size;
WJR_ASSUME(size() == _Size);
}
WJR_INLINE_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
_M_size += _Size;
}
size_type _M_size = 0;
union {
alignas(_MaxAlignment) uint8_t _M_storage[_MaxMemroy];
};
};
template<typename T, size_t N, typename Alloc>
struct vector_sso_data {
using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
using _Alty_traits = std::allocator_traits<_Alty>;
using value_type = T;
using pointer = typename _Alty_traits::pointer;
using const_pointer = typename _Alty_traits::const_pointer;
using size_type = typename _Alty_traits::size_type;
using difference_type = typename _Alty_traits::difference_type;
static_assert(N > 0, "N must be greater than 0");
constexpr static size_t _MaxAlignment = std::max(alignof(T),
std::max(alignof(size_type), alignof(pointer)));
constexpr static size_t _MaxMemroy = std::max(
(sizeof(T) * N + _MaxAlignment - 1) & (~(_MaxAlignment - 1)),
sizeof(size_type));
constexpr static size_t max_capacity = _MaxMemroy / sizeof(T);
static_assert(max_capacity >= N, "");
WJR_INLINE_CONSTEXPR20 vector_sso_data() {}
vector_sso_data(const vector_sso_data&) = delete;
vector_sso_data& operator=(const vector_sso_data&) = delete;
~vector_sso_data() = default;
WJR_CONSTEXPR20 vector_sso_data(
_Alty& al,
const size_type _Newsize,
const size_type _Newcapacity,
extend_tag)
: _M_ptr(_Alty_traits::allocate(al, _Newcapacity)),
_M_size(_Newsize),
_M_capacity(_Newcapacity) {
WJR_ASSUME(_Newcapacity > max_capacity);
}
WJR_CONSTEXPR20 vector_sso_data(
_Alty& al,
const size_type _Newsize,
const size_type _Newcapacity)
: _M_size(_Newsize){
if (_Newcapacity > max_capacity) {
_M_ptr = _Alty_traits::allocate(al, _Newcapacity);
_M_capacity = _Newcapacity;
}
}
WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const vector_sso_data& _Src, vector_sso_data& _Dest) {
const auto _Size = _Src._M_size;
if (_Size > max_capacity) {
vector_sso_data data(al, 0, _Size);
moveConstruct(al, std::move(data), _Dest);
}
_Dest._M_size = _Size;
wjr::uninitialized_copy_n(al, _Src.data(), _Src.size(), _Dest.data());
}
WJR_INLINE_CONSTEXPR20 static void moveConstruct(_Alty& al, vector_sso_data&& _Src, vector_sso_data& _Dest) {
const auto _Size = _Src._M_size;
if(!_Src.is_small()){
_Dest._M_ptr = _Src._M_ptr;
_Dest._M_capacity = _Src._M_capacity;
}
else {
WJR_ASSUME(_Size <= max_capacity);
wjr::uninitialized_move_n(al, _Src.data(), _Size, _Dest.data());
wjr::destroy_n(al, _Src.data(), _Size);
}
_Dest._M_size = _Size;
_Src.reset_ptr();
_Src._M_size = 0;
}
WJR_CONSTEXPR20 static void Deallocate(_Alty& al, vector_sso_data& _Data) {
if (!_Data.is_small()) {
_Alty_traits::deallocate(al, _Data.data(), _Data.capacity());
}
_Data.reset_ptr();
_Data._M_size = 0;
}
WJR_CONSTEXPR20 static void Tidy(_Alty& al, vector_sso_data& _Data) {
wjr::destroy_n(al, _Data.data(), _Data.size());
Deallocate(al, _Data);
}
WJR_INLINE_CONSTEXPR20 static void shrinkToFit(_Alty& al, vector_sso_data& _Data) {
if (!_Data.is_small()) {
const auto _Oldsize = _Data.size();
const auto _Oldcapacity = _Data.capacity();
if (_Oldsize != _Oldcapacity) {
if (_Oldsize <= max_capacity) {
wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, reinterpret_cast<pointer>(_Data._M_storage));
wjr::destroy_n(al, _Data.data(), _Oldsize);
_Alty_traits::deallocate(al, _Data.data(), _Oldcapacity);
_Data.reset_ptr();
}
else {
auto _Newdata = _Alty_traits::allocate(al, _Oldsize);
wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, _Newdata);
wjr::destroy_n(al, _Data.data(), _Oldsize);
_Alty_traits::deallocate(al, _Data.data(), _Oldcapacity);
_Data._M_ptr = _Newdata;
_Data._M_capacity = _Oldsize;
}
_Data._M_size = _Oldsize;
}
}
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) size_type size() const noexcept {
return _M_size;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) size_type capacity() const noexcept {
return is_small() ? max_capacity : _M_capacity;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer data() noexcept {
return _M_ptr;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer data() const noexcept {
return _M_ptr;
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer lastPtr() noexcept {
return data() + size();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer lastPtr() const noexcept {
return data() + size();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer endPtr() noexcept {
return data() + capacity();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer endPtr() const noexcept {
return data() + capacity();
}
WJR_INLINE_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
_M_size = _Size;
}
WJR_INLINE_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
_M_size += _Size;
}
WJR_INLINE_CONSTEXPR20 bool is_small() const noexcept{
const bool f = static_cast<const void*>(_M_ptr) == static_cast<const void*>(_M_storage);
return f;
}
WJR_INLINE_CONSTEXPR20 void reset_ptr() noexcept {
_M_ptr = reinterpret_cast<pointer>(_M_storage);
}
pointer _M_ptr = reinterpret_cast<pointer>(_M_storage);
size_type _M_size = 0;
union {
alignas(_MaxAlignment) uint8_t _M_storage[_MaxMemroy];
alignas(_MaxAlignment) size_type _M_capacity;
};
};
namespace _Vector_helper {
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(Swap, Swap);
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(getGrowthCapacity, getGrowthCapacity);
}
template<typename T, typename Alloc = std::allocator<T>, typename Data = vector_data<T, Alloc>>
class vector {
public:
using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
using _Alty_traits = std::allocator_traits<_Alty>;
static_assert(std::is_same_v<typename _Alty_traits::value_type, T>,
"Alloc::value_type must be the same as T");
using value_type = T;
using allocator_type = _Alty;
using size_type = typename _Alty_traits::size_type;
using difference_type = typename _Alty_traits::difference_type;
using reference = value_type&;
using const_reference = const value_type&;
using pointer = typename _Alty_traits::pointer;
using const_pointer = typename _Alty_traits::const_pointer;
using iterator = pointer;
using const_iterator = const_pointer;
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;
using data_type = Data;
static_assert(std::is_pointer_v<pointer>, "");
static_assert(std::is_pointer_v<const_pointer>, "");
WJR_CONSTEXPR20 vector()
noexcept(std::is_nothrow_default_constructible_v<_Alty>
&& std::is_nothrow_default_constructible_v<data_type>) : _Myval() {}
WJR_CONSTEXPR20 explicit vector(const allocator_type& al)
noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>
&& std::is_nothrow_default_constructible_v<data_type>)
: _Myval(std::piecewise_construct_t{},
std::forward_as_tuple(al),
std::forward_as_tuple()) {}
WJR_CONSTEXPR20 explicit vector(const size_type _Count, const allocator_type& al = allocator_type())
: vector(al) {
_M_construct_n(_Count, value_construct_tag());
}
WJR_CONSTEXPR20 vector(size_type _Count, const value_type& _Val, const allocator_type& al = allocator_type())
: vector(al) {
_M_construct_n(_Count, _Val);
}
private:
constexpr static bool _IsNoThrowCopyConstruct =
noexcept(data_type::copyConstruct(std::declval<_Alty&>(),
std::declval<const data_type&>(), std::declval<data_type&>()));
constexpr static bool _IsNoThrowMoveConstruct =
noexcept(data_type::moveConstruct(std::declval<_Alty&>(),
std::declval<data_type&&>(), std::declval<data_type&>()));
public:
template<typename _Alloc>
WJR_CONSTEXPR20 vector(const vector& other, _Alloc&& al, disable_tag)
noexcept(_IsNoThrowCopyConstruct
&& std::is_nothrow_constructible_v<_Alty, _Alloc&&>)
: _Myval(std::piecewise_construct_t{},
std::forward_as_tuple(std::forward<_Alloc>(al)),
std::forward_as_tuple()) {
copyConstruct(getAllocator(), other.getData(), getData());
}
WJR_CONSTEXPR20 vector(const vector& other)
: vector(other, _Alty_traits::select_on_container_copy_construction(other.getAllocator()),
disable_tag{}) {}
WJR_CONSTEXPR20 vector(const vector& other, const allocator_type& al)
: vector(other, al, disable_tag{}) {}
WJR_CONSTEXPR20 vector(vector&& other)
noexcept(_IsNoThrowMoveConstruct
&& std::is_nothrow_move_constructible_v<_Alty>)
: _Myval(std::piecewise_construct_t{},
std::forward_as_tuple(std::move(other.getAllocator())),
std::forward_as_tuple()) {
moveConstruct(getAllocator(), std::move(other.getData()), getData());
}
private:
WJR_CONSTEXPR20 vector(vector&& other, const allocator_type& al, std::true_type) noexcept
: _Myval(std::piecewise_construct_t{},
std::forward_as_tuple(al),
std::forward_as_tuple()) {
moveConstruct(getAllocator(), std::move(other.getData()), getData());
}
WJR_CONSTEXPR20 vector(vector&& other, const allocator_type& _Al, std::false_type)
: _Myval(std::piecewise_construct_t{},
std::forward_as_tuple(_Al),
std::forward_as_tuple()) {
auto& al = getAllocator();
if (al == other.getAllocator()) {
moveConstruct(al, std::move(other.getData()), getData());
}
else {
const auto _Size = other.size();
if (_Size != 0) {
{
data_type _Data(al, 0, _Size);
moveConstruct(al, std::move(_Data), getData());
}
set_size(_Size);
wjr::uninitialized_move_n(al, other.data(), _Size, data());
}
}
}
public:
WJR_CONSTEXPR20 vector(vector&& other, const allocator_type& al)
noexcept(noexcept(vector(std::declval<vector&&>(),
std::declval<const allocator_type&>(), std::declval<typename _Alty_traits::is_always_equal>())))
: vector(std::move(other), al, typename _Alty_traits::is_always_equal{}) {}
template<typename _Iter, std::enable_if_t<wjr::is_iterator_v<_Iter>, int> = 0>
WJR_CONSTEXPR20 vector(_Iter _First, _Iter _Last, const allocator_type& al = allocator_type())
: vector(al) {
_M_range_init(_First, _Last, typename std::iterator_traits<_Iter>::iterator_category());
}
WJR_CONSTEXPR20 vector(std::initializer_list<value_type> il, const allocator_type& al = allocator_type())
: vector(il.begin(), il.end(), al) { }
WJR_CONSTEXPR20 ~vector() noexcept {
tidy();
}
WJR_CONSTEXPR20 vector& operator=(const vector& other) {
if (WJR_LIKELY(this != std::addressof(other))) {
auto& al = getAllocator();
auto& other_al = other.getAllocator();
if constexpr (std::conjunction_v<typename _Alty_traits::propagate_on_container_copy_assignment
, std::negation<typename _Alty_traits::is_always_equal>>) {
if (al != other_al) {
tidy();
}
}
if constexpr (_Alty_traits::propagate_on_container_copy_assignment::value) {
al = other_al;
}
assign(other.begin(), other.end());
}
return *this;
}
WJR_CONSTEXPR20 vector& operator=(vector&& other)
noexcept(_Alty_traits::propagate_on_container_move_assignment::value
|| _Alty_traits::is_always_equal::value) {
using type = std::disjunction<typename _Alty_traits::propagate_on_container_move_assignment,
typename _Alty_traits::is_always_equal>;
if (WJR_LIKELY(this != std::addressof(other))) {
_M_move_assign(std::move(other), type());
}
return *this;
}
WJR_CONSTEXPR20 vector& operator=(std::initializer_list<value_type> il) {
return assign(il);
}
WJR_CONSTEXPR20 vector& assign(size_type _Count, const value_type& _Val) {
_M_fill_assign(_Count, _Val);
return *this;
}
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 vector& assign(iter _First, iter _Last) {
_M_range_assign(_First, _Last, typename std::iterator_traits<iter>::iterator_category());
return *this;
}
WJR_CONSTEXPR20 vector& assign(std::initializer_list<value_type> il) {
return assign(il.begin(), il.end());
}
WJR_INLINE_CONSTEXPR20 iterator begin() noexcept {
return data();
}
WJR_INLINE_CONSTEXPR20 const_iterator begin() const noexcept {
return data();
}
WJR_INLINE_CONSTEXPR20 const_iterator cbegin() const noexcept {
return data();
}
WJR_INLINE_CONSTEXPR20 iterator end() noexcept {
return lastPtr();
}
WJR_INLINE_CONSTEXPR20 const_iterator end() const noexcept {
return lastPtr();
}
WJR_INLINE_CONSTEXPR20 const_iterator cend() const noexcept {
return lastPtr();
}
WJR_INLINE_CONSTEXPR20 reverse_iterator rbegin() noexcept {
return reverse_iterator(end());
}
WJR_INLINE_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
return const_reverse_iterator(end());
}
WJR_INLINE_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept {
return const_reverse_iterator(cend());
}
WJR_INLINE_CONSTEXPR20 reverse_iterator rend() noexcept {
return reverse_iterator(begin());
}
WJR_INLINE_CONSTEXPR20 const_reverse_iterator rend() const noexcept {
return const_reverse_iterator(begin());
}
WJR_INLINE_CONSTEXPR20 const_reverse_iterator crend() const noexcept {
return const_reverse_iterator(cbegin());
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) size_type size() const noexcept {
return getData().size();
}
WJR_CONSTEXPR20 void resize(const size_type _Newsize) {
_M_resize(_Newsize, value_construct_tag());
}
WJR_CONSTEXPR20 void resize(const size_type _Newsize, const value_type& _Val) {
_M_resize(_Newsize, _Val);
}
WJR_CONSTEXPR20 void shrink_to_fit() {
shrinkToFit(getAllocator(), getData());
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) size_type capacity() const noexcept {
return getData().capacity();
}
WJR_INLINE_CONSTEXPR20 bool empty() const noexcept {
return size() == 0;
}
WJR_CONSTEXPR20 void reserve(size_type n) {
const size_type _Oldsize = size();
const size_type _Oldcapacity = capacity();
if (_Oldcapacity < n) {
auto& al = getAllocator();
const size_type _Newcapacity = getGrowthCapacity(_Oldcapacity, n);
data_type _Newdata(al, _Oldsize, _Newcapacity, extend_tag());
wjr::uninitialized_move_n(al, data(), _Oldsize, _Newdata.data());
tidy();
moveConstruct(al, std::move(_Newdata), getData());
}
WJR_ASSUME(size() == _Oldsize);
}
WJR_INLINE_CONSTEXPR20 reference operator[](size_type _Pos) noexcept {
return *(data() + _Pos);
}
WJR_INLINE_CONSTEXPR20 const_reference operator[](size_type _Pos) const noexcept {
return *(data() + _Pos);
}
WJR_INLINE_CONSTEXPR20 reference at(size_type _Pos) {
if (_Pos >= size()) {
throw (std::out_of_range("wjr::vector::at"));
}
return (*this)[_Pos];
}
WJR_INLINE_CONSTEXPR20 const_reference at(size_type _Pos) const {
if (_Pos >= size()) {
throw (std::out_of_range("wjr::vector::at"));
}
return (*this)[_Pos];
}
WJR_INLINE_CONSTEXPR20 reference front() noexcept {
return *data();
}
WJR_INLINE_CONSTEXPR20 const_reference front() const noexcept {
return *data();
}
WJR_INLINE_CONSTEXPR20 reference back() noexcept {
return *(lastPtr() - 1);
}
WJR_INLINE_CONSTEXPR20 const_reference back() const noexcept {
return *(lastPtr() - 1);
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer data() noexcept {
return getData().data();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer data() const noexcept {
return getData().data();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer cdata() const noexcept {
return getData().data();
}
template<typename...Args>
WJR_INLINE_CONSTEXPR20 reference emplace_back(Args&&... args) {
auto& al = getAllocator();
const pointer _Mylast = lastPtr();
const pointer _Myend = endPtr();
if (_Mylast != _Myend) {
wjr::construct_at(al, _Mylast, std::forward<Args>(args)...);
inc_size(1);
}
else {
_M_realloc_insert_at_end(std::forward<Args>(args)...);
}
return back();
}
WJR_INLINE_CONSTEXPR20 void push_back(const value_type& _Val) {
emplace_back(_Val);
}
WJR_INLINE_CONSTEXPR20 void push_back(value_type&& _Val) {
emplace_back(std::move(_Val));
}
WJR_INLINE_CONSTEXPR20 void pop_back() noexcept {
inc_size(-1);
const pointer _Mylast = lastPtr();
wjr::destroy_at(getAllocator(), _Mylast);
}
template<typename...Args>
WJR_INLINE_CONSTEXPR20 iterator emplace(const_iterator _Where, Args&&... args) {
return _M_emplace_aux(_Where, std::forward<Args>(args)...);
}
WJR_INLINE_CONSTEXPR20 iterator insert(const_iterator _Where, const value_type& _Val) {
return emplace(_Where, _Val);
}
WJR_INLINE_CONSTEXPR20 iterator insert(const_iterator _Where, value_type&& _Val) {
return emplace(_Where, std::move(_Val));
}
WJR_CONSTEXPR20 iterator insert(const_iterator _Where, std::initializer_list<value_type> il) {
return insert(_Where, il.begin(), il.end());
}
WJR_CONSTEXPR20 iterator insert(const_iterator _Where, size_type _Count, const value_type& _Val) {
const auto __old_pos = static_cast<size_type>(_Where - cbegin());
_M_fill_insert(begin() + __old_pos, _Count, _Val);
return begin() + __old_pos;
}
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 iterator insert(const_iterator _Where, iter _First, iter _Last) {
const auto __old_pos = static_cast<size_type>(_Where - cbegin());
_M_range_insert(begin() + __old_pos, _First, _Last, typename std::iterator_traits<iter>::iterator_category());
return begin() + __old_pos;
}
WJR_INLINE_CONSTEXPR20 iterator erase(const_iterator _Where) {
return _M_erase(begin() + (_Where - cbegin()));
}
WJR_INLINE_CONSTEXPR20 iterator erase(const_iterator _First, const_iterator _Last) {
const iterator __beg = begin();
const const_iterator __cbeg = cbegin();
return _M_erase(__beg + (_First - __cbeg), __beg + (_Last - __cbeg));
}
WJR_INLINE_CONSTEXPR20 void swap(vector& _Right) noexcept {
Swap(getAllocator(), getData(), _Right.getData());
if constexpr (_Alty_traits::propagate_on_container_swap::value) {
std::swap(getAllocator(), _Right.getAllocator());
}
}
WJR_CONSTEXPR20 void clear() {
_M_erase_at_end(data());
WJR_ASSUME(size() == 0);
}
WJR_INLINE_CONSTEXPR20 allocator_type& get_allocator() noexcept {
return getAllocator();
}
WJR_INLINE_CONSTEXPR20 const allocator_type& get_allocator() const noexcept {
return getAllocator();
}
/*------Internal management function------*/
WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const data_type& _Src, data_type& _Dest)
noexcept(_IsNoThrowCopyConstruct) {
data_type::copyConstruct(al, _Src, _Dest);
}
WJR_INLINE_CONSTEXPR20 static void moveConstruct(_Alty& al, data_type&& _Src, data_type& _Dest)
noexcept(_IsNoThrowMoveConstruct) {
data_type::moveConstruct(al, std::move(_Src), _Dest);
}
template<typename _D = data_type, std::enable_if_t<
_Vector_helper::has_static_member_function_Swap_v<_D, _Alty&, _D&, _D&>, int> = 0>
WJR_INLINE_CONSTEXPR20 static void Swap(_Alty& al, _D& _Left, _D& _Right)
noexcept(noexcept(_D::Swap(al, _Left, _Right))) {
return _D::Swap(al, _Left, _Right);
}
template<typename _D = Data, std::enable_if_t<
!_Vector_helper::has_static_member_function_Swap_v<_D, _Alty&, _D&, _D&>, int> = 0>
WJR_INLINE_CONSTEXPR20 static void Swap(_Alty& al, _D& _Left, _D& _Right)
noexcept(
noexcept(moveConstruct(std::declval<_Alty&>(), std::declval<_D&&>(), std::declval<_D&>()))
&& std::is_nothrow_default_constructible_v<_D>) {
_D _Tmp;
moveConstruct(al, std::move(_Left), _Tmp);
moveConstruct(al, std::move(_Right), _Left);
moveConstruct(al, std::move(_Tmp), _Right);
}
WJR_CONSTEXPR20 static void Deallocate(_Alty& al, data_type& _Data)
noexcept(noexcept(data_type::Deallocate(al, _Data))) {
data_type::Deallocate(al, _Data);
WJR_ASSUME(_Data.size() == 0);
}
WJR_CONSTEXPR20 static void Tidy(_Alty& al, data_type& _Data) {
data_type::Tidy(al, _Data);
WJR_ASSUME(_Data.size() == 0);
}
template<typename _D = Data, std::enable_if_t<
_Vector_helper::has_static_member_function_getGrowthCapacity_v<
_D, const size_type, const size_type>, int> = 0>
WJR_INLINE_CONSTEXPR20 static size_type getGrowthCapacity(
const size_type _Oldcapacity, const size_type _Newsize) noexcept {
return Data::getGrowthCapacity(_Oldcapacity, _Newsize);
}
template<typename _D = Data, std::enable_if_t<
!_Vector_helper::has_static_member_function_getGrowthCapacity_v<
_D, const size_type, const size_type>, int> = 0>
WJR_INLINE_CONSTEXPR20 static size_type getGrowthCapacity(
const size_type _Oldcapacity, const size_type _Newsize) noexcept {
WJR_ASSUME(_Newsize > _Oldcapacity);
const auto _Newcapacity = _Oldcapacity + _Oldcapacity / 2;
const auto _FixedCapacity = _Newcapacity < _Newsize ? _Newsize : _Newcapacity;
const auto ret = _FixedCapacity < 32 ? ((_FixedCapacity + 3) & (-4)) :
_FixedCapacity < 256 ? ((_FixedCapacity + 15) & (-16)) :
((_FixedCapacity + 63) & (-64));
WJR_ASSUME(ret >= _Newsize);
return ret;
}
WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, data_type& _Data) {
data_type::shrinkToFit(al, _Data);
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) _Alty& getAllocator() noexcept {
return _Myval.first();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const _Alty& getAllocator() const noexcept {
return _Myval.first();
}
WJR_INLINE_CONSTEXPR20 data_type& getData() noexcept {
return _Myval.second();
}
WJR_INLINE_CONSTEXPR20 const data_type& getData() const noexcept {
return _Myval.second();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer lastPtr() noexcept {
return getData().lastPtr();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer lastPtr() const noexcept {
return getData().lastPtr();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) pointer endPtr() noexcept {
return getData().endPtr();
}
WJR_ATTRIBUTE(PURE, INLINE_CONSTEXPR20) const_pointer endPtr() const noexcept {
return getData().endPtr();
}
WJR_INLINE_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
getData().set_size(_Size);
WJR_ASSUME(size() == _Size);
}
WJR_INLINE_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
const auto _Oldsize = size();
getData().inc_size(_Size);
WJR_ASSUME(_Oldsize + _Size == size());
}
WJR_CONSTEXPR20 void tidy() noexcept {
Tidy(getAllocator(), getData());
WJR_ASSUME(size() == 0);
}
/*------External extension function------*/
/*------default construct/value_construct------*/
WJR_CONSTEXPR20 vector(const size_type _Count, default_construct_tag, const allocator_type& al = allocator_type())
: vector(al) {
_M_construct_n(_Count, default_construct_tag());
}
WJR_CONSTEXPR20 vector(const size_type _Count, value_construct_tag, const allocator_type& al = allocator_type())
: vector(al) {
_M_construct_n(_Count, value_construct_tag());
}
WJR_CONSTEXPR20 vector(data_type&& _Data, const allocator_type& al = allocator_type())
: vector(al) {
moveConstruct(getAllocator(), std::move(_Data), getData());
}
WJR_CONSTEXPR20 void resize(const size_type _Newsize, default_construct_tag) {
_M_resize(_Newsize, default_construct_tag());
}
WJR_CONSTEXPR20 void resize(const size_type _Newsize, value_construct_tag) {
_M_resize(_Newsize, value_construct_tag());
}
WJR_CONSTEXPR20 void push_back(default_construct_tag) {
emplace_back(default_construct_tag());
}
WJR_CONSTEXPR20 void push_back(value_construct_tag) {
emplace_back(value_construct_tag());
}
WJR_CONSTEXPR20 vector& append(const size_type n, default_construct_tag) {
_M_append(n, default_construct_tag());
return *this;
}
WJR_CONSTEXPR20 vector& append(const size_type n, value_construct_tag) {
_M_append(n, value_construct_tag());
return *this;
}
/*------------------------------------------------------------*/
WJR_INLINE_CONSTEXPR20 vector& chop(const size_type n) noexcept {
_M_erase_at_end(end() - n);
return *this;
}
WJR_INLINE_CONSTEXPR20 vector& truncate(const size_type n) noexcept {
return chop(size() - n);
}
WJR_INLINE_CONSTEXPR20 vector& append(const T& val) {
emplace_back(val);
return *this;
}
WJR_INLINE_CONSTEXPR20 vector& append(T&& val) {
emplace_back(std::move(val));
return *this;
}
template<typename _Iter, std::enable_if_t<is_iterator_v<_Iter>, int> = 0>
WJR_CONSTEXPR20 vector& append(_Iter _First, _Iter _Last) {
_M_range_append(_First, _Last, typename std::iterator_traits<_Iter>::iterator_category());
return *this;
}
WJR_CONSTEXPR20 vector& append(const size_type n, const T& val) {
_M_append(n, val);
return *this;
}
WJR_CONSTEXPR20 vector& append(std::initializer_list<value_type> il) {
return append(il.begin(), il.end());
}
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 vector& replace(
const_iterator _Oldfirst, const_iterator _Oldlast,
iter _Newfirst, iter _Newlast) {
const pointer __beg = begin();
const const_iterator __cbeg = cbegin();
const auto __offset1 = static_cast<size_type>(_Oldfirst - __cbeg);
const auto __offset2 = static_cast<size_type>(_Oldlast - __cbeg);
_M_range_replace(__beg + __offset1, __beg + __offset2,
_Newfirst, _Newlast, typename std::iterator_traits<iter>::iterator_category());
return *this;
}
WJR_CONSTEXPR20 vector& replace(
const_iterator _Oldfirst, const_iterator _Oldlast,
const size_type _Count, const value_type& _Val
) {
const pointer __beg = begin();
const const_iterator __cbeg = cbegin();
const auto __offset1 = static_cast<size_type>(_Oldfirst - __cbeg);
const auto __offset2 = static_cast<size_type>(_Oldlast - __cbeg);
_M_fill_replace(__beg + __offset1, __beg + __offset2,
_Count, _Val);
return *this;
}
WJR_INLINE_CONSTEXPR20 void assume_total_capacity(const size_type n) const noexcept {
WJR_ASSUME(static_cast<size_type>(endPtr() - data()) >= n);
WJR_ASSUME(capacity() >= n);
}
WJR_INLINE_CONSTEXPR20 void assume_rest_capacity(const size_type n) const noexcept {
WJR_ASSUME(static_cast<size_type>(endPtr() - lastPtr()) >= n);
WJR_ASSUME(capacity() - size() >= n);
}
private:
template<typename...Args, std::enable_if_t<is_any_index_of_v<sizeof...(Args),1,2>, int> = 0>
WJR_CONSTEXPR20 void _M_construct_n(const size_type _Count, Args&&... args) {
if (_Count != 0) {
auto& al = getAllocator();
{
data_type _Newdata(al, 0, _Count);
moveConstruct(al, std::move(_Newdata), getData());
}
set_size(_Count);
const pointer _Ptr = data();
if constexpr (sizeof...(Args) == 1) {
wjr::uninitialized_fill_n(al, _Ptr, _Count, std::forward<Args>(args)...);
}
else if constexpr (sizeof...(Args) == 2) {
wjr::uninitialized_copy(al, std::forward<Args>(args)..., _Ptr);
}
}
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_init(iter _First, iter _Last, std::input_iterator_tag) {
for (; _First != _Last; ++_First) {
emplace_back(*_First);
}
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_init(iter _First, iter _Last, std::forward_iterator_tag) {
const auto _Count = static_cast<size_type>(std::distance(_First, _Last));
_M_construct_n(_Count, _First, _Last);
}
WJR_CONSTEXPR20 void _M_erase_at_end(const_pointer _Where) noexcept {
const pointer _Myfirst = data();
const pointer _Mylast = lastPtr();
const auto __new_size = static_cast<size_type>(_Where - _Myfirst);
wjr::destroy(getAllocator(), _Myfirst + __new_size, _Mylast);
set_size(__new_size);
}
WJR_CONSTEXPR20 iterator _M_erase(iterator _Where) noexcept {
const pointer _Mylast = lastPtr();
if (_Where + 1 != _Mylast) {
wjr::move(_Where + 1, _Mylast, _Where);
}
wjr::destroy_at(getAllocator(), _Mylast - 1);
inc_size(-1);
return _Where;
}
WJR_CONSTEXPR20 iterator _M_erase(iterator _First, iterator _Last) noexcept {
const pointer _Mylast = lastPtr();
if (_First != _Last) {
if (_Last != _Mylast) {
wjr::move(_Last, _Mylast, _First);
}
_M_erase_at_end(_Mylast - (_Last - _First));
}
return _First;
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_insert(iterator _Where, iter _First, iter _Last, std::input_iterator_tag) {
if (_Where == end()) {
_M_range_append(_First, _Last, std::input_iterator_tag{});
}
else if (_First != _Last) {
vector _Tmp(_First, _Last, getAllocator());
insert(_Where, _Tmp.begin(), _Tmp.end());
}
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_insert(iterator _Where, iter _First, iter _Last, std::forward_iterator_tag) {
if (_First != _Last) {
auto& al = getAllocator();
const pointer _Myfirst = data();
const pointer _Mylast = lastPtr();
const pointer _Myend = endPtr();
const auto n = static_cast<size_type>(std::distance(_First, _Last));
const auto __rest = static_cast<size_type>(_Myend - _Mylast);
if (__rest >= n) {
const auto __elements_after = static_cast<size_type>(_Mylast - _Where);
auto __old_last = _Mylast;
if (__elements_after > n) {
wjr::uninitialized_move(al, _Mylast - n, _Mylast, _Mylast);
wjr::move_backward(_Where, __old_last - n, __old_last);
wjr::copy(_First, _Last, _Where);
}
else {
auto _Mid = _First;
std::advance(_Mid, __elements_after);
wjr::uninitialized_copy(al, _Mid, _Last, _Mylast);
wjr::uninitialized_move(al, _Where, __old_last, _Mylast + n - __elements_after);
wjr::copy(_First, _Mid, _Where);
}
inc_size(n);
}
else {
const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
const size_type _Newcapacity = getGrowthCapacity(capacity(), __old_size + n);
data_type _Newdata(al, __old_size + n, _Newcapacity, extend_tag());
const pointer _Newfirst = _Newdata.data();
wjr::uninitialized_copy(al, _First, _Last, _Newfirst + __old_pos);
wjr::uninitialized_move(al, _Myfirst, _Where, _Newfirst);
wjr::uninitialized_move(al, _Where, _Mylast, _Newfirst + __old_pos + n);
tidy();
moveConstruct(al, std::move(_Newdata), getData());
}
}
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_append(iter _First, iter _Last, std::input_iterator_tag) {
for (; _First != _Last; ++_First) {
emplace_back(*_First);
}
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_append(iter _First, iter _Last, std::forward_iterator_tag) {
if (_First != _Last) {
auto& al = getAllocator();
const pointer _Myfirst = data();
const pointer _Mylast = lastPtr();
const pointer _Myend = endPtr();
const auto n = static_cast<size_type>(std::distance(_First, _Last));
const auto __rest = static_cast<size_type>(_Myend - _Mylast);
if (__rest >= n) {
wjr::uninitialized_copy(al, _First, _Last, _Mylast);
inc_size(n);
}
else {
const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
const size_type _Newcapacity = getGrowthCapacity(capacity(), __old_size + n);
data_type _Newdata(al, __old_size + n, _Newcapacity, extend_tag());
const pointer _Newfirst = _Newdata.data();
wjr::uninitialized_copy(al, _First, _Last, _Newfirst + __old_size);
wjr::uninitialized_move(al, _Myfirst, _Mylast, _Newfirst);
tidy();
moveConstruct(al, std::move(_Newdata), getData());
}
}
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_assign(iter _First, iter _Last, std::input_iterator_tag) {
const pointer _Myfirst = data();
const pointer _Mylast = lastPtr();
pointer cur = _Myfirst;
for (; _First != _Last && cur != _Mylast; ++_First, ++cur) {
*cur = *_First;
}
if (_First == _Last) {
_M_erase_at_end(cur);
}
else {
_M_range_append(_First, _Last, std::input_iterator_tag{});
}
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_assign(iter _First, iter _Last, std::forward_iterator_tag) {
auto _Count = static_cast<size_type>(std::distance(_First, _Last));
auto& al = getAllocator();
const pointer _Myfirst = data();
const pointer _Mylast = lastPtr();
if (_Count <= size()) {
wjr::copy(_First, _Last, _Myfirst);
_M_erase_at_end(_Myfirst + _Count);
}
else if (_Count <= capacity()) {
auto _Mid = _First;
std::advance(_Mid, size());
wjr::copy(_First, _Mid, begin());
wjr::uninitialized_copy(al, _Mid, _Last, _Mylast);
set_size(_Count);
}
else {
size_type _Newcapacity = getGrowthCapacity(capacity(), _Count);
data_type _Newdata(al, _Count, _Newcapacity, extend_tag());
const pointer _Newfirst = _Newdata.data();
wjr::uninitialized_copy(al, _First, _Last, _Newfirst);
tidy();
moveConstruct(al, std::move(_Newdata), getData());
}
}
WJR_CONSTEXPR20 void _M_fill_assign(size_type _Count, const value_type& _Val) {
auto& al = getAllocator();
if (_Count > capacity()) {
tidy();
data_type _Newdata(al, _Count, _Count, extend_tag());
moveConstruct(al, std::move(_Newdata), getData());
wjr::uninitialized_fill_n(al, data(), _Count, _Val);
return;
}
if (_Count > size()) {
wjr::fill(begin(), end(), _Val);
wjr::uninitialized_fill_n(al, end(), _Count - size(), _Val);
set_size(_Count);
}
else {
_M_erase_at_end(wjr::fill_n(begin(), _Count, _Val));
}
}
WJR_CONSTEXPR20 void _M_move_assign(vector&& other, std::true_type) {
auto& al = getAllocator();
tidy();
moveConstruct(al, std::move(other.getData()), getData());
if constexpr (_Alty_traits::propagate_on_container_move_assignment::value) {
al = std::move(other.getAllocator());
}
}
WJR_CONSTEXPR20 void _M_move_assign(vector&& other, std::false_type) {
if (getAllocator() == other.getAllocator()) {
_M_move_assign(std::move(other), std::true_type{});
}
else {
_M_range_assign(
std::make_move_iterator(other.begin()),
std::make_move_iterator(other.end()),
std::random_access_iterator_tag{});
clear();
}
}
template<typename...Args>
WJR_CONSTEXPR20 void _M_realloc_insert(iterator _Where, Args&&...args) {
auto& al = getAllocator();
const pointer _Myfirst = data();
const pointer _Mylast = lastPtr();
const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
const size_type __new_size = __old_size + 1;
const size_type _Newcapacity = getGrowthCapacity(__old_size, __new_size);
data_type _Newdata(al, __new_size, _Newcapacity, extend_tag());
const pointer _Newfirst = _Newdata.data();
const pointer _Newwhere = _Newfirst + __old_pos;
wjr::construct_at(al, _Newwhere, std::forward<Args>(args)...);
wjr::uninitialized_move(al, _Myfirst, _Where, _Newfirst);
wjr::uninitialized_move(al, _Where, _Mylast, _Newwhere + 1);
tidy();
moveConstruct(al, std::move(_Newdata), getData());
}
template<typename...Args>
WJR_CONSTEXPR20 void _M_realloc_insert_at_end(Args&&...args) {
auto& al = getAllocator();
const pointer _Myfirst = data();
const pointer _Mylast = lastPtr();
const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
const auto __new_size = __old_size + 1;
const size_type _Newcapacity = getGrowthCapacity(__old_size, __new_size);
data_type _Newdata(al, __new_size, _Newcapacity, extend_tag());
const pointer _Newfirst = _Newdata.data();
const pointer _Newwhere = _Newfirst + __old_size;
wjr::construct_at(al, _Newwhere, std::forward<Args>(args)...);
wjr::uninitialized_move_n(al, _Myfirst, __old_size, _Newfirst);
tidy();
moveConstruct(al, std::move(_Newdata), getData());
}
WJR_CONSTEXPR20 void _M_fill_insert(iterator _Where, size_type n, const value_type& _Val) {
if (n == 0) {
return;
}
auto& al = getAllocator();
const pointer _Myfirst = data();
const pointer _Mylast = lastPtr();
const pointer _Myend = endPtr();
const auto __rest = static_cast<size_type>(_Myend - _Mylast);
if (__rest >= n) {
const temporary_allocator_value<Alloc> _Tmp(al, _Val);
const auto& _Copy = _Tmp.value();
const auto __elements_after = static_cast<size_type>(_Mylast - _Where);
if (__elements_after > n) {
wjr::uninitialized_move(al, _Mylast - n, _Mylast, _Mylast);
wjr::move_backward(_Where, _Mylast - n, _Mylast);
wjr::fill_n(_Where, n, _Copy);
}
else {
wjr::uninitialized_fill_n(al, _Mylast, n - __elements_after, _Copy);
wjr::uninitialized_move(al, _Where, _Mylast, _Where + n);
wjr::fill(_Where, _Mylast, _Copy);
}
inc_size(n);
}
else {
const auto _Newcapacity = getGrowthCapacity(capacity(), size() + n);
data_type _Newdata(al, size() + n, _Newcapacity, extend_tag());
const pointer _Newfirst = _Newdata.data();
const auto __old_pos = static_cast<size_type>(_Where - _Myfirst);
wjr::uninitialized_fill_n(al, _Newfirst + __old_pos, n, _Val);
wjr::uninitialized_move(al, _Myfirst, _Where, _Newfirst);
wjr::uninitialized_move(al, _Where, _Mylast, _Newfirst + __old_pos + n);
tidy();
moveConstruct(al, std::move(_Newdata), getData());
}
}
template<typename _Ty>
WJR_CONSTEXPR20 void _M_resize(const size_type _Newsize, const _Ty& _Val) {
const auto _Oldsize = size();
if (_Newsize > _Oldsize) {
_M_append(_Newsize - _Oldsize, _Val);
}
else if (_Newsize < _Oldsize) {
_M_erase_at_end(data() + _Newsize);
}
}
template<typename _Ty>
WJR_CONSTEXPR20 void _M_append(size_type n, const _Ty& _Val) {
auto& al = getAllocator();
const auto _Oldsize = size();
const auto _Oldcapacity = capacity();
const pointer _Myfirst = data();
const pointer _Mylast = _Myfirst + _Oldsize;
const auto __rest = _Oldcapacity - _Oldsize;
const auto _Newsize = _Oldsize + n;
if (__rest >= n) {
wjr::uninitialized_fill_n(al, _Mylast, n, _Val);
set_size(_Newsize);
}
else {
auto _Newcapacity = getGrowthCapacity(_Oldcapacity, _Newsize);
data_type _Newdata(al, _Newsize, _Newcapacity, extend_tag());
const pointer _Newfirst = _Newdata.data();
wjr::uninitialized_fill_n(al, _Newfirst + _Oldsize, n, _Val);
wjr::uninitialized_move(al, _Myfirst, _Mylast, _Newfirst);
tidy();
moveConstruct(al, std::move(_Newdata), getData());
}
}
template<typename Args>
WJR_CONSTEXPR20 void _M_insert_aux(iterator _Where, Args&& args) {
auto& al = getAllocator();
const pointer _Mylast = lastPtr();
wjr::construct_at(al, _Mylast, std::move(*(_Mylast - 1)));
wjr::move_backward(_Where, _Mylast - 1, _Mylast);
*_Where = std::forward<Args>(args);
inc_size(1);
}
template<typename...Args>
WJR_CONSTEXPR20 iterator _M_emplace_aux(const_iterator _Where, Args&&...args) {
const auto __offset = _Where - cbegin();
auto& al = getAllocator();
const pointer _Mylast = lastPtr();
const pointer _Myend = endPtr();
if (_Mylast != _Myend) {
if (_Where == _Mylast) {
wjr::construct_at(al, _Mylast, std::forward<Args>(args)...);
inc_size(1);
}
else {
temporary_allocator_value<Alloc> _Tmp(al, std::forward<Args>(args)...);
_M_insert_aux(begin() + __offset, std::move(_Tmp.value()));
}
}
else {
_M_realloc_insert(begin() + __offset, std::forward<Args>(args)...);
}
return begin() + __offset;
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_replace(
iterator _Oldfirst, iterator _Oldlast,
iter _Newfirst, iter _Newlast, std::input_iterator_tag
) {
for (; _Oldfirst != _Oldlast && _Newfirst != _Newlast; ++_Oldfirst, ++_Newfirst) {
*_Oldfirst = *_Newfirst;
}
if (_Newfirst == _Newlast) {
_M_erase_at_end(_Oldfirst, _Oldlast);
}
else {
_M_range_insert(_Oldlast, _Newfirst, _Newlast, std::input_iterator_tag());
}
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_replace(
iterator _Oldfirst, iterator _Oldlast,
iter _Newfirst, iter _Newlast, std::forward_iterator_tag
) {
const auto __n = static_cast<size_type>(_Oldlast - _Oldfirst);
const auto __m = static_cast<size_type>(std::distance(_Newfirst, _Newlast));
if (__m <= __n) {
erase(wjr::copy_n(_Newfirst, __m, _Oldfirst), _Oldlast);
}
else {
const auto __delta = __m - __n;
auto& al = getAllocator();
const auto _Myfirst = data();
const auto _Mylast = lastPtr();
const auto _Myend = endPtr();
const auto __rest = static_cast<size_type>(_Myend - _Mylast);
if (__rest >= __delta) {
const auto __elements_after = static_cast<size_type>(_Mylast - _Oldfirst);
if (__elements_after > __m) {
wjr::uninitialized_move(al, _Mylast - __delta, _Mylast, _Mylast);
wjr::move_backward(_Oldlast, _Mylast - __delta, _Mylast);
wjr::copy(_Newfirst, _Newlast, _Oldfirst);
}
else {
auto _Mid = _Newfirst;
std::advance(_Mid, __elements_after);
wjr::uninitialized_copy(al, _Mid, _Newlast, _Mylast);
wjr::uninitialized_move(al, _Oldlast, _Mylast, _Oldfirst + __m);
wjr::copy(_Newfirst, _Mid, _Oldfirst);
}
inc_size(__delta);
}
else {
const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
const auto __old_pos = static_cast<size_type>(_Oldfirst - _Myfirst);
const auto _Newcapacity = getGrowthCapacity(capacity(), __old_size + __delta);
data_type _Newdata(al, __old_size + __delta, _Newcapacity, extend_tag());
const pointer _Ptr = _Newdata.data();
wjr::uninitialized_copy(al, _Newfirst, _Newlast, _Ptr + __old_pos);
wjr::uninitialized_move(al, _Myfirst, _Oldfirst, _Ptr);
wjr::uninitialized_move(al, _Oldlast, _Mylast, _Ptr + __old_pos + __m);
tidy();
moveConstruct(al, std::move(_Newdata), getData());
}
}
}
WJR_CONSTEXPR20 void _M_fill_replace(
iterator _Oldfirst, iterator _Oldlast,
size_type __m, const value_type& _Val) {
const auto __n = static_cast<size_type>(_Oldlast - _Oldfirst);
if (__m <= __n) {
erase(wjr::fill_n(_Oldfirst, __m, _Val), _Oldlast);
}
else {
const auto __delta = __m - __n;
auto& al = getAllocator();
const auto _Myfirst = data();
const auto _Mylast = lastPtr();
const auto _Myend = endPtr();
const auto __rest = static_cast<size_type>(_Myend - _Mylast);
if (__rest >= __delta) {
const temporary_allocator_value<Alloc> _Tmp(al, _Val);
const auto& _Copy = _Tmp.value();
const auto __elements_after = static_cast<size_type>(_Mylast - _Oldfirst);
if (__elements_after > __m) {
wjr::uninitialized_move(al, _Mylast - __delta, _Mylast, _Mylast);
wjr::move_backward(_Oldlast, _Mylast - __delta, _Mylast);
wjr::fill_n(_Oldfirst, __m, _Copy);
}
else {
wjr::uninitialized_fill_n(al, _Mylast, __m - __elements_after, _Copy);
wjr::uninitialized_move(al, _Oldlast, _Mylast, _Oldfirst + __m);
wjr::fill(_Oldfirst, _Mylast, _Copy);
}
inc_size(__delta);
}
else {
const auto __old_size = static_cast<size_type>(_Mylast - _Myfirst);
const auto __old_pos = static_cast<size_type>(_Oldfirst - _Myfirst);
const auto _Newcapacity = getGrowthCapacity(capacity(), __old_size + __delta);
data_type _Newdata(al, __old_size + __delta, _Newcapacity, extend_tag());
const pointer _Ptr = _Newdata.data();
wjr::uninitialized_fill_n(al, _Ptr + __old_pos, __m, _Val);
wjr::uninitialized_move(al, _Myfirst, _Oldfirst, _Ptr);
wjr::uninitialized_move(al, _Oldlast, _Mylast, _Ptr + __old_pos + __m);
tidy();
moveConstruct(al, std::move(_Newdata), getData());
}
}
}
private:
wjr::compressed_pair<_Alty, data_type> _Myval;
};
template<typename iter,
typename v = typename std::iterator_traits<iter>::value_type,
typename Alloc = std::allocator<v>,
typename Data = vector_data<v, Alloc>,
std::enable_if_t<is_iterator_v<iter>, int> = 0>
vector(iter, iter, Alloc = Alloc()) -> vector<v, Data>;
template<typename T, size_t N, typename Alloc = std::allocator<T>>
using static_vector = vector<T, Alloc, vector_static_data<T, N, Alloc>>;
template<typename T, size_t N, typename Alloc = std::allocator<T>>
using sso_vector = vector<T, Alloc, vector_sso_data<T, N, Alloc>>;
template<typename T, typename Alloc, typename Data>
bool operator==(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template<typename T, typename Alloc, typename Data>
bool operator!=(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
return !(lhs == rhs);
}
template<typename T, typename Alloc, typename Data>
bool operator<(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
return wjr::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template<typename T, typename Alloc, typename Data>
bool operator>(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
return rhs < lhs;
}
template<typename T, typename Alloc, typename Data>
bool operator<=(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
return !(rhs < lhs);
}
template<typename T, typename Alloc, typename Data>
bool operator>=(const vector<T, Alloc, Data>& lhs, const vector<T, Alloc, Data>& rhs) {
return !(lhs < rhs);
}
_WJR_END
namespace std {
template<typename T, typename Alloc, typename Data>
WJR_CONSTEXPR20 void swap(wjr::vector<T, Alloc, Data>& lhs, wjr::vector<T, Alloc, Data>& rhs)
noexcept(noexcept(lhs.swap(rhs))) {
lhs.swap(rhs);
}
template<typename T, typename Alloc, typename Data>
WJR_CONSTEXPR20 typename wjr::vector<T, Alloc, Data>::size_type erase(wjr::vector<T, Alloc, Data>& c, const T& value) {
auto it = std::remove(c.begin(), c.end(), value);
const auto n = std::distance(it, c.end());
c.erase(it, c.end());
return n;
}
template<typename T, typename Alloc, typename Data, typename Pred>
WJR_CONSTEXPR20 typename wjr::vector<T, Alloc, Data>::size_type erase_if(wjr::vector<T, Alloc, Data>& c, Pred pred) {
auto it = std::remove_if(c.begin(), c.end(), pred);
const auto n = std::distance(it, c.end());
c.erase(it, c.end());
return n;
}
}
#pragma pop_macro("new")
#endif // !__WJR_VECTOR_H

#pragma once
#ifndef __WJR_ERROR_HANDLE_H
#define __WJR_ERROR_HANDLE_H
namespace wjr {
enum class errc {
ok               = 0x00,
overflow         = 0x01,
underflow        = 0x02,
out_of_range     = overflow | underflow,
empty			 = 0x04,
noconv           = 0x05,
buffer_too_small = 0x06,
};
}
#endif // __WJR_ERROR_HANDLE_H

_WJR_BEGIN
/*---------------------------data for string---------------------------*/
template<typename Char, size_t N, typename Alloc>
struct string_static_data : public vector_static_data<Char, N, Alloc> {
using _Mybase = vector_static_data<Char, N, Alloc>;
using _Alty = typename _Mybase::_Alty;
using _Alty_traits = typename _Mybase::_Alty_traits;
using value_type = typename _Mybase::value_type;
using pointer = typename _Mybase::pointer;
using const_pointer = typename _Mybase::const_pointer;
using size_type = typename _Mybase::size_type;
using difference_type = typename _Mybase::difference_type;
constexpr static size_t max_capacity = _Mybase::max_capacity - 1;
WJR_INLINE_CONSTEXPR20 string_static_data() : _Mybase() {}
string_static_data(const string_static_data&) = delete;
string_static_data& operator=(const string_static_data&) = delete;
WJR_CONSTEXPR20 ~string_static_data() = default;
WJR_CONSTEXPR20 string_static_data(
_Alty& al,
const size_type _Newsize,
const size_type _Newcapacity,
extend_tag)
: _Mybase(al, _Newsize, _Newcapacity + 1, extend_tag{}) {
}
WJR_CONSTEXPR20 string_static_data(
_Alty& al,
const size_type _Newsize,
const size_type _Newcapacity)
: _Mybase(al, _Newsize, _Newcapacity + 1) {
}
WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const string_static_data& _Src, string_static_data& _Dest) {
return _Mybase::copyConstruct(al, std::move(_Src), _Dest);
}
WJR_INLINE_CONSTEXPR20 static void moveConstruct(_Alty& al,
string_static_data&& _Src, string_static_data& _Dest) {
return _Mybase::moveConstruct(al, std::move(_Src), _Dest);
}
WJR_CONSTEXPR20 static void Deallocate(_Alty& al, string_static_data& _Data) {
return _Mybase::Deallocate(al, _Data);
}
WJR_CONSTEXPR20 static void Tidy(_Alty& al, string_static_data& _Data) {
return _Mybase::Tidy(al, _Data);
}
WJR_INLINE_CONSTEXPR20 static void shrinkToFit(_Alty& al, string_static_data& _Data) {
return _Mybase::shrinkToFit(al, _Data);
}
WJR_ATTRIBUTE(CONST, INLINE_CONSTEXPR20) size_type capacity() const noexcept {
return _Mybase::capacity() - 1;
}
WJR_INLINE_CONSTEXPR20 pointer endPtr() noexcept {
return _Mybase::data() + capacity();
}
WJR_INLINE_CONSTEXPR20 const_pointer endPtr() const noexcept {
return _Mybase::data() + capacity();
}
};
template<typename Char, size_t N, typename Alloc>
struct string_sso_data : public vector_sso_data<Char, N, Alloc> {
using _Mybase = vector_sso_data<Char, N, Alloc>;
using _Alty = typename _Mybase::_Alty;
using _Alty_traits = typename _Mybase::_Alty_traits;
using value_type = typename _Mybase::value_type;
using pointer = typename _Mybase::pointer;
using const_pointer = typename _Mybase::const_pointer;
using size_type = typename _Mybase::size_type;
using difference_type = typename _Mybase::difference_type;
constexpr static size_t max_capacity = _Mybase::max_capacity - 1;
WJR_INLINE_CONSTEXPR20 string_sso_data() : _Mybase() {}
string_sso_data(const string_sso_data&) = delete;
string_sso_data& operator=(const string_sso_data&) = delete;
WJR_CONSTEXPR20 ~string_sso_data() = default;
WJR_CONSTEXPR20 string_sso_data(
_Alty& al,
const size_type _Newsize,
const size_type _Newcapacity,
extend_tag)
: _Mybase(al, _Newsize, _Newcapacity + 1, extend_tag{}) {
}
WJR_CONSTEXPR20 string_sso_data(
_Alty& al,
const size_type _Newsize,
const size_type _Newcapacity)
: _Mybase(al, _Newsize, _Newcapacity + 1) {
}
WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const string_sso_data& _Src, string_sso_data& _Dest) {
const auto _Size = _Src._M_size;
if (!_Src.is_small()) {
string_sso_data data(al, _Size, _Size, extend_tag{});
_Dest._M_ptr = data._M_ptr;
_Dest._M_capacity = data._M_capacity;
}
_Dest._M_size = _Size;
wjr::uninitialized_copy_n(al, _Src.data(), _Src.size(), _Dest.data());
}
WJR_INLINE_CONSTEXPR20 static void moveConstruct(_Alty& al, string_sso_data&& _Src, string_sso_data& _Dest) {
return _Mybase::moveConstruct(al, std::move(_Src), _Dest);
}
WJR_CONSTEXPR20 static void Deallocate(_Alty& al, string_sso_data& _Data) {
return _Mybase::Deallocate(al, _Data);
}
WJR_CONSTEXPR20 static void Tidy(_Alty& al, string_sso_data& _Data) {
return _Mybase::Tidy(al, _Data);
}
WJR_INLINE_CONSTEXPR20 static void shrinkToFit(_Alty& al, string_sso_data& _Data) {
if (!_Data.is_small()) {
const auto _Oldsize = _Data.size();
const auto _Oldcapacity = _Data.capacity();
if (_Oldsize != _Oldcapacity) {
if (_Oldsize <= max_capacity) {
wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, reinterpret_cast<pointer>(_Data._M_storage));
wjr::destroy_n(al, _Data.data(), _Oldsize);
_Alty_traits::deallocate(al, _Data.data(), _Oldcapacity + 1);
_Data.reset_ptr();
}
else {
auto _Newdata = _Alty_traits::allocate(al, _Oldsize + 1);
wjr::uninitialized_move_n(al, _Data.data(), _Oldsize, _Newdata);
wjr::destroy_n(al, _Data.data(), _Oldsize);
_Alty_traits::deallocate(al, _Data.data(), _Oldcapacity + 1);
_Data._M_ptr = _Newdata;
_Data._M_capacity = _Oldsize + 1;
}
_Data._M_size = _Oldsize;
}
}
}
WJR_ATTRIBUTE(CONST, INLINE_CONSTEXPR20) size_type capacity() const noexcept {
return _Mybase::capacity() - 1;
}
WJR_INLINE_CONSTEXPR20 pointer endPtr() noexcept {
return _Mybase::data() + capacity();
}
WJR_INLINE_CONSTEXPR20 const_pointer endPtr() const noexcept {
return _Mybase::data() + capacity();
}
};
template<typename Traits>
struct __traits_helper {
using is_default_equal = std::false_type;
using is_default_less = std::false_type;
};
template<typename Char>
struct __traits_helper<std::char_traits<Char>> {
using is_default_equal = std::true_type;
using is_default_less = std::true_type;
};
template<typename Derived>
struct __base_string_view_traits;
template<template<typename...>typename C, typename Char, typename Traits>
struct __base_string_view_traits<C<Char, Traits>> {
using _Derived = C<Char, Traits>;
using _Std_view_type = std::basic_string_view<Char, Traits>;
template<typename T>
struct _Is_string_view_like : std::conjunction<
std::is_convertible<const T&, _Derived>
> {};
template<typename T>
constexpr static bool _Is_string_view_like_v = _Is_string_view_like<T>::value;
template<typename T>
struct _Is_noptr_std_string_view_like : std::conjunction<
std::is_convertible<const T&, _Std_view_type>,
std::negation<std::is_convertible<const T&, const Char*>>
> {};
template<typename T>
constexpr static bool _Is_noptr_std_string_view_like_v = _Is_noptr_std_string_view_like<T>::value;
template<typename T>
struct _Is_noptr_string_view_like : std::conjunction<
_Is_string_view_like<T>,
std::negation<std::is_convertible<const T&, const Char*>>
> {};
template<typename T>
constexpr static bool _Is_noptr_string_view_like_v = _Is_noptr_string_view_like<T>::value;
using traits_type = Traits;
using value_type = Char;
using pointer = Char*;
using const_pointer = const Char*;
using reference = Char&;
using const_reference = const Char&;
using const_iterator = const Char*;
using iterator = const_iterator;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;
using reverse_iterator = const_reverse_iterator;
using size_type = size_t;
using difference_type = ptrdiff_t;
static constexpr auto npos = static_cast<size_type>(-1);
};
template<typename Char, typename Traits = std::char_traits<Char>>
class basic_string_view {
protected:
using _Mybase = __base_string_view_traits<basic_string_view<Char, Traits>>;
using _Traits_helper = __traits_helper<Traits>;
using _Std_view_type = typename _Mybase::_Std_view_type;
template<typename T>
using _Is_string_view_like = typename _Mybase::template _Is_string_view_like<T>;
template<typename T>
constexpr static bool _Is_string_view_like_v = _Is_string_view_like<T>::value;
template<typename T>
using _Is_noptr_std_string_view_like = typename _Mybase::template _Is_noptr_std_string_view_like<T>;
template<typename T>
constexpr static bool _Is_noptr_std_string_view_like_v = _Is_noptr_std_string_view_like<T>::value;
template<typename T>
using _Is_noptr_string_view_like = typename _Mybase::template _Is_noptr_string_view_like<T>;
template<typename T>
constexpr static bool _Is_noptr_string_view_like_v = _Is_noptr_string_view_like<T>::value;
public:
using traits_type = typename _Mybase::traits_type;
using value_type = typename _Mybase::value_type;
using pointer = typename _Mybase::pointer;
using const_pointer = typename _Mybase::const_pointer;
using reference = typename _Mybase::reference;
using const_reference = typename _Mybase::const_reference;
using const_iterator = typename _Mybase::const_iterator;
using iterator = typename _Mybase::iterator;
using const_reverse_iterator = typename _Mybase::const_reverse_iterator;
using reverse_iterator = typename _Mybase::reverse_iterator;
using size_type = typename _Mybase::size_type;
using difference_type = typename _Mybase::difference_type;
constexpr static size_type npos = _Mybase::npos;
public:
static_assert(!std::is_array_v<Char>&& std::is_trivial_v<Char>&& std::is_standard_layout_v<Char>,
"The character type of basic_string_view must be a non-array trivial standard-layout type. See N4910 "
"23.1 [strings.general]/1.");
static_assert(std::is_same_v<Char, typename Traits::char_type>, "");
WJR_INLINE_CONSTEXPR void swap(basic_string_view& other) noexcept {
const basic_string_view tmp{ other };
other = *this;
*this = tmp;
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view substr(
const size_type off = 0, size_type n = npos) const WJR_NOEXCEPT {
#if defined(_WJR_EXCEPTION)
if (WJR_LIKELY(off <= size())) {
#endif // _WJR_EXCEPTION
return nesubstr(off, n);
#if defined(_WJR_EXCEPTION)
}
throw std::out_of_range("out of range at basic_string_view::substr");
#endif // _WJR_EXCEPTION
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view
nesubstr(const size_type off = 0, size_type n = npos) const noexcept {
n = std::min(n, size() - off);
return basic_string_view(data() + off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_view view(
const value_type* s, const size_type n) noexcept {
return basic_string_view(s, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_view view(
const value_type* s) noexcept {
return basic_string_view(s);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_view view(
std::initializer_list<value_type> il) noexcept {
return view(il.begin(), il.size());
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_view view(const StringView& t) noexcept {
return basic_string_view(t);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_view view(const StringView& t,
const size_type off, const size_type n = npos) noexcept {
return view(t).nesubstr(off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view view() const noexcept {
return *this;
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_view eview(const StringView& t,
const size_type off, const size_type n = npos) WJR_NOEXCEPT {
return view(t).substr(off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view prefix(size_type n) const noexcept {
n = std::min(n, size());
return basic_string_view(begin(), n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view suffix(size_type n) const noexcept {
n = std::min(n, size());
return basic_string_view(end() - n, n);
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) basic_string_view adjust_begin(const_iterator iter) const noexcept {
return basic_string_view(iter, static_cast<size_type>(end() - iter));
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) basic_string_view adjust_end(const_iterator iter) const noexcept {
return basic_string_view(begin(), static_cast<size_type>(iter - begin()));
}
private:
constexpr static size_type traits_length(const value_type* s) {
return traits_type::length(s);
}
public:
WJR_INLINE_CONSTEXPR basic_string_view() noexcept : _Mydata(), _Mysize(0) {}
WJR_INLINE_CONSTEXPR basic_string_view(const basic_string_view&) noexcept = default;
WJR_INLINE_CONSTEXPR basic_string_view& operator=(const basic_string_view&) noexcept = default;
WJR_INLINE_CONSTEXPR basic_string_view(const const_pointer ptr)
: _Mydata(ptr), _Mysize(traits_length(ptr)) {}
WJR_INLINE_CONSTEXPR basic_string_view(
const const_pointer ptr, const size_type n)
: _Mydata(ptr), _Mysize(n) {}
private:
WJR_INLINE_CONSTEXPR basic_string_view(_Std_view_type s, disable_tag) noexcept
: _Mydata(s.data()), _Mysize(s.size()) {}
public:
template<typename StringView, std::enable_if_t<_Is_noptr_std_string_view_like_v<StringView>, int> = 0>
WJR_INLINE_CONSTEXPR basic_string_view(const StringView& t) noexcept
: basic_string_view(_Std_view_type(t), disable_tag{}) {}
WJR_NODISCARD WJR_INLINE_CONSTEXPR operator _Std_view_type() const noexcept {
return { data(), size() };
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_iterator begin() const noexcept {
return const_iterator(_Mydata);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_iterator end() const noexcept {
return const_iterator(_Mydata + _Mysize);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_iterator cbegin() const noexcept {
return begin();
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_iterator cend() const noexcept {
return end();
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reverse_iterator rbegin() const noexcept {
return const_reverse_iterator{ end() };
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reverse_iterator rend() const noexcept {
return const_reverse_iterator{ begin() };
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reverse_iterator crbegin() const noexcept {
return rbegin();
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reverse_iterator crend() const noexcept {
return rend();
}
WJR_ATTRIBUTE(CONST, INLINE_CONSTEXPR20) size_type size() const noexcept {
return _Mysize;
}
WJR_ATTRIBUTE(CONST, INLINE_CONSTEXPR20) size_type length() const noexcept {
return _Mysize;
}
WJR_ATTRIBUTE(CONST, INLINE_CONSTEXPR20) bool empty() const noexcept {
return _Mysize == 0;
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_pointer data() const noexcept {
return _Mydata;
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR size_type max_size() const noexcept {
return std::min(static_cast<size_t>(PTRDIFF_MAX), static_cast<size_t>(-1) / sizeof(Char));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reference operator[](const size_type off) const noexcept {
return _Mydata[off];
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reference at(const size_type off) const {
if (off >= size()) {
throw std::out_of_range("error at basic_string_view::at");
}
return _Mydata[off];
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reference front() const noexcept {
return _Mydata[0];
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR const_reference back() const noexcept {
return _Mydata[_Mysize - 1];
}
WJR_INLINE_CONSTEXPR void remove_prefix(const size_type n) noexcept {
_Mydata += n;
_Mysize -= n;
}
WJR_INLINE_CONSTEXPR void remove_suffix(const size_type n) noexcept {
_Mysize -= n;
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(const StringView& t) const noexcept {
const auto sv = view(t);
if constexpr (_Traits_helper::is_default_equal::value) {
if constexpr (std::is_same_v<Char, char>) {
if (!wjr::is_constant_evaluated()) {
return wjr::compare(
reinterpret_cast<const uint8_t*>(begin()),
reinterpret_cast<const uint8_t*>(end()),
reinterpret_cast<const uint8_t*>(sv.begin()),
reinterpret_cast<const uint8_t*>(sv.end())
);
}
auto _First1 = begin();
auto _Last1 = end();
auto _First2 = sv.begin();
auto _Last2 = sv.end();
for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, ++_First2) {
if (*_First1 < *_First2) {
return -1;
}
if (*_First2 < *_First1) {
return 1;
}
}
return _First1 == _Last1 ? (_First2 == _Last2 ? 0 : -1) : 1;
}
else {
return wjr::compare(begin(), end(), sv.begin(), sv.end());
}
}
else {
const int ret = traits_type::compare(begin(), sv.begin(), std::min(size(), sv.size()));
return ret != 0 ? ret : (size() < sv.size() ? -1 : 1);
}
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(const size_type off, const size_type n, const StringView& t)
const WJR_NOEXCEPT {
return eview(*this, off, n).compare(t);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(
const size_type off1, const size_type n1, const StringView& t,
const size_type off2, const size_type n2)
const WJR_NOEXCEPT {
return eview(*this, off1, n1).compare(eview(t, off2, n2));
}
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(const Char* const ptr) const {
return compare(view(ptr));
}
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(const size_type off, const size_type n, const Char* const ptr) const {
return eview(*this, off, n).compare(view(ptr));
}
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(const size_type off1, const size_type n1,
const Char* const ptr, const size_type n2) const {
return eview(*this, off1, n1).compare(eview(ptr, n2));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const StringView& t) const noexcept {
const auto sv = view(t);
return prefix(sv.size()).equal(sv);
}
WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const Char ch) const noexcept {
return !empty() && traits_type::eq(front(), ch);
}
WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const Char* const other) const {
return starts_with(view(other));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const StringView& t) const noexcept {
const auto sv = view(t);
return suffix(sv.size()).equal(sv);
}
WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const Char ch) const noexcept {
return !empty() && traits_type::eq(back(), ch);
}
WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const Char* const other) const {
return ends_with(view(other));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const StringView& t) const noexcept {
return find(t) != npos;
}
WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const Char other) const noexcept {
return find(other) != npos;
}
WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const Char* const other) const {
return find(other) != npos;
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const StringView& t, const size_type off = 0) const noexcept {
if (WJR_LIKELY(off <= size())) {
const auto sv1 = view(*this, off);
const auto sv2 = view(t);
if constexpr (_Traits_helper::is_default_equal::value) {
auto iter = wjr::search(sv1.begin(), sv1.end(), sv2.begin(), sv2.end());
return iter == end() ? npos : static_cast<size_type>(iter - begin());
}
else {
auto iter = wjr::search(sv1.begin(), sv1.end(), sv2.begin(), sv2.end(),
[](auto x, auto y) {
return traits_type::eq(x, y);
});
return iter == end() ? npos : static_cast<size_type>(iter - begin());
}
}
return npos;
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const Char ch, const size_type off = 0) const noexcept {
if (WJR_LIKELY(off <= size())) {
const auto sv = view(*this, off);
if constexpr (_Traits_helper::is_default_equal::value) {
auto iter = wjr::find(sv.begin(), sv.end(), ch);
return iter == end() ? npos : static_cast<size_type>(iter - begin());
}
else {
auto iter = traits_type::find(sv.begin(), sv.end(), ch);
return iter == end() ? npos : static_cast<size_type>(iter - begin());
}
}
return npos;
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const Char* const ptr, const size_type off,
const size_type n) const {
return find(view(ptr, n), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const Char* const ptr, const size_type off = 0)
const {
return find(view(ptr), off);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const StringView& t, const size_type off = npos) const noexcept {
if (WJR_LIKELY(size() != 0)) {
const auto sv1 = view(*this, 0, std::min(off, size() - 1) + 1);
const auto sv2 = view(t);
if constexpr (_Traits_helper::is_default_equal::value) {
auto iter = wjr::search(sv1.rbegin(), sv1.rend(),
sv2.rbegin(), sv2.rend());
return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - sv2.size();
}
else {
auto iter = wjr::search(sv1.rbegin(), sv1.rend(),
sv2.rbegin(), sv2.rend(),
[](auto x, auto y) {
return traits_type::eq(x, y);
});
return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - sv2.size();
}
}
return npos;
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const Char ch, const size_type off = npos) const noexcept {
if (WJR_LIKELY(size() != 0)) {
const auto sv = view(*this, 0, std::min(off, size() - 1) + 1);
if constexpr (_Traits_helper::is_default_equal::value) {
auto iter = wjr::find(sv.rbegin(), sv.rend(), ch);
return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - 1;
}
else {
auto iter = wjr::find(sv.rbegin(), sv.rend(), ch,
[](auto x, auto y) {
return traits_type::eq(x, y);
});
return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - 1;
}
}
return npos;
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const Char* const ptr, const size_type off,
const size_type n) const {
return rfind(view(ptr, n), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const Char* const ptr, const size_type off = npos)
const {
return rfind(view(ptr), off);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(const StringView& t,
const size_type off = 0) const noexcept {
auto sv = _Std_view_type(view(t));
return _Std_view_type(*this).find_first_of(sv, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(const Char ch, const size_type off = 0) const noexcept {
return find(ch, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(const Char* const ptr, const size_type off,
const size_type n) const {
return find_first_of(view(ptr, n), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(
const Char* const ptr, const size_type off = 0) const {
return find_first_of(view(ptr), off);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(const StringView& t,
const size_type off = npos) const noexcept {
const auto sv = _Std_view_type(view(t));
return _Std_view_type(*this).find_last_of(sv, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(const Char ch, const size_type off = npos) const noexcept {
return rfind(ch, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(const Char* const ptr, const size_type off,
const size_type n) const {
return find_last_of(view(ptr, n), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(
const Char* const ptr, const size_type off = npos) const {
return find_last_of(view(ptr), off);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(const StringView& t,
const size_type off = 0) const noexcept {
const auto sv = _Std_view_type(view(t));
return _Std_view_type(*this).find_first_not_of(sv, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(const Char ch, const size_type off = 0) const noexcept {
if (WJR_LIKELY(off <= size())) {
const auto sv = view(*this, off);
if constexpr (_Traits_helper::is_default_equal::value) {
auto iter = wjr::find(sv.begin(), sv.end(), ch, std::not_equal_to<>{});
return iter == end() ? npos : static_cast<size_type>(iter - begin());
}
else {
auto iter = wjr::find(sv.begin(), sv.end(), ch,
[](auto x, auto y) {
return !traits_type::eq(x, y);
});
return iter == end() ? npos : static_cast<size_type>(iter - begin());
}
}
return npos;
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(const Char* const ptr, const size_type off,
const size_type n) const {
return find_first_not_of(view(ptr, n), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(
const Char* const ptr, const size_type off = 0) const {
return find_first_not_of(view(ptr), off);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(const StringView& t,
const size_type off = npos) const noexcept {
const auto sv = _Std_view_type(view(t));
return _Std_view_type(*this).find_last_not_of(sv, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(const Char ch, const size_type off = npos) const noexcept {
if (WJR_LIKELY(size() != 0)) {
const auto sv = view(*this, 0, std::min(off, size() - 1) + 1);
if constexpr (_Traits_helper::is_default_equal::value) {
auto iter = wjr::find(sv.rbegin(), sv.rend(), ch, std::not_equal_to<>{});
return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - 1;
}
else {
auto iter = wjr::find(sv.rbegin(), sv.rend(), ch,
[](auto x, auto y) {
return !traits_type::eq(x, y);
});
return iter == rend() ? npos : static_cast<size_type>(rend() - iter) - 1;
}
}
return npos;
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(const Char* const ptr, const size_type off,
const size_type count) const {
return find_last_not_of(view(ptr, count), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(
const Char* const ptr, const size_type off = npos) const {
return find_last_not_of(view(ptr), off);
}
/*------External extension function------*/
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR20) bool equal(const StringView& t) const noexcept {
const auto sv = view(t);
if constexpr (_Traits_helper::is_default_equal::value) {
return wjr::equal(begin(), end(), sv.begin(), sv.end());
}
else {
return compare(sv) == 0;
}
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR20) bool equal(const size_type off, const size_type n, const StringView& t)
const WJR_NOEXCEPT {
return eview(*this, off, n).equal(view(t));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR20) bool equal(const size_type off1, const size_type n1, const StringView& t,
const size_type off2, const size_type n2) const WJR_NOEXCEPT {
return eview(*this, off1, n1).equal(eview(t, off2, n2));
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR20) bool equal(const Char* const ptr) const {
return equal(view(ptr));
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR20) bool equal(const size_type off, const size_type n, const Char* const ptr) const {
return equal(off, n, view(ptr));
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR20) bool equal(const size_type off1, const size_type n1,
const Char* const ptr, const size_type n2) const {
return equal(off1, n1, view(ptr, n2));
}
private:
const_pointer _Mydata;
size_type _Mysize;
};
template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
const basic_string_view<Char, Traits> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return lhs.equal(rhs);
}
template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
const type_identity_t<basic_string_view<Char, Traits>> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return lhs.equal(rhs);
}
template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
const basic_string_view<Char, Traits> lhs,
const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
return lhs.equal(rhs);
}
template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
const basic_string_view<Char, Traits> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return !(lhs == rhs);
}
template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
const type_identity_t<basic_string_view<Char, Traits>> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return !(lhs == rhs);
}
template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
const basic_string_view<Char, Traits> lhs,
const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
return !(lhs == rhs);
}
template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
const basic_string_view<Char, Traits> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return lhs.compare(rhs) < 0;
}
template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
const type_identity_t<basic_string_view<Char, Traits>> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return lhs.compare(rhs) < 0;
}
template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
const basic_string_view<Char, Traits> lhs,
const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
return lhs.compare(rhs) < 0;
}
template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
const basic_string_view<Char, Traits> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return rhs < lhs;
}
template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
const type_identity_t<basic_string_view<Char, Traits>> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return rhs < lhs;
}
template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
const basic_string_view<Char, Traits> lhs,
const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
return rhs < lhs;
}
template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
const basic_string_view<Char, Traits> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return !(rhs < lhs);
}
template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
const type_identity_t<basic_string_view<Char, Traits>> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return !(rhs < lhs);
}
template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
const basic_string_view<Char, Traits> lhs,
const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
return !(rhs < lhs);
}
template<typename Char, typename Traits>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
const basic_string_view<Char, Traits> lhs,
const basic_string_view<Char, Traits> rhs) noexcept {
return !(lhs < rhs);
}
template<typename Char, typename Traits, int = 1>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
const type_identity_t<basic_string_view<Char, Traits>> lhs,
const basic_string_view<Char> rhs) noexcept {
return !(lhs < rhs);
}
template<typename Char, typename Traits, int = 2>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
const basic_string_view<Char, Traits> lhs,
const type_identity_t<basic_string_view<Char, Traits>> rhs) noexcept {
return !(lhs < rhs);
}
template<typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(
std::basic_ostream<Char, Traits>& os, wjr::basic_string_view<Char, Traits> sv) {
return (os << (std::basic_string_view<Char, Traits>)(sv));
}
using string_view = basic_string_view<char>;
#if defined(WJR_CHAR8_T)
using u8string_view = basic_string_view<char8_t>;
#endif // WJR_CHAR8_T
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;
using wstring_view = basic_string_view<wchar_t>;
template<
typename Char,
typename Traits = std::char_traits<Char>,
typename Alloc = std::allocator<Char>,
typename Data = string_sso_data<Char, std::max(size_t(1), size_t(16 / sizeof(Char))), Alloc>>
class basic_string {
public:
using traits_type = Traits;
using allocator_type = Alloc;
using value_type = Char;
using reference = value_type&;
using const_reference = const value_type&;
using size_type = size_t;
using difference_type = size_t;
using pointer = value_type*;
using const_pointer = const value_type*;
using iterator = pointer;
using const_iterator = const_pointer;
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;
constexpr static size_type npos = static_cast<size_type>(-1);
using vector_type = wjr::vector<Char, Alloc, Data>;
using data_type = typename vector_type::data_type;
using view_type = wjr::basic_string_view<Char, Traits>;
using _Alty = typename vector_type::_Alty;
using _Alty_traits = typename vector_type::_Alty_traits;
static_assert(std::is_same_v<iterator, typename vector_type::iterator>, "");
static_assert(std::is_same_v<const_iterator, typename vector_type::const_iterator>, "");
static_assert(std::is_same_v<const_iterator, typename view_type::const_iterator>, "");
static_assert(std::is_trivially_destructible_v<Char>, "");
private:
template<typename T>
struct _Is_string_view_like : std::conjunction<
std::is_convertible<const T&, view_type>
> {};
template<typename T>
constexpr static bool _Is_string_view_like_v = _Is_string_view_like<T>::value;
template<typename T>
struct _Is_noptr_string_view_like : std::conjunction<
_Is_string_view_like<T>,
std::negation<std::is_convertible<const T&, const Char*>>
> {};
template<typename T>
constexpr static bool _Is_noptr_string_view_like_v = _Is_noptr_string_view_like<T>::value;
constexpr static size_type traits_length(const value_type* s) {
return traits_type::length(s);
}
public:
WJR_CONSTEXPR20 basic_string()
noexcept(std::is_nothrow_default_constructible_v<vector_type>)
: m_core() {
finalize();
}
WJR_CONSTEXPR20 explicit basic_string(const allocator_type& al) noexcept
: m_core(al) {
finalize();
}
WJR_CONSTEXPR20 basic_string(const basic_string& other)
: m_core(other.m_core) {
finalize();
}
WJR_CONSTEXPR20 basic_string(const basic_string& other, const allocator_type& al)
: m_core(other.m_core, al) {
finalize();
}
WJR_CONSTEXPR20 basic_string(basic_string&& other)
: m_core(std::move(other.m_core)) {
finalize();
other.finalize();
}
WJR_CONSTEXPR20 basic_string(basic_string&& other, const allocator_type& al)
: m_core(std::move(other.m_core), al) {
finalize();
other.finalize();
}
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 basic_string(iter first, iter last, const allocator_type& al = allocator_type())
: m_core(first, last, al) {
finalize();
}
WJR_CONSTEXPR20 basic_string(const basic_string& other,
const size_type off, const allocator_type& al = allocator_type())
: basic_string(eview(other, off), al) {}
WJR_CONSTEXPR20 basic_string(const basic_string& other,
const size_type off, const size_type n, const allocator_type& al = allocator_type())
: basic_string(eview(other, off, n), al) {}
WJR_CONSTEXPR20 basic_string(const value_type* s, const size_type n, const allocator_type& al = allocator_type())
: basic_string(view(s, n), al) {}
WJR_CONSTEXPR20 basic_string(const value_type* s, const allocator_type& al = allocator_type())
: basic_string(view(s), al) {}
WJR_CONSTEXPR20 basic_string(std::initializer_list<value_type> il, const allocator_type& al = allocator_type())
: basic_string(view(il), al) {}
private:
WJR_CONSTEXPR20 basic_string(view_type sv, const allocator_type& al, disable_tag)
: basic_string(sv.data(), sv.data() + sv.size(), al) {}
WJR_CONSTEXPR20 basic_string(
view_type sv,
const size_type off, const size_type n,
const allocator_type& al, disable_tag)
: basic_string(eview(sv, off, n), al) {}
public:
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 explicit basic_string(const StringView& t, const allocator_type& al = allocator_type())
:basic_string(view_type(t), al, disable_tag{}) {}
template<typename StringView, std::enable_if_t<_Is_string_view_like_v<StringView>, int> = 0>
explicit basic_string(const StringView& t,
const size_type off, const size_type n,
const allocator_type& al = allocator_type())
: basic_string(view_type(t), off, n, al, disable_tag{}) {}
WJR_CONSTEXPR20 explicit basic_string(const size_type n,
const value_type c = value_type(), const allocator_type& al = allocator_type())
: m_core(n, c, al) {
finalize();
}
WJR_CONSTEXPR20 operator std::basic_string_view<Char, traits_type>() const noexcept {
return { data(), size() };
}
WJR_CONSTEXPR20 ~basic_string() noexcept = default;
WJR_CONSTEXPR20 basic_string& operator=(const basic_string& other) {
if (WJR_LIKELY(this != std::addressof(other))) {
m_core = other.m_core;
finalize();
}
return *this;
}
WJR_CONSTEXPR20 basic_string& operator=(basic_string&& other) noexcept {
if (WJR_LIKELY(this != std::addressof(other))) {
m_core = std::move(other.m_core);
finalize();
other.finalize();
}
return *this;
}
WJR_CONSTEXPR20 basic_string& operator=(const value_type* str) {
return assign(str);
}
WJR_CONSTEXPR20 basic_string& operator=(std::initializer_list<value_type> il) {
return assign(il);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& operator=(const StringView& t) {
return assign(t);
}
WJR_CONSTEXPR20 basic_string& operator=(const value_type c) {
resize(1);
*begin() = c;
return *this;
}
WJR_INLINE_CONSTEXPR20 iterator begin() { return m_core.begin(); }
WJR_INLINE_CONSTEXPR20 const_iterator begin()const { return m_core.begin(); }
WJR_INLINE_CONSTEXPR20 const_iterator cbegin()const { return m_core.cbegin(); }
WJR_INLINE_CONSTEXPR20 iterator end() { return m_core.end(); }
WJR_INLINE_CONSTEXPR20 const_iterator end()const { return m_core.end(); }
WJR_INLINE_CONSTEXPR20 const_iterator cend()const { return m_core.end(); }
WJR_INLINE_CONSTEXPR20 reverse_iterator rbegin() { return reverse_iterator(end()); }
WJR_INLINE_CONSTEXPR20 const_reverse_iterator rbegin()const { return const_reverse_iterator(end()); }
WJR_INLINE_CONSTEXPR20 const_reverse_iterator crbegin()const { return const_reverse_iterator(cend()); }
WJR_INLINE_CONSTEXPR20 reverse_iterator rend() { return reverse_iterator(begin()); }
WJR_INLINE_CONSTEXPR20 const_reverse_iterator rend()const { return const_reverse_iterator(begin()); }
WJR_INLINE_CONSTEXPR20 const_reverse_iterator crend()const { return const_reverse_iterator(begin()); }
WJR_INLINE_CONSTEXPR20 reference front() { return m_core.front(); }
WJR_INLINE_CONSTEXPR20 const_reference front()const { return m_core.front(); }
WJR_INLINE_CONSTEXPR20 reference back() {
return m_core.back();
}
WJR_INLINE_CONSTEXPR20 const_reference back()const {
return m_core.back();
}
WJR_CONSTEXPR20 void resize(const size_type n) {
m_core.resize(n);
finalize();
}
WJR_CONSTEXPR20 void resize(const size_type n, const value_type c) {
m_core.resize(n, c);
finalize();
}
WJR_CONSTEXPR20 void reserve(const size_type c) {
m_core.reserve(c);
finalize();
}
WJR_CONSTEXPR20 void shrink_to_fit() {
m_core.shrink_to_fit();
finalize();
}
WJR_CONSTEXPR20 void clear() {
resize(0);
}
WJR_ATTRIBUTE(CONST, INLINE_CONSTEXPR20) size_type size() const {
return m_core.size();
}
WJR_INLINE_CONSTEXPR20 size_type length() const {
return size();
}
WJR_INLINE_CONSTEXPR20 size_type max_size() const { return std::numeric_limits<size_type>::max(); }
WJR_ATTRIBUTE(CONST, INLINE_CONSTEXPR20) size_type capacity() const {
return m_core.capacity();
}
WJR_INLINE_CONSTEXPR20 reference operator[](const size_type index) {
return m_core[index];
}
WJR_INLINE_CONSTEXPR20 const_reference operator[](const size_type index)const {
return m_core[index];
}
WJR_INLINE_CONSTEXPR20 reference at(const size_type index) {
return m_core.at(index);
}
WJR_INLINE_CONSTEXPR20 const_reference at(const size_type index)const {
return m_core.at(index);
}
WJR_CONSTEXPR20 basic_string& operator+=(const basic_string& other) { return append(other); }
WJR_CONSTEXPR20 basic_string& operator+=(const value_type* s) { return append(s); }
WJR_CONSTEXPR20 basic_string& operator+=(std::initializer_list<value_type> il) {
append(il);
return *this;
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& operator+=(const StringView& t) {
return append(t);
}
WJR_CONSTEXPR20 basic_string& operator+=(const value_type c) { return append(c); }
WJR_CONSTEXPR20 basic_string& append(const basic_string& other) {
return append(view(other));
}
WJR_CONSTEXPR20 basic_string& append(
const basic_string& other, const size_type off, const size_type n = npos) {
return append(eview(other, off, n));
}
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 basic_string& append(iter first, iter last) {
m_core.append(first, last);
finalize();
return *this;
}
WJR_CONSTEXPR20 basic_string& append(const value_type* s, size_type n) {
return append(view(s, n));
}
WJR_CONSTEXPR20 basic_string& append(const value_type* s) {
return append(view(s));
}
WJR_CONSTEXPR20 basic_string& append(std::initializer_list<value_type> il) {
return append(view(il));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& append(const StringView& t) {
const auto sv = view(t);
return append(sv.data(), sv.data() + sv.size());
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& append(const StringView& t,
const size_type off, const size_type n = npos) {
return apend(eview(t, off, n));
}
WJR_CONSTEXPR20 basic_string& append(const size_type n, const value_type c) {
m_core.append(n, c);
finalize();
return *this;
}
WJR_CONSTEXPR20 basic_string& append(const value_type c) {
push_back(c);
return *this;
}
WJR_CONSTEXPR20 void push_back(const value_type c) {
m_core.push_back(c);
finalize();
}
WJR_CONSTEXPR20 void push_front(const value_type c) {
prepend(c);
}
WJR_CONSTEXPR20 void pop_back() {
m_core.pop_back();
finalize();
}
WJR_CONSTEXPR20 basic_string& assign(const basic_string& other) {
return (*this) = other;
}
WJR_CONSTEXPR20 basic_string& assign(basic_string&& other) noexcept {
return (*this) = std::move(other);
}
WJR_CONSTEXPR20 basic_string& assign(const basic_string& other, const size_type off, const size_type n = npos) {
return assign(eview(other, off, n));
}
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 basic_string& assign(iter first, iter last) {
m_core.assign(first, last);
finalize();
return *this;
}
WJR_CONSTEXPR20 basic_string& assign(const value_type* s, const size_type n) {
return assign(view(s, n));
}
WJR_CONSTEXPR20 basic_string& assign(const value_type* s) {
return assign(view(s));
}
WJR_CONSTEXPR20 basic_string& assign(std::initializer_list<value_type> il) {
return assign(view(il));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& assign(const StringView& t) {
const auto sv = view(t);
return assign(sv.data(), sv.data() + sv.size());
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& assign(const StringView& t, const size_type off, const size_type n = npos) {
return assign(eview(t, off, n));
}
WJR_CONSTEXPR20 basic_string& assign(const size_type n, const value_type c) {
m_core.assign(n, c);
finalize();
return *this;
}
WJR_CONSTEXPR20 basic_string& insert(const size_type off, const basic_string& str) {
return insert(off, view(str));
}
WJR_CONSTEXPR20 basic_string& insert(const size_type off1,
const basic_string& str, const size_type off2, const size_type n = npos) {
return insert(off1, eview(str, off2, n));
}
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 iterator insert(const_iterator _Where, iter first, iter last) {
auto __old_pos = static_cast<size_type>(_Where - cbegin());
m_core.insert(_Where, first, last);
finalize();
return begin() + __old_pos;
}
WJR_CONSTEXPR20 basic_string& insert(const size_type off, const value_type* s, const size_type n) {
return insert(off, view(s, n));
}
WJR_CONSTEXPR20 basic_string& insert(const size_type off, const value_type* s) {
return insert(off, view(s));
}
WJR_CONSTEXPR20 iterator insert(const_iterator _Where, const size_type n, const value_type c) {
auto __old_pos = static_cast<size_type>(_Where - cbegin());
m_core.insert(_Where, n, c);
finalize();
return begin() + __old_pos;
}
WJR_CONSTEXPR20 basic_string& insert(const size_type off, const size_type n, const value_type c) {
(void)(eview(*this, off));
insert(cbegin() + off, n, c);
return *this;
}
WJR_CONSTEXPR20 iterator insert(const_iterator _Where, const value_type c) {
auto __old_pos = static_cast<size_type>(_Where - cbegin());
m_core.insert(_Where, c);
finalize();
return begin() + __old_pos;
}
WJR_CONSTEXPR20 iterator insert(const_iterator _Where, std::initializer_list<value_type> il) {
return insert(_Where, il.begin(), il.end());
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& insert(const size_type off, const StringView& t) {
(void)(eview(*this, off));
const auto sv = view(t);
insert(cbegin() + off, sv.data(), sv.data() + sv.size());
return *this;
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& insert(const size_type off1,
const StringView& t, const size_type off2, const size_type n = npos) {
return insert(off1, view(t, off2, n));
}
WJR_CONSTEXPR20 iterator erase(const_iterator first, const_iterator last) {
auto __old_pos = first - cbegin();
m_core.erase(first, last);
finalize();
return begin() + __old_pos;
}
WJR_CONSTEXPR20 basic_string& erase(const size_type off = 0, size_type n = npos) {
const auto sv = eview(*this, off, n);
erase(sv.begin(), sv.end());
return *this;
}
WJR_CONSTEXPR20 iterator erase(const_iterator _Where) {
auto __old_pos = _Where - cbegin();
m_core.erase(_Where);
finalize();
return begin() + __old_pos;
}
WJR_CONSTEXPR20 void swap(basic_string& other)noexcept {
m_core.swap(other.m_core);
finalize();
other.finalize();
}
WJR_INLINE_CONSTEXPR20 const value_type* c_str() const { return m_core.data(); }
WJR_INLINE_CONSTEXPR20 value_type* data() { return m_core.data(); }
WJR_INLINE_CONSTEXPR20 const value_type* data() const { return m_core.data(); }
WJR_INLINE_CONSTEXPR20 bool empty()const { return m_core.empty(); }
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(const basic_string& other) const noexcept {
return compare(view(other));
}
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(
const size_type off1, const size_type n1, const basic_string& other) const {
return compare(off1, n1, view(other));
}
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(
const size_type off1, const size_type n1, const basic_string& other,
const size_type off2, const size_type n2 = npos) const {
return compare(off1, n1, view(other, off2, n2));
}
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(const value_type* s) const {
return compare(view(s));
}
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(
const size_type off1, const size_type n1, const value_type* s) const {
return compare(off1, n1, view(s));
}
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(
const size_type off1, const size_type n1, const value_type* s,
const size_type n2) const {
return compare(off1, n1, view(s, n2));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(const StringView& t) const noexcept {
return view().compare(view(t));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(
const size_type off1, const size_type n1, const StringView& t) const {
return view(*this, off1, n1).compare(view(t));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE_CONSTEXPR20) int compare(
const size_type off1, const size_type n1, const StringView& t,
const size_type off2, const size_type n2 = npos) const {
return compare(off1, n1, view(t, off2, n2));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const StringView& t) const noexcept {
return view().starts_with(view(t));
}
WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const value_type ch) const noexcept {
return view().starts_with(ch);
}
WJR_NODISCARD WJR_CONSTEXPR20 bool starts_with(const value_type* const ptr) const {
return view().starts_with(ptr);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const StringView& t) const noexcept {
return view().ends_with(view(t));
}
WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const value_type ch) const noexcept {
return view().ends_with(ch);
}
WJR_NODISCARD WJR_CONSTEXPR20 bool ends_with(const value_type* const ptr) const {
return view().ends_with(ptr);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const StringView& t) const noexcept {
return view().contains(view(t));
}
WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const value_type ch) const noexcept {
return view().contains(ch);
}
WJR_NODISCARD WJR_CONSTEXPR20 bool contains(const value_type* const ptr) const {
return view().contains(ptr);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const StringView& t, const size_type off = 0) const noexcept {
return view().find(view(t), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const value_type ch, const size_type off = 0) const noexcept {
return view().find(ch, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const value_type* const ptr,
const size_type off, const size_type n) const {
return view().find(ptr, off, n);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find(const value_type* const ptr, const size_type off = 0) const {
return view().find(ptr, off);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const StringView& t, const size_type off = npos) const noexcept {
return view().rfind(view(t), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const value_type ch, const size_type off = npos) const noexcept {
return view().rfind(ch, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const value_type* const ptr,
const size_type off, const size_type n) const {
return view().rfind(ptr, off, n);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type rfind(const value_type* const ptr, const size_type off = npos) const {
return view().rfind(ptr, off);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(
const StringView& t, const size_type off = 0) const noexcept {
return view().find_first_of(view(t), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(
const value_type ch, const size_type off = 0) const noexcept {
return view().find_first_of(ch, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(const value_type* const ptr,
const size_type off, const size_type n) const {
return view().find_first_of(ptr, off, n);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_of(
const value_type* const ptr, const size_type off = 0) const {
return view().find_first_of(ptr, off);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(
const StringView& t, const size_type off = npos) const noexcept {
return view().find_last_of(view(t), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(
const value_type ch, const size_type off = npos) const noexcept {
return view().find_last_of(ch, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(const value_type* const ptr,
const size_type off, const size_type n) const {
return view().find_last_of(ptr, off, n);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_of(
const value_type* const ptr, const size_type off = npos) const {
return view().find_last_of(ptr, off);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(
const StringView& t, const size_type off = 0) const noexcept {
return view().find_first_not_of(view(t), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(
const value_type ch, const size_type off = 0) const noexcept {
return view().find_first_not_of(ch, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(const value_type* const ptr,
const size_type off, const size_type n) const {
return view().find_first_not_of(ptr, off, n);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_first_not_of(
const value_type* const ptr, const size_type off = 0) const {
return view().find_first_not_of(ptr, off);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(
const StringView& t, const size_type off = npos) const noexcept {
return view().find_last_not_of(view(t), off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(
const value_type ch, const size_type off = npos) const noexcept {
return view().find_last_not_of(ch, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(const value_type* const ptr,
const size_type off, const size_type n) const {
return view().find_last_not_of(ptr, off, n);
}
WJR_NODISCARD WJR_CONSTEXPR20 size_type find_last_not_of(
const value_type* const ptr, const size_type off = npos) const {
return view().find_last_not_of(ptr, off);
}
WJR_NODISCARD WJR_CONSTEXPR20 basic_string substr(
const size_type off = 0, const size_type n = npos) const {
return basic_string(eview(*this, off, n));
}
WJR_CONSTEXPR20 basic_string& replace(size_type off, size_type n,
const basic_string& str) {
return replace(off, n, view(str));
}
WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
const basic_string& str) {
return replace(first, last, view(str));
}
WJR_CONSTEXPR20 basic_string& replace(size_type off1, size_type n1,
const basic_string& str,
size_type off2, size_type n2 = npos) {
return replace(off1, n1, view(str, off2, n2));
}
template<typename iter>
WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
iter first2, iter last2) {
m_core.replace(first, last, first2, last2);
finalize();
return *this;
}
WJR_CONSTEXPR20 basic_string& replace(size_type off, size_type n1,
const value_type* ptr, size_type n2) {
const auto sv = view(*this, off, n1);
return replace(sv.begin(), sv.end(), view(ptr, n2));
}
WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
const value_type* ptr, size_type n2) {
return replace(first, last, view(ptr, n2));
}
WJR_CONSTEXPR20 basic_string& replace(size_type off, size_type n1,
const value_type* ptr) {
return replace(off, n1, view(ptr));
}
WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
const value_type* ptr) {
return replace(first, last, view(ptr));
}
WJR_CONSTEXPR20 basic_string& replace(size_type off, size_type n1,
size_type n2, value_type ch) {
const auto sv = view(*this, off, n1);
return replace(sv.begin(), sv.end(), n2, ch);
}
WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
size_type n2, value_type ch) {
m_core.replace(first, last, n2, ch);
finalize();
return *this;
}
WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
std::initializer_list<value_type> il) {
return replace(first, last, view(il));
}
template <typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& replace(size_type off, size_type n,
const StringView& t) {
const auto sv = view(*this, off, n);
return replace(sv.begin(), sv.end(), view(t));
}
template <typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& replace(const_iterator first, const_iterator last,
const StringView& t) {
const auto sv = view(t);
return replace(first, last, sv.begin(), sv.end());
}
template <typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& replace(size_type off1, size_type n1, const StringView& t,
size_type off2, size_type n2 = npos) {
return replace(off1, n1, view(t, off2, n2));
}
WJR_CONSTEXPR20 allocator_type& get_allocator() {
return m_core.get_allocator();
}
WJR_CONSTEXPR20 const allocator_type& get_allocator()const {
return m_core.get_allocator();
}
/*------Internal management function------*/
WJR_CONSTEXPR20 static void copyConstruct(_Alty& al, const data_type& _Src, data_type& _Dest)
noexcept(noexcept(vector_type::copyConstruct(al, _Src, _Dest))) {
vector_type::copyConstruct(al, _Src, _Dest);
}
WJR_CONSTEXPR20 static void moveConstruct(_Alty& al, data_type&& _Src, data_type& _Dest)
noexcept(noexcept(vector_type::moveConstruct(al, std::move(_Src), _Dest))) {
vector_type::moveConstruct(al, std::move(_Src), _Dest);
}
WJR_CONSTEXPR20 static void Swap(_Alty& al, data_type& _Left, data_type& other)
noexcept(noexcept(vector_type::Swap(al, _Left, other))) {
vector_type::Swap(al, _Left, other);
}
WJR_CONSTEXPR20 static void Deallocate(_Alty& al, data_type& _Data)
noexcept(noexcept(data_type::Deallocate(al, _Data))) {
vector_type::Deallocate(al, _Data);
}
WJR_CONSTEXPR20 static void Tidy(_Alty& al, data_type& _Data) {
vector_type::Tidy(al, _Data);
}
WJR_INLINE_CONSTEXPR20 static size_type getGrowthCapacity(
const size_type _Oldcapacity, const size_type _Newsize) noexcept {
return vector_type::getGrowthCapacity(_Oldcapacity, _Newsize);
}
WJR_CONSTEXPR20 static void shrinkToFit(_Alty& al, data_type& _Data) {
vector_type::shrinkToFit(al, _Data);
}
WJR_INLINE_CONSTEXPR20 _Alty& getAllocator() noexcept {
return m_core.getAllocator();
}
WJR_INLINE_CONSTEXPR20 const _Alty& getAllocator() const noexcept {
return m_core.getAllocator();
}
WJR_INLINE_CONSTEXPR20 data_type& getData() noexcept {
return m_core.getData();
}
WJR_INLINE_CONSTEXPR20 const data_type& getData() const noexcept {
return m_core.getData();
}
WJR_INLINE_CONSTEXPR20 pointer lastPtr() noexcept {
return getData().lastPtr();
}
WJR_INLINE_CONSTEXPR20 const_pointer lastPtr() const noexcept {
return getData().lastPtr();
}
WJR_INLINE_CONSTEXPR20 pointer endPtr() noexcept {
return getData().endPtr();
}
WJR_INLINE_CONSTEXPR20 const_pointer endPtr() const noexcept {
return getData().endPtr();
}
WJR_INLINE_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
getData().set_size(_Size);
}
WJR_INLINE_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
getData().inc_size(_Size);
}
WJR_INLINE_CONSTEXPR20 void finalize() {
wjr::construct_at(getAllocator(), lastPtr());
}
/*------External extension function------*/
WJR_NODISCARD WJR_CONSTEXPR20 basic_string nesubstr(
const size_type off = 0, const size_type n = npos) const {
return basic_string(view(*this, off, n));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const basic_string& str) noexcept {
return view_type::view(str);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const basic_string& str,
const size_type off, const size_type n = npos) noexcept {
return view_type::view(str, off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const value_type* s, const size_type n) noexcept {
return view_type::view(s, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const value_type* s) noexcept {
return view_type::view(s);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(std::initializer_list<value_type> il) noexcept {
return view_type::view(il);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const StringView& t) noexcept {
return view_type::view(t);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type view(const StringView& t,
const size_type off, const size_type n = npos) noexcept {
return view_type::view(t, off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 view_type view() const noexcept {
return view(*this);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 static view_type eview(const StringView& t,
const size_type off, const size_type n = npos) WJR_NOEXCEPT {
return view_type::eview(t, off, n);
}
WJR_CONSTEXPR20 basic_string& prepend(const basic_string& other) {
return prepend(view(other));
}
WJR_CONSTEXPR20 basic_string& prepend(const basic_string& other, const size_type off, const size_type n = npos) {
return prepend(view(other, off, n));
}
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 basic_string& prepend(iter first, iter last) {
insert(begin(), first, last);
return *this;
}
WJR_CONSTEXPR20 basic_string& prepend(const value_type* s, size_type n) {
return prepend(view(s, n));
}
WJR_CONSTEXPR20 basic_string& prepend(const value_type* s) {
return prepend(view(s));
}
WJR_CONSTEXPR20 basic_string& prepend(std::initializer_list<value_type> il) {
return prepend(view(il));
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& prepend(const StringView& t) {
const auto sv = view_type(t);
return prepend(sv.data(), sv.data() + sv.size());
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_CONSTEXPR20 basic_string& prepend(const StringView& t,
const size_type off, const size_type n = npos) {
return prepend(view(t, off, n));
}
WJR_CONSTEXPR20 basic_string& prepend(const size_type n, const value_type c) {
insert(begin(), n, c);
return *this;
}
WJR_CONSTEXPR20 basic_string& prepend(const value_type c) {
m_core.emplace(m_core.begin(), c);
finalize();
return *this;
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const StringView& t) const noexcept {
return view().equal(t);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off, const size_type n, const StringView& t)
const WJR_NOEXCEPT {
return view().equal(off, n, t);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off1, const size_type n1, const StringView& t,
const size_type off2, const size_type n2) const WJR_NOEXCEPT {
return view().equal(off1, n1, t, off2, n2);
}
WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const Char* const ptr) const {
return view().equal(ptr);
}
WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off, const size_type n, const Char* const ptr) const {
return view().equal(off, n, ptr);
}
WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off1, const size_type n1,
const Char* const ptr, const size_type n2) const {
return view().equal(off1, n1, ptr, n2);
}
WJR_CONSTEXPR20 basic_string(const size_type _Count, default_construct_tag, const allocator_type& al = allocator_type())
: m_core(_Count, default_construct_tag(), al) {
finalize();
}
WJR_CONSTEXPR20 basic_string(const size_type _Count, value_construct_tag, const allocator_type& al = allocator_type())
: m_core(_Count, value_construct_tag(), al) {
finalize();
}
WJR_CONSTEXPR20 void resize(const size_type _Newsize, default_construct_tag) {
m_core.resize(_Newsize, default_construct_tag());
finalize();
}
WJR_CONSTEXPR20 void resize(const size_type _Newsize, value_construct_tag) {
m_core.resize(_Newsize, value_construct_tag());
finalize();
}
WJR_CONSTEXPR20 void push_back(default_construct_tag) {
m_core.push_back(default_construct_tag());
finalize();
}
WJR_CONSTEXPR20 void push_back(value_construct_tag) {
m_core.push_back(value_construct_tag());
finalize();
}
WJR_CONSTEXPR20 basic_string& append(const size_type n, default_construct_tag) {
m_core.append(n, default_construct_tag());
finalize();
return *this;
}
WJR_CONSTEXPR20 basic_string& append(const size_type n, value_construct_tag) {
m_core.append(n, value_construct_tag());
finalize();
return *this;
}
WJR_CONSTEXPR20 basic_string& chop(const size_type n) noexcept {
m_core.chop(n);
finalize();
return *this;
}
WJR_CONSTEXPR20 basic_string& truncate(const size_type n) noexcept {
m_core.truncate(n);
finalize();
return *this;
}
WJR_INLINE_CONSTEXPR20 void assume_total_capacity(const size_type n) const noexcept {
m_core.assume_total_capacity(n);
}
WJR_INLINE_CONSTEXPR20 void assume_rest_capacity(const size_type n) const noexcept {
m_core.assume_rest_capacity(n);
}
private:
vector_type m_core;
};
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs
) {
basic_string<Char, Traits, Alloc, Data> ans;
ans.reserve(lhs.size() + rhs.size());
ans.assume_rest_capacity(lhs.size());
ans.append(lhs);
ans.assume_rest_capacity(rhs.size());
ans.append(rhs);
return ans;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const Char* const rhs
) {
basic_string<Char, Traits, Alloc, Data> ans;
const auto rhs_size = Traits::length(rhs);
ans.reserve(lhs.size() + rhs_size);
ans.assume_rest_capacity(lhs.size());
ans.append(lhs);
ans.assume_rest_capacity(rhs_size);
ans.append(rhs, rhs_size);
return ans;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const Char rhs
) {
basic_string<Char, Traits, Alloc, Data> ans;
ans.reserve(lhs.size() + 1);
ans.assume_rest_capacity(lhs.size());
ans.append(lhs);
ans.assume_rest_capacity(1);
ans.append(rhs);
return ans;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
const Char* const lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs
) {
basic_string<Char, Traits, Alloc, Data> ans;
const auto lhs_size = Traits::length(lhs);
ans.reserve(lhs_size + rhs.size());
ans.assume_rest_capacity(lhs_size);
ans.append(lhs, lhs_size);
ans.assume_rest_capacity(rhs.size());
ans.append(rhs);
return ans;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
const Char lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs
) {
basic_string<Char, Traits, Alloc, Data> ans;
ans.reserve(rhs.size() + 1);
ans.assume_rest_capacity(1);
ans.append(lhs);
ans.assume_rest_capacity(rhs.size());
ans.append(rhs);
return ans;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
basic_string<Char, Traits, Alloc, Data>&& lhs,
basic_string<Char, Traits, Alloc, Data>&& rhs
) {
return std::move(lhs.append(rhs));
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
basic_string<Char, Traits, Alloc, Data>&& lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs
) {
return std::move(lhs.append(rhs));
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
const basic_string<Char, Traits, Alloc, Data>& lhs,
basic_string<Char, Traits, Alloc, Data>&& rhs
) {
return std::move(rhs.prepend(lhs));
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
basic_string<Char, Traits, Alloc, Data>&& lhs,
const Char* const rhs
) {
return std::move(lhs.append(rhs));
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
const Char* const lhs,
basic_string<Char, Traits, Alloc, Data>&& rhs
) {
return std::move(rhs.prepend(lhs));
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
basic_string<Char, Traits, Alloc, Data>&& lhs,
const Char rhs
) {
return std::move(lhs.append(rhs));
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 basic_string<Char, Traits, Alloc, Data> operator+(
const Char lhs,
basic_string<Char, Traits, Alloc, Data>&& rhs
) {
return std::move(rhs.prepend(lhs));
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return lhs.equal(rhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const Char* rhs) noexcept {
return lhs.equal(rhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator==(
const Char* lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return rhs == lhs;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return !(lhs == rhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const Char* rhs) noexcept {
return !(lhs == rhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator!=(
const Char* lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return !(lhs == rhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return lhs.compare(rhs) < 0;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const Char* rhs) noexcept {
return lhs.compare(rhs) < 0;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<(
const Char* lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return rhs.compare(lhs) > 0;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return rhs < lhs;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const Char* rhs) noexcept {
return rhs < lhs;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>(
const Char* lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return rhs < lhs;
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return !(rhs < lhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const Char* rhs) noexcept {
return !(rhs < lhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator<=(
const Char* lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return !(rhs < lhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return !(lhs < rhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
const basic_string<Char, Traits, Alloc, Data>& lhs,
const Char* rhs) noexcept {
return !(lhs < rhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
WJR_NODISCARD WJR_CONSTEXPR20 bool operator>=(
const Char* lhs,
const basic_string<Char, Traits, Alloc, Data>& rhs) noexcept {
return !(lhs < rhs);
}
template<typename Char, typename Traits, typename Alloc, typename Data>
std::basic_ostream<Char, Traits>& operator<<(
std::basic_ostream<Char, Traits>& os,
const basic_string<Char, Traits, Alloc, Data>& str) {
return os << str.view();
}
template<typename Char, typename Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& operator>>(
std::basic_istream<Char, Traits>& is,
basic_string<Char, Traits, Alloc, Data>& str) {
using IS = std::basic_istream<Char, Traits>;
using size_type = typename basic_string<Char, Traits, Alloc, Data>::size_type;
typename IS::iostate state = IS::goodbit;
const typename IS::sentry sentry(is);
bool changed = false;
if (sentry) { // state okay, extract characters
str.clear();
size_type _Size;
if (is.width() && static_cast<size_type>(is.width()) < str.max_size()) {
_Size = static_cast<size_type>(is.width());
}
else {
_Size = str.max_size();
}
typename Traits::int_type _Meta = is.rdbuf()->sgetc();
for (; 0 < _Size; --_Size, _Meta = is.rdbuf()->snextc()) {
if (Traits::eq_int_type(Traits::eof(), _Meta)) {
state |= IS::eofbit;
break;
}
if (isspace(Traits::to_char_type(_Meta))) {
break;
}
str.push_back(Traits::to_char_type(_Meta));
changed = true;
}
}
is.width(0);
if (!changed) {
state |= IS::failbit;
}
is.setstate(state);
return is;
}
using string = basic_string<char>;
#if defined(WJR_CHAR8_T)
using u8string = basic_string<char8_t>;
#endif // WJR_CHAR8_T
using u16string = basic_string<char16_t>;
using u32string = basic_string<char32_t>;
using wstring = basic_string<wchar_t>;
template<typename Char, size_t N, typename Traits = std::char_traits<Char>, typename Alloc = std::allocator<Char>>
using static_basic_string = basic_string<Char, Traits, Alloc, string_static_data<Char, N, Alloc>>;
template<size_t N>
using static_string = static_basic_string<char, N>;
#if defined(WJR_CHAR8_T)
template<size_t N>
using static_u8string = static_basic_string<char8_t, N>;
#endif // WJR_CHAR8_T
template<size_t N>
using static_u16string = static_basic_string<char16_t, N>;
template<size_t N>
using static_u32string = static_basic_string<char32_t, N>;
template<size_t N>
using static_wstring = static_basic_string<wchar_t, N>;
namespace literals {
WJR_NODISCARD WJR_INLINE_CONSTEXPR string_view operator""_sv(const char* str, size_t len) noexcept {
return string_view(str, len);
}
#if defined(WJR_CHAR8_T)
WJR_NODISCARD WJR_INLINE_CONSTEXPR u8string_view operator""_sv(const char8_t* str, size_t len) noexcept {
return u8string_view(str, len);
}
#endif // WJR_CHAR8_T
WJR_NODISCARD WJR_INLINE_CONSTEXPR u16string_view operator""_sv(const char16_t* str, size_t len) noexcept {
return u16string_view(str, len);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR u32string_view operator""_sv(const char32_t* str, size_t len) noexcept {
return u32string_view(str, len);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR wstring_view operator""_sv(const wchar_t* str, size_t len) noexcept {
return wstring_view(str, len);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 string operator""_s(const char* str, size_t len) noexcept {
return string(str, len);
}
#if defined(WJR_CHAR8_T)
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 u8string operator""_s(const char8_t* str, size_t len) noexcept {
return u8string(str, len);
}
#endif // WJR_CHAR8_T
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 u16string operator""_s(const char16_t* str, size_t len) noexcept {
return u16string(str, len);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 u32string operator""_s(const char32_t* str, size_t len) noexcept {
return u32string(str, len);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 wstring_view operator""_s(const wchar_t* str, size_t len) noexcept {
return wstring(str, len);
}
}
/*---------string elated function---------*/
template <typename Char, typename Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& getline(
std::basic_istream<Char, Traits>&& is,
basic_string<Char, Traits, Alloc, Data>& str, const Char delim) {
using IS = std::basic_istream<Char, Traits>;
typename IS::iostate state = IS::goodbit;
const typename IS::sentry sentry(is, true);
bool changed = false;
if (sentry) {
str.clear();
const typename Traits::int_type _Metadelim = Traits::to_int_type(delim);
typename Traits::int_type _Meta = is.rdbuf()->sgetc();
for (;; _Meta = is.rdbuf()->snextc()) {
if (Traits::eq_int_type(Traits::eof(), _Meta)) { // end of file, quit
state |= IS::eofbit;
break;
}
else if (Traits::eq_int_type(_Meta, _Metadelim)) { // got a delimiter, discard it and quit
changed = true;
is.rdbuf()->sbumpc();
break;
}
else if (str.max_size() <= str.size()) { // string too large, quit
state |= IS::failbit;
break;
}
else { // got a character, add it to string
str.append(Traits::to_char_type(_Meta));
changed = true;
}
}
}
if (!changed) {
state |= IS::failbit;
}
is.setstate(state);
return static_cast<IS&>(is);
}
template <typename Char, typename  Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& getline(
std::basic_istream<Char, Traits>&& is, basic_string<Char, Traits, Alloc, Data>& str) {
return getline(std::move(is), str, is.widen('\n'));
}
template <typename Char, typename Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& getline(
std::basic_istream<Char, Traits>& is, basic_string<Char, Traits, Alloc, Data>& str, const Char delim) {
return getline(std::move(is), str, delim);
}
template <typename Char, typename Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& getline(
std::basic_istream<Char, Traits>& is, basic_string<Char, Traits, Alloc, Data>& str) {
return getline(std::move(is), str, is.widen('\n'));
}
/*---------------------------algorithm for string---------------------------*/
template<typename To, typename Char, typename From>
WJR_NODISCARD inline To str2i_helper(const Char* str, Char** str_end, From ans) {
constexpr auto _Max = std::numeric_limits<To>::max();
constexpr auto _Min = std::numeric_limits<To>::min();
if constexpr (_Max == std::numeric_limits<From>::max()
&& _Min == std::numeric_limits<From>::min()) {
return ans;
}
else {
if (str == *str_end) return static_cast<To>(0);
int& errno_ref = errno;
if (errno_ref == ERANGE) {
return ans == std::numeric_limits<From>::max() ? _Max : _Min;
}
if (ans > _Max) {
errno_ref = ERANGE;
return _Max;
}
if (ans < _Min) {
errno_ref = ERANGE;
return _Min;
}
return static_cast<To>(ans);
}
}
template<typename To, std::enable_if_t<std::is_integral_v<To>, int> = 0>
WJR_NODISCARD inline To str2i(const char* str, char** str_end, int base) {
constexpr auto _Nd = std::numeric_limits<To>::digits;
constexpr auto _Max = std::numeric_limits<To>::max();
constexpr auto _Min = std::numeric_limits<To>::min();
if constexpr (std::is_unsigned_v<To>) {
constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
if constexpr (_Nd <= _Nd_ul) {
return str2i_helper<To>(str, str_end, std::strtoul(str, str_end, base));
}
else {
return str2i_helper<To>(str, str_end, std::strtoull(str, str_end, base));
}
}
else {
constexpr auto _Nd_l = std::numeric_limits<long>::digits;
if constexpr (_Nd <= _Nd_l) {
return str2i_helper<To>(str, str_end, std::strtol(str, str_end, base));
}
else {
return str2i_helper<To>(str, str_end, std::strtoll(str, str_end, base));
}
}
}
template<typename To, std::enable_if_t<std::is_floating_point_v<To>, int> = 0>
WJR_NODISCARD inline To str2f(const char* str, char** str_end) {
if constexpr (std::is_same_v<To, float>) {
return std::strtof(str, str_end);
}
else if constexpr (std::is_same_v<To, double>) {
return std::strtod(str, str_end);
}
else {
return std::strtold(str, str_end);
}
}
template<typename To, std::enable_if_t<std::is_integral_v<To>, int> = 0>
WJR_NODISCARD inline To str2i(const wchar_t* str, wchar_t** str_end, int base) {
constexpr auto _Nd = std::numeric_limits<To>::digits;
constexpr auto _Max = std::numeric_limits<To>::max();
constexpr auto _Min = std::numeric_limits<To>::min();
if constexpr (std::is_unsigned_v<To>) {
constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
if constexpr (_Nd <= _Nd_ul) {
return str2i_helper<To>(str, str_end, std::wcstoul(str, str_end, base));
}
else {
return str2i_helper<To>(str, str_end, std::wcstoull(str, str_end, base));
}
}
else {
constexpr auto _Nd_l = std::numeric_limits<long>::digits;
if constexpr (_Nd <= _Nd_l) {
return str2i_helper<To>(str, str_end, std::wcstol(str, str_end, base));
}
else {
return str2i_helper<To>(str, str_end, std::wcstoll(str, str_end, base));
}
}
}
template<typename To, std::enable_if_t<std::is_floating_point_v<To>, int> = 0>
WJR_NODISCARD inline To str2f(const wchar_t* str, wchar_t** str_end) {
if constexpr (std::is_same_v<To, float>) {
return std::wcstof(str, str_end);
}
else if constexpr (std::is_same_v<To, double>) {
return std::wcstod(str, str_end);
}
else {
return std::wcstold(str, str_end);
}
}
template<typename Char>
WJR_NODISCARD inline int stoi(const basic_string<Char>& str, size_t* pos = nullptr, int base = 10) {
int& errno_ref = errno;
errno_ref = 0;
const Char* ptr = str.data();
Char* e = nullptr;
const int ans = str2i<int>(ptr, &e, base);
#if defined(_WJR_EXCEPTION)
if (ptr == e) {
throw std::invalid_argument("invalid stoi argument");
}
if (errno_ref == ERANGE) {
throw std::out_of_range("stoi argument out of range");
}
#endif // _WJR_EXCEPTION
if (pos) {
*pos = static_cast<size_t>(e - ptr);
}
return ans;
}
template<typename Char>
WJR_NODISCARD inline long stol(const basic_string<Char>& str, size_t* pos = nullptr, int base = 10) {
int& errno_ref = errno;
errno_ref = 0;
const Char* ptr = str.data();
Char* e = nullptr;
const long ans = str2i<long>(ptr, &e, base);
#if defined(_WJR_EXCEPTION)
if (ptr == e) {
throw std::invalid_argument("invalid stol argument");
}
if (errno_ref == ERANGE) {
throw std::out_of_range("stol argument out of range");
}
#endif // _WJR_EXCEPTION
if (pos) {
*pos = static_cast<size_t>(e - ptr);
}
return ans;
}
template<typename Char>
WJR_NODISCARD inline long long stoll(const basic_string<Char>& str, size_t* pos = nullptr, int base = 10) {
int& errno_ref = errno;
errno_ref = 0;
const Char* ptr = str.data();
Char* e = nullptr;
const long long ans = str2i<long long>(ptr, &e, base);
#if defined(_WJR_EXCEPTION)
if (ptr == e) {
throw std::invalid_argument("invalid stoll argument");
}
if (errno_ref == ERANGE) {
throw std::out_of_range("stoll argument out of range");
}
#endif // _WJR_EXCEPTION
if (pos) {
*pos = static_cast<size_t>(e - ptr);
}
return ans;
}
template<typename Char>
WJR_NODISCARD inline unsigned long stoul(const basic_string<Char>& str, size_t* pos = nullptr, int base = 10) {
int& errno_ref = errno;
errno_ref = 0;
const Char* ptr = str.data();
Char* e = nullptr;
const unsigned long ans = str2i<unsigned long>(ptr, &e, base);
#if defined(_WJR_EXCEPTION)
if (ptr == e) {
throw std::invalid_argument("invalid stoul argument");
}
if (errno_ref == ERANGE) {
throw std::out_of_range("stoul argument out of range");
}
#endif // _WJR_EXCEPTION
if (pos) {
*pos = static_cast<size_t>(e - ptr);
}
return ans;
}
template<typename Char>
WJR_NODISCARD inline unsigned long long stoull(const basic_string<Char>& str, size_t* pos = nullptr, int base = 10) {
int& errno_ref = errno;
errno_ref = 0;
const Char* ptr = str.data();
Char* e = nullptr;
const unsigned long long ans = str2i<unsigned long long>(ptr, &e, base);
#if defined(_WJR_EXCEPTION)
if (ptr == e) {
throw std::invalid_argument("invalid stoull argument");
}
if (errno_ref == ERANGE) {
throw std::out_of_range("stoull argument out of range");
}
#endif // _WJR_EXCEPTION
if (pos) {
*pos = static_cast<size_t>(e - ptr);
}
return ans;
}
template<typename Char>
WJR_NODISCARD inline float stof(const basic_string<Char>& str, size_t* pos = nullptr) {
int& errno_ref = errno;
errno_ref = 0;
const Char* ptr = str.data();
Char* e = nullptr;
const float ans = str2f<float>(ptr, &e);
#if defined(_WJR_EXCEPTION)
if (ptr == e) {
throw std::invalid_argument("invalid stof argument");
}
if (errno_ref == ERANGE) {
throw std::out_of_range("stof argument out of range");
}
#endif // _WJR_EXCEPTION
if (pos) {
*pos = static_cast<size_t>(e - ptr);
}
return ans;
}
template<typename Char>
WJR_NODISCARD inline double stod(const basic_string<Char>& str, size_t* pos = nullptr) {
int& errno_ref = errno;
errno_ref = 0;
const Char* ptr = str.data();
Char* e = nullptr;
const double ans = str2f<double>(ptr, &e);
#if defined(_WJR_EXCEPTION)
if (ptr == e) {
throw std::invalid_argument("invalid stod argument");
}
if (errno_ref == ERANGE) {
throw std::out_of_range("stod argument out of range");
}
#endif // _WJR_EXCEPTION
if (pos) {
*pos = static_cast<size_t>(e - ptr);
}
return ans;
}
template<typename Char>
WJR_NODISCARD inline long double stold(const basic_string<Char>& str, size_t* pos = nullptr) {
int& errno_ref = errno;
errno_ref = 0;
const Char* ptr = str.data();
Char* e = nullptr;
const long double ans = str2f<long double>(ptr, &e);
#if defined(_WJR_EXCEPTION)
if (ptr == e) {
throw std::invalid_argument("invalid stold argument");
}
if (errno_ref == ERANGE) {
throw std::out_of_range("stold argument out of range");
}
#endif // _WJR_EXCEPTION
if (pos) {
*pos = static_cast<size_t>(e - ptr);
}
return ans;
}
WJR_NODISCARD inline string to_string(int val) {
char buf[12];
const int len = snprintf(buf, sizeof(buf), "%d", val);
return string(buf, static_cast<size_t>(len));
}
WJR_NODISCARD inline string to_string(long val) {
char buf[24];
const int len = snprintf(buf, sizeof(buf), "%ld", val);
return string(buf, static_cast<size_t>(len));
}
WJR_NODISCARD inline string to_string(long long val) {
char buf[24];
const int len = snprintf(buf, sizeof(buf), "%lld", val);
return string(buf, static_cast<size_t>(len));
}
WJR_NODISCARD inline string to_string(unsigned val) {
char buf[12];
const int len = snprintf(buf, sizeof(buf), "%u", val);
return string(buf, static_cast<size_t>(len));
}
WJR_NODISCARD inline string to_string(unsigned long val) {
char buf[24];
const int len = snprintf(buf, sizeof(buf), "%lu", val);
return string(buf, static_cast<size_t>(len));
}
WJR_NODISCARD inline string to_string(unsigned long long val) {
char buf[24];
const int len = snprintf(buf, sizeof(buf), "%llu", val);
return string(buf, static_cast<size_t>(len));
}
WJR_NODISCARD inline string to_string(float val) {
char buf[24];
const int len = snprintf(buf, sizeof(buf), "%f", val);
return string(buf, static_cast<size_t>(len));
}
WJR_NODISCARD inline string to_string(double val) {
char buf[24];
const int len = snprintf(buf, sizeof(buf), "%f", val);
return string(buf, static_cast<size_t>(len));
}
WJR_NODISCARD inline string to_string(long double val) {
char buf[24];
const int len = snprintf(buf, sizeof(buf), "%Lf", val);
return string(buf, static_cast<size_t>(len));
}
_WJR_END
namespace std {
template<typename Char, typename Traits>
struct hash<wjr::basic_string_view<Char, Traits>> {
constexpr decltype(auto) operator()(const wjr::basic_string_view<Char, Traits>& str) const noexcept {
return std::hash<std::basic_string_view<Char, Traits>>()(str);
}
};
template<typename Char, typename Traits, typename Alloc, typename Data>
struct hash<wjr::basic_string<Char, Traits, Alloc, Data>> {
constexpr decltype(auto) operator()(const wjr::basic_string<Char, Traits, Alloc, Data>& str) const noexcept {
return std::hash<std::basic_string_view<Char, Traits>>()(str);
}
};
}
_WJR_BEGIN
template<typename Char, typename Traits = std::char_traits<Char>>
class basic_string_modifier :
public basic_string_view<Char, Traits>{
using _Mybase = basic_string_view<Char, Traits>;
template<typename T>
constexpr static bool _Is_noptr_string_view_like_v = _Mybase::template _Is_noptr_string_view_like_v<T>;
public:
using traits_type = typename _Mybase::traits_type;
using value_type = typename _Mybase::value_type;
using pointer = value_type*;
using const_pointer = typename _Mybase::const_pointer;
using reference = value_type&;
using const_reference = typename _Mybase::const_reference;
using const_iterator = typename _Mybase::const_iterator;
using iterator = pointer;
using const_reverse_iterator = typename _Mybase::const_reverse_iterator;
using reverse_iterator = std::reverse_iterator<iterator>;
using size_type = typename _Mybase::size_type;
using difference_type = typename _Mybase::difference_type;
static constexpr size_type npos = _Mybase::npos;
using _Mybase::_Mybase;
using _Mybase::operator=;
using _Mybase::data;
using _Mybase::begin;
using _Mybase::end;
using _Mybase::rbegin;
using _Mybase::rend;
using _Mybase::front;
WJR_CONSTEXPR basic_string_modifier(const _Mybase& other)
: _Mybase(other) {}
WJR_INLINE_CONSTEXPR void swap(basic_string_modifier& other) noexcept {
return _Mybase::swap(other);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_modifier substr(
const size_type off = 0, size_type n = npos) const WJR_NOEXCEPT {
return basic_string_modifier(_Mybase::substr(off, n));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_modifier
nesubstr(const size_type off = 0, size_type n = npos) const noexcept {
return basic_string_modifier(_Mybase::nesubstr(off, n));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_modifier view(
const value_type* s, const size_type n) noexcept {
return basic_string_modifier(s, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_modifier view(
const value_type* s) noexcept {
return basic_string_modifier(s);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_modifier view(
std::initializer_list<value_type> il) noexcept {
return view(il.begin(), il.size());
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_modifier view(const StringView& t) noexcept {
return basic_string_modifier(t);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_modifier view(const StringView& t,
const size_type off, const size_type n = npos) noexcept {
return view(t).nesubstr(off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_modifier view() const noexcept {
return *this;
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_string_modifier eview(const StringView& t,
const size_type off, const size_type n = npos) WJR_NOEXCEPT {
return view(t).substr(off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_modifier prefix(size_type n) const noexcept {
return basic_string_modifier(prefix(n));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_modifier suffix(size_type n) const noexcept {
return basic_string_modifier(suffix(n));
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) basic_string_modifier adjust_begin(const_iterator iter) const noexcept {
return basic_string_modifier(_Mybase::adjust_begin(iter));
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) basic_string_modifier adjust_end(const_iterator iter) const noexcept {
return basic_string_modifier(_Mybase::adjust_end(iter));
}
WJR_INLINE_CONSTEXPR pointer data() { return const_cast<pointer>(_Mybase::data()); }
WJR_INLINE_CONSTEXPR iterator begin() { return const_cast<iterator>(_Mybase::begin()); }
WJR_INLINE_CONSTEXPR iterator end() { return const_cast<iterator>(_Mybase::end()); }
WJR_INLINE_CONSTEXPR reverse_iterator rbegin() {
return reverse_iterator(end());
}
WJR_INLINE_CONSTEXPR reverse_iterator rend() {
return reverse_iterator(begin());
}
WJR_INLINE_CONSTEXPR reference front() {
return *begin();
}
WJR_INLINE_CONSTEXPR reference operator[](size_type n) {
return data()[n];
}
using _Mybase::size;
template<typename otherChar, typename otherTraits>
WJR_INLINE_CONSTEXPR20 basic_string_modifier append(basic_string_view<otherChar, otherTraits> str) {
static_assert(sizeof(otherChar) <= sizeof(Char), "");
wjr::uninitialized_copy_n(str.begin(), str.size(), data());
return basic_string_modifier(data() + str.size(), size() - str.size());
}
template<typename otherChar>
WJR_INLINE_CONSTEXPR20 basic_string_modifier append(size_type n, otherChar ch) {
static_assert(sizeof(otherChar) <= sizeof(Char), "");
wjr::uninitialized_fill_n(data(), n, ch);
return basic_string_modifier(data() + n, size() - n);
}
WJR_INLINE_CONSTEXPR void finalize() {
front() = value_type();
}
};
using string_modifier = basic_string_modifier<char>;
using wstring_modifier = basic_string_modifier<wchar_t>;
using u16string_modifier = basic_string_modifier<char16_t>;
using u32string_modifier = basic_string_modifier<char32_t>;
_WJR_END
#endif // __WJR_STRING_H

#pragma once
#ifndef __WJR_RANDOM_H
#define __WJR_RANDOM_H
#include <random>
_WJR_BEGIN
template<typename Engine>
struct basic_random_static_wrapper {
static Engine m_engine;
Engine& engine() const {
return m_engine;
}
};
template<typename Engine>
Engine basic_random_static_wrapper<Engine>::m_engine(std::random_device{}());
template<typename Engine>
struct basic_random_thread_local_wrapper {
Engine& engine() const {
thread_local Engine m_engine(std::random_device{}());
return m_engine;
}
};
template<
typename Engine,
template<typename> typename EngineWrapper,
template<typename> typename IntegerDist = std::uniform_int_distribution,
template<typename> typename RealDist = std::uniform_real_distribution,
typename BoolDist = std::bernoulli_distribution
>
class __basic_random : public EngineWrapper<Engine> {
template<typename T>
using _Is_container = std::conjunction<
has_global_function_std_begin<T>,
has_global_function_std_end<T>
>;
public:
using _Mybase = EngineWrapper<Engine>;
using engine_type = Engine;
template<typename T>
using integer_dist_t = IntegerDist<T>;
template<typename T>
using real_dist_t = RealDist<T>;
using bool_dist_t = BoolDist;
using common = std::common_type<>;
using result_type = typename engine_type::result_type;
using _Mybase::engine;
private:
template<typename _Seq>
struct _Is_seed_seq : std::bool_constant<
!std::is_convertible_v<_Seq, result_type>
&& !std::is_same_v<remove_cvref_t<_Seq>, engine_type>
&& !std::is_same_v<remove_cvref_t<_Seq>, __basic_random>
> {};
template<typename _Seq>
constexpr static bool _Is_seed_seq_v = _Is_seed_seq<_Seq>::value;
public:
constexpr static result_type min() {
return engine_type::min();
}
constexpr static result_type max() {
return engine_type::max();
}
void discard(size_t n) const {
engine().discard(n);
}
void reseed() const {
engine().seed(std::random_device{}());
}
void seed(result_type seed = engine_type::default_seed) const {
engine().seed(seed);
}
template<typename _Seq, std::enable_if_t<_Is_seed_seq_v<_Seq>, int> = 0>
void seed(_Seq& seq) const {
engine().seed(seq);
}
result_type get() const {
return engine()();
}
template<typename T, std::enable_if_t<std::conjunction_v<
std::is_arithmetic<T>, std::negation<std::is_same<T, bool>>>, int> = 0>
T get(
T __min = std::numeric_limits<T>::min(),
T __max = std::numeric_limits<T>::max()) const {
if constexpr (std::is_integral_v<T>) {
if constexpr (sizeof(T) < sizeof(short)) {
using short_t = std::conditional_t<std::is_signed<T>::value,
short, unsigned short>;
return static_cast<T>(integer_dist_t<short_t>{__min, __max}(engine()));
}
else {
using type = int_or_uint_t<sizeof(T) * 8, std::is_signed_v<T>>;
return integer_dist_t<type>{__min, __max}(engine());
}
}
else {
return real_dist_t<T>{__min, __max}(engine());
}
}
template<typename X, typename T, typename U, typename R = std::common_type_t<T, U>,
std::enable_if_t<std::conjunction_v<
std::is_same<X, common>,
std::is_arithmetic<T>,
std::is_arithmetic<U>>, int> = 0>
R get(
T __min = std::numeric_limits<T>::min(),
U __max = std::numeric_limits<U>::max()) const {
return get<R>(static_cast<R>(__min), static_cast<R>(__max));
}
template<typename T, std::enable_if_t<std::is_same_v<T, bool>, int> = 0>
T get(double p = 0.5) const {
return bool_dist_t{ p }(engine());
}
template<typename Dist, typename...Args>
typename Dist::result_type get(Args&&...args) const {
return Dist{ std::forward<Args>(args)... }(engine());
}
template<typename Dist>
typename Dist::result_type get(Dist& dist) const {
return dist(engine());
}
template<typename _Container, std::enable_if_t<std::conjunction_v<
has_global_function_std_begin<_Container>,
has_global_function_std_end<_Container>>, int> = 0>
decltype(auto) get(_Container& container) const {
auto first = std::begin(container);
auto last = std::end(container);
auto n = wjr::size(container);
if (n == 0)return last;
using diff_t = wjr::iter_diff_t<decltype(first)>;
return std::next(first, get<diff_t>(0, n - 1));
}
template<typename iter, std::enable_if_t<wjr::is_iterator_v<iter>, int> = 0>
iter get(iter first, iter last) const {
return get(wjr::make_iter_wrapper(first, last));
}
template<typename T>
T get(std::initializer_list<T> il) const {
return *get(std::begin(il), std::end(il));
}
template<typename _Ty, size_t _Size>
_Ty get(_Ty(&arr)[_Size]) const {
return *get(std::begin(arr), std::end(arr));
}
template<typename _Container>
void shuffle(_Container& container) const {
std::shuffle(std::begin(container), std::end(container), engine());
}
template<typename iter>
void shuffle(iter first, iter last) const {
shuffle(wjr::make_iter_wrapper(first, last));
}
};
template<typename Engine,
template<typename>typename IntegerDist = std::uniform_int_distribution,
template<typename>typename RealDist = std::uniform_real_distribution,
typename BoolDist = std::bernoulli_distribution>
using basic_random = __basic_random<Engine, basic_random_static_wrapper, IntegerDist, RealDist, BoolDist>;
template<typename Engine,
template<typename>typename IntegerDist = std::uniform_int_distribution,
template<typename>typename RealDist = std::uniform_real_distribution,
typename BoolDist = std::bernoulli_distribution>
using basic_random_thread_local = __basic_random<Engine, basic_random_thread_local_wrapper, IntegerDist, RealDist, BoolDist>;
template<
template<typename>typename IntegerDist = std::uniform_int_distribution,
template<typename>typename RealDist = std::uniform_real_distribution,
typename BoolDist = std::bernoulli_distribution>
using default_random = basic_random<std::mt19937, IntegerDist, RealDist, BoolDist>;
template<
template<typename>typename IntegerDist = std::uniform_int_distribution,
template<typename>typename RealDist = std::uniform_real_distribution,
typename BoolDist = std::bernoulli_distribution>
using default_random_thread_local = basic_random_thread_local<std::mt19937, IntegerDist, RealDist, BoolDist>;
inline constexpr default_random<> random;
inline constexpr default_random_thread_local<> random_thread_local;
_WJR_END
#endif // __WJR_RANDOM_H

#pragma once
#ifndef __WJR_ALLOCATOR_H
#define __WJR_ALLOCATOR_H
#include <new>
#include <memory>
_WJR_BEGIN
template<typename _Ty>
struct __aligned_helper;
template<>
struct __aligned_helper<void> {
constexpr static size_t value = std::max((size_t)(16), (size_t)(alignof(max_align_t)));
};
template<typename _Ty>
struct __aligned_helper {
constexpr static size_t value = std::max((size_t)alignof(_Ty), __aligned_helper<void>::value);
};
#if defined(_DEBUG)
#define WJR_TEST_ALLOCATOR
#endif
#if defined(WJR_TEST_ALLOCATOR)
struct __test_allocator {
void test()const;
~__test_allocator();
long long _Count = 0;
};
extern __test_allocator __test_allocator_instance;
#endif
template<typename _Ty, size_t _Al = __aligned_helper<_Ty>::value, size_t _Off = 0>
class aligned_allocator {
public:
static_assert(!std::is_const_v<_Ty>, "The C++ Standard forbids containers of const elements "
"because allocator<const T> is ill-formed.");
static_assert(_Al >= 8, "The alignment must be greater than or equal to 8.");
static_assert(_Off >= 0 && _Off < _Al,
"The offset must be greater than or equal to 0 and less than the alignment.");
static_assert((_Al & (_Al - 1)) == 0, "The alignment must be a power of 2.");
using value_type = _Ty;
#if !defined(WJR_CPP_20)
using pointer = _Ty*;
using const_pointer = const _Ty*;
using reference = _Ty&;
using const_reference = const _Ty&;
#endif // !WJR_CPP_20
using size_type = size_t;
using difference_type = ptrdiff_t;
using propagate_on_container_move_assignment = std::true_type;
using is_always_equal = std::true_type;
template <typename _Other>
struct rebind {
using other = aligned_allocator<_Other, _Al, _Off>;
};
#if !defined(WJR_CPP_20)
WJR_NODISCARD _Ty* address(_Ty& _Val) const noexcept {
return std::addressof(_Val);
}
WJR_NODISCARD const _Ty* address(const _Ty& _Val) const noexcept {
return std::addressof(_Val);
}
#endif // !WJR_CPP_20
constexpr aligned_allocator() noexcept {}
constexpr aligned_allocator(const aligned_allocator&) noexcept = default;
template <typename _Other>
constexpr aligned_allocator(const aligned_allocator<_Other, _Al, _Off>&) noexcept {}
WJR_CONSTEXPR20 ~aligned_allocator() = default;
WJR_CONSTEXPR20 aligned_allocator& operator=(const aligned_allocator&) = default;
WJR_CONSTEXPR20 void deallocate(_Ty* const _Ptr, const size_t _Count) {
#if defined(WJR_TEST_ALLOCATOR)
__test_allocator_instance._Count -= _Count * sizeof(_Ty) + _Off;
#endif
WJR_ASSUME(reinterpret_cast<uintptr_t>(_Ptr) % _Al == _Off);
auto ptr = reinterpret_cast<char*>(_Ptr) - _Off;
::operator delete(ptr, _Count * sizeof(_Ty) + _Off, static_cast<std::align_val_t>(_Al));
}
WJR_NODISCARD WJR_CONSTEXPR20 WJR_MALLOC _Ty* allocate(const size_t _Count) {
#if defined(WJR_TEST_ALLOCATOR)
__test_allocator_instance._Count += _Count * sizeof(_Ty) + _Off;
#endif
auto ptr = static_cast<char*>(::operator new(_Count * sizeof(_Ty) + _Off,
static_cast<std::align_val_t>(_Al))) + _Off;
WJR_ASSUME(reinterpret_cast<uintptr_t>(ptr) % _Al == _Off);
return reinterpret_cast<_Ty*>(ptr);
}
#if !defined(WJR_CPP_20)
WJR_NODISCARD size_t max_size() const noexcept {
return static_cast<size_t>(-1) / sizeof(_Ty);
}
#endif // !WJR_CPP_20
};
template <size_t _Al, size_t _Off>
class aligned_allocator<void, _Al, _Off> {
public:
using value_type = void;
#if !defined(WJR_CPP_20)
using pointer = void*;
using const_pointer = const void*;
#endif // !WJR_CPP_20
using size_type = size_t;
using difference_type = ptrdiff_t;
using propagate_on_container_move_assignment = std::true_type;
using is_always_equal = std::true_type;
#if !defined(WJR_CPP_20)
template <class _Other>
struct rebind {
using other = aligned_allocator<_Other>;
};
#endif // !WJR_CPP_20
};
template <typename _Ty, typename _Other, size_t _Al, size_t _Off>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool operator==(
const aligned_allocator<_Ty, _Al, _Off>&,
const aligned_allocator<_Other, _Al, _Off>&) noexcept {
return true;
}
template <typename _Ty, size_t _Al1, size_t _Off1, typename _Other, size_t _Al2, size_t _Off2>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool operator==(
const aligned_allocator<_Ty, _Al1, _Off1>&,
const aligned_allocator<_Other, _Al2, _Off2>&) noexcept {
return false;
}
template <typename _Ty, size_t _Al1, size_t _Off1, typename _Other, size_t _Al2, size_t _Off2>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool operator!=(
const aligned_allocator<_Ty, _Al1, _Off1>& lhs,
const aligned_allocator<_Other, _Al2, _Off2>& rhs) noexcept {
return !(lhs == rhs);
}
template<typename T, size_t _Al, size_t _Off>
struct is_default_allocator<aligned_allocator<T, _Al, _Off>> : std::true_type {};
template <int __inst>
class __malloc_alloc_template__ {
private:
static void* _S_oom_malloc(size_t);
static void* _S_oom_realloc(void*, size_t);
#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
static void (*__malloc_alloc_oom_handler)();
#endif
public:
static void* allocate(size_t __n) {
void* __result = malloc(__n);
if (WJR_UNLIKELY(0 == __result)) __result = _S_oom_malloc(__n);
return __result;
}
static void deallocate(void* __p, size_t /* __n */) {
free(__p);
}
static void* reallocate(void* __p, size_t /* old_sz */, size_t __new_sz) {
void* __result = realloc(__p, __new_sz);
if (WJR_UNLIKELY(0 == __result)) __result = _S_oom_realloc(__p, __new_sz);
return __result;
}
static void (*__set_malloc_handler(void (*__f)()))()
{
void (*__old)() = __malloc_alloc_oom_handler;
__malloc_alloc_oom_handler = __f;
return(__old);
}
};
#ifndef __STL_STATIC_TEMPLATE_MEMBER_BUG
template <int __inst>
void (*__malloc_alloc_template__<__inst>::__malloc_alloc_oom_handler)() = 0;
#endif
template <int __inst>
void*
__malloc_alloc_template__<__inst>::_S_oom_malloc(size_t __n) {
void (*__my_malloc_handler)();
void* __result;
for (;;) {
__my_malloc_handler = __malloc_alloc_oom_handler;
(*__my_malloc_handler)();
__result = malloc(__n);
if (__result) return(__result);
}
}
template <int __inst>
void* __malloc_alloc_template__<__inst>::_S_oom_realloc(void* __p, size_t __n) {
void (*__my_malloc_handler)();
void* __result;
for (;;) {
__my_malloc_handler = __malloc_alloc_oom_handler;
(*__my_malloc_handler)();
__result = realloc(__p, __n);
if (__result) return(__result);
}
}
typedef __malloc_alloc_template__<0> malloc_alloc;
enum { ALLOC_ALIGN = 8 };
enum { ALLOC_MAX_BYTES = 256 };
enum { ALLOC_NFRELISTS = ALLOC_MAX_BYTES / ALLOC_ALIGN };
template <bool threads, int inst>
class __default_alloc_template__;
template<bool threads>
class __basic_default_alloc_template__;
template<>
class __basic_default_alloc_template__<true> {
private:
template<bool, int>
friend class __default_alloc_template__;
enum { __ALIGN = ALLOC_ALIGN };
enum { __MAX_BYTES = ALLOC_MAX_BYTES };
enum { __NFREELISTS = ALLOC_NFRELISTS };
union obj {
union obj* free_list_link;
char client_data[1];
};
thread_local static obj* volatile free_list[__NFREELISTS];
thread_local static char* start_free;
thread_local static char* end_free;
thread_local static size_t heap_size;
};
template<>
class __basic_default_alloc_template__<false> {
private:
template<bool, int>
friend class __default_alloc_template__;
enum { __ALIGN = ALLOC_ALIGN };
enum { __MAX_BYTES = ALLOC_MAX_BYTES };
enum { __NFREELISTS = ALLOC_NFRELISTS };
union obj {
union obj* free_list_link;
char client_data[1];
};
static obj* volatile free_list[__NFREELISTS];
static char* start_free;
static char* end_free;
static size_t heap_size;
};
template <bool threads, int inst>
class __default_alloc_template__ {
private:
using base = __basic_default_alloc_template__<threads>;
using obj = typename base::obj;
enum { __ALIGN = ALLOC_ALIGN };
enum { __MAX_BYTES = ALLOC_MAX_BYTES };
enum { __NFREELISTS = ALLOC_NFRELISTS };
static inline size_t ROUND_UP(size_t bytes) {
return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
}
static inline size_t FREELIST_INDEX(size_t bytes) {
return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
}
WJR_NOINLINE static void* refill(size_t n) noexcept;
static char* chunk_alloc(size_t size, int& nobjs) noexcept;
public:
WJR_INTRINSIC_INLINE static void* allocate(size_t n) noexcept //n must be > 0
{
if (n > (size_t)__MAX_BYTES) {
return malloc_alloc::allocate(n);
}
obj* volatile* my_free_list = base::free_list + FREELIST_INDEX(n);
obj* result = *my_free_list;
if (WJR_LIKELY(result != nullptr)) {
*my_free_list = result->free_list_link;
return result;
}
return refill(ROUND_UP(n));
}
static void deallocate(void* p, size_t n) noexcept //p may not be 0
{
if (n > (size_t)__MAX_BYTES) {
malloc_alloc::deallocate(p, n);
return;
}
obj* q = (obj*)p;
obj* volatile* my_free_list = base::free_list + FREELIST_INDEX(n);
q->free_list_link = *my_free_list;
*my_free_list = q;
}
};
template <bool threads, int inst>
char* __default_alloc_template__<threads, inst>::
chunk_alloc(size_t size, int& nobjs) noexcept {
char* result;
size_t total_bytes = size * nobjs;
const auto bytes_left = static_cast<size_t>(base::end_free - base::start_free);
if (bytes_left >= total_bytes) {
result = base::start_free;
base::start_free += total_bytes;
return (result);
}
if (bytes_left >= size) {
nobjs = static_cast<int>(bytes_left / size);
total_bytes = size * nobjs;
result = base::start_free;
base::start_free += total_bytes;
return (result);
}
const size_t bytes_to_get =
2 * total_bytes + ROUND_UP(base::heap_size >> 4);
if (bytes_left > 0) {
obj* volatile* my_free_list =
base::free_list + FREELIST_INDEX(bytes_left);
((obj*)base::start_free)->free_list_link = *my_free_list;
*my_free_list = (obj*)base::start_free;
}
base::start_free = (char*)malloc(bytes_to_get);
if (WJR_UNLIKELY(0 == base::start_free)) {
obj* volatile* my_free_list, * p;
for (int i = static_cast<int>(size); i <= __MAX_BYTES; i += __ALIGN) {
my_free_list = base::free_list + FREELIST_INDEX(i);
p = *my_free_list;
if (0 != p) {
*my_free_list = p->free_list_link;
base::start_free = (char*)p;
base::end_free = base::start_free + i;
return (chunk_alloc(size, nobjs));
}
}
base::end_free = 0; //In case of exception.
base::start_free = (char*)malloc(bytes_to_get);
}
base::heap_size += bytes_to_get;
base::end_free = base::start_free + bytes_to_get;
return (chunk_alloc(size, nobjs));
}
template <bool threads, int inst>
void* __default_alloc_template__<threads, inst>::
refill(size_t n) noexcept {
int nobjs = 20;
char* chunk = chunk_alloc(n, nobjs);
obj* current_obj;
obj* next_obj;
if (1 == nobjs) return (chunk);
obj* volatile* my_free_list = base::free_list + FREELIST_INDEX(n);
obj* result = (obj*)chunk;
*my_free_list = current_obj = (obj*)(chunk + n);
nobjs -= 2;
while (nobjs) {
--nobjs;
next_obj = (obj*)((char*)current_obj + n);
current_obj->free_list_link = next_obj;
current_obj = next_obj;
}
current_obj->free_list_link = 0;
return (result);
}
template <typename Ty, bool threads>
class basic_allocator {
private:
using allocator_type = __default_alloc_template__<threads, 0>;
public:
using value_type = Ty;
using reference = Ty&;
using const_reference = const Ty&;
using pointer = Ty*;
using const_pointer = const Ty*;
using size_type = size_t;
using difference_type = ptrdiff_t;
using propagate_on_container_move_assignment = std::true_type;
using is_always_equal = std::true_type;
template <typename _Other>
struct rebind {
using other = basic_allocator<_Other, threads>;
};
#if !defined(WJR_CPP_20)
WJR_NODISCARD Ty* address(Ty& _Val) const noexcept {
return std::addressof(_Val);
}
WJR_NODISCARD const Ty* address(const Ty& _Val) const noexcept {
return std::addressof(_Val);
}
#endif // !WJR_CPP_20
constexpr basic_allocator() noexcept = default;
constexpr basic_allocator(const basic_allocator&) noexcept = default;
template <class _Other>
constexpr explicit basic_allocator(const basic_allocator<_Other, threads>&) noexcept {}
~basic_allocator() = default;
basic_allocator& operator=(const basic_allocator&) noexcept = default;
WJR_NODISCARD WJR_CONSTEXPR20 WJR_MALLOC Ty* allocate(size_t n) const noexcept {
#if defined(WJR_TEST_ALLOCATOR)
__test_allocator_instance._Count += n;
#endif
if (!is_constant_evaluated()) {
if (WJR_UNLIKELY(0 == n)) {
return nullptr;
}
return static_cast<Ty*>(allocator_type::allocate(sizeof(Ty) * n));
}
return std::allocator<Ty>().allocate(n);
}
WJR_CONSTEXPR20 void deallocate(Ty* ptr, size_t n) const noexcept {
#if defined(WJR_TEST_ALLOCATOR)
__test_allocator_instance._Count -= n;
#endif
if (!is_constant_evaluated()) {
if (WJR_UNLIKELY(0 == n)) return;
return allocator_type::deallocate(static_cast<void*>(ptr), sizeof(Ty) * n);
}
return std::allocator<Ty>().deallocate(ptr, n);
}
constexpr size_t max_size()const {
return static_cast<size_t>(-1) / sizeof(Ty);
}
};
template<typename T, typename U, bool t1, bool t2>
constexpr bool operator==(const basic_allocator<T, t1>&, const basic_allocator<U, t2>&) {
return t1 == t2;
}
template<typename T, typename U, bool t1, bool t2>
constexpr bool operator!=(const basic_allocator<T, t1>&, const basic_allocator<U, t2>&) {
return t1 != t2;
}
template<typename T, bool f>
struct is_default_allocator<basic_allocator<T, f>> : std::true_type {};
template<typename T>
using nsallocator = basic_allocator<T, false>;
template<typename T>
using sallocator = basic_allocator<T, true>;
template<typename T, typename Alloc>
class unique_ptr_with_allocator_delete
: private _Pair_wrapper<0, typename std::allocator_traits<Alloc>::template rebind_alloc<T>>{
using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
using _Mybase = _Pair_wrapper<0, _Alty>;
using _Alty_traits = std::allocator_traits<Alloc>;
public:
using allocator_type = Alloc;
constexpr unique_ptr_with_allocator_delete()
noexcept(std::is_nothrow_default_constructible_v<_Alty>) : _Mybase() {}
constexpr unique_ptr_with_allocator_delete(const allocator_type& _Al)
noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>) : _Mybase(_Al) {}
constexpr unique_ptr_with_allocator_delete(const unique_ptr_with_allocator_delete&) = default;
constexpr unique_ptr_with_allocator_delete(unique_ptr_with_allocator_delete&&) = default;
constexpr unique_ptr_with_allocator_delete& operator=(const unique_ptr_with_allocator_delete&) = default;
constexpr unique_ptr_with_allocator_delete& operator=(unique_ptr_with_allocator_delete&&) = default;
constexpr void operator()(T* ptr) {
destroy_at(_Mybase::value(), ptr);
_Mybase::value().deallocate(ptr, 1);
}
};
template<typename T, typename Alloc>
class unique_ptr_with_allocator_delete<T[], Alloc> {
using _Alty = typename std::allocator_traits<Alloc>::template rebind<T>;
using _Alty_traits = std::allocator_traits<Alloc>;
public:
using allocator_type = Alloc;
constexpr unique_ptr_with_allocator_delete() = default;
constexpr unique_ptr_with_allocator_delete(size_t n, const allocator_type& _Al = allocator_type())
noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>)
: m_value(std::piecewise_construct_t(),
std::forward_as_tuple(_Al),
std::forward_as_tuple(n)) {}
constexpr unique_ptr_with_allocator_delete(const unique_ptr_with_allocator_delete&) = default;
constexpr unique_ptr_with_allocator_delete(unique_ptr_with_allocator_delete&&) = default;
constexpr unique_ptr_with_allocator_delete& operator=(const unique_ptr_with_allocator_delete&) = default;
constexpr unique_ptr_with_allocator_delete& operator=(unique_ptr_with_allocator_delete&&) = default;
constexpr void operator()(T* ptr) {
destroy_n(m_value.first(), ptr, m_value.second());
m_value.first().deallocate(ptr, m_value.second());
}
private:
compressed_pair<_Alty, size_t> m_value;
};
template<typename T>
using unique_ptr_with_nsallocator_delete = unique_ptr_with_allocator_delete<T, nsallocator<T>>;
template<typename T>
using unique_ptr_with_sallocator_delete = unique_ptr_with_allocator_delete<T, sallocator<T>>;
template<typename T>
using unique_ptr_with_nsallocator = std::unique_ptr<T, unique_ptr_with_nsallocator_delete<T>>;
template<typename T>
using unique_ptr_with_sallocator = std::unique_ptr<T, unique_ptr_with_sallocator_delete<T>>;
template<typename T, typename...Args, std::enable_if_t<!std::is_array_v<T>, int> = 0>
constexpr auto make_unique_with_nsallocator(Args&&...args) {
auto al = nsallocator<T>();
auto _Ptr = al.allocate(1);
construct_at(al, _Ptr, std::forward<Args>(args)...);
return unique_ptr_with_nsallocator<T>(_Ptr);
}
template<typename T, std::enable_if_t<std::is_array_v<T>&& std::extent_v<T> == 0, int> = 0>
constexpr auto make_unique_with_nsallocator(size_t n) {
using elem = std::remove_extent_t<T>;
auto al = nsallocator<elem>();
auto _Ptr = al.allocate(n);
uninitialized_default_construct_n(al, _Ptr, n);
return unique_ptr_with_nsallocator<T>(_Ptr, unique_ptr_with_nsallocator_delete<T>(n));
}
template<typename T, typename...Args, std::enable_if_t<!std::is_array_v<T>, int> = 0>
constexpr auto make_unique_with_sallocator(Args&&...args) {
auto al = sallocator<T>();
auto _Ptr = al.allocate(1);
construct_at(al, _Ptr, std::forward<Args>(args)...);
return unique_ptr_with_sallocator<T>(_Ptr);
}
template<typename T, std::enable_if_t<std::is_array_v<T>&& std::extent_v<T> == 0, int> = 0>
constexpr auto make_unique_with_sallocator(size_t n) {
using elem = std::remove_extent_t<T>;
auto al = nsallocator<elem>();
auto _Ptr = al.allocate(n);
uninitialized_default_construct_n(al, _Ptr, n);
return unique_ptr_with_sallocator<T>(_Ptr, unique_ptr_with_sallocator_delete<T>(n));
}
_WJR_END
#endif // __WJR_ALLOCATOR_H
#include <stdio.h>
_WJR_BEGIN
#if defined(WJR_TEST_ALLOCATOR)
void __test_allocator::test() const {
if (_Count != 0) {
printf("memory leak: %lld bytes", _Count);
}
}
__test_allocator::~__test_allocator() {
test();
}
__test_allocator __test_allocator_instance;
#endif
thread_local char* __basic_default_alloc_template__<true>::start_free = 0;
thread_local char* __basic_default_alloc_template__<true>::end_free = 0;
thread_local size_t __basic_default_alloc_template__<true>::heap_size = 0;
thread_local typename __basic_default_alloc_template__<true>::obj* volatile
__basic_default_alloc_template__<true>::free_list[__NFREELISTS]
= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
char* __basic_default_alloc_template__<false>::start_free = 0;
char* __basic_default_alloc_template__<false>::end_free = 0;
size_t __basic_default_alloc_template__<false>::heap_size = 0;
typename __basic_default_alloc_template__<false>::obj* volatile
__basic_default_alloc_template__<false>::free_list[__NFREELISTS]
= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
_WJR_END


#pragma once
#ifndef __WJR_NETWORK_THREAD_POOL_H
#define __WJR_NETWORK_THREAD_POOL_H
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#pragma once
#ifndef __WJR_CIRCULAR_BUFFER_H
#define __WJR_CIRCULAR_BUFFER_H
_WJR_BEGIN
template<typename T>
class circular_buffer_const_iterator {
public:
using iterator_category = std::random_access_iterator_tag;
using value_type = T;
using difference_type = std::ptrdiff_t;
using pointer = const T*;
using reference = const T&;
WJR_CONSTEXPR20 circular_buffer_const_iterator() noexcept = default;
WJR_CONSTEXPR20 circular_buffer_const_iterator(const circular_buffer_const_iterator&) noexcept = default;
WJR_CONSTEXPR20 circular_buffer_const_iterator&
operator=(const circular_buffer_const_iterator&) noexcept = default;
WJR_CONSTEXPR20 circular_buffer_const_iterator(pointer m_ptr, size_t m_pos, size_t m_capacity) noexcept
: m_ptr(m_ptr), m_pos(m_pos), m_capacity(m_capacity) {}
WJR_NODISCARD WJR_CONSTEXPR20 reference operator*() const noexcept {
return m_ptr[offset()];
}
WJR_NODISCARD WJR_CONSTEXPR20 pointer operator->() const noexcept {
return m_ptr + offset();
}
WJR_CONSTEXPR20 circular_buffer_const_iterator& operator++() noexcept {
assume_no_add_overflow(m_pos, static_cast<size_t>(1));
++m_pos;
return *this;
}
WJR_CONSTEXPR20 circular_buffer_const_iterator operator++(int)noexcept {
circular_buffer_const_iterator tmp = *this;
++(*this);
return tmp;
}
WJR_CONSTEXPR20 circular_buffer_const_iterator& operator--() noexcept {
assume_no_sub_overflow(m_pos, static_cast<size_t>(1));
--m_pos;
return *this;
}
WJR_CONSTEXPR20 circular_buffer_const_iterator operator--(int)noexcept {
circular_buffer_const_iterator tmp = *this;
--(*this);
return tmp;
}
WJR_CONSTEXPR20 circular_buffer_const_iterator& operator+=(const difference_type n) noexcept {
assume_no_add_overflow(m_pos, static_cast<size_t>(n));
m_pos += n;
return *this;
}
WJR_CONSTEXPR20 circular_buffer_const_iterator operator+(const difference_type n) const noexcept {
circular_buffer_const_iterator tmp = *this;
tmp += n;
return tmp;
}
WJR_CONSTEXPR20 circular_buffer_const_iterator& operator-=(const difference_type n) noexcept {
(*this) += (-n);
return *this;
}
WJR_CONSTEXPR20 circular_buffer_const_iterator operator-(const difference_type n) const noexcept {
circular_buffer_const_iterator tmp = *this;
tmp -= n;
return tmp;
}
WJR_CONSTEXPR20 friend difference_type operator-(
const circular_buffer_const_iterator& lhs,
const circular_buffer_const_iterator& rhs
) noexcept {
return lhs.m_pos - rhs.m_pos;
}
WJR_NODISCARD WJR_CONSTEXPR20 reference operator[](const difference_type n) const noexcept {
return *(*this + n);
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator==(
const circular_buffer_const_iterator& lhs,
const circular_buffer_const_iterator& rhs
) noexcept {
return lhs.m_pos == rhs.m_pos;
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator!=(
const circular_buffer_const_iterator& lhs,
const circular_buffer_const_iterator& rhs
) noexcept {
return !(lhs == rhs);
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator<(
const circular_buffer_const_iterator& lhs,
const circular_buffer_const_iterator& rhs
) noexcept {
return lhs.m_pos < rhs.m_pos;
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator<=(
const circular_buffer_const_iterator& lhs,
const circular_buffer_const_iterator& rhs
) noexcept {
return !(rhs < lhs);
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator>(
const circular_buffer_const_iterator& lhs,
const circular_buffer_const_iterator& rhs
) noexcept {
return rhs < lhs;
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator>=(
const circular_buffer_const_iterator& lhs,
const circular_buffer_const_iterator& rhs
) noexcept {
return !(lhs < rhs);
}
WJR_CONSTEXPR20 pointer ptr() const noexcept {
return m_ptr;
}
WJR_CONSTEXPR20 size_t pos() const noexcept {
return m_pos;
}
WJR_CONSTEXPR20 size_t offset() const noexcept {
return m_pos < m_capacity ? m_pos : m_pos - m_capacity;
}
WJR_CONSTEXPR20 size_t capacity() const noexcept {
return m_capacity;
}
private:
pointer m_ptr = nullptr;
size_t m_pos = 0;
size_t m_capacity = 0;
};
template<typename T>
class circular_buffer_iterator : public circular_buffer_const_iterator<T> {
public:
using iterator_category = std::random_access_iterator_tag;
using value_type = T;
using difference_type = std::ptrdiff_t;
using pointer = T*;
using reference = T&;
using _Mybase = circular_buffer_const_iterator<T>;
WJR_CONSTEXPR20 circular_buffer_iterator() noexcept = default;
WJR_CONSTEXPR20 circular_buffer_iterator(const circular_buffer_iterator&) noexcept = default;
WJR_CONSTEXPR20 circular_buffer_iterator&
operator=(const circular_buffer_iterator&) noexcept = default;
WJR_CONSTEXPR20 circular_buffer_iterator(pointer m_ptr, size_t m_pos, size_t m_capacity) noexcept
: _Mybase(m_ptr, m_pos, m_capacity) {}
WJR_NODISCARD WJR_CONSTEXPR20 reference operator*() const noexcept {
return const_cast<reference>(_Mybase::operator*());
}
WJR_NODISCARD WJR_CONSTEXPR20 pointer operator->() const noexcept {
return const_cast<pointer>(_Mybase::operator->());
}
WJR_CONSTEXPR20 circular_buffer_iterator& operator++() noexcept {
_Mybase::operator++();
return *this;
}
WJR_CONSTEXPR20 circular_buffer_iterator operator++(int)noexcept {
circular_buffer_iterator tmp = *this;
++(*this);
return tmp;
}
WJR_CONSTEXPR20 circular_buffer_iterator& operator--() noexcept {
_Mybase::operator--();
return *this;
}
WJR_CONSTEXPR20 circular_buffer_iterator operator--(int)noexcept {
circular_buffer_iterator tmp = *this;
--(*this);
return tmp;
}
WJR_CONSTEXPR20 circular_buffer_iterator& operator+=(const difference_type n) noexcept {
_Mybase::operator+=(n);
return *this;
}
WJR_CONSTEXPR20 circular_buffer_iterator operator+(const difference_type n) const noexcept {
circular_buffer_iterator tmp = *this;
tmp += n;
return tmp;
}
WJR_CONSTEXPR20 circular_buffer_iterator& operator-=(const difference_type n) noexcept {
(*this) += (-n);
return *this;
}
WJR_CONSTEXPR20 circular_buffer_iterator operator-(const difference_type n) const noexcept {
circular_buffer_iterator tmp = *this;
tmp -= n;
return tmp;
}
WJR_CONSTEXPR20 friend difference_type operator-(
const circular_buffer_iterator& lhs,
const circular_buffer_iterator& rhs
) noexcept {
return lhs.pos() - rhs.pos();
}
WJR_NODISCARD WJR_CONSTEXPR20 reference operator[](const difference_type n) const noexcept {
return *(*this + n);
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator==(
const circular_buffer_iterator& lhs,
const circular_buffer_iterator& rhs
) noexcept {
return lhs.pos() == rhs.pos();
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator!=(
const circular_buffer_iterator& lhs,
const circular_buffer_iterator& rhs
) noexcept {
return !(lhs == rhs);
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator<(
const circular_buffer_iterator& lhs,
const circular_buffer_iterator& rhs
) noexcept {
return lhs.pos() < rhs.pos();
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator<=(
const circular_buffer_iterator& lhs,
const circular_buffer_iterator& rhs
) noexcept {
return !(rhs < lhs);
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator>(
const circular_buffer_iterator& lhs,
const circular_buffer_iterator& rhs
) noexcept {
return rhs < lhs;
}
WJR_NODISCARD WJR_CONSTEXPR20 friend bool operator>=(
const circular_buffer_iterator& lhs,
const circular_buffer_iterator& rhs
) noexcept {
return !(lhs < rhs);
}
WJR_NODISCARD WJR_CONSTEXPR20 _Mybase& base() {
return *this;
}
WJR_NODISCARD WJR_CONSTEXPR20 const _Mybase& base() const {
return *this;
}
};
template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);
template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);
template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(Alloc& al,
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);
template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(Alloc& al,
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);
template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);
template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);
template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(Alloc& al,
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);
template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(Alloc& al,
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);
template<typename T, typename Alloc = std::allocator<T>>
class circular_buffer {
using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
using _Alty_traits = std::allocator_traits<_Alty>;
public:
using value_type = T;
using allocator_type = Alloc;
using size_type = typename _Alty_traits::size_type;
using difference_type = typename _Alty_traits::difference_type;
using reference = value_type&;
using const_reference = const value_type&;
using pointer = typename _Alty_traits::pointer;
using const_pointer = typename _Alty_traits::const_pointer;
using iterator = circular_buffer_iterator<T>;
using const_iterator = circular_buffer_const_iterator<T>;
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;
private:
struct __data {
WJR_INTRINSIC_CONSTEXPR20 void reset() noexcept {
_Myptr = nullptr;
_Myhead = _Mytail = _Mysize = _Mycapacity = 0;
}
pointer _Myptr = nullptr;
size_type _Myhead = 0;
size_type _Mytail = 0;
size_type _Mysize = 0;
size_type _Mycapacity = 0;
};
public:
WJR_CONSTEXPR20 circular_buffer()
noexcept(std::is_nothrow_default_constructible_v<_Alty>) = default;
WJR_CONSTEXPR20 circular_buffer(const allocator_type& al)
noexcept(std::is_nothrow_constructible_v<_Alty, const allocator_type&>)
: _Myval(std::piecewise_construct_t{},
std::forward_as_tuple(al),
std::forward_as_tuple()) {}
template<typename _Alloc>
WJR_CONSTEXPR20 circular_buffer(const circular_buffer& other, _Alloc&& _Al, disable_tag)
noexcept(std::is_nothrow_constructible_v<_Alty, _Alloc&&>)
: _Myval(std::piecewise_construct_t{},
std::forward_as_tuple(_Al),
std::forward_as_tuple()){
auto& al = getAllocator();
auto& _Mydata = getData();
const auto& _Otherdata = other.getData();
if (_Otherdata._Mysize != 0) {
const auto _Newcapacity = getGrowthCapacity(0, _Otherdata._Mysize);
_Mydata._Myptr = _Alty_traits::allocate(al, _Newcapacity);
_Mydata._Myhead = 0;
_Mydata._Mytail = _Otherdata._Mysize;
_Mydata._Mysize = _Otherdata._Mysize;
_Mydata._Mycapacity = _Newcapacity;
wjr::uninitialized_copy(other.begin(), other.end(), _Mydata._Myptr);
}
}
WJR_CONSTEXPR20 circular_buffer(const circular_buffer& other)
: circular_buffer(other, _Alty_traits::select_on_container_copy_construction(other.getAllocator()),
disable_tag{}) {}
WJR_CONSTEXPR20 circular_buffer(const circular_buffer& other, const allocator_type& al)
: circular_buffer(other, al, disable_tag{}) {}
WJR_CONSTEXPR20 circular_buffer(circular_buffer&& other)
noexcept(std::is_nothrow_move_constructible_v<_Alty>)
: _Myval(std::piecewise_construct_t{},
std::forward_as_tuple(std::move(other.getAllocator())),
std::forward_as_tuple(other.getData())) {
other.getData().reset();
}
WJR_CONSTEXPR20 circular_buffer& operator=(const circular_buffer&) = delete;
WJR_CONSTEXPR20 circular_buffer& operator=(circular_buffer&&) = delete;
template<typename...Args>
WJR_INLINE_CONSTEXPR20 decltype(auto) emplace_back(Args&&...args) {
auto& al = getAllocator();
auto& _Mydata = getData();
if (WJR_LIKELY(_Mydata._Mysize != _Mydata._Mycapacity)) {
wjr::construct_at(al, _Mydata._Myptr + _Mydata._Mytail, std::forward<Args>(args)...);
++_Mydata._Mytail;
_Mydata._Mytail = _Mydata._Mytail == _Mydata._Mycapacity ? 0 : _Mydata._Mytail;
++_Mydata._Mysize;
}
else {
_M_realloc_insert_at_end(std::forward<Args>(args)...);
}
return back();
}
WJR_INLINE_CONSTEXPR20 void push_back(const value_type& val) { emplace_back(val); }
WJR_INLINE_CONSTEXPR20 void push_back(value_type&& val) { emplace_back(std::move(val)); }
template<typename...Args>
WJR_INLINE_CONSTEXPR20 decltype(auto) emplac_front(Args&&...args) {
auto& al = getAllocator();
auto& _Mydata = getData();
if (WJR_LIKELY(_Mydata._Mysize != _Mydata._Mycapacity)) {
_Mydata._Myhead = _Mydata._Myhead == 0 ? _Mydata._Mycapacity : _Mydata._Myhead;
--_Mydata._Myhead;
wjr::construct_at(al, _Mydata._Myptr + _Mydata._Myhead, std::forward<Args>(args)...);
++_Mydata._Mysize;
}
else {
_M_realloc_insert_at_begin(std::forward<Args>(args)...);
}
return back();
}
WJR_INLINE_CONSTEXPR20 void push_front(const value_type& val) { emplace_front(val); }
WJR_INLINE_CONSTEXPR20 void push_front(value_type&& val) { emplace_front(std::move(val)); }
WJR_INTRINSIC_CONSTEXPR20 void pop_front() noexcept {
auto& al = getAllocator();
auto& _Mydata = getData();
wjr::destroy_at(al, _Mydata._Myptr + _Mydata._Myhead);
++_Mydata._Myhead;
_Mydata._Myhead = _Mydata._Myhead == _Mydata._Mycapacity ? 0 : _Mydata._Myhead;
--_Mydata._Mysize;
}
WJR_INTRINSIC_CONSTEXPR20 void pop_back() noexcept {
auto& al = getAllocator();
auto& _Mydata = getData();
_Mydata._Mytail = _Mydata._Mytail == 0 ? _Mydata._Mycapacity : _Mydata._Mytail;
--_Mydata._Mytail;
wjr::destroy_at(al, _Mydata._Myptr + _Mydata._Mytail);
--_Mydata._Mysize;
}
WJR_INTRINSIC_CONSTEXPR20 reference front() noexcept {
const auto& _Mydata = getData();
return _Mydata._Myptr[_Mydata._Myhead];
}
WJR_INTRINSIC_CONSTEXPR20 const_reference front() const noexcept {
const auto& _Mydata = getData();
return _Mydata._Myptr[_Mydata._Myhead];
}
WJR_INTRINSIC_CONSTEXPR20 reference back() noexcept {
const auto& _Mydata = getData();
const auto __offset = _Mydata._Mytail == 0 ? _Mydata._Mycapacity : _Mydata._Mytail;
return _Mydata._Myptr[__offset - 1];
}
WJR_INTRINSIC_CONSTEXPR20 const_reference back() const noexcept {
const auto& _Mydata = getData();
const auto __offset = _Mydata._Mytail == 0 ? _Mydata._Mycapacity : _Mydata._Mytail;
return _Mydata._Myptr[__offset - 1];
}
WJR_INTRINSIC_CONSTEXPR20 size_type size() const noexcept { return getData()._Mysize; }
WJR_INTRINSIC_CONSTEXPR20 size_type capacity() const noexcept { return getData()._Mycapacity; }
WJR_INTRINSIC_CONSTEXPR20 bool empty() const noexcept { return size() == 0; }
WJR_CONSTEXPR20 void clear() {
auto& al = getAllocator();
auto& _Mydata = getData();
if (_Mydata._Mysize) {
wjr::destroy(al, begin(), end());
_Mydata._Myhead = _Mydata._Mytail = _Mydata._Mysize = 0;
}
}
WJR_INTRINSIC_CONSTEXPR20 iterator begin() {
const auto& _Mydata = getData();
return iterator(_Mydata._Myptr, _Mydata._Myhead, _Mydata._Mycapacity);
}
WJR_INTRINSIC_CONSTEXPR20 const_iterator begin() const {
const auto& _Mydata = getData();
return iterator(_Mydata._Myptr, _Mydata._Myhead, _Mydata._Mycapacity);
}
WJR_INTRINSIC_CONSTEXPR20 const_iterator cbegin() const {
return begin();
}
WJR_INTRINSIC_CONSTEXPR20 iterator end() {
return begin() + size();
}
WJR_INTRINSIC_CONSTEXPR20 const_iterator end() const {
return begin() + size();
}
WJR_INTRINSIC_CONSTEXPR20 const_iterator cend() const {
return end();
}
WJR_INTRINSIC_CONSTEXPR20 reverse_iterator rbegin() {
return reverse_iterator(end());
}
WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator rbegin() const {
return const_reverse_iterator(end());
}
WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator crbegin() const {
return const_reverse_iterator(end());
}
WJR_INTRINSIC_CONSTEXPR20 reverse_iterator rend() {
return reverse_iterator(begin());
}
WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator rend() const {
return const_reverse_iterator(begin());
}
WJR_INTRINSIC_CONSTEXPR20 const_reverse_iterator crend() const {
return const_reverse_iterator(begin());
}
WJR_INTRINSIC_CONSTEXPR20 reference operator[](size_type pos) {
return *(begin() + pos);
}
WJR_INTRINSIC_CONSTEXPR20 const_reference operator[](size_type pos) const {
return *(begin() + pos);
}
WJR_CONSTEXPR20 void tidy() {
auto& al = getAllocator();
auto& _Mydata = getData();
if (_Mydata._Myptr) {
clear();
_Alty_traits::deallocate(al, _Mydata._Myptr, _Mydata._Mycapacity);
_Mydata._Myptr = nullptr;
_Mydata._Mycapacity = 0;
}
}
WJR_INTRINSIC_CONSTEXPR20 _Alty& getAllocator() noexcept { return _Myval.first(); }
WJR_INTRINSIC_CONSTEXPR20 const _Alty& getAllocator() const noexcept { return _Myval.first(); }
WJR_INTRINSIC_CONSTEXPR20 __data& getData() noexcept { return _Myval.second(); }
WJR_INTRINSIC_CONSTEXPR20 const __data& getData() const noexcept { return _Myval.second(); }
template<typename _Iter, std::enable_if_t<is_iterator_v<_Iter>, int> = 0>
WJR_CONSTEXPR20 circular_buffer& append(_Iter _First, _Iter _Last) {
_M_range_append(_First, _Last, typename std::iterator_traits<_Iter>::iterator_category());
return *this;
}
private:
WJR_CONSTEXPR20 static size_type getGrowthCapacity(
const size_type _Oldcapacity, const size_type _Newsize) noexcept {
WJR_ASSUME(_Newsize > _Oldcapacity);
const auto _Newcapacity = _Oldcapacity + _Oldcapacity / 2;
const auto _FixedCapacity = _Newcapacity < _Newsize ? _Newsize : _Newcapacity;
const auto ret = _FixedCapacity < 32 ? ((_FixedCapacity + 3) & (-4)) :
_FixedCapacity < 256 ? ((_FixedCapacity + 15) & (-16)) :
((_FixedCapacity + 63) & (-64));
WJR_ASSUME(ret >= _Newsize);
return ret;
}
template<typename...Args>
WJR_CONSTEXPR20 void _M_realloc_insert_at_begin(Args&&...args) {
auto& al = getAllocator();
auto& _Mydata = getData();
const auto _Oldsize = _Mydata._Mysize;
WJR_ASSUME(_Mydata._Mysize == _Mydata._Mycapacity);
WJR_ASSUME(_Mydata._Myhead == _Mydata._Mytail);
const auto _Newsize = _Oldsize + 1;
const auto _Newcapacity = getGrowthCapacity(_Oldsize, _Newsize);
auto _Newptr = _Alty_traits::allocate(al, _Newcapacity);
wjr::construct_at(al, _Newptr, std::forward<Args>(args)...);
wjr::uninitialized_move(al, begin(), begin() + capacity(), _Newptr + 1);
tidy();
_Mydata._Myptr = _Newptr;
_Mydata._Myhead = 0;
_Mydata._Mytail = _Mydata._Mysize = _Newsize;
_Mydata._Mycapacity = _Newcapacity;
}
template<typename...Args>
WJR_CONSTEXPR20 void _M_realloc_insert_at_end(Args&&...args) {
auto& al = getAllocator();
auto& _Mydata = getData();
const auto _Oldsize = _Mydata._Mysize;
WJR_ASSUME(_Mydata._Mysize == _Mydata._Mycapacity);
WJR_ASSUME(_Mydata._Myhead == _Mydata._Mytail);
const auto _Newsize = _Oldsize + 1;
const auto _Newcapacity = getGrowthCapacity(_Oldsize, _Newsize);
auto _Newptr = _Alty_traits::allocate(al, _Newcapacity);
wjr::construct_at(al, _Newptr + _Oldsize, std::forward<Args>(args)...);
wjr::uninitialized_move(al, begin(), begin() + capacity(), _Newptr);
tidy();
_Mydata._Myptr = _Newptr;
_Mydata._Myhead = 0;
_Mydata._Mytail = _Mydata._Mysize = _Newsize;
_Mydata._Mycapacity = _Newcapacity;
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_append(iter _First, iter _Last, std::input_iterator_tag) {
for (; _First != _Last; ++_First) {
emplace_back(*_First);
}
}
template<typename iter>
WJR_CONSTEXPR20 void _M_range_append(iter _First, iter _Last, std::forward_iterator_tag) {
if (_First != _Last) {
auto& al = getAllocator();
auto& _Mydata = getData();
auto& _Myptr = _Mydata._Myptr;
auto& _Myhead = _Mydata._Myhead;
auto& _Mytail = _Mydata._Mytail;
auto& _Mysize = _Mydata._Mysize;
auto& _Mycapacity = _Mydata._Mycapacity;
const auto n = static_cast<size_type>(std::distance(_First, _Last));
const auto __rest = static_cast<size_type>(_Mycapacity - _Mysize);
if (__rest >= n) {
const auto __tail_rest = static_cast<size_type>(_Mycapacity - _Mytail);
if (__tail_rest >= n) {
wjr::uninitialized_copy(al, _First, _Last, _Myptr + _Mytail);
_Mytail += n;
}
else {
auto _Mid = _First;
std::advance(_Mid, __tail_rest);
wjr::uninitialized_copy_n(al,
_First,
__tail_rest,
_Myptr + _Mytail
);
wjr::uninitialized_copy(al,
_Mid,
_Last,
_Myptr
);
_Mytail = n - __tail_rest;
}
_Mysize += n;
}
else {
const auto _Newsize = _Mysize + n;
const auto _Newcapacity = getGrowthCapacity(capacity(), _Newsize);
auto _Newptr = _Alty_traits::allocate(al, _Newcapacity);
wjr::uninitialized_copy(al, _First, _Last, _Newptr + _Mysize);
wjr::uninitialized_move(al, begin(), end(), _Newptr);
tidy();
_Mydata._Myptr = _Newptr;
_Mydata._Myhead = 0;
_Mydata._Mytail = _Mydata._Mysize = _Newsize;
_Mydata._Mycapacity = _Newcapacity;
}
}
}
compressed_pair<_Alty, __data> _Myval;
};
_WJR_END
#ifndef __WJR_CIRCULAR_BUFFER_H
#error "This file should not be included directly. Include <wjr/circular_buffer.h> instead."
#endif // __WJR_CIRCULAR_BUFFER_H
_WJR_BEGIN
template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest) {
const auto ptr = _First.ptr();
const auto head_pos = _First.pos();
const auto tail_pos = _Last.pos();
const auto c = _First.capacity();
if (tail_pos < c) {
return wjr::uninitialized_copy(ptr + head_pos, ptr + tail_pos, _Dest);
}
return wjr::uninitialized_copy(
ptr,
ptr + tail_pos - c,
wjr::uninitialized_copy(
ptr + head_pos,
ptr + c,
_Dest
)
);
}
template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
return wjr::uninitialized_copy(_First.base(), _Last.base(), _Dest);
}
template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(Alloc& al,
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest) {
const auto ptr = _First.ptr();
const auto head_pos = _First.pos();
const auto tail_pos = _Last.pos();
const auto c = _First.capacity();
if (tail_pos < c) {
return wjr::uninitialized_copy(al, ptr + head_pos, ptr + tail_pos, _Dest);
}
return wjr::uninitialized_copy(al,
ptr,
ptr + tail_pos - c,
wjr::uninitialized_copy(al,
ptr + head_pos,
ptr + c,
_Dest
)
);
}
template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_copy(Alloc& al,
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
return wjr::uninitialized_copy(al, _First.base(), _Last.base(), _Dest);
}
template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest) {
const auto ptr = _First.ptr();
const auto head_pos = _First.pos();
const auto tail_pos = _Last.pos();
const auto c = _First.capacity();
if (tail_pos < c) {
return wjr::uninitialized_move(ptr + head_pos, ptr + tail_pos, _Dest);
}
return wjr::uninitialized_move(
ptr,
ptr + tail_pos - c,
wjr::uninitialized_move(
ptr + head_pos,
ptr + c,
_Dest
)
);
}
template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
return wjr::uninitialized_move(_First.base(), _Last.base(), _Dest);
}
template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(Alloc& al,
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest) {
const auto ptr = _First.ptr();
const auto head_pos = _First.pos();
const auto tail_pos = _Last.pos();
const auto c = _First.capacity();
if (tail_pos < c) {
return wjr::uninitialized_move(al, ptr + head_pos, ptr + tail_pos, _Dest);
}
return wjr::uninitialized_move(al,
ptr,
ptr + tail_pos - c,
wjr::uninitialized_move(al,
ptr + head_pos,
ptr + c,
_Dest
)
);
}
template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter do_uninitialized_move(Alloc& al,
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
return wjr::uninitialized_move(al, _First.base(), _Last.base(), _Dest);
}
_WJR_END

#endif // __WJR_CIRCULAR_BUFFER_H

#pragma once
#ifndef __WJR_CRTP_H
#define __WJR_CRTP_H
_WJR_BEGIN
class nocopyable {
protected:
nocopyable() {}
~nocopyable() {}
private:
nocopyable(const nocopyable&);
const nocopyable& operator=(const nocopyable&);
};
class nomoveable {
protected:
nomoveable() {}
~nomoveable() {}
nomoveable(const nomoveable&) = default;
nomoveable& operator=(const nomoveable&) = default;
private:
nomoveable(nomoveable&&);
nomoveable& operator=(nomoveable&&);
};
class nocopy_and_moveable : public nocopyable, public nomoveable { };
_WJR_END
#endif // __WJR_CRTP_H

_WJR_BEGIN
class thread_pool : public nocopy_and_moveable {
public:
using queue_type = circular_buffer<std::function<void()>,
aligned_allocator<std::function<void()>>>;
static size_t default_threads_size();
thread_pool(
unsigned int core_threads_size = default_threads_size());
~thread_pool();
std::mutex& get_mutex();
void pause();
void unpause();
void flush();
void shutdown();
queue_type shutdown(bool);
template<typename Func, typename...Args>
void push(Func&& func, Args&&...args);
template<typename Func, typename...Args, typename R = std::invoke_result_t<Func&&, Args&&...>>
WJR_NODISCARD std::future<R> submit(Func&& func, Args&&... args);
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
void append(iter _First, iter _Last);
unsigned int get_threads_size() const;
private:
void core_work();
void create_all_threads(unsigned int core_threads_size);
alignas(8) std::mutex m_task_mutex;
alignas(8) queue_type m_task_queue;
alignas(8) bool m_valid = true;
alignas(64) std::condition_variable m_task_cv;
alignas(8) bool m_pause = false;
alignas(8) std::atomic<size_t> m_real_tasks = 0;
alignas(8) wjr::vector<std::thread> m_core_threads;
alignas(64) bool m_flush = false;
alignas(8) std::condition_variable m_flush_cv;
};
template<typename Func, typename...Args>
void thread_pool::push(Func&& func, Args&&...args) {
std::function<void()> function = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
{
std::unique_lock task_lock(m_task_mutex);
m_task_queue.push_back(std::move(function));
#if defined(_WJR_EXCEPTION)
if (WJR_UNLIKELY(!m_valid)) {
throw std::runtime_error("can't put task into invalid thread_pool");
}
#endif // _WJR_EXCEPTION
}
m_real_tasks.fetch_add(1, std::memory_order_relaxed);
m_task_cv.notify_one();
}
template<typename Func, typename...Args, typename R>
WJR_NODISCARD std::future<R> thread_pool::submit(Func&& func, Args&&... args) {
std::function<R()> task_function = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
std::shared_ptr<std::promise<R>> task_promise = std::make_shared<std::promise<R>>();
push(
[func = std::move(task_function), task_promise]() {
#if defined(_WJR_EXCEPTION)
try {
#endif // _WJR_EXCEPTION
if constexpr (std::is_void_v<R>) {
std::invoke(func);
task_promise->set_value();
}
else {
task_promise->set_value(std::invoke(func));
}
#if defined(_WJR_EXCEPTION)
}
catch (...) {
try {
task_promise->set_exception(std::current_exception());
}
catch (...) {
}
}
#endif // _WJR_EXCEPTION
});
return task_promise->get_future();
}
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int>>
void thread_pool::append(iter _First, iter _Last) {
std::unique_lock task_lock(m_task_mutex);
const auto _Oldsize = m_task_queue.size();
m_task_queue.append(_First, _Last);
const auto _Newsize = m_task_queue.size();
task_lock.unlock();
size_t n;
if constexpr (is_random_iterator_v<iter>) {
n = std::distance(_First, _Last);
}
else {
n = _Newsize - _Oldsize;
}
if (n == 0) {
}
else {
m_real_tasks.fetch_add(n, std::memory_order_relaxed);
if (n > get_threads_size()) {
m_task_cv.notify_all();
}
else {
for (; n != 0; --n) {
m_task_cv.notify_one();
}
}
}
}
_WJR_END
#endif // __WJR_NETWORK_THREAD_POOL_H
_WJR_BEGIN
size_t thread_pool::default_threads_size() {
return std::max(static_cast<size_t>(1), static_cast<size_t>(std::thread::hardware_concurrency()));
}
thread_pool::thread_pool(
unsigned int core_threads_size) {
create_all_threads(core_threads_size);
}
thread_pool::~thread_pool() {
if (m_valid) {
shutdown();
}
}
std::mutex& thread_pool::get_mutex() {
return m_task_mutex;
}
void thread_pool::pause() {
std::unique_lock task_lock(m_task_mutex);
m_pause = true;
}
void thread_pool::unpause() {
std::unique_lock task_lock(m_task_mutex);
m_pause = false;
}
void thread_pool::flush() {
std::unique_lock task_lock(m_task_mutex);
if (m_flush) return;
m_flush = true;
m_flush_cv.wait(task_lock, [this] {
return (m_real_tasks.load(std::memory_order_relaxed)
== (m_pause ? m_task_queue.size() : 0));
});
m_flush = false;
}
void thread_pool::shutdown() {
#if defined(_WJR_EXCEPTION)
if (!m_valid) {
throw std::runtime_error("can't shutdown thread pool more than once");
}
#endif // _WJR_EXCEPTION
flush();
{
std::unique_lock task_lock(m_task_mutex);
m_valid = false;
}
m_task_cv.notify_all();
for (auto& i : m_core_threads) i.join();
}
typename thread_pool::queue_type thread_pool::shutdown(bool) {
shutdown();
std::unique_lock task_lock(m_task_mutex);
return std::move(m_task_queue);
}
unsigned int thread_pool::get_threads_size() const {
return m_core_threads.size();
}
void thread_pool::core_work() {
for (;;) {
std::function<void()> task;
{
using namespace wjr::enum_ops;
std::unique_lock task_lock(m_task_mutex);
m_task_cv.wait(task_lock, [this] {
return !m_task_queue.empty() || !m_valid;
});
if (WJR_LIKELY(m_valid)) {
if (!m_pause) {
task = std::move(m_task_queue.front());
m_task_queue.pop_front();
task_lock.unlock();
task();
task_lock.lock();
m_real_tasks.fetch_sub(1, std::memory_order_relaxed);
if (m_flush) {
task_lock.unlock();
m_flush_cv.notify_one();
}
}
continue;
}
break;
}
}
}
void thread_pool::create_all_threads(
unsigned int core_threads_size) {
#if defined(_WJR_EXCEPTION)
if (!core_threads_size) {
throw std::invalid_argument("!core_threads_size");
}
#endif // _WJR_EXCEPTION
m_core_threads.reserve(core_threads_size);
for (unsigned int i = 0; i < core_threads_size; ++i) {
m_core_threads.emplace_back(&thread_pool::core_work, this);
}
}
_WJR_END


#pragma once
#ifndef __WJR_PARSE_H
#define __WJR_PARSE_H
#pragma once
#ifndef __WJR_PARSE_LL1_H
#define __WJR_PARSE_LL1_H
#define _WJR_PARSE_LL1_BEGIN _WJR_PARSE_BEGIN namespace LL1{
#define _WJR_PARSE_LL1_END } _WJR_PARSE_END
#pragma once
#ifndef __WJR_PARSE_GENERIC_H
#define __WJR_PARSE_GENERIC_H
#pragma once
#ifndef __WJR_SWITCH_H
#define __WJR_SWITCH_H
#pragma once
#ifndef __WJR_TP_LIST_H
#define __WJR_TP_LIST_H
#include <type_traits>
#include <variant>
_WJR_BEGIN
struct tp_null {};
template<typename...Args>
struct tp_list {};
template<typename T, T I>
using tp_c = std::integral_constant<T, I>;
template<typename T, T...I>
struct tp_list_c {
using type = tp_list<tp_c<T, I>...>;
};
template<typename T, T... I>
using tp_list_c_t = typename tp_list_c<T, I...>::type;
template<typename T>
struct tp_is_list : std::false_type {};
template<typename...Args>
struct tp_is_list<tp_list<Args...>> : std::true_type {};
template<typename T>
inline constexpr bool tp_is_list_v = tp_is_list<T>::value;
template<typename T>
struct tp_is_container : std::false_type {};
template<
template<typename...>typename C,
typename...Args>
struct tp_is_container<C<Args...>> : std::true_type {};
template<typename T>
inline constexpr bool tp_is_container_v = tp_is_container<T>::value;
template<typename T>
struct tp_size;
template<template<typename...>typename C, typename...Args>
struct tp_size<C<Args...>> {
constexpr static size_t value = sizeof...(Args);
};
template<typename T>
inline constexpr size_t tp_size_v = tp_size<T>::value;
template<size_t idx>
using tp_size_t = tp_c<size_t, idx>;
template<char ch>
using tp_char_t = tp_c<char, ch>;
template<typename T, typename U>
struct tp_equal_c : std::bool_constant<(T::value == U::value)> {};
template<typename T, typename U>
struct tp_not_equal_c : std::bool_constant<(T::value != U::value)> {};
template<typename T, typename U>
struct tp_less_c : std::bool_constant<(T::value < U::value)>{};
template<typename T, typename U>
struct tp_less_equal_c : std::bool_constant < (T::value <= U::value)> {};
template<typename T, typename U>
struct tp_greater_c : std::bool_constant < (T::value > U::value)> {};
template<typename T, typename U>
struct tp_greater_equalv : std::bool_constant < (T::value >= U::value)>{};
template<typename T>
struct tp_is_fn : std::false_type {};
template<typename T>
inline constexpr bool tp_is_fn_v = tp_is_fn<T>::value;
template<typename _Enable, template<typename...>typename F, typename...Args>
struct __tp_valid_helper : std::false_type {};
template<template<typename...>typename F, typename...Args>
struct __tp_valid_helper<std::void_t<F<Args...>>, F, Args...> : std::true_type {};
template<template<typename...>typename F, typename...Args>
struct tp_valid : __tp_valid_helper<void, F, Args...> {};
template<template<typename...>typename F, typename...Args>
inline constexpr bool tp_valid_v = tp_valid<F, Args...>::value;
template<typename F, typename...Args>
inline constexpr bool tp_valid_f = tp_valid_v<F::template fn, Args...>;
template<template<typename...>typename F, typename...Args>
struct __tp_defer_helper {
using type = F<Args...>;
};
template<template<typename...>typename F, typename...Args>
struct tp_defer {
using type = std::enable_if_t<tp_valid_v<F, Args...>, typename __tp_defer_helper<F, Args...>::type>;
};
template<template<typename...>typename F, typename...Args>
using tp_defer_t = typename tp_defer<F, Args...>::type;
template<typename F, typename...Args>
using tp_defer_f = tp_defer_t<F::template fn, Args...>;
template<typename T>
struct tp_identity {
using type = T;
};
template<typename T>
using tp_identity_t = typename tp_identity<T>::type;
template<template<typename...>typename F1, template<typename...>typename F2>
struct tp_bind_fn {
template<typename...Args>
using fn = tp_defer_t<F1, tp_defer_t<F2, Args...>>;
};
template<template<typename...>typename F>
struct tp_make_fn {
template<typename...Args>
using fn = tp_defer_t<F, Args...>;
};
template<template<typename...>typename F>
struct tp_not_fn {
template<typename...Args>
using fn = typename tp_bind_fn<std::negation, F>::template fn<Args...>;
};
template<typename...Args>
struct tp_always_true : std::true_type {};
template<typename...Args>
struct tp_always_false : std::false_type {};
template<typename T>
struct tp_is_empty : std::bool_constant<tp_size_v<T> == 0> {};
template<typename T>
inline constexpr bool tp_is_empty_v = tp_is_empty<T>::value;
template<typename T, typename U>
struct tp_assign;
template<typename...Args1, template<typename...>typename T1,
typename...Args2, template<typename...>typename T2>
struct tp_assign<T1<Args1...>, T2<Args2...>> {
using type = T1<Args2...>;
};
template<typename T, typename U>
using tp_assign_t = typename tp_assign<T, U>::type;
template<typename T>
struct tp_clear;
template<template<typename...>typename T, typename...Args>
struct tp_clear<T<Args...>> {
using type = T<>;
};
template<typename T>
using tp_clear_t = typename tp_clear<T>::type;
template<typename T, typename...Args>
struct tp_push_front;
template<template<typename...>typename C, typename...Args1, typename...Args2>
struct tp_push_front<C<Args1...>, Args2...> {
using type = C<Args2..., Args1...>;
};
template<typename T, typename...Args>
using tp_push_front_t = typename tp_push_front<T, Args...>::type;
template<typename T, typename...Args>
struct tp_push_back;
template<template<typename...>typename C, typename...Args1, typename...Args2>
struct tp_push_back<C<Args1...>, Args2...> {
using type = C<Args1..., Args2...>;
};
template<typename T, typename...Args>
using tp_push_back_t = typename tp_push_back<T, Args...>::type;
template<typename _Enable, size_t I, size_t N, typename...Args>
struct __tp_cut_helper;
template<size_t I, size_t N, typename T, typename...Args>
struct __tp_cut_helper<std::enable_if_t<N != 0, void>, I, N, T, Args...> {
using type = typename __tp_cut_helper<void, I - 1, N, Args...>::type;
};
template<size_t I, size_t N, typename T, typename...Args>
struct __tp_cut_helper<std::enable_if_t<N == 0, void>, I, N, T, Args...> {
using type = tp_list<>;
};
template<size_t N, typename...Args2>
struct __tp_cut_helper2;
template<size_t N, typename T, typename...Args>
struct __tp_cut_helper2<N, T, Args...> {
using type = tp_push_front_t<typename __tp_cut_helper2<N - 1, Args...>::type, T>;
};
template<typename...Args>
struct __tp_cut_helper2<0, Args...> {
using type = tp_list<>;
};
template<typename T, typename...Args>
struct __tp_cut_helper2<0, T, Args...> {
using type = tp_list<>;
};
template<size_t N, typename...Args>
struct __tp_cut_helper<std::enable_if_t<N != 0>, 0, N, Args...> {
using type = typename __tp_cut_helper2<N, Args...>::type;
};
template<size_t N, typename T, typename...Args>
struct __tp_cut_helper<std::enable_if_t<N != 0>, 0, N, T, Args...> {
using type = typename __tp_cut_helper2<N, T, Args...>::type;
};
template<typename T, template<typename...>typename U>
struct tp_rename;
template<template<typename...>typename C, typename...Args, template<typename...>typename U>
struct tp_rename<C<Args...>, U> {
using type = U<Args...>;
};
template<typename T, template<typename...>typename U>
using tp_rename_t = typename tp_rename<T, U>::type;
template<typename T, size_t I, size_t N>
struct tp_cut;
template<template<typename...>typename C, typename...Args, size_t I, size_t N>
struct tp_cut<C<Args...>, I, N> {
static_assert(N <= sizeof...(Args) && I <= (sizeof...(Args) - N), "tp_cut: invalid index");
using type = tp_rename_t<typename __tp_cut_helper<void, I, N, Args...>::type, C>;
};
template<typename T, size_t I, size_t N>
using tp_cut_t = typename tp_cut<T, I, N>::type;
template<typename T>
struct tp_pop_front : tp_cut<T, 1, tp_size_v<T> - 1> {};
template<typename T>
using tp_pop_front_t = typename tp_pop_front<T>::type;
template<typename T>
struct tp_pop_back : tp_cut<T, 0, tp_size_v<T> -1> {};
template<typename T>
using tp_pop_back_t = typename tp_pop_back<T>::type;
template<size_t index, typename...Args>
struct __tp_at_helper;
template<size_t index, typename T, typename...Args>
struct __tp_at_helper<index, T, Args...> {
using type = typename __tp_at_helper<index - 1, Args...>::type;
};
template<typename T, typename...Args>
struct __tp_at_helper<0, T, Args...> {
using type = T;
};
template<typename T, size_t index>
struct tp_at;
template<template<typename...Args>typename C, typename...Args, size_t index>
struct tp_at<C<Args...>, index> {
static_assert(index < sizeof...(Args), "tp_at: invalid index");
using type = typename __tp_at_helper<index, Args...>::type;
};
template<typename T, size_t index>
using tp_at_t = typename tp_at<T, index>::type;
template<typename T>
struct tp_front {
using type = tp_at_t<T, 0>;
};
template<typename T>
using tp_front_t = typename tp_front<T>::type;
template<typename T>
struct tp_back {
using type = tp_at_t<T, tp_size_v<T> -1>;
};
template<typename T>
using tp_back_t = typename tp_back<T>::type;
template<typename T, size_t idx>
struct tp_prefix {
using type = tp_cut_t<T, 0, idx>;
};
template<typename T, size_t idx>
using tp_prefix_t = typename tp_prefix<T, idx>::type;
template<typename T, size_t idx>
struct tp_suffix {
using type = tp_cut_t<T, tp_size_v<T> - idx, idx>;
};
template<typename T, size_t idx>
using tp_suffix_t = typename tp_suffix<T, idx>::type;
template<typename T, size_t idx>
struct tp_remove_prefix {
using type = tp_suffix_t<T, tp_size_v<T> - idx>;
};
template<typename T, size_t idx>
using tp_remove_prefix_t = typename tp_remove_prefix<T, idx>::type;
template<typename T, size_t idx>
struct tp_remove_suffix {
using type = tp_prefix_t<T, tp_size_v<T> - idx>;
};
template<typename T, size_t idx>
using tp_remove_suffix_t = typename tp_remove_suffix<T, idx>::type;
template<typename...Args>
struct tp_concat;
template<typename T>
struct tp_concat<T> {
using type = T;
};
template<
template<typename...>typename C1, typename...Args1,
template<typename...>typename C2, typename...Args2>
struct tp_concat<C1<Args1...>, C2<Args2...>> {
using type = C1<Args1..., Args2...>;
};
template<
typename T,
typename U,
typename...Args3>
struct tp_concat<T, U, Args3...> {
using type = typename tp_concat<typename tp_concat<T, U>::type, Args3...>::type;
};
template<typename...Args>
using tp_concat_t = typename tp_concat<Args...>::type;
template<typename T, size_t idx, typename U>
struct tp_replace_at {
using type = tp_concat_t<tp_push_back_t<tp_cut_t<T, 0, idx>, U>, tp_cut_t<T, idx + 1, tp_size_v<T> -idx - 1>>;
};
template<typename T, typename U>
struct tp_replace_at<T, 0, U> {
using type = tp_push_front_t<tp_pop_front_t<T>, U>;
};
template<typename T, size_t idx, typename U>
using tp_replace_at_t = typename tp_replace_at<T, idx, U>::type;
template<typename T, typename U>
struct tp_replace_front_at {
using type = tp_replace_at_t<T, 0, U>;
};
template<typename T, typename U>
using tp_replace_front_at_t = typename tp_replace_front_at<T, U>::type;
template<typename T, typename U>
struct tp_replace_back_at {
using type = tp_replace_at_t<T, tp_size_v<T> -1, U>;
};
template<typename T, typename U>
using tp_replace_back_at_t = typename tp_replace_back_at<T, U>::type;
template<typename V, typename T, typename...Args>
struct tp_conditional {
using type = std::conditional_t<V::value, T, typename tp_conditional<Args...>::type>;
};
template<typename V, typename T1, typename T2>
struct tp_conditional<V, T1, T2> {
using type = std::conditional_t<V::value, T1, T2>;
};
template<typename V, typename T, typename...Args>
using tp_conditional_t = typename tp_conditional<V, T, Args...>::type;
template<size_t idx>
struct tp_arg;
template<template<typename...>typename F, typename...Args>
struct tp_bind;
template<template<typename...>typename F, typename...Args>
struct tp_bind_front;
template<template<typename...>typename F, typename...Args>
struct tp_bind_back;
template<size_t idx>
struct tp_is_fn<tp_arg<idx>> : std::true_type {};
template<template<typename...>typename F, typename...Args>
struct tp_is_fn<tp_bind<F, Args...>> : std::true_type {};
template<template<typename...>typename F, typename...Args>
struct tp_is_fn<tp_bind_front<F, Args...>> : std::true_type {};
template<template<typename...>typename F, typename...Args>
struct tp_is_fn<tp_bind_back<F, Args...>> : std::true_type {};
template<size_t idx>
struct tp_arg {
template<typename...Args>
using fn = tp_at_t<tp_list<Args...>, idx>;
};
template<template<typename...>typename F,
typename T>
struct tp_apply {
using type = tp_rename_t<T, F>;
};
template<template<typename...>typename F,
typename T>
using tp_apply_t = typename tp_apply<F, T>::type;
template<typename F, typename T>
using tp_apply_f = tp_apply_t<F::template fn, T>;
template<typename _Enable, typename T, typename...Args>
struct __tp_bind_helper {
using type = T;
};
template<typename F, typename...Args>
struct __tp_bind_helper<std::enable_if_t<tp_is_fn_v<F>, void>, F, Args...> {
using type = typename F::template fn<Args...>;
};
template<template<typename...>typename F, typename...Args>
struct tp_bind {
template<typename...Args2>
using fn = F<typename __tp_bind_helper<void, Args, Args2...>::type...>;
};
template<typename F, typename...Args>
using tp_bind_f = tp_bind<F::template fn, Args...>;
template<template<typename...>typename F, typename...Args>
struct tp_bind_front {
template<typename...Args2>
using fn = tp_defer_t<F, Args..., Args2...>;
};
template<typename F, typename...Args>
using tp_bind_front_f = tp_bind_front<F::template fn, Args...>;
template<template<typename...>typename F, typename...Args>
struct tp_bind_back {
template<typename...Args2>
using fn = tp_defer_t<F, Args2..., Args...>;
};
template<typename F, typename...Args>
using tp_bind_back_f = tp_bind_back<F::template fn, Args...>;
template<typename T, template<typename...>typename F>
struct tp_transform;
template<template<typename...>typename C, typename...Args, template<typename...>typename F>
struct tp_transform<C<Args...>, F> {
using type = C<F<Args>...>;
};
template<typename T, template<typename...>typename F>
using tp_transform_t = typename tp_transform<T, F>::type;
template<typename T, typename F>
using tp_transform_f = typename tp_transform<T, F::template fn>::type;
template<template<typename...>typename C, typename...Args>
struct tp_zip;
template<template<typename...>typename C, typename T>
struct __tp_zip_helper;
template<template<typename...>typename C, size_t...idx>
struct __tp_zip_helper<C, std::index_sequence<idx...>> {
template<size_t I, typename...Args>
using __type = C<tp_at_t<Args, I>...>;
template<typename...Args>
using type = tp_list<__type<idx, Args...>...>;
};
template<template<typename...>typename C>
struct tp_zip<C> {
using type = tp_list<>;
};
template<template<typename...>typename C, typename T>
struct tp_zip<C, T> {
using type = typename __tp_zip_helper<C, std::make_index_sequence<tp_size_v<T>>>::template type<T>;
};
template<template<typename...>typename C, typename T, typename...Args>
struct tp_zip<C, T, Args...> {
constexpr static size_t size = tp_size_v<T>;
static_assert(((size == tp_size_v<Args>) && ...),
"tp_zip arguments must have same size, \
you can make all arguments have same size by tp_");
using type = typename __tp_zip_helper<C, std::make_index_sequence<tp_size_v<T>>>::template type<T, Args...>;
};
template<template<typename...>typename C, typename...Args>
using tp_zip_t = typename tp_zip<C, Args...>::type;
template<typename...Args>
struct __tp_max_size_helper;
template<typename T>
struct __tp_max_size_helper<T> {
constexpr static size_t value = tp_size_v<T>;
};
template<typename T, typename...Args>
struct __tp_max_size_helper<T, Args...> {
constexpr static size_t value = std::max(tp_size_v<T>, __tp_max_size_helper<Args...>::value);
};
template<typename T, typename...Args>
struct tp_max_size {
constexpr static size_t value = __tp_max_size_helper<T, Args...>::value;
};
template<typename T, typename...Args>
inline constexpr size_t tp_max_size_v = tp_max_size<T, Args...>::value;
template<typename T>
struct tp_wrap {
using type = tp_list<T>;
};
template<typename T>
using tp_wrap_t = typename tp_wrap<T>::type;
template<typename T>
struct tp_unwrap {
static_assert(tp_size_v<T> == 1, "only container that size = 1 can use unwrap");
};
template<template<typename...>typename C, typename T>
struct tp_unwrap<C<T>> {
using type = T;
};
template<typename T>
using tp_unwrap_t = typename tp_unwrap<T>::type;
template<typename T, template<typename...>typename P, typename U>
struct tp_replace_if;
template<template<typename...>typename C, typename...Args, template<typename...>typename P, typename U>
struct tp_replace_if<C<Args...>, P, U> {
using type = C<tp_conditional_t<P<Args>, U, Args>...>;
};
template<typename T, template<typename...>typename P, typename U>
using tp_replace_if_t = typename tp_replace_if<T, P, U>::type;
template<typename T, typename P, typename U>
using tp_replace_if_f = tp_replace_if_t<T, P::template fn, U>;
template<typename T, typename U>
struct tp_replace_if_true {
using type = tp_replace_if_t<T, tp_identity_t, U>;
};
template<typename T, typename U>
using tp_replace_if_true_t = typename tp_replace_if_true<T, U>::type;
template<typename T, typename U>
struct tp_replace_if_false {
using type = tp_replace_if_f<T, tp_not_fn<tp_identity_t>, U>;
};
template<typename T, typename U>
using tp_replace_if_false_t = typename tp_replace_if_false<T, U>::type;
template<typename T, typename O, typename N>
struct tp_replace {
using type = tp_replace_if_f<T, tp_bind_front<std::is_same, O>, N>;
};
template<typename T, typename O, typename N>
using tp_replace_t = typename tp_replace<T, O, N>::type;
template<typename T, typename U>
struct tp_fill {
using type = tp_replace_if_t<T, tp_always_true, U>;
};
template<typename T, typename U>
using tp_fill_t = typename tp_fill<T, U>::type;
template<typename T, template<typename...>typename P>
struct tp_count_if;
template<template<typename...>typename C, template<typename...>typename P>
struct tp_count_if<C<>, P> {
static constexpr size_t value = 0;
};
template<template<typename...>typename C, typename...Args, template<typename...>typename P>
struct tp_count_if<C<Args...>, P> {
static constexpr size_t value = (P<Args>::value + ...);
};
template<typename T, template<typename...>typename P>
constexpr size_t tp_count_if_v = tp_count_if<T, P>::value;
template<typename T, typename P>
constexpr size_t tp_count_if_f_v = tp_count_if_v<T, P::template fn>;
template<typename T, typename V>
struct tp_count {
static constexpr size_t value = tp_count_if_f_v<T, tp_bind_front<std::is_same, V>>;
};
template<typename T, typename V>
constexpr size_t tp_count_v = tp_count<T, V>::value;
template<typename T, typename V>
struct tp_contains {
static constexpr bool value = tp_count_v<T, V> != 0;
};
template<typename T, typename V>
constexpr bool tp_contains_v = tp_contains<T, V>::value;
template<typename T, template<typename...>typename P>
struct tp_remove_if;
template<template<typename...>typename C, typename...Args, template<typename...>typename P>
struct tp_remove_if<C<Args...>, P> {
using type = tp_concat_t<C<>, tp_conditional_t<P<Args>, C<>, C<Args>>...>;
};
template<typename T, template<typename...>typename P>
using tp_remove_if_t = typename tp_remove_if<T, P>::type;
template<typename T, typename P>
using tp_remove_if_f = tp_remove_if_t<T, P::template fn>;
template<typename T, typename V>
struct tp_remove {
using type = tp_remove_if_f<T, tp_bind_front<std::is_same, V>>;
};
template<typename T, typename V>
using tp_remove_t = typename tp_remove<T, V>::type;
template<typename T, template<typename...>typename P>
struct tp_filter {
using type = tp_remove_if_f<T, tp_not_fn<P>>;
};
template<typename T, template<typename...>typename P>
using tp_filter_t = typename tp_filter<T, P>::type;
template<typename T, typename P>
using tp_filter_f = tp_filter_t<T, P::template fn>;
template<typename T, typename U>
struct tp_equal;
template<typename _Enable, typename T, typename U>
struct __tp_equal_helper : std::false_type{};
template<template<typename...>typename C, typename...Args, template<typename...>typename D, typename...Args2>
struct __tp_equal_helper<std::enable_if_t<sizeof...(Args) == sizeof...(Args2), void>, C<Args...>, D<Args2...>>
: std::conjunction<std::is_same<Args, Args2>...> {};
template<typename T, typename U>
struct tp_equal : __tp_equal_helper<void, T, U> {};
template<typename T, typename U>
inline constexpr bool tp_equal_v = tp_equal<T, U>::value;
template<typename T, size_t N>
struct tp_repeat {
using type = tp_concat_t<T, typename tp_repeat<T, N - 1>::type>;
};
template<typename T>
struct tp_repeat<T, 0> {
using type = tp_clear_t<T>;
};
template<typename C, size_t N>
using tp_repeat_t = typename tp_repeat<C, N>::type;
template<typename _Enable, typename C, size_t N, typename V>
struct __tp_resize_helper {
using type = tp_cut_t<C, 0, N>;
};
template<typename C, size_t N, typename V>
struct __tp_resize_helper<std::enable_if_t<N >= tp_size_v<C>, void>, C, N, V> {
using type = tp_concat_t<C, tp_repeat_t<V, N - tp_size_v<C>>>;
};
template<typename C, size_t N, typename V>
struct tp_resize {
using tyep = typename __tp_resize_helper<void, C, N, V>::type;
};
template<typename C, size_t N, typename V>
using tp_resize_t = typename tp_resize<C, N, V>::type;
template<template<typename...>typename C, typename...Args>
struct tp_product;
template<typename _Enable, template<typename...>typename C, typename...Args>
struct __tp_product_helper{
using type = tp_list<>;
};
template<typename _Enable, template<typename...>typename C, typename T>
struct __tp_product_helper<_Enable, C, T> {
using type = tp_list<tp_rename_t<T, C>>;
};
template<
template<typename...>typename C,
typename T,
template<typename...>typename C1,
typename...Args1,
typename...Args>
struct __tp_product_helper<std::enable_if_t<sizeof...(Args1) != 0, void>, C, T, C1<Args1...>, Args...> {
using type = tp_concat_t<typename __tp_product_helper<void, C, tp_push_back_t<T, Args1>, Args...>::type...>;
};
template<template<typename...>typename C, typename...Args>
struct tp_product {
using type = typename __tp_product_helper<void, C, tp_list<>, Args...>::type;
};
template<template<typename...>typename C, typename...Args>
using tp_product_t = typename tp_product<C, Args...>::type;
template<typename C, size_t I, typename...Args>
struct tp_insert {
static_assert(I <= tp_size_v<C>, "tp insert index out of range");
using type = tp_concat_t<tp_push_back_t<tp_prefix_t<C, I>, Args...>, tp_suffix_t<C, tp_size_v<C> -I>>;
};
template<typename C, size_t I, typename...Args>
using tp_insert_t = typename tp_insert<C, I, Args...>::type;
template<typename C, size_t I, size_t N>
struct tp_erase {
static_assert(N <= tp_size_v<C> && I <= tp_size_v<C> -N, "tp erase index out of range");
using type = tp_concat_t<tp_prefix_t<C, I>, tp_suffix_t<C, tp_size_v<C> -I - N>>;
};
template<typename C, size_t I, size_t N>
using tp_erase_t = typename tp_erase<C, I, N>::type;
template<typename C>
struct tp_reverse;
template<template<typename...>typename C>
struct tp_reverse<C<>> {
using type = C<>;
};
template<template<typename...>typename C, typename T, typename...Args>
struct tp_reverse<C<T, Args...>> {
using type = tp_push_back_t<typename tp_reverse<C<Args...>>::type, T>;
};
template<typename C>
using tp_reverse_t = typename tp_reverse<C>::type;
template<typename _Enable, size_t idx, typename C, template<typename...>typename P>
struct __tp_find_if_helper;
template<typename _Enable, size_t idx,
template<typename...>typename C,
typename T,
typename...Args,
template<typename...>typename P>
struct __tp_find_if_helper<_Enable, idx, C<T, Args...>, P> {
constexpr static size_t value = __tp_find_if_helper<void, idx + 1, C<Args...>, P>::value;
};
template<typename _Enable, size_t idx,
template<typename...>typename C,
template<typename...>typename P>
struct __tp_find_if_helper<_Enable, idx, C<>, P> {
constexpr static size_t value = -1;
};
template<size_t idx,
template<typename...>typename C,
typename T,
typename...Args,
template<typename...>typename P>
struct __tp_find_if_helper<std::enable_if_t<P<T>::value, void>, idx, C<T, Args...>, P> {
constexpr static size_t value = idx;
};
template<typename C, template<typename...>typename P>
struct tp_find_if {
constexpr static size_t value = __tp_find_if_helper<void, 0, C, P>::value;
};
template<typename C, template<typename...>typename P>
inline constexpr size_t tp_find_if_v = tp_find_if<C, P>::value;
template<typename C, typename P>
inline constexpr size_t tp_find_if_f = tp_find_if<C, P::template fn>::value;
template<typename C, template<typename...>typename P>
struct tp_find_if_not {
constexpr static size_t value = tp_find_if_f<C, tp_not_fn<P>>;
};
template<typename C, template<typename...>typename P>
inline constexpr size_t tp_find_if_not_v = tp_find_if_not<C, P>::value;
template<typename C, typename P>
inline constexpr size_t tp_find_if_not_f = tp_find_if_not<C, P::template fn>::value;
template<typename C, typename V>
struct tp_find{
constexpr static size_t value = tp_find_if_f<C, tp_bind_front<std::is_same, V>>;
};
template<typename C, typename V>
inline constexpr size_t tp_find_v = tp_find<C, V>::value;
template<typename C, typename E, template<typename...>typename F>
struct tp_left_fold;
template<
template<typename...>typename C,
typename E,
template<typename...>typename F>
struct tp_left_fold<C<>, E, F> {
using type = E;
};
template<
template<typename...>typename C,
typename T,
typename...Args,
typename E,
template<typename...>typename F>
struct tp_left_fold<C<T, Args...>, E, F> {
using type = typename tp_left_fold<C<Args...>, F<E, T>, F>::type;
};
template<typename C, typename E, template<typename...>typename F>
using tp_left_fold_t = typename tp_left_fold<C, E, F>::type;
template<typename C, typename E, typename F>
using tp_left_fold_f = typename tp_left_fold<C, E, F::template fn>::type;
template<typename C, typename E, template<typename...>typename F>
struct tp_right_fold;
template<
template<typename...>typename C,
typename E,
template<typename...>typename F>
struct tp_right_fold<C<>, E, F> {
using type = E;
};
template<
template<typename...>typename C,
typename T,
typename...Args,
typename E,
template<typename...>typename F>
struct tp_right_fold<C<T, Args...>, E, F> {
using next_type = typename tp_right_fold<C<Args...>, E, F>::type;
using type = F<T, next_type>;
};
template<typename C, typename E, template<typename...>typename F>
using tp_right_fold_t = typename tp_right_fold<C, E, F>::type;
template<typename C, typename E, typename F>
using tp_right_fold_f = typename tp_right_fold<C, E, F::template fn>::type;
template<typename C, template<typename...>typename P>
struct tp_unique_if {
using type = tp_left_fold_f<C, tp_clear_t<C>,
tp_bind<tp_conditional_t, tp_bind_front<P>, tp_arg<0>,
tp_bind_front<tp_push_back_t>>>;
};
template<typename C, template<typename...>typename P>
using tp_unique_if_t = typename tp_unique_if<C, P>::type;
template<typename C, typename P>
using tp_unique_if_f = typename tp_unique_if<C, P::template fn>::type;
template<typename C>
struct tp_unique{
using type = tp_unique_if_t<C, tp_contains>;
};
template<typename C>
using tp_unique_t = typename tp_unique<C>::type;
template<typename _Enable, typename C, typename C1, typename C2, template<typename...>typename P>
struct __tp_merge_helper;
template<
typename _Enable,
template<typename...>typename C,
typename...Args,
template<typename...>typename C1,
template<typename...>typename C2,
typename...Args2,
template<typename...>typename P>
struct __tp_merge_helper<_Enable, C<Args...>, C1<>, C2<Args2...>, P> {
using type = tp_list<Args..., Args2...>;
};
template<
typename _Enable,
template<typename...>typename C,
typename...Args,
template<typename...>typename C1,
typename...Args1,
template<typename...>typename C2,
template<typename...>typename P>
struct __tp_merge_helper<_Enable, C<Args...>, C1<Args1...>, C2<>, P> {
using type = tp_list<Args..., Args1...>;
};
template<
typename _Enable,
template<typename...>typename C,
typename...Args,
template<typename...>typename C1,
template<typename...>typename C2,
template<typename...>typename P>
struct __tp_merge_helper<_Enable, C<Args...>, C1<>, C2<>, P> {
using type = tp_list<Args...>;
};
template<
template<typename...>typename C,
typename...Args,
template<typename...>typename C1,
typename T1, typename...Args1,
template<typename...>typename C2,
typename T2, typename...Args2,
template<typename...>typename P>
struct __tp_merge_helper<std::enable_if_t<P<T1, T2>::value, void>, C<Args...>, C1<T1, Args1...>, C2<T2, Args2...>, P> {
using type = typename __tp_merge_helper<void, C<Args..., T1>, C1<Args1...>, C2<T2, Args2...>, P>::type;
};
template<
template<typename...>typename C,
typename...Args,
template<typename...>typename C1,
typename T1, typename...Args1,
template<typename...>typename C2,
typename T2, typename...Args2,
template<typename...>typename P>
struct __tp_merge_helper<std::enable_if_t<!P<T1, T2>::value, void>, C<Args...>, C1<T1, Args1...>, C2<T2, Args2...>, P> {
using type = typename __tp_merge_helper<void, C<Args..., T2>, C1<T1, Args1...>, C2<Args2...>, P>::type;
};
template<typename C1, typename C2, template<typename...>typename P>
struct tp_merge {
using type = typename __tp_merge_helper<void, tp_list<>, C1, C2, P>::type;
};
template<typename C1, typename C2, template<typename...>typename P>
using tp_merge_t = typename tp_merge<C1, C2, P>::type;
template<typename C1, typename C2, typename P>
using tp_merge_f = typename tp_merge<C1, C2, P::template fn>::type;
template<typename C, template<typename...>typename P>
struct tp_sort;
template<
typename C,
template<typename...>typename P>
struct __tp_sort_helper;
template<
template<typename...>typename C,
typename...Args,
template<typename...>typename P>
struct __tp_sort_helper<C<Args...>, P> {
using _Container = C<Args...>;
constexpr static size_t size = tp_size_v<_Container>;
constexpr static size_t mid = size / 2;
using type1 = typename __tp_sort_helper<tp_prefix_t<_Container, mid>, P>::type;
using type2 = typename __tp_sort_helper<tp_suffix_t<_Container, size - mid>, P>::type;
using type = tp_merge_t<type1, type2, P>;
};
template<
template<typename...>typename C,
typename T,
template<typename...>typename P>
struct __tp_sort_helper<C<T>, P> {
using type = C<T>;
};
template<
template<typename...>typename C,
template<typename...>typename P>
struct __tp_sort_helper<C<>, P> {
using type = C<>;
};
template<
template<typename...>typename C,
typename...Args,
template<typename...>typename P>
struct tp_sort<C<Args...>, P> {
using type = tp_rename_t<typename __tp_sort_helper<C<Args...>, P>::type, C>;
};
template<typename C, template<typename...>typename P>
using tp_sort_t = typename tp_sort<C, P>::type;
template<typename C, typename P>
using tp_sort_f = typename tp_sort<C, P::template fn>::type;
template<size_t I, typename T>
struct __tp_iota_helper;
template<size_t I, size_t...Idx>
struct __tp_iota_helper<I, std::index_sequence<Idx...>> {
using type = tp_list<tp_size_t<I + Idx>...>;
};
template<size_t I, size_t N>
struct tp_iota {
using type = typename __tp_iota_helper<I, std::make_index_sequence<N>>::type;
};
template<size_t I, size_t N>
using tp_iota_t = typename tp_iota<I, N>::type;
template<typename T, typename...Args>
struct tp_set_push_back {
using type = tp_unique_t<tp_push_back_t<T, Args...>>;
};
template<typename T, typename...Args>
using tp_set_push_back_t = typename tp_set_push_back<T, Args...>::type;
template<typename T, typename...Args>
struct tp_set_push_front {
using type = tp_unique_t<tp_push_front_t<T, Args...>>;
};
template<typename T, typename...Args>
using tp_set_push_front_t = typename tp_set_push_front<T, Args...>::type;
template<typename...Args>
struct tp_set_union {
using type = tp_unique_t<tp_concat_t<Args...>>;
};
template<typename...Args>
using tp_set_union_t = typename tp_set_union<Args...>::type;
template<typename T, typename...Args>
struct tp_set_difference;
template<typename T>
struct tp_set_difference<T> {
using type = T;
};
template<typename T, typename U>
struct tp_set_difference<T, U> {
private:
template<typename C>
using __filter = tp_contains<U, C>;
public:
using type = tp_remove_if_t<T, __filter>;
};
template<typename T, typename U, typename...Args>
struct tp_set_difference<T, U, Args...> {
using type = typename tp_set_difference<typename tp_set_difference<T, U>::type, Args...>::type;
};
template<typename T, typename...Args>
using tp_set_difference_t = typename tp_set_difference<T, Args...>::type;
template<typename...Args>
struct tp_set_intersection;
template<>
struct tp_set_intersection<> {
using type = tp_list<>;
};
template<typename T>
struct tp_set_intersection<T> {
using type = T;
};
template<typename T, typename U>
struct tp_set_intersection<T, U> {
private:
template<typename C>
using __filter = tp_contains<U, C>;
public:
using type = tp_filter_t<T, __filter>;
};
template<typename T, typename U, typename...Args>
struct tp_set_intersection<T, U, Args...> {
using type = typename tp_set_intersection<typename tp_set_intersection<T, U>::type, Args...>::type;
};
template<typename...Args>
using tp_set_intersection_t = typename tp_set_intersection<Args...>::type;
template<typename C, typename T, typename = void>
struct __tp_find_unique_constructible_helper {
using type = tp_size_t<(size_t)(-1)>;
};
template<typename C, typename T>
struct __tp_find_unique_constructible_helper<C, T, std::enable_if_t<tp_size_v<T> == 1, void>> {
using type = tp_size_t<tp_find_v<C, tp_front_t<T>>>;
};
template<typename C, typename...Args>
struct tp_find_unique_constructible {
private:
template<typename U>
struct __remove : std::negation<std::is_constructible<U, Args...>> {};
using type1 = tp_remove_if_t<C, __remove>;
using type2 = typename __tp_find_unique_constructible_helper<C, type1, void>::type;
public:
constexpr static size_t value = type2::value;
};
template<typename C, typename...Args>
inline constexpr size_t tp_find_unique_constructible_v = tp_find_unique_constructible<C, Args...>::value;
template<typename C, size_t idx, typename T>
struct __tp_find_best_convertible_func {
auto operator()(tp_at_t<C, idx>)->tp_c<size_t, idx>;
};
template<typename C, typename I, typename T>
struct __tp_find_best_convertible_func_table;
template<typename C, typename...Args, typename T>
struct __tp_find_best_convertible_func_table<C, tp_list<Args...>, T>
: __tp_find_best_convertible_func<C, Args::value, T>... {
using __tp_find_best_convertible_func<C, Args::value, T>::operator()...;
};
template<typename C, typename I, typename T>
struct __tp_find_best_convertible_helper;
template<typename C, typename...Args, typename T>
struct __tp_find_best_convertible_helper<C, tp_list<Args...>, T> {
template<typename U>
struct __remove : std::negation<std::is_convertible<T, tp_at_t<C, U::value>>> {};
using __seq = tp_remove_if_t<tp_list<Args...>, __remove>;
using type = __tp_find_best_convertible_func_table<C, __seq, T>;
};
template<typename C, typename I, typename T>
using __tp_find_best_convertible_helper_t = typename __tp_find_best_convertible_helper<C, I, T>::type;
template<typename _Enable, typename C, typename T>
struct __tp_find_best_convertible{
using type = tp_c<size_t, (size_t)(-1)>;
};
template<typename C, typename T>
struct __tp_find_best_convertible<std::void_t<decltype(
__tp_find_best_convertible_helper_t<C, tp_iota_t<0, tp_size_v<C>>, T>{}(std::declval<T>()))>, C, T> {
using type = decltype(
__tp_find_best_convertible_helper_t<C, tp_iota_t<0, tp_size_v<C>>, T>{}(std::declval<T>()));
};
template<typename C, typename T>
struct tp_find_best_convertible {
using __type= typename __tp_find_best_convertible<void, C, T>::type;
constexpr static size_t value =  __type::value;
};
template<typename C, typename T>
inline constexpr size_t tp_find_best_convertible_v = tp_find_best_convertible<C, T>::value;
class tp_fn {
public:
template<typename C, typename F>
constexpr static F for_each(F&& f) {
return for_each(tp_rename_t<C, tp_list>{}, std::forward<F>(f));
}
template<template<typename...>typename C, typename...Args, typename F>
constexpr static F for_each(C<Args...>, F&& f) {
int dummy[] = { (f(Args()), 0)... };
(void)(dummy);
return std::forward<F>(f);
}
template<typename C, typename F, typename U>
constexpr static decltype(auto) accumulate(F&& f, U&& u) {
constexpr auto len = tp_size_v<C>;
if constexpr (len == 0) { return std::forward<U>(u); }
else {
#define __WJR_REGISTER_ACCUMULATE_FUNC(x, y)			\
if constexpr(len == x) {return ans##y;}		\
else{										\
auto ans##x = f(tp_at_t<C, x>(), ans##y)
#define __WJR_REGISTER_ACCUMULATE_FUNC_END	}
auto ans0 = f(tp_at_t<C, 0>(), std::forward<U>(u));
__WJR_REGISTER_ACCUMULATE_FUNC(1, 0);
__WJR_REGISTER_ACCUMULATE_FUNC(2, 1);
__WJR_REGISTER_ACCUMULATE_FUNC(3, 2);
__WJR_REGISTER_ACCUMULATE_FUNC(4, 3);
__WJR_REGISTER_ACCUMULATE_FUNC(5, 4);
__WJR_REGISTER_ACCUMULATE_FUNC(6, 5);
__WJR_REGISTER_ACCUMULATE_FUNC(7, 6);
__WJR_REGISTER_ACCUMULATE_FUNC(8, 7);
return tp_remove_prefix_t<C, 8>{}.accumulate(std::forward<F>(f), ans4);
__WJR_REGISTER_ACCUMULATE_FUNC_END;
__WJR_REGISTER_ACCUMULATE_FUNC_END;
__WJR_REGISTER_ACCUMULATE_FUNC_END;
__WJR_REGISTER_ACCUMULATE_FUNC_END;
__WJR_REGISTER_ACCUMULATE_FUNC_END;
__WJR_REGISTER_ACCUMULATE_FUNC_END;
__WJR_REGISTER_ACCUMULATE_FUNC_END;
__WJR_REGISTER_ACCUMULATE_FUNC_END;
#undef __WJR_REGISTER_ACCUMULATE_FUNC_END
#undef __WJR_REGISTER_ACCUMULATE_FUNC
}
}
private:
};
inline constexpr tp_fn tp;
_WJR_END
#endif // __WJR_TP_LIST_H

_WJR_BEGIN
struct switch_fallthrough {};
struct switch_default {};
namespace __switch_details {
#define __WJR_SWITCH_STRATEGY_CASE(X)	                                                                                \
case (tp_at_t<__clist, (X)>::value) : {																				\
if constexpr((X) < tp_size_v<IndexList>){	                                                                    \
constexpr bool __is_fallthrough = tp_contains_v<tp_at_t<StateList, (X)>, switch_fallthrough>;	            \
if constexpr (__is_fallthrough){	                                                                        \
(void)(func(tp_size_t<(X)>{}, std::forward<Args>(args)...));	                                        \
}else {	                                                                                                    \
return static_cast<Ret>(func(tp_size_t<(X)>{}, std::forward<Args>(args)...));	                        \
WJR_UNREACHABLE;	                                                                                    \
}	                                                                                                        \
}	                                                                                                            \
else if constexpr(!Default){WJR_UNREACHABLE;}	                                                                \
WJR_FALLTHROUGH;	                                                                                            \
}
#define __WJR_CALL1(CASE, I)     \
CASE(I)
#define __WJR_CALL2(CASE, I)     \
__WJR_CALL1(CASE, I);	         \
__WJR_CALL1(CASE, I + 1)
#define __WJR_CALL4(CASE, I)     \
__WJR_CALL2(CASE, I);	         \
__WJR_CALL2(CASE, I + 2)
#define __WJR_CALL8(CASE, I)     \
__WJR_CALL4(CASE, I);	         \
__WJR_CALL4(CASE, I + 4)
#define __WJR_CALL16(CASE, I)    \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 8)
#define __WJR_CALL32(CASE, I)    \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 16)
#define __WJR_CALL64(CASE, I)    \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 32)
#define __WJR_CALL128(CASE, I)   \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 64)
#define __WJR_CALL256(CASE, I)   \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 128)
#define __WJR_CALL512(CASE, I)   \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 256)
#define __WJR_CALL1024(CASE, I)  \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 512)
#define __WJR_CALL2048(CASE, I)  \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 1024)
#define __WJR_CALL4096(CASE, I)  \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 2048)
#define __WJR_CALL8192(CASE, I)  \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 4096)
#define __WJR_CALL16384(CASE, I) \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 8192)
#define __WJR_SWITCH_STRATEGY_CASES(N)	                                                 \
switch(token){	                                                                     \
WJR_MACRO_CONCAT(__WJR_CALL, N)(__WJR_SWITCH_STRATEGY_CASE, 0)	                 \
default:{	                                                                         \
if constexpr(Default){	                                                         \
return static_cast<Ret>(func(switch_default{}, std::forward<Args>(args)...));\
}	                                                                             \
WJR_UNREACHABLE;	                                                                 \
}	                                                                                 \
}
template<int S>
struct switch_strategy;
template<size_t I>
struct __complete_helper {
template<typename T>
using fn = tp_equal_c<T, tp_size_t<I>>;
};
template<size_t N, size_t I, typename IndexList>
constexpr auto __complete() {
if constexpr (N == tp_size_v<IndexList>) return IndexList{};
else {
if constexpr (tp_find_if_v<IndexList, __complete_helper<I>::template fn> != -1) {
return __complete<N, I + 1, IndexList>();
}
else {
return __complete<N, I + 1, tp_push_back_t<IndexList, tp_size_t<I>>>();
}
}
}
#define __WJR_SWITCH_STRATEGY(N)	                                                                     \
template<>	                                                                                         \
struct switch_strategy<(base2_width<unsigned int>(N) - 1)> {	                                     \
template<typename Ret, typename IndexList, bool Default, typename StateList,	                 \
typename Token, typename Func, typename...Args>	                                             \
inline constexpr static Ret invoke(Token token, Func func, Args&&...args) {						 \
constexpr auto __value = __complete<(N), 0, IndexList>();	                                 \
using __clist = WJR_PRIMITIVE_TYPE(__value);	                                             \
__WJR_SWITCH_STRATEGY_CASES(N);	                                                             \
}	                                                                                             \
};
WJR_MACRO_CALL(__WJR_SWITCH_STRATEGY, , 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384);
#undef __WJR_CALL1
#undef __WJR_CALL2
#undef __WJR_CALL4
#undef __WJR_CALL8
#undef __WJR_CALL16
#undef __WJR_CALL32
#undef __WJR_CALL64
#undef __WJR_CALL128
#undef __WJR_CALL256
#undef __WJR_CALL512
#undef __WJR_CALL1024
#undef __WJR_CALL2048
#undef __WJR_CALL4096
#undef __WJR_CALL8192
#undef __WJR_CALL16384
#undef __WJR_SWITCH_STRATEGY_CASE
#undef __WJR_SWITCH_STRATEGY
}
template<typename IndexList, bool Default, typename StateList>
struct switch_invoke_fn {
static_assert(tp_size_v<IndexList> == tp_size_v<StateList>, "");
constexpr static size_t size() { return tp_size_v<IndexList>; }
static_assert(size() >= 1, "");
private:
template<typename T>
using __remove_if = tp_contains<tp_at_t<StateList, tp_front_t<T>::value>, switch_fallthrough>;
constexpr static int __strategy = base2_width<unsigned int>(size() - 1);
static_assert(size() <= ((size_t)1 << __strategy), "");
template<typename T, typename Func, typename...Args>
constexpr static auto __get_ret_list() {
if constexpr (Default) {
return tp_push_back_t<T, std::invoke_result_t<Func, switch_default, Args&&...>>{};
}
else return T{};
}
public:
template<typename Token, typename Func, typename...Args>
inline constexpr decltype(auto) operator()(Token token, Func func, Args&&...args) const {
using RetList = tp_transform_f<tp_iota_t<0, size()>,
tp_bind<std::invoke_result_t, Func, tp_arg<0>, Args&&...>>;
using ZipFilterRetList = tp_remove_if_t<tp_zip_t<tp_list, tp_iota_t<0, size()>, RetList>, __remove_if>;
using _FilterRetList = tp_transform_t<ZipFilterRetList, tp_back_t>;
constexpr auto __value = __get_ret_list<_FilterRetList, Func, Args&&...>();
using FilterRetList = WJR_PRIMITIVE_TYPE(__value);
using UniqueRetList = tp_unique_t<FilterRetList>;
if constexpr (tp_is_empty_v<UniqueRetList>) {
return __switch_details::switch_strategy<__strategy>
::template invoke<void, IndexList, Default, StateList>(token,
func, std::forward<Args>(args)...);
}
else {
static_assert(tp_size_v<UniqueRetList> == 1, "");
using Ret = tp_front_t<UniqueRetList>;
return __switch_details::switch_strategy<__strategy>
::template invoke<Ret, IndexList, Default, StateList>(token,
func, std::forward<Args>(args)...);
}
}
};
template<typename IndexList, bool Default = false,
typename StateList = tp_repeat_t<tp_list<tp_list<>>, tp_size_v<IndexList>>>
inline constexpr switch_invoke_fn<IndexList, Default, StateList> switch_invoke{};
_WJR_END
#endif // __WJR_SWITCH_H

_WJR_PARSE_BEGIN
struct empty {};
_WJR_PARSE_END
#endif // __WJR_PARSE_GENERIC_H

_WJR_PARSE_LL1_BEGIN
template<typename BNF>
struct non_terminator_list {
using type = tp_unique_t<tp_transform_t<BNF, tp_front_t>>;
};
template<typename BNF>
using non_terminator_list_t = typename non_terminator_list<BNF>::type;
template<typename BNF, typename T>
inline constexpr size_t find_non_terminator_v = tp_find_v<non_terminator_list_t<BNF>, T>;
template<typename BNF, typename T>
inline constexpr bool is_non_terminator_v = find_non_terminator_v<BNF, T> != -1;
template<typename BNF>
struct terminator_list {
using type = tp_set_difference_t<
tp_left_fold_t<tp_transform_t<BNF, tp_pop_front_t>, tp_list<>, tp_set_union_t>,
non_terminator_list_t<BNF>
>;
};
template<typename BNF>
using terminator_list_t = typename terminator_list<BNF>::type;
template<typename BNF, typename T>
inline constexpr size_t find_terminator_v = tp_find_v<terminator_list_t<BNF>, T>;
template<typename BNF, typename T>
inline constexpr bool is_terminator_v = find_terminator_v<BNF, T> != -1;
template<typename BNF, typename FIRST, typename LAST_FIRST>
inline constexpr auto __first_helper();
template<typename BNF, typename E, typename FIRST>
inline constexpr auto __first_helper2();
template<typename BNF, typename FA, typename SON, typename FIRST>
inline constexpr auto __first_helper3();
template<typename BNF, typename FIRST, typename LAST_FIRST>
inline constexpr auto __first_helper() {
if constexpr (std::is_same_v<FIRST, LAST_FIRST>) {
return FIRST{};
}
else {
constexpr auto __value = __first_helper2<BNF, BNF, FIRST>();
using __type = WJR_PRIMITIVE_TYPE(__value);
return __first_helper<BNF, __type, FIRST>();
}
}
template<typename BNF, typename E, typename FIRST>
inline constexpr auto __first_helper2() {
constexpr auto len = tp_size_v<E>;
if constexpr (len == 0) {
return FIRST{};
}
else {
using front = tp_front_t<E>;
constexpr auto __value = __first_helper3<BNF, tp_front_t<front>, tp_pop_front_t<front>, FIRST>();
using __type = WJR_PRIMITIVE_TYPE(__value);
return __first_helper2<BNF, tp_pop_front_t<E>, __type>();
}
}
template<typename BNF, typename FA, typename SON, typename FIRST>
inline constexpr auto __first_helper3() {
constexpr auto len = tp_size_v<SON>;
if constexpr (len == 0) {
return FIRST{};
}
else {
using front = tp_front_t<SON>;
constexpr auto idx = find_non_terminator_v<BNF, FA>;
if constexpr (is_terminator_v<BNF, front>) {
return tp_replace_at_t<FIRST, idx, tp_set_push_back_t<tp_at_t<FIRST, idx>, front>>();
}
else {
constexpr auto idx2 = find_non_terminator_v<BNF, front>;
using __type1 = tp_replace_at_t<FIRST, idx, tp_set_union_t<tp_at_t<FIRST, idx>, tp_at_t<FIRST, idx2>>>;
if constexpr (tp_contains_v<tp_at_t<FIRST, idx2>, empty>) {
constexpr auto __value = __first_helper2<BNF, FA, tp_pop_front_t<SON>, __type1>();
using __type2 = WJR_PRIMITIVE_TYPE(__value);
return __type2();
}
else {
return __type1();
}
}
}
}
template<typename BNF>
struct first {
private:
constexpr static auto __value = __first_helper<BNF,
tp_repeat_t<tp_list<tp_list<>>, tp_size_v<non_terminator_list_t<BNF>>>,
void>();
public:
using type = WJR_PRIMITIVE_TYPE(__value);
};
template<typename BNF>
using first_t = typename first<BNF>::type;
template<typename BNF, typename E>
struct get_first;
template<typename BNF,  typename E>
using get_first_t = typename get_first<BNF, E>::type;
template<typename BNF, typename T, typename Enable>
struct __get_first_helper2 {
using type = tp_at_t<first_t<BNF>, find_non_terminator_v<BNF, T>>;
};
template<typename BNF, typename T>
struct __get_first_helper2<BNF, T, std::enable_if_t<is_terminator_v<BNF, T>, void>> {
using type = tp_list<T>;
};
template<typename BNF, typename E>
struct __get_first_helper {
using type = typename __get_first_helper2<BNF, tp_front_t<E>, void>::type;
};
template<typename BNF, typename...Args>
struct __get_first_helper<BNF, tp_list<empty, Args...>> {
using type = typename __get_first_helper2<BNF, tp_list<Args...>, void>::type;
};
template<typename BNF, typename T>
struct __get_first_helper<BNF, tp_list<T>> {
using type = typename __get_first_helper2<BNF, T, void>::type;
};
template<typename BNF>
struct __get_first_helper<BNF, tp_list<empty>> {
using type = typename __get_first_helper2<BNF, empty, void>::type;
};
template<typename BNF, typename E>
struct get_first {
using type = typename __get_first_helper<BNF, E>::type;
};
template<typename BNF>
struct start;
template<typename BNF>
using start_t = typename start<BNF>::type;
template<typename BNF, typename FOLLOW, typename LAST_FOLLOW>
inline constexpr auto __follow_helper();
template<typename BNF, typename E, typename FOLLOW>
inline constexpr auto __follow_helper2();
template<typename BNF, typename FA, typename SON, typename FOLLOW>
inline constexpr auto __follow_helper3();
template<typename BNF, typename FOLLOW, typename LAST_FOLLOW>
inline constexpr auto __follow_helper() {
if constexpr (std::is_same_v<FOLLOW, LAST_FOLLOW>) {
return FOLLOW{};
}
else {
constexpr auto __value = __follow_helper2<BNF, BNF, FOLLOW>();
using __type = WJR_PRIMITIVE_TYPE(__value);
return __follow_helper<BNF, __type, FOLLOW>();
}
}
template<typename BNF, typename E, typename FOLLOW>
inline constexpr auto __follow_helper2() {
constexpr auto len = tp_size_v<E>;
if constexpr (len == 0) {
return FOLLOW{};
}
else {
using front = tp_front_t<E>;
constexpr auto __value = __follow_helper3<BNF, tp_front_t<front>, tp_pop_front_t<front>, FOLLOW>();
using __type = WJR_PRIMITIVE_TYPE(__value);
return __follow_helper2<BNF, tp_pop_front_t<E>, __type>();
}
}
template<typename BNF, typename FA, typename SON, typename FOLLOW>
inline constexpr auto __follow_helper3() {
constexpr auto len = tp_size_v<SON>;
if constexpr (len == 0) {
return FOLLOW{};
}
else {
constexpr auto idx0 = find_non_terminator_v<BNF, FA>;
using first = tp_front_t<SON>;
if constexpr (is_terminator_v<BNF, first>) {
return __follow_helper3<BNF, FA, tp_pop_front_t<SON>, FOLLOW>();
}
else {
constexpr auto idx1 = find_non_terminator_v<BNF, first>;
if constexpr (len == 1) {
return tp_replace_at_t<FOLLOW, idx1, tp_set_union_t<tp_at_t<FOLLOW, idx1>, tp_at_t<FOLLOW, idx0>>>{};
}
else {
using second_first = get_first_t<BNF, tp_pop_front_t<SON>>;
using __type = tp_set_union_t<tp_at_t<FOLLOW, idx1>,
tp_remove_t<second_first, empty>>;
if constexpr (tp_contains_v<second_first, empty>) {
using __type2 = tp_replace_at_t<FOLLOW, idx1, tp_set_union_t<__type, tp_at_t<FOLLOW, idx0>>>;
return __follow_helper3<BNF, FA, tp_pop_front_t<SON>, __type2>();
}
else {
using __type2 = tp_replace_at_t<FOLLOW, idx1, __type>;
return __follow_helper3<BNF, FA, tp_pop_front_t<SON>, __type2>();
}
}
}
}
}
template<typename BNF>
struct follow {
private:
using __follow = tp_repeat_t<tp_list<tp_list<>>, tp_size_v<non_terminator_list_t<BNF>>>;
constexpr static auto __value = __follow_helper<BNF,
__follow,
void>();
public:
using type = WJR_PRIMITIVE_TYPE(__value);
};
template<typename BNF>
using follow_t = typename follow<BNF>::type;
template<typename BNF, typename E, typename SELECT>
inline constexpr auto __select_helper() {
constexpr auto len = tp_size_v<E>;
if constexpr (len == 0) {
return SELECT{};
}
else {
using first = tp_front_t<E>;
using fa = tp_front_t<first>;
using son = tp_pop_front_t<first>;
using NT = non_terminator_list_t<BNF>;
using FOLLOW = follow_t<BNF>;
constexpr auto idx0 = tp_find_v<NT, fa>;
using __first = get_first_t<BNF, son>;
if constexpr (tp_contains_v<__first, empty>) {
using __follow = tp_at_t<FOLLOW, idx0>;
using __select = tp_push_back_t<SELECT, tp_set_union_t<tp_remove_t<__first, empty>, __follow>>;
return __select_helper<BNF, tp_pop_front_t<E>, __select>();
}
else {
using __select = tp_push_back_t<SELECT, __first>;
return __select_helper<BNF, tp_pop_front_t<E>, __select>();
}
}
}
template<typename BNF>
struct select {
private:
constexpr static auto __value = __select_helper<BNF, BNF, tp_list<>>();
public:
using type = WJR_PRIMITIVE_TYPE(__value);
static_assert(tp_size_v<type> == tp_size_v<BNF>, "");
};
template<typename BNF>
using select_t = typename select<BNF>::type;
template<typename BNF>
struct parse_fn {
using SELECT = select_t<BNF>;
private:
template<typename T, typename ACTION, typename...Args>
inline constexpr static decltype(auto) invoke(ACTION act, Args&&...args) {
if constexpr (std::is_same_v<T, disable_tag>) {
(void)(std::forward<Args>(args), ...);
return;
}
else {
return act(T{}, std::forward<Args>(args)...);
}
}
template<typename T, typename __SELECT, size_t I, typename __LIST>
constexpr static auto __get_switch_helper() {
constexpr auto len = tp_size_v<__SELECT>;
if constexpr (len == 0) { return __LIST{}; }
else {
using front = tp_front_t<__SELECT>;
using thisT = tp_front_t<tp_at_t<BNF, I>>;
if constexpr (std::is_same_v<thisT, T> && tp_size_v<front> != 0) {
auto pre = __LIST{};
auto aft = tp.accumulate<front>([](auto x, auto l) {
using typex = WJR_PRIMITIVE_TYPE(x);
using typel = WJR_PRIMITIVE_TYPE(l);
using thisT = tp_list<tp_size_t<typex::value>, disable_tag, tp_list<switch_fallthrough>>;
return tp_push_back_t<typel, thisT>{};
}, pre);
using thisList = WJR_PRIMITIVE_TYPE(aft);
constexpr auto ListSize = tp_size_v<thisList>;
using Last = tp_at_t<thisList, ListSize - 1>;
using List = tp_replace_at_t<thisList, ListSize - 1,
tp_list<tp_front_t<Last>, tp_size_t<I>, tp_list<>>>;
return __get_switch_helper<T, tp_pop_front_t<__SELECT>, I + 1, List>();
}
else return __get_switch_helper<T, tp_pop_front_t<__SELECT>, I + 1, __LIST>();
}
}
public:
template<typename T, typename ACTION, typename...Args>
inline constexpr decltype(auto) operator()(T, size_t token, ACTION act, Args&&...args) const {
constexpr auto __value = __get_switch_helper<T, SELECT, 0, tp_list<>>();
using __switch = WJR_PRIMITIVE_TYPE(__value);
using __index = tp_transform_t<__switch, tp_front_t>;
using __state = tp_transform_t<__switch, tp_back_t>;
return switch_invoke<__index, false, __state>(token, [](auto I, auto&&...args) {
return invoke<tp_at_t<tp_at_t<__switch, I>, 1>>(std::forward<decltype(args)>(args)...);
}, act, std::forward<Args>(args)...);
}
};
template<typename BNF>
inline constexpr parse_fn<BNF> parse{};
_WJR_PARSE_LL1_END
#endif // __WJR_PARSE_LL1_H

#endif // __WJR_PARSE_H

#pragma once
#ifndef __WJR_JSON_H
#define __WJR_JSON_H
#include <map>
#pragma once
#ifndef __WJR_STRING_FUNC_H
#define __WJR_STRING_FUNC_H
#include <optional>
#ifndef DOUBLE_CONVERSION_DOUBLE_CONVERSION_H_
#define DOUBLE_CONVERSION_DOUBLE_CONVERSION_H_
#ifndef DOUBLE_CONVERSION_STRING_TO_DOUBLE_H_
#define DOUBLE_CONVERSION_STRING_TO_DOUBLE_H_
#ifndef DOUBLE_CONVERSION_UTILS_H_
#define DOUBLE_CONVERSION_UTILS_H_
#include <cstdlib>
#include <cstring>
#if __cplusplus >= 201103L
#define DOUBLE_CONVERSION_NULLPTR nullptr
#else
#define DOUBLE_CONVERSION_NULLPTR NULL
#endif
#include <cassert>
#ifndef DOUBLE_CONVERSION_ASSERT
#define DOUBLE_CONVERSION_ASSERT(condition)         \
assert(condition)
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(ASSERT)
#define ASSERT DOUBLE_CONVERSION_ASSERT
#endif
#ifndef DOUBLE_CONVERSION_UNIMPLEMENTED
#define DOUBLE_CONVERSION_UNIMPLEMENTED() (abort())
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(UNIMPLEMENTED)
#define UNIMPLEMENTED DOUBLE_CONVERSION_UNIMPLEMENTED
#endif
#ifndef DOUBLE_CONVERSION_NO_RETURN
#ifdef _MSC_VER
#define DOUBLE_CONVERSION_NO_RETURN __declspec(noreturn)
#else
#define DOUBLE_CONVERSION_NO_RETURN __attribute__((noreturn))
#endif
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(NO_RETURN)
#define NO_RETURN DOUBLE_CONVERSION_NO_RETURN
#endif
#ifndef DOUBLE_CONVERSION_UNREACHABLE
#ifdef _MSC_VER
void DOUBLE_CONVERSION_NO_RETURN abort_noreturn();
inline void abort_noreturn() { abort(); }
#define DOUBLE_CONVERSION_UNREACHABLE()   (abort_noreturn())
#else
#define DOUBLE_CONVERSION_UNREACHABLE()   (abort())
#endif
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(UNREACHABLE)
#define UNREACHABLE DOUBLE_CONVERSION_UNREACHABLE
#endif
#ifdef __has_attribute
#   define DOUBLE_CONVERSION_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#   define DOUBLE_CONVERSION_HAS_ATTRIBUTE(x) 0
#endif
#ifndef DOUBLE_CONVERSION_UNUSED
#if DOUBLE_CONVERSION_HAS_ATTRIBUTE(unused)
#define DOUBLE_CONVERSION_UNUSED __attribute__((unused))
#else
#define DOUBLE_CONVERSION_UNUSED
#endif
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(UNUSED)
#define UNUSED DOUBLE_CONVERSION_UNUSED
#endif
#if DOUBLE_CONVERSION_HAS_ATTRIBUTE(uninitialized)
#define DOUBLE_CONVERSION_STACK_UNINITIALIZED __attribute__((uninitialized))
#else
#define DOUBLE_CONVERSION_STACK_UNINITIALIZED
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(STACK_UNINITIALIZED)
#define STACK_UNINITIALIZED DOUBLE_CONVERSION_STACK_UNINITIALIZED
#endif
/*
double Div_double(double x, double y) { return x / y; }
double Div_double(double x, double y);  // Forward declaration.
int main(int argc, char** argv) {
return Div_double(89255.0, 1e22) == 89255e-22;
}
*/
#if defined(_M_X64) || defined(__x86_64__) || \
defined(__ARMEL__) || defined(__avr32__) || defined(_M_ARM) || defined(_M_ARM64) || \
defined(__hppa__) || defined(__ia64__) || \
defined(__mips__) || \
defined(__loongarch__) || \
defined(__nios2__) || defined(__ghs) || \
defined(__powerpc__) || defined(__ppc__) || defined(__ppc64__) || \
defined(_POWER) || defined(_ARCH_PPC) || defined(_ARCH_PPC64) || \
defined(__sparc__) || defined(__sparc) || defined(__s390__) || \
defined(__SH4__) || defined(__alpha__) || \
defined(_MIPS_ARCH_MIPS32R2) || defined(__ARMEB__) ||\
defined(__AARCH64EL__) || defined(__aarch64__) || defined(__AARCH64EB__) || \
defined(__riscv) || defined(__e2k__) || \
defined(__or1k__) || defined(__arc__) || defined(__ARC64__) || \
defined(__microblaze__) || defined(__XTENSA__) || \
defined(__EMSCRIPTEN__) || defined(__wasm32__)
#define DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS 1
#elif defined(__mc68000__) || \
defined(__pnacl__) || defined(__native_client__)
#undef DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS
#elif defined(_M_IX86) || defined(__i386__) || defined(__i386)
#if defined(_WIN32)
#define DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS 1
#else
#undef DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS
#endif  // _WIN32
#else
#error Target architecture was not detected as supported by Double-Conversion.
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(CORRECT_DOUBLE_OPERATIONS)
#define CORRECT_DOUBLE_OPERATIONS DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS
#endif
#if defined(_WIN32) && !defined(__MINGW32__)
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;  // NOLINT
typedef unsigned short uint16_t;  // NOLINT
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif
typedef uint16_t uc16;
#define DOUBLE_CONVERSION_UINT64_2PART_C(a, b) (((static_cast<uint64_t>(a) << 32) + 0x##b##u))
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(UINT64_2PART_C)
#define UINT64_2PART_C DOUBLE_CONVERSION_UINT64_2PART_C
#endif
#ifndef DOUBLE_CONVERSION_ARRAY_SIZE
#define DOUBLE_CONVERSION_ARRAY_SIZE(a)                                   \
((sizeof(a) / sizeof(*(a))) /                         \
static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(ARRAY_SIZE)
#define ARRAY_SIZE DOUBLE_CONVERSION_ARRAY_SIZE
#endif
#ifndef DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN
#define DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(TypeName)      \
TypeName(const TypeName&);                    \
void operator=(const TypeName&)
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(DC_DISALLOW_COPY_AND_ASSIGN)
#define DC_DISALLOW_COPY_AND_ASSIGN DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN
#endif
#ifndef DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS
#define DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
TypeName();                                    \
DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(TypeName)
#endif
#if defined(DOUBLE_CONVERSION_NON_PREFIXED_MACROS) && !defined(DC_DISALLOW_IMPLICIT_CONSTRUCTORS)
#define DC_DISALLOW_IMPLICIT_CONSTRUCTORS DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS
#endif
namespace double_conversion {
inline int StrLength(const char* string) {
size_t length = strlen(string);
DOUBLE_CONVERSION_ASSERT(length == static_cast<size_t>(static_cast<int>(length)));
return static_cast<int>(length);
}
template <typename T>
class Vector {
public:
Vector() : start_(DOUBLE_CONVERSION_NULLPTR), length_(0) {}
Vector(T* data, int len) : start_(data), length_(len) {
DOUBLE_CONVERSION_ASSERT(len == 0 || (len > 0 && data != DOUBLE_CONVERSION_NULLPTR));
}
Vector<T> SubVector(int from, int to) {
DOUBLE_CONVERSION_ASSERT(to <= length_);
DOUBLE_CONVERSION_ASSERT(from < to);
DOUBLE_CONVERSION_ASSERT(0 <= from);
return Vector<T>(start() + from, to - from);
}
int length() const { return length_; }
bool is_empty() const { return length_ == 0; }
T* start() const { return start_; }
T& operator[](int index) const {
DOUBLE_CONVERSION_ASSERT(0 <= index && index < length_);
return start_[index];
}
T& first() { return start_[0]; }
T& last() { return start_[length_ - 1]; }
void pop_back() {
DOUBLE_CONVERSION_ASSERT(!is_empty());
--length_;
}
private:
T* start_;
int length_;
};
class StringBuilder {
public:
StringBuilder(char* buffer, int buffer_size)
: buffer_(buffer, buffer_size), position_(0) { }
~StringBuilder() { if (!is_finalized()) Finalize(); }
int size() const { return buffer_.length(); }
int position() const {
DOUBLE_CONVERSION_ASSERT(!is_finalized());
return position_;
}
void Reset() { position_ = 0; }
void AddCharacter(char c) {
DOUBLE_CONVERSION_ASSERT(c != '\0');
DOUBLE_CONVERSION_ASSERT(!is_finalized() && position_ < buffer_.length());
buffer_[position_++] = c;
}
void AddString(const char* s) {
AddSubstring(s, StrLength(s));
}
void AddSubstring(const char* s, int n) {
DOUBLE_CONVERSION_ASSERT(!is_finalized() && position_ + n < buffer_.length());
DOUBLE_CONVERSION_ASSERT(static_cast<size_t>(n) <= strlen(s));
memmove(&buffer_[position_], s, static_cast<size_t>(n));
position_ += n;
}
void AddPadding(char c, int count) {
for (int i = 0; i < count; i++) {
AddCharacter(c);
}
}
char* Finalize() {
DOUBLE_CONVERSION_ASSERT(!is_finalized() && position_ < buffer_.length());
buffer_[position_] = '\0';
DOUBLE_CONVERSION_ASSERT(strlen(buffer_.start()) == static_cast<size_t>(position_));
position_ = -1;
DOUBLE_CONVERSION_ASSERT(is_finalized());
return buffer_.start();
}
private:
Vector<char> buffer_;
int position_;
bool is_finalized() const { return position_ < 0; }
DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS(StringBuilder);
};
template <class Dest, class Source>
Dest BitCast(const Source& source) {
#if __cplusplus >= 201103L
static_assert(sizeof(Dest) == sizeof(Source),
"source and destination size mismatch");
#else
DOUBLE_CONVERSION_UNUSED
typedef char VerifySizesAreEqual[sizeof(Dest) == sizeof(Source) ? 1 : -1];
#endif
Dest dest;
memmove(&dest, &source, sizeof(dest));
return dest;
}
template <class Dest, class Source>
Dest BitCast(Source* source) {
return BitCast<Dest>(reinterpret_cast<uintptr_t>(source));
}
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_UTILS_H_

namespace double_conversion {
class StringToDoubleConverter {
public:
enum Flags {
NO_FLAGS = 0,
ALLOW_HEX = 1,
ALLOW_OCTALS = 2,
ALLOW_TRAILING_JUNK = 4,
ALLOW_LEADING_SPACES = 8,
ALLOW_TRAILING_SPACES = 16,
ALLOW_SPACES_AFTER_SIGN = 32,
ALLOW_CASE_INSENSITIVITY = 64,
ALLOW_CASE_INSENSIBILITY = 64,  // Deprecated
ALLOW_HEX_FLOATS = 128,
};
static const uc16 kNoSeparator = '\0';
StringToDoubleConverter(int flags,
double empty_string_value,
double junk_string_value,
const char* infinity_symbol,
const char* nan_symbol,
uc16 separator = kNoSeparator)
: flags_(flags),
empty_string_value_(empty_string_value),
junk_string_value_(junk_string_value),
infinity_symbol_(infinity_symbol),
nan_symbol_(nan_symbol),
separator_(separator) {
}
double StringToDouble(const char* buffer,
int length,
int* processed_characters_count) const;
double StringToDouble(const uc16* buffer,
int length,
int* processed_characters_count) const;
float StringToFloat(const char* buffer,
int length,
int* processed_characters_count) const;
float StringToFloat(const uc16* buffer,
int length,
int* processed_characters_count) const;
template <typename T>
T StringTo(const char* buffer,
int length,
int* processed_characters_count) const;
template <typename T>
T StringTo(const uc16* buffer,
int length,
int* processed_characters_count) const;
private:
const int flags_;
const double empty_string_value_;
const double junk_string_value_;
const char* const infinity_symbol_;
const char* const nan_symbol_;
const uc16 separator_;
template <class Iterator>
double StringToIeee(Iterator start_pointer,
int length,
bool read_as_double,
int* processed_characters_count) const;
DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS(StringToDoubleConverter);
};
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_STRING_TO_DOUBLE_H_
#include <climits>
#include <locale>
#include <cmath>
#ifndef DOUBLE_CONVERSION_DOUBLE_H_
#define DOUBLE_CONVERSION_DOUBLE_H_
#ifndef DOUBLE_CONVERSION_DIY_FP_H_
#define DOUBLE_CONVERSION_DIY_FP_H_
namespace double_conversion {
class DiyFp {
public:
static const int kSignificandSize = 64;
DiyFp() : f_(0), e_(0) {}
DiyFp(const uint64_t significand, const int32_t exponent) : f_(significand), e_(exponent) {}
void Subtract(const DiyFp& other) {
DOUBLE_CONVERSION_ASSERT(e_ == other.e_);
DOUBLE_CONVERSION_ASSERT(f_ >= other.f_);
f_ -= other.f_;
}
static DiyFp Minus(const DiyFp& a, const DiyFp& b) {
DiyFp result = a;
result.Subtract(b);
return result;
}
void Multiply(const DiyFp& other) {
const uint64_t kM32 = 0xFFFFFFFFU;
const uint64_t a = f_ >> 32;
const uint64_t b = f_ & kM32;
const uint64_t c = other.f_ >> 32;
const uint64_t d = other.f_ & kM32;
const uint64_t ac = a * c;
const uint64_t bc = b * c;
const uint64_t ad = a * d;
const uint64_t bd = b * d;
const uint64_t tmp = (bd >> 32) + (ad & kM32) + (bc & kM32) + (1U << 31);
e_ += other.e_ + 64;
f_ = ac + (ad >> 32) + (bc >> 32) + (tmp >> 32);
}
static DiyFp Times(const DiyFp& a, const DiyFp& b) {
DiyFp result = a;
result.Multiply(b);
return result;
}
void Normalize() {
DOUBLE_CONVERSION_ASSERT(f_ != 0);
uint64_t significand = f_;
int32_t exponent = e_;
const uint64_t k10MSBits = DOUBLE_CONVERSION_UINT64_2PART_C(0xFFC00000, 00000000);
while ((significand & k10MSBits) == 0) {
significand <<= 10;
exponent -= 10;
}
while ((significand & kUint64MSB) == 0) {
significand <<= 1;
exponent--;
}
f_ = significand;
e_ = exponent;
}
static DiyFp Normalize(const DiyFp& a) {
DiyFp result = a;
result.Normalize();
return result;
}
uint64_t f() const { return f_; }
int32_t e() const { return e_; }
void set_f(uint64_t new_value) { f_ = new_value; }
void set_e(int32_t new_value) { e_ = new_value; }
private:
static const uint64_t kUint64MSB = DOUBLE_CONVERSION_UINT64_2PART_C(0x80000000, 00000000);
uint64_t f_;
int32_t e_;
};
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_DIY_FP_H_

namespace double_conversion {
static uint64_t double_to_uint64(double d) { return BitCast<uint64_t>(d); }
static double uint64_to_double(uint64_t d64) { return BitCast<double>(d64); }
static uint32_t float_to_uint32(float f) { return BitCast<uint32_t>(f); }
static float uint32_to_float(uint32_t d32) { return BitCast<float>(d32); }
class Double {
public:
static const uint64_t kSignMask = DOUBLE_CONVERSION_UINT64_2PART_C(0x80000000, 00000000);
static const uint64_t kExponentMask = DOUBLE_CONVERSION_UINT64_2PART_C(0x7FF00000, 00000000);
static const uint64_t kSignificandMask = DOUBLE_CONVERSION_UINT64_2PART_C(0x000FFFFF, FFFFFFFF);
static const uint64_t kHiddenBit = DOUBLE_CONVERSION_UINT64_2PART_C(0x00100000, 00000000);
static const uint64_t kQuietNanBit = DOUBLE_CONVERSION_UINT64_2PART_C(0x00080000, 00000000);
static const int kPhysicalSignificandSize = 52;  // Excludes the hidden bit.
static const int kSignificandSize = 53;
static const int kExponentBias = 0x3FF + kPhysicalSignificandSize;
static const int kMaxExponent = 0x7FF - kExponentBias;
Double() : d64_(0) {}
explicit Double(double d) : d64_(double_to_uint64(d)) {}
explicit Double(uint64_t d64) : d64_(d64) {}
explicit Double(DiyFp diy_fp)
: d64_(DiyFpToUint64(diy_fp)) {}
DiyFp AsDiyFp() const {
DOUBLE_CONVERSION_ASSERT(Sign() > 0);
DOUBLE_CONVERSION_ASSERT(!IsSpecial());
return DiyFp(Significand(), Exponent());
}
DiyFp AsNormalizedDiyFp() const {
DOUBLE_CONVERSION_ASSERT(value() > 0.0);
uint64_t f = Significand();
int e = Exponent();
while ((f & kHiddenBit) == 0) {
f <<= 1;
e--;
}
f <<= DiyFp::kSignificandSize - kSignificandSize;
e -= DiyFp::kSignificandSize - kSignificandSize;
return DiyFp(f, e);
}
uint64_t AsUint64() const {
return d64_;
}
double NextDouble() const {
if (d64_ == kInfinity) return Double(kInfinity).value();
if (Sign() < 0 && Significand() == 0) {
return 0.0;
}
if (Sign() < 0) {
return Double(d64_ - 1).value();
} else {
return Double(d64_ + 1).value();
}
}
double PreviousDouble() const {
if (d64_ == (kInfinity | kSignMask)) return -Infinity();
if (Sign() < 0) {
return Double(d64_ + 1).value();
} else {
if (Significand() == 0) return -0.0;
return Double(d64_ - 1).value();
}
}
int Exponent() const {
if (IsDenormal()) return kDenormalExponent;
uint64_t d64 = AsUint64();
int biased_e =
static_cast<int>((d64 & kExponentMask) >> kPhysicalSignificandSize);
return biased_e - kExponentBias;
}
uint64_t Significand() const {
uint64_t d64 = AsUint64();
uint64_t significand = d64 & kSignificandMask;
if (!IsDenormal()) {
return significand + kHiddenBit;
} else {
return significand;
}
}
bool IsDenormal() const {
uint64_t d64 = AsUint64();
return (d64 & kExponentMask) == 0;
}
bool IsSpecial() const {
uint64_t d64 = AsUint64();
return (d64 & kExponentMask) == kExponentMask;
}
bool IsNan() const {
uint64_t d64 = AsUint64();
return ((d64 & kExponentMask) == kExponentMask) &&
((d64 & kSignificandMask) != 0);
}
bool IsQuietNan() const {
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
return IsNan() && ((AsUint64() & kQuietNanBit) == 0);
#else
return IsNan() && ((AsUint64() & kQuietNanBit) != 0);
#endif
}
bool IsSignalingNan() const {
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
return IsNan() && ((AsUint64() & kQuietNanBit) != 0);
#else
return IsNan() && ((AsUint64() & kQuietNanBit) == 0);
#endif
}
bool IsInfinite() const {
uint64_t d64 = AsUint64();
return ((d64 & kExponentMask) == kExponentMask) &&
((d64 & kSignificandMask) == 0);
}
int Sign() const {
uint64_t d64 = AsUint64();
return (d64 & kSignMask) == 0? 1: -1;
}
DiyFp UpperBoundary() const {
DOUBLE_CONVERSION_ASSERT(Sign() > 0);
return DiyFp(Significand() * 2 + 1, Exponent() - 1);
}
void NormalizedBoundaries(DiyFp* out_m_minus, DiyFp* out_m_plus) const {
DOUBLE_CONVERSION_ASSERT(value() > 0.0);
DiyFp v = this->AsDiyFp();
DiyFp m_plus = DiyFp::Normalize(DiyFp((v.f() << 1) + 1, v.e() - 1));
DiyFp m_minus;
if (LowerBoundaryIsCloser()) {
m_minus = DiyFp((v.f() << 2) - 1, v.e() - 2);
} else {
m_minus = DiyFp((v.f() << 1) - 1, v.e() - 1);
}
m_minus.set_f(m_minus.f() << (m_minus.e() - m_plus.e()));
m_minus.set_e(m_plus.e());
*out_m_plus = m_plus;
*out_m_minus = m_minus;
}
bool LowerBoundaryIsCloser() const {
bool physical_significand_is_zero = ((AsUint64() & kSignificandMask) == 0);
return physical_significand_is_zero && (Exponent() != kDenormalExponent);
}
double value() const { return uint64_to_double(d64_); }
static int SignificandSizeForOrderOfMagnitude(int order) {
if (order >= (kDenormalExponent + kSignificandSize)) {
return kSignificandSize;
}
if (order <= kDenormalExponent) return 0;
return order - kDenormalExponent;
}
static double Infinity() {
return Double(kInfinity).value();
}
static double NaN() {
return Double(kNaN).value();
}
private:
static const int kDenormalExponent = -kExponentBias + 1;
static const uint64_t kInfinity = DOUBLE_CONVERSION_UINT64_2PART_C(0x7FF00000, 00000000);
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
static const uint64_t kNaN = DOUBLE_CONVERSION_UINT64_2PART_C(0x7FF7FFFF, FFFFFFFF);
#else
static const uint64_t kNaN = DOUBLE_CONVERSION_UINT64_2PART_C(0x7FF80000, 00000000);
#endif
const uint64_t d64_;
static uint64_t DiyFpToUint64(DiyFp diy_fp) {
uint64_t significand = diy_fp.f();
int exponent = diy_fp.e();
while (significand > kHiddenBit + kSignificandMask) {
significand >>= 1;
exponent++;
}
if (exponent >= kMaxExponent) {
return kInfinity;
}
if (exponent < kDenormalExponent) {
return 0;
}
while (exponent > kDenormalExponent && (significand & kHiddenBit) == 0) {
significand <<= 1;
exponent--;
}
uint64_t biased_exponent;
if (exponent == kDenormalExponent && (significand & kHiddenBit) == 0) {
biased_exponent = 0;
} else {
biased_exponent = static_cast<uint64_t>(exponent + kExponentBias);
}
return (significand & kSignificandMask) |
(biased_exponent << kPhysicalSignificandSize);
}
DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(Double);
};
class Single {
public:
static const uint32_t kSignMask = 0x80000000;
static const uint32_t kExponentMask = 0x7F800000;
static const uint32_t kSignificandMask = 0x007FFFFF;
static const uint32_t kHiddenBit = 0x00800000;
static const uint32_t kQuietNanBit = 0x00400000;
static const int kPhysicalSignificandSize = 23;  // Excludes the hidden bit.
static const int kSignificandSize = 24;
Single() : d32_(0) {}
explicit Single(float f) : d32_(float_to_uint32(f)) {}
explicit Single(uint32_t d32) : d32_(d32) {}
DiyFp AsDiyFp() const {
DOUBLE_CONVERSION_ASSERT(Sign() > 0);
DOUBLE_CONVERSION_ASSERT(!IsSpecial());
return DiyFp(Significand(), Exponent());
}
uint32_t AsUint32() const {
return d32_;
}
int Exponent() const {
if (IsDenormal()) return kDenormalExponent;
uint32_t d32 = AsUint32();
int biased_e =
static_cast<int>((d32 & kExponentMask) >> kPhysicalSignificandSize);
return biased_e - kExponentBias;
}
uint32_t Significand() const {
uint32_t d32 = AsUint32();
uint32_t significand = d32 & kSignificandMask;
if (!IsDenormal()) {
return significand + kHiddenBit;
} else {
return significand;
}
}
bool IsDenormal() const {
uint32_t d32 = AsUint32();
return (d32 & kExponentMask) == 0;
}
bool IsSpecial() const {
uint32_t d32 = AsUint32();
return (d32 & kExponentMask) == kExponentMask;
}
bool IsNan() const {
uint32_t d32 = AsUint32();
return ((d32 & kExponentMask) == kExponentMask) &&
((d32 & kSignificandMask) != 0);
}
bool IsQuietNan() const {
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
return IsNan() && ((AsUint32() & kQuietNanBit) == 0);
#else
return IsNan() && ((AsUint32() & kQuietNanBit) != 0);
#endif
}
bool IsSignalingNan() const {
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
return IsNan() && ((AsUint32() & kQuietNanBit) != 0);
#else
return IsNan() && ((AsUint32() & kQuietNanBit) == 0);
#endif
}
bool IsInfinite() const {
uint32_t d32 = AsUint32();
return ((d32 & kExponentMask) == kExponentMask) &&
((d32 & kSignificandMask) == 0);
}
int Sign() const {
uint32_t d32 = AsUint32();
return (d32 & kSignMask) == 0? 1: -1;
}
void NormalizedBoundaries(DiyFp* out_m_minus, DiyFp* out_m_plus) const {
DOUBLE_CONVERSION_ASSERT(value() > 0.0);
DiyFp v = this->AsDiyFp();
DiyFp m_plus = DiyFp::Normalize(DiyFp((v.f() << 1) + 1, v.e() - 1));
DiyFp m_minus;
if (LowerBoundaryIsCloser()) {
m_minus = DiyFp((v.f() << 2) - 1, v.e() - 2);
} else {
m_minus = DiyFp((v.f() << 1) - 1, v.e() - 1);
}
m_minus.set_f(m_minus.f() << (m_minus.e() - m_plus.e()));
m_minus.set_e(m_plus.e());
*out_m_plus = m_plus;
*out_m_minus = m_minus;
}
DiyFp UpperBoundary() const {
DOUBLE_CONVERSION_ASSERT(Sign() > 0);
return DiyFp(Significand() * 2 + 1, Exponent() - 1);
}
bool LowerBoundaryIsCloser() const {
bool physical_significand_is_zero = ((AsUint32() & kSignificandMask) == 0);
return physical_significand_is_zero && (Exponent() != kDenormalExponent);
}
float value() const { return uint32_to_float(d32_); }
static float Infinity() {
return Single(kInfinity).value();
}
static float NaN() {
return Single(kNaN).value();
}
private:
static const int kExponentBias = 0x7F + kPhysicalSignificandSize;
static const int kDenormalExponent = -kExponentBias + 1;
static const int kMaxExponent = 0xFF - kExponentBias;
static const uint32_t kInfinity = 0x7F800000;
#if (defined(__mips__) && !defined(__mips_nan2008)) || defined(__hppa__)
static const uint32_t kNaN = 0x7FBFFFFF;
#else
static const uint32_t kNaN = 0x7FC00000;
#endif
const uint32_t d32_;
DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(Single);
};
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_DOUBLE_H_

#ifndef DOUBLE_CONVERSION_STRTOD_H_
#define DOUBLE_CONVERSION_STRTOD_H_
namespace double_conversion {
double Strtod(Vector<const char> buffer, int exponent);
float Strtof(Vector<const char> buffer, int exponent);
double StrtodTrimmed(Vector<const char> trimmed, int exponent);
float StrtofTrimmed(Vector<const char> trimmed, int exponent);
inline Vector<const char> TrimTrailingZeros(Vector<const char> buffer) {
for (int i = buffer.length() - 1; i >= 0; --i) {
if (buffer[i] != '0') {
return buffer.SubVector(0, i + 1);
}
}
return Vector<const char>(buffer.start(), 0);
}
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_STRTOD_H_
#include <climits>
#include <cstdarg>
#ifndef DOUBLE_CONVERSION_BIGNUM_H_
#define DOUBLE_CONVERSION_BIGNUM_H_
namespace double_conversion {
class Bignum {
public:
static const int kMaxSignificantBits = 3584;
Bignum() : used_bigits_(0), exponent_(0) {}
void AssignUInt16(const uint16_t value);
void AssignUInt64(uint64_t value);
void AssignBignum(const Bignum& other);
void AssignDecimalString(const Vector<const char> value);
void AssignHexString(const Vector<const char> value);
void AssignPowerUInt16(uint16_t base, const int exponent);
void AddUInt64(const uint64_t operand);
void AddBignum(const Bignum& other);
void SubtractBignum(const Bignum& other);
void Square();
void ShiftLeft(const int shift_amount);
void MultiplyByUInt32(const uint32_t factor);
void MultiplyByUInt64(const uint64_t factor);
void MultiplyByPowerOfTen(const int exponent);
void Times10() { return MultiplyByUInt32(10); }
uint16_t DivideModuloIntBignum(const Bignum& other);
bool ToHexString(char* buffer, const int buffer_size) const;
static int Compare(const Bignum& a, const Bignum& b);
static bool Equal(const Bignum& a, const Bignum& b) {
return Compare(a, b) == 0;
}
static bool LessEqual(const Bignum& a, const Bignum& b) {
return Compare(a, b) <= 0;
}
static bool Less(const Bignum& a, const Bignum& b) {
return Compare(a, b) < 0;
}
static int PlusCompare(const Bignum& a, const Bignum& b, const Bignum& c);
static bool PlusEqual(const Bignum& a, const Bignum& b, const Bignum& c) {
return PlusCompare(a, b, c) == 0;
}
static bool PlusLessEqual(const Bignum& a, const Bignum& b, const Bignum& c) {
return PlusCompare(a, b, c) <= 0;
}
static bool PlusLess(const Bignum& a, const Bignum& b, const Bignum& c) {
return PlusCompare(a, b, c) < 0;
}
private:
typedef uint32_t Chunk;
typedef uint64_t DoubleChunk;
static const int kChunkSize = sizeof(Chunk) * 8;
static const int kDoubleChunkSize = sizeof(DoubleChunk) * 8;
static const int kBigitSize = 28;
static const Chunk kBigitMask = (1 << kBigitSize) - 1;
static const int kBigitCapacity = kMaxSignificantBits / kBigitSize;
static void EnsureCapacity(const int size) {
if (size > kBigitCapacity) {
DOUBLE_CONVERSION_UNREACHABLE();
}
}
void Align(const Bignum& other);
void Clamp();
bool IsClamped() const {
return used_bigits_ == 0 || RawBigit(used_bigits_ - 1) != 0;
}
void Zero() {
used_bigits_ = 0;
exponent_ = 0;
}
void BigitsShiftLeft(const int shift_amount);
int BigitLength() const { return used_bigits_ + exponent_; }
Chunk& RawBigit(const int index);
const Chunk& RawBigit(const int index) const;
Chunk BigitOrZero(const int index) const;
void SubtractTimes(const Bignum& other, const int factor);
int16_t used_bigits_;
int16_t exponent_;
Chunk bigits_buffer_[kBigitCapacity];
DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(Bignum);
};
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_BIGNUM_H_
#include <algorithm>
#include <cstring>
namespace double_conversion {
Bignum::Chunk& Bignum::RawBigit(const int index) {
DOUBLE_CONVERSION_ASSERT(static_cast<unsigned>(index) < kBigitCapacity);
return bigits_buffer_[index];
}
const Bignum::Chunk& Bignum::RawBigit(const int index) const {
DOUBLE_CONVERSION_ASSERT(static_cast<unsigned>(index) < kBigitCapacity);
return bigits_buffer_[index];
}
template<typename S>
static int BitSize(const S value) {
(void) value;  // Mark variable as used.
return 8 * sizeof(value);
}
void Bignum::AssignUInt16(const uint16_t value) {
DOUBLE_CONVERSION_ASSERT(kBigitSize >= BitSize(value));
Zero();
if (value > 0) {
RawBigit(0) = value;
used_bigits_ = 1;
}
}
void Bignum::AssignUInt64(uint64_t value) {
Zero();
for(int i = 0; value > 0; ++i) {
RawBigit(i) = value & kBigitMask;
value >>= kBigitSize;
++used_bigits_;
}
}
void Bignum::AssignBignum(const Bignum& other) {
exponent_ = other.exponent_;
for (int i = 0; i < other.used_bigits_; ++i) {
RawBigit(i) = other.RawBigit(i);
}
used_bigits_ = other.used_bigits_;
}
static uint64_t ReadUInt64(const Vector<const char> buffer,
const int from,
const int digits_to_read) {
uint64_t result = 0;
for (int i = from; i < from + digits_to_read; ++i) {
const int digit = buffer[i] - '0';
DOUBLE_CONVERSION_ASSERT(0 <= digit && digit <= 9);
result = result * 10 + digit;
}
return result;
}
void Bignum::AssignDecimalString(const Vector<const char> value) {
static const int kMaxUint64DecimalDigits = 19;
Zero();
int length = value.length();
unsigned pos = 0;
while (length >= kMaxUint64DecimalDigits) {
const uint64_t digits = ReadUInt64(value, pos, kMaxUint64DecimalDigits);
pos += kMaxUint64DecimalDigits;
length -= kMaxUint64DecimalDigits;
MultiplyByPowerOfTen(kMaxUint64DecimalDigits);
AddUInt64(digits);
}
const uint64_t digits = ReadUInt64(value, pos, length);
MultiplyByPowerOfTen(length);
AddUInt64(digits);
Clamp();
}
static uint64_t HexCharValue(const int c) {
if ('0' <= c && c <= '9') {
return c - '0';
}
if ('a' <= c && c <= 'f') {
return 10 + c - 'a';
}
DOUBLE_CONVERSION_ASSERT('A' <= c && c <= 'F');
return 10 + c - 'A';
}
void Bignum::AssignHexString(Vector<const char> value) {
Zero();
EnsureCapacity(((value.length() * 4) + kBigitSize - 1) / kBigitSize);
DOUBLE_CONVERSION_ASSERT(sizeof(uint64_t) * 8 >= kBigitSize + 4);  // TODO: static_assert
uint64_t tmp = 0;
for (int cnt = 0; !value.is_empty(); value.pop_back()) {
tmp |= (HexCharValue(value.last()) << cnt);
if ((cnt += 4) >= kBigitSize) {
RawBigit(used_bigits_++) = (tmp & kBigitMask);
cnt -= kBigitSize;
tmp >>= kBigitSize;
}
}
if (tmp > 0) {
DOUBLE_CONVERSION_ASSERT(tmp <= kBigitMask);
RawBigit(used_bigits_++) = static_cast<Bignum::Chunk>(tmp & kBigitMask);
}
Clamp();
}
void Bignum::AddUInt64(const uint64_t operand) {
if (operand == 0) {
return;
}
Bignum other;
other.AssignUInt64(operand);
AddBignum(other);
}
void Bignum::AddBignum(const Bignum& other) {
DOUBLE_CONVERSION_ASSERT(IsClamped());
DOUBLE_CONVERSION_ASSERT(other.IsClamped());
Align(other);
EnsureCapacity(1 + (std::max)(BigitLength(), other.BigitLength()) - exponent_);
Chunk carry = 0;
int bigit_pos = other.exponent_ - exponent_;
DOUBLE_CONVERSION_ASSERT(bigit_pos >= 0);
for (int i = used_bigits_; i < bigit_pos; ++i) {
RawBigit(i) = 0;
}
for (int i = 0; i < other.used_bigits_; ++i) {
const Chunk my = (bigit_pos < used_bigits_) ? RawBigit(bigit_pos) : 0;
const Chunk sum = my + other.RawBigit(i) + carry;
RawBigit(bigit_pos) = sum & kBigitMask;
carry = sum >> kBigitSize;
++bigit_pos;
}
while (carry != 0) {
const Chunk my = (bigit_pos < used_bigits_) ? RawBigit(bigit_pos) : 0;
const Chunk sum = my + carry;
RawBigit(bigit_pos) = sum & kBigitMask;
carry = sum >> kBigitSize;
++bigit_pos;
}
used_bigits_ = static_cast<int16_t>(std::max(bigit_pos, static_cast<int>(used_bigits_)));
DOUBLE_CONVERSION_ASSERT(IsClamped());
}
void Bignum::SubtractBignum(const Bignum& other) {
DOUBLE_CONVERSION_ASSERT(IsClamped());
DOUBLE_CONVERSION_ASSERT(other.IsClamped());
DOUBLE_CONVERSION_ASSERT(LessEqual(other, *this));
Align(other);
const int offset = other.exponent_ - exponent_;
Chunk borrow = 0;
int i;
for (i = 0; i < other.used_bigits_; ++i) {
DOUBLE_CONVERSION_ASSERT((borrow == 0) || (borrow == 1));
const Chunk difference = RawBigit(i + offset) - other.RawBigit(i) - borrow;
RawBigit(i + offset) = difference & kBigitMask;
borrow = difference >> (kChunkSize - 1);
}
while (borrow != 0) {
const Chunk difference = RawBigit(i + offset) - borrow;
RawBigit(i + offset) = difference & kBigitMask;
borrow = difference >> (kChunkSize - 1);
++i;
}
Clamp();
}
void Bignum::ShiftLeft(const int shift_amount) {
if (used_bigits_ == 0) {
return;
}
exponent_ += static_cast<int16_t>(shift_amount / kBigitSize);
const int local_shift = shift_amount % kBigitSize;
EnsureCapacity(used_bigits_ + 1);
BigitsShiftLeft(local_shift);
}
void Bignum::MultiplyByUInt32(const uint32_t factor) {
if (factor == 1) {
return;
}
if (factor == 0) {
Zero();
return;
}
if (used_bigits_ == 0) {
return;
}
DOUBLE_CONVERSION_ASSERT(kDoubleChunkSize >= kBigitSize + 32 + 1);
DoubleChunk carry = 0;
for (int i = 0; i < used_bigits_; ++i) {
const DoubleChunk product = static_cast<DoubleChunk>(factor) * RawBigit(i) + carry;
RawBigit(i) = static_cast<Chunk>(product & kBigitMask);
carry = (product >> kBigitSize);
}
while (carry != 0) {
EnsureCapacity(used_bigits_ + 1);
RawBigit(used_bigits_) = carry & kBigitMask;
used_bigits_++;
carry >>= kBigitSize;
}
}
void Bignum::MultiplyByUInt64(const uint64_t factor) {
if (factor == 1) {
return;
}
if (factor == 0) {
Zero();
return;
}
if (used_bigits_ == 0) {
return;
}
DOUBLE_CONVERSION_ASSERT(kBigitSize < 32);
uint64_t carry = 0;
const uint64_t low = factor & 0xFFFFFFFF;
const uint64_t high = factor >> 32;
for (int i = 0; i < used_bigits_; ++i) {
const uint64_t product_low = low * RawBigit(i);
const uint64_t product_high = high * RawBigit(i);
const uint64_t tmp = (carry & kBigitMask) + product_low;
RawBigit(i) = tmp & kBigitMask;
carry = (carry >> kBigitSize) + (tmp >> kBigitSize) +
(product_high << (32 - kBigitSize));
}
while (carry != 0) {
EnsureCapacity(used_bigits_ + 1);
RawBigit(used_bigits_) = carry & kBigitMask;
used_bigits_++;
carry >>= kBigitSize;
}
}
void Bignum::MultiplyByPowerOfTen(const int exponent) {
static const uint64_t kFive27 = DOUBLE_CONVERSION_UINT64_2PART_C(0x6765c793, fa10079d);
static const uint16_t kFive1 = 5;
static const uint16_t kFive2 = kFive1 * 5;
static const uint16_t kFive3 = kFive2 * 5;
static const uint16_t kFive4 = kFive3 * 5;
static const uint16_t kFive5 = kFive4 * 5;
static const uint16_t kFive6 = kFive5 * 5;
static const uint32_t kFive7 = kFive6 * 5;
static const uint32_t kFive8 = kFive7 * 5;
static const uint32_t kFive9 = kFive8 * 5;
static const uint32_t kFive10 = kFive9 * 5;
static const uint32_t kFive11 = kFive10 * 5;
static const uint32_t kFive12 = kFive11 * 5;
static const uint32_t kFive13 = kFive12 * 5;
static const uint32_t kFive1_to_12[] =
{ kFive1, kFive2, kFive3, kFive4, kFive5, kFive6,
kFive7, kFive8, kFive9, kFive10, kFive11, kFive12 };
DOUBLE_CONVERSION_ASSERT(exponent >= 0);
if (exponent == 0) {
return;
}
if (used_bigits_ == 0) {
return;
}
int remaining_exponent = exponent;
while (remaining_exponent >= 27) {
MultiplyByUInt64(kFive27);
remaining_exponent -= 27;
}
while (remaining_exponent >= 13) {
MultiplyByUInt32(kFive13);
remaining_exponent -= 13;
}
if (remaining_exponent > 0) {
MultiplyByUInt32(kFive1_to_12[remaining_exponent - 1]);
}
ShiftLeft(exponent);
}
void Bignum::Square() {
DOUBLE_CONVERSION_ASSERT(IsClamped());
const int product_length = 2 * used_bigits_;
EnsureCapacity(product_length);
if ((1 << (2 * (kChunkSize - kBigitSize))) <= used_bigits_) {
DOUBLE_CONVERSION_UNIMPLEMENTED();
}
DoubleChunk accumulator = 0;
const int copy_offset = used_bigits_;
for (int i = 0; i < used_bigits_; ++i) {
RawBigit(copy_offset + i) = RawBigit(i);
}
for (int i = 0; i < used_bigits_; ++i) {
int bigit_index1 = i;
int bigit_index2 = 0;
while (bigit_index1 >= 0) {
const Chunk chunk1 = RawBigit(copy_offset + bigit_index1);
const Chunk chunk2 = RawBigit(copy_offset + bigit_index2);
accumulator += static_cast<DoubleChunk>(chunk1) * chunk2;
bigit_index1--;
bigit_index2++;
}
RawBigit(i) = static_cast<Chunk>(accumulator) & kBigitMask;
accumulator >>= kBigitSize;
}
for (int i = used_bigits_; i < product_length; ++i) {
int bigit_index1 = used_bigits_ - 1;
int bigit_index2 = i - bigit_index1;
while (bigit_index2 < used_bigits_) {
const Chunk chunk1 = RawBigit(copy_offset + bigit_index1);
const Chunk chunk2 = RawBigit(copy_offset + bigit_index2);
accumulator += static_cast<DoubleChunk>(chunk1) * chunk2;
bigit_index1--;
bigit_index2++;
}
RawBigit(i) = static_cast<Chunk>(accumulator) & kBigitMask;
accumulator >>= kBigitSize;
}
DOUBLE_CONVERSION_ASSERT(accumulator == 0);
used_bigits_ = static_cast<int16_t>(product_length);
exponent_ *= 2;
Clamp();
}
void Bignum::AssignPowerUInt16(uint16_t base, const int power_exponent) {
DOUBLE_CONVERSION_ASSERT(base != 0);
DOUBLE_CONVERSION_ASSERT(power_exponent >= 0);
if (power_exponent == 0) {
AssignUInt16(1);
return;
}
Zero();
int shifts = 0;
while ((base & 1) == 0) {
base >>= 1;
shifts++;
}
int bit_size = 0;
int tmp_base = base;
while (tmp_base != 0) {
tmp_base >>= 1;
bit_size++;
}
const int final_size = bit_size * power_exponent;
EnsureCapacity(final_size / kBigitSize + 2);
int mask = 1;
while (power_exponent >= mask) mask <<= 1;
mask >>= 2;
uint64_t this_value = base;
bool delayed_multiplication = false;
const uint64_t max_32bits = 0xFFFFFFFF;
while (mask != 0 && this_value <= max_32bits) {
this_value = this_value * this_value;
if ((power_exponent & mask) != 0) {
DOUBLE_CONVERSION_ASSERT(bit_size > 0);
const uint64_t base_bits_mask =
~((static_cast<uint64_t>(1) << (64 - bit_size)) - 1);
const bool high_bits_zero = (this_value & base_bits_mask) == 0;
if (high_bits_zero) {
this_value *= base;
} else {
delayed_multiplication = true;
}
}
mask >>= 1;
}
AssignUInt64(this_value);
if (delayed_multiplication) {
MultiplyByUInt32(base);
}
while (mask != 0) {
Square();
if ((power_exponent & mask) != 0) {
MultiplyByUInt32(base);
}
mask >>= 1;
}
ShiftLeft(shifts * power_exponent);
}
uint16_t Bignum::DivideModuloIntBignum(const Bignum& other) {
DOUBLE_CONVERSION_ASSERT(IsClamped());
DOUBLE_CONVERSION_ASSERT(other.IsClamped());
DOUBLE_CONVERSION_ASSERT(other.used_bigits_ > 0);
if (BigitLength() < other.BigitLength()) {
return 0;
}
Align(other);
uint16_t result = 0;
while (BigitLength() > other.BigitLength()) {
DOUBLE_CONVERSION_ASSERT(other.RawBigit(other.used_bigits_ - 1) >= ((1 << kBigitSize) / 16));
DOUBLE_CONVERSION_ASSERT(RawBigit(used_bigits_ - 1) < 0x10000);
result += static_cast<uint16_t>(RawBigit(used_bigits_ - 1));
SubtractTimes(other, RawBigit(used_bigits_ - 1));
}
DOUBLE_CONVERSION_ASSERT(BigitLength() == other.BigitLength());
const Chunk this_bigit = RawBigit(used_bigits_ - 1);
const Chunk other_bigit = other.RawBigit(other.used_bigits_ - 1);
if (other.used_bigits_ == 1) {
int quotient = this_bigit / other_bigit;
RawBigit(used_bigits_ - 1) = this_bigit - other_bigit * quotient;
DOUBLE_CONVERSION_ASSERT(quotient < 0x10000);
result += static_cast<uint16_t>(quotient);
Clamp();
return result;
}
const int division_estimate = this_bigit / (other_bigit + 1);
DOUBLE_CONVERSION_ASSERT(division_estimate < 0x10000);
result += static_cast<uint16_t>(division_estimate);
SubtractTimes(other, division_estimate);
if (other_bigit * (division_estimate + 1) > this_bigit) {
return result;
}
while (LessEqual(other, *this)) {
SubtractBignum(other);
result++;
}
return result;
}
template<typename S>
static int SizeInHexChars(S number) {
DOUBLE_CONVERSION_ASSERT(number > 0);
int result = 0;
while (number != 0) {
number >>= 4;
result++;
}
return result;
}
static char HexCharOfValue(const int value) {
DOUBLE_CONVERSION_ASSERT(0 <= value && value <= 16);
if (value < 10) {
return static_cast<char>(value + '0');
}
return static_cast<char>(value - 10 + 'A');
}
bool Bignum::ToHexString(char* buffer, const int buffer_size) const {
DOUBLE_CONVERSION_ASSERT(IsClamped());
DOUBLE_CONVERSION_ASSERT(kBigitSize % 4 == 0);
static const int kHexCharsPerBigit = kBigitSize / 4;
if (used_bigits_ == 0) {
if (buffer_size < 2) {
return false;
}
buffer[0] = '0';
buffer[1] = '\0';
return true;
}
const int needed_chars = (BigitLength() - 1) * kHexCharsPerBigit +
SizeInHexChars(RawBigit(used_bigits_ - 1)) + 1;
if (needed_chars > buffer_size) {
return false;
}
int string_index = needed_chars - 1;
buffer[string_index--] = '\0';
for (int i = 0; i < exponent_; ++i) {
for (int j = 0; j < kHexCharsPerBigit; ++j) {
buffer[string_index--] = '0';
}
}
for (int i = 0; i < used_bigits_ - 1; ++i) {
Chunk current_bigit = RawBigit(i);
for (int j = 0; j < kHexCharsPerBigit; ++j) {
buffer[string_index--] = HexCharOfValue(current_bigit & 0xF);
current_bigit >>= 4;
}
}
Chunk most_significant_bigit = RawBigit(used_bigits_ - 1);
while (most_significant_bigit != 0) {
buffer[string_index--] = HexCharOfValue(most_significant_bigit & 0xF);
most_significant_bigit >>= 4;
}
return true;
}
Bignum::Chunk Bignum::BigitOrZero(const int index) const {
if (index >= BigitLength()) {
return 0;
}
if (index < exponent_) {
return 0;
}
return RawBigit(index - exponent_);
}
int Bignum::Compare(const Bignum& a, const Bignum& b) {
DOUBLE_CONVERSION_ASSERT(a.IsClamped());
DOUBLE_CONVERSION_ASSERT(b.IsClamped());
const int bigit_length_a = a.BigitLength();
const int bigit_length_b = b.BigitLength();
if (bigit_length_a < bigit_length_b) {
return -1;
}
if (bigit_length_a > bigit_length_b) {
return +1;
}
for (int i = bigit_length_a - 1; i >= (std::min)(a.exponent_, b.exponent_); --i) {
const Chunk bigit_a = a.BigitOrZero(i);
const Chunk bigit_b = b.BigitOrZero(i);
if (bigit_a < bigit_b) {
return -1;
}
if (bigit_a > bigit_b) {
return +1;
}
}
return 0;
}
int Bignum::PlusCompare(const Bignum& a, const Bignum& b, const Bignum& c) {
DOUBLE_CONVERSION_ASSERT(a.IsClamped());
DOUBLE_CONVERSION_ASSERT(b.IsClamped());
DOUBLE_CONVERSION_ASSERT(c.IsClamped());
if (a.BigitLength() < b.BigitLength()) {
return PlusCompare(b, a, c);
}
if (a.BigitLength() + 1 < c.BigitLength()) {
return -1;
}
if (a.BigitLength() > c.BigitLength()) {
return +1;
}
if (a.exponent_ >= b.BigitLength() && a.BigitLength() < c.BigitLength()) {
return -1;
}
Chunk borrow = 0;
const int min_exponent = (std::min)((std::min)(a.exponent_, b.exponent_), c.exponent_);
for (int i = c.BigitLength() - 1; i >= min_exponent; --i) {
const Chunk chunk_a = a.BigitOrZero(i);
const Chunk chunk_b = b.BigitOrZero(i);
const Chunk chunk_c = c.BigitOrZero(i);
const Chunk sum = chunk_a + chunk_b;
if (sum > chunk_c + borrow) {
return +1;
} else {
borrow = chunk_c + borrow - sum;
if (borrow > 1) {
return -1;
}
borrow <<= kBigitSize;
}
}
if (borrow == 0) {
return 0;
}
return -1;
}
void Bignum::Clamp() {
while (used_bigits_ > 0 && RawBigit(used_bigits_ - 1) == 0) {
used_bigits_--;
}
if (used_bigits_ == 0) {
exponent_ = 0;
}
}
void Bignum::Align(const Bignum& other) {
if (exponent_ > other.exponent_) {
const int zero_bigits = exponent_ - other.exponent_;
EnsureCapacity(used_bigits_ + zero_bigits);
for (int i = used_bigits_ - 1; i >= 0; --i) {
RawBigit(i + zero_bigits) = RawBigit(i);
}
for (int i = 0; i < zero_bigits; ++i) {
RawBigit(i) = 0;
}
used_bigits_ += static_cast<int16_t>(zero_bigits);
exponent_ -= static_cast<int16_t>(zero_bigits);
DOUBLE_CONVERSION_ASSERT(used_bigits_ >= 0);
DOUBLE_CONVERSION_ASSERT(exponent_ >= 0);
}
}
void Bignum::BigitsShiftLeft(const int shift_amount) {
DOUBLE_CONVERSION_ASSERT(shift_amount < kBigitSize);
DOUBLE_CONVERSION_ASSERT(shift_amount >= 0);
Chunk carry = 0;
for (int i = 0; i < used_bigits_; ++i) {
const Chunk new_carry = RawBigit(i) >> (kBigitSize - shift_amount);
RawBigit(i) = ((RawBigit(i) << shift_amount) + carry) & kBigitMask;
carry = new_carry;
}
if (carry != 0) {
RawBigit(used_bigits_) = carry;
used_bigits_++;
}
}
void Bignum::SubtractTimes(const Bignum& other, const int factor) {
DOUBLE_CONVERSION_ASSERT(exponent_ <= other.exponent_);
if (factor < 3) {
for (int i = 0; i < factor; ++i) {
SubtractBignum(other);
}
return;
}
Chunk borrow = 0;
const int exponent_diff = other.exponent_ - exponent_;
for (int i = 0; i < other.used_bigits_; ++i) {
const DoubleChunk product = static_cast<DoubleChunk>(factor) * other.RawBigit(i);
const DoubleChunk remove = borrow + product;
const Chunk difference = RawBigit(i + exponent_diff) - (remove & kBigitMask);
RawBigit(i + exponent_diff) = difference & kBigitMask;
borrow = static_cast<Chunk>((difference >> (kChunkSize - 1)) +
(remove >> kBigitSize));
}
for (int i = other.used_bigits_ + exponent_diff; i < used_bigits_; ++i) {
if (borrow == 0) {
return;
}
const Chunk difference = RawBigit(i) - borrow;
RawBigit(i) = difference & kBigitMask;
borrow = difference >> (kChunkSize - 1);
}
Clamp();
}
}  // namespace double_conversion


#ifndef DOUBLE_CONVERSION_CACHED_POWERS_H_
#define DOUBLE_CONVERSION_CACHED_POWERS_H_
namespace double_conversion {
namespace PowersOfTenCache {
static const int kDecimalExponentDistance = 8;
static const int kMinDecimalExponent = -348;
static const int kMaxDecimalExponent = 340;
void GetCachedPowerForBinaryExponentRange(int min_exponent,
int max_exponent,
DiyFp* power,
int* decimal_exponent);
void GetCachedPowerForDecimalExponent(int requested_exponent,
DiyFp* power,
int* found_exponent);
}  // namespace PowersOfTenCache
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_CACHED_POWERS_H_
#include <climits>
#include <cmath>
#include <cstdarg>
namespace double_conversion {
namespace PowersOfTenCache {
struct CachedPower {
uint64_t significand;
int16_t binary_exponent;
int16_t decimal_exponent;
};
static const CachedPower kCachedPowers[] = {
{DOUBLE_CONVERSION_UINT64_2PART_C(0xfa8fd5a0, 081c0288), -1220, -348},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xbaaee17f, a23ebf76), -1193, -340},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x8b16fb20, 3055ac76), -1166, -332},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xcf42894a, 5dce35ea), -1140, -324},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x9a6bb0aa, 55653b2d), -1113, -316},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xe61acf03, 3d1a45df), -1087, -308},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xab70fe17, c79ac6ca), -1060, -300},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xff77b1fc, bebcdc4f), -1034, -292},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xbe5691ef, 416bd60c), -1007, -284},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x8dd01fad, 907ffc3c), -980, -276},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xd3515c28, 31559a83), -954, -268},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x9d71ac8f, ada6c9b5), -927, -260},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xea9c2277, 23ee8bcb), -901, -252},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xaecc4991, 4078536d), -874, -244},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x823c1279, 5db6ce57), -847, -236},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xc2109436, 4dfb5637), -821, -228},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x9096ea6f, 3848984f), -794, -220},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xd77485cb, 25823ac7), -768, -212},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xa086cfcd, 97bf97f4), -741, -204},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xef340a98, 172aace5), -715, -196},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xb23867fb, 2a35b28e), -688, -188},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x84c8d4df, d2c63f3b), -661, -180},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xc5dd4427, 1ad3cdba), -635, -172},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x936b9fce, bb25c996), -608, -164},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xdbac6c24, 7d62a584), -582, -156},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xa3ab6658, 0d5fdaf6), -555, -148},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xf3e2f893, dec3f126), -529, -140},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xb5b5ada8, aaff80b8), -502, -132},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x87625f05, 6c7c4a8b), -475, -124},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xc9bcff60, 34c13053), -449, -116},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x964e858c, 91ba2655), -422, -108},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xdff97724, 70297ebd), -396, -100},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xa6dfbd9f, b8e5b88f), -369, -92},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xf8a95fcf, 88747d94), -343, -84},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xb9447093, 8fa89bcf), -316, -76},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x8a08f0f8, bf0f156b), -289, -68},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xcdb02555, 653131b6), -263, -60},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x993fe2c6, d07b7fac), -236, -52},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xe45c10c4, 2a2b3b06), -210, -44},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xaa242499, 697392d3), -183, -36},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xfd87b5f2, 8300ca0e), -157, -28},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xbce50864, 92111aeb), -130, -20},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x8cbccc09, 6f5088cc), -103, -12},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xd1b71758, e219652c), -77, -4},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x9c400000, 00000000), -50, 4},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xe8d4a510, 00000000), -24, 12},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xad78ebc5, ac620000), 3, 20},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x813f3978, f8940984), 30, 28},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xc097ce7b, c90715b3), 56, 36},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x8f7e32ce, 7bea5c70), 83, 44},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xd5d238a4, abe98068), 109, 52},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x9f4f2726, 179a2245), 136, 60},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xed63a231, d4c4fb27), 162, 68},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xb0de6538, 8cc8ada8), 189, 76},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x83c7088e, 1aab65db), 216, 84},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xc45d1df9, 42711d9a), 242, 92},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x924d692c, a61be758), 269, 100},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xda01ee64, 1a708dea), 295, 108},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xa26da399, 9aef774a), 322, 116},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xf209787b, b47d6b85), 348, 124},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xb454e4a1, 79dd1877), 375, 132},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x865b8692, 5b9bc5c2), 402, 140},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xc83553c5, c8965d3d), 428, 148},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x952ab45c, fa97a0b3), 455, 156},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xde469fbd, 99a05fe3), 481, 164},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xa59bc234, db398c25), 508, 172},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xf6c69a72, a3989f5c), 534, 180},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xb7dcbf53, 54e9bece), 561, 188},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x88fcf317, f22241e2), 588, 196},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xcc20ce9b, d35c78a5), 614, 204},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x98165af3, 7b2153df), 641, 212},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xe2a0b5dc, 971f303a), 667, 220},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xa8d9d153, 5ce3b396), 694, 228},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xfb9b7cd9, a4a7443c), 720, 236},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xbb764c4c, a7a44410), 747, 244},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x8bab8eef, b6409c1a), 774, 252},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xd01fef10, a657842c), 800, 260},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x9b10a4e5, e9913129), 827, 268},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xe7109bfb, a19c0c9d), 853, 276},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xac2820d9, 623bf429), 880, 284},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x80444b5e, 7aa7cf85), 907, 292},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xbf21e440, 03acdd2d), 933, 300},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x8e679c2f, 5e44ff8f), 960, 308},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xd433179d, 9c8cb841), 986, 316},
{DOUBLE_CONVERSION_UINT64_2PART_C(0x9e19db92, b4e31ba9), 1013, 324},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xeb96bf6e, badf77d9), 1039, 332},
{DOUBLE_CONVERSION_UINT64_2PART_C(0xaf87023b, 9bf0ee6b), 1066, 340},
};
static const int kCachedPowersOffset = 348;  // -1 * the first decimal_exponent.
static const double kD_1_LOG2_10 = 0.30102999566398114;  //  1 / lg(10)
void GetCachedPowerForBinaryExponentRange(
int min_exponent,
int max_exponent,
DiyFp* power,
int* decimal_exponent) {
int kQ = DiyFp::kSignificandSize;
double k = ceil((min_exponent + kQ - 1) * kD_1_LOG2_10);
int foo = kCachedPowersOffset;
int index =
(foo + static_cast<int>(k) - 1) / kDecimalExponentDistance + 1;
DOUBLE_CONVERSION_ASSERT(0 <= index && index < static_cast<int>(DOUBLE_CONVERSION_ARRAY_SIZE(kCachedPowers)));
CachedPower cached_power = kCachedPowers[index];
DOUBLE_CONVERSION_ASSERT(min_exponent <= cached_power.binary_exponent);
(void) max_exponent;  // Mark variable as used.
DOUBLE_CONVERSION_ASSERT(cached_power.binary_exponent <= max_exponent);
*decimal_exponent = cached_power.decimal_exponent;
*power = DiyFp(cached_power.significand, cached_power.binary_exponent);
}
void GetCachedPowerForDecimalExponent(int requested_exponent,
DiyFp* power,
int* found_exponent) {
DOUBLE_CONVERSION_ASSERT(kMinDecimalExponent <= requested_exponent);
DOUBLE_CONVERSION_ASSERT(requested_exponent < kMaxDecimalExponent + kDecimalExponentDistance);
int index =
(requested_exponent + kCachedPowersOffset) / kDecimalExponentDistance;
CachedPower cached_power = kCachedPowers[index];
*power = DiyFp(cached_power.significand, cached_power.binary_exponent);
*found_exponent = cached_power.decimal_exponent;
DOUBLE_CONVERSION_ASSERT(*found_exponent <= requested_exponent);
DOUBLE_CONVERSION_ASSERT(requested_exponent < *found_exponent + kDecimalExponentDistance);
}
}  // namespace PowersOfTenCache
}  // namespace double_conversion


namespace double_conversion {
#if defined(DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS)
static const int kMaxExactDoubleIntegerDecimalDigits = 15;
#endif // #if defined(DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS)
static const int kMaxUint64DecimalDigits = 19;
static const int kMaxDecimalPower = 309;
static const int kMinDecimalPower = -324;
static const uint64_t kMaxUint64 = DOUBLE_CONVERSION_UINT64_2PART_C(0xFFFFFFFF, FFFFFFFF);
#if defined(DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS)
static const double exact_powers_of_ten[] = {
1.0,  // 10^0
10.0,
100.0,
1000.0,
10000.0,
100000.0,
1000000.0,
10000000.0,
100000000.0,
1000000000.0,
10000000000.0,  // 10^10
100000000000.0,
1000000000000.0,
10000000000000.0,
100000000000000.0,
1000000000000000.0,
10000000000000000.0,
100000000000000000.0,
1000000000000000000.0,
10000000000000000000.0,
100000000000000000000.0,  // 10^20
1000000000000000000000.0,
10000000000000000000000.0
};
static const int kExactPowersOfTenSize = DOUBLE_CONVERSION_ARRAY_SIZE(exact_powers_of_ten);
#endif // #if defined(DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS)
static const int kMaxSignificantDecimalDigits = 780;
static Vector<const char> TrimLeadingZeros(Vector<const char> buffer) {
for (int i = 0; i < buffer.length(); i++) {
if (buffer[i] != '0') {
return buffer.SubVector(i, buffer.length());
}
}
return Vector<const char>(buffer.start(), 0);
}
static void CutToMaxSignificantDigits(Vector<const char> buffer,
int exponent,
char* significant_buffer,
int* significant_exponent) {
for (int i = 0; i < kMaxSignificantDecimalDigits - 1; ++i) {
significant_buffer[i] = buffer[i];
}
DOUBLE_CONVERSION_ASSERT(buffer[buffer.length() - 1] != '0');
significant_buffer[kMaxSignificantDecimalDigits - 1] = '1';
*significant_exponent =
exponent + (buffer.length() - kMaxSignificantDecimalDigits);
}
static void TrimAndCut(Vector<const char> buffer, int exponent,
char* buffer_copy_space, int space_size,
Vector<const char>* trimmed, int* updated_exponent) {
Vector<const char> left_trimmed = TrimLeadingZeros(buffer);
Vector<const char> right_trimmed = TrimTrailingZeros(left_trimmed);
exponent += left_trimmed.length() - right_trimmed.length();
if (right_trimmed.length() > kMaxSignificantDecimalDigits) {
(void) space_size;  // Mark variable as used.
DOUBLE_CONVERSION_ASSERT(space_size >= kMaxSignificantDecimalDigits);
CutToMaxSignificantDigits(right_trimmed, exponent,
buffer_copy_space, updated_exponent);
*trimmed = Vector<const char>(buffer_copy_space,
kMaxSignificantDecimalDigits);
} else {
*trimmed = right_trimmed;
*updated_exponent = exponent;
}
}
static uint64_t ReadUint64(Vector<const char> buffer,
int* number_of_read_digits) {
uint64_t result = 0;
int i = 0;
while (i < buffer.length() && result <= (kMaxUint64 / 10 - 1)) {
int digit = buffer[i++] - '0';
DOUBLE_CONVERSION_ASSERT(0 <= digit && digit <= 9);
result = 10 * result + digit;
}
*number_of_read_digits = i;
return result;
}
static void ReadDiyFp(Vector<const char> buffer,
DiyFp* result,
int* remaining_decimals) {
int read_digits;
uint64_t significand = ReadUint64(buffer, &read_digits);
if (buffer.length() == read_digits) {
*result = DiyFp(significand, 0);
*remaining_decimals = 0;
} else {
if (buffer[read_digits] >= '5') {
significand++;
}
int exponent = 0;
*result = DiyFp(significand, exponent);
*remaining_decimals = buffer.length() - read_digits;
}
}
static bool DoubleStrtod(Vector<const char> trimmed,
int exponent,
double* result) {
#if !defined(DOUBLE_CONVERSION_CORRECT_DOUBLE_OPERATIONS)
(void) trimmed;
(void) exponent;
(void) result;
return false;
#else
if (trimmed.length() <= kMaxExactDoubleIntegerDecimalDigits) {
int read_digits;
if (exponent < 0 && -exponent < kExactPowersOfTenSize) {
*result = static_cast<double>(ReadUint64(trimmed, &read_digits));
DOUBLE_CONVERSION_ASSERT(read_digits == trimmed.length());
*result /= exact_powers_of_ten[-exponent];
return true;
}
if (0 <= exponent && exponent < kExactPowersOfTenSize) {
*result = static_cast<double>(ReadUint64(trimmed, &read_digits));
DOUBLE_CONVERSION_ASSERT(read_digits == trimmed.length());
*result *= exact_powers_of_ten[exponent];
return true;
}
int remaining_digits =
kMaxExactDoubleIntegerDecimalDigits - trimmed.length();
if ((0 <= exponent) &&
(exponent - remaining_digits < kExactPowersOfTenSize)) {
*result = static_cast<double>(ReadUint64(trimmed, &read_digits));
DOUBLE_CONVERSION_ASSERT(read_digits == trimmed.length());
*result *= exact_powers_of_ten[remaining_digits];
*result *= exact_powers_of_ten[exponent - remaining_digits];
return true;
}
}
return false;
#endif
}
static DiyFp AdjustmentPowerOfTen(int exponent) {
DOUBLE_CONVERSION_ASSERT(0 < exponent);
DOUBLE_CONVERSION_ASSERT(exponent < PowersOfTenCache::kDecimalExponentDistance);
DOUBLE_CONVERSION_ASSERT(PowersOfTenCache::kDecimalExponentDistance == 8);
switch (exponent) {
case 1: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0xa0000000, 00000000), -60);
case 2: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0xc8000000, 00000000), -57);
case 3: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0xfa000000, 00000000), -54);
case 4: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0x9c400000, 00000000), -50);
case 5: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0xc3500000, 00000000), -47);
case 6: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0xf4240000, 00000000), -44);
case 7: return DiyFp(DOUBLE_CONVERSION_UINT64_2PART_C(0x98968000, 00000000), -40);
default:
DOUBLE_CONVERSION_UNREACHABLE();
}
}
static bool DiyFpStrtod(Vector<const char> buffer,
int exponent,
double* result) {
DiyFp input;
int remaining_decimals;
ReadDiyFp(buffer, &input, &remaining_decimals);
const int kDenominatorLog = 3;
const int kDenominator = 1 << kDenominatorLog;
exponent += remaining_decimals;
uint64_t error = (remaining_decimals == 0 ? 0 : kDenominator / 2);
int old_e = input.e();
input.Normalize();
error <<= old_e - input.e();
DOUBLE_CONVERSION_ASSERT(exponent <= PowersOfTenCache::kMaxDecimalExponent);
if (exponent < PowersOfTenCache::kMinDecimalExponent) {
*result = 0.0;
return true;
}
DiyFp cached_power;
int cached_decimal_exponent;
PowersOfTenCache::GetCachedPowerForDecimalExponent(exponent,
&cached_power,
&cached_decimal_exponent);
if (cached_decimal_exponent != exponent) {
int adjustment_exponent = exponent - cached_decimal_exponent;
DiyFp adjustment_power = AdjustmentPowerOfTen(adjustment_exponent);
input.Multiply(adjustment_power);
if (kMaxUint64DecimalDigits - buffer.length() >= adjustment_exponent) {
DOUBLE_CONVERSION_ASSERT(DiyFp::kSignificandSize == 64);
} else {
error += kDenominator / 2;
}
}
input.Multiply(cached_power);
int error_b = kDenominator / 2;
int error_ab = (error == 0 ? 0 : 1);  // We round up to 1.
int fixed_error = kDenominator / 2;
error += error_b + error_ab + fixed_error;
old_e = input.e();
input.Normalize();
error <<= old_e - input.e();
int order_of_magnitude = DiyFp::kSignificandSize + input.e();
int effective_significand_size =
Double::SignificandSizeForOrderOfMagnitude(order_of_magnitude);
int precision_digits_count =
DiyFp::kSignificandSize - effective_significand_size;
if (precision_digits_count + kDenominatorLog >= DiyFp::kSignificandSize) {
int shift_amount = (precision_digits_count + kDenominatorLog) -
DiyFp::kSignificandSize + 1;
input.set_f(input.f() >> shift_amount);
input.set_e(input.e() + shift_amount);
error = (error >> shift_amount) + 1 + kDenominator;
precision_digits_count -= shift_amount;
}
DOUBLE_CONVERSION_ASSERT(DiyFp::kSignificandSize == 64);
DOUBLE_CONVERSION_ASSERT(precision_digits_count < 64);
uint64_t one64 = 1;
uint64_t precision_bits_mask = (one64 << precision_digits_count) - 1;
uint64_t precision_bits = input.f() & precision_bits_mask;
uint64_t half_way = one64 << (precision_digits_count - 1);
precision_bits *= kDenominator;
half_way *= kDenominator;
DiyFp rounded_input(input.f() >> precision_digits_count,
input.e() + precision_digits_count);
if (precision_bits >= half_way + error) {
rounded_input.set_f(rounded_input.f() + 1);
}
*result = Double(rounded_input).value();
if (half_way - error < precision_bits && precision_bits < half_way + error) {
return false;
} else {
return true;
}
}
static int CompareBufferWithDiyFp(Vector<const char> buffer,
int exponent,
DiyFp diy_fp) {
DOUBLE_CONVERSION_ASSERT(buffer.length() + exponent <= kMaxDecimalPower + 1);
DOUBLE_CONVERSION_ASSERT(buffer.length() + exponent > kMinDecimalPower);
DOUBLE_CONVERSION_ASSERT(buffer.length() <= kMaxSignificantDecimalDigits);
DOUBLE_CONVERSION_ASSERT(((kMaxDecimalPower + 1) * 333 / 100) < Bignum::kMaxSignificantBits);
Bignum buffer_bignum;
Bignum diy_fp_bignum;
buffer_bignum.AssignDecimalString(buffer);
diy_fp_bignum.AssignUInt64(diy_fp.f());
if (exponent >= 0) {
buffer_bignum.MultiplyByPowerOfTen(exponent);
} else {
diy_fp_bignum.MultiplyByPowerOfTen(-exponent);
}
if (diy_fp.e() > 0) {
diy_fp_bignum.ShiftLeft(diy_fp.e());
} else {
buffer_bignum.ShiftLeft(-diy_fp.e());
}
return Bignum::Compare(buffer_bignum, diy_fp_bignum);
}
static bool ComputeGuess(Vector<const char> trimmed, int exponent,
double* guess) {
if (trimmed.length() == 0) {
*guess = 0.0;
return true;
}
if (exponent + trimmed.length() - 1 >= kMaxDecimalPower) {
*guess = Double::Infinity();
return true;
}
if (exponent + trimmed.length() <= kMinDecimalPower) {
*guess = 0.0;
return true;
}
if (DoubleStrtod(trimmed, exponent, guess) ||
DiyFpStrtod(trimmed, exponent, guess)) {
return true;
}
if (*guess == Double::Infinity()) {
return true;
}
return false;
}
static bool IsDigit(const char d) {
return ('0' <= d) && (d <= '9');
}
static bool IsNonZeroDigit(const char d) {
return ('1' <= d) && (d <= '9');
}
#ifdef __has_cpp_attribute
#if __has_cpp_attribute(maybe_unused)
[[maybe_unused]]
#endif
#endif
static bool AssertTrimmedDigits(const Vector<const char>& buffer) {
for(int i = 0; i < buffer.length(); ++i) {
if(!IsDigit(buffer[i])) {
return false;
}
}
return (buffer.length() == 0) || (IsNonZeroDigit(buffer[0]) && IsNonZeroDigit(buffer[buffer.length()-1]));
}
double StrtodTrimmed(Vector<const char> trimmed, int exponent) {
DOUBLE_CONVERSION_ASSERT(trimmed.length() <= kMaxSignificantDecimalDigits);
DOUBLE_CONVERSION_ASSERT(AssertTrimmedDigits(trimmed));
double guess;
const bool is_correct = ComputeGuess(trimmed, exponent, &guess);
if (is_correct) {
return guess;
}
DiyFp upper_boundary = Double(guess).UpperBoundary();
int comparison = CompareBufferWithDiyFp(trimmed, exponent, upper_boundary);
if (comparison < 0) {
return guess;
} else if (comparison > 0) {
return Double(guess).NextDouble();
} else if ((Double(guess).Significand() & 1) == 0) {
return guess;
} else {
return Double(guess).NextDouble();
}
}
double Strtod(Vector<const char> buffer, int exponent) {
char copy_buffer[kMaxSignificantDecimalDigits];
Vector<const char> trimmed;
int updated_exponent;
TrimAndCut(buffer, exponent, copy_buffer, kMaxSignificantDecimalDigits,
&trimmed, &updated_exponent);
return StrtodTrimmed(trimmed, updated_exponent);
}
static float SanitizedDoubletof(double d) {
DOUBLE_CONVERSION_ASSERT(d >= 0.0);
float max_finite = 3.4028234663852885981170418348451692544e+38;
double half_max_finite_infinity =
3.40282356779733661637539395458142568448e+38;
if (d >= max_finite) {
if (d >= half_max_finite_infinity) {
return Single::Infinity();
} else {
return max_finite;
}
} else {
return static_cast<float>(d);
}
}
float Strtof(Vector<const char> buffer, int exponent) {
char copy_buffer[kMaxSignificantDecimalDigits];
Vector<const char> trimmed;
int updated_exponent;
TrimAndCut(buffer, exponent, copy_buffer, kMaxSignificantDecimalDigits,
&trimmed, &updated_exponent);
exponent = updated_exponent;
return StrtofTrimmed(trimmed, exponent);
}
float StrtofTrimmed(Vector<const char> trimmed, int exponent) {
DOUBLE_CONVERSION_ASSERT(trimmed.length() <= kMaxSignificantDecimalDigits);
DOUBLE_CONVERSION_ASSERT(AssertTrimmedDigits(trimmed));
double double_guess;
bool is_correct = ComputeGuess(trimmed, exponent, &double_guess);
float float_guess = SanitizedDoubletof(double_guess);
if (float_guess == double_guess) {
return float_guess;
}
double double_next = Double(double_guess).NextDouble();
double double_previous = Double(double_guess).PreviousDouble();
float f1 = SanitizedDoubletof(double_previous);
float f2 = float_guess;
float f3 = SanitizedDoubletof(double_next);
float f4;
if (is_correct) {
f4 = f3;
} else {
double double_next2 = Double(double_next).NextDouble();
f4 = SanitizedDoubletof(double_next2);
}
(void) f2;  // Mark variable as used.
DOUBLE_CONVERSION_ASSERT(f1 <= f2 && f2 <= f3 && f3 <= f4);
if (f1 == f4) {
return float_guess;
}
DOUBLE_CONVERSION_ASSERT((f1 != f2 && f2 == f3 && f3 == f4) ||
(f1 == f2 && f2 != f3 && f3 == f4) ||
(f1 == f2 && f2 == f3 && f3 != f4));
float guess = f1;
float next = f4;
DiyFp upper_boundary;
if (guess == 0.0f) {
float min_float = 1e-45f;
upper_boundary = Double(static_cast<double>(min_float) / 2).AsDiyFp();
} else {
upper_boundary = Single(guess).UpperBoundary();
}
int comparison = CompareBufferWithDiyFp(trimmed, exponent, upper_boundary);
if (comparison < 0) {
return guess;
} else if (comparison > 0) {
return next;
} else if ((Single(guess).Significand() & 1) == 0) {
return guess;
} else {
return next;
}
}
}  // namespace double_conversion


#ifdef _MSC_VER
#  if _MSC_VER >= 1900
__pragma(warning(disable: 4244))
#  endif
#  if _MSC_VER <= 1700 // VS2012, see IsDecimalDigitForRadix warning fix, below
#    define VS2012_RADIXWARN
#  endif
#endif
namespace double_conversion {
namespace {
inline char ToLower(char ch) {
static const std::ctype<char>& cType =
std::use_facet<std::ctype<char> >(std::locale::classic());
return cType.tolower(ch);
}
inline char Pass(char ch) {
return ch;
}
template <class Iterator, class Converter>
static inline bool ConsumeSubStringImpl(Iterator* current,
Iterator end,
const char* substring,
Converter converter) {
DOUBLE_CONVERSION_ASSERT(converter(**current) == *substring);
for (substring++; *substring != '\0'; substring++) {
++*current;
if (*current == end || converter(**current) != *substring) {
return false;
}
}
++*current;
return true;
}
template <class Iterator>
static bool ConsumeSubString(Iterator* current,
Iterator end,
const char* substring,
bool allow_case_insensitivity) {
if (allow_case_insensitivity) {
return ConsumeSubStringImpl(current, end, substring, ToLower);
} else {
return ConsumeSubStringImpl(current, end, substring, Pass);
}
}
inline bool ConsumeFirstCharacter(char ch,
const char* str,
bool case_insensitivity) {
return case_insensitivity ? ToLower(ch) == str[0] : ch == str[0];
}
}  // namespace
const int kMaxSignificantDigits = 772;
static const char kWhitespaceTable7[] = { 32, 13, 10, 9, 11, 12 };
static const int kWhitespaceTable7Length = DOUBLE_CONVERSION_ARRAY_SIZE(kWhitespaceTable7);
static const uc16 kWhitespaceTable16[] = {
160, 8232, 8233, 5760, 6158, 8192, 8193, 8194, 8195,
8196, 8197, 8198, 8199, 8200, 8201, 8202, 8239, 8287, 12288, 65279
};
static const int kWhitespaceTable16Length = DOUBLE_CONVERSION_ARRAY_SIZE(kWhitespaceTable16);
static bool isWhitespace(int x) {
if (x < 128) {
for (int i = 0; i < kWhitespaceTable7Length; i++) {
if (kWhitespaceTable7[i] == x) return true;
}
} else {
for (int i = 0; i < kWhitespaceTable16Length; i++) {
if (kWhitespaceTable16[i] == x) return true;
}
}
return false;
}
template <class Iterator>
static inline bool AdvanceToNonspace(Iterator* current, Iterator end) {
while (*current != end) {
if (!isWhitespace(**current)) return true;
++*current;
}
return false;
}
static bool isDigit(int x, int radix) {
return (x >= '0' && x <= '9' && x < '0' + radix)
|| (radix > 10 && x >= 'a' && x < 'a' + radix - 10)
|| (radix > 10 && x >= 'A' && x < 'A' + radix - 10);
}
static double SignedZero(bool sign) {
return sign ? -0.0 : 0.0;
}
#ifdef VS2012_RADIXWARN
#pragma optimize("",off)
static bool IsDecimalDigitForRadix(int c, int radix) {
return '0' <= c && c <= '9' && (c - '0') < radix;
}
#pragma optimize("",on)
#else
static bool inline IsDecimalDigitForRadix(int c, int radix) {
return '0' <= c && c <= '9' && (c - '0') < radix;
}
#endif
static bool IsCharacterDigitForRadix(int c, int radix, char a_character) {
return radix > 10 && c >= a_character && c < a_character + radix - 10;
}
template<class Iterator>
static bool Advance (Iterator* it, uc16 separator, int base, Iterator& end) {
if (separator == StringToDoubleConverter::kNoSeparator) {
++(*it);
return *it == end;
}
if (!isDigit(**it, base)) {
++(*it);
return *it == end;
}
++(*it);
if (*it == end) return true;
if (*it + 1 == end) return false;
if (**it == separator && isDigit(*(*it + 1), base)) {
++(*it);
}
return *it == end;
}
template<class Iterator>
static bool IsHexFloatString(Iterator start,
Iterator end,
uc16 separator,
bool allow_trailing_junk) {
DOUBLE_CONVERSION_ASSERT(start != end);
Iterator current = start;
bool saw_digit = false;
while (isDigit(*current, 16)) {
saw_digit = true;
if (Advance(&current, separator, 16, end)) return false;
}
if (*current == '.') {
if (Advance(&current, separator, 16, end)) return false;
while (isDigit(*current, 16)) {
saw_digit = true;
if (Advance(&current, separator, 16, end)) return false;
}
}
if (!saw_digit) return false;
if (*current != 'p' && *current != 'P') return false;
if (Advance(&current, separator, 16, end)) return false;
if (*current == '+' || *current == '-') {
if (Advance(&current, separator, 16, end)) return false;
}
if (!isDigit(*current, 10)) return false;
if (Advance(&current, separator, 16, end)) return true;
while (isDigit(*current, 10)) {
if (Advance(&current, separator, 16, end)) return true;
}
return allow_trailing_junk || !AdvanceToNonspace(&current, end);
}
template <int radix_log_2, class Iterator>
static double RadixStringToIeee(Iterator* current,
Iterator end,
bool sign,
uc16 separator,
bool parse_as_hex_float,
bool allow_trailing_junk,
double junk_string_value,
bool read_as_double,
bool* result_is_junk) {
DOUBLE_CONVERSION_ASSERT(*current != end);
DOUBLE_CONVERSION_ASSERT(!parse_as_hex_float ||
IsHexFloatString(*current, end, separator, allow_trailing_junk));
const int kDoubleSize = Double::kSignificandSize;
const int kSingleSize = Single::kSignificandSize;
const int kSignificandSize = read_as_double? kDoubleSize: kSingleSize;
*result_is_junk = true;
int64_t number = 0;
int exponent = 0;
const int radix = (1 << radix_log_2);
bool post_decimal = false;
while (**current == '0') {
if (Advance(current, separator, radix, end)) {
*result_is_junk = false;
return SignedZero(sign);
}
}
while (true) {
int digit;
if (IsDecimalDigitForRadix(**current, radix)) {
digit = static_cast<char>(**current) - '0';
if (post_decimal) exponent -= radix_log_2;
} else if (IsCharacterDigitForRadix(**current, radix, 'a')) {
digit = static_cast<char>(**current) - 'a' + 10;
if (post_decimal) exponent -= radix_log_2;
} else if (IsCharacterDigitForRadix(**current, radix, 'A')) {
digit = static_cast<char>(**current) - 'A' + 10;
if (post_decimal) exponent -= radix_log_2;
} else if (parse_as_hex_float && **current == '.') {
post_decimal = true;
Advance(current, separator, radix, end);
DOUBLE_CONVERSION_ASSERT(*current != end);
continue;
} else if (parse_as_hex_float && (**current == 'p' || **current == 'P')) {
break;
} else {
if (allow_trailing_junk || !AdvanceToNonspace(current, end)) {
break;
} else {
return junk_string_value;
}
}
number = number * radix + digit;
int overflow = static_cast<int>(number >> kSignificandSize);
if (overflow != 0) {
int overflow_bits_count = 1;
while (overflow > 1) {
overflow_bits_count++;
overflow >>= 1;
}
int dropped_bits_mask = ((1 << overflow_bits_count) - 1);
int dropped_bits = static_cast<int>(number) & dropped_bits_mask;
number >>= overflow_bits_count;
exponent += overflow_bits_count;
bool zero_tail = true;
for (;;) {
if (Advance(current, separator, radix, end)) break;
if (parse_as_hex_float && **current == '.') {
Advance(current, separator, radix, end);
DOUBLE_CONVERSION_ASSERT(*current != end);
post_decimal = true;
}
if (!isDigit(**current, radix)) break;
zero_tail = zero_tail && **current == '0';
if (!post_decimal) exponent += radix_log_2;
}
if (!parse_as_hex_float &&
!allow_trailing_junk &&
AdvanceToNonspace(current, end)) {
return junk_string_value;
}
int middle_value = (1 << (overflow_bits_count - 1));
if (dropped_bits > middle_value) {
number++;  // Rounding up.
} else if (dropped_bits == middle_value) {
if ((number & 1) != 0 || !zero_tail) {
number++;  // Rounding up.
}
}
if ((number & ((int64_t)1 << kSignificandSize)) != 0) {
exponent++;
number >>= 1;
}
break;
}
if (Advance(current, separator, radix, end)) break;
}
DOUBLE_CONVERSION_ASSERT(number < ((int64_t)1 << kSignificandSize));
DOUBLE_CONVERSION_ASSERT(static_cast<int64_t>(static_cast<double>(number)) == number);
*result_is_junk = false;
if (parse_as_hex_float) {
DOUBLE_CONVERSION_ASSERT(**current == 'p' || **current == 'P');
Advance(current, separator, radix, end);
DOUBLE_CONVERSION_ASSERT(*current != end);
bool is_negative = false;
if (**current == '+') {
Advance(current, separator, radix, end);
DOUBLE_CONVERSION_ASSERT(*current != end);
} else if (**current == '-') {
is_negative = true;
Advance(current, separator, radix, end);
DOUBLE_CONVERSION_ASSERT(*current != end);
}
int written_exponent = 0;
while (IsDecimalDigitForRadix(**current, 10)) {
if (abs(written_exponent) <= 100 * Double::kMaxExponent) {
written_exponent = 10 * written_exponent + **current - '0';
}
if (Advance(current, separator, radix, end)) break;
}
if (is_negative) written_exponent = -written_exponent;
exponent += written_exponent;
}
if (exponent == 0 || number == 0) {
if (sign) {
if (number == 0) return -0.0;
number = -number;
}
return static_cast<double>(number);
}
DOUBLE_CONVERSION_ASSERT(number != 0);
double result = Double(DiyFp(number, exponent)).value();
return sign ? -result : result;
}
template <class Iterator>
double StringToDoubleConverter::StringToIeee(
Iterator input,
int length,
bool read_as_double,
int* processed_characters_count) const {
Iterator current = input;
Iterator end = input + length;
*processed_characters_count = 0;
const bool allow_trailing_junk = (flags_ & ALLOW_TRAILING_JUNK) != 0;
const bool allow_leading_spaces = (flags_ & ALLOW_LEADING_SPACES) != 0;
const bool allow_trailing_spaces = (flags_ & ALLOW_TRAILING_SPACES) != 0;
const bool allow_spaces_after_sign = (flags_ & ALLOW_SPACES_AFTER_SIGN) != 0;
const bool allow_case_insensitivity = (flags_ & ALLOW_CASE_INSENSITIVITY) != 0;
if (current == end) return empty_string_value_;
if (allow_leading_spaces || allow_trailing_spaces) {
if (!AdvanceToNonspace(&current, end)) {
*processed_characters_count = static_cast<int>(current - input);
return empty_string_value_;
}
if (!allow_leading_spaces && (input != current)) {
return junk_string_value_;
}
}
int exponent = 0;
int significant_digits = 0;
int insignificant_digits = 0;
bool nonzero_digit_dropped = false;
bool sign = false;
if (*current == '+' || *current == '-') {
sign = (*current == '-');
++current;
Iterator next_non_space = current;
if (!AdvanceToNonspace(&next_non_space, end)) return junk_string_value_;
if (!allow_spaces_after_sign && (current != next_non_space)) {
return junk_string_value_;
}
current = next_non_space;
}
if (infinity_symbol_ != DOUBLE_CONVERSION_NULLPTR) {
if (ConsumeFirstCharacter(*current, infinity_symbol_, allow_case_insensitivity)) {
if (!ConsumeSubString(&current, end, infinity_symbol_, allow_case_insensitivity)) {
return junk_string_value_;
}
if (!(allow_trailing_spaces || allow_trailing_junk) && (current != end)) {
return junk_string_value_;
}
if (!allow_trailing_junk && AdvanceToNonspace(&current, end)) {
return junk_string_value_;
}
*processed_characters_count = static_cast<int>(current - input);
return sign ? -Double::Infinity() : Double::Infinity();
}
}
if (nan_symbol_ != DOUBLE_CONVERSION_NULLPTR) {
if (ConsumeFirstCharacter(*current, nan_symbol_, allow_case_insensitivity)) {
if (!ConsumeSubString(&current, end, nan_symbol_, allow_case_insensitivity)) {
return junk_string_value_;
}
if (!(allow_trailing_spaces || allow_trailing_junk) && (current != end)) {
return junk_string_value_;
}
if (!allow_trailing_junk && AdvanceToNonspace(&current, end)) {
return junk_string_value_;
}
*processed_characters_count = static_cast<int>(current - input);
return sign ? -Double::NaN() : Double::NaN();
}
}
bool leading_zero = false;
if (*current == '0') {
if (Advance(&current, separator_, 10, end)) {
*processed_characters_count = static_cast<int>(current - input);
return SignedZero(sign);
}
leading_zero = true;
if (((flags_ & ALLOW_HEX) || (flags_ & ALLOW_HEX_FLOATS)) &&
(*current == 'x' || *current == 'X')) {
++current;
if (current == end) return junk_string_value_;  // "0x"
bool parse_as_hex_float = (flags_ & ALLOW_HEX_FLOATS) &&
IsHexFloatString(current, end, separator_, allow_trailing_junk);
if (!parse_as_hex_float && !isDigit(*current, 16)) {
return junk_string_value_;
}
bool result_is_junk;
double result = RadixStringToIeee<4>(&current,
end,
sign,
separator_,
parse_as_hex_float,
allow_trailing_junk,
junk_string_value_,
read_as_double,
&result_is_junk);
if (!result_is_junk) {
if (allow_trailing_spaces) AdvanceToNonspace(&current, end);
*processed_characters_count = static_cast<int>(current - input);
}
return result;
}
while (*current == '0') {
if (Advance(&current, separator_, 10, end)) {
*processed_characters_count = static_cast<int>(current - input);
return SignedZero(sign);
}
}
}
bool octal = leading_zero && (flags_ & ALLOW_OCTALS) != 0;
const int kBufferSize = kMaxSignificantDigits + 10;
DOUBLE_CONVERSION_STACK_UNINITIALIZED char
buffer[kBufferSize];  // NOLINT: size is known at compile time.
int buffer_pos = 0;
while (*current >= '0' && *current <= '9') {
if (significant_digits < kMaxSignificantDigits) {
DOUBLE_CONVERSION_ASSERT(buffer_pos < kBufferSize);
buffer[buffer_pos++] = static_cast<char>(*current);
significant_digits++;
} else {
insignificant_digits++;  // Move the digit into the exponential part.
nonzero_digit_dropped = nonzero_digit_dropped || *current != '0';
}
octal = octal && *current < '8';
if (Advance(&current, separator_, 10, end)) goto parsing_done;
}
if (significant_digits == 0) {
octal = false;
}
if (*current == '.') {
if (octal && !allow_trailing_junk) return junk_string_value_;
if (octal) goto parsing_done;
if (Advance(&current, separator_, 10, end)) {
if (significant_digits == 0 && !leading_zero) {
return junk_string_value_;
} else {
goto parsing_done;
}
}
if (significant_digits == 0) {
while (*current == '0') {
if (Advance(&current, separator_, 10, end)) {
*processed_characters_count = static_cast<int>(current - input);
return SignedZero(sign);
}
exponent--;  // Move this 0 into the exponent.
}
}
while (*current >= '0' && *current <= '9') {
if (significant_digits < kMaxSignificantDigits) {
DOUBLE_CONVERSION_ASSERT(buffer_pos < kBufferSize);
buffer[buffer_pos++] = static_cast<char>(*current);
significant_digits++;
exponent--;
} else {
nonzero_digit_dropped = nonzero_digit_dropped || *current != '0';
}
if (Advance(&current, separator_, 10, end)) goto parsing_done;
}
}
if (!leading_zero && exponent == 0 && significant_digits == 0) {
return junk_string_value_;
}
if (*current == 'e' || *current == 'E') {
if (octal && !allow_trailing_junk) return junk_string_value_;
if (octal) goto parsing_done;
Iterator junk_begin = current;
++current;
if (current == end) {
if (allow_trailing_junk) {
current = junk_begin;
goto parsing_done;
} else {
return junk_string_value_;
}
}
char exponen_sign = '+';
if (*current == '+' || *current == '-') {
exponen_sign = static_cast<char>(*current);
++current;
if (current == end) {
if (allow_trailing_junk) {
current = junk_begin;
goto parsing_done;
} else {
return junk_string_value_;
}
}
}
if (current == end || *current < '0' || *current > '9') {
if (allow_trailing_junk) {
current = junk_begin;
goto parsing_done;
} else {
return junk_string_value_;
}
}
const int max_exponent = INT_MAX / 2;
DOUBLE_CONVERSION_ASSERT(-max_exponent / 2 <= exponent && exponent <= max_exponent / 2);
int num = 0;
do {
int digit = *current - '0';
if (num >= max_exponent / 10
&& !(num == max_exponent / 10 && digit <= max_exponent % 10)) {
num = max_exponent;
} else {
num = num * 10 + digit;
}
++current;
} while (current != end && *current >= '0' && *current <= '9');
exponent += (exponen_sign == '-' ? -num : num);
}
if (!(allow_trailing_spaces || allow_trailing_junk) && (current != end)) {
return junk_string_value_;
}
if (!allow_trailing_junk && AdvanceToNonspace(&current, end)) {
return junk_string_value_;
}
if (allow_trailing_spaces) {
AdvanceToNonspace(&current, end);
}
parsing_done:
exponent += insignificant_digits;
if (octal) {
double result;
bool result_is_junk;
char* start = buffer;
result = RadixStringToIeee<3>(&start,
buffer + buffer_pos,
sign,
separator_,
false, // Don't parse as hex_float.
allow_trailing_junk,
junk_string_value_,
read_as_double,
&result_is_junk);
DOUBLE_CONVERSION_ASSERT(!result_is_junk);
*processed_characters_count = static_cast<int>(current - input);
return result;
}
if (nonzero_digit_dropped) {
buffer[buffer_pos++] = '1';
exponent--;
}
DOUBLE_CONVERSION_ASSERT(buffer_pos < kBufferSize);
buffer[buffer_pos] = '\0';
Vector<const char> chars(buffer, buffer_pos);
chars = TrimTrailingZeros(chars);
exponent += buffer_pos - chars.length();
double converted;
if (read_as_double) {
converted = StrtodTrimmed(chars, exponent);
} else {
converted = StrtofTrimmed(chars, exponent);
}
*processed_characters_count = static_cast<int>(current - input);
return sign? -converted: converted;
}
double StringToDoubleConverter::StringToDouble(
const char* buffer,
int length,
int* processed_characters_count) const {
return StringToIeee(buffer, length, true, processed_characters_count);
}
double StringToDoubleConverter::StringToDouble(
const uc16* buffer,
int length,
int* processed_characters_count) const {
return StringToIeee(buffer, length, true, processed_characters_count);
}
float StringToDoubleConverter::StringToFloat(
const char* buffer,
int length,
int* processed_characters_count) const {
return static_cast<float>(StringToIeee(buffer, length, false,
processed_characters_count));
}
float StringToDoubleConverter::StringToFloat(
const uc16* buffer,
int length,
int* processed_characters_count) const {
return static_cast<float>(StringToIeee(buffer, length, false,
processed_characters_count));
}
template<>
double StringToDoubleConverter::StringTo<double>(
const char* buffer,
int length,
int* processed_characters_count) const {
return StringToDouble(buffer, length, processed_characters_count);
}
template<>
float StringToDoubleConverter::StringTo<float>(
const char* buffer,
int length,
int* processed_characters_count) const {
return StringToFloat(buffer, length, processed_characters_count);
}
template<>
double StringToDoubleConverter::StringTo<double>(
const uc16* buffer,
int length,
int* processed_characters_count) const {
return StringToDouble(buffer, length, processed_characters_count);
}
template<>
float StringToDoubleConverter::StringTo<float>(
const uc16* buffer,
int length,
int* processed_characters_count) const {
return StringToFloat(buffer, length, processed_characters_count);
}
}  // namespace double_conversion


#ifndef DOUBLE_CONVERSION_DOUBLE_TO_STRING_H_
#define DOUBLE_CONVERSION_DOUBLE_TO_STRING_H_
namespace double_conversion {
class DoubleToStringConverter {
public:
static const int kMaxFixedDigitsBeforePoint = 60;
static const int kMaxFixedDigitsAfterPoint = 100;
static const int kMaxExponentialDigits = 120;
static const int kMinPrecisionDigits = 1;
static const int kMaxPrecisionDigits = 120;
static const int kBase10MaximalLength = 17;
static const int kBase10MaximalLengthSingle = 9;
static const int kMaxCharsEcmaScriptShortest = 25;
enum Flags {
NO_FLAGS = 0,
EMIT_POSITIVE_EXPONENT_SIGN = 1,
EMIT_TRAILING_DECIMAL_POINT = 2,
EMIT_TRAILING_ZERO_AFTER_POINT = 4,
UNIQUE_ZERO = 8,
NO_TRAILING_ZERO = 16
};
DoubleToStringConverter(int flags,
const char* infinity_symbol,
const char* nan_symbol,
char exponent_character,
int decimal_in_shortest_low,
int decimal_in_shortest_high,
int max_leading_padding_zeroes_in_precision_mode,
int max_trailing_padding_zeroes_in_precision_mode,
int min_exponent_width = 0)
: flags_(flags),
infinity_symbol_(infinity_symbol),
nan_symbol_(nan_symbol),
exponent_character_(exponent_character),
decimal_in_shortest_low_(decimal_in_shortest_low),
decimal_in_shortest_high_(decimal_in_shortest_high),
max_leading_padding_zeroes_in_precision_mode_(
max_leading_padding_zeroes_in_precision_mode),
max_trailing_padding_zeroes_in_precision_mode_(
max_trailing_padding_zeroes_in_precision_mode),
min_exponent_width_(min_exponent_width) {
DOUBLE_CONVERSION_ASSERT(((flags & EMIT_TRAILING_DECIMAL_POINT) != 0) ||
!((flags & EMIT_TRAILING_ZERO_AFTER_POINT) != 0));
}
static const DoubleToStringConverter& EcmaScriptConverter();
bool ToShortest(double value, StringBuilder* result_builder) const {
return ToShortestIeeeNumber(value, result_builder, SHORTEST);
}
bool ToShortestSingle(float value, StringBuilder* result_builder) const {
return ToShortestIeeeNumber(value, result_builder, SHORTEST_SINGLE);
}
bool ToFixed(double value,
int requested_digits,
StringBuilder* result_builder) const;
bool ToExponential(double value,
int requested_digits,
StringBuilder* result_builder) const;
bool ToPrecision(double value,
int precision,
StringBuilder* result_builder) const;
enum DtoaMode {
SHORTEST,
SHORTEST_SINGLE,
FIXED,
PRECISION
};
static void DoubleToAscii(double v,
DtoaMode mode,
int requested_digits,
char* buffer,
int buffer_length,
bool* sign,
int* length,
int* point);
private:
bool ToShortestIeeeNumber(double value,
StringBuilder* result_builder,
DtoaMode mode) const;
bool HandleSpecialValues(double value, StringBuilder* result_builder) const;
void CreateExponentialRepresentation(const char* decimal_digits,
int length,
int exponent,
StringBuilder* result_builder) const;
void CreateDecimalRepresentation(const char* decimal_digits,
int length,
int decimal_point,
int digits_after_point,
StringBuilder* result_builder) const;
const int flags_;
const char* const infinity_symbol_;
const char* const nan_symbol_;
const char exponent_character_;
const int decimal_in_shortest_low_;
const int decimal_in_shortest_high_;
const int max_leading_padding_zeroes_in_precision_mode_;
const int max_trailing_padding_zeroes_in_precision_mode_;
const int min_exponent_width_;
DOUBLE_CONVERSION_DISALLOW_IMPLICIT_CONSTRUCTORS(DoubleToStringConverter);
};
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_DOUBLE_TO_STRING_H_
#include <algorithm>
#include <climits>
#include <cmath>
#ifndef DOUBLE_CONVERSION_BIGNUM_DTOA_H_
#define DOUBLE_CONVERSION_BIGNUM_DTOA_H_
namespace double_conversion {
enum BignumDtoaMode {
BIGNUM_DTOA_SHORTEST,
BIGNUM_DTOA_SHORTEST_SINGLE,
BIGNUM_DTOA_FIXED,
BIGNUM_DTOA_PRECISION
};
void BignumDtoa(double v, BignumDtoaMode mode, int requested_digits,
Vector<char> buffer, int* length, int* point);
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_BIGNUM_DTOA_H_
#include <cmath>
namespace double_conversion {
static int NormalizedExponent(uint64_t significand, int exponent) {
DOUBLE_CONVERSION_ASSERT(significand != 0);
while ((significand & Double::kHiddenBit) == 0) {
significand = significand << 1;
exponent = exponent - 1;
}
return exponent;
}
static int EstimatePower(int exponent);
static void InitialScaledStartValues(uint64_t significand,
int exponent,
bool lower_boundary_is_closer,
int estimated_power,
bool need_boundary_deltas,
Bignum* numerator,
Bignum* denominator,
Bignum* delta_minus,
Bignum* delta_plus);
static void FixupMultiply10(int estimated_power, bool is_even,
int* decimal_point,
Bignum* numerator, Bignum* denominator,
Bignum* delta_minus, Bignum* delta_plus);
static void GenerateShortestDigits(Bignum* numerator, Bignum* denominator,
Bignum* delta_minus, Bignum* delta_plus,
bool is_even,
Vector<char> buffer, int* length);
static void BignumToFixed(int requested_digits, int* decimal_point,
Bignum* numerator, Bignum* denominator,
Vector<char> buffer, int* length);
static void GenerateCountedDigits(int count, int* decimal_point,
Bignum* numerator, Bignum* denominator,
Vector<char> buffer, int* length);
void BignumDtoa(double v, BignumDtoaMode mode, int requested_digits,
Vector<char> buffer, int* length, int* decimal_point) {
DOUBLE_CONVERSION_ASSERT(v > 0);
DOUBLE_CONVERSION_ASSERT(!Double(v).IsSpecial());
uint64_t significand;
int exponent;
bool lower_boundary_is_closer;
if (mode == BIGNUM_DTOA_SHORTEST_SINGLE) {
float f = static_cast<float>(v);
DOUBLE_CONVERSION_ASSERT(f == v);
significand = Single(f).Significand();
exponent = Single(f).Exponent();
lower_boundary_is_closer = Single(f).LowerBoundaryIsCloser();
} else {
significand = Double(v).Significand();
exponent = Double(v).Exponent();
lower_boundary_is_closer = Double(v).LowerBoundaryIsCloser();
}
bool need_boundary_deltas =
(mode == BIGNUM_DTOA_SHORTEST || mode == BIGNUM_DTOA_SHORTEST_SINGLE);
bool is_even = (significand & 1) == 0;
int normalized_exponent = NormalizedExponent(significand, exponent);
int estimated_power = EstimatePower(normalized_exponent);
if (mode == BIGNUM_DTOA_FIXED && -estimated_power - 1 > requested_digits) {
buffer[0] = '\0';
*length = 0;
*decimal_point = -requested_digits;
return;
}
Bignum numerator;
Bignum denominator;
Bignum delta_minus;
Bignum delta_plus;
DOUBLE_CONVERSION_ASSERT(Bignum::kMaxSignificantBits >= 324*4);
InitialScaledStartValues(significand, exponent, lower_boundary_is_closer,
estimated_power, need_boundary_deltas,
&numerator, &denominator,
&delta_minus, &delta_plus);
FixupMultiply10(estimated_power, is_even, decimal_point,
&numerator, &denominator,
&delta_minus, &delta_plus);
switch (mode) {
case BIGNUM_DTOA_SHORTEST:
case BIGNUM_DTOA_SHORTEST_SINGLE:
GenerateShortestDigits(&numerator, &denominator,
&delta_minus, &delta_plus,
is_even, buffer, length);
break;
case BIGNUM_DTOA_FIXED:
BignumToFixed(requested_digits, decimal_point,
&numerator, &denominator,
buffer, length);
break;
case BIGNUM_DTOA_PRECISION:
GenerateCountedDigits(requested_digits, decimal_point,
&numerator, &denominator,
buffer, length);
break;
default:
DOUBLE_CONVERSION_UNREACHABLE();
}
buffer[*length] = '\0';
}
static void GenerateShortestDigits(Bignum* numerator, Bignum* denominator,
Bignum* delta_minus, Bignum* delta_plus,
bool is_even,
Vector<char> buffer, int* length) {
if (Bignum::Equal(*delta_minus, *delta_plus)) {
delta_plus = delta_minus;
}
*length = 0;
for (;;) {
uint16_t digit;
digit = numerator->DivideModuloIntBignum(*denominator);
DOUBLE_CONVERSION_ASSERT(digit <= 9);  // digit is a uint16_t and therefore always positive.
buffer[(*length)++] = static_cast<char>(digit + '0');
bool in_delta_room_minus;
bool in_delta_room_plus;
if (is_even) {
in_delta_room_minus = Bignum::LessEqual(*numerator, *delta_minus);
} else {
in_delta_room_minus = Bignum::Less(*numerator, *delta_minus);
}
if (is_even) {
in_delta_room_plus =
Bignum::PlusCompare(*numerator, *delta_plus, *denominator) >= 0;
} else {
in_delta_room_plus =
Bignum::PlusCompare(*numerator, *delta_plus, *denominator) > 0;
}
if (!in_delta_room_minus && !in_delta_room_plus) {
numerator->Times10();
delta_minus->Times10();
if (delta_minus != delta_plus) {
delta_plus->Times10();
}
} else if (in_delta_room_minus && in_delta_room_plus) {
int compare = Bignum::PlusCompare(*numerator, *numerator, *denominator);
if (compare < 0) {
} else if (compare > 0) {
DOUBLE_CONVERSION_ASSERT(buffer[(*length) - 1] != '9');
buffer[(*length) - 1]++;
} else {
if ((buffer[(*length) - 1] - '0') % 2 == 0) {
} else {
DOUBLE_CONVERSION_ASSERT(buffer[(*length) - 1] != '9');
buffer[(*length) - 1]++;
}
}
return;
} else if (in_delta_room_minus) {
return;
} else {  // in_delta_room_plus
DOUBLE_CONVERSION_ASSERT(buffer[(*length) -1] != '9');
buffer[(*length) - 1]++;
return;
}
}
}
static void GenerateCountedDigits(int count, int* decimal_point,
Bignum* numerator, Bignum* denominator,
Vector<char> buffer, int* length) {
DOUBLE_CONVERSION_ASSERT(count >= 0);
for (int i = 0; i < count - 1; ++i) {
uint16_t digit;
digit = numerator->DivideModuloIntBignum(*denominator);
DOUBLE_CONVERSION_ASSERT(digit <= 9);  // digit is a uint16_t and therefore always positive.
buffer[i] = static_cast<char>(digit + '0');
numerator->Times10();
}
uint16_t digit;
digit = numerator->DivideModuloIntBignum(*denominator);
if (Bignum::PlusCompare(*numerator, *numerator, *denominator) >= 0) {
digit++;
}
DOUBLE_CONVERSION_ASSERT(digit <= 10);
buffer[count - 1] = static_cast<char>(digit + '0');
for (int i = count - 1; i > 0; --i) {
if (buffer[i] != '0' + 10) break;
buffer[i] = '0';
buffer[i - 1]++;
}
if (buffer[0] == '0' + 10) {
buffer[0] = '1';
(*decimal_point)++;
}
*length = count;
}
static void BignumToFixed(int requested_digits, int* decimal_point,
Bignum* numerator, Bignum* denominator,
Vector<char> buffer, int* length) {
if (-(*decimal_point) > requested_digits) {
*decimal_point = -requested_digits;
*length = 0;
return;
} else if (-(*decimal_point) == requested_digits) {
DOUBLE_CONVERSION_ASSERT(*decimal_point == -requested_digits);
denominator->Times10();
if (Bignum::PlusCompare(*numerator, *numerator, *denominator) >= 0) {
buffer[0] = '1';
*length = 1;
(*decimal_point)++;
} else {
*length = 0;
}
return;
} else {
int needed_digits = (*decimal_point) + requested_digits;
GenerateCountedDigits(needed_digits, decimal_point,
numerator, denominator,
buffer, length);
}
}
static int EstimatePower(int exponent) {
const double k1Log10 = 0.30102999566398114;  // 1/lg(10)
const int kSignificandSize = Double::kSignificandSize;
double estimate = ceil((exponent + kSignificandSize - 1) * k1Log10 - 1e-10);
return static_cast<int>(estimate);
}
static void InitialScaledStartValuesPositiveExponent(
uint64_t significand, int exponent,
int estimated_power, bool need_boundary_deltas,
Bignum* numerator, Bignum* denominator,
Bignum* delta_minus, Bignum* delta_plus) {
DOUBLE_CONVERSION_ASSERT(estimated_power >= 0);
numerator->AssignUInt64(significand);
numerator->ShiftLeft(exponent);
denominator->AssignPowerUInt16(10, estimated_power);
if (need_boundary_deltas) {
denominator->ShiftLeft(1);
numerator->ShiftLeft(1);
delta_plus->AssignUInt16(1);
delta_plus->ShiftLeft(exponent);
delta_minus->AssignUInt16(1);
delta_minus->ShiftLeft(exponent);
}
}
static void InitialScaledStartValuesNegativeExponentPositivePower(
uint64_t significand, int exponent,
int estimated_power, bool need_boundary_deltas,
Bignum* numerator, Bignum* denominator,
Bignum* delta_minus, Bignum* delta_plus) {
numerator->AssignUInt64(significand);
denominator->AssignPowerUInt16(10, estimated_power);
denominator->ShiftLeft(-exponent);
if (need_boundary_deltas) {
denominator->ShiftLeft(1);
numerator->ShiftLeft(1);
delta_plus->AssignUInt16(1);
delta_minus->AssignUInt16(1);
}
}
static void InitialScaledStartValuesNegativeExponentNegativePower(
uint64_t significand, int exponent,
int estimated_power, bool need_boundary_deltas,
Bignum* numerator, Bignum* denominator,
Bignum* delta_minus, Bignum* delta_plus) {
Bignum* power_ten = numerator;
power_ten->AssignPowerUInt16(10, -estimated_power);
if (need_boundary_deltas) {
delta_plus->AssignBignum(*power_ten);
delta_minus->AssignBignum(*power_ten);
}
DOUBLE_CONVERSION_ASSERT(numerator == power_ten);
numerator->MultiplyByUInt64(significand);
denominator->AssignUInt16(1);
denominator->ShiftLeft(-exponent);
if (need_boundary_deltas) {
numerator->ShiftLeft(1);
denominator->ShiftLeft(1);
}
}
static void InitialScaledStartValues(uint64_t significand,
int exponent,
bool lower_boundary_is_closer,
int estimated_power,
bool need_boundary_deltas,
Bignum* numerator,
Bignum* denominator,
Bignum* delta_minus,
Bignum* delta_plus) {
if (exponent >= 0) {
InitialScaledStartValuesPositiveExponent(
significand, exponent, estimated_power, need_boundary_deltas,
numerator, denominator, delta_minus, delta_plus);
} else if (estimated_power >= 0) {
InitialScaledStartValuesNegativeExponentPositivePower(
significand, exponent, estimated_power, need_boundary_deltas,
numerator, denominator, delta_minus, delta_plus);
} else {
InitialScaledStartValuesNegativeExponentNegativePower(
significand, exponent, estimated_power, need_boundary_deltas,
numerator, denominator, delta_minus, delta_plus);
}
if (need_boundary_deltas && lower_boundary_is_closer) {
denominator->ShiftLeft(1);  // *2
numerator->ShiftLeft(1);    // *2
delta_plus->ShiftLeft(1);   // *2
}
}
static void FixupMultiply10(int estimated_power, bool is_even,
int* decimal_point,
Bignum* numerator, Bignum* denominator,
Bignum* delta_minus, Bignum* delta_plus) {
bool in_range;
if (is_even) {
in_range = Bignum::PlusCompare(*numerator, *delta_plus, *denominator) >= 0;
} else {
in_range = Bignum::PlusCompare(*numerator, *delta_plus, *denominator) > 0;
}
if (in_range) {
*decimal_point = estimated_power + 1;
} else {
*decimal_point = estimated_power;
numerator->Times10();
if (Bignum::Equal(*delta_minus, *delta_plus)) {
delta_minus->Times10();
delta_plus->AssignBignum(*delta_minus);
} else {
delta_minus->Times10();
delta_plus->Times10();
}
}
}
}  // namespace double_conversion


#ifndef DOUBLE_CONVERSION_FAST_DTOA_H_
#define DOUBLE_CONVERSION_FAST_DTOA_H_
namespace double_conversion {
enum FastDtoaMode {
FAST_DTOA_SHORTEST,
FAST_DTOA_SHORTEST_SINGLE,
FAST_DTOA_PRECISION
};
static const int kFastDtoaMaximalLength = 17;
static const int kFastDtoaMaximalSingleLength = 9;
bool FastDtoa(double d,
FastDtoaMode mode,
int requested_digits,
Vector<char> buffer,
int* length,
int* decimal_point);
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_FAST_DTOA_H_
namespace double_conversion {
static const int kMinimalTargetExponent = -60;
static const int kMaximalTargetExponent = -32;
static bool RoundWeed(Vector<char> buffer,
int length,
uint64_t distance_too_high_w,
uint64_t unsafe_interval,
uint64_t rest,
uint64_t ten_kappa,
uint64_t unit) {
uint64_t small_distance = distance_too_high_w - unit;
uint64_t big_distance = distance_too_high_w + unit;
DOUBLE_CONVERSION_ASSERT(rest <= unsafe_interval);
while (rest < small_distance &&  // Negated condition 1
unsafe_interval - rest >= ten_kappa &&  // Negated condition 2
(rest + ten_kappa < small_distance ||  // buffer{-1} > w_high
small_distance - rest >= rest + ten_kappa - small_distance)) {
buffer[length - 1]--;
rest += ten_kappa;
}
if (rest < big_distance &&
unsafe_interval - rest >= ten_kappa &&
(rest + ten_kappa < big_distance ||
big_distance - rest > rest + ten_kappa - big_distance)) {
return false;
}
return (2 * unit <= rest) && (rest <= unsafe_interval - 4 * unit);
}
static bool RoundWeedCounted(Vector<char> buffer,
int length,
uint64_t rest,
uint64_t ten_kappa,
uint64_t unit,
int* kappa) {
DOUBLE_CONVERSION_ASSERT(rest < ten_kappa);
if (unit >= ten_kappa) return false;
if (ten_kappa - unit <= unit) return false;
if ((ten_kappa - rest > rest) && (ten_kappa - 2 * rest >= 2 * unit)) {
return true;
}
if ((rest > unit) && (ten_kappa - (rest - unit) <= (rest - unit))) {
buffer[length - 1]++;
for (int i = length - 1; i > 0; --i) {
if (buffer[i] != '0' + 10) break;
buffer[i] = '0';
buffer[i - 1]++;
}
if (buffer[0] == '0' + 10) {
buffer[0] = '1';
(*kappa) += 1;
}
return true;
}
return false;
}
static unsigned int const kSmallPowersOfTen[] =
{0, 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000,
1000000000};
static void BiggestPowerTen(uint32_t number,
int number_bits,
uint32_t* power,
int* exponent_plus_one) {
DOUBLE_CONVERSION_ASSERT(number < (1u << (number_bits + 1)));
int exponent_plus_one_guess = ((number_bits + 1) * 1233 >> 12);
exponent_plus_one_guess++;
if (number < kSmallPowersOfTen[exponent_plus_one_guess]) {
exponent_plus_one_guess--;
}
*power = kSmallPowersOfTen[exponent_plus_one_guess];
*exponent_plus_one = exponent_plus_one_guess;
}
static bool DigitGen(DiyFp low,
DiyFp w,
DiyFp high,
Vector<char> buffer,
int* length,
int* kappa) {
DOUBLE_CONVERSION_ASSERT(low.e() == w.e() && w.e() == high.e());
DOUBLE_CONVERSION_ASSERT(low.f() + 1 <= high.f() - 1);
DOUBLE_CONVERSION_ASSERT(kMinimalTargetExponent <= w.e() && w.e() <= kMaximalTargetExponent);
uint64_t unit = 1;
DiyFp too_low = DiyFp(low.f() - unit, low.e());
DiyFp too_high = DiyFp(high.f() + unit, high.e());
DiyFp unsafe_interval = DiyFp::Minus(too_high, too_low);
DiyFp one = DiyFp(static_cast<uint64_t>(1) << -w.e(), w.e());
uint32_t integrals = static_cast<uint32_t>(too_high.f() >> -one.e());
uint64_t fractionals = too_high.f() & (one.f() - 1);
uint32_t divisor;
int divisor_exponent_plus_one;
BiggestPowerTen(integrals, DiyFp::kSignificandSize - (-one.e()),
&divisor, &divisor_exponent_plus_one);
*kappa = divisor_exponent_plus_one;
*length = 0;
while (*kappa > 0) {
int digit = integrals / divisor;
DOUBLE_CONVERSION_ASSERT(digit <= 9);
buffer[*length] = static_cast<char>('0' + digit);
(*length)++;
integrals %= divisor;
(*kappa)--;
uint64_t rest =
(static_cast<uint64_t>(integrals) << -one.e()) + fractionals;
if (rest < unsafe_interval.f()) {
return RoundWeed(buffer, *length, DiyFp::Minus(too_high, w).f(),
unsafe_interval.f(), rest,
static_cast<uint64_t>(divisor) << -one.e(), unit);
}
divisor /= 10;
}
DOUBLE_CONVERSION_ASSERT(one.e() >= -60);
DOUBLE_CONVERSION_ASSERT(fractionals < one.f());
DOUBLE_CONVERSION_ASSERT(DOUBLE_CONVERSION_UINT64_2PART_C(0xFFFFFFFF, FFFFFFFF) / 10 >= one.f());
for (;;) {
fractionals *= 10;
unit *= 10;
unsafe_interval.set_f(unsafe_interval.f() * 10);
int digit = static_cast<int>(fractionals >> -one.e());
DOUBLE_CONVERSION_ASSERT(digit <= 9);
buffer[*length] = static_cast<char>('0' + digit);
(*length)++;
fractionals &= one.f() - 1;  // Modulo by one.
(*kappa)--;
if (fractionals < unsafe_interval.f()) {
return RoundWeed(buffer, *length, DiyFp::Minus(too_high, w).f() * unit,
unsafe_interval.f(), fractionals, one.f(), unit);
}
}
}
static bool DigitGenCounted(DiyFp w,
int requested_digits,
Vector<char> buffer,
int* length,
int* kappa) {
DOUBLE_CONVERSION_ASSERT(kMinimalTargetExponent <= w.e() && w.e() <= kMaximalTargetExponent);
DOUBLE_CONVERSION_ASSERT(kMinimalTargetExponent >= -60);
DOUBLE_CONVERSION_ASSERT(kMaximalTargetExponent <= -32);
uint64_t w_error = 1;
DiyFp one = DiyFp(static_cast<uint64_t>(1) << -w.e(), w.e());
uint32_t integrals = static_cast<uint32_t>(w.f() >> -one.e());
uint64_t fractionals = w.f() & (one.f() - 1);
uint32_t divisor;
int divisor_exponent_plus_one;
BiggestPowerTen(integrals, DiyFp::kSignificandSize - (-one.e()),
&divisor, &divisor_exponent_plus_one);
*kappa = divisor_exponent_plus_one;
*length = 0;
while (*kappa > 0) {
int digit = integrals / divisor;
DOUBLE_CONVERSION_ASSERT(digit <= 9);
buffer[*length] = static_cast<char>('0' + digit);
(*length)++;
requested_digits--;
integrals %= divisor;
(*kappa)--;
if (requested_digits == 0) break;
divisor /= 10;
}
if (requested_digits == 0) {
uint64_t rest =
(static_cast<uint64_t>(integrals) << -one.e()) + fractionals;
return RoundWeedCounted(buffer, *length, rest,
static_cast<uint64_t>(divisor) << -one.e(), w_error,
kappa);
}
DOUBLE_CONVERSION_ASSERT(one.e() >= -60);
DOUBLE_CONVERSION_ASSERT(fractionals < one.f());
DOUBLE_CONVERSION_ASSERT(DOUBLE_CONVERSION_UINT64_2PART_C(0xFFFFFFFF, FFFFFFFF) / 10 >= one.f());
while (requested_digits > 0 && fractionals > w_error) {
fractionals *= 10;
w_error *= 10;
int digit = static_cast<int>(fractionals >> -one.e());
DOUBLE_CONVERSION_ASSERT(digit <= 9);
buffer[*length] = static_cast<char>('0' + digit);
(*length)++;
requested_digits--;
fractionals &= one.f() - 1;  // Modulo by one.
(*kappa)--;
}
if (requested_digits != 0) return false;
return RoundWeedCounted(buffer, *length, fractionals, one.f(), w_error,
kappa);
}
static bool Grisu3(double v,
FastDtoaMode mode,
Vector<char> buffer,
int* length,
int* decimal_exponent) {
DiyFp w = Double(v).AsNormalizedDiyFp();
DiyFp boundary_minus, boundary_plus;
if (mode == FAST_DTOA_SHORTEST) {
Double(v).NormalizedBoundaries(&boundary_minus, &boundary_plus);
} else {
DOUBLE_CONVERSION_ASSERT(mode == FAST_DTOA_SHORTEST_SINGLE);
float single_v = static_cast<float>(v);
Single(single_v).NormalizedBoundaries(&boundary_minus, &boundary_plus);
}
DOUBLE_CONVERSION_ASSERT(boundary_plus.e() == w.e());
DiyFp ten_mk;  // Cached power of ten: 10^-k
int mk;        // -k
int ten_mk_minimal_binary_exponent =
kMinimalTargetExponent - (w.e() + DiyFp::kSignificandSize);
int ten_mk_maximal_binary_exponent =
kMaximalTargetExponent - (w.e() + DiyFp::kSignificandSize);
PowersOfTenCache::GetCachedPowerForBinaryExponentRange(
ten_mk_minimal_binary_exponent,
ten_mk_maximal_binary_exponent,
&ten_mk, &mk);
DOUBLE_CONVERSION_ASSERT((kMinimalTargetExponent <= w.e() + ten_mk.e() +
DiyFp::kSignificandSize) &&
(kMaximalTargetExponent >= w.e() + ten_mk.e() +
DiyFp::kSignificandSize));
DiyFp scaled_w = DiyFp::Times(w, ten_mk);
DOUBLE_CONVERSION_ASSERT(scaled_w.e() ==
boundary_plus.e() + ten_mk.e() + DiyFp::kSignificandSize);
DiyFp scaled_boundary_minus = DiyFp::Times(boundary_minus, ten_mk);
DiyFp scaled_boundary_plus  = DiyFp::Times(boundary_plus,  ten_mk);
int kappa;
bool result = DigitGen(scaled_boundary_minus, scaled_w, scaled_boundary_plus,
buffer, length, &kappa);
*decimal_exponent = -mk + kappa;
return result;
}
static bool Grisu3Counted(double v,
int requested_digits,
Vector<char> buffer,
int* length,
int* decimal_exponent) {
DiyFp w = Double(v).AsNormalizedDiyFp();
DiyFp ten_mk;  // Cached power of ten: 10^-k
int mk;        // -k
int ten_mk_minimal_binary_exponent =
kMinimalTargetExponent - (w.e() + DiyFp::kSignificandSize);
int ten_mk_maximal_binary_exponent =
kMaximalTargetExponent - (w.e() + DiyFp::kSignificandSize);
PowersOfTenCache::GetCachedPowerForBinaryExponentRange(
ten_mk_minimal_binary_exponent,
ten_mk_maximal_binary_exponent,
&ten_mk, &mk);
DOUBLE_CONVERSION_ASSERT((kMinimalTargetExponent <= w.e() + ten_mk.e() +
DiyFp::kSignificandSize) &&
(kMaximalTargetExponent >= w.e() + ten_mk.e() +
DiyFp::kSignificandSize));
DiyFp scaled_w = DiyFp::Times(w, ten_mk);
int kappa;
bool result = DigitGenCounted(scaled_w, requested_digits,
buffer, length, &kappa);
*decimal_exponent = -mk + kappa;
return result;
}
bool FastDtoa(double v,
FastDtoaMode mode,
int requested_digits,
Vector<char> buffer,
int* length,
int* decimal_point) {
DOUBLE_CONVERSION_ASSERT(v > 0);
DOUBLE_CONVERSION_ASSERT(!Double(v).IsSpecial());
bool result = false;
int decimal_exponent = 0;
switch (mode) {
case FAST_DTOA_SHORTEST:
case FAST_DTOA_SHORTEST_SINGLE:
result = Grisu3(v, mode, buffer, length, &decimal_exponent);
break;
case FAST_DTOA_PRECISION:
result = Grisu3Counted(v, requested_digits,
buffer, length, &decimal_exponent);
break;
default:
DOUBLE_CONVERSION_UNREACHABLE();
}
if (result) {
*decimal_point = *length + decimal_exponent;
buffer[*length] = '\0';
}
return result;
}
}  // namespace double_conversion


#ifndef DOUBLE_CONVERSION_FIXED_DTOA_H_
#define DOUBLE_CONVERSION_FIXED_DTOA_H_
namespace double_conversion {
bool FastFixedDtoa(double v, int fractional_count,
Vector<char> buffer, int* length, int* decimal_point);
}  // namespace double_conversion
#endif  // DOUBLE_CONVERSION_FIXED_DTOA_H_
#include <cmath>
namespace double_conversion {
class UInt128 {
public:
UInt128() : high_bits_(0), low_bits_(0) { }
UInt128(uint64_t high, uint64_t low) : high_bits_(high), low_bits_(low) { }
void Multiply(uint32_t multiplicand) {
uint64_t accumulator;
accumulator = (low_bits_ & kMask32) * multiplicand;
uint32_t part = static_cast<uint32_t>(accumulator & kMask32);
accumulator >>= 32;
accumulator = accumulator + (low_bits_ >> 32) * multiplicand;
low_bits_ = (accumulator << 32) + part;
accumulator >>= 32;
accumulator = accumulator + (high_bits_ & kMask32) * multiplicand;
part = static_cast<uint32_t>(accumulator & kMask32);
accumulator >>= 32;
accumulator = accumulator + (high_bits_ >> 32) * multiplicand;
high_bits_ = (accumulator << 32) + part;
DOUBLE_CONVERSION_ASSERT((accumulator >> 32) == 0);
}
void Shift(int shift_amount) {
DOUBLE_CONVERSION_ASSERT(-64 <= shift_amount && shift_amount <= 64);
if (shift_amount == 0) {
return;
} else if (shift_amount == -64) {
high_bits_ = low_bits_;
low_bits_ = 0;
} else if (shift_amount == 64) {
low_bits_ = high_bits_;
high_bits_ = 0;
} else if (shift_amount <= 0) {
high_bits_ <<= -shift_amount;
high_bits_ += low_bits_ >> (64 + shift_amount);
low_bits_ <<= -shift_amount;
} else {
low_bits_ >>= shift_amount;
low_bits_ += high_bits_ << (64 - shift_amount);
high_bits_ >>= shift_amount;
}
}
int DivModPowerOf2(int power) {
if (power >= 64) {
int result = static_cast<int>(high_bits_ >> (power - 64));
high_bits_ -= static_cast<uint64_t>(result) << (power - 64);
return result;
} else {
uint64_t part_low = low_bits_ >> power;
uint64_t part_high = high_bits_ << (64 - power);
int result = static_cast<int>(part_low + part_high);
high_bits_ = 0;
low_bits_ -= part_low << power;
return result;
}
}
bool IsZero() const {
return high_bits_ == 0 && low_bits_ == 0;
}
int BitAt(int position) const {
if (position >= 64) {
return static_cast<int>(high_bits_ >> (position - 64)) & 1;
} else {
return static_cast<int>(low_bits_ >> position) & 1;
}
}
private:
static const uint64_t kMask32 = 0xFFFFFFFF;
uint64_t high_bits_;
uint64_t low_bits_;
};
static const int kDoubleSignificandSize = 53;  // Includes the hidden bit.
static void FillDigits32FixedLength(uint32_t number, int requested_length,
Vector<char> buffer, int* length) {
for (int i = requested_length - 1; i >= 0; --i) {
buffer[(*length) + i] = '0' + number % 10;
number /= 10;
}
*length += requested_length;
}
static void FillDigits32(uint32_t number, Vector<char> buffer, int* length) {
int number_length = 0;
while (number != 0) {
int digit = number % 10;
number /= 10;
buffer[(*length) + number_length] = static_cast<char>('0' + digit);
number_length++;
}
int i = *length;
int j = *length + number_length - 1;
while (i < j) {
char tmp = buffer[i];
buffer[i] = buffer[j];
buffer[j] = tmp;
i++;
j--;
}
*length += number_length;
}
static void FillDigits64FixedLength(uint64_t number,
Vector<char> buffer, int* length) {
const uint32_t kTen7 = 10000000;
uint32_t part2 = static_cast<uint32_t>(number % kTen7);
number /= kTen7;
uint32_t part1 = static_cast<uint32_t>(number % kTen7);
uint32_t part0 = static_cast<uint32_t>(number / kTen7);
FillDigits32FixedLength(part0, 3, buffer, length);
FillDigits32FixedLength(part1, 7, buffer, length);
FillDigits32FixedLength(part2, 7, buffer, length);
}
static void FillDigits64(uint64_t number, Vector<char> buffer, int* length) {
const uint32_t kTen7 = 10000000;
uint32_t part2 = static_cast<uint32_t>(number % kTen7);
number /= kTen7;
uint32_t part1 = static_cast<uint32_t>(number % kTen7);
uint32_t part0 = static_cast<uint32_t>(number / kTen7);
if (part0 != 0) {
FillDigits32(part0, buffer, length);
FillDigits32FixedLength(part1, 7, buffer, length);
FillDigits32FixedLength(part2, 7, buffer, length);
} else if (part1 != 0) {
FillDigits32(part1, buffer, length);
FillDigits32FixedLength(part2, 7, buffer, length);
} else {
FillDigits32(part2, buffer, length);
}
}
static void RoundUp(Vector<char> buffer, int* length, int* decimal_point) {
if (*length == 0) {
buffer[0] = '1';
*decimal_point = 1;
*length = 1;
return;
}
buffer[(*length) - 1]++;
for (int i = (*length) - 1; i > 0; --i) {
if (buffer[i] != '0' + 10) {
return;
}
buffer[i] = '0';
buffer[i - 1]++;
}
if (buffer[0] == '0' + 10) {
buffer[0] = '1';
(*decimal_point)++;
}
}
static void FillFractionals(uint64_t fractionals, int exponent,
int fractional_count, Vector<char> buffer,
int* length, int* decimal_point) {
DOUBLE_CONVERSION_ASSERT(-128 <= exponent && exponent <= 0);
if (-exponent <= 64) {
DOUBLE_CONVERSION_ASSERT(fractionals >> 56 == 0);
int point = -exponent;
for (int i = 0; i < fractional_count; ++i) {
if (fractionals == 0) break;
fractionals *= 5;
point--;
int digit = static_cast<int>(fractionals >> point);
DOUBLE_CONVERSION_ASSERT(digit <= 9);
buffer[*length] = static_cast<char>('0' + digit);
(*length)++;
fractionals -= static_cast<uint64_t>(digit) << point;
}
DOUBLE_CONVERSION_ASSERT(fractionals == 0 || point - 1 >= 0);
if ((fractionals != 0) && ((fractionals >> (point - 1)) & 1) == 1) {
RoundUp(buffer, length, decimal_point);
}
} else {  // We need 128 bits.
DOUBLE_CONVERSION_ASSERT(64 < -exponent && -exponent <= 128);
UInt128 fractionals128 = UInt128(fractionals, 0);
fractionals128.Shift(-exponent - 64);
int point = 128;
for (int i = 0; i < fractional_count; ++i) {
if (fractionals128.IsZero()) break;
fractionals128.Multiply(5);
point--;
int digit = fractionals128.DivModPowerOf2(point);
DOUBLE_CONVERSION_ASSERT(digit <= 9);
buffer[*length] = static_cast<char>('0' + digit);
(*length)++;
}
if (fractionals128.BitAt(point - 1) == 1) {
RoundUp(buffer, length, decimal_point);
}
}
}
static void TrimZeros(Vector<char> buffer, int* length, int* decimal_point) {
while (*length > 0 && buffer[(*length) - 1] == '0') {
(*length)--;
}
int first_non_zero = 0;
while (first_non_zero < *length && buffer[first_non_zero] == '0') {
first_non_zero++;
}
if (first_non_zero != 0) {
for (int i = first_non_zero; i < *length; ++i) {
buffer[i - first_non_zero] = buffer[i];
}
*length -= first_non_zero;
*decimal_point -= first_non_zero;
}
}
bool FastFixedDtoa(double v,
int fractional_count,
Vector<char> buffer,
int* length,
int* decimal_point) {
const uint32_t kMaxUInt32 = 0xFFFFFFFF;
uint64_t significand = Double(v).Significand();
int exponent = Double(v).Exponent();
if (exponent > 20) return false;
if (fractional_count > 20) return false;
*length = 0;
if (exponent + kDoubleSignificandSize > 64) {
const uint64_t kFive17 = DOUBLE_CONVERSION_UINT64_2PART_C(0xB1, A2BC2EC5);  // 5^17
uint64_t divisor = kFive17;
int divisor_power = 17;
uint64_t dividend = significand;
uint32_t quotient;
uint64_t remainder;
if (exponent > divisor_power) {
dividend <<= exponent - divisor_power;
quotient = static_cast<uint32_t>(dividend / divisor);
remainder = (dividend % divisor) << divisor_power;
} else {
divisor <<= divisor_power - exponent;
quotient = static_cast<uint32_t>(dividend / divisor);
remainder = (dividend % divisor) << exponent;
}
FillDigits32(quotient, buffer, length);
FillDigits64FixedLength(remainder, buffer, length);
*decimal_point = *length;
} else if (exponent >= 0) {
significand <<= exponent;
FillDigits64(significand, buffer, length);
*decimal_point = *length;
} else if (exponent > -kDoubleSignificandSize) {
uint64_t integrals = significand >> -exponent;
uint64_t fractionals = significand - (integrals << -exponent);
if (integrals > kMaxUInt32) {
FillDigits64(integrals, buffer, length);
} else {
FillDigits32(static_cast<uint32_t>(integrals), buffer, length);
}
*decimal_point = *length;
FillFractionals(fractionals, exponent, fractional_count,
buffer, length, decimal_point);
} else if (exponent < -128) {
DOUBLE_CONVERSION_ASSERT(fractional_count <= 20);
buffer[0] = '\0';
*length = 0;
*decimal_point = -fractional_count;
} else {
*decimal_point = 0;
FillFractionals(significand, exponent, fractional_count,
buffer, length, decimal_point);
}
TrimZeros(buffer, length, decimal_point);
buffer[*length] = '\0';
if ((*length) == 0) {
*decimal_point = -fractional_count;
}
return true;
}
}  // namespace double_conversion


namespace double_conversion {
const DoubleToStringConverter& DoubleToStringConverter::EcmaScriptConverter() {
int flags = UNIQUE_ZERO | EMIT_POSITIVE_EXPONENT_SIGN;
static DoubleToStringConverter converter(flags,
"Infinity",
"NaN",
'e',
-6, 21,
6, 0);
return converter;
}
bool DoubleToStringConverter::HandleSpecialValues(
double value,
StringBuilder* result_builder) const {
Double double_inspect(value);
if (double_inspect.IsInfinite()) {
if (infinity_symbol_ == DOUBLE_CONVERSION_NULLPTR) return false;
if (value < 0) {
result_builder->AddCharacter('-');
}
result_builder->AddString(infinity_symbol_);
return true;
}
if (double_inspect.IsNan()) {
if (nan_symbol_ == DOUBLE_CONVERSION_NULLPTR) return false;
result_builder->AddString(nan_symbol_);
return true;
}
return false;
}
void DoubleToStringConverter::CreateExponentialRepresentation(
const char* decimal_digits,
int length,
int exponent,
StringBuilder* result_builder) const {
DOUBLE_CONVERSION_ASSERT(length != 0);
result_builder->AddCharacter(decimal_digits[0]);
if (length != 1) {
result_builder->AddCharacter('.');
result_builder->AddSubstring(&decimal_digits[1], length-1);
}
result_builder->AddCharacter(exponent_character_);
if (exponent < 0) {
result_builder->AddCharacter('-');
exponent = -exponent;
} else {
if ((flags_ & EMIT_POSITIVE_EXPONENT_SIGN) != 0) {
result_builder->AddCharacter('+');
}
}
DOUBLE_CONVERSION_ASSERT(exponent < 1e4);
const int kMaxExponentLength = 5;
char buffer[kMaxExponentLength + 1];
buffer[kMaxExponentLength] = '\0';
int first_char_pos = kMaxExponentLength;
if (exponent == 0) {
buffer[--first_char_pos] = '0';
} else {
while (exponent > 0) {
buffer[--first_char_pos] = '0' + (exponent % 10);
exponent /= 10;
}
}
while(kMaxExponentLength - first_char_pos < std::min(min_exponent_width_, kMaxExponentLength)) {
buffer[--first_char_pos] = '0';
}
result_builder->AddSubstring(&buffer[first_char_pos],
kMaxExponentLength - first_char_pos);
}
void DoubleToStringConverter::CreateDecimalRepresentation(
const char* decimal_digits,
int length,
int decimal_point,
int digits_after_point,
StringBuilder* result_builder) const {
if (decimal_point <= 0) {
result_builder->AddCharacter('0');
if (digits_after_point > 0) {
result_builder->AddCharacter('.');
result_builder->AddPadding('0', -decimal_point);
DOUBLE_CONVERSION_ASSERT(length <= digits_after_point - (-decimal_point));
result_builder->AddSubstring(decimal_digits, length);
int remaining_digits = digits_after_point - (-decimal_point) - length;
result_builder->AddPadding('0', remaining_digits);
}
} else if (decimal_point >= length) {
result_builder->AddSubstring(decimal_digits, length);
result_builder->AddPadding('0', decimal_point - length);
if (digits_after_point > 0) {
result_builder->AddCharacter('.');
result_builder->AddPadding('0', digits_after_point);
}
} else {
DOUBLE_CONVERSION_ASSERT(digits_after_point > 0);
result_builder->AddSubstring(decimal_digits, decimal_point);
result_builder->AddCharacter('.');
DOUBLE_CONVERSION_ASSERT(length - decimal_point <= digits_after_point);
result_builder->AddSubstring(&decimal_digits[decimal_point],
length - decimal_point);
int remaining_digits = digits_after_point - (length - decimal_point);
result_builder->AddPadding('0', remaining_digits);
}
if (digits_after_point == 0) {
if ((flags_ & EMIT_TRAILING_DECIMAL_POINT) != 0) {
result_builder->AddCharacter('.');
}
if ((flags_ & EMIT_TRAILING_ZERO_AFTER_POINT) != 0) {
result_builder->AddCharacter('0');
}
}
}
bool DoubleToStringConverter::ToShortestIeeeNumber(
double value,
StringBuilder* result_builder,
DoubleToStringConverter::DtoaMode mode) const {
DOUBLE_CONVERSION_ASSERT(mode == SHORTEST || mode == SHORTEST_SINGLE);
if (Double(value).IsSpecial()) {
return HandleSpecialValues(value, result_builder);
}
int decimal_point;
bool sign;
const int kDecimalRepCapacity = kBase10MaximalLength + 1;
char decimal_rep[kDecimalRepCapacity];
int decimal_rep_length;
DoubleToAscii(value, mode, 0, decimal_rep, kDecimalRepCapacity,
&sign, &decimal_rep_length, &decimal_point);
bool unique_zero = (flags_ & UNIQUE_ZERO) != 0;
if (sign && (value != 0.0 || !unique_zero)) {
result_builder->AddCharacter('-');
}
int exponent = decimal_point - 1;
if ((decimal_in_shortest_low_ <= exponent) &&
(exponent < decimal_in_shortest_high_)) {
CreateDecimalRepresentation(decimal_rep, decimal_rep_length,
decimal_point,
(std::max)(0, decimal_rep_length - decimal_point),
result_builder);
} else {
CreateExponentialRepresentation(decimal_rep, decimal_rep_length, exponent,
result_builder);
}
return true;
}
bool DoubleToStringConverter::ToFixed(double value,
int requested_digits,
StringBuilder* result_builder) const {
DOUBLE_CONVERSION_ASSERT(kMaxFixedDigitsBeforePoint == 60);
const double kFirstNonFixed = 1e60;
if (Double(value).IsSpecial()) {
return HandleSpecialValues(value, result_builder);
}
if (requested_digits > kMaxFixedDigitsAfterPoint) return false;
if (value >= kFirstNonFixed || value <= -kFirstNonFixed) return false;
int decimal_point;
bool sign;
const int kDecimalRepCapacity =
kMaxFixedDigitsBeforePoint + kMaxFixedDigitsAfterPoint + 1;
char decimal_rep[kDecimalRepCapacity];
int decimal_rep_length;
DoubleToAscii(value, FIXED, requested_digits,
decimal_rep, kDecimalRepCapacity,
&sign, &decimal_rep_length, &decimal_point);
bool unique_zero = ((flags_ & UNIQUE_ZERO) != 0);
if (sign && (value != 0.0 || !unique_zero)) {
result_builder->AddCharacter('-');
}
CreateDecimalRepresentation(decimal_rep, decimal_rep_length, decimal_point,
requested_digits, result_builder);
return true;
}
bool DoubleToStringConverter::ToExponential(
double value,
int requested_digits,
StringBuilder* result_builder) const {
if (Double(value).IsSpecial()) {
return HandleSpecialValues(value, result_builder);
}
if (requested_digits < -1) return false;
if (requested_digits > kMaxExponentialDigits) return false;
int decimal_point;
bool sign;
const int kDecimalRepCapacity = kMaxExponentialDigits + 2;
DOUBLE_CONVERSION_ASSERT(kDecimalRepCapacity > kBase10MaximalLength);
char decimal_rep[kDecimalRepCapacity];
#ifndef NDEBUG
memset(decimal_rep, 0, sizeof(decimal_rep));
#endif
int decimal_rep_length;
if (requested_digits == -1) {
DoubleToAscii(value, SHORTEST, 0,
decimal_rep, kDecimalRepCapacity,
&sign, &decimal_rep_length, &decimal_point);
} else {
DoubleToAscii(value, PRECISION, requested_digits + 1,
decimal_rep, kDecimalRepCapacity,
&sign, &decimal_rep_length, &decimal_point);
DOUBLE_CONVERSION_ASSERT(decimal_rep_length <= requested_digits + 1);
for (int i = decimal_rep_length; i < requested_digits + 1; ++i) {
decimal_rep[i] = '0';
}
decimal_rep_length = requested_digits + 1;
}
bool unique_zero = ((flags_ & UNIQUE_ZERO) != 0);
if (sign && (value != 0.0 || !unique_zero)) {
result_builder->AddCharacter('-');
}
int exponent = decimal_point - 1;
CreateExponentialRepresentation(decimal_rep,
decimal_rep_length,
exponent,
result_builder);
return true;
}
bool DoubleToStringConverter::ToPrecision(double value,
int precision,
StringBuilder* result_builder) const {
if (Double(value).IsSpecial()) {
return HandleSpecialValues(value, result_builder);
}
if (precision < kMinPrecisionDigits || precision > kMaxPrecisionDigits) {
return false;
}
int decimal_point;
bool sign;
const int kDecimalRepCapacity = kMaxPrecisionDigits + 1;
char decimal_rep[kDecimalRepCapacity];
int decimal_rep_length;
DoubleToAscii(value, PRECISION, precision,
decimal_rep, kDecimalRepCapacity,
&sign, &decimal_rep_length, &decimal_point);
DOUBLE_CONVERSION_ASSERT(decimal_rep_length <= precision);
bool unique_zero = ((flags_ & UNIQUE_ZERO) != 0);
if (sign && (value != 0.0 || !unique_zero)) {
result_builder->AddCharacter('-');
}
int exponent = decimal_point - 1;
int extra_zero = ((flags_ & EMIT_TRAILING_ZERO_AFTER_POINT) != 0) ? 1 : 0;
bool as_exponential =
(-decimal_point + 1 > max_leading_padding_zeroes_in_precision_mode_) ||
(decimal_point - precision + extra_zero >
max_trailing_padding_zeroes_in_precision_mode_);
if ((flags_ & NO_TRAILING_ZERO) != 0) {
int stop = as_exponential ? 1 : std::max(1, decimal_point);
while (decimal_rep_length > stop && decimal_rep[decimal_rep_length - 1] == '0') {
--decimal_rep_length;
}
precision = std::min(precision, decimal_rep_length);
}
if (as_exponential) {
for (int i = decimal_rep_length; i < precision; ++i) {
decimal_rep[i] = '0';
}
CreateExponentialRepresentation(decimal_rep,
precision,
exponent,
result_builder);
} else {
CreateDecimalRepresentation(decimal_rep, decimal_rep_length, decimal_point,
(std::max)(0, precision - decimal_point),
result_builder);
}
return true;
}
static BignumDtoaMode DtoaToBignumDtoaMode(
DoubleToStringConverter::DtoaMode dtoa_mode) {
switch (dtoa_mode) {
case DoubleToStringConverter::SHORTEST:  return BIGNUM_DTOA_SHORTEST;
case DoubleToStringConverter::SHORTEST_SINGLE:
return BIGNUM_DTOA_SHORTEST_SINGLE;
case DoubleToStringConverter::FIXED:     return BIGNUM_DTOA_FIXED;
case DoubleToStringConverter::PRECISION: return BIGNUM_DTOA_PRECISION;
default:
DOUBLE_CONVERSION_UNREACHABLE();
}
}
void DoubleToStringConverter::DoubleToAscii(double v,
DtoaMode mode,
int requested_digits,
char* buffer,
int buffer_length,
bool* sign,
int* length,
int* point) {
Vector<char> vector(buffer, buffer_length);
DOUBLE_CONVERSION_ASSERT(!Double(v).IsSpecial());
DOUBLE_CONVERSION_ASSERT(mode == SHORTEST || mode == SHORTEST_SINGLE || requested_digits >= 0);
if (Double(v).Sign() < 0) {
*sign = true;
v = -v;
} else {
*sign = false;
}
if (mode == PRECISION && requested_digits == 0) {
vector[0] = '\0';
*length = 0;
return;
}
if (v == 0) {
vector[0] = '0';
vector[1] = '\0';
*length = 1;
*point = 1;
return;
}
bool fast_worked;
switch (mode) {
case SHORTEST:
fast_worked = FastDtoa(v, FAST_DTOA_SHORTEST, 0, vector, length, point);
break;
case SHORTEST_SINGLE:
fast_worked = FastDtoa(v, FAST_DTOA_SHORTEST_SINGLE, 0,
vector, length, point);
break;
case FIXED:
fast_worked = FastFixedDtoa(v, requested_digits, vector, length, point);
break;
case PRECISION:
fast_worked = FastDtoa(v, FAST_DTOA_PRECISION, requested_digits,
vector, length, point);
break;
default:
fast_worked = false;
DOUBLE_CONVERSION_UNREACHABLE();
}
if (fast_worked) return;
BignumDtoaMode bignum_mode = DtoaToBignumDtoaMode(mode);
BignumDtoa(v, bignum_mode, requested_digits, vector, length, point);
vector[*length] = '\0';
}
}  // namespace double_conversion


#endif  // DOUBLE_CONVERSION_DOUBLE_CONVERSION_H_

_WJR_ENCODE_BEGIN
namespace __string_func_traits {
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(do_skipw, do_skipw);
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(do_rskipw, do_rskipw);
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(do_skipz, do_skipz);
}
class __base_encode_common {
public:
template<typename T, typename Char>
WJR_INLINE_CONSTEXPR static void from_integral(T _Val,
basic_string_modifier<Char>& it, int base, errc& _Err) noexcept;
using from_f_mode = typename double_conversion::DoubleToStringConverter::DtoaMode;
using from_f_flags = typename double_conversion::DoubleToStringConverter::Flags;
using default_from_f_mode = std::integral_constant<
from_f_mode,
from_f_mode::SHORTEST
>;
using default_from_f_flags = std::integral_constant<
from_f_flags,
from_f_flags::NO_FLAGS
>;
template<typename T, typename Char,
typename M = default_from_f_mode,
typename F = default_from_f_flags>
WJR_INLINE static void from_floating_point(
T value, basic_string_modifier<Char>& it, errc& err, int precision = 0, M m = M(), F _Flags = F()) noexcept;
private:
inline constexpr static char charconv_lower[] =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e',
'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
inline constexpr static char charconv_upper[] =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
};
template<typename T, typename Char>
WJR_INLINE_CONSTEXPR void __base_encode_common::from_integral(T _Val,
basic_string_modifier<Char>& it, int base, errc& _Err) noexcept {
using uT = std::make_unsigned_t<T>;
auto uval = make_unsigned_v(_Val);
auto _First = it.begin();
const auto _Size = it.size();
if (WJR_UNLIKELY(_Size == 0)) {
_Err = errc::buffer_too_small;
return;
}
if constexpr (std::is_signed_v<T>) {
if (_Val < 0) {
if (_Size == 1) {
_Err = errc::buffer_too_small;
return;
}
*_First = '-';
++_First;
uval = static_cast<uT>(0 - uval);
}
}
constexpr size_t _Buff_size = sizeof(uT) * 8;
char _Buff[_Buff_size];
char* const _Buff_end = _Buff + _Buff_size;
char* _RNext = _Buff_end;
switch (base) {
case 10:
{ // Derived from _UIntegral_to_buff()
constexpr bool _Use_chunks = sizeof(uT) > sizeof(size_t);
if constexpr (_Use_chunks) { // For 64-bit numbers on 32-bit platforms, work in chunks to avoid 64-bit
while (uval > 0xFFFF'FFFFU) {
unsigned long _Chunk = static_cast<unsigned long>(uval % 1'000'000'000);
uval = static_cast<uT>(uval / 1'000'000'000);
for (int _Idx = 0; _Idx != 9; ++_Idx) {
*--_RNext = static_cast<char>('0' + _Chunk % 10);
_Chunk /= 10;
}
}
}
using _Truncated = std::conditional_t<_Use_chunks, unsigned long, uT>;
_Truncated _Trunc = static_cast<_Truncated>(uval);
do {
*--_RNext = static_cast<char>('0' + _Trunc % 10);
_Trunc /= 10;
} while (_Trunc != 0);
break;
}
case 2:
do {
*--_RNext = static_cast<char>('0' + (uval & 0b1));
uval >>= 1;
} while (uval != 0);
break;
case 4:
do {
*--_RNext = static_cast<char>('0' + (uval & 0b11));
uval >>= 2;
} while (uval != 0);
break;
case 8:
do {
*--_RNext = static_cast<char>('0' + (uval & 0b111));
uval >>= 3;
} while (uval != 0);
break;
case 16:
do {
*--_RNext = charconv_lower[uval & 0b1111];
uval >>= 4;
} while (uval != 0);
break;
case 32:
do {
*--_RNext = charconv_lower[uval & 0b11111];
uval >>= 5;
} while (uval != 0);
break;
case 3:
case 5:
case 6:
case 7:
case 9:
do {
*--_RNext = static_cast<char>('0' + uval % base);
uval = static_cast<uT>(uval / base);
} while (uval != 0);
break;
default:
do {
*--_RNext = charconv_lower[uval % base];
uval = static_cast<uT>(uval / base);
} while (uval != 0);
break;
}
size_t _Digits_written = _Buff_end - _RNext;
if (_Size < _Digits_written) {
_Err = errc::buffer_too_small;
return;
}
it = it.append(string_view(_RNext, _Digits_written));
it.finalize();
_Err = errc::ok;
return;
}
template<typename T, typename Char,
typename M,
typename F>
WJR_INLINE void __base_encode_common::from_floating_point(
T value, basic_string_modifier<Char>& it, errc& err, int precision, M m, F _Flags) noexcept {
using double_conversion::DoubleToStringConverter;
using double_conversion::StringBuilder;
DoubleToStringConverter conv(
_Flags,
"Infinity",
"NaN",
'E',
-6,
21,
6, // max leading padding zeros
1); // max trailing padding zeros
char buffer[256];
StringBuilder builder(buffer, sizeof(buffer));
const auto mode = m;
switch (mode) {
case DoubleToStringConverter::SHORTEST:
conv.ToShortest(value, &builder);
break;
case DoubleToStringConverter::SHORTEST_SINGLE:
conv.ToShortestSingle(static_cast<float>(value), &builder);
break;
case DoubleToStringConverter::FIXED:
conv.ToFixed(value, precision, &builder);
break;
case DoubleToStringConverter::PRECISION:
default:
conv.ToPrecision(value, precision, &builder);
break;
}
auto length = static_cast<size_t>(builder.position());
builder.Finalize();
const auto n = it.size();
if (n < length) {
err = errc::buffer_too_small;
return;
}
it = it.append(string_view(buffer, length));
it.finalize();
err = errc::ok;
return;
}
template<typename Traits>
class __base_encode : public __base_encode_common {
using _Mybase = __base_encode_common;
public:
using traits_type = Traits;
using encode_type = typename traits_type::encode_type;
using value_type = typename traits_type::value_type;
using pointer = typename traits_type::pointer;
using const_pointer = typename traits_type::const_pointer;
using iterator = typename traits_type::iterator;
using const_iterator = typename traits_type::const_iterator;
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalnum(value_type ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalpha(value_type ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool islower(value_type ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isupper(value_type ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isxdigit(value_type ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isspace(value_type ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static value_type tolower(value_type ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static value_type toupper(value_type ch);
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static uint8_t todigit(value_type ch);
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static uint8_t toalnum(value_type ch);
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static bool isdigit(value_type ch, int Base = 10);
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static const_iterator
skipw(const_iterator _First, const_iterator _Last);
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static
auto rskipw(const_iterator _First, const_iterator _Last)
-> std::enable_if_t<is_random_iterator_v<const_iterator>, const_iterator>;
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static const_iterator skipz(const_iterator _First, const_iterator _Last);
enum to_i_flags {
NO_FLAGS = 0,
ALLOW_PREFIX = 0x01,
ALLOW_TAIL = 0x02,
ALLOW_SIGN = 0x04,
ALLOW_LEADING_SPACE = 0x08,
ALLOW_SPACE_AFTER_SIGN = 0x10,
ALLOW_LEADING_ZEROS = 0x20,
};
using default_to_i_flags = std::integral_constant< to_i_flags,
static_cast<to_i_flags>(
ALLOW_PREFIX
| ALLOW_TAIL
| ALLOW_SIGN
| ALLOW_LEADING_SPACE
| ALLOW_LEADING_ZEROS)>;
template<typename T, typename F = default_to_i_flags>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static T to_integral(
const_iterator _First, const_iterator _Last,
const_iterator& _Pos, int base, errc& _Err, F f = F()) noexcept;
template<typename T, typename F = default_to_i_flags>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static T to_integral(
const_iterator _First, const_iterator _Last,
const_iterator* _Pos = nullptr, int base = 10, errc* _Err = nullptr, F f = F()) noexcept;
private:
using _Mybase::from_integral;
public:
template<typename T>
WJR_INLINE_CONSTEXPR20 static void from_integral(
T _Val, basic_string_modifier<value_type>& it, int base, errc& _Err) noexcept;
template<typename T>
WJR_INLINE_CONSTEXPR20 static void from_integral(
T _Val, pointer _First, pointer _Last,
pointer& _Pos, int base, errc& _Err) noexcept;
template<typename T>
WJR_INLINE_CONSTEXPR20 static void from_integral(
T _Val, pointer _First, pointer _Last,
pointer* _Pos = nullptr, int base = 10, errc* _Err = nullptr) noexcept;
using to_f_flags = typename double_conversion::StringToDoubleConverter::Flags;
using default_to_f_flags = std::integral_constant<
to_f_flags,
static_cast<to_f_flags>
(to_f_flags::ALLOW_LEADING_SPACES
| to_f_flags::ALLOW_TRAILING_JUNK)
>;
template<typename T, typename F = default_to_f_flags>
WJR_NODISCARD WJR_INLINE static T to_floating_point(
const_iterator _First, const_iterator _Last,
const_iterator& _Pos, errc& _Err, F f = F()) noexcept;
template<typename T, typename F = default_to_f_flags>
WJR_NODISCARD WJR_INLINE static T to_floating_point(
const_iterator _First, const_iterator _Last,
const_iterator* _Pos = nullptr, errc* _Err = nullptr, F f = F()) noexcept;
using from_f_mode = typename _Mybase::from_f_mode;
using from_f_flags = typename _Mybase::from_f_flags;
using default_from_f_mode = typename _Mybase::default_from_f_mode;
using default_from_f_flags = typename _Mybase::default_from_f_flags;
private:
using _Mybase::from_floating_point;
public:
template<typename T,
typename M = default_from_f_mode,
typename F = default_from_f_flags>
WJR_INLINE_CONSTEXPR20 static void from_floating_point(
T _Val, basic_string_modifier<value_type>& it,
errc& err, int precision = 0, M m = M(), F _Flags = F()) noexcept;
template<typename T,
typename M = default_from_f_mode,
typename F = default_from_f_flags>
WJR_INLINE static void from_floating_point(
T value, pointer first, pointer last,
pointer& pos, errc& err, int precision = 0, M m = M(), F _Flags = F()) noexcept;
template<typename T,
typename M = default_from_f_mode,
typename F = default_from_f_flags>
WJR_INLINE static void from_floating_point(
T value, pointer first, pointer last,
pointer* pos = nullptr, errc* err = nullptr, int precision = 0, M m = M(), F _Flags = F()) noexcept;
WJR_INLINE_CONSTEXPR static iterator make_iterator(pointer);
WJR_INLINE_CONSTEXPR static const_iterator make_iterator(const_pointer);
WJR_INLINE_CONSTEXPR static pointer make_pointer(iterator);
WJR_INLINE_CONSTEXPR static const_pointer make_pointer(const_iterator);
};
template<typename T, typename Encode>
class integral_conversion_details;
template<typename T, typename Encode>
class floating_point_conversion_details;
template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::isalnum(value_type ch) {
return encode_type::isalnum(ch);
}
template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::isalpha(value_type ch) {
return encode_type::isalpha(ch);
}
template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::islower(value_type ch) {
return encode_type::islower(ch);
}
template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::isupper(value_type ch) {
return encode_type::isupper(ch);
}
template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::isxdigit(value_type ch) {
return encode_type::isxdigit(ch);
}
template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool __base_encode<Traits>::isspace(value_type ch) {
return encode_type::isspace(ch);
}
template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::value_type
__base_encode<Traits>::tolower(value_type ch) {
return encode_type::tolower(ch);
}
template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::value_type
__base_encode<Traits>::toupper(value_type ch) {
return encode_type::toupper(ch);
}
template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint8_t __base_encode<Traits>::todigit(value_type ch) {
return encode_type::todigit(ch);
}
template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint8_t __base_encode<Traits>::toalnum(value_type ch) {
return todigit(ch);
}
template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR bool __base_encode<Traits>::isdigit(value_type ch, int Base) {
return toalnum(ch) < Base;
}
template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR typename __base_encode<Traits>::const_iterator
__base_encode<Traits>::skipw(const_iterator _First, const_iterator _Last) {
if constexpr (__string_func_traits::has_static_member_function_do_skipw_v<encode_type, const_iterator, const_iterator>) {
return encode_type::do_skipw(_First, _Last);
}
else {
while (_First != _Last && isspace(*_First)) ++_First;
return _First;
}
}
template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR auto
__base_encode<Traits>::rskipw(const_iterator _First, const_iterator _Last)
-> std::enable_if_t<is_random_iterator_v<const_iterator>, const_iterator> {
if constexpr (__string_func_traits::
has_static_member_function_do_rskipw_v<encode_type, const_iterator, const_iterator>) {
return encode_type::do_rskipw(_First, _Last);
}
else {
while (_First != _Last && isspace(*(_Last - 1))) --_Last;
return _Last;
}
}
template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR typename __base_encode<Traits>::const_iterator
__base_encode<Traits>::skipz(const_iterator _First, const_iterator _Last) {
if constexpr (__string_func_traits::has_static_member_function_do_skipz_v<encode_type, const_iterator, const_iterator>) {
return encode_type::do_skipz(_First, _Last);
}
else {
while (_First != _Last && *_First == '0') ++_First;
return _First;
}
}
template<typename Traits>
template<typename T, typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T __base_encode<Traits>::to_integral(
const_iterator _First, const_iterator _Last,
const_iterator& _Pos, int base, errc& _Err, F f) noexcept {
return integral_conversion_details<T, __base_encode<Traits>>
::template to(_First, _Last, _Pos, base, _Err, f);
}
template<typename Traits>
template<typename T, typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T __base_encode<Traits>::to_integral(
const_iterator _First, const_iterator _Last,
const_iterator* _Pos, int base, errc* _Err, F f) noexcept {
const_iterator _End = _First;
errc c = errc::ok;
auto ret = to_integral<T>(_First, _Last, _End, base, c, f);
if (_Pos != nullptr) *_Pos = _End;
if (_Err != nullptr) *_Err = c;
return ret;
}
template<typename Traits>
template<typename T>
WJR_INLINE_CONSTEXPR20 void __base_encode<Traits>::from_integral(
T _Val, basic_string_modifier<value_type>& it, int base, errc& _Err) noexcept {
_Mybase::from_integral(_Val, it, base, _Err);
}
template<typename Traits>
template<typename T>
WJR_INLINE_CONSTEXPR20 void __base_encode<Traits>::from_integral(
T _Val, pointer _First, pointer _Last,
pointer& _Pos, int base, errc& _Err) noexcept {
basic_string_modifier<value_type> it(_First, static_cast<size_t>(_Last - _First));
from_integral(_Val, it, base, _Err);
_Pos = it.begin();
}
template<typename Traits>
template<typename T>
WJR_INLINE_CONSTEXPR20 void __base_encode<Traits>::from_integral(
T _Val, pointer _First, pointer _Last,
pointer* _Pos, int base, errc* _Err) noexcept {
pointer end_ptr = _First;
errc c = errc::ok;
from_integral(_Val, _First, _Last, end_ptr, base, c);
if (_Pos != nullptr) *_Pos = end_ptr;
if (_Err != nullptr) *_Err = c;
}
template<typename Traits>
template<typename T, typename F>
WJR_NODISCARD WJR_INLINE T __base_encode<Traits>::to_floating_point(
const_iterator _First, const_iterator _Last,
const_iterator& _Pos, errc& _Err, F f) noexcept {
return floating_point_conversion_details<T, __base_encode<Traits>>
::template to(_First, _Last, _Pos, _Err, f);
}
template<typename Traits>
template<typename T, typename F>
WJR_NODISCARD WJR_INLINE T __base_encode<Traits>::to_floating_point(
const_iterator _First, const_iterator _Last,
const_iterator* _Pos, errc* _Err, F f) noexcept {
const_iterator _End = _First;
errc c = errc::ok;
auto ret = to_floating_point<T>(_First, _Last, _End, c, f);
if (_Pos != nullptr) *_Pos = _End;
if (_Err != nullptr) *_Err = c;
return ret;
}
template<typename Traits>
template<typename T,
typename M,
typename F>
WJR_INLINE_CONSTEXPR20 void __base_encode<Traits>::from_floating_point(
T _Val, basic_string_modifier<value_type>& it,
errc& err, int precision, M m, F _Flags) noexcept {
_Mybase::from_floating_point(_Val, it, err, precision, m, _Flags);
}
template<typename Traits>
template<typename T,
typename M,
typename F>
WJR_INLINE void __base_encode<Traits>::from_floating_point(
T value, pointer first, pointer last,
pointer& pos, errc& err, int precision, M m, F _Flags) noexcept {
basic_string_modifier<value_type> it(first, static_cast<size_t>(last - first));
from_floating_point(value, it, err, precision, m, _Flags);
pos = it.begin();
}
template<typename Traits>
template<typename T,
typename M,
typename F>
WJR_INLINE void __base_encode<Traits>::from_floating_point(
T value, pointer first, pointer last,
pointer* pos, errc* err, int precision, M m, F _Flags) noexcept {
pointer end_ptr = first;
errc c = errc::ok;
from_floating_point(value, first, last, end_ptr, c, precision, m, _Flags);
if (pos != nullptr) *pos = end_ptr;
if (err != nullptr) *err = c;
}
template<typename Traits>
WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::iterator
__base_encode<Traits>::make_iterator(pointer ptr) {
return encode_type::make_iterator(ptr);
}
template<typename Traits>
WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::const_iterator
__base_encode<Traits>::make_iterator(const_pointer ptr) {
return encode_type::make_iterator(ptr);
}
template<typename Traits>
WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::pointer
__base_encode<Traits>::make_pointer(iterator ptr) {
return encode_type::make_pointer(ptr);
}
template<typename Traits>
WJR_INLINE_CONSTEXPR typename __base_encode<Traits>::const_pointer
__base_encode<Traits>::make_pointer(const_iterator ptr) {
return encode_type::make_pointer(ptr);
}
template<typename T, typename Encode>
class integral_conversion_details {
public:
using encode_type = Encode;
using value_type = typename encode_type::value_type;
using to_i_flags = typename encode_type::to_i_flags;
using iterator = typename encode_type::iterator;
using const_iterator = typename encode_type::const_iterator;
template<typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static T to(
const_iterator _First, const_iterator _Last,
const_iterator& _Pos, int base, errc& _Err, F f) noexcept;
private:
inline constexpr static char charconv_lower[] =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e',
'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
inline constexpr static char charconv_upper[] =
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
using uT = std::make_unsigned_t<T>;
constexpr static T min() { return std::numeric_limits<T>::min(); }
constexpr static T max() { return std::numeric_limits<T>::max(); }
constexpr static uT umax() { return std::max(make_unsigned_v(min()), make_unsigned_v(max())); }
template<typename _Iter, typename F>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static T __to(
_Iter _First, _Iter _Last,
WJR_MAYBE_UNUSED _Iter& _Pos,
int base, errc& _Err, F f, _Iter _Zero, bool _Is_minus) noexcept;
};
#define __CONV_EMPTY_RET				        \
_Err = errc::noconv;					\
return static_cast<T>(0);
#define __CONV_NEXT								\
if(WJR_UNLIKELY(++_First == _Last)){	    \
__CONV_EMPTY_RET					\
}
template<typename T, typename Func>
template<typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T integral_conversion_details<T, Func>::to(
const_iterator _First, const_iterator _Last,
const_iterator& _Pos, int base, errc& _Err, F _Flags) noexcept {
static_assert(std::is_integral_v<T>, "");
if (_Flags & to_i_flags::ALLOW_LEADING_SPACE) {
_First = encode_type::skipw(_First, _Last);
}
if (WJR_UNLIKELY(_First == _Last)) {
__CONV_EMPTY_RET;
}
bool _Is_minus = false;
if (_Flags & to_i_flags::ALLOW_SIGN) {
auto ch = *_First;
switch (ch) {
case '+': {
__CONV_NEXT;
break;
}
case '-': {
_Is_minus = true;
__CONV_NEXT;
break;
}
}
}
if (_Flags & to_i_flags::ALLOW_SPACE_AFTER_SIGN) {
_First = encode_type::skipw(_First, _Last);
if (WJR_UNLIKELY(_First == _Last)) {
__CONV_EMPTY_RET;
}
}
constexpr auto _B = encode_type::toalnum('B');
constexpr auto _X = encode_type::toalnum('X');
WJR_MAYBE_UNUSED auto _Zero = _Last;
if (_Flags & to_i_flags::ALLOW_PREFIX) {
switch (base) {
case 0: {
if (*_First == '0') {
if (WJR_UNLIKELY(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
_Zero = _First;
auto ch = encode_type::toalnum(*_First);
switch (ch) {
case _B: {
if (WJR_UNLIKELY(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
return __to(_First, _Last, _Pos, 2, _Err, _Flags, _Zero, _Is_minus);
}
case _X: {
if (WJR_UNLIKELY(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
return __to(_First, _Last, _Pos, 16, _Err, _Flags, _Zero, _Is_minus);
}
default: {
return __to(_First, _Last, _Pos, 8, _Err, _Flags, _Zero, _Is_minus);
}
}
}
return __to(_First, _Last, _Pos, 10, _Err, _Flags, _Last, _Is_minus);
}
case 2: {
if (*_First == '0') {
if (WJR_UNLIKELY(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
_Zero = _First;
if (encode_type::toalnum(*_First) == _B) {
if (WJR_UNLIKELY(++_First == _Last)) {
_Pos = _Zero;
return static_cast<T>(0);
}
}
}
break;
}
case 8: {
if (*_First == '0') {
if (WJR_UNLIKELY(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
_Zero = _First;
}
break;
}
case 16: {
if (*_First == '0') {
if (WJR_UNLIKELY(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
_Zero = _First;
if (encode_type::toalnum(*_First) == _X) {
if (WJR_UNLIKELY(++_First == _Last)) {
_Pos = _Zero;
return static_cast<T>(0);
}
}
}
break;
}
default: {
break;
}
}
}
return __to(_First, _Last, _Pos, base, _Err, _Flags, _Zero, _Is_minus);
}
template<typename T, typename Func>
template<typename _Iter, typename F>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR T integral_conversion_details<T, Func>::__to(
_Iter _First, _Iter _Last,
WJR_MAYBE_UNUSED _Iter& _Pos,
int base, errc& _Err, F _Flags, _Iter _Zero, bool _Is_minus) noexcept {
auto _Next = _First;
if (_Flags & to_i_flags::ALLOW_LEADING_ZEROS) {
_Next = encode_type::skipz(_First, _Last);
}
uT _Max1 = 0;
uT _Max2 = 0;
if constexpr (std::is_unsigned_v<T>) {
_Max1 = umax() / base;
_Max2 = umax() % base;
}
else {
if (_Is_minus) {
_Max1 = umax() / base;
_Max2 = umax() % base;
}
else {
constexpr auto __umax2 = umax() - 1;
_Max1 = __umax2 / base;
_Max2 = __umax2 % base;
}
}
uT uret = 0;
bool overflowed = false;
for (; _Next != _Last; ++_Next) {
auto ch = encode_type::toalnum(*_Next);
if (ch >= base) break;
if (uret < _Max1 || (uret == _Max1 && ch <= _Max2)) {
uret *= base;
uret += ch;
}
else overflowed = true;
}
if (_First == _Next) {
if (_Zero != _Last) {
_Pos = _Zero;
return static_cast<T>(uret);
}
__CONV_EMPTY_RET;
}
_Pos = _Next;
if (overflowed) {
if constexpr (std::is_unsigned_v<T>) {
_Err = errc::overflow;
}
else {
if (_Is_minus) {
_Err = errc::underflow;
}
else {
_Err = errc::overflow;
}
}
return static_cast<T>(0);
}
_Err = errc::ok;
if (_Is_minus) {
uret = static_cast<uT>(0 - uret);
}
return static_cast<T>(uret);
}
#undef __CONV_EMPTY_RET
#undef __CONV_NEXT
template<typename T, typename Encode>
class floating_point_conversion_details {
public:
using encode_type = Encode;
using value_type = typename encode_type::value_type;
using to_f_flags = typename encode_type::to_f_flags;
using iterator = typename encode_type::iterator;
using const_iterator = typename encode_type::const_iterator;
template<typename F>
WJR_ATTRIBUTE(NODISCARD, INLINE) static T to(
const_iterator _First, const_iterator _Last,
const_iterator& _Pos, errc& _Err, F f) noexcept;
};
class ascii;
struct ascii_traits;
template<typename T, typename Encode>
template<typename F>
WJR_ATTRIBUTE(NODISCARD, INLINE) T floating_point_conversion_details<T, Encode>::to(
const_iterator _First, const_iterator _Last,
const_iterator& _Pos, errc& _Err, F f) noexcept {
static_assert(is_any_of_v<T, float, double>, "");
if constexpr (std::is_same_v<Encode, __base_encode<ascii_traits>>) {
using double_conversion::StringToDoubleConverter;
if (_First == _Last) {
_Err = errc::noconv;
return static_cast<T>(0);
}
const StringToDoubleConverter conv(
f,
0.0,
std::numeric_limits<T>::quiet_NaN(),
nullptr,
nullptr);
int length = 0; // processed char count
T result = 0;
if constexpr (std::is_same_v<T, float>) {
result = conv.StringToFloat(_First, static_cast<size_t>(_Last - _First), &length);
}
else {
result = conv.StringToDouble(_First, static_cast<size_t>(_Last - _First), &length);
}
if (!std::isnan(result)) {
if (length == 0 ||
(result == 0.0 && encode_type::isspace(_First[length - 1]))) {
_Err = errc::noconv;
return static_cast<T>(0);
}
if (length >= 2) {
const char* suffix = _First + length - 1;
if (*suffix == '-' || *suffix == '+') {
--suffix;
--length;
}
if (*suffix == 'e' || *suffix == 'E') {
--length;
}
}
_Pos = _First + length;
_Err = errc::ok;
return result;
}
auto s = encode_type::skipw(_First, _Last);
if (s == _Last) {
_Err = errc::noconv;
return static_cast<T>(0);
}
auto size = size_t(_Last - s);
bool negative = false;
if (*s == '-') {
negative = true;
++s;
--size;
if (size == 0) {
_Err = errc::noconv;
return static_cast<T>(0);
}
}
WJR_ASSUME(size > 0);
result = 0.0;
switch (encode_type::tolower(*s)) {
case 'i':
if (size >= 3 && encode_type::tolower(s[1]) == 'n' &&
encode_type::tolower(s[2]) == 'f') {
if (size >= 8 && encode_type::tolower(s[3]) == 'i' &&
encode_type::tolower(s[4]) == 'n' && encode_type::tolower(s[5]) == 'i' &&
encode_type::tolower(s[6]) == 't' && encode_type::tolower(s[7]) == 'y') {
s += 8;
}
else {
s += 3;
}
result = std::numeric_limits<T>::infinity();
}
break;
case 'n':
if (size >= 3 && encode_type::tolower(s[1]) == 'a' &&
encode_type::tolower(s[2]) == 'n') {
s += 3;
result = std::numeric_limits<T>::quiet_NaN();
}
break;
default:
break;
}
if (result == 0.0) {
_Err = errc::noconv;
return static_cast<T>(0);
}
if (negative) {
result = -result;
}
_Pos = s;
_Err = errc::ok;
return static_cast<T>(result);
}
else {
static_assert(std::is_same_v<Encode, __base_encode<ascii_traits>>, "");
return static_cast<T>(0);
}
}
struct ascii_traits {
using encode_type = encode::ascii;
using value_type = char;
using pointer = value_type*;
using const_pointer = const value_type*;
using iterator = pointer;
using const_iterator = const_pointer;
};
struct __ascii_to_digit_table {
constexpr static uint8_t invalid = 0xff;
constexpr __ascii_to_digit_table() : table() {
for (int i = 0; i < 256; ++i) {
table[i] = invalid;
}
for (int i = '0'; i <= '9'; ++i) {
table[i] = (i - '0');
}
for (int i = 'a'; i <= 'z'; ++i) {
table[i] = (i - 'a') + 10;
}
for (int i = 'A'; i <= 'Z'; ++i) {
table[i] = (i - 'A') + 10;
}
}
WJR_ATTRIBUTE(CONST, CONSTEXPR) uint8_t operator[](uint8_t ch) const { return table[ch]; }
private:
std::array<uint8_t, 256> table;
};
constexpr static __ascii_to_digit_table __ascii_to_digit_table_v = {};
class ascii : public __base_encode<ascii_traits> {
public:
friend __base_encode<ascii_traits>;
using _Mybase = __base_encode<ascii_traits>;
using traits_type = ascii_traits;
using value_type = typename traits_type::value_type;
using pointer = typename traits_type::pointer;
using const_pointer = typename traits_type::const_pointer;
using iterator = typename traits_type::iterator;
using const_iterator = typename traits_type::const_iterator;
enum __char_code {
__none = 0x00,
__lower = 0x01,
__upper = 0x02,
__digit = 0x04,
__xdigit = 0x08,
__white = 0x10
};
static constexpr int __char_code_table[256] = {
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,         __white,         __white,         __white,
__white,         __white,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__white,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__digit | __xdigit,__digit | __xdigit,__digit | __xdigit,__digit | __xdigit,
__digit | __xdigit,__digit | __xdigit,__digit | __xdigit,__digit | __xdigit,
__digit | __xdigit,__digit | __xdigit,          __none,          __none,
__none,          __none,          __none,          __none,
__none,__upper | __xdigit,__upper | __xdigit,__upper | __xdigit,
__upper | __xdigit,__upper | __xdigit,__upper | __xdigit,         __upper,
__upper,         __upper,         __upper,         __upper,
__upper,         __upper,         __upper,         __upper,
__upper,         __upper,         __upper,         __upper,
__upper,         __upper,         __upper,         __upper,
__upper,         __upper,         __upper,          __none,
__none,          __none,          __none,          __none,
__none,__lower | __xdigit,__lower | __xdigit,__lower | __xdigit,
__lower | __xdigit,__lower | __xdigit,__lower | __xdigit,         __lower,
__lower,         __lower,         __lower,         __lower,
__lower,         __lower,         __lower,         __lower,
__lower,         __lower,         __lower,         __lower,
__lower,         __lower,         __lower,         __lower,
__lower,         __lower,         __lower,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none,
__none,          __none,          __none,          __none
};
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isalnum(value_type ch) {
return __char_code_table[make_unsigned_v(ch)] & (__lower | __upper | __digit);
}
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isalpha(value_type ch) {
return __char_code_table[make_unsigned_v(ch)] & (__lower | __upper);
}
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool islower(value_type ch) {
return __char_code_table[make_unsigned_v(ch)] & __lower;
}
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isupper(value_type ch) {
return __char_code_table[make_unsigned_v(ch)] & __upper;
}
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isdigit(value_type ch) {
return __char_code_table[make_unsigned_v(ch)] & __digit;
}
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isxdigit(value_type ch) {
return __char_code_table[make_unsigned_v(ch)] & __xdigit;
}
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static bool isspace(value_type ch) {
return __char_code_table[make_unsigned_v(ch)] & __white;
}
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static value_type tolower(value_type ch) {
return isupper(ch) ? ch + 'a' - 'A' : ch;
}
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static value_type toupper(value_type ch) {
return islower(ch) ? ch + 'A' - 'a' : ch;
}
WJR_ATTRIBUTE(NODISCARD, CONST, INLINE_CONSTEXPR) static uint8_t todigit(char ch) {
return __ascii_to_digit_table_v[ch];
}
WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) static const_iterator do_skipw(const_iterator s, const_iterator e) {
if constexpr (::wjr::algo::__has_fast_memskipw_v<value_type>) {
return ::wjr::algo::memskipw(s, e);
}
else {
for (; s != e && isspace(*s); ++s);
return s;
}
}
WJR_INLINE_CONSTEXPR static iterator make_iterator(pointer ptr) { return ptr; }
WJR_INLINE_CONSTEXPR static const_iterator make_iterator(const_pointer ptr) { return ptr; }
WJR_INLINE_CONSTEXPR static pointer make_pointer(iterator ptr) { return ptr; }
WJR_INLINE_CONSTEXPR static const_pointer make_pointer(const_iterator ptr) { return ptr; }
};
class utf8 {
public:
};
_WJR_ENCODE_END
_WJR_BEGIN
template<typename Traits>
class basic_encode_view
: public basic_string_view<typename Traits::value_type> {
public:
using _Mybase = basic_string_view<typename Traits::value_type>;
template<typename T>
constexpr static bool _Is_noptr_string_view_like_v = _Mybase::template _Is_noptr_string_view_like_v<T>;
public:
using traits_type = typename _Mybase::traits_type;
using value_type = typename _Mybase::value_type;
using pointer = typename _Mybase::pointer;
using const_pointer = typename _Mybase::const_pointer;
using reference = typename _Mybase::reference;
using const_reference = typename _Mybase::const_reference;
using const_iterator = typename _Mybase::const_iterator;
using iterator = typename _Mybase::iterator;
using const_reverse_iterator = typename _Mybase::const_reverse_iterator;
using reverse_iterator = typename _Mybase::reverse_iterator;
using size_type = typename _Mybase::size_type;
using difference_type = typename _Mybase::difference_type;
constexpr static size_type npos = _Mybase::npos;
using encode_type = encode::__base_encode<Traits>;
using encode_iterator = typename encode_type::iterator;
using encode_const_iterator = typename encode_type::const_iterator;
using _Mybase::_Mybase;
using _Mybase::operator=;
using _Mybase::begin;
using _Mybase::end;
WJR_CONSTEXPR basic_encode_view(const _Mybase& other)
: _Mybase(other) {}
WJR_INLINE_CONSTEXPR void swap(basic_encode_view& other) noexcept {
return _Mybase::swap(other);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_encode_view substr(
const size_type off = 0, size_type n = npos) const WJR_NOEXCEPT {
return basic_encode_view(_Mybase::substr(off, n));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_encode_view
nesubstr(const size_type off = 0, size_type n = npos) const noexcept {
return basic_encode_view(_Mybase::nesubstr(off, n));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_view view(
const value_type* s, const size_type n) noexcept {
return basic_encode_view(s, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_view view(
const value_type* s) noexcept {
return basic_encode_view(s);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_view view(
std::initializer_list<value_type> il) noexcept {
return view(il.begin(), il.size());
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_view view(const StringView& t) noexcept {
return basic_encode_view(t);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_view view(const StringView& t,
const size_type off, const size_type n = npos) noexcept {
return view(t).nesubstr(off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_encode_view view() const noexcept {
return *this;
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_view eview(const StringView& t,
const size_type off, const size_type n = npos) WJR_NOEXCEPT {
return view(t).substr(off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_encode_view prefix(size_type n) const noexcept {
return basic_encode_view(prefix(n));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_encode_view suffix(size_type n) const noexcept {
return basic_encode_view(suffix(n));
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) basic_encode_view adjust_begin(const_iterator iter) const noexcept {
return basic_encode_view(_Mybase::adjust_begin(iter));
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) basic_encode_view adjust_end(const_iterator iter) const noexcept {
return basic_encode_view(_Mybase::adjust_end(iter));
}
#define _WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC(name, ret_type, type)	\
WJR_INLINE_CONSTEXPR static ret_type name(type arg){return encode_type::name(arg);}
#define WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC(x) _WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC x
WJR_MACRO_CALL(WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC, ,
(isalnum, bool, value_type),
(isalpha, bool, value_type),
(islower, bool, value_type),
(isupper, bool, value_type),
(isxdigit, bool, value_type),
(isspace, bool, value_type),
(tolower, value_type, value_type),
(toupper, value_type, value_type),
(todigit, uint8_t, value_type),
(toalnum, uint8_t, value_type),
(isdigit, bool, value_type),
(make_iterator, encode_iterator, pointer),
(make_iterator, encode_const_iterator, const_pointer),
(make_pointer, pointer, encode_iterator),
(make_pointer, const_pointer, encode_const_iterator)
);
#undef WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC
#undef _WJR_REGISTER_ENCODE_VIEW_DERIVED_FUNC
WJR_INLINE_CONSTEXPR encode_iterator encode_begin() { return make_iterator(begin()); }
WJR_INLINE_CONSTEXPR encode_const_iterator encode_begin() const { return make_iterator(begin()); }
WJR_INLINE_CONSTEXPR encode_iterator encode_end() { return make_iterator(end()); }
WJR_INLINE_CONSTEXPR encode_const_iterator encode_end() const { return make_iterator(end()); }
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) basic_encode_view ltrim() const;
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR)auto rtrim() const
->std::enable_if_t<is_random_iterator_v<encode_const_iterator>, basic_encode_view>;
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR)auto trim() const
->std::enable_if_t<is_random_iterator_v<encode_const_iterator>, basic_encode_view>;
using to_i_flags = typename encode_type::to_i_flags;
using default_to_i_flags = typename encode_type::default_to_i_flags;
template<typename T, typename F = default_to_i_flags>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) T to_integral(
const_iterator* _Pos = nullptr, int base = 10, errc* _Err = nullptr, F f = F()) const noexcept;
#define _WJR_REGISTER_STRING_TO_INTEGRAL_FUNC(T, name)	                                            \
template<typename F = default_to_i_flags>	                                                    \
WJR_NODISCARD WJR_INLINE_CONSTEXPR T to_##name(	                                                \
const_iterator* _Pos = nullptr, int base = 10, errc* _Err = nullptr, F f = F()) const noexcept{	\
return to_integral<T>(_Pos, base, _Err, f);	                                                \
}
#define WJR_REGISTER_STRING_TO_INTEGRAL_FUNC(x) _WJR_REGISTER_STRING_TO_INTEGRAL_FUNC x
WJR_MACRO_CALL(WJR_REGISTER_STRING_TO_INTEGRAL_FUNC, ,
(char, char),
(signed char, schar),
(unsigned char, uchar),
(short, short),
(unsigned short, ushort),
(int, int),
(unsigned int, uint),
(long, long),
(unsigned long, ulong),
(long long, llong),
(unsigned long long, ullong)
);
#undef WJR_REGISTER_STRING_TO_INTEGRAL_FUNC
#undef _WJR_REGISTER_STRING_TO_INTEGRAL_FUNC
using to_f_flags = typename encode_type::to_f_flags;
using default_to_f_flags = typename encode_type::default_to_f_flags;
template<typename T, typename F = default_to_f_flags>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) T to_floating_point(
const_iterator* _Pos = nullptr, errc* _Err = nullptr, F f = F()) const noexcept;
#define _WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC(T, name)	                                            \
template<typename F = default_to_i_flags>	                                                            \
WJR_NODISCARD WJR_INLINE_CONSTEXPR T to_##name(	                                                        \
const_iterator* _Pos = nullptr, errc* _Err = nullptr, F f = F()) const noexcept{	                    \
return to_floating_point<T>(_Pos, _Err, f);	                                                        \
}
#define WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC(x) _WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC x
WJR_MACRO_CALL(WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC, ,
(float, float),
(double, double)
);
#undef WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC
#undef _WJR_REGISTER_STRING_TO_FLOATING_POINT_FUNC
};
template<typename Traits>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) basic_encode_view<Traits> basic_encode_view<Traits>::ltrim() const {
return this->adjust_begin(encode_type::make_pointer(
encode_type::skipw(encode_begin(), encode_end())));
}
template<typename Traits>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR)
auto basic_encode_view<Traits>::rtrim() const
-> std::enable_if_t<is_random_iterator_v<typename basic_encode_view<Traits>::encode_const_iterator>, basic_encode_view<Traits>> {
return this->adjust_end(encode_type::make_pointer(
encode_type::rskipw(encode_begin(), encode_end())));
}
template<typename Traits>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR)
auto basic_encode_view<Traits>::trim() const
-> std::enable_if_t<is_random_iterator_v<typename basic_encode_view<Traits>::encode_const_iterator>, basic_encode_view<Traits>> {
return ltrim().rtrim();
}
template<typename Traits>
template<typename T, typename F>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) T basic_encode_view<Traits>::to_integral(
const_iterator* _Pos, int base, errc* _Err, F f) const noexcept {
return encode_type::template to_integral<T>(encode_begin(), encode_end(), _Pos, base, _Err, f);
}
template<typename Traits>
template<typename T, typename F>
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) T basic_encode_view<Traits>::to_floating_point(
const_iterator* _Pos, errc* _Err, F f) const noexcept {
return encode_type::template to_floating_point<T>(encode_begin(), encode_end(), _Pos, _Err, f);
}
template<typename Traits>
class basic_encode_modifier
: public basic_encode_view<Traits>{
using _Mybase = basic_encode_view<Traits>;
using _Mybase2 = basic_string_modifier<typename Traits::value_type, std::char_traits<typename Traits::value_type>>;
template<typename T>
constexpr static bool _Is_noptr_string_view_like_v = _Mybase::template _Is_noptr_string_view_like_v<T>;
public:
using traits_type = typename _Mybase::traits_type;
using value_type = typename _Mybase::value_type;
using pointer = typename _Mybase2::pointer;
using const_pointer = typename _Mybase::const_pointer;
using reference = typename _Mybase2::reference;
using const_reference = typename _Mybase::const_reference;
using const_iterator = typename _Mybase::const_iterator;
using iterator = typename _Mybase2::iterator;
using const_reverse_iterator = typename _Mybase::const_reverse_iterator;
using reverse_iterator = typename _Mybase2::reverse_iterator;
using size_type = typename _Mybase::size_type;
using difference_type = typename _Mybase::difference_type;
constexpr static size_type npos = _Mybase::npos;
using encode_type = encode::__base_encode<Traits>;
using encode_iterator = typename encode_type::iterator;
using encode_const_iterator = typename encode_type::const_iterator;
using _Mybase::_Mybase;
using _Mybase::operator=;
using _Mybase::data;
using _Mybase::begin;
using _Mybase::end;
using _Mybase::rbegin;
using _Mybase::rend;
using _Mybase::front;
WJR_CONSTEXPR basic_encode_modifier(const _Mybase& other)
: _Mybase(other) {}
WJR_INLINE_CONSTEXPR void swap(basic_encode_modifier& other) noexcept {
return _Mybase::swap(other);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_encode_modifier substr(
const size_type off = 0, size_type n = npos) const WJR_NOEXCEPT {
return basic_encode_modifier(_Mybase::substr(off, n));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_encode_modifier
nesubstr(const size_type off = 0, size_type n = npos) const noexcept {
return basic_encode_modifier(_Mybase::nesubstr(off, n));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_modifier view(
const value_type* s, const size_type n) noexcept {
return basic_encode_modifier(s, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_modifier view(
const value_type* s) noexcept {
return basic_encode_modifier(s);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_modifier view(
std::initializer_list<value_type> il) noexcept {
return view(il.begin(), il.size());
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_modifier view(const StringView& t) noexcept {
return basic_encode_modifier(t);
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_modifier view(const StringView& t,
const size_type off, const size_type n = npos) noexcept {
return view(t).nesubstr(off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_encode_modifier view() const noexcept {
return *this;
}
template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static basic_encode_modifier eview(const StringView& t,
const size_type off, const size_type n = npos) WJR_NOEXCEPT {
return view(t).substr(off, n);
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_encode_modifier prefix(size_type n) const noexcept {
return basic_encode_modifier(prefix(n));
}
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_encode_modifier suffix(size_type n) const noexcept {
return basic_encode_modifier(suffix(n));
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) basic_encode_modifier adjust_begin(const_iterator iter) const noexcept {
return basic_encode_modifier(_Mybase::adjust_begin(iter));
}
WJR_ATTRIBUTE(NODISCARD, INLINE_CONSTEXPR) basic_encode_modifier adjust_end(const_iterator iter) const noexcept {
return basic_encode_modifier(_Mybase::adjust_end(iter));
}
WJR_INLINE_CONSTEXPR pointer data() { return const_cast<pointer>(_Mybase::data()); }
WJR_INLINE_CONSTEXPR iterator begin() { return const_cast<iterator>(_Mybase::begin()); }
WJR_INLINE_CONSTEXPR iterator end() { return const_cast<iterator>(_Mybase::end()); }
WJR_INLINE_CONSTEXPR reverse_iterator rbegin() {
return reverse_iterator(end());
}
WJR_INLINE_CONSTEXPR reverse_iterator rend() {
return reverse_iterator(begin());
}
WJR_INLINE_CONSTEXPR reference front() {
return *begin();
}
WJR_INLINE_CONSTEXPR reference operator[](size_type n) {
return data()[n];
}
using _Mybase::size;
template<typename otherChar, typename otherTraits>
WJR_INLINE_CONSTEXPR20 basic_encode_modifier append(basic_string_view<otherChar, otherTraits> str) {
static_assert(sizeof(otherChar) <= sizeof(value_type), "");
wjr::uninitialized_copy_n(str.begin(), str.size(), data());
return basic_encode_modifier(data() + str.size(), size() - str.size());
}
template<typename otherChar>
WJR_INLINE_CONSTEXPR20 basic_encode_modifier append(size_type n, otherChar ch) {
static_assert(sizeof(otherChar) <= sizeof(value_type), "");
wjr::uninitialized_fill_n(data(), n, ch);
return basic_encode_modifier(data() + n, size() - n);
}
template<typename otherChar>
WJR_INLINE_CONSTEXPR20 basic_encode_modifier append(otherChar ch) {
static_assert(sizeof(otherChar) <= sizeof(value_type), "");
front() = ch;
return basic_encode_modifier(data() + 1, size() - 1);
}
WJR_INLINE_CONSTEXPR void finalize() {
front() = value_type();
}
template<typename T>
WJR_INLINE_CONSTEXPR void from_integral(
T _Val, iterator* pos = nullptr, int base = 10, errc* _Err = nullptr) noexcept;
using from_f_mode = typename encode_type::from_f_mode;
using from_f_flags = typename encode_type::from_f_flags;
using default_from_f_mode = typename encode_type::default_from_f_mode;
using default_from_f_flags = typename encode_type::default_from_f_flags;
template<typename T,
typename M = default_from_f_mode,
typename F = default_from_f_flags>
WJR_INLINE void from_floating_point(
T value, iterator* pos = nullptr, errc* err = nullptr, int precision = 0, M m = M(), F _Flags = F()) noexcept;
using _Mybase::tolower;
using _Mybase::toupper;
WJR_CONSTEXPR20 void tolower() noexcept;
WJR_CONSTEXPR20 void toupper() noexcept;
};
template<typename Traits>
template<typename T>
WJR_INLINE_CONSTEXPR void basic_encode_modifier<Traits>::from_integral(
T _Val, iterator* pos, int base, errc* err) noexcept {
basic_string_modifier<value_type, traits_type> it(*this);
errc c = errc::ok;
encode_type::from_integral(_Val, it, base, c);
if (pos != nullptr) *pos = it.begin();
if (err != nullptr) *err = c;
}
template<typename Traits>
template<typename T,
typename M,
typename F>
WJR_INLINE void basic_encode_modifier<Traits>::from_floating_point(
T value, iterator* pos, errc* err, int precision, M m, F _Flags) noexcept {
basic_string_modifier<value_type, traits_type> it(*this);
errc c = errc::ok;
encode_type::from_floating_point(value, it, c, precision, m, _Flags);
if (pos != nullptr) *pos = it.begin();
if (err != nullptr) *err = c;
}
template<typename Traits>
WJR_CONSTEXPR20 void basic_encode_modifier<Traits>::tolower() noexcept {
auto first = make_iterator(begin());
auto last = make_iterator(end());
while (first != last) {
*first = tolower(*first);
++first;
}
}
template<typename Traits>
WJR_CONSTEXPR20 void basic_encode_modifier<Traits>::toupper() noexcept {
auto first = make_iterator(begin());
auto last = make_iterator(end());
while (first != last) {
*first = toupper(*first);
++first;
}
}
using ascii_encode = encode::ascii;
using ascii_view = basic_encode_view<encode::ascii_traits>;
using ascii_modifier = basic_encode_modifier<encode::ascii_traits>;
_WJR_END
#endif // __WJR_STRING_FUNC_H

#pragma once
#ifndef __WJR_VARIANT_H
#define __WJR_VARIANT_H
#include <variant>
_WJR_BEGIN
template<size_t from_I, size_t to_I, typename...Vars, typename...Args>
WJR_CONSTEXPR20 tp_at_t<tp_list<Vars...>, to_I>& emplace_from(std::variant<Vars...>& var, Args&&...args) noexcept {
wjr::destroy_at(std::addressof(std::get<from_I>(var)));
wjr::construct_at(std::addressof(var), std::in_place_index<to_I>, std::forward<Args>(args)...);
return std::get<to_I>(var);
}
template<size_t from_I, typename to_T, typename...Vars, typename...Args>
WJR_CONSTEXPR20 to_T& emplace_from(std::variant<Vars...>& var, Args&&...args) noexcept {
wjr::destroy_at(std::addressof(std::get<from_I>(var)));
wjr::construct_at(std::addressof(var), std::in_place_type<to_T>, std::forward<Args>(args)...);
return std::get<to_T>(var);
}
_WJR_END
#endif // __WJR_VARIANT_H

_WJR_BEGIN
class json;
struct json_traits {
template<typename T>
using allocator_type = sallocator<T>;
using encode_type = wjr::encode::ascii;
using null = std::nullptr_t;
using boolean = bool;
using number = double;
using string = basic_string<char, std::char_traits<char>, allocator_type<char>>;
using array = vector<json, allocator_type<json>>;
using object = std::map<string, json, std::less<>, allocator_type<std::pair<const string, json>>>;
using cstring = string*;
using carray = array*;
using cobject = object*;
using ocont = std::variant<null, boolean, number, string, array, object>;
using cont = std::variant<null, boolean, number, cstring, carray, cobject>;
using type_list = tp_list<null, boolean, number, string, array, object, cstring, carray, cobject>;
template<size_t idx>
using ocont_at_t = tp_at_t<ocont, idx>;
template<size_t idx>
using cont_at_t = tp_at_t<cont, idx>;
template<size_t idx>
using type_at_t = tp_at_t<type_list, idx>;
template<typename T>
constexpr static size_t ocont_find_v = tp_find_v<ocont, T>;
template<typename T>
constexpr static size_t cont_find_v = tp_find_v<cont, T>;
template<typename T>
constexpr static size_t type_find_v = tp_find_v<type_list, T>;
template<typename T>
constexpr static size_t find_best_convertible_v = tp_find_best_convertible_v<type_list, T>;
private:
template<typename _Enable, typename...Args>
struct __find_unique_constructible_helper {
constexpr static size_t value = tp_find_unique_constructible_v<type_list, Args...>;
};
template<typename T>
struct __find_unique_constructible_helper<
std::enable_if_t<std::is_integral_v<remove_cvref_t<T>>, void>, T> {
constexpr static size_t value = 2;
};
public:
template<typename...Args>
constexpr static size_t find_unique_constructible_v = __find_unique_constructible_helper<void, Args...>::value;
using key_type = string;
using size_type = size_t;
using object_iterator = typename object::iterator;
using object_const_iterator = typename object::const_iterator;
};
template<size_t idx>
constexpr json_traits::type_at_t<idx>& get(json&);
template<size_t idx>
constexpr const json_traits::type_at_t<idx>& get(const json&);
template<size_t idx>
constexpr json_traits::type_at_t<idx>&& get(json&&);
template<typename T>
constexpr T& get(json&);
template<typename T>
constexpr const T& get(const json& j);
template<typename T>
constexpr T&& get(json&& j);
template<size_t idx>
constexpr std::add_pointer_t<json_traits::type_at_t<idx>> get_if(json*) noexcept;
template<size_t idx>
constexpr std::add_pointer_t<const json_traits::type_at_t<idx>> get_if(const json*)noexcept;
template<typename T>
constexpr std::add_pointer_t<T> get_if(json*) noexcept;
template<typename T>
constexpr std::add_pointer_t<const T> get_if(const json*) noexcept;
class json : public json_traits {
public:
using traits_type = json_traits;
template<typename T>
using allocator_type = typename traits_type::template allocator_type<T>;
using encode_type = typename traits_type::encode_type;
using null = typename traits_type::null;
using boolean = typename traits_type::boolean;
using number = typename traits_type::number;
using string = typename traits_type::string;
using array = typename traits_type::array;
using object = typename traits_type::object;
using cstring = typename traits_type::cstring;
using carray = typename traits_type::carray;
using cobject = typename traits_type::cobject;
using ocont = typename traits_type::ocont;
using cont = typename traits_type::cont;
using type_list = typename traits_type::type_list;
template<size_t idx>
using ocont_at_t = typename traits_type::template ocont_at_t<idx>;
template<size_t idx>
using cont_at_t = typename traits_type::template cont_at_t<idx>;
template<size_t idx>
using type_at_t = typename traits_type::template type_at_t<idx>;
template<typename T>
constexpr static size_t ocont_find_v = traits_type::template ocont_find_v<T>;
template<typename T>
constexpr static size_t cont_find_v = traits_type::template cont_find_v<T>;
template<typename T>
constexpr static size_t type_find_v = traits_type::template type_find_v<T>;
template<typename T>
constexpr static size_t find_best_convertible_v = traits_type::template find_best_convertible_v<T>;
template<typename...Args>
constexpr static size_t find_unique_constructible_v = traits_type::template find_unique_constructible_v<Args...>;
using key_type = typename traits_type::key_type;
using size_type = size_t;
template<typename T>
inline static decltype(auto) value(T&& t) {
using type = remove_cvref_t<T>;
constexpr auto idx = type_find_v<type>;
static_assert(idx < 9, "");
if constexpr (idx < 6) {
return std::forward<T>(t);
}
else {
return *(std::forward<T>(t));
}
}
inline json() noexcept = default;
inline json(const json& other) noexcept {
std::visit([this](const auto& x) {
constexpr auto idx = cont_find_v<WJR_PRIMITIVE_TYPE(x)>;
this->emplace_from<0, idx>(json::value(x));
}, other.m_value);
}
inline json(json&& other)
noexcept : m_value(std::move(other.m_value)) {
other.m_value.emplace<null>();
}
template<size_t idx, typename...Args, std::enable_if_t<(idx >= 0) && (idx < 3), int> = 0>
inline explicit json(std::in_place_index_t<idx>, Args&&...args) noexcept
: m_value(std::in_place_index_t<idx>{}, std::forward<Args>(args)...) {}
template<size_t idx, typename...Args, std::enable_if_t<(idx >= 3 && idx < 6), int> = 0>
inline explicit json(std::in_place_index_t<idx>, Args&&...args) noexcept
: m_value(std::in_place_index_t<idx>{},
allocator_type<type_at_t<idx>>().allocate(1)) {
construct_at(std::get<idx>(m_value), std::forward<Args>(args)...);
}
template<size_t idx, typename...Args, std::enable_if_t<(idx >= 6) && (idx < 9), int> = 0>
inline explicit json(std::in_place_index_t<idx>, Args&&...args) noexcept
: m_value(std::in_place_index_t<idx - 3>{}, std::forward<Args>(args)...) {}
template<typename T, typename... Args, std::enable_if_t<type_find_v<T> != -1, int> = 0>
inline explicit json(std::in_place_type_t<T>, Args&&...args) noexcept
: json(std::in_place_index_t<type_find_v<T>>{}, std::forward<Args>(args)...) {}
template<typename T, std::enable_if_t<
find_best_convertible_v<T&&> != -1
&& !is_in_place_v<remove_cvref_t<T>>
&& !std::is_same_v<remove_cvref_t<T>, json>, int> = 0>
inline json(T&& t) noexcept
: json(std::in_place_index_t<find_best_convertible_v<T&&>>{}, std::forward<T>(t)) {}
template<typename T, std::enable_if_t<
find_best_convertible_v<T&&> == -1
&& find_unique_constructible_v<T&&> != -1
&& !is_in_place_v<remove_cvref_t<T>>
&& !std::is_same_v<remove_cvref_t<T>, json>, int> = 0>
inline explicit json(T&& t) noexcept
: json(std::in_place_index_t<find_unique_constructible_v<T&&>>{},
std::forward<T>(t)) {}
template<typename T, typename U, typename...Args, std::enable_if_t<
find_unique_constructible_v<T&&, U&&, Args&&...> != -1
&& !is_in_place_v<remove_cvref_t<T>>, int> = 0>
inline explicit json(T&& t, U&& u, Args&&...args) noexcept
: json(std::in_place_index_t<find_unique_constructible_v<T&&, U&&, Args&&...>>{},
std::forward<T>(t), std::forward<U>(u), std::forward<Args>(args)...) {}
struct parse_tag {};
json(const char*& s, const char* e, parse_tag);
inline json& operator=(const json& other) noexcept {
if (this == std::addressof(other)) {
return *this;
}
std::visit([this](const auto& x) {
constexpr auto idx = cont_find_v<remove_cvref_t<decltype(x)>>;
this->emplace<idx>(json::value(x));
}, other.m_value);
return *this;
}
inline json& operator=(json&& other) noexcept {
if (this == std::addressof(other)) {
return *this;
}
m_value = std::move(other.m_value);
other.m_value.emplace<null>();
return *this;
}
template<typename T, std::enable_if_t<
find_best_convertible_v<T&&> != -1
&& !is_in_place_v<remove_cvref_t<T>>
&& !std::is_same_v<remove_cvref_t<T>, json>, int> = 0>
inline json& operator=(T&& t) noexcept {
emplace<find_best_convertible_v<T&&>>(std::forward<T>(t));
return *this;
}
inline ~json() noexcept {
tidy();
}
private:
template<size_t I, std::enable_if_t<(I < 6), int> = 0>
inline void tidy_from() noexcept {
if constexpr (I < 3) {
}
else {
using value_type = type_at_t<I>;
auto ptr = std::get<I>(m_value);
ptr->~value_type();
allocator_type<value_type>().deallocate(ptr, 1);
}
}
public:
inline void tidy() noexcept {
std::visit([this](const auto& x) {
constexpr auto idx = cont_find_v<WJR_PRIMITIVE_TYPE(x)>;
this->tidy_from<idx>();
}, m_value);
}
template<size_t from_I, size_t to_I, typename...Args, std::enable_if_t<
(from_I < 6 && to_I < 9), int> = 0>
inline type_at_t<to_I>& emplace_from(Args&&...args) noexcept {
if constexpr (to_I < 3) {
tidy_from<from_I>();
return wjr::emplace_from<from_I, to_I>(m_value, std::forward<Args>(args)...);
}
else if constexpr (to_I < 6) {
if constexpr (from_I == to_I) {
auto ptr = std::addressof(*std::get<from_I>(m_value));
wjr::destroy_at(ptr);
wjr::construct_at(ptr, std::forward<Args>(args)...);
return *ptr;
}
else {
tidy_from<from_I>();
using value_type = type_at_t<to_I>;
auto ptr = allocator_type<value_type>().allocate(1);
wjr::construct_at(ptr, std::forward<Args>(args)...);
return *wjr::emplace_from<from_I, to_I>(m_value, ptr);
}
}
else {
tidy_from<from_I>();
return wjr::emplace_from<from_I, to_I - 3>(m_value, std::forward<Args>(args)...);
}
}
template<size_t from_I, typename to_T, typename...Args, std::enable_if_t<
(from_I < 6 && type_find_v<to_T> != -1), int> = 0>
inline to_T& emplace_from(Args&&...args) noexcept {
return emplace_from<from_I, type_find_v<to_T>>(std::forward<Args>(args)...);
}
template<size_t I, typename...Args, std::enable_if_t<(I < 9), int> = 0>
inline type_at_t<I>& emplace(Args&&...args) noexcept {
return std::visit([this, tp = std::forward_as_tuple(std::forward<Args>(args)...)](const auto& x) ->type_at_t<I>&{
WJR_MAYBE_UNUSED constexpr auto idx = cont_find_v<WJR_PRIMITIVE_TYPE(x)>;
return std::apply([this](auto&&...args) ->type_at_t<I>&{
return this->emplace_from<idx, I>(std::forward<decltype(args)>(args)...);
}, std::move(tp));
}, m_value);
}
template<typename T, typename...Args, std::enable_if_t<
tp_contains_v<type_list, T>, int> = 0>
inline T& emplace(Args&&...args) noexcept {
return emplace<type_find_v<T>>(std::forward<Args>(args)...);
}
inline void swap(json& other) noexcept {
m_value.swap(other.m_value);
}
WJR_ATTRIBUTE(NODISCARD, PURE) static bool accept(const char* first, const char* last);
WJR_ATTRIBUTE(NODISCARD, PURE) static json parse(const char* first, const char* last) {
return json(first, last, parse_tag{});
}
enum stringify_mode {
SHORTEST = 0,
TAB_ALIGN = 1, // 1 tab
TWO_SPACE_ALIGN = 2, // 2 space
FOUR_SPACE_ALIGN = 3, // 4 space
};
string stringify(stringify_mode m = TWO_SPACE_ALIGN) const noexcept {
string ret;
switch (m) {
case SHORTEST: _stringify<SHORTEST>(ret, 0); break;
case TAB_ALIGN: _stringify<TAB_ALIGN>(ret, 0); break;
case TWO_SPACE_ALIGN: _stringify<TWO_SPACE_ALIGN>(ret, 0); break;
case FOUR_SPACE_ALIGN: _stringify<FOUR_SPACE_ALIGN>(ret, 0); break;
default: WJR_UNREACHABLE; break;
}
return ret;
}
WJR_ATTRIBUTE(PURE, INLINE) size_t index() const { return m_value.index(); }
WJR_ATTRIBUTE(PURE, INLINE) const char* type_name() const {
switch (m_value.index()) {
case 0: return "null";
case 1: return "boolean";
case 2: return "number";
case 3: return "string";
case 4: return "array";
case 5: return "object";
default: WJR_UNREACHABLE;
}
}
WJR_ATTRIBUTE(PURE, INLINE) cont& get()& { return m_value; }
WJR_ATTRIBUTE(PURE, INLINE) cont&& get()&& { return std::move(m_value); }
WJR_ATTRIBUTE(PURE, INLINE) const cont& get() const& { return m_value; }
#define WJR_REGISTER_JSON_GET_FUNC(x)															\
WJR_PURE WJR_INLINE x& get_##x() noexcept {return wjr::get<x>(*this);}	                        \
WJR_PURE WJR_INLINE  const x& get_##x() const noexcept {return wjr::get<x>(*this);}	            \
WJR_PURE WJR_INLINE  x* get_if_##x() noexcept {return wjr::get_if<x>(this);}	                \
WJR_PURE WJR_INLINE  const x* get_if_##x() const noexcept {return wjr::get_if<x>(this);}
WJR_MACRO_CALL(WJR_REGISTER_JSON_GET_FUNC, , null, boolean, number, string, array, object, cstring, carray, cobject);
#undef WJR_REGISTER_JSON_GET_FUNC
#define WJR_REGISTER_JSON_EMPLACE_FUNC(x)	\
template<typename...Args>	\
inline x& emplace_##x(Args&&...args) noexcept{ return emplace<x>(std::forward<Args>(args)...);}
WJR_MACRO_CALL(WJR_REGISTER_JSON_EMPLACE_FUNC, , null, boolean, number, string, array, object, cstring, carray, cobject);
#undef WJR_REGISTER_JSON_EMPLACE_FUNC
object_iterator find(const key_type& key) { return get_object().find(key); }
object_const_iterator find(const key_type& key) const { return get_object().find(key); }
template<typename Key>
object_iterator find(const Key& key) { return get_object().find(key); }
template<typename Key>
object_const_iterator find(const Key& key) const { return get_object().find(key); }
object_iterator lower_bound(const key_type& key) { return get_object().lower_bound(key); }
object_const_iterator lower_bound(const key_type& key) const { return get_object().lower_bound(key); }
template<typename Key>
object_iterator lower_bound(const Key& key) { return get_object().lower_bound(key); }
template<typename Key>
object_const_iterator lower_bound(const Key& key) const { return get_object().lower_bound(key); }
object_iterator upper_bound(const key_type& key) { return get_object().upper_bound(key); }
object_const_iterator upper_bound(const key_type& key) const { return get_object().upper_bound(key); }
template<typename Key>
object_iterator upper_bound(const Key& key) { return get_object().upper_bound(key); }
template<typename Key>
object_const_iterator upper_bound(const Key& key) const { return get_object().upper_bound(key); }
json& at(size_type n) { return get_array().at(n); }
const json& at(size_type n) const { return get_array().at(n); }
json& at(const key_type& key) { return get_object().at(key); }
const json& at(const key_type& key) const { return get_object().at(key); }
template<typename Key>
json& at(const Key& key) {
auto iter = find(key);
if (iter == get_object().end()) {
throw std::out_of_range("invalid json key");
}
return iter->second;
}
template<typename Key>
const json& at(const Key& key) const {
auto iter = find(key);
if (iter == get_object().end()) {
throw std::out_of_range("invalid json key");
}
return iter->second;
}
json& operator[](size_type n) { return get_array()[n]; }
const json& operator[](size_type n) const { return get_array()[n]; }
json& operator[](const key_type& key) {
return get_object()[key];
}
json& operator[](key_type&& key) {
return get_object()[std::move(key)];
}
template<typename T, std::enable_if_t<std::is_constructible_v<key_type, T&&>, int> = 0>
json& operator[](T&& key) {
return get_object()[static_cast<key_type>(std::forward<T>(key))];
}
template<typename Key>
const json& operator[](const Key& key) const {
return at(key);
}
private:
static bool __accept(const char*& first, const char* last, uint8_t state);
template<int m>
void _stringify(string&, int) const noexcept;
cont m_value;
};
extern template void json::_stringify<0>(string&, int) const noexcept;
extern template void json::_stringify<1>(string&, int) const noexcept;
extern template void json::_stringify<2>(string&, int) const noexcept;
extern template void json::_stringify<3>(string&, int) const noexcept;
template<size_t idx>
constexpr json_traits::type_at_t<idx>& get(json& it) {
if constexpr (idx < 3) {
return std::get<idx>(it.get());
}
else if constexpr (idx < 6) {
return *(std::get<idx>(it.get()));
}
else {
return std::get<idx - 3>(it.get());
}
}
template<size_t idx>
constexpr const json_traits::type_at_t<idx>& get(const json& it) {
if constexpr (idx < 3) {
return std::get<idx>(it.get());
}
else if constexpr (idx < 6) {
return *(std::get<idx>(it.get()));
}
else {
return std::get<idx - 3>(it.get());
}
}
template<size_t idx>
constexpr json_traits::type_at_t<idx>&& get(json&& it) {
if constexpr (idx < 3) {
return std::get<idx>(std::move(it).get());
}
else if constexpr (idx < 6) {
return *(std::get<idx>(std::move(it).get()));
}
else {
return std::get<idx - 3>(std::move(it).get());
}
}
template<typename T>
constexpr T& get(json& it) {
return wjr::get<json::type_find_v<T>>(it);
}
template<typename T>
constexpr const T& get(const json& it) {
return wjr::get<json::type_find_v<T>>(it);
}
template<typename T>
constexpr T&& get(json&& it) {
return wjr::get<json::type_find_v<T>>(std::move(it));
}
template<size_t idx>
constexpr std::add_pointer_t<json_traits::type_at_t<idx>> get_if(json* it) noexcept {
if constexpr (idx < 3) {
return std::get_if<idx>(std::addressof(it->get()));
}
else if constexpr (idx < 6) {
auto ptr = std::get_if<idx>(std::addressof(it->get()));
if (ptr == nullptr) {
return nullptr;
}
return std::addressof(**ptr);
}
else {
return std::get_if<idx - 3>(std::addressof(it->get()));
}
}
template<size_t idx>
constexpr std::add_pointer_t<const json_traits::type_at_t<idx>> get_if(const json* it) noexcept {
if constexpr (idx < 3) {
return std::get_if<idx>(std::addressof(it->get()));
}
else if constexpr (idx < 6) {
auto ptr = std::get_if<idx>(std::addressof(it->get()));
if (ptr == nullptr) {
return nullptr;
}
return std::addressof(**ptr);
}
else {
return std::get_if<idx - 3>(std::addressof(it->get()));
}
}
template<typename  T>
constexpr std::add_pointer_t<T> get_if(json* it) noexcept {
return get_if<json::type_find_v<T>>(it);
}
template<typename  T>
constexpr std::add_pointer_t<const T> get_if(const json* it) noexcept {
return get_if<json::type_find_v<T>>(it);
}
template<typename Func, typename...Var, std::enable_if_t<
std::conjunction_v<std::is_same<remove_cvref_t<Var>, json>...>, int> = 0>
constexpr decltype(auto) visit(Func&& fn, Var&&...var) {
return std::visit([_Fn = std::forward<Func>(fn)](auto&&...args) {
return _Fn(json::value(std::forward<decltype(args)>(args))...);
}, (std::forward<Var>(var).get())...);
}
inline void swap(json& a, json& b) noexcept {
a.swap(b);
}
extern bool operator==(const json& a, const json& b);
inline bool operator!=(const json& a, const json& b) { return !(a == b); }
_WJR_END
#endif // __WJR_JSON_H
#include <array>
_WJR_BEGIN
constexpr static std::array<size_t, 256> string_skip_step = {
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
constexpr iter skip_string(iter first, WJR_MAYBE_UNUSED iter last) {
using value_t = typename std::iterator_traits<iter>::value_type;
using uvalue_t = std::make_unsigned_t<value_t>;
if constexpr (std::numeric_limits<uvalue_t>::max() < 256) {
if constexpr (std::is_same_v<value_t, char> && is_contiguous_iter_v<iter>) {
do {
auto pos = wjr::find(first, last, '"');
auto pos2 = wjr::find_if(std::reverse_iterator(pos), std::reverse_iterator(first), [](auto x) {
return x != '\\';
});
if (static_cast<size_t>(pos - pos2.base()) % 2 == 0) {
return pos;
}
first = pos + 1;
} while (true);
}
else {
auto u_value = make_unsigned_v(*first);
while (string_skip_step[u_value]) {
first += string_skip_step[u_value];
u_value = make_unsigned_v(*first);
}
return first;
}
}
else {
auto u_value = make_unsigned_v(*first);
while (u_value >= 256 || string_skip_step[u_value]) {
first += u_value >= 256 ? 1 : string_skip_step[u_value];
u_value = make_unsigned_v(*first);
}
return first;
}
}
template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
constexpr static bool check_string(iter& s, iter e) {
using encode_type = typename json_traits::encode_type;
if (*s != '"')return false;
++s;
while (s != e && *s != '"') {
if (*s == '\\') {
++s;
if (s == e)return false;
switch (*s) {
case '"':
case '\\':
case '/':
case 'b':
case 'f':
case 'n':
case 'r':
case 't':
break;
case 'u':
if (s + 4 > e)return false;
if (!encode_type::isalnum(*s)) {
return false;
}
++s;
if (!encode_type::isalnum(*s)) {
return false;
}
++s;
if (!encode_type::isalnum(*s)) {
return false;
}
++s;
if (!encode_type::isalnum(*s)) {
return false;
}
++s;
break;
default:
return false;
}
}
else {
if ((*s >= 0 && *s < 32) || *s == 127)
return false;
}
++s;
}
return s != e;
}
bool json::__accept(const char*& first, const char* last, uint8_t state) {
bool head = true;
for (;; ++first) {
first = encode_type::skipw(first, last);
if (first == last)return false;
if (state == '}') {
if (*first == '}') {
if (!head) return false;
++first;
return true;
}
if (*first != '"')return false;
if (!check_string(first, last))
return false;
++first;
first = encode_type::skipw(first, last);
if (first == last || *first != ':') return false;
++first;
first = encode_type::skipw(first, last);
if (first == last) return false;
}
else {
if (*first == ']') {
if (!head)return false;
++first;
return true;
}
}
head = false;
switch (*first) {
case 'n':
if (first + 3 < last) {
if(memcmp(first + 1, "ull", 3) == 0) {
first += 4;
break;
}
}
return false;
case 't':
if (first + 3 < last) {
if (memcmp(first + 1, "rue", 3) == 0) {
first += 4;
break;
}
}
return false;
case 'f':
if (first + 4 < last) {
if (memcmp(first + 1, "alse", 4) == 0) {
first += 5;
break;
}
}
return false;
case '"':
if (!check_string(first, last))
return false;
++first;
break;
case '[':
++first;
if (!__accept(first, last, ']'))
return false;
break;
case '{':
++first;
if (!__accept(first, last, '}'))
return false;
break;
default:
const char* pos = nullptr;
errc c = errc::ok;
(void)encode_type::to_floating_point<double>(first, last, &pos, &c,
std::integral_constant<encode_type::to_f_flags, encode_type::to_f_flags::ALLOW_TRAILING_JUNK>());
if (c != errc::ok)
return false;
first = pos;
break;
}
first = encode_type::skipw(first, last);
if (first == last)return false;
if (*first == ',')
continue;
if (*first != state)
return false;
++first;
return true;
}
}
bool json::accept(const char* first, const char* last) {
first = encode_type::skipw(first, last);
if (first == last) {
return false;
}
switch (*first) {
case '[':
++first;
if (!__accept(first, last, ']'))
return false;
first = encode_type::skipw(first, last);
return first == last;
case '{':
++first;
if (!__accept(first, last, '}'))
return false;
first = encode_type::skipw(first, last);
return first == last;
default:
return false;
}
}
json::json(const char*& first, const char* last, parse_tag) {
first = encode_type::skipw(first, last);
switch (*first) {
case 'n': {
this->emplace_from<0, null>();
first += 4;
break;
}
case 't': {
this->emplace_from<0, boolean>(true);
first += 4;
break;
}
case 'f': {
this->emplace_from<0, boolean>(false);
first += 5;
break;
}
case '"': {
++first;
auto t = skip_string(first, last);
this->emplace_from<0, string>(first, t);
first = t + 1;
break;
}
case '[': {
this->emplace_from<0, array>();
auto& arr = this->get_array();
++first;
first = encode_type::skipw(first, last);
if (*first == ']') {
++first;
break;
}
for (;;) {
arr.emplace_back(first, last, parse_tag{});
first = encode_type::skipw(first, last);
if (*first == ']') {
++first;
break;
}
++first;
}
arr.shrink_to_fit();
break;
}
case '{': {
this->emplace_from<0, object>();
auto& obj = this->get_object();
++first;
first = encode_type::skipw(first, last);
if (*first == '}') {
++first;
break;
}
for(;;) {
first = encode_type::skipw(first, last);
++first;
auto p = skip_string(first, last);
string name(first, p);
first = p + 1;
first = encode_type::skipw(first, last);
++first;
obj.emplace(std::piecewise_construct,
std::forward_as_tuple(std::move(name)),
std::forward_as_tuple(first, last, parse_tag{}));
first = encode_type::skipw(first, last);
if (*first == '}') {
++first;
break;
}
++first;
}
break;
}
default: {
const char* pos = nullptr;
auto val = encode_type::to_floating_point<double>(first, last, &pos, nullptr,
std::integral_constant<encode_type::to_f_flags, encode_type::to_f_flags::ALLOW_TRAILING_JUNK>());
first = pos;
this->emplace_from<0, number>(val);
break;
}
}
}
template<int m>
void json::_stringify(string& str, int a) const noexcept {
constexpr int LEN = []() {
switch (m) {
case SHORTEST:return 0;
case TAB_ALIGN: return 1;
case TWO_SPACE_ALIGN: return 2;
case FOUR_SPACE_ALIGN: return 4;
}
}();
switch (m_value.index()) {
case 0: {
str.append("null");
break;
}
case 1: {
if (get_boolean()) {
str.append("true");
}
else {
str.append("false");
}
break;
}
case 2: {
str.reserve(str.size() + 256);
ascii_modifier it(str.lastPtr(), str.endPtr() - str.lastPtr());
char* e = nullptr;
it.from_floating_point(get_number(), &e);
str.set_size(e - str.data());
str.finalize();
break;
}
case 3: {
str.append('"').append(get_string()).append('"');
break;
}
case 4: {
const auto& arr = get_array();
str.push_back('[');
if (!arr.empty()) {
bool head = true;
for (const auto& val : arr) {
if (head) head = false;
else str.push_back(',');
if constexpr (m != SHORTEST) {
str.push_back('\n');
if constexpr (m == TAB_ALIGN) {
str.append(a + LEN, '\t');
}
else {
str.append(a + LEN, ' ');
}
}
val.template _stringify<m>(str, a + LEN);
}
if constexpr (m != SHORTEST) {
str.push_back('\n');
if constexpr (m == TAB_ALIGN) {
str.append(a + LEN, '\t');
}
else {
str.append(a + LEN, ' ');
}
}
}
str.push_back(']');
break;
}
case 5: {
auto& obj = get_object();
str.push_back('{');
if (!obj.empty()) {
bool head = true;
for (const auto& [name, val] : obj) {
if (head) head = false;
else str.push_back(',');
if constexpr (m != SHORTEST) {
str.push_back('\n');
if constexpr (m == TAB_ALIGN) {
str.append(a + LEN, '\t');
}
else {
str.append(a + LEN, ' ');
}
}
str.append('"').append(name).append("\": ");
val.template _stringify<m>(str, a + LEN);
}
if constexpr (m != SHORTEST) {
str.push_back('\n');
if constexpr (m == TAB_ALIGN) {
str.append(a, '\t');
}
else {
str.append(a, ' ');
}
}
}
str.push_back('}');
break;
}
default:WJR_UNREACHABLE; break;
}
}
template void json::_stringify<0>(string& str, int a) const noexcept;
template void json::_stringify<1>(string& str, int a) const noexcept;
template void json::_stringify<2>(string& str, int a) const noexcept;
template void json::_stringify<3>(string& str, int a) const noexcept;
bool operator==(const json& a, const json& b) {
if (a.index() != b.index()) {
return false;
}
return wjr::visit([&b](const auto& x) {
using value_type = remove_cvref_t<decltype(x)>;
return x == wjr::get<value_type>(b);
}, a);
}
_WJR_END


