#ifndef WJR_X86_MATH_ADD_HPP__
#define WJR_X86_MATH_ADD_HPP__

#include <wjr/type_traits.hpp>

#ifndef WJR_X86
#error "x86 required"
#endif

namespace wjr {

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
#define WJR_HAS_BUILTIN_ASM_ADDC WJR_HAS_DEF
#define WJR_HAS_BUILTIN_ASM_ADDC_N WJR_HAS_DEF
#define WJR_HAS_BUILTIN___ASM_ADDC_128 WJR_HAS_DEF

#if WJR_HAS_FEATURE(INLINE_ASM_CCCOND)
#define WJR_HAS_BUILTIN_ASM_ADDC_CC WJR_HAS_DEF
#endif

#endif

#if WJR_HAS_BUILTIN(ASM_ADDC)

template <typename U>
WJR_INTRINSIC_INLINE uint64_t asm_addc(uint64_t a, uint64_t b, U c_in, U &c_out) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 1) && c_in == 1) {
        if (WJR_BUILTIN_CONSTANT_P(b) && b <= std::numeric_limits<uint32_t>::max()) {
            asm("stc\n\t"
                "adc{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
                : "ri"(b), "0"(a)
                : "cc");
        } else {
            asm("stc\n\t"
                "adc{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(a), "+r"(c_in)
                : "r"(b), "0"(a)
                : "cc");
        }
        c_out = c_in;
        return a;
    }

    if (WJR_BUILTIN_CONSTANT_P(a)) {
        if (a <= std::numeric_limits<uint32_t>::max()) {
            asm("add{b $255, %b1| %b1, 255}\n\t"
                "adc{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(b), "+&r"(c_in)
                : "ri"(a), "0"(b)
                : "cc");
        } else {
            asm("add{b $255, %b1| %b1, 255}\n\t"
                "adc{q %2, %0| %0, %2}\n\t"
                "setb %b1"
                : "=r"(b), "+&r"(c_in)
                : "r"(a), "0"(b)
                : "cc");
        }
        c_out = c_in;
        return b;
    }

    if (WJR_BUILTIN_CONSTANT_P(b) && b <= std::numeric_limits<uint32_t>::max()) {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "adc{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(a), "+&r"(c_in)
            : "ri"(b), "0"(a)
            : "cc");
    } else {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "adc{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(a), "+&r"(c_in)
            : "r"(b), "0"(a)
            : "cc");
    }
    c_out = c_in;
    return a;
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDC_CC)

WJR_INTRINSIC_INLINE uint64_t asm_addc_cc(uint64_t a, uint64_t b, uint8_t c_in,
                                          uint8_t &c_out) {
    if (WJR_BUILTIN_CONSTANT_P(c_in == 1) && c_in == 1) {
        if (WJR_BUILTIN_CONSTANT_P(b) && b <= std::numeric_limits<uint32_t>::max()) {
            asm("stc\n\t"
                "adc{q %2, %0| %0, %2}\n\t" WJR_ASM_CCSET(c)
                : "=r"(a), WJR_ASM_CCOUT(c)(c_out)
                : "ri"(b), "0"(a)
                : "cc");
        } else {
            asm("stc\n\t"
                "adc{q %2, %0| %0, %2}\n\t" WJR_ASM_CCSET(c)
                : "=r"(a), WJR_ASM_CCOUT(c)(c_out)
                : "r"(b), "0"(a)
                : "cc");
        }
        return a;
    }

    if (WJR_BUILTIN_CONSTANT_P(a)) {
        if (a <= std::numeric_limits<uint32_t>::max()) {
            asm("add{b $255, %b1| %b1, 255}\n\t"
                "adc{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
                : "=r"(b), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
                : "ri"(a), "0"(b)
                : "cc");
        } else {
            asm("add{b $255, %b1| %b1, 255}\n\t"
                "adc{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
                : "=r"(b), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
                : "r"(a), "0"(b)
                : "cc");
        }
        return b;
    }

    if (WJR_BUILTIN_CONSTANT_P(b) && b <= std::numeric_limits<uint32_t>::max()) {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "adc{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
            : "=r"(a), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : "ri"(b), "0"(a)
            : "cc");
    } else {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "adc{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
            : "=r"(a), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : "r"(b), "0"(a)
            : "cc");
    }
    return a;
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDC_N)
#define WJR_ADDSUB_I 1
#include <wjr/x86/math/gen_addsub.hpp>
#endif

#if WJR_HAS_BUILTIN(__ASM_ADDC_128)

WJR_INTRINSIC_INLINE void __asm_addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                         uint64_t hi0, uint64_t lo1, uint64_t hi1) {
    if (WJR_BUILTIN_CONSTANT_P(hi0) && hi0 <= UINT32_MAX) {
        asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi0], %[hi1]| %[hi1], %[hi0]}"
            : [lo0] "+&r"(lo0), [hi1] "+r"(hi1)
            : [lo1] "r"(lo1), [hi0] "i"(hi0)
            : "cc");
        al = lo0;
        ah = hi1;
        return;
    } else if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return;
    }

    asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return;
}

WJR_INTRINSIC_INLINE uint64_t __asm_addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                             uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                             uint64_t c_in) {
    if (WJR_BUILTIN_CONSTANT_P(hi0) && hi0 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t"
            "setb %b[c_in]"
            : [lo0] "+&r"(lo0), [hi1] "+r"(hi1), [c_in] "+&r"(c_in)
            : [lo1] "r"(lo1), [hi0] "i"(hi0)
            : "cc");
        al = lo0;
        ah = hi1;
        return c_in;
    } else if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
            "setb %b[c_in]"
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_in;
    }

    asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
        "add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
        "setb %b[c_in]"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_in;
}

#endif

} // namespace wjr

#endif // WJR_X86_MATH_ADD_HPP__