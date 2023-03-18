#pragma once
#ifndef __WJR_OPTIMIZER_H
#define __WJR_OPTIMIZER_H

// 1. fill dst with val (memset)
// The minimum cycle section length must be 1, 2, 4, 8 when _WJR_FAST_MEMSET defined
// and must be 1 when NWJR_FAST_MEMSET defined
// Get the bytes to be filled through the get function
// 2. copy src to dst (memcpy)

// TODO 
// 1. fill a constant value when default-construct (memset)
// 2. fill a constant value when value-cosntruct (memset)
// 3. relocate , copy src to dst, and then fill dst with a constant value (memcpy and memset)
// 4. reloacte , copy src to dst, and then run a function on dst (memcpy and run func(User defined))

#include <wjr/type_traits.h>

_WJR_BEGIN

template<size_t N>
constexpr size_t _Get_max_bytes_num() {
#if defined(WJR_X86_64)
	if constexpr (N % 8 == 0)return 8;
	else 
#endif
	if constexpr (N % 4 == 0)return 4;
	else if constexpr (N % 2 == 0)return 2;
	else return 1;
}

template<size_t C, typename _Ty>
size_t _Get_bytes_num(const _Ty& val) {
	constexpr size_t N = sizeof(_Ty);
	constexpr size_t M = _Get_max_bytes_num<C>();
	const auto ptr = reinterpret_cast<const uint8_t*>(&val);
	if constexpr (N == C) {
		if constexpr (N == 1) {
			return 1;
		}
		else if constexpr (N == 2) {
			return (ptr[0] == ptr[1]) ? 1 : 2;
		}
		else if constexpr (N == 3) {
			return (*reinterpret_cast<const uint16_t*>(ptr) == *reinterpret_cast<const uint16_t*>(ptr + 1)) ? 1 : 0;
		}
		else if constexpr (N == 4) {
			auto x = *reinterpret_cast<const uint32_t*>(ptr);
			return ((x >> 8) == (x & 0x00FFFFFF)) ? 1 : 4;
		}
		else if constexpr (N == 5) {
			return (*reinterpret_cast<const uint32_t*>(ptr) == *reinterpret_cast<const uint32_t*>(ptr + 1)) ? 1 : 0;
		}
		else if constexpr (N == 6) {
			return (*reinterpret_cast<const uint32_t*>(ptr) == *reinterpret_cast<const uint32_t*>(ptr + 2)) ? 2 : 6;
		}
		else if constexpr (N == 7) {
			auto x = *reinterpret_cast<const uint32_t*>(ptr);
			auto y = *reinterpret_cast<const uint32_t*>(ptr + 4);
			return ((x >> 8) == (x & 0x00FFFFFF)) && ((y >> 8) == (y & 0x00FFFFFF)) ? 1 : 0;
		}
		else if constexpr (N == 8) {
			auto x = *reinterpret_cast<const uint64_t*>(ptr);
			return ((x >> 8) == (x & 0x00FFFFFFFFFFFFFF)) ? 1 : 8;
		}
		else {
			if (!is_constant_p(val)) {
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
	}
	else {
		// all zeros
		if constexpr (N <= 8) {
			constexpr uint8_t zero[8] = { 0,0,0,0,0,0,0,0 };
			return ::memcmp(ptr, zero, N) == 0 ? 1 : 0;
		}
		else {
			if (!is_constant_p(val)) {
				return 0;
			}
			else {
				for (size_t i = 0; i < N; ++i) {
					if (ptr[i] != 0) {
						return 0;
					}
				}
				return 1;
			}
		}
	}
}

template<typename T, typename U,
	bool =
	std::is_same_v<bool, std::remove_reference_t<U>> >= std::is_same_v<bool, T> &&
	is_arithmetics_v<T, std::remove_reference_t<U>> &&
	!std::is_volatile_v<T> && 
	!std::is_volatile_v<std::remove_reference_t<U>>
>
struct __is_byte_constructible {
	constexpr static bool is_copy = sizeof(T) == sizeof(U);
	constexpr static bool is_fill = true;
	constexpr static T get(const U& val) {
		return static_cast<T>(val);
	}
};

template<typename T, bool = 
	std::is_trivially_copyable_v<T> &&
	!std::is_volatile_v<T>>
struct __is_byte_copy_constructible_helper {
	constexpr static bool is_copy = true;
	constexpr static bool is_fill = !std::is_empty_v<T>;
	constexpr static const T& get(const T& val){
		return val;
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
	constexpr static const T& get(const T& val) {
		return val;
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

#endif // __WJR_OPTIMIZER_H