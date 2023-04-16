#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif

_WJR_ASM_BEGIN

WJR_INTRINSIC_CONSTEXPR static uint8_t __wjr_fallback_bswap8(uint8_t x) {
	return x;
}

WJR_INTRINSIC_CONSTEXPR static uint16_t __wjr_fallback_bswap16(uint16_t x) {
	return static_cast<uint16_t>(x >> 8) | static_cast<uint16_t>(x << 8);
}

WJR_INTRINSIC_CONSTEXPR static uint32_t __wjr_fallback_bswap32(uint32_t x) {
	return (x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | (x << 24);
}

WJR_INTRINSIC_CONSTEXPR static uint64_t __wjr_fallback_bswap64(uint64_t x) {
	return (x >> 56) | ((x >> 40) & 0xff00) | ((x >> 24) & 0xff0000) | ((x >> 8) & 0xff000000)
		| ((x << 8) & 0xff00000000) | ((x << 24) & 0xff0000000000) | ((x << 40) & 0xff000000000000) | (x << 56);
}

template<typename T>
WJR_INTRINSIC_CONSTEXPR T __wjr_fallback_bswap(T x) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if constexpr (_Nd <= 8) {
		return __wjr_fallback_bswap8(x);
	}
	else if constexpr (_Nd <= 16) {
		return __wjr_fallback_bswap16(x);
	}
	else if constexpr (_Nd <= 32) {
		return __wjr_fallback_bswap32(x);
	}
	else if constexpr (_Nd <= 64) {
		return __wjr_fallback_bswap64(x);
	}
	else {
		static_assert(_Nd <= 64, "Unsupported type.");
	}
}

// byte swap
#if WJR_HAS_BUILTIN(__builtin_bswap16) || WJR_HAS_GCC(10, 1, 0) || WJR_HAS_CLANG(15, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE T __wjr_builtin_bswap(T x) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
	constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
	constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

	if constexpr (_Nd <= _Nd_ub) {
		return x;
	}
	else if constexpr (_Nd <= _Nd_us) {
		return __builtin_bswap16(x);
	}
	else if constexpr (_Nd <= _Nd_ui) {
		return __builtin_bswap32(x);
	}
	else if constexpr (_Nd <= _Nd_ul) {
		return __builtin_bswap64(x);
	}
#if defined(WJR_X86_64)
	else if constexpr (_Nd <= _Nd_ull) {
		return __builtin_bswap64(x);
	}
#endif // WJR_X86_64
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_INTRINSIC_INLINE T __wjr_msvc_bswap(T x) {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	constexpr auto _Nd_ull = std::numeric_limits<unsigned long long>::digits;
	//constexpr auto _Nd_ul = std::numeric_limits<unsigned long>::digits;
	//constexpr auto _Nd_ui = std::numeric_limits<unsigned int>::digits;
	//constexpr auto _Nd_us = std::numeric_limits<unsigned short>::digits;
	//constexpr auto _Nd_ub = std::numeric_limits<unsigned char>::digits;

	if constexpr (_Nd <= 8) {
		return x;
	}
	else if constexpr (_Nd <= 16) {
		return _byteswap_ushort(x);
	}
	else if constexpr (_Nd <= 32) {
		return _byteswap_ulong(x);
	}
	else if constexpr (_Nd <= 64) {
		return _byteswap_uint64(x);
	}
	else {
		static_assert(_Nd <= _Nd, "unsupported integer type");
	}
}
#endif // WJR_HAS_BUILTIN(__builtin_bswap16) || WJR_HAS_CLANG(3, 2, 0)

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_INLINE T bswap(T x) {
	if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_bswap16) || WJR_HAS_GCC(10, 1, 0) || WJR_HAS_CLANG(15, 0, 0)
		return __wjr_builtin_bswap(x);
#elif defined(WJR_COMPILER_MSVC)
		return __wjr_msvc_bswap(x);
#endif
	}
	return __wjr_fallback_bswap(x);
}

_WJR_ASM_END