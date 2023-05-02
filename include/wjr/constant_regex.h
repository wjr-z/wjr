#pragma once
#ifndef __WJR_CONSTANT_REGEX_H
#define __WJR_CONSTANT_REGEX_H

#include <wjr/tp_graph.h>

_WJR_BEGIN

constexpr inline size_t __get_constant_string_lenth(const char* str) {
	const char* s = str;
	while (*str != '\0') ++str;
	return str - s;
}

// make to constant string
// string must be const char*
// if you use string_view, use a constexpr pointer to point to data and pass it in
// return type is tp_list_c<char, ...>
// for example
// "abc" -> tp_list_c<char, 'a', 'b', 'c'> = tp_list<tp_c<char, 'a'>, tp_c<char, 'b'>, tp_c<char, 'c'>>
// afterwards, it can be like a regular tp_List container usage
#define WJR_MAKE_CONSTANT_STRING(STRING)                                                                    \
	[](){	                                                                                                \
		constexpr auto __wjr_str = STRING;	                                                                \
		constexpr auto __wjr_len = ::wjr::__get_constant_string_lenth(__wjr_str);	                        \
		tp_list<> it;	                                                                                    \
		return ::wjr::tp.accumulate<tp_iota_t<0, __wjr_len>>([](auto x, auto y) {	                        \
			using type = remove_cvref_t<decltype(y)>;	                                                    \
			using ret_type = tp_push_back_t<type, tp_char_t<__wjr_str[x]>>;									\
			return ret_type();	                                                                            \
			}, it);	                                                                                        \
	}()

_WJR_END

#endif // __WJR_CONSTANT_REGEX_H