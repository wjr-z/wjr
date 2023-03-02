#pragma once
#ifndef __WJR_ALGO_MEMCNT_H
#define __WJR_ALGO_MEMCNT_H

#include <wjr/literals.h>
#include <wjr/simd/simd_intrin.h>

#if defined(__HAS_FAST_MEMCNT)

#define __WJR_MEMCNT_SMALL_INIT_BEGIN(...)	size_t cnt = 0;
#define __WJR_MEMCNT_SMALL_INIT_END(...) return cnt;

#define __WJR_MEMCNT_SMALL_FUNC(s, val, i, n)	                                \
	{	                                                                        \
		if(n <= 4){	                                                            \
			for(size_t j = 0;j < n;++j){	                                    \
				cnt += (s[i + j] == val);	                                    \
			}	                                                                \
		}else{	                                                                \
			auto q = traits::set1(val, T());	                                \
			auto ns = n * sizeof(T);	                                        \
			auto ql = ns >> 1;	                                                \
			auto sl = ql << 1;	                                                \
			auto x = traits::preloadu_si16x(s + i, ql);	                        \
			auto r = traits::cmpeq(x, q, T());	                                \
			auto z = traits::movemask_epi8(r) & ((1u << sl) - 1);	            \
			cnt += wjr::popcount(static_cast<value_type>(z)) / _Mycor;	        \
			if constexpr(sizeof(T) == 1){	                                    \
				if(ns & 1){	                                                    \
					cnt += (s[i + n - 1] == val);	                            \
				}	                                                            \
			}	                                                                \
		}	                                                                    \
	}

#define __WJR_MEMCNT_MID_INIT_BEGIN(...)	                                    \
	auto c = traits::zeros();	                                                \
	auto step = _Maxstep;	                                                    \
	auto q = traits::set1(val, T());

#define __WJR_MEMCNT_MID_INIT_END(s, val)	cnt += traits::add(c, T());

#define __WJR_MEMCNT_MID_FUNC(s, val, i)	                                    \
	{	                                                                        \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>(s + i));	    \
		auto r = traits::cmpeq(x, q, T{});	                                    \
		c = traits::sub(c, r, T{});				                                \
	}

#define __WJR_MEMCNT_LARGE_FUNC(s, val, i)		                                \
	{	                                                                        \
		__WJR_MEMCNT_MID_FUNC(s, val, i)	                                    \
		__WJR_MEMCNT_MID_FUNC(s, val, (i + _Mysize * 1))	                    \
		__WJR_MEMCNT_MID_FUNC(s, val, (i + _Mysize * 2))	                    \
		__WJR_MEMCNT_MID_FUNC(s, val, (i + _Mysize * 3))	                    \
		--step;																	\
		if (!step) {	                                                        \
			cnt += traits::add(c, T{});	                                        \
			c = traits::zeros();	                                            \
			step = _Maxstep;	                                                \
		}	                                                                    \
	}

_WJR_ALGO_BEGIN

template<typename T>
size_t __memcnt(const T* s, T val, size_t n) {
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
	// Prevent overflow
	constexpr auto _Maxstep_four = std::numeric_limits<T>::max() / _Mysize + 1;
	static_assert(_Maxstep_four >= 4 && _Maxstep_four % 4 == 0, "invalid _Maxstep");
	constexpr auto _Maxstep = _Maxstep_four / 4;

	using namespace enum_ops;
	constexpr auto tag =
		GetSmallThreshold(_Mysize) |
		GetVectorizedLength(_Mysize);

	WJR_AUTO_VECTORIZE(tag, n,
		__WJR_MEMCNT_SMALL_FUNC, __WJR_MEMCNT_MID_FUNC, __WJR_MEMCNT_LARGE_FUNC,
		__WJR_MEMCNT_SMALL_INIT_BEGIN, __WJR_MEMCNT_SMALL_INIT_END,
		__WJR_MEMCNT_MID_INIT_BEGIN, __WJR_MEMCNT_MID_INIT_END,
		s, val);
}

_WJR_ALGO_END

#endif // __HAS_FAST_MEMCNT

#endif // __WJR_ALGO_MEMCNT_H