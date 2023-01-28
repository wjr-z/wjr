#pragma once
#ifndef __WJR_ASM_H
#define __WJR_ASM_H

#include <limits>
#include <wjr/type_traits.h>

_WJR_ASM_BEGIN

WJR_INTRINSIC_CONSTEXPR bool is_likely(bool f) noexcept {
#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	return __builtin_expect(f, true);
#else
	return f;
#endif
}

WJR_INTRINSIC_CONSTEXPR bool is_unlikely(bool f) noexcept {
#if WJR_HAS_BUILTIN(__builtin_expect) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	return __builtin_expect(f, false);
#else
	return f;
#endif
}

WJR_INTRINSIC_CONSTEXPR bool is_constant_evaluated() noexcept {
#if defined(__cpp_lib_is_constant_evaluated) 
	return std::is_constant_evaluated();
#elif WJR_HAS_BUILTIN(__builtin_is_constant_evaluated) || WJR_HAS_GCC(9,1,0) || WJR_HAS_CLANG(9,0,0)
	return __builtin_is_constant_evaluated();
#else
	return false;
#endif
}

WJR_INTRINSIC_INLINE void unreachable() noexcept {
#if defined(_cpp_lib_unreachable)
	return std::unreachable();
#elif WJR_HAS_BUILTIN(__builtin_unreachable) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	__builtin_unreachable();
#elif defined(WJR_COMPILER_MSVC)
	__assume(0);
#endif
}

WJR_INTRINSIC_CONSTEXPR void assume(bool cond) noexcept{
	if(!cond){
		unreachable();
	}
}

template<typename T>
WJR_INTRINSIC_CONSTEXPR bool is_constant_p(T x) noexcept {
#if WJR_HAS_BUILTIN(__builtin_constant_p) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
	return __builtin_constant_p(x);
#else
	return is_constant_evaluated();
#endif
}

const static int _WJR_LOG_TABLE[256] = {
		0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
};

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR int __wjr_fallback_clz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}

	int n = 0;
	// use _Table

	if constexpr (_Nd > 32) {
		if (x >> 32) {
			n += 32;
			x >>= 32;
		}
	}
	if constexpr (_Nd > 16) {
		if (x >> 16) {
			n += 16;
			x >>= 16;
		}
	}
	if constexpr (_Nd > 8) {
		if (x >> 8) {
			n += 8;
			x >>= 8;
		}
	}
	return  _Nd - (n + _WJR_LOG_TABLE[x]);
}

#if WJR_HAS_BUILTIN(__builtin_clz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_INTRINSIC_INLINE int __wjr_builtin_clz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;

	if constexpr (_Nd <= _Nd_ui) {
		constexpr auto __diff = _Nd_ui - _Nd;
		return __builtin_clz(x) - __diff;
	}
	else if constexpr (_Nd <= _Nd_ul) {
		constexpr auto __diff = _Nd_ul - _Nd;
		return __builtin_clzl(x) - __diff;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		constexpr auto __diff = _Nd_ull - _Nd;
		return __builtin_clzll(x) - __diff;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "countl_zero is not implemented for this type");
	}
}
#elif defined(WJR_COMPILER_MSVC)
#if defined(WJR_X86_64)
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_INLINE int __wjr_msvc_x86_64_clz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
#if defined(__AVX2__)
	if constexpr (_Nd <= 16) {
		return static_cast<int>(__lzcnt16(x) - (16 - _Nd));
	}
	else if constexpr (_Nd <= 32) {
		return static_cast<int>(__lzcnt(x));
	}
	else {
#if defined(WJR_X86)
		const auto xh = static_cast<unsigned int>(x >> 32);
		const auto xl = static_cast<unsigned int>(x);
		if (xh == 0) {
			return 32 + static_cast<int>(__lzcnt(xl));
		}
		else {
			return static_cast<int>(__lzcnt(xh));
		}
#else
		return static_cast<int>(__lzcnt64(x));
#endif
	}
#else
	unsigned long _Result;
	if constexpr (_Nd <= 32) {
		if (!_BitScanReverse(&_Result, x)) {
			return _Nd;
		}
	}
	else {
#if defined(WJR_X86)
		const auto xh = static_cast<unsigned int>(x >> 32);
		if (_BitScanReverse(&_Result, xh)) {
			return static_cast<int>(31 - _Result);
		}
		const auto xl = static_cast<unsigned int>(x);
		if (!_BitScanReverse(&_Result, xl)) {
			return _Nd;
		}
#else
		if (!_BitScanReverse64(&_Result, x)) {
			return _Nd;
		}
#endif
	}
	return static_cast<int>(_Nd - 1 - _Result);
