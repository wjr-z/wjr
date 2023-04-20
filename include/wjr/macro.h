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

// pure attribute
#if WJR_HAS_ATTRIBUTE(pure)
#define WJR_PURE __attribute__((pure))
#else
#define WJR_PURE
#endif 

// const attribute
#if WJR_HAS_ATTRIBUTE(const)
#define WJR_CONST __attribute__((const))
#else
#define WJR_CONST
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