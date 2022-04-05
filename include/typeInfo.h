#ifndef __WJR_TYPEINFO_H
#define __WJR_TYPEINFO_H

#include <array>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wjr {
	// easy reflection

	template<typename T>
	struct Field {
	public:

		constexpr Field(const char* name, T&& value)
			:name(name), value(std::forward<T>(value)) {
		}

		constexpr Field(const Field& other)
			: name(other.name), value(other.value) {
		}

		constexpr Field(Field&& other)noexcept
			:name(other.name), value(other.value) {
		}

		Field& operator=(const Field&) = delete;

		template<typename U>
		decltype(auto) get_value(U&& data) const {
			return std::forward<U>(data).*value;
		}

		constexpr std::string_view getName()const {
			return name;
		}

		const T& get_value()const {
			return value;
		}

		const std::string_view name;
		const T value;
	};

	template<typename... Args>
	class FieldList {
	public:

		constexpr static size_t size =
			sizeof...(Args);

		constexpr FieldList(Args&&... List)
			:_List(std::forward<Args>(List)...) {
		}

		FieldList(const FieldList&) = delete;

		FieldList& operator=(const FieldList&) = delete;

		constexpr size_t find(const std::string_view _Name)const {
			return find_index<size - 1>(_Name);
		}

		template<size_t index>
		constexpr decltype(auto) getField()const {
			return std::get<index>(_List);
		}

		template<typename T, typename U>
		constexpr bool setField(T&& _Class, const std::string_view _Name, U&& _Value)const {
			return dfsSetField<T, U, size - 1>(
				std::forward<T>(_Class), _Name, std::forward<U>(_Value));
		}

		template<typename Fn>
		constexpr void forEach(Fn&& fn) const {
			dfsForEach<Fn, 0>(std::forward<Fn>(fn));
		}

		template<typename Fn>
		constexpr void forEach(const std::string_view& _Name, Fn&& fn)const {
			dfsForEach<Fn, 0>(_Name, std::forward<Fn>(fn));
		}

	private:

		template<size_t n>
		constexpr size_t find_index(const std::string_view& _Name)const {
			if constexpr (n == -1)return -1;
			else {
				if (std::get<n>(_List).getName() == _Name)return n;
				return find_index<n - 1>(_Name);
			}
		}

		template<typename T, typename U, size_t n>
		constexpr bool dfsSetField(T&& _Class, const std::string_view& _Name, U&& _Value)const {
			if constexpr (n == -1)return false;
			else {
				auto val = std::get<n>(_List);
				if (val.getName() == _Name) {
					val.get_value(std::forward<T>(_Class))
						= std::forward<U>(_Value);
					return true;
				}
				return dfsSetField<T, U, n - 1>(
					std::forward<T>(_Class), _Name, std::forward<U>(_Value));
			}
		}

		template<typename Fn, size_t n>
		constexpr void dfsForEach(Fn&& fn)const {
			if constexpr (n == size)return;
			else {
				std::forward<Fn>(fn)(std::get<n>(_List));
				dfsForEach<Fn, n + 1>(std::forward<Fn>(fn));
			}
		}

		template<typename Fn, size_t n>
		constexpr void dfsForEach(const std::string_view& _Name, Fn&& fn)const {
			if constexpr (n == size) return;
			else {
				if (std::get<n>(_List).getName() == _Name) {
					std::forward<Fn>(fn)(std::get<n>(_List));
					return;
				}
				dfsForEach<Fn, n + 1>(_Name, std::forward<Fn>(fn));
			}
		}

		const std::tuple<Args...> _List;
	};

	enum : unsigned int {
		empty_container = 0,
		default_json = 0x01,
		array_json = 0x02,
		object_json = 0x04,
	};

	template<typename T>
	class basic_typeInfo {
	public:
		constexpr static size_t container_tag = empty_container;
		constexpr static std::string_view name = "";
		constexpr static FieldList fields = {};
	};

	template<typename T>
	class typeInfo : public basic_typeInfo<T> {
	};

	template<typename T, typename U>
	static bool setField(T&& _Class, const std::string_view& _Name, U&& _Value) {
		static_assert(decltype(typeInfo<std::decay_t<T>>::fields)::size != 0);
		return typeInfo<std::decay_t<T>>::fields.setField(
			std::forward<T>(_Class), _Name, std::forward<U>(_Value));
	}

	template<typename T, size_t n>
	class typeInfo<std::array<T, n>> : public basic_typeInfo<std::array<T, n>> {
	public:
		constexpr static size_t container_tag = array_json;
		constexpr static std::string_view name = "array";
	};

	template<typename T, typename Alloc>
	class typeInfo<std::vector<T, Alloc>> : public basic_typeInfo < std::vector<T, Alloc> > {
	public:
		constexpr static size_t container_tag = array_json;
		constexpr static std::string_view name = "vector";
	};

	template<typename T, typename Alloc>
	class typeInfo<std::list<T, Alloc>> : public basic_typeInfo<std::list<T, Alloc>> {
	public:
		constexpr static size_t container_tag = array_json;
		constexpr static std::string_view name = "list";
	};

	template<typename T, typename Alloc>
	class typeInfo<std::forward_list<T, Alloc>> : public basic_typeInfo<std::forward_list<T, Alloc>> {
	public:
		constexpr static size_t container_tag = array_json;
		constexpr static std::string_view name = "forward_list";
	};

	template<typename T, typename Alloc>
	class typeInfo<std::deque<T, Alloc>> : public basic_typeInfo<std::deque<T, Alloc>> {
	public:
		constexpr static size_t container_tag = array_json;
		constexpr static std::string_view name = "deque";
	};

	template<typename _Ty, typename _Pr, typename _Alloc>
	class typeInfo<std::set<_Ty, _Pr, _Alloc>> :
		public basic_typeInfo<std::set<_Ty, _Pr, _Alloc>> {
	public:
		constexpr static size_t container_tag = array_json;
		constexpr static std::string_view name = "set";
	};

	template<typename _Ty, typename _Hasher, typename _Keyeq, typename _Alloc>
	class typeInfo<std::unordered_set<_Ty, _Hasher, _Keyeq, _Alloc>> :
		public basic_typeInfo<std::unordered_set<_Ty, _Hasher, _Keyeq, _Alloc>> {
	public:
		constexpr static size_t container_tag = array_json;
		constexpr static std::string_view name = "unordered_set";
	};

	template<typename _Ty, typename _Pr, typename _Alloc>
	class typeInfo<std::multiset<_Ty, _Pr, _Alloc>> :
		public basic_typeInfo<std::multiset<_Ty, _Pr, _Alloc>> {
	public:
		constexpr static size_t container_tag = array_json;
		constexpr static std::string_view name = "multiset";
	};

	template<typename _Key, typename _Ty, typename _Pr, typename _Alloc>
	class typeInfo<std::map<_Key, _Ty, _Pr, _Alloc>> :
		public basic_typeInfo<std::map<_Key, _Ty, _Pr, _Alloc>> {
	public:
		constexpr static size_t container_tag = object_json;
		constexpr static std::string_view name = "map";
	};

	template<typename _Key, typename _Ty, typename _Hasher, typename _Keyeq, typename _Alloc>
	class typeInfo<std::unordered_map<_Key, _Ty, _Hasher, _Keyeq, _Alloc>> :
		public basic_typeInfo<std::unordered_map<_Key, _Ty, _Hasher, _Keyeq, _Alloc>> {
	public:
		constexpr static size_t container_tag = object_json;
		constexpr static std::string_view name = "unordered_map";
	};

	template<typename _Key, typename _Ty, typename _Pr, typename _Alloc>
	class typeInfo<std::multimap<_Key, _Ty, _Pr, _Alloc>> :
		public basic_typeInfo<std::multimap<_Key, _Ty, _Pr, _Alloc>> {
	public:
		constexpr static size_t container_tag = object_json;
		constexpr static std::string_view name = "multimap";
	};
}

#endif