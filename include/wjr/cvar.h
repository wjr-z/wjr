#pragma once
#ifndef __WJR_RTC_VAR_H
#define __WJR_RTC_VAR_H

#include <wjr/type_traits.h>

_WJR_BEGIN

// TODO : 
// 1. supports lazy combination of runtime and compile time variables
//	For example, a runtime iterator, 
// but the distance between two iterators is known at compile time

// can store runtime variables and compile time constants (represented by integral_constant)
// Although there are some methods for determining constants, 
// such as __builtin_constant_p and is_evaluated_constant, however, 
// often the number determined by this method cannot be used as a constant for the template
// It can also be used to force the compiler to optimize constants
template<typename T>
class cvar;

template<typename T, T val>
class cvar<std::integral_constant<T, val>> {
public:
	using value_type = T;
	constexpr static value_type value = val;
};

template<typename T>
struct __is_cvar_helper : std::false_type {};

template<typename T>
struct __is_cvar_helper<cvar<T>> : std::true_type {};

template<typename T>
struct is_cvar : __is_cvar_helper<remove_cvref_t<T>> {};

template<typename T>
inline constexpr bool is_cvar_v = is_cvar<T>::value;

template<typename V, typename T>
struct __is_cvar_of_t : std::is_same<V, T> {};

template<typename V,typename T>
struct __is_cvar_of_t<cvar<V>, T> : std::is_same<typename V::value_type, T> {};

// if V is T or constexpr var of T
template<typename V, typename T>
inline constexpr bool __is_cvar_of_t_v = __is_cvar_of_t<V, T>::value;

template<typename T>
struct __to_cvar {
	using type = T;
	constexpr static auto holder_value(T value) { return value; }
	constexpr static auto args_value(T value) { return value; }
};

template<typename T>
struct __to_cvar<cvar<T>> {
	using type = cvar<T>;
	constexpr static auto holder_value(T) { return static_cast<int>(0); }
	constexpr static auto args_value(T value) { return value; }
};

template<auto val, typename U>
constexpr auto __make_cvar(U u) {
	using T = decltype(val);
	if constexpr (std::is_same_v<T, U>) {
		return cvar<std::integral_constant<T, val>>();
	}
	else {
		return u;
	}
}

#define __WJR_MAKE_CVAR(val, helper) \
	wjr::__make_cvar<helper::holder_value(val)>(helper::args_value(val))
// make constexpr var or cvar<T> to cvar
#define WJR_MAKE_CVAR(val) __WJR_MAKE_CVAR(val, wjr::__to_cvar<decltype(val)>)

template<typename T>
constexpr decltype(auto) get_cvar(T&& value) { return std::forward<T>(value); }

template<typename T>
constexpr auto get_cvar(cvar<T> value) { return get_cvar(value.value); }

// ensure fn is constexpr
template<typename Func, typename...Args>
constexpr auto invoke_cvar(Func fn, Args&&...args) {
	return fn(get_cvar(std::forward<Args>(args))...);
}

// ensure fn is constexpr
// if all args is constexpr and fn is constexpr
// we should get a constexpr var 
template<typename Func, typename...Args>
constexpr auto invoke_cvar(Func fn, cvar<Args>...) {
	constexpr auto ret = fn(get_cvar(Args{})...);
	return WJR_MAKE_CVAR(ret);
}

// vector for cvar
template<typename...Args>
class cvar_vector {
	template<size_t idx>
	using value_type = std::tuple_element_t<idx, std::tuple<Args...>>;
};

template<typename...Args>
class cvar<cvar_vector<cvar<Args>...>> {
public:
	using value_type = cvar_vector<cvar<Args>...>;
	constexpr value_type value() const { return {}; }
};

template<size_t idx, typename...Args>
constexpr auto get_cvar(cvar_vector<Args...>) {
	using value_type = typename cvar_vector<Args...>::template value_type<idx>;
	return get_cvar(value_type());
}

_WJR_END

#endif // __WJR_RTC_VAR_H