#ifndef WJR_X86_MATH_DETAILS_HPP__
#define WJR_X86_MATH_DETAILS_HPP__

#include <wjr/preprocessor.hpp>

namespace wjr {

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_TRY_SUB_AND_SET WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_TRY_SUB_AND_ADD WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_TRY_SUB_AND_SET)

/*
 qh = x >= y;
 out = qh ? x - y : x;
 return qh;
*/
template <typename T>
T asm_try_sub_and_set(T &out, T x, T y) {
    asm (
        "sub "
        :
        : [x] "r"(x), [y] "r"(y)
        : "cc", "memory"
    );
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_DETAILS_HPP__