#ifndef WJR_MATH_BIT_HPP__
#define WJR_MATH_BIT_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool is_power_of_two(T n) {
    return (n & (n - 1)) == 0;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool has_single_bit(T n) {
    return n != 0 && is_power_of_two(n);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T lowbit(T n) {
    return n & -n;
}

} // namespace wjr

#endif // WJR_MATH_BIT_HPP__