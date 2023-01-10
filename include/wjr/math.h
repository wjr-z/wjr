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
constexpr int __wjr_fallback_clz(T x) noexcept {
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
#define __WJR_HAS_BUILTIN_CLZ
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int __wjr_builtin_clz(T x) noexcept {
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
#define __WJR_HAS_MSVC_X86_64_CLZ
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int __wjr_msvc_x86_64_clz(T x) noexcept {
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
#define __WJR_HAS_MSVC_ARM_CLZ
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int __wjr_msvc_arm_clz(T x) noexcept {
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

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR int countl_zero(T x) noexcept {

WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(__WJR_HAS_BUILTIN_CLZ)
	return __wjr_builtin_clz(x);
#elif defined(__WJR_HAS_MSVC_X86_64_CLZ)
	return __wjr_msvc_x86_64_clz(x);
#elif defined(__WJR_HAS_MSVC_ARM_CLZ)
	return __wjr_msvc_arm_clz(x);
#endif
WJR_IS_NOT_CONSTANT_EVALUATED_END
	return __wjr_fallback_clz(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR int countl_one(T x) noexcept {
	return countl_zero(static_cast<T>(~x));
}

#if defined(__GUNC__) || defined(__clang__) || WJR_HAS_BUILTIN(__builtin_ctz)
#define __WJR_HAS_BUILTIN_CTZ
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int __wjr_builtin_ctz(T x) noexcept {
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
#define __WJR_HAS_MSVC_CTZ
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int __wjr_msvc_ctz(T x) noexcept {
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

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR int countr_zero(T x) noexcept {

	constexpr auto _Nd = std::numeric_limits<T>::digits;
WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN

#if defined(__WJR_HAS_BUILTIN_CTZ)
	return __wjr_builtin_ctz(x);
#elif defined(__WJR_HAS_MSVC_CTZ)
	return __wjr_msvc_ctz(x);
#endif

WJR_IS_NOT_CONSTANT_EVALUATED_END

	return _Nd - __wjr_fallback_clz(static_cast<T>(static_cast<T>(~x) & static_cast<T>(x - 1)));
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

template<typename _Ty, std::enable_if_t<wjr::is_unsigned_integral_v<_Ty>, int> = 0>
WJR_CONSTEXPR int __wjr_fallback_popcount(_Ty _Val) noexcept {
	constexpr int _Digits = std::numeric_limits<_Ty>::digits;
#if defined(_M_IX86) || defined(_M_ARM)
	if constexpr (_Digits == 64) {
		// 64-bit bit operations on architectures without 64-bit registers are less efficient,
		// hence we split the value so that it fits in 32-bit registers
		return _Popcount_fallback(static_cast<unsigned long>(_Val))
			+ _Popcount_fallback(static_cast<unsigned long>(_Val >> 32));
	}
#endif // defined(_M_IX86) || defined(_M_ARM)
	// we static_cast these bit patterns in order to truncate them to the correct size
	_Val = static_cast<_Ty>(_Val - ((_Val >> 1) & static_cast<_Ty>(0x5555'5555'5555'5555ull)));
	_Val = static_cast<_Ty>((_Val & static_cast<_Ty>(0x3333'3333'3333'3333ull))
		+ ((_Val >> 2) & static_cast<_Ty>(0x3333'3333'3333'3333ull)));
	_Val = static_cast<_Ty>((_Val + (_Val >> 4)) & static_cast<_Ty>(0x0F0F'0F0F'0F0F'0F0Full));
	// Multiply by one in each byte, so that it will have the sum of all source bytes in the highest byte
	_Val = static_cast<_Ty>(_Val * static_cast<_Ty>(0x0101'0101'0101'0101ull));
	// Extract highest byte
	return static_cast<int>(_Val >> (_Digits - 8));
}

#if defined(__GUNC__) || defined(__clang__) || WJR_HAS_BUILTIN(__builtin_popcount)
#define __WJR_HAS_BUILTIN_POPCOUNT
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
int __wjr_builtin_popcount(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;

	if constexpr (_Nd <= _Nd_ui) {
		return __builtin_popcount(static_cast<unsigned>(x));
	}
	else if constexpr (_Nd <= _Nd_ul) {
		return __builtin_popcountl(static_cast<unsigned long>(x));
	}
	else if constexpr (_Nd <= _Nd_ull) {
		return __builtin_popcountll(static_cast<unsigned long long>(x));
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}

}
#endif 

#if (defined(_M_IX86) || (defined(_M_X64) && !defined(_M_ARM64EC))) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
    && !defined(__INTEL_COMPILER)
#define __WJR_HAS_POPCNT_INTRINSICS 1
#else // ^^^ intrinsics available ^^^ / vvv intrinsics unavailable vvv
#define __WJR_HAS_POPCNT_INTRINSICS 0
#endif // ^^^ intrinsics unavailable ^^^

#if defined(_MSC_VER)
#if __WJR_HAS_POPCNT_INTRINSICS
#define __WJR_HAS_MSVC_X86_64_popcount
template <typename T>
int __wjr_msvc_x86_64_popcount(T x) noexcept {
	constexpr int _Digits = std::numeric_limits<T>::digits;
	if constexpr (_Digits <= 16) {
		return static_cast<int>(__popcnt16(x));
	}
	else if constexpr (_Digits == 32) {
		return static_cast<int>(__popcnt(x));
	}
	else {
#ifdef _M_IX86
		return static_cast<int>(__popcnt(x >> 32) + __popcnt(static_cast<unsigned int>(x)));
#else // ^^^ _M_IX86 / !_M_IX86 vvv
		return static_cast<int>(__popcnt64(x));
#endif // _M_IX86
	}
}
#endif // __WJR_HAS_POPCNT_INTRINSICS
#endif // _MSC_VER

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR int popcount(T x) noexcept {
WJR_IS_NOT_CONSTANT_EVALUATED_BEGIN
#if defined(__WJR_HAS_BUILTIN_POPCOUNT)
	return __wjr_builtin_popcount(x);
#elif defined(__WJR_HAS_MSVC_X86_64_popcount)
	return __wjr_msvc_x86_64_popcount(x);
#endif 
WJR_IS_NOT_CONSTANT_EVALUATED_END
	return __wjr_fallback_popcount(x);
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

template<typename R, typename T>
constexpr bool __maybe_equal(T t) noexcept {
	using type = arithmetic_conversion_t<R, T>;
	if constexpr (std::is_signed_v<R> && std::is_unsigned_v<type>) {
		if constexpr (static_cast<R>(-1) == std::numeric_limits<type>::max()) {
			return (std::numeric_limits<R>::min() <= t) || (t <= std::numeric_limits<R>::max());
		}
		else {
			return (std::numeric_limits<R>::min() <= t && t <= static_cast<R>(-1))
				|| (t <= std::numeric_limits<R>::max());
		}
	}
	else {
		return std::numeric_limits<R>::min() <= t &&
			t <= std::numeric_limits<R>::max();
	}
}

_WJR_END

#endif // __WJR_MATH_H__