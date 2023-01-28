#pragma once
#ifndef __WJR_SIMD___MEMCNT_H
#define __WJR_SIMD___MEMCNT_H

#include <wjr/macro.h>

#if defined(__AVX2__) || defined(__SSE2__)
#include <wjr/simd/simd_intrin.h>

#define __REGISTER_MEMCNT_FUNC_ONE(s)	                                \
	{	                                                                \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>(s));	\
		auto r = traits::cmpeq(x, y, T{});	                            \
		c = traits::sub(c, r, T{});				                            \
	}

#define __REGISTER_MEMCNT_FUNC_FOUR(s)		                            \
	{	                                                                \
		__REGISTER_MEMCNT_FUNC_ONE(s)	                                \
		__REGISTER_MEMCNT_FUNC_ONE(s + _Mysize * 1)	                    \
		__REGISTER_MEMCNT_FUNC_ONE(s + _Mysize * 2)	                    \
		__REGISTER_MEMCNT_FUNC_ONE(s + _Mysize * 3)	                    \
	}

_WJR_SIMD_BEGIN

template<typename T>
size_t __memcnt(const T* s, T val, size_t n) {
	constexpr bool is_avx =
#if defined(__AVX2__)
		true;
#else
		false;
#endif
	using traits = std::conditional_t<is_avx, avx, sse>;
	using simd_type = std::conditional_t<is_avx, __m256i, __m128i>;
	using value_type = std::conditional_t<is_avx, uint32_t, uint16_t>;
	constexpr int _Mysize = traits::width() / (8 * sizeof(T));
	constexpr int _Mycor = sizeof(T) / sizeof(uint8_t);
	// Prevent overflow
	constexpr auto _Maxstep_four = std::numeric_limits<T>::max() / _Mysize + 1;
	static_assert(_Maxstep_four >= 4 && _Maxstep_four % 4 == 0 , "invalid _Maxstep");
	constexpr auto _Maxstep = _Maxstep_four / 4;

	if (n == 0) { return 0; }

	size_t cnt = 0;
	auto y = traits::set1(val, T{});
	
	if (n >= _Mysize) {
		auto c = traits::set_all_zeros();
		auto step = _Maxstep;

		constexpr auto __mask = _Mysize * 4 - 1;
		auto __s = s + (n & ~__mask);
		n &= __mask;
		while (s != __s) {
			__REGISTER_MEMCNT_FUNC_FOUR(s);
			s += _Mysize * 4;
			--step;
			if (!step) {
				cnt += traits::add(c, T{});
				c = traits::set_all_zeros();
				step = _Maxstep;
			}
		}
		switch (n / _Mysize) {
		case 3: __REGISTER_MEMCNT_FUNC_ONE(s); s += _Mysize; n -= _Mysize;
		case 2: __REGISTER_MEMCNT_FUNC_ONE(s); s += _Mysize; n -= _Mysize;
		case 1: __REGISTER_MEMCNT_FUNC_ONE(s); s += _Mysize; n -= _Mysize;
		default:cnt += traits::add(c, T{});
		}
	}

	auto ns = n * sizeof(T);
	auto ql = ns >> 1;
	auto sl = ql << 1;
	if (ql) {
		auto x = traits::preload_si16x(s, ql);
		auto r = traits::cmpeq(x, y, T{});
		auto z = traits::movemask_epi8(r) & ((1u << sl) - 1);
		cnt += wjr::popcount(static_cast<value_type>(z)) / _Mycor;
		s += sl / sizeof(T);
	}
	if constexpr (sizeof(T) == 1) {
		if (ns & 1) {
			cnt += *s == val;
		}
	}
	
	return cnt;
}

_WJR_SIMD_END

#endif // __AVX2__ || __SSE2__

#endif // __WJR_SIMD___MEMCNT_H