#pragma once
#ifndef __WJR_SIMD___MEMCMP_H
#define __WJR_SIMD___MEMCMP_H

#include <wjr/macro.h>

#if defined(__AVX2__) || defined(__SSE2__)
#include <wjr/simd/simd_intrin.h>

#define __REGISTER_MEMCMP_FUNC_ONE_HELPER__	                                                \
	auto r = traits::cmp(x, y, pred, T{});	                                                    \
	auto z = traits::movemask_epi8(r);	                                                    \
	if(z != __Mask){	                                                                    \
		return false;	                                                                    \
	}

#define __REGISTER_MEMCMP_FUNC_FOUR_HELPER__	                                            \
	auto r0 = traits::cmp(x0, x4, pred, T{});												    \
	auto r1 = traits::cmp(x1, x5, pred, T{});									                \
	auto r2 = traits::cmp(x2, x6, pred, T{});									                \
	auto r3 = traits::cmp(x3, x7, pred, T{});									                \
																						    \
	auto z = traits::movemask_epi8(														    \
		traits::And(traits::And(r0, r1), traits::And(r2, r3)));							    \
																						    \
	if(z != __Mask){																	    \
		return false;																	    \
	}																					    \

#if defined(__SSE4_1__)
#define __REGISTER_MEMCMP_FUNC_ONE__SSE4_1__HELPER__	                                    \
	auto r = traits::Xor(x, y);	                                                            \
	auto z = traits::test_all_zeros(r);	                                                    \
	if(!z){	                                                                                \
		return false;	                                                                    \
	}

#define __REGISTER_MEMCMP_FUNC_FOUR__SSE4_1__HELPER__	                                    \
	auto r0 = traits::Xor(x0, x4);												            \
	auto r1 = traits::Xor(x1, x5);									                        \
	auto r2 = traits::Xor(x2, x6);									                        \
	auto r3 = traits::Xor(x3, x7);									                        \
																							\
	auto z = traits::test_all_zeros(														\
		traits::Or(traits::Or(r0, r1), traits::Or(r2, r3)));							    \
																							\
	if(!z){																	                \
		return false;																	    \
	}																					

#define __REGISTER_MEMCMP_FUNC_ONE_HELPER	                                                \
	if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                                \
		__REGISTER_MEMCMP_FUNC_ONE__SSE4_1__HELPER__	                                    \
	}	                                                                                    \
	else {	                                                                                \
		__REGISTER_MEMCMP_FUNC_ONE_HELPER__	                                                \
	}

#define __REGISTER_MEMCMP_FUNC_FOUR_HELPER	                                                \
	if constexpr(std::is_same_v<_Pred, std::equal_to<>>){	                                \
		__REGISTER_MEMCMP_FUNC_FOUR__SSE4_1__HELPER__	                                    \
	}	                                                                                    \
	else {	                                                                                \
		__REGISTER_MEMCMP_FUNC_FOUR_HELPER__	                                            \
	}

#else
#define __REGISTER_MEMCMP_FUNC_ONE_HELPER	__REGISTER_MEMCMP_FUNC_INE_HELPER__
#define __REGISTER_MEMCMP_FUNC_FOUR_HELPER	__REGISTER_MEMCMP_FUNC_FOUR_HELPER__
#endif

#define __REGISTER_MEMCMP_FUNC_ONE(s0, s1)													\
	{																			            \
		auto x = traits::loadu(reinterpret_cast<const simd_type*>((s0)));		            \
		auto y = traits::loadu(reinterpret_cast<const simd_type*>((s1)));		            \
		__REGISTER_MEMCMP_FUNC_ONE_HELPER	                                                \
	}

#define __REGISTER_MEMCMP_FUNC_FOUR(s0, s1)													\
	{																						\
		auto x0 = traits::loadu(reinterpret_cast<const simd_type*>((s0)));					\
		auto x1 = traits::loadu(reinterpret_cast<const simd_type*>((s0) + _Mysize * 1));	\
		auto x2 = traits::loadu(reinterpret_cast<const simd_type*>((s0) + _Mysize * 2));	\
		auto x3 = traits::loadu(reinterpret_cast<const simd_type*>((s0) + _Mysize * 3));	\
																							\
		auto x4 = traits::loadu(reinterpret_cast<const simd_type*>((s1)));					\
		auto x5 = traits::loadu(reinterpret_cast<const simd_type*>((s1) + _Mysize * 1));	\
		auto x6 = traits::loadu(reinterpret_cast<const simd_type*>((s1) + _Mysize * 2));	\
		auto x7 = traits::loadu(reinterpret_cast<const simd_type*>((s1) + _Mysize * 3));	\
																							\
		__REGISTER_MEMCMP_FUNC_FOUR_HELPER	                                                \
	}

