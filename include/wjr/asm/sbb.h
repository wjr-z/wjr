#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif

_WJR_ASM_BEGIN

#if WJR_HAS_BUILTIN(__builtin_subc) || WJR_HAS_CLANG(5, 0, 0)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_builtin_sbb(T a, T b, T carry_in, T* carry_out) {
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
	else if constexpr (_Nd <= _Nd_ul) {
		unsigned long CF = 0;
		T ret = __builtin_subcl(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
#if defined(WJR_X86_64)
	else if constexpr (_Nd <= _Nd_ull) {
		unsigned long long CF = 0;
		T ret = __builtin_subcll(a, b, carry_in, &CF);
		*carry_out = CF;
		return ret;
	}
#endif // WJR_X86_64
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#elif defined(WJR_COMPILER_MSVC)
template<typename T>
WJR_INTRINSIC_INLINE static T __wjr_msvc_sbb(T a, T b, T carry_in, T* carry_out) {
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
#if defined(WJR_X86_64)
	else if constexpr (_Nd <= _Nd_ull) {
		unsigned long long ret = 0;
		*carry_out = _subborrow_u64(carry_in, a, b, &ret);
		return ret;
	}
#endif // WJR_X86_64
	else {
		static_assert(_Nd <= _Nd_ull, "unsupported integer type");
	}
}
#endif

template<typename T, std::enable_if_t<wjr::is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR20 T sbb(T a, T b, T carry_in, T* carry_out) {
	if (!wjr::is_constant_evaluated()) {
		if (!((is_constant_p(a) && is_constant_p(b)) || (is_constant_p(carry_in) && carry_in == 0))) {
#if WJR_HAS_BUILTIN(__builtin_subc) || WJR_HAS_CLANG(5, 0, 0)
			return __wjr_builtin_sbb(a, b, carry_in, carry_out);
#elif defined(WJR_COMPILER_MSVC)
			return __wjr_msvc_sbb(a, b, carry_in, carry_out);
#elif defined(WJR_INLINE_ASM) // Clang does not need inline assembly
#if defined(WJR_BETTER_INLINE_ASM)
			asm("add $255, %b0\n\t"
				"sbb %3, %2\n\t"
				"setb %b1"
				: "+r"(carry_in), "=rm"(*carry_out), "+r"(a)
				: "rm"(b)
				: "cc");
#else
			asm("add $255, %b0\n\t"
				"sbb %3, %2\n\t"
				"setb %b1"
				: "+r"(carry_in), "=r"(*carry_out), "+r"(a)
				: "r"(b)
				: "cc");
			return a;
#endif // WJR_BETTER_INLINE_ASM
#endif
		}
	}
	T c = 0;
	c = a < b;
	a -= b;
	c |= a < carry_in;
	a -= carry_in;
	*carry_out = c;
	return a;
}

_WJR_ASM_END
