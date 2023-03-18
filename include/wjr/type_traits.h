#pragma once
#ifndef __WJR_TYPE_TRAITS_H
#define __WJR_TYPE_TRAITS_H

#include <functional>
#include <limits>

#include <wjr/cpuinfo.h>

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

WJR_NORETURN WJR_INTRINSIC_INLINE void unreachable() noexcept {
	WJR_UNREACHABLE;
}

template<typename T>
WJR_INTRINSIC_CONSTEXPR bool is_constant_p(T x) noexcept {
#if WJR_HAS_BUILTIN(__builtin_constant_p) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	return __builtin_constant_p(x);
#else
	return is_constant_evaluated();
#endif
}

template<typename T>
WJR_INTRINSIC_CONSTEXPR void assume_no_add_overflow(T a, T b) noexcept {
	constexpr T _Max = std::numeric_limits<T>::max();
	WJR_ASSUME(a <= _Max - b);
}

template<typename T>
WJR_INTRINSIC_CONSTEXPR void assume_no_sub_overflow(T a, T b) noexcept {
	WJR_ASSUME(a >= b);
}

struct disable_tag {};

struct default_construct_tag {};
struct value_construct_tag {};

struct extend_tag {};
struct reserve_tag {};

struct defer_tag {};

template<typename T, typename U, typename _Pred>
struct has_global_binary_operator : std::false_type {};

template<typename T, typename U, typename _Pred>
constexpr bool has_global_binary_operator_v = has_global_binary_operator<T, U, _Pred>::value;

WJR_REGISTER_HAS_MEMBER_FUNCTION(operator(), call_operator);
WJR_REGISTER_HAS_MEMBER_FUNCTION(operator[], subscript_operator);
WJR_REGISTER_HAS_MEMBER_FUNCTION(operator->, point_to_operator);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(+, plus);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(-, minus);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(&, bit_and);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(| , bit_or);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(^, bit_xor);
WJR_REGISTER_HAS_GLOBAL_UNARY_OPERATOR(~, bit_not);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(== , equal_to);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(!= , not_equal_to);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(> , greater);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(>= , greater_equal);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(< , less);
WJR_REGISTER_HAS_GLOBAL_BINARY_OPERATOR(<= , less_equal);
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(min, min);
WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(max, max);

template<typename T>
using aligned_storage_for_t = std::aligned_storage_t<sizeof(T), alignof(T)>;

template<typename T, typename...Args>
struct is_any_of : std::disjunction<std::is_same<T, Args>...> {};

template<typename T, typename...Args>
constexpr bool is_any_of_v = is_any_of<T, Args...>::value;

template<typename T>
using remove_ref_t = std::remove_reference_t<T>;

template<typename T>
using remove_cvref_t = std::remove_cv_t<remove_ref_t<T>>;

template<typename T>
using remove_cref_t = std::remove_const_t<remove_ref_t<T>>;

template<typename T>
using add_lref_t = std::add_lvalue_reference_t<T>;

template<typename T>
using add_rref_t = std::add_rvalue_reference_t<T>;

template<typename T>
using add_cvref_t = add_lref_t<std::add_cv_t<T>>;

template<typename T>
using add_cref_t = add_lref_t<std::add_const_t<T>>;

template<typename T>
struct is_standard_comparator : 
	is_any_of<remove_cvref_t<T>, 
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
struct is_left_standard_comparator :
	is_any_of<std::remove_cv_t<T>,
	std::less<>,
	std::less_equal<>
	> {};

template<typename T>
constexpr bool is_left_standard_comparator_v = is_left_standard_comparator<T>::value;

template<typename T>
struct is_right_standard_comparator :
	is_any_of<remove_cvref_t<T>,
	std::greater<>,
	std::greater_equal<>
	> {};

template<typename T>
constexpr bool is_right_standard_comparator_v = is_right_standard_comparator<T>::value;

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

template<typename T>
using iter_ref_t = typename std::iterator_traits<T>::reference;

//template<typename T>
//using iter_ptr_t = typename std::iterator_traits<T>::pointer;

template<typename T, typename = void>
struct is_iterator : std::false_type {};

template<typename T>
struct is_iterator<T, std::void_t<iter_cat_t<T>>> : std::true_type {};

template<typename T>
constexpr bool is_iterator_v = is_iterator<T>::value;

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

