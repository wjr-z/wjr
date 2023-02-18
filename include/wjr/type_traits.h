#pragma once
#ifndef __WJR_TYPE_TRAITS_H
#define __WJR_TYPE_TRAITS_H

#include <cassert>
#include <memory>
#include <stdexcept>
#include <iterator>
#include <vector>
#include <functional>
//#include <wjr/macro.h>
#include <wjr/literals.h>

_WJR_BEGIN

WJR_INTRINSIC_CONSTEXPR bool is_likely(bool f) noexcept {
#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	return __builtin_expect(f, true);
#else
	return f;
#endif
}

WJR_INTRINSIC_CONSTEXPR bool is_unlikely(bool f) noexcept {
#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	return __builtin_expect(f, false);
#else
	return f;
#endif
}

WJR_INTRINSIC_CONSTEXPR bool is_constant_evaluated() noexcept {
#if defined(__cpp_lib_is_constant_evaluated) 
	return std::is_constant_evaluated();
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated) || WJR_HAS_GCC(9,1,0) || WJR_HAS_CLANG(9,0,0)
	return __builtin_is_constant_evaluated();
#else
	return false;
#endif
}

WJR_INTRINSIC_INLINE void unreachable() noexcept {
#if defined(_cpp_lib_unreachable)
	return std::unreachable();
#elif WJR_HAS_BUILTIN(__builtin_unreachable) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	__builtin_unreachable();
#elif defined(WJR_COMPILER_MSVC)
	__assume(0);
#endif
}

WJR_INTRINSIC_CONSTEXPR void assume(bool cond) noexcept {
#if WJR_HAS_BUILTIN(__builtin_assume)
	__builtin_assume(cond);
#else
	if (is_unlikely(!cond)) {
		unreachable();
	}
#endif 
}

template<typename T>
WJR_INTRINSIC_CONSTEXPR bool is_constant_p(T x) noexcept {
#if WJR_HAS_BUILTIN(__builtin_constant_p) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	return __builtin_constant_p(x);
#else
	return is_constant_evaluated();
#endif
}

#if defined(__cpp_lib_is_constant_evaluated) || WJR_HAS_BUILTIN(__builtin_is_constant_evaluated) ||		\
	WJR_HAS_GCC(9,1,0) || WJR_HAS_CLANG(9,0,0)
#define WJR_HAS_CONSTANT_EVALUATED
#endif 

#if WJR_HAS_BUILTIN(__builtin_constant_p) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
#define WJR_HAS_CONSTANT_P
#define WJR_HAS_STRONG_CONSTANT_P
#define WJR_HAS_WEAK_CONSTANT_P
#elif defined(WJR_HAS_CONSTANT_EVALUATED)
#define WJR_HAS_WEAK_CONSTANT_P
#endif

struct move_tag {};

struct power_of_two_tag {
	unsigned long long value;
};

struct zero_tag {};

constexpr size_t byte_width = WJR_BYTE_WIDTH;

template<typename T, typename U, typename _Pred>
struct has_global_binary_operator : std::false_type {};

template<typename T, typename U, typename _Pred>
constexpr bool has_global_binary_operator_v = has_global_binary_operator<T, U, _Pred>::value;

#define REGISTER_HAS_MEMBER_FUNCTION(FUNC, NAME)								\
template<typename Enable, typename T, typename...Args>							\
struct __has_member_function_##NAME : std::false_type {};						\
template<typename T, typename...Args>											\
struct __has_member_function_##NAME <std::void_t<decltype(						\
	std::declval<T>(). FUNC (std::declval<Args>()...))>, T, Args...>			\
	: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_member_function_##NAME :												\
	__has_member_function_##NAME<void, T, Args...> {};							\
template<typename T, typename...Args>											\
constexpr bool has_member_function_##NAME##_v =									\
	has_member_function_##NAME<T, Args...>::value;

