#ifndef __WJR_ASM_ASM_H
#error "This file should not be included directly. Include <wjr/asm.h> instead."
#endif

_WJR_ASM_BEGIN

WJR_INTRINSIC_INLINE void __volatile_pause() noexcept {
	volatile int x = 0;
	++x; ++x; ++x; ++x; ++x;
	++x; ++x; ++x; ++x; ++x;
	++x; ++x; ++x; ++x; ++x;
}

WJR_INTRINSIC_INLINE void pause() noexcept {
#if defined(WJR_MSVC) && defined(WJR_X86)
	_mm_pause();
#elif defined(WJR_INLINE_ASM)
#if defined(WJR_X86) 
	asm volatile("pause");
#else
	__volatile_pause();
#endif
#else
	__volatile_pause();
#endif 
}

_WJR_ASM_END