#ifndef WJR_ARCH_X86_BIGINTEGER_DETAIL_SUB_HPP__
#define WJR_ARCH_X86_BIGINTEGER_DETAIL_SUB_HPP__

#include <wjr/arch/x86/math/sub.hpp>

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
    #define WJR_HAS_BUILTIN_ASM_SUBC_N WJR_HAS_DEF
#else
    #if defined(WJR_ENABLE_ASSEMBLY)
        #define WJR_HAS_BUILTIN_ASM_SUBC_N WJR_HAS_ASSEMBLY_DEF
    #endif
#endif

namespace wjr {

#if WJR_HAS_BUILTIN(ASM_SUBC_N)
    #define WJR_ADDSUB_I 0
    #include <wjr/arch/x86/math/gen_addsub.hpp>
#endif

} // namespace wjr

#endif // WJR_ARCH_X86_BIGINTEGER_DETAIL_SUB_HPP__