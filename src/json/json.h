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

#include "../generic/mString.h"
#include "../generic/sptr_wrapper.h"
#include "json_tool.h"

namespace wjr {

	template<typename json_traits>
	class basic_json;

	struct json_mode {
		enum flags {
			is_null = 0x00,
			is_array = 0x01,
			is_object = 0x02,
			is_to_json = 0x04,
			is_from_json = 0x08,
			is_sequence = 0x10,
			is_associative = 0x20
		};

		constexpr static bool can_to_json(flags f) {
			auto n = static_cast<uint8_t>(f);
			return (n & is_to_json) || ((n & (is_array | is_object)) && (n & (is_sequence | is_associative)));
		}
		constexpr static bool can_from_json(flags f) {
			auto n = static_cast<uint8_t>(f);
			return (n & is_from_json) || ((n & (is_array | is_object)) && (n & (is_sequence | is_associative)));
		}
	};

	template<typename T>
	struct simple_json_info {
		constexpr static json_mode::flags mode = json_mode::is_null;
	};

#define REGISTER_STL_JSON_MODE(STL_NAME,INDEX)										\
	template<typename...Args>														\
	struct simple_json_info<STL_NAME<Args...>> {									\
		static constexpr auto mode = INDEX;											\
	};

	REGISTER_STL_JSON_MODE(std::vector, json_mode::is_array | json_mode::is_sequence);
	REGISTER_STL_JSON_MODE(std::deque, json_mode::is_array | json_mode::is_sequence);
	REGISTER_STL_JSON_MODE(std::list, json_mode::is_array | json_mode::is_sequence);
	REGISTER_STL_JSON_MODE(std::forward_list, json_mode::is_array | json_mode::is_sequence);
	REGISTER_STL_JSON_MODE(std::set, json_mode::is_array | json_mode::is_associative);
	REGISTER_STL_JSON_MODE(std::unordered_set, json_mode::is_array | json_mode::is_associative);
	REGISTER_STL_JSON_MODE(std::multiset, json_mode::is_array | json_mode::is_associative);
	REGISTER_STL_JSON_MODE(std::unordered_multiset, json_mode::is_array | json_mode::is_associative);

	REGISTER_STL_JSON_MODE(std::map, json_mode::is_object | json_mode::is_associative);
	REGISTER_STL_JSON_MODE(std::multimap, json_mode::is_object | json_mode::is_associative);
	REGISTER_STL_JSON_MODE(std::unordered_map, json_mode::is_object | json_mode::is_associative);
	REGISTER_STL_JSON_MODE(std::unordered_multimap, json_mode::is_object | json_mode::is_associative);

	template<sptr_wrapper_mode Mode>
	struct default_json_traits {
		using json_type = basic_json<default_json_traits>;
		using null = std::nullptr_t;
		using boolean = bool;
		using number = double;

		using string = String;
		using array = std::vector<json_type, mallocator<json_type>>;
		using object = std::map<string, json_type, std::less<string>,
			mallocator<std::pair<const string, json_type>>>;

		constexpr static
			sptr_wrapper_mode mode = Mode;

		using string_wrapper = sptr_wrapper<string, mode>;
		using array_wrapper = sptr_wrapper<array, mode>;
		using object_wrapper = sptr_wrapper<object, mode>;

		using json_func = typename wjr::json_func;

	};

	template<typename json_traits>
	struct json_container {
		using type = std::variant<
			typename json_traits::null,
			typename json_traits::boolean,
			typename json_traits::number,
			typename json_traits::string_wrapper,
			typename json_traits::array_wrapper,
			typename json_traits::object_wrapper
		>;
	};

	template<typename json_traits>
	using json_container_t = typename json_container<json_traits>::type;

	template<typename Json, typename T>
	struct json_index_map {
		constexpr static size_t value = Json::undefined_v;
	};
	template<typename Json, size_t index>
	struct json_type_map {
		using type = void;
	};

#define REGISTER_JSON_INDEX_AND_TYPE_MAP(NAME,INDEX)				\
	template<typename Json>											\
	struct json_index_map<Json, typename Json::NAME>{				\
		static constexpr auto value = INDEX;						\
	};																\
	template<typename Json>											\
	struct json_type_map<Json, INDEX>{								\
		using type = typename Json::NAME;							\
	};

