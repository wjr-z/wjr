#pragma once
#ifndef __WJR_ALGO_ALOG_H
#define __WJR_ALGO_ALOG_H

#include <wjr/algo/memchr.h>
#include <wjr/algo/memrchr.h>
#include <wjr/algo/memcmp.h>
#include <wjr/algo/memmis.h>
#include <wjr/algo/memrmis.h>
#include <wjr/algo/memcnt.h>
#include <wjr/algo/memset.h>

// Note :
// 1. When the data volume is very small,
// the performance of the following simd functions
// may not be better than some optimized standard library functions
// 2. Limited by the inability to determine whether the function parameters are compile-time constants,
// in order to improve dynamic performance, some cases when the parameters are constants,
// the compiler may not be able to perform enough optimizations, such as inlining, and better optimization

_WJR_ALGO_BEGIN

#if defined(__HAS_FAST_MEMCHR)
template<typename T, typename U>
struct __has_fast_memchr : std::conjunction<
	is_comparable<T, U, std::equal_to<>>,
	is_normalized_integrals<integral_normalization_t<T>, integral_normalization_t<U>>,
	is_any_of<std::make_unsigned_t<integral_normalization_t<T>>, uint8_t, uint16_t, uint32_t>
> {};

template<typename T, typename U>
constexpr bool __has_fast_memchr_v = __has_fast_memchr<T, U>::value;

template<typename T, typename U>
struct __has_fast_memrchr : __has_fast_memchr<T, U> {};

template<typename T, typename U>
constexpr bool __has_fast_memrchr_v = __has_fast_memrchr<T, U>::value;

template<typename T, typename U, std::enable_if_t<__has_fast_memchr_v<T, U>, int> = 0>
const T* memchr(const T* s, U val, size_t n) {
	auto p = is_possible_memory_comparable<T>(val, std::equal_to<>{});
	if (p == ipmc_result::none) {
		return s + n;
	}
	if (p == ipmc_result::all) {
		return s;
	}
	using value_type = std::make_unsigned_t<integral_normalization_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
	auto __val = static_cast<value_type>(make_integral_normalization(val));
	return reinterpret_cast<const T*>(__memchr(__s, __val, n));
}
template<typename T, typename U, std::enable_if_t<__has_fast_memchr_v<T, U>, int> = 0>
const T* memrchr(const T* s, U val, size_t n) {
	auto p = is_possible_memory_comparable<T>(val, std::equal_to<>{});
	if (p == ipmc_result::none) {
		return s;
	}
	if (p == ipmc_result::all) {
		return s + n;
	}
	using value_type = std::make_unsigned_t<integral_normalization_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
	return reinterpret_cast<const T*>(__memrchr(__s, static_cast<value_type>(val), n));
}
#else
template<typename T, typename U>
struct __has_fast_memchr : std::conjunction<
	is_comparable<T, U, std::equal_to<>>,
	is_normalized_integrals<T, U>,
	is_any_index_of<sizeof(T), sizeof(char), sizeof(wchar_t)>
> {};

template<typename T, typename U>
constexpr bool __has_fast_memchr_v = __has_fast_memchr<T, U>::value;

template<typename T, typename U>
struct __has_fast_memrchr : std::false_type {};

template<typename T, typename U>
constexpr bool __has_fast_memrchr_v = __has_fast_memrchr<T, U>::value;

template<typename T, typename U, std::enable_if_t<__has_fast_memchr_v<T, U>, int> = 0>
const T* memchr(const T* s, U val, size_t n) {
	auto p = is_possible_memory_comparable<T>(val, std::equal_to<>{});
	if (p == ipmc_result::none) {
		return s + n;
	}
	if (p == ipmc_result::all) {
		return s;
	}
	using value_type = std::make_unsigned_t<integral_normalization_t<T>>;
	if constexpr (sizeof(T) == sizeof(char)) {
		auto pos = ::memchr(reinterpret_cast<const char*>(s), static_cast<T>(val), n);
		if (pos == nullptr) {
			return s + n;
		}
		return reinterpret_cast<const T*>(pos);
	}
	else {
		auto pos = ::wmemchr(reinterpret_cast<const wchar_t*>(s), static_cast<T>(val), n);
		if (pos == nullptr) {
			return s + n;
		}
		return reinterpret_cast<const T*>(pos);
	}
}

#endif // __HAS_FAST_MEMCHR

#if defined(__HAS_FAST_MEMCMP)
template<typename T, typename U, typename _Pred>
struct __has_fast_memcmp : std::conjunction<
	is_memory_comparable<T, U, _Pred>,
	is_any_of<std::make_unsigned_t<integral_normalization_t<T>>, uint8_t, uint16_t, uint32_t>
> {};

template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::equal_to<>>
> {};

