
#pragma once
#ifndef __WJR_STRING_H
#define __WJR_STRING_H

#include <string_view>
#include <iostream>
#include <cctype>
#include <locale>
#include <cstring>
#include <charconv>

#pragma once
#ifndef __WJR_VECTOR_H
#define __WJR_VECTOR_H

#pragma push_macro("new")
#undef new

#include <stdexcept>
#include <string>

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

// judge if i can use inline asm
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
// It's a big-endian target architecture
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
#define _WJR_FAST_MEMSKIPW
#define _WJR_FAST_MEMCMP
#define _WJR_FAST_MEMMIS
//#define _WJR_FAST_MEMCNT
#define _WJR_FAST_MEMSET
//#define _WJR_FAST_MEMCPY
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
//#define WJR_HAS_EXECUTION
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

// NOINLINE for MSVC/GCC/CLANG ...
#if WJR_HAS_ATTRIBUTE(noinline)
#define WJR_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define WJR_NOINLINE __declspec(noinline)
#else
#define WJR_NOINLINE
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


// Compiler support for constexpr
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

#define WJR_FORCEINLINE_CONSTEXPR WJR_FORCEINLINE constexpr
#define WJR_FORCEINLINE_CONSTEXPR20 WJR_FORCEINLINE WJR_CONSTEXPR20
#define WJR_FORCEINLINE_E_CONSTEXPR WJR_FORCEINLINE WJR_E_CONSTEXPR

#define WJR_INTRINSIC_CONSTEXPR WJR_INTRINSIC_INLINE constexpr
#define WJR_INTRINSIC_CONSTEXPR20 WJR_INTRINSIC_INLINE WJR_CONSTEXPR20
#define WJR_INTRINSIC_E_CONSTEXPR WJR_INTRINSIC_INLINE WJR_E_CONSTEXPR

#define WJR_INLINE_CONSTEXPR inline constexpr
#define WJR_INLINE_CONSTEXPR20 inline WJR_CONSTEXPR20
#define WJR_INLINE_E_CONSTEXPR inline WJR_E_CONSTEXPR

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

#define WJR_MACRO_NULL(...)

#define WJR_MACRO_LABEL(NAME) __wjr_label_##NAME

#if defined(WJR_MSVC)
#define WJR_COUNTER __COUNTER__
#else
#define WJR_COUNTER __LINE__
#endif

#define WJR_CONCAT(x, y) x##y
#define WJR_MACRO_CONCAT(x, y) WJR_CONCAT(x, y)

#define WJR_EXPAND(x) x

#define __WJR_DEFINE_0(x)
#define __WJR_DEFINE_1(x) x
#define WJR_DEFINE(x, y) __WJR_DEFINE_##y (x)

#define WJR_STR(x) #x
#define WJR_MACRO_STR(x) WJR_STR(x)

#define WJR_MACRO_IOTA(N) WJR_MACRO_IOTA_##N
#define WJR_MACRO_IOTA_0
#define WJR_MACRO_IOTA_1 1
#define WJR_MACRO_IOTA_2 WJR_MACRO_IOTA_1, 2
#define WJR_MACRO_IOTA_3 WJR_MACRO_IOTA_2, 3
#define WJR_MACRO_IOTA_4 WJR_MACRO_IOTA_3, 4
#define WJR_MACRO_IOTA_5 WJR_MACRO_IOTA_4, 5
#define WJR_MACRO_IOTA_6 WJR_MACRO_IOTA_5, 6
#define WJR_MACRO_IOTA_7 WJR_MACRO_IOTA_6, 7
#define WJR_MACRO_IOTA_8 WJR_MACRO_IOTA_7, 8
#define WJR_MACRO_IOTA_9 WJR_MACRO_IOTA_8, 9
#define WJR_MACRO_IOTA_10 WJR_MACRO_IOTA_9, 10
#define WJR_MACRO_IOTA_11 WJR_MACRO_IOTA_10, 11
#define WJR_MACRO_IOTA_12 WJR_MACRO_IOTA_11, 12
#define WJR_MACRO_IOTA_13 WJR_MACRO_IOTA_12, 13
#define WJR_MACRO_IOTA_14 WJR_MACRO_IOTA_13, 14
#define WJR_MACRO_IOTA_15 WJR_MACRO_IOTA_14, 15
#define WJR_MACRO_IOTA_16 WJR_MACRO_IOTA_15, 16
#define WJR_MACRO_IOTA_17 WJR_MACRO_IOTA_16, 17
#define WJR_MACRO_IOTA_18 WJR_MACRO_IOTA_17, 18
#define WJR_MACRO_IOTA_19 WJR_MACRO_IOTA_18, 19
#define WJR_MACRO_IOTA_20 WJR_MACRO_IOTA_19, 20
#define WJR_MACRO_IOTA_21 WJR_MACRO_IOTA_20, 21
#define WJR_MACRO_IOTA_22 WJR_MACRO_IOTA_21, 22
#define WJR_MACRO_IOTA_23 WJR_MACRO_IOTA_22, 23
#define WJR_MACRO_IOTA_24 WJR_MACRO_IOTA_23, 24
#define WJR_MACRO_IOTA_25 WJR_MACRO_IOTA_24, 25
#define WJR_MACRO_IOTA_26 WJR_MACRO_IOTA_25, 26
#define WJR_MACRO_IOTA_27 WJR_MACRO_IOTA_26, 27
#define WJR_MACRO_IOTA_28 WJR_MACRO_IOTA_27, 28
#define WJR_MACRO_IOTA_29 WJR_MACRO_IOTA_28, 29
#define WJR_MACRO_IOTA_30 WJR_MACRO_IOTA_29, 30
#define WJR_MACRO_IOTA_31 WJR_MACRO_IOTA_30, 31
#define WJR_MACRO_IOTA_32 WJR_MACRO_IOTA_31, 32
#define WJR_MACRO_IOTA_33 WJR_MACRO_IOTA_32, 33
#define WJR_MACRO_IOTA_34 WJR_MACRO_IOTA_33, 34
#define WJR_MACRO_IOTA_35 WJR_MACRO_IOTA_34, 35
#define WJR_MACRO_IOTA_36 WJR_MACRO_IOTA_35, 36
#define WJR_MACRO_IOTA_37 WJR_MACRO_IOTA_36, 37
#define WJR_MACRO_IOTA_38 WJR_MACRO_IOTA_37, 38
#define WJR_MACRO_IOTA_39 WJR_MACRO_IOTA_38, 39
#define WJR_MACRO_IOTA_40 WJR_MACRO_IOTA_39, 40
#define WJR_MACRO_IOTA_41 WJR_MACRO_IOTA_40, 41
#define WJR_MACRO_IOTA_42 WJR_MACRO_IOTA_41, 42
#define WJR_MACRO_IOTA_43 WJR_MACRO_IOTA_42, 43
#define WJR_MACRO_IOTA_44 WJR_MACRO_IOTA_43, 44
#define WJR_MACRO_IOTA_45 WJR_MACRO_IOTA_44, 45
#define WJR_MACRO_IOTA_46 WJR_MACRO_IOTA_45, 46
#define WJR_MACRO_IOTA_47 WJR_MACRO_IOTA_46, 47
#define WJR_MACRO_IOTA_48 WJR_MACRO_IOTA_47, 48
#define WJR_MACRO_IOTA_49 WJR_MACRO_IOTA_48, 49
#define WJR_MACRO_IOTA_50 WJR_MACRO_IOTA_49, 50
#define WJR_MACRO_IOTA_51 WJR_MACRO_IOTA_50, 51
#define WJR_MACRO_IOTA_52 WJR_MACRO_IOTA_51, 52
#define WJR_MACRO_IOTA_53 WJR_MACRO_IOTA_52, 53
#define WJR_MACRO_IOTA_54 WJR_MACRO_IOTA_53, 54
#define WJR_MACRO_IOTA_55 WJR_MACRO_IOTA_54, 55
#define WJR_MACRO_IOTA_56 WJR_MACRO_IOTA_55, 56
#define WJR_MACRO_IOTA_57 WJR_MACRO_IOTA_56, 57
#define WJR_MACRO_IOTA_58 WJR_MACRO_IOTA_57, 58
#define WJR_MACRO_IOTA_59 WJR_MACRO_IOTA_58, 59
#define WJR_MACRO_IOTA_60 WJR_MACRO_IOTA_59, 60
#define WJR_MACRO_IOTA_61 WJR_MACRO_IOTA_60, 61
#define WJR_MACRO_IOTA_62 WJR_MACRO_IOTA_61, 62
#define WJR_MACRO_IOTA_63 WJR_MACRO_IOTA_62, 63
#define WJR_MACRO_IOTA_64 WJR_MACRO_IOTA_63, 64

// don't support 0 agument
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

// WJR_MACRO_CALL(Func, OP, ...) -> Func(Args) OP ...
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

WJR_INTRINSIC_CONSTEXPR bool is_likely(bool f) noexcept {
#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
return __builtin_expect(f, true);
#else
return f;
#endif
}

WJR_INTRINSIC_CONSTEXPR bool is_unlikely(bool f) noexcept {
#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
return __builtin_expect(f, false);
#else
return f;
#endif
}

WJR_INTRINSIC_CONSTEXPR bool is_constant_evaluated() noexcept {
#if defined(__cpp_lib_is_constant_evaluated)
return std::is_constant_evaluated();
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated) || WJR_HAS_GCC(9,1,0) || WJR_HAS_CLANG(9,0,0)
return __builtin_is_constant_evaluated();
#else
return false;
#endif
}

