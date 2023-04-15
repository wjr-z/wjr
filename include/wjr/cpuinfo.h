#pragma once
#ifndef __WJR_CPUINFO_H
#define __WJR_CPUINFO_H

#include <wjr/macro.h>
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