	REGISTER_JSON_INDEX_AND_TYPE_MAP(null, 0);
	REGISTER_JSON_INDEX_AND_TYPE_MAP(boolean, 1);
	REGISTER_JSON_INDEX_AND_TYPE_MAP(number, 2);
	REGISTER_JSON_INDEX_AND_TYPE_MAP(string, 3);
	REGISTER_JSON_INDEX_AND_TYPE_MAP(array, 4);
	REGISTER_JSON_INDEX_AND_TYPE_MAP(object, 5);

	template<typename T>
	using _Is_json_number = is_any_of<T, short, unsigned short, int, unsigned int, long,
		unsigned long, long long, unsigned long long, float, double>;

	template<typename T>
	constexpr static bool _Is_json_number_v = _Is_json_number<T>::value;

	template<typename Json, typename T, typename...Args>
	struct _Json_get_type_index {

		using null = typename Json::null;
		using boolean = typename Json::boolean;
		using number = typename Json::number;
		using string = typename Json::string;
		using array = typename Json::array;
		using object = typename Json::object;

		constexpr static size_t get_index() {
			using type = std::decay_t<T>;
			if constexpr (sizeof...(Args) == 0 && (is_any_of_v<type, null, boolean>
				|| _Is_json_number_v<type>)) {
				if constexpr (is_any_of_v<type, null, boolean>)
					return Json::template index_map_v<type>;
				else return Json::template index_map_v<number>;
			}
			else if constexpr (std::is_constructible_v<string, T, Args...>) {
				return Json::template index_map_v<string>;
			}
			else if constexpr (std::is_constructible_v<array, T, Args...>) {
				return Json::template index_map_v<array>;
			}
			else if constexpr (std::is_constructible_v<object, T, Args...>) {
				return Json::template index_map_v<object>;
			}
			else {
				return Json::undefined_v;
			}
		}

		constexpr static size_t index = get_index();
	};

	template<typename json_traits, typename...Args>
	struct _Can_to_json : std::conditional_t<
		_Json_get_type_index<basic_json<json_traits>, Args...>::index == basic_json<json_traits>::undefined_v &&
		json_mode::can_to_json(basic_json<json_traits>::template json_info<Args...>::mode)
		, std::true_type, std::false_type> {};

	template<typename json_traits, typename...Args>
	constexpr static bool _Can_to_json_v = _Can_to_json<json_traits, Args...>::value;

	template<typename json_traits, typename T, typename U, typename...Args>
	struct _Can_assign_from_json_helper {
		constexpr static bool value = std::disjunction_v<std::is_assignable<T, U>
			, _Can_assign_from_json_helper<json_traits, T, Args...>>;
	};

	template<typename json_traits, typename T, typename U>
	struct _Can_assign_from_json_helper<json_traits, T, U> {
		constexpr static bool value = std::is_assignable_v<T, U>;
	};

	template<typename json_traits, typename T>
	struct _Can_assign_from_json {
		constexpr static bool value =
			_Can_assign_from_json_helper<json_traits, T,
			typename basic_json<json_traits>::null,
			typename basic_json<json_traits>::boolean,
			typename basic_json<json_traits>::number,
			typename basic_json<json_traits>::string,
			typename basic_json<json_traits>::array,
			typename basic_json<json_traits>::object>::value;
	};

	template<typename json_traits, typename T, typename...Args>
	struct _Can_from_json : std::conditional_t<
		json_mode::can_from_json(basic_json<json_traits>::template json_info<T, Args...>::mode)
		|| (sizeof...(Args) == 0 && _Can_assign_from_json<json_traits, T>::value), std::true_type, std::false_type> {};

	template<typename json_traits, typename...Args>
	constexpr static bool _Can_from_json_v = _Can_from_json<json_traits, Args...>::value;

#define JSON_EXPLICIT explicit

	template<typename json_traits>
	class basic_json {
	private:
		using _Container = json_container_t<json_traits>;

	public:

		template<typename..._Args>
		struct json_info {
		private:
			template<typename Enable, typename Json_traits, typename...Args>
			struct _Is_to_json : std::false_type {};

			template<typename Json_traits, typename...Args>
			struct _Is_to_json<std::void_t<decltype(to_json(std::declval<basic_json<Json_traits>&>(),
				std::declval<Args>()...))>, Json_traits, Args...> : std::true_type {};

			template<typename Enable, typename Json_traits, typename...Args>
			struct _Is_from_json : std::false_type {};

			template<typename Json_traits, typename...Args>
			struct _Is_from_json<std::void_t<decltype(from_json(std::declval<const basic_json<Json_traits>&>(),
				std::declval<Args>()...))>, Json_traits, Args...> : std::true_type {};

