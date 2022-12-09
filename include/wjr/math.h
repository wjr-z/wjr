#pragma once
#ifndef __WJR_MATH_H__
#define __WJR_MATH_H__

#include <limits>
#include <wjr/type_traits.h>

_WJR_BEGIN

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
constexpr bool has_single_bit(T x) noexcept {
	return x != 0 && (x & (x - 1)) == 0;
}

const static int _WJR_LOG_TABLE[256] = {
		0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
};

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
constexpr int _Countl_zero_fallback(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}

	int n = 0;
	// use _Table

	if constexpr (_Nd > 32) {
		if (x >> 32) {
			n += 32;
			x >>= 32;
		}
	}
	if constexpr (_Nd > 16) {
		if (x >> 16) {
			n += 16;
			x >>= 16;
		}
	}
	if constexpr (_Nd > 8) {
		if (x >> 8) {
			n += 8;
			x >>= 8;
		}
	}
	return  _Nd - (n + _WJR_LOG_TABLE[x]);
}

#if defined(__GUNC__) || defined(__clang__) || WJR_HAS_BUILTIN(__builtin_clz)
#define __WJR_HAS_BUILTIN_COUNTL_ZERO
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int __builtin_countl_zero(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;

	if constexpr (_Nd <= _Nd_ui) {
		constexpr auto __diff = _Nd_ui - _Nd;
		return __builtin_clz(x) - __diff;
	}
	else if constexpr (_Nd <= _Nd_ul) {
		constexpr auto __diff = _Nd_ul - _Nd;
		return __builtin_clzl(x) - __diff;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		constexpr auto __diff = _Nd_ull - _Nd;
		return __builtin_clzll(x) - __diff;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "countl_zero is not implemented for this type");
	}
}
#endif // __GUNC__ || __clang__ || WJR_HAS_BUILTIN(__builtin_clz)

#if defined(_MSC_VER)
#if defined(_M_IX86) || (defined(_M_X64) && !defined(_M_ARM64EC))
#define __WJR_HAS_MSC_X86_64_COUNTL_ZERO
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int _MSC_x86_64_countl_zero(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
#if defined(__AVX2__)
	if constexpr (_Nd <= 16) {
		return static_cast<int>(__lzcnt16(x) - (16 - _Nd));
	}
	else if constexpr (_Nd <= 32) {
		return static_cast<int>(__lzcnt(x));
	}
	else {
#if defined(_M_IX86)
		const auto xh = static_cast<unsigned int>(x >> 32);
		const auto xl = static_cast<unsigned int>(x);
		if (xh == 0) {
			return 32 + static_cast<int>(__lzcnt(xl));
		}
		else {
			return static_cast<int>(__lzcnt(xh));
		}
#else
		return static_cast<int>(__lzcnt64(x));
#endif
	}
#else
	unsigned long _Result;
	if constexpr (_Nd <= 32) {
		if (!_BitScanReverse(&_Result, x)) {
			return _Nd;
		}
	}
	else {
#if defined(_M_IX86)
		const auto xh = static_cast<unsigned int>(x >> 32);
		if (_BitScanReverse(&_Result, xh)) {
			return static_cast<int>(31 - _Result);
		}
		const auto xl = static_cast<unsigned int>(x);
		if (!_BitScanReverse(&_Result, xl)) {
			return _Nd;
		}
#else
		if (!_BitScanReverse64(&_Result, x)) {
			return _Nd;
		}
#endif
	}
	return static_cast<int>(_Nd - 1 - _Result);
#endif 
}
#elif defined(_M_ARM) || defined(_M_ARM64)
#define __WJR_HAS_MSC_ARM_COUNTL_ZERO
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int _MSC_ARM_countl_zero(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}
	if constexpr (_Nd <= 32) {
		return static_cast<int>(_CountLeadingZeros(x));
	}
	else {
		return static_cast<int>(_CountLeadingZeros64(x));
	}
}
#endif
#endif // _MSC_VER

#if defined(__WJR_HAS_BUILTIN_COUNTL_ZERO)		|| \
	defined(__WJR_HAS_MSC_X86_64_COUNTL_ZERO)	|| \
	defined(__WJR_HAS_MSC_ARM_COUNTL_ZERO)
