#include <wjr/algorithm.h>

_WJR_BEGIN

// find

#if defined(__SSE3__)
#define __MM_LOADU _mm_lddqu_si128
#else
#define __MM_LOADU _mm_loadu_si128
#endif

#if defined(__AVX2__) || defined(__SSE2__)
template<typename T>
WJR_FORCEINLINE decltype(auto) _MM_set_epi(const T& _Val) {
#if defined(__AVX2__)
	if constexpr (std::is_same_v<T, uint8_t>) {
		return _mm256_set1_epi8(_Val);
	}
	else if constexpr (std::is_same_v<T, uint16_t>) {
		return _mm256_set1_epi16(_Val);
	}
	else if constexpr (std::is_same_v<T, uint32_t>) {
		return _mm256_set1_epi32(_Val);
	}
#else
	if constexpr (std::is_same_v<T, uint8_t>) {
		return _mm_set1_epi8(_Val);
	}
	else if constexpr (std::is_same_v<T, uint16_t>) {
		return _mm_set1_epi16(_Val);
	}
	else if constexpr (std::is_same_v<T, uint32_t>) {
		return _mm_set1_epi32(_Val);
	}
#endif
}

template<typename T>
WJR_FORCEINLINE decltype(auto) _MM_loadu(const T* ptr) {
#if defined(__AVX2__)
	return _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
#else
#if defined(__SSE3__)
	return _mm_lddqu_si128(reinterpret_cast<const __m128i*>(ptr));
#else
	return _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
#endif
#endif
}

template<typename T, typename _MM>
WJR_FORCEINLINE decltype(auto) _MM_cmpeq(const _MM& _Left, const _MM& _Right) {
#if defined(__AVX2__)
	if constexpr (std::is_same_v<T, uint8_t>) {
		return _mm256_cmpeq_epi8(_Left, _Right);
}
	else if constexpr (std::is_same_v<T, uint16_t>) {
		return _mm256_cmpeq_epi16(_Left, _Right);
	}
	else if constexpr (std::is_same_v<T, uint32_t>) {
		return _mm256_cmpeq_epi32(_Left, _Right);
	}
#else
	if constexpr (std::is_same_v<T, uint8_t>) {
		return _mm_cmpeq_epi8(_Left, _Right);
	}
	else if constexpr (std::is_same_v<T, uint16_t>) {
		return _mm_cmpeq_epi16(_Left, _Right);
	}
	else if constexpr (std::is_same_v<T, uint32_t>) {
		return _mm_cmpeq_epi32(_Left, _Right);
	}
#endif
}

template<typename _MM>
WJR_FORCEINLINE decltype(auto) _MM_movemask(const _MM& _Val) {
#if defined(__AVX2__)
	return static_cast<uint32_t>(_mm256_movemask_epi8(_Val));
#else
	return static_cast<uint16_t>(_mm_movemask_epi8(_Val));
#endif
}

constexpr size_t _MM_bits() {
#if defined(__AVX2__)
	return 256;
#else
	return 128;
#endif
}

template<typename T>
constexpr size_t _MM_step() {
	return _MM_bits() / (sizeof(T) * 8);
}

#endif

#if defined(__HAS_FAST_INSTRUCTION_MEMCHR)

