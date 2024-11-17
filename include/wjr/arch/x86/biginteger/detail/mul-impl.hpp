#ifndef WJR_ARCH_X86_BIGINTEGER_DETAIL_MUL_IMPL_HPP__
#define WJR_ARCH_X86_BIGINTEGER_DETAIL_MUL_IMPL_HPP__

#include <wjr/arch/x86/math/mul-impl.hpp>

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

} // namespace wjr

#endif // WJR_ARCH_X86_BIGINTEGER_DETAIL_MUL_IMPL_HPP__