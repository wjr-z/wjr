#pragma once
#ifndef __WJR_PARSE_LL1_H
#define __WJR_PARSE_LL1_H

#define _WJR_PARSE_LL1_BEGIN _WJR_PARSE_BEGIN namespace LL1{
#define _WJR_PARSE_LL1_END } _WJR_PARSE_END

#include <wjr/parse/generic.h>

_WJR_PARSE_LL1_BEGIN

// get FIRST, FOLLOW, SELECT
// BNF is like :
// tp_list<E1, E2, E3, ... , En>
// Ei is edge
// Ei is like :
// tp_list<fa, son1, son2, ... , sonn>

// TODO :
// Support for parsing custom actions

template<typename BNF>
struct non_terminator_list {
	using type = tp_unique_t<tp_transform_t<BNF, tp_front_t>>;
};

// get all non terminator_list node
template<typename BNF>
using non_terminator_list_t = typename non_terminator_list<BNF>::type;

template<typename BNF, typename T>
inline constexpr size_t find_non_terminator_v = tp_find_v<non_terminator_list_t<BNF>, T>;

template<typename BNF, typename T>
inline constexpr bool is_non_terminator_v = find_non_terminator_v<BNF, T> != -1;

template<typename BNF>
struct terminator_list {
	using type = tp_set_difference_t<
		tp_left_fold_t<tp_transform_t<BNF, tp_pop_front_t>, tp_list<>, tp_set_union_t>,
		non_terminator_list_t<BNF>
	>;
};

// get all terminator_list (leaf)
template<typename BNF>
using terminator_list_t = typename terminator_list<BNF>::type;

template<typename BNF, typename T>
inline constexpr size_t find_terminator_v = tp_find_v<terminator_list_t<BNF>, T>;

template<typename BNF, typename T>
inline constexpr bool is_terminator_v = find_terminator_v<BNF, T> != -1;

template<typename BNF, typename FIRST, typename LAST_FIRST>
inline constexpr auto __first_helper();

template<typename BNF, typename E, typename FIRST>
inline constexpr auto __first_helper2();

template<typename BNF, typename FA, typename SON, typename FIRST>
inline constexpr auto __first_helper3();

template<typename BNF, typename FIRST, typename LAST_FIRST>
inline constexpr auto __first_helper() {
	// don't update more
	if constexpr (std::is_same_v<FIRST, LAST_FIRST>) {
		return FIRST{};
	}
	// update all edges
	else {
		constexpr auto __value = __first_helper2<BNF, BNF, FIRST>();
		using __type = WJR_PRIMITIVE_TYPE(__value);
		return __first_helper<BNF, __type, FIRST>();
	}
}

template<typename BNF, typename E, typename FIRST>
inline constexpr auto __first_helper2() {
	constexpr auto len = tp_size_v<E>;
	if constexpr (len == 0) {
		return FIRST{};
	}
	// update one edge
	else {
		using front = tp_front_t<E>;
		constexpr auto __value = __first_helper3<BNF, tp_front_t<front>, tp_pop_front_t<front>, FIRST>();
		using __type = WJR_PRIMITIVE_TYPE(__value);
		return __first_helper2<BNF, tp_pop_front_t<E>, __type>();
	}
}

template<typename BNF, typename FA, typename SON, typename FIRST>
inline constexpr auto __first_helper3() {
	constexpr auto len = tp_size_v<SON>;
	// this is the end
	if constexpr (len == 0) {
		return FIRST{};
	}
	else {
		using front = tp_front_t<SON>;
		constexpr auto idx = find_non_terminator_v<BNF, FA>;
		// if this node is terminator_list
		if constexpr (is_terminator_v<BNF, front>) {
			// we need to update the first set
			// add this terminator_list to the first set of FA
			return tp_replace_at_t<FIRST, idx, tp_set_push_back_t<tp_at_t<FIRST, idx>, front>>();
		}
		// if this node is non-terminator_list
		else {
			constexpr auto idx2 = find_non_terminator_v<BNF, front>;
			// we need to update the first set
			// add the first set of this non-terminator_list to the first set of FA
			using __type1 = tp_replace_at_t<FIRST, idx, tp_set_union_t<tp_at_t<FIRST, idx>, tp_at_t<FIRST, idx2>>>;
			// if this node can reach empty
			// we need to update the first set
			if constexpr (tp_contains_v<tp_at_t<FIRST, idx2>, empty>) {
				constexpr auto __value = __first_helper2<BNF, FA, tp_pop_front_t<SON>, __type1>();
				using __type2 = WJR_PRIMITIVE_TYPE(__value);
				return __type2();
			}
			else {
				return __type1();
			}
		}
	}
}

template<typename BNF>
struct first {
private:
	constexpr static auto __value = __first_helper<BNF,
		// start state
		tp_repeat_t<tp_list<tp_list<>>, tp_size_v<non_terminator_list_t<BNF>>>,
		void>();
public:
	using type = WJR_PRIMITIVE_TYPE(__value);
};

