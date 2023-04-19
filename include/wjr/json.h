#pragma once
#ifndef __WJR_JSON_H
#define __WJR_JSON_H

#include <cstdint>
#include <cstdlib>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include <wjr/string.h>
#include <wjr/allocator.h>
#include <wjr/tp_list.h>

_WJR_BEGIN

class json;

struct json_traits {
	template<typename T>
	using allocator_type = nsallocator<T>;

	using null = std::nullptr_t;
	using boolean = bool;
	using number = double;

	using vstring = basic_string<char, std::char_traits<char>, allocator_type<char>>;
	using varray = vector<json, allocator_type<json>>;
	using vobject = std::map<vstring, json, std::less<vstring>, allocator_type<std::pair<const vstring, json>>>;

	using string = unique_ptr_with_nsallocator<vstring>;
	using array = unique_ptr_with_nsallocator<varray>;
	using object = unique_ptr_with_nsallocator<vobject>;

	using vcont = std::variant<null, boolean, number, vstring, varray, vobject>;
	using cont = std::variant<null, boolean, number, string, array, object>;

	using type_list = tp_list<null, boolean, number, vstring, varray, vobject, string, array, object>;
};

template<typename T, typename = void>
struct __json_get {
	using type = T;
};

template<typename T>
struct __json_get<T, std::enable_if_t<(tp_find_v<json_traits::type_list, T> >= 6)>> {
	constexpr static size_t value = tp_find_v<json_traits::type_list, T>;
	static_assert(value != -1, "");
	using type = tp_at_t<json_traits::vcont, value - 3>;
};

template<typename T>
using __json_get_t = typename __json_get<T, void>::type;

template<typename T>
constexpr const __json_get_t<T>& get(const json& j);

template<typename T>
constexpr __json_get_t<T>& get(json& j);

template<typename T>
constexpr __json_get_t<T>&& get(json&& j);

class json : public json_traits {
public:

	using traits_type = json_traits;
	template<typename T>
	using allocator_type = typename traits_type::template allocator_type<T>;

	using null = typename traits_type::null;
	using boolean = typename traits_type::boolean;
	using number = typename traits_type::number;

	using vstring = typename traits_type::vstring;
	using varray = typename traits_type::varray;
	using vobject = typename traits_type::vobject;

	using string = typename traits_type::string;
	using array = typename traits_type::array;
	using object = typename traits_type::object;

	using vcont = typename traits_type::vcont;
	using cont = typename traits_type::cont;

	using type_list = typename traits_type::type_list;

	inline json() = default;
	inline json(const json& other) {
		std::visit([this](const auto& x) {
			constexpr auto idx1 = tp_find_v<vcont, remove_cvref_t<decltype(x)>>;
			constexpr auto idx2 = tp_find_v<cont, remove_cvref_t<decltype(x)>>;
			if constexpr (idx1 != -1) {
				this->emplace<tp_at_t<vcont, idx1>>(x);
			}
			else {
				this->emplace<tp_at_t<vcont, idx2>>(*x);
			}
			}, other.m_value);
	}
	inline json(json&& other) = default;

private:
	template<size_t idx, typename...Args, std::enable_if_t<(idx >= 0) && (idx < 3), int> = 0>
	inline json(std::in_place_index_t<idx>, Args&&...args) 
		: m_value(std::in_place_index_t<idx>{}, std::forward<Args>(args)...) {}
	template<size_t idx, typename...Args, std::enable_if_t<(idx >= 6) && (idx < 9), int> = 0>
	inline json(std::in_place_index_t<idx>, Args&&...args)
		: m_value(std::in_place_index_t<idx - 3>{}, std::forward<Args>(args)...) {}
	template<size_t idx, typename...Args, std::enable_if_t<(idx >= 3 && idx < 6), int> = 0>
	inline json(std::in_place_index_t<idx>, Args&&...args)
		: m_value(std::in_place_index_t<idx>{},
			std::move(make_unique_with_nsallocator<tp_at_t<vcont, idx>>(std::forward<Args>(args)...))) {}
public:

	template<typename T, typename... Args, std::enable_if_t<tp_find_v<type_list, T> != -1, int> = 0>
	inline json(std::in_place_type_t<T>, Args&&...args)
		: json(std::in_place_index_t<tp_find_v<type_list, T>>{}, std::forward<Args>(args)...) {}

	template<typename T, std::enable_if_t<
		tp_find_constructible_v<type_list, T&&> != -1 
		&& !is_in_place_type_v<remove_cvref_t<T>>, int> = 0>
	inline json(T&& t) : json(std::in_place_index_t<tp_find_constructible_v<type_list, T&&>>{}, std::forward<T>(t)) {}

