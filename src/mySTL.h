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

		static std::mt19937 basic_mt_rand(std::time(NULL));

		template<typename T>
		T mt19937_rand() {
			return static_cast<T>(basic_mt_rand());
		}

		template<typename T>
		T mt19937_rand(T L, T R) { // return a rand value of [L,R)
			assert(L < R);
			T mod = R - L;
			if constexpr (std::is_unsigned_v<T>) {
				return (mt19937_rand<T>() % mod) + L;
			}
			else {
				return ((mt19937_rand<T>() % mod) + mod) % mod + L;
			}
		}

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

	}

}

#endif