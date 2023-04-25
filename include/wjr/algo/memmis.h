#ifndef __WJR_ALGO_MEM_ALL_H
#error "This file should not be included directly. Include <wjr/algo/mem-all.h> instead."
#endif // __WJR_ALGO_MEM_ALL_H

#if defined(_WJR_FAST_MEMMIS)

#define __WJR_MEMMIS_ONE(st, _s)																	\
	{	                                                                                            \
		auto r = st::cmp(x, y, pred, T());	                                                        \
		st::mask_type z = st::movemask_epi8(r);		                                                \
		if(WJR_LIKELY(z != st::mask())) {	                                                        \
			return WJR_SIMD_FIRST_ZERO_PTR(_s, z);													\
		}	                                                                                        \
	}

#define __WJR_MEMMIS_FOUR(st, _s0, _s1, _s2, _s3)													\
	{	                                                                                            \
		auto r0 = st::cmp(x0, y0, pred, T());	                                                    \
		auto r1 = st::cmp(x1, y1, pred, T());	                                                    \
		auto r2 = st::cmp(x2, y2, pred, T());	                                                    \
		auto r3 = st::cmp(x3, y3, pred, T());	                                                    \
																							        \
		r3 = st::And(st::And(r0, r1), st::And(r2, r3));	                                            \
		st::mask_type z = st::movemask_epi8(r3);		                                            \
		if(WJR_UNLIKELY(z != st::mask())) {	                                                        \
			st::mask_type tmp = st::movemask_epi8(r0);	                                            \
			if(tmp != st::mask()){	                                                                \
				return WJR_SIMD_FIRST_ZERO_PTR(_s0, tmp);	                                        \
			}	                                                                                    \
			tmp = st::movemask_epi8(r1);	                                                        \
			if(tmp != st::mask()){	                                                                \
				return WJR_SIMD_FIRST_ZERO_PTR(_s1, tmp);	                                        \
			}	                                                                                    \
			tmp = st::movemask_epi8(r2);	                                                        \
			if(tmp != st::mask()){	                                                                \
				return WJR_SIMD_FIRST_ZERO_PTR(_s2, tmp);	                                        \
			}	                                                                                    \
			tmp = z;	                                                                            \
			return WJR_SIMD_FIRST_ZERO_PTR(_s3, tmp);	                                            \
		}	                                                                                        \
	}

#define WJR_SIMD_IS_BACKWARD 0
#define __WJR_MEMMIS_NAME __memmis
#include <wjr/algo/memmis-impl.h>
#undef __WJR_MEMMIS_NAME
#undef WJR_SIMD_IS_BACKWARD

#define WJR_SIMD_IS_BACKWARD 1
#define __WJR_MEMMIS_NAME __memrmis
#include <wjr/algo/memmis-impl.h>
#undef __WJR_MEMMIS_NAME
#undef WJR_SIMD_IS_BACKWARD

_WJR_ALGO_BEGIN

extern template const uint8_t* __large__memmis(
	const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;

extern template const uint8_t* __large__memrmis(
	const uint8_t*, const uint8_t*, size_t, std::equal_to<>) noexcept;

_WJR_ALGO_END

#undef __WJR_MEMMIS_FOUR
#undef __WJR_MEMMIS_ONE

#endif // _WJR_FAST_MEMMIS