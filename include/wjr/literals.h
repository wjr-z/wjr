#include <wjr/macro.h>

_WJR_LITERALS_BEGIN

constexpr unsigned long long operator"" _KiB(unsigned long long n) {
	return n * 1024;
}

constexpr unsigned long long operator"" _MiB(unsigned long long n) {
	return n * 1024 * 1024;
}

constexpr unsigned long long operator"" _GiB(unsigned long long n) {
	return n * 1024 * 1024 * 1024;
}

constexpr unsigned long long operator""_KB(unsigned long long n) {
	return n * 1000;
}

constexpr unsigned long long operator""_MB(unsigned long long n) {
	return n * 1000 * 1000;
}

constexpr unsigned long long operator""_GB(unsigned long long n) {
	return n * 1000 * 1000 * 1000;
}

_WJR_LITERALS_END