#ifndef WJR_MATH_BIT_HPP
#define WJR_MATH_BIT_HPP

#include <wjr/math/clz.hpp>
#include <wjr/math/ctz.hpp>
#include <wjr/memory/details.hpp>

namespace wjr {

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool has_single_bit(T n) noexcept {
    return (n != 0) && is_zero_or_single_bit(n);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countl_zero(T x) noexcept {
    if (WJR_UNLIKELY(x == 0)) {
        return std::numeric_limits<T>::digits;
    }

    return clz(x);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countr_zero(T x) noexcept {
    if (WJR_UNLIKELY(x == 0)) {
        return std::numeric_limits<T>::digits;
    }

    return ctz(x);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countl_one(T x) noexcept {
    return countl_zero(static_cast<T>(~x));
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int countr_one(T x) noexcept {
    return countr_zero(static_cast<T>(~x));
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E int bit_width(T x) noexcept {
    return std::numeric_limits<T>::digits - countl_zero(x);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
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

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR_E T bit_floor(T x) noexcept {
    if (x != 0) {
        return T{1} << (bit_width(x) - 1);
    }
    return 0;
}

} // namespace wjr

#endif // WJR_MATH_BIT_HPP