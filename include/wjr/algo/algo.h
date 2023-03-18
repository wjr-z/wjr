#pragma once
#ifndef __WJR_ALGO_ALOG_H
#define __WJR_ALGO_ALOG_H

#include <wjr/Optimizer.h>

#include <wjr/literals.h>
#include <wjr/simd/simd_intrin.h>

#include <wjr/algo/memchr.h>
#include <wjr/algo/memrchr.h>
#include <wjr/algo/memcmp.h>
#include <wjr/algo/memmis.h>
#include <wjr/algo/memrmis.h>
#include <wjr/algo/memcnt.h>
#include <wjr/algo/memset.h>

_WJR_ALGO_BEGIN

#if defined(_WJR_FAST_MEMCHR)
template<typename T, typename U, typename _Pred>
struct __has_fast_memchr : std::conjunction<
	is_any_of<_Pred, std::equal_to<>, std::not_equal_to<>>,
	is_comparable<T, U, _Pred>,
	is_integrals<T, U>,
	is_any_index_of<sizeof(T), 1, 2, 4>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memchr_v = __has_fast_memchr<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_fast_memrchr : __has_fast_memchr<T, U, _Pred> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrchr_v = __has_fast_memrchr<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memchr_v<T, U, _Pred>, int> = 0>
const T* memchr(const T* s, U val, size_t n, _Pred pred) {
	auto p = is_possible_memory_comparable<T>(val, pred);
	if (p == ipmc_result::none) {
		return s + n;
	}
	if (p == ipmc_result::all) {
		return s;
	}
	using value_type = std::make_unsigned_t<make_integral_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
	auto __val = static_cast<value_type>(static_cast<T>(val));
	return reinterpret_cast<const T*>(__memchr(__s, __val, n, pred));
}
template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memchr_v<T, U, _Pred>, int> = 0>
const T* memrchr(const T* s, U val, size_t n, _Pred pred) {
	auto p = is_possible_memory_comparable<T>(val, pred);
	if (p == ipmc_result::none) {
		return s;
	}
	if (p == ipmc_result::all) {
		return s + n;
	}
	using value_type = std::make_unsigned_t<make_integral_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
	auto __val = static_cast<value_type>(val);
	return reinterpret_cast<const T*>(__memrchr(__s, __val, n, pred));
}
#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memchr : std::conjunction<
	std::is_same<_Pred, std::equal_to<>>,
	is_comparable<T, U, _Pred>,
	is_integrals<T, U>,
	is_any_index_of<sizeof(T), sizeof(char), sizeof(wchar_t)>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memchr_v = __has_fast_memchr<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_fast_memrchr : std::false_type {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrchr_v = __has_fast_memrchr<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memchr_v<T, U, _Pred>, int> = 0>
const T* memchr(const T* s, U val, size_t n, _Pred pred) {
	auto p = is_possible_memory_comparable<T>(val, pred);
	if (p == ipmc_result::none) {
		return s + n;
	}
	if (p == ipmc_result::all) {
		return s;
	}
	if constexpr (sizeof(T) == sizeof(char)) {
		using value_type = char;
		auto __s = reinterpret_cast<const value_type*>(s);
		auto __val = static_cast<value_type>(val);
		auto pos = ::memchr(__s, __val, n);
		if (pos == nullptr) {
			return s + n;
		}
		return reinterpret_cast<const T*>(pos);
	}
	else {
		using value_type = wchar_t;
		auto __s = reinterpret_cast<const value_type*>(s);
		auto __val = static_cast<value_type>(val);
		auto pos = ::wmemchr(__s, __val, n);
		if (pos == nullptr) {
			return s + n;
		}
		return reinterpret_cast<const T*>(pos);
	}
}

#endif // _WJR_FAST_MEMCHR

#if defined(_WJR_FAST_MEMCMP)
template<typename T, typename U, typename _Pred>
struct __has_fast_memcmp : std::conjunction<
	is_memory_comparable<T, U, _Pred>,
	is_integrals<T, U>,
	is_any_index_of<sizeof(T), 1, 2, 4>
> {};

template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::equal_to<>>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memcmp_v = __has_fast_memcmp<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memcmp_v<T, U, _Pred>, int> = 0>
bool memcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (is_any_of_v<_Pred, std::equal_to<>>) {
		using value_type = uint8_t;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		return __memcmp(__s0, __s1, n * sizeof(T), pred);
	}
	else {
		using value_type = make_integral_t<T>;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		return __memcmp(__s0, __s1, n, pred);
	}
}

