#pragma once
#ifndef __WJR_VARIANT_H
#define __WJR_VARIANT_H

#include <variant>

#include <wjr/tp_list.h>
#include <wjr/algorithm.h>

_WJR_BEGIN

template<size_t from_I, size_t to_I, typename...Vars, typename...Args>
WJR_CONSTEXPR20 tp_at_t<tp_list<Vars...>, to_I>& emplace_from(std::variant<Vars...>& var, Args&&...args) {
	WJR_ASSUME(var.index() == from_I);
	using from_value_type = tp_at_t<tp_list<Vars...>, from_I>;
	wjr::destroy_at(std::addressof(std::get<from_I>(var)));
	wjr::construct_at(std::addressof(var), std::in_place_index<to_I>, std::forward<Args>(args)...);
	return std::get<to_I>(var);
}

template<size_t from_I, typename to_T, typename...Vars, typename...Args>
WJR_CONSTEXPR20 to_T& emplace_from(std::variant<Vars...>& var, Args&&...args) {
	WJR_ASSUME(var.index() == from_I);
	using from_value_type = tp_at_t<tp_list<Vars...>, from_I>;
	wjr::destroy_at(std::addressof(std::get<from_I>(var)));
	wjr::construct_at(std::addressof(var), std::in_place_type<to_T>, std::forward<Args>(args)...);
	return std::get<to_T>(var);
}

_WJR_END

#endif // __WJR_VARIANT_H