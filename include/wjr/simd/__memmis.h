#pragma once
#ifndef __WJR_SIMD___MEMMIS_H
#define __WJR_SIMD___MEMMIS_H

#include <wjr/macro.h>

#if defined(__AVX2__) || defined(__SSE2__)
#include <wjr/simd/simd_helper.h>

#define __REGISTER_MEMMIS_FUNC_ONE(s0, s1)															\
	{																			                    \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>((s0)));			                \
		auto y = traits::loadu(reinterpret_cast<const simd_type*>((s1)));			                \
		auto r = traits::cmp(x, y, pred);											                \
		auto z = traits::movemask_epi8(r);										                    \
		if (z != __Mask) {														                    \
			return (s0) + wjr::countr_one(static_cast<value_type>(z)) / _Mycor;	                    \
		}																		                    \
	}

#define __REGISTER_MEMMIS_FUNC_FOUR(s0, s1)															\
	{																					            \
		auto x0 = traits::loadu(reinterpret_cast<const simd_type*>((s0)));					        \
		auto x1 = traits::loadu(reinterpret_cast<const simd_type*>((s0) + _Mysize * 1));	        \
		auto x2 = traits::loadu(reinterpret_cast<const simd_type*>((s0) + _Mysize * 2));	        \
		auto x3 = traits::loadu(reinterpret_cast<const simd_type*>((s0) + _Mysize * 3));	        \
																						            \
		auto x4 = traits::loadu(reinterpret_cast<const simd_type*>((s1)));					        \
		auto x5 = traits::loadu(reinterpret_cast<const simd_type*>((s1) + _Mysize * 1));	        \
		auto x6 = traits::loadu(reinterpret_cast<const simd_type*>((s1) + _Mysize * 2));	        \
		auto x7 = traits::loadu(reinterpret_cast<const simd_type*>((s1) + _Mysize * 3));	        \
																						            \
		auto r0 = traits::cmp(x0, x4, pred);											            \
		auto r1 = traits::cmp(x1, x5, pred);											            \
		auto r2 = traits::cmp(x2, x6, pred);											            \
		auto r3 = traits::cmp(x3, x7, pred);											            \
																							        \
		auto Z = traits::movemask_epi8(traits::And(traits::And(r0, r1), traits::And(r2, r3)));		\
																							        \
		if(Z != __Mask){																	        \
			auto z0 = traits::movemask_epi8(r0);											        \
			if(z0 != __Mask){																        \
				return (s0) + wjr::countr_one(static_cast<value_type>(z0)) / _Mycor;				\
			}																				        \
			auto z1 = traits::movemask_epi8(r1);											        \
			if(z1 != __Mask){																        \
				return (s0) + _Mysize * 1 + wjr::countr_one(static_cast<value_type>(z1)) / _Mycor;	\
			}																				        \
			auto z2 = traits::movemask_epi8(r2);											        \
			if(z2 != __Mask){																        \
				return (s0) + _Mysize * 2 + wjr::countr_one(static_cast<value_type>(z2)) / _Mycor;	\
			}																				        \
			auto z3 = traits::movemask_epi8(r3);											        \
			if(z3 != __Mask){																        \
				return (s0) + _Mysize * 3 + wjr::countr_one(static_cast<value_type>(z3)) / _Mycor;	\
			}																				        \
		}																					        \
	}

#define __REGISTER_MEMRMIS_FUNC_ONE(s0, s1)															\
	{																			                    \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>((s0)));			                \
		auto y = traits::loadu(reinterpret_cast<const simd_type*>((s1)));			                \
		auto r = traits::cmp(x, y, pred);											                \
		auto z = traits::movemask_epi8(r);										                    \
		if (z != __Mask) {														                    \
			return (s0) + _Mysize - wjr::countl_one(static_cast<value_type>(z)) / _Mycor;	        \
		}																		                    \
	}

