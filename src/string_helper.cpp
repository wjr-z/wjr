#include "string_helper.h"
#include <iostream>
#include <math.h>
#include <cassert>

namespace wjr {

	std::tuple<size_t, size_t> split(const char* ptr, const char cl, const char cr, const size_t _MaxCount) {
		const void* p = memchr(ptr, cl, _MaxCount);
		size_t pos1 = !p ? -1 : (const char*)p - ptr;
		size_t pos2 = -1;
		if (pos1 != _MaxCount) {
			const void* q = memchr(ptr + pos1 + 1, cr, _MaxCount - pos1);
			pos2 = !q ? -1 : (const char*)q - ptr;
		}
		return std::make_tuple(pos1, pos2);
	}

	size_t find_first_pos(const char* ptr, const char ch, const size_t _MaxCount) {
		const void* pos = memchr(ptr, ch, _MaxCount);
		return !pos ? -1 : (const char*)pos - ptr;
	}

	size_t get_number_of_char(const char* str, const size_t length, const char ch) {
		const char* s = str;
		const char* e = s + length;
		size_t cnt = 0;
		do {
			s = (const char*)memchr(s, ch, e - s);
			if (s == nullptr)break;
			++s;
			++cnt;
		} while (true);
		return cnt;
	}

	size_t get_number_of_char(const char* str, const char ch) {
		return get_number_of_char(str, strlen(str), ch);
	}

	size_t get_number_of_char(const std::string& str, const char ch) {
		return get_number_of_char(str.c_str(), str.length(), ch);
	}

	inline bool isdigit_or_sign(uint8_t ch) {
		if(isdigit(ch))return true;
		switch (ch) {
		case '+':return true;
		case '-':return true;
		default:return false;
		}
	}

	size_t read_uint(const char* str,unsigned int&val) {
		return read_integer<unsigned int>(str,val);
	}

	size_t read_int(const char* str,int&val) {
		return read_integer<int>(str,val);
	}

	size_t read_ull(const char* str,unsigned long long&val) {
		return read_integer<unsigned long long>(str,val);
	}

	size_t read_ll(const char* str,long long&val) {
		return read_integer<long long>(str,val);
	}

	inline double power_of_10(int index) {
		struct pw_cache {
			double pw10[325], npw10[325];
			pw_cache() {
				pw10[0] = npw10[0] = 1.0;
				for (int i = 1; i <= 324; ++i) {
					pw10[i] = pw10[i - 1] * 10.0;
					npw10[i] = 1.0 / pw10[i];
				}
			}
		};
		static pw_cache _cache;
		assert(index > -325 && index < 325);
		return (index >= 0) ? _cache.pw10[index] : _cache.npw10[-index];
	}

	size_t read_double(const char* str,double&val) {
		auto* ptr = (const uint8_t*)str;
		auto* s = ptr;
		bool neg = false;

		for(;!isdigit_or_sign(*ptr);++ptr);
		switch (*ptr) {
		case '-':neg = true;++ptr;break;
		case '+':++ptr;break;
		}

		unsigned long long v = 0;
		int num = 0;

		for (; isdigit(*ptr) && num < 18; ++ptr, ++num) {
			v = v * 10 + (*ptr - '0');
		}

		int pw10 = 0;
		for (; isdigit(*ptr); ++ptr, ++pw10) ;

		if (*ptr == '.') {
			++ptr;
			for (; isdigit(*ptr) && num < 18; ++ptr, ++num, --pw10) {
				v = v * 10 + (*ptr - '0');
			}
			for(;isdigit(*ptr);++ptr);
		}

		val = v * power_of_10(pw10);
		if ((*ptr == 'e') || (*ptr == 'E')) {
			++ptr;
			int pw;
			ptr += read_int((const char*)ptr,pw);
			val *= power_of_10(pw);
		}
		if(neg) val = -val;
		return ptr - s;
	}

}
