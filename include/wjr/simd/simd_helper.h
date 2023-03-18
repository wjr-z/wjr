#pragma once

#include <wjr/type_traits.h>
#include <wjr/math.h>

_WJR_SIMD_BEGIN

template<bool r, typename SIMD, typename iter>
WJR_INTRINSIC_INLINE decltype(auto) __getp(iter ptr){
	if constexpr (!r) {
		return ptr;
	}
	else {
		return ptr.base() - SIMD::width();
	}
}

template<bool r, typename SIMD>
WJR_INTRINSIC_INLINE int __clz(typename SIMD::mask_type t) {
	if constexpr (!r) {
		return wjr::countl_zero(t);
	}
	else {
		return wjr::countr_zero(t);
	}
}

template<bool r, typename SIMD>
WJR_INTRINSIC_INLINE int __ctz(typename SIMD::mask_type t) {
	return __clz<!r, SIMD>(t);
}

template<bool r, typename SIMD>
WJR_INTRINSIC_INLINE int __clo(typename SIMD::mask_type t) {
	if constexpr (!r) {
		return wjr::countl_one(t);
	}
	else {
		return wjr::countr_one(t);
	}
}

template<bool r, typename SIMD>
WJR_INTRINSIC_INLINE int __cto(typename SIMD::mask_type t) {
	return __clo<!r, SIMD>(t);
}

//template<bool r, typename SIMD>
//WJR_INTRINSIC_INLINE 

_WJR_SIMD_END