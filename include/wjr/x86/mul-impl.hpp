#ifndef WJR_X86_MUL_IMPL_HPP__
#define WJR_X86_MUL_IMPL_HPP__

#include <wjr/type_traits.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T mul(T a, T b, T &hi);

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T mul_1(T *dst, const T *src, size_t n, type_identity_t<T> ml);

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E T addmul_1(T *dst, const T *src, size_t n,
                                     type_identity_t<T> ml);

#define WJR_HAS_BUILTIN_MUL64 WJR_HAS_DEF

#if WJR_HAS_FEATURE(INT128)
#define WJR_HAS_BUILTIN_INT128_MUL64 WJR_HAS_DEF
#elif WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_MUL64 WJR_HAS_DEF
#else
#undef WJR_HAS_BUILTIN_MUL64
#endif

#if WJR_HAS_BUILTIN(MUL64)

WJR_INTRINSIC_INLINE uint64_t builtin_mul64(uint64_t a, uint64_t b, uint64_t &hi);

#endif

#if defined(__BMI2__)
#define WJR_HAS_BUILTIN_MULX_U64 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(MULX_U64)

template <typename T>
WJR_INTRINSIC_INLINE T mulx(T a, T b, T &hi);

#endif

#if WJR_HAS_BUILTIN(MULX_U64) && WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_MUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)

inline uint64_t asm_mul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t rdx);

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1) && defined(__ADX__)
#define WJR_HAS_BUILTIN_ASM_ADDMUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)

inline uint64_t asm_addmul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t rdx);

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1) && WJR_HAS_BUILTIN(ASM_ADDMUL_1)
#define WJR_HAS_BUILTIN_ASM_BASECASE_MUL_S WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_BASECASE_SQR WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_MUL_S)

extern void __asm_basecase_mul_s_impl(uint64_t *dst, const uint64_t *src0, size_t rdx,
                                      const uint64_t *src1, size_t m);

inline void asm_basecase_mul_s(uint64_t *dst, const uint64_t *src0, size_t n,
                               const uint64_t *src1, size_t m);

#endif

#if WJR_HAS_BUILTIN(ASM_BASECASE_SQR)

extern void __asm_basecase_sqr_impl(uint64_t *dst, const uint64_t *src, size_t rdx);

#endif

#if WJR_HAS_BUILTIN(ASM_ADDMUL_1)
#define WJR_HAS_BUILTIN_ASM_SUBMUL_1 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_SUBMUL_1)

// slower than asm_addmul_1
inline uint64_t asm_submul_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t rdx);

#endif

#if WJR_HAS_BUILTIN(ASM_MUL_1)
#define WJR_HAS_BUILTIN_ASM_ADDLSH_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_RSBLSH_N WJR_HAS_DEF
#endif

} // namespace wjr

#endif // WJR_X86_MUL_IMPL_HPP__