			constexpr static decltype(auto) get_json_mode() {
				uint8_t _mode = 0;
				if constexpr (sizeof...(_Args) == 1) {
					using T = first_parameter_t<_Args...>;
					_mode |= simple_json_info<std::decay_t<T>>::mode;
				}
				if constexpr (_Is_to_json<void, json_traits, _Args...>::value) {
					_mode |= json_mode::is_to_json;
				}
				if constexpr (_Is_from_json<void, json_traits, _Args...>::value) {
					_mode |= json_mode::is_from_json;
				}
				return static_cast<json_mode::flags>(_mode);
			}
		public:
			constexpr static json_mode::flags mode = get_json_mode();
		};

		using traits = json_traits;
		using json_func = typename traits::json_func;

		template<typename T>
		using index_map = json_index_map<basic_json<json_traits>, T>;
		template<size_t index>
		using type_map = json_type_map<basic_json<json_traits>, index>;

		template<typename T>
		constexpr static size_t
			index_map_v = index_map<T>::value;
		template<size_t index>
		using type_map_t = typename type_map<index>::type;

		using null = typename traits::null;
		using boolean = typename traits::boolean;
		using number = typename traits::number;
		using string = typename traits::string;
		using array = typename traits::array;
		using object = typename traits::object;

		constexpr static size_t undefined_v = std::variant_npos;
		constexpr static size_t null_v = index_map_v<null>;
		constexpr static size_t boolean_v = index_map_v<boolean>;
		constexpr static size_t number_v = index_map_v<number>;
		constexpr static size_t string_v = index_map_v<string>;
		constexpr static size_t array_v = index_map_v<array>;
		constexpr static size_t object_v = index_map_v<object>;

		template<typename T>
		using normalize_type = type_map_t<index_map_v<T>>;

		basic_json() = default;
		basic_json(const basic_json&) = default;
		basic_json(basic_json&&) noexcept = default;

		basic_json& operator=(const basic_json&) = default;
		basic_json& operator=(basic_json&&) noexcept = default;

		~basic_json() = default;

		template<typename T, typename...Args, std::enable_if_t<
			_Json_get_type_index<basic_json, T, Args...>::index != undefined_v, int> = 0>
		JSON_EXPLICIT basic_json(T&& value, Args&&...args)
			: m_value(std::in_place_index_t<_Json_get_type_index<basic_json, T, Args...>::index>{},
				std::forward<T>(value), std::forward<Args>(args)...) {}

		template<size_t _Idx, typename...Args, std::enable_if_t<
			std::is_constructible_v<_Container, std::in_place_index_t<_Idx>, Args...>, int> = 0>
		JSON_EXPLICIT basic_json(std::in_place_index_t<_Idx>, Args&&...args)
			: m_value(std::in_place_index_t<_Idx>{}, std::forward<Args>(args)...) {}

		template<typename T, typename...Args, std::enable_if_t<
			std::is_constructible_v<_Container, std::in_place_type_t<T>, Args...>, int> = 0>
		JSON_EXPLICIT basic_json(std::in_place_type_t<T>, Args&&...args)
			: m_value(std::in_place_type_t<T>{}, std::forward<Args>(args)...) {}

		template<typename T, typename...Args, std::enable_if_t<
			_Can_to_json_v<json_traits, T, Args...>, int> = 0>
		JSON_EXPLICIT basic_json(T&& value, Args&&...args) noexcept;

		template<typename T, typename...Args, std::enable_if_t<
			_Can_to_json_v<json_traits, T, Args...>, int> = 0>
		basic_json& assign(T&& value, Args&&...args) noexcept;

		template<typename U, std::enable_if_t<
			std::is_assignable_v<_Container, U> &&
			!std::is_same_v<std::decay_t<U>, basic_json>, int> = 0>
		basic_json& operator=(U&& value) noexcept {
			m_value = std::forward<U>(value);
			return *this;
		}

		template<typename T, std::enable_if_t<
			_Is_json_number_v<T>, int> = 0>
		basic_json& operator=(T value) noexcept {
			m_value = static_cast<double>(value);
			return *this;
		}

		template<typename T, std::enable_if_t<
			_Can_to_json_v<json_traits, T>, int> = 0>
		basic_json& operator=(T&& value) noexcept;

		constexpr size_t index()const { return m_value.index(); }
		const char* type_name()const;

		template<typename T, typename...Args>
		constexpr T& set(Args&& ...args);

