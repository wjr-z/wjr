#pragma once
#ifndef __WJR_JSON_H
#define __WJR_JSON_H

#include <map>

#include <wjr/string_encode.h>
#include <wjr/allocator.h>
#include <wjr/variant.h>

_WJR_BEGIN

class json;

// cstring/cobject/carray is unique_ptr of string/object/array
// container use the former
// you can use both to init json
struct json_traits {
	template<typename T>
	using allocator_type = sallocator<T>;
	
	using encode_type = wjr::ascii;

	using null = std::nullptr_t;
	using boolean = bool;
	using number = double;

	using string = basic_string<char, std::char_traits<char>, allocator_type<char>>;
	using array = vector<json, allocator_type<json>>;
	using object = std::map<string, json, std::less<>, allocator_type<std::pair<const string, json>>>;

	using cstring = string*;
	using carray = array*;
	using cobject = object*;

	// original container
	using ocont = std::variant<null, boolean, number, string, array, object>;
	// container for json
	using cont = std::variant<null, boolean, number, cstring, carray, cobject>;

	using type_list = tp_list<null, boolean, number, string, array, object, cstring, carray, cobject>;

	template<size_t idx>
	using ocont_at_t = tp_at_t<ocont, idx>;

	template<size_t idx>
	using cont_at_t = tp_at_t<cont, idx>;

	template<size_t idx>
	using type_at_t = tp_at_t<type_list, idx>;

	template<typename T>
	constexpr static size_t ocont_find_v = tp_find_v<ocont, T>;

	template<typename T>
	constexpr static size_t cont_find_v = tp_find_v<cont, T>;

	template<typename T>
	constexpr static size_t type_find_v = tp_find_v<type_list, T>;

	template<typename T>
	constexpr static size_t find_best_convertible_v = tp_find_best_convertible_v<type_list, T>;

private:
	template<typename _Enable, typename...Args>
	struct __find_unique_constructible_helper {
		constexpr static size_t value = tp_find_unique_constructible_v<type_list, Args...>;
	};
	template<typename T>
	struct __find_unique_constructible_helper<
		std::enable_if_t<std::is_integral_v<remove_cvref_t<T>>, void>, T> {
		constexpr static size_t value = 2;
	};
public:

	template<typename...Args>
	constexpr static size_t find_unique_constructible_v = __find_unique_constructible_helper<void, Args...>::value;

	using key_type = string;
	using size_type = size_t;

	using object_iterator = typename object::iterator;
	using object_const_iterator = typename object::const_iterator;

};

template<size_t idx>
constexpr json_traits::type_at_t<idx>& get(json&);

template<size_t idx>
constexpr const json_traits::type_at_t<idx>& get(const json&);

template<size_t idx>
constexpr json_traits::type_at_t<idx>&& get(json&&);

template<typename T>
constexpr T& get(json&);

template<typename T>
constexpr const T& get(const json& j);

template<typename T>
constexpr T&& get(json&& j);

template<size_t idx>
constexpr std::add_pointer_t<json_traits::type_at_t<idx>> get_if(json*) noexcept;

template<size_t idx>
constexpr std::add_pointer_t<const json_traits::type_at_t<idx>> get_if(const json*)noexcept;

template<typename T>
constexpr std::add_pointer_t<T> get_if(json*) noexcept;

template<typename T>
constexpr std::add_pointer_t<const T> get_if(const json*) noexcept;

// parse does not perform error detection
// use accept to detect error before parse
class json : public json_traits {
public:

	using traits_type = json_traits;

	template<typename T>
	using allocator_type = typename traits_type::template allocator_type<T>;

	using encode_type = typename traits_type::encode_type;

	using null = typename traits_type::null;
	using boolean = typename traits_type::boolean;
	using number = typename traits_type::number;

	using string = typename traits_type::string;
	using array = typename traits_type::array;
	using object = typename traits_type::object;

	using cstring = typename traits_type::cstring;
	using carray = typename traits_type::carray;
	using cobject = typename traits_type::cobject;

	using ocont = typename traits_type::ocont;
	using cont = typename traits_type::cont;

	using type_list = typename traits_type::type_list;

	template<size_t idx>
	using ocont_at_t = typename traits_type::template ocont_at_t<idx>;

