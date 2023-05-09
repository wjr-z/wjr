#ifndef __WJR_PREPROCESSOR_WJR_H
#define __WJR_PREPROCESSOR_WJR_H

#include <wjr/preprocessor/compiler.h>
#include <wjr/preprocessor/simd.h>

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

// judge if i can use inline asm
#if defined(WJR_X86) && (defined(WJR_COMPILER_GCC) || defined(WJR_COMPILER_CLANG))
#	define WJR_INLINE_ASM
#	if defined(WJR_COMPILER_GCC)
#		define WJR_BETTER_INLINE_ASM
#	endif // WJR_COMPILER_GCC
#endif

#if defined(WJR_INLINE_ASM) || defined(WJR_COMPILER_MSVC)
#define _WJR_FAST_REP
#endif

#define _WJR_CPUINFO

#if defined(NWJR_CPUINFO)
#undef _WJR_CPUINFO
#endif // NWJR_CPUINFO

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

#endif // __WJR_PREPROCESSOR_WJR_H