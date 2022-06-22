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

	struct default_json_traits;

	template<typename json_traits>
	class basic_json;

	enum class generic_json_mode : uint8_t {
		is_null			= 0x00,
		is_array		= 0x01,
		is_object		= 0x02,
		is_to_json		= 0x04,
		is_from_json	= 0x08,
		masks			= 0xff
	};

	template<typename T>
	struct generic_json_simple_info {
		constexpr static generic_json_mode mode = generic_json_mode::is_null;
	};

	template<typename T>
	struct generic_json_detail_info {
		// to_json and from_json
	};

	template<typename T>
	struct generic_json_traits : 
		public generic_json_simple_info<T>, public generic_json_detail_info<T> {
	private:
		using Base1 = generic_json_simple_info<T>;
		using Base2 = generic_json_detail_info<T>;

		template<typename json_traits, typename U, typename = void>
		struct _Is_to_json : std::false_type {};

		template<typename json_traits, typename U>
		struct _Is_to_json<typename json_traits, U, std::void_t<decltype(
			Base2::to_json(std::declval<basic_json<json_traits>&>(), std::declval<U>()))>> : std::true_type{};

		template<typename json_traits, typename U, typename = void>
		struct _Is_from_json : std::false_type {};

		template<typename json_traits, typename U>
		struct _Is_from_json<typename json_traits, U, std::void_t<decltype(
			Base2::from_json(std::declval<const basic_json<json_traits>&>(), std::declval<U>()))>> : std::true_type{};

		template<typename json_traits, typename U>
		constexpr static decltype(auto) get_json_mode() {
			auto mode = Base1::mode;
			if constexpr (_Is_to_json<json_traits, U>::value) {
				mode |= generic_json_mode::is_to_json;
			} 
			if constexpr (_Is_from_json<json_traits, U>::value) {
				mode |= generic_json_mode::is_from_json;
			}
			return mode;
		}

	public:
		template<typename json_traits, typename U>
		constexpr static generic_json_mode mode = get_json_mode<json_traits, U>();
	};
	
#define REGISTER_STL_JSON_ARRAY(STL_NAME)											\
	template<typename...Args>														\
	struct generic_json_simple_info<STL_NAME<Args...>> {							\
		static constexpr generic_json_mode mode = generic_json_mode::is_array;		\
	};

