#ifndef STRING_TOOL_H
#define STRING_TOOL_H

#include "mallocator.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include "String.h"

namespace wjr {

	template <class _Elem, class _Traits, class _Alloc>
	std::vector<std::basic_string<_Elem, _Traits, _Alloc>> split(
		const std::basic_string<_Elem, _Traits, _Alloc>& str
		,const char Separator) {
		using string_type = std::basic_string<_Elem, _Traits, _Alloc>;
		const size_t length = str.length();
		const _Elem* data = str.c_str();
		std::vector<string_type> strVec;
		size_t pos = 0;
		while (pos < length) {
			void* fnd = memchr((void*)(data + pos), Separator, length - pos);
			if (fnd == nullptr) {
				strVec.emplace_back(str.substr(pos, length - pos));
				break;
			}
			size_t nxt = (const _Elem*)fnd - data;
			if (pos != nxt)
				strVec.emplace_back(str.substr(pos, nxt - pos));
			pos = nxt + 1;
		}
		return strVec;
	}

	template <class _Elem, class _Traits, class _Alloc,class _Elem2,class _Traits2,class _Alloc2>
	std::vector<std::basic_string<_Elem, _Traits, _Alloc>> split(
		const std::basic_string<_Elem, _Traits, _Alloc>& str
		, const std::basic_string<_Elem2, _Traits2, _Alloc2>& Separator) {
		using string_type = std::basic_string<_Elem, _Traits, _Alloc>;
		const size_t length = str.length();
		const size_t separator_length = Separator.length();
		const _Elem* data1 = str.c_str();
		const _Elem2* data2 = Separator.c_str();
		std::vector<string_type> strVec;
		size_t pos = 0;
		skmp_searcher searcher(data2,data2+separator_length);
		while (pos < length) {
			auto fnd = searcher(data1 + pos,data1 + length);
			if (fnd.first == data1 + length) {
				strVec.emplace_back(str.substr(pos, length - pos));
				break;
			}
			size_t nxt = fnd.first - data1;
			if (pos != nxt)
				strVec.emplace_back(str.substr(pos, nxt - pos));
			pos = nxt + separator_length;
		}
		return strVec;
	}

	std::tuple<size_t, size_t> split(const char* ptr, const char cl, const char cr, const size_t _MaxCount);

	template<typename _Elem,typename _Traits,typename _Alloc>
	std::basic_string<_Elem, _Traits, _Alloc>& ltrim(std::basic_string<_Elem, _Traits, _Alloc>& str) {
		if (str.empty()) return str;
		auto iter = str.cbegin();
		auto e = str.cend();
		while (iter != e && isspace(*iter))++iter;
		str.erase(str.cbegin(),iter);
		return str;
	}

	template<typename _Elem, typename _Traits, typename _Alloc>
	std::basic_string<_Elem, _Traits, _Alloc>& rtrim(std::basic_string<_Elem, _Traits, _Alloc>& str) {
		if (str.empty()) return str;
		auto iter = str.cend();
		auto e = str.cbegin();
		while (iter != e && isspace(*(--iter)));
		str.erase(++iter,str.cend());
		return str;
	}

	template<typename _Elem, typename _Traits, typename _Alloc>
	std::basic_string<_Elem, _Traits, _Alloc>& trim(std::basic_string<_Elem, _Traits, _Alloc>& str) {
		return ltrim(rtrim(str));
	}

	size_t find_first_pos(const char* ptr, const char ch, const size_t _MaxCount);
	size_t get_number_of_char(const char* str, const size_t length, const char ch);
	size_t get_number_of_char(const char* str, const char ch);
	size_t get_number_of_char(const std::string&, const char ch);

	bool isdigit_or_sign(uint8_t ch);

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