	template<typename T, typename...Args, 
		std::enable_if_t<tp_find_constructible_v<type_list, T&&, Args&&...> != -1
		&& !is_in_place_type_v<remove_cvref_t<T>>, int> = 0>
	inline json(T&& t, Args&&...args) 
		: json(std::in_place_index_t<tp_find_constructible_v<type_list, T&&>>{}, 
			std::forward<T>(t), std::forward<Args>(args)...) {}

	inline json& operator=(const json& other) {

		if (this == std::addressof(other)) {
			return *this;
		}

		std::visit([this](const auto& x) {
			constexpr auto idx1 = tp_find_v<vcont, remove_cvref_t<decltype(x)>>;
			constexpr auto idx2 = tp_find_v<cont, remove_cvref_t<decltype(x)>>;
			if constexpr (idx1 != -1) {
				this->emplace<tp_at_t<vcont, idx1>>(x);
			}
			else {
				this->emplace<tp_at_t<vcont, idx2>>(*x);
			}
			}, other.m_value);

		return *this;
	}

	inline json& operator=(json&& other) = default;

	template<typename T, std::enable_if_t<tp_find_assignable_v<type_list, T&&> != -1 
		&& !std::is_same_v<json, std::decay_t<T>>, int> = 0>
	inline json& operator=(T&& t) {
		emplace<tp_at_t<type_list, tp_find_assignable_v<type_list, T&&>>>(std::forward<T>(t));
		return *this;
	}

	inline~json() = default;

	inline void reset() { emplace<null>(nullptr); }

	inline size_t index() const { return m_value.index(); }

	template<typename T, typename...Args, std::enable_if_t<
		tp_contains_v<type_list, T>, int> = 0>
	inline T& emplace(Args&&...args) {
		constexpr auto idx1 = tp_find_v<vcont, T>;
		constexpr auto idx2 = tp_find_v<cont, T>;
		if constexpr (idx1 < 3) {
			static_assert(idx1 == idx2, "");
			return m_value.emplace<T>(std::forward<Args>(args)...);
		}
		else if constexpr (idx2 != -1) {
			static_assert(idx1 == -1, "");
			return m_value.emplace<idx2>(std::move(make_unique_with_nsallocator<tp_at_t<vcont, idx2>>(std::forward<Args>(args)...)));
		}
		else {
			static_assert(idx2 == -1, "");
			return *(m_value.emplace<idx1>(std::move(make_unique_with_nsallocator<tp_at_t<vcont, idx1>>(std::forward<Args>(args)...))));
		}
	}

	inline cont& get() &  { return m_value; }
	inline cont&& get()&& { return std::move(m_value); }
	inline const cont& get() const & { return m_value; }

	static bool accept(const char* first, const char* last) {
		first = ascii::encode::skipw(first, last);
		if (first == last) {
			return false;
		}

		switch (*first) {
		case '[':
			++first;
			if (!__accept(first, last, ']'))
				return false;
			first = ascii::encode::skipw(first, last);
			return first == last;
		case '{':
			++first;
			if (!__accept(first, last, '}'))
				return false;
			first = ascii::encode::skipw(first, last);
			return first == last;
		default:
			return false;
		}
	}

	static json parse(const char* first, const char* last) {
		return __parse(first, last);
	}

	template<typename T>
	static decltype(auto) value(T&& t) {
		constexpr auto idx = tp_find_v<type_list, remove_cvref_t<T>>;
		if constexpr (idx < 6) {
			return std::forward<T>(t);
		}
		else {
			return *(std::forward<T>(t));
		}
	}

private:

	static json __parse(const char*& first, const char* last);
	static bool __accept(const char*& first, const char* last, uint8_t state);

	cont m_value;
};

template<typename T>
constexpr const __json_get_t<T>& get(const json& j) {
	constexpr auto idx = tp_find_v<json::type_list, T>;
	if constexpr (idx < 3 || idx >= 6) {
		return std::get<T>(j.get());
	}
	else {
		return *(std::get<tp_at_t<json::cont, idx>>(j.get()));
	}
}

template<typename T>
constexpr __json_get_t<T>& get(json& j) {
	constexpr auto idx = tp_find_v<json::type_list, T>;
	if constexpr (idx < 3 || idx >= 6) {
		return std::get<T>(j.get());
	}
	else {
		return *(std::get<tp_at_t<json::cont, idx>>(j.get()));
	}
}

template<typename T>
constexpr __json_get_t<T>&& get(json&& j) {
	constexpr auto idx = tp_find_v<json::type_list, T>;
	if constexpr (idx < 3 || idx >= 6) {
		return std::get<T>(std::move(j).get());
	}
	else {
		return *(std::get<tp_at_t<json::cont, idx>>(std::move(j).get()));
	}
}

_WJR_END

#endif // __WJR_JSON_H