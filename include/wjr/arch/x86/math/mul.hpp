#ifndef WJR_ARCH_X86_MATH_MUL_HPP__
#define WJR_ARCH_X86_MATH_MUL_HPP__

#include <cstdint>

#include <wjr/arch/x86/math/mul-impl.hpp>
#include <wjr/assert.hpp>

#if WJR_HAS_BUILTIN(MSVC_UMUL128)
    #include <wjr/arch/x86/simd/intrin.hpp>
#endif

namespace wjr {

#if WJR_HAS_BUILTIN(UMUL128)

WJR_INTRINSIC_INLINE uint64_t builtin_umul128(uint64_t a, uint64_t b, uint64_t &hi) noexcept {
    #if WJR_HAS_BUILTIN(ASM_UMUL128)
    uint64_t lo;
    asm("mul{q %3| %3}\n\t" : "=a,a"(lo), "=d,d"(hi) : "%a,r"(a), "r,a"(b) : "cc");
    return lo;
    #elif WJR_HAS_BUILTIN(INT128_UMUL128)
    const __uint128_t x = static_cast<__uint128_t>(a) * b;
    hi = x >> 64;
    return static_cast<uint64_t>(x);
    #else
    return _umul128(a, b, &hi);
    #endif
}

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_MATH_MUL_HPP__