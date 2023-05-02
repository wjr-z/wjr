#pragma once
#ifndef __WJR_SWITCH_H
#define __WJR_SWITCH_H

#include <wjr/tp_list.h>
#include <wjr/math.h>

_WJR_BEGIN

struct switch_fallthrough {};
struct switch_default {};

// Use these to control cases

namespace __switch_details {

#define __WJR_SWITCH_STRATEGY_CASE(X)	                                                                                \
	case (tp_at_t<__clist, (X)>::value) : {																				\
		if constexpr((X) < tp_size_v<IndexList>){	                                                                    \
			constexpr bool __is_fallthrough = tp_contains_v<tp_at_t<StateList, (X)>, switch_fallthrough>;	            \
			if constexpr (__is_fallthrough){	                                                                        \
				(void)(func(tp_size_t<(X)>{}, std::forward<Args>(args)...));	                                        \
			}else {	                                                                                                    \
				return static_cast<Ret>(func(tp_size_t<(X)>{}, std::forward<Args>(args)...));	                        \
				WJR_UNREACHABLE;	                                                                                    \
			}	                                                                                                        \
		}	                                                                                                            \
		else if constexpr(!Default){WJR_UNREACHABLE;}	                                                                \
		WJR_FALLTHROUGH;	                                                                                            \
	}

#define __WJR_CALL1(CASE, I)     \
CASE(I)
#define __WJR_CALL2(CASE, I)     \
__WJR_CALL1(CASE, I);	         \
__WJR_CALL1(CASE, I + 1)
#define __WJR_CALL4(CASE, I)     \
__WJR_CALL2(CASE, I);	         \
__WJR_CALL2(CASE, I + 2)
#define __WJR_CALL8(CASE, I)     \
__WJR_CALL4(CASE, I);	         \
__WJR_CALL4(CASE, I + 4)
#define __WJR_CALL16(CASE, I)    \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 8)
#define __WJR_CALL32(CASE, I)    \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 16)
#define __WJR_CALL64(CASE, I)    \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 32)
#define __WJR_CALL128(CASE, I)   \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 64)
#define __WJR_CALL256(CASE, I)   \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 128)
#define __WJR_CALL512(CASE, I)   \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 256)
#define __WJR_CALL1024(CASE, I)  \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 512)
#define __WJR_CALL2048(CASE, I)  \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 1024)
#define __WJR_CALL4096(CASE, I)  \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 2048)
#define __WJR_CALL8192(CASE, I)  \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 4096)
#define __WJR_CALL16384(CASE, I) \
__WJR_CALL8(CASE, I);	         \
__WJR_CALL8(CASE, I + 8192)

#define __WJR_SWITCH_STRATEGY_CASES(N)	                                                 \
	switch(token){	                                                                     \
		WJR_MACRO_CONCAT(__WJR_CALL, N)(__WJR_SWITCH_STRATEGY_CASE, 0)	                 \
	default:{	                                                                         \
		if constexpr(Default){	                                                         \
			return static_cast<Ret>(func(switch_default{}, std::forward<Args>(args)...));\
		}	                                                                             \
	WJR_UNREACHABLE;	                                                                 \
	}	                                                                                 \
	}

	template<int S>
	struct switch_strategy;

	template<size_t I>
	struct __complete_helper {
		template<typename T>
		using fn = tp_equal_c<T, tp_size_t<I>>;
	};

	template<size_t N, size_t I, typename IndexList>
	constexpr auto __complete() {
		if constexpr (N == tp_size_v<IndexList>) return IndexList{};
		else {
			if constexpr (tp_find_if_v<IndexList, __complete_helper<I>::template fn> != -1) {
				return __complete<N, I + 1, IndexList>();
			}
			else {
				return __complete<N, I + 1, tp_push_back_t<IndexList, tp_size_t<I>>>();
			}
		}
	}

#define __WJR_SWITCH_STRATEGY(N)	                                                                     \
	template<>	                                                                                         \
	struct switch_strategy<(base2_width<unsigned int>(N) - 1)> {	                                     \
		template<typename Ret, typename IndexList, bool Default, typename StateList,	                 \
			typename Token, typename Func, typename...Args>	                                             \
		inline constexpr static Ret invoke(Token token, Func func, Args&&...args) {						 \
			constexpr auto __value = __complete<(N), 0, IndexList>();	                                 \
			using __clist = WJR_PRIMITIVE_TYPE(__value);	                                             \
			__WJR_SWITCH_STRATEGY_CASES(N);	                                                             \
		}	                                                                                             \
	};

