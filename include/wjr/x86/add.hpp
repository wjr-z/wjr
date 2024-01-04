#ifndef WJR_X86_ADD_HPP__
#define WJR_X86_ADD_HPP__

#include <wjr/type_traits.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_FEATURE(INLINE_ASM) &&                                                       \
    (defined(WJR_COMPILER_CLANG) || defined(WJR_COMPILER_GCC))
#define WJR_HAS_BUILTIN_ASM_ADDC_1 WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_ADDC_N WJR_HAS_DEF
#endif

#define WJR_ADDSUB_I 1

#include <wjr/x86/addsub.hpp>

} // namespace wjr

#endif // WJR_X86_ADD_HPP__