		template<typename T>
		constexpr bool is()const;

		template<typename T>
		constexpr T& get_ref();

		template<typename T>
		constexpr const T& get_ref()const;

		template<typename T>
		constexpr const T& get_const_ref();

		template<typename T>
		constexpr const T& get_const_ref()const;

		template<typename T>
		constexpr T& get_mutable_ref();

		template<typename T, typename...Args,
			std::enable_if_t<_Can_from_json_v<json_traits, T&&, Args&&...>, int> = 0>
		decltype(auto) get_to(T&&, Args&&...)const;

		template<typename T, std::enable_if_t<_Can_from_json_v<json_traits, T&>, int> = 0>
		T get()const;

		_Container& get_variant() { return m_value; }
		const _Container& get_variant()const { return m_value; }

		basic_json& operator[](const string& key) {
			return get_mutable_ref<object>()[key];
		}
		basic_json& operator[](string&& key) {
			return get_mutable_ref<object>()[std::move(key)];
		}
		basic_json& operator[](size_t index) {
			return get_mutable_ref<array>()[index];
		}
		const basic_json& operator[](size_t index)const {
			return get_const_ref<array>()[index];
		}
		const basic_json& operator[](const string& key)const {
			return get_const_ref<object>().at(key);
		}
		const basic_json& operator[](string&& key)const {
			return get_const_ref<object>().at(std::move(key));
		}

		basic_json& at(const string& key) {
			return get_mutable_ref<object>().at(key);
		}
		const basic_json& at(const string& key)const {
			return get_const_ref<object>(key);
		}

		basic_json& at(size_t index) {
			return get_mutable_ref<array>().at(index);
		}
		const basic_json& at(size_t index)const {
			return get_const_ref<array>().at(index);
		}

		template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
		constexpr static bool accept(iter first, iter last);

		template<typename container, std::enable_if_t<wjr_is_container_v<container>, int> = 0 >
		constexpr static bool accept(container&& cont) {
			return accept(std::begin(cont), std::end(cont));
		}

		template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
		static basic_json parse(iter first, iter last);

		template<typename container, std::enable_if_t<wjr_is_container_v<container>, int> = 0 >
		static basic_json parse(container&& cont) {
			return parse(std::begin(cont), std::end(cont));
		}

		string stringify(int tab = 2)const;
		string minify()const;

		template<typename Char, typename Traits>
		friend std::basic_ostream<Char, Traits>& operator<<(
			std::basic_ostream<Char, Traits>& os, const basic_json& json) {
			os << json.stringify(os.width());
			return os;
		}

	private:

		template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
		constexpr static bool _Accept(iter& first, iter last, uint8_t state);

		template<typename iter>
		void dfs_parse(iter& first, iter last);

		void _minify(string&)const;

		static void format_tab(string& str, int tab);

		void stringify(string& str, int tab, int delta)const;

		_Container m_value;
	};

	template<typename json_traits>
	template<typename T, typename...Args, std::enable_if_t<
		_Can_to_json_v<json_traits, T, Args...>, int> >
	basic_json<json_traits>::basic_json(T&& value, Args&&...args) noexcept {
		assign(std::forward<T>(value), std::forward<Args>(args)...);
	}

	template<typename json_traits>
	template<typename T, typename...Args, std::enable_if_t<
		_Can_to_json_v<json_traits, T, Args...>, int> >
	basic_json<json_traits>& basic_json<json_traits>::assign(T&& value, Args&&...args) noexcept {
		constexpr auto mode = (uint8_t)json_info<T, Args...>::mode;
		if constexpr (mode & json_mode::is_to_json) {
			to_json(*this, std::forward<T>(value), std::forward<Args>(args)...);
		}
		else if constexpr (mode & json_mode::is_array) {
			if (this->index() != array_v) {
				set<array>(std::begin(value), std::end(value));
			}
			else {
				auto& arr = get_ref<array>();
				arr.assign(std::begin(value), std::end(value));
			}
		}
		else if constexpr (mode & json_mode::is_object) {
			if (this->index() != object_v) {
				set<object>(std::begin(value), std::end(value));
			}
			else {
				auto& obj = get_ref<object>();
				obj.clear();
				obj.insert(std::begin(value), std::end(value));
			}
		}
		return *this;
	}

	template<typename json_traits>
	template<typename T, std::enable_if_t<
		_Can_to_json_v<json_traits, T>, int> >
	basic_json<json_traits>& basic_json<json_traits>::operator=(T&& value) noexcept {
		return assign(std::forward<T>(value));
	}