#define REGISTER_STL_JSON_OBJECT(STL_NAME)											\
	template<typename...Args>														\
	struct generic_json_simple_info<STL_NAME<Args...>> {							\
		static constexpr generic_json_mode mode = generic_json_mode::is_object;		\
	};

	REGISTER_STL_JSON_ARRAY(std::vector);
	REGISTER_STL_JSON_ARRAY(std::deque);
	REGISTER_STL_JSON_ARRAY(std::list);
	REGISTER_STL_JSON_ARRAY(std::forward_list);
	REGISTER_STL_JSON_OBJECT(std::set);
	REGISTER_STL_JSON_OBJECT(std::unordered_set);
	REGISTER_STL_JSON_OBJECT(std::map);
	REGISTER_STL_JSON_OBJECT(std::unordered_map);


	struct default_json_traits {
		using json_type			= basic_json<default_json_traits>;
		using json_null			= std::nullptr_t;
		using json_boolean		= bool;
		using json_number		= double;
		using json_string		= String;
		using json_array		= std::vector<json_type, mallocator<json_type>>;
		using json_object		= std::map<json_string, json_type, std::less<json_string>, 
								mallocator<std::pair<const json_string, json_type>>>;
		
		using json_container = std::variant<json_null, json_boolean, 
								json_number, json_string, json_array, json_object>;
	};

	template<typename json_traits>
	class basic_json : public json_traits::json_container {
		using Base				= typename json_traits::json_container;
	public:
		using traits			= json_traits;
		using json_type			= typename traits::json_type;
		using json_null			= typename traits::json_null;
		using json_boolean		= typename traits::json_boolean;
		using json_number		= typename traits::json_number;
		using json_string		= typename traits::json_string;
		using json_array		= typename traits::json_array;
		using json_object		= typename traits::json_object;

		static constexpr size_t json_undefined_v = std::variant_npos;
		static constexpr size_t json_null_v      = 0;
		static constexpr size_t json_boolean_v   = 1;
		static constexpr size_t json_number_v    = 2;
		static constexpr size_t json_string_v    = 3;
		static constexpr size_t json_array_v     = 4;
		static constexpr size_t json_object_v    = 5;

		using Base::Base;
		using Base::operator=;

		basic_json() = default;
		basic_json(const basic_json&) = default;
		basic_json(basic_json&&) noexcept = default;
		template<typename T, std::enable_if_t<
			generic_json_traits<std::decay_t<T>>::template
			mode<json_traits, T&&> != generic_json_mode::is_null, int> = 0>
		explicit basic_json(T&& value);

		basic_json& operator=(const basic_json&) = default;
		basic_json& operator=(basic_json&&) noexcept = default;
		template<typename T, std::enable_if_t<
			generic_json_traits<std::decay_t<T>>::template
			mode<json_traits, T&&> != generic_json_mode::is_null, int> = 0>
		basic_json& operator=(T&& value);

		~basic_json() = default;

		template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
		static bool accept(iter first, iter last);

		template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
		static basic_json parse(iter first, iter last);

		json_string stringify(int tab = 2)const;
		json_string minify()const;

		const char* type_name()const;

		basic_json& operator[](const json_string& key) {
			return std::get<json_object>(*this)[key];
		}
		basic_json& operator[](json_string&& key) {
			return std::get<json_object>(*this)[std::move(key)];
		}
		basic_json& operator[](size_t index) {
			return std::get<json_array>(*this)[index];
		}
		const basic_json&operator[](size_t index)const {
			return std::get<json_array>(*this)[index];
		}
		const basic_json& operator[](const json_string& key)const {
			return std::get<json_object>(*this).at(key);
		}
		const basic_json& operator[](json_string&& key)const {
			return std::get<json_object>(*this).at(std::move(key));
		}

	private:

		template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> = 0>
		static bool _Accept(iter& first, iter last, uint8_t state);

		template<typename iter>
		[[nodiscard]] iter dfs_parse(iter first, iter last);

		void _minify(json_string&)const;

		static void format_tab(json_string& str, int tab);

		void stringify(json_string& str, int tab, int delta)const;

	};

	template<typename json_traits>
	template<typename T, std::enable_if_t<
		generic_json_traits<std::decay_t<T>>::template
		mode<json_traits, T&&> != generic_json_mode::is_null, int> >
	basic_json<json_traits>::basic_json(T&& value) {
		*this = std::forward<T>(value);
	}

	template<typename json_traits>
	template<typename T, std::enable_if_t<
		generic_json_traits<std::decay_t<T>>::template
		mode<json_traits, T&&> != generic_json_mode::is_null, int> >
	basic_json<json_traits>& basic_json<json_traits>::operator=(T&& value) {
		constexpr auto mode = 
			generic_json_traits<std::decay_t<T>>::template mode<json_traits, T&&>;
		if constexpr ((uint8_t)mode & (uint8_t)generic_json_mode::is_to_json) {
			generic_json_detail_info<T>::to_json(*this, std::forward<T>(value));
		}
		else if constexpr ((uint8_t)mode & (uint8_t)generic_json_mode::is_array) {
			if constexpr (std::is_constructible_v<json_array, T&&>) {
				*this = std::forward<T>(value);
			}
			else {
				this->template emplace<json_array>();
				auto& arr = std::get<json_array>(*this);
				if constexpr (wjr_has_size_v<T&&>) {
					arr.reserve(std::size(value));
				}
				for (auto&& v : value) {
					std::get<json_array>(*this).emplace_back(std::forward<decltype(v)>(v));
				}
			}
		}
		else if constexpr ((uint8_t)mode & (uint8_t)generic_json_mode::is_object) {
			if constexpr (std::is_constructible_v<json_object, T&&>) {
				*this = std::forward<T>(value);
			}
			else {
				this->template emplace<json_object>();
				for (auto&& [name, item] : value) {
					std::get<json_object>(*this)[std::forward<decltype(name)>(name)] 
						= std::forward<decltype(item)>(item);
				}
			}
		}
		return *this;
	}

	
	template<typename json_traits>
	template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int> >
	basic_json<json_traits> basic_json<json_traits>::parse(iter first, iter last) {
		basic_json x;
		x.dfs_parse(first, last);
		return x;
	}

	template<typename json_traits>
	template<typename iter>
	iter basic_json<json_traits>::dfs_parse(iter first, iter last) {
		first = skip_whitespace(first, last);
		switch (*first) {
		case 'n': {
			this->template emplace<json_null>(std::nullptr_t{});
			first += 4;
			break;
		}
		case 't': {
			this->template emplace<json_boolean>(true);
			first += 4;
			break;
		}
		case 'f': {
			this->template emplace<json_boolean>(false);
			first += 5;
			break;
		}
		case '{': {
			this->template emplace<json_object>();
			auto& obj = std::get<json_object>(*this);
			++first;
			first = skip_whitespace(first, last);
			// if is empty
			if (*first == '}') {
				++first;
				break;
			}
			while(true) {
				first = skip_whitespace(first, last);
				++first;
				auto p = json_tool::skip_string(first, last);
				auto& item = obj[json_string(first, p)];
				first = skip_whitespace(p + 1, last);
				++first;
				first = item.dfs_parse(first, last);
				first = skip_whitespace(first, last);
				if (*first == '}') {
					++first;
					break;
				}
				++first;
			}
			break;
		}
		case '[': {
			this->template emplace<json_array>();
			auto& arr = std::get<json_array>(*this);
			++first;
			first = skip_whitespace(first, last);
			if (*first == ']') {
				++first;
				break;
			}
			for (;;) {
				arr.emplace_back();
				first = arr.back().dfs_parse(first, last);
				first = skip_whitespace(first, last);
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
			auto t = json_tool::skip_string(first, last);
			this->template emplace<json_string>(first, t);
			first = t + 1;
			break;
		}
		default: {
			double val = read_number(first, last);
			this->template emplace<json_number>(val);
			break;
		}
		}
		return first;
	}

	template<typename json_traits>
	template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int>>
	bool basic_json<json_traits>::accept(iter first, iter last) {
		first = skip_whitespace(first, last);

		if (first == last) // empty
			return false;

		switch (*first) {
		case '[':
			++first;
			if (!_Accept(first, last, ']'))
				return false;
			first = skip_whitespace(first, last);
			return first == last;
		case '{':
			++first;
			if (!_Accept(first, last, '}'))
				return false;
			first = skip_whitespace(first, last);
			return first == last;
		default:
			return false;
		}
	}

	template<typename json_traits>
	template<typename iter, std::enable_if_t<wjr_is_iterator_v<iter>, int>>
	bool basic_json<json_traits>::_Accept(iter& first, iter last, uint8_t state) {
		bool head = true;
		for (;; ++first) {
			first = skip_whitespace(first, last);
			if (first == last)return false;

			if (state == '}') { // object
				if (*first == '}') {
					if (!head) return false;
					++first;
					return true;
				}
				if (*first != '"')return false;
				if (!json_tool::check_string(first, last))
					return false;
				++first;
				first = skip_whitespace(first, last);
				if (first == last || *first != ':') return false;
				++first;
				first = skip_whitespace(first, last);
				if (first == last) return false;
			}
			else { // array
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
				if (!json_tool::check_string(first, last))
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

			first = skip_whitespace(first, last);
			if (*first == ',')
				continue;
			if (*first != state)
				return false;
			++first;
			return true;
		}
	}

	template<typename json_traits>
	typename basic_json<json_traits>::json_string basic_json<json_traits>::stringify(int tab)const {
		json_string str;
		stringify(str, 0, tab);
		return str;
	}

	template<typename json_traits>
	typename basic_json<json_traits>::json_string basic_json<json_traits>::minify()const {
		json_string str;
		_minify(str);
		return str;
	}

	template<typename json_traits>
	void basic_json<json_traits>::_minify(json_string& str)const {
		switch (this->index()) {
		case json_null_v: {
			str.append("null");
			break;
		}
		case json_boolean_v: {
			str.append(std::get<json_boolean>(*this) ? "true" : "false");
			break;
		}
		case json_number_v: {
			str.append(json_string::fixed_number(std::get<json_number>(*this)));
			break;
		}
		case json_string_v: {
			str.multiple_append('"', String_view(std::get<json_string>(*this)), '"');
			break;
		}
		case json_array_v: {
			str.push_back('[');
			const auto& umap = std::get<json_array>(*this);
			bool head = true;
			for (auto& i : umap) {
				if (head) head = false;
				else str.push_back(',');
				i._minify(str);
			}
			str.push_back(']');
			break;
		}
		case json_object_v: {
			str.push_back('{');
			auto& obj = std::get<json_object>(*this);
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
	void basic_json<json_traits>::format_tab(json_string&str, int tab) {
		str.append(tab, ' ');
	}

	template<typename json_traits>
	void basic_json<json_traits>::stringify(json_string&str,int tab, int delta)const {
		switch (this->index()) {
			case json_null_v : {
				str.append("null");
				break;
			}
			case json_boolean_v : {
				str.append(std::get<json_boolean>(*this) ? "true" : "false");
				break;
			}
			case json_number_v : {
				str.append(json_string::fixed_number(std::get<json_number>(*this)));
				break;
			}
			case json_string_v : {
				str.multiple_append('"', String_view(std::get<json_string>(*this)), '"');
				break;
			}
			case json_array_v: {
				str.push_back('[');
				const auto& umap = std::get<json_array>(*this);
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
			case json_object_v : {
				str.push_back('{');
				auto& obj = std::get<json_object_v>(*this);
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
		case json_null_v: {
			return "null";
		}
		case json_boolean_v: {
			return "boolean";
		}
		case json_number_v: {
			return "number";
		}
		case json_string_v: {
			return "string";
		}
		case json_array_v: {
			return "array";
		}
		case json_object_v: {
			return "object";
		}
		case json_undefined_v: {
			return "undefined";
		}
		default: {
			return "error";
		}
		}
	}

	using json = basic_json<default_json_traits>;
}

#endif
