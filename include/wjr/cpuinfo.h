#pragma once
#ifndef __WJR_CPUINFO_H
#define __WJR_CPUINFO_H

#include <wjr/macro.h>
#include <cpuinfo_x86.h>

_WJR_BEGIN

/*
static const cpu_features::X86Info cpuinfo = cpu_features::GetX86Info();
static const cpu_features::CacheInfo cacheinfo = cpu_features::GetX86CacheInfo();
static const cpu_features::X86Microarchitecture microarchitecture = cpu_features::GetX86Microarchitecture(&cpuinfo);
static const size_t max_cache_size = []() {
	size_t max_size = 0;
	for (int i = 0; i < cacheinfo.size; ++i) {
		if (cacheinfo.levels[i].cache_type != cpu_features::CacheType::CPU_FEATURE_CACHE_NULL) {
			max_size = max_size < cacheinfo.levels[i].cache_size ?
				cacheinfo.levels[i].cache_size : max_size;
		}
	}
	return max_size;
}();
static const bool __is_intel = []() {
	return microarchitecture >= cpu_features::INTEL_80486 && microarchitecture <= cpu_features::INTEL_NETBURST;
}();
static const bool __is_amd = []() {
	return microarchitecture >= cpu_features::AMD_HAMMER && microarchitecture <= cpu_features::AMD_ZEN3;
}();
*/

extern const cpu_features::X86Info cpuinfo;
extern const cpu_features::CacheInfo cacheinfo;
extern const cpu_features::X86Microarchitecture microarchitecture;
extern const size_t max_cache_size;
extern const bool __is_intel;
extern const bool __is_amd;

inline bool is_sse() {
	return CPU_FEATURES_COMPILED_X86_SSE || cpuinfo.features.sse;
}
inline bool is_sse2() {
	return CPU_FEATURES_COMPILED_X86_SSE2 || cpuinfo.features.sse2;
}
inline bool is_sse3() {
	return CPU_FEATURES_COMPILED_X86_SSE3 || cpuinfo.features.sse3;
}
inline bool is_ssse3() {
	return CPU_FEATURES_COMPILED_X86_SSSE3 || cpuinfo.features.ssse3;
}
inline bool is_sse4_1() {
	return CPU_FEATURES_COMPILED_X86_SSE4_1 || cpuinfo.features.sse4_1;
}
inline bool is_sse4_2() {
	return CPU_FEATURES_COMPILED_X86_SSE4_2 || cpuinfo.features.sse4_2;
}
inline bool is_avx() {
	return CPU_FEATURES_COMPILED_X86_AVX || cpuinfo.features.avx;
}
inline bool is_avx2() {
	return CPU_FEATURES_COMPILED_X86_AVX2 || cpuinfo.features.avx2;
}

inline bool is_popcnt() {
	return cpuinfo.features.popcnt;
}

inline bool is_intel() { return __is_intel; }
inline bool is_amd() { return __is_amd; }

inline bool is_enhanced_rep() {
#if defined(_WJR_ENHANCED_REP)
	return is_intel();
#else
	return false;
#endif
}

_WJR_END

#endif // __WJR_CPUINFO_H