// get FIRST set
// Corresponds one-to-one with non_terminator_list_t<BNF>
// for example :
// non_terminator_list_t<BNF> : tp_list<A, B, C>
// then first_t : tp_list<FIRST<A>, FIRST<B>, FIRST<C>>
// FIRST<A> : tp_list<...>
template<typename BNF>
using first_t = typename first<BNF>::type;

template<typename BNF, typename E>
struct get_first;

// get first set of E
// E is like :
// tp_list<V0, V1, ... , Vn>
// if V0 is empty, then get_first_t<BNF, tp_list<V0, Args...>> = get_first_t<BNF, tp_list<Args...>>
// else get_first_t<BNF, tp_list<V0, Args...>> = FIRST(V0)
template<typename BNF,  typename E>
using get_first_t = typename get_first<BNF, E>::type;

template<typename BNF, typename T, typename Enable>
struct __get_first_helper2 {
	using type = tp_at_t<first_t<BNF>, find_non_terminator_v<BNF, T>>;
};

template<typename BNF, typename T>
struct __get_first_helper2<BNF, T, std::enable_if_t<is_terminator_v<BNF, T>, void>> {
	using type = tp_list<T>;
};

template<typename BNF, typename E>
struct __get_first_helper {
	using type = typename __get_first_helper2<BNF, tp_front_t<E>, void>::type;
};

template<typename BNF, typename...Args>
struct __get_first_helper<BNF, tp_list<empty, Args...>> {
	using type = typename __get_first_helper2<BNF, tp_list<Args...>, void>::type;
};

template<typename BNF, typename T>
struct __get_first_helper<BNF, tp_list<T>> {
	using type = typename __get_first_helper2<BNF, T, void>::type;
};

template<typename BNF>
struct __get_first_helper<BNF, tp_list<empty>> {
	using type = typename __get_first_helper2<BNF, empty, void>::type;
};

template<typename BNF, typename E>
struct get_first {
	using type = typename __get_first_helper<BNF, E>::type;
};

template<typename BNF>
struct start;

// Start symbol needs to be specified
template<typename BNF>
using start_t = typename start<BNF>::type;

template<typename BNF, typename FOLLOW, typename LAST_FOLLOW>
inline constexpr auto __follow_helper();

template<typename BNF, typename E, typename FOLLOW>
inline constexpr auto __follow_helper2();

template<typename BNF, typename FA, typename SON, typename FOLLOW>
inline constexpr auto __follow_helper3();

template<typename BNF, typename FOLLOW, typename LAST_FOLLOW>
inline constexpr auto __follow_helper() {
	if constexpr (std::is_same_v<FOLLOW, LAST_FOLLOW>) {
		return FOLLOW{};
	}
	else {
		constexpr auto __value = __follow_helper2<BNF, BNF, FOLLOW>();
		using __type = WJR_PRIMITIVE_TYPE(__value);
		return __follow_helper<BNF, __type, FOLLOW>();
	}
}

template<typename BNF, typename E, typename FOLLOW>
inline constexpr auto __follow_helper2() {
	constexpr auto len = tp_size_v<E>;
	if constexpr (len == 0) {
		return FOLLOW{};
	}
	else {
		using front = tp_front_t<E>;
		constexpr auto __value = __follow_helper3<BNF, tp_front_t<front>, tp_pop_front_t<front>, FOLLOW>();
		using __type = WJR_PRIMITIVE_TYPE(__value);
		return __follow_helper2<BNF, tp_pop_front_t<E>, __type>();
	}
}


template<typename BNF, typename FA, typename SON, typename FOLLOW>
inline constexpr auto __follow_helper3() {
	constexpr auto len = tp_size_v<SON>;
	if constexpr (len == 0) {
		return FOLLOW{};
	}
	else {
		constexpr auto idx0 = find_non_terminator_v<BNF, FA>;
		using first = tp_front_t<SON>;
		// if this node is terminator_list
		// we don't need to update the follow set of this node
		if constexpr (is_terminator_v<BNF, first>) {
			return __follow_helper3<BNF, FA, tp_pop_front_t<SON>, FOLLOW>();
		}
		// this node is non-terminator_list
		else {
			constexpr auto idx1 = find_non_terminator_v<BNF, first>;
			// if this node is the last node
			if constexpr (len == 1) {
				// we need to update the follow set of this node
				// add the follow set of FA to the follow set of this node
				return tp_replace_at_t<FOLLOW, idx1, tp_set_union_t<tp_at_t<FOLLOW, idx1>, tp_at_t<FOLLOW, idx0>>>{};
			}
			else {
				using second_first = get_first_t<BNF, tp_pop_front_t<SON>>;
				using __type = tp_set_union_t<tp_at_t<FOLLOW, idx1>,
					tp_remove_t<second_first, empty>>;
				if constexpr (tp_contains_v<second_first, empty>) {
					// add the follow set of FA to the follow set of this node
					using __type2 = tp_replace_at_t<FOLLOW, idx1, tp_set_union_t<__type, tp_at_t<FOLLOW, idx0>>>;
					return __follow_helper3<BNF, FA, tp_pop_front_t<SON>, __type2>();
				}
				else {
					// add the first set of the next node to the follow set of this node
					using __type2 = tp_replace_at_t<FOLLOW, idx1, __type>;
					return __follow_helper3<BNF, FA, tp_pop_front_t<SON>, __type2>();
				}
			}
		}
	}
}

