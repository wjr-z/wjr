#ifndef WJR_PREPROCESSOR_COMPILER_HAS_HPP__
#define WJR_PREPROCESSOR_COMPILER_HAS_HPP__

#include <wjr/preprocessor/compiler/arch.hpp>
#include <wjr/preprocessor/compiler/compiler.hpp>
#include <wjr/preprocessor/details/basic.hpp>
#include <wjr/preprocessor/logical/basic.hpp>


#define WJR_HAS_DEF_VAR(var) WJR_PP_MAP_DEF(var)
#define WJR_HAS_DEF WJR_HAS_DEF_VAR(1)

#define WJR_HAS_FIND(MAP, KEY) WJR_HAS_FIND_I(WJR_PP_MAP_FIND(MAP, KEY))
#define WJR_HAS_FIND_I(VAL) WJR_PP_BOOL_IF(WJR_PP_IS_NULLPTR(VAL), 0, VAL)

// Currently only has_builtin, has_attribute, has_feature are supported.
#define WJR_HAS_BUILTIN_FIND(KEY) WJR_HAS_FIND(WJR_HAS_BUILTIN_, KEY)
#define WJR_HAS_ATTRIBUTE_FIND(KEY) WJR_HAS_FIND(WJR_HAS_ATTRIBUTE_, KEY)
#define WJR_HAS_FEATURE_FIND(KEY) WJR_HAS_FIND(WJR_HAS_FEATURE_, KEY)
#define WJR_HAS_SIMD_FIND(KEY) WJR_HAS_FIND(WJR_HAS_SIMD_, KEY)

//

#if (defined(WJR_COMPILER_GCC) && WJR_HAS_GCC(10, 1, 0)) ||                              \
    (defined(WJR_COMPILER_CLANG) && WJR_HAS_CLANG(10, 0, 0)) ||                          \
    (!defined(WJR_COMPILER_GCC) && !defined(WJR_COMPILER_CLANG) &&                       \
     defined(__has_builtin))
#define WJR_HAS_BUILTIN(x) WJR_PP_BOOL_IF(WJR_HAS_BUILTIN_FIND(x), 1, __has_builtin(x))
#else
#define WJR_HAS_BUILTIN(x) WJR_HAS_BUILTIN_FIND(x)
#endif

#if defined(__has_include)
#define WJR_HAS_INCLUDE(x) __has_include(x)
#else
#define WJR_HAS_INCLUDE(x) 0
#endif // __has_includeF

#if defined(__has_attribute)
#define WJR_HAS_ATTRIBUTE(x)                                                             \
    WJR_PP_BOOL_IF(WJR_HAS_ATTRIBUTE_FIND(x), 1, __has_attribute(x))
#else
#define WJR_HAS_ATTRIBUTE(x) WJR_HAS_ATTRIBUTE_FIND(x)
#endif

#if defined(__has_cpp_attribute)
#define WJR_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define WJR_HAS_CPP_ATTRIBUTE(x) 0
#endif

#define WJR_HAS_FEATURE(x) WJR_HAS_FEATURE_FIND(x)

#define WJR_HAS_SIMD(x) WJR_HAS_SIMD_FIND(x)

// WJR_HAS_BUILTIN

#if WJR_HAS_GCC(7, 1, 0) || WJR_HAS_CLANG(5, 0, 0)
#define WJR_HAS_BUILTIN___builtin_unreachable WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_expect WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_constant_p WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_clz WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_ctz WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_popcount WJR_HAS_DEF
#endif

#if WJR_HAS_GCC(9, 1, 0) || WJR_HAS_CLANG(9, 0, 0)
#define WJR_HAS_BUILTIN___builtin_is_constant_evaluated WJR_HAS_DEF
#endif

#if WJR_HAS_CLANG(5, 0, 0)
#define WJR_HAS_BUILTIN___builtin_addc WJR_HAS_DEF
#define WJR_HAS_BUILTIN___builtin_subc WJR_HAS_DEF
#endif

#if WJR_HAS_GCC(9, 1, 0) || WJR_HAS_CLANG(11, 0, 0)
#define WJR_HAS_BUILTIN___builtin_expect_with_probability WJR_HAS_DEF
#endif

// WJR_HAS_FEATURE

