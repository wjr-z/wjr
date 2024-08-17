#ifndef WJR_ARCH_X86_MATH_DIVIDER_HPP__
#define WJR_ARCH_X86_MATH_DIVIDER_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
    #define WJR_HAS_BUILTIN_ASM_DIV2BY1_ADJUST WJR_HAS_DEF

    #if defined(WJR_COMPILER_CLANG) && !WJR_HAS_CLANG(13, 0, 0)
        #define WJR_HAS_BUILTIN_ASM_DIV2BY1_ADJUST_BRANCH WJR_HAS_DEF
    #endif

#endif

#if WJR_HAS_BUILTIN(ASM_DIV2BY1_ADJUST)

template <typename T, WJR_REQUIRES(std::is_same_v<T, uint64_t>)>
WJR_INTRINSIC_INLINE void asm_div2by1_adjust(T rax, T div, T &r8, T &rdx) noexcept {
    T r9 = r8 + div;
    asm("cmp{q %[rax], %[r8]| %[r8], %[rax]}\n\t"
        "cmovb{q %[r8], %[r9]| %[r9], %[r8]}\n\t"
        "adc{q $-1, %[rdx]| %[rdx], -1}"
        : [r9] "+r"(r9), [r8] "+r"(r8), [rdx] "+r"(rdx)
        : [rax] "r"(rax)
        : "cc", "memory");
    r8 = r9;
}

#endif

#if WJR_HAS_BUILTIN(ASM_DIV2BY1_ADJUST_BRANCH)

template <typename T, WJR_REQUIRES(std::is_same_v<T, uint64_t>)>
WJR_INTRINSIC_INLINE void asm_div2by1_adjust_branch(T div, T &lo) noexcept {
    asm("sub %1, %0" : "+r"(lo) : "r"(div));
}

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_MATH_DIVIDER_HPP__