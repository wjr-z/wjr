#pragma once
#ifndef __WJR_TYPE_TRAITS_H
#define __WJR_TYPE_TRAITS_H

#include <memory>
#include <stdexcept>
#include <iterator>
#include <vector>

#include <wjr/tag.h>

_WJR_BEGIN

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
	has_global_binary_operator_##NAME<T, U>::value;

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
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(+, add);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(-, minus);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(&, and);
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(| , or );
REGISTER_HAS_GLOBAL_BINARY_OPERATOR(^, xor);
REGISTER_HAS_GLOBAL_UNARY_OPERATOR(~, not);
REGISTER_HAS_STATIC_MEMBER_FUNCTION(min, min);
REGISTER_HAS_STATIC_MEMBER_FUNCTION(max, max);

template<typename T, typename...Args>
struct is_any_of : std::disjunction<std::is_same<T, Args>...> {};

template<typename T, typename...Args>
constexpr bool is_any_of_v = is_any_of<T, Args...>::value;

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

template<typename T>
struct is_memory_compare : std::is_integral<T> {};

template<typename T>
constexpr bool is_memory_compare_v = is_memory_compare<T>::value;

template<typename T>
struct is_unsigned_integral : std::conjunction<std::is_integral<T>, std::is_unsigned<T>> {};

template<typename T>
constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

template<typename T>
struct is_reverse_iterator : std::false_type {};

template<typename _Iter>
struct is_reverse_iterator<std::reverse_iterator<_Iter>> : std::negation<is_reverse_iterator<_Iter>> {};

template<typename T>
constexpr bool is_reverse_iterator_v = is_reverse_iterator<T>::value;

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

#if defined(WJR_CPP_20)
template<typename T>
struct is_contiguous_iterator : std::bool_constant<std::contiguous_iterator<T>> {};
#else
template<typename T>
struct is_contiguous_iterator : std::is_pointer<T> {};
#endif

template<typename T>
struct is_reverse_contiguous_iterator : std::false_type {};

template<typename _Iter>
struct is_reverse_contiguous_iterator<std::reverse_iterator<_Iter>> : is_contiguous_iterator<_Iter> {};

template<typename _Iter>
struct is_contiguous_iterator<std::reverse_iterator<_Iter>> : is_reverse_contiguous_iterator<_Iter> {};

template<typename T>
constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<T>::value;

template<typename T>
constexpr bool is_reverse_contiguous_iterator_v = is_reverse_contiguous_iterator<T>::value;

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

template<typename Tag, typename...Args, std::enable_if_t<wjr::tag::is_set_v<Tag>, int> = 0>
WJR_CONSTEXPR20 decltype(auto) uninitialized_a(
	Tag, Args&&...args
) {
	if constexpr (Tag{}(wjr::tag::DEFAULT_CONSTRUCT)) {
		return wjr::uninitialized_default_construct_a(std::forward<Args>(args)...);
	}
	else if constexpr (Tag{}(wjr::tag::VALUE_CONSTRUCT)) {
		return wjr::uninitialized_value_construct_a(std::forward<Args>(args)...);
	}
	else if constexpr (Tag{}(wjr::tag::COPY)) {
		return wjr::uninitialized_copy_a(std::forward<Args>(args)...);
	}
	else if constexpr (Tag{}(wjr::tag::MOVE)) {
		return wjr::uninitialized_move_a(std::forward<Args>(args)...);
	}
	else if constexpr (Tag{}(wjr::tag::FILL)) {
		return wjr::uninitialized_fill_a(std::forward<Args>(args)...);
	}
}