#if WJR_HAS_GCC(7, 1, 0) || WJR_HAS_CLANG(5, 0, 0)
#define WJR_HAS_FEATURE_PRAGMA_UNROLL WJR_HAS_DEF
#endif

#if defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_MSVC)
#define WJR_HAS_FEATURE_INLINE_ASM WJR_HAS_DEF
#endif

#if defined(__SIZEOF_INT128__)
#define WJR_HAS_FEATURE_INT128 WJR_HAS_DEF
#if !(defined(__clang__) && defined(LIBDIVIDE_VC))
#define WJR_HAS_FEATURE_INT128_DIV WJR_HAS_DEF
#endif
#endif

#define WJR_HAS_SIMD_SIMD WJR_HAS_DEF

#if WJR_HAS_GCC(11, 1, 0) || WJR_HAS_CLANG(5, 0, 0)
#define WJR_HAS_ATTRIBUTE_FORCEINLINE_LAMBDA WJR_HAS_DEF
#endif

#if defined(_MSC_VER)
/* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
/* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
/* GCC-compatible compiler, targeting ARM with NEON */
#include <arm_neon.h>
#else
#undef WJR_HAS_SIMD_SIMD
#endif

#if defined(__AVX512VL__)
#define WJR_HAS_SIMD_AVX512VL WJR_HAS_DEF
#endif

#if defined(__AVX512BW__)
#define WJR_HAS_SIMD_AVX512BW WJR_HAS_DEF
#endif

#if defined(__AVX512DQ__)
#define WJR_HAS_SIMD_AVX512DQ WJR_HAS_DEF
#endif

#if defined(__AVX512F__) ||                                                              \
    (WJR_HAS_SIMD(AVX512VL) && WJR_HAS_SIMD(AVX512BW) && WJR_HAS_SIMD(AVX512DQ))
#define WJR_HAS_SIMD_AVX512F WJR_HAS_DEF
#endif

#if defined(__AVX512__) ||                                                               \
    (WJR_HAS_SIMD(AVX512F) && WJR_HAS_SIMD(AVX512BW) && WJR_HAS_SIMD(AVX512DQ))
#define WJR_HAS_SIMD_AVX512 WJR_HAS_DEF
#endif

#if defined(__AVX2__) || (WJR_HAS_SIMD(AVX512) || WJR_HAS_SIMD(AVX512F))
#define WJR_HAS_SIMD_AVX2 WJR_HAS_DEF
#endif

#if defined(__AVX__) || WJR_HAS_SIMD(AVX2)
#define WJR_HAS_SIMD_AVX WJR_HAS_DEF
#endif

#if defined(__SSE4_2__) || WJR_HAS_SIMD(AVX)
#define WJR_HAS_SIMD_SSE4_2 WJR_HAS_DEF
#endif

#if defined(__SSE4_1__) || WJR_HAS_SIMD(SSE4_2)
#define WJR_HAS_SIMD_SSE4_1 WJR_HAS_DEF
#endif

#if defined(__SSSE3__) || WJR_HAS_SIMD(SSE4_1)
#define WJR_HAS_SIMD_SSSE3 WJR_HAS_DEF
#endif

#if defined(__SSE3__) || WJR_HAS_SIMD(SSSE3)
#define WJR_HAS_SIMD_SSE3 WJR_HAS_DEF
#endif

#if defined(__SSE2__) || WJR_HAS_SIMD(SSE3) || _M_IX86_FP >= 2 ||                        \
    (defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_X64)))
#define WJR_HAS_SIMD_SSE2 WJR_HAS_DEF
#endif

#if defined(__SSE__) || WJR_HAS_SIMD(SSE2) || _M_IX86_FP >= 1
#define WJR_HAS_SIMD_SSE WJR_HAS_DEF
#endif

#if defined(__MMX__) || WJR_HAS_SIMD(SSE)
#define WJR_HAS_SIMD_MMX WJR_HAS_DEF
#endif

#if defined(__XOP__)
#define WJR_HAS_SIMD_XOP WJR_HAS_DEF
#endif

#if defined(__POPCNT__)
#define WJR_HAS_SIMD_POPCNT WJR_HAS_DEF
#endif

#endif // WJR_PREPROCESSOR_COMPILER_HAS_HPP__