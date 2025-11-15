/**
 * @file mul.hpp
 * @brief Multiplication operations for multi-precision arithmetic
 * @author wjr
 *
 * Provides optimized multiplication functions including full-precision multiply
 * that returns both the low and high parts of the product.
 *
 * @todo Optimize temporary memory usage of mul_s, mul_n, sqr
 */

#ifndef WJR_MATH_MUL_HPP__
#define WJR_MATH_MUL_HPP__

#include <wjr/math/add.hpp>
#include <wjr/math/ctz.hpp>
#include <wjr/math/detail.hpp>

#if defined(_MSC_VER) && defined(WJR_ARCH_X86)
    #define WJR_HAS_BUILTIN_MSVC_MULH64 WJR_HAS_DEF
#endif

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/math/mul.hpp>
#endif

#if WJR_HAS_BUILTIN(MSVC_MULH64)
    #include <wjr/arch/x86/simd/intrin.hpp>
#endif

namespace wjr {

template <typename T>
WJR_INTRINSIC_CONSTEXPR T fallback_mul(T a, T b, T &hi) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;
    using T2 = uint_t<nd * 2>;
    T2 x = static_cast<T2>(a) * b;
    hi = x >> nd;
    return static_cast<T>(x);
}

WJR_INTRINSIC_CONSTEXPR uint64_t fallback_mul64(uint64_t a, uint64_t b, uint64_t &hi) noexcept {
    uint64_t ah = a >> 32;
    uint64_t al = a & 0xFFFFFFFF;
    uint64_t bh = b >> 32;
    uint64_t bl = b & 0xFFFFFFFF;

    uint64_t rh = ah * bh;
    uint64_t rm0 = ah * bl;
    uint64_t rm1 = al * bh;
    uint64_t rl = al * bl;

    add_128(rl, rh, rl, rh, rm0 << 32, rm0 >> 32);
    add_128(rl, rh, rl, rh, rm1 << 32, rm1 >> 32);

    hi = rh;
    return rl;
}

WJR_INTRINSIC_CONSTEXPR uint64_t _mul_u64(uint64_t a, uint64_t b, uint64_t &hi) noexcept {
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P(a)) {
        if (a == 0) {
            hi = 0;
            return 0;
        }

        if (is_zero_or_single_bit(a)) {
            const auto shift = constant::ctz(a);
            hi = b >> (64 - shift);
            return b << shift;
        }
    }

    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P(b)) {
        if (b == 0) {
            hi = 0;
            return 0;
        }

        if (is_zero_or_single_bit(b)) {
            const auto shift = constant::ctz(b);
            hi = a >> (64 - shift);
            return a << shift;
        }
    }

#if WJR_HAS_BUILTIN(UMUL128)
    if (is_constant_evaluated()
    #if WJR_HAS_BUILTIN(ASM_UMUL128)
        || (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))
    #endif
    ) {
        return fallback_mul64(a, b, hi);
    }

    #if WJR_HAS_BUILTIN(ASM_UMUL128)
    // mov b to rax, then mul a
    // instead of mov a to rax, mov b to register, then mul
    if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P(b)) {
        return builtin_umul128(b, a, hi);
    }
    #endif
    return builtin_umul128(a, b, hi);
#else
    return fallback_mul64(a, b, hi);
#endif
}

/**
 * @brief Full-precision multiplication returning both low and high parts
 *
 * Multiplies two unsigned integers and returns the full double-width product.
 * The low part is returned, while the high part is stored in the hi parameter.
 *
 * @tparam T Unsigned integral type
 * @param[in] a First multiplicand
 * @param[in] b Second multiplicand
 * @param[out] hi Reference to store the high part of the product
 * @return T The low part of the product
 */
template <nonbool_unsigned_integral T>
WJR_INTRINSIC_CONSTEXPR T mul(T a, T b, T &hi) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

    if constexpr (nd < 64) {
        return fallback_mul(a, b, hi);
    } else {
        return _mul_u64(a, b, hi);
    }
}

/**
 * @brief Get the high part of a full-precision multiplication
 *
 * Returns only the high part of the double-width product of two integers.
 * Optimized to avoid computing the low part when not needed.
 *
 * @tparam T Unsigned integral type
 * @param[in] a First multiplicand
 * @param[in] b Second multiplicand
 * @return T The high part of the product
 */
template <nonbool_unsigned_integral T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T mulhi(T a, T b) noexcept {
#if WJR_HAS_BUILTIN(MSVC_MULH64)
    constexpr auto nd = std::numeric_limits<T>::digits;
    if constexpr (nd < 64) {
#endif
        T ret = 0;
        (void)mul(a, b, ret);
        return ret;
#if WJR_HAS_BUILTIN(MSVC_MULH64)
    } else {
        return __umulh(a, b);
    }
#endif
}

/**
 * @brief Get the low part of a multiplication
 *
 * Returns only the low part of the product. This is equivalent to
 * standard multiplication but provided for API consistency.
 *
 * @tparam T Unsigned integral type
 * @param[in] a First multiplicand
 * @param[in] b Second multiplicand
 * @return T The low part of the product
 */
template <nonbool_unsigned_integral T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T mullo(T a, T b) noexcept {
    return a * b;
}

#if WJR_HAS_BUILTIN(__builtin_mul_overflow)
    #define WJR_HAS_BUILTIN_MUL_OVERFLOW WJR_HAS_DEF
#endif

template <typename T>
WJR_INTRINSIC_CONSTEXPR bool fallback_mul_overflow(T a, T b, T &ret) noexcept {
    T hi;
    ret = mul(a, b, hi);
    return hi != 0;
}

/**
 * @brief Multiply with overflow detection
 *
 * Multiplies two integers and detects if the result overflows the type.
 * Returns true if overflow occurred, false otherwise.
 *
 * @tparam T Unsigned integral type
 * @param[in] a First multiplicand
 * @param[in] b Second multiplicand
 * @param[out] ret Reference to store the product (low part if overflow)
 * @return bool True if overflow occurred, false otherwise
 */
template <nonbool_unsigned_integral T>
WJR_INTRINSIC_CONSTEXPR bool mul_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret) noexcept {
#if WJR_HAS_BUILTIN(MUL_OVERFLOW)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
        return fallback_mul_overflow(a, b, ret);
    }

    return __builtin_mul_overflow(a, b, &ret);
#else
    return fallback_mul_overflow(a, b, ret);
#endif
}

} // namespace wjr

#endif // WJR_MATH_MUL_HPP__