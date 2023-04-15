#pragma once
#ifndef __WJR_REFLEXPR_H
#define __WJR_REFLEXPR_H

#include <wjr/tp_list.h>
#include <wjr/string.h>

_WJR_BEGIN

enum class reflexpr_mode {
	public_access    = 1,
	protected_access = 2,
	private_access   = 4,
	all_access       = public_access | protected_access | private_access,
	sort			 = 8
};

template<typename T>
class reflexpr_info;

template<typename T>
class reflexpr;

template<typename T>
struct __reflection_is_variant : std::false_type {};

template<typename...Args>
struct __reflection_is_variant<std::variant<Args...>> : std::false_type {};

class reflection_fn{
	template<typename RF>
	struct __is_reflexpr : std::bool_constant<std::is_same_v<RF, tp_unique_t<RF>>
		&& (tp_size_v<RF> >= 1)
		&& (tp_find_v<RF, std::monostate> == -1)> {};

public:
	// variant<Args..., std::monostate>
	// if not find, variant.index() == sizeof...(Args)
	template<typename RF, typename Pred, std::enable_if_t<__is_reflexpr<RF>::value, int> = 0>
	constexpr static decltype(auto) find_if(Pred pred);

	template<typename RF, std::enable_if_t< __is_reflexpr<RF>::value, int> = 0>
	WJR_CONSTEXPR20 static decltype(auto) find_name(wjr::string_view Name);

private:
	template<typename Ret, typename Pred, typename RF>
	WJR_CONSTEXPR20 static decltype(auto) __find_if_helper(Pred pred, RF);

};

inline constexpr reflection_fn reflection;

template<typename RF, typename Pred, std::enable_if_t<reflection_fn::__is_reflexpr<RF>::value, int>>
constexpr decltype(auto) reflection_fn::find_if(Pred pred) {
	using Ret = tp_rename_t<tp_push_back_t<RF, std::monostate>, std::variant>;
	return __find_if_helper<Ret>(pred, tp_rename_t<RF, tp_list>());
}

template<typename RF, std::enable_if_t<reflection_fn::__is_reflexpr<RF>::value, int>>
WJR_CONSTEXPR20 decltype(auto) reflection_fn::find_name(wjr::string_view Name) {
	return find_if<RF>([Name](auto _Val) {
		return Name == _Val.name;
		});
}

#define __WJR_REFLEXPR_FIND_IF_CASE(x)	                        \
	if constexpr(_Size >= (x + 1)){	                            \
		using T = tp_at_t<RF, x>;	                            \
		if(pred(T())){	                                        \
			return static_cast<Ret>(std::in_place_type_t<T>());	\
		}	                                                    \
	}

template<typename Ret, typename Pred, typename RF>
WJR_CONSTEXPR20 decltype(auto) reflection_fn::__find_if_helper(Pred pred, RF) {
	constexpr auto _Size = tp_size_v<RF>;
	// optimized 
	if constexpr (_Size <= 16) {
		WJR_MACRO_CALL(__WJR_REFLEXPR_FIND_IF_CASE, , 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		return static_cast<Ret>(std::in_place_type_t<std::monostate>());
	}
	else {
		WJR_MACRO_CALL(__WJR_REFLEXPR_FIND_IF_CASE, , 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		return __find_if_helper(pred, tp_remove_prefix_t<RF, 16>());
	}
}

#undef __WJR_REFLEXPR_FIND_IF_CASE

_WJR_END

#endif // __WJR_REFLEXPR_H