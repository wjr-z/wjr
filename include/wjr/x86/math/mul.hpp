#ifndef WJR_X86_MATH_MUL_HPP__
#define WJR_X86_MATH_MUL_HPP__

#include <wjr/assert.hpp>
#include <wjr/x86/math/mul-impl.hpp>

#if WJR_HAS_BUILTIN(MSVC_UMUL128)
#include <wjr/x86/simd/intrin.hpp>
#endif

namespace wjr {

#if WJR_HAS_BUILTIN(UMUL128)

WJR_INTRINSIC_INLINE uint64_t builtin_umul128(uint64_t a, uint64_t b,
                                              uint64_t &hi) noexcept {
#if WJR_HAS_BUILTIN(INT128_UMUL128)
    const __uint128_t x = static_cast<__uint128_t>(a) * b;
    hi = x >> 64;
    return static_cast<uint64_t>(x);
#elif WJR_HAS_BUILTIN(ASM_UMUL128)
    uint64_t lo;
    asm("mul{q %3| %3}\n\t" : "=a,a"(lo), "=d,d"(hi) : "%a,r"(a), "r,a"(b) : "cc");
    return lo;
#else
    return _umul128(a, b, &hi);
#endif
}

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)

#if WJR_HAS_BUILTIN(ASM_MUL_1) == 1
extern uint64_t __wjr_asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                uint64_t rdx) noexcept;
#else
extern "C" WJR_MS_ABI uint64_t __wjr_asm_mul_1(uint64_t *dst, const uint64_t *src,
                                               size_t n, uint64_t rdx) noexcept;
#endif

WJR_INTRINSIC_INLINE uint64_t asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                        uint64_t rdx) noexcept {
    return __wjr_asm_mul_1(dst, src, n, rdx);
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1) == 1
extern uint64_t __wjr_asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                   uint64_t rdx) noexcept;
#else
extern "C" WJR_MS_ABI uint64_t __wjr_asm_addmul_1(uint64_t *dst, const uint64_t *src,
                                                  size_t n, uint64_t rdx) noexcept;
#endif

WJR_INTRINSIC_INLINE uint64_t asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                           uint64_t rdx) noexcept {
    return __wjr_asm_addmul_1(dst, src, n, rdx);
}

#endif

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1) == 1
// slower than asm_addmul_1
extern uint64_t __wjr_asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                   uint64_t rdx) noexcept;
#else
extern "C" WJR_MS_ABI uint64_t __wjr_asm_submul_1(uint64_t *dst, const uint64_t *src,
                                                  size_t n, uint64_t rdx) noexcept;
#endif

WJR_INTRINSIC_INLINE uint64_t asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                           uint64_t rdx) noexcept {
    return __wjr_asm_submul_1(dst, src, n, rdx);
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDLSH_N)
#define WJR_ADDSUB_I 1
#include <wjr/x86/math/gen_addrsblsh_n.hpp>
#endif

#if WJR_HAS_BUILTIN(ASM_RSBLSH_N)
#define WJR_ADDSUB_I 0
#include <wjr/x86/math/gen_addrsblsh_n.hpp>
#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S) == 1
extern void __wjr_asm_basecase_mul_s_impl(uint64_t *dst, const uint64_t *src0, size_t rdx,
                                          const uint64_t *src1, size_t m) noexcept;
#else
extern "C" WJR_MS_ABI void __wjr_asm_basecase_mul_s_impl(uint64_t *dst,
                                                         const uint64_t *src0, size_t rdx,
                                                         const uint64_t *src1,
                                                         size_t m) noexcept;
#endif

inline void asm_basecase_mul_s(uint64_t *dst, const uint64_t *src0, size_t n,
                               const uint64_t *src1, size_t m) noexcept {
    WJR_ASSERT(n >= m);
    WJR_ASSERT(m >= 1);
    __wjr_asm_basecase_mul_s_impl(dst, src0, n, src1, m);
}

#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR)

#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR) == 1
extern void __wjr_asm_basecase_sqr_impl(uint64_t *dst, const uint64_t *src,
                                        size_t rdx) noexcept;
#else
extern "C" WJR_MS_ABI void __wjr_asm_basecase_sqr_impl(uint64_t *dst, const uint64_t *src,
                                                       size_t rdx) noexcept;
#endif

inline void asm_basecase_sqr(uint64_t *dst, const uint64_t *src, size_t n) noexcept {
    WJR_ASSERT(n >= 1);
    __wjr_asm_basecase_sqr_impl(dst, src, n);
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_MUL_HPP__