WJR_NORETURN WJR_INTRINSIC_INLINE void unreachable() noexcept {
WJR_UNREACHABLE;
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

// disable tag, for example, used in enable_if_t
struct disable_tag {};

// used in construct_at, use default construct instead of value construct
struct default_construct_tag {};
struct value_construct_tag {};

// used in string
struct extend_tag {};
// used if the memory is reserved for optimize
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

// find if T is in Args...
template<typename T, typename...Args>
struct is_any_of : std::disjunction<std::is_same<T, Args>...> {};

template<typename T, typename...Args>
inline constexpr bool is_any_of_v = is_any_of<T, Args...>::value;

// find if i is in _Index...
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

template<typename T>
using add_lref_t = std::add_lvalue_reference_t<T>;

template<typename T>
using add_rref_t = std::add_rvalue_reference_t<T>;

template<typename T>
using add_cvref_t = add_lref_t<std::add_cv_t<T>>;

template<typename T>
using add_cref_t = add_lref_t<std::add_const_t<T>>;

// default comparator
// can be used for optimize, such as find, compare...
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

// used to unwrap std::reference_wrapper
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

// weak judgment for iterator type
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

// dont't support int128 / uint128 yet

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

// make T (must be integral) to my int/uint type
// for example, bool/char/signed char/unsigne char will be normalized to uint8_t

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

// note that (int8_t)(-1) != (uint8_t)(-1)
// but in memory they are the same
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
constexpr auto __nt_negone = static_cast<cat>(static_cast<nt>(-1));

if constexpr (std::is_signed_v<nt> && std::is_unsigned_v<cat>) {
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

// non-bool integral
template<typename T>
struct is_standard_numer : std::conjunction<std::is_arithmetic<T>,
std::negation<std::is_same<std::remove_cv_t<T>, bool>>> {};

template<typename T>
inline constexpr bool is_standard_numer_v = is_standard_numer<T>::value;

// reverse<iter> is
// but reverse<reverse<iter>> is not
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

// enum ops
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

// constexpr endian
enum class endian {
little = 0,
big = 1,
native = __is_little_endian_helper::value ? little : big
};

// make ptr to void*
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

// when the iterator is not a pointer and there is no operator->() and
// no pointer_traits<T>::to_address, but there is operator*()
// use get_address instead of to_address
// notice that if the object pointed to by the pointer does not exist,
// an error may occur, but this cannot be avoided unless the to_address related functions are provided
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

// type identity
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

// The default allocator uses the default constructor and destructor, which is beneficial to optimization.
// But if the constructor and destructor of the allocator are not default, then the allocator cannot be used.
template<typename T>
struct is_default_allocator : std::false_type {};

template<typename T>
struct is_default_allocator<std::allocator<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_default_allocator_v = is_default_allocator<T>::value;

WJR_REGISTER_HAS_MEMBER_FUNCTION(destroy, destroy);
WJR_REGISTER_HAS_MEMBER_FUNCTION(construct, construct);

// Use the default destructor, which is beneficial for optimization.
// But if the destructor of the allocator has side effects, then do not use this allocator.

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

// using empty base optimization

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
WJR_CONSTEXPR20 _Iter find(_Iter _First, _Iter _Last, const _Ty& _val, _Pred pred);

template<typename _Iter, typename _Ty>
WJR_CONSTEXPR20 _Iter find(_Iter _First, _Iter _Last, const _Ty& _val);

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 _Iter find_if(_Iter _First, _Iter _Last, _Pr _Pred);

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 _Iter find_if_not(_Iter _First, _Iter _Last, _Pr _Pred);

template<typename _Iter, typename _Ty>
WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
count(_Iter _First, _Iter _Last, const _Ty& _Val);

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
count_if(_Iter _First, _Iter _Last, _Pr _Pred);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool lexicographical_compare(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool lexicographical_compare(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 int compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 int compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2);

template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void fill(_Iter _First, _Iter _Last, const _Val& value);

template<typename _Iter, typename _Size, typename _Val>
WJR_CONSTEXPR20 _Iter fill_n(_Iter _First, _Size count, const _Val& value);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output copy(_Input _First1, _Input _Last1, _Output _First2);

template<typename _Input, typename _Size, typename _Output>
WJR_CONSTEXPR20 _Output copy_n(_Input _First1, _Size count, _Output _First2);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output copy_backward(_Input _First1, _Input _Last1, _Output _Last2);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output move(_Input _First1, _Input _Last1, _Output _First2);

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output move_backward(_Input _First1, _Input _Last1, _Output _Last2);

template<typename _Iter, typename _Pred = std::equal_to<>>
class default_searcher;

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter1 search(_Iter1 _First1, _Iter1 _Last1,
_Iter2 _First2, _Iter2 _Last2);

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 _Iter1 search(_Iter1 _First1, _Iter1 _Last1,
_Iter2 _First2, _Iter2 _Last2, _Pred pred);

template<typename _Iter, typename _Searcher>
WJR_CONSTEXPR20 _Iter search(_Iter _First, _Iter _Last, const _Searcher& _Srch);

template<typename _Iter, typename...Args,
std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int> = 0>
WJR_CONSTEXPR20 void construct_at(_Iter iter, Args&&... args);

template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int> = 0>
WJR_CONSTEXPR20 void construct_at(_Iter iter, default_construct_tag);

template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int> = 0>
WJR_CONSTEXPR20 void construct_at(_Iter iter, value_construct_tag);

template<typename Alloc, typename _Iter, typename...Args,
std::enable_if_t<!is_iterator_v<wjr::remove_cvref_t<Alloc>>, int> = 0>
WJR_CONSTEXPR20 void construct_at(Alloc& al, _Iter iter, Args&&...args);

template<typename _Iter>
WJR_CONSTEXPR20 void destroy_at(_Iter ptr);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void destroy_at(Alloc& al, _Iter iter);

template<typename _Iter>
WJR_CONSTEXPR20 void destroy(_Iter _First, _Iter _Last);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void destroy(Alloc& al, _Iter _First, _Iter _Last);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void destroy_n(_Iter _First, const _Diff n);

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void destroy_n(Alloc& al, _Iter _First, _Diff n);

template<typename _Iter>
WJR_CONSTEXPR20 void uninitialized_default_construct(_Iter _First, _Iter _Last);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void uninitialized_default_construct(
Alloc& al, _Iter _First, _Iter _Last);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_default_construct_n(_Iter _First, _Diff n);

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_default_construct_n(
Alloc& al, _Iter _First, _Diff n);

template<typename _Iter>
WJR_CONSTEXPR20 void uninitialized_value_construct(_Iter _First, _Iter _Last);

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void uninitialized_value_construct(
Alloc& al, _Iter _First, _Iter _Last);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_value_construct_n(_Iter _First, _Diff n);

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_value_construct_n(
Alloc& al, _Iter _First, _Diff n);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_copy(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_copy(
Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_copy_n(_Iter1 _First, _Diff n, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_copy_n(Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest);


template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void uninitialized_fill(_Iter _First, _Iter _Last, const _Val& val);

template<typename _Iter>
WJR_CONSTEXPR20 void uninitialized_fill(_Iter _First, _Iter _Last, default_construct_tag);

template<typename _Iter>
WJR_CONSTEXPR20 void uninitialized_fill(_Iter _First, _Iter _Last, value_construct_tag);

template<typename Alloc, typename _Iter, typename _Val>
WJR_CONSTEXPR20 void uninitialized_fill(Alloc& al, _Iter _First, _Iter _Last, const _Val& val);

template<typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(_Iter _First, _Diff count, const _Val& val);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(_Iter _First, _Diff n, default_construct_tag);

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(_Iter _First, _Diff n, value_construct_tag);

template<typename Alloc, typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(Alloc& al, _Iter _First, _Diff n, const _Val& val);

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_move(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_move(Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest);

template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> uninitialized_move_n(_Iter1 _First, _Diff n, _Iter2 _Dest);

template<typename Alloc, typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> uninitialized_move_n(
Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest);

template<typename _Iter, typename _Func>
WJR_CONSTEXPR20 void for_each(_Iter _First, _Iter _Last, _Func fn);

template<typename _Iter, typename _SizeT, typename _Func>
WJR_CONSTEXPR20 void for_each_n(_Iter _First, _SizeT n, _Func fn);

template<typename Alloc>
class temporary_allocator_value;

_WJR_END

#ifndef __WJR_ALGORITHM_H
#error  "This file should not be included directly. Include <wjr/algorithm.h> instead."
#endif //__WJR_ALGORITHM_H

#include <algorithm>
#include <memory>
#include <limits>
#include <cstring>

#pragma once
#ifndef __WJR_ALGO_ALOG_H
#define __WJR_ALGO_ALOG_H

#pragma once
#ifndef __WJR_SIMD_SIMD_INTRIN_H
#define __WJR_SIMD_SIMD_INTRIN_H

#pragma once
#ifndef __WJR_SIMD_SIMD_CAST_H__
#define __WJR_SIMD_SIMD_CAST_H__


_WJR_SIMD_BEGIN

struct lo {};
struct hi {};

template<typename T, typename U>
struct simd_cast_fn;

template<typename T, typename U>
constexpr simd_cast_fn<T, U> simd_cast{};

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

#endif // __WJR_SIMD_SIMD_CAST_H__

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

// notice that mask must be 0 or 255(every byte)
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
#define WJR_SIMD_ALIGNMENT(simd_t, ptr) (WJR_SIMD_WIDTH(simd_t) / (WJR_SIMD_VALUE_SIZE(ptr)))
#define WJR_SIMD_UINTPTR(ptr) reinterpret_cast<uintptr_t>(ptr)

#define WJR_SIMD_SUFFIX_0 _FORWARD
#define WJR_SIMD_SUFFIX_1 _BACKWARD

// use this to control if is forward of backward
//#define WJR_SIMD_IS_BACKWARD 0

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

#define WJR_SIMD_LEFT_PTR(simd_t, ptr) WJR_SIMD_LEFT_PTR_N(ptr, WJR_SIMD_OFFSET(simd_t, ptr))
#define WJR_SIMD_RIGHT_PTR(simd_t, ptr) WJR_SIMD_RIGHT_PTR_N(ptr, WJR_SIMD_OFFSET(simd_t, ptr))

// do nothing
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
#define WJR_SIMD_CEIL_ALIGN_OFFSET_FORWARD(ptr, W) ((-WJR_SIMD_UINTPTR(ptr)) & ((W) - 1))
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

#define _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, FUNC, SUFFIX) \
_WJR_SIMD_USE_SUFFIX(WJR_SIMD_ADD_PTR, SUFFIX)(ptr, FUNC(x) / WJR_SIMD_VALUE_SIZE(ptr))

#define WJR_SIMD_FIRST_ZERO_PTR_FORWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_FIRST_ZERO, WJR_SIMD_FORWARD_SUFFIX)
#define WJR_SIMD_LAST_ZERO_PTR_FORWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_LAST_ZERO, WJR_SIMD_FORWARD_SUFFIX)
#define WJR_SIMD_FIRST_ONE_PTR_FORWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_FIRST_ONE, WJR_SIMD_FORWARD_SUFFIX)
#define WJR_SIMD_LAST_ONE_PTR_FORWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_LAST_ONE, WJR_SIMD_FORWARD_SUFFIX)

#define WJR_SIMD_FIRST_ZERO_PTR_BACKWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_FIRST_ZERO, WJR_SIMD_BACKWARD_SUFFIX)
#define WJR_SIMD_LAST_ZERO_PTR_BACKWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_LAST_ZERO, WJR_SIMD_BACKWARD_SUFFIX)
#define WJR_SIMD_FIRST_ONE_PTR_BACKWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_FIRST_ONE, WJR_SIMD_BACKWARD_SUFFIX)
#define WJR_SIMD_LAST_ONE_PTR_BACKWARD(ptr, x) _WJR_SIMD_FIRST_OR_ZERO_PTR(ptr, x, WJR_SIMD_LAST_ONE, WJR_SIMD_BACKWARD_SUFFIX)

#define WJR_SIMD_FIRST_ZERO_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_FIRST_ZERO(x) / WJR_SIMD_VALUE_SIZE(ptr))
#define WJR_SIMD_LAST_ZERO_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_LAST_ZERO(x) / WJR_SIMD_VALUE_SIZE(ptr))
#define WJR_SIMD_FIRST_ONE_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_FIRST_ONE(x) / WJR_SIMD_VALUE_SIZE(ptr))
#define WJR_SIMD_LAST_ONE_PTR(ptr, x) WJR_SIMD_ADD_PTR(ptr, WJR_SIMD_LAST_ONE(x) / WJR_SIMD_VALUE_SIZE(ptr))

#define _WJR_SIMD_LOAD(simd_t, PREFIX, ptr, LOAD, J)	                                                                        \
auto PREFIX = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr)));
#define _WJR_SIMD_LOAD2(simd_t, PREFIX, ptr0, ptr1, LOAD, J)	                                                                \
auto PREFIX##0 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr0)));	                        \
auto PREFIX##1 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr1)));
#define _WJR_SIMD_LOAD4(simd_t, PREFIX, ptr0, ptr1, ptr2, ptr3 , LOAD, J)	                                                    \
auto PREFIX##0 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr0)));	                        \
auto PREFIX##1 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr1)));	                        \
auto PREFIX##2 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr2)));	                        \
auto PREFIX##3 = simd_t::LOAD(reinterpret_cast<const typename simd_t::int_type*>(J(simd_t, ptr3)));

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

#pragma once
#ifndef __WJR_MATH_H__
#define __WJR_MATH_H__

#pragma once
#ifndef __WJR_ASM_ASM_H
#define __WJR_ASM_ASM_H


#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif

_WJR_ASM_BEGIN

#if WJR_HAS_BUILTIN(__builtin_addc) || WJR_HAS_CLANG(5, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_builtin_adc(T a, T b, T carry_in, T* carry_out) {
constexpr auto _Nd = std::numeric_limits<T>::digits;

constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

if constexpr (_Nd <= _Nd_ub) {
unsigned char CF = 0;
T ret = __builtin_addcb(a, b, carry_in, &CF);
*carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_us) {
unsigned short CF = 0;
T ret = __builtin_addcs(a, b, carry_in, &CF);
*carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_ui) {
unsigned int CF = 0;
T ret = __builtin_addc(a, b, carry_in, &CF);
*carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_ul) {
unsigned long CF = 0;
T ret = __builtin_addcl(a, b, carry_in, &CF);
*carry_out = CF;
return ret;
}
#if defined(WJR_X86_64)
else if constexpr (_Nd <= _Nd_ull) {
unsigned long long CF = 0;
T ret = __builtin_addcll(a, b, carry_in, &CF);
*carry_out = CF;
return ret;
}
#endif // WJR_X86_64
else {
static_assert(_Nd <= _Nd_ull, "unsupported integer type");
}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_msvc_adc(T a, T b, T carry_in, T* carry_out) {
constexpr auto _Nd = std::numeric_limits<T>::digits;

constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
//constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

if constexpr (_Nd <= _Nd_ub) {
unsigned char ret = 0;
*carry_out = _addcarry_u8(carry_in, a, b, &ret);
return ret;
}
else if constexpr (_Nd <= _Nd_us) {
unsigned short ret = 0;
*carry_out = _addcarry_u16(carry_in, a, b, &ret);
return ret;
}
else if constexpr (_Nd <= _Nd_ui) {
unsigned int ret = 0;
*carry_out = _addcarry_u32(carry_in, a, b, &ret);
return ret;
}
#if defined(WJR_X86_64)
else if constexpr (_Nd <= _Nd_ull) {
unsigned long long ret = 0;
*carry_out = _addcarry_u64(carry_in, a, b, &ret);
return ret;
}
#endif // WJR_X86_64
else {
static_assert(_Nd <= _Nd_ull, "unsupported integer type");
}
}
#endif

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T adc(T a, T b, T carry_in, T* carry_out) {
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
: "+r"(carry_in), "=rm"(*carry_out), "+%r"(a)
: "rm"(b)
: "cc");
#else
asm("add $255, %b0\n\t"
"adc %3, %2\n\t"
"setb %b1"
: "+r"(carry_in), "=r"(*carry_out), "+%r"(a)
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
*carry_out = c;
return a;
}

_WJR_ASM_END
#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif

_WJR_ASM_BEGIN

#if WJR_HAS_BUILTIN(__builtin_subc) || WJR_HAS_CLANG(5, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_builtin_sbb(T a, T b, T carry_in, T* carry_out) {
constexpr auto _Nd = std::numeric_limits<T>::digits;

constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

if constexpr (_Nd <= _Nd_ub) {
unsigned char CF = 0;
T ret = __builtin_subcb(a, b, carry_in, &CF);
*carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_us) {
unsigned short CF = 0;
T ret = __builtin_subcs(a, b, carry_in, &CF);
*carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_ui) {
unsigned int CF = 0;
T ret = __builtin_subc(a, b, carry_in, &CF);
*carry_out = CF;
return ret;
}
else if constexpr (_Nd <= _Nd_ul) {
unsigned long CF = 0;
T ret = __builtin_subcl(a, b, carry_in, &CF);
*carry_out = CF;
return ret;
}
#if defined(WJR_X86_64)
else if constexpr (_Nd <= _Nd_ull) {
unsigned long long CF = 0;
T ret = __builtin_subcll(a, b, carry_in, &CF);
*carry_out = CF;
return ret;
}
#endif // WJR_X86_64
else {
static_assert(_Nd <= _Nd_ull, "unsupported integer type");
}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_msvc_sbb(T a, T b, T carry_in, T* carry_out) {
constexpr auto _Nd = std::numeric_limits<T>::digits;

constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
//constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

if constexpr (_Nd <= _Nd_ub) {
unsigned char ret = 0;
*carry_out = _subborrow_u8(carry_in, a, b, &ret);
return ret;
}
else if constexpr (_Nd <= _Nd_us) {
unsigned short ret = 0;
*carry_out = _subborrow_u16(carry_in, a, b, &ret);
return ret;
}
else if constexpr (_Nd <= _Nd_ui) {
unsigned int ret = 0;
*carry_out = _subborrow_u32(carry_in, a, b, &ret);
return ret;
}
#if defined(WJR_X86_64)
else if constexpr (_Nd <= _Nd_ull) {
unsigned long long ret = 0;
*carry_out = _subborrow_u64(carry_in, a, b, &ret);
return ret;
}
#endif // WJR_X86_64
else {
static_assert(_Nd <= _Nd_ull, "unsupported integer type");
}
}
#endif

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T sbb(T a, T b, T carry_in, T* carry_out) {
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
: "+r"(carry_in), "=rm"(*carry_out), "+r"(a)
: "rm"(b)
: "cc");
#else
asm("add $255, %b0\n\t"
"sbb %3, %2\n\t"
"setb %b1"
: "+r"(carry_in), "=r"(*carry_out), "+r"(a)
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
*carry_out = c;
return a;
}

_WJR_ASM_END
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
WJR_INTRINSIC_CONSTEXPR static int __wjr_fallback_clz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;

if (is_likely(x != 0)) {
int n = 0;
// use _Table

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
WJR_INTRINSIC_INLINE static int __wjr_builtin_clz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;

if (is_likely(x != 0)) {

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
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_avx2_clz(T x) noexcept {
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
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_normal_clz(T x) noexcept {
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
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_clz(T x) noexcept {
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
WJR_INTRINSIC_INLINE static int __wjr_msvc_arm_clz(T x) noexcept {
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
WJR_INTRINSIC_E_CONSTEXPR int clz(T x) noexcept {
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
WJR_INTRINSIC_INLINE static int __wjr_builtin_ctz(T x) noexcept {
constexpr auto _Nd = std::numeric_limits<T>::digits;

if (is_likely(x != 0)) {

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
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_avx2_ctz(T x) noexcept {
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
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_normal_ctz(T x) noexcept {
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
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_ctz(T x) noexcept {
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
WJR_INTRINSIC_E_CONSTEXPR int ctz(T x) noexcept {
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
WJR_INTRINSIC_CONSTEXPR static int __wjr_fallback_popcount(_Ty _Val) noexcept {
constexpr int _Digits = std::numeric_limits<_Ty>::digits;
#if defined(WJR_X86_32) || defined(WJR_ARM)
if constexpr (_Digits == 64) {
// 64-bit bit operations on architectures without 64-bit registers are less efficient,
// hence we split the value so that it fits in 32-bit registers
return _Popcount_fallback(static_cast<unsigned long>(_Val))
+ _Popcount_fallback(static_cast<unsigned long>(_Val >> 32));
}
#endif // defined(WJR_X86_32) || defined(WJR_ARM)
// we static_cast these bit patterns in order to truncate them to the correct size
_Val = static_cast<_Ty>(_Val - ((_Val >> 1) & static_cast<_Ty>(0x5555'5555'5555'5555ull)));
_Val = static_cast<_Ty>((_Val & static_cast<_Ty>(0x3333'3333'3333'3333ull))
+ ((_Val >> 2) & static_cast<_Ty>(0x3333'3333'3333'3333ull)));
_Val = static_cast<_Ty>((_Val + (_Val >> 4)) & static_cast<_Ty>(0x0F0F'0F0F'0F0F'0F0Full));
// Multiply by one in each byte, so that it will have the sum of all source bytes in the highest byte
_Val = static_cast<_Ty>(_Val * static_cast<_Ty>(0x0101'0101'0101'0101ull));
// Extract highest byte
return static_cast<int>(_Val >> (_Digits - 8));
}

#if WJR_HAS_BUILTIN(__builtin_popcount) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_INTRINSIC_INLINE static int __wjr_builtin_popcount(T x) noexcept {
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
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_popcount(T x) noexcept {
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
WJR_INTRINSIC_CONSTEXPR20 int popcnt(T x) noexcept {
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

WJR_INTRINSIC_CONSTEXPR static uint8_t __wjr_fallback_bswap8(uint8_t x) {
return x;
}

WJR_INTRINSIC_CONSTEXPR static uint16_t __wjr_fallback_bswap16(uint16_t x) {
return static_cast<uint16_t>(x >> 8) | static_cast<uint16_t>(x << 8);
}

WJR_INTRINSIC_CONSTEXPR static uint32_t __wjr_fallback_bswap32(uint32_t x) {
return (x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | (x << 24);
}

WJR_INTRINSIC_CONSTEXPR static uint64_t __wjr_fallback_bswap64(uint64_t x) {
return (x >> 56) | ((x >> 40) & 0xff00) | ((x >> 24) & 0xff0000) | ((x >> 8) & 0xff000000)
| ((x << 8) & 0xff00000000) | ((x << 24) & 0xff0000000000) | ((x << 40) & 0xff000000000000) | (x << 56);
}

template<typename T>
WJR_INTRINSIC_CONSTEXPR static T __wjr_fallback_bswap(T x) {
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

// byte swap
#if WJR_HAS_BUILTIN(__builtin_bswap16) || WJR_HAS_GCC(10, 1, 0) || WJR_HAS_CLANG(15, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_builtin_bswap(T x) {
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
WJR_INTRINSIC_INLINE static T __wjr_msvc_bswap(T x) {
constexpr auto _Nd = std::numeric_limits<T>::digits;

constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
//constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
//constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
//constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
//constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

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
WJR_INTRINSIC_INLINE static T bswap(T x) {
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

// rep stos

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

WJR_INTRINSIC_INLINE void __volatile_pause() noexcept {
volatile int x = 0;
++x; ++x; ++x; ++x; ++x;
++x; ++x; ++x; ++x; ++x;
++x; ++x; ++x; ++x; ++x;
}

WJR_INTRINSIC_INLINE void pause() noexcept {
#if defined(WJR_MSVC) && defined(WJR_X86)
_mm_pause();
#elif defined(WJR_INLINE_ASM)
#if defined(WJR_X86)
asm volatile("pause");
#else
__volatile_pause();
#endif
#else
__volatile_pause();
#endif
}

_WJR_ASM_END

#endif // __WJR_ASM_ASM_H

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
WJR_INTRINSIC_CONSTEXPR20 To bit_cast(const From & src) noexcept {
static_assert(std::is_trivially_constructible_v<To>,
"This implementation additionally requires destination type to be trivially constructible");
To storage;
if constexpr (is_any_index_of_v<sizeof(From), 1, 2, 4, 8>) {
using value_type = uint_t<8 * sizeof(From)>;
*reinterpret_cast<value_type*>(&storage) = *reinterpret_cast<const value_type*>(&src);
}
else {
std::memcpy(&storage, &src, sizeof(From));
}
return storage;
}

#endif

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
constexpr bool has_single_bit(T x) noexcept {
return x != 0 && (x & (x - 1)) == 0;
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_E_CONSTEXPR int countl_zero(T x) noexcept {
return wjr::masm::clz(x);
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_E_CONSTEXPR int countl_one(T x) noexcept {
return wjr::countl_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_E_CONSTEXPR int countr_zero(T x) noexcept {
return wjr::masm::ctz(x);
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_E_CONSTEXPR int countr_one(T x) noexcept {
return wjr::countr_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_E_CONSTEXPR int bit_width(T x) noexcept {
return std::numeric_limits<T>::digits - wjr::countl_zero(x);
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_E_CONSTEXPR T bit_floor(T x) noexcept {
if (x != 0) {
return static_cast<T>(T{ 1 } << (wjr::bit_width(x) - 1));
}
return T{ 0 };
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_E_CONSTEXPR T bit_ceil(T x) noexcept {
if (x <= 1) {
return T{ 1 };
}
return static_cast<T>(T{ 1 } << wjr::bit_width(x - 1));
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_E_CONSTEXPR int popcount(T x) noexcept {
return wjr::masm::popcnt(x);
}

template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T byteswap(T x) noexcept {
using value_type = uint_t<8 * sizeof(T)>;
return bit_cast<T>(wjr::masm::bswap(bit_cast<value_type>(x)));
}

template<endian to = endian::native, typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T endian_convert(T x, endian from = endian::native) noexcept {
if (from != to) {
return byteswap(x);
}
return x;
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

// calc a ^ b
// O(log2 b)
template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
inline constexpr T power(T a, unsigned int b) {
T ret = 1;
while (b) {
if (b & 1)ret *= a;
a *= a;
b >>= 1;
}
return ret;
}

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

// calc depth of __width
template<unsigned int base, typename T, unsigned int digits>
inline constexpr unsigned int __width_depth() {
// digits = 2, that's to say value < base ^ 2
// we can just test if val >= base
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

// optimized
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
//static_assert(__mid <= __table_size, "");
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

// calc ceil(log(a) / log(base))
// for a = 0, return 0
// for a = (1, base), return 1
// force constexpr, so when base is power of 2, the performance may be worse
template<unsigned int base, typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base_width(T a) {
constexpr auto __digits = base_digits_v<T, base>;
constexpr auto __depth = __width_depth<base, T, __digits>();
constexpr auto __use_table = __width_end_with_table<base, T, __digits>();
if (is_unlikely(!a)) return 0;
unsigned int ret = 1;
__width<base, T, __digits, __depth, __use_table>(a, ret);
return ret;
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base2_width(T a) {
return base_width<2>(a);
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base10_width(T a) {
return base_width<10>(a);
}

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
inline constexpr unsigned int base_width(unsigned int b, T a) {
// optimize for common cases
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

template<typename R, typename T>
struct broadcast_fn {};

template<typename R, typename T>
inline constexpr broadcast_fn<R, T> broadcast{};

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

#endif // __WJR_MATH_H__

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

// notice that mask must be 0 or 255(every byte)
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
// preloadu_si(n * 16)
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

#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif

#if defined(_WJR_FAST_MEMCHR)

#define __WJR_MEMCHR_ONE(st, q, ptr)												\
{	                                                                            \
auto r = st::cmp(x, q, pred, T());	                                        \
st::mask_type z = st::movemask_epi8(r);	                                    \
if(z != 0){	                                                                \
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
#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif

// __WJR_MEMCHR_ONE
// __WJR_MEMCHR_FOUR
// __WJR_MEMCHR_NAME

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
WJR_NODISCARD WJR_NOINLINE const T* WJR_MACRO_CONCAT(__large, __WJR_MEMCHR_NAME)(const T* s, T val, size_t n, _Pred pred) noexcept {

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
WJR_SIMD_LOADU4(simd::sse, x, WJR_SIMD_ADD_PTR(s, negdelta - delta),
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

// m = std::min(n, 128 / _Mysize)
// m == n -> n <= 128 / _Mysize
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

// align

if (is_likely(WJR_SIMD_UINTPTR(s) % _Mysize == 0)) {

const auto __align_s = WJR_SIMD_CEIL_ALIGN_OFFSET(s, bound);

// align
if (is_constant_p(__align_s) && __align_s == 0) {
// do nothing
}
else {
WJR_SIMD_LOADU(simd_t, x, s);

__WJR_MEMCHR_ONE(simd_t, q, s);

WJR_SIMD_INC_PTR(s, __align_s / _Mysize);
n -= __align_s / _Mysize;

if (is_unlikely(n < width * 4)) {
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

// unaligned algorithm
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

// inline this function, and call __large_memchr for ans pos > 32
// Doing so does not cause the code to become overly bloated, while also balancing performance
// But what's awkward is that MSVC doesn't inline it
template<typename T, typename _Pred>
WJR_NODISCARD inline const T* (__WJR_MEMCHR_NAME)(const T* s, T val, size_t n, _Pred pred) noexcept {

constexpr size_t _Mysize = sizeof(T);

if (is_unlikely(n == 0)) return s;

WJR_SIMD_INIT_PTR(s, n);

if (n >= 16 / _Mysize) {
auto qx = simd::sse::set1(val, T());

{
// solve first 16 bytes
WJR_SIMD_LOADU(simd::sse, x, s);
__WJR_MEMCHR_ONE(simd::sse, qx, s);
}

if (n <= 32 / _Mysize) {
// solve last 16 bytes

WJR_SIMD_INC_PTR(s, n - 16 / _Mysize);

WJR_SIMD_LOADU(simd::sse, x, s);
__WJR_MEMCHR_ONE(simd::sse, qx, s);

return WJR_SIMD_ADD_PTR(s, 16 / _Mysize);
}

{
// solve first 16 bytes
WJR_SIMD_LOADU(simd::sse, x, WJR_SIMD_ADD_PTR(s, 16 / _Mysize));
__WJR_MEMCHR_ONE(simd::sse, qx, WJR_SIMD_ADD_PTR(s, 16 / _Mysize));
}

return WJR_MACRO_CONCAT(__large, __WJR_MEMCHR_NAME)(s, val, n, pred);
}

#if !WJR_SIMD_IS_BACKWARD
if constexpr (_Mysize == 8) {
// n = [1, 2)
return pred(*s, val) ? s : s + 1;
}

if constexpr (_Mysize == 2) {
// n = [1, 8)
if (n >= 4) {
// n = [4, 8)
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
// n = [1, 16)
if (n >= 4) {
// n = [4, 16)
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
// n = [1, 4)
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
// n = [1, 2)
return pred(s[-1], val) ? s : s - 1;
}

if constexpr (_Mysize == 2) {
// n = [1, 8)
if (n >= 4) {
// n = [4, 8)
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
// n = [1, 16)
if (n >= 4) {
// n = [4, 16)
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
// n = [1, 4)
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

_WJR_ALGO_END
#undef __WJR_MEMCHR_NAME
#undef WJR_SIMD_IS_BACKWARD

#define WJR_SIMD_IS_BACKWARD 1
#define __WJR_MEMCHR_NAME __memrchr
#undef __WJR_MEMCHR_NAME
#undef WJR_SIMD_IS_BACKWARD

_WJR_ALGO_BEGIN

extern template const uint8_t* __large__memchr(
const uint8_t*, uint8_t, size_t, std::equal_to<>) noexcept;

extern template const uint8_t* __large__memrchr(
const uint8_t*, uint8_t, size_t, std::equal_to<>) noexcept;

_WJR_ALGO_END

#undef __WJR_MEMCHR_ONE
#undef __WJR_MEMCHR_FOUR

#endif // _WJR_FAST_MEMCHR
#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif

#if defined(_WJR_FAST_MEMSKIPW)

_WJR_ALGO_BEGIN

static inline const bool __memskipw_table[] = {
false,false,false,false,false,false,false,false,false, true, true, true, true, true,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
true,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false
};

_WJR_ALGO_END

#define __WJR_MEMSKIPW_ONE(st, ptr)

#define WJR_SIMD_IS_BACKWARD 0

#define __WJR_MEMSKIPW_NAME __memskipw
#define __WJR_MEMSKIPW_PRED(x) (!__memskipw_table[x])
#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif // __WJR_ALGO_ALOG_H

// __WJR_MEMSKIPW_ONE
// __WJR_MEMSKIPW_FOUR
// __WJR_MEMSKIPW_NAME
// __WJR_MEMSKIPW_PRED

_WJR_ALGO_BEGIN

WJR_NODISCARD inline const uint8_t* (__WJR_MEMSKIPW_NAME)(const uint8_t* s, size_t n) noexcept {
if (is_unlikely(n == 0)) return s;

WJR_SIMD_INIT_PTR(s, n);

if (n >= 4) {

// for most cases, it is possible to quickly find non whitespace characters
// at this time, using simd can actually reduce performance

// solve first 4 bytes

{
const auto u = WJR_SIMD_PTR(s);
if (__WJR_MEMSKIPW_PRED(*u)) {
return s;
}
}

{
const auto u = WJR_SIMD_PTR(WJR_SIMD_ADD_PTR(s, 1));
if (__WJR_MEMSKIPW_PRED(*u)) {
return WJR_SIMD_ADD_PTR(s, 1);
}
}

{
const auto u = WJR_SIMD_PTR(WJR_SIMD_ADD_PTR(s, 2));
if (__WJR_MEMSKIPW_PRED(*u)) {
return WJR_SIMD_ADD_PTR(s, 2);
}
}

{
const auto u = WJR_SIMD_PTR(WJR_SIMD_ADD_PTR(s, 3));
if (__WJR_MEMSKIPW_PRED(*u)) {
return WJR_SIMD_ADD_PTR(s, 3);
}
}

WJR_SIMD_INC_PTR(s, 4);
n -= 4;
}

// n = [1, 2, 3]
{
if (__WJR_MEMSKIPW_PRED(*WJR_SIMD_PTR(s))) { return s; }
if (n == 1) { return WJR_SIMD_ADD_PTR(s, 1); }
if (__WJR_MEMSKIPW_PRED(*WJR_SIMD_PTR(WJR_SIMD_ADD_PTR(s, 1)))) {
return WJR_SIMD_ADD_PTR(s, 1);
}
if (n == 2) { return WJR_SIMD_ADD_PTR(s, 2); }
if (__WJR_MEMSKIPW_PRED(*WJR_SIMD_PTR(WJR_SIMD_ADD_PTR(s, 2)))) {
return WJR_SIMD_ADD_PTR(s, 2);
}
return WJR_SIMD_ADD_PTR(s, 3);
}

const auto e = WJR_SIMD_ADD_PTR(s, n);

for (; s != e; WJR_SIMD_INC_PTR(s, 1)) {
auto u = WJR_SIMD_PTR(s);
if (__WJR_MEMSKIPW_PRED(*u)) {
break;
}
}

return s;
}

_WJR_ALGO_END
#undef __WJR_MEMSKIPW_NAME

#undef WJR_SIMD_IS_BACKWARD

#define WJR_SIMD_IS_BACKWARD 0

#define __WJR_MEMSKIPW_NAME __memskipnw
#define __WJR_MEMSKIPW_PRED(x) (__memskipw_table[x])
#undef __WJR_MEMSKIPW_NAME

#undef WJR_SIMD_IS_BACKWARD

#undef __WJR_MEMSKIPW_ONE
#undef __WJR_MEMSKIPW_FOUR

#endif // _WJR_FAST_MEMSKIPW
#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif

#if defined(_WJR_FAST_MEMCMP)

#define __WJR_MEMCMP_ONE_NORMAL(st)														\
{	\
auto r = st::cmp(x, y, pred, T());	                                                \
st::mask_type z = st::movemask_epi8(r);		                                        \
if(z != st::mask()){	                                                            \
return false;	                                                                \
}	\
}

#define __WJR_MEMCMP_FOUR_NORMAL(st)													\
{	\
auto r0 = st::cmp(x0, y0, pred, T());	                                            \
auto r1 = st::cmp(x1, y1, pred, T());	                                            \
auto r2 = st::cmp(x2, y2, pred, T());	                                            \
auto r3 = st::cmp(x3, y3, pred, T());	                                            \
\
r3 = st::And(st::And(r0, r1), st::And(r2, r3));	                                    \
st::mask_type z = st::movemask_epi8(r3);		                                    \
if(z != st::mask()){	                                                            \
return false;	                                                                \
}	\
}

#define __WJR_MEMCMP_ONE(st) __WJR_MEMCMP_ONE_NORMAL(st)
#define __WJR_MEMCMP_FOUR(st) __WJR_MEMCMP_FOUR_NORMAL(st)

#if WJR_SSE4_1
#undef __WJR_MEMCMP_ONE
#undef __WJR_MEMCMP_FOUR

#define __WJR_MEMCMP_ONE(st)															\
if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                            \
auto r = st::Xor(x, y);	                                                        \
auto z = st::test_all_zeros(r);	                                                \
if(!z){	                                                                        \
return false;	                                                            \
}	                                                                            \
}	                                                                                \
else{	                                                                            \
__WJR_MEMCMP_ONE_NORMAL(st)														\
}

#define __WJR_MEMCMP_FOUR(st)															\
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
__WJR_MEMCMP_FOUR_NORMAL(st)													\
}
#endif // WJR_SSE4_1

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
bool __memcmp(const T* s0, const T* s1, size_t n, _Pred pred) {
constexpr size_t _Mysize = sizeof(T);

#if WJR_AVX2
using simd_t = simd::avx;
#else
using simd_t = simd::sse;
#endif // WJR_AVX2
using sint = typename simd_t::int_type;
constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
constexpr uintptr_t bound = width * _Mysize;

if (is_constant_p(n) && n <= 64 / _Mysize) {
return ::memcmp(s0, s1, n * _Mysize) == 0;
}

if (is_unlikely(n == 0)) return true;

if (n >= 16 / _Mysize) {

if (n <= 32 / _Mysize) {
// solve first 16 bytes
auto x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
auto y = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));

__WJR_MEMCMP_ONE(simd::sse);

// solve last 16 bytes

s0 += n - 16 / _Mysize;
s1 += n - 16 / _Mysize;

x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
y = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));

__WJR_MEMCMP_ONE(simd::sse);

return true;
}

// solve first min(n, 128 / _Mysize) bytes
// no branch algorithm

{
const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
const auto negdelta = m - 32 / _Mysize;

#if WJR_AVX2
auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0));
auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + delta));
auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + negdelta - delta));
auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + negdelta));

auto y0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1));
auto y1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + delta));
auto y2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + negdelta - delta));
auto y3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + negdelta));

__WJR_MEMCMP_FOUR(simd::avx);
#else
{
auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + 16 / _Mysize));
auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + delta));
auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + delta + 16 / _Mysize));

auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));
auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + 16 / _Mysize));
auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + delta));
auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + delta + 16 / _Mysize));

__WJR_MEMCMP_FOUR(simd::sse);
}

{
auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta - delta));
auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta - delta + 16 / _Mysize));
auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta));
auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta + 16 / _Mysize));

auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta - delta));
auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta - delta + 16 / _Mysize));
auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta));
auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta + 16 / _Mysize));

__WJR_MEMCMP_FOUR(simd::sse);
}
#endif // WJR_AVX2

// m = std::min(n, 128 / _Mysize)
// m == n -> n <= 128 / _Mysize
if (m == n) {
return true;
}
}

if (n <= 128 / _Mysize + width * 4) {

WJR_MACRO_LABEL(unaligned_last_4vec) :

s0 += n;
s1 += n;

auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 4));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 3));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 4));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 3));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMCMP_FOUR(simd_t);

return true;
}

s0 += 128 / _Mysize;
s1 += 128 / _Mysize;
n -= 128 / _Mysize;

// align

if (is_likely(reinterpret_cast<uintptr_t>(s0) % _Mysize == 0)) {

const auto off0 = reinterpret_cast<uintptr_t>(s0) % bound;
const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;

if (off0 != off1) {
// only align first pointer

if (is_constant_p(off0) && off0 == 0) {
// do nothing
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

__WJR_MEMCMP_ONE(simd_t);

const auto __align_s = bound - off0;
s0 += __align_s / _Mysize;
s1 += __align_s / _Mysize;
n -= __align_s / _Mysize;

if (is_unlikely(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0);
}
}

do {
auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 + width));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 2));
auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 3));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 3));

