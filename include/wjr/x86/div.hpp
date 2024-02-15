#ifndef WJR_X86_DIV_HPP__
#define WJR_X86_DIV_HPP__

#include <wjr/type_traits.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_DIVEXACT_DBM1C WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ASM_DIVEXACT_DBM1C)

inline uint64_t asm_divexact_dbm1c(uint64_t *dst, const uint64_t *src, size_t n,
                                   uint64_t bd, uint64_t h) {
    uint64_t r8 = h, r9 = n, r10, r11 = static_cast<uint32_t>(n);

    src += r9;
    dst += r9;
    r9 = -r9;

    asm volatile(
        "and{l $3, %k[r11]| %k[r11], 3}\n\t"
        "je .Lb0%=\n\t"
        "lea{q -4(%[r9], %[r11], 1), %[r9]| %[r9], [%[r9] + %[r11] * 1 - 4]}\n\t"
        "cmp{l $2, %k[r11]| %k[r11], 2}\n\t"
        "jb .Lb1%=\n\t"
        "je .Lb2%=\n\t"
        "jmp .Lb3%=\n\t"

        ".Lloop%=:\n\t"

        ".Lb0%=:\n\t"
        "mov{q (%[src], %[r9], 8), %[r10]| %[r10], [%[src] + %[r9] * 8]}\n\t"
        "mul{q %[bd]| %[bd]}\n\t"
        "sub{q %[r10], %[r8]| %[r8], %[r10]}\n\t"
        "mov{q %[r8], (%[dst], %[r9], 8)| [%[dst] + %[r9] * 8], %[r8]}\n\t"
        "sbb{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb3%=:\n\t"
        "mov{q 8(%[src], %[r9], 8), %[r10]| %[r10], [%[src] + %[r9] * 8 + 8]}\n\t"
        "mul{q %[bd]| %[bd]}\n\t"
        "sub{q %[r10], %[r8]| %[r8], %[r10]}\n\t"
        "mov{q %[r8], 8(%[dst], %[r9], 8)| [%[dst] + %[r9] * 8 + 8], %[r8]}\n\t"
        "sbb{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb2%=:\n\t"
        "mov{q 16(%[src], %[r9], 8), %[r10]| %[r10], [%[src] + %[r9] * 8 + 16]}\n\t"
        "mul{q %[bd]| %[bd]}\n\t"
        "sub{q %[r10], %[r8]| %[r8], %[r10]}\n\t"
        "mov{q %[r8], 16(%[dst], %[r9], 8)| [%[dst] + %[r9] * 8 + 16], %[r8]}\n\t"
        "sbb{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        ".Lb1%=:\n\t"
        "mov{q 24(%[src], %[r9], 8), %[r10]| %[r10], [%[src] + %[r9] * 8 + 24]}\n\t"
        "mul{q %[bd]| %[bd]}\n\t"
        "sub{q %[r10], %[r8]| %[r8], %[r10]}\n\t"
        "mov{q %[r8], 24(%[dst], %[r9], 8)| [%[dst] + %[r9] * 8 + 24], %[r8]}\n\t"
        "sbb{q %[r11], %[r8]| %[r8], %[r11]}\n\t"

        "add $4, %[r9]\n\t"
        "jne .Lloop%=\n\t"

        : [dst] "+r"(dst), [src] "+r"(src), [bd] "+r"(bd), [r8] "+r"(r8), [r9] "+r"(r9),
          [r10] "=a"(r10), [r11] "+d"(r11)
        :
        : "cc", "memory");

    return r8;
}

#endif

} // namespace wjr

#endif // WJR_X86_DIV_HPP__