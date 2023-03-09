#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif

_WJR_ASM_BEGIN

#if defined(_WJR_FAST_REP)

WJR_INTRINSIC_INLINE void rep_stosb(uint8_t* s, uint8_t val, size_t n) {
#if defined(WJR_COMPILER_MSVC)
	__stosb(reinterpret_cast<unsigned char*>(s), val, n);
#else
	asm volatile("rep stosb" : "+D"(s), "+c"(n) : "a"(val) : "memory");
#endif
}

WJR_INTRINSIC_INLINE void rep_stosw(uint16_t* s, uint16_t val, size_t n) {
#if defined(WJR_COMPILER_MSVC)
	__stosw(reinterpret_cast<unsigned short*>(s), val, n);
#else
	asm volatile("rep stosw" : "+D"(s), "+c"(n) : "a"(val) : "memory");
#endif
}

WJR_INTRINSIC_INLINE void rep_stosd(uint32_t* s, uint32_t val, size_t n) {
#if defined(WJR_COMPILER_MSVC)
	__stosd(reinterpret_cast<unsigned int*>(s), val, n);
#else
	asm volatile("rep stosd" : "+D"(s), "+c"(n) : "a"(val) : "memory");
#endif
}

WJR_INTRINSIC_INLINE void rep_stosq(uint64_t* s, uint64_t val, size_t n) {
#if defined(WJR_COMPILER_MSVC)
	__stosq(reinterpret_cast<unsigned long long*>(s), val, n);
#else
	asm volatile("rep stosq" : "+D"(s), "+c"(n) : "a"(val) : "memory");
#endif
}

#endif  // _WJR_FAST_REP

_WJR_ASM_END