	template<size_t idx>
	using cont_at_t = typename traits_type::template cont_at_t<idx>;

	template<size_t idx>
	using type_at_t = typename traits_type::template type_at_t<idx>;

	template<typename T>
	constexpr static size_t ocont_find_v = traits_type::template ocont_find_v<T>;

	template<typename T>
	constexpr static size_t cont_find_v = traits_type::template cont_find_v<T>;

	template<typename T>
	constexpr static size_t type_find_v = traits_type::template type_find_v<T>;

	template<typename T>
	constexpr static size_t find_best_convertible_v = traits_type::template find_best_convertible_v<T>;

	template<typename...Args>
	constexpr static size_t find_unique_constructible_v = traits_type::template find_unique_constructible_v<Args...>;

	using key_type = typename traits_type::key_type;
	using size_type = size_t;

	template<typename T>
	inline static decltype(auto) value(T&& t) {
		using type = remove_cvref_t<T>;
		constexpr auto idx = type_find_v<type>;
		static_assert(idx < 9, "");
		if constexpr (idx < 6) {
			return std::forward<T>(t);
		}
		else {
			return *(std::forward<T>(t));
		}
	}

	// default , nullptr
	inline json() noexcept = default;

	inline json(const json& other) noexcept {
		std::visit([this](const auto& x) {
			constexpr auto idx = cont_find_v<WJR_PRI_TYPE(x)>;
			this->emplace_from<0, idx>(json::value(x));
			}, other.m_value);
	}

	inline json(json&& other)
		noexcept : m_value(std::move(other.m_value)) {
		// use this emplace to avoid tidy
		other.m_value.emplace<null>();
	}

	template<size_t idx, typename...Args, std::enable_if_t<(idx >= 0) && (idx < 3), int> = 0>
	inline explicit json(std::in_place_index_t<idx>, Args&&...args) noexcept
		: m_value(std::in_place_index_t<idx>{}, std::forward<Args>(args)...) {}

	// init a json with string/object/array
	template<size_t idx, typename...Args, std::enable_if_t<(idx >= 3 && idx < 6), int> = 0>
	inline explicit json(std::in_place_index_t<idx>, Args&&...args) noexcept
		// allocate
		: m_value(std::in_place_index_t<idx>{},
			allocator_type<type_at_t<idx>>().allocate(1)) {
		// construct
		construct_at(std::get<idx>(m_value), std::forward<Args>(args)...);
	}

	// init a json with cstring/cobject/carray
	template<size_t idx, typename...Args, std::enable_if_t<(idx >= 6) && (idx < 9), int> = 0>
	inline explicit json(std::in_place_index_t<idx>, Args&&...args) noexcept
		: m_value(std::in_place_index_t<idx - 3>{}, std::forward<Args>(args)...) {}

	template<typename T, typename... Args, std::enable_if_t<type_find_v<T> != -1, int> = 0>
	inline explicit json(std::in_place_type_t<T>, Args&&...args) noexcept
		: json(std::in_place_index_t<type_find_v<T>>{}, std::forward<Args>(args)...) {}

	template<typename T, std::enable_if_t<
		find_best_convertible_v<T&&> != -1
		&& !is_in_place_v<remove_cvref_t<T>>
		&& !std::is_same_v<remove_cvref_t<T>, json>, int> = 0>
	inline json(T&& t) noexcept
		: json(std::in_place_index_t<find_best_convertible_v<T&&>>{}, std::forward<T>(t)) {}

	template<typename T, std::enable_if_t<
		find_best_convertible_v<T&&> == -1
		&& find_unique_constructible_v<T&&> != -1
		&& !is_in_place_v<remove_cvref_t<T>>
		&& !std::is_same_v<remove_cvref_t<T>, json>, int> = 0>
	inline explicit json(T&& t) noexcept
		: json(std::in_place_index_t<find_unique_constructible_v<T&&>>{},
			std::forward<T>(t)) {}

	// more than 1 arg
	template<typename T, typename U, typename...Args, std::enable_if_t<
		find_unique_constructible_v<T&&, U&&, Args&&...> != -1
		&& !is_in_place_v<remove_cvref_t<T>>, int> = 0>
	inline explicit json(T&& t, U&& u, Args&&...args) noexcept
		: json(std::in_place_index_t<find_unique_constructible_v<T&&, U&&, Args&&...>>{},
			std::forward<T>(t), std::forward<U>(u), std::forward<Args>(args)...) {}