#define REGISTER_HAS_STATIC_MEMBER_FUNCTION(FUNC, NAME)							\
template<typename Enable, typename T, typename...Args>							\
struct __has_static_member_function_##NAME : std::false_type {};				\
template<typename T, typename...Args>											\
struct __has_static_member_function_##NAME <std::void_t<decltype(				\
	T:: FUNC (std::declval<Args>()...))>, T, Args...>							\
	: std::true_type {};														\
template<typename T, typename...Args>											\
struct has_static_member_function_##NAME :										\
	__has_static_member_function_##NAME<void, T, Args...> {};					\
template<typename T, typename...Args>											\
constexpr bool has_static_member_function_##NAME##_v =							\
	has_static_member_function_##NAME<T, Args...>::value;

#define REGISTER_HAS_GLOBAL_FUNCTION(FUNC, NAME)								\
template<typename Enable, typename...Args>										\
struct __has_global_function_##NAME : std::false_type {};						\
template<typename...Args>														\
struct __has_global_function_##NAME <std::void_t<decltype(						\
	FUNC(std::declval<Args>()...))>, Args...>									\
	: std::true_type {};														\
template<typename...Args>														\
struct has_global_function_##NAME :												\
	__has_global_function_##NAME<void, Args...> {};								\
template<typename...Args>														\
constexpr bool has_global_function_##NAME##_v =									\
	has_global_function_##NAME<Args...>::value;

#define REGISTER_HAS_GLOBAL_BINARY_OPERATOR(OP, NAME)							\
template<typename Enable, typename T, typename U>								\
struct __has_global_binary_operator_##NAME : std::false_type {};				\
template<typename T, typename U>												\
struct __has_global_binary_operator_##NAME <std::void_t<decltype(				\
	std::declval<T>() OP std::declval<U>())>, T, U>								\
	: std::true_type {};														\
template<typename T, typename U>												\
struct has_global_binary_operator_##NAME :										\
	__has_global_binary_operator_##NAME<void, T, U> {};							\
template<typename T, typename U>												\
constexpr bool has_global_binary_operator_##NAME##_v =							\
	has_global_binary_operator_##NAME<T, U>::value;								\
template<typename T, typename U>												\
struct has_global_binary_operator<T, U, std:: NAME<>> :							\
	has_global_binary_operator_##NAME<T, U> {};

#define REGISTER_HAS_GLOBAL_UNARY_OPERATOR(OP, NAME)							\
template<typename Enable, typename T>											\
struct __has_global_unary_operator_##NAME : std::false_type {};					\
template<typename T>															\
struct __has_global_unary_operator_##NAME <std::void_t<decltype(				\
	OP std::declval<T>())>, T>													\
	: std::true_type {};														\
template<typename T>															\
struct has_global_unary_operator_##NAME :										\
	__has_global_unary_operator_##NAME<void, T> {};								\
template<typename T>															\
constexpr bool has_global_unary_operator_##NAME##_v =							\
	has_global_unary_operator_##NAME<T>::value;

REGISTER_HAS_MEMBER_FUNCTION(operator(), call_operator);
REGISTER_HAS_MEMBER_FUNCTION(operator[], subscript_operator);
REGISTER_HAS_MEMBER_FUNCTION(operator->, point_to_operator);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(+, plus);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(-, minus);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(&, bit_and);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(| , bit_or);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(^, bit_xor);
REGISTER_HAS_GLOBAL_UNARY_OPERATOR(~, bit_not);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(== , equal_to);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(!= , not_equal_to);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(> , greater);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(>= , greater_equal);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(< , less);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(<= , less_equal);
REGISTER_HAS_STATIC_MEMBER_FUNCTION(min, min);
REGISTER_HAS_STATIC_MEMBER_FUNCTION(max, max);

template<typename T, typename...Args>
struct is_any_of : std::disjunction<std::is_same<T, Args>...> {};

template<typename T, typename...Args>
constexpr bool is_any_of_v = is_any_of<T, Args...>::value;

template<typename T>
struct is_standard_comparator : 
	is_any_of<std::remove_cv_t<T>, 
	std::less<>, 
	std::less_equal<>, 
	std::equal_to<>, 
	std::not_equal_to<>,
	std::greater<>, 
	std::greater_equal<>
	> {};