#define __REGISTER_MEMRMIS_FUNC_FOUR(s0, s1)															\
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
		auto r0 = traits::cmp(x0, x4, pred);											                \
		auto r1 = traits::cmp(x1, x5, pred);											                \
		auto r2 = traits::cmp(x2, x6, pred);											                \
		auto r3 = traits::cmp(x3, x7, pred);											                \
																							            \
		auto Z = traits::movemask_epi8(traits::And(traits::And(r0, r1), traits::And(r2, r3)));			\
																							            \
		if(Z != __Mask){																	            \
			auto z3 = traits::movemask_epi8(r3);											            \
			if(z3 != __Mask){																            \
				return (s0) + _Mysize * 4 - wjr::countl_one(static_cast<value_type>(z3)) / _Mycor;	    \
			}																				            \
			auto z2 = traits::movemask_epi8(r2);											            \
			if(z2 != __Mask){																            \
				return (s0) + _Mysize * 3 - wjr::countl_one(static_cast<value_type>(z2)) / _Mycor;	    \
			}																				            \
			auto z1 = traits::movemask_epi8(r1);											            \
			if(z1 != __Mask){																            \
				return (s0) + _Mysize * 2 - wjr::countl_one(static_cast<value_type>(z1)) / _Mycor;	    \
			}																				            \
			auto z0 = traits::movemask_epi8(r0);											            \
			if(z0 != __Mask){																            \
				return (s0) + _Mysize * 1 - wjr::countl_one(static_cast<value_type>(z0)) / _Mycor;	    \
			}																				            \
		}																					            \
	}

_WJR_SIMD_BEGIN

template<simd_type S, typename T, typename _Pred>
const T* __memmis(const T* s0, const T* s1, size_t n, _Pred pred) {
	using traits = traits<T, S>;
	using simd_type = typename traits::simd_type;
	using value_type = std::conditional_t<simd::simd_type::AVX == S, uint32_t, uint16_t>;

	constexpr int _Mysize = traits::size();
	constexpr int _Mycor = sizeof(T) / sizeof(uint8_t);
	constexpr int __Mask = simd::simd_type::AVX == S ? 0xffffffff : 0x0000ffff;

	if (n < _Mysize) {
		auto ns = n * sizeof(T);
		auto ql = ns >> 1;
		auto sl = ql << 1;
		if (ql) {
			auto x0 = traits::preload_si16x(s0, ql);
			auto x1 = traits::preload_si16x(s1, ql);
			auto r = traits::cmp(x0, x1, pred);
			auto z = traits::movemask_epi8(r) | (__Mask ^ ((1u << sl) - 1));

			if (z != __Mask) {
				return s0 + wjr::countr_one(static_cast<value_type>(z)) / _Mycor;
			}
			s0 += sl / sizeof(T);
			s1 += sl / sizeof(T);
		}

		if constexpr (sizeof(T) == 1) {
			if (ns & 1) {
				if (!pred(*s0, *s1)) {
					return s0;
				}
				++s0;
			}
		}
		return s0;
	}

	{
		__REGISTER_MEMMIS_FUNC_ONE(s0, s1)
	}

	if (n <= _Mysize * 2) {
		s0 += n;
		if (n != _Mysize) {
			s1 += n;
			__REGISTER_MEMMIS_FUNC_ONE(s0 - _Mysize, s1 - _Mysize);
		}
		return s0;
	}

	{
		__REGISTER_MEMMIS_FUNC_ONE(s0 + _Mysize, s1 + _Mysize);
	}

	if (n <= _Mysize * 4) {
		s0 += n;
		s1 += n;
		__REGISTER_MEMMIS_FUNC_ONE(s0 - _Mysize * 2, s1 - _Mysize * 2);
		__REGISTER_MEMMIS_FUNC_ONE(s0 - _Mysize, s1 - _Mysize);
		return s0;
	}

	{
		__REGISTER_MEMMIS_FUNC_ONE(s0 + _Mysize * 2, s1 + _Mysize * 2);
		__REGISTER_MEMMIS_FUNC_ONE(s0 + _Mysize * 3, s1 + _Mysize * 3);
	}

	if (n <= _Mysize * 8) {
		s0 += n;
		s1 += n;
		__REGISTER_MEMMIS_FUNC_FOUR(s0 - _Mysize * 4, s1 - _Mysize * 4);
		return s0;
	}

	__REGISTER_MEMMIS_FUNC_FOUR(s0 + _Mysize * 4, s1 + _Mysize * 4);

	s0 += _Mysize * 8;
	s1 += _Mysize * 8;
	n -= _Mysize * 8;

	while (n >= _Mysize * 4) {
		__REGISTER_MEMMIS_FUNC_FOUR(s0, s1);
		s0 += _Mysize * 4;
		s1 += _Mysize * 4;
		n -= _Mysize * 4;
	}

	if (n) {
		s0 += n;
		s1 += n;
		__REGISTER_MEMMIS_FUNC_FOUR(s0 - _Mysize * 4, s1 - _Mysize * 4);
	}

	return s0;
}

