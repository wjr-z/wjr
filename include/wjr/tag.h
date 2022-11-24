#ifndef __WJR_MTAG_H
#define __WJR_MTAG_H

#include <tuple>

#include <wjr/macro.h>

#define _WJR_TAG_BEGIN namespace tag{
#define _WJR_TAG_END }

_WJR_BEGIN
_WJR_TAG_BEGIN

template<typename...Args>
struct set;

template<typename T>
struct is_set : std::is_base_of<set<T>, T> {};

template<typename...Args>
struct is_set<set<Args...>> : std::true_type {};

template<typename T>
constexpr bool is_set_v = is_set<T>::value;

template<typename T, typename U>
struct is_in : std::false_type {};

template<typename T, typename...Args>
struct is_in<T, set<Args...>> : std::disjunction<std::is_same<T, Args>...> {};

template<typename T, typename U>
constexpr bool is_in_v = is_in<T, U>::value;

template<typename T, typename U>
struct is_subset : std::false_type {};

template<typename...Args1, typename...Args2>
struct is_subset<set<Args1...>, set<Args2...>> : std::conjunction<is_in<Args1, set<Args2...>>...> {};

template<typename T, typename U>
constexpr bool is_subset_v = is_subset<T, U>::value;

template<typename...Args1, typename...Args2>
constexpr bool subset(set<Args1...>, set<Args2...>) {
	return is_subset_v<set<Args1...>, set<Args2...>>;
}

template<typename T, typename...Args>
constexpr bool in(T, set<Args...>) {
	return is_in_v<T, set<Args...>>;
}

template<typename...Args>
struct set {
	constexpr set() = default;

	template<typename...Other>
	constexpr bool operator()(set<Other...>)const {
		return subset(set<Other...>{}, set<Args...>{});
	}
	template<typename T>
	constexpr bool operator[](T) const {
		return in(T{}, set<Args...>{});
	}
};

template<typename T, typename U>
struct __front_bind_set;

template<typename T, typename...Args>
struct __front_bind_set<T, set<Args...>> {
	using type = set<T, Args...>;
};

template<typename T, typename...Args>
using __front_bind_set_t = typename __front_bind_set<T, Args...>::type;

//

template<typename...Args>
struct make_cup;

template<typename T, typename...Args1, typename...Args2>
struct make_cup<set<T, Args1...>, set<Args2...>> {
	using __next_type = typename make_cup<set<Args1...>, set<Args2...>>::type;
	using type = std::conditional_t<
		is_in_v<T, set<Args2...>>,
		__next_type,
		__front_bind_set_t<T, __next_type>>;
};

template<typename...Args>
struct make_cup<set<>, set<Args...>> {
	using type = set<Args...>;
};

template<typename...Args1, typename...Args2, typename...Args>
struct make_cup<set<Args1...>, set<Args2...>, Args...> {
	using type = typename make_cup<typename make_cup<set<Args1...>, set<Args2...>>::type, Args...>::type;
};

template<typename...Args>
using make_cup_t = typename make_cup<Args...>::type;

template<typename...Args>
struct make_minus;

template<typename T, typename...Args1, typename...Args2>
struct make_minus<set<T, Args1...>, set<Args2...>> {
	using __next_type = typename make_minus<set<Args1...>, set<Args2...>>::type;
	using type = std::conditional_t<
		is_in_v<T, set<Args2...>>,
		__next_type,
		__front_bind_set_t<T, __next_type>>;
};

template<typename...Args>
struct make_minus<set<>, set<Args...>> {
	using type = set<>;
};

template<typename...Args>
using make_minus_t = typename make_minus<Args...>::type;

template<typename...Args>
struct make_cap;

template<typename...Args1, typename...Args2>
struct make_cap<set<Args1...>, set<Args2...>> {
	using type = make_minus_t<set<Args1...>, make_minus_t<set<Args1...>, set<Args2...>>>;
};

template<typename...Args1, typename...Args2, typename...Args>
struct make_cap<set<Args1...>, set<Args2...>, Args...> {
	using type = typename make_cap<typename make_cap<set<Args1...>, set<Args2...>>::type, Args...>::type;
};

template<typename...Args>
using make_cap_t = typename make_cap<Args...>::type;

template<typename...Args>
struct make_bigoplus;

template<typename...Args1, typename...Args2>
struct make_bigoplus<set<Args1...>, set<Args2...>> {
	using type = make_minus_t<make_cup_t<set<Args1...>, set<Args2...>>, make_cap_t<set<Args1...>, set<Args2...>>>;
};

template<typename...Args1, typename...Args2, typename...Args>
struct make_bigoplus<set<Args1...>, set<Args2...>, Args...> {
	using type = typename make_bigoplus<typename make_bigoplus<set<Args1...>, set<Args2...>>::type, Args...>::type;
};

template<typename...Args>
using make_bigoplus_t = typename make_bigoplus<Args...>::type;

//

template<typename...Args1, typename...Args2>
constexpr make_cup_t<set<Args1...>, set<Args2...>> operator|(set<Args1...>, set<Args2...>) {
	return {};
}

template<typename...Args, std::enable_if_t<std::conjunction_v<is_set<Args>...>, int> = 0>
constexpr make_cup_t<Args...> cup(Args...) {
	return {};
}

