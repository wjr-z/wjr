#ifndef WJR_ARCH_X86_SUB_HPP__
#define WJR_ARCH_X86_SUB_HPP__

#include <wjr/type_traits.hpp>

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
    #define WJR_HAS_BUILTIN_ASM_SUBC WJR_HAS_DEF
    #define WJR_HAS_BUILTIN_ASM_SUBC_N WJR_HAS_DEF
    #define WJR_HAS_BUILTIN___ASM_SUB_128 WJR_HAS_DEF
    #define WJR_HAS_BUILTIN___ASM_SUBC_128 WJR_HAS_DEF

    #if WJR_HAS_FEATURE(INLINE_ASM_CCCOND)
        #define WJR_HAS_BUILTIN_ASM_SUBC_CC WJR_HAS_DEF
        #define WJR_HAS_BUILTIN___ASM_SUBC_CC_128 WJR_HAS_DEF
    #endif
#else
    #if defined(_MSC_VER)
        #define WJR_HAS_BUILTIN_ASM_SUBC WJR_HAS_DEF_VAR(2)
    #endif

    #if defined(WJR_ENABLE_ASSEMBLY)
        #define WJR_HAS_BUILTIN_ASM_SUBC_N WJR_HAS_DEF_VAR(3)
    #endif
#endif

#if WJR_HAS_BUILTIN(ASM_SUBC) == 2
    #include <wjr/arch/x86/simd/intrin.hpp>
#endif

namespace wjr {

#if WJR_HAS_BUILTIN(ASM_SUBC)

template <typename U>
WJR_INTRINSIC_INLINE uint64_t asm_subc(uint64_t a, uint64_t b, U c_in, U &c_out) noexcept {
    #if WJR_HAS_BUILTIN(ASM_SUBC) == 1
    if (WJR_BUILTIN_CONSTANT_P_TRUE(c_in == 1)) {
        if (WJR_BUILTIN_CONSTANT_P(b) && in_range<int32_t>(b)) {
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

    if (WJR_BUILTIN_CONSTANT_P(b) && in_range<int32_t>(b)) {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "sbb{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(a), "+&r"(c_in)
            : "ri"(b), "0"(a)
            : "cc");
    } else {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "sbb{q %2, %0| %0, %2}\n\t"
            "setb %b1"
            : "=r"(a), "+&r"(c_in)
            : "r"(b), "0"(a)
            : "cc");
    }
    c_out = c_in;
    return a;
    #else
    uint64_t ret;
    c_out = fast_cast<U>(_subborrow_u64(fast_cast<unsigned char>(c_in), a, b, &ret));
    return ret;
    #endif
}

#endif

#if WJR_HAS_BUILTIN(ASM_SUBC_CC)

WJR_INTRINSIC_INLINE uint64_t asm_subc_cc(uint64_t a, uint64_t b, uint8_t c_in,
                                          uint8_t &c_out) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(c_in == 1)) {
        if (WJR_BUILTIN_CONSTANT_P(b) && in_range<int32_t>(b)) {
            asm("stc\n\t"
                "sbb{q %2, %0| %0, %2}\n\t" WJR_ASM_CCSET(c)
                : "=r"(a), WJR_ASM_CCOUT(c)(c_out)
                : "ri"(b), "0"(a)
                : "cc");
        } else {
            asm("stc\n\t"
                "sbb{q %2, %0| %0, %2}\n\t" WJR_ASM_CCSET(c)
                : "=r"(a), WJR_ASM_CCOUT(c)(c_out)
                : "r"(b), "0"(a)
                : "cc");
        }
        return a;
    }

    if (WJR_BUILTIN_CONSTANT_P(b) && in_range<int32_t>(b)) {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "sbb{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
            : "=r"(a), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : "ri"(b), "0"(a)
            : "cc");
    } else {
        asm("add{b $255, %b1| %b1, 255}\n\t"
            "sbb{q %3, %0| %0, %3}\n\t" WJR_ASM_CCSET(c)
            : "=r"(a), "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : "r"(b), "0"(a)
            : "cc");
    }
    return a;
}

#endif

#if WJR_HAS_BUILTIN(ASM_SUBC_N)
    #define WJR_ADDSUB_I 0
    #include <wjr/arch/x86/math/gen_addsub.hpp>
#endif

#if WJR_HAS_BUILTIN(__ASM_SUB_128)

WJR_INTRINSIC_INLINE void __asm_sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                        uint64_t lo1, uint64_t hi1) noexcept {
    if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return;
    }
    asm("sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_SUBC_128) || WJR_HAS_BUILTIN(__ASM_SUBC_CC_128)

WJR_INTRINSIC_INLINE uint8_t __asm_subc_cc_zero_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                    uint64_t hi0, uint64_t lo1,
                                                    uint64_t hi1) noexcept {
    uint8_t c_out;
    if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_out;
    }

    asm("sub{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), WJR_ASM_CCOUT(c)(c_out)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_out;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_SUBC_128)

WJR_INTRINSIC_INLINE uint64_t __asm_subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                             uint64_t lo1, uint64_t hi1, uint64_t c_in) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(c_in == 0)) {
        return __asm_subc_cc_zero_128(al, ah, lo0, hi0, lo1, hi1);
    }

    if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "sbb{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
            "setb %b[c_in]"
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_in;
    }

    asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
        "sbb{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t"
        "setb %b[c_in]"
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_in;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_SUBC_CC_128)

WJR_INTRINSIC_INLINE uint8_t __asm_subc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                               uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                               uint8_t c_in) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(c_in == 0)) {
        return __asm_subc_cc_zero_128(al, ah, lo0, hi0, lo1, hi1);
    }

    uint8_t c_out;
    if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "sbb{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_out;
    }

    asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
        "sbb{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_out;
}

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_SUB_HPP__