template<typename Tag, typename...Args, std::enable_if_t<wjr::tag::is_set_v<Tag>, int> = 0>
WJR_CONSTEXPR20 decltype(auto) uninitialized_n_a(
	Tag, Args&&...args
) {
	if constexpr (Tag{}(wjr::tag::DEFAULT_CONSTRUCT)) {
		return wjr::uninitialized_default_construct_n_a(std::forward<Args>(args)...);
	}
	else if constexpr (Tag{}(wjr::tag::VALUE_CONSTRUCT)) {
		return wjr::uninitialized_value_construct_n_a(std::forward<Args>(args)...);
	}
	else if constexpr (Tag{}(wjr::tag::COPY)) {
		return wjr::uninitialized_copy_n_a(std::forward<Args>(args)...);
	}
	else if constexpr (Tag{}(wjr::tag::MOVE)) {
		return wjr::uninitialized_move_n_a(std::forward<Args>(args)...);
	}
	else if constexpr (Tag{}(wjr::tag::FILL)) {
		return wjr::uninitialized_fill_n_a(std::forward<Args>(args)...);
	}
}

template<typename Alloc>
class temporary_allocator_value {
public:
	using value_type = alloc_value_t<Alloc>;
	using traits = std::allocator_traits<Alloc>;

	template<typename...Args>
	constexpr explicit temporary_allocator_value(Alloc& al, Args&&...args) noexcept
		: al(al) {
		traits::construct(al, get_ptr(), std::forward<Args>(args)...);
	}

	temporary_allocator_value(const temporary_allocator_value&) = delete;
	temporary_allocator_value& operator=(const temporary_allocator_value&) = delete;

	~temporary_allocator_value() {
		destroy_at_a(get_ptr(), al);
	}

	constexpr value_type* get_ptr() {
		return reinterpret_cast<value_type*>(std::addressof(vl));
	}

	constexpr const value_type* get_ptr() const {
		return reinterpret_cast<const value_type*>(std::addressof(vl));
	}

	constexpr value_type& value() {
		return *get_ptr();
	}

	constexpr const value_type& value() const {
		return *get_ptr();
	}

private:

	Alloc& al;
	std::aligned_storage_t<sizeof(value_type), alignof(value_type)> vl;
};

// 

class allocator_listener {
public:
	static int64_t allocated_size;
	static int64_t constructed_size;
};

template<typename T, typename Alloc = std::allocator<T>>
class test_allocator : public Alloc {
	using _Alty_traits = std::allocator_traits<Alloc>;
public:
	using Alloc::Alloc;
	template<typename Other>
	struct rebind {
		using other = test_allocator<Other, typename _Alty_traits::template rebind_alloc<Other>>;
	};

	test_allocator() = default;
	test_allocator(const test_allocator&) = default;
	test_allocator(test_allocator&&) = default;
	test_allocator& operator=(const test_allocator&) = default;
	test_allocator& operator=(test_allocator&&) = default;
	~test_allocator() = default;

	WJR_CONSTEXPR20 decltype(auto) allocate(size_t n) {
		allocator_listener::allocated_size += n * sizeof(typename _Alty_traits::value_type);
		__allocated_size += static_cast<int64_t>(n);
		return _Alty_traits::allocate(*this, n);
	}

	WJR_CONSTEXPR20 decltype(auto) deallocate(typename _Alty_traits::pointer p, size_t n) {
		allocator_listener::allocated_size -= n * sizeof(typename _Alty_traits::value_type);
		__allocated_size -= static_cast<int64_t>(n);
		return _Alty_traits::deallocate(*this, p, n);
	}

	template<typename _Ty, typename...Args>
	WJR_CONSTEXPR20 decltype(auto) construct(_Ty* p, Args&&... args) {
		allocator_listener::constructed_size++;
		__constructed_size++;
		return _Alty_traits::construct(*this, p, std::forward<Args>(args)...);
	}

	template<typename _Ty>
	WJR_CONSTEXPR20 decltype(auto) destroy(_Ty* p) {
		allocator_listener::constructed_size--;
		__constructed_size--;
		return _Alty_traits::destroy(*this, p);
	}

	WJR_CONSTEXPR20 static int64_t allocated_size() {
		return __allocated_size;
	}

	WJR_CONSTEXPR20 static int64_t constructed_size() {
		return __constructed_size;
	}

private:
	static int64_t __allocated_size;
	static int64_t __constructed_size;
};

template<typename T, typename Alloc>
int64_t test_allocator<T, Alloc>::__allocated_size = 0;

template<typename T, typename Alloc>
int64_t test_allocator<T, Alloc>::__constructed_size = 0;

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
}

#endif