template<typename T>
constexpr bool is_standard_comparator_v = is_standard_comparator<T>::value;

template<typename T>
struct unrefwrap {
	using type = T;
};

template<typename T>
struct unrefwrap<std::reference_wrapper<T>> {
	using type = T&;
};

template<typename T>
using unrefwrap_t = typename unrefwrap<T>::type;

template<typename T>
using iter_cat_t = typename std::iterator_traits<T>::iterator_category;

template<typename T>
using iter_diff_t = typename std::iterator_traits<T>::difference_type;

template<typename T>
using iter_val_t = typename std::iterator_traits<T>::value_type;

template<typename T, typename = void>
struct is_iterator : std::false_type {};

template<typename T>
struct is_iterator<T, std::void_t<iter_cat_t<T>>> : std::true_type {};

template<typename T>
constexpr bool is_iterator_v = is_iterator<T>::value;

#if defined(WJR_CPP_20)
template<typename T>
struct is_contiguous_iterator : std::bool_constant<std::contiguous_iterator<T>> {};
#else
template<typename T>
struct is_contiguous_iterator : std::is_pointer<T> {};
#endif

template<typename _Iter>
struct is_contiguous_iterator<std::reverse_iterator<_Iter>> : is_contiguous_iterator<_Iter> {};

template<typename T>
constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<T>::value;

template<typename T>
struct is_input_iter : std::is_convertible<iter_cat_t<T>, std::input_iterator_tag> {};

template<typename T>
constexpr bool is_input_iter_v = is_input_iter<T>::value;

template<typename T>
struct is_output_iter : std::is_convertible<iter_cat_t<T>, std::output_iterator_tag> {};

template<typename T>
constexpr bool is_output_iter_v = is_output_iter<T>::value;

template<typename T>
struct is_forward_iter : std::is_convertible<iter_cat_t<T>, std::forward_iterator_tag> {};

template<typename T>
constexpr bool is_forward_iter_v = is_forward_iter<T>::value;

template<typename T>
struct is_bidir_iter : std::is_convertible<iter_cat_t<T>, std::bidirectional_iterator_tag> {};

template<typename T>
constexpr bool is_bidir_iter_v = is_bidir_iter<T>::value;

template<typename T>
struct is_random_iter : std::is_convertible<iter_cat_t<T>, std::random_access_iterator_tag> {};

template<typename T>
constexpr bool is_random_iter_v = is_random_iter<T>::value;

template<size_t n>
struct __uint_helper{};

template<>
struct __uint_helper<8> {
	using type = unsigned char;
};
template<>
struct __uint_helper<16> {
	using type = unsigned short;
};
template<>
struct __uint_helper<32> {
	using type = std::conditional_t<sizeof(unsigned int) == 4, unsigned int, unsigned long>;
};
template<>
struct __uint_helper<64> {
	using type = unsigned long long;
};

template<size_t n> 
using uint_t = typename __uint_helper<n>::type;

template<size_t n>
struct __int_helper {};

template<>
struct __int_helper<8> {
	using type = signed char;
};
template<>
struct __int_helper<16> {
	using type = signed short;
};
template<>
struct __int_helper<32> {
	using type = std::conditional_t<sizeof(int) == 4, int, long>;
};
template<>
struct __int_helper<64> {
	using type = long long;
};

template<size_t n>
using int_t = typename __int_helper<n>::type;

using int8_t = int_t<8>;
using int16_t = int_t<16>;
using int32_t = int_t<32>;
using int64_t = int_t<64>;

using uint8_t = uint_t<8>;
using uint16_t = uint_t<16>;
using uint32_t = uint_t<32>;
using uint64_t = uint_t<64>;

using intptr_t = int_t<sizeof(void*) * 8>;
using uintptr_t = uint_t<sizeof(void*) * 8>;

template<typename T>
struct is_my_standard_integral : 
	is_any_of<std::remove_cv_t<T>, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t> {};

