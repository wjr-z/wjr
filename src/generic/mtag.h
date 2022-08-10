#ifndef __WJR_MTAG_H
#define __WJR_MTAG_H

#include <type_traits>
#include "mtype_traits.h"

namespace wjr::tag {
	struct tag {};

	template<typename T>
	struct is_tag {
		constexpr static bool value = std::is_base_of_v<tag, T>;
	};

	template<typename T>
	constexpr bool is_tag_v = is_tag<T>::value;

	template<typename...Args>
	struct tag_set;

	template<typename...Args1, typename...Args2>
	constexpr bool __check_in(tag_set<Args1...>, tag_set<Args2...>) {
		return (... && wjr_is_any_of_v<Args1, Args2...>);
	}

#define REGISTER_TAG_CHECK_IN	return __check_in(tag{}, typename U::tag{});

	template<typename...Args1>
	struct tag_set : tag {
		using tag = tag_set<Args1...>;
		template<typename U, std::enable_if_t<is_tag_v<U>, int> = 0>
		constexpr bool operator()(U) const {
			REGISTER_TAG_CHECK_IN;
		}
	};

	template<typename T>
	struct tag_set<T> : tag {
		using tag = tag_set<T>;
		template<typename U, std::enable_if_t<is_tag_v<U>, int> = 0>
		constexpr bool operator()(U) const {
			REGISTER_TAG_CHECK_IN;
		}
	};

	template<typename T, typename U>
	struct __tag_set;

	template<typename T, typename...Args>
	struct __tag_set<T, tag_set<Args...>> {
		constexpr tag_set<T, Args...> operator()() const {
			return {};
		}
	};

	template<typename T, typename...Args1, typename...Args2>
	constexpr decltype(auto) union_tag(tag_set<T, Args1...>, tag_set<Args2...>) {
		using type = decltype(union_tag(tag_set<Args1...>{}, tag_set<Args2...>{}));
		if constexpr (wjr_is_any_of_v<T, Args2...>) {
			return type{};
		}
		else {
			return __tag_set<T, type>{}();
		}
	}

	template<typename...Args2>
	constexpr decltype(auto) union_tag(tag_set<>, tag_set<Args2...>) {
		return tag_set<Args2...>{};
	}

	template<typename T, typename...Args1, typename...Args2>
	constexpr decltype(auto) difference_tag(tag_set<T, Args1...>, tag_set<Args2...>) {
		using type = decltype(difference_tag(tag_set<Args1...>{}, tag_set<Args2...>{}));
		if constexpr (wjr_is_any_of_v<T, Args2...>) {
			return type{};
		}
		else {
			return __tag_set<T, type>{}();
		}
	}

	template<typename...Args2>
	constexpr decltype(auto) difference_tag(tag_set<>, tag_set<Args2...>) {
		return tag_set<>{};
	}

	template<typename T, typename U,
		std::enable_if_t<std::conjunction_v<is_tag<T>, is_tag<U>>, int> = 0>
	constexpr decltype(auto) operator|(T, U) {
		using type = decltype(union_tag(typename T::tag{}, typename U::tag{}));
		return type{};
	}

	template<typename T, typename U,
		std::enable_if_t<std::conjunction_v<is_tag<T>, is_tag<U>>, int> = 0>
	constexpr decltype(auto) operator-(T, U) {
		using type = decltype(difference_tag(typename T::tag{}, typename U::tag{}));
		return type{};
	}

	template<typename T, typename U,
		std::enable_if_t<std::conjunction_v<is_tag<T>, is_tag<U>>, int> = 0>
	constexpr decltype(auto) operator&(T, U) {
		return T{} - (T{} - U{});
	}

	template<typename T, typename U,
		std::enable_if_t<std::conjunction_v<is_tag<T>, is_tag<U>>, int> = 0>
	constexpr decltype(auto) operator^(T, U) {
		return (T{} - U{}) | (U{} - T{});
	}

