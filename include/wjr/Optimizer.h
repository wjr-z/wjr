#pragma once
#include <wjr/byte_array.h>

_WJR_BEGIN

template<size_t N>
constexpr size_t _Get_max_bytes_num() {
	if constexpr (N % 8 == 0)return 8;
	else if constexpr (N % 4 == 0)return 4;
	else if constexpr (N % 2 == 0)return 2;
	else return 1;
}

template<size_t N>
size_t _Get_bytes_num(const byte_array<N>& a) {
	constexpr size_t M = _Get_max_bytes_num<N>();
	auto ptr = a.data();
	if constexpr (N > 8) {
		if (!is_constant_p(a)) {
			return 0;
		}
		else {
			for (size_t i = 0; i < N - M; ++i) {
				if (ptr[i] != ptr[i + M]) {
					return 0;
				}
			}
			for (size_t i = 0; i < M - 1; ++i) {
				if (ptr[i] != ptr[i + 1]) {
					return M;
				}
			}
			return 1;
		}
	}
	if constexpr (M == 1) {
		return 1;
	}
	else if constexpr (M == 2) {
		return (ptr[0] == ptr[1]) ? 1 : 2;
	}
	else if constexpr (M == 3) {
		return (*reinterpret_cast<const uint16_t*>(ptr) == *reinterpret_cast<const uint16_t*>(ptr + 1)) ? 1 : 0;
	}
	else if constexpr (M == 4) {
		auto x = *reinterpret_cast<const uint32_t*>(ptr);
		return ((x >> 8) == (x & 0x00FFFFFF)) ? 1 : 4;
	}
	else if constexpr (M == 5) {
		return (*reinterpret_cast<const uint32_t*>(ptr) == *reinterpret_cast<const uint32_t*>(ptr + 1)) ? 1 : 0;
	}else if constexpr(M == 6){
		return (*reinterpret_cast<const uint32_t*>(ptr) == *reinterpret_cast<const uint32_t*>(ptr + 2)) ? 2 : 6;
	}
	else if constexpr (M == 7) {
		auto x = *reinterpret_cast<const uint32_t*>(ptr);
		auto y = *reinterpret_cast<const uint32_t*>(ptr + 4);
		return ((x >> 8) == (x & 0x00FFFFFF)) && ((y >> 8) == (y & 0x00FFFFFF)) ? 1 : 0;
	}
	else {
		auto x = *reinterpret_cast<const uint64_t*>(ptr);
		return ((x >> 8) == (x & 0x00FFFFFFFFFFFFFF)) ? 1 : 8;
	}
}

template<typename T, typename U,
	bool =
	std::is_same_v<bool, std::remove_reference_t<U>> >= std::is_same_v<bool, T> &&
	((std::is_integral_v<T> && std::is_integral_v<std::remove_reference_t<U>>) ||
		(std::is_floating_point_v<T> && std::is_floating_point_v<std::remove_reference_t<U>>)) &&
	!std::is_volatile_v<T> && 
	!std::is_volatile_v<std::remove_reference_t<U>>
>
struct __is_byte_constructible {
	constexpr static bool is_copy = sizeof(T) == sizeof(U);
	constexpr static bool is_fill = true;
	constexpr static byte_array<sizeof(T)> get(const U& src) {
		return to_byte_array(static_cast<T>(src));
	}
};

template<typename T, bool = 
	std::is_trivially_copyable_v<T> &&
	!std::is_volatile_v<T>>
struct __is_byte_copy_constructible_helper {
	constexpr static bool is_copy = true;
	constexpr static bool is_fill = !std::is_empty_v<T>;
	constexpr static byte_array<sizeof(T)> get(const T& src) {
		return to_byte_array(src);
	}
};

template<typename T>
struct __is_byte_copy_constructible_helper<T, false> {
	constexpr static bool is_copy = false;
	constexpr static bool is_fill = false;
};

template<typename T, bool =
	std::is_trivially_copyable_v<T> &&
	!std::is_volatile_v<T>>
struct __is_byte_move_constructible_helper {
	constexpr static bool is_copy = true;
	constexpr static bool is_fill = !std::is_empty_v<T>;
	constexpr static byte_array<sizeof(T)> get(const T& src) {
		return to_byte_array(src);
	}
};

template<typename T>
struct __is_byte_move_constructible_helper<T, false> {
	constexpr static bool is_copy = false;
	constexpr static bool is_fill = false;
};

template<typename T, typename U>
struct __is_byte_constructible<T, U, false> {
	constexpr static bool is_copy = false;
	constexpr static bool is_fill = false;
};

template<typename T>
struct __is_byte_constructible<T, T, false> : __is_byte_copy_constructible_helper<T> {};

template<typename T>
struct __is_byte_constructible<T, const T, false> : __is_byte_copy_constructible_helper<T> {};

template<typename T>
struct __is_byte_constructible<T, T&, false> : __is_byte_copy_constructible_helper<T> {};

template<typename T>
struct __is_byte_constructible<T, const T&, false> : __is_byte_copy_constructible_helper<T> {};

template<typename T>
struct __is_byte_constructible<T, T&&, false> : __is_byte_move_constructible_helper<T> {};

template<typename T, typename U>
struct is_byte_constructible : __is_byte_constructible<std::remove_reference_t<T>, U> {};

template<typename T, typename U>
struct is_byte_assignable : __is_byte_constructible<std::remove_reference_t<T>, U> {};

_WJR_END