template<typename T>
constexpr bool is_my_standard_integral_v = is_my_standard_integral<T>::value;

template<size_t n, bool __s>
using __int_or_uint = std::conditional_t<__s, int_t<n>, uint_t<n>>;

template<typename T, bool = std::is_integral_v<T>>
struct __integral_normalization_helper {
	using type = __int_or_uint<sizeof(T) * 8, std::is_signed_v<T>>;
};

//template<typename T>
//struct __integral_normalization_helper<T*, false> {
	//using type = __int_or_uint<sizeof(void*) * 8, std::is_signed_v<iter_diff_t<T*>>>;
//};

template<typename T>
struct __integral_normalization_helper<T, false> {
	using type = T;
};

template<typename T>
struct integral_normalization {
	using type = typename __integral_normalization_helper<std::remove_cv_t<T>>::type;
};

template<typename T>
using integral_normalization_t = typename integral_normalization<T>::type;

template<typename T>
constexpr integral_normalization_t<T> make_integral_normalization(T t) {
	return static_cast<integral_normalization_t<T>>(t);
}

template<typename T>
constexpr std::make_unsigned_t<T> make_unsigned(T t) {
	return static_cast<std::make_unsigned_t<T>>(t);
}

template<typename T>
struct _Auto_variable_helper {
	using type = std::conditional_t<sizeof(T) <= 256 / 8, T, const T&>;
};

template<typename T>
using auto_var_t = typename _Auto_variable_helper<T>::type;

template<typename T>
struct is_left_standard_comparator : 
	is_any_of<std::remove_cv_t<T>, 
	std::less<>, 
	std::less_equal<>
	> {};

template<typename T>
constexpr bool is_left_standard_comparator_v = is_left_standard_comparator<T>::value;

template<typename T>
struct is_right_standard_comparator :
	is_any_of<std::remove_cv_t<T>, 
	std::greater<>, 
	std::greater_equal<>
	> {};

template<typename T>
constexpr bool is_right_standard_comparator_v = is_right_standard_comparator<T>::value;

template<typename T>
struct swap_standard_comparator_helper {
	using type = T;
};

template<>
struct swap_standard_comparator_helper<std::less<>> {
	using type = std::greater<>;
};

template<>
struct swap_standard_comparator_helper<std::less_equal<>> {
	using type = std::greater_equal<>;
};

template<>
struct swap_standard_comparator_helper<std::greater<>> {
	using type = std::less<>;
};

template<>
struct swap_standard_comparator_helper<std::greater_equal<>> {
	using type = std::less_equal<>;
};

template<>
struct swap_standard_comparator_helper<std::equal_to<>> {
	using type = std::equal_to<>;
};

template<typename T>
struct swap_standard_comparator {
	using type = typename swap_standard_comparator_helper<std::remove_cv_t<T>>::type;
};

template<typename T>
using swap_standard_comparator_t = typename swap_standard_comparator<T>::type;

template<size_t i, size_t..._Index>
struct is_any_index_of : std::disjunction<std::bool_constant<i == _Index>...> {};

template<size_t i, size_t..._Index>
constexpr bool is_any_index_of_v = is_any_index_of<i, _Index...>::value;

template<typename T>
struct ref_wrapper {
	using type = T;
};

template<typename T>
struct ref_wrapper<T&> {
	using type = std::reference_wrapper<T>;
};

template<typename T>
struct ref_wrapper<T&&> {
	using type = T;
};

template<typename T>
using ref_wrapper_t = typename ref_wrapper<T>::type;

template<typename...Args>
struct is_integrals : std::conjunction<std::is_integral<Args>...> {};

template<typename...Args>
constexpr bool is_integrals_v = is_integrals<Args...>::value;

template<typename T>
struct is_unsigned_integral : std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {};

template<typename T>
constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

template<typename T>
struct is_signed_integral : std::conjunction<std::is_integral<T>, std::is_signed<T>> {};

