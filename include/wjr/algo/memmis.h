#pragma once
#ifndef __WJR_ALGO_MEMMIS_H
#define __WJR_ALGO_MEMMIS_H

#include <wjr/algo/macro.h>
#if defined(__HAS_FAST_MEMMIS)

#define __WJR_MEMMIS_SMALL_INIT_END(s0, s1, i)	return s0 + i;

#define __WJR_MEMMIS_SMALL_FUNC(s0, s1, i, n)	                                                            \
	{	                                                                                                    \
		if(n <= 4){	                                                                                        \
			for(size_t j = 0;j < n;++j){	                                                                \
				if(!pred(s0[i + j], s1[i + j])){	                                                        \
					return s0 + i + j;	                                                                    \
				}	                                                                                        \
			}	                                                                                            \
		}else{	                                                                                            \
			auto ns = n * sizeof(T);	                                                                    \
			auto ql = ns >> 1;	                                                                            \
			auto sl = ql << 1;	                                                                            \
			auto x0 = traits::preloadu_si16x(s0 + i, ql);	                                                \
			auto x1 = traits::preloadu_si16x(s1 + i, ql);	                                                \
			auto r = traits::cmp(x0, x1, pred, T{});	                                                    \
			auto z = traits::movemask_epi8(r) | (__Mask ^ ((1u << sl) - 1));	                            \
			if (z != __Mask) {	                                                                            \
				return s0 + i + wjr::countr_one(static_cast<value_type>(z)) / _Mycor;	                    \
			}	                                                                                            \
			if constexpr (sizeof(T) == 1) {	                                                                \
				if (ns & 1) {	                                                                            \
					if (!pred(s0[i + n - 1], s1[i + n - 1])) {	                                            \
						return s0 + i + n - 1;	                                                            \
					}	                                                                                    \
				}	                                                                                        \
			}	                                                                                            \
		}	                                                                                                \
	}

#define __WJR_MEMMIS_MID_FUNC(s0, s1, i)															        \
	{																			                            \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>(s0 + i));			                        \
		auto y = traits::loadu(reinterpret_cast<const simd_type*>(s1 + i));			                        \
		auto r = traits::cmp(x, y, pred, T{});											                    \
		auto z = traits::movemask_epi8(r);										                            \
		if (z != __Mask) {														                            \
			return s0 + i + wjr::countr_one(static_cast<value_type>(z)) / _Mycor;	                        \
		}																		                            \
	}

#define __WJR_MEMMIS_LARGE_FUNC(s0, s1, i)															        \
	{																					                    \
		auto x0 = traits::loadu(reinterpret_cast<const simd_type*>(s0 + i));					            \
		auto x1 = traits::loadu(reinterpret_cast<const simd_type*>(s0 + i + _Mysize * 1));	                \
		auto x2 = traits::loadu(reinterpret_cast<const simd_type*>(s0 + i + _Mysize * 2));	                \
		auto x3 = traits::loadu(reinterpret_cast<const simd_type*>(s0 + i + _Mysize * 3));	                \
																						                    \
		auto x4 = traits::loadu(reinterpret_cast<const simd_type*>(s1 + i));					            \
		auto x5 = traits::loadu(reinterpret_cast<const simd_type*>(s1 + i + _Mysize * 1));	                \
		auto x6 = traits::loadu(reinterpret_cast<const simd_type*>(s1 + i + _Mysize * 2));	                \
		auto x7 = traits::loadu(reinterpret_cast<const simd_type*>(s1 + i + _Mysize * 3));	                \
																						                    \
		auto r0 = traits::cmp(x0, x4, pred, T{});											                \
		auto r1 = traits::cmp(x1, x5, pred, T{});											                \
		auto r2 = traits::cmp(x2, x6, pred, T{});											                \
		auto r3 = traits::cmp(x3, x7, pred, T{});											                \
																							                \
		auto Z = traits::movemask_epi8(traits::And(traits::And(r0, r1), traits::And(r2, r3)));		        \
																							                \
		if(Z != __Mask){																	                \
			auto z = traits::movemask_epi8(r0);											                    \
			if(z != __Mask){																                \
				return s0 + i + wjr::countr_one(static_cast<value_type>(z)) / _Mycor;				        \
			}																				                \
			z = traits::movemask_epi8(r1);											                        \
			if(z != __Mask){																                \
				return s0 + i + _Mysize * 1 + wjr::countr_one(static_cast<value_type>(z)) / _Mycor;	        \
			}																				                \
			z = traits::movemask_epi8(r2);											                        \
			if(z != __Mask){																                \
				return s0 + i + _Mysize * 2 + wjr::countr_one(static_cast<value_type>(z)) / _Mycor;	        \
			}																				                \
			z = Z;											                                                \
			return s0 + i + _Mysize * 3 + wjr::countr_one(static_cast<value_type>(z)) / _Mycor;	            \
		}																					                \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
const T* __memmis(const T* s0, const T* s1, size_t n, _Pred pred) {
	constexpr bool is_avx =
#if defined(__AVX2__)
		true;
#else
		false;
#endif
	using traits = std::conditional_t<is_avx, simd::avx, simd::sse>;
	using simd_type = typename traits::int_type;
	using value_type = typename traits::mask_type;
	constexpr int _Mysize = traits::width() / (8 * sizeof(T));
	constexpr int _Mycor = sizeof(T) / sizeof(uint8_t);
	constexpr int __Mask = is_avx ? 0xffffffff : 0x0000ffff;

	using namespace enum_ops;
	constexpr auto tag =
		vectorize_t::CanOverlap |
		GetMidLargeSpeed(2) |
		GetSmallThreshold(_Mysize) |
		GetVectorizedLength(_Mysize);

	WJR_AUTO_VECTORIZE(tag, n,
		__WJR_MEMMIS_SMALL_FUNC, __WJR_MEMMIS_MID_FUNC, __WJR_MEMMIS_LARGE_FUNC,
		WJR_MACRO_NULL, __WJR_MEMMIS_SMALL_INIT_END,
		WJR_MACRO_NULL, WJR_MACRO_NULL,
		s0, s1);
	
}

_WJR_ALGO_END

#undef __REGISTER_MEMMIS_FUNC_FOUR
#undef __REGISTER_MEMMIS_FUNC_ONE

#endif // __HAS_FAST_MEMMIS

#endif // __WJR_ALGO_MEMMIS_H