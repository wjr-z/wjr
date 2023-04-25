#ifndef __WJR_ALGO_MEM_ALL_H
#error "This file should not be included directly. Include <wjr/algo/mem-all.h> instead."
#endif // __WJR_ALGO_MEM_ALL_H

// fast skip white space(' ', '\n', '\t', '\r', '\v', '\f') -> (9 ~ 13, 32)
// notice that
// 1. For non line leaders : 
//    most formatted strings have a continuous white space character count of 1
// 2. For line leaders :
//	  except for the first line, each other line will have a newline character, may be "\n"/"\r\n"
//    and most strings will use at least 1 tabs or at least 2 spaces
//
// to ensure that the worst-case scenario does not degrade significantly, 
// it is necessary to make a simple judgment on the first four characters before using simd
// for strings less than 4 + 16 in length, use a simple loop

#if defined(_WJR_FAST_MEMSKIPW)

_WJR_ALGO_BEGIN

struct __memskipw_table {
	constexpr __memskipw_table() : m_table() {
		for (auto i = 0; i < 256; ++i) {
			m_table[i] = (i >= 9 && i <= 13) || i == 32;
		}
	}
	WJR_PURE constexpr bool operator[](char c) const {
		return m_table[wjr::make_unsigned_v(c)];
	}
private:
	bool m_table[256];
};

constexpr static __memskipw_table __memskipw_table_instance = {};

#define WJR_SIMD_IS_BACKWARD 0

WJR_ATTRIBUTE(NODISCARD, PURE, INLINE) const char* __memskipw(const char* s, const char* e) {
	if (WJR_UNLIKELY(s == e)) return s;
	if(WJR_LIKELY(!__memskipw_table_instance[*s])) return s;
	++s;
	if (WJR_LIKELY(s + 3 <= e)) {
		if (!__memskipw_table_instance[*s]) return s;
		if (!__memskipw_table_instance[s[1]]) return s + 1;
		if (!__memskipw_table_instance[s[2]]) return s + 2;

		s += 3;
		if (WJR_LIKELY(s + 16 <= e)) {
			const auto simd_32 = simd::sse::set1_epi8(32);
			const auto simd_9 = simd::sse::set1_epi8(9);
			const auto simd_5 = simd::sse::set1_epi8(4);

			do {
				WJR_SIMD_LOADU(simd::sse, x, s);
				auto y = simd::sse::cmpeq(x, simd_32, uint8_t());
				auto z = simd::sse::sub(x, simd_9, uint8_t());
				z = simd::sse::cmple(z, simd_5, uint8_t());
				auto r = simd::sse::movemask_epi8(simd::sse::Or(y, z));
				if (WJR_LIKELY(r != simd::sse::mask())) {
					return WJR_SIMD_FIRST_ZERO_PTR(s, r);
				}
				s += 16;
			} while (s <= e);
		}
	}
	for (; s < e && __memskipw_table_instance[*s]; ++s);
	return s;
}

#undef WJR_SIMD_IS_BACKWARD

_WJR_ALGO_END

#endif // _WJR_FAST_MEMSKIPW