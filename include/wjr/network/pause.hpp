#ifndef WJR_NETWORK_PAUSE_HPP__
#define WJR_NETWORK_PAUSE_HPP__

#include <wjr/preprocessor.hpp>

namespace wjr {

#if defined(_MSC_VER)
#if defined(_M_AMD64) || defined(_M_IX86)
extern "C" void _mm_pause(void);
#if defined(WJR_COMPILER_MSVC)
#pragma intrinsic(_mm_pause)
#endif
#elif defined(_M_ARM64) || defined(_M_ARM)
extern "C" void __yield(void);
#if defined(WJR_COMPILER_MSVC)
#pragma intrinsic(__yield)
#endif
#endif
#endif

WJR_INTRINSIC_INLINE void pause() noexcept {
#if defined(_MSC_VER)
#if defined(_M_AMD64) || defined(_M_IX86)
    _mm_pause();
#elif defined(_M_ARM64) || defined(_M_ARM)
    __yield();
#endif
#elif defined(__GNUC__)
#if defined(__i386__) || defined(__x86_64__)
    asm volatile("pause\n\t" : : : "memory");
#elif (defined(__ARM_ARCH) && __ARM_ARCH >= 8) || defined(__ARM_ARCH_8A__) ||            \
    defined(__aarch64__)
    asm volatile("yield\n\t" : : : "memory");
#elif defined(__riscv) && __riscv_xlen == 64
#if defined(__riscv_zihintpause)
    asm volatile("pause\n\t" : : : "memory");
#else
    /* Encoding of the pause instruction */
    asm volatile(".4byte 0x100000F\n\t");
#endif
#endif
#endif
}

} // namespace wjr

#include <wjr/atomic/detail/footer.hpp>

#endif // WJR_NETWORK_PAUSE_HPP__
