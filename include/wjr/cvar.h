#pragma once
#ifndef __WJR_RTC_VAR_H
#define __WJR_RTC_VAR_H

#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename T>
constexpr decltype(auto) get_cvar(T&& value) { return std::forward<T>(value); }

template<typename T, T _Val>
constexpr auto get_cvar(std::integral_constant<T, _Val>) { return get_cvar(_Val); }


_WJR_END

#endif // __WJR_RTC_VAR_H