template<typename T, std::enable_if_t<wjr::is_any_of_v<T, uint8_t, uint16_t, uint32_t>, int> = 0>
const T* _MM_memchr(const T* _First, const T* _Last, const T& _Val) {
	constexpr auto step = _MM_step<T>();
	constexpr auto cor = sizeof(T) / sizeof(uint8_t);

	const T* i = _First;
	const auto n = _Last - _First;
	auto q = _MM_set_epi<T>(_Val);

	if (n >= step) {
		do {
			auto x = _MM_loadu(i);
			auto r = _MM_cmpeq<T>(x, q);
			auto z = _MM_movemask(r);
			if (z) {
				return i + wjr::countr_zero(z) / cor;
			}
			i += step;
		} while (i + step <= _Last);
		if (i < _Last) {
			i = _Last - step;
			auto x = _MM_loadu(i);
			auto r = _MM_cmpeq<T>(x, q);
			auto z = _MM_movemask(r);
			if (z) {
				return i + wjr::countr_zero(z) / cor;
			}
		}
	}
	else {
#if defined(__AVX2__)
		if constexpr (step <= 16) {
#endif
			for (; i < _Last; ++i) {
				if (*i == _Val) {
					return i;
				}
			}
#if defined(__AVX2__)
		}
		else {
			if (n < 16) {
				for (; i < _Last; ++i) {
					if (*i == _Val) {
						return i;
					}
				}
			}
			else {
				__m256i x = _mm256_loadu2_m128i(
					reinterpret_cast<const __m128i*>(i + (n - 16)),
					reinterpret_cast<const __m128i*>(i)
				);
				__m256i r = _mm256_cmpeq_epi8(x, q);
				auto z =_MM_movemask(r);
				if (z) {
					int cl = wjr::countr_zero(z);
					if (cl < 16) {
						return i + cl;
					}
					else {
						return i + (n - 16) + (cl - 16);
					}
				}
			}
		}
#endif
	}
	return _Last;
}

const uint8_t* memchr_u8(const uint8_t* _First, const uint8_t* _Last, const uint8_t& _Val) {
	return _MM_memchr(_First, _Last, _Val);
}

const uint16_t* memchr_u16(const uint16_t* _First, const uint16_t* _Last, const uint16_t& _Val) {
	return _MM_memchr(_First, _Last, _Val);
}

const uint32_t* memchr_u32(const uint32_t* _First, const uint32_t* _Last, const uint32_t& _Val) {
	return _MM_memchr(_First, _Last, _Val);
}

template<typename T, std::enable_if_t<wjr::is_any_of_v<T, uint8_t, uint16_t, uint32_t>, int> = 0>
const T* _MM_memrchr(const T* _First, const T* _Last, const T& _Val) {
	constexpr auto step = _MM_step<T>();
	constexpr auto cor = sizeof(T) / sizeof(uint8_t);

	const T* i = _Last;
	const auto n = _Last - _First;
	auto q = _MM_set_epi<T>(_Val);

	if (n >= step) {
		do {
			i -= step;
			auto x = _MM_loadu(i);
			auto r = _MM_cmpeq<T>(x, q);
			auto z = _MM_movemask(r);
			if (z) {
				return i + step - wjr::countl_zero(z) / cor;
			}
		} while (i - step >= _First);
		if (i > _First) {
			i = _First;
			auto x = _MM_loadu(i);
			auto r = _MM_cmpeq<T>(x, q);
			auto z = _MM_movemask(r);
			if (z) {
				return i + step - wjr::countl_zero(z) / cor;
			}
		}
	}
	else {
#if defined(__AVX2__)
		if constexpr (step <= 16) {
#endif
			for (; i > _First; ) {
				--i;
				if (*i == _Val) {
					return i + 1;
				}
			}
#if defined(__AVX2__)
		}
		else {
			if (n < 16) {
				for (; i > _First; ) {
					--i;
					if (*i == _Val) {
						return i + 1;
					}
				}
			}
			else {
				__m256i x = _mm256_loadu2_m128i(
					reinterpret_cast<const __m128i*>(_First + (n - 16)),
					reinterpret_cast<const __m128i*>(_First)
				);
				__m256i r = _mm256_cmpeq_epi8(x, q);
				auto z = _MM_movemask(r);
				if (z) {
					int cl = wjr::countl_zero(z);
					if (cl < 16) {
						return _First + (n - 16) + (16 - cl);
					}
					else {
						return _First + (16 - (cl - 16));
					}
				}
			}
		}
#endif
	}
	return _First;
}

const uint8_t* memrchr_u8(const uint8_t* _First, const uint8_t* _Last, const uint8_t& _Val) {
	return _MM_memrchr(_First, _Last, _Val);
}

const uint16_t* memrchr_u16(const uint16_t* _First, const uint16_t* _Last, const uint16_t& _Val) {
	return _MM_memrchr(_First, _Last, _Val);
}

const uint32_t* memrchr_u32(const uint32_t* _First, const uint32_t* _Last, const uint32_t& _Val) {
	return _MM_memrchr(_First, _Last, _Val);
}

#endif // defined(__AVX2__) || defined(__SSE2__)

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