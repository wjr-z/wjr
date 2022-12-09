#include <wjr/simd/memchr-avx2.h>

#if defined(__AVX2__)
#include <wjr/simd/simd.h>

_WJR_SIMD_BEGIN

#define __WJR_AVX2_ONE_MEMCHR(s, q)												                \
	{																			                \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>((s)));		                \
		auto r = traits::cmpeq(x, (q));									                        \
		auto z = traits::movemask_epi8(r);									                    \
		if(z){																	                \
			return (s) + wjr::countr_zero(static_cast<uint32_t>(z)) / _Mycor;	                \
		}																		                \
	}

#define __WJR_AVX2_FOUR_MEMCHR(s, q)															\
	{																							\
		auto x0 = traits::loadu(reinterpret_cast<const simd_type*>((s)));					    \
		auto x1 = traits::loadu(reinterpret_cast<const simd_type*>((s) + _Mysize));			    \
		auto x2 = traits::loadu(reinterpret_cast<const simd_type*>((s) + _Mysize * 2));		    \
		auto x3 = traits::loadu(reinterpret_cast<const simd_type*>((s) + _Mysize * 3));		    \
																								\
		auto r0 = traits::cmpeq(x0, (q));												        \
		auto r1 = traits::cmpeq(x1, (q));												        \
		auto r2 = traits::cmpeq(x2, (q));												        \
		auto r3 = traits::cmpeq(x3, (q));												        \
																								\
		auto Z = traits::movemask_epi8(														    \
			traits::Or(traits::Or(r0, r1), traits::Or(r2, r3)));				                \
		if(Z){																					\
			auto z0 = traits::movemask_epi8(r0);												\
			if(z0){																				\
				return (s) + wjr::countr_zero(static_cast<uint32_t>(z0)) / _Mycor;				\
			}																					\
			auto z1 = traits::movemask_epi8(r1);												\
			if(z1){																				\
				return (s) + _Mysize + wjr::countr_zero(static_cast<uint32_t>(z1)) / _Mycor;	\
			}																					\
			auto z2 = traits::movemask_epi8(r2);												\
			if(z2){																				\
				return (s) + _Mysize * 2 + wjr::countr_zero(static_cast<uint32_t>(z2)) / _Mycor;\
			}																					\
			auto z3 = traits::movemask_epi8(r3);												\
			if(z3){																				\
				return (s) + _Mysize * 3 + wjr::countr_zero(static_cast<uint32_t>(z3)) / _Mycor;\
			}																					\
		}																						\
	}


template<typename T>
const T* __AVX2_memchr(const T* s, T val, size_t n) {
	using traits = traits<T, simd_type::AVX>;
	using simd_type = typename traits::simd_type;

	constexpr int _Mysize = traits::size();
	constexpr int _Mycor = sizeof(T) / sizeof(uint8_t);

	auto q = traits::set1(val);

	if (n < _Mysize) {
		constexpr int threshold = sizeof(uint32_t) / sizeof(T);
		if (n >= threshold) {

			int k = n / threshold;
			int l = k * threshold;

			auto x = traits::preload_si32(s, k);
			auto r = traits::cmpeq(x, q);
			int z = traits::movemask_epi8(r) & ((1u << (l * _Mycor)) - 1);

			if (z) {
				return s + wjr::countr_zero(static_cast<uint32_t>(z)) / _Mycor;
			}

			s += l;
			n -= l;
		}

		switch (n) {
		case 3:
			if (*s == val) {
				return s;
			}
			++s;
			WJR_FALLTHROUGH;
		case 2:
			if (*s == val) {
				return s;
			}
			++s;
			WJR_FALLTHROUGH;
		case 1:
			if (*s == val) {
				return s;
			}
			++s;
			break;
		default:break;
		}
		return s;
	}

	{
		__WJR_AVX2_ONE_MEMCHR(s, q);
	}

	// [_Mysize, _Mysize * 2]
	if (n <= _Mysize * 2) {
		s += n;
		if (n != _Mysize) {
			__WJR_AVX2_ONE_MEMCHR(s - _Mysize, q);
		}
		return s;
	}

	{
		__WJR_AVX2_ONE_MEMCHR(s + _Mysize, q);
	}

	// (_Mysize * 2, _Mysize * 4]
	if (n <= _Mysize * 4) {
		s += n;
		__WJR_AVX2_ONE_MEMCHR(s - _Mysize * 2, q);
		__WJR_AVX2_ONE_MEMCHR(s - _Mysize, q);
		return s;
	}

	{
		__WJR_AVX2_ONE_MEMCHR(s + _Mysize * 2, q);
		__WJR_AVX2_ONE_MEMCHR(s + _Mysize * 3, q);
	}

	if (n <= _Mysize * 8) {
		s += n;
		__WJR_AVX2_FOUR_MEMCHR(s - _Mysize * 4, q);
		return s;
	}

	__WJR_AVX2_FOUR_MEMCHR(s + 4 * _Mysize, q);

	s += 8 * _Mysize;
	n -= 8 * _Mysize;

	while (n >= _Mysize * 4) {
		__WJR_AVX2_FOUR_MEMCHR(s, q);
		s += _Mysize * 4;
		n -= _Mysize * 4;
	}
	if (n) {
		s += n;
		__WJR_AVX2_FOUR_MEMCHR(s - _Mysize * 4, q);
	}
	return s;
}

const uint8_t* avx2_memchr(const uint8_t* s, uint8_t val, size_t n) {
	return __AVX2_memchr(s, val, n);
}

const uint16_t* avx2_memchr(const uint16_t* s, uint16_t val, size_t n) {
	return __AVX2_memchr(s, val, n);
}

const uint32_t* avx2_memchr(const uint32_t* s, uint32_t val, size_t n) {
	return __AVX2_memchr(s, val, n);
}

_WJR_SIMD_END

#endif // __AVX2__