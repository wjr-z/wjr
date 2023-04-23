#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif

_WJR_ASM_BEGIN

#if WJR_HAS_BUILTIN(__builtin_ctz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_builtin_ctz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;

	if (is_likely(x != 0)) {

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

	return _Nd;
}
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
    && !defined(__INTEL_COMPILER)
template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_x86_64_avx2_ctz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	constexpr T _Max = std::numeric_limits<T>::max();
	if constexpr (_Nd <= 32) {
		return static_cast<int>(_tzcnt_u32(static_cast<unsigned int>(~_Max | x)));
	}
	else {
#if defined(WJR_X86_32)
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
#endif // WJR_X86_32
	}
}

template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_x86_64_normal_ctz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	constexpr T _Max = std::numeric_limits<T>::max();
	unsigned long _Result;
	if constexpr (_Nd <= 32) {
		if (!_BitScanForward(&_Result, x)) {
			return _Nd;
		}
	}
	else {
#if defined(WJR_X86_32)
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
}

template<typename T>
WJR_ATTRIBUTE(CONST, INTRINSIC_INLINE) int __wjr_msvc_x86_64_ctz(T x) noexcept {
#if WJR_AVX2
	return __wjr_msvc_x86_64_avx2_ctz(x);
#elif defined(_WJR_CPUINFO)
	if (is_avx2()) {
		return __wjr_msvc_x86_64_avx2_ctz(x);
	}
	else {
		return __wjr_msvc_x86_64_normal_ctz(x);
	}
#else
	return __wjr_msvc_x86_64_normal_ctz(x);
#endif // WJR_AVX2
}
#endif

template<typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_ATTRIBUTE(CONST, INTRINSIC_E_CONSTEXPR) int ctz(T x) noexcept {
	constexpr auto _Nd = std::numeric_limits<T>::digits;
	if (!wjr::is_constant_evaluated()) {
#if WJR_HAS_BUILTIN(__builtin_ctz) || WJR_HAS_GCC(7,1,0) || WJR_HAS_CLANG(5,0,0)
		return __wjr_builtin_ctz(x);
#elif defined(WJR_COMPILER_MSVC) && defined(WJR_X86) && !defined(_M_CEE_PURE) && !defined(__CUDACC__) \
	&& !defined(__INTEL_COMPILER)
		return __wjr_msvc_x86_64_ctz(x);
#endif
	}
	return _Nd - clz(static_cast<T>(static_cast<T>(~x) & static_cast<T>(x - 1)));
}

_WJR_ASM_END