template<typename T>
constexpr bool is_signed_integral_v = is_signed_integral<T>::value;

template<typename T, typename U>
struct common_arithmetic {
	using type = decltype(T{} + U{});
};

template<typename T, typename U>
using common_arithmetic_t = typename common_arithmetic<T, U>::type;

template<typename T, typename U, bool = sizeof(T) == sizeof(U) && std::is_integral_v<T>&& std::is_integral_v<U>>
constexpr bool __is_memory_comparable_helper_v = 
std::is_same_v<T, bool> || std::is_same_v<U, bool> || static_cast<T>(-1) == static_cast<U>(-1);

template<typename T, typename U>
constexpr bool __is_memory_comparable_helper_v<T, U, false> = false;

template<typename T, typename U, typename _Pred>
struct is_memory_comparable : std::conjunction<
	is_standard_comparator<_Pred>,
	has_global_binary_operator<T, U, _Pred>,
	has_global_binary_operator<U, T, _Pred>,
	std::bool_constant<__is_memory_comparable_helper_v<integral_normalization_t<T>, integral_normalization_t<U>>>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool is_memory_comparable_v = is_memory_comparable<T, U, _Pred>::value;

//template<typename T, typename U, typename _Pred>
//struct is_memory_copyable : std::conjunction<
//> {};

template<typename T>
struct is_reverse_iterator : std::false_type {};

template<typename _Iter>
struct is_reverse_iterator<std::reverse_iterator<_Iter>> : std::negation<is_reverse_iterator<_Iter>> {};

template<typename T>
constexpr bool is_reverse_iterator_v = is_reverse_iterator<T>::value;

template<typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
constexpr auto enum_cast(T t) noexcept {
	return static_cast<std::underlying_type_t<T>>(t);
}

