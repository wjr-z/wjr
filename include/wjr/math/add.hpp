#ifndef WJR_MATH_ADD_HPP__
#define WJR_MATH_ADD_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/add-impl.hpp>
#include <wjr/math/detail.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/math/add.hpp>
#endif

namespace wjr::math {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_addc(T a, T b, U c_in, U &c_out) noexcept {
    T ret = a;
    ret += b;
    U c = ret < b;
    ret += c_in;
    c |= ret < c_in;
    c_out = c;
    return ret;
}

#if WJR_HAS_BUILTIN(__builtin_add_overflow)
    #define WJR_HAS_BUILTIN_ADD_OVERFLOW WJR_HAS_DEF
#endif

// todo : Support GCC optimization, GCC don't support size of 1, 2. Support add_overflow.
#if WJR_HAS_BUILTIN(__builtin_addcb) && WJR_HAS_BUILTIN(__builtin_addcll)
    #define WJR_HAS_BUILTIN_ADDC WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(ADDC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T builtin_addc(T a, T b, U c_in, U &c_out) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

    #define WJR_REGISTER_BUILTIN_ADDC(suffix, type)                                                \
        if constexpr (nd <= std::numeric_limits<type>::digits) {                                   \
            type __c_out;                                                                          \
            T ret = __builtin_addc##suffix(a, b, static_cast<type>(c_in), &__c_out);               \
            c_out = static_cast<U>(__c_out);                                                       \
            return ret;                                                                            \
        } else

    WJR_REGISTER_BUILTIN_ADDC(b, unsigned char)
    WJR_REGISTER_BUILTIN_ADDC(s, unsigned short)
    WJR_REGISTER_BUILTIN_ADDC(, unsigned int)
    WJR_REGISTER_BUILTIN_ADDC(l, unsigned long)
    WJR_REGISTER_BUILTIN_ADDC(ll, unsigned long long) {
        static_assert(nd <= 64, "not supported yet");
    }

    #undef WJR_REGISTER_BUILTIN_ADDC
}

#endif // WJR_HAS_BUILTIN(ADDC)

/**
 * @brief Add two numbers with carry-in, and return the result and carry-out
 *
 * @details If c_out is needed, and its type is not `uint8_t`, then this maybe faster than addc_cc,
 * because addc_cc maybe need to cast `uint8_t` to type `U`.
 *
 * @note The carry-in and carry-out are limited to 0 and 1
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral
 * type. the default type is the same as `T`
 * @param[in] c_in The carry-in flag.
 * @param[out] c_out The carry-out flag.
 *
 * @return a + b + c_in
 */
template <typename T, typename U,
          WJR_REQUIRES_I(is_nonbool_unsigned_integral_v<T> &&is_unsigned_integral_v<U>)>
WJR_INTRINSIC_CONSTEXPR20 T addc(T a, T b, type_identity_t<U> c_in, U &c_out) noexcept {
    WJR_ASSERT_ASSUME_L2(c_in <= 1);

#if !WJR_HAS_BUILTIN(ADDC) && !WJR_HAS_BUILTIN(ASM_ADDC)
    return fallback_addc(a, b, c_in, c_out);
#else
    constexpr auto is_constant_or_zero = [](auto x) -> int {
        return WJR_BUILTIN_CONSTANT_P_TRUE(x == 0) ? 2 : WJR_BUILTIN_CONSTANT_P(x) ? 1 : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when
    // enabling optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than
        // 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >= 2)) {
        return fallback_addc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return WJR_PP_BOOL_IF_NE(
            WJR_HAS_BUILTIN(ASM_ADDC), asm_addc,
            WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ADDC), builtin_addc, fallback_addc))(a, b, c_in, c_out);
    } else {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(ADDC), builtin_addc, fallback_addc)(a, b, c_in,
                                                                                  c_out);
    }
#endif
}

/**
 * @brief Performs addition with carry-in and carry-out, optimized for
 * subsequent branching based on carry-out.
 *
 * @details This function, `addc_cc`, adds two numbers with a carry-in, and
 * returns the result and a carry-out. The carry-out (`c_out`) is optimized for
 * subsequent code that branches based on its value. For example, it can be used
 * with jump instructions like `je` or `jne`. This is in contrast to the `addc`
 * function, which may use instructions like `setc` or `test` for branching.
 *
 * @note The carry-in and carry-out are limited to 0 and 1
 * @tparam U Type of the carry-in and carry-out. It must be an unsigned integral
 * type.
 * @param[in] c_in The carry-in flag.
 * @param[out] c_out The carry-out flag.
 */
template <typename T, WJR_REQUIRES_I(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 T addc_cc(T a, T b, uint8_t c_in, uint8_t &c_out) noexcept {
    WJR_ASSERT_ASSUME_L2(c_in <= 1);

#if WJR_HAS_BUILTIN(ASM_ADDC_CC)
    constexpr auto is_constant_or_zero = [](auto x) -> int {
        return WJR_BUILTIN_CONSTANT_P_TRUE(x == 0) ? 2 : WJR_BUILTIN_CONSTANT_P(x) ? 1 : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when
    // enabling optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than
        // 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >= 2)) {
        return fallback_addc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return asm_addc_cc(a, b, c_in, c_out);
    } else {
        return addc(a, b, c_in, c_out);
    }
#else
    return addc(a, b, c_in, c_out);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR20 bool fallback_add_overflow(T a, T b, T &ret) noexcept {
    ret = a + b;
    return ret < a;
}

template <typename T, WJR_REQUIRES_I(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 bool add_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret) noexcept {
#if WJR_HAS_BUILTIN(ADD_OVERFLOW)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
        return fallback_add_overflow(a, b, ret);
    }

    return __builtin_add_overflow(a, b, &ret);
#else
    return fallback_add_overflow(a, b, ret);
#endif
}

/// @brief <ah, al> = <hi0, lo0> + <hi1, lo1>
WJR_INTRINSIC_CONSTEXPR void add_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                     uint64_t lo1, uint64_t hi1) noexcept {
    const uint64_t __al = lo0 + lo1;
    ah = hi0 + hi1 + (__al < lo0);
    al = __al;
}

WJR_INTRINSIC_CONSTEXPR20 uint64_t __fallback_addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                       uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                                       uint64_t c_in) noexcept {
    al = addc(lo0, lo1, c_in, c_in);
    ah = addc(hi0, hi1, c_in, c_in);
    return c_in;
}

/**
 * @return carry-out
 */
WJR_INTRINSIC_CONSTEXPR20 uint64_t __addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                              uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                              uint64_t c_in) noexcept {
#if WJR_HAS_BUILTIN(__ASM_ADDC_128)
    if (is_constant_evaluated()) {
        return __fallback_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
    }

    return __asm_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return __fallback_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#endif
}

WJR_INTRINSIC_CONSTEXPR20 uint8_t __addc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                                uint8_t c_in) noexcept {
#if WJR_HAS_BUILTIN(__ASM_ADDC_CC_128)
    if (is_constant_evaluated()) {
        return truncate_cast<uint8_t>(__fallback_addc_128(al, ah, lo0, hi0, lo1, hi1, c_in));
    }

    return __asm_addc_cc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return truncate_cast<uint8_t>(__addc_128(al, ah, lo0, hi0, lo1, hi1, c_in));
#endif
}

} // namespace wjr::math

#endif // WJR_MATH_ADD_HPP__