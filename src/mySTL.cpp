#include "../include/mySTL.h"

namespace wjr {
	inline namespace math {

		constexpr static const char* const tabel = "\0\0\1\1\2\2\2\2\3\3\3\3\3\3\3\3";

		constexpr static const int debruijn[32] = {
			0,  1, 28,  2, 29, 14, 24,  3, 30, 22, 20, 15, 25, 17,  4,  8,
			31, 27, 13, 23, 21, 19, 16,  7, 26, 12, 18,  6, 11,  5, 10,  9
		};

	#define MY_QUICK_LOG2(ANS,X)			\
		if(X >> 16) { ANS |= 16; X>>= 16; } \
		if(X >> 8) { ANS |= 8; X>>= 8; }	\
		if(X >> 4) { ANS |= 4; X>>=4; }		\

		constexpr size_t qlog2(uint32_t x) {
			size_t ans = 0;
			MY_QUICK_LOG2(ans, x);
			return ans | tabel[x];
		}

		constexpr size_t qlog2(int x) {
			return qlog2(static_cast<uint32_t>(x));
		}

		constexpr size_t qlog2(uint64_t x) {
			size_t ans = 0;
			if (x >> 32) { ans |= 32; x >>= 32; }
			MY_QUICK_LOG2(ans, x);
			return ans | tabel[x];
		}

		constexpr size_t qlog2(long long x) {
			return qlog2(static_cast<uint64_t>(x));
		}

		constexpr unsigned int find_number(uint32_t n) {
			n = n - ((n >> 1) & 0x55555555);
			n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
			n = ((n >> 4) + n) & 0x0f0f0f0f;
			n = (n * 0x01010101) >> 24;
			return n;
		}

		constexpr unsigned int find_number(uint64_t n) {
			return find_number(static_cast<uint32_t>(n)) +
				find_number(static_cast<uint32_t>(n >> sizeof(uint32_t)));
		}

		constexpr unsigned int _minx(uint32_t x) {
			return debruijn[((x & ((~x) + 1)) * 0x077CB531U) >> 27];
		}

		constexpr bool is_power_of_two(int x) {
			return (x > 0) && (!(x & (x - 1)));
		}

		constexpr bool is_power_of_two(unsigned int x) {
			return (x != 0) && (!(x & (x - 1)));
		}

		constexpr bool is_power_of_two(long long x) {
			return (x > 0) && (!(x & (x - 1)));
		}

		constexpr bool is_power_of_two(unsigned long long x) {
			return (x != 0) && (!(x & (x - 1)));
		}

	}

}