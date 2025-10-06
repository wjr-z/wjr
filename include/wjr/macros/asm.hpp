/**
 * @file macros/asm.hpp
 * @brief Assembly language macro utilities
 * @author wjr
 *
 * Provides macros for inline assembly code with compiler-specific adaptations.
 */

#ifndef WJR_MACROS_ASM_HPP__
#define WJR_MACROS_ASM_HPP__

#include <wjr/config/has.hpp>

#define WJR_ASM_PIC_JMPL(LABEL, TABLE) ".long " #LABEL "-" #TABLE
#define WJR_ASM_NOPIC_JMPL(LABEL) ".quad " #LABEL

#if WJR_HAS_FEATURE(INLINE_ASM_CCCOND)
    #define WJR_ASM_CCSET(c) "/* set condition codes */\n\t"
    #define WJR_ASM_CCOUT(c) "=@cc" #c
#else
    #define WJR_ASM_CCSET(c) "set" #c " %[_cc_" #c "]\n\t"
    #define WJR_ASM_CCOUT(c) [_cc_##c] "=r"
#endif

#endif // WJR_MACROS_ASM_HPP__