#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memcmp : std::false_type {};

template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::equal_to<>>
> {};

template<typename T, typename U, typename _Pred>
inline constexpr bool __has_fast_memcmp_v = __has_fast_memcmp<T, U, _Pred>::type::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memcmp_v<T, U, _Pred>, int> = 0>
bool memcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
	return ::memcmp(s0, s1, n * sizeof(T)) == 0;
}

#endif // _WJR_FAST_MEMCMP

#if defined(_WJR_FAST_MEMMIS)
template<typename T, typename U, typename _Pred>
struct __has_fast_memmis : std::conjunction<
	is_memory_comparable<T, U, _Pred>,
	is_integrals<T, U>,
	is_any_index_of<sizeof(T), 1, 2, 4>
> {};

template<typename T, typename U>
struct __has_fast_memmis<T, U, std::equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::equal_to<>>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memmis_v = __has_fast_memmis<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_fast_memrmis : __has_fast_memmis<T, U, _Pred> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrmis_v = __has_fast_memrmis<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<
	__has_fast_memmis_v<T, U, _Pred>, int> = 0>
const T* memmis(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (is_any_of_v<_Pred, std::equal_to<>>) {
		using value_type = uint8_t;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		auto __pos =
			__memmis(__s0, __s1, n * sizeof(T), pred);
		auto __dif = (__pos - __s0) / sizeof(T);
		return s0 + __dif;
	}
	else {
		using value_type = make_integral_t<T>;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		return reinterpret_cast<const T*>(__memmis(__s0, __s1, n, pred));
	}
}

template<typename T, typename U, typename _Pred, std::enable_if_t<
	__has_fast_memmis_v<T, U, _Pred>, int> = 0>
const T* memrmis(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (is_any_of_v<_Pred, std::equal_to<>>) {
		using value_type = uint8_t;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		auto __pos =
			__memrmis(__s0, __s1, n * sizeof(T), pred);
		auto __e0 = __s0 + n * sizeof(T);
		auto __dif = (__e0 - __pos) / sizeof(T);
		return (s0 + n) - __dif;
	}
	else {
		using value_type = make_integral_t<T>;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		return reinterpret_cast<const T*>(__memrmis(__s0, __s1, n, pred));
	}
}