	template<typename...Args>
	struct _Bind_tag : tag {
		using tag = tag_set<_Bind_tag<Args...>>;
		template<typename U, std::enable_if_t<is_tag_v<U>, int> = 0>
		constexpr bool operator()(U) const {
			REGISTER_TAG_CHECK_IN;
		}
	};

	template<typename...Args>
	constexpr decltype(auto) __bind_tag(tag_set<Args...>) {
		return tag_set<_Bind_tag<Args...>>{};
	}

	template<typename T, typename U>
	constexpr decltype(auto) __bind_tag(T, U) {
		return _Bind_tag<T, U>{};
	}

	template<typename T, typename...Args2>
	constexpr decltype(auto) __bind_tag(T, tag_set<_Bind_tag<Args2...>>) {
		return _Bind_tag<T, Args2...>{};
	}

	template<typename...Args1, typename U>
	constexpr decltype(auto) __bind_tag(tag_set<_Bind_tag<Args1...>>, U) {
		return _Bind_tag<Args1..., U>{};
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) __bind_tag(tag_set<_Bind_tag<Args1...>>, tag_set<_Bind_tag<Args2...>>) {
		return _Bind_tag<Args1..., Args2...>{};
	}

	template<typename T, typename...Args2>
	constexpr decltype(auto) __bind_tag1(T, tag_set<Args2...>) {
		return (... | __bind_tag(T{}, Args2{}));
	}

	template<typename T>
	constexpr decltype(auto) __bind_tag1(T, tag_set<>) {
		return T{};
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) bind_tag(tag_set<Args1...>, tag_set<Args2...>) {
		return (... | __bind_tag1(Args1{}, tag_set<Args2...>{}));
	}

	template<typename...Args2>
	constexpr decltype(auto) bind_tag(tag_set<>, tag_set<Args2...>) {
		return tag_set<Args2...>{};
	}

	template<typename T, typename U,
		std::enable_if_t<std::conjunction_v<is_tag<T>, is_tag<U>>, int> = 0>
	constexpr decltype(auto) operator*(T, U) {
		return bind_tag(typename T::tag{}, typename U::tag{});
	}

#define REGISTER_WJR_TAG(name)													\
		struct name : tag {												\
			using tag = tag_set<name>;											\
			template<typename U, std::enable_if_t<is_tag_v<U>, int> = 0>		\
			constexpr bool operator()(U) const{									\
				REGISTER_TAG_CHECK_IN;											\
			}																	\
		};																		\
		inline constexpr name name##_tag;

	REGISTER_WJR_TAG(empty);
	REGISTER_WJR_TAG(uninitialized);
	REGISTER_WJR_TAG(initilized);
	REGISTER_WJR_TAG(reserved);
	REGISTER_WJR_TAG(unreserved);
	REGISTER_WJR_TAG(same_address);
	REGISTER_WJR_TAG(dif_address);
	REGISTER_WJR_TAG(left);
	REGISTER_WJR_TAG(right);
	REGISTER_WJR_TAG(store);

	template<typename T, typename From, typename To,
		std::enable_if_t<std::conjunction_v<is_tag<T>, is_tag<From>, is_tag<To>>, int> = 0>
	constexpr decltype(auto) change(T, From, To) {
		if constexpr (T{}(From{})) {
			return (T{} - From{}) | To{};
		}
		else return T{};
	}

	template<typename T, typename A, typename B,
		std::enable_if_t<std::conjunction_v<is_tag<T>, is_tag<A>, is_tag<B>>, int> = 0>
	constexpr decltype(auto) swap(T, A, B) {
		constexpr bool hasa = T{}(A{});
		constexpr bool hasb = T{}(B{});
		if constexpr (hasa == hasb) {
			return T{};
		}
		else return T{} ^ A{} ^ B{};
	}

#undef REGISTER_WJR_TAG
#undef REGISTER_WJR_TAG

}

#endif