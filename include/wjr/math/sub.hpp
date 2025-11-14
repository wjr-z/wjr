/**
 * @file sub.hpp
 * @brief Subtraction operations with borrow support
 * @author wjr
 *
 * Provides optimized subtraction functions with borrow propagation for
 * multi-precision arithmetic. Uses compiler intrinsics when available.
 */

#ifndef WJR_MATH_SUB_HPP__
#define WJR_MATH_SUB_HPP__

#include <wjr/assert.hpp>
#include <wjr/math/detail.hpp>
#include <wjr/math/sub-impl.hpp>

#if defined(WJR_ARCH_X86)
    #include <wjr/arch/x86/math/sub.hpp>
#endif

namespace wjr::math {

template <typename T, typename U>
WJR_INTRINSIC_CONSTEXPR T fallback_subc(T a, T b, U c_in, U &c_out) noexcept {
    T ret = a - b;
    U c = ret > a;
    a = ret;
    ret -= c_in;
    c |= ret > a;
    c_out = c;
    return ret;
}

#if WJR_HAS_BUILTIN(__builtin_sub_overflow)
    #define WJR_HAS_BUILTIN_SUB_OVERFLOW WJR_HAS_DEF
#endif

// todo : Support GCC optimization, GCC don't support size of 1, 2. Support sub_overflow
#if WJR_HAS_BUILTIN(__builtin_subcb) && WJR_HAS_BUILTIN(__builtin_subcll)
    #define WJR_HAS_BUILTIN_SUBC WJR_HAS_DEF
#endif

#if WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U>
WJR_INTRINSIC_INLINE T builtin_subc(T a, T b, U c_in, U &c_out) noexcept {
    constexpr auto nd = std::numeric_limits<T>::digits;

    #define WJR_REGISTER_BUILTIN_SUBC(suffix, type)                                                \
        if constexpr (nd <= std::numeric_limits<type>::digits) {                                   \
            type _c_out;                                                                           \
            T ret = __builtin_subc##suffix(a, b, static_cast<type>(c_in), &_c_out);                \
            c_out = static_cast<U>(_c_out);                                                        \
            return ret;                                                                            \
        } else

    WJR_REGISTER_BUILTIN_SUBC(b, unsigned char)
    WJR_REGISTER_BUILTIN_SUBC(s, unsigned short)
    WJR_REGISTER_BUILTIN_SUBC(, unsigned int)
    WJR_REGISTER_BUILTIN_SUBC(l, unsigned long)
    WJR_REGISTER_BUILTIN_SUBC(ll, unsigned long long) {
        static_assert(nd <= 64, "not supported yet");
    }

    #undef WJR_REGISTER_BUILTIN_SUBC
}

#endif // WJR_HAS_BUILTIN(SUBC)

template <typename T, typename U>
requires(is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>)
WJR_INTRINSIC_CONSTEXPR20 T subc(T a, T b, type_identity_t<U> c_in, U &c_out) noexcept {
    WJR_ASSERT_ASSUME_L2(c_in <= 1);

#if !WJR_HAS_BUILTIN(SUBC) && !WJR_HAS_BUILTIN(ASM_SUBC)
    return fallback_subc(a, b, c_in, c_out);
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
        return fallback_subc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return WJR_PP_BOOL_IF_NE(
            WJR_HAS_BUILTIN(ASM_SUBC), asm_subc,
            WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(SUBC), builtin_subc, fallback_subc))(a, b, c_in, c_out);
    } else {
        return WJR_PP_BOOL_IF(WJR_HAS_BUILTIN(SUBC), builtin_subc, fallback_subc)(a, b, c_in,
                                                                                  c_out);
    }
#endif
}

/*
 Used for subc and then jump according to cc flag. Therefore, the types of c_in
 and c_out are limited to uint8_t, while the default c_in and c_out types of
 normal subc are the same as T, so that the high register is not cleared.
 Currently, GCC/Clang @=cccond cannot know that the high register is not
 cleared, so the performance is worse than the normal version when cc flag is
 not needed immediately.
*/
template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_INTRINSIC_CONSTEXPR20 T subc_cc(T a, T b, uint8_t c_in, uint8_t &c_out) noexcept {
    WJR_ASSERT_ASSUME_L2(c_in <= 1);

#if WJR_HAS_BUILTIN(ASM_SUBC_CC)
    constexpr auto is_constant_or_zero = [](auto x) -> int {
        return WJR_BUILTIN_CONSTANT_P_TRUE(x == 0) ? 2 : WJR_BUILTIN_CONSTANT_P(x) ? 1 : 0;
    };

    // The compiler should be able to optimize the judgment condition of if when
    // enabling optimization. If it doesn't work, then there should be a issue
    if (is_constant_evaluated() ||
        // constant value is zero or constant value number greater or equal than
        // 2
        (is_constant_or_zero(a) + is_constant_or_zero(b) + is_constant_or_zero(c_in) >= 2)) {
        return fallback_subc(a, b, c_in, c_out);
    }

    if constexpr (sizeof(T) == 8) {
        return asm_subc_cc(a, b, c_in, c_out);
    } else {
        return subc(a, b, c_in, c_out);
    }
#else
    return subc(a, b, c_in, c_out);
#endif
}

template <typename T>
WJR_INTRINSIC_CONSTEXPR20 bool fallback_sub_overflow(T a, T b, T &ret) noexcept {
    ret = a - b;
    return ret > a;
}

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_INTRINSIC_CONSTEXPR20 bool sub_overflow(type_identity_t<T> a, type_identity_t<T> b,
                                            T &ret) noexcept {
#if WJR_HAS_BUILTIN(SUB_OVERFLOW)
    if (is_constant_evaluated() || (WJR_BUILTIN_CONSTANT_P(a) && WJR_BUILTIN_CONSTANT_P(b))) {
        return fallback_sub_overflow(a, b, ret);
    }

    return __builtin_sub_overflow(a, b, &ret);
#else
    return fallback_sub_overflow(a, b, ret);
#endif
}

// <ah, al> = <hi0, lo0> - <hi1, lo1>
WJR_INTRINSIC_CONSTEXPR void sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                     uint64_t lo1, uint64_t hi1) noexcept {
    const uint64_t _al = lo0 - lo1;
    ah = hi0 - hi1 - (_al > lo0);
    al = _al;
}

WJR_INTRINSIC_CONSTEXPR20 uint64_t _fallback_subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                                      uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                                      uint64_t c_in) noexcept {
    al = subc(lo0, lo1, c_in, c_in);
    ah = subc(hi0, hi1, c_in, c_in);
    return c_in;
}

// return c_out
WJR_INTRINSIC_CONSTEXPR20 uint64_t _subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                             uint64_t lo1, uint64_t hi1, uint64_t c_in) noexcept {
#if WJR_HAS_BUILTIN(__ASM_SUBC_128)
    if (is_constant_evaluated()) {
        return _fallback_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
    }

    return _asm_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return _fallback_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#endif
}

WJR_INTRINSIC_CONSTEXPR20 uint8_t _subc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                               uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                               uint8_t c_in) noexcept {
#if WJR_HAS_BUILTIN(__ASM_SUBC_CC_128)
    if (is_constant_evaluated()) {
        return truncate_cast<uint8_t>(_fallback_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in));
    }

    return _asm_subc_cc_128(al, ah, lo0, hi0, lo1, hi1, c_in);
#else
    return truncate_cast<uint8_t>(_subc_128(al, ah, lo0, hi0, lo1, hi1, c_in));
#endif
}

} // namespace wjr::math

#endif // WJR_MATH_SUB_HPP__