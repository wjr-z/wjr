#include <wjr/cpuinfo.h>
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

const bool __is_little_endian = []() {
	union C{
		uint32_t a;
		uint8_t b;
	};
	C x;
	x.a = 0x01020304;
	bool f = x.b == 0x04;
#if defined(WJR_BIG_ENDIAN)
	if (!f) {
		fprintf(stderr, "WJR_BIG_ENDIAN is defined, but the current platform is little endian.");
	}
#elif defined(WJR_LITTLE_ENDIAN)
	if (!f) {
		fprintf(stderr, "WJR_LITTLE_ENDIAN is defined, but the current platform is big endian.");
	}
#endif
	return f;
}();

_WJR_END