#pragma once
#ifndef __WJR_SWITCHER_H
#define __WJR_SWITCHER_H

#include <variant>

#include <wjr/tp_list.h>
#include <wjr/string.h>

_WJR_BEGIN

template<typename T>
class switcher;

template<typename...Args>
class switcher<tp_list<Args...>> {
public:
private:
};

#define __WJR_REGISTER_SWTICHER(x)	                            \
decltype([](){		                                            \
	struct __value{	                                            \
		constexpr static wjr::string_view name() {return x;}	\
	};	                                                        \
	return __value();	                                        \
}())

#define WJR_REGISTER_SWITCHER(...) wjr::switcher<wjr::tp_list<	\
WJR_EXPAND(WJR_MACRO_CALL_COMMA(__WJR_REGISTER_SWTICHER, __VA_ARGS__))>>

_WJR_END

#endif // __WJR_SWITCHER_H