	WJR_MACRO_CALL(__WJR_SWITCH_STRATEGY, , 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384);

#undef __WJR_CALL1
#undef __WJR_CALL2
#undef __WJR_CALL4
#undef __WJR_CALL8
#undef __WJR_CALL16
#undef __WJR_CALL32
#undef __WJR_CALL64
#undef __WJR_CALL128
#undef __WJR_CALL256
#undef __WJR_CALL512
#undef __WJR_CALL1024
#undef __WJR_CALL2048
#undef __WJR_CALL4096
#undef __WJR_CALL8192
#undef __WJR_CALL16384
#undef __WJR_SWITCH_STRATEGY_CASE
#undef __WJR_SWITCH_STRATEGY

}

// You can customize the case value of the I th by using IndexList.
// And you can specify whether the I th case falls through, if tp_at_t<StateList, I> contains switch_fallthrough¡£
// Currently, a maximum of 16384 cases are supported. Please note that table jumping for continuous values 
// is currently not supported, which may result in increased compilation time for many cases.However, 
// compared to the 16384 functions themselves, this has little impact.
// Purpose:
// 1. It is convenient to implement variant visits.
// 2. Can be used to achieve prediction and jump in wjr/parse/LL1.h
template<typename IndexList, bool Default, typename StateList>
struct switch_invoke_fn {
	static_assert(tp_size_v<IndexList> == tp_size_v<StateList>, "");
	constexpr static size_t size() { return tp_size_v<IndexList>; }
	static_assert(size() >= 1, "");
private:
	template<typename T>
	using __remove_if = tp_contains<tp_at_t<StateList, tp_front_t<T>::value>, switch_fallthrough>;
	constexpr static int __strategy = base2_width<unsigned int>(size() - 1);
	static_assert(size() <= ((size_t)1 << __strategy), "");
	template<typename T, typename Func, typename...Args>
	constexpr static auto __get_ret_list() {
		if constexpr (Default) {
			return tp_push_back_t<T, std::invoke_result_t<Func, switch_default, Args&&...>>{};
		}
		else return T{};
	}
public:

	template<typename Token, typename Func, typename...Args>
	inline constexpr decltype(auto) operator()(Token token, Func func, Args&&...args) const {
		using RetList = tp_transform_f<tp_iota_t<0, size()>,
			tp_bind<std::invoke_result_t, Func, tp_arg<0>, Args&&...>>;
		using ZipFilterRetList = tp_remove_if_t<tp_zip_t<tp_list, tp_iota_t<0, size()>, RetList>, __remove_if>;
		using _FilterRetList = tp_transform_t<ZipFilterRetList, tp_back_t>;
		constexpr auto __value = __get_ret_list<_FilterRetList, Func, Args&&...>();
		using FilterRetList = WJR_PRIMITIVE_TYPE(__value);
		using UniqueRetList = tp_unique_t<FilterRetList>;
		if constexpr (tp_is_empty_v<UniqueRetList>) {
			return __switch_details::switch_strategy<__strategy>
				::template invoke<void, IndexList, Default, StateList>(token,
					func, std::forward<Args>(args)...);
		}
		else {
			static_assert(tp_size_v<UniqueRetList> == 1, "");
			using Ret = tp_front_t<UniqueRetList>;
			return __switch_details::switch_strategy<__strategy>
				::template invoke<Ret, IndexList, Default, StateList>(token,
					func, std::forward<Args>(args)...);
		}
	}
};

template<typename IndexList, bool Default = false,
	typename StateList = tp_repeat_t<tp_list<tp_list<>>, tp_size_v<IndexList>>>
inline constexpr switch_invoke_fn<IndexList, Default, StateList> switch_invoke{};

_WJR_END

#endif // __WJR_SWITCH_H