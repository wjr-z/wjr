#pragma once
#ifndef __WJR_JSON_H
#define __WJR_JSON_H

#include <map>
#include <variant>

#include <wjr/string.h>
#include <wjr/allocator.h>
#include <wjr/tp_list.h>

_WJR_BEGIN

class json;

// cstring/cobject/carray is unique_ptr of string/object/array
// container use the former
// you can use both to init json
struct json_traits {
	template<typename T>
	using allocator_type = sallocator<T>;

	using null = std::nullptr_t;
	using boolean = bool;
	using number = double;

	using string = basic_string<char, std::char_traits<char>, allocator_type<char>>;
	using array = vector<json, allocator_type<json>>;
	using object = std::map<string, json, std::less<string>, allocator_type<std::pair<const string, json>>>;

	using cstring = unique_ptr_with_sallocator<string>;
	using carray = unique_ptr_with_sallocator<array>;
	using cobject = unique_ptr_with_sallocator<object>;

	using vcont = std::variant<null, boolean, number, string, array, object>;
	using cont = std::variant<null, boolean, number, cstring, carray, cobject>;

	using type_list = tp_list<null, boolean, number, string, array, object, cstring, carray, cobject>;

	template<typename T>
	constexpr static size_t find_vcont_v = tp_find_v<vcont, T>;

	template<typename T>
	constexpr static size_t find_cont_v = tp_find_v<cont, T>;

	template<typename T>
	constexpr static size_t find_type_v = tp_find_v<type_list, T>;

private:
	template<typename T, typename = void>
	struct __find_best_constructible {
		using type = tp_find_best_constructible<type_list, T>;
	};
	template<typename T>
	struct __find_best_constructible<T, std::enable_if_t<std::is_integral_v<remove_cvref_t<T>>, void>> {
		using type = tp_size_t<2>;
	};
public:

	template<typename T>
	using find_best_constructible = typename __find_best_constructible<T>::type;

	template<typename T>
	constexpr static size_t find_best_constructible_v = find_best_constructible<T>::value;

};

template<size_t idx>
constexpr tp_at_t<json_traits::type_list, idx>& get(json&);

template<size_t idx>
constexpr const tp_at_t<json_traits::type_list, idx>& get(const json&);

template<size_t idx>
constexpr tp_at_t<json_traits::type_list, idx>&& get(json&&);

template<typename T>
constexpr T& get(json&);

template<typename T>
constexpr const T& get(const json& j);

template<typename T>
constexpr T&& get(json&& j);

template<size_t idx>
constexpr std::add_pointer_t<tp_at_t<json_traits::type_list, idx>> get_if(json*) noexcept;

template<size_t idx>
constexpr std::add_pointer_t<const tp_at_t<json_traits::type_list, idx>> get_if(const json*)noexcept;

template<typename T>
constexpr std::add_pointer_t<T> get_if(json*) noexcept;


template<typename T>
constexpr std::add_pointer_t<const T> get_if(const json*) noexcept;

class json : public json_traits {
public:

	using traits_type = json_traits;
	template<typename T>
	using allocator_type = typename traits_type::template allocator_type<T>;

	using null = typename traits_type::null;
	using boolean = typename traits_type::boolean;
	using number = typename traits_type::number;

	using string = typename traits_type::string;
	using array = typename traits_type::array;
	using object = typename traits_type::object;

	using cstring = typename traits_type::cstring;
	using carray = typename traits_type::carray;
	using cobject = typename traits_type::cobject;

	using vcont = typename traits_type::vcont;
	using cont = typename traits_type::cont;

	using type_list = typename traits_type::type_list;

	template<typename T>
	constexpr static size_t find_vcont_v = traits_type::template find_vcont_v<T>;

	template<typename T>
	constexpr static size_t find_cont_v = traits_type::template find_cont_v<T>;

	template<typename T>
	constexpr static size_t find_type_v = traits_type::template find_type_v<T>;

	template<typename T>
	constexpr static size_t find_best_constructible_v = traits_type::template find_best_constructible_v<T>;

	template<typename T>
	WJR_CONSTEXPR20 static decltype(auto) value(T&& t) {
		using type = remove_cvref_t<T>;
		constexpr auto idx = find_type_v<type>;
		static_assert(idx < 9, "");
		if constexpr (idx < 6) {
			return std::forward<T>(t);
		}
		else {
			return *(std::forward<T>(t));
		}
	}

	// default , nullptr
	WJR_CONSTEXPR20 json() noexcept = default;

	WJR_CONSTEXPR20 json(const json& other) noexcept {
		std::visit([this](const auto& x) {
			constexpr auto idx = find_cont_v<remove_cvref_t<decltype(x)>>;
			wjr::construct_at(this, std::in_place_index_t<idx>{}, json::value(x));
		}, other.m_value);
	}

	WJR_CONSTEXPR20 json(json&& other) noexcept = default;