	template<typename json_traits>
	typename basic_json<json_traits>::
		string basic_json<json_traits>::stringify(int tab)const {
		string str;
		if (!tab) {
			_minify(str);
		}
		else {
			stringify(str, 0, tab);
		}
		return str;
	}

	template<typename json_traits>
	typename basic_json<json_traits>::
		string basic_json<json_traits>::minify()const {
		string str;
		_minify(str);
		return str;
	}

	template<typename json_traits>
	void basic_json<json_traits>::_minify(string& str)const {
		switch (this->index()) {
		case null_v: {
			str.append("null");
			break;
		}
		case boolean_v: {
			str.append(get_ref<boolean>() ? "true" : "false");
			break;
		}
		case number_v: {
			json_func::write(str, get_ref<number>());
			break;
		}
		case string_v: {
			str.multiple_append('"', String_view(get_ref<string>()), '"');
			break;
		}
		case array_v: {
			str.push_back('[');
			const auto& umap = get_ref<array>();
			bool head = true;
			for (auto& i : umap) {
				if (head) head = false;
				else str.push_back(',');
				i._minify(str);
			}
			str.push_back(']');
			break;
		}
		case object_v: {
			str.push_back('{');
			auto& obj = get_ref<object>();
			bool head = true;
			for (const auto& [name, val] : obj) {
				if (head) head = false;
				else str.push_back(',');
				str.multiple_append('"', name, "\":");
				val._minify(str);
			}
			str.push_back('}');
			break;
		}
		default: {
			assert(false);
			break;
		}
		}
	}

	template<typename json_traits>
	void basic_json<json_traits>::format_tab(string& str, int tab) {
		str.append(tab, ' ');
	}

	template<typename json_traits>
	void basic_json<json_traits>::stringify(string& str, int tab, int delta)const {
		switch (this->index()) {
		case null_v: {
			str.append("null");
			break;
		}
		case boolean_v: {
			str.append(get_ref<boolean>() ? "true" : "false");
			break;
		}
		case number_v: {
			json_func::write(str, get_ref<number>());
			break;
		}
		case string_v: {
			str.multiple_append('"', String_view(get_ref<string>()), '"');
			break;
		}
		case array_v: {
			str.push_back('[');
			const auto& umap = get_ref<array>();
			if (!umap.empty()) {
				bool head = true;
				for (auto& i : umap) {
					if (head) head = false;
					else str.push_back(',');
					str.push_back('\n');
					format_tab(str, tab + 2);
					i.stringify(str, tab + delta, delta);
				}
				str.push_back('\n');
				format_tab(str, tab);
			}
			str.push_back(']');
			break;
		}
		case object_v: {
			str.push_back('{');
			auto& obj = get_ref<object>();
			if (!obj.empty()) {
				bool head = true;
				for (const auto& [name, val] : obj) {
					if (head) head = false;
					else str.push_back(',');
					str.push_back('\n');
					format_tab(str, tab + delta);
					str.multiple_append('"', name, "\": ");
					val.stringify(str, tab + delta, delta);
				}
				str.push_back('\n');
				format_tab(str, tab);
			}
			str.push_back('}');
			break;
		}
		default: {
			assert(false);
			break;
		}
		}
	}

	template<typename json_traits>
	const char* basic_json<json_traits>::type_name()const {
		switch (this->index()) {
		case null_v: {
			return "null";
		}
		case boolean_v: {
			return "boolean";
		}
		case number_v: {
			return "number";
		}
		case string_v: {
			return "string";
		}
		case array_v: {
			return "array";
		}
		case object_v: {
			return "object";
		}
		case undefined_v: {
			return "undefined";
		}
		default: {
			return "error";
		}
		}
	}

	template<typename json_traits>
	template<typename T, typename...Args>
	constexpr T& basic_json<json_traits>::set(Args&& ... args) {
		constexpr size_t _Index = index_map_v<T>;
		return get_sptr_ref(m_value.template emplace<_Index>(std::forward<Args>(args)...));
	}

	template<typename json_traits>
	template<typename T>
	constexpr bool basic_json<json_traits>::is()const {
		constexpr size_t _Index = index_map_v<T>;
		return index() == _Index;
	}

	template<typename json_traits>
	template<typename T>
	constexpr T& basic_json<json_traits>::get_ref() {
		constexpr size_t _Index = index_map_v<T>;
		return get_sptr_ref(std::get<_Index>(m_value));
	}

