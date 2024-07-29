#ifndef WJR_MATH_BIT_HPP__
#define WJR_MATH_BIT_HPP__

#include <wjr/math/clz.hpp>
#include <wjr/math/ctz.hpp>
#include <wjr/memory/detail.hpp>

namespace wjr {

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool has_single_bit(T n) noexcept {
    return (n != 0) && is_zero_or_single_bit(n);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int countl_zero(T x) noexcept {
    // If not use __builtin_clz and use popcount, then don't need to handle zero.
#if WJR_HAS_BUILTIN(CLZ) || !(WJR_HAS_BUILTIN(POPCOUNT) && WJR_HAS_SIMD(POPCNT))
    if (WJR_UNLIKELY(x == 0)) {
        return std::numeric_limits<T>::digits;
    }
#endif

    return clz(x);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int countr_zero(T x) noexcept {
    // If not use __builtin_ctz and use popcount, then don't need to handle zero.
#if WJR_HAS_BUILTIN(CTZ) || !(WJR_HAS_BUILTIN(POPCOUNT) && WJR_HAS_SIMD(POPCNT))
    if (WJR_UNLIKELY(x == 0)) {
        return std::numeric_limits<T>::digits;
    }
#endif

    return ctz(x);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int countl_one(T x) noexcept {
    return countl_zero(static_cast<T>(~x));
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int countr_one(T x) noexcept {
    return countr_zero(static_cast<T>(~x));
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 int bit_width(T x) noexcept {
    return std::numeric_limits<T>::digits - countl_zero(x);
}

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
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

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_CONST WJR_INTRINSIC_CONSTEXPR20 T bit_floor(T x) noexcept {
    if (x != 0) {
        return T{1} << (bit_width(x) - 1);
    }
    return 0;
}

template <typename To, typename From,
          WJR_REQUIRES(sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> &&
                       std::is_trivially_copyable_v<To>)>
WJR_PURE WJR_INTRINSIC_INLINE To bit_cast(const From &src) noexcept {
    static_assert(std::is_trivially_constructible_v<To>, "");
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

} // namespace wjr

#endif // WJR_MATH_BIT_HPP__