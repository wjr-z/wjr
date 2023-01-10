#pragma once
#ifndef __WJR_SIMD_SIMD_H
#define __WJR_SIMD_SIMD_H

#include <wjr/simd/simd_helper.h>
#include <wjr/simd/__memchr.h>
#include <wjr/simd/__memrchr.h>
#include <wjr/simd/__memcmp.h>
#include <wjr/simd/__memmis.h>
#include <wjr/simd/__memcnt.h>

#if defined(__AVX2__) || defined(__SSE2__)
#define __HAS_SIMD_MEMCHR
#define __HAS_SIMD_MEMCMP
#define __HAS_SIMD_MEMMIS
#define __HAS_SIMD_MEMCNT
#endif // __AVX2__ || __SSE2__

_WJR_SIMD_BEGIN

template<typename T, typename U>
struct __has_simple_memory_equal : std::conjunction<
	wjr::is_memory_compare<T, U>,
	std::is_same<std::make_unsigned_t<get_memory_t<T>>, std::make_unsigned_t<get_memory_t<U>>>,
	std::conditional_t<
	std::is_signed_v<T> == std::is_signed_v<U>,
	std::true_type,
	std::negation<std::disjunction<wjr::is_no_symbol_integral<T>, wjr::is_no_symbol_integral<U>>>
	>
> {};

#if defined(__HAS_SIMD_MEMCHR)
template<typename T>
struct __has_simd_memchr : std::conjunction<
	wjr::is_memory_compare<T, T>,
	wjr::is_any_of<std::make_unsigned_t<get_memory_t<T>>, uint8_t, uint16_t, uint32_t>
> {};

template<typename T>
constexpr bool __has_simd_memchr_v = __has_simd_memchr<T>::value;

template<typename T>
struct __has_simd_memrchr : __has_simd_memchr<T> {};

template<typename T>
constexpr bool __has_simd_memrchr_v = __has_simd_memrchr<T>::value;

template<typename T, std::enable_if_t<__has_simd_memchr_v<T>, int> = 0>
const T* memchr(const T* s, T val, size_t n) {
	using value_type = std::make_unsigned_t<get_memory_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
#if defined(__AVX2__)
	return reinterpret_cast<const T*>(__memchr<simd_type::AVX>(__s, val, n));
#else
	return reinterpret_cast<const T*>(__memchr<simd_type::SSE>(__s, val, n));
#endif 
}
template<typename T, std::enable_if_t<__has_simd_memchr_v<T>, int> = 0>
const T* memrchr(const T* s, T val, size_t n) {
	using value_type = std::make_unsigned_t<get_memory_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
#if defined(__AVX2__)
	return reinterpret_cast<const T*>(__memrchr<simd_type::AVX>(__s, val, n));
#else
	return reinterpret_cast<const T*>(__memrchr<simd_type::SSE>(__s, val, n));
#endif 
}
#else
template<typename T>
struct __has_simd_memchr : std::conjunction <
	wjr::is_memory_compare<T, T>,
	wjr::is_any_index_of<sizeof(T), sizeof(char), sizeof(wchar_t)>
> {};

template<typename T>
constexpr bool __has_simd_memchr_v = __has_simd_memchr<T>::value;

template<typename T>
struct __has_simd_memrchr : __has_simd_memchr<T> {};

template<typename T>
constexpr bool __has_simd_memrchr_v = __has_simd_memrchr<T>::value;

template<typename T, std::enable_if_t<__has_simd_memchr_v<T>, int> = 0>
const T* memchr(const T* s, T val, size_t n) {
	using value_type = std::make_unsigned_t<get_memory_t<T>>;
	if constexpr (sizeof(T) == sizeof(char)) {
		auto pos = ::memchr((const char*)s, (char)val, n);
		if (pos == nullptr) {
			return s + n;
		}
		return reinterpret_cast<const T*>(pos);
	}
	else {
		auto pos = ::wmemchr((const wchar_t*)s, (wchar_t)val, n);
		if (pos == nullptr) {
			return s + n;
		}
		return reinterpret_cast<const T*>(pos);
	}
}

#endif // __HAS_SIMD_MEMCHR

#if defined(__HAS_SIMD_MEMCMP)
template<typename T, typename U, typename _Pred>
struct __has_simd_memcmp : std::conjunction<
	std::is_same<get_memory_t<T>, get_memory_t<U>>,
	wjr::is_any_of<get_memory_t<T>, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t>,
	wjr::is_standard_comparator<_Pred>