#endif 
}
#elif defined(WJR_ARM)
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_INLINE int __wjr_msvc_arm_clz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}
	if constexpr (_Nd <= 32) {
		return static_cast<int>(_CountLeadingZeros(x));
	}
	else {
		return static_cast<int>(_CountLeadingZeros64(x));
	}
}
#endif
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int clz(T x) noexcept {
	if (!is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_clz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
		return __wjr_builtin_clz(x);
#elif defined(WJR_COMPILER_MSVC)
#if defined(WJR_X86_64)
		return __wjr_msvc_x86_64_clz(x);
#elif defined(WJR_ARM)
		return __wjr_msvc_arm_clz(x);
#endif
#endif
	}
	return __wjr_fallback_clz(x);
}

#if WJR_HAS_BUILTIN(__builtin_ctz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_INTRINSIC_INLINE int __wjr_builtin_ctz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (x == 0) {
		return _Nd;
	}

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;

	if constexpr (_Nd <= _Nd_ui) {
		return __builtin_ctz(x);
	}
	else if constexpr (_Nd <= _Nd_ul) {
		return __builtin_ctzl(x);
	}
	else if constexpr (_Nd <= _Nd_ull) {
		return __builtin_ctzll(x);
	}
	else {
		static_assert(_Nd <= _Nd_ull, "countr_zero is not implemented for this type");
	}
}
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
    && !defined(__INTEL_COMPILER)
template<typename T>
WJR_INTRINSIC_INLINE int __wjr_msvc_x86_64_ctz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	constexpr T _Max = std::numeric_limits<T>::max();
#if defined(__AVX2__)
	if constexpr (_Nd <= 32) {
		return static_cast<int>(_tzcnt_u32(static_cast<unsigned int>(~_Max | x)));
	}
	else {
#if defined(WJR_X86)
		const auto xh = static_cast<unsigned int>(x >> 32);
		const auto xl = static_cast<unsigned int>(x);
		if (xh == 0) {
			return 32 + static_cast<int>(_tzcnt_u32(xl));
		}
		else {
			return static_cast<int>(_tzcnt_u32(xh));
		}
#else
		return static_cast<int>(_tzcnt_u64(x));
#endif // WJR_X86
	}
#else
	unsigned long _Result;
	if constexpr (_Nd <= 32) {
		if (!_BitScanForward(&_Result, x)) {
			return _Nd;
		}
	}
	else {
#if defined(WJR_X86)
		const auto xl = static_cast<unsigned int>(x);
		if (_BitScanForward(&_Result, xl)) {
			return static_cast<int>(_Result);
		}
		const auto xh = static_cast<unsigned int>(x >> 32);
		if (!_BitScanForward(&_Result, xh)) {
			return _Nd;
		}
		else {
			return static_cast<int>(32 + _Result);
		}

#else
		if (!_BitScanForward64(&_Result, x)) {
			return _Nd;
		}
#endif
	}
	return static_cast<int>(_Result);
#endif
}
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int ctz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (!wjr::masm::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_ctz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
		return __wjr_builtin_ctz(x);
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
	&& !defined(__INTEL_COMPILER)
		return __wjr_msvc_x86_64_ctz(x);
#endif
	}
	return _Nd - clz(static_cast<T>(static_cast<T>(~x) & static_cast<T>(x - 1)));
}

template<typename _Ty>
WJR_INTRINSIC_CONSTEXPR int __wjr_fallback_popcount(_Ty _Val) noexcept {
	constexpr int _Digits = std::numeric_limits<_Ty>::digits;
#if defined(WJR_X86) || defined(WJR_ARM)
	if constexpr (_Digits == 64) {
		// 64-bit bit operations on architectures without 64-bit registers are less efficient,
		// hence we split the value so that it fits in 32-bit registers
		return _Popcount_fallback(static_cast<unsigned long>(_Val))
			+ _Popcount_fallback(static_cast<unsigned long>(_Val >> 32));
	}
#endif // defined(WJR_X86) || defined(WJR_ARM)
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
WJR_INTRINSIC_INLINE int __wjr_builtin_popcount(T x) noexcept {
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
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86_64) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
	&& !defined(__INTEL_COMPILER)
template <typename T>
WJR_INTRINSIC_INLINE int __wjr_msvc_x86_64_popcount(T x) noexcept {
	constexpr int _Digits = std::numeric_limits<T>::digits;
	if constexpr (_Digits <= 16) {
		return static_cast<int>(__popcnt16(x));
	}
	else if constexpr (_Digits == 32) {
		return static_cast<int>(__popcnt(x));
	}
	else {
#ifdef WJR_X86
		return static_cast<int>(__popcnt(x >> 32) + __popcnt(static_cast<unsigned int>(x)));
#else
		return static_cast<int>(__popcnt64(x));
#endif // WJR_X86
	}
}
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 int popcnt(T x) noexcept {
	if (!wjr::masm::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_popcount) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
		return __wjr_builtin_popcount(x);
#elif defined(_MSC_VER) && defined(WJR_X86_64) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
	&& !defined(__INTEL_COMPILER)
		return __wjr_msvc_x86_64_popcount(x);
#endif 
	}
	return __wjr_fallback_popcount(x);
}

