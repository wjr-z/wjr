#pragma once
#ifndef __WJR_MATH_H__
#define __WJR_MATH_H__

#include <wjr/asm/asm.h>

_WJR_BEGIN

#ifdef __cpp_lib_bit_cast
using std::bit_cast;
#else
template <
	typename To,
	typename From,
	std::enable_if_t<
	sizeof(From) == sizeof(To) && std::is_trivially_copyable<To>::value&&
	std::is_trivially_copyable<From>::value,
	int> = 0>
WJR_INTRINSIC_CONSTEXPR20 To bit_cast(const From & src) noexcept {
	static_assert(std::is_trivially_constructible_v<To>,
		"This implementation additionally requires destination type to be trivially constructible");
	To storage;
	if constexpr (is_any_index_of_v<sizeof(From), 1, 2, 4, 8>) {
		using value_type = uint_t<8 * sizeof(From)>;
		*reinterpret_cast<value_type*>(&storage) = *reinterpret_cast<const value_type*>(&src);
	}
	else {
		std::memcpy(&storage, &src, sizeof(From));
	}
	return storage;
}

#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
constexpr bool has_single_bit(T x) noexcept {
	return x != 0 && (x & (x - 1)) == 0;
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int countl_zero(T x) noexcept {
	return wjr::masm::clz(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int countl_one(T x) noexcept {
	return wjr::countl_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int countr_zero(T x) noexcept {
	return wjr::masm::ctz(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int countr_one(T x) noexcept {
	return wjr::countr_zero(static_cast<T>(~x));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int bit_width(T x) noexcept {
	return std::numeric_limits<T>::digits - wjr::countl_zero(x);
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T bit_floor(T x) noexcept {
	if (x != 0) {
		return static_cast<T>(T{ 1 } << (wjr::bit_width(x) - 1));
	}
	return T{ 0 };
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T bit_ceil(T x) noexcept {
	if (x <= 1) {
		return T{ 1 };
	}
	return static_cast<T>(T{ 1 } << wjr::bit_width(x - 1));
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int popcount(T x) noexcept {
	return wjr::masm::popcnt(x);
}

template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T byteswap(T x) noexcept {
	using value_type = uint_t<8 * sizeof(T)>;
	return bit_cast<T>(wjr::masm::bswap(bit_cast<value_type>(x)));
}

template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T from_big_endian(T x) noexcept {
	if (is_little_endian()) {
		return byteswap(x);
	}
	return x;
}

template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T to_big_endian(T x) noexcept {
	return from_big_endian(x);
}

template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T from_little_endian(T x) noexcept {
	if (is_little_endian()) {
		return x;
	}
	return byteswap(x);
}

template<typename T, std::enable_if_t<is_standard_numer_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T to_little_endian(T x) noexcept {
	return to_little_endian(x);
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
struct broadcast_fn;

template<typename R, typename T>
constexpr broadcast_fn<R, T> broadcast{};

template<>
struct broadcast_fn<uint8_t, uint8_t> {
	WJR_INTRINSIC_CONSTEXPR uint8_t operator()(uint8_t x)const {
		return x;
	}
};
template<>
struct broadcast_fn<uint16_t, uint16_t> {
	WJR_INTRINSIC_CONSTEXPR uint16_t operator()(uint16_t x)const {
		return x;
	}
};
template<>
struct broadcast_fn<uint32_t, uint32_t> {
	WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint32_t x)const {
		return x;
	}
};
template<>
struct broadcast_fn<uint64_t, uint64_t> {
	WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint64_t x)const {
		return x;
	}
};

template<>
struct broadcast_fn<uint16_t, uint8_t> {
	WJR_INTRINSIC_CONSTEXPR uint16_t operator()(uint8_t x)const {
		return x | ((uint16_t)x << 8);
	}
};

template<>
struct broadcast_fn<uint32_t, uint16_t> {
	WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint16_t x)const {
		return x | ((uint32_t)x << 16);
	}
};
template<>
struct broadcast_fn<uint64_t, uint32_t> {
	WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint32_t x)const {
		return x | ((uint64_t)x << 32);
	}
};

template<>
struct broadcast_fn<uint32_t, uint8_t> {
	WJR_INTRINSIC_CONSTEXPR uint32_t operator()(uint8_t x)const {
		return x * (uint32_t)0x01010101;
	}
};
template<>
struct broadcast_fn<uint64_t, uint16_t> {
	WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint16_t x)const {
		return x * (uint64_t)0x0001000100010001;
	}
};

template<>
struct broadcast_fn<uint64_t, uint8_t> {
	WJR_INTRINSIC_CONSTEXPR uint64_t operator()(uint8_t x)const {
		return x * (uint64_t)0x0101010101010101;
	}
};

_WJR_END

#endif // __WJR_MATH_H__