> {};

template<typename T, typename U>
struct __has_simd_memcmp<T, U, std::equal_to<>> : std::conjunction<
	__has_simple_memory_equal<T, U>
> {};

template<typename T, typename U>
struct __has_simd_memcmp<T, U, std::not_equal_to<>> : std::conjunction<
	__has_simple_memory_equal<T, U>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_simd_memcmp_v = __has_simd_memcmp<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_simd_memrcmp : __has_simd_memcmp<T, U, _Pred> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_simd_memrcmp_v = __has_simd_memrcmp<T, U, _Pred>::value;

template<typename T, typename U, std::enable_if_t<__has_simd_memcmp_v<T, U, std::equal_to<>>, int> = 0>
bool memcmpeq(const T* s0, const U* s1, size_t n) {
	using value_type = uint8_t;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
#if defined(__AVX2__)
	return __memcmp<simd_type::AVX>(__s0, __s1, n * sizeof(T), std::equal_to<>{});
#else
	return __memcmp<simd_type::SSE>(__s0, __s1, n * sizeof(T), std::equal_to<>{});
#endif
}

template<typename T, typename U, std::enable_if_t<__has_simd_memcmp_v<T, U, std::less<>>, int> = 0>
bool memcmplt(const T* s0, const U* s1, size_t n) {
	using value_type = get_memory_t<T>;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
#if defined(__AVX2__)
	return __memcmp<simd_type::AVX>(__s0, __s1, n, std::less<>{});
#else
	return __memcmp<simd_type::SSE>(__s0, __s1, n, std::less<>{});
#endif
}

template<typename T, typename U, std::enable_if_t<__has_simd_memcmp_v<T, U, std::less_equal<>>, int> = 0>
bool memcmple(const T* s0, const U* s1, size_t n) {
	using value_type = get_memory_t<T>;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
#if defined(__AVX2__)
	return __memcmp<simd_type::AVX>(__s0, __s1, n, std::less_equal<>{});
#else
	return __memcmp<simd_type::SSE>(__s0, __s1, n, std::less_equal<>{});
#endif
}