#define __WJR_HAS_FAST_COUNTL_ZERO
#endif 

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR int countl_zero(T x) noexcept {

WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(__WJR_HAS_BUILTIN_COUNTL_ZERO)
	return __builtin_countl_zero(x);
#elif defined(__WJR_HAS_MSC_X86_64_COUNTL_ZERO)
	return _MSC_x86_64_countl_zero(x);
#elif defined(__WJR_HAS_MSC_ARM_COUNTL_ZERO)
	return _MSC_ARM_countl_zero(x);
#endif
WJR_IS_NOT_CONSTANT_EVALUATED_END
	return _Countl_zero_fallback(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR int countl_one(T x) noexcept {
	return countl_zero(static_cast<T>(~x));
}

#if defined(__GUNC__) || defined(__clang__) || WJR_HAS_BUILTIN(__builtin_ctz)
#define __WJR_HAS_BUILTIN_COUNTR_ZERO
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int __builtin_countr_zero(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;

	if constexpr (_Nd <= _Nd_ui) {
		return __builtin_ctz(x);
	}
	else if constexpr (_Nd <= _Nd_ul) {
		return __builtin_ctzl(x);
	}
	else if constexpr (_Nd <= _Nd_ull) {
		return __builtin_ctzll(x);
	}
	else {
		static_assert(_Nd <= _Nd_ull, "countr_zero is not implemented for this type");
	}
}
#endif

#if (defined(_M_IX86) || (defined(_M_X64) && !defined(_M_ARM64EC))) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
    && !defined(__INTEL_COMPILER)
#define __WJR_HAS_TZCNT_BSF_INTRINSICS 1
#endif

#if defined(_MSC_VER) && defined(__WJR_HAS_TZCNT_BSF_INTRINSICS)
#define __WJR_HAS_MSC_COUNTR_ZERO
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int _MSC_tzcnt_countr_zero(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	constexpr T _Max = std::numeric_limits<T>::max();
#if defined(__AVX2__)
	if constexpr (_Nd <= 32) {
		return static_cast<int>(_tzcnt_u32(static_cast<unsigned int>(~_Max | x)));
	}
	else {
#if defined(_M_IX86)
		const auto xh = static_cast<unsigned int>(x >> 32);
		const auto xl = static_cast<unsigned int>(x);
		if (xh == 0) {
			return 32 + static_cast<int>(_tzcnt_u32(xl));
		}
		else {
			return static_cast<int>(_tzcnt_u32(xh));
		}
#else
		return static_cast<int>(_tzcnt_u64(x));
#endif // _M_IX86
	}
#else
	unsigned long _Result;
	if constexpr (_Nd <= 32) {
		if (!_BitScanForward(&_Result, x)) {
			return _Nd;
		}
	}
	else {
#if defined(_M_IX86)
		const auto xl = static_cast<unsigned int>(x);
		if (_BitScanForward(&_Result, xl)) {
			return static_cast<int>(_Result);
		}
		const auto xh = static_cast<unsigned int>(x >> 32);
		if (!_BitScanForward(&_Result, xh)) {
			return _Nd;
		}
		else {
			return static_cast<int>(32 + _Result);
		}

#else
		if (!_BitScanForward64(&_Result, x)) {
			return _Nd;
		}
#endif
	}
	return static_cast<int>(_Result);
#endif
}
#endif

#if defined(__WJR_HAS_BUILTIN_COUNTR_ZERO)		|| \
	defined(__WJR_HAS_MSC_COUNTR_ZERO)
#define __WJR_HAS_FAST_COUNTR_ZERO
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR int countr_zero(T x) noexcept {

	constexpr auto _Nd = std::numeric_limits<T>::digits;
WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN

#if defined(__WJR_HAS_BUILTIN_COUNTR_ZERO)
	return __builtin_countr_zero(x);
#elif defined(__WJR_HAS_MSC_COUNTR_ZERO)
	return _MSC_tzcnt_countr_zero(x);
#endif

WJR_IS_NOT_CONSTANT_EVALUATED_END

	return _Nd - _Countl_zero_fallback(static_cast<T>(static_cast<T>(~x) & static_cast<T>(x - 1)));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR int countr_one(T x) noexcept {
	return countr_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR int bit_width(T x) noexcept {
	return std::numeric_limits<T>::digits - wjr::countl_zero(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR T bit_floor(T x) noexcept {
	if (x != 0) {
		return static_cast<T>(T{ 1 } << (wjr::bit_width(x) - 1));
	}
	return T{ 0 };
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR T bit_ceil(T x) noexcept {
	if (x <= 1) {
		return T{ 1 };
	}
	return static_cast<T>(T{ 1 } << wjr::bit_width(x - 1));
}

template< typename T, typename U >
constexpr bool cmp_equal(T t, U u) noexcept
{
	using UT = std::make_unsigned_t<T>;
	using UU = std::make_unsigned_t<U>;
	if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
		return t == u;
	else if constexpr (std::is_signed_v<T>)
		return t < 0 ? false : UT(t) == u;
	else
		return u < 0 ? false : t == UU(u);
}

template< typename T, typename U >
constexpr bool cmp_not_equal(T t, U u) noexcept {
	return !cmp_equal(t, u);
}

template< typename T, typename U >
constexpr bool cmp_less(T t, U u) noexcept {
	using UT = std::make_unsigned_t<T>;
	using UU = std::make_unsigned_t<U>;
	if constexpr (std::is_signed_v<T> == std::is_signed_v<U>)
		return t < u;
	else if constexpr (std::is_signed_v<T>)
		return t < 0 ? true : UT(t) < u;
	else
		return u < 0 ? false : t < UU(u);
}

template< typename T, typename U >
constexpr bool cmp_greater(T t, U u) noexcept {
	return wjr::cmp_less(u, t);
}

template< typename T, typename U >
constexpr bool cmp_less_equal(T t, U u) noexcept {
	return !wjr::cmp_greater(t, u);
}

template< typename T, typename U >
constexpr bool cmp_greater_equal(T t, U u) noexcept {
	return !wjr::cmp_less(t, u);
}

template< typename R, typename T>
constexpr bool in_range(T t) noexcept {
	return wjr::cmp_greater_equal(t, std::numeric_limits<R>::min()) &&
		wjr::cmp_less_equal(t, std::numeric_limits<R>::max());
}

_WJR_END

#endif // __WJR_MATH_H__