	template<size_t idx, typename...Args, std::enable_if_t<(idx >= 0) && (idx < 3), int> = 0>
	WJR_CONSTEXPR20 json(std::in_place_index_t<idx>, Args&&...args) noexcept
		: m_value(std::in_place_index_t<idx>{}, std::forward<Args>(args)...) {}

	// init a json with cstring/cobject/carray
	template<size_t idx, typename...Args, std::enable_if_t<(idx >= 6) && (idx < 9), int> = 0>
	WJR_CONSTEXPR20 json(std::in_place_index_t<idx>, Args&&...args) noexcept
		: m_value(std::in_place_index_t<idx - 3>{}, std::forward<Args>(args)...) {}

	// init a json with string/object/array
	template<size_t idx, typename...Args, std::enable_if_t<(idx >= 3 && idx < 6), int> = 0>
	WJR_CONSTEXPR20 json(std::in_place_index_t<idx>, Args&&...args) noexcept
		: m_value(std::in_place_index_t<idx>{},
			make_unique_with_sallocator<tp_at_t<vcont, idx>>(std::forward<Args>(args)...)) {}

	template<typename T, typename... Args, std::enable_if_t<find_type_v<T> != -1, int> = 0>
	WJR_CONSTEXPR20 json(std::in_place_type_t<T>, Args&&...args) noexcept
		: json(std::in_place_index_t<find_type_v<T>>{}, std::forward<Args>(args)...) {}

	template<typename T, std::enable_if_t<
		find_best_constructible_v<T&&> != -1 
		&& !is_in_place_type_v<remove_cvref_t<T>>
		&& !std::is_same_v<remove_cvref_t<T>, json>, int> = 0>
	WJR_CONSTEXPR20 json(T&& t) noexcept
		: json(std::in_place_index_t<find_best_constructible_v<T&&>>{}, std::forward<T>(t)) {}

	WJR_CONSTEXPR20 json& operator=(const json& other) noexcept {
		if (this == std::addressof(other)) {
			return *this;
		}

		std::visit([this](const auto& x) {
			constexpr auto idx = find_cont_v<remove_cvref_t<decltype(x)>>;
			if constexpr (idx < 3) {
				this->emplace<idx>(x);
			}
			else {
				this->emplace<idx>(*x);
			}
			}, other.m_value);

		return *this;
	}

	WJR_CONSTEXPR20 json& operator=(json&& other) noexcept = default;

	template<typename T, std::enable_if_t<
		find_best_constructible_v<T&&> != -1
		&& !is_in_place_type_v<remove_cvref_t<T>>
		&& !std::is_same_v<remove_cvref_t<T>, json>, int> = 0>
	WJR_CONSTEXPR20 json& operator=(T&& t) noexcept {
		emplace<find_best_constructible_v<T&&>>(std::forward<T>(t));
		return *this;
	}

	WJR_CONSTEXPR20 ~json() noexcept = default;

	WJR_CONSTEXPR20 void reset() { emplace<null>(nullptr); }

	WJR_CONSTEXPR20 size_t index() const { return m_value.index(); }
	WJR_CONSTEXPR20 const char* type_name() const {
		switch (m_value.index()) {
		case 0: return "null";
		case 1: return "boolean";
		case 2: return "number";
		case 3: return "string";
		case 4: return "array";
		case 5: return "object";
		default: WJR_UNREACHABLE;
		}
	}

	template<size_t I, typename...Args, std::enable_if_t<(I < 9), int> = 0>
	WJR_CONSTEXPR20 tp_at_t<type_list, I>& emplace(Args&&...args) noexcept {
		if constexpr (I < 3) {
			return m_value.emplace<I>(std::forward<Args>(args)...);
		}
		else if constexpr (I < 6) {
			if (m_value.index() == I) {
				auto ptr = std::addressof(*std::get<I>(m_value));
				wjr::destroy_at(ptr);
				wjr::construct_at(ptr, std::forward<Args>(args)...);
				return *ptr;
			}
			return *(m_value.emplace<I>(make_unique_with_sallocator<tp_at_t<type_list, I>>(std::forward<Args>(args)...)));
		}
		else {
			return m_value.emplace<I - 3>(std::forward<Args>(args)...);
		}
	}

	template<typename T, typename...Args, std::enable_if_t<
		tp_contains_v<type_list, T>, int> = 0>
	WJR_CONSTEXPR20 T& emplace(Args&&...args) noexcept {
		return emplace<find_type_v<T>>(std::forward<Args>(args)...);
	}

	WJR_CONSTEXPR20 void swap(json& other) noexcept {
		m_value.swap(other.m_value);
	}

	WJR_CONSTEXPR20 cont& get() & { return m_value; }
	WJR_CONSTEXPR20 cont&& get()&& { return std::move(m_value); }
	WJR_CONSTEXPR20 const cont& get() const & { return m_value; }

