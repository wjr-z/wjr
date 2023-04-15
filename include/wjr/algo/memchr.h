#ifndef __WJR_ALGO_ALOG_H
#error "This file should not be included directly. Include <wjr/algo.h> instead."
#endif 

#if defined(_WJR_FAST_MEMCHR)

#define __WJR_MEMCHR_ONE(st, q, ptr)												\
	{	                                                                            \
		auto r = st::cmp(x, q, pred, T());	                                        \
		st::mask_type z = st::movemask_epi8(r);	                                    \
		if(z != 0){	                                                                \
			return WJR_SIMD_FIRST_ONE_PTR(ptr, z);	                                \
		}	                                                                        \
	}

#define __WJR_MEMCHR_FOUR(st, q, ptr0, ptr1, ptr2, ptr3)							\
	{	                                                                            \
		auto r0 = st::cmp(x0, q, pred, T());	                                    \
		auto r1 = st::cmp(x1, q, pred, T());	                                    \
		auto r2 = st::cmp(x2, q, pred, T());	                                    \
		auto r3 = st::cmp(x3, q, pred, T());	                                    \
																	                \
		r3 = st::Or(st::Or(r0, r1), st::Or(r2, r3));	                            \
		st::mask_type z = st::movemask_epi8(r3);	                                \
		if(z != 0){	                                                                \
			st::mask_type tmp = st::movemask_epi8(r0);	                            \
			if(tmp != 0){	                                                        \
				return WJR_SIMD_FIRST_ONE_PTR(ptr0, tmp);	                        \
			}	                                                                    \
			tmp = st::movemask_epi8(r1);	                                        \
			if(tmp != 0){	                                                        \
				return WJR_SIMD_FIRST_ONE_PTR(ptr1, tmp);	                        \
			}	                                                                    \
			tmp = st::movemask_epi8(r2);	                                        \
			if(tmp != 0){	                                                        \
				return WJR_SIMD_FIRST_ONE_PTR(ptr2, tmp);	                        \
			}	                                                                    \
			tmp = z;	                                                            \
			return WJR_SIMD_FIRST_ONE_PTR(ptr3, tmp);	                            \
		}	                                                                        \
	}

#define WJR_SIMD_IS_BACKWARD 0
#define __WJR_MEMCHR_NAME __memchr
#include <wjr/algo/memchr-impl.h>
#undef __WJR_MEMCHR_NAME
#undef WJR_SIMD_IS_BACKWARD

#define WJR_SIMD_IS_BACKWARD 1
#define __WJR_MEMCHR_NAME __memrchr
#include <wjr/algo/memchr-impl.h>
#undef __WJR_MEMCHR_NAME
#undef WJR_SIMD_IS_BACKWARD

_WJR_ALGO_BEGIN

extern template const uint8_t* __large__memchr(
	const uint8_t*, uint8_t, size_t, std::equal_to<>) noexcept;

extern template const uint8_t* __large__memrchr(
	const uint8_t*, uint8_t, size_t, std::equal_to<>) noexcept;

_WJR_ALGO_END

#undef __WJR_MEMCHR_ONE
#undef __WJR_MEMCHR_FOUR

#endif // _WJR_FAST_MEMCHR