	template<typename json_traits>
	template<typename T>
	constexpr const T& basic_json<json_traits>::get_ref()const {
		constexpr size_t _Index = index_map_v<T>;
		return get_sptr_ref(std::get<_Index>(m_value));
	}

	template<typename json_traits>
	template<typename T>
	constexpr const T& basic_json<json_traits>::get_const_ref() {
		constexpr size_t _Index = index_map_v<T>;
		return get_sptr_ref(std::get<_Index>(m_value));
	}

	template<typename json_traits>
	template<typename T>
	constexpr const T& basic_json<json_traits>::get_const_ref() const {
		constexpr size_t _Index = index_map_v<T>;
		return get_sptr_ref(std::get<_Index>(m_value));
	}

	template<typename json_traits>
	template<typename T>
	constexpr T& basic_json<json_traits>::get_mutable_ref() {
		constexpr size_t _Index = index_map_v<T>;
		return get_sptr_mutable_ref(std::get<_Index>(m_value));
	}

	template<typename json_traits>
	template<typename T, typename...Args, std::enable_if_t<_Can_from_json_v<json_traits, T&&, Args&&...>, int> >
	decltype(auto) basic_json<json_traits>::get_to(T&& value, Args&&...args)const {
		constexpr auto mode = (uint8_t)json_info<T, Args...>::mode;
		if constexpr (mode & json_mode::is_from_json) {
			return from_json(std::forward<T>(value), std::forward<Args>(args)...);
		}
		else if constexpr (sizeof...(Args) == 0 && _Can_assign_from_json<json_traits, T&&>::value) {
			return std::forward<T>(value) = get_ref<T>();
		}
		else if constexpr (mode & json_mode::is_sequence) {
			value.clear();
			const auto& arr = get_const_ref<array>();
			for (const auto& i : arr) {
				value.emplace_back(i);
			}
			return std::forward<T>(value);
		}
		else if constexpr (mode & json_mode::is_associative) {
			value.clear();
			const auto& obj = get_const_ref<object>();
			for (auto&& [name, item] : obj) {
				value.emplace(std::forward<decltype(name)>(name),
					std::forward<decltype(item)>(item));
			}
			return std::forward<T>(value);
		}
	}

	template<typename json_traits>
	template<typename T, std::enable_if_t<_Can_from_json_v<json_traits, T&>, int> >
	T basic_json<json_traits>::get() const {
		T value;
		return std::move(get_to(value));
	}

	template<typename json_traits>
	template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> >
	basic_json<json_traits> basic_json<json_traits>::parse(iter first, iter last) {
		basic_json it;
		it.dfs_parse(first, last);
		return it;
	}

	template<typename json_traits>
	template<typename iter>
	void basic_json<json_traits>::dfs_parse(iter& first, iter last) {
		first = json_func::skip_whitespace(first, last);
		switch (*first) {
		case 'n': {
			set<null>();
			first += 4;
			break;
		}
		case 't': {
			set<boolean>(true);
			first += 4;
			break;
		}
		case 'f': {
			set<boolean>(false);
			first += 5;
			break;
		}
		case '{': {
			auto& obj = set<object>();
			++first;
			first = json_func::skip_whitespace(first, last);
			if (*first == '}') {
				++first;
				break;
			}
			while (true) {
				first = json_func::skip_whitespace(first, last);
				++first;
				auto p = json_func::skip_string(first, last);
				auto& item = obj[string(first, p)];
				first = json_func::skip_whitespace(p + 1, last);
				++first;
				item.dfs_parse(first, last);
				first = json_func::skip_whitespace(first, last);
				if (*first == '}') {
					++first;
					break;
				}
				++first;
			}
			break;
		}
		case '[': {
			auto& arr = set<array>();
			++first;
			first = json_func::skip_whitespace(first, last);
			if (*first == ']') {
				++first;
				break;
			}
			for (;;) {
				arr.emplace_back();
				arr.back().dfs_parse(first, last);
				first = json_func::skip_whitespace(first, last);
				if (*first == ']') {
					++first;
					break;
				}
				++first;
			}
			arr.shrink_to_fit();
			break;
		}
		case '"': {
			++first;
			auto t = json_func::skip_string(first, last);
			set<string>(first, t);
			first = t + 1;
			break;
		}
		default: {
			double val = read_number(first, last);
			set<number>(val);
			break;
		}
		}
	}

