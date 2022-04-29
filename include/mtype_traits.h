#ifndef __WJR_MY_STL_H
#define __WJR_MY_STL_H

#include <cassert>
#include <cstdint>
#include <ctime>
#include <functional>
#include <locale>
#include <random>
#include <type_traits>

#define USE_LIBDIVIDE
#ifdef USE_LIBDIVIDE
#include "libdivide.h"
#else 
namespace libdivide {
	enum Branching {
		BRANCHFULL,  // use branching algorithms
		BRANCHFREE   // use branchfree algorithms
	};
	template<typename T,Branching ALGO = BRANCHFULL>
	using divider = T;
}
#endif


#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace wjr {
#ifndef NDEBUG
	//#define NDEBUG
#endif

#define __USE_THREADS

#if defined(__clang__) || defined(__GNUC__)
#define WJR_CPP_STANDARD __cplusplus
#elif defined(_MSC_VER)
#define WJR_CPP_STANDARD _MSVC_LANG
#endif

#if WJR_CPP_STANDARD >= 199711L
#define WJR_CPP_03
#endif
#if WJR_CPP_STANDARD >= 201103L
#define WJR_CPP_11
#endif
#if WJR_CPP_STANDARD >= 201402L
#define WJR_CPP_14
#endif
#if WJR_CPP_STANDARD >= 201703L
#define WJR_CPP_17
#endif
#if WJR_CPP_STANDARD >= 202002L
#define WJR_CPP_20
#endif

#ifndef is_little_endian
#if defined(__BYTE_ORDER__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define is_little_endian true
#else
#define is_little_endian false
#endif
#else
#define is_little_endian true
#endif
#endif

#if defined(__has_builtin)
#define whas_builtin(x) __has_builtin(x)
#else
#define whas_builtin(x) 0
#endif

#if whas_builtin(__builtin_expect) && !defined(WJR_CPP_20)
#ifndef likely
#define likely(expr) __builtin_expect(!!(expr), 1)
#endif
#ifndef unlikely
#define unlikely(expr) __builtin_expect(!!(expr),0)
#endif
#else
	
#ifndef likely
#define likely(expr) expr
#endif
#ifndef unlikely
#define unlikely(expr) expr
#endif
#endif

#ifdef __USE_THREADS
#define WJR_THREADS (true)
#else
#define WJR_THREADS (false)
#endif

#if WJR_THREADS
#define USE_THREAD_LOCAL thread_local
#else
#define USE_THREAD_LOCAL
#endif

#if defined(KB)
#undef KB
#endif
#define KB *(size_t(1) << 10)

#if defined(MB)
#undef MB
#endif
#define MB *((size_t)(1) << 20)

#if defined(GB)
#undef GB
#endif
#define GB *((size_t)(1) << 30)

#define CONNECT(A,B) A##B
#define STD_FUNCTION(FUNC) std::FUNC

#ifndef _DEBUG 
#define WDEBUG_LEVEL 0
#else 
#define WDEBUG_LEVEL 3
#endif

