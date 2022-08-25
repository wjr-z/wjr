#ifndef __WJR_MTAG_H
#define __WJR_MTAG_H

#include <type_traits>
#include "mtype_traits.h"

namespace wjr::tag {

	template<typename...Args>
	struct tag_set;

	template<typename T>
	struct is_tag_set {
		constexpr static bool value = std::is_base_of_v<tag_set<T>, T>;
	};

	template<typename...Args>
	struct is_tag_set<tag_set<Args...>> : std::true_type {};

	template<typename T> 
	constexpr bool is_tag_set_v = is_tag_set<T>::value;

	template<typename T, typename...Args>
	constexpr bool in(T, tag_set<Args...>) {
		return (std::is_same_v<T, Args> || ...);
	}

	template<typename...Args1, typename...Args2>
	constexpr bool subset(tag_set<Args1...>, tag_set<Args2...>) {
		return (in(Args1{}, tag_set<Args2...>{}) && ...);
	}

	template<typename...Args>
	struct tag_set {
		template<typename...Other>
		constexpr bool operator()(tag_set<Other...>)const {
			return subset(tag_set<Other...>{}, tag_set<Args...>{});
		}
		template<typename T>
		constexpr bool operator[](T) const {
			return in(T{}, tag_set<Args...>{});
		}
	};

	template<typename T, typename U>
	struct __front_bind_tag_set;

	template<typename T, typename...Args>
	struct __front_bind_tag_set<T, tag_set<Args...>> {
		using type = tag_set<T, Args...>;
	};

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) __cup(tag_set<Args1...>, tag_set<Args2...>);

	template<typename T, typename...Args1, typename...Args2>
	constexpr decltype(auto) __cup(tag_set<T, Args1...>, tag_set<Args2...>) {
		using __tag = decltype(__cup(tag_set<Args1...>{}, tag_set<Args2...>{}));
		if constexpr (in(T{}, tag_set<Args2...>{})) {
			return __tag{};
		}
		else {
			return typename __front_bind_tag_set<T, __tag>::type{};
		}
	}

	template<typename...Args2>
	constexpr decltype(auto) __cup(tag_set<>, tag_set<Args2...>) {
		return tag_set<Args2...>{};
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) cup(tag_set<Args1...>, tag_set<Args2...>) {
		return __cup(tag_set<Args1...>{}, tag_set<Args2...>{});
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) operator|(tag_set<Args1...>, tag_set<Args2...>) {
		return cup(tag_set<Args1...>{}, tag_set<Args2...>{});
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) __setminus(tag_set<Args1...>, tag_set<Args2...>);

	template<typename T, typename...Args1, typename...Args2>
	constexpr decltype(auto) __setminus(tag_set<T, Args1...>, tag_set<Args2...>) {
		using __tag = decltype(__setminus(tag_set<Args1...>{}, tag_set<Args2...>{}));
		if constexpr (in(T{}, tag_set<Args2...>{})) {
			return __tag{};
		}
		else {
			return typename __front_bind_tag_set<T, __tag>::type{};
		}
	}

	template<typename...Args2>
	constexpr decltype(auto) __setminus(tag_set<>, tag_set<Args2...>) {
		return tag_set<>{};
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) setminus(tag_set<Args1...>, tag_set<Args2...>) {
		return __setminus(tag_set<Args1...>{}, tag_set<Args2...>{});
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) operator-(tag_set<Args1...>, tag_set<Args2...>) {
		return setminus(tag_set<Args1...>{}, tag_set<Args2...>{});
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) cap(tag_set<Args1...>, tag_set<Args2...>) {
		return tag_set<Args1...>{} - (tag_set<Args1...>{} - tag_set<Args2...>{});
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) operator&(tag_set<Args1...>, tag_set<Args2...>) {
		return cap(tag_set<Args1...>{}, tag_set<Args2...>{});
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) bigoplus(tag_set<Args1...>, tag_set<Args2...>) {
		return (tag_set<Args1...>{} - tag_set<Args2...>{}) | (tag_set<Args2...>{} - tag_set<Args1...>{});
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) operator^(tag_set<Args1...>, tag_set<Args2...>) {
		return bigoplus(tag_set<Args1...>{}, tag_set<Args2...>{});
	}

	template<typename...Args1, typename...Args2>
	constexpr bool operator==(tag_set<Args1...>, tag_set<Args2...>) {
		return tag_set<Args1...>{}(tag_set<Args2...>{}) && tag_set<Args2...>{}(tag_set<Args1...>{});
	}

	template<typename...Args1, typename...Args2>
	constexpr bool operator!=(tag_set<Args1...>, tag_set<Args2...>) {
		return !(tag_set<Args1...>{} == tag_set<Args2...>{});
	}

	template<typename T, typename U>
	struct _Bind_tag {};

	template<typename T, typename...Args>
	constexpr decltype(auto) __cross_join(T, tag_set<Args...>) {
		return tag_set<_Bind_tag<T, Args>...>{};
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) cross_join(tag_set<Args1...>, tag_set<Args2...>) {
		return (__cross_join(Args1{}, tag_set<Args2...>{}) | ...);
	}

	template<typename...Args1, typename...Args2>
	constexpr decltype(auto) operator*(tag_set<Args1...>, tag_set<Args2...>) {
		return cross_join(tag_set<Args1...>{}, tag_set<Args2...>{});
	}

#define REGISTER_WJR_TAG(name)													\
		struct name##_tag : tag_set<name##_tag> {};								\
		inline constexpr name##_tag name;

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
	REGISTER_WJR_TAG(exact);
	REGISTER_WJR_TAG(_unsigned);
	REGISTER_WJR_TAG(_signed);
	REGISTER_WJR_TAG(cache);

#undef REGISTER_WJR_TAG

}

#endif