template<typename _Iter, typename = void>
struct _is_contiguous_iter_helper : std::false_type {};

template<typename _Iter>
struct _is_contiguous_iter_helper<_Iter, typename _Iter::is_contiguous_iter> : std::true_type {};

template<typename T>
struct _is_contiguous_iter_helper<T*, void> : std::true_type {};

#if defined(WJR_CPP_20)
template<typename T>
struct is_contiguous_iter : std::bool_constant<std::contiguous_iterator<T> || _is_contiguous_iter_helper<T>::value> {};
#else
template<typename T>
struct is_contiguous_iter : _is_contiguous_iter_helper<T> {};
#endif

template<typename _Iter>
struct is_contiguous_iter<std::reverse_iterator<_Iter>> : is_contiguous_iter<_Iter> {};

template<typename T>
constexpr bool is_contiguous_iter_v = is_contiguous_iter<T>::value;

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

template<size_t n, bool __s>
using int_or_uint_t = std::conditional_t<__s, int_t<n>, uint_t<n>>;

template<typename T>
struct make_integral {
	static_assert(std::is_integral_v<T>, "T must be an integral type");
	using type = int_or_uint_t<sizeof(T) * 8, std::is_signed_v<T>>;
};

template<typename T>
using make_integral_t = typename make_integral<T>::type;

template<typename T>
constexpr std::make_unsigned_t<T> make_unsigned(T t) {
	return static_cast<std::make_unsigned_t<T>>(t);
}

template<typename T, typename U, typename _Pred>
struct is_comparable : std::conjunction<
	has_global_binary_operator<T, U, _Pred>,
	has_global_binary_operator<U, T, _Pred>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool is_comparable_v = is_comparable<T, U, _Pred>::value;

// note that (int8_t)(-1) != (uint8_t)(-1)
// but in memory they are the same
template<typename T, typename U, bool = sizeof(T) == sizeof(U) && std::is_integral_v<T> && std::is_integral_v<U>>
constexpr bool __is_memory_comparable_helper_v =
std::is_same_v<T, bool> || std::is_same_v<U, bool> || static_cast<T>(-1) == static_cast<U>(-1);

template<typename T, typename U>
constexpr bool __is_memory_comparable_helper_v<T, U, false> = false;

template<typename T, typename U, typename _Pred>
struct is_memory_comparable : std::conjunction<
	is_standard_comparator<_Pred>,
	is_comparable<T, U, _Pred>,
	std::bool_constant<__is_memory_comparable_helper_v<T, U>>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool is_memory_comparable_v = is_memory_comparable<T, U, _Pred>::value;

enum class ipmc_result {
	none,
	all,
	exit
};

template<typename T, typename U, typename _Pred, 
	std::enable_if_t<is_any_of_v<_Pred, std::equal_to<>, std::not_equal_to<>>, int> = 0>
constexpr ipmc_result is_possible_memory_comparable(const U& v, _Pred) {
	static_assert(std::is_integral_v<T> && std::is_integral_v<U>, "T and U must be integral types");
	using nt = T;
	using nu = U;
	using cat = std::common_type_t<nt, nu>;
	auto _Val = static_cast<cat>(v);
	
	constexpr auto __nt_min = static_cast<cat>(std::numeric_limits<nt>::min());
	constexpr auto __nt_max = static_cast<cat>(std::numeric_limits<nt>::max());
	constexpr auto __nt_negone = static_cast<cat>(static_cast<nt>(-1));

	if constexpr (std::is_signed_v<nt> && std::is_unsigned_v<cat>) {
		static_assert(__nt_negone == std::numeric_limits<cat>::max(), "error");
		static_assert(__nt_min <= __nt_negone, "error");
		static_assert(__nt_max < __nt_min, "error");
		if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
			return ((__nt_min <= _Val && _Val <= __nt_negone)
				|| _Val <= __nt_max) ? ipmc_result::exit : ipmc_result::none;
		}
		else {
			return (__nt_max < _Val && _Val < __nt_min) ? 
				ipmc_result::all : ipmc_result::exit;
		}
	}
	else {
		static_assert(__nt_min <= __nt_max, "error");
		if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
			return (__nt_min <= _Val && _Val <= __nt_max) ? 
				ipmc_result::exit : ipmc_result::none;
		}
		else {
			return (__nt_max < _Val || _Val < __nt_min) ?
				ipmc_result::all : ipmc_result::exit;
		}
	}
}

