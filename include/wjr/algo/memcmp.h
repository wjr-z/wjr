#pragma once
#ifndef __WJR_ALGO_MEMCMP_H
#define __WJR_ALGO_MEMCMP_H

#include <wjr/algo/macro.h>
#if defined(__HAS_FAST_MEMCMP)

#define __WJR_MEMCMP_FUNC_ONE_HELPER__	                                                        \
	auto r = traits::cmp(x, y, pred, T());	                                                    \
	auto z = traits::movemask_epi8(r);	                                                        \
	if(z != __Mask){	                                                                        \
		return false;	                                                                        \
	}

#define __WJR_MEMCMP_FUNC_FOUR_HELPER__	                                                        \
	auto r0 = traits::cmp(x0, x4, pred, T());												    \
	auto r1 = traits::cmp(x1, x5, pred, T());									                \
	auto r2 = traits::cmp(x2, x6, pred, T());									                \
	auto r3 = traits::cmp(x3, x7, pred, T());									                \
																						        \
	auto z = traits::movemask_epi8(														        \
		traits::And(traits::And(r0, r1), traits::And(r2, r3)));							        \
																						        \
	if(z != __Mask){																	        \
		return false;																	        \
	}																					        \

#if defined(__SSE4_1__)
#define __WJR_MEMCMP_FUNC_ONE__SSE4_1__HELPER__	                                                \
	auto r = traits::Xor(x, y);	                                                                \
	auto z = traits::test_all_zeros(r);	                                                        \
	if(!z){	                                                                                    \
		return false;	                                                                        \
	}

#define __WJR_MEMCMP_FUNC_FOUR__SSE4_1__HELPER__	                                            \
	auto r0 = traits::Xor(x0, x4);												                \
	auto r1 = traits::Xor(x1, x5);									                            \
	auto r2 = traits::Xor(x2, x6);									                            \
	auto r3 = traits::Xor(x3, x7);									                            \
																							    \
	auto z = traits::test_all_zeros(														    \
		traits::Or(traits::Or(r0, r1), traits::Or(r2, r3)));							        \
																							    \
	if(!z){																	                    \
		return false;																	        \
	}																					

#define __WJR_MEMCMP_FUNC_ONE_HELPER	                                                        \
	if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                                    \
		__WJR_MEMCMP_FUNC_ONE__SSE4_1__HELPER__	                                                \
	}	                                                                                        \
	else {	                                                                                    \
		__WJR_MEMCMP_FUNC_ONE_HELPER__	                                                        \
	}

#define __WJR_MEMCMP_FUNC_FOUR_HELPER	                                                        \
	if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                                    \
		__WJR_MEMCMP_FUNC_FOUR__SSE4_1__HELPER__	                                            \
	}	                                                                                        \
	else {	                                                                                    \
		__WJR_MEMCMP_FUNC_FOUR_HELPER__	                                                        \
	}

#else
#define __WJR_MEMCMP_FUNC_ONE_HELPER	__WJR_MEMCMP_FUNC_ONE_HELPER__
#define __WJR_MEMCMP_FUNC_FOUR_HELPER	__WJR_MEMCMP_FUNC_FOUR_HELPER__
#endif

#define __WJR_MEMCMP_SMALL_INIT_END(...) return true;

#define __WJR_MEMCMP_SMALL_FUNC(s0, s1, i, n)	                                                \
	{	                                                                                        \
		if(n <= 4){	                                                                            \
			for(size_t j = 0;j < n;++j){	                                                    \
				if(!pred(s0[i + j], s1[i + j])){	                                            \
					return false;	                                                            \
				}	                                                                            \
			}	                                                                                \
		}	                                                                                    \
		else{	                                                                                \
			auto ns = n * sizeof(T);	                                                        \
			auto ql = ns >> 1;	                                                                \
			auto sl = ql << 1;	                                                                \
			auto x = traits::preloadu_si16x(s0 + i, ql);	                                        \
			auto y = traits::preloadu_si16x(s1 + i, ql);	                                        \
			auto r = traits::cmp(x, y, pred, T());	                                            \
			auto z = traits::movemask_epi8(r) | (__Mask ^ ((1u << sl) - 1));	                \
			                                                                                    \
			if(z != __Mask){	                                                                \
				return false;	                                                                \
			}	                                                                                \
			if constexpr(sizeof(T) == 1){	                                                    \
				if(ns & 1){	                                                                    \
					if(!pred(s0[n - 1], s1[n - 1])){	                                        \
						return false;	                                                        \
					}	                                                                        \
				}	                                                                            \
			}	                                                                                \
		}	                                                                                    \
	}

#define __WJR_MEMCMP_MID_FUNC(s0, s1, i)													    \
	{																			                \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>(s0 + i));		                \
		auto y = traits::loadu(reinterpret_cast<const simd_type*>(s1 + i));		                \
		__WJR_MEMCMP_FUNC_ONE_HELPER	                                                        \
	}

#define __WJR_MEMCMP_LARGE_FUNC(s0, s1, i)													    \
	{																						    \
		auto x0 = traits::loadu(reinterpret_cast<const simd_type*>(s0 + i));					\
		auto x1 = traits::loadu(reinterpret_cast<const simd_type*>(s0 + i + _Mysize * 1));	    \
		auto x2 = traits::loadu(reinterpret_cast<const simd_type*>(s0 + i + _Mysize * 2));	    \
		auto x3 = traits::loadu(reinterpret_cast<const simd_type*>(s0 + i + _Mysize * 3));	    \
																							    \
		auto x4 = traits::loadu(reinterpret_cast<const simd_type*>(s1 + i));					\
		auto x5 = traits::loadu(reinterpret_cast<const simd_type*>(s1 + i + _Mysize * 1));	    \
		auto x6 = traits::loadu(reinterpret_cast<const simd_type*>(s1 + i + _Mysize * 2));	    \
		auto x7 = traits::loadu(reinterpret_cast<const simd_type*>(s1 + i + _Mysize * 3));	    \
																							    \
		__WJR_MEMCMP_FUNC_FOUR_HELPER	                                                        \
	}

_WJR_ALGO_BEGIN

template<typename T, typename _Pred>
bool __memcmp(const T* s0, const T* s1, size_t n, _Pred pred) {
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
	constexpr int __Mask = is_avx ? 0xffffffff : 0xffff;

	using namespace enum_ops;
	constexpr auto tag =
		vectorize_t::CanOverlap |
		GetMidLargeSpeed(1) |
		GetSmallThreshold(_Mysize) |
		GetVectorizedLength(_Mysize);

	WJR_AUTO_VECTORIZE(tag, n,
		__WJR_MEMCMP_SMALL_FUNC, __WJR_MEMCMP_MID_FUNC, __WJR_MEMCMP_LARGE_FUNC,
		WJR_MACRO_NULL, __WJR_MEMCMP_SMALL_INIT_END,
		WJR_MACRO_NULL, WJR_MACRO_NULL,
		s0, s1);
}

_WJR_ALGO_END

#endif // __AVX2__ || __SSE2__


#endif // __WJR_ALGO_MEMCMP_H