#pragma once
#ifndef __WJR_SIMD___MEMRCHR_H
#define __WJR_SIMD___MEMRCHR_H

#include <wjr/macro.h>

#if defined(__AVX2__) || defined(__SSE2__)
#include <wjr/simd/simd_helper.h>
#include <wjr/math.h>

_WJR_SIMD_BEGIN

#define __WJR_ONE_MEMRCHR(s, q)																		\
	{																				                \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>((s)));			                \
		auto r = traits::cmpeq(x, (q));										                        \
		auto z = traits::movemask_epi8(r);										                    \
		if(z){																		                \
			return (s) + _Mysize - wjr::countl_zero(static_cast<value_type>(z)) / _Mycor;	        \
		}																			                \
	}

#define __WJR_FOUR_MEMRCHR(s, q)																	\
	{																						        \
		auto x0 = traits::loadu(reinterpret_cast<const simd_type*>((s)));				            \
		auto x1 = traits::loadu(reinterpret_cast<const simd_type*>((s) + _Mysize));		            \
		auto x2 = traits::loadu(reinterpret_cast<const simd_type*>((s) + _Mysize * 2));	            \
		auto x3 = traits::loadu(reinterpret_cast<const simd_type*>((s) + _Mysize * 3));	            \
																							        \
		auto r0 = traits::cmpeq(x0, (q));											                \
		auto r1 = traits::cmpeq(x1, (q));											                \
		auto r2 = traits::cmpeq(x2, (q));											                \
		auto r3 = traits::cmpeq(x3, (q));											                \
																							        \
		auto Z = traits::movemask_epi8(traits::Or(traits::Or(r0, r1), traits::Or(r2, r3)));         \
		if(Z){																				        \
			auto z3 = traits::movemask_epi8(r3);											        \
			if(z3){																			        \
				return (s) + _Mysize * 4 - wjr::countl_zero(static_cast<value_type>(z3)) / _Mycor;	\
			}																				        \
			auto z2 = traits::movemask_epi8(r2);											        \
			if(z2){																			        \
				return (s) + _Mysize * 3 - wjr::countl_zero(static_cast<value_type>(z2)) / _Mycor;	\
			}																				        \
			auto z1 = traits::movemask_epi8(r1);											        \
			if(z1){																			        \
				return (s) + _Mysize * 2 - wjr::countl_zero(static_cast<value_type>(z1)) / _Mycor;	\
			}																				        \
			auto z0 = traits::movemask_epi8(r0);											        \
			if(z0){																			        \
				return (s) + _Mysize - wjr::countl_zero(static_cast<value_type>(z0)) / _Mycor;		\
			}																				        \
		}																					        \
	}

template<simd_type S, typename T>
const T* __memrchr(const T* s, T val, size_t n) {
	using traits = traits<T, S>;
	using simd_type = typename traits::simd_type;
	using value_type = std::conditional_t<simd::simd_type::AVX == S, uint32_t, uint16_t>;

	constexpr int _Mysize = traits::size();
	constexpr int _Mycor = sizeof(T) / sizeof(uint8_t);

	if (n == 0) { return s; }
	if (n == 1) { return *s == val ? s + 1 : s; }

	auto q = traits::set1(val);

	s += n;

	if (n < _Mysize) {
		auto ns = n * sizeof(T);
		auto ql = ns >> 1;
		auto sl = ql << 1;
		s -= sl / sizeof(T);
		auto x = traits::preload_si16x(s, ql);
		auto r = traits::cmpeq(x, q);
		auto z = traits::movemask_epi8(r) & ((1u << sl) - 1);
		if (z) {
			return s + _Mysize - wjr::countl_zero(static_cast<value_type>(z)) / _Mycor;
		}
		n -= sl / sizeof(T);
		if constexpr (sizeof(T) == 1) {
			if (ns & 1) {
				--s;
				if (*s == val) {
					return 1 + s;
				}
			}
		}
		return s;
	}

	{
		__WJR_ONE_MEMRCHR(s - _Mysize, q);
	}

	// [_Mysize, _Mysize * 2]
	if (n <= 2 * _Mysize) {
		s -= n;
		if (n != _Mysize) {
			__WJR_ONE_MEMRCHR(s, q);
		}
		return s;
	}

	{
		__WJR_ONE_MEMRCHR(s - _Mysize * 2, q);
	}

	// (_Mysize * 2, _Mysize * 4] 
	if (n <= 4 * _Mysize) {
		s -= n;
		__WJR_ONE_MEMRCHR(s + _Mysize, q);
		__WJR_ONE_MEMRCHR(s, q);
		return s;
	}

	{
		__WJR_ONE_MEMRCHR(s - _Mysize * 3, q);
		__WJR_ONE_MEMRCHR(s - _Mysize * 4, q);
	}

	if (n <= _Mysize * 8) {
		s -= n;
		__WJR_FOUR_MEMRCHR(s, q);
		return s;
	}

	s -= _Mysize * 8;
	__WJR_FOUR_MEMRCHR(s, q);

	n -= _Mysize * 8;

	while (n >= _Mysize * 4) {
		s -= _Mysize * 4;
		__WJR_FOUR_MEMRCHR(s, q);
		n -= _Mysize * 4;
	}

	if (n) {
		s -= n;
		__WJR_FOUR_MEMRCHR(s, q);
	}
	return s;
}

#undef __WJR_ONE_MEMRCHR
#undef __WJR_FOUR_MEMRCHR

_WJR_SIMD_END

#endif // __AVX2__ || __SSE2__

#endif // __WJR_SIMD___MEMRCHR_H