	WJR_CONSTEXPR20 static bool accept(const char* first, const char* last);

	WJR_CONSTEXPR20 static json parse(const char* first, const char* last) {
		return __parse(first, last);
	}

	enum stringify_mode {
		SHORTEST = 0,
		TWO_SPACE_ALIGN = 1, // 2 space
		FOUR_SPACE_ALIGN = 2, // 4 space
		TAB_ALGIN = 3 // 1 tab
	};

	WJR_CONSTEXPR20 string stringify(stringify_mode m = SHORTEST) const noexcept {
		string ret;
		switch (m) {
		case SHORTEST: _stringify<SHORTEST>(ret, 0); break;
		case TWO_SPACE_ALIGN: _stringify<TWO_SPACE_ALIGN>(ret, 0); break;
		case FOUR_SPACE_ALIGN: _stringify<FOUR_SPACE_ALIGN>(ret, 0); break;
		case TAB_ALGIN: _stringify<TAB_ALGIN>(ret, 0); break;
		default: WJR_UNREACHABLE; break;
		}
		return ret;
	}

private:

	WJR_CONSTEXPR20 static json __parse(const char*& first, const char* last);
	WJR_CONSTEXPR20 static bool __accept(const char*& first, const char* last, uint8_t state);

	template<int m>
	WJR_CONSTEXPR20 void _stringify(string&, int) const noexcept;


	cont m_value;
};

extern template void json::_stringify<0>(string&, int) const noexcept;
extern template void json::_stringify<1>(string&, int) const noexcept;
extern template void json::_stringify<2>(string&, int) const noexcept;
extern template void json::_stringify<3>(string&, int) const noexcept;

template<size_t idx>
constexpr tp_at_t<json_traits::type_list, idx>& get(json& it) {
	if constexpr (idx < 3) {
		return std::get<idx>(it.get());
	}
	else if constexpr (idx < 6) {
		return *(std::get<idx>(it.get()));
	}
	else {
		return std::get<idx - 3>(it.get());
	}
}

template<size_t idx>
constexpr const tp_at_t<json_traits::type_list, idx>& get(const json& it) {
	if constexpr (idx < 3) {
		return std::get<idx>(it.get());
	}
	else if constexpr (idx < 6) {
		return *(std::get<idx>(it.get()));
	}
	else {
		return std::get<idx - 3>(it.get());
	}
}

template<size_t idx>
constexpr tp_at_t<json_traits::type_list, idx>&& get(json&& it) {
	if constexpr (idx < 3) {
		return std::get<idx>(std::move(it).get());
	}
	else if constexpr (idx < 6) {
		return *(std::get<idx>(std::move(it).get()));
	}
	else {
		return std::get<idx - 3>(std::move(it).get());
	}
}

template<typename T>
constexpr T& get(json& it) {
	return wjr::get<json::find_type_v<T>>(it);
}

template<typename T>
constexpr const T& get(const json& it) {
	return wjr::get<json::find_type_v<T>>(it);
}

template<typename T>
constexpr T&& get(json&& it) {
	return wjr::get<json::find_type_v<T>>(std::move(it));
}

template<size_t idx>
constexpr std::add_pointer_t<tp_at_t<json_traits::type_list, idx>> get_if(json* it) noexcept {
	if constexpr (idx < 3) {
		return std::get_if<idx>(it->get());
	}
	else if constexpr (idx < 6) {
		auto ptr = std::get_if<idx>(it->get());
		return ptr == nullptr ? nullptr : (*ptr);
	}
	else {
		return std::get_if<idx - 3>(it->get());
	}
}

template<size_t idx>
constexpr std::add_pointer_t<const tp_at_t<json_traits::type_list, idx>> get_if(const json* it) noexcept {
	if constexpr (idx < 3) {
		return std::get_if<idx>(it->get());
	}
	else if constexpr (idx < 6) {
		auto ptr = std::get_if<idx>(it->get());
		return ptr == nullptr ? nullptr : (*ptr);
	}
	else {
		return std::get_if<idx - 3>(it->get());
	}
}

template<typename  T>
constexpr std::add_pointer_t<T> get_if(json* it) noexcept {
	return get_if<json::find_type_v<T>>(it);
}

template<typename  T>
constexpr std::add_pointer_t<const T> get_if(const json* it) noexcept {
	return get_if<json::find_type_v<T>>(it);
}

template<typename Func, typename...Var, std::enable_if_t<
	std::conjunction_v<std::is_same<remove_cvref_t<Var>, json>...>, int> = 0>
constexpr decltype(auto) visit(Func&& fn, Var&&...var) {
	return std::visit([_Fn = std::forward<Func>(fn)](auto&&...args) {
		return _Fn(json::value(std::forward<decltype(args)>(args))...);
		}, (std::forward<Var>(var).get())...);
}

_WJR_END

#endif // __WJR_JSON_H