	struct parse_tag {};
	json(const char*& s, const char* e, parse_tag);

	inline json& operator=(const json& other) noexcept {
		if (this == std::addressof(other)) {
			return *this;
		}

		std::visit([this](const auto& x) {
			constexpr auto idx = cont_find_v<remove_cvref_t<decltype(x)>>;
			this->emplace<idx>(json::value(x));
			}, other.m_value);

		return *this;
	}

	inline json& operator=(json&& other) noexcept {
		if (this == std::addressof(other)) {
			return *this;
		}
		m_value = std::move(other.m_value);
		// use m_value.emplace to avoid tidy
		other.m_value.emplace<null>();
		return *this;
	}

	template<typename T, std::enable_if_t<
		find_best_convertible_v<T&&> != -1
		&& !is_in_place_v<remove_cvref_t<T>>
		&& !std::is_same_v<remove_cvref_t<T>, json>, int> = 0>
	inline json& operator=(T&& t) noexcept {
		emplace<find_best_convertible_v<T&&>>(std::forward<T>(t));
		return *this;
	}

	inline ~json() noexcept {
		tidy();
	}

private:
	// don't use json after tidy
	template<size_t I, std::enable_if_t<(I < 6), int> = 0>
	inline void tidy_from() noexcept {
		if constexpr (I < 3) {
			// do nothing
		}
		else {
			using value_type = type_at_t<I>;
			auto ptr = std::get<I>(m_value);
			ptr->~value_type();
			allocator_type<value_type>().deallocate(ptr, 1);
		}
	}
public:

	// don't use json after tidy
	// set to null
	inline void tidy() noexcept {
		std::visit([this](const auto& x) {
			constexpr auto idx = cont_find_v<WJR_PRI_TYPE(x)>;
			this->tidy_from<idx>();
		}, m_value);
	}

	template<size_t from_I, size_t to_I, typename...Args, std::enable_if_t<
		(from_I < 6 && to_I < 9), int> = 0>
	inline type_at_t<to_I>& emplace_from(Args&&...args) noexcept {
		if constexpr (to_I < 3) {
			tidy_from<from_I>();
			return wjr::emplace_from<from_I, to_I>(m_value, std::forward<Args>(args)...);
		}
		else if constexpr (to_I < 6) {
			// don't tidy for this
			if constexpr (from_I == to_I) {
				auto ptr = std::addressof(*std::get<from_I>(m_value));
				wjr::destroy_at(ptr);
				wjr::construct_at(ptr, std::forward<Args>(args)...);
				return *ptr;
			}
			else {
				tidy_from<from_I>();
				using value_type = type_at_t<to_I>;
				auto ptr = allocator_type<value_type>().allocate(1);
				wjr::construct_at(ptr, std::forward<Args>(args)...);
				return *wjr::emplace_from<from_I, to_I>(m_value, ptr);
			}
		}
		else {
			tidy_from<from_I>();
			return wjr::emplace_from<from_I, to_I - 3>(m_value, std::forward<Args>(args)...);
		}
	}

	template<size_t from_I, typename to_T, typename...Args, std::enable_if_t<
		(from_I < 6 && type_find_v<to_T> != -1), int> = 0>
	inline to_T& emplace_from(Args&&...args) noexcept {
		return emplace_from<from_I, type_find_v<to_T>>(std::forward<Args>(args)...);
	}

	template<size_t I, typename...Args, std::enable_if_t<(I < 9), int> = 0>
	inline type_at_t<I>& emplace(Args&&...args) noexcept {
		return std::visit([this, tp = std::forward_as_tuple(std::forward<Args>(args)...)](const auto& x) ->type_at_t<I>&{
			WJR_MAYBE_UNUSED constexpr auto idx = cont_find_v<WJR_PRI_TYPE(x)>;
			return std::apply([this](auto&&...args) ->type_at_t<I>&{
				return this->emplace_from<idx, I>(std::forward<decltype(args)>(args)...);
				}, std::move(tp));
			}, m_value);
	}

	template<typename T, typename...Args, std::enable_if_t<
		tp_contains_v<type_list, T>, int> = 0>
	inline T& emplace(Args&&...args) noexcept {
		return emplace<type_find_v<T>>(std::forward<Args>(args)...);
	}

