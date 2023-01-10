#pragma once
#ifndef __WJR_SIMD___MEMCHR_H__
#define __WJR_SIMD___MEMCHR_H__

#include <wjr/macro.h>
#if defined(__AVX2__) || defined(__SSE2__)
#include <wjr/simd/simd_helper.h>

_WJR_SIMD_BEGIN

#define __WJR_ONE_MEMCHR(s, q)																		\
	{																			                    \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>((s)));			                \
		auto r = traits::cmpeq(x, (q));											                    \
		auto z = traits::movemask_epi8(r);										                    \
		if(z){																	                    \
			return (s) + wjr::countr_zero(static_cast<value_type>(z)) / _Mycor;	                    \
		}																		                    \
	}

#define __WJR_FOUR_MEMCHR(s, q)																		\
	{																								\
		auto x0 = traits::loadu(reinterpret_cast<const simd_type*>((s)));							\
		auto x1 = traits::loadu(reinterpret_cast<const simd_type*>((s) + _Mysize));					\
		auto x2 = traits::loadu(reinterpret_cast<const simd_type*>((s) + _Mysize * 2));				\
		auto x3 = traits::loadu(reinterpret_cast<const simd_type*>((s) + _Mysize * 3));				\
																									\
		auto r0 = traits::cmpeq(x0, (q));															\
		auto r1 = traits::cmpeq(x1, (q));															\
		auto r2 = traits::cmpeq(x2, (q));															\
		auto r3 = traits::cmpeq(x3, (q));															\
																									\
		auto Z = traits::movemask_epi8(traits::Or(traits::Or(r0, r1), traits::Or(r2, r3)));			\
		if(Z){																						\
			auto z0 = traits::movemask_epi8(r0);													\
			if(z0){																					\
				return (s) + wjr::countr_zero(static_cast<value_type>(z0)) / _Mycor;				\
			}																						\
			auto z1 = traits::movemask_epi8(r1);													\
			if(z1){																					\
				return (s) + _Mysize + wjr::countr_zero(static_cast<value_type>(z1)) / _Mycor;		\
			}																						\
			auto z2 = traits::movemask_epi8(r2);													\
			if(z2){																					\
				return (s) + _Mysize * 2 + wjr::countr_zero(static_cast<value_type>(z2)) / _Mycor;	\
			}																						\
			auto z3 = traits::movemask_epi8(r3);													\
			if(z3){																					\
				return (s) + _Mysize * 3 + wjr::countr_zero(static_cast<value_type>(z3)) / _Mycor;	\
			}																						\
		}																							\
	}

template<simd_type S, typename T>
const T* __memchr(const T* s, T val, size_t n) {
	using traits = traits<T, S>;
	using simd_type = typename traits::simd_type;
	using value_type = std::conditional_t<simd::simd_type::AVX == S, uint32_t, uint16_t>;
	constexpr int _Mysize = traits::size();
	constexpr int _Mycor = sizeof(T) / sizeof(uint8_t);

	auto q = traits::set1(val);

	if (n < _Mysize) {
		auto ns = n * sizeof(T);
		auto ql = ns >> 1;
		auto sl = ql << 1;
		if (ql) {
			auto x = traits::preload_si16x(s, ql);
			auto r = traits::cmpeq(x, q);
			auto z = traits::movemask_epi8(r) & ((1u << sl) - 1);
			if (z) {
				return s + wjr::countr_zero(static_cast<value_type>(z)) / _Mycor;
			}
			s += sl / sizeof(T);
		}
		if constexpr (sizeof(T) == 1) {
			if (ns & 1) {
				if (*s == val) {
					return s;
				}
				++s;
			}
		}
		return s;
	}

	{
		__WJR_ONE_MEMCHR(s, q);
	}

	// [_Mysize, _Mysize * 2]
	if (n <= _Mysize * 2) {
		s += n;
		if (n != _Mysize) {
			__WJR_ONE_MEMCHR(s - _Mysize, q);
		}
		return s;
	}

	{
		__WJR_ONE_MEMCHR(s + _Mysize, q);
	}

	// (_Mysize * 2, _Mysize * 4]
	if (n <= _Mysize * 4) {
		s += n;
		__WJR_ONE_MEMCHR(s - _Mysize * 2, q);
		__WJR_ONE_MEMCHR(s - _Mysize, q);
		return s;
	}

	{
		__WJR_ONE_MEMCHR(s + _Mysize * 2, q);
		__WJR_ONE_MEMCHR(s + _Mysize * 3, q);
	}

	if (n <= _Mysize * 8) {
		s += n;
		__WJR_FOUR_MEMCHR(s - _Mysize * 4, q);
		return s;
	}

	__WJR_FOUR_MEMCHR(s + 4 * _Mysize, q);

	s += 8 * _Mysize;
	n -= 8 * _Mysize;

	while (n >= _Mysize * 4) {
		__WJR_FOUR_MEMCHR(s, q);
		s += _Mysize * 4;
		n -= _Mysize * 4;
	}
	if (n) {
		s += n;
		__WJR_FOUR_MEMCHR(s - _Mysize * 4, q);
	}
	return s;
}

#undef __WJR_ONE_MEMCHR
#undef __WJR_FOUR_MEMCHR

_WJR_SIMD_END

#endif // __AVX2__ || __SSE2__

#endif // WJR_SIMD___MEMCHR_H