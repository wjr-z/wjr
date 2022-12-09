#include <wjr/simd/memrcmp-sse2.h>

#if defined(__SSE2__)
#include <wjr/simd/simd.h>

_WJR_SIMD_BEGIN

#define __WJR_AVX2_ONE_MEMRCMP(s0, s1)												                    \
	{																				                    \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>((s0)));		                        \
		auto y = traits::loadu(reinterpret_cast<const simd_type*>((s1)));		                        \
		auto r = traits::cmpeq(x, y);										                            \
		auto z = traits::movemask_epi8(r);										                        \
		if (z != __Mask) {															                    \
			return (s0) + _Mysize - wjr::countl_one(static_cast<uint16_t>(z)) / _Mycor;	                \
		}																			                    \
	}

#define __WJR_AVX2_FOUR_MEMRCMP(s0, s1)														            \
	{																						            \
		auto x0 = traits::loadu(reinterpret_cast<const simd_type*>((s0)));				                \
		auto x1 = traits::loadu(reinterpret_cast<const simd_type*>((s0) + _Mysize * 1));	            \
		auto x2 = traits::loadu(reinterpret_cast<const simd_type*>((s0) + _Mysize * 2));	            \
		auto x3 = traits::loadu(reinterpret_cast<const simd_type*>((s0) + _Mysize * 3));	            \
																							            \
		auto x4 = traits::loadu(reinterpret_cast<const simd_type*>((s1)));				                \
		auto x5 = traits::loadu(reinterpret_cast<const simd_type*>((s1) + _Mysize * 1));	            \
		auto x6 = traits::loadu(reinterpret_cast<const simd_type*>((s1) + _Mysize * 2));	            \
		auto x7 = traits::loadu(reinterpret_cast<const simd_type*>((s1) + _Mysize * 3));	            \
																							            \
		auto r0 = traits::cmpeq(x0, x4);												                \
		auto r1 = traits::cmpeq(x1, x5);												                \
		auto r2 = traits::cmpeq(x2, x6);												                \
		auto r3 = traits::cmpeq(x3, x7);												                \
																							            \
		auto Z = traits::movemask_epi8(traits::And(traits::And(r0, r1), traits::And(r2, r3)));			\
																							            \
		if(Z != __Mask){																	            \
			auto z0 = traits::movemask_epi8(r0);											            \
			if(z0 != __Mask){																            \
				return (s0) + _Mysize * 1 - wjr::countl_one(static_cast<uint16_t>(z0)) / _Mycor;	    \
			}																				            \
			auto z1 = traits::movemask_epi8(r1);											            \
			if(z1 != __Mask){																            \
				return (s0) + _Mysize * 2 - wjr::countl_one(static_cast<uint16_t>(z1)) / _Mycor;	    \
			}																				            \
			auto z2 = traits::movemask_epi8(r2);											            \
			if(z2 != __Mask){																            \
				return (s0) + _Mysize * 3 - wjr::countl_one(static_cast<uint16_t>(z2)) / _Mycor;	    \
			}																				            \
			auto z3 = traits::movemask_epi8(r3);											            \
			if(z3 != __Mask){																            \
				return (s0) + _Mysize * 4 - wjr::countl_one(static_cast<uint16_t>(z3)) / _Mycor;	    \
			}																				            \
		}																					            \
	}

const uint8_t* sse2_memrcmp(const uint8_t* s0, const uint8_t* s1, size_t n) {
	using traits = traits<uint8_t, simd_type::SSE>;
	using simd_type = typename traits::simd_type;

	constexpr int _Mysize = traits::size();
	constexpr int _Mycor = 1;
	constexpr int __Mask = 0x0000ffff;

	s0 += n;
	s1 += n;

	switch (n / _Mysize) {
	case 0: {
		for (; n; --n) {
			--s0;
			--s1;
			if (*s0 != *s1) {
				return 1 + s0;
			}
		}
		return s0;
	}
	case 3: {
		s0 -= _Mysize;
		s1 -= _Mysize;
		__WJR_AVX2_ONE_MEMRCMP(s0, s1);
		n -= _Mysize;
		WJR_FALLTHROUGH;
	}
	case 2: {
		s0 -= _Mysize;
		s1 -= _Mysize;
		__WJR_AVX2_ONE_MEMRCMP(s0, s1);
		n -= _Mysize;
		WJR_FALLTHROUGH;
	}
	case 1: {
		s0 -= _Mysize;
		s1 -= _Mysize;
		__WJR_AVX2_ONE_MEMRCMP(s0, s1);
		n -= _Mysize;

		if (n) {
			s0 -= n;
			s1 -= n;
			__WJR_AVX2_ONE_MEMRCMP(s0, s1);
		}

		return s0;
	}
	default: {
		do {
			s0 -= _Mysize * 4;
			s1 -= _Mysize * 4;
			__WJR_AVX2_FOUR_MEMRCMP(s0, s1);
			n -= _Mysize * 4;
		} while (n >= _Mysize * 4);
		
		if (n) {
			s0 -= n;
			s1 -= n;
			__WJR_AVX2_FOUR_MEMRCMP(s0, s1);
		}

		return s0;
	}
	}
}

_WJR_SIMD_END

#endif // __SSE2__