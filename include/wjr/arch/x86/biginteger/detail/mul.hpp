#ifndef WJR_ARCH_X86_BIGINTEGER_DETAIL_MUL_HPP__
#define WJR_ARCH_X86_BIGINTEGER_DETAIL_MUL_HPP__

#include <wjr/arch/x86/math/mul.hpp>
#include <wjr/assert.hpp>

namespace wjr {

#if defined(__BMI2__)
    #if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
        #define WJR_HAS_BUILTIN_ASM_MUL_1 WJR_HAS_DEF
    #elif defined(WJR_ENABLE_ASSEMBLY)
        #define WJR_HAS_BUILTIN_ASM_MUL_1 WJR_HAS_ASSEMBLY_DEF
    #endif
#endif

#if defined(__BMI2__) && defined(__ADX__)
    #if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
        #define WJR_HAS_BUILTIN_ASM_ADDMUL_1 WJR_HAS_DEF
    #elif defined(WJR_ENABLE_ASSEMBLY)
        #define WJR_HAS_BUILTIN_ASM_ADDMUL_1 WJR_HAS_ASSEMBLY_DEF
    #endif
#endif

#if defined(__BMI2__) && defined(__ADX__)
    #if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
        #define WJR_HAS_BUILTIN_ASM_SUBMUL_1 WJR_HAS_DEF
    #elif defined(WJR_ENABLE_ASSEMBLY)
        #define WJR_HAS_BUILTIN_ASM_SUBMUL_1 WJR_HAS_ASSEMBLY_DEF
    #endif
#endif

#if defined(__BMI2__)
    #if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
        #define WJR_HAS_BUILTIN_ASM_ADDLSH_N WJR_HAS_DEF
        #define WJR_HAS_BUILTIN_ASM_RSBLSH_N WJR_HAS_DEF
    #elif defined(WJR_ENABLE_ASSEMBLY)
        #define WJR_HAS_BUILTIN_ASM_ADDLSH_N WJR_HAS_ASSEMBLY_DEF
        #define WJR_HAS_BUILTIN_ASM_RSBLSH_N WJR_HAS_ASSEMBLY_DEF
    #endif
#endif

#if defined(__BMI2__) && defined(__ADX__)
    #if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
        #define WJR_HAS_BUILTIN_ASM_BASECASE_MUL_S WJR_HAS_DEF
        #define WJR_HAS_BUILTIN_ASM_BASECASE_SQR WJR_HAS_DEF
    #elif defined(WJR_ENABLE_ASSEMBLY)
        #define WJR_HAS_BUILTIN_ASM_BASECASE_MUL_S WJR_HAS_ASSEMBLY_DEF
        #define WJR_HAS_BUILTIN_ASM_BASECASE_SQR WJR_HAS_ASSEMBLY_DEF
    #endif
#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)

    #if WJR_HAS_BUILTIN(ASM_MUL_1) == 1
extern WJR_ALL_NONNULL uint64_t __wjr_asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                                uint64_t rdx) noexcept;
    #else
extern "C" WJR_ALL_NONNULL WJR_MS_ABI uint64_t __wjr_asm_mul_1(uint64_t *dst, const uint64_t *src,
                                                               size_t n, uint64_t rdx) noexcept;
    #endif

WJR_INTRINSIC_INLINE uint64_t asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                        uint64_t rdx) noexcept {
    return __wjr_asm_mul_1(dst, src, n, rdx);
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)

    #if WJR_HAS_BUILTIN(ASM_ADDMUL_1) == 1
extern WJR_ALL_NONNULL uint64_t __wjr_asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                                   uint64_t rdx) noexcept;
    #else
extern "C" WJR_ALL_NONNULL WJR_MS_ABI uint64_t __wjr_asm_addmul_1(uint64_t *dst,
                                                                  const uint64_t *src, size_t n,
                                                                  uint64_t rdx) noexcept;
    #endif

WJR_INTRINSIC_INLINE uint64_t asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                           uint64_t rdx) noexcept {
    return __wjr_asm_addmul_1(dst, src, n, rdx);
}

#endif

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)

    #if WJR_HAS_BUILTIN(ASM_SUBMUL_1) == 1
// slower than asm_addmul_1
extern WJR_ALL_NONNULL uint64_t __wjr_asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                                   uint64_t rdx) noexcept;
    #else
extern "C" WJR_ALL_NONNULL WJR_MS_ABI uint64_t __wjr_asm_submul_1(uint64_t *dst,
                                                                  const uint64_t *src, size_t n,
                                                                  uint64_t rdx) noexcept;
    #endif

WJR_INTRINSIC_INLINE uint64_t asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n,
                                           uint64_t rdx) noexcept {
    return __wjr_asm_submul_1(dst, src, n, rdx);
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDLSH_N)
    #define WJR_ADDSUB_I 1
    #include <wjr/arch/x86/biginteger/detail/gen_addrsblsh_n.hpp>
#endif

#if WJR_HAS_BUILTIN(ASM_RSBLSH_N)
    #define WJR_ADDSUB_I 0
    #include <wjr/arch/x86/biginteger/detail/gen_addrsblsh_n.hpp>
#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)

    #if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S) == 1
extern WJR_ALL_NONNULL void __wjr_asm_basecase_mul_s_impl(uint64_t *dst, const uint64_t *src0,
                                                          size_t rdx, const uint64_t *src1,
                                                          size_t m) noexcept;
    #else
extern "C" WJR_ALL_NONNULL WJR_MS_ABI void
__wjr_asm_basecase_mul_s_impl(uint64_t *dst, const uint64_t *src0, size_t rdx, const uint64_t *src1,
                              size_t m) noexcept;
    #endif

inline void asm_basecase_mul_s(uint64_t *dst, const uint64_t *src0, size_t n, const uint64_t *src1,
                               size_t m) noexcept {
    WJR_ASSERT(n >= m);
    WJR_ASSERT(m >= 1);
    __wjr_asm_basecase_mul_s_impl(dst, src0, n, src1, m);
}

#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR)

    #if WJR_HAS_BUILTIN(ASM_BASECASE_SQR) == 1
extern WJR_ALL_NONNULL void __wjr_asm_basecase_sqr_impl(uint64_t *dst, const uint64_t *src,
                                                        size_t rdx) noexcept;
    #else
extern "C" WJR_ALL_NONNULL WJR_MS_ABI void
__wjr_asm_basecase_sqr_impl(uint64_t *dst, const uint64_t *src, size_t rdx) noexcept;
    #endif

inline void asm_basecase_sqr(uint64_t *dst, const uint64_t *src, size_t n) noexcept {
    WJR_ASSERT(n >= 1);
    __wjr_asm_basecase_sqr_impl(dst, src, n);
}

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_BIGINTEGER_DETAIL_MUL_HPP__