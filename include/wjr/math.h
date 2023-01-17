#pragma once
#ifndef __WJR_MATH_H__
#define __WJR_MATH_H__

#include <wjr/asm.h>

_WJR_BEGIN

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
constexpr bool has_single_bit(T x) noexcept {
	return x != 0 && (x & (x - 1)) == 0;
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int countl_zero(T x) noexcept {
	return wjr::masm::clz(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int countl_one(T x) noexcept {
	return countl_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int countr_zero(T x) noexcept {
	return wjr::masm::ctz(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int countr_one(T x) noexcept {
	return countr_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int bit_width(T x) noexcept {
	return std::numeric_limits<T>::digits - wjr::countl_zero(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 T bit_floor(T x) noexcept {
	if (x != 0) {
		return static_cast<T>(T{ 1 } << (wjr::bit_width(x) - 1));
	}
	return T{ 0 };
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 T bit_ceil(T x) noexcept {
	if (x <= 1) {
		return T{ 1 };
	}
	return static_cast<T>(T{ 1 } << wjr::bit_width(x - 1));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_CONSTEXPR20 int popcount(T x) noexcept {
	return wjr::masm::popcnt(x);
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
	using type = typename wjr::common_arithmetic_t<R, T>;
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
		if constexpr (std::numeric_limits<R>::min() <= std::numeric_limits<T>::min()) {
			if constexpr (std::numeric_limits<T>::max() <= std::numeric_limits<R>::max()) {
				return true;
			}
			else {
				return t <= std::numeric_limits<R>::max();
			}
		}
		else {
			if constexpr (std::numeric_limits<T>::max() <= std::numeric_limits<R>::max()) {
				return std::numeric_limits<R>::min() <= t;
			}
			else {
				return std::numeric_limits<R>::min() <= t && t <= std::numeric_limits<R>::max();
			}
		}
	}
}

_WJR_END

#endif // __WJR_MATH_H__