_WJR_SIMD_BEGIN

template<typename T, typename _Pred>
bool __memcmp(const T* s0, const T* s1, size_t n, _Pred pred) {
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
	constexpr int __Mask = is_avx ? 0xffffffff : 0xffff;

	if (n == 0) { return true; }
	if (n == 1) { return pred(*s0, *s1); }

	if (n < _Mysize) {
		auto ns = n * sizeof(T);
		auto ql = ns >> 1;
		auto sl = ql << 1;
		auto x = traits::preload_si16x(s0, ql);
		auto y = traits::preload_si16x(s1, ql);
		auto r = traits::cmp(x, y, pred, T{});
		auto z = traits::movemask_epi8(r) | (__Mask ^ ((1u << sl) - 1));

		if (z != __Mask) {
			return false;
		}
		s0 += sl / sizeof(T);
		s1 += sl / sizeof(T);
	         
		if constexpr(sizeof(T) == 1){	
			if (ns & 1) {         
				if (!pred(*s0, *s1)) {  
					return false;	
				}	    
			}	        
		}	 
		return true;	
	}	     
	         
	{	     
		__REGISTER_MEMCMP_FUNC_ONE(s0, s1);   
	}	     
             
	if (n <= _Mysize * 2) {
		s0 += n;
		if (n != _Mysize) {	 
			s1 += n;	
			__REGISTER_MEMCMP_FUNC_ONE(s0 - _Mysize, s1 - _Mysize);	    
		}
		return true;
	}
	  
	{	     
		__REGISTER_MEMCMP_FUNC_ONE(s0 + _Mysize, s1 + _Mysize);	 
	}	     
	         
	if (n <= _Mysize * 4) {	          
		s0 += n;	    
		s1 += n;	    
		__REGISTER_MEMCMP_FUNC_ONE(s0 - _Mysize * 2, s1 - _Mysize * 2);	
		__REGISTER_MEMCMP_FUNC_ONE(s0 - _Mysize, s1 - _Mysize);
		return true;	
	}	     
             
	{	     
		__REGISTER_MEMCMP_FUNC_ONE(s0 + _Mysize * 2, s1 + _Mysize * 2);	
		__REGISTER_MEMCMP_FUNC_ONE(s0 + _Mysize * 3, s1 + _Mysize * 3);	
	}	     
	         
	if (n <= _Mysize * 8) {	          
		s0 += n;	    
		s1 += n;	    
		__REGISTER_MEMCMP_FUNC_FOUR(s0 - _Mysize * 4, s1 - _Mysize * 4);	
		return true;	
	}	     
	         
	__REGISTER_MEMCMP_FUNC_FOUR(s0 + _Mysize * 4, s1 + _Mysize * 4);	    
	         
	s0 += _Mysize * 8;	
	s1 += _Mysize * 8;	
	n -= _Mysize * 8;	
	         
	while (n >= _Mysize * 4) {	      
		__REGISTER_MEMCMP_FUNC_FOUR(s0, s1);	
		s0 += _Mysize * 4;
		s1 += _Mysize * 4;
		n -= _Mysize * 4;
	}	     
	         
	if (n) {
		s0 += n;
		s1 += n;
		__REGISTER_MEMCMP_FUNC_FOUR(s0 - _Mysize * 4, s1 - _Mysize * 4);	
	}
	         
	return true;  
}

_WJR_SIMD_END

#undef __REGISTER_MEMCMP_FUNC_ONE
#undef __REGISTER_MEMCMP_FUNC_FOUR
#undef __REGISTER_MEMCMP_FUNC_ONE_HELPER
#undef __REGISTER_MEMCMP_FUNC_FOUR_HELPER
#undef __REGISTER_MEMCMP_FUNC_ONE_HELPER__
#undef __REGISTER_MEMCMP_FUNC_FOUR_HELPER__
#undef __REGISTER_MEMCMP_FUNC_ONE__SSE4_1__HELPER__
#undef __REGISTER_MEMCMP_FUNC_FOUR__SSE4_1__HELPER__

#endif // __AVX2__ || __SSE2__


#endif // __WJR_SIMD___MEMCMP_H