__WJR_MEMCMP_FOUR(simd_t);

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

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 + width));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 + width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMCMP_FOUR(simd_t);
}

return true;
}

// align two pointers
if (is_constant_p(off0) && off0 == 0) {
// do nothing
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

__WJR_MEMCMP_ONE(simd_t);

const auto __align_s = bound - off0;
s0 += __align_s / _Mysize;
s1 += __align_s / _Mysize;
n -= __align_s / _Mysize;

if (is_unlikely(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0_1);
}
}

do {
auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 + width));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 2));
auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 3));

auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 + width));
auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 2));
auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 3));

__WJR_MEMCMP_FOUR(simd_t);

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

__WJR_MEMCMP_FOUR(simd_t);
}

return true;
}

if (is_likely(reinterpret_cast<uintptr_t>(s1) % _Mysize == 0)) {

const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;
// only align second pointer
if (is_constant_p(off1) && off1 == 0) {
// do nothing
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

__WJR_MEMCMP_ONE(simd_t);

const auto __align_s = bound - off1;
s0 += __align_s / _Mysize;
s1 += __align_s / _Mysize;
n -= __align_s / _Mysize;

if (is_unlikely(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_1);
}
}

do {
auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 3));

auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 + width));
auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 2));
auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 3));

__WJR_MEMCMP_FOUR(simd_t);

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

__WJR_MEMCMP_FOUR(simd_t);
}

return true;
}

// unaligned algorithm
do {
auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 3));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 3));

__WJR_MEMCMP_FOUR(simd_t);

s0 += width * 4;
s1 += width * 4;
n -= width * 4;
} while (n >= width * 4);

if (n == 0) {
return true;
}

goto WJR_MACRO_LABEL(unaligned_last_4vec);
}

if constexpr (_Mysize == 8) {
// n = [1, 2)
return pred(s0[0], s1[0]);
}

if constexpr (_Mysize == 2) {
// n = [1, 8)
if (n >= 4) {
// n = [4, 8)
auto A0 = *reinterpret_cast<const uint64_t*>(s0);
auto B0 = *reinterpret_cast<const uint64_t*>(s0 + n - 4);

auto A1 = *reinterpret_cast<const uint64_t*>(s1);
auto B1 = *reinterpret_cast<const uint64_t*>(s1 + n - 4);

auto x = simd::sse::set_epi64x(B0, A0);
auto y = simd::sse::set_epi64x(B1, A1);

__WJR_MEMCMP_ONE(simd::sse);

return true;
}
}

if constexpr (_Mysize == 1) {
// n = [1, 16)
if (n >= 4) {
// n = [4, 16)
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

__WJR_MEMCMP_ONE(simd::sse);

return true;
}
}

if constexpr (_Mysize <= 4) {
// n = [1, 4)
if (!pred(s0[0], s1[0])) return false;
if (n == 1) return true;
const bool f = pred(s0[n - 2], s1[n - 2]);
const bool g = pred(s0[n - 1], s1[n - 1]);
return f && g;
}

}

_WJR_ALGO_END

#undef __WJR_MEMCMP_ONE
#undef __WJR_MEMCMP_FOUR
#undef __WJR_MEMCMP_ONE_NORMAL
#undef __WJR_MEMCMP_FOUR_NORMAL

#endif // _WJR_FAST_MEMCMP
#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif

#if defined(_WJR_FAST_MEMMIS)

#define __WJR_MEMMIS_ONE(st, _s)																	\
{	                                                                                            \
auto r = st::cmp(x, y, pred, T());	                                                        \
st::mask_type z = st::movemask_epi8(r);		                                                \
if(is_unlikely(z != st::mask())) {	                                                        \
return (_s) + wjr::countr_one(z) / _Mysize;		                                        \
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
if(is_unlikely(z != st::mask())) {	                                                        \
st::mask_type tmp = st::movemask_epi8(r0);	                                            \
if(tmp != st::mask()){	                                                                \
return (_s0) + wjr::countr_one(tmp) / _Mysize;	                                    \
}	                                                                                    \
tmp = st::movemask_epi8(r1);	                                                        \
if(tmp != st::mask()){	                                                                \
return (_s1) + wjr::countr_one(tmp) / _Mysize;	                                    \
}	                                                                                    \
tmp = st::movemask_epi8(r2);	                                                        \
if(tmp != st::mask()){	                                                                \
return (_s2) + wjr::countr_one(tmp) / _Mysize;	                                    \
}	                                                                                    \
tmp = z;	                                                                            \
return (_s3) + wjr::countr_one(tmp) / _Mysize;	                                        \
}	                                                                                        \
}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memmis(const T* s0, const T* s1, size_t n, _Pred pred) {
constexpr size_t _Mysize = sizeof(T);

#if WJR_AVX2
using simd_t = simd::avx;
#else
using simd_t = simd::sse;
#endif // WJR_AVX2
using sint = typename simd_t::int_type;

constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
constexpr uintptr_t bound = width * _Mysize;

if (is_unlikely(n == 0)) return s0;

if (n >= 16 / _Mysize) {

if (n <= 32 / _Mysize) {
// solve first 16 bytes
auto x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
auto y = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));

__WJR_MEMMIS_ONE(simd::sse, s0);

// solve last 16 bytes

s0 += n - 16 / _Mysize;
s1 += n - 16 / _Mysize;

x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
y = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));

__WJR_MEMMIS_ONE(simd::sse, s0);

return s0 + 16 / _Mysize;
}

// solve first min(n, 128 / _Mysize) bytes
// no branch algorithm

{
const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;
const auto negdelta = m - 32 / _Mysize;

#if WJR_AVX2
{
auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0));
auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + delta));
auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + negdelta - delta));
auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 + negdelta));

auto y0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1));
auto y1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + delta));
auto y2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + negdelta - delta));
auto y3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 + negdelta));

__WJR_MEMMIS_FOUR(simd::avx, s0, s0 + delta, s0 + negdelta - delta, s0 + negdelta);
}
#else
{
auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + 16 / _Mysize));
auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + delta));
auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + delta + 16 / _Mysize));

auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));
auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + 16 / _Mysize));
auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + delta));
auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + delta + 16 / _Mysize));

__WJR_MEMMIS_FOUR(simd::sse, s0, s0 + 16 / _Mysize, s0 + delta, s0 + delta + 16 / _Mysize);
}

{
auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta - delta));
auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta - delta + 16 / _Mysize));
auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta));
auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 + negdelta + 16 / _Mysize));

auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta - delta));
auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta - delta + 16 / _Mysize));
auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta));
auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 + negdelta + 16 / _Mysize));

__WJR_MEMMIS_FOUR(simd::sse,
s0 + negdelta - delta,
s0 + negdelta - delta + 16 / _Mysize,
s0 + negdelta,
s0 + negdelta + 16 / _Mysize);
}
#endif // WJR_AVX2

// m = std::min(n, 128 / _Mysize)
// m == n -> n <= 128 / _Mysize
if (m == n) {
return s0 + n;
}
}

if (n <= 128 / _Mysize + width * 4) {

WJR_MACRO_LABEL(unaligned_last_4vec) :

s0 += n;
s1 += n;

auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 4));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 3));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 4));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 3));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMMIS_FOUR(simd_t, s0 - width * 4, s0 - width * 3, s0 - width * 2, s0 - width);

return s0;
}

s0 += 128 / _Mysize;
s1 += 128 / _Mysize;
n -= 128 / _Mysize;

// align

if (is_likely(reinterpret_cast<uintptr_t>(s0) % _Mysize == 0)) {

const auto off0 = reinterpret_cast<uintptr_t>(s0) % bound;
const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;

if (off0 != off1) {
// only align first pointer

if (is_constant_p(off0) && off0 == 0) {
// do nothing
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

__WJR_MEMMIS_ONE(simd_t, s0);

const auto __align_s = bound - off0;
s0 += __align_s / _Mysize;
s1 += __align_s / _Mysize;
n -= __align_s / _Mysize;

if (is_unlikely(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0);
}
}

do {
auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 + width));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 2));
auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 3));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 3));

__WJR_MEMMIS_FOUR(simd_t, s0, s0 + width, s0 + width * 2, s0 + width * 3);

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

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 + width));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 + width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMMIS_FOUR(simd_t, ptr0, ptr0 + width, ptr0 + width * 2, s0 - width);
}

return s0;
}

// align two pointers
if (is_constant_p(off0) && off0 == 0) {
// do nothing
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

__WJR_MEMMIS_ONE(simd_t, s0);

const auto __align_s = bound - off0;
s0 += __align_s / _Mysize;
s1 += __align_s / _Mysize;
n -= __align_s / _Mysize;

if (is_unlikely(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0_1);
}
}

do {
auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 + width));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 2));
auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 + width * 3));

auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 + width));
auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 2));
auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 3));

__WJR_MEMMIS_FOUR(simd_t, s0, s0 + width, s0 + width * 2, s0 + width * 3);

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

__WJR_MEMMIS_FOUR(simd_t, ptr0, ptr0 + width, ptr0 + width * 2, s0 - width);
}

return s0;
}

if (is_likely(reinterpret_cast<uintptr_t>(s1) % _Mysize == 0)) {

const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;
// only align second pointer
if (is_constant_p(off1) && off1 == 0) {
// do nothing
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1));

__WJR_MEMMIS_ONE(simd_t, s0);

const auto __align_s = bound - off1;
s0 += __align_s / _Mysize;
s1 += __align_s / _Mysize;
n -= __align_s / _Mysize;

if (is_unlikely(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_1);
}
}

do {
auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 3));

auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 + width));
auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 2));
auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 + width * 3));

__WJR_MEMMIS_FOUR(simd_t, s0, s0 + width, s0 + width * 2, s0 + width * 3);

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

__WJR_MEMMIS_FOUR(simd_t, ptr0, ptr0 + width, ptr0 + width * 2, s0 - width);
}

return s0;
}

// unaligned algorithm
do {
auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 3));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 3));

__WJR_MEMMIS_FOUR(simd_t, s0, s0 + width, s0 + width * 2, s0 + width * 3);

s0 += width * 4;
s1 += width * 4;
n -= width * 4;
} while (n >= width * 4);

if (n == 0) {
return s0;
}

goto WJR_MACRO_LABEL(unaligned_last_4vec);
}

if constexpr (_Mysize == 8) {
// n = [1, 2)
return pred(s0[0], s1[0]) ? s0 + 1 : s0;
}