template<typename...Args1, typename...Args2>
constexpr make_minus_t<set<Args1...>, set<Args2...>> operator-(set<Args1...>, set<Args2...>) {
	return {};
}

template<typename...Args, std::enable_if_t<std::conjunction_v<is_set<Args>...>, int> = 0>
constexpr make_minus_t<Args...> minus(Args...) {
	return {};
}

template<typename...Args1, typename...Args2>
constexpr make_cap_t<set<Args1...>, set<Args2...>> operator&(set<Args1...>, set<Args2...>) {
	return {};
}

template<typename...Args1, typename...Args2>
constexpr make_cap_t<set<Args1...>, set<Args2...>> cap(set<Args1...>, set<Args2...>) {
	return {};
}

template<typename...Args1, typename...Args2>
constexpr make_bigoplus_t<set<Args1...>, set<Args2...>> operator^(set<Args1...>, set<Args2...>) {
	return {};
}

template<typename...Args, std::enable_if_t<std::conjunction_v<is_set<Args>...>, int> = 0>
constexpr make_bigoplus_t<Args...> bigoplus(Args...) {
	return {};
}

template<size_t I, typename T>
struct set_alternative;

template<size_t I, typename T, typename...Args>
struct set_alternative<I, set<T, Args...>> {
	using type = typename set_alternative<I - 1, set<Args...>>::type;
};

template<typename T, typename...Args>
struct set_alternative<0, set<T, Args...>> {
	using type = T;
};

template<size_t I, typename T>
using set_alternative_t = typename set_alternative<I, T>::type;

template<typename T>
struct set_size;

template<typename...Args>
struct set_size<set<Args...>> {
	static constexpr size_t value = sizeof...(Args);
};

template<typename T>
constexpr size_t set_size_v = set_size<T>::value;

template<size_t index, typename T, typename...Args>
struct __Get_set_index;

template<size_t index, typename T, typename U, typename...Args>
struct __Get_set_index<index, T, U, Args...> {
	constexpr static size_t value = __Get_set_index<index + 1, T, Args...>::value;
};

template<size_t index, typename T, typename...Args>
struct __Get_set_index<index, T, T, Args...> {
	constexpr static size_t value = index;
};

template<size_t index, typename T>
struct __Get_set_index<index, T> {
	constexpr static size_t value = -1;
};

template<typename T, typename U>
struct set_index;

template<typename T, typename...Args>
struct set_index<T, set<Args...>> {
	constexpr static size_t value = __Get_set_index<0, T, Args...>::value;
};

template<typename T, typename U>
constexpr size_t set_index_v = set_index<T, U>::value;

template<template<typename...>typename T, typename U>
struct bind_set {
};

template<template<typename...Args>typename T, typename...Args>
struct bind_set<T, set<Args...>> {
	using type = T<Args...>;
};

template<template<typename...>typename T, typename U>
using bind_set_t = typename bind_set<T, U>::type;

#define REGISTER_WJR_TAG(name)											\
		struct name##_TAG : wjr::tag::set<name##_TAG> {					\
			constexpr explicit name##_TAG() noexcept = default;			\
		};																\
		constexpr name##_TAG name;	



// Disable_tag	:
// It can't be used in any operation.
// It is used in std::conditional_t to disable a function overload.
struct DISABLE_TAG : set<DISABLE_TAG> {
	DISABLE_TAG() = delete;
	DISABLE_TAG(const DISABLE_TAG&) = delete;
	DISABLE_TAG(DISABLE_TAG&&) = delete;
	DISABLE_TAG& operator=(const DISABLE_TAG&) = delete;
	DISABLE_TAG& operator=(DISABLE_TAG&&) = delete;
	~DISABLE_TAG() = delete;
};

// none_tag	:
// It is used as a placeholder.
REGISTER_WJR_TAG(NONE);
REGISTER_WJR_TAG(SELF);
REGISTER_WJR_TAG(SSO);
REGISTER_WJR_TAG(COW);
REGISTER_WJR_TAG(SMALL);
REGISTER_WJR_TAG(MEDIUM);
REGISTER_WJR_TAG(LARGE);
REGISTER_WJR_TAG(UNINITIALIZED);
REGISTER_WJR_TAG(DEFAULT_CONSTRUCT);
REGISTER_WJR_TAG(VALUE_CONSTRUCT);
REGISTER_WJR_TAG(COPY);
REGISTER_WJR_TAG(MOVE);
REGISTER_WJR_TAG(FILL);
REGISTER_WJR_TAG(CONNECT);
REGISTER_WJR_TAG(ENOUGH);

_WJR_TAG_END
_WJR_END

namespace std {
	template<typename...Args>
	struct tuple_size<wjr::tag::set<Args...>> : std::integral_constant<size_t, wjr::tag::set_size_v<wjr::tag::set<Args...>>> {};

	template<size_t index, typename...Args>
	struct tuple_element<index, wjr::tag::set<Args...>> {
		static_assert(index < tuple_size<wjr::tag::set<Args...>>::value, "index out of range");
		using type = wjr::tag::set_alternative_t<index, wjr::tag::set<Args...>>;
	};
}

#undef _WJR_TAG_END
#undef _WJR_TAG_BEGIN

#endif