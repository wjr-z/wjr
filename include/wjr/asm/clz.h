#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif

_WJR_ASM_BEGIN

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
WJR_INTRINSIC_CONSTEXPR static int __wjr_fallback_clz(T x) noexcept {
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
WJR_INTRINSIC_INLINE static int __wjr_builtin_clz(T x) noexcept {
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
#if defined(WJR_X86_64)
	else if constexpr (_Nd <= _Nd_ull) {
		constexpr auto __diff = _Nd_ull - _Nd;
		return __builtin_clzll(x) - __diff;
	}
#endif // WJR_X86_64
	else {
		static_assert(_Nd <= _Nd_ull, "countl_zero is not implemented for this type");
	}
}
#elif defined(WJR_COMPILER_MSVC)
#if defined(WJR_X86)

template<typename T>
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_avx2_clz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if constexpr (_Nd <= 16) {
		return static_cast<int>(__lzcnt16(x) - (16 - _Nd));
	}
	else if constexpr (_Nd <= 32) {
		return static_cast<int>(__lzcnt(x));
	}
	else {
#if defined(WJR_X86_32)
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
}

template<typename T>
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_normal_clz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	unsigned long _Result;
	if constexpr (_Nd <= 32) {
		if (!_BitScanReverse(&_Result, x)) {
			return _Nd;
		}
	}
	else {
#if defined(WJR_X86_32)
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
}

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_INLINE static int __wjr_msvc_x86_64_clz(T x) noexcept {
#if WJR_AVX2
	return __wjr_msvc_x86_64_avx2_clz(x);
#elif defined(_WJR_CPUINFO)
	if (is_avx2()) {
		return __wjr_msvc_x86_64_avx2_clz(x);
	}
	else {
		return __wjr_msvc_x86_64_normal_clz(x);
	}
#else
	return __wjr_msvc_x86_64_normal_clz(x);
#endif // WJR_AVX2
}
#elif defined(WJR_ARM)
template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_INLINE static int __wjr_msvc_arm_clz(T x) noexcept {
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
	if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_clz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
		return __wjr_builtin_clz(x);
#elif defined(WJR_COMPILER_MSVC)
#if defined(WJR_X86)
		return __wjr_msvc_x86_64_clz(x);
#elif defined(WJR_ARM)
		return __wjr_msvc_arm_clz(x);
#endif
#endif
	}
	return __wjr_fallback_clz(x);
}

_WJR_ASM_END