if constexpr (_Mysize == 2) {
// n = [1, 8)
if (n >= 4) {
// n = [4, 8)
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
// n = [1, 16)
if (n >= 4) {
// n = [4, 16)
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
// n = [1, 4)
if (!pred(s0[0], s1[0])) return s0;
if (n == 1) return s0 + 1;
const bool f = pred(s0[n - 2], s1[n - 2]);
const bool g = pred(s0[n - 1], s1[n - 1]);
const size_t i1 = g ? n : n - 1;
const size_t i2 = f ? i1 : n - 2;
return s0 + i2;
}

}

_WJR_ALGO_END

#undef __WJR_MEMMIS_FOUR
#undef __WJR_MEMMIS_ONE

#endif // _WJR_FAST_MEMMIS
#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif

#if defined(_WJR_FAST_MEMMIS)

#define __WJR_MEMRMIS_ONE(st, _s)														    \
{	                                                                                    \
auto r = st::cmp(x, y, pred, T());	                                                \
st::mask_type z = st::movemask_epi8(r);		                                        \
if(z != st::mask()) {	                                                            \
return (_s) - wjr::countl_one(z) / _Mysize;		                                \
}	                                                                                \
}

#define __WJR_MEMRMIS_FOUR(st, _s0, _s1, _s2, _s3)										    \
{	                                                                                    \
auto r0 = st::cmp(x0, y0, pred, T());	                                            \
auto r1 = st::cmp(x1, y1, pred, T());	                                            \
auto r2 = st::cmp(x2, y2, pred, T());	                                            \
auto r3 = st::cmp(x3, y3, pred, T());	                                            \
\
r0 = st::And(st::And(r0, r1), st::And(r2, r3));	                                    \
st::mask_type z = st::movemask_epi8(r0);		                                    \
if(z != st::mask()) {	                                                            \
st::mask_type tmp = st::movemask_epi8(r3);	                                    \
if(tmp != st::mask()){	                                                        \
return (_s3) - wjr::countl_one(tmp) / _Mysize;	                            \
}	                                                                            \
tmp = st::movemask_epi8(r2);	                                                \
if(tmp != st::mask()){	                                                        \
return (_s2) - wjr::countl_one(tmp) / _Mysize;	                            \
}	                                                                            \
tmp = st::movemask_epi8(r1);	                                                \
if(tmp != st::mask()){	                                                        \
return (_s1) - wjr::countl_one(tmp) / _Mysize;	                            \
}	                                                                            \
tmp = z;	                                                                    \
return (_s0) - wjr::countl_one(tmp) / _Mysize;	                                \
}	                                                                                \
}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memrmis(const T* s0, const T* s1, size_t n, _Pred pred) {

constexpr size_t _Mysize = sizeof(T);

#if WJR_AVX2
using simd_t = simd::avx;
#else
using simd_t = simd::sse;
#endif // WJR_AVX2
using sint = typename simd_t::int_type;
constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
constexpr uintptr_t bound = width * _Mysize;

if (is_unlikely(n == 0)) return s0;

s0 += n;
s1 += n;

if (n >= 16 / _Mysize) {

if (n <= 32 / _Mysize) {
// solve first 16 bytes
auto x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - (16 / _Mysize)));
auto y = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - (16 / _Mysize)));

__WJR_MEMRMIS_ONE(simd::sse, s0);

// solve last 16 bytes

s0 -= n;
s1 -= n;

x = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0));
y = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1));

__WJR_MEMRMIS_ONE(simd::sse, s0 + (16 / _Mysize));

return s0;
}

// solve first min(n, 128 / _Mysize) bytes
// no branch algorithm

{
const auto m = n <= 128 / _Mysize ? n : 128 / _Mysize;
const auto delta = ((m - 1) & (64 / _Mysize)) >> 1;

#if WJR_AVX2
auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - m));
auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - m + delta));
auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - (32 / _Mysize) - delta));
auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s0 - (32 / _Mysize)));

auto y0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - m));
auto y1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - m + delta));
auto y2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - (32 / _Mysize) - delta));
auto y3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s1 - (32 / _Mysize)));

__WJR_MEMRMIS_FOUR(simd::avx,
s0 - m + 32 / _Mysize,
s0 - m + delta + 32 / _Mysize,
s0 - delta,
s0);
#else
{
auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - (32 / _Mysize) - delta));
auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - (16 / _Mysize) - delta));
auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - (32 / _Mysize)));
auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - (16 / _Mysize)));

auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - (32 / _Mysize) - delta));
auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - (16 / _Mysize) - delta));
auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - (32 / _Mysize)));
auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - (16 / _Mysize)));

__WJR_MEMRMIS_FOUR(simd::sse,
s0 - (16 / _Mysize) - delta,
s0 - delta,
s0 - (16 / _Mysize),
s0);
}

{
auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - m));
auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - m + 16 / _Mysize));
auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - m + delta));
auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s0 - m + delta + 16 / _Mysize));

auto y0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - m));
auto y1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - m + 16 / _Mysize));
auto y2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - m + delta));
auto y3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s1 - m + delta + 16 / _Mysize));

__WJR_MEMRMIS_FOUR(simd::sse,
s0 - m + 16 / _Mysize,
s0 - m + 32 / _Mysize,
s0 - m + delta + 16 / _Mysize,
s0 - m + delta + 32 / _Mysize);
}
#endif // WJR_AVX2

// m = std::min(n, 128 / _Mysize)
// m == n -> n <= 128 / _Mysize
if (m == n) {
return s0 - n;
}
}

if (n <= 128 / _Mysize + width * 4) {

WJR_MACRO_LABEL(unaligned_last_4vec) :

s0 -= n;
s1 -= n;

auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 + width * 3));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 + width * 3));

__WJR_MEMRMIS_FOUR(simd_t, s0 + width, s0 + width * 2, s0 + width * 3, s0 + width * 4);

return s0;
}

s0 -= 128 / _Mysize;
s1 -= 128 / _Mysize;
n -= 128 / _Mysize;

// align

if (is_likely(reinterpret_cast<uintptr_t>(s0) % _Mysize == 0)) {

const auto off0 = reinterpret_cast<uintptr_t>(s0) % bound;
const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;

if (off0 != off1) {
// only align first pointer

if (is_constant_p(off0) && off0 == 0) {
// do nothing
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMRMIS_ONE(simd_t, s0);

const auto __align_s = off0;
s0 -= __align_s / _Mysize;
s1 -= __align_s / _Mysize;
n -= __align_s / _Mysize;

if (is_unlikely(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0);
}
}

do {
auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 4));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 3));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 2));
auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 - width));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 4));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 3));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMRMIS_FOUR(simd_t, s0 - width * 3, s0 - width * 2, s0 - width, s0);

s0 -= width * 4;
s1 -= width * 4;
n -= width * 4;
} while (n >= width * 4);

if (n != 0) {

WJR_MACRO_LABEL(last_solve_align_0) :

s0 -= n;
s1 -= n;

const auto ptr0 = reinterpret_cast<T*>(
(reinterpret_cast<uintptr_t>(s0 + width * 4)) & (~(bound - 1)));
const auto dif = ptr0 - s0;
const auto ptr1 = s1 + dif;

auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width * 3));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width * 2));
auto x3 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 - width * 3));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 - width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(ptr1 - width));

__WJR_MEMRMIS_FOUR(simd_t, s0 + width, ptr0 - width * 2, ptr0 - width, ptr0);

}

return s0;
}

// align two pointers
if (is_constant_p(off0) && off0 == 0) {
// do nothing
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMRMIS_ONE(simd_t, s0);

const auto __align_s = off0;
s0 -= __align_s / _Mysize;
s1 -= __align_s / _Mysize;
n -= __align_s / _Mysize;

if (is_unlikely(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_0_1);
}
}

do {
auto x0 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 4));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 3));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(s0 - width * 2));
auto x3 = simd_t::load(reinterpret_cast<const sint*>(s0 - width));

auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 4));
auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 3));
auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 2));
auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMRMIS_FOUR(simd_t, s0 - width * 3, s0 - width * 2, s0 - width, s0);

s0 -= width * 4;
s1 -= width * 4;
n -= width * 4;
} while (n >= width * 4);

if (n != 0) {

WJR_MACRO_LABEL(last_solve_align_0_1) :

s0 -= n;
s1 -= n;

const auto ptr0 = reinterpret_cast<T*>(
(reinterpret_cast<uintptr_t>(s0 + width * 4)) & (~(bound - 1)));
const auto dif = ptr0 - s0;
const auto ptr1 = s1 + dif;

auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width * 3));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width * 2));
auto x3 = simd_t::load(reinterpret_cast<const sint*>(ptr0 - width));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width * 3));
auto y2 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width * 2));
auto y3 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width));

__WJR_MEMRMIS_FOUR(simd_t, s0 + width, ptr0 - width * 2, ptr0 - width, ptr0);

}

return s0;
}

if (is_likely(reinterpret_cast<uintptr_t>(s1) % _Mysize == 0)) {
const auto off1 = reinterpret_cast<uintptr_t>(s1) % bound;
// only align second pointer
if (is_constant_p(off1) && off1 == 0) {
// do nothing
}
else {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));
auto y = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMRMIS_ONE(simd_t, s0);

const auto __align_s = off1;
s0 -= __align_s / _Mysize;
s1 -= __align_s / _Mysize;
n -= __align_s / _Mysize;

if (is_unlikely(n < width * 4)) {
goto WJR_MACRO_LABEL(last_solve_align_1);
}
}

do {
auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 4));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 3));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

auto y0 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 4));
auto y1 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 3));
auto y2 = simd_t::load(reinterpret_cast<const sint*>(s1 - width * 2));
auto y3 = simd_t::load(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMRMIS_FOUR(simd_t, s0 - width * 3, s0 - width * 2, s0 - width, s0);

s0 -= width * 4;
s1 -= width * 4;
n -= width * 4;
} while (n >= width * 4);

if (n != 0) {

WJR_MACRO_LABEL(last_solve_align_1) :

s0 -= n;
s1 -= n;

const auto ptr1 = reinterpret_cast<T*>(
(reinterpret_cast<uintptr_t>(s1 + width * 4)) & (~(bound - 1)));
const auto dif = ptr1 - s1;
const auto ptr0 = s0 + dif;

auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0 - width * 3));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0 - width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(ptr0 - width));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1));
auto y1 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width * 3));
auto y2 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width * 2));
auto y3 = simd_t::load(reinterpret_cast<const sint*>(ptr1 - width));

__WJR_MEMRMIS_FOUR(simd_t, s0 + width, ptr0 - width * 2, ptr0 - width, ptr0);

}

return s0;
}

// unaligned algorithm
do {
auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 4));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 3));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s0 - width));

auto y0 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 4));
auto y1 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 3));
auto y2 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width * 2));
auto y3 = simd_t::loadu(reinterpret_cast<const sint*>(s1 - width));

__WJR_MEMRMIS_FOUR(simd_t, s0 - width * 3, s0 - width * 2, s0 - width, s0);

s0 -= width * 4;
s1 -= width * 4;
n -= width * 4;
} while (n >= width * 4);

if (n == 0) {
return s0;
}

goto WJR_MACRO_LABEL(unaligned_last_4vec);
}

if constexpr (_Mysize == 8) {
// n = [1, 2)
return pred(s0[-1], s1[-1]) ? s0 : s0 - 1;
}

if constexpr (_Mysize == 2) {
// n = [1, 8)
if (n >= 4) {
// n = [4, 8)
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
// n = [1, 16)
if (n >= 4) {
// n = [4, 16)
auto delta = (n & 8) >> 1;

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
// n = [1, 4)
if (!pred(s0[-1], s1[-1])) return s0;
if (n == 1) return s0 - 1;
const bool f = pred(s0[static_cast<size_t>(1 - n)], s1[static_cast<size_t>(1 - n)]);
const bool g = pred(s0[static_cast<size_t>(0 - n)], s1[static_cast<size_t>(0 - n)]);
const size_t i1 = g ? -n : 1 - n;
const size_t i2 = f ? i1 : 2 - n;
return s0 + i2;
}
}

_WJR_ALGO_END

#undef __WJR_MEMRMIS_FOUR
#undef __WJR_MEMRMIS_ONE

#endif // _WJR_FAST_MEMMIS
#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif

#if defined(_WJR_FAST_MEMCNT)

_WJR_ALGO_BEGIN

template<typename T>
size_t __memcnt(const T* s, T val, size_t n) {

constexpr size_t _Mysize = sizeof(T);

#if WJR_AVX2
using simd_t = simd::avx;
#else
using simd_t = simd::sse;
#endif // WJR_AVX2
using sint = typename simd_t::int_type;
constexpr uintptr_t width = simd_t::width() / (8 * _Mysize);
constexpr uintptr_t bound = width * _Mysize;

constexpr auto max_step_4 = std::numeric_limits<T>::max() / width + 1;
static_assert(max_step_4 >= 4 && max_step_4 % 4 == 0, "invalid _Maxstep");
constexpr auto max_step = max_step / 4;

if (is_unlikely(n == 0)) return 0;

if (n >= 16 / _Mysize) {
if (n >= width * 4) {
const T* _lst;

auto y = simd_t::set1(val, T());

if (is_constant_p(reinterpret_cast<uintptr_t>(s) % bound) &&
reinterpret_cast<uintptr_t>(s) % bound == 0) {
// do nothing
}
else if (_Mysize == 1 ||
reinterpret_cast<uintptr_t>(s) % _Mysize == 0) {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

__WJR_MEMCHR_ONE(simd_t, s);

auto __align_s = bound - reinterpret_cast<uintptr_t>(s) % bound;
s += __align_s / _Mysize;
n -= __align_s / _Mysize;
if (is_unlikely(n < width * 4)) {
_lst = s + n;
goto WJR_MACRO_LABEL(aft_align);
}
}
else {
// unalign algorithm
do {
auto x0 = simd_t::loadu(reinterpret_cast<const sint*>(s));
auto x1 = simd_t::loadu(reinterpret_cast<const sint*>(s + width));
auto x2 = simd_t::loadu(reinterpret_cast<const sint*>(s + width * 2));
auto x3 = simd_t::loadu(reinterpret_cast<const sint*>(s + width * 3));

__WJR_MEMCHR_FOUR(simd_t, s, s + width, s + width * 2, s + width * 3);

s += width * 4;
n -= width * 4;
} while (n >= width * 4);

_lst = s + n;
if (n != 0) {
switch ((n + width - 1) / width) {
default: unreachable(); break;
case 4: {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

__WJR_MEMCHR_ONE(simd_t, s);
s += width;
}
case 3: {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

__WJR_MEMCHR_ONE(simd_t, s);
s += width;
}
case 2: {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(s));

__WJR_MEMCHR_ONE(simd_t, s);
}
case 1: {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst - width));

__WJR_MEMCHR_ONE(simd_t, _lst - width);
}
}
}
return _lst;
}

do {
auto x0 = simd_t::load(reinterpret_cast<const sint*>(s));
auto x1 = simd_t::load(reinterpret_cast<const sint*>(s + width));
auto x2 = simd_t::load(reinterpret_cast<const sint*>(s + width * 2));
auto x3 = simd_t::load(reinterpret_cast<const sint*>(s + width * 3));

__WJR_MEMCHR_FOUR(simd_t, s, s + width, s + width * 2, s + width * 3);

s += width * 4;
n -= width * 4;
} while (n >= width * 4);

_lst = s + n;
if (n != 0) {
switch ((n + width - 1) / width) {
default: unreachable(); break;
case 4: {
WJR_MACRO_LABEL(aft_align) :
auto x = simd_t::load(reinterpret_cast<const sint*>(s));

__WJR_MEMCHR_ONE(simd_t, s);
s += width;
}
case 3: {
auto x = simd_t::load(reinterpret_cast<const sint*>(s));

__WJR_MEMCHR_ONE(simd_t, s);
s += width;
}
case 2: {
auto x = simd_t::load(reinterpret_cast<const sint*>(s));

__WJR_MEMCHR_ONE(simd_t, s);
}
case 1: {
auto x = simd_t::loadu(reinterpret_cast<const sint*>(_lst - width));

__WJR_MEMCHR_ONE(simd_t, _lst - width);
}
}
}
return _lst;
}

#if WJR_AVX2
static_assert(width * 4 == 128 / _Mysize, "width * 4 == 128 / _Mysize");
if (n >= 64 / _Mysize) {
auto y = simd::avx::set1(val, T());

auto x0 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s));
auto x1 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + 32 / _Mysize));
auto x2 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + n - 64 / _Mysize));
auto x3 = simd::avx::loadu(reinterpret_cast<const __m256i*>(s + n - 32 / _Mysize));

__WJR_MEMCHR_FOUR(simd::avx, s, s + 32 / _Mysize, s + n - 64 / _Mysize, s + n - 32 / _Mysize);

return s + n;
}
#endif // WJR_AVX2

auto y = simd::sse::set1(val, T());
auto delta = (n & (32 / _Mysize)) >> 1;

auto x0 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s));
auto x1 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + delta));
auto x2 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + n - 16 / _Mysize - delta));
auto x3 = simd::sse::loadu(reinterpret_cast<const __m128i*>(s + n - 16 / _Mysize));

__WJR_MEMCHR_FOUR(simd::sse, s, s + delta, s + n - 16 / _Mysize - delta, s + n - 16 / _Mysize);

return s + n;
}

if constexpr (_Mysize == 8) {
// n = [1, 2)
return pred(*s, val) ? s : s + 1;
}

