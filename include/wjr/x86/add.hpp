#ifndef WJR_X86_ADD_HPP__
#define WJR_X86_ADD_HPP__

#include <wjr/type_traits.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_ADDC WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_ADDC_N WJR_HAS_DEF

#define WJR_ADDSUB_I 1
#include <wjr/x86/gen_addsub.hpp>

#endif

// There are certain issues with optimizing in GCC
#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM) && defined(WJR_COMPILER_GCC)
#define WJR_HAS_BUILTIN___ASM_ADDC_128 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(__ASM_ADDC_128)

WJR_INTRINSIC_INLINE void __asm_addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                         uint64_t hi0, uint64_t lo1, uint64_t hi1) {
    asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc", "memory");
    al = lo0;
    ah = hi0;
}

#endif

} // namespace wjr

#endif // WJR_X86_ADD_HPP__