#define WASSERT_LEVEL_MESSAGE(LEVEL,expression)											\
	do{																					\
		if (unlikely(!(expression))) {													\
				fprintf(stderr,"Assertion failed: %s in %s : %d\nBUG_LEVEL : %d",		\
				#expression,__FILE__,__LINE__,LEVEL);									\
				std::abort();															\
		}																				\
	}while(0)

// WDEBUG_LEVEL		1
// The impact on program running time or space is only constant
// Some small bugs that are not easy to test under a small part or a small range of data
#if WDEBUG_LEVEL >= 1	
#define WASSERT_LEVEL_1(expression)	WASSERT_LEVEL_MESSAGE(1,expression)
#else
#define WASSERT_LEVEL_1(expression)
#endif

// WDEBUG_LEVEL		2
// The impact on program running time or space is only linear or less
// Some bugs that may be found and fixed in small-scale tests
#if WDEBUG_LEVEL >= 2
#define WASSERT_LEVEL_2(expression)	WASSERT_LEVEL_MESSAGE(2,expression)
#else
#define WASSERT_LEVEL_2(expression)
#endif

// WDEBUG_LEVEL		3
// The impact on program running time or space is O(nlogn),O(n sqrt(n)),O(n^2) or less
// It can still run in a large range
#if WDEBUG_LEVEL >= 3
#define WASSERT_LEVEL_3(expression)	WASSERT_LEVEL_MESSAGE(3,expression)
#else
#define WASSERT_LEVEL_3(expression)
#endif

// WDEBUG_LEVEL		4
// no limit
// It can only be tested in a small range
#if WDEBUG_LEVEL >= 4
#define WASSERT_LEVEL_4(expression)	WASSERT_LEVEL_MESSAGE(4,expression)
#else
#define WASSERT_LEVEL_4(expression)
#endif

	enum wbyte_order {
		w_little_endian = 0,
		w_big_endian = 1,
		w_endian = is_little_endian ? w_little_endian : w_big_endian
	};

	inline namespace wjr_type_traits {
		using wjr_ssize_t = std::make_signed_t<size_t>;

		struct wjr_empty_tag {};

		struct wjr_use_threads
			: std::conditional_t<WJR_THREADS, std::true_type, std::false_type> {
		};

		template<typename T>
		struct midentity {
			using type = T;
		};

		template<typename T>
		using midentity_t = typename midentity<T>::type;

		template<template<typename,typename>typename judger,typename...>
		struct is_any_of_helper {
			static constexpr bool value = false;
		};
		
		template<template<typename,typename>typename judger,typename T,typename U>
		struct is_any_of_helper<judger, T, U> {
			static constexpr bool value = judger<T, U>::value;
		};
		
		template<template<typename, typename>typename judger, typename T, typename U, typename... Args>
		struct is_any_of_helper<judger, T, U, Args...> {
			static constexpr bool value = 
				std::disjunction_v<
				is_any_of_helper<judger, T, Args...>
				,is_any_of_helper<judger, U, Args...>>;
		};

		template<typename T,typename...Args>
		struct is_any_of {
			static constexpr bool value = is_any_of_helper<std::is_same, T, Args...>::value;
		};

		template<typename T, typename...args>
		constexpr static bool is_any_of_v = is_any_of_helper<std::is_same,T, args...>::value;
		
		template<typename T, typename = void>
		struct wjr_is_iterator : std::false_type {};

		template<typename T>
		struct wjr_is_iterator<T, std::void_t<typename
			std::iterator_traits<T>::iterator_category>> : std::true_type {};

		template<typename T>
		constexpr static bool wjr_is_iterator_v = wjr_is_iterator<T>::value;

		template<typename T>
		struct is_reverse_iterator : std::false_type {};

		template<typename T>
		struct is_reverse_iterator<std::reverse_iterator<T>> : std::true_type {};

		template<typename T>
		struct _Is_signed_integral :
			std::conjunction<std::is_integral<T>, std::is_signed<T>> {};

		template<typename T>
		constexpr static bool _Is_signed_integral_v = _Is_signed_integral<T>::value;

		template<typename T>
		struct _Is_unsigned_integral :
			std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {};

		template<typename T>
		constexpr static bool _Is_unsigned_integral_v = _Is_unsigned_integral<T>::value;

#if defined(WJR_CPP_17)
		template<typename F, typename...Args>
		struct wjr_result_of {
			using type = std::invoke_result_t<F, Args...>;
		};
#else
		template<typename F, typename ...Args>
		struct wjr_result_of {
			using type = std::result_of_t<F(Args...)>;
		};
#endif

		template<typename F, typename...Args>
		using wjr_result_of_t = typename wjr_result_of<F, Args...>::type;

		template<size_t index,typename T,typename...Args>
		constexpr decltype(auto) wjr_kth_val_helper(T&& val, Args&&...args) {
			if constexpr (index == 0) {
				return std::forward<T>(val);
			}
			else {
				return wjr_kth_val_helper<index - 1>(std::forward<Args>(args)...);
			}				
		}

		template<size_t index,typename...Args,
			std::enable_if_t<(index < sizeof...(Args)),int> = 0>
		constexpr decltype(auto) kth_val(Args&&...args) {
			return wjr_kth_val_helper<index>(std::forward<Args>(args)...);
		}

		template<size_t index,typename T,typename...Args>
		struct wjr_kth_type_helper {
			using type = typename wjr_kth_type_helper<index - 1, Args...>::type;
		};

		template<typename T,typename...Args>
		struct wjr_kth_type_helper<0, T, Args...> {
			using type = T;
		};

		template<size_t index, typename...Args>
		using kth_type = typename wjr_kth_type_helper<index, Args...>::type;

		template<size_t index, typename T>
		struct index_sequence_prepend;

		template<size_t index, size_t...res>
		struct index_sequence_prepend<index, std::index_sequence<res...>> {
			using type = std::index_sequence<index, res...>;
		};

		template<size_t index, typename T>
		using index_sequence_prepend_t = typename index_sequence_prepend<index, T>::type;

		template<size_t cnt, size_t now, size_t...index>
		struct _Make_Complement_Index_Sequence_Helper {
			constexpr decltype(auto) operator()() {
				if constexpr (now == cnt) {
					return std::index_sequence<>{};
				}
				else  if constexpr (std::disjunction_v<
					std::is_same<std::in_place_index_t<now>, std::in_place_index_t<index>>...>) {
					return _Make_Complement_Index_Sequence_Helper<cnt, now + 1, index...>{}();
				}
				else {
					return index_sequence_prepend_t < now,
						decltype(_Make_Complement_Index_Sequence_Helper<cnt, now + 1, index...>{}()) > {};
				}
			}
		};

		template<size_t cnt, typename T>
		struct _Make_complement_index_sequence;

		template<size_t cnt, size_t...res>
		struct _Make_complement_index_sequence<cnt, std::index_sequence<res...>> {
			using type = decltype(_Make_Complement_Index_Sequence_Helper<cnt, 0, res...>{}());
		};

		template<size_t cnt, typename T>
		using make_complement_index_sequence = typename _Make_complement_index_sequence<cnt, T>::type;

		template<typename...Args>
		struct parameter_list {
			template<typename U>
			using pre_bind = parameter_list<U, Args...>;
			template<typename U>
			using nxt_bind = parameter_list<Args..., U>;
		};

		template<typename T, typename...Args>
		struct parameter_list<T, Args...> {
			template<typename U>
			using pre_bind = parameter_list<U, T, Args...>;
			template<typename U>
			using nxt_bind = parameter_list<T, Args..., U>;
			using first_type = T;
			using nxt_parameter = parameter_list<Args...>;
			using last_type = typename nxt_parameter::last_type;
			using pre_parameter = typename nxt_parameter::pre_parameter::template pre_bind<T>;
		};

		template<typename T>
		struct parameter_list<T> {
			template<typename U>
			using pre_bind = parameter_list<U, T>;
			template<typename U>
			using nxt_bind = parameter_list<T, U>;
			using first_type = T;
			using nxt_parameter = parameter_list<>;
			using last_type = T;
			using pre_parameter = parameter_list<>;
		};

		template<size_t, typename...>
		struct parameter_split_two;

		template<size_t index, typename T, typename...Args>
		struct parameter_split_two<index, T, Args...> {
			using Type = parameter_split_two<index - 1, Args...>;
			using first = typename Type::first::template pre_bind<T>;
			using second = typename Type::second;
			using type = parameter_list<first, second>;
		};

		template<typename T, typename...Args>
		struct parameter_split_two<0, T, Args...> {
			using first = parameter_list<>;
			using second = parameter_list<T, Args...>;
			using type = parameter_list<first, second>;
		};

		template<>
		struct parameter_split_two<0> {
			using first = parameter_list<>;
			using second = parameter_list<>;
			using type = parameter_list<first, second>;
		};

		template<size_t index,typename...Args>
		using parameter_split_two_first_type =
			typename parameter_split_two<index, Args...>::first;
		
		template<size_t index,typename...Args>
		using parameter_split_two_second_type =
			typename parameter_split_two<index, Args...>::second;

		template<typename T, typename...Args>
		struct parameter_split;

		template<size_t index, size_t...Args1, typename...Args2>
		struct parameter_split < std::index_sequence<index, Args1...>, Args2...> {
			using split_two_type = parameter_split_two<index, Args2...>;
			template<typename...>
			struct get_type;
			template<size_t...T1, typename...T2>
			struct get_type<std::index_sequence<T1...>, parameter_list<T2...>> {
				using type = typename parameter_split<std::index_sequence<T1...>, T2...>::type;
			};
			using first = typename split_two_type::first;
			using second = typename split_two_type::second;
			using type = typename get_type<std::index_sequence<Args1...>, second>::type::template pre_bind<first>;
		};

		template<size_t index, typename...Args>
		struct parameter_split<std::index_sequence<index>, Args...> {
			using split_two_type = parameter_split_two<index, Args...>;
			using first = typename split_two_type::first;
			using second = typename split_two_type::second;
			using type = typename split_two_type::type;
		};

		template<typename T, typename...Args>
		struct parameter_split_helper {
			using type = wjr_empty_tag;
		};

		template<size_t...index, typename...Args>
		struct parameter_split_helper<std::index_sequence<index...>, Args...> {
			using type = typename parameter_split<std::index_sequence<index...>, Args...>::type;
		};

		template<typename T, typename...Args>
		using parameter_split_t = typename parameter_split_helper<T, Args...>::type;

		template<typename T, typename...Args>
		struct first_parameter {
			using type = T;
		};

		template<typename...Args>
		using first_parameter_t = typename first_parameter<Args...>::type;

		template<typename...Args>
		struct check_first_parameter : std::false_type {};

		template<typename T, typename...Args>
		struct check_first_parameter<T, T, Args...> : std::true_type {};

		template<typename...Args>
		constexpr static bool check_first_parameter_v = check_first_parameter<Args...>::value;

		template<typename T>
		struct mrun_parameter_helper;

		template<typename...Args>
		struct mrun_parameter_helper<parameter_list<Args...>> {
			template<typename Func,typename...Other>
			constexpr static decltype(auto) run_first(Func&& fn, Args&&...args, Other&&...) {
				// even if Args... is known,but this place still need to use std::forward
				return std::forward<Func>(fn)(std::forward<Args>(args)...);
			}
			template<typename Func,typename...Other>
			constexpr static decltype(auto) run_second(Func&& fn, Args&&..., Other&&...args) {
				return std::forward<Func>(fn)(std::forward<Other>(args)...);
			}
		};

		template<size_t index,typename Func,typename...Args>
		constexpr decltype(auto) run_first_parameter(Func&& fn, Args&&...args) {
			using first_parameter_list_t = parameter_split_two_first_type<index, Args...>;
			using run_type = mrun_parameter_helper<first_parameter_list_t>;
			return run_type::run_first(std::forward<Func>(fn), std::forward<Args>(args)...);
		}
		
		template<size_t index, typename Func, typename...Args>
		constexpr decltype(auto) run_second_parameter(Func&& fn, Args&&...args) {
			using first_parameter_list_t = parameter_split_two_first_type<index, Args...>;
			using run_type = mrun_parameter_helper<first_parameter_list_t>;
			return run_type::run_second(std::forward<Func>(fn), std::forward<Args>(args)...);
		}

		template<typename T>
		struct forward_wrapper {
			static_assert(!std::is_lvalue_reference_v<T>, "");
			using type = std::remove_reference_t<T>;
		};

		template<typename T>
		struct forward_wrapper<T&> {
			using type = std::reference_wrapper<T>;
		};

		template<typename T>
		using forward_wrapper_t = typename forward_wrapper<T>::type;

		enum class empty_base_optimize {
			first_empty,
			second_empty,
			non_empty,
			all_empty
		};

		template<typename T, typename U, empty_base_optimize = empty_base_optimize::non_empty>
		class base_mcompressed_pair {
		public:
			template<typename Other1, typename Other2, std::enable_if_t<
				std::is_constructible_v<T, Other1>&& std::is_constructible_v<U, Other2>, int> = 0>
				constexpr base_mcompressed_pair(Other1&& lhs, Other2&& rhs)
				: Myfirst(std::forward<Other1>(lhs)), Mysecond(std::forward<Other2>(rhs)) {
			}
			constexpr T& first() {
				return Myfirst;
			}
			constexpr const T& first()const {
				return Myfirst;
			}
			constexpr U& second() {
				return Mysecond;
			}
			constexpr const U& second()const {
				return Mysecond;
			}
		private:
			T Myfirst;
			U Mysecond;
		};

		template<typename T, typename U>
		class base_mcompressed_pair<T, U, empty_base_optimize::first_empty>
			: private T {
			using base = T;
		public:
			template<typename Other1, typename Other2, std::enable_if_t<
				std::is_constructible_v<T, Other1>&& std::is_constructible_v<U, Other2>, int> = 0>
			constexpr base_mcompressed_pair(Other1&& lhs, Other2&& rhs)
				: base(std::forward<Other1>(lhs)), Mysecond(std::forward<Other2>(rhs)) {
			}
			constexpr T& first() {
				return *this;
			}
			constexpr const T& first()const {
				return *this;
			}
			constexpr U& second() {
				return Mysecond;
			}
			constexpr const U& second()const {
				return Mysecond;
			}
		private:
			U Mysecond;
		};

		template<typename T, typename U>
		class base_mcompressed_pair<T, U, empty_base_optimize::second_empty>
			: private U {
			using base = U;
		public:
			template<typename Other1, typename Other2, std::enable_if_t<
				std::is_constructible_v<T, Other1>&& std::is_constructible_v<U, Other2>, int> = 0>
			constexpr base_mcompressed_pair(Other1&& lhs, Other2&& rhs)
				: Myfirst(std::forward<Other1>(lhs)), base(std::forward<Other2>(rhs)) {
			}
			constexpr T& first() {
				return Myfirst;
			}
			constexpr const T& first()const {
				return Myfirst;
			}
			constexpr U& second() {
				return *this;
			}
			constexpr const U& second()const {
				return *this;
			}
		private:
			T Myfirst;
		};

		template<typename T, typename U>
		using mcompressed_pair = std::conditional_t<std::is_empty_v<T>,
			base_mcompressed_pair<T, U, empty_base_optimize::first_empty>,
			std::conditional_t<std::is_empty_v<U>, base_mcompressed_pair<T, U, empty_base_optimize::second_empty>,
			base_mcompressed_pair<T, U, empty_base_optimize::non_empty>>>;
		
	}

	inline namespace wjr_math {
		constexpr static uint64_t binary_mask[65] = {
			0x0000000000000000,
			0x0000000000000001,0x0000000000000003,0x0000000000000007,0x000000000000000f,
			0x000000000000001f,0x000000000000003f,0x000000000000007f,0x00000000000000ff,
			0x00000000000001ff,0x00000000000003ff,0x00000000000007ff,0x0000000000000fff,
			0x0000000000001fff,0x0000000000003fff,0x0000000000007fff,0x000000000000ffff,
			0x000000000001ffff,0x000000000003ffff,0x000000000007ffff,0x00000000000fffff,
			0x00000000001fffff,0x00000000003fffff,0x00000000007fffff,0x0000000000ffffff,
			0x0000000001ffffff,0x0000000003ffffff,0x0000000007ffffff,0x000000000fffffff,
			0x000000001fffffff,0x000000003fffffff,0x000000007fffffff,0x00000000ffffffff,
			0x00000001ffffffff,0x00000003ffffffff,0x00000007ffffffff,0x0000000fffffffff,
			0x0000001fffffffff,0x0000003fffffffff,0x0000007fffffffff,0x000000ffffffffff,
			0x000001ffffffffff,0x000003ffffffffff,0x000007ffffffffff,0x00000fffffffffff,
			0x00001fffffffffff,0x00003fffffffffff,0x00007fffffffffff,0x0000ffffffffffff,
			0x0001ffffffffffff,0x0003ffffffffffff,0x0007ffffffffffff,0x000fffffffffffff,
			0x001fffffffffffff,0x003fffffffffffff,0x007fffffffffffff,0x00ffffffffffffff,
			0x01ffffffffffffff,0x03ffffffffffffff,0x07ffffffffffffff,0x0fffffffffffffff,
			0x1fffffffffffffff,0x3fffffffffffffff,0x7fffffffffffffff,0xffffffffffffffff
		};

		constexpr static size_t quick_log2_tabel[32] =
		{ 0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3 };

		constexpr size_t quick_log2(uint16_t x) {
			size_t ans = 0;
			if (x >> 8) { ans += 8; x >>= 8; }
			if (x >> 4) { ans += 4; x >>= 4; }
			return ans + quick_log2_tabel[x];
		}

		constexpr size_t quick_log2(int16_t x) {
			return quick_log2((uint16_t)x);
		}

		constexpr size_t quick_log2(uint32_t x) {
			size_t ans = 0;
			if (x >> 16) { ans += 16; x >>= 16; }
			if (x >> 8) { ans += 8; x >>= 8; }
			if (x >> 4) { ans += 4; x >>= 4; }
			return ans + quick_log2_tabel[x];
		}

		constexpr size_t quick_log2(int32_t x) {
			return quick_log2((uint32_t)x);
		}

		constexpr size_t quick_log2(uint64_t x) {
			size_t ans = 0;
			if (x >> 32) { ans += 32; x >>= 32; }
			if (x >> 16) { ans += 16; x >>= 16; }
			if (x >> 8) { ans += 8; x >>= 8; }
			if (x >> 4) { ans += 4; x >>= 4; }
			return ans + quick_log2_tabel[x];
		}

		constexpr size_t quick_log2(int64_t x) {
			return quick_log2((uint64_t)x);
		}

		template<typename T>
		constexpr T quick_pow(T a, T b) {
			T s = 1;
			while (b) {
				if (b & 1) {
					s *= a;
				}
				a *= a;
				b >>= 1;
			}
			return s;
		}

		template<size_t base, typename T>
		constexpr size_t __get_log_length() {
			size_t S = std::numeric_limits<T>::max();
			size_t cnt = 0;
			while (S >= base) {
				S /= base;
				++cnt;
			}
			return cnt + 1;
		}

		template<size_t base, typename T>
		constexpr size_t log_length = __get_log_length<base, T>();

		template<size_t base, size_t index, typename T>
		constexpr size_t __get_quick_log(T x) {
			constexpr size_t X = quick_pow(base, (size_t)(1) << index);
			if constexpr (index == 0) {
				return x >= X;
			}
			else {
				return x >= X ? (__get_quick_log<base, index - 1, T>(x / X) + ((size_t)(1) << index))
					: (__get_quick_log<base, index - 1, T>(x));
			}
		}

		template<size_t base, typename T>
		constexpr size_t quick_log(T x) {
			constexpr size_t length = log_length<base, T>;
			constexpr size_t l = quick_log2(length - 1);
			return __get_quick_log<base, l, T>(x);
		}

		template<size_t base, typename T>
		constexpr bool quick_is_power_of(T x) {
			return quick_pow(quick_log<base>(x), base) == x;
		}

		constexpr uint16_t bswap_16(uint16_t x) {
			return (x >> 8) | (x << 8);
		}

		constexpr uint32_t bswap_32(uint32_t x) {
			x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0x00FF00FF);
			return (x >> 16) | (x << 16);
		}

		constexpr uint64_t bswap_64(uint64_t x) {
			x = ((x << 8) & 0xFF00FF00FF00FF00) | ((x >> 8) & 0x00FF00FF00FF00FF);
			x = ((x << 16) & 0xFFFF0000FFFF0000) | ((x >> 16) & 0x0000FFFF0000FFFF);
			return (x >> 32) | (x << 32);
		}

		template<int w_default_endian>
		constexpr char basic_auto_bswap(char x) {
			return x;
		}

		template<int w_default_endian>
		constexpr uint8_t basic_auto_bswap(uint8_t x) {
			return x;
		}

		template<int w_default_endian>
		constexpr short basic_auto_bswap(short x) {
			if constexpr (w_default_endian == w_endian) {
				return x;
			}
			else {
				return bswap_16(x);
			}
		}

		template<int w_default_endian>
		constexpr uint16_t basic_auto_bswap(uint16_t x) {
			if constexpr (w_default_endian == w_endian) {
				return x;
			}
			else {
				return bswap_16(x);
			}
		}

		template<int w_default_endian>
		constexpr int basic_auto_bswap(int x) {
			if constexpr (w_default_endian == w_endian) {
				return x;
			}
			else {
				return bswap_32(x);
			}
		}

		template<int w_default_endian>
		constexpr uint32_t basic_auto_bswap(uint32_t x) {
			if constexpr (w_default_endian == w_endian) {
				return x;
			}
			else {
				return bswap_32(x);
			}
		}

		template<int w_default_endian>
		constexpr long long basic_auto_bswap(long long x) {
			if constexpr (w_default_endian == w_endian) {
				return x;
			}
			else {
				return bswap_64(x);
			}
		}

		template<int w_default_endian>
		constexpr uint64_t basic_auto_bswap(uint64_t x) {
			if constexpr (w_default_endian == w_endian) {
				return x;
			}
			else {
				return bswap_64(x);
			}
		}

		template<typename T, int w_default_endian = w_big_endian>
		constexpr T auto_bswap(T x) {
			return basic_auto_bswap<w_default_endian>(x);
		}

		template<typename pointer, int w_default_endian = w_big_endian,
			std::enable_if_t<wjr_is_iterator_v<pointer>, int> = 0>
			void auto_bswap(pointer first, pointer last) {
			if constexpr (w_endian != w_default_endian) {
				while (first != last) {
					*first = auto_bswap(*first);
					++first;
				}
			}
		}

		template<size_t index,typename T>
		struct auto_split_number {
			T vec[index];
		};

		template<size_t kth,size_t index,typename T>
		constexpr T get(const auto_split_number<index, T>&v) {
			if constexpr (is_little_endian) {
				return v.vec[kth];
			}
			else {
				return v.vec[index - kth - 1];
			}
		}

		template<size_t index,typename T,typename U>
		constexpr decltype(auto) split_number(U x) {
			static_assert(index * sizeof(T) == sizeof(U), "");
			union {
				auto_split_number<index, T> a;
				U b;
			} c;
			c.b = x;
			return c.a;
		}

		template<typename T, int w_defualt_endian>
		class byte_order_iterator {
		public:
			byte_order_iterator(T* ptr)
				: ptr(ptr) {
			}
			operator T() {
				return auto_bswap<T, w_defualt_endian>(*ptr);
			}
			byte_order_iterator& operator=(T val) {
				*ptr = auto_bswap<T, w_defualt_endian>(val);
				return *this;
			}
		private:
			T* ptr;
		};

		template<typename T, int w_default_endian>
		class byte_order_const_iterator {
		public:
			byte_order_const_iterator(const T* ptr)
				: ptr(ptr) {
			}
			operator T()const {
				return auto_bswap<T, w_default_endian>(ptr);
			}
		private:
			const T* ptr;
		};

		template<typename T, int w_default_endian = w_big_endian>
		class basic_byte_order_ptr {
		public:
			using value_type = T;
			using reference = T&;
			using const_reference = const T&;
			using pointer = T*;
			using const_pointer = const T*;
			using iterator = byte_order_iterator<T, w_default_endian>;
			using const_iterator = byte_order_const_iterator<T, w_default_endian>;
			using difference_type = ptrdiff_t;
			using iterator_category = std::random_access_iterator_tag;
			explicit basic_byte_order_ptr(pointer ptr = nullptr)
				: ptr(ptr) {
			}
			basic_byte_order_ptr(const basic_byte_order_ptr& other) = default;
			basic_byte_order_ptr& operator=(const basic_byte_order_ptr& other) = default;
			basic_byte_order_ptr& operator=(pointer cptr) {
				ptr = cptr;
				return *this;
			}
			iterator operator*() {
				return ptr;
			}
			const_iterator operator*()const {
				return ptr;
			}
			basic_byte_order_ptr& operator++() {
				++ptr;
				return *this;
			}
			basic_byte_order_ptr operator++(int) {
				basic_byte_order_ptr p(ptr);
				++ptr;
				return p;
			}
			basic_byte_order_ptr& operator--() {
				--ptr;
				return *this;
			}
			basic_byte_order_ptr operator--(int) {
				basic_byte_order_ptr p(ptr);
				--ptr;
				return p;
			}
			basic_byte_order_ptr& operator+=(difference_type index) {
				ptr += index;
				return *this;
			}
			friend basic_byte_order_ptr operator+(
				const basic_byte_order_ptr& a, difference_type index) {
				return basic_byte_order_ptr(a + index);
			}
			basic_byte_order_ptr& operator-=(difference_type index) {
				ptr -= index;
				return *this;
			}
			friend basic_byte_order_ptr operator-(
				const basic_byte_order_ptr& a, difference_type index) {
				return basic_byte_order_ptr(a - index);
			}
			friend difference_type operator-(
				const basic_byte_order_ptr& a, const basic_byte_order_ptr& b) {
				return a.ptr - b.ptr;
			}
			bool operator==(const basic_byte_order_ptr& other)const {
				return ptr == other.ptr;
			}
			bool operator!=(const basic_byte_order_ptr& other)const {
				return ptr != other.ptr;
			}
			operator pointer () {
				return ptr;
			}
			operator const_pointer()const {
				return ptr;
			}
		private:
			pointer ptr;
		};

		template<typename T, int w_default_endian = w_big_endian>
		using byte_order_ptr = std::conditional_t<is_any_of_v<
			std::decay_t<T>, char, uint8_t> || w_default_endian == w_endian,
			T*, basic_byte_order_ptr<T, w_default_endian>>;

		using u8byte_order_ptr = byte_order_ptr<uint8_t, w_big_endian>;
		using u16byte_order_ptr = byte_order_ptr<uint16_t, w_big_endian>;
		using u32byte_order_ptr = byte_order_ptr<uint32_t, w_big_endian>;
		using u64byte_order_ptr = byte_order_ptr<uint64_t, w_big_endian>;
	}

	inline namespace wjr_hash {
#if defined(_WIN64)
		constexpr static size_t _FNV_offset_basis = 14695981039346656037ULL;
		constexpr static size_t _FNV_prime = 1099511628211ULL;
#else // defined(_WIN64)
		constexpr static size_t _FNV_offset_basis = 2166136261U;
		constexpr static size_t _FNV_prime = 16777619U;
#endif // defined(_WIN64)

		constexpr size_t normal_fnv1a_append_bytes(size_t _Val, const unsigned char* const _First,
			const size_t _Count) noexcept { // accumulate range [_First, _First + _Count) into partial FNV-1a hash _Val
			for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
				_Val ^= static_cast<size_t>(_First[_Idx]);
				_Val *= _FNV_prime;
			}
			return _Val;
		}

		template<size_t byte_size, typename const_pointer>
		constexpr size_t constexpr_fnv1a_append_bytes(size_t _Val, const_pointer _First,
			const size_t _Count)noexcept {
			for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
				auto val = static_cast<size_t>(_First[_Idx]);
#if is_little_endian
				for (size_t i = 0; i < byte_size; ++i) {
					_Val ^= (val >> (i << 3)) & 0xFF;
					_Val *= _FNV_prime;
				}
#else
				for (size_t i = byte_size - 1; i > 0; ++i) {
					_Val ^= (val >> (i << 3)) & 0xFF;
					_Val *= _FNV_prime;
				}
#endif
			}
			return _Val;
		}

		template<typename const_pointer>
		constexpr size_t fnv1a_append_bytes(size_t _Val, const_pointer _First,
			const size_t _Count)noexcept {
			using value_type = typename std::iterator_traits<const_pointer>::value_type;
			constexpr size_t byte_size = sizeof(value_type);
			if constexpr (byte_size >= 1 && byte_size <= sizeof(size_t)) {
				return constexpr_fnv1a_append_bytes<byte_size>(_Val, _First, _Count);
			}
			else {
				return normal_fnv1a_append_bytes(_Val,
					reinterpret_cast<const unsigned char*>(_First), byte_size * _Count);
			}
		}

		template <class _Ty>
		constexpr size_t fnv1a_append_range(const size_t _Val, const _Ty* const _First,
			const _Ty* const _Last) noexcept { // accumulate range [_First, _Last) into partial FNV-1a hash _Val
			static_assert(std::is_trivial_v<_Ty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(_Val, _First, static_cast<size_t>(_Last - _First));
		}

		template <class _Kty>
		constexpr size_t fnv1a_append_value(
			const size_t _Val, const _Kty& _Keyval) noexcept { // accumulate _Keyval into partial FNV-1a hash _Val
			static_assert(std::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(_Val, reinterpret_cast<const _Kty*>(&_Keyval), 1);
		}

		// FUNCTION TEMPLATE _Hash_representation
		template <class _Kty>
		constexpr size_t hash_representation(const _Kty& _Keyval) noexcept { // bitwise hashes the representation of a key
			return fnv1a_append_value(_FNV_offset_basis, _Keyval);
		}

		// FUNCTION TEMPLATE _Hash_array_representation
		template <class _Kty>
		constexpr size_t hash_array_representation(
			const _Kty* const _First, const size_t _Count) noexcept { // bitwise hashes the representation of an array
			static_assert(std::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(
				_FNV_offset_basis, _First, _Count);
		}
	}

	inline namespace wjr_mem {
		// ' ','\n','\r','\t'
		constexpr static bool is_white_space_char[256] = { 
			0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		};
		constexpr static char white_space[16] = " \n\r\t";

		static void* simple_skip_whitespace(const void* _s, const void* _e) {
			const uint8_t* s = (const uint8_t*)_s, * e = (const uint8_t*)_e;
			while (s != e && is_white_space_char[*s])++s;
			return (void*)s;
		}

		static void* skip_whitespace(const void* s, const void* e) {
#if defined(_MSC_VER)
			uint8_t* _s = (uint8_t*)s;
			uint8_t* _e = (uint8_t*)e;
			if (is_white_space_char[*_s])
				++_s;
			else return _s;

			const __m128i w = _mm_load_si128((const __m128i*) & white_space[0]);
			for (; _s <= _e - 16; _s += 16) {
				const __m128i ss = _mm_loadu_si128(reinterpret_cast<const __m128i*>(_s));
				const int r = _mm_cmpistri(w, ss, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_LEAST_SIGNIFICANT | _SIDD_NEGATIVE_POLARITY);
				if (r != 16)
					return _s + r;
			}
#endif
			return simple_skip_whitespace(s, e);
		}
	}
}

#endif