template<typename T>
struct _Auto_variable_helper {
	using type = std::conditional_t<sizeof(T) <= 256 / 8, T, const T&>;
};

template<typename T>
using auto_var_t = typename _Auto_variable_helper<T>::type;

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

template<typename...Args>
struct is_floating_points : std::conjunction<std::is_floating_point<Args>...> {};

template<typename...Args>
constexpr bool is_floating_points_v = is_floating_points<Args...>::value;

template<typename...Args>
struct is_arithmetics : std::conjunction<std::is_arithmetic<Args>...> {};

template<typename...Args>
constexpr bool is_arithmetics_v = is_arithmetics<Args...>::value;

template<typename T>
struct is_unsigned_integral : std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {};

template<typename T>
constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

template<typename T>
struct is_signed_integral : std::conjunction<std::is_integral<T>, std::is_signed<T>> {};

template<typename T>
constexpr bool is_signed_integral_v = is_signed_integral<T>::value;

template<typename T>
struct is_standard_numer : std::conjunction<std::is_arithmetic<T>, 
	std::negation<std::is_same<std::remove_cv_t<T>, bool>>> {};

template<typename T>
constexpr bool is_standard_numer_v = is_standard_numer<T>::value;

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

namespace _To_address_helper {
	WJR_REGISTER_HAS_STATIC_MEMBER_FUNCTION(to_address, to_address);
}

template<typename T>
constexpr auto to_address(T* p) noexcept {
	static_assert(!std::is_function_v<T>, "");
	return p;
}

template<typename T>
constexpr auto to_address(const T& p) noexcept {
	if constexpr (_To_address_helper::has_static_member_function_to_address_v<std::pointer_traits<T>, T>) {
		return std::pointer_traits<T>::to_address(p);
	}
	else {
		return wjr::to_address(p.operator->());
	}
}

template<typename T>
constexpr void* voidify(const T* ptr) {
	return const_cast<void*>(static_cast<const volatile void*>(ptr));
}

template<typename T>
constexpr auto get_address(T* p) noexcept {
	return p;
}

template<typename T>
constexpr auto get_address(const T& p) noexcept {
	if constexpr (_To_address_helper::has_static_member_function_to_address_v<std::pointer_traits<T>, T>) {
		return std::pointer_traits<T>::to_address(p);
	}
	else if constexpr (wjr::has_member_function_point_to_operator_v<add_cref_t<T>>) {
		return wjr::get_address(p.operator->());
	}
	else {
		return std::addressof(*std::forward<T>(p));
	}
}

template<typename iter>
using iter_address_t = std::add_pointer_t<remove_ref_t<iter_ref_t<iter>>>;

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
using alloc_ptr_t = typename std::allocator_traits<T>::pointer;

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

WJR_REGISTER_HAS_MEMBER_FUNCTION(destroy, destroy);
WJR_REGISTER_HAS_MEMBER_FUNCTION(construct, construct);

// Use the default destructor, which is beneficial for optimization.
// But if the destructor of the allocator has side effects, then do not use this allocator.

template<typename Alloc, typename Iter, typename...Args>
struct is_default_allocator_construct : std::disjunction<is_default_allocator<Alloc>,
	std::negation<has_member_function_construct<Alloc, iter_address_t<Iter>, Args...>>> {};

template<typename Alloc, typename Iter, typename...Args>
constexpr bool is_default_allocator_construct_v = is_default_allocator_construct<Alloc, Iter, Args...>::value;

template<typename Alloc, typename Iter>
struct is_default_allocator_destroy : std::disjunction<is_default_allocator<Alloc>,
	std::negation<has_member_function_destroy<Alloc, iter_address_t<Iter>>>> {};

template<typename Alloc, typename Iter>
constexpr bool is_default_allocator_destroy_v = is_default_allocator_destroy<Alloc, Iter>::value;

_WJR_END

namespace std {
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(begin, begin);
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(cbegin, cbegin);
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(end, end);
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(cend, cend);
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(rbegin, rbegin);
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(crbegin, crbegin);
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(rend, rend);
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(crend, crend);
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(data, data);
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(size, size);
	WJR_REGISTER_HAS_MEMBER_FUNCTION(size, size);
	WJR_REGISTER_HAS_GLOBAL_FUNCTION(swap, swap);
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