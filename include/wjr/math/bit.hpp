/**
 * @file bit.hpp
 * @brief Bit manipulation utilities
 * @author wjr
 *
 * Provides bit-level operations including counting leading/trailing zeros,
 * bit width calculation, and power-of-two operations. Similar to C++20 <bit>.
 */

#ifndef WJR_MATH_BIT_HPP__
#define WJR_MATH_BIT_HPP__

#include <wjr/math/clz.hpp>
#include <wjr/math/ctz.hpp>
#include <wjr/math/rotate.hpp>
#include <wjr/memory/detail.hpp>

namespace wjr {
namespace constant {
template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST constexpr int countl_zero(T x) noexcept {
    if (WJR_UNLIKELY(x == 0)) {
        return std::numeric_limits<T>::digits;
    }

    return constant::clz(x);
}

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST constexpr int countr_zero(T x) noexcept {
    if (WJR_UNLIKELY(x == 0)) {
        return std::numeric_limits<T>::digits;
    }

    return constant::ctz(x);
}

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST constexpr int countl_one(T x) noexcept {
    return constant::countl_zero(static_cast<T>(~x));
}

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST constexpr int countr_one(T x) noexcept {
    return constant::countr_zero(static_cast<T>(~x));
}

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST constexpr int bit_width(T x) noexcept {
    return std::numeric_limits<T>::digits - constant::countl_zero(x);
}

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST constexpr T bit_ceil(T x) noexcept {
    if (x <= 1) {
        return T(1);
    }

    if constexpr (std::is_same_v<T, decltype(+x)>) {
        return T(1) << constant::bit_width(T(x - 1));
    } else {
        constexpr int offset_for_ub =
            std::numeric_limits<unsigned>::digits - std::numeric_limits<T>::digits;
        return T(1 << (constant::bit_width(T(x - 1)) + offset_for_ub) >> offset_for_ub);
    }
}

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST constexpr T bit_floor(T x) noexcept {
    if (x != 0) {
        return T{1} << (constant::bit_width(x) - 1);
    }

    return 0;
}
} // namespace constant

/**
 * @brief Check if value is a power of two
 *
 * Returns true if n is a power of two (has exactly one bit set).
 * Equivalent to C++20 std::has_single_bit.
 *
 * @tparam T Unsigned integral type
 * @param[in] n Value to check
 * @return bool True if n is a power of two, false otherwise
 */
template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool has_single_bit(T n) noexcept {
    return (n != 0) && is_zero_or_single_bit(n);
}

/**
 * @brief Count leading zero bits
 *
 * Returns the number of consecutive zero bits starting from the most
 * significant bit. If x is 0, returns the number of bits in T.
 * Equivalent to C++20 std::countl_zero.
 *
 * @tparam T Unsigned integral type
 * @param[in] x Value to count leading zeros
 * @return int Number of leading zero bits
 */
template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int countl_zero(T x) noexcept {
    // If not use __builtin_clz and use popcount, then don't need to handle
    // zero.
#if defined(WJR_CLZ_NOT_FULL)
    if (WJR_UNLIKELY(x == 0)) {
        return std::numeric_limits<T>::digits;
    }
#endif

    return clz(x);
}

/**
 * @brief Count trailing zero bits
 *
 * Returns the number of consecutive zero bits starting from the least
 * significant bit. If x is 0, returns the number of bits in T.
 * Equivalent to C++20 std::countr_zero.
 *
 * @tparam T Unsigned integral type
 * @param[in] x Value to count trailing zeros
 * @return int Number of trailing zero bits
 */
template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int countr_zero(T x) noexcept {
    // If not use __builtin_ctz and use popcount, then don't need to handle
    // zero.
#if defined(WJR_CTZ_NOT_FULL)
    if (WJR_UNLIKELY(x == 0)) {
        return std::numeric_limits<T>::digits;
    }
#endif

    return ctz(x);
}

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int countl_one(T x) noexcept {
    return countl_zero(static_cast<T>(~x));
}

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int countr_one(T x) noexcept {
    return countr_zero(static_cast<T>(~x));
}

/**
 * @brief Calculate bit width of value
 *
 * Returns the number of bits needed to represent x. For x=0, returns 0.
 * Equivalent to C++20 std::bit_width.
 *
 * @tparam T Unsigned integral type
 * @param[in] x Value to calculate bit width
 * @return int Number of bits needed (0 to digits)
 */
template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int bit_width(T x) noexcept {
    return std::numeric_limits<T>::digits - countl_zero(x);
}

/**
 * @brief Round up to next power of two
 *
 * Returns the smallest power of two that is not less than x.
 * If x is 0 or 1, returns 1. Equivalent to C++20 std::bit_ceil.
 *
 * @tparam T Unsigned integral type
 * @param[in] x Value to round up
 * @return T Next power of two >= x
 */
template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 T bit_ceil(T x) noexcept {
    if (x <= 1) {
        return T(1);
    }

    if constexpr (std::is_same_v<T, decltype(+x)>) {
        return T(1) << bit_width(T(x - 1));
    } else {
        constexpr int offset_for_ub =
            std::numeric_limits<unsigned>::digits - std::numeric_limits<T>::digits;
        return T(1 << (bit_width(T(x - 1)) + offset_for_ub) >> offset_for_ub);
    }
}

/**
 * @brief Round down to previous power of two
 *
 * Returns the largest power of two that is not greater than x.
 * If x is 0, returns 0. Equivalent to C++20 std::bit_floor.
 *
 * @tparam T Unsigned integral type
 * @param[in] x Value to round down
 * @return T Previous power of two <= x
 */
template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 T bit_floor(T x) noexcept {
    if (x != 0) {
        return T{1} << (bit_width(x) - 1);
    }

    return 0;
}

#if WJR_HAS_BUILTIN(__builtin_bit_cast) || WJR_HAS_MSVC(19, 27)
    #define WJR_HAS_BUILTIN_BIT_CAST WJR_HAS_DEF
#endif

template <typename To, typename From>
requires(sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> &&
         std::is_trivially_copyable_v<To>)
WJR_PURE WJR_INTRINSIC_INLINE To bit_cast(const From &src) noexcept {
    static_assert(std::is_trivially_constructible_v<To>);
#if WJR_HAS_BUILTIN(BIT_CAST)
    return __builtin_bit_cast(To, src);
#else
    To dst;
    builtin_memcpy(std::addressof(dst), std::addressof(src), sizeof(To));
    return dst;
#endif
}

namespace bit_detail {

template <size_t ValueSize, typename To>
WJR_INTRINSIC_INLINE void clear_tail_padding_bits(To &to, std::true_type) noexcept {
    std::memset(reinterpret_cast<unsigned char *>(std::addressof(to)) + ValueSize, 0,
                sizeof(To) - ValueSize);
}

template <size_t ValueSize, typename To>
WJR_INTRINSIC_INLINE void clear_tail_padding_bits(To &, std::false_type) noexcept {}

template <size_t ValueSize, typename To>
WJR_INTRINSIC_INLINE void clear_tail_padding_bits(To &to) noexcept {
    clear_tail_padding_bits<ValueSize>(to, std::bool_constant<(ValueSize < sizeof(To))>());
}

template <typename To, typename From>
WJR_INTRINSIC_INLINE To bitwise_cast_memcpy(From const &from) noexcept {
    static constexpr auto copy_size = sizeof(From) < sizeof(To) ? sizeof(From) : sizeof(To);
    To dst;
    builtin_memcpy(std::addressof(dst), std::addressof(from), copy_size);
    clear_tail_padding_bits<sizeof(From)>(dst);
    return dst;
}

template <typename To, typename From>
WJR_INTRINSIC_INLINE To bitwise_cast_impl(From const &from, std::true_type) noexcept {
    // This implementation is only called when the From type has no padding and
    // From and To have the same size
    return bit_cast<To>(from);
}

template <typename To, typename From>
WJR_INTRINSIC_INLINE To bitwise_cast_impl(From const &from, std::false_type) noexcept {
    return bitwise_cast_memcpy<To>(from);
}

} // namespace bit_detail

template <typename To, typename From>
requires(std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>)
WJR_INTRINSIC_INLINE To bitwise_cast(From const &from) noexcept {
    return bit_detail::bitwise_cast_impl<To>(
        from, std::bool_constant<(sizeof(From) == sizeof(To) &&
                                  std::has_unique_object_representations<From>::value)>());
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T maintain_minimum(T num, type_identity_t<T> minimum) {
    return num < minimum ? minimum : num;
}

template <typename T>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T maintain_maximum(T num, type_identity_t<T> maximum) {
    return num > maximum ? maximum : num;
}

} // namespace wjr

#endif // WJR_MATH_BIT_HPP__