#else
template<typename T, typename U, typename _Pred>
struct __has_fast_memmis : std::false_type {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memmis_v = __has_fast_memmis<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_fast_memrmis : __has_fast_memmis<T, U, _Pred> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memrmis_v = __has_fast_memrmis<T, U, _Pred>::value;

#endif // _WJR_FAST_MEMMIS

#if defined(_WJR_FAST_MEMCNT)
template<typename T, typename U>
struct __has_fast_memcnt : std::conjunction<
	is_comparable<T, U, std::equal_to<>>,
	is_integrals<T, U>,
	is_any_index_of<sizeof(T), 1, 2, 4>
> {};

template<typename T, typename U>
constexpr bool __has_fast_memcnt_v = __has_fast_memcnt<T, U>::value;

template<typename T, typename U, std::enable_if_t<__has_fast_memcnt_v<T, U>, int> = 0>
size_t memcnt(const T* s, U val, size_t n) {
	auto p = is_possible_memory_comparable<T>(val, std::equal_to<>{});
	if (p == ipmc_result::none) {
		return 0;
	}
	using value_type = std::make_unsigned_t<make_integral_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
	auto __val = static_cast<value_type>(val);
	return __memcnt(__s, __val, n);
}

#else
template<typename T, typename U>
struct __has_fast_memcnt : std::false_type {};

template<typename T, typename U>
constexpr bool __has_fast_memcnt_v = __has_fast_memcnt<T, U>::value;

#endif // _WJR_FAST_MEMCNT

template<template<typename X, typename Y> typename TEST, typename T, typename U>
struct __has_fast_memset_helper : std::conjunction<
	std::bool_constant<TEST<T, U>::is_fill>
> {};

template<template<typename X, typename Y> typename TEST, typename T, typename U>
constexpr bool __has_fast_memset_helper_v = __has_fast_memset_helper<TEST, T, U>::value;

template<template<typename X, typename Y> typename TEST, typename T, typename U>
static void __memset_helper(T* s, const U& val, size_t n) {
	
	if (is_constant_p(n) && n <= 4 / sizeof(T)) {
		std::fill_n(s, n, val);
		return;
	}

	const auto& _Val = TEST<T, const U&>::get(val);

	auto __bytes_num = _Get_bytes_num<sizeof(T)>(_Val);

#if defined(_WJR_FAST_MEMSET)
	constexpr auto __max_bytes_num = _Get_max_bytes_num<sizeof(T)>();
	if (__bytes_num == 0) {
		std::fill_n(s, n, val);
		return;
	}
#else
	constexpr auto __max_bytes_num = 1;
	if (__bytes_num != 1) {
		std::fill_n(s, n, val);
		return;
	}
#endif 

	using value_type = uint_t<__max_bytes_num * 8>;
	auto __s = reinterpret_cast<value_type*>(s);
	auto __val = *reinterpret_cast<const value_type*>(&_Val);
	static_assert(std::is_same_v<decltype(__val), value_type>, "type mismatch");
	
#if defined(_WJR_FAST_MEMSET)
	__memset(__s, __val, n * (sizeof(T) / __max_bytes_num));
#else
	::memset(__s, __val, n * (sizeof(T) / __max_bytes_num));
#endif
}

template<typename T, typename U>
struct __has_fast_construct_memset : __has_fast_memset_helper<is_byte_constructible, T, U> {};

template<typename T, typename U>
constexpr bool __has_fast_construct_memset_v = __has_fast_construct_memset<T, U>::value;

template<typename T, typename U>
void construct_memset(T* s, const U& val, size_t n) {
	__memset_helper<is_byte_constructible>(s, val, n);
}

template<typename T, typename U>
struct __has_fast_assign_memset : __has_fast_memset_helper<is_byte_assignable, T, U> {};

template<typename T, typename U>
constexpr bool __has_fast_assign_memset_v = __has_fast_assign_memset<T, U>::value;

template<typename T, typename U>
void assign_memset(T* s, const U& val, size_t n) {
	__memset_helper<is_byte_assignable>(s, val, n);
}

template<template<typename X, typename Y> typename TEST, typename T, typename U>
struct __has_fast_memcpy_helper : std::conjunction <
	std::bool_constant<TEST<T, U>::is_copy>
> {};

template<template<typename X, typename Y> typename TEST, typename T, typename U>
constexpr bool __has_fast_memcpy_helper_v = __has_fast_memcpy_helper<TEST, T, U>::value;

template<template<typename X, typename Y> typename TEST, typename T, typename U>
static void __memcpy_helper(T* s, const U* t, size_t n) {
	static_assert(sizeof(T) == sizeof(U), "type mismatch");
#if defined(_WJR_FAST_MEMCPY)
#else
	::memcpy(s, t, n * sizeof(T));
#endif
}

template<template<typename X, typename Y> typename TEST, typename T, typename U>
static void __memmove_helper(T* s, const U* t, size_t n) {
	static_assert(sizeof(T) == sizeof(U), "type mismatch");
#if defined(_WJR_FAST_MEMCPY)
#else
	::memmove(s, t, n * sizeof(T));
#endif
}

template<typename T, typename U>
struct __has_fast_construct_memcpy : __has_fast_memcpy_helper<is_byte_constructible, T, U> {};

template<typename T, typename U>
constexpr bool __has_fast_construct_memcpy_v = __has_fast_construct_memcpy<T, U>::value;

template<typename T, typename U>
void construct_memcpy(T* s, const U* t, size_t n) {
	__memcpy_helper<is_byte_constructible>(s, t, n);
}

template<typename T, typename U>
void construct_memmove(T* s, const U* t, size_t n) {
	__memmove_helper<is_byte_constructible>(s, t, n);
}

template<typename T, typename U>
struct __has_fast_assign_memcpy : __has_fast_memcpy_helper<is_byte_assignable, T, U> {};

template<typename T, typename U>
constexpr bool __has_fast_assign_memcpy_v = __has_fast_assign_memcpy<T, U>::value;

template<typename T, typename U>
void assign_memcpy(T* s, const U* t, size_t n) {
	__memcpy_helper<is_byte_assignable>(s, t, n);
}

template<typename T, typename U>
void assign_memmove(T* s, const U* t, size_t n) {
	__memmove_helper<is_byte_assignable>(s, t, n);
}

_WJR_ALGO_END

#endif // __WJR_ALGO_ALOG_H