template<typename BNF>
struct follow {
private:
	using __follow = tp_repeat_t<tp_list<tp_list<>>, tp_size_v<non_terminator_list_t<BNF>>>;
	constexpr static auto __value = __follow_helper<BNF,
		// start state
		__follow,
		void>();
public:
	using type = WJR_PRIMITIVE_TYPE(__value);
};

template<typename BNF>
using follow_t = typename follow<BNF>::type;

template<typename BNF, typename E, typename SELECT>
inline constexpr auto __select_helper() {
	constexpr auto len = tp_size_v<E>;
	if constexpr (len == 0) {
		return SELECT{};
	}
	else {
		using first = tp_front_t<E>;
		using fa = tp_front_t<first>;
		using son = tp_pop_front_t<first>;
		using NT = non_terminator_list_t<BNF>;
		using FOLLOW = follow_t<BNF>;
		constexpr auto idx0 = tp_find_v<NT, fa>;
		using __first = get_first_t<BNF, son>;
		if constexpr (tp_contains_v<__first, empty>) {
			using __follow = tp_at_t<FOLLOW, idx0>;
			using __select = tp_push_back_t<SELECT, tp_set_union_t<tp_remove_t<__first, empty>, __follow>>;
			return __select_helper<BNF, tp_pop_front_t<E>, __select>();
		}
		else {
			using __select = tp_push_back_t<SELECT, __first>;
			return __select_helper<BNF, tp_pop_front_t<E>, __select>();
		}
	}
}

template<typename BNF>
struct select {
private:
	constexpr static auto __value = __select_helper<BNF, BNF, tp_list<>>();
public:
	using type = WJR_PRIMITIVE_TYPE(__value);
	static_assert(tp_size_v<type> == tp_size_v<BNF>, "");
};

// select
// tp_size_v<select_t<BNF>> == tp_size_v<BNF>
// select_t<BNF> is like :
// tp_list<tp_list<...>, ...>
template<typename BNF>
using select_t = typename select<BNF>::type;

template<typename BNF>
struct parse_fn {
	using SELECT = select_t<BNF>;

private:
	template<typename T, typename ACTION, typename...Args>
	inline constexpr static decltype(auto) invoke(ACTION act, Args&&...args) {
		if constexpr (std::is_same_v<T, disable_tag>) {
			(void)(std::forward<Args>(args), ...);
			return;
		}
		else {
			return act(T{}, std::forward<Args>(args)...);
		}
	}

	// tp_list<tp_list<I, S, T>...>
	// I is index
	// S is state
	// T is disable or action index
	template<typename T, typename __SELECT, size_t I, typename __LIST>
	constexpr static auto __get_switch_helper() {
		constexpr auto len = tp_size_v<__SELECT>;
		if constexpr (len == 0) { return __LIST{}; }
		else {
			using front = tp_front_t<__SELECT>;
			using thisT = tp_front_t<tp_at_t<BNF, I>>;
			if constexpr (std::is_same_v<thisT, T> && tp_size_v<front> != 0) {
				auto pre = __LIST{};
				auto aft = tp.accumulate<front>([](auto x, auto l) {
					using typex = WJR_PRIMITIVE_TYPE(x);
					using typel = WJR_PRIMITIVE_TYPE(l);
					using thisT = tp_list<tp_size_t<typex::value>, disable_tag, tp_list<switch_fallthrough>>;
					return tp_push_back_t<typel, thisT>{};
					}, pre);
				using thisList = WJR_PRIMITIVE_TYPE(aft);
				constexpr auto ListSize = tp_size_v<thisList>;
				using Last = tp_at_t<thisList, ListSize - 1>;
				using List = tp_replace_at_t<thisList, ListSize - 1,
					tp_list<tp_front_t<Last>, tp_size_t<I>, tp_list<>>>;
				return __get_switch_helper<T, tp_pop_front_t<__SELECT>, I + 1, List>();
			}
			else return __get_switch_helper<T, tp_pop_front_t<__SELECT>, I + 1, __LIST>();
		}
	}
public:

	template<typename T, typename ACTION, typename...Args>
	inline constexpr decltype(auto) operator()(T, size_t token, ACTION act, Args&&...args) const {
		constexpr auto __value = __get_switch_helper<T, SELECT, 0, tp_list<>>();
		using __switch = WJR_PRIMITIVE_TYPE(__value);
		using __index = tp_transform_t<__switch, tp_front_t>;
		using __state = tp_transform_t<__switch, tp_back_t>;
		return switch_invoke<__index, false, __state>(token, [](auto I, auto&&...args) {
			return invoke<tp_at_t<tp_at_t<__switch, I>, 1>>(std::forward<decltype(args)>(args)...);
			}, act, std::forward<Args>(args)...);
	}
};

template<typename BNF>
inline constexpr parse_fn<BNF> parse{};

_WJR_PARSE_LL1_END

#endif // __WJR_PARSE_LL1_H