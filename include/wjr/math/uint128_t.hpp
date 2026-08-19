/**
 * @file math/uint128_t.hpp
 * @brief 128-bit unsigned integer type
 * @author wjr
 *
 * Provides a lightweight uint128_t implementation with arithmetic operations.
 * Division operators are declared here and defined by <wjr/math/div.hpp>.
 */

#ifndef WJR_MATH_UINT128_T_HPP__
#define WJR_MATH_UINT128_T_HPP__

#include <cstddef>
#include <cstdint>
#include <limits>
#include <tuple>
#include <utility>

#include <wjr/math/add.hpp>
#include <wjr/math/div-impl.hpp>
#include <wjr/math/divider.hpp>
#include <wjr/math/mul.hpp>
#include <wjr/math/shift.hpp>
#include <wjr/math/sub.hpp>

namespace wjr {

class int128_t;

/**
 * @brief Lightweight 128-bit unsigned integer.
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

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    constexpr uint128_t &operator+=(T value) noexcept {
        return *this += uint128_t(value);
    }

    constexpr uint128_t operator+() const noexcept { return *this; }

    friend WJR_CONST constexpr uint128_t operator+(uint128_t lhs, uint64_t rhs) noexcept {
        return lhs += rhs;
    }

    friend WJR_CONST constexpr uint128_t operator+(uint64_t lhs, uint128_t rhs) noexcept {
        return rhs += lhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST constexpr uint128_t operator+(uint128_t lhs, T rhs) noexcept {
        return lhs += rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST constexpr uint128_t operator+(T lhs, uint128_t rhs) noexcept {
        return rhs += lhs;
    }

    WJR_CONSTEXPR20 uint128_t &operator-=(uint128_t other) noexcept {
        sub_128(low, high, low, high, other.low, other.high);
        return *this;
    }

    uint128_t &operator+=(int128_t other) noexcept;
    uint128_t &operator-=(int128_t other) noexcept;

    friend WJR_CONST WJR_CONSTEXPR20 uint128_t operator-(uint128_t lhs, uint128_t rhs) noexcept {
        return lhs -= rhs;
    }

    constexpr uint128_t &operator-=(uint64_t lo_) noexcept {
        sub_128(low, high, low, high, lo_, 0);
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    constexpr uint128_t &operator-=(T value) noexcept {
        return *this -= uint128_t(value);
    }

    friend WJR_CONST constexpr uint128_t operator-(uint128_t lhs, uint64_t rhs) noexcept {
        return lhs -= rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST constexpr uint128_t operator-(uint128_t lhs, T rhs) noexcept {
        return lhs -= rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST constexpr uint128_t operator-(T lhs, uint128_t rhs) noexcept {
        return uint128_t(lhs) - rhs;
    }

    WJR_CONSTEXPR20 uint128_t operator-() const noexcept { return uint128_t(0, 0) - *this; }

    constexpr uint128_t &operator&=(uint128_t other) noexcept {
        low &= other.low;
        high &= other.high;
        return *this;
    }

    constexpr uint128_t &operator|=(uint128_t other) noexcept {
        low |= other.low;
        high |= other.high;
        return *this;
    }

    constexpr uint128_t &operator^=(uint128_t other) noexcept {
        low ^= other.low;
        high ^= other.high;
        return *this;
    }

    friend constexpr uint128_t operator~(uint128_t value) noexcept {
        value.low = ~value.low;
        value.high = ~value.high;
        return value;
    }

    friend constexpr uint128_t operator&(uint128_t lhs, uint128_t rhs) noexcept {
        return lhs &= rhs;
    }

    friend constexpr uint128_t operator|(uint128_t lhs, uint128_t rhs) noexcept {
        return lhs |= rhs;
    }

    friend constexpr uint128_t operator^(uint128_t lhs, uint128_t rhs) noexcept {
        return lhs ^= rhs;
    }

    constexpr uint128_t &operator<<=(unsigned int shift) noexcept {
        if (shift >= 128) {
            low = 0;
            high = 0;
        } else if (shift >= 64) {
            high = low << (shift - 64);
            low = 0;
        } else if (shift != 0) {
            high = (high << shift) | (low >> (64 - shift));
            low <<= shift;
        }

        return *this;
    }

    friend constexpr uint128_t operator<<(uint128_t value, unsigned int shift) noexcept {
        return value <<= shift;
    }

    constexpr uint128_t &operator>>=(unsigned int shift) noexcept {
        if (shift >= 128) {
            low = 0;
            high = 0;
        } else if (shift >= 64) {
            low = high >> (shift - 64);
            high = 0;
        } else if (shift != 0) {
            low = (low >> shift) | (high << (64 - shift));
            high >>= shift;
        }

        return *this;
    }

    friend constexpr uint128_t operator>>(uint128_t value, unsigned int shift) noexcept {
        return value >>= shift;
    }

    WJR_CONSTEXPR20 uint128_t &operator*=(uint128_t other) noexcept {
        const auto [_lo, _hi] = other;
        const uint64_t tmp = low * _hi + high * _lo;
        low = mul(low, _lo, high);
        high += tmp;
        return *this;
    }

    uint128_t &operator*=(int128_t other) noexcept;

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

    constexpr uint128_t &operator++() noexcept { return *this += uint64_t{1}; }
    constexpr uint128_t operator++(int) noexcept {
        const auto value = *this;
        ++*this;
        return value;
    }

    constexpr uint128_t &operator--() noexcept { return *this -= uint64_t{1}; }
    constexpr uint128_t operator--(int) noexcept {
        const auto value = *this;
        --*this;
        return value;
    }

    uint128_t &operator/=(uint128_t other) noexcept;

    uint128_t &operator%=(uint128_t other) noexcept;

    uint128_t &operator/=(int128_t other) noexcept;
    uint128_t &operator%=(int128_t other) noexcept;

    uint128_t &operator&=(int128_t other) noexcept;
    uint128_t &operator|=(int128_t other) noexcept;
    uint128_t &operator^=(int128_t other) noexcept;

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    uint128_t &operator/=(T value) noexcept {
        return *this /= uint128_t(value);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    uint128_t &operator%=(T value) noexcept {
        return *this %= uint128_t(value);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator<(uint128_t lhs, uint128_t rhs) noexcept {
        return _less_128(lhs.low, lhs.high, rhs.low, rhs.high);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator>(uint128_t lhs, uint128_t rhs) noexcept {
        return rhs < lhs;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator<=(uint128_t lhs, uint128_t rhs) noexcept {
        return _less_equal_128(lhs.low, lhs.high, rhs.low, rhs.high);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator>=(uint128_t lhs, uint128_t rhs) noexcept {
        return rhs <= lhs;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator==(uint128_t lhs, uint128_t rhs) noexcept {
        return _equal_128(lhs.low, lhs.high, rhs.low, rhs.high);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator!=(uint128_t lhs, uint128_t rhs) noexcept {
        return !(lhs == rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator<(uint128_t lhs, T rhs) noexcept {
        return lhs < uint128_t(rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator<(T lhs, uint128_t rhs) noexcept {
        return uint128_t(lhs) < rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator<=(uint128_t lhs, T rhs) noexcept {
        return lhs <= uint128_t(rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator<=(T lhs, uint128_t rhs) noexcept {
        return uint128_t(lhs) <= rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator>(uint128_t lhs, T rhs) noexcept {
        return lhs > uint128_t(rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator>(T lhs, uint128_t rhs) noexcept {
        return uint128_t(lhs) > rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator>=(uint128_t lhs, T rhs) noexcept {
        return lhs >= uint128_t(rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator>=(T lhs, uint128_t rhs) noexcept {
        return uint128_t(lhs) >= rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator==(uint128_t lhs, T rhs) noexcept {
        return lhs == uint128_t(rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator==(T lhs, uint128_t rhs) noexcept {
        return uint128_t(lhs) == rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator!=(uint128_t lhs, T rhs) noexcept {
        return !(lhs == rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend constexpr bool operator!=(T lhs, uint128_t rhs) noexcept {
        return !(lhs == rhs);
    }

    template <size_t I>
    constexpr uint64_t &get() & noexcept {
        static_assert(I < 2);
        if constexpr (I == 0) {
            return low;
        } else {
            return high;
        }
    }

    template <size_t I>
    constexpr const uint64_t &get() const & noexcept {
        static_assert(I < 2);
        if constexpr (I == 0) {
            return low;
        } else {
            return high;
        }
    }

    template <size_t I>
    constexpr uint64_t &&get() && noexcept {
        static_assert(I < 2);
        if constexpr (I == 0) {
            return std::move(low);
        } else {
            return std::move(high);
        }
    }

    template <size_t I>
    constexpr const uint64_t &&get() const && noexcept {
        static_assert(I < 2);
        if constexpr (I == 0) {
            return std::move(low);
        } else {
            return std::move(high);
        }
    }

    uint64_t low;
    uint64_t high;
};

WJR_CONST WJR_INTRINSIC_CONSTEXPR20 uint128_t mul64x64to128(uint64_t a, uint64_t b) noexcept {
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
    static_assert(I < 2);
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

namespace std {

template <>
class numeric_limits<wjr::uint128_t> {
public:
    static constexpr bool is_specialized = true;
    static constexpr int digits = 128;
    static constexpr int digits10 = 38;
    static constexpr int max_digits10 = 0;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int max_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;
    static constexpr int traps = 0;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;

    static constexpr wjr::uint128_t min() noexcept { return wjr::uint128_t(0, 0); }
    static constexpr wjr::uint128_t lowest() noexcept { return min(); }
    static constexpr wjr::uint128_t max() noexcept {
        return wjr::uint128_t(UINT64_MAX, UINT64_MAX);
    }
    static constexpr wjr::uint128_t epsilon() noexcept { return min(); }
    static constexpr wjr::uint128_t round_error() noexcept { return min(); }
    static constexpr wjr::uint128_t infinity() noexcept { return min(); }
    static constexpr wjr::uint128_t quiet_NaN() noexcept { return min(); }
    static constexpr wjr::uint128_t signaling_NaN() noexcept { return min(); }
    static constexpr wjr::uint128_t denorm_min() noexcept { return min(); }
};

} // namespace std

#endif // WJR_MATH_UINT128_T_HPP__