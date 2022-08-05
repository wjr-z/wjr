#ifndef __WJR_MTAG_H
#define __WJR_MTAG_H

#include <type_traits>
#include "mtype_traits.h"

namespace wjr {
	namespace tag {
		struct __base_tag {};

		template<typename T>
		struct is_tag {
			constexpr static bool value = std::is_base_of_v<__base_tag, T>;
		};

		template<typename T>
		constexpr bool is_tag_v = is_tag<T>::value;

		template<typename...Args>
		struct tag_set : __base_tag {
			using tag = tag_set<Args...>;
		};

		template<typename T>
		struct tag_set<T> : __base_tag {
			using tag = tag_set<T>;
		private:
			template<typename...Args>
			constexpr bool is(tag_set<Args...>) const {
				return (... || T{}(Args{}));
			}
		public:
			template<typename U, std::enable_if_t<is_tag_v<U>, int> = 0>
			constexpr bool operator()(U) const {
				return is(typename U::tag{});
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

		template<typename...Args1, typename T, typename...Args2>
		constexpr decltype(auto) union_tag(tag_set<Args1...>, tag_set<T, Args2...>) {
			using type = decltype(union_tag(tag_set<Args1...>{}, tag_set<Args2...>{}));
			if constexpr (wjr_is_any_of_v<T, Args1...>) {
				return type{};
			}
			else {
				return __tag_set<T, type>{}();
			}
		}

		template<typename...Args1>
		constexpr decltype(auto) union_tag(tag_set<Args1...>, tag_set<>) {
			return tag_set<Args1...>{};
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

		template<typename...Args1>
		struct _Bind_tag : __base_tag {
			
			// bind servral tags to one tag
			template<typename...Args2>
			friend struct _Bind_tag;
			using tag = tag_set<_Bind_tag<Args1...>>;
		private:
			template<typename T>
			constexpr bool is(T) const {
				if constexpr (sizeof...(Args1) != 1) {
					return false;
				}
				else {
					return (... || Args1{}(T{}));
				}
			}
			template<typename...Args2>
			constexpr bool is(tag_set<Args2...>) const {
				return (... || operator()(Args2{}));
			}
			template<typename T, typename...Args2>
			constexpr bool judge(T, tag_set<_Bind_tag<Args2...>>) const {
				return (... || T{}(Args2{}));
			}
			template<typename...Args2>
			constexpr bool judge(tag_set<_Bind_tag<Args2...>>) const {
				return (... && judge(Args1{}, tag_set<_Bind_tag<Args2...>>{}));
			}
			template<typename...Args2>
			constexpr bool is(tag_set<_Bind_tag<Args2...>>) const {
				if constexpr (sizeof...(Args1) != sizeof...(Args2)) {
					return false;
				}
				else {
					return (... && judge(Args1{}, tag_set<_Bind_tag<Args2...>>{}))
						&& _Bind_tag<Args2...>{}.judge(tag_set<_Bind_tag<Args2...>>{});
				}
			}
		public:
			template<typename T, std::enable_if_t<is_tag_v<T>, int> = 0>
			constexpr bool operator()(T) const {
				return is(typename T::tag{});
			}
		};

		template<typename...Args>
		constexpr decltype(auto) __bind_tag(tag_set<Args...>) {
			return tag_set<_Bind_tag<Args...>>{};
		}

		template<typename T, typename U>
		constexpr decltype(auto) __bind_tag(T, U) {
			return tag_set<_Bind_tag<T, U>>{};
		}

		template<typename T, typename...Args2>
		constexpr decltype(auto) __bind_tag(T, tag_set<_Bind_tag<Args2...>>) {
			return tag_set<_Bind_tag<T, Args2...>>{};
		}

		template<typename...Args1, typename U>
		constexpr decltype(auto) __bind_tag(tag_set<_Bind_tag<Args1...>>, U) {
			return tag_set<_Bind_tag<Args1..., U>>{};
		}

		template<typename...Args1, typename...Args2>
		constexpr decltype(auto) __bind_tag(tag_set<_Bind_tag<Args1...>>, tag_set<_Bind_tag<Args2...>>) {
			return tag_set<_Bind_tag<Args1..., Args2...>>{};
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
		struct name : __base_tag {												\
			using tag = tag_set<name>;											\
		private:																\
			template<typename...Args>											\
			constexpr bool is(tag_set<Args...>) const {							\
				return wjr_is_any_of_v<name, Args...>;							\
			}																	\
		public:																	\
			template<typename T, std::enable_if_t<is_tag_v<T>, int> = 0>		\
			constexpr bool operator()(T) const{									\
				return is(typename T::tag{});									\
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

#undef REGISTER_WJR_TAG

	}

}

#endif