#ifndef __WJR_MTAG_H
#define __WJR_MTAG_H

#include <type_traits>

namespace wjr {
	namespace tag {
		struct __base_tag {};

		template<typename T>
		struct is_tag {
			constexpr static bool value = std::is_base_of_v<__base_tag, T>;
		};

		template<typename T>
		constexpr bool is_tag_v = is_tag<T>::value;
		
		template<typename T, typename U>
		struct __combine_tag : T, U {
			template<typename C, std::enable_if_t<is_tag_v<C>, int> = 0>
			constexpr bool operator()(C) {
				return std::is_base_of_v<A, C> && std::is_base_of_v<B, C>;
			}
		};

		template<typename T, typename U, std::enable_if_t<
			std::conjunction_v<tag::is_tag<T>, tag::is_tag<U>>, int> = 0>
		constexpr __combine_tag<T, U> operator|(T, U) {
			return {};
		}

#define REGISTER_WJR_TAG(name)													\
		struct name : __base_tag {												\
			template<typename T, std::enable_if_t<tag::is_tag_v<T>, int> = 0>	\
			constexpr bool operator()(T) const{									\
				return std::is_base_of_v<name, T>;								\
			}																	\
		};																		\
		inline constexpr name name##_tag;

#define REGISTER_WJR_NEGATION_TAG(name1, name2)									\
		struct name2;															\
		struct name1 : __base_tag {												\
			template<typename T, std::enable_if_t<tag::is_tag_v<T>, int> = 0>	\
			constexpr bool operator()(T) const{									\
				return std::is_base_of_v<name1, T> &&							\
					!std::is_base_of_v<name2, T>;								\
			}																	\
		};																		\
		inline constexpr name1 name1##_tag;										\
		struct name2 : __base_tag {												\
			template<typename T, std::enable_if_t<tag::is_tag_v<T>, int> = 0>	\
			constexpr bool operator()(T) const{									\
				return std::is_base_of_v<name2, T> &&							\
					!std::is_base_of_v<name1, T>;								\
			}																	\
		};																		\
		inline constexpr name2 name2##_tag;										
		

		REGISTER_WJR_TAG(empty);
		REGISTER_WJR_NEGATION_TAG(uninitialized, initialized);
		REGISTER_WJR_NEGATION_TAG(unreserved, reserved);
		REGISTER_WJR_NEGATION_TAG(dif_address, same_address);
		REGISTER_WJR_NEGATION_TAG(left, right);
		REGISTER_WJR_TAG(overlap);
		REGISTER_WJR_TAG(forward_overlap);
		REGISTER_WJR_TAG(backwrad_overlap);

#undef REGISTER_WJR_NEGATION_TAG
#undef REGISTER_WJR_TAG

	}

}

#endif