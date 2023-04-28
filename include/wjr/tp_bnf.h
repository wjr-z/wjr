#pragma once
#ifndef __WJR_TP_BNF_H
#define __WJR_TP_BNF_H

#include <wjr/tp_list.h>

_WJR_BEGIN

// solve FIRST, FOLLOW, SELECT
// BNF is like :
// tp_list<E1, E2, E3, ... , En>
// Ei is edge
// Ei is like :
// tp_list<fa, son1, son2, ... , sonn>
// and do something
// now is only support LL(1)
// TODO : support LALR

struct tp_bnf_empty {};

template<typename BNF>
struct tp_bnf_non_terminator {
	using type = tp_unique_t<tp_transform_t<BNF, tp_front_t>>;
};

// get all non terminator node
template<typename BNF>
using tp_bnf_non_terminator_t = typename tp_bnf_non_terminator<BNF>::type;

template<typename BNF, typename T>
inline constexpr size_t tp_bnf_find_non_terminator_v = tp_find_v<tp_bnf_non_terminator_t<BNF>, T>;

template<typename BNF, typename T>
inline constexpr bool tp_bnf_is_non_terminator_v = tp_bnf_find_non_terminator_v<BNF, T> != -1;

template<typename BNF>
struct tp_bnf_terminator {
	using type = tp_set_difference_t<
		tp_left_fold_t<tp_transform_t<BNF, tp_pop_front_t>, tp_list<>, tp_set_union_t>,
		tp_bnf_non_terminator_t<BNF>
	>;
};

// get all terminator (leaf)
template<typename BNF>
using tp_bnf_terminator_t = typename tp_bnf_terminator<BNF>::type;

template<typename BNF, typename T>
inline constexpr size_t tp_bnf_find_terminator_v = tp_find_v<tp_bnf_terminator_t<BNF>, T>;

template<typename BNF, typename T>
inline constexpr bool tp_bnf_is_terminator_v = tp_bnf_find_terminator_v<BNF, T> != -1;

template<typename BNF, typename FIRST, typename LAST_FIRST>
inline constexpr auto __tp_bnf_first_helper();

template<typename BNF, typename E, typename FIRST>
inline constexpr auto __tp_bnf_first_helper2();

template<typename BNF, typename FA, typename SON, typename FIRST>
inline constexpr auto __tp_bnf_first_helper3();

template<typename BNF, typename FIRST, typename LAST_FIRST>
inline constexpr auto __tp_bnf_first_helper() {
	// don't update more
	if constexpr (std::is_same_v<FIRST, LAST_FIRST>) {
		return FIRST{};
	}
	// update all edges
	else {
		constexpr auto __value = __tp_bnf_first_helper2<BNF, BNF, FIRST>();
		using __type = WJR_PRIMITIVE_TYPE(__value);
		return __tp_bnf_first_helper<BNF, __type, FIRST>();
	}
}

template<typename BNF, typename E, typename FIRST>
inline constexpr auto __tp_bnf_first_helper2() {
	constexpr auto len = tp_size_v<E>;
	if constexpr (len == 0) {
		return FIRST{};
	}
	// update one edge
	else {
		using front = tp_front_t<E>;
		constexpr auto __value = __tp_bnf_first_helper3<BNF, tp_front_t<front>, tp_pop_front_t<front>, FIRST>();
		using __type = WJR_PRIMITIVE_TYPE(__value);
		return __tp_bnf_first_helper2<BNF, tp_pop_front_t<E>, __type>();
	}
}

