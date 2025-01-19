#ifndef WJR_ARCH_X86_MATH_ADD_HPP__
#define WJR_ARCH_X86_MATH_ADD_HPP__

#include <wjr/type_traits.hpp>

#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
    #define WJR_HAS_BUILTIN_ASM_ADDC WJR_HAS_DEF
    #define WJR_HAS_BUILTIN___ASM_ADDC_128 WJR_HAS_DEF

    #if WJR_HAS_FEATURE(INLINE_ASM_CCCOND)
        #define WJR_HAS_BUILTIN_ASM_ADDC_CC WJR_HAS_DEF
        #define WJR_HAS_BUILTIN___ASM_ADDC_CC_128 WJR_HAS_DEF
    #endif
#else
    #if defined(_MSC_VER)
        #define WJR_HAS_BUILTIN_ASM_ADDC WJR_HAS_SIMD_DEF
    #endif
#endif

#if WJR_HAS_BUILTIN(ASM_ADDC) == WJR_SIMD_DEF_VAR
    #include <wjr/arch/x86/simd/intrin.hpp>
#endif

namespace wjr::math {

#if WJR_HAS_BUILTIN(ASM_ADDC)

/**
 * @brief Use inline assembly to add two 64-bit integers with carry-in and
 * return the carry-out.
 *
 * @details The carry-in and carry-out flags are both 0 or 1. \n
 * The carry-out flag is set to 1 if the result overflows. \n
 * Optimization: \n
 * 1. Use constraint "i" if a or b is a constant and is in i32 range. \n
 * 2. If c_in is a constant and c_in == 1, use "stc" to set the carry flag.
 *
 * @tparam U The type of the carry.
 * @param[in] c_in The carry-in flag.
 * @param[out] c_out The carry-out flag.
 * @return a + b + c_in
 */
template <typename U>
WJR_INTRINSIC_INLINE uint64_t asm_addc(uint64_t a, uint64_t b, U c_in, U &c_out) noexcept {
    #if WJR_HAS_BUILTIN(ASM_ADDC) == 1
    if (WJR_BUILTIN_CONSTANT_P_TRUE(c_in == 1)) {
        if (WJR_BUILTIN_CONSTANT_P(b) && in_range<int32_t>(b)) {
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
        if (in_range<int32_t>(a)) {
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

    if (WJR_BUILTIN_CONSTANT_P(b) && in_range<int32_t>(b)) {
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
    #else
    uint64_t ret;
    c_out = fast_cast<U>(_addcarry_u64(fast_cast<unsigned char>(c_in), a, b, &ret));
    return ret;
    #endif
}

#endif

#if WJR_HAS_BUILTIN(ASM_ADDC_CC)

/**
 * @brief Use inline assembly to add two 64-bit integers with carry-in and
 * return the carry-out.
 *
 * @details Similar to asm_addc, but the carry-out flag is set by using
 * constraint
 * "=@cccond" instead of "setb". \n
 *
 * @param[in] c_in
 * @param[out] c_out
 */
WJR_INTRINSIC_INLINE uint64_t asm_addc_cc(uint64_t a, uint64_t b, uint8_t c_in,
                                          uint8_t &c_out) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(c_in == 1)) {
        if (WJR_BUILTIN_CONSTANT_P(b) && in_range<int32_t>(b)) {
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
        if (in_range<int32_t>(a)) {
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

    if (WJR_BUILTIN_CONSTANT_P(b) && in_range<int32_t>(b)) {
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

#if WJR_HAS_BUILTIN(__ASM_ADDC_128) || WJR_HAS_BUILTIN(__ASM_ADDC_CC_128)

/**
 * @brief Use inline assembly to add two 64-bit integers and return the
 * carry-out.
 *
 * @details Optimzation for __asm_addc_cc_128 and __asm_addc_128 when the
 * carry-in is 0.
 *
 */
WJR_INTRINSIC_INLINE uint8_t __asm_addc_cc_zero_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                    uint64_t hi0, uint64_t lo1,
                                                    uint64_t hi1) noexcept {
    uint8_t c_out;
    if (WJR_BUILTIN_CONSTANT_P(hi0) && hi0 <= UINT32_MAX) {
        asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi1] "+r"(hi1), WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi0] "i"(hi0)
            : "cc");
        al = lo0;
        ah = hi1;
        return c_out;
    } else if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_out;
    }

    asm("add{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), WJR_ASM_CCOUT(c)(c_out)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_out;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_ADDC_128)

/**
 * @brief Use inline assembly to add two 64-bit integers with carry-in and
 * return the carry-out.
 *
 */
WJR_INTRINSIC_INLINE uint64_t __asm_addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                             uint64_t lo1, uint64_t hi1, uint64_t c_in) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(c_in == 0)) {
        return __asm_addc_cc_zero_128(al, ah, lo0, hi0, lo1, hi1);
    }

    if (WJR_BUILTIN_CONSTANT_P(hi0) && hi0 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
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
            "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
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
        "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
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

#if WJR_HAS_BUILTIN(__ASM_ADDC_CC_128)

/**
 * @brief Use inline assembly to add two 64-bit integers with carry-in and
 * return the carry-out.
 *
 * @details Similar to __asm_addc_128, but the carry-out flag is set by using
 * constraint
 * "=@cccond" instead of "setb".
 *
 */
WJR_INTRINSIC_INLINE uint8_t __asm_addc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                               uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                               uint8_t c_in) noexcept {
    if (WJR_BUILTIN_CONSTANT_P_TRUE(c_in == 0)) {
        return __asm_addc_cc_zero_128(al, ah, lo0, hi0, lo1, hi1);
    }

    uint8_t c_out;
    if (WJR_BUILTIN_CONSTANT_P(hi0) && hi0 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi1] "+r"(hi1), [c_in] "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi0] "i"(hi0)
            : "cc");
        al = lo0;
        ah = hi1;
        return c_out;
    } else if (WJR_BUILTIN_CONSTANT_P(hi1) && hi1 <= UINT32_MAX) {
        asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
            "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
            "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
            : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
            : [lo1] "r"(lo1), [hi1] "i"(hi1)
            : "cc");
        al = lo0;
        ah = hi0;
        return c_out;
    }

    asm("add{b $0xff, %b[c_in]| %b[c_in], 0xff}\n\t"
        "adc{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "adc{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(c)
        : [lo0] "+&r"(lo0), [hi0] "+r"(hi0), [c_in] "+&r"(c_in), WJR_ASM_CCOUT(c)(c_out)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    al = lo0;
    ah = hi0;
    return c_out;
}

#endif

} // namespace wjr::math

#endif // WJR_ARCH_X86_MATH_ADD_HPP__