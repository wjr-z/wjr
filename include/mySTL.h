#ifndef MY_STL_H
#define MY_STL_H

#include <cstdint>
#include <ctime>
#include <random>
#include <cassert>
#include <type_traits>
#include <locale>

namespace wjr {

#if defined(__BYTE_ORDER__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define wis_little_endian true
#else
#define wis_little_endian false
#endif
#else
#define wis_little_endian true
#endif

	inline namespace my_math {

		constexpr size_t qlog2(int);
		constexpr size_t qlog2(uint32_t);
		constexpr size_t qlog2(long long);
		constexpr size_t qlog2(uint64_t);

		constexpr unsigned int find_number(uint32_t n);
		constexpr unsigned int find_number(uint64_t n);

		constexpr unsigned int _minx(uint32_t n); // find the index of the lowest bit

		constexpr bool is_power_of_two(int);
		constexpr bool is_power_of_two(unsigned int);
		constexpr bool is_power_of_two(long long);
		constexpr bool is_power_of_two(unsigned long long);

	}

	inline namespace mt_type_traits {

		template<typename...>
		struct is_any_of {
			constexpr static bool value = false;
		};

		template<typename T, typename U>
		struct is_any_of<T, U> {
			constexpr static bool value = std::is_same_v<T, U>;
		};

		template<typename T, typename U, typename...args>
		struct is_any_of<T, U, args...> {
			constexpr static bool value = std::disjunction_v<is_any_of<T, U>, is_any_of<T, args...>>;
		};

		template<typename...args>
		constexpr static bool is_any_of_v = is_any_of<args...>::value;

		template<typename T>
		struct is_reverse_iterator : std::false_type {};

		template<typename T>
		struct is_reverse_iterator<std::reverse_iterator<T>> : std::true_type {};

	#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) && (_MSC_VER >= 1913))
		template<typename F,typename...Args>
		struct wjr_result_of {
			using type = std::invoke_result_t<F,Args...>;
		};
	#else
		template<typename F,typename ...Args>
		struct wjr_result_of {
			using type = std::result_of_t<F(Args...)>;
		};
	#endif
		template<typename F,typename...Args>
		using wjr_result_of_t = typename wjr_result_of<F,Args...>::type;
	}

	inline namespace wjr_hash {
	#if defined(_WIN64)
		constexpr static size_t _FNV_offset_basis = 14695981039346656037ULL;
		constexpr static size_t _FNV_prime = 1099511628211ULL;
	#else // defined(_WIN64)
		constexpr static size_t _FNV_offset_basis = 2166136261U;
		constexpr static size_t _FNV_prime = 16777619U;
	#endif // defined(_WIN64)

		constexpr size_t normal_fnv1a_append_bytes(size_t _Val, const unsigned char* const _First,
			const size_t _Count) noexcept { // accumulate range [_First, _First + _Count) into partial FNV-1a hash _Val
			for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
				_Val ^= static_cast<size_t>(_First[_Idx]);
				_Val *= _FNV_prime;
			}
			return _Val;
		}

		template<size_t byte_size, typename const_pointer>
		constexpr size_t constexpr_fnv1a_append_bytes(size_t _Val, const_pointer _First,
			const size_t _Count)noexcept {
			for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
				auto val = static_cast<size_t>(_First[_Idx]);
				if constexpr (wis_little_endian) {
					for (size_t i = 0; i < byte_size; ++i) {
						_Val ^= (val >> (i << 3)) & 0xFF;
						_Val *= _FNV_prime;
					}
				}
				else {
					for (size_t i = byte_size - 1; i > 0; ++i) {
						_Val ^= (val >> (i << 3)) & 0xFF;
						_Val *= _FNV_prime;
					}
				}
			}
			return _Val;
		}

		template<typename const_pointer>
		constexpr size_t fnv1a_append_bytes(size_t _Val, const_pointer _First,
			const size_t _Count)noexcept {
			using value_type = typename std::iterator_traits<const_pointer>::value_type;
			constexpr size_t byte_size = sizeof(value_type);
			if constexpr (byte_size >= 1 && byte_size <= sizeof(size_t)) {
				return constexpr_fnv1a_append_bytes<byte_size>(_Val, _First, _Count);
			}
			else {
				return normal_fnv1a_append_bytes(_Val,
					reinterpret_cast<const unsigned char*>(_First), byte_size * _Count);
			}
		}

		template <class _Ty>
		constexpr size_t fnv1a_append_range(const size_t _Val, const _Ty* const _First,
			const _Ty* const _Last) noexcept { // accumulate range [_First, _Last) into partial FNV-1a hash _Val
			static_assert(std::is_trivial_v<_Ty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(_Val, _First, static_cast<size_t>(_Last - _First));
		}

		template <class _Kty>
		constexpr size_t fnv1a_append_value(
			const size_t _Val, const _Kty& _Keyval) noexcept { // accumulate _Keyval into partial FNV-1a hash _Val
			static_assert(std::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(_Val, reinterpret_cast<const _Kty*>(&_Keyval), 1);
		}

		// FUNCTION TEMPLATE _Hash_representation
		template <class _Kty>
		constexpr size_t hash_representation(const _Kty& _Keyval) noexcept { // bitwise hashes the representation of a key
			return fnv1a_append_value(_FNV_offset_basis, _Keyval);
		}

		// FUNCTION TEMPLATE _Hash_array_representation
		template <class _Kty>
		constexpr size_t hash_array_representation(
			const _Kty* const _First, const size_t _Count) noexcept { // bitwise hashes the representation of an array
			static_assert(std::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(
				_FNV_offset_basis, _First, _Count);
		}
	}

}

#endif