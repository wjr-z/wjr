#include <wjr/algorithm.h>

_WJR_BEGIN

// compare
#if defined(__HAS_FAST_INSTRUCTION_MEMCMP)

const uint8_t* memcmp_u8(const uint8_t* _First1, const uint8_t* _Last1, const uint8_t* _First2) {
	const uint8_t* i = _First1;
	const uint8_t* j = _First2;
	const auto n = _Last1 - _First1;
#if defined(__AVX2__)
	if (n >= 32) {
		do {
			__m256i x = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(i));
			__m256i y = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(j));
			__m256i r = _mm256_cmpeq_epi8(x, y);
			int z = _mm256_movemask_epi8(r);
			if (z != 0xffffffff) {
				return i + wjr::countr_one(static_cast<unsigned int>(z));
			}
			i += 32;
			j += 32;
		} while (i + 32 <= _Last1);
		if (i < _Last1) {
			i = _Last1 - 32;
			j = _First2 + n - 32;
			__m256i x = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(i));
			__m256i y = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(j));
			__m256i r = _mm256_cmpeq_epi8(x, y);
			int z = _mm256_movemask_epi8(r);
			if (z != 0xffffffff) {
				return i + wjr::countr_one(static_cast<unsigned int>(z));
			}
		}
	}
	else {
		// i is equal to _First1
		// j is equal to _First2
		if (n < 16) {
			for (; i < _Last1; ++i, ++j) {
				if (*i != *j) {
					return i;
				}
			}
		}
		else {
			__m256i x = _mm256_loadu2_m128i(
				reinterpret_cast<const __m128i*>(i + (n - 16)),
				reinterpret_cast<const __m128i*>(i)
			);
			__m256i y = _mm256_loadu2_m128i(
				reinterpret_cast<const __m128i*>(j + (n - 16)),
				reinterpret_cast<const __m128i*>(j)
			);
			__m256i r = _mm256_cmpeq_epi8(x, y);
			int z = _mm256_movemask_epi8(r);
			if (z != 0xffffffff) {
				int cl = wjr::countr_one(static_cast<unsigned int>(z));
				if (cl < 16) {
					return i + cl;
				}
				else {
					return i + (n - 16) + (cl - 16);
				}
			}
		}
	}

#else
	if (n >= 16) {
		do {
			__m128i x = __MM_LOADU(reinterpret_cast<const __m128i*>(i));
			__m128i y = __MM_LOADU(reinterpret_cast<const __m128i*>(j));
			__m128i r = _mm_cmpeq_epi8(x, y);
			int z = _mm_movemask_epi8(r);
			if (z != 0xffff) {
				return i + wjr::countr_one(static_cast<uint16_t>(z));
			}
			i += 16;
			j += 16;
		} while (i + 16 <= _Last1);
		if (i < _Last1) {
			i = _Last1 - 16;
			j = _First2 + n - 16;
			__m128i x = __MM_LOADU(reinterpret_cast<const __m128i*>(i));
			__m128i y = __MM_LOADU(reinterpret_cast<const __m128i*>(j));
			__m128i r = _mm_cmpeq_epi8(x, y);
			int z = _mm_movemask_epi8(r);
			if (z != 0xffff) {
				return i + wjr::countr_one(static_cast<uint16_t>(z));
			}
		}
	}
	else {
		// i is equal to _First1
		// j is equal to _First2
		for (; i < _Last1; ++i, ++j) {
			if (*i != *j) {
				return i;
			}
		}
	}
#endif
	return _Last1;
}

#endif // defined(__HAS_FAST_INSTRUCTION)

_WJR_END