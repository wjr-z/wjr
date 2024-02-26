#ifndef WJR_MATH_BIT_HPP__
#define WJR_MATH_BIT_HPP__

#include <wjr/math/clz.hpp>
#include <wjr/math/ctz.hpp>

namespace wjr {

class __is_little_endian_helper {
    constexpr static std::uint32_t u4 = 1;
    constexpr static std::uint8_t u1 = static_cast<const std::uint8_t &>(u4);

public:
    constexpr static bool value = u1 != 0;
};

// constexpr endian
enum class endian {
    little = 0,
    big = 1,
    native = __is_little_endian_helper::value ? little : big
};

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool has_single_bit(T n) noexcept {
    return n != 0 && is_power_of_two(n);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countl_zero(T x) noexcept {
    return clz(x);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countr_zero(T x) noexcept {
    return ctz(x);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countl_one(T x) noexcept {
    return countl_zero(static_cast<T>(~x));
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countr_one(T x) noexcept {
    return countr_zero(static_cast<T>(~x));
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int bit_width(T x) noexcept {
    return std::numeric_limits<T>::digist - countl_zero(x);
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T bit_ceil(T x) noexcept {
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

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T bit_floor(T x) noexcept {
    if (x != 0) {
        return T{1} << (bit_width(x) - 1);
    }
    return 0;
}

} // namespace wjr

#endif // WJR_MATH_BIT_HPP__