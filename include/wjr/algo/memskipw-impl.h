#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif // __WJR_ALGO_ALOG_H

// __WJR_MEMSKIPW_ONE
// __WJR_MEMSKIPW_FOUR
// __WJR_MEMSKIPW_NAME
// __WJR_MEMSKIPW_PRED

_WJR_ALGO_BEGIN

WJR_NODISCARD inline const uint8_t* (__WJR_MEMSKIPW_NAME)(const uint8_t* s, size_t n) noexcept {
	if (is_unlikely(n == 0)) return s;

	WJR_SIMD_INIT_PTR(s, n);

	if (n >= 4) {

		// for most cases, it is possible to quickly find non whitespace characters
		// at this time, using simd can actually reduce performance

		// solve first 4 bytes

		{
			const auto u = WJR_SIMD_PTR(s);
			if (__WJR_MEMSKIPW_PRED(*u)) {
				return s;
			}
		}

		{
			const auto u = WJR_SIMD_PTR(WJR_SIMD_ADD_PTR(s, 1));
			if (__WJR_MEMSKIPW_PRED(*u)) {
				return WJR_SIMD_ADD_PTR(s, 1);
			}
		}

		{
			const auto u = WJR_SIMD_PTR(WJR_SIMD_ADD_PTR(s, 2));
			if (__WJR_MEMSKIPW_PRED(*u)) {
				return WJR_SIMD_ADD_PTR(s, 2);
			}
		}

		{
			const auto u = WJR_SIMD_PTR(WJR_SIMD_ADD_PTR(s, 3));
			if (__WJR_MEMSKIPW_PRED(*u)) {
				return WJR_SIMD_ADD_PTR(s, 3);
			}
		}

		WJR_SIMD_INC_PTR(s, 4);
		n -= 4;
	}

	// n = [1, 2, 3]
	{
		if (__WJR_MEMSKIPW_PRED(*WJR_SIMD_PTR(s))) { return s; }
		if (n == 1) { return WJR_SIMD_ADD_PTR(s, 1); }
		if (__WJR_MEMSKIPW_PRED(*WJR_SIMD_PTR(WJR_SIMD_ADD_PTR(s, 1)))) { 
			return WJR_SIMD_ADD_PTR(s, 1); 
		}
		if (n == 2) { return WJR_SIMD_ADD_PTR(s, 2); }
		if (__WJR_MEMSKIPW_PRED(*WJR_SIMD_PTR(WJR_SIMD_ADD_PTR(s, 2)))) {
			return WJR_SIMD_ADD_PTR(s, 2);
		}
		return WJR_SIMD_ADD_PTR(s, 3);
	}

	const auto e = WJR_SIMD_ADD_PTR(s, n);

	for (; s != e; WJR_SIMD_INC_PTR(s, 1)) {
		auto u = WJR_SIMD_PTR(s);
		if (__WJR_MEMSKIPW_PRED(*u)) {
			break;
		}
	}

	return s;
}

_WJR_ALGO_END