namespace enum_ops {
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T> operator|(T lhs, T rhs) noexcept {
		return static_cast<T>(enum_cast(lhs) | enum_cast(rhs));
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T> operator&(T lhs, T rhs) noexcept {
		return static_cast<T>(enum_cast(lhs) & enum_cast(rhs));
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T> operator^(T lhs, T rhs) noexcept {
		return static_cast<T>(enum_cast(lhs) ^ enum_cast(rhs));
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T> operator~(T t) noexcept {
		return static_cast<T>(~enum_cast(t));
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T&> operator|=(T& lhs, T rhs) noexcept {
		return lhs = lhs | rhs;
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T&> operator&=(T& lhs, T rhs) noexcept {
		return lhs = lhs & rhs;
	}
	template<typename T>
	constexpr std::enable_if_t<std::is_enum_v<T>, T&> operator^=(T& lhs, T rhs) noexcept {
		return lhs = lhs ^ rhs;
	}
}

namespace __To_address {
	REGISTER_HAS_STATIC_MEMBER_FUNCTION(to_address, to_address);
}

template<typename T>
constexpr auto to_address(T&& p) noexcept {
	if constexpr (__To_address::has_static_member_function_to_address_v<std::pointer_traits<std::decay_t<T&&>>, T&&>) {
		return std::pointer_traits<std::decay_t<T&&>>::to_address(std::forward<T>(p));
	}
	else if constexpr (wjr::has_member_function_point_to_operator_v<T&&>) {
		return to_address(std::forward<T>(p).operator->());
	}
	else {
		return std::forward<T>(p);
	}
}

template<typename T>
using to_address_t = decltype(to_address(std::declval<T>()));

template<typename T>
struct type_identity {
	using type = T;
};

template<typename T>
using type_identity_t = typename type_identity<T>::type;

template<typename T, typename = void>
struct _Is_default_convertible : std::false_type {};

template<typename T>
void _Test_default_convertible(const T&);

template<typename T>
struct _Is_default_convertible<T, std::void_t<decltype(_Test_default_convertible<T>({}))>> : std::true_type{};

template<typename T>
using is_default_convertible = _Is_default_convertible<T>;

template<typename T>
constexpr bool is_default_convertible_v = is_default_convertible<T>::value;

template<typename T, typename U, typename = void>
struct _Is_swappable_with : std::false_type {};

template<typename T, typename U>
struct _Is_swappable_with<T, U, std::void_t<decltype(std::swap(std::declval<T>(), std::declval<U>()))>>
	: std::true_type {};

template<typename T, typename U>
struct is_swappable_with :
	std::conjunction<_Is_swappable_with<T, U>, _Is_swappable_with<U, T>> {};

template<typename T, typename U>
constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;

template<typename T>
struct is_swappable : is_swappable_with<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<T>> {};

template<typename T>
constexpr bool is_swappable_v = is_swappable<T>::value;

template<typename T, typename U>
struct _Is_nothrow_swappable_with : std::bool_constant<noexcept(std::swap(std::declval<T>(), std::declval<U>()))
	&& noexcept(std::swap(std::declval<U>(), std::declval<T>()))> {};

template<typename T, typename U>
struct is_nothrow_swappable_with :
	std::conjunction<is_swappable_with<T, U>, _Is_nothrow_swappable_with<T, U>> {};

template<typename T, typename U>
constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<T, U>::value;

template<typename T>
struct is_nothrow_swappable : 
	is_nothrow_swappable_with<std::add_lvalue_reference_t<T>, std::add_lvalue_reference_t<T>> {};

template<typename T>
constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;

template<typename iter, std::enable_if_t<is_iterator_v<iter>, int> = 0>
struct __make_iter_wrapper : public std::tuple<iter, iter>{
	using _Mybase = std::tuple<iter, iter>;
	using _Mybase::_Mybase;
	using _Mybase::operator=;

	constexpr iter begin() const noexcept { return std::get<0>(*this); }
	constexpr iter end() const noexcept { return std::get<1>(*this); }

	constexpr decltype(auto) size() const noexcept {
		return std::distance(begin(), end());
	}

};

template<typename iter>
constexpr __make_iter_wrapper<iter> make_iter_wrapper(iter first, iter last) {
	return { first, last };
}

template<typename T>
using alloc_pointer_t = typename std::allocator_traits<T>::pointer;

template<typename T>
using alloc_value_t = typename std::allocator_traits<T>::value_type;

template<typename T>
using alloc_size_t = typename std::allocator_traits<T>::size_type;

template<typename T>
using alloc_diff_t = typename std::allocator_traits<T>::difference_type;

// The default allocator uses the default constructor and destructor, which is beneficial to optimization.
// But if the constructor and destructor of the allocator are not default, then the allocator cannot be used.
template<typename T>
struct is_default_allocator : std::false_type {};

template<typename T>
struct is_default_allocator<std::allocator<T>> : std::true_type {};

template<typename T>
constexpr bool is_default_allocator_v = is_default_allocator<T>::value;

REGISTER_HAS_MEMBER_FUNCTION(destroy, destroy);
REGISTER_HAS_MEMBER_FUNCTION(construct, construct);

// Use the default destructor, which is beneficial for optimization.
// But if the destructor of the allocator has side effects, then do not use this allocator.
template<typename Alloc, typename _Ptr>
struct use_default_allocator_destroy : std::disjunction<is_default_allocator<Alloc>,
	std::negation<has_member_function_destroy<Alloc, _Ptr>>> {};

template<typename Alloc, typename _Ptr>
constexpr bool use_default_allocator_destroy_v = use_default_allocator_destroy<Alloc, _Ptr>::value;

template<typename Alloc>
WJR_CONSTEXPR20 void destroy_at_a(alloc_pointer_t<Alloc> _Where, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	if constexpr (use_default_allocator_destroy_v<Alloc, pointer>) {
		std::destroy_at(_Where);
	}
	else {
		std::allocator_traits<Alloc>::destroy(al, std::addressof(*_Where));
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void destroy_a(alloc_pointer_t<Alloc> _First, alloc_pointer_t<Alloc> _Last, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	using value_type = alloc_value_t<Alloc>;
	if constexpr (!std::conjunction_v<std::is_trivially_destructible<value_type>, 
		use_default_allocator_destroy<Alloc, pointer>>) {
		for (; _First != _Last; ++_First) {
			destroy_at_a(_First, al);
		}
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void destroy_n_a(alloc_pointer_t<Alloc> _First, alloc_size_t<Alloc> n, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	using value_type = alloc_value_t<Alloc>;
	if constexpr (!std::conjunction_v<std::is_trivially_destructible<value_type>,
		use_default_allocator_destroy<Alloc, pointer>>) {
		for (; n > 0; --n, ++_First) {
			destroy_at_a(_First, al);
		}
	}
}

template<typename Alloc, typename Ptr, typename...Args>
struct use_default_allocator_construct : std::disjunction<is_default_allocator<Alloc>,
	std::negation<has_member_function_construct<Alloc, Ptr, Args...>>> {};

template<typename Alloc, typename Ptr, typename...Args>
constexpr bool use_default_allocator_construct_v = use_default_allocator_construct<Alloc, Ptr, Args...>::value;

template<typename Alloc>
WJR_CONSTEXPR20 void uninitialized_default_construct_a(
	alloc_pointer_t<Alloc> _First, alloc_pointer_t<Alloc> _Last, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	using value_type = alloc_value_t<Alloc>;
	if constexpr (use_default_allocator_construct_v<Alloc, pointer>) {
		std::uninitialized_default_construct(_First, _Last);
	}
	else {
		if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
			for (; _First != _Last; ++_First) {
				std::allocator_traits<Alloc>::construct(al, std::addressof(*_First));
			}
		}
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void uninitialized_default_construct_n_a(
	alloc_pointer_t<Alloc> _First, alloc_size_t<Alloc> _Count, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	using value_type = alloc_value_t<Alloc>;
	if constexpr (use_default_allocator_construct_v<Alloc, pointer>) {
		std::uninitialized_default_construct_n(_First, _Count);
	}
	else {
		if constexpr (!std::is_trivially_default_constructible_v<value_type>) {
			for (; _Count > 0; --_Count, ++_First) {
				std::allocator_traits<Alloc>::construct(al, std::addressof(*_First));
			}
		}
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 void uninitialized_value_construct_a(
	alloc_pointer_t<Alloc> _First, alloc_pointer_t<Alloc> _Last, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	if constexpr (use_default_allocator_construct_v<Alloc, pointer>) {
		std::uninitialized_value_construct(_First, _Last);
	}
	else {
		pointer _Next = _First;
		for (; _Next != _Last; ++_Next) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next));
		}
	}
}

template<typename Alloc>
WJR_CONSTEXPR20 alloc_pointer_t<Alloc> uninitialized_value_construct_n_a(
	alloc_pointer_t<Alloc> _First, alloc_size_t<Alloc> _Count, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	if constexpr (use_default_allocator_construct_v<Alloc, pointer>) {
		return std::uninitialized_value_construct_n(_First, _Count);
	}
	else {
		pointer _Next = _First;
		for (; _Count > 0; --_Count) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next));
			++_Next;
		}
		return _Next;
	}
}

template<typename iter, typename Alloc, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 alloc_pointer_t<Alloc> uninitialized_copy_a(
	iter _First, iter _Last, alloc_pointer_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	if constexpr (use_default_allocator_construct_v<Alloc, pointer, decltype(*_First)>) {
		return std::uninitialized_copy(_First, _Last, _Dest);
	}
	else {
		pointer _Next = _Dest;
		for (; _First != _Last; ++_First) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), *_First);
			++_Next;
		}
		return _Next;
	}
}

template<typename iter, typename Alloc, std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 alloc_pointer_t<Alloc> uninitialized_copy_n_a(
	iter _First, size_t _Count, alloc_pointer_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	if constexpr (use_default_allocator_construct_v<Alloc, pointer, decltype(*_First)>) {
		return std::uninitialized_copy_n(_First, _Count, _Dest);
	}
	else {
		pointer _Next = _Dest;
		for (; _Count > 0; --_Count) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), *_First);
			++_Next;
			++_First;
		}
		return _Next;
	}
}

