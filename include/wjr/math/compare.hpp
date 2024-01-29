#ifndef WJR_MATH_CMP_HPP__
#define WJR_MATH_CMP_HPP__

#include <wjr/math/sub-impl.hpp>
#include <wjr/type_traits.hpp>

#if defined(WJR_X86)
#include <wjr/x86/compare.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR int fallback_compare_n(const T *src0, const T *src1, size_t n) {
    for (size_t idx = 0; idx < n; ++idx) {
        if (src0[idx] != src1[idx]) {
            return src0[idx] < src1[idx] ? -1 : 1;
        }
    }

    return 0;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E int compare_n(const T *src0, const T *src1, size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return 0;
    }

#if WJR_HAS_BUILTIN(COMPARE_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_compare_n(src0, src1, n);
        }

        return builtin_compare_n(src0, src1, n);
    } else {
        return fallback_compare_n(src0, src1, n);
    }
#else
    return fallback_compare_n(src0, src1, n);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR int fallback_reverse_compare_n(const T *src0, const T *src1,
                                                       size_t n) {
    src0 += n;
    src1 += n;

    for (size_t idx = 0; idx < n; ++idx) {
        if (src0[-1 - idx] != src1[-1 - idx]) {
            return src0[-1 - idx] < src1[-1 - idx] ? -1 : 1;
        }
    }

    return 0;
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR_E int reverse_compare_n(const T *src0, const T *src1, size_t n) {
    if (WJR_BUILTIN_CONSTANT_P(src0 == src1) && src0 == src1) {
        return 0;
    }

#if WJR_HAS_BUILTIN(COMPARE_N)
    if constexpr (sizeof(T) == 8) {
        if (is_constant_evaluated()) {
            return fallback_reverse_compare_n(src0, src1, n);
        }

        return builtin_reverse_compare_n(src0, src1, n);
    } else {
        return fallback_reverse_compare_n(src0, src1, n);
    }
#else
    return fallback_reverse_compare_n(src0, src1, n);
#endif
}

WJR_INTRINSIC_CONSTEXPR_E bool __fallback_less_128(uint64_t lo0, uint64_t hi0,
                                                   uint64_t lo1, uint64_t hi1) {
    unsigned char f = lo0 < lo1;
    (void)subc(hi0, hi1, f, f);
    return f;
}

// return <hi0, lo0> < <hi1, lo1>
WJR_INTRINSIC_CONSTEXPR_E bool __less_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                          uint64_t hi1) {
#if WJR_HAS_BUILTIN(__ASM_LESS_128)
    if (is_constant_evaluated()) {
        return __fallback_less_128(lo0, hi0, lo1, hi1);
    }
    return __asm_less_128(lo0, hi0, lo1, hi1);
#else
    return __fallback_less_128(lo0, hi0, lo1, hi1);
#endif
}

WJR_INTRINSIC_CONSTEXPR_E bool __fallback_less_equal_128(uint64_t lo0, uint64_t hi0,
                                                         uint64_t lo1, uint64_t hi1) {
    return !__less_128(lo1, hi1, lo0, hi0);
}

// return <hi0, lo0> < <hi1, lo1>
WJR_INTRINSIC_CONSTEXPR_E bool __less_equal_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                uint64_t hi1) {
#if WJR_HAS_BUILTIN(__ASM_LESS_EQUAL_128)
    if (is_constant_evaluated()) {
        return __fallback_less_equal_128(lo0, hi0, lo1, hi1);
    }
    return __asm_less_equal_128(lo0, hi0, lo1, hi1);
#else
    return __fallback_less_equal_128(lo0, hi0, lo1, hi1);
#endif
}

} // namespace wjr

#endif // WJR_MATH_CMP_HPP__