#if WJR_HAS_BUILTIN(__builtin_addc) || WJR_HAS_CLANG(5, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE T __wjr_builtin_adc(T a, T b, T carry_in, T* carry_out) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
	constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
	constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

	if constexpr (_Nd <= _Nd_ub) {
		unsigned char CF = 0;
		T ret = __builtin_addcb(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_us) {
		unsigned short CF = 0;
		T ret = __builtin_addcs(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned int CF = 0;
		T ret = __builtin_addc(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned long CF = 0;
		T ret = __builtin_addcl(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		unsigned long long CF = 0;
		T ret = __builtin_addcll(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_INTRINSIC_INLINE T __wjr_msvc_adc(T a, T b, T carry_in, T* carry_out) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	
	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	//constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
	constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
	constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

	if constexpr (_Nd <= _Nd_ub) {
		unsigned char ret = 0;
		*carry_out = _addcarry_u8(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_us) {
		unsigned short ret = 0;
		*carry_out = _addcarry_u16(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned int ret = 0;
		*carry_out = _addcarry_u32(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		unsigned long long ret = 0;
		*carry_out = _addcarry_u64(carry_in, a, b, &ret);
		return ret;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T __adc(T a, T b, T carry_in, T* carry_out) {
	if (!(is_constant_p(a) || is_constant_p(b) || is_constant_p(carry_in))) {
#if defined(WJR_COMPILER_MSVC)
		return __wjr_msvc_adc(a, b, carry_in, carry_out);
#elif WJR_HAS_BUILTIN(__builtin_addc) || WJR_HAS_CLANG(5, 0, 0)
		return __wjr_builtin_adc(a, b, carry_in, carry_out);
#elif defined(WJR_COMPILER_GCC)
		asm("add $255, %b1\n\t"
			"adc %3, %0\n\t"
			"setc %b2"
			: "+r"(a), "+r"(carry_in), "=r"(*carry_out)
			: "rm"(b)
			: "cc"
		);
		return a;
#endif
	}
	T c = 0;
	a += b;
	c = a < b;
	a += carry_in;
	c |= a < carry_in;
	*carry_out = c;
	return a;
}

WJR_INTRINSIC_CONSTEXPR20 uint8_t adc(uint8_t a, uint8_t b, uint8_t carry_in, uint8_t* carry_out) {
	return __adc(a, b, carry_in, carry_out);
}
WJR_INTRINSIC_CONSTEXPR20 uint16_t adc(uint16_t a, uint16_t b, uint16_t carry_in, uint16_t* carry_out) {
	return __adc(a, b, carry_in, carry_out);
}
WJR_INTRINSIC_CONSTEXPR20 uint32_t adc(uint32_t a, uint32_t b, uint32_t carry_in, uint32_t* carry_out) {
	return __adc(a, b, carry_in, carry_out);
}
#if defined(WJR_X64)
WJR_INTRINSIC_CONSTEXPR20 uint64_t adc(uint64_t a, uint64_t b, uint64_t carry_in, uint64_t* carry_out) {
	return __adc(a, b, carry_in, carry_out);
}
#endif // WJR_X64

#if WJR_HAS_BUILTIN(__builtin_subc) || WJR_HAS_CLANG(5, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE T __wjr_builtin_sbb(T a, T b, T carry_in, T* carry_out) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
	constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
	constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

	if constexpr (_Nd <= _Nd_ub) {
		unsigned char CF = 0;
		T ret = __builtin_subcb(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_us) {
		unsigned short CF = 0;
		T ret = __builtin_subcs(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned int CF = 0;
		T ret = __builtin_subc(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned long CF = 0;
		T ret = __builtin_subcl(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		unsigned long long CF = 0;
		T ret = __builtin_subcll(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_INTRINSIC_INLINE T __wjr_msvc_sbb(T a, T b, T carry_in, T* carry_out) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	//constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
	constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
	constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

	if constexpr (_Nd <= _Nd_ub) {
		unsigned char ret = 0;
		*carry_out = _subborrow_u8(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_us) {
		unsigned short ret = 0;
		*carry_out = _subborrow_u16(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ui) {
		unsigned int ret = 0;
		*carry_out = _subborrow_u32(carry_in, a, b, &ret);
		return ret;
	}
	else if constexpr (_Nd <= _Nd_ull) {
		unsigned long long ret = 0;
		*carry_out = _subborrow_u64(carry_in, a, b, &ret);
		return ret;
	}
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T __sbb(T a, T b, T carry_in, T* carry_out) {
	if (!(is_constant_p(a) || is_constant_p(b) || is_constant_p(carry_in))) {
#if defined(WJR_COMPILER_MSVC)
		return __wjr_msvc_sbb(a, b, carry_in, carry_out);
#elif WJR_HAS_BUILTIN(__builtin_subc) || WJR_HAS_CLANG(5, 0, 0)
		return __wjr_builtin_sbb(a, b, carry_in, carry_out);
#elif defined(WJR_COMPILER_GCC) // Clang does not need inline assembly
		asm("add $255, %b1\n\t"
			"sbb %3, %0\n\t"
			"setc %b2"
			: "+r"(a), "+r"(carry_in), "=r"(*carry_out)
			: "rm"(b)
			: "cc"
		);
		return a;
#endif
	}
	T c = 0;
	c = a < b;
	a -= b;
	c |= a < carry_in;
	a -= carry_in;
	*carry_out = c;
	return a;
}

WJR_INTRINSIC_CONSTEXPR20 uint8_t sbb(uint8_t a, uint8_t b, uint8_t carry_in, uint8_t* carry_out) {
	return __sbb(a, b, carry_in, carry_out);
}
WJR_INTRINSIC_CONSTEXPR20 uint16_t sbb(uint16_t a, uint16_t b, uint16_t carry_in, uint16_t* carry_out) {
	return __sbb(a, b, carry_in, carry_out);
}
WJR_INTRINSIC_CONSTEXPR20 uint32_t sbb(uint32_t a, uint32_t b, uint32_t carry_in, uint32_t* carry_out) {
	return __sbb(a, b, carry_in, carry_out);
}
#if defined(WJR_X64)
WJR_INTRINSIC_CONSTEXPR20 uint64_t sbb(uint64_t a, uint64_t b, uint64_t carry_in, uint64_t* carry_out) {
	return __sbb(a, b, carry_in, carry_out);
}
#endif // WJR_X64

#if defined(WJR_X64)

#if defined(WJR_INLINE_ASM)
WJR_INTRINSIC_INLINE uint64_t __asm_shld(uint64_t hi, uint64_t lo, unsigned char c) noexcept {
	asm("shld %2, %1, %0" : "+r"(hi) : "r"(lo), "ci"(c) : "cc");
	return hi;
}
#endif

WJR_INTRINSIC_CONSTEXPR20 uint64_t shld(uint64_t hi, uint64_t lo, unsigned char c) noexcept {
	if (!is_constant_p(lo) && !is_constant_p(hi)) {
#if defined(WJR_INLINE_ASM)
		return __asm_shld(hi, lo, c);
#elif defined(_MSC_VER)
		return __shiftleft128(lo, hi, c);
#endif
	}
	return (hi << (c & 63)) | (lo >> (64 - (c & 63)));
}

#if defined(WJR_INLINE_ASM)
WJR_INTRINSIC_INLINE uint64_t __asm_shrd(uint64_t lo, uint64_t hi, unsigned char c) noexcept {
	asm("shrd %2, %1, %0" : "+r"(lo) : "r"(hi), "ci"(c) : "cc");
	return lo;
}
#endif 

WJR_INTRINSIC_CONSTEXPR20 uint64_t shrd(uint64_t lo, uint64_t hi, unsigned char c) noexcept {
	if (!is_constant_p(lo) && !is_constant_p(hi)) {
#if defined(WJR_INLINE_ASM)
		return __asm_shrd(lo, hi, c);
#elif defined(_MSC_VER)
		return __shiftright128(lo, hi, c);
#endif
	}
	return (lo >> (c & 63)) | (hi << (64 - (c & 63)));
}

#endif // WJR_X64

_WJR_ASM_END

#endif // __WJR_ASM_H