template<typename iter, typename Alloc,	std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 alloc_pointer_t<Alloc> uninitialized_move_a(
	iter _First, iter _Last, alloc_pointer_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	if constexpr (use_default_allocator_construct_v<Alloc, pointer, decltype(std::move(*_First))>) {
		return std::uninitialized_move(_First, _Last, _Dest);
	}
	else {
		pointer _Next = _Dest;
		for (; _First != _Last; ++_First) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), std::move(*_First));
			++_Next;
		}
		return _Next;
	}
}

template<typename iter, typename Alloc,	std::enable_if_t<is_iterator_v<iter>, int> = 0>
WJR_CONSTEXPR20 std::pair<iter, alloc_pointer_t<Alloc>> uninitialized_move_n_a(
	iter _First, size_t _Count, alloc_pointer_t<Alloc> _Dest, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	if constexpr (use_default_allocator_construct_v<Alloc, pointer, decltype(std::move(*_First))>) {
		return std::uninitialized_move_n(_First, _Count, _Dest);
	}
	else {
		pointer _Next = _Dest;
		for (; _Count > 0; --_Count) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), std::move(*_First));
			++_Next;
			++_First;
		}
		return { _First, _Next };
	}
}

template<typename Alloc, typename ValueType>
WJR_CONSTEXPR20 void uninitialized_fill_a(
	alloc_pointer_t<Alloc> _First, alloc_pointer_t<Alloc> _Last,
	const ValueType& _Val, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	if constexpr (use_default_allocator_construct_v<Alloc, pointer, decltype(_Val)>) {
		std::uninitialized_fill(_First, _Last, _Val);
	}
	else {
		pointer _Next = _First;
		for (; _Next != _Last; ++_Next) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), _Val);
		}
	}
}