template<simd_type S, typename T, typename _Pred>
const T* __memrmis(const T* s0, const T* s1, size_t n, _Pred pred) {
	using traits = traits<T, S>;
	using simd_type = typename traits::simd_type;
	using value_type = std::conditional_t<simd::simd_type::AVX == S, uint32_t, uint16_t>;

	constexpr int _Mysize = traits::size();
	constexpr int _Mycor = sizeof(T) / sizeof(uint8_t);
	constexpr int __Mask = simd::simd_type::AVX == S ? 0xffffffff : 0x0000ffff;

	s0 += n;
	s1 += n;

	if (n < _Mysize) {
		auto ns = n * sizeof(T);
		auto ql = ns >> 1;
		auto sl = ql << 1;
		if (ql) {
			s0 -= sl / sizeof(T);
			s1 -= sl / sizeof(T);
			auto x0 = traits::preload_si16x(s0, ql);
			auto x1 = traits::preload_si16x(s1, ql);
			auto r = traits::cmp(x0, x1, pred);
			auto z = traits::movemask_epi8(r) | (__Mask ^ ((1u << sl) - 1));

			if (z != __Mask) {
				return s0 + _Mysize - wjr::countl_one(static_cast<value_type>(z)) / _Mycor;
			}
		}

		if constexpr (sizeof(T) == 1) {
			if (ns & 1) {
				--s0;
				--s1;
				if (!pred(*s0, *s1)) {
					return 1 + s0;
				}
			}
		}
		return s0;

	}

	{
		__REGISTER_MEMRMIS_FUNC_ONE(s0 - _Mysize, s1 - _Mysize);
	}

	if (n <= _Mysize * 2) {
		s0 -= n;
		if (n != _Mysize) {
			s1 -= n;
			__REGISTER_MEMRMIS_FUNC_ONE(s0, s1);
		}
		return s0;
	}

	{
		__REGISTER_MEMRMIS_FUNC_ONE(s0 - _Mysize * 2, s1 - _Mysize * 2);
	}

	if (n <= _Mysize * 4) {
		s0 -= n;
		s1 -= n;
		__REGISTER_MEMRMIS_FUNC_ONE(s0 + _Mysize, s1 + _Mysize);
		__REGISTER_MEMRMIS_FUNC_ONE(s0, s1);
		return s0;
	}

	{
		__REGISTER_MEMRMIS_FUNC_ONE(s0 - _Mysize * 3, s1 - _Mysize * 3);
		__REGISTER_MEMRMIS_FUNC_ONE(s0 - _Mysize * 4, s1 - _Mysize * 4);
	}

	if (n <= _Mysize * 8) {
		s0 -= n;
		s1 -= n;
		__REGISTER_MEMRMIS_FUNC_FOUR(s0, s1);
		return s0;
	}

	s0 -= _Mysize * 8;
	s1 -= _Mysize * 8;
	__REGISTER_MEMRMIS_FUNC_FOUR(s0, s1);

	n -= _Mysize * 8;

	while (n >= _Mysize * 4) {
		s0 -= _Mysize * 4;
		s1 -= _Mysize * 4;
		__REGISTER_MEMRMIS_FUNC_FOUR(s0, s1);
		n -= _Mysize * 4;
	}

	if (n) {
		s0 -= n;
		s1 -= n;
		__REGISTER_MEMRMIS_FUNC_FOUR(s0, s1);
	}

	return s0;
}


_WJR_SIMD_END

#undef __REGISTER_MEMMIS_FUNC
#undef __REGISTER_MEMMIS_FUNC_FOUR
#undef __REGISTER_MEMMIS_FUNC_ONE
#undef __REGISTER_MEMMIS_FUNC_CMP

#undef __REGISTER_MEMRMIS_FUNC
#undef __REGISTER_MEMRMIS_FUNC_FOUR
#undef __REGISTER_MEMRMIS_FUNC_ONE
#undef __REGISTER_MEMRMIS_FUNC_CMP

#endif // __AVX2__ || __SSE2__

#endif // __WJR_SIMD___MEMMIS_H