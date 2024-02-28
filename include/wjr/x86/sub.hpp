#ifndef WJR_X86_SUB_HPP__
#define WJR_X86_SUB_HPP__

#include <wjr/type_traits.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_SUBC WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_SUBC_N WJR_HAS_DEF

template <typename U>
WJR_INTRINSIC_INLINE uint64_t asm_subc(uint64_t a, uint64_t b, U c_in, U &c_out) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 1) && c_in == 1) {
        c_in = 0;
        if (WJR_BUILTIN_CONSTANT_P(b) && b <= std::numeric_limits<uint32_t>::max()) {
            asm("stc\n\t"
                "sbb{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
                : "ri"(b), "0"(a)
                : "cc");
        } else {
            asm("stc\n\t"
                "sbb{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
                : "r"(b), "0"(a)
                : "cc");
        }
        c_out = c_in;
        return a;
    }

    if (WJR_BUILTIN_CONSTANT_P(b) && b <= std::numeric_limits<uint32_t>::max()) {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "sbb{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(a), "+r"(c_in)
            : "ri"(b), "0"(a)
            : "cc");
    } else {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "sbb{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(a), "+r"(c_in)
            : "r"(b), "0"(a)
            : "cc");
    }
    c_out = c_in;
    return a;
}

#define WJR_ADDSUB_I 0
#include <wjr/x86/gen_addsub.hpp>

#endif

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN___ASM_SUBC_128 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(__ASM_SUBC_128)

WJR_INTRINSIC_INLINE void __asm_subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                         uint64_t hi0, uint64_t lo1, uint64_t hi1) {
    if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= (uint32_t)in_place_max) {
        asm("sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc", "memory");
        al = lo0;
        ah = hi0;
        return;
    }
    asm("sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc", "memory");
    al = lo0;
    ah = hi0;
}

WJR_INTRINSIC_INLINE uint64_t __asm_subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                             uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                             uint64_t c_in) {
    if (WJR_BUILTIN_CONSTANT_P(hi0) && hi0 <= (uint32_t)in_place_max) {
        asm("addb {$0xff, %b[c_in]|%b[c_in], 0xff}\n\t"
            "sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t"
            "setb %b[c_in]"
            : [lo0] "+&r"(lo0), [hi1] "+r"(hi1), [c_in] "+r"(c_in)
            : [lo1] "r"(lo1), [hi0] "i"(hi0)
            : "cc", "memory");
        al = lo0;
        ah = hi1;
        return c_in;
    } else if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= (uint32_t)in_place_max) {
        asm("addb {$0xff, %b[c_in]|%b[c_in], 0xff}\n\t"
            "sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
            "setb %b[c_in]"
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+r"(c_in)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc", "memory");
        al = lo0;
        ah = hi0;
        return c_in;
    }

    asm("addb {$0xff, %b[c_in]|%b[c_in], 0xff}\n\t"
        "sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
        "setb %b[c_in]"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+r"(c_in)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc", "memory");
    al = lo0;
    ah = hi0;
    return c_in;
}

#endif

} // namespace wjr

#endif // WJR_X86_SUB_HPP__