template<typename BNF, typename FA, typename SON, typename FIRST>
inline constexpr auto __tp_bnf_first_helper3() {
	constexpr auto len = tp_size_v<SON>;
	// this is the end
	if constexpr (len == 0) {
		return FIRST{};
	}
	else {
		using front = tp_front_t<SON>;
		constexpr auto idx = tp_bnf_find_non_terminator_v<BNF, FA>;
		// if this node is terminator
		if constexpr (tp_bnf_is_terminator_v<BNF, front>) {
			// we need to update the first set
			// add this terminator to the first set of FA
			return tp_replace_at_t<FIRST, idx, tp_set_push_back_t<tp_at_t<FIRST, idx>, front>>();
		}
		// if this node is non-terminator
		else {
			constexpr auto idx2 = tp_bnf_find_non_terminator_v<BNF, front>;
			// we need to update the first set
			// add the first set of this non-terminator to the first set of FA
			using __type1 = tp_replace_at_t<FIRST, idx, tp_set_union_t<tp_at_t<FIRST, idx>, tp_at_t<FIRST, idx2>>>;
			// if this node can reach empty
			// we need to update the first set
			if constexpr (tp_contains_v<tp_at_t<FIRST, idx2>, tp_bnf_empty>) {
				constexpr auto __value = __tp_bnf_first_helper2<BNF, FA, tp_pop_front_t<SON>, __type1>();
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
struct tp_bnf_first {
private:
	constexpr static auto __value = __tp_bnf_first_helper<BNF,
		// start state
		tp_repeat_t<tp_list<tp_list<>>, tp_size_v<tp_bnf_non_terminator_t<BNF>>>,
		void>();
public:
	using type = WJR_PRIMITIVE_TYPE(__value);
};

// get FIRST set
// Corresponds one-to-one with tp_bnf_non_terminator_t<BNF>
// for example :
// tp_bnf_non_terminator_t<BNF> : tp_list<A, B, C>
// then tp_bnf_first_t : tp_list<FIRST<A>, FIRST<B>, FIRST<C>>
// FIRST<A> : tp_list<...>
template<typename BNF>
using tp_bnf_first_t = typename tp_bnf_first<BNF>::type;

template<typename BNF>
struct tp_bnf_start;

// Start symbol needs to be specified
template<typename BNF>
using tp_bnf_start_t = typename tp_bnf_start<BNF>::type;

template<typename BNF, typename FIRST, typename FOLLOW, typename LAST_FOLLOW>
inline constexpr auto __tp_bnf_follow_helper();

template<typename BNF, typename FIRST, typename E, typename FOLLOW>
inline constexpr auto __tp_bnf_follow_helper2();

template<typename BNF, typename FIRST, typename FA, typename SON, typename FOLLOW>
inline constexpr auto __tp_bnf_follow_helper3();

template<typename BNF, typename FIRST, typename FOLLOW, typename LAST_FOLLOW>
inline constexpr auto __tp_bnf_follow_helper() {
	if constexpr (std::is_same_v<FOLLOW, LAST_FOLLOW>) {
		return FOLLOW{};
	}
	else {
		constexpr auto __value = __tp_bnf_follow_helper2<BNF, FIRST, BNF, FOLLOW>();
		using __type = WJR_PRIMITIVE_TYPE(__value);
		return __tp_bnf_follow_helper<BNF, FIRST, __type, FOLLOW>();
	}
}

template<typename BNF, typename FIRST, typename E, typename FOLLOW>
inline constexpr auto __tp_bnf_follow_helper2() {
	constexpr auto len = tp_size_v<E>;
	if constexpr (len == 0) {
		return FOLLOW{};
	}
	else {
		using front = tp_front_t<E>;
		constexpr auto __value = __tp_bnf_follow_helper3<BNF, FIRST, tp_front_t<front>, tp_pop_front_t<front>, FOLLOW>();
		using __type = WJR_PRIMITIVE_TYPE(__value);
		return __tp_bnf_follow_helper2<BNF, FIRST, tp_pop_front_t<E>, __type>();
	}
}


template<typename BNF, typename FIRST, typename FA, typename SON, typename FOLLOW>
inline constexpr auto __tp_bnf_follow_helper3() {
	constexpr auto len = tp_size_v<SON>;
	if constexpr (len == 0) {
		return FOLLOW{};
	}
	else {
		constexpr auto idx0 = tp_bnf_find_non_terminator_v<BNF, FA>;
		using first = tp_front_t<SON>;
		// if this node is terminator
		// we don't need to update the follow set of this node
		if constexpr (tp_bnf_is_terminator_v<BNF, first>) {
			return __tp_bnf_follow_helper3<BNF, FIRST, FA, tp_pop_front_t<SON>, FOLLOW>();
		}
		// this node is non-terminator
		else {
			constexpr auto idx1 = tp_bnf_find_non_terminator_v<BNF, first>;
			// if this node is the last node
			if constexpr (len == 1) {
				// we need to update the follow set of this node
				// add the follow set of FA to the follow set of this node
				return tp_replace_at_t<FOLLOW, idx1, tp_set_union_t<tp_at_t<FOLLOW, idx1>, tp_at_t<FOLLOW, idx0>>>{};
			}
			else {
				using second = tp_at_t<SON, 1>;
				// if the next node is terminator
				if constexpr (tp_bnf_is_terminator_v<BNF, second>) {
					// we need to update the follow set of this node
					// add the next node to the follow set of this node
					using __type = tp_replace_at_t<FOLLOW, idx1, tp_set_push_back_t<tp_at_t<FOLLOW, idx1>, second>>;
					return __tp_bnf_follow_helper3<BNF, FIRST, FA, tp_pop_front_t<SON>, __type>();
				}
				// if the next node is non-terminator
				else {
					// we need to update the follow set of this node
					constexpr auto idx2 = tp_bnf_find_non_terminator_v<BNF, second>;
					// add the first set of the next node to the follow set of this node
					using __type = tp_set_union_t<tp_at_t<FOLLOW, idx1>,
						tp_remove_t<tp_at_t<FIRST, idx2>, tp_bnf_empty>>;
					// if next node can reach empty
					if constexpr (tp_contains_v<tp_at_t<FIRST, idx2>, tp_bnf_empty>) {
						// add the follow set of FA to the follow set of this node
						using __type2 = tp_replace_at_t<FOLLOW, idx1, tp_set_union_t<__type, tp_at_t<FOLLOW, idx0>>>;
						return __tp_bnf_follow_helper3<BNF, FIRST, FA, tp_pop_front_t<SON>, __type2>();
					}
					else {
						// add the first set of the next node to the follow set of this node
						using __type2 = tp_replace_at_t<FOLLOW, idx1, __type>;
						return __tp_bnf_follow_helper3<BNF, FIRST, FA, tp_pop_front_t<SON>, __type2>();
					}
				}
			}
		}
	}
}

template<typename BNF>
struct tp_bnf_follow {
private:
	using __follow = tp_repeat_t<tp_list<tp_list<>>, tp_size_v<tp_bnf_non_terminator_t<BNF>>>;
	constexpr static auto __value = __tp_bnf_follow_helper<BNF,
		tp_bnf_first_t<BNF>,
		// start state
		__follow,
		void>();
public:
	using type = WJR_PRIMITIVE_TYPE(__value);
};

template<typename BNF>
using tp_bnf_follow_t = typename tp_bnf_follow<BNF>::type;

template<typename BNF, typename E, typename SELECT>
inline constexpr auto __tp_bnf_select_helper() {
	constexpr auto len = tp_size_v<E>;
	if constexpr (len == 0) {
		return SELECT{};
	}
	else {
		using first = tp_front_t<E>;
		using fa = tp_front_t<first>;
		using son = tp_pop_front_t<first>;
		using son_first = tp_front_t<son>;
		using NT = tp_bnf_non_terminator_t<BNF>;
		using FIRST = tp_bnf_first_t<BNF>;
		using FOLLOW = tp_bnf_follow_t<BNF>;
		constexpr auto idx0 = tp_find_v<NT, fa>;
		if constexpr (tp_bnf_is_terminator_v<BNF, son_first>) {
			if constexpr (std::is_same_v<son_first, tp_bnf_empty>) {
				using __follow = tp_at_t<FOLLOW, idx0>;
				return __tp_bnf_select_helper<BNF, tp_pop_front_t<E>, tp_push_back_t<SELECT, __follow>>();
			}
			else {
				return __tp_bnf_select_helper<BNF, tp_pop_front_t<E>, tp_push_back_t<SELECT, tp_list<son_first>>>();
			}
		}
		else {
			constexpr auto idx1 = tp_find_v<NT, son_first>;
			if constexpr (tp_contains_v<tp_at_t<FIRST, idx1>, tp_bnf_empty>) {
				using __follow = tp_at_t<FOLLOW, idx0>;
				using __select = tp_set_union_t<tp_remove_t<tp_at_t<FIRST, idx1>, tp_bnf_empty>, __follow>;
				return __tp_bnf_select_helper<BNF, tp_pop_front_t<E>, tp_push_back_t<SELECT, __select>>();
			}
			else {
				using __select = tp_at_t<FIRST, idx1>;
				return __tp_bnf_select_helper<BNF, tp_pop_front_t<E>, tp_push_back_t<SELECT, __select>>();
			}
		}
	}
}

template<typename BNF>
struct tp_bnf_select {
private:
	constexpr static auto __value = __tp_bnf_select_helper<BNF, BNF, tp_list<>>();
public:
	using type = WJR_PRIMITIVE_TYPE(__value);
};

// select
// tp_size_v<tp_bnf_select_t<BNF>> == tp_size_v<BNF>
// tp_bnf_select_t<BNF> is like :
// tp_list<tp_list<...>, ...>
template<typename BNF>
using tp_bnf_select_t = typename tp_bnf_select<BNF>::type;

// this is a constexpr map from char to index of select set
// MAX is max size of token
template<typename X, uint8_t MAX, typename Enable>
struct __tp_bnf_token_map {
	constexpr __tp_bnf_token_map() : m_table() {
		for (uint8_t i = 0; i != MAX; ++i)m_table[i] = -1;
		tp.for_each<X>([this](auto x) {
			using type = WJR_PRIMITIVE_TYPE(x);
			constexpr auto idx = tp_find_v<X, type>;
			this->m_table[idx] = this->m_size++;
			});
		bool f = false;
		for (uint8_t i = 0; i != MAX; ++i) {
			if (m_table[i] == -1) {
				f = true;
				m_table[i] = m_size;
			}
		}
		if (f) {
			++m_size;
		}
	}
	constexpr uint8_t size() const { return m_size; }
private:
	uint8_t m_size;
	uint8_t m_table[MAX];
};

template<typename X, uint8_t MAX>
struct __tp_bnf_token_map<X, MAX,
	std::enable_if_t<(tp_size_v<X> * 4 >= MAX), void>> {
	constexpr uint8_t size() const { return MAX; }
	constexpr uint8_t get(uint8_t x)const { return x; }
};

template<typename BNF, uint8_t MAX>
struct tp_bnf_token_map {
private:
	constexpr static __tp_bnf_token_map<tp_bnf_non_terminator_t<BNF>, MAX, void> map_x = {};
	constexpr static __tp_bnf_token_map<tp_bnf_terminator_t<BNF>, MAX, void> map_y = {};
public:
	constexpr static uint8_t sizex() { return map_x.size(); }
	constexpr static uint8_t sizey() { return map_y.size(); }
	constexpr static uint8_t getx(uint8_t x) { return map_x.get(x); }
	constexpr static uint8_t gety(uint8_t y) { return map_y.get(y); }
};

// obtain prediction table
template<typename BNF, uint8_t MAX>
class tp_bnf_prediction_table {
	constexpr static tp_bnf_token_map<BNF, MAX> m_map = {};
public:
	using non_terminal_t = tp_bnf_non_terminator_t<BNF>;
	using terminal_t = tp_bnf_terminator_t<BNF>;

	using FIRST = tp_bnf_first_t<BNF>;
	using FOLLOW = tp_bnf_follow_t<BNF>;
	using SELECT = tp_bnf_select_t<BNF>;

	constexpr static size_t non_terminal_size = tp_size_v<tp_bnf_non_terminator_t<BNF>>;
	constexpr static size_t terminal_size = tp_size_v<tp_bnf_terminator_t<BNF>>;
	constexpr static size_t edge_size = tp_size_v<BNF>;

	template<typename T>
	constexpr static size_t find_non_terminal_v = tp_find_v<non_terminal_t, T>;

	template<size_t idx>
	using non_terminal_at = tp_at_t<non_terminal_t, idx>;

	template<typename T>
	constexpr static size_t find_terminal_v = tp_find_v<terminal_t, T>;

	template<size_t idx>
	using terminal_at = tp_at_t<terminal_t, idx>;

	constexpr tp_bnf_prediction_table() : m_table() {
		for (int i = 0; i < sizex(); ++i) {
			for (int j = 0; j < sizex(); ++j) {
				m_table[i][j] = -1;
			}
		}
		tp.for_each<tp_zip_t<tp_list, tp_iota_t<0, edge_size>, SELECT>>([this](auto x) {
			using type = WJR_PRIMITIVE_TYPE(x);
			using select = tp_back_t<type>;
			tp.for_each<select>([this](auto x) {
				constexpr auto idx = tp_front_t<type>::value;
				using edge = tp_at_t<BNF, idx>;
				using fa = tp_front_t<edge>;
				this->m_table[getx(fa{})][gety(x)] = idx;
				});
			});
	}

	constexpr static size_t sizex() { return m_map.sizex(); }
	constexpr static size_t sizey() { return m_map.sizey(); }

	constexpr static uint8_t getx(uint8_t c) { return m_map.getx(c); }
	constexpr static uint8_t gety(uint8_t c) { return m_map.gety(c); }

	constexpr uint8_t get(uint8_t a, uint8_t b) const { return m_table[a][b]; }
	constexpr const uint8_t* operator[](uint8_t a) const { return m_table[a]; }

private:
	uint8_t m_table[sizex()][sizey()];
};

_WJR_END

#endif // __WJR_TP_BNF_H