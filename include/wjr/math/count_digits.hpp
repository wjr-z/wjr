/**
 * @file count_digits.hpp
 * @brief Utilities for counting digits in different bases
 * @author wjr
 *
 * Provides optimized functions for counting the number of digits required to
 * represent a number in various bases (binary, octal, decimal, hexadecimal).
 * These functions are useful for formatting, buffer allocation, and general
 * numeric analysis.
 */

#ifndef WJR_MATH_COUNT_DIGITS_HPP__
#define WJR_MATH_COUNT_DIGITS_HPP__

#include <wjr/math/bit.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

/**
 * @brief Fallback implementation for counting base-10 digits
 * @details Uses division-based approach, optimized for compile-time evaluation
 */
template <typename UnsignedValue>
WJR_INTRINSIC_CONSTEXPR int fallback_count_digits10(UnsignedValue n) noexcept {
    int count = 0;

    if (WJR_UNLIKELY(n >= 1000)) {
        do {
            n /= 10000;
            count += 4;
        } while (n >= 1000);

        if (n == 0) {
            return count;
        }
    }

    if (n < 10) {
        return count + 1;
    }

    if (n < 100) {
        return count + 2;
    }

    return count + 3;
}

/// @private Internal implementation details for count_digits
namespace charconv_detail {

#define WJR_INC(T) (((sizeof(#T) - 1ull) << 32) - T)

inline constexpr uint64_t _count_digits10_u32_table[] = {
    WJR_INC(0),          WJR_INC(0),          WJR_INC(0),          // 8
    WJR_INC(10),         WJR_INC(10),         WJR_INC(10),         // 64
    WJR_INC(100),        WJR_INC(100),        WJR_INC(100),        // 512
    WJR_INC(1000),       WJR_INC(1000),       WJR_INC(1000),       // 4096
    WJR_INC(10000),      WJR_INC(10000),      WJR_INC(10000),      // 32k
    WJR_INC(100000),     WJR_INC(100000),     WJR_INC(100000),     // 256k
    WJR_INC(1000000),    WJR_INC(1000000),    WJR_INC(1000000),    // 2048k
    WJR_INC(10000000),   WJR_INC(10000000),   WJR_INC(10000000),   // 16M
    WJR_INC(100000000),  WJR_INC(100000000),  WJR_INC(100000000),  // 128M
    WJR_INC(1000000000), WJR_INC(1000000000), WJR_INC(1000000000), // 1024M
    WJR_INC(1000000000), WJR_INC(1000000000)                       // 4B
};

#undef WJR_INC

#define WJR_POWERS_OF_10(factor)                                                                   \
    factor * 10, (factor) * 100, (factor) * 1000, (factor) * 10000, (factor) * 100000,             \
        (factor) * 1000000, (factor) * 10000000, (factor) * 100000000, (factor) * 1000000000

inline constexpr uint8_t _count_digits10_u64_bsr2log10[] = {
    1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,
    7,  8,  8,  8,  9,  9,  9,  10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14,
    14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20};

inline constexpr uint64_t _count_digits10_u64_zero_or_powers_of_10[] = {
    0, 0, WJR_POWERS_OF_10(1U), WJR_POWERS_OF_10(1000000000ull), 10000000000000000000ull};

#undef WJR_POWERS_OF_10

} // namespace charconv_detail

/**
 * @brief Optimized base-10 digit counting for 32-bit unsigned integers
 * @details Uses lookup table and CLZ (count leading zeros) for O(1) performance
 */
WJR_INTRINSIC_CONSTEXPR20 int builtin_count_digits10_u32(uint32_t n) noexcept {
    const auto inc = charconv_detail::_count_digits10_u32_table[clz(n | 1) ^ 31];
    return static_cast<int>((n + inc) >> 32);
}

/**
 * @brief Optimized base-10 digit counting for 64-bit unsigned integers
 * @details Uses lookup table and CLZ for O(1) performance
 */
WJR_INTRINSIC_CONSTEXPR20 int builtin_count_digits10_u64(uint64_t n) noexcept {
    const auto t = charconv_detail::_count_digits10_u64_bsr2log10[clz(n | 1) ^ 63];
    return t - (n < charconv_detail::_count_digits10_u64_zero_or_powers_of_10[t]);
}

/**
 * @brief Internal implementation dispatcher for base-10 digit counting
 */
template <typename T>
WJR_INTRINSIC_CONSTEXPR20 int count_digits10_impl(T n) noexcept {
    if (is_constant_evaluated() || WJR_BUILTIN_CONSTANT_P(n)) {
        return fallback_count_digits10(n);
    }

    if constexpr (sizeof(T) <= sizeof(uint32_t)) {
        return builtin_count_digits10_u32(static_cast<uint32_t>(n));
    } else {
        return builtin_count_digits10_u64(static_cast<uint64_t>(n));
    }
}

/**
 * @brief Functor for counting digits in a specific base
 * @tparam Base The numeric base (2, 8, 10, 16, or 1 for dynamic base)
 */
template <uint64_t Base>
struct count_digits_fn {};

/**
 * @brief Global constant for counting digits in a specific base
 * @tparam Base The numeric base to count in
 *
 * @code
 * int digits = count_digits<10>(12345);  // Returns 5
 * int bits = count_digits<2>(255);       // Returns 8
 * @endcode
 */
template <uint64_t Base>
inline constexpr count_digits_fn<Base> count_digits{};

/// @private Specialization for binary (base-2) digit counting
template <>
struct count_digits_fn<2> {
    template<nonbool_unsigned_integral T>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int operator()(T n) const noexcept {
        return bit_width(n);
    }
};

/// @private Specialization for octal (base-8) digit counting
template <>
struct count_digits_fn<8> {
    template<nonbool_unsigned_integral T>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int operator()(T n) const noexcept {
        return _ceil_div(to_unsigned(bit_width(n)), 3);
    }
};

/// @private Specialization for hexadecimal (base-16) digit counting
template <>
struct count_digits_fn<16> {
    template<nonbool_unsigned_integral T>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int operator()(T n) const noexcept {
        return _ceil_div(to_unsigned(bit_width(n)), 4);
    }
};

/**
 * @brief Specialization for dynamic power-of-2 bases
 * @details When Base=1, the bit count is provided as a runtime parameter
 */
template <>
struct count_digits_fn<1> {
    template<nonbool_unsigned_integral T>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int operator()(T n, int bits) const noexcept {
        return _ceil_div(to_unsigned(bit_width(n)), bits);
    }
};

/// @private Specialization for decimal (base-10) digit counting
template <>
struct count_digits_fn<10> {
    template<nonbool_unsigned_integral T>
    WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int operator()(T n) const noexcept {
        const int ret = count_digits10_impl(n);
        WJR_ASSUME(1 <= ret && ret <= std::numeric_limits<T>::digits10 + 1);
        return ret;
    }
};

} // namespace wjr

#endif // WJR_MATH_COUNT_DIGITS_HPP__
