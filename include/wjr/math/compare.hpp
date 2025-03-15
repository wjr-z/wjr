#ifndef WJR_MATH_CMP_HPP__
#define WJR_MATH_CMP_HPP__

#include <wjr/math/sub.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/math/compare.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR int fallback_compare_n(const T *src0, const T *src1, size_t n) noexcept {
    for (size_t idx = 0; idx < n; ++idx) {
        if (src0[idx] != src1[idx]) {
            return src0[idx] < src1[idx] ? -1 : 1;
        }
    }

    return 0;
}

template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
WJR_PURE WJR_INTRINSIC_CONSTEXPR20 int compare_n(const T *src0, const T *src1, size_t n) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(src0 == src1)) {
        return 0;
    }

#if WJR_HAS_BUILTIN(COMPARE_N)
    if constexpr (sizeof(T) == 8 && std::is_unsigned_v<T>) {
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
                                                       size_t n) noexcept {
    src0 += n;
    src1 += n;

    for (size_t idx = 0; idx < n; ++idx) {
        if (src0[-1 - idx] != src1[-1 - idx]) {
            return src0[-1 - idx] < src1[-1 - idx] ? -1 : 1;
        }
    }

    return 0;
}

template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
WJR_PURE WJR_INTRINSIC_CONSTEXPR20 int reverse_compare_n(const T *src0, const T *src1,
                                                         size_t n) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(src0 == src1)) {
        return 0;
    }

#if WJR_HAS_BUILTIN(COMPARE_N)
    if constexpr (sizeof(T) == 8 && std::is_unsigned_v<T>) {
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

WJR_CONST WJR_INTRINSIC_CONSTEXPR20 bool __equal_128_64(uint64_t lo0, uint64_t hi0,
                                                        uint64_t lo1) noexcept {
    return lo0 == lo1 && hi0 == 0;
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR20 bool __not_equal_128_64(uint64_t lo0, uint64_t hi0,
                                                            uint64_t lo1) noexcept {
    return !__equal_128_64(lo0, hi0, lo1);
}

WJR_INTRINSIC_CONSTEXPR20 bool __less_128_64(uint64_t lo0, uint64_t hi0, uint64_t lo1) noexcept {
    return hi0 == 0 && lo0 < lo1;
}

WJR_INTRINSIC_CONSTEXPR20 bool __less_equal_128_64(uint64_t lo0, uint64_t hi0,
                                                   uint64_t lo1) noexcept {
    return hi0 == 0 && lo0 <= lo1;
}

WJR_INTRINSIC_CONSTEXPR20 bool __greater_128_64(uint64_t lo0, uint64_t hi0, uint64_t lo1) noexcept {
    return !__less_equal_128_64(lo0, hi0, lo1);
}

WJR_INTRINSIC_CONSTEXPR20 bool __greater_equal_128_64(uint64_t lo0, uint64_t hi0,
                                                      uint64_t lo1) noexcept {
    return !__less_128_64(lo0, hi0, lo1);
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR20 bool __equal_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                     uint64_t hi1) noexcept {
    return lo0 == lo1 && hi0 == hi1;
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR20 bool __not_equal_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                         uint64_t hi1) noexcept {
    return !__equal_128(lo0, hi0, lo1, hi1);
}

#if WJR_HAS_FEATURE(FAST_INT128_COMPARE)
    #define WJR_HAS_BUILTIN___BUILTIN_LESS_128 WJR_HAS_DEF
    #define WJR_HAS_BUILTIN___BUILTIN_LESS_EQUAL_128 WJR_HAS_DEF
#endif

WJR_INTRINSIC_CONSTEXPR20 bool __fallback_less_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                   uint64_t hi1) noexcept {
    uint8_t f = lo0 < lo1;
    (void)subc_cc(hi0, hi1, f, f);
    WJR_ASSUME(f <= 1);
    return f != 0;
}

#if WJR_HAS_BUILTIN(__BUILTIN_LESS_128)

WJR_INTRINSIC_INLINE bool __builtin_less_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                             uint64_t hi1) noexcept {
    const auto x0 = static_cast<__uint128_t>(hi0) << 64 | lo0;
    const auto x1 = static_cast<__uint128_t>(hi1) << 64 | lo1;

    return x0 < x1;
}

#endif

// return <hi0, lo0> < <hi1, lo1>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 bool __less_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                    uint64_t hi1) noexcept {
#if WJR_HAS_BUILTIN(__BUILTIN_LESS_128) || WJR_HAS_BUILTIN(__ASM_LESS_128)
    if (is_constant_evaluated()) {
        return __fallback_less_128(lo0, hi0, lo1, hi1);
    }

    return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(__BUILTIN_LESS_128), __builtin_less_128,
                          __asm_less_128)(lo0, hi0, lo1, hi1);
#else
    return __fallback_less_128(lo0, hi0, lo1, hi1);
#endif
}

WJR_INTRINSIC_CONSTEXPR20 bool __fallback_less_equal_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                         uint64_t hi1) noexcept {
    return !__less_128(lo1, hi1, lo0, hi0);
}

#if WJR_HAS_BUILTIN(__BUILTIN_LESS_EQUAL_128)

WJR_INTRINSIC_INLINE bool __builtin_less_equal_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                   uint64_t hi1) noexcept {
    const auto x0 = static_cast<__uint128_t>(hi0) << 64 | lo0;
    const auto x1 = static_cast<__uint128_t>(hi1) << 64 | lo1;

    return x0 <= x1;
}

#endif

// return <hi0, lo0> < <hi1, lo1>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 bool __less_equal_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                          uint64_t hi1) noexcept {
#if WJR_HAS_BUILTIN(__BUILTIN_LESS_EQUAL_128) || WJR_HAS_BUILTIN(__ASM_LESS_EQUAL_128)
    if (is_constant_evaluated()) {
        return __fallback_less_equal_128(lo0, hi0, lo1, hi1);
    }

    return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(__BUILTIN_LESS_EQUAL_128), __builtin_less_equal_128,
                          __asm_less_equal_128)(lo0, hi0, lo1, hi1);
#else
    return __fallback_less_equal_128(lo0, hi0, lo1, hi1);
#endif
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR20 bool __greater_128(uint64_t lo0, uint64_t hi0, uint64_t lo1,
                                                       uint64_t hi1) noexcept {
    return __less_128(lo1, hi1, lo0, hi0);
}

WJR_CONST WJR_INTRINSIC_CONSTEXPR20 bool __greater_equal_128(uint64_t lo0, uint64_t hi0,
                                                             uint64_t lo1, uint64_t hi1) noexcept {
    return __less_equal_128(lo1, hi1, lo0, hi0);
}

} // namespace wjr

#endif // WJR_MATH_CMP_HPP__