	template<typename json_traits>
	template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int>>
	constexpr bool basic_json<json_traits>::accept(iter first, iter last) {
		first = json_func::skip_whitespace(first, last);

		if (first == last)
			return false;

		switch (*first) {
		case '[':
			++first;
			if (!_Accept(first, last, ']'))
				return false;
			first = json_func::skip_whitespace(first, last);
			return first == last;
		case '{':
			++first;
			if (!_Accept(first, last, '}'))
				return false;
			first = json_func::skip_whitespace(first, last);
			return first == last;
		default:
			return false;
		}
	}

	template<typename json_traits>
	template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int>>
	constexpr bool basic_json<json_traits>::_Accept(iter& first, iter last, uint8_t state) {
		bool head = true;
		for (;; ++first) {
			first = json_func::skip_whitespace(first, last);
			if (first == last)return false;

			if (state == '}') {
				if (*first == '}') {
					if (!head) return false;
					++first;
					return true;
				}
				if (*first != '"')return false;
				if (!json_func::check_string(first, last))
					return false;
				++first;
				first = json_func::skip_whitespace(first, last);
				if (first == last || *first != ':') return false;
				++first;
				first = json_func::skip_whitespace(first, last);
				if (first == last) return false;
			}
			else {
				if (*first == ']') {
					if (!head)return false;
					++first;
					return true;
				}
			}
			head = false;

			switch (*first) {
			case 'n':
				if (first + 3 < last && *(first + 1) == 'u' && *(first + 2) == 'l' && *(first + 3) == 'l') {
					first += 4;
					break;
				}
				return false;
			case 't':
				if (first + 3 < last && *(first + 1) == 'r' && *(first + 2) == 'u' && *(first + 3) == 'e') {
					first += 4;
					break;
				}
				return false;
			case 'f':
				if (first + 4 < last && *(first + 1) == 'a'
					&& *(first + 2) == 'l' && *(first + 3) == 's' && *(first + 4) == 'e') {
					first += 5;
					break;
				}
				return false;
			case '"':
				if (!json_func::check_string(first, last))
					return false;
				++first;
				break;
			case '[':
				++first;
				if (!_Accept(first, last, ']'))
					return false;
				break;
			case '{':
				++first;
				if (!_Accept(first, last, '}'))
					return false;
				break;
			default:
				if (!check_number(first, last))
					return false;
				break;
			}

			first = json_func::skip_whitespace(first, last);
			if (*first == ',')
				continue;
			if (*first != state)
				return false;
			++first;
			return true;
		}
	}

	using json = basic_json<default_json_traits<sptr_wrapper_mode::unique>>;
	using shared_json = basic_json<default_json_traits<sptr_wrapper_mode::shared>>;

	template<typename T>
	struct is_basic_json : std::false_type {};

	template<typename json_traits>
	struct is_basic_json<basic_json<json_traits>> : std::true_type {};

	template<typename Func, typename...Args,
		std::enable_if_t<std::conjunction_v<is_basic_json<std::decay_t<Args>>...>, int> = 0>
	constexpr decltype(auto) visit(Func&& fn, Args&&...args) {
		return std::visit([t = forward_wrapper_t<Func>(std::forward<Func>(fn))](auto&&...x) mutable{
			return t(std::forward<decltype(x)>(x)...); }, (std::forward<Args>(args).get_variant())...);
	}

	template<typename traits_1, typename traits_2>
	struct default_json_less {
		template<typename T, typename U>
		bool operator()(const T& lhs, const U& rhs) const {
			return std::less<>{}(lhs, rhs);
		}
		bool operator()(
			const typename basic_json<traits_1>::number& lhs,
			const typename basic_json<traits_2>::number& rhs
			)const {
			return lhs < rhs - std::numeric_limits<double>::epsilon();
		}
	};

	template<typename traits_1, typename traits_2>
	struct _Json_equal {

		template<typename T, typename U>
		bool operator()(const T& lhs, const U& rhs)const {
			return false;
		}

		bool operator()(
			const typename basic_json<traits_1>::null& lhs,
			const typename basic_json<traits_2>::null& rhs
			)const {
			return true;
		}

		bool operator()(
			const typename basic_json<traits_1>::boolean& lhs,
			const typename basic_json<traits_2>::boolean& rhs
			)const {
			return std::equal_to<>{}(lhs, rhs);
		}

		bool operator()(
			const typename basic_json<traits_1>::number& lhs,
			const typename basic_json<traits_2>::number& rhs
			)const {
			return fabs(lhs - rhs) < std::numeric_limits<double>::epsilon();
		}

