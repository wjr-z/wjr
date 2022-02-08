#include "../include/mySTL.h"

namespace wjr {
	inline namespace wjr_math {

		constexpr static unsigned int quick_log2_tabel[32] =
		{ 0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3 };

		constexpr unsigned int quick_log2(int x) {
			assert(x >= 0);
			return quick_log2((unsigned int)x);
		}

		constexpr unsigned int quick_log2(unsigned int x) {
			unsigned int ans = 0;
			if (x >> 16) { ans += 16; x >>= 16; }
			if (x >> 8) { ans += 8; x >>= 8; }
			if (x >> 4) { ans += 4; x >>= 4; }
			return ans + quick_log2_tabel[x];
		}

		constexpr unsigned int quick_log2(long long x) {
			assert(x >= 0);
			return quick_log2((unsigned long long)x);
		}

		constexpr unsigned int quick_log2(unsigned long long x) {
			unsigned int ans = 0;
			if (x >> 32) { ans += 32; x >>= 32; }
			if (x >> 16) { ans += 16; x >>= 16; }
			if (x >> 8) { ans += 8; x >>= 8; }
			if (x >> 4) { ans += 4; x >>= 4; }
			return ans + quick_log2_tabel[x];
		}

	}
}