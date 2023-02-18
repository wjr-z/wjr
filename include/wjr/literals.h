#pragma once

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

_WJR_LITERALS_END