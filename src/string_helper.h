#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include "String.h"

namespace wjr {

	template<typename T>
	size_t read_integer(const char* str,T& val) {
		auto* ptr = (const uint8_t*)str;
		auto* s = ptr;
		val = 0;
		bool neg = false;
		for (; !isdigit_or_sign(*ptr); ++ptr);
		switch (*ptr) {
		case '-':neg = true; ++ptr; break;
		case '+':++ptr; break;
		}
		val = *ptr - '0';
		++ptr;
		for (; isdigit(*ptr); ++ptr) {
			val = val * 10 + (*ptr - '0');
		}
		if(!neg) val = -val;
		return ptr - s;
	}

	size_t read_uint(const char*str,unsigned int&);
	size_t read_int(const char*str,int&);
	size_t read_ull(const char*str,unsigned long long&);
	size_t read_ll(const char*str,long long&);

	inline double power_of_10(int index);
	size_t read_double(const char*str,double&);

}

#endif