template<typename T, typename U>
struct __has_fast_memcmp<T, U, std::not_equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::not_equal_to<>>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_fast_memcmp_v = __has_fast_memcmp<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_fast_memcmp_v<T, U, _Pred>, int> = 0>
bool memcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (is_any_of_v<_Pred, std::equal_to<>, std::not_equal_to<>>) {
		using value_type = uint8_t;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		return __memcmp(__s0, __s1, n * sizeof(T), pred);
	}
	else {
		using value_type = integral_normalization_t<T>;
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

#endif // __HAS_FAST_MEMCMP

#if defined(__HAS_FAST_MEMMIS)
template<typename T, typename U, typename _Pred>
struct __has_fast_memmis : std::conjunction<
	is_memory_comparable<T, U, _Pred>,
	is_any_of<std::make_unsigned_t<integral_normalization_t<T>>, uint8_t, uint16_t, uint32_t>
> {};

template<typename T, typename U>
struct __has_fast_memmis<T, U, std::equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::equal_to<>>
> {};

template<typename T, typename U>
struct __has_fast_memmis<T, U, std::not_equal_to<>> : std::conjunction<
	is_memory_comparable<T, U, std::not_equal_to<>>
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
	if constexpr (is_any_of_v<_Pred, std::equal_to<>, std::not_equal_to<>>) {
		using value_type = uint8_t;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		auto __pos =
			__memmis(__s0, __s1, n * sizeof(T), pred);
		auto __dif = (__pos - __s0) / sizeof(T);
		return s0 + __dif;
	}
	else {
		using value_type = integral_normalization_t<T>;
		auto __s0 = reinterpret_cast<const value_type*>(s0);
		auto __s1 = reinterpret_cast<const value_type*>(s1);
		return reinterpret_cast<const T*>(__memmis(__s0, __s1, n, pred));
	}
}

template<typename T, typename U, typename _Pred, std::enable_if_t<
	__has_fast_memmis_v<T, U, _Pred>, int> = 0>
const T* memrmis(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (is_any_of_v<_Pred, std::equal_to<>, std::not_equal_to<>>) {
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
		using value_type = integral_normalization_t<T>;
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

#endif // __HAS_FAST_MEMMIS

#if defined(__HAS_FAST_MEMCNT)
template<typename T, typename U>
struct __has_fast_memcnt : std::conjunction<
	is_comparable<T, U, std::equal_to<>>,
	is_normalized_integrals<integral_normalization_t<T>, integral_normalization_t<U>>,
	is_any_of<std::make_unsigned_t<integral_normalization_t<T>>, uint8_t, uint16_t, uint32_t>
> {};

template<typename T, typename U>
constexpr bool __has_fast_memcnt_v = __has_fast_memcnt<T, U>::value;

template<typename T, typename U, std::enable_if_t<__has_fast_memcnt_v<T, U>, int> = 0>
size_t memcnt(const T* s, U val, size_t n) {
	auto p = is_possible_memory_comparable<T>(val, std::equal_to<>{});
	if (p == ipmc_result::none) {
		return 0;
	}
	using value_type = std::make_unsigned_t<integral_normalization_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
	auto __val = static_cast<value_type>(make_integral_normalization(val));
	return __memcnt(__s, __val, n);
}

#else
template<typename T, typename U>
struct __has_fast_memcnt : std::false_type {};

template<typename T, typename U>
constexpr bool __has_fast_memcnt_v = __has_fast_memcnt<T, U>::value;

#endif // __HAS_FAST_MEMCNT

#if defined(__HAS_FAST_MEMSET)
template<typename T, typename U>
struct __has_fast_memset : std::conjunction<
	is_normalized_integrals<integral_normalization_t<T>, integral_normalization_t<U>>,
	is_any_of<std::make_unsigned_t<integral_normalization_t<T>>, uint8_t, uint16_t, uint32_t, uint64_t>
> {};

template<typename T, typename U>
constexpr bool __has_fast_memset_v = __has_fast_memset<T, U>::value;

template<typename T, typename U, std::enable_if_t<__has_fast_memset_v<T, U>, int> = 0>
void memset(T* s, U val, size_t n) {
	using value_type = std::make_unsigned_t<integral_normalization_t<T>>;
	auto __s = reinterpret_cast<value_type*>(s);
	auto __val = static_cast<value_type>(make_integral_normalization(val));
	__memset(__s, __val, n);
}
#else
template<typename T, typename U>
struct __has_fast_memset : std::conjunction<
	is_normalized_integrals<integral_normalization_t<T>, integral_normalization_t<U>>,
	is_any_of<std::make_unsigned_t<integral_normalization_t<T>>, uint8_t>
> {};

template<typename T, typename U>
constexpr bool __has_fast_memset_v = __has_fast_memset<T, U>::value;

template<typename T, typename U, std::enable_if_t<__has_fast_memset_v<T, U>, int> = 0>
void memset(T* s, U val, size_t n) {
	using value_type = std::make_unsigned_t<integral_normalization_t<T>>;
	auto __s = reinterpret_cast<value_type*>(s);
	auto __val = static_cast<value_type>(make_integral_normalization(val));

	::memset(__s, __val, n);
}

#endif // __HAS_FAST_MEMSET

_WJR_ALGO_END

#endif // __WJR_ALGO_ALOG_H