		bool operator()(
			const typename basic_json<traits_1>::string& lhs,
			const typename basic_json<traits_2>::string& rhs
			)const {
			return std::equal_to<>{}(lhs.get(), rhs.get());
		}

		bool operator()(
			const typename basic_json<traits_1>::array& lhs,
			const typename basic_json<traits_2>::array& rhs
			)const {
			const auto& l = lhs.get();
			const auto& r = rhs.get();
			return std::equal(std::begin(l), std::end(l),
				std::begin(r));
		}

		bool operator()(
			const typename basic_json<traits_1>::object& lhs,
			const typename basic_json<traits_2>::object& rhs
			)const {
			const auto& l = lhs.get();
			const auto& r = rhs.get();
			return std::equal(std::begin(l), std::end(l),
				std::begin(r));
		}

	};

	template<typename traits_1, typename traits_2>
	struct _Json_lexicographical_compare {

		template<typename T, typename U>
		bool operator()(const T& lhs, const U& rhs)const {
			return false;
		}

		bool operator()(
			const typename basic_json<traits_1>::null& lhs,
			const typename basic_json<traits_2>::null& rhs
			)const {
			return false;
		}

		bool operator()(
			const typename basic_json<traits_1>::boolean& lhs,
			const typename basic_json<traits_2>::boolean& rhs
			)const {
			return std::less<>{}(lhs, rhs);
		}

		bool operator()(
			const typename basic_json<traits_1>::number& lhs,
			const typename basic_json<traits_2>::number& rhs
			)const {
			return lhs < rhs - std::numeric_limits<double>::epsilon();
		}

		bool operator()(
			const typename basic_json<traits_1>::string& lhs,
			const typename basic_json<traits_2>::string& rhs
			)const {
			return std::less<>{}(lhs.get(), rhs.get());
		}

		bool operator()(
			const typename basic_json<traits_1>::array& lhs,
			const typename basic_json<traits_2>::array& rhs
			)const {
			const auto& l = lhs.get();
			const auto& r = rhs.get();

			return std::lexicographical_compare(
				std::begin(l), std::end(l),
				std::begin(l), std::end(r)
			);
		}

		bool operator()(
			const typename basic_json<traits_1>::object& lhs,
			const typename basic_json<traits_2>::object& rhs
			)const {
			const auto& l = lhs.get();
			const auto& r = rhs.get();

			auto _First1 = std::begin(l);
			auto _Last1 = std::end(l);
			auto _First2 = std::begin(r);
			auto _Last2 = std::end(r);
			for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, ++_First2) {
				if (std::less<>{}(_First1->first, _First2->first)) {
					return true;
				}
				else if (std::less<>{}(_First2->first, _First1->first)) {
					return false;
				}

				if (std::less<>{}(_First1->second, _First2->second)) {
					return true;
				}
				else if (std::less<>{}(_First2->second, _First1->second)) {
					return false;
				}
			}
			return _First1 == _Last1 && _First2 != _Last2;
		}

	};

	template<typename traits_1, typename traits_2>
	bool operator==(const basic_json<traits_1>& lhs, const basic_json<traits_2>& rhs) {
		return lhs.index() == rhs.index() &&
			visit(_Json_equal<traits_1, traits_2>{}, lhs, rhs);
	}

	template<typename traits_1, typename traits_2>
	bool operator!=(const basic_json<traits_1>& lhs, const basic_json<traits_2>& rhs) {
		return !(lhs == rhs);
	}

	template<typename traits_1, typename traits_2>
	bool operator<(const basic_json<traits_1>& lhs, const basic_json<traits_2>& rhs) {
		return lhs.index() != rhs.index() ? lhs.index() < rhs.index() :
			visit(_Json_lexicographical_compare<traits_1, traits_2>{}, lhs, rhs);
	}

	template<typename traits_1, typename traits_2>
	bool operator>(const basic_json<traits_1>& lhs, const basic_json<traits_2>& rhs) {
		return rhs < lhs;
	}

	template<typename traits_1, typename traits_2>
	bool operator<=(const basic_json<traits_1>& lhs, const basic_json<traits_2>& rhs) {
		return !(rhs > lhs);
	}

	template<typename traits_1, typename traits_2>
	bool operator>=(const basic_json<traits_1>& lhs, const basic_json<traits_2>& rhs) {
		return !(rhs < lhs);
	}

	inline json operator"" _json(const char* str, const size_t n) {
		return json::parse(str, str + n);
	}

}

#endif
