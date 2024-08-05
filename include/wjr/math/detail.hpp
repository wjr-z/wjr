#ifndef WJR_MATH_DETAIL_HPP__
#define WJR_MATH_DETAIL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

/**
 * @brief
 *
 * @note `n & -n` is the lowest bit of n.
 */
template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T lowbit(T n) noexcept {
    return n & -n;
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T clear_lowbit(T n) noexcept {
    return n & (n - 1);
}

// preview :

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr bool is_zero_or_single_bit(T n) noexcept {
    return (n & (n - 1)) == 0;
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr bool __has_high_bit(T n) noexcept {
    return n >> (std::numeric_limits<T>::digits - 1);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T __ceil_div(T n, type_identity_t<T> div) noexcept {
    return (n + div - 1) / div;
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T __align_down(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(is_zero_or_single_bit(alignment));
    return n & (-alignment);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T __align_down_offset(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(is_zero_or_single_bit(alignment));
    return n & (alignment - 1);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T __align_up(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(is_zero_or_single_bit(alignment));
    return (n + alignment - 1) & (-alignment);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr T __align_up_offset(T n, type_identity_t<T> alignment) noexcept {
    WJR_ASSERT_ASSUME_L2(is_zero_or_single_bit(alignment));
    return (-n) & (alignment - 1);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST constexpr std::make_signed_t<T> __fasts_from_unsigned(T x) noexcept {
    const std::make_signed_t<T> ret = x;
    WJR_ASSERT_ASSUME_L2(ret >= 0, "overflow");
    return ret;
}

template <typename T, typename U = std::make_unsigned_t<T>,
          WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
WJR_CONST constexpr U __fasts_abs(T x) noexcept {
    return static_cast<U>(x < 0 ? -x : x);
}

template <typename T, WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
WJR_CONST constexpr T __fasts_negate(T x) noexcept {
    return -x;
}

template <typename T, typename U = std::make_unsigned_t<T>,
          WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
WJR_CONST constexpr T __fasts_conditional_negate(bool condition, T x) noexcept {
    return condition ? -x : x;
}

template <typename T, typename U = std::make_unsigned_t<T>,
          WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
WJR_CONST constexpr T __fasts_negate_with(T condition, T x) noexcept {
    return __fasts_conditional_negate(condition < 0, x);
}

template <typename T, WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
WJR_CONST constexpr T __fasts_increment(T x) noexcept {
    WJR_ASSERT_L2(x != std::numeric_limits<T>::min() &&
                      x != std::numeric_limits<T>::max(),
                  "overflow");

    return x < 0 ? x - 1 : x + 1;
}

template <typename T, WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
WJR_CONST constexpr T __fasts_decrement(T x) noexcept {
    WJR_ASSERT_L2(x != 0 && x + 1 != T(0), "overflow");

    return x < 0 ? x + 1 : x - 1;
}

template <typename T, WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
WJR_CONST constexpr T __fasts_add(T x, std::make_unsigned_t<T> y) noexcept {
    return x < 0 ? x - y : x + y;
}

template <typename T, WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
WJR_CONST constexpr T __fasts_sub(T x, std::make_unsigned_t<T> y) noexcept {
    return x < 0 ? x + y : x - y;
}

} // namespace wjr

#endif // WJR_MATH_DETAIL_HPP__