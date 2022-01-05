#ifndef MY_STL_H
#define MY_STL_H

#include <cstdint>
#include <ctime>
#include <random>
#include <cassert>
#include <type_traits>

namespace wjr {

	inline namespace my_math {

		size_t qlog2(int);
		size_t qlog2(uint32_t);
		size_t qlog2(long long);
		size_t qlog2(uint64_t);

		unsigned int find_number(uint32_t n);
		unsigned int find_number(uint64_t n);

		unsigned int _minx(uint32_t n); // find the index of the lowest bit

		bool is_power_of_two(int);
		bool is_power_of_two(unsigned int);
		bool is_power_of_two(long long);
		bool is_power_of_two(unsigned long long);

	}

	inline namespace mt_type_traits{

		template<typename...>
		struct is_any_of {
			constexpr static bool value = false;
		};

		template<typename T, typename U>
		struct is_any_of<T,U> {
			constexpr static bool value = std::is_same_v<T,U>;
		};

		template<typename T, typename U, typename...args>
		struct is_any_of<T,U,args...> {
			constexpr static bool value = std::disjunction_v<is_any_of<T,U>,is_any_of<T,args...>>;
		};

		template<typename...args>
		constexpr static bool is_any_of_v = is_any_of<args...>::value;

		template<typename T>
		struct is_reverse_iterator : std::false_type {};

		template<typename T>
		struct is_reverse_iterator<std::reverse_iterator<T>> : std::true_type {};

	}

	inline namespace _hash {
	#if defined(_WIN64)
		constexpr static size_t _FNV_offset_basis = 14695981039346656037ULL;
		constexpr static size_t _FNV_prime = 1099511628211ULL;
	#else // defined(_WIN64)
		constexpr static size_t _FNV_offset_basis = 2166136261U;
		constexpr static size_t _FNV_prime = 16777619U;
	#endif // defined(_WIN64)

		inline size_t fnv1a_append_bytes(size_t _Val, const unsigned char* const _First,
			const size_t _Count) noexcept { // accumulate range [_First, _First + _Count) into partial FNV-1a hash _Val
			for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
				_Val ^= static_cast<size_t>(_First[_Idx]);
				_Val *= _FNV_prime;
			}

			return _Val;
		}

		template <class _Ty>
		size_t fnv1a_append_range(const size_t _Val, const _Ty* const _First,
			const _Ty* const _Last) noexcept { // accumulate range [_First, _Last) into partial FNV-1a hash _Val
			static_assert(std::is_trivial_v<_Ty>, "Only trivial types can be directly hashed.");
			const auto _Firstb = reinterpret_cast<const unsigned char*>(_First);
			const auto _Lastb = reinterpret_cast<const unsigned char*>(_Last);
			return fnv1a_append_bytes(_Val, _Firstb, static_cast<size_t>(_Lastb - _Firstb));
		}

		template <class _Kty>
		size_t fnv1a_append_value(
			const size_t _Val, const _Kty& _Keyval) noexcept { // accumulate _Keyval into partial FNV-1a hash _Val
			static_assert(std::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(_Val, &reinterpret_cast<const unsigned char&>(_Keyval), sizeof(_Kty));
		}

		// FUNCTION TEMPLATE _Hash_representation
		template <class _Kty>
		size_t hash_representation(const _Kty& _Keyval) noexcept { // bitwise hashes the representation of a key
			return fnv1a_append_value(_FNV_offset_basis, _Keyval);
		}

		// FUNCTION TEMPLATE _Hash_array_representation
		template <class _Kty>
		size_t hash_array_representation(
			const _Kty* const _First, const size_t _Count) noexcept { // bitwise hashes the representation of an array
			static_assert(std::is_trivial_v<_Kty>, "Only trivial types can be directly hashed.");
			return fnv1a_append_bytes(
				_FNV_offset_basis, reinterpret_cast<const unsigned char*>(_First), _Count * sizeof(_Kty));
		}
	}

}

#endif