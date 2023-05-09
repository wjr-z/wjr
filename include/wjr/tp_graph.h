#pragma once
#ifndef __WJR_TP_GRAPGH_H
#define __WJR_TP_GRAPGH_H

#include <wjr/tp_list.h>

// The design is not good and may be improved in the future

_WJR_BEGIN

// a grapgh is like 
// G = tp_list<V...>
// V = tp_list<ID, E...>
// E = tp_list<edge, ID>
// ID = tp_size_t<idx>

struct tp_graph_empty_edge {};

template<typename G, size_t ID>
struct tp_graph_find {
private:
	template<typename T>
	using __find_if = std::bool_constant<tp_front_t<T>::value == ID>;
public:
	constexpr static size_t value = tp_find_if_v<G, __find_if>;
};

// find node ID 's pos in G
// V = tp_at_t<G, tp_graph_find<G, ID>> is node
// and tp_front_t<V>::value == ID
template<typename G, size_t ID>
inline constexpr size_t tp_graph_find_v = tp_graph_find<G, ID>::value;

template<typename G, size_t ID>
struct tp_graph_find_fa {
private:
	template<typename T>
	using __filter2 = std::bool_constant<tp_back_t<T>::value == ID>;
	template<typename T>
	using __filter = std::bool_constant<tp_find_if_v<tp_pop_front_t<T>, __filter2> != -1>;
public:
	using type = tp_transform_t<tp_filter_t<G, __filter>, tp_front_t>;
};

// find all node X, that X 's E cotnains ID
template<typename G, size_t ID>
using tp_graph_find_fa_t = typename tp_graph_find_fa<G, ID>::type;

template<typename G>
struct tp_graph_start_node {
private:
	template<typename T>
	using __remove = std::bool_constant<
		tp_size_v<tp_graph_find_fa_t<G, tp_front_t<T>::value>> == 0>;
public:
	using type = tp_transform_t<tp_filter_t<G, __remove>, tp_front_t>;
};

// find all nodes that in degree is 0
template<typename G>
using tp_graph_start_node_t = typename tp_graph_start_node<G>::type;

template<typename G>
struct tp_graph_end_node {
private:
	template<typename T>
	using __remove = std::bool_constant<tp_size_v<T> == 1>;
public:
	using type = tp_transform_t<tp_filter_t<G, __remove>, tp_front_t>;
};

// find all nodes that out degree is 0
template<typename G>
using tp_graph_end_node_t = typename tp_graph_end_node<G>::type;

template<typename G>
struct tp_graph_unique_start_node {
private:
	using __type = tp_graph_start_node_t<G>;
	static_assert(tp_size_v<__type> == 1, "start node is not unique");
public:
	using type = tp_front_t<__type>;
};

// find the unique start node
template<typename G>
using tp_graph_unique_start_node_t = typename tp_graph_unique_start_node<G>::type;

template<typename G>
struct tp_graph_unique_end_node {
private:
	using __type = tp_graph_end_node_t<G>;
	static_assert(tp_size_v<__type> == 1, "end node is not unique");
public:
	using type = tp_front_t<__type>;
};

// find the unique end node
template<typename G>
using tp_graph_unique_end_node_t = typename tp_graph_unique_end_node<G>::type;

template<typename G>
struct tp_graph_remove_empty_node {
private:
	template<typename T>
	using __remove = std::bool_constant<
		tp_size_v<T> == 1
		&& tp_size_v<tp_graph_find_fa_t<G, tp_front_t<T>::value>> == 0>;
public:
	using type = tp_remove_if_t<G, __remove>;
};

// remove all empty nodes
template<typename G>
using tp_graph_remove_empty_node_t = typename tp_graph_remove_empty_node<G>::type;

template<typename G>
struct tp_graph_max_node {
	static_assert(tp_size_v<G> > 0, "graph is empty");
private:
	template<typename T, typename U>
	using __left_fold = tp_max_c<T, tp_front_t<U>>;
public:
	using type = tp_left_fold_t<tp_pop_front_t<G>, tp_front_t<tp_front_t<G>>, __left_fold>;
};

// find the max node
template<typename G>
using tp_graph_max_node_t = typename tp_graph_max_node<G>::type;

template<typename G, typename E, size_t I, size_t J>
struct tp_graph_add_edge {
private:
	constexpr static auto pos = tp_graph_find_v<G, I>;
	using pos_type = tp_at_t<G, pos>;
	using next_type = tp_push_back_t<pos_type, tp_list<E, tp_size_t<J>>>;
public:
	using type = tp_replace_at_t<G, pos, next_type>;
};

// add a edge E from I to J
template<typename G, typename E, size_t I, size_t J>
using tp_graph_add_edge_t = typename tp_graph_add_edge<G, E, I, J>::type;

template<typename G, size_t N>
struct tp_graph_add_all_number {
private:
	template<typename T>
	using __transform2 = tp_list<tp_front_t<T>, tp_size_t<tp_back_t<T>::value + N>>;
	template<typename T>
	using __transform = tp_push_front_t<
		tp_transform_t<tp_pop_front_t<T>, __transform2>,
		tp_size_t<tp_front_t<T>::value + N>>;
public:
	using type = tp_transform_t<G, __transform>;
};

// add N to all node ID
template<typename G, size_t N>
using tp_graph_add_all_number_t = typename tp_graph_add_all_number<G, N>::type;

_WJR_END

#endif // __WJR_TP_GRAPGH_H