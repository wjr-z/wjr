#pragma once
#ifndef __WJR_ASM_POPCNT_H__
#define __WJR_ASM_POPCNT_H__

#include <wjr/type_traits.h>

_WJR_ASM_BEGIN

template<typename _Ty>
WJR_INTRINSIC_CONSTEXPR static int __wjr_fallback_popcount(_Ty _Val) noexcept {
	constexpr int _Digits = std::numeric_limits<_Ty>::digits;
#if defined(WJR_X86_32) || defined(WJR_ARM)
	if constexpr (_Digits == 64) {
		// 64-bit bit operations on architectures without 64-bit registers are less efficient,
		// hence we split the value so that it fits in 32-bit registers
		return _Popcount_fallback(static_cast<unsigned long>(_Val))
			+ _Popcount_fallback(static_cast<unsigned long>(_Val >> 32));
	}
#endif // defined(WJR_X86_32) || defined(WJR_ARM)
	// we static_cast these bit patterns in order to truncate them to the correct size
	_Val = static_cast<_Ty>(_Val - ((_Val >> 1) & static_cast<_Ty>(0x5555'5555'5555'5555ull)));
	_Val = static_cast<_Ty>((_Val & static_cast<_Ty>(0x3333'3333'3333'3333ull))
		+ ((_Val >> 2) & static_cast<_Ty>(0x3333'3333'3333'3333ull)));
	_Val = static_cast<_Ty>((_Val + (_Val >> 4)) & static_cast<_Ty>(0x0F0F'0F0F'0F0F'0F0Full));
	// Multiply by one in each byte, so that it will have the sum of all source bytes in the highest byte
	_Val = static_cast<_Ty>(_Val * static_cast<_Ty>(0x0101'0101'0101'0101ull));
	// Extract highest byte
	return static_cast<int>(_Val >> (_Digits - 8));
}

#if WJR_HAS_BUILTIN(__builtin_popcount) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_INTRINSIC_INLINE static int __wjr_builtin_popcount(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;

	if constexpr (_Nd <= _Nd_ui) {
		return __builtin_popcount(static_cast<unsigned>(x));
	}
	else if constexpr (_Nd <= _Nd_ul) {
		return __builtin_popcountl(static_cast<unsigned long>(x));
	}
	else if constexpr (_Nd <= _Nd_ull) {
		return __builtin_popcountll(static_cast<unsigned long long>(x));
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}

}
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
	&& !defined(__INTEL_COMPILER)
template <typename T>
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_popcount(T x) noexcept {
	constexpr int _Digits = std::numeric_limits<T>::digits;
	if constexpr (_Digits <= 16) {
		return static_cast<int>(__popcnt16(x));
	}
	else if constexpr (_Digits == 32) {
		return static_cast<int>(__popcnt(x));
	}
	else {
#ifdef WJR_X86_32
		return static_cast<int>(__popcnt(x >> 32) + __popcnt(static_cast<unsigned int>(x)));
#else
		return static_cast<int>(__popcnt64(x));
#endif // WJR_X86_32
	}
}
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int popcnt(T x) noexcept {
	if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_popcount) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
		return __wjr_builtin_popcount(x);
#elif defined(_MSC_VER) && defined(WJR_X86) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
	&& !defined(__INTEL_COMPILER)
		return __wjr_msvc_x86_64_popcount(x);
#endif 
	}
	return __wjr_fallback_popcount(x);
}

_WJR_ASM_END

#endif // __WJR_ASM_POPCNT_H__