if constexpr (_Mysize == 2) {
// n = [1, 8)
if (n >= 4) {
// n = [4, 8)
auto A = *reinterpret_cast<const uint64_t*>(s);
auto B = *reinterpret_cast<const uint64_t*>(s + n - 4);

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
// n = [1, 16)
if (n >= 4) {
// n = [4, 16)
auto delta = (n & 8) >> 1;

auto A = *reinterpret_cast<const uint32_t*>(s);
auto B = *reinterpret_cast<const uint32_t*>(s + delta);
auto C = *reinterpret_cast<const uint32_t*>(s + n - 4 - delta);
auto D = *reinterpret_cast<const uint32_t*>(s + n - 4);

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
// n = [1, 4)
if (pred(s[0], val)) return s;
if (n == 1) return s + 1;
if (pred(s[1], val)) return s + 1;
if (n == 2) return s + 2;
return pred(s[2], val) ? s + 2 : s + 3;
}
}

_WJR_ALGO_END

#endif // _WJR_FAST_MEMCNT
#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif // __WJR_ALGO_ALOG_H

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
// threshold for enhanced rep
// enhanced rep will be faster when 64-byte memory is aligned
// but it will be slower when 64-byte memory is not aligned
// so we use a threshold to determine whether to use enhanced rep
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

// [128, ...)
auto q = broadcast<simd::__simd_wrapper_t<sint>, simd::__m128i_tag>(qx);

if (is_likely(reinterpret_cast<uintptr_t>(s) % _Mysize == 0)) {

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

// align first 64 byte

if (is_constant_p(reinterpret_cast<uintptr_t>(s) % 64) &&
reinterpret_cast<uintptr_t>(s) % 64 == 0) {
// do nothing
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
// do nothing
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

// align first 64 byte

if (is_constant_p(reinterpret_cast<uintptr_t>(s) % 64) &&
reinterpret_cast<uintptr_t>(s) % 64 == 0) {
// do nothing
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

// store last (width * 4) byte

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

// non-aligned algorithm
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

#pragma once
#ifndef __WJR_OPTIMIZER_H
#define __WJR_OPTIMIZER_H

// 1. fill dst with val (memset)
// The minimum cycle section length must be 1, 2, 4, 8 when _WJR_FAST_MEMSET defined
// and must be 1 when NWJR_FAST_MEMSET defined
// Get the bytes to be filled through the get function
// 2. copy src to dst (memcpy)

// TODO
// 1. fill a constant value when default-construct (memset)
// 2. fill a constant value when value-cosntruct (memset)
// 3. relocate , copy src to dst, and then fill dst with a constant value (memcpy and memset)
// 4. reloacte , copy src to dst, and then run a function on dst (memcpy and run func(User defined))


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
// all zeros
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
WJR_NODISCARD WJR_INTRINSIC_INLINE const T* memchr(const T* s, U val, size_t n, _Pred pred) noexcept {
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
template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memchr_v<T, U, _Pred>, int> = 0>
WJR_NODISCARD const T* memrchr(const T* s, U val, size_t n, _Pred pred) {
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
WJR_NODISCARD const T* memchr(const T* s, U val, size_t n, _Pred pred) {
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

#endif // _WJR_FAST_MEMCHR

#if defined(_WJR_FAST_MEMCMP)
template<typename T, typename U, typename _Pred>
struct __has_fast_memcmp : std::conjunction<
is_memory_comparable<T, U, _Pred>,
is_integrals<T, U>,
is_any_index_of<sizeof(T), 1, 2, 4>
> {};

template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::equal_to<>> : std::conjunction<
is_memory_comparable<T, U, std::equal_to<>>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memcmp_v = __has_fast_memcmp<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memcmp_v<T, U, _Pred>, int> = 0>
WJR_NODISCARD bool memcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
if constexpr (is_any_of_v<_Pred, std::equal_to<>>) {
using value_type = uint8_t;
auto __s0 = reinterpret_cast<const value_type*>(s0);
auto __s1 = reinterpret_cast<const value_type*>(s1);
return __memcmp(__s0, __s1, n * sizeof(T), pred);
}
else {
using value_type = make_integral_t<T>;
auto __s0 = reinterpret_cast<const value_type*>(s0);
auto __s1 = reinterpret_cast<const value_type*>(s1);
return __memcmp(__s0, __s1, n, pred);
}
}

#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memcmp : std::false_type {};

template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::equal_to<>> : std::conjunction<
is_memory_comparable<T, U, std::equal_to<>>
> {};

template<typename T, typename U, typename _Pred>
inline constexpr bool __has_fast_memcmp_v = __has_fast_memcmp<T, U, _Pred>::type::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memcmp_v<T, U, _Pred>, int> = 0>
WJR_NODISCARD bool memcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
return ::memcmp(s0, s1, n * sizeof(T)) == 0;
}

#endif // _WJR_FAST_MEMCMP

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
WJR_NODISCARD const T* memmis(const T* s0, const U* s1, size_t n, _Pred pred) {
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
__has_fast_memmis_v<T, U, _Pred>, int> = 0>
WJR_NODISCARD const T* memrmis(const T* s0, const U* s1, size_t n, _Pred pred) {
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

#endif // _WJR_FAST_MEMMIS

#if defined(_WJR_FAST_MEMCNT)
template<typename T, typename U>
struct __has_fast_memcnt : std::conjunction<
is_comparable<T, U, std::equal_to<>>,
is_integrals<T, U>,
is_any_index_of<sizeof(T), 1, 2, 4>
> {};

template<typename T, typename U>
constexpr bool __has_fast_memcnt_v = __has_fast_memcnt<T, U>::value;

template<typename T, typename U, std::enable_if_t<__has_fast_memcnt_v<T, U>, int> = 0>
WJR_NODISCARD size_t memcnt(const T* s, U val, size_t n) {
auto p = is_possible_memory_comparable<T>(val, std::equal_to<>{});
if (p == ipmc_result::none) {
return 0;
}
using value_type = std::make_unsigned_t<make_integral_t<T>>;
auto __s = reinterpret_cast<const value_type*>(s);
auto __val = static_cast<value_type>(val);
return __memcnt(__s, __val, n);
}

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
static void __memset_helper(T* s, const U& val, size_t n) {

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
static void __memcpy_helper(T* s, const U* t, size_t n) {
static_assert(sizeof(T) == sizeof(U), "type mismatch");
#if defined(_WJR_FAST_MEMCPY)
#else
::memcpy(s, t, n * sizeof(T));
#endif
}

template<template<typename X, typename Y> typename TEST, typename T, typename U>
static void __memmove_helper(T* s, const U* t, size_t n) {
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
constexpr bool __has_fast_assign_memcpy_v = __has_fast_assign_memcpy<T, U>::value;

template<typename T, typename U>
void assign_memcpy(T* s, const U* t, size_t n) {
__memcpy_helper<is_byte_assignable>(s, t, n);
}

template<typename T, typename U>
void assign_memmove(T* s, const U* t, size_t n) {
__memmove_helper<is_byte_assignable>(s, t, n);
}

_WJR_ALGO_END

#endif // __WJR_ALGO_ALOG_H

_WJR_ALGO_BEGIN

template const uint8_t* __large__memchr(
const uint8_t*, uint8_t, size_t, std::equal_to<>)noexcept;

template const uint8_t* __large__memrchr(
const uint8_t*, uint8_t, size_t, std::equal_to<>)noexcept;

_WJR_ALGO_END

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
constexpr bool __has_fast_find_v = __has_fast_find<_Iter, _Val, _Pred>::value;

// find

template<typename _Iter, typename _Ty, typename _Pred>
WJR_CONSTEXPR20 _Iter find(
_Iter _First, _Iter _Last, const _Ty& _Val, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_find_v<_Iter, _Ty, _Pred>) {
const auto n = std::distance(_First, _Last);
if constexpr (!wjr::is_reverse_iterator_v<_Iter>) {
const auto first = wjr::get_address(_First);
return _First + (algo::memchr(first, _Val, n, pred) - first);
}
#if defined(_WJR_FAST_MEMCHR) // use algo::memchr
else {
const auto first = wjr::get_address(_Last - 1);
return _Last - (algo::memrchr(first, _Val, n, pred) - first);
}
#endif // _WJR_FAST_MEMCHR
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
WJR_CONSTEXPR20 _Iter find(_Iter _First, _Iter _Last, const _Ty& _Val) {
return wjr::find(_First, _Last, _Val, std::equal_to<>{});
}

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 _Iter find_if(_Iter _First, _Iter _Last, _Pr _Pred) {
return std::find_if(_First, _Last, _Pred);
}

template<typename _Iter, typename _Pr>
WJR_CONSTEXPR20 _Iter find_if_not(_Iter _First, _Iter _Last, _Pr _Pred) {
return std::find_if_not(_First, _Last, _Pred);
}

template<typename _Iter, typename _Val,
typename _Iter_value = iter_val_t<_Iter>>
struct __has_fast_count : std::conjunction<
wjr::is_contiguous_iter<_Iter>,
algo::__has_fast_memcnt<_Iter_value, _Val>
> {};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_count_v = __has_fast_count<_Iter, _Val>::value;

template<typename _Iter, typename _Ty>
WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
count(_Iter _First, _Iter _Last, const _Ty& _Val) {
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
WJR_CONSTEXPR20 typename std::iterator_traits<_Iter>::difference_type
count_if(_Iter _First, _Iter _Last, _Pr _Pred) {
return std::count_if(_First, _Last, _Pred);
}

// First use algo::memcmp
// Then use memcmp
template<typename _Iter1, typename _Iter2, typename _Pred,
typename _Iter_value1 = iter_val_t<_Iter1>,
typename _Iter_value2 = iter_val_t<_Iter2>>
struct __has_fast_equal_helper : std::conjunction<
wjr::is_contiguous_iter<_Iter1>,
wjr::is_contiguous_iter<_Iter2>,
std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
algo::__has_fast_memcmp<_Iter_value1, _Iter_value2, _Pred>
> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
struct __has_fast_equal : std::bool_constant<__has_fast_equal_helper<_Iter1, _Iter2, _Pred>::value> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_equal_v = __has_fast_equal<_Iter1, _Iter2, _Pred>::value;

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
constexpr bool __has_fast_mismatch_v = __has_fast_mismatch<_Iter1, _Iter2, _Pred>::value;

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
#if defined(_WJR_FAST_MEMMIS)
if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
const auto n = std::distance(_First1, _Last1);
if (is_unlikely(n == 0)) { return std::make_pair(_First1, _First2); }
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
#endif // _WJR_FAST_MEMMIS
}
return std::mismatch(_First1, _Last1, _First2, pred);
}

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(
_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_mismatch_v<_Iter1, _Iter2, _Pred>) {
const auto n = _Last1 - _First1;
const auto m = _Last2 - _First2;
const auto _M = n < m ? n : m;
return wjr::mismatch(_First1, _First1 + _M, _First2, pred);
}
}
return std::mismatch(_First1, _Last1, _First2, _Last2, pred);
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) {
return wjr::mismatch(_First1, _Last1, _First2, std::equal_to<>{});
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> mismatch(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
return wjr::mismatch(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
}

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
const auto n = _Last1 - _First1;
if (is_unlikely(n == 0)) { return true; }
if constexpr (!wjr::is_reverse_iterator_v<_Iter1>) {
const auto first1 = wjr::get_address(_First1);
const auto first2 = wjr::get_address(_First2);
return algo::memcmp(first1, first2, n, pred);
}
else {
const auto first1 = wjr::get_address(_Last1 - 1);
const auto _Last2 = _First2 + n;
const auto first2 = wjr::get_address(_Last2 - 1);
return algo::memcmp(first1, first2, n, pred);
}
}
}
return std::equal(_First1, _Last1, _First2, pred);
}

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_equal_v<_Iter1, _Iter2, _Pred>) {
const auto n = std::distance(_First1, _Last1);
const auto m = std::distance(_First2, _Last2);
if (n != m) { return false; }
return wjr::equal(_First1, _Last1, _First2, pred);
}
}
return std::equal(_First1, _Last1, _First2, _Last2, pred);
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2) {
return wjr::equal(_First1, _Last1, _First2, std::equal_to<>{});
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool equal(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
return wjr::equal(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
}

template<typename _Iter1, typename _Iter2, typename _Pred,
typename _Iter_value1 = iter_val_t<_Iter1>,
typename _Iter_value2 = iter_val_t<_Iter2>>
struct __has_fast_lexicographical_compare : std::conjunction<
wjr::is_any_of<_Pred, std::less<>>,
#if defined(_WJR_FAST_MEMMIS)
__has_fast_mismatch<_Iter1, _Iter2, _Pred>
#else
wjr::is_contiguous_iter<_Iter1>,
wjr::is_contiguous_iter<_Iter2>,
std::bool_constant<wjr::is_reverse_iterator_v<_Iter1> == wjr::is_reverse_iterator_v<_Iter2>>,
std::conditional_t<
wjr::is_reverse_iterator_v<_Iter1>,
std::false_type,
is_memory_comparable<_Iter_value1, _Iter_value2, _Pred>
>,
std::is_unsigned<_Iter_value1>,
std::is_unsigned<_Iter_value2>,
wjr::is_any_index_of<sizeof(_Iter_value1), 1>
#endif // _WJR_FAST_MEMMIS
> {};

template<typename _Iter1, typename _Iter2, typename _Pred>
constexpr bool __has_fast_lexicographical_compare_v = __has_fast_lexicographical_compare<_Iter1, _Iter2, _Pred>::value;

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 bool lexicographical_compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_lexicographical_compare_v<_Iter1, _Iter2, _Pred>) {
const auto n = std::distance(_First1, _Last1);
const auto m = std::distance(_First2, _Last2);
auto cl = std::min(n, m);
#if defined(_WJR_FAST_MEMMIS)
auto e = _First1 + cl;
auto pos = wjr::mismatch(_First1, e, _First2, std::equal_to<>{}).first - _First1;
if (pos != cl) {
return pred(_First1[pos], _First2[pos]);
}
#else
auto first1 = get_address(_First1);
auto first2 = get_address(_First2);
auto f = ::memcmp(first1, first2, cl * sizeof(iter_val_t<_Iter1>));
if (f != 0) {
return f < 0;
}
#endif // _WJR_FAST_MEMMIS
return n < m;
}
}
return std::lexicographical_compare(_First1, _Last1, _First2, _Last2, pred);
}

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 bool lexicographical_compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
return wjr::lexicographical_compare(_First1, _Last1, _First2, _Last2, std::less<>{});
}

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 int compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2, _Pred pred) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_lexicographical_compare_v<_Iter1, _Iter2, _Pred>) {
const auto n = std::distance(_First1, _Last1);
const auto m = std::distance(_First2, _Last2);
auto cl = std::min(n, m);
#if defined(_WJR_FAST_MEMMIS)
auto e = _First1 + cl;
auto pos = wjr::mismatch(_First1, e, _First2, std::equal_to<>{}).first - _First1;
if (pos != cl) {
return pred(_First1[pos], _First2[pos]) ? -1 : 1;
}
#else
auto first1 = get_address(_First1);
auto first2 = get_address(_First2);
auto f = ::memcmp(first1, first2, cl * sizeof(iter_val_t<_Iter1>));
if (f != 0) {
return f;
}
#endif // _WJR_FAST_MEMMIS
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
WJR_CONSTEXPR20 int compare(_Iter1 _First1, _Iter1 _Last1, _Iter2 _First2, _Iter2 _Last2) {
return wjr::compare(_First1, _Last1, _First2, _Last2, std::less<>{});
}

template<typename _Iter, typename _Val,
typename _Iter_ref = iter_ref_t<_Iter>>
struct __has_fast_fill : std::conjunction<
is_contiguous_iter<_Iter>,
algo::__has_fast_assign_memset<remove_cref_t<_Iter_ref>, add_cref_t<_Val>>
> {};

template<typename _Iter, typename _Val>
constexpr bool __has_fast_fill_v = __has_fast_fill<_Iter, _Val>::value;

template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void fill(_Iter _First, _Iter _Last, const _Val& value) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_fill_v<_Iter, _Val>) {
const auto n = std::distance(_First, _Last);
if constexpr (!is_reverse_iterator_v<_Iter>) {
const auto first = wjr::get_address(_First);
algo::assign_memset(first, value, n);
}
else {
if (is_unlikely(n == 0)) { return; }
const auto first = wjr::get_address(_Last - 1);
algo::assign_memset(first, value, n);
}
return;
}
}
return std::fill(_First, _Last, value);
}

template<typename _Iter, typename _Size, typename _Val>
WJR_CONSTEXPR20 _Iter fill_n(_Iter _First, _Size count, const _Val& value) {
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
constexpr bool __has_fast_copy_v = __has_fast_copy<_Input, _Output>::value;

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output copy(_Input _First1, _Input _Last1, _Output _First2) {
static_assert(!std::is_const_v<iter_ref_t<_Output>>, "Output iterator must not be const");
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_copy_v<_Input, _Output>) {
const auto n = std::distance(_First1, _Last1);
if (is_unlikely(n == 0)) { return _First2; }
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
WJR_CONSTEXPR20 _Output copy_n(_Input _First1, _Size count, _Output _First2) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_copy_v<_Input, _Output>) {
if (count <= 0) { return _First2; }
return wjr::copy(_First1, _First1 + count, _First2);
}
}
return std::copy_n(_First1, count, _First2);
}

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output copy_backward(_Input _First1, _Input _Last1, _Output _Last2) {
return wjr::copy(std::make_reverse_iterator(_Last1),
std::make_reverse_iterator(_First1),
std::make_reverse_iterator(_Last2)).base();
}

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output move(_Input _First1, _Input _Last1, _Output _First2) {
return wjr::copy(std::make_move_iterator(_First1), std::make_move_iterator(_Last1), _First2);
}

