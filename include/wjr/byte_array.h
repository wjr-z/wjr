#pragma once
#include <array>
#include <wjr/type_traits.h>

_WJR_BEGIN

template<size_t N>
using byte_array = std::array<uint8_t, N>;

template<typename T>
constexpr byte_array<sizeof(T)> to_byte_array(const T& _Val) {
	auto ptr = reinterpret_cast<const uint8_t*>(&_Val);
	byte_array<sizeof(T)> ret;
	for(size_t i = 0;i < sizeof(T);++i) {
		ret[i] = ptr[i];
	}
	return ret;
}

template<size_t M, size_t N, std::enable_if_t<is_any_index_of_v<M, 1, 2, 4, 8>, int> = 0>
constexpr uint_t<M * 8> to_uint(const byte_array<N>& _Val) {
	return *reinterpret_cast<const uint_t<M * 8>*>(_Val.data());
}

_WJR_END