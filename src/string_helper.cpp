#include "../include/string_helper.h"
#include <iostream>
#include <math.h>
#include <cassert>

namespace wjr {

	inline double power_of_10(int index) {
		struct pw_cache {
			double pw10[310], npw10[310];
			pw_cache() {
				pw10[0] = npw10[0] = 1.0;
				for (int i = 1; i <= 309; ++i) {
					pw10[i] = pw10[i - 1] * 10.0;
					npw10[i] = 1.0 / pw10[i];
				}
			}
		};
		static pw_cache _cache;
		assert(index > -310 && index < 310);
		return (index >= 0) ? _cache.pw10[index] : _cache.npw10[-index];
	}

	double read_double(const char* s,const char*e,const char*& next) {
		auto* ptr = (const uint8_t*)s;
		auto* first = ptr;
		bool neg = false;

		for(;!isdigit_or_sign(*ptr);++ptr);
		switch (*ptr) {
		case '-':neg = true;++ptr;break;
		case '+':++ptr;break;
		}

		unsigned long long v = 0;
		int num = 0;

		for (; qisdigit(*ptr) && num < 18; ++ptr, ++num) {
			v = v * 10 + (*ptr - '0');
		}

		int pw10 = 0;
		for (; qisdigit(*ptr); ++ptr, ++pw10) ;

		if (*ptr == '.') {
			++ptr;
			for (; qisdigit(*ptr) && num < 18; ++ptr, ++num, --pw10) {
				v = v * 10 + (*ptr - '0');
			}
			for(;qisdigit(*ptr);++ptr);
		}

		double val = v * power_of_10(pw10);
		if ((*ptr == 'e') || (*ptr == 'E')) {
			++ptr;
			int pw = String_traits_helper<uint8_t>::unsafe_first_to_int(ptr,(const uint8_t*)e,ptr);
			val *= power_of_10(pw);
		}
		if(neg) val = -val;
		next = (const char*)ptr;
		return val;
	}

}