template<typename T, typename U, std::enable_if_t<__has_simd_memcmp_v<T, U, std::not_equal_to<>>, int> = 0>
bool memcmpne(const T* s0, const U* s1, size_t n) {
	using value_type = uint8_t;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
#if defined(__AVX2__)
	return __memcmp<simd_type::AVX>(__s0, __s1, n * sizeof(T), std::not_equal_to<>{});
#else
	return __memcmp<simd_type::SSE>(__s0, __s1, n * sizeof(T), std::not_equal_to<>{});
#endif
}

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_simd_memcmp_v<T, U, _Pred>, int> = 0>
bool memcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
		return memcmpeq(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::not_equal_to<>>) {
		return memcmpne(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::less<>>) {
		return memcmplt(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::less_equal<>>) {
		return memcmple(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::greater<>>) {
		return memcmplt(s1, s0, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::greater_equal<>>) {
		return memcmple(s1, s0, n);
	}
}

#else
template<typename T, typename U, typename _Pred>
struct __has_simd_memcmp : std::false_type {};

template<typename T, typename U>
struct __has_simd_memcmp<T, U, std::equal_to<>> : std::conjunction<
	__has_simple_memory_equal<T, U>
> {};

template<typename T, typename U, typename _Pred>
inline constexpr bool __has_simd_memcmp_v = __has_simd_memcmp<T, U, _Pred>::type::value;

template<typename T, typename U, typename _Pred>
struct __has_simd_memrcmp : __has_simd_memcmp<T, U, _Pred> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_simd_memrcmp_v = __has_simd_memrcmp<T, U, _Pred>::type::value;

template<typename T, typename U, typename _Pred, std::enable_if_t<__has_simd_memcmp_v<T, U, _Pred>, int> = 0>
bool memcmp(const T* s0, const U* s1, size_t n, _Pred pred) {
	return ::memcmp(s0, s1, n * sizeof(T)) == 0;
}

#endif // __HAS_SIMD_MEMCMP

#if defined(__HAS_SIMD_MEMMIS)
template<typename T, typename U, typename _Pred>
struct __has_simd_memmis : std::conjunction<
	std::is_same<get_memory_t<T>, get_memory_t<U>>,
	wjr::is_any_of<get_memory_t<T>, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t>,
	wjr::is_standard_comparator<_Pred>
> {};

template<typename T, typename U>
struct __has_simd_memmis<T, U, std::equal_to<>> : std::conjunction<
	__has_simple_memory_equal<T, U>
> {};

template<typename T, typename U>
struct __has_simd_memmis<T, U, std::not_equal_to<>> : std::conjunction<
	__has_simple_memory_equal<T, U>
> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_simd_memmis_v = __has_simd_memmis<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_simd_memrmis : __has_simd_memmis<T, U, _Pred> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_simd_memrmis_v = __has_simd_memrmis<T, U, _Pred>::value;

template<typename T, typename U, std::enable_if_t<__has_simd_memmis_v<T, U, std::equal_to<>>, int> = 0>
const T* memmiseq(const T* s0, const U* s1, size_t n) {
	using value_type = uint8_t;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
	auto __pos = 
#if defined(__AVX2__)
	__memmis<simd_type::AVX>(__s0, __s1, n * sizeof(T), std::equal_to<>{});
#else
	__memmis<simd_type::SSE>(__s0, __s1, n * sizeof(T), std::equal_to<>{});
#endif
	auto __dif = (__pos - __s0) / sizeof(T);
	return s0 + __dif;
}

template<typename T, typename U, std::enable_if_t<__has_simd_memmis_v<T, U, std::less<>>, int> = 0>
const T* memmislt(const T* s0, const U* s1, size_t n) {
	using value_type = get_memory_t<T>;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
#if defined(__AVX2__)
	return reinterpret_cast<const T*>(__memmis<simd_type::AVX>(__s0, __s1, n, std::less<>{}));
#else
	return reinterpret_cast<const T*>(__memmis<simd_type::SSE>(__s0, __s1, n, std::less<>{}));
#endif
}

template<typename T, typename U, std::enable_if_t<__has_simd_memmis_v<T, U, std::less_equal<>>, int> = 0>
const T* memmisle(const T* s0, const U* s1, size_t n) {
	using value_type = get_memory_t<T>;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
#if defined(__AVX2__)
	return reinterpret_cast<const T*>(__memmis<simd_type::AVX>(__s0, __s1, n, std::less_equal<>{}));
#else
	return reinterpret_cast<const T*>(__memmis<simd_type::SSE>(__s0, __s1, n, std::less_equal<>{}));
#endif
}

template<typename T, typename U, std::enable_if_t<__has_simd_memmis_v<T, U, std::not_equal_to<>>, int> = 0>
const T* memmisne(const T* s0, const U* s1, size_t n) {
	using value_type = uint8_t;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
	auto __pos =
#if defined(__AVX2__)
		__memmis<simd_type::AVX>(__s0, __s1, n * sizeof(T), std::not_equal_to<>{});
#else
		__memmis<simd_type::SSE>(__s0, __s1, n * sizeof(T), std::not_equal_to<>{});
#endif
	auto __dif = (__pos - __s0) / sizeof(T);
	return s0 + __dif;
}

template<typename T, typename U, typename _Pred, std::enable_if_t<
	__has_simd_memmis_v<T, U, _Pred>, int> = 0>
const T* memmis(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
		return memmiseq(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::not_equal_to<>>) {
		return memmisne(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::less<>>) {
		return memmislt(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::less_equal<>>) {
		return memmisle(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::greater<>>) {
		return s0 + (memmislt(s1, s0, n) - s1);
	}
	else if constexpr (std::is_same_v<_Pred, std::greater_equal<>>) {
		return s0 + (memmisle(s1, s0, n) - s1);
	}
}

template<typename T, typename U, std::enable_if_t<__has_simd_memmis_v<T, U, std::equal_to<>>, int> = 0>
const T* memrmiseq(const T* s0, const U* s1, size_t n) {
	using value_type = uint8_t;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
	auto __pos = 
#if defined(__AVX2__)
	__memrmis<simd_type::AVX>(__s0, __s1, n * sizeof(T), std::equal_to<>{});
#else
	__memrmis<simd_type::SSE>(__s0, __s1, n * sizeof(T), std::equal_to<>{});
#endif
	auto __e0 = __s0 + n * sizeof(T);
	auto __dif = (__e0 - __pos) / sizeof(T);
	return (s0 + n) - __dif;
}

template<typename T, typename U, std::enable_if_t<__has_simd_memmis_v<T, U, std::less<>>, int> = 0>
const T* memrmislt(const T* s0, const U* s1, size_t n) {
	using value_type = get_memory_t<T>;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
#if defined(__AVX2__)
	return reinterpret_cast<const T*>(__memrmis<simd_type::AVX>(__s0, __s1, n, std::less<>{}));
#else
	return reinterpret_cast<const T*>(__memrmis<simd_type::SSE>(__s0, __s1, n, std::less<>{}));
#endif
}

template<typename T, typename U, std::enable_if_t<__has_simd_memmis_v<T, U, std::less_equal<>>, int> = 0>
const T* memrmisle(const T* s0, const U* s1, size_t n) {
	using value_type = get_memory_t<T>;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
#if defined(__AVX2__)
	return reinterpret_cast<const T*>(__memrmis<simd_type::AVX>(__s0, __s1, n, std::less_equal<>{}));
#else
	return reinterpret_cast<const T*>(__memrmis<simd_type::SSE>(__s0, __s1, n, std::less_equal<>{}));
#endif
}

template<typename T, typename U, std::enable_if_t<__has_simd_memmis_v<T, U, std::not_equal_to<>>, int> = 0>
const T* memrmisne(const T* s0, const U* s1, size_t n) {
	using value_type = uint8_t;
	auto __s0 = reinterpret_cast<const value_type*>(s0);
	auto __s1 = reinterpret_cast<const value_type*>(s1);
	auto __pos =
#if defined(__AVX2__)
		__memrmis<simd_type::AVX>(__s0, __s1, n * sizeof(T), std::not_equal_to<>{});
#else
		__memrmis<simd_type::SSE>(__s0, __s1, n * sizeof(T), std::not_equal_to<>{});
#endif
	auto __e0 = __s0 + n * sizeof(T);
	auto __dif = (__e0 - __pos) / sizeof(T);
	return (s0 + n) - __dif;
}

template<typename T, typename U, typename _Pred, std::enable_if_t<
	__has_simd_memmis_v<T, U, _Pred>, int> = 0>
const T* memrmis(const T* s0, const U* s1, size_t n, _Pred pred) {
	if constexpr (std::is_same_v<_Pred, std::equal_to<>>) {
		return memrmiseq(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::not_equal_to<>>) {
		return memrmisne(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::less<>>) {
		return memrmislt(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::less_equal<>>) {
		return memrmisle(s0, s1, n);
	}
	else if constexpr (std::is_same_v<_Pred, std::greater<>>) {
		return s0 + (memrmislt(s1, s0, n) - s1);
	}
	else if constexpr (std::is_same_v<_Pred, std::greater_equal<>>) {
		return s0 + (memrmisle(s1, s0, n) - s1);
	}
}

#else
template<typename T, typename U, typename _Pred>
struct __has_simd_memmis : std::false_type{};

template<typename T, typename U, typename _Pred>
constexpr bool __has_simd_memmis_v = __has_simd_memmis<T, U, _Pred>::value;

template<typename T, typename U, typename _Pred>
struct __has_simd_memrmis : __has_simd_memmis<T, U, _Pred> {};

template<typename T, typename U, typename _Pred>
constexpr bool __has_simd_memrmis_v = __has_simd_memrmis<T, U, _Pred>::value;

#endif // __HAS_SIMD_MEMMIS

#if defined(__HAS_SIMD_MEMCNT)
template<typename T>
struct __has_simd_memcnt : std::conjunction<
	wjr::is_memory_compare<T, T>,
	wjr::is_any_of<std::make_unsigned_t<get_memory_t<T>>, uint8_t, uint16_t, uint32_t>
> {};

template<typename T>
constexpr bool __has_simd_memcnt_v = __has_simd_memcnt<T>::value;

template<typename T, std::enable_if_t<__has_simd_memcnt_v<T>, int> = 0>
size_t memcnt(const T* s, T val, size_t n) {
	using value_type = std::make_unsigned_t<get_memory_t<T>>;
	auto __s = reinterpret_cast<const value_type*>(s);
#if defined(__AVX2__)
	return __memcnt<simd_type::AVX>(__s, static_cast<value_type>(val), n);
#else
	return __memcnt<simd_type::SSE>(__s, static_cast<value_type>(val), n);
#endif
}

#else
template<typename T>
struct __has_simd_memcnt : std::false_type {};

template<typename T>
constexpr bool __has_simd_memcnt_v = __has_simd_memcnt<T>::value;

#endif // __HAS_SIMD_MEMCNT

_WJR_SIMD_END

#endif // __WJR_SIMD_SIMD_H