	inline void swap(json& other) noexcept {
		m_value.swap(other.m_value);
	}

	WJR_ATTRIBUTE(NODISCARD, PURE) static bool accept(const char* first, const char* last);

	WJR_ATTRIBUTE(NODISCARD, PURE) static json parse(const char* first, const char* last) {
		return json(first, last, parse_tag{});
	}

	enum stringify_mode {
		SHORTEST = 0,
		TAB_ALIGN = 1, // 1 tab
		TWO_SPACE_ALIGN = 2, // 2 space
		FOUR_SPACE_ALIGN = 3, // 4 space
	};

	string stringify(stringify_mode m = TWO_SPACE_ALIGN) const noexcept {
		string ret;
		switch (m) {
		case SHORTEST: _stringify<SHORTEST>(ret, 0); break;
		case TAB_ALIGN: _stringify<TAB_ALIGN>(ret, 0); break;
		case TWO_SPACE_ALIGN: _stringify<TWO_SPACE_ALIGN>(ret, 0); break;
		case FOUR_SPACE_ALIGN: _stringify<FOUR_SPACE_ALIGN>(ret, 0); break;
		default: WJR_UNREACHABLE; break;
		}
		return ret;
	}

	WJR_ATTRIBUTE(PURE, INLINE) size_t index() const { return m_value.index(); }
	WJR_ATTRIBUTE(PURE, INLINE) const char* type_name() const {
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

	WJR_ATTRIBUTE(PURE, INLINE) cont& get()& { return m_value; }
	WJR_ATTRIBUTE(PURE, INLINE) cont&& get()&& { return std::move(m_value); }
	WJR_ATTRIBUTE(PURE, INLINE) const cont& get() const& { return m_value; }

#define WJR_REGISTER_JSON_GET_FUNC(x)															\
WJR_PURE WJR_INLINE x& get_##x() noexcept {return wjr::get<x>(*this);}	                        \
WJR_PURE WJR_INLINE  const x& get_##x() const noexcept {return wjr::get<x>(*this);}	            \
WJR_PURE WJR_INLINE  x* get_if_##x() noexcept {return wjr::get_if<x>(this);}	                \
WJR_PURE WJR_INLINE  const x* get_if_##x() const noexcept {return wjr::get_if<x>(this);}

	WJR_MACRO_CALL(WJR_REGISTER_JSON_GET_FUNC, , null, boolean, number, string, array, object, cstring, carray, cobject);

#undef WJR_REGISTER_JSON_GET_FUNC

#define WJR_REGISTER_JSON_EMPLACE_FUNC(x)	\
template<typename...Args>	\
inline x& emplace_##x(Args&&...args) noexcept{ return emplace<x>(std::forward<Args>(args)...);}

	WJR_MACRO_CALL(WJR_REGISTER_JSON_EMPLACE_FUNC, , null, boolean, number, string, array, object, cstring, carray, cobject);

#undef WJR_REGISTER_JSON_EMPLACE_FUNC

	object_iterator find(const key_type& key) { return get_object().find(key); }
	object_const_iterator find(const key_type& key) const { return get_object().find(key); }

	template<typename Key>
	object_iterator find(const Key& key) { return get_object().find(key); }
	template<typename Key>
	object_const_iterator find(const Key& key) const { return get_object().find(key); }

	object_iterator lower_bound(const key_type& key) { return get_object().lower_bound(key); }
	object_const_iterator lower_bound(const key_type& key) const { return get_object().lower_bound(key); }

	template<typename Key>
	object_iterator lower_bound(const Key& key) { return get_object().lower_bound(key); }
	template<typename Key>
	object_const_iterator lower_bound(const Key& key) const { return get_object().lower_bound(key); }

	object_iterator upper_bound(const key_type& key) { return get_object().upper_bound(key); }
	object_const_iterator upper_bound(const key_type& key) const { return get_object().upper_bound(key); }

	template<typename Key>
	object_iterator upper_bound(const Key& key) { return get_object().upper_bound(key); }
	template<typename Key>
	object_const_iterator upper_bound(const Key& key) const { return get_object().upper_bound(key); }

	json& at(size_type n) { return get_array().at(n); }
	const json& at(size_type n) const { return get_array().at(n); }

	json& at(const key_type& key) { return get_object().at(key); }
	const json& at(const key_type& key) const { return get_object().at(key); }

	template<typename Key>
	json& at(const Key& key) {
		auto iter = find(key);
		if (iter == get_object().end()) {
			throw std::out_of_range("invalid json key");
		}
		return iter->second;
	}

	template<typename Key>
	const json& at(const Key& key) const {
		auto iter = find(key);
		if (iter == get_object().end()) {
			throw std::out_of_range("invalid json key");
		}
		return iter->second;
	}

	json& operator[](size_type n) { return get_array()[n]; }
	const json& operator[](size_type n) const { return get_array()[n]; }

	json& operator[](const key_type& key) {
		return get_object()[key];
	}

	json& operator[](key_type&& key) {
		return get_object()[std::move(key)];
	}

	template<typename T, std::enable_if_t<std::is_constructible_v<key_type, T&&>, int> = 0>
	json& operator[](T&& key) {
		return get_object()[static_cast<key_type>(std::forward<T>(key))];
	}

	template<typename Key>
	const json& operator[](const Key& key) const {
		return at(key);
	}

private:

	static bool __accept(const char*& first, const char* last, uint8_t state);

	template<int m>
	void _stringify(string&, int) const noexcept;


	cont m_value;
};

extern template void json::_stringify<0>(string&, int) const noexcept;
extern template void json::_stringify<1>(string&, int) const noexcept;
extern template void json::_stringify<2>(string&, int) const noexcept;
extern template void json::_stringify<3>(string&, int) const noexcept;

template<size_t idx>
constexpr json_traits::type_at_t<idx>& get(json& it) {
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
constexpr const json_traits::type_at_t<idx>& get(const json& it) {
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
constexpr json_traits::type_at_t<idx>&& get(json&& it) {
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
	return wjr::get<json::type_find_v<T>>(it);
}

template<typename T>
constexpr const T& get(const json& it) {
	return wjr::get<json::type_find_v<T>>(it);
}

template<typename T>
constexpr T&& get(json&& it) {
	return wjr::get<json::type_find_v<T>>(std::move(it));
}

template<size_t idx>
constexpr std::add_pointer_t<json_traits::type_at_t<idx>> get_if(json* it) noexcept {
	if constexpr (idx < 3) {
		return std::get_if<idx>(std::addressof(it->get()));
	}
	else if constexpr (idx < 6) {
		auto ptr = std::get_if<idx>(std::addressof(it->get()));
		if (ptr == nullptr) {
			return nullptr;
		}
		return std::addressof(**ptr);
	}
	else {
		return std::get_if<idx - 3>(std::addressof(it->get()));
	}
}

template<size_t idx>
constexpr std::add_pointer_t<const json_traits::type_at_t<idx>> get_if(const json* it) noexcept {
	if constexpr (idx < 3) {
		return std::get_if<idx>(std::addressof(it->get()));
	}
	else if constexpr (idx < 6) {
		auto ptr = std::get_if<idx>(std::addressof(it->get()));
		if (ptr == nullptr) {
			return nullptr;
		}
		return std::addressof(**ptr);
	}
	else {
		return std::get_if<idx - 3>(std::addressof(it->get()));
	}
}

template<typename  T>
constexpr std::add_pointer_t<T> get_if(json* it) noexcept {
	return get_if<json::type_find_v<T>>(it);
}

template<typename  T>
constexpr std::add_pointer_t<const T> get_if(const json* it) noexcept {
	return get_if<json::type_find_v<T>>(it);
}

template<typename Func, typename...Var, std::enable_if_t<
	std::conjunction_v<std::is_same<remove_cvref_t<Var>, json>...>, int> = 0>
constexpr decltype(auto) visit(Func&& fn, Var&&...var) {
	return std::visit([_Fn = std::forward<Func>(fn)](auto&&...args) {
		return _Fn(json::value(std::forward<decltype(args)>(args))...);
		}, (std::forward<Var>(var).get())...);
}

inline void swap(json& a, json& b) noexcept {
	a.swap(b);
}

extern bool operator==(const json& a, const json& b);
inline bool operator!=(const json& a, const json& b) { return !(a == b); }

_WJR_END

#endif // __WJR_JSON_H