template<typename Alloc, typename ValueType>
WJR_CONSTEXPR20 alloc_pointer_t<Alloc> uninitialized_fill_n_a(
	alloc_pointer_t<Alloc> _First,
	alloc_size_t<Alloc> _Count,
	const ValueType& _Val, Alloc& al) {
	using pointer = alloc_pointer_t<Alloc>;
	if constexpr (use_default_allocator_construct_v<Alloc, pointer, decltype(_Val)>) {
		return std::uninitialized_fill_n(_First, _Count, _Val);
	}
	else {
		pointer _Next = _First;
		for (; 0 < _Count; --_Count) {
			std::allocator_traits<Alloc>::construct(al, std::addressof(*_Next), _Val);
			++_Next;
		}
		return _Next;
	}
}

_WJR_END

namespace std {
	REGISTER_HAS_GLOBAL_FUNCTION(begin, begin);
	REGISTER_HAS_GLOBAL_FUNCTION(cbegin, cbegin);
	REGISTER_HAS_GLOBAL_FUNCTION(end, end);
	REGISTER_HAS_GLOBAL_FUNCTION(cend, cend);
	REGISTER_HAS_GLOBAL_FUNCTION(rbegin, rbegin);
	REGISTER_HAS_GLOBAL_FUNCTION(crbegin, crbegin);
	REGISTER_HAS_GLOBAL_FUNCTION(rend, rend);
	REGISTER_HAS_GLOBAL_FUNCTION(crend, crend);
	REGISTER_HAS_GLOBAL_FUNCTION(data, data);
	REGISTER_HAS_GLOBAL_FUNCTION(size, size);
	REGISTER_HAS_MEMBER_FUNCTION(size, size);
}

_WJR_BEGIN

template<typename _Container>
constexpr auto size(const _Container& c){
	if constexpr (std::has_member_function_size_v<_Container>) {
		return c.size();
	}
	else {
		return std::distance(std::begin(c), std::end(c));
	}
}

template<typename _Ty, size_t _Size>
constexpr size_t size(const _Ty(&)[_Size]) noexcept {
	return _Size;
}

_WJR_END

#endif