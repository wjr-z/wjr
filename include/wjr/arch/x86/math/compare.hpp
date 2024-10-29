#ifndef WJR_ARCH_X86_MATH_COMPARE_HPP__
#define WJR_ARCH_X86_MATH_COMPARE_HPP__

#include <wjr/arch/x86/math/large-compare-impl.hpp>

namespace wjr {

#if WJR_HAS_BUILTIN(COMPARE_N)

/**
 * @brief Compare two arrays of uint64_t.
 *
 * @details Inline detection of the first two positions, then use @ref
 * large_builtin_compare_n to compare the rest.
 *
 * @tparam T Requires uint64_t currently.
 * @param src0 Pointer to the first array.
 * @param src1 Pointer to the second array.
 * @param n Number of elements to compare.
 * @return
 * \code
 * negative : src0 < src1
 * 0        : src0 == src1
 * positive : src0 > src1
 * \endcode
 */
template <typename T>
WJR_PURE WJR_INTRINSIC_INLINE int builtin_compare_n(const T *src0, const T *src1,
                                                    size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_LIKELY(src0[0] != src1[0])) {
        return src0[0] < src1[0] ? -1 : 1;
    }

    if (WJR_UNLIKELY(n == 1)) {
        return 0;
    }

    if (WJR_LIKELY(src0[1] != src1[1])) {
        return src0[1] < src1[1] ? -1 : 1;
    }

    if (WJR_UNLIKELY(n == 2)) {
        return 0;
    }

    return large_builtin_compare_n(src0, src1, n);
}

#endif

#if WJR_HAS_BUILTIN(REVERSE_COMPARE_N)

/**
 * @brief Compare two arrays of uint64_t in reverse order.
 *
 * @details @ref builtin_compare_n in reverse order.
 */
template <typename T>
WJR_PURE WJR_INTRINSIC_INLINE int builtin_reverse_compare_n(const T *src0, const T *src1,
                                                            size_t n) noexcept {
    if (WJR_UNLIKELY(n == 0)) {
        return 0;
    }

    if (WJR_LIKELY(src0[n - 1] != src1[n - 1])) {
        return src0[n - 1] < src1[n - 1] ? -1 : 1;
    }

    if (WJR_UNLIKELY(n == 1)) {
        return 0;
    }

    if (WJR_LIKELY(src0[n - 2] != src1[n - 2])) {
        return src0[n - 2] < src1[n - 2] ? -1 : 1;
    }

    if (WJR_UNLIKELY(n == 2)) {
        return 0;
    }

    return large_builtin_reverse_compare_n(src0, src1, n);
}

#endif

// __uint128_t has certain bugs in GCC 13.2, resulting in low performance
#if WJR_HAS_FEATURE(GCC_STYLE_INLINE_ASM)
    #define WJR_HAS_BUILTIN___ASM_LESS_128 WJR_HAS_DEF
    #define WJR_HAS_BUILTIN___ASM_LESS_EQUAL_128 WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(__ASM_LESS_128)

WJR_CONST WJR_INTRINSIC_INLINE bool __asm_less_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                   uint64_t hi1) noexcept {
    bool ret;
    asm("cmp{q %[lo1], %[lo0]| %[lo0], %[lo1]}\n\t"
        "sbb{q %[hi1], %[hi0]| %[hi0], %[hi1]}\n\t" WJR_ASM_CCSET(b)
        : WJR_ASM_CCOUT(b)(ret), [lo0] "+&r"(lo0), [hi0] "+r"(hi0)
        : [lo1] "r"(lo1), [hi1] "r"(hi1)
        : "cc");
    return ret;
}

#endif

#if WJR_HAS_BUILTIN(__ASM_LESS_EQUAL_128)

WJR_CONST WJR_INTRINSIC_INLINE bool __asm_less_equal_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                         uint64_t hi1) noexcept {
    bool ret;
    asm("cmp{q %[lo0], %[lo1]| %[lo1], %[lo0]}\n\t"
        "sbb{q %[hi0], %[hi1]| %[hi1], %[hi0]}\n\t" WJR_ASM_CCSET(ae)
        : WJR_ASM_CCOUT(ae)(ret), [lo1] "+&r"(lo1), [hi1] "+r"(hi1)
        : [lo0] "r"(lo0), [hi0] "r"(hi0)
        : "cc");
    return ret;
}

#endif

} // namespace wjr

#endif // WJR_ARCH_X86_MATH_COMPARE_HPP__