template<typename _Input, typename _Output>
WJR_CONSTEXPR20 _Output move_backward(_Input _First1, _Input _Last1, _Output _Last2) {
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
WJR_CONSTEXPR20 _Iter1 search(_Iter1 _First1, _Iter1 _Last1,
_Iter2 _First2, _Iter2 _Last2) {
return wjr::search(_First1, _Last1, _First2, _Last2, std::equal_to<>{});
}

template<typename _Iter1, typename _Iter2, typename _Pred>
WJR_CONSTEXPR20 _Iter1 search(_Iter1 _First1, _Iter1 _Last1,
_Iter2 _First2, _Iter2 _Last2, _Pred pred) {
return wjr::search(_First1, _Last1, wjr::default_searcher(_First2, _Last2, pred));
}

template<typename _Iter, typename _Searcher>
WJR_CONSTEXPR20 _Iter search(_Iter _First, _Iter _Last, const _Searcher& _Srch) {
return _Srch(_First, _Last).first;
}

template<typename _Iter, typename...Args,
std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int>>
WJR_CONSTEXPR20 void construct_at(_Iter iter, Args&&... args) {
#if defined(WJR_CPP_20)
std::construct_at(get_address(iter), std::forward<Args>(args)...);
#else
using value_type = iter_val_t<_Iter>;
(void)(::new (voidify(get_address(iter))) value_type(std::forward<Args>(args)...));
#endif // WJR_CPP_20
}

template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int>>
WJR_CONSTEXPR20 void construct_at(_Iter iter, default_construct_tag) {
using value_type = iter_val_t<_Iter>;
if (!is_constant_evaluated()) {
(void)(::new (voidify(get_address(iter))) value_type);
return;
}

if constexpr (std::is_trivially_default_constructible_v<value_type>) {
// do nothing
}
else {
// use value_construct
construct_at(iter, value_construct_tag());
}
}

template<typename _Iter, std::enable_if_t<is_iterator_v<wjr::remove_cvref_t<_Iter>>, int>>
WJR_CONSTEXPR20 void construct_at(_Iter iter, value_construct_tag) {
wjr::construct_at(iter);
}

template<typename Alloc, typename _Iter, typename...Args,
std::enable_if_t<!is_iterator_v<wjr::remove_cvref_t<Alloc>>, int>>
WJR_CONSTEXPR20 void construct_at(Alloc& al, _Iter iter, Args&&...args) {
using pointer = iter_address_t<_Iter>;
if constexpr (is_default_allocator_construct_v<Alloc, pointer, Args...>) {
wjr::construct_at(iter, std::forward<Args>(args)...);
}
else {
if constexpr(sizeof...(Args) == 1){
// allocator may not support default construct
// do default construct as value construct
if constexpr (is_any_of_v<default_construct_tag, Args...>
|| is_any_of_v<value_construct_tag, Args...>) {
return construct_at(al, iter);
}
}
std::allocator_traits<Alloc>::construct(al, get_address(iter), std::forward<Args>(args)...);
}
}

template<typename _Iter>
WJR_CONSTEXPR20 void destroy_at(_Iter ptr) {
using value_type = iter_val_t<_Iter>;
if constexpr (!std::is_trivially_destructible_v<value_type>) {
get_address(ptr)->~value_type();
}
}

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void destroy_at(Alloc& al, _Iter iter) {
if constexpr (is_default_allocator_destroy_v<Alloc, _Iter>) {
wjr::destroy_at(iter);
}
else {
std::allocator_traits<Alloc>::destroy(al, get_address(iter));
}
}

template<typename _Iter>
WJR_CONSTEXPR20 void destroy(_Iter _First, _Iter _Last) {
using value_type = iter_val_t<_Iter>;
if constexpr (!std::is_trivially_destructible_v<value_type>) {
for (; _First != _Last; ++_First) {
wjr::destroy_at(_First);
}
}
}

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void destroy(Alloc& al, _Iter _First, _Iter _Last) {
using value_type = iter_val_t<_Iter>;
if constexpr (!(is_default_allocator_destroy_v<Alloc, _Iter>
&& std::is_trivially_destructible_v<value_type>)) {
for (; _First != _Last; ++_First) {
wjr::destroy_at(al, _First);
}
}
}


template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void destroy_n(_Iter _First, const _Diff n) {
using value_type = iter_val_t<_Iter>;
if constexpr (!std::is_trivially_destructible_v<value_type>) {
for (; n > 0; ++_First, (void)--n) {
wjr::destroy_at(_First);
}
}
}

template<typename Alloc, typename _Iter, typename _Diff>
WJR_CONSTEXPR20 void destroy_n(Alloc& al, _Iter _First, _Diff n) {
using value_type = iter_val_t<_Iter>;
if constexpr (!(is_default_allocator_destroy_v<Alloc, _Iter> && std::is_trivially_destructible_v<value_type>)) {
for (; n > 0; ++_First, (void)--n) {
wjr::destroy_at(al, _First);
}
}
}

template<typename _Iter>
WJR_CONSTEXPR20 void uninitialized_default_construct(_Iter _First, _Iter _Last) {
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
WJR_CONSTEXPR20 void uninitialized_default_construct(
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
WJR_CONSTEXPR20 _Iter uninitialized_default_construct_n(_Iter _First, _Diff n) {
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
WJR_CONSTEXPR20 _Iter uninitialized_default_construct_n(
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
WJR_CONSTEXPR20 void uninitialized_value_construct(_Iter _First, _Iter _Last) {
if (!wjr::is_constant_evaluated()) {
return std::uninitialized_value_construct(_First, _Last);
}
for (; _First != _Last; ++_First) {
wjr::construct_at(_First, value_construct_tag());
}
}

template<typename Alloc, typename _Iter>
WJR_CONSTEXPR20 void uninitialized_value_construct(
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
WJR_CONSTEXPR20 _Iter uninitialized_value_construct_n(_Iter _First, _Diff n) {
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
WJR_CONSTEXPR20 _Iter uninitialized_value_construct_n(
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
constexpr bool __has_fast_uninitialized_copy_v = __has_fast_uninitialized_copy<_Input, _Output>::value;

template<typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_copy(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_uninitialized_copy_v<_Iter1, _Iter2>) {
const auto n = std::distance(_First, _Last);
if (is_unlikely(n == 0)) { return _Dest; }
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
WJR_CONSTEXPR20 _Iter2 uninitialized_copy(
Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
if constexpr (is_default_allocator_construct_v<Alloc, _Iter2, decltype(*_First)>) {
return wjr::uninitialized_copy(_First, _Last, _Dest);
}
else {
for (; _First != _Last; ++_Dest, (void)++_First) {
wjr::construct_at(al, _Dest, *_First);
}
}
}

template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_copy_n(_Iter1 _First, _Diff n, _Iter2 _Dest) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_uninitialized_copy_v<_Iter1, _Iter2>) {
if (n <= 0) { return _Dest; }
return wjr::uninitialized_copy(_First, _First + n, _Dest);
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
WJR_CONSTEXPR20 _Iter2 uninitialized_copy_n(Alloc& al, _Iter1 _First, _Diff n, _Iter2 _Dest) {
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
constexpr bool __has_fast_uninitialized_fill_v = __has_fast_uninitialized_fill<_Iter, _Val>::value;

template<typename _Iter, typename _Val>
WJR_CONSTEXPR20 void uninitialized_fill(_Iter _First, _Iter _Last, const _Val& val) {
if (!wjr::is_constant_evaluated()) {
if constexpr (__has_fast_uninitialized_fill_v<_Iter, _Val>) {
const auto n = static_cast<size_t>(std::distance(_First, _Last));
if constexpr (!is_reverse_iterator_v<_Iter>) {
const auto first = wjr::get_address(_First);
algo::construct_memset(first, val, n);
}
else {
if (is_unlikely(n == 0)) { return; }
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
WJR_CONSTEXPR20 void uninitialized_fill(_Iter _First, _Iter _Last, default_construct_tag) {
wjr::uninitialized_default_construct(_First, _Last);
}

template<typename _Iter>
WJR_CONSTEXPR20 void uninitialized_fill(_Iter _First, _Iter _Last, value_construct_tag) {
wjr::uninitialized_value_construct(_First, _Last);
}

template<typename Alloc, typename _Iter, typename _Val>
WJR_CONSTEXPR20 void uninitialized_fill(Alloc& al, _Iter _First, _Iter _Last, const _Val& val) {
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
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(_Iter _First, _Diff n, const _Val& val) {
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
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(_Iter _First, _Diff n, default_construct_tag) {
return wjr::uninitialized_default_construct_n(_First, n);
}

template<typename _Iter, typename _Diff>
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(_Iter _First, _Diff n, value_construct_tag) {
return wjr::uninitialized_value_construct_n(_First, n);
}

template<typename Alloc, typename _Iter, typename _Diff, typename _Val>
WJR_CONSTEXPR20 _Iter uninitialized_fill_n(Alloc& al, _Iter _First, _Diff n, const _Val& val) {
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
WJR_CONSTEXPR20 _Iter2 uninitialized_move(_Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
return wjr::uninitialized_copy(
std::make_move_iterator(_First),
std::make_move_iterator(_Last),
_Dest
);
}

template<typename Alloc, typename _Iter1, typename _Iter2>
WJR_CONSTEXPR20 _Iter2 uninitialized_move(Alloc& al, _Iter1 _First, _Iter1 _Last, _Iter2 _Dest) {
return wjr::uninitialized_copy(al,
std::make_move_iterator(_First),
std::make_move_iterator(_Last),
_Dest
);
}

template<typename _Iter1, typename _Diff, typename _Iter2>
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> uninitialized_move_n(_Iter1 _First, _Diff n, _Iter2 _Dest) {
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
WJR_CONSTEXPR20 std::pair<_Iter1, _Iter2> uninitialized_move_n(
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
WJR_CONSTEXPR20 void for_each(_Iter _First, _Iter _Last, _Func fn) {
for (; _First != _Last; ++_First) {
fn(*_First);
}
}

template<typename _Iter, typename _SizeT, typename _Func>
WJR_CONSTEXPR20 void for_each_n(_Iter _First, _SizeT n, _Func fn) {
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

// support small-size-optimize
// don't support copy-on-write yet

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

WJR_INLINE_CONSTEXPR20 size_type size() const noexcept {
return static_cast<size_type>(_Mylast - _Myfirst);
}

WJR_INLINE_CONSTEXPR20 size_type capacity() const noexcept {
return static_cast<size_type>(_Myend - _Myfirst);
}

WJR_INLINE_CONSTEXPR20 pointer data() noexcept {
return _Myfirst;
}

WJR_INLINE_CONSTEXPR20 const_pointer data() const noexcept {
return _Myfirst;
}

WJR_INLINE_CONSTEXPR20 pointer lastPtr() noexcept {
return _Mylast;
}

WJR_INLINE_CONSTEXPR20 const_pointer lastPtr() const noexcept {
return _Mylast;
}

WJR_INLINE_CONSTEXPR20 pointer endPtr() noexcept {
return _Myend;
}

WJR_INLINE_CONSTEXPR20 const_pointer endPtr() const noexcept {
return _Myend;
}

WJR_INLINE_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
_Mylast = _Myfirst + _Size;
}

WJR_INLINE_CONSTEXPR20 void inc_size(const difference_type _Size) noexcept {
_Mylast += _Size;
}

pointer _Myfirst = nullptr;
pointer _Mylast = nullptr;
pointer _Myend = nullptr;
};

// Not tested

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
std::string str = "vector_static_data is too small to hold the requested data";
str += "\n old capacity = " + std::to_string(max_capacity);
str += "\n allocate new capacity = " + std::to_string(_Newcapacity);
throw std::length_error(str);
#endif // _WJR_EXCEPTION
unreachable();
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
// do nothing
}

WJR_INLINE_CONSTEXPR20 size_type size() const noexcept {
return _M_size;
}

WJR_INLINE_CONSTEXPR20 size_type capacity() const noexcept {
return max_capacity;
}

WJR_INLINE_CONSTEXPR20 pointer data() noexcept {
return reinterpret_cast<pointer>(_M_storage);
}

WJR_INLINE_CONSTEXPR20 const_pointer data() const noexcept {
return reinterpret_cast<const_pointer>(_M_storage);
}

WJR_INLINE_CONSTEXPR20 pointer lastPtr() noexcept {
return data() + size();
}

WJR_INLINE_CONSTEXPR20 const_pointer lastPtr() const noexcept {
return data() + size();
}

WJR_INLINE_CONSTEXPR20 pointer endPtr() noexcept {
return data() + capacity();
}

WJR_INLINE_CONSTEXPR20 const_pointer endPtr() const noexcept {
return data() + capacity();
}

WJR_INLINE_CONSTEXPR20 void set_size(const size_type _Size) noexcept {
_M_size = _Size;
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

WJR_INLINE_CONSTEXPR20 size_type size() const noexcept {
return _M_size;
}

WJR_INLINE_CONSTEXPR20 size_type capacity() const noexcept {
return is_small() ? max_capacity : _M_capacity;
}

WJR_INLINE_CONSTEXPR20 pointer data() noexcept {
return _M_ptr;
}

WJR_INLINE_CONSTEXPR20 const_pointer data() const noexcept {
return _M_ptr;
}

WJR_INLINE_CONSTEXPR20 pointer lastPtr() noexcept {
return data() + size();
}

WJR_INLINE_CONSTEXPR20 const_pointer lastPtr() const noexcept {
return data() + size();
}

WJR_INLINE_CONSTEXPR20 pointer endPtr() noexcept {
return data() + capacity();
}

WJR_INLINE_CONSTEXPR20 const_pointer endPtr() const noexcept {
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

if (is_likely(this != std::addressof(other))) {
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

if (is_likely(this != std::addressof(other))) {
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

WJR_INLINE_CONSTEXPR20 size_type size() const noexcept {
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

WJR_INLINE_CONSTEXPR20 size_type capacity() const noexcept {
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

WJR_INLINE_CONSTEXPR20 pointer data() noexcept {
return getData().data();
}

WJR_INLINE_CONSTEXPR20 const_pointer data() const noexcept {
return getData().data();
}

WJR_INLINE_CONSTEXPR20 const_pointer cdata() const noexcept {
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

// non-standard extension functions

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

WJR_INLINE_CONSTEXPR20 _Alty& getAllocator() noexcept {
return _Myval.first();
}
WJR_INLINE_CONSTEXPR20 const _Alty& getAllocator() const noexcept {
return _Myval.first();
}

WJR_INLINE_CONSTEXPR20 data_type& getData() noexcept {
return _Myval.second();
}
WJR_INLINE_CONSTEXPR20 const data_type& getData() const noexcept {
return _Myval.second();
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

// n must less or equal
WJR_INLINE_CONSTEXPR20 vector& chop(const size_type n) noexcept {
_M_erase_at_end(end() - n);
return *this;
}

// n <= size()
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
#ifndef __WJR_STRING_FUNC_H
#define __WJR_STRING_FUNC_H

// functions for string encode

#include <optional>

#pragma once
#ifndef __WJR_ERROR_HANDLE_H
#define __WJR_ERROR_HANDLE_H

namespace wjr {
// TODO :

enum class errc {
ok               = 0x00,
overflow         = 0x01,
underflow        = 0x02,
out_of_range     = overflow | underflow,
noconv           = 0x04,
buffer_too_small = 0x05,
};
}

#endif // __WJR_ERROR_HANDLE_H
#pragma once
#ifndef __WJR_RTC_VAR_H
#define __WJR_RTC_VAR_H


_WJR_BEGIN

template<typename T>
constexpr decltype(auto) get_cvar(T&& value) { return std::forward<T>(value); }

template<typename T, T _Val>
constexpr auto get_cvar(std::integral_constant<T, _Val>) { return get_cvar(_Val); }


_WJR_END

#endif // __WJR_RTC_VAR_H

_WJR_BEGIN

namespace __string_func_traits {
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(skipw, skipw);
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(rskipw, rskipw);
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(skipz, skipz);
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(skipd, skipd);
}

// fast integer conversion of strings with different encodings
// requires :
// 1. forward iterator
// 2. constexpr uint32_t todigit<base = 10[, idx = 0]>
// related to base
// return (digit(ch)) * (base ^ idx);
// if is a invalid digit, return 1u << 24;
// use this function to find last of digit
// 3. constexpr bool isspace
// use this function to skip white space
template<typename Traits>
class string_func {
public:
using traits_type = Traits;
using value_type = typename traits_type::value_type;

constexpr static uint32_t invalid_digit = 1u << 24;

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalnum(value_type ch);

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalpha(value_type ch);

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool islower(value_type ch);

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isupper(value_type ch);

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isxdigit(value_type ch);

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isspace(value_type ch);

WJR_NODISCARD WJR_INLINE_CONSTEXPR static value_type tolower(value_type ch);

WJR_NODISCARD WJR_INLINE_CONSTEXPR static value_type toupper(value_type ch);

// base <= 10, digit is ['0', '0' + base - 1]
// base > 10, digit is ['0', '9'] and ['a', 'a' + (base - 10) - 1] and ['A', 'A' + (base - 10) - 1]
template<unsigned int Base = 10>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static bool isdigit(value_type ch);

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static bool isdigit(value_type ch, int Base);

template<unsigned int Base = 10>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static uint32_t todigit(value_type ch);

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static uint32_t toalnum(value_type ch);

// skip whit space
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static _Iter skipw(_Iter _First, _Iter _Last);

// skip whit space at end
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static _Iter rskipw(_Iter _First, _Iter _Last);

// skip leading zeros
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static _Iter skipz(_Iter _First, _Iter _Last);

// skip digit
template<unsigned int Base, typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static _Iter skipd(_Iter _First, _Iter _Last);

template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static _Iter skipd(_Iter _First, _Iter _Last, int Base);

enum flags {
NO_FLAGS                   = 0,
ALLOW_PREFIX               = 0x01,
ALLOW_TAIL                 = 0x02,
ALLOW_SIGN                 = 0x04,
ALLOW_LEADING_SPACE        = 0x08,
ALLOW_TRAILING_SPACE       = 0x10,
ALLOW_LEADING_ZEROS        = 0x20,
ALLOW_ONLY_LOWERCASE	   = 0x40,
ALLOW_ONLY_UPPERCASE	   = 0x80
};

template<typename T, typename _Iter, typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static T to_integral(
F f,
_Iter _First, _Iter _Last,
errc& _Err, _Iter& _Pos, int base) noexcept;

template<typename T, typename _Iter>
WJR_INLINE_CONSTEXPR20 static void from_integral(
T _Val, _Iter _First, _Iter _Last,
errc& _Err, _Iter& _Pos, int base) noexcept;

};

template<typename T, typename Func>
class integral_conversion_details {
public:
using func_type = Func;
using value_type = typename func_type::value_type;
using flags = typename func_type::flags;

template<typename _Iter, typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static T work(
F f,
_Iter _First, _Iter _Last,
errc& _Err, _Iter& _Pos, int base) noexcept;

// copy of std::to_chars
template <typename _Iter>
WJR_INLINE_CONSTEXPR20 static void work(
T _Val, _Iter _First, _Iter _Last,
errc& _Err, _Iter& _Pos, int base) noexcept;

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
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR static T __work(
F f,
_Iter _First, _Iter _Last,
errc& _Err, WJR_MAYBE_UNUSED _Iter& _Pos,
int base, _Iter _Zero, bool _Is_minus) noexcept;

};

#define __CONV_EMPTY_RET				        \
_Err = errc::noconv;					\
return static_cast<T>(0);
#define __CONV_NEXT								\
if(is_unlikely(++_First == _Last)){	    \
__CONV_EMPTY_RET					\
}

template<typename T, typename Func>
template<typename _Iter, typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T integral_conversion_details<T, Func>::work(
F f,
_Iter _First, _Iter _Last,
errc& _Err, _Iter& _Pos, int base) noexcept {
const auto _Flags = get_cvar(f);

// skip white space
if (_Flags & flags::ALLOW_LEADING_SPACE) {
_First = func_type::skipw(_First, _Last);
}

if (is_unlikely(_First == _Last)) {
__CONV_EMPTY_RET;
}

bool _Is_minus = false;

// eat sign
if (_Flags & flags::ALLOW_SIGN) {
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

if (_Flags & flags::ALLOW_TRAILING_SPACE) {
_First = func_type::skipw(_First, _Last);
}

constexpr auto _B = func_type::toalnum('B');
constexpr auto _X = func_type::toalnum('X');

WJR_MAYBE_UNUSED auto _Zero = _Last;

if (_Flags & flags::ALLOW_PREFIX) {
switch (base) {
case 0: {
if (*_First == '0') {
// eat '0'
if (is_unlikely(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
_Zero = _First;
auto ch = func_type::toalnum(*_First);
switch (ch) {
case _B: {
// eat 'b'/'B'
if (is_unlikely(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
return __work(f, _First, _Last, _Err, _Pos, 2, _Zero, _Is_minus);
}
case _X: {
// eat 'x'/'X'
if (is_unlikely(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
return __work(f, _First, _Last, _Err, _Pos, 16, _Zero, _Is_minus);
}
default: {
return __work(f, _First, _Last, _Err, _Pos, 8, _Zero, _Is_minus);
}
}
}
return __work(f, _First, _Last, _Err, _Pos, 10, _Last, _Is_minus);
}
case 2: {
if (*_First == '0') {
if (is_unlikely(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
_Zero = _First;
if (func_type::toalnum(*_First) == _B) {
if (is_unlikely(++_First == _Last)) {
_Pos = _Zero;
return static_cast<T>(0);
}
}
}
break;
}
case 8: {
if (*_First == '0') {
if (is_unlikely(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
_Zero = _First;
}
break;
}
case 16: {
if (*_First == '0') {
if (is_unlikely(++_First == _Last)) {
_Pos = _First;
return static_cast<T>(0);
}
_Zero = _First;
if (func_type::toalnum(*_First) == _X) {
if (is_unlikely(++_First == _Last)) {
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

return __work(f, _First, _Last, _Err, _Pos, base, _Zero, _Is_minus);
}

template<typename T, typename Func>
template<typename _Iter, typename F>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR T integral_conversion_details<T, Func>::__work(
F f,
_Iter _First, _Iter _Last,
errc& _Err, WJR_MAYBE_UNUSED _Iter& _Pos,
int base, _Iter _Zero, bool _Is_minus) noexcept {

const auto _Flags = get_cvar(f);

auto _Next = _First;

if (_Flags & flags::ALLOW_LEADING_ZEROS) {
_Next = func_type::skipz(_First, _Last);
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
auto ch = func_type::toalnum(*_Next);

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

template<typename T, typename Func>
template <typename _Iter>
WJR_INLINE_CONSTEXPR20 void integral_conversion_details<T, Func>::work(
T _Val, _Iter _First, _Iter _Last,
errc& _Err, _Iter& _Pos, int base) noexcept {

auto uval = make_unsigned_v(_Val);

if constexpr (std::is_signed_v<T>) {
if (_Val < 0) {
if (_First == _Last) {
_Err = errc::buffer_too_small;
return;
}
*_First = '-';
++_First;
uval = static_cast<uT>(0 - uval);
}
}

constexpr size_t _Buff_size = sizeof(uT) * 8;
value_type _Buff[_Buff_size];
char* const _Buff_end = _Buff + _Buff_size;
char* _RNext = _Buff_end;

switch (base) {
case 10:
{ // Derived from _UIntegral_to_buff()
// Performance note: Ryu's digit table should be faster here.
constexpr bool _Use_chunks = sizeof(uT) > sizeof(size_t);

if constexpr (_Use_chunks) { // For 64-bit numbers on 32-bit platforms, work in chunks to avoid 64-bit
// divisions.
while (uval > 0xFFFF'FFFFU) {
// Performance note: Ryu's division workaround would be faster here.
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

ptrdiff_t _Digits_written = _Buff_end - _RNext;

if constexpr (is_random_iterator_v<_Iter>) {
const auto _Size = std::distance(_First, _Last);
if (_Size < _Digits_written) {
_Err = errc::buffer_too_small;
return;
}
_First = wjr::copy_n(_RNext, _Digits_written, _First);
}
else {
for (; _Digits_written && _First != _Last; ++_RNext, ++_First, --_Digits_written) {
*_First = *_RNext;
}
if (_Digits_written) {
_Err = errc::buffer_too_small;
return;
}
}

_Pos = _First;
_Err = errc::ok;
return;
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isalnum(value_type ch) {
return traits_type::isalnum(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isalpha(value_type ch) {
return traits_type::isalpha(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::islower(value_type ch) {
return traits_type::islower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isupper(value_type ch) {
return traits_type::isupper(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isxdigit(value_type ch) {
return traits_type::isxdigit(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool string_func<Traits>::isspace(value_type ch) {
return traits_type::isspace(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR typename string_func<Traits>::value_type
string_func<Traits>::tolower(value_type ch) {
return traits_type::tolower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR typename string_func<Traits>::value_type
string_func<Traits>::toupper(value_type ch) {
return traits_type::toupper(ch);
}

template<typename Traits>
template<unsigned int Base>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR bool string_func<Traits>::isdigit(value_type ch) {
return todigit<Base>(ch) != invalid_digit;
}

template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR bool string_func<Traits>::isdigit(value_type ch, int Base) {
return toalnum(ch) < Base;
}

template<typename Traits>
template<unsigned int Base>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint32_t string_func<Traits>::todigit(value_type ch) {
return traits_type::template todigit<Base>(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint32_t string_func<Traits>::toalnum(value_type ch) {
return todigit<36>(ch);
}

template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::skipw(_Iter _First, _Iter _Last) {
if constexpr (__string_func_traits::has_static_member_function_skipw_v<traits_type, _Iter, _Iter>) {
return traits_type::skipw(_First, _Last);
}
else {
while (_First != _Last && isspace(*_First)) ++_First;
return _First;
}
}

// skip whit space at end
template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::rskipw(_Iter _First, _Iter _Last) {
if constexpr (__string_func_traits::has_static_member_function_rskipw_v<traits_type, _Iter, _Iter>) {
return traits_type::rskipw(_First, _Last);
}
else {
while (_First != _Last && isspace(*(_Last - 1))) --_Last;
return _Last;
}
}

template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::skipz(_Iter _First, _Iter _Last) {
if constexpr (__string_func_traits::has_static_member_function_skipz_v<traits_type, _Iter, _Iter>) {
return traits_type::skipz(_First, _Last);
}
else {
while (_First != _Last && *_First == '0') ++_First;
return _First;
}
}

// skip digit
template<typename Traits>
template<unsigned int Base, typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>::skipd(_Iter _First, _Iter _Last) {
while (_First != _Last && isdigit<Base>(*_First)) ++_First;
return _First;
}

template<typename Traits>
template<typename _Iter>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR _Iter string_func<Traits>
::skipd(_Iter _First, _Iter _Last, int Base) {
if constexpr (__string_func_traits::has_static_member_function_skipz_v<traits_type, _Iter, _Iter, int>) {
return traits_type::skipz(_First, _Last, Base);
}
else {
while (_First != _Last && isdigit(*_First, Base)) ++_First;
return _First;
}
}

template<typename Traits>
template<typename T, typename _Iter, typename F>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T string_func<Traits>::to_integral(
F f,
_Iter _First, _Iter _Last,
errc& _Err, _Iter& _Pos, int base) noexcept {
return integral_conversion_details<T, string_func<Traits>>
::template work(f, _First, _Last, _Err, _Pos, base);
}

template<typename Traits>
template<typename T, typename _Iter>
WJR_INLINE_CONSTEXPR20 void string_func<Traits>::from_integral(
T _Val, _Iter _First, _Iter _Last,
errc& _Err, _Iter& _Pos, int base) noexcept {
return integral_conversion_details<T, string_func<Traits>>
::template work(_Val, _First, _Last, _Err, _Pos, base);
}

_WJR_END

#endif // __WJR_STRING_FUNC_H

_WJR_BEGIN

/*---------------------------data for string---------------------------*/

// Please note that string always has a '\0' terminator, so the capacity of string
// should be the capacity of the container - 1,
// by inheriting vector_sso_data and setting the virtual capacity to the actual capacity - 1
// All operations on string_data are performed only through the virtual capacity,
// and the virtual capacity is converted to the actual capacity inside string_data
// virtual size is equal to actual size, so EOF needs to be handled manually
// virtual size = actual size <= virtual capacity = actual capacity - 1 < actual capacity

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

// virtual max capacity = max capacity - 1
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

WJR_INLINE_CONSTEXPR20 size_type capacity() const noexcept {
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

// virtual max capacity = max capacity - 1
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

WJR_INLINE_CONSTEXPR20 size_type capacity() const noexcept {
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

template<typename Char, typename Traits = std::char_traits<Char>>
class basic_string_view;

template<typename Char, typename Traits>
struct __base_string_view_traits<basic_string_view<Char, Traits>> {
using _Derived = basic_string_view<Char, Traits>;

using _Traits_helper = __traits_helper<Traits>;

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

template<typename Derived>
class __base_string_view : public __base_string_view_traits<Derived> {
public:
using _Mybase = __base_string_view_traits<Derived>;
template<typename T>
constexpr static bool _Is_noptr_string_view_like_v = _Mybase::template _Is_noptr_string_view_like_v<T>;

using value_type = typename _Mybase::value_type;
using size_type = typename _Mybase::size_type;

constexpr static size_type npos = _Mybase::npos;

WJR_INLINE_CONSTEXPR void swap(Derived& other) noexcept {
const Derived tmp{ other };
other = this->derived();
this->derived() = tmp;
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR Derived substr(
const size_type off = 0, size_type n = npos) const WJR_NOEXCEPT {
#if defined(_WJR_EXCEPTION)
if (is_likely(off <= derived().size())) {
#endif // _WJR_EXCEPTION
return nesubstr(off, n);
#if defined(_WJR_EXCEPTION)
}
throw std::out_of_range("out of range at basic_string_view::substr");
#endif // _WJR_EXCEPTION
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR Derived
nesubstr(const size_type off = 0, size_type n = npos) const noexcept {
n = std::min(n, this->derived().size() - off);
return Derived(this->derived().data() + off, n);
}

// no exception view

WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived view(
const value_type* s, const size_type n) noexcept {
return Derived(s, n);
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived view(
const value_type* s) noexcept {
return Derived(s);
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived view(
std::initializer_list<value_type> il) noexcept {
return view(il.begin(), il.size());
}

template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived view(const StringView& t) noexcept {
return Derived(t);
}

template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived view(const StringView& t,
const size_type off, const size_type n = npos) noexcept {
return view(t).nesubstr(off, n);
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR Derived view() const noexcept {
return this->derived();
}

template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static Derived eview(const StringView& t,
const size_type off, const size_type n = npos) WJR_NOEXCEPT {
return view(t).substr(off, n);
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR Derived prefix(size_type n) const noexcept {
n = std::min(n, this->derived().size());
return Derived(this->derived().begin(), n);
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR Derived suffix(size_type n) const noexcept {
n = std::min(n, this->derived().size());
return Derived(this->derived().end() - n, n);
}
protected:
Derived& derived() { return static_cast<Derived&>(*this); }
const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

template<typename Char, typename Traits>
class basic_string_view : public __base_string_view<basic_string_view<Char, Traits>> {
public:
using _Mybase = __base_string_view<basic_string_view<Char, Traits>>;
private:

using _Traits_helper = typename _Mybase::_Traits_helper;

using _Std_view_type = typename _Mybase::_Std_view_type;

template<typename T>
constexpr static bool _Is_string_view_like_v = _Mybase::template _Is_string_view_like_v<T>;

template<typename T>
constexpr static bool _Is_noptr_std_string_view_like_v = _Mybase::template _Is_noptr_std_string_view_like_v<T>;

template<typename T>
constexpr static bool _Is_noptr_string_view_like_v = _Mybase::template _Is_noptr_string_view_like_v<T>;

public:
static_assert(!std::is_array_v<Char>&& std::is_trivial_v<Char>&& std::is_standard_layout_v<Char>,
"The character type of basic_string_view must be a non-array trivial standard-layout type. See N4910 "
"23.1 [strings.general]/1.");

static_assert(std::is_same_v<Char, typename Traits::char_type>, "");

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

using _Mybase::swap;
using _Mybase::substr;
using _Mybase::view;
using _Mybase::eview;
using _Mybase::prefix;
using _Mybase::suffix;

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

WJR_NODISCARD WJR_INLINE_CONSTEXPR size_type size() const noexcept {
return _Mysize;
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR size_type length() const noexcept {
return _Mysize;
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR bool empty() const noexcept {
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
WJR_NODISCARD WJR_CONSTEXPR20 int compare(const StringView& t) const noexcept {
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
WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off, const size_type n, const StringView& t)
const WJR_NOEXCEPT {
return eview(*this, off, n).compare(t);
}

template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const StringView& t,
const size_type off2, const size_type n2)
const WJR_NOEXCEPT {
return eview(*this, off1, n1).compare(eview(t, off2, n2));
}

WJR_NODISCARD WJR_CONSTEXPR20 int compare(const Char* const ptr) const {
return compare(view(ptr));
}

WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off, const size_type n, const Char* const ptr) const {
return eview(*this, off, n).compare(view(ptr));
}

WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1,
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
if (is_likely(off <= size())) {
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
if (is_likely(off <= size())) {
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
if (is_likely(size() != 0)) {
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
if (is_likely(size() != 0)) {
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
if (is_likely(off <= size())) {
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
if (is_likely(size() != 0)) {
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

// non-standard extension functions

/*------External extension function------*/

template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const StringView& t) const noexcept {
const auto sv = view(t);
if constexpr (_Traits_helper::is_default_equal::value) {
return wjr::equal(begin(), end(), sv.begin(), sv.end());
}
else {
return compare(sv) == 0;
}
}

template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off, const size_type n, const StringView& t)
const WJR_NOEXCEPT {
return eview(*this, off, n).equal(view(t));
}

template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off1, const size_type n1, const StringView& t,
const size_type off2, const size_type n2) const WJR_NOEXCEPT {
return eview(*this, off1, n1).equal(eview(t, off2, n2));
}

WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const Char* const ptr) const {
return equal(view(ptr));
}

WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off, const size_type n, const Char* const ptr) const {
return equal(off, n, view(ptr));
}

WJR_NODISCARD WJR_CONSTEXPR20 bool equal(const size_type off1, const size_type n1,
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
set_end();
}

WJR_CONSTEXPR20 explicit basic_string(const allocator_type& al) noexcept
: m_core(al) {
set_end();
}

WJR_CONSTEXPR20 basic_string(const basic_string& other)
: m_core(other.m_core) {
set_end();
}

WJR_CONSTEXPR20 basic_string(const basic_string& other, const allocator_type& al)
: m_core(other.m_core, al) {
set_end();
}

WJR_CONSTEXPR20 basic_string(basic_string&& other)
: m_core(std::move(other.m_core)) {
set_end();
other.set_end();
}

WJR_CONSTEXPR20 basic_string(basic_string&& other, const allocator_type& al)
: m_core(std::move(other.m_core), al) {
set_end();
other.set_end();
}

template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 basic_string(iter first, iter last, const allocator_type& al = allocator_type())
: m_core(first, last, al) {
set_end();
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
set_end();
}

WJR_CONSTEXPR20 operator std::basic_string_view<Char, traits_type>() const noexcept {
return { data(), size() };
}

WJR_CONSTEXPR20 ~basic_string() noexcept = default;

WJR_CONSTEXPR20 basic_string& operator=(const basic_string& other) {

if (is_likely(this != std::addressof(other))) {
m_core = other.m_core;
set_end();
}

return *this;
}

WJR_CONSTEXPR20 basic_string& operator=(basic_string&& other) noexcept {

if (is_likely(this != std::addressof(other))) {
m_core = std::move(other.m_core);
set_end();
other.set_end();
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
set_end();
}

WJR_CONSTEXPR20 void resize(const size_type n, const value_type c) {
m_core.resize(n, c);
set_end();
}

WJR_CONSTEXPR20 void reserve(const size_type c) {
m_core.reserve(c);
set_end();
}

WJR_CONSTEXPR20 void shrink_to_fit() {
m_core.shrink_to_fit();
set_end();
}

WJR_CONSTEXPR20 void clear() {
resize(0);
}

WJR_INLINE_CONSTEXPR20 size_type size() const {
return m_core.size();
}

WJR_INLINE_CONSTEXPR20 size_type length() const {
return size();
}

WJR_INLINE_CONSTEXPR20 size_type max_size() const { return std::numeric_limits<size_type>::max(); }

WJR_INLINE_CONSTEXPR20 size_type capacity() const {
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
set_end();
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
set_end();
return *this;
}

WJR_CONSTEXPR20 basic_string& append(const value_type c) {
push_back(c);
return *this;
}

WJR_CONSTEXPR20 void push_back(const value_type c) {
m_core.push_back(c);
set_end();
}

WJR_CONSTEXPR20 void push_front(const value_type c) {
prepend(c);
}

WJR_CONSTEXPR20 void pop_back() {
m_core.pop_back();
set_end();
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
set_end();
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
set_end();
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
set_end();
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
set_end();
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
set_end();
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
set_end();
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
set_end();
return begin() + __old_pos;
}

WJR_CONSTEXPR20 void swap(basic_string& other)noexcept {
m_core.swap(other.m_core);
set_end();
other.set_end();
}

WJR_INLINE_CONSTEXPR20 const value_type* c_str() const { return m_core.data(); }
WJR_INLINE_CONSTEXPR20 value_type* data() { return m_core.data(); }
WJR_INLINE_CONSTEXPR20 const value_type* data() const { return m_core.data(); }

WJR_INLINE_CONSTEXPR20 bool empty()const { return m_core.empty(); }

WJR_NODISCARD WJR_CONSTEXPR20 int compare(const basic_string& other) const noexcept {
return compare(view(other));
}

WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const basic_string& other) const {
return compare(off1, n1, view(other));
}

WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const basic_string& other,
const size_type off2, const size_type n2 = npos) const {
return compare(off1, n1, view(other, off2, n2));
}

WJR_NODISCARD WJR_CONSTEXPR20 int compare(const value_type* s) const {
return compare(view(s));
}

WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const value_type* s) const {
return compare(off1, n1, view(s));
}

WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const value_type* s,
const size_type n2) const {
return compare(off1, n1, view(s, n2));
}

template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 int compare(const StringView& t) const noexcept {
return view().compare(view(t));
}

template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const StringView& t) const {
return view(*this, off1, n1).compare(view(t));
}

template<typename StringView, std::enable_if_t<_Is_noptr_string_view_like_v<StringView>, int> = 0>
WJR_NODISCARD WJR_CONSTEXPR20 int compare(const size_type off1, const size_type n1, const StringView& t,
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
set_end();
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
set_end();
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

// non-standard extension functions

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

WJR_INLINE_CONSTEXPR20 void set_end() {
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
set_end();
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
set_end();
}

WJR_CONSTEXPR20 basic_string(const size_type _Count, value_construct_tag, const allocator_type& al = allocator_type())
: m_core(_Count, value_construct_tag(), al) {
set_end();
}

WJR_CONSTEXPR20 void resize(const size_type _Newsize, default_construct_tag) {
m_core.resize(_Newsize, default_construct_tag());
set_end();
}

WJR_CONSTEXPR20 void resize(const size_type _Newsize, value_construct_tag) {
m_core.resize(_Newsize, value_construct_tag());
set_end();
}

WJR_CONSTEXPR20 void push_back(default_construct_tag) {
m_core.push_back(default_construct_tag());
set_end();
}

WJR_CONSTEXPR20 void push_back(value_construct_tag) {
m_core.push_back(value_construct_tag());
set_end();
}

WJR_CONSTEXPR20 basic_string& append(const size_type n, default_construct_tag) {
m_core.append(n, default_construct_tag());
set_end();
return *this;
}

WJR_CONSTEXPR20 basic_string& append(const size_type n, value_construct_tag) {
m_core.append(n, value_construct_tag());
set_end();
return *this;
}

WJR_CONSTEXPR20 basic_string& chop(const size_type n) noexcept {
m_core.chop(n);
set_end();
return *this;
}

WJR_CONSTEXPR20 basic_string& truncate(const size_type n) noexcept {
m_core.truncate(n);
set_end();
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
const auto rhs_size = traits_length(rhs);
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
const auto lhs_size = traits_length(lhs);
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
// get characters into string, discard delimiter
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
// get characters into string, discard newline
return getline(std::move(is), str, is.widen('\n'));
}

template <typename Char, typename Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& getline(
std::basic_istream<Char, Traits>& is, basic_string<Char, Traits, Alloc, Data>& str, const Char delim) {
// get characters into string, discard delimiter
return getline(std::move(is), str, delim);
}

template <typename Char, typename Traits, typename Alloc, typename Data>
std::basic_istream<Char, Traits>& getline(
std::basic_istream<Char, Traits>& is, basic_string<Char, Traits, Alloc, Data>& str) {
// get characters into string, discard newline
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
return std::hash<std::string_view>(str);
}
};
template<typename Char, typename Traits, typename Alloc, typename Data>
struct hash<wjr::basic_string<Char, Traits, Alloc, Data>> {
constexpr decltype(auto) operator()(const wjr::basic_string<Char, Traits, Alloc, Data>& str) const noexcept {
return std::hash<wjr::string_view>(str);
}
};
}

_WJR_BEGIN

// encode functions
// the code writing specification is related to the function namespace,
// which requires encoding auxiliary functions and auxiliary classes,
// followed by defining corresponding traits and basic_string_view, implemented after all

namespace ascii {

template<int Base, int idx = 0>
struct __to_digit_table {

constexpr static uint32_t invalid = 1u << 24;
constexpr static uint32_t multi = power<uint32_t>(Base, idx);

static_assert(Base >= 2 && Base <= 36, "");
static_assert(idx >= 0 && idx < 4, "");

constexpr __to_digit_table() : table() {
for (int i = 0; i < 256; ++i) {
table[i] = invalid;
}

{
constexpr int _Max = '0' + (Base >= 10 ? 10 : Base) - 1;
for (int i = '0'; i <= _Max; ++i) {
table[i] = (i - '0') * multi;
}
}

if constexpr (Base > 10) {
constexpr int _Maxl = 'a' + (Base - 10) - 1;
constexpr int _Maxu = _Maxl + ('A' - 'a');
for (int i = 'a'; i <= _Maxl; ++i) {
table[i] = ((i - 'a') + 10) * multi;
}

for (int i = 'A'; i <= _Maxu; ++i) {
table[i] = ((i - 'A') + 10) * multi;
}
}

}

constexpr uint32_t operator[](uint8_t ch) const { return table[ch]; }

private:
uint32_t table[256];
};

template<int Base, int idx = 0>
constexpr static __to_digit_table<Base, idx> __to_digit_table_v;

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

class encode {
public:

using value_type = char;

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalnum(char ch) {
return __char_code_table[make_unsigned_v(ch)] & (__lower | __upper | __digit);
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalpha(char ch) {
return __char_code_table[make_unsigned_v(ch)] & (__lower | __upper);
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool islower(char ch) {
return __char_code_table[make_unsigned_v(ch)] & __lower;
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isupper(char ch) {
return __char_code_table[make_unsigned_v(ch)] & __upper;
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isdigit(char ch) {
return __char_code_table[make_unsigned_v(ch)] & __digit;
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isxdigit(char ch) {
return __char_code_table[make_unsigned_v(ch)] & __xdigit;
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isspace(char ch) {
return __char_code_table[make_unsigned_v(ch)] & __white;
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static char tolower(char ch) {
return isupper(ch) ? ch + 'a' - 'A' : ch;
}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static char toupper(char ch) {
return islower(ch) ? ch + 'A' - 'a' : ch;
}

// transfrom ch to digit
// if ch is a valid digit, return digit(ch) * (Base ^ idx)
// else return invalid
template<unsigned int Base = 10, int idx = 0>
WJR_NODISCARD WJR_INLINE_CONSTEXPR static uint32_t todigit(char ch) {
return __to_digit_table_v<Base, idx>[ch];
}

};

using func_type = string_func<encode>;

}

template<typename Traits = std::char_traits<char>>
struct ascii_traits : public Traits {};

template<typename Traits>
struct __traits_helper<ascii_traits<Traits>> : __traits_helper<Traits> {};

template<typename Traits>
class basic_string_view<char, ascii_traits<Traits>>
: public basic_string_view<char, Traits>,
public __base_string_view<basic_string_view<char, ascii_traits<Traits>>> {
private:
using _Mybase1 = basic_string_view<char, Traits>;
using _Mybase2 = __base_string_view<basic_string_view<char, ascii_traits<Traits>>>;
public:
using _Mybase1::_Mybase1;
using _Mybase1::operator=;
using _Mybase1::begin;
using _Mybase1::end;

using _Mybase2::swap;
using _Mybase2::substr;
using _Mybase2::view;
using _Mybase2::eview;
using _Mybase2::prefix;
using _Mybase2::suffix;

using traits_type = ascii_traits<Traits>;
using size_type = typename _Mybase1::size_type;
using func_type = string_func<typename ascii::encode>;
using flags = typename func_type::flags;

basic_string_view(const _Mybase1& base) : _Mybase1(base) {}

WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalnum(char ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isalpha(char ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool islower(char ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isupper(char ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isdigit(char ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isxdigit(char ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static bool isspace(char ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static char tolower(char ch);
WJR_NODISCARD WJR_INLINE_CONSTEXPR static char toupper(char ch);

WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view ltrim() const;
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view rtrim() const;
WJR_NODISCARD WJR_INLINE_CONSTEXPR basic_string_view trim() const;

// support constexpr if str is constexpr
template<typename T>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T to_integral(
errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

WJR_NODISCARD WJR_INLINE_CONSTEXPR int toi(
errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

WJR_NODISCARD WJR_INLINE_CONSTEXPR long tol(
errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

WJR_NODISCARD WJR_INLINE_CONSTEXPR long long toll(
errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned int toui(
errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned long toul(
errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned long long toull(
errc* err = nullptr, size_type* pos = nullptr, int base = 10) const;

};

using ascii_view = basic_string_view<char, ascii_traits<>>;

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isalnum(char ch) {
return func_type::isalnum(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isalpha(char ch) {
return func_type::isalpha(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::islower(char ch) {
return func_type::islower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isupper(char ch) {
return func_type::isupper(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isdigit(char ch) {
return func_type::isdigit(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isxdigit(char ch) {
return func_type::isxdigit(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR bool basic_string_view<char, ascii_traits<Traits>>::isspace(char ch) {
return func_type::isspace(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR char basic_string_view<char, ascii_traits<Traits>>::tolower(char ch) {
return func_type::tolower(ch);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR char basic_string_view<char, ascii_traits<Traits>>::toupper(char ch) {
return func_type::toupper(ch);
}

template<typename Traits>
WJR_INLINE_CONSTEXPR basic_string_view<char, ascii_traits<Traits>>
basic_string_view<char, ascii_traits<Traits>>::ltrim() const {
const char* s = begin();
const char* e = end();
s = func_type::skipw(s, e);
return basic_string_view(s, e - s);
}

template<typename Traits>
WJR_INLINE_CONSTEXPR basic_string_view<char, ascii_traits<Traits>>
basic_string_view<char, ascii_traits<Traits>>::rtrim() const {
const char* s = begin();
const char* e = end();
e = func_type::rskipw(s, e);
return basic_string_view(s, e - s);
}

template<typename Traits>
WJR_INLINE_CONSTEXPR basic_string_view<char, ascii_traits<Traits>>
basic_string_view<char, ascii_traits<Traits>>::trim() const {
return ltrim().rtrim();
}

template<typename Traits>
template<typename T>
WJR_NODISCARD WJR_INLINE_CONSTEXPR T basic_string_view<char, ascii_traits<Traits>>::to_integral(
errc* err, size_type* pos, int base) const {
using namespace enum_ops;

constexpr auto _Flags =
flags::ALLOW_PREFIX
| flags::ALLOW_SIGN
| flags::ALLOW_LEADING_SPACE
| flags::ALLOW_LEADING_ZEROS;

errc cc = errc::ok;
const char* end_ptr = nullptr;
T ret = func_type::to_integral<T>(
std::integral_constant<flags, _Flags>(),
begin(), end(), cc, end_ptr, base);

if (err != nullptr) {
*err = cc;
}

if (pos != nullptr) {
if (cc == errc::noconv) {
*pos = static_cast<size_type>(0);
}
else {
*pos = static_cast<size_type>(end_ptr - begin());
}
}

return ret;
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR int basic_string_view<char, ascii_traits<Traits>>::toi(
errc* err, size_type* pos, int base) const {
return to_integral<int>(err, pos, base);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR long basic_string_view<char, ascii_traits<Traits>>::tol(
errc* err, size_type* pos, int base) const {
return to_integral<long>(err, pos, base);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR long long basic_string_view<char, ascii_traits<Traits>>::toll(
errc* err, size_type* pos, int base) const {
return to_integral<long long>(err, pos, base);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned int basic_string_view<char, ascii_traits<Traits>>::toui(
errc* err, size_type* pos, int base) const {
return to_integral<unsigned int>(err, pos, base);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned long basic_string_view<char, ascii_traits<Traits>>::toul(
errc* err, size_type* pos, int base) const {
return to_integral<unsigned long>(err, pos, base);
}

template<typename Traits>
WJR_NODISCARD WJR_INLINE_CONSTEXPR unsigned long long basic_string_view<char, ascii_traits<Traits>>::toull(
errc* err, size_type* pos, int base) const {
return to_integral<unsigned long long>(err, pos, base);
}

namespace utf8 {
enum __char_code {
__invalid_byte = 0x00,
__one_byte = 0x01,
__two_byte = 0x02,
__three_byte = 0x03,
__four_byte = 0x04,
__tail_byte = 0x08,
};

static constexpr int __char_code_table[256] = {

__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,
__one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte, __one_byte,

__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,
__tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte, __tail_byte,

__two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte,
__two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte,
__two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte,
__two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte, __two_byte,

__three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte,
__three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte, __three_byte,

__four_byte, __four_byte, __four_byte, __four_byte, __four_byte, __four_byte, __four_byte, __four_byte,

__invalid_byte, __invalid_byte, __invalid_byte, __invalid_byte,
__invalid_byte, __invalid_byte, __invalid_byte, __invalid_byte,
};

WJR_NODISCARD WJR_INLINE_CONSTEXPR20 int get_code(uint8_t c) {
return __char_code_table[c];
}

// return - 1 if the length is insufficient
// return - 2 if illegal
template<typename iter1, typename iter2>
WJR_NODISCARD WJR_CONSTEXPR20 uint32_t decode(iter1& _First, iter2 _Last) {
if (is_unlikely(_First == _Last)) {
return -1;
}
uint8_t c;
c = *_First;
++_First;
int len = get_code(c) & 0x07;
if (!len) {
return -2;
}

switch (len) {
case 4: {
uint8_t t0, t1, t2;

if (is_unlikely(_First == _Last)) {
return -1;
}
t0 = *_First;
++_First;
if (get_code(t0) != __tail_byte) {
return -2;
}

if (is_unlikely(_First == _Last)) {
return -1;
}
t1 = *_First;
++_First;
if (get_code(t1) != __tail_byte) {
return -2;
}

if (is_unlikely(_First == _Last)) {
return -1;
}
t2 = *_First;
++_First;
if (get_code(t2) != __tail_byte) {
return -2;
}

return ((c & 0x07) << 18)
| ((t0 & 0x3f) << 12)
| ((t1 & 0x3f) << 6)
| (t2 & 0x3f);
}
case 3: {
uint8_t t0, t1;

if (is_unlikely(_First == _Last)) {
return -1;
}
t0 = *_First;
++_First;
if (get_code(t0) != __tail_byte) {
return -2;
}

if (is_unlikely(_First == _Last)) {
return -1;
}
t1 = *_First;
++_First;
if (get_code(t1) != __tail_byte) {
return -2;
}

return ((c & 0x0f) << 12)
| ((t0 & 0x3f) << 6)
| (t1 & 0x3f);
}
case 2: {
uint8_t t0;

if (is_unlikely(_First == _Last)) {
return -1;
}
t0 = *_First;
++_First;
if (get_code(t0) != __tail_byte) {
return -2;
}

return ((c & 0x1f) << 6)
| (t0 & 0x3f);
}
case 1: {
return c;
}
default: {
WJR_UNREACHABLE;
}
}

}

template<typename iter1, typename iter2>
WJR_NODISCARD WJR_CONSTEXPR20 uint32_t rdecode(iter1& _First, iter2 _Last) {
if (is_unlikely(_First == _Last)) {
return -1;
}
uint8_t c;
c = *_First;
++_First;
int len = get_code(c) & 0x07;
if (!len) {
return -2;
}

switch (len) {
case 4: {
uint8_t t0, t1, t2;

if (is_unlikely(_First == _Last)) {
return -1;
}
t0 = *_First;
++_First;
if (get_code(t0) != __tail_byte) {
return -2;
}

if (is_unlikely(_First == _Last)) {
return -1;
}
t1 = *_First;
++_First;
if (get_code(t1) != __tail_byte) {
return -2;
}

if (is_unlikely(_First == _Last)) {
return -1;
}
t2 = *_First;
++_First;
if (get_code(t2) != __tail_byte) {
return -2;
}

return ((c & 0x07) << 18)
| ((t0 & 0x3f) << 12)
| ((t1 & 0x3f) << 6)
| (t2 & 0x3f);
}
case 3: {
uint8_t t0, t1;

if (is_unlikely(_First == _Last)) {
return -1;
}
t0 = *_First;
++_First;
if (get_code(t0) != __tail_byte) {
return -2;
}

if (is_unlikely(_First == _Last)) {
return -1;
}
t1 = *_First;
++_First;
if (get_code(t1) != __tail_byte) {
return -2;
}

return ((c & 0x0f) << 12)
| ((t0 & 0x3f) << 6)
| (t1 & 0x3f);
}
case 2: {
uint8_t t0;

if (is_unlikely(_First == _Last)) {
return -1;
}
t0 = *_First;
++_First;
if (get_code(t0) != __tail_byte) {
return -2;
}

return ((c & 0x1f) << 6)
| (t0 & 0x3f);
}
case 1: {
return c;
}
default: {
WJR_UNREACHABLE;
}
}

}

// return - 2 if illegal
// return 0 if legal
template<typename iter>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 uint32_t encode(uint32_t c, iter& _First) {
if (c > 0x10ffff)return -2;
if (c < 0x80) {
*_First = static_cast<uint8_t>(c);
++_First;
return 0;
}
if (c < 0x800) {
*_First = static_cast<uint8_t>(0xc0 | (c >> 6));
++_First;
*_First = static_cast<uint8_t>(0x80 | (c & 0x3f));
++_First;
return 0;
}
if (c < 0x10000) {
*_First = static_cast<uint8_t>(0xe0 | (c >> 12));
++_First;
*_First = static_cast<uint8_t>(0x80 | ((c >> 6) & 0x3f));
++_First;
*_First = static_cast<uint8_t>(0x80 | (c & 0x3f));
++_First;
return 0;
}
*_First = static_cast<uint8_t>(0xf0 | (c >> 18));
++_First;
*_First = static_cast<uint8_t>(0x80 | ((c >> 12) & 0x3f));
++_First;
*_First = static_cast<uint8_t>(0x80 | ((c >> 6) & 0x3f));
++_First;
*_First = static_cast<uint8_t>(0x80 | (c & 0x3f));
++_First;
return 0;
}

}

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

WJR_NODISCARD WJR_CONSTEXPR20 _Ty* allocate(const size_t _Count) {
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

_WJR_END

#endif // __WJR_ALLOCATOR_H
#include <stdio.h>

_WJR_BEGIN
#if defined(WJR_TEST_ALLOCATOR)
__test_allocator::~__test_allocator() {
if (_Count != 0) {
printf("memory leak: %lld bytes", _Count);
}
}
__test_allocator __test_allocator_instance;
#endif
_WJR_END
#pragma once
#ifndef __WJR_NETWORK_THREAD_POOL_H
#define __WJR_NETWORK_THREAD_POOL_H

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <chrono>

#pragma once
#ifndef __WJR_CIRCULAR_BUFFER_H
#define __WJR_CIRCULAR_BUFFER_H


_WJR_BEGIN

// TODO

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

// algorithm overload

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_copy(
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_copy(
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_copy(Alloc& al,
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_copy(Alloc& al,
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_move(
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_move(
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest);

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_move(Alloc& al,
circular_buffer_const_iterator<T> _First, circular_buffer_const_iterator<T> _Last, _Iter _Dest);

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_move(Alloc& al,
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
if (is_likely(_Mydata._Mysize != _Mydata._Mycapacity)) {
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
if (is_likely(_Mydata._Mysize != _Mydata._Mycapacity)) {
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

// algorithm

_WJR_BEGIN

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_copy(
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
WJR_CONSTEXPR20 _Iter uninitialized_copy(
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
return wjr::uninitialized_copy(_First.base(), _Last.base(), _Dest);
}

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_copy(Alloc& al,
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
WJR_CONSTEXPR20 _Iter uninitialized_copy(Alloc& al,
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
return wjr::uninitialized_copy(al, _First.base(), _Last.base(), _Dest);
}

template<typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_move(
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
WJR_CONSTEXPR20 _Iter uninitialized_move(
circular_buffer_iterator<T> _First, circular_buffer_iterator<T> _Last, _Iter _Dest) {
return wjr::uninitialized_move(_First.base(), _Last.base(), _Dest);
}

template<typename Alloc, typename T, typename _Iter>
WJR_CONSTEXPR20 _Iter uninitialized_move(Alloc& al,
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
WJR_CONSTEXPR20 _Iter uninitialized_move(Alloc& al,
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

// testing, don't use this
template<typename Derived>
class explicit_operator_complement {
public:
static_assert(has_global_binary_operator_equal_to_v<const Derived&, const Derived&>
&& has_global_binary_operator_less_v<const Derived&, const Derived&>,
"must have operator== and operator <");
Derived& derived() { return static_cast<Derived&>(*this); }
const Derived& derived() const { return static_cast<const Derived&>(*this); }
private:
WJR_CONSTEXPR20 explicit_operator_complement() {}
friend Derived;
};

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator==(
const explicit_operator_complement<Derived>& lhs,
const explicit_operator_complement<Derived>& rhs) {
return (lhs.derived() == rhs.derived());
}

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator!=(
const explicit_operator_complement<Derived>& lhs,
const explicit_operator_complement<Derived>& rhs) {
return !(lhs.derived() == rhs.derived());
}

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator<(
const explicit_operator_complement<Derived>& lhs,
const explicit_operator_complement<Derived>& rhs) {
return (lhs.derived() < rhs.derived());
}

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator<=(
const explicit_operator_complement<Derived>& lhs,
const explicit_operator_complement<Derived>& rhs) {
return !(rhs.derived() < lhs.derived());
}

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator>(
const explicit_operator_complement<Derived>& lhs,
const explicit_operator_complement<Derived>& rhs) {
return (rhs.derived() < lhs.derived());
}

template<typename Derived>
WJR_NODISCARD WJR_INLINE_CONSTEXPR20 bool operator>=(
const explicit_operator_complement<Derived>& lhs,
const explicit_operator_complement<Derived>& rhs) {
return !(lhs.derived() < rhs.derived());
}

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

// if is pause, only flush active tasks
// else flush all tasks
// then destroy threads
void shutdown();
// if is pause, only flush active tasks
// else flush all tasks
// then destroy threads
// return tasks that not done
// notice that it will return nothing if not in a paused state
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
// Not used frequently, so average consumption is reduced through mutual exclusion protection
alignas(8) bool m_pause = false;
alignas(8) std::atomic<size_t> m_real_tasks = 0;
// padding
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
if (is_unlikely(!m_valid)) {
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
// can't flush multiple times
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

if (is_likely(m_valid)) {
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
#ifndef __WJR_TP_LIST_H
#define __WJR_TP_LIST_H

#include <type_traits>
#include <variant>


// comply with STD naming standards as much as possible
// try to place the containers that need to be processed in the first parameter as much as possible
// when it is possible to operate on multiple containers,
// the operation should be placed before the container disaster

// Try to make each function only work on one container as much as possible.
// If you need to work on multiple containers, you can usually use tp_zip or tp_product,
// the former compresses the same bit of multiple containers into a container,
// and then throws each bit of container into container C (user-defined),
// while the latter generates a Cartesian product
// for example ,
// tp_zip<tp_list, tp_list<int, double, float>, tp_list<int, float, double>>
// will generate tp_list<tp_list<int, int>, tp_list<double, float>, tp_list<float, double>>
// tp_product<tp_list, tp_list<int, double>, tp_list<int, float, double>>
// will generate tp_list<tp_list<int, int>, tp_list<int, float>, tp_list<int, double>,
// tp_list<double, int>, tp_list<double, float>, tp_list<double, double>>

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

// L<std::integral_constant<T, I>...>
template<typename T, T... I>
using tp_list_c_t = typename tp_list_c<T, I...>::type;

template<typename T>
struct tp_is_list : std::false_type {};

template<typename...Args>
struct tp_is_list<tp_list<Args...>> : std::true_type {};

// check if is tp_list
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

// get size of L<Args...>
template<typename T>
inline constexpr size_t tp_size_v = tp_size<T>::value;

template<size_t idx>
using tp_size_t = tp_c<size_t, idx>;

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

// use std::enable_if_t to defer the instantiation of F<Args...>
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

// F1<F2<Args...>>
template<template<typename...>typename F1, template<typename...>typename F2>
struct tp_bind_fn {
template<typename...Args>
using fn = tp_defer_t<F1, tp_defer_t<F2, Args...>>;
};

// make F can be used as fn
template<template<typename...>typename F>
struct tp_make_fn {
template<typename...Args>
using fn = tp_defer_t<F, Args...>;
};

// std::negation<F<Args...>>
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

// f(L1<Args1...>, L2<Args2...>) -> L1<Args2...>
template<typename T, typename U>
using tp_assign_t = typename tp_assign<T, U>::type;

template<typename T>
struct tp_clear;

template<template<typename...>typename T, typename...Args>
struct tp_clear<T<Args...>> {
using type = T<>;
};

// f(L<Args...>) -> L<>
template<typename T>
using tp_clear_t = typename tp_clear<T>::type;

template<typename T, typename...Args>
struct tp_push_front;

template<template<typename...>typename C, typename...Args1, typename...Args2>
struct tp_push_front<C<Args1...>, Args2...> {
using type = C<Args2..., Args1...>;
};

// f(L<Args1...>, Args2...) -> L<Args1..., Args2...)
template<typename T, typename...Args>
using tp_push_front_t = typename tp_push_front<T, Args...>::type;

template<typename T, typename...Args>
struct tp_push_back;

template<template<typename...>typename C, typename...Args1, typename...Args2>
struct tp_push_back<C<Args1...>, Args2...> {
using type = C<Args1..., Args2...>;
};

// f(L<Args1...>, Args2...) -> L<Args2..., Args1...)
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

// f(L1<Args1...>, L2<Args2...>) -> L2<Args1...>
template<typename T, template<typename...>typename U>
using tp_rename_t = typename tp_rename<T, U>::type;

template<typename T, size_t I, size_t N>
struct tp_cut;

template<template<typename...>typename C, typename...Args, size_t I, size_t N>
struct tp_cut<C<Args...>, I, N> {
static_assert(N <= sizeof...(Args) && I <= (sizeof...(Args) - N), "tp_cut: invalid index");
using type = tp_rename_t<typename __tp_cut_helper<void, I, N, Args...>::type, C>;
};

// f(L<Args...>, I, N) -> L<Args(0 ~ max(0, I - 1)), Args(I + 1 ~ N - 1)>
template<typename T, size_t I, size_t N>
using tp_cut_t = typename tp_cut<T, I, N>::type;

template<typename T>
struct tp_pop_front : tp_cut<T, 1, tp_size_v<T> - 1> {};

// f(L<T, Args...>) -> L<Args...>
template<typename T>
using tp_pop_front_t = typename tp_pop_front<T>::type;

template<typename T>
struct tp_pop_back : tp_cut<T, 0, tp_size_v<T> -1> {};

// f(L<Args..., T>) -> L<Args...>
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

// f(L<Args...>, index) - > Args(index)
template<typename T, size_t index>
using tp_at_t = typename tp_at<T, index>::type;

template<typename T>
struct tp_front {
using type = tp_at_t<T, 0>;
};

// tp_at_t(T, 0)
template<typename T>
using tp_front_t = typename tp_front<T>::type;

template<typename T>
struct tp_back {
using type = tp_at_t<T, tp_size_v<T> -1>;
};

// tp_at_t(T, tp_size_v<T> - 1)
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

// f(L1<Args...>, L2<Args2...>, ... Ln<Argsn...>) -> L1<Args..., Args2..., Argsn...>
template<typename...Args>
using tp_concat_t = typename tp_concat<Args...>::type;

template<typename T, size_t idx, typename U>
struct tp_replace_at {
using type = tp_concat_t<tp_push_back_t<tp_cut_t<T, 0, idx - 1>, U>, tp_cut_t<T, idx, tp_size_v<T> -idx>>;
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

// f(L<Args...>) -> f(Args...)
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

// f(L<Args...>, Fn) -> L<Fn(Args)...>
// use with apply, bind, bind_front, bind_back...
// for example:
// tp_transform_f<tp_bind_front<tp_apply_f, tp_bind_front<std::is_same>>,
// tp_list<tp_list<int, float>, tp_list<float, float>, tp_list<int, double>>>
// -> tp_list<std::is_same<int, float>, std::is_same<float, float>, std::is_same<int, double>>
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
using type = tp_list<T>;
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

// f(L<Args...>, P, U) -> L<if P(Args)::value then U else Args...>
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

// f(L<Args...>, P) -> count(P(Args)::value)
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

// f(L<Args...>, P) -> L<if P(Args)::value then L<> else L<Args>...>
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
using type = tp_concat<T, typename tp_repeat<T, N - 1>::type>;
};

template<typename T>
struct tp_repeat<T, 0> {
using type = T;
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
using type = tp_left_fold_f<C, tp_clear_t<C>, tp_bind<tp_conditional_t, tp_bind_front<P>, tp_arg<0>,
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

class tp_fn {
public:
template<typename C, typename F>
constexpr static F for_each(F&& f) {
return __for_each_helper(std::forward<F>(f), tp_rename_t<C, tp_list>());
}

private:
template<typename F, typename...Args>
constexpr static F __for_each_helper(F&& f, tp_list<Args...>) {
int dummy[] = { (f(Args()), 0)... };
return std::forward<F>(f);
}
};

inline constexpr tp_fn tp;

_WJR_END

#endif // __WJR_TP_LIST_H
