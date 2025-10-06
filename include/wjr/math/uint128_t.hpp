/**
 * @file math/uint128_t.hpp
 * @brief 128-bit unsigned integer type
 * @author wjr
 *
 * Provides a portable uint128_t implementation with arithmetic operations.
 * Used primarily for 128-bit division operations.
 *
 * @todo Implement more functions
 */

#ifndef WJR_MATH_UINT128_T_HPP__
#define WJR_MATH_UINT128_T_HPP__

#include <wjr/math/add.hpp>
#include <wjr/math/div-impl.hpp>
#include <wjr/math/divider.hpp>
#include <wjr/math/mul.hpp>
#include <wjr/math/shift.hpp>
#include <wjr/math/sub.hpp>

namespace wjr {

/**
 * @brief temporary uint128_t for divide 128
 *
 * @todo implement more functions
 *
 */
class uint128_t {
public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(uint128_t);

    constexpr uint128_t(uint64_t lo_, uint64_t hi_) noexcept : low(lo_), high(hi_) {}

    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    explicit constexpr uint128_t(T value) noexcept : low(value), high(0) {}

    template <typename T, WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
    explicit constexpr uint128_t(T value) noexcept
        : low(static_cast<T>(value)), high(static_cast<T>(value >= 0 ? 0 : -1)) {}

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    constexpr uint128_t &operator=(T value) noexcept {
        return *this = uint128_t(value);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    explicit constexpr operator T() const noexcept {
        return static_cast<T>(low);
    }

#if WJR_HAS_FEATURE(INT128)
    explicit constexpr uint128_t(__uint128_t value) noexcept
        : low(static_cast<uint64_t>(value)), high(static_cast<uint64_t>(value >> 64)) {}

    constexpr uint128_t &operator=(__uint128_t value) noexcept { return *this = uint128_t(value); }

    explicit constexpr operator __uint128_t() const noexcept {
        return (static_cast<__uint128_t>(high) << 64) | low;
    }
#endif

    WJR_CONSTEXPR20 uint128_t &operator+=(uint128_t other) noexcept {
        add_128(low, high, low, high, other.low, other.high);
        return *this;
    }

    friend WJR_CONST WJR_CONSTEXPR20 uint128_t operator+(uint128_t lhs, uint128_t rhs) noexcept {
        return lhs += rhs;
    }

    constexpr uint128_t &operator+=(uint64_t lo_) noexcept {
        add_128(low, high, low, high, lo_, 0);
        return *this;
    }

    friend WJR_CONST constexpr uint128_t operator+(uint128_t lhs, uint64_t rhs) noexcept {
        return lhs += rhs;
    }

    friend WJR_CONST constexpr uint128_t operator+(uint64_t lhs, uint128_t rhs) noexcept {
        return rhs += lhs;
    }

    WJR_CONSTEXPR20 uint128_t &operator-=(uint128_t other) noexcept {
        sub_128(low, high, low, high, other.low, other.high);
        return *this;
    }

    friend WJR_CONST WJR_CONSTEXPR20 uint128_t operator-(uint128_t lhs, uint128_t rhs) noexcept {
        return lhs -= rhs;
    }

    constexpr uint128_t &operator-=(uint64_t lo_) noexcept {
        sub_128(low, high, low, high, lo_, 0);
        return *this;
    }

    friend WJR_CONST constexpr uint128_t operator-(uint128_t lhs, uint64_t rhs) noexcept {
        return lhs -= rhs;
    }

    WJR_CONSTEXPR20 uint128_t &operator*=(uint128_t other) noexcept {
        const auto [__lo, __hi] = other;
        const uint64_t tmp = low * __hi + high * __lo;
        low = mul(low, __lo, high);
        high += tmp;
        return *this;
    }

private:
    static WJR_CONST WJR_CONSTEXPR20 uint128_t mul_u64(uint128_t lhs, uint64_t value) noexcept {
        const auto [low, high] = lhs;
        uint128_t tmp;
        tmp.low = mul(low, value, tmp.high);
        tmp.high += high * value;
        return tmp;
    }

    static WJR_CONST WJR_CONSTEXPR20 uint128_t mul_i64(uint128_t lhs, int64_t value) noexcept {
        const auto [low, high] = lhs;
        const uint64_t uvalue = to_unsigned(value);
        uint128_t tmp;
        tmp.low = mul(low, uvalue, tmp.high);
        tmp.high += high * uvalue + (value >= 0 ? 0 : -low);
        return tmp;
    }

public:
    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    WJR_CONSTEXPR20 uint128_t &operator*=(T value) noexcept {
        return (*this) = mul_u64(*this, value);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
    WJR_CONSTEXPR20 uint128_t &operator*=(T value) noexcept {
        return (*this) = mul_i64(*this, value);
    }

    friend WJR_CONST WJR_CONSTEXPR20 uint128_t operator*(uint128_t lhs, uint128_t rhs) noexcept {
        return lhs *= rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST WJR_CONSTEXPR20 uint128_t operator*(uint128_t lhs, T rhs) noexcept {
        return lhs *= rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST WJR_CONSTEXPR20 uint128_t operator*(T lhs, uint128_t rhs) noexcept {
        return rhs *= lhs;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator<(uint128_t lhs, uint128_t rhs) noexcept {
        return __less_128(lhs.low, lhs.high, rhs.low, rhs.high);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator>(uint128_t lhs, uint128_t rhs) noexcept {
        return rhs < lhs;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator<=(uint128_t lhs, uint128_t rhs) noexcept {
        return __less_equal_128(lhs.low, lhs.high, rhs.low, rhs.high);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator>=(uint128_t lhs, uint128_t rhs) noexcept {
        return rhs <= lhs;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator==(uint128_t lhs, uint128_t rhs) noexcept {
        return __equal_128(lhs.low, lhs.high, rhs.low, rhs.high);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator!=(uint128_t lhs, uint128_t rhs) noexcept {
        return !(lhs == rhs);
    }

    template <size_t I>
    constexpr uint64_t &get() & noexcept {
        if constexpr (I == 0) {
            return low;
        } else {
            return high;
        }
    }

    template <size_t I>
    constexpr const uint64_t &get() const & noexcept {
        if constexpr (I == 0) {
            return low;
        } else {
            return high;
        }
    }

    uint64_t low;
    uint64_t high;
};

WJR_INTRINSIC_CONSTEXPR20 uint128_t mul64x64to128(uint64_t a, uint64_t b) noexcept {
    uint64_t low, high;
    low = mul(a, b, high);
    return uint128_t(low, high);
}

} // namespace wjr

namespace std {

template <>
struct tuple_size<wjr::uint128_t> : integral_constant<size_t, 2> {};

template <size_t I>
struct tuple_element<I, wjr::uint128_t> {
    using type = uint64_t;
};

template <size_t I>
WJR_NODISCARD constexpr uint64_t &get(wjr::uint128_t &u) noexcept {
    return u.get<I>();
}

template <size_t I>
WJR_NODISCARD constexpr const uint64_t &get(const wjr::uint128_t &u) noexcept {
    return u.get<I>();
}

template <size_t I>
WJR_NODISCARD constexpr uint64_t &&get(wjr::uint128_t &&u) noexcept {
    return std::move(u).get<I>();
}

template <size_t I>
WJR_NODISCARD constexpr const uint64_t &&get(const wjr::uint128_t &&u) noexcept {
    return std::move(u).get<I>();
}

} // namespace std

#endif // WJR_MATH_UINT128_T_HPP__