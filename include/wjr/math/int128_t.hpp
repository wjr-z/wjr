/**
 * @file math/int128_t.hpp
 * @brief 128-bit signed integer type
 * @author wjr
 *
 * Provides a two's-complement signed 128-bit integer backed by uint128_t.
 * Division operators use the unsigned 128-bit division implementation.
 */

#ifndef WJR_MATH_INT128_T_HPP__
#define WJR_MATH_INT128_T_HPP__

#include <cstdint>
#include <limits>

#include <wjr/math/div.hpp>

namespace wjr {

/**
 * @brief Lightweight two's-complement signed 128-bit integer.
 *
 * Arithmetic wraps to the signed 128-bit bit pattern. In particular,
 * INT128_MIN / -1 produces INT128_MIN, matching the underlying modulo result.
 */
class int128_t {
    struct raw_bits_t {};

    constexpr int128_t(uint128_t bits, raw_bits_t) noexcept : m_bits(bits) {}

    static constexpr int128_t from_bits(uint128_t bits) noexcept {
        return int128_t(bits, raw_bits_t{});
    }

    static WJR_CONSTEXPR20 uint128_t magnitude(int128_t value) noexcept {
        if (!value.is_negative()) {
            return value.m_bits;
        }

        return uint128_t(0, 0) - value.m_bits;
    }

    static WJR_CONSTEXPR20 int128_t apply_sign(uint128_t magnitude_value, bool negative) noexcept {
        if (negative) {
            magnitude_value = uint128_t(0, 0) - magnitude_value;
        }

        return from_bits(magnitude_value);
    }

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(int128_t);

    constexpr int128_t(uint64_t low, uint64_t high) noexcept : m_bits(low, high) {}

    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    explicit constexpr int128_t(T value) noexcept : m_bits(value) {}

    template <typename T, WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
    explicit constexpr int128_t(T value) noexcept
        : m_bits(static_cast<uint64_t>(value), value < 0 ? UINT64_MAX : uint64_t{0}) {}

#if WJR_HAS_FEATURE(INT128)
    explicit constexpr int128_t(__uint128_t value) noexcept : m_bits(value) {}

    explicit constexpr int128_t(__int128_t value) noexcept
        : m_bits(static_cast<__uint128_t>(value)) {}

    constexpr int128_t &operator=(__int128_t value) noexcept {
        m_bits = static_cast<__uint128_t>(value);
        return *this;
    }

    explicit constexpr operator __int128_t() const noexcept {
        return static_cast<__int128_t>(static_cast<__uint128_t>(m_bits));
    }
#endif

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    constexpr int128_t &operator=(T value) noexcept {
        return *this = int128_t(value);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    explicit constexpr operator T() const noexcept {
        return static_cast<T>(m_bits.low);
    }

    constexpr uint128_t bits() const noexcept { return m_bits; }
    constexpr bool is_negative() const noexcept {
        return m_bits.high >= UINT64_C(0x8000000000000000);
    }

    WJR_CONSTEXPR20 int128_t &operator+=(int128_t other) noexcept {
        m_bits += other.m_bits;
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    WJR_CONSTEXPR20 int128_t &operator+=(T value) noexcept {
        return *this += int128_t(value);
    }

    WJR_CONSTEXPR20 int128_t &operator+=(uint128_t value) noexcept {
        m_bits += value;
        return *this;
    }

    friend WJR_CONST WJR_CONSTEXPR20 int128_t operator+(int128_t lhs, int128_t rhs) noexcept {
        return lhs += rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST WJR_CONSTEXPR20 int128_t operator+(int128_t lhs, T rhs) noexcept {
        return lhs += int128_t(rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST WJR_CONSTEXPR20 int128_t operator+(T lhs, int128_t rhs) noexcept {
        return rhs += int128_t(lhs);
    }

    WJR_CONSTEXPR20 int128_t &operator-=(int128_t other) noexcept {
        m_bits -= other.m_bits;
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    WJR_CONSTEXPR20 int128_t &operator-=(T value) noexcept {
        return *this -= int128_t(value);
    }

    WJR_CONSTEXPR20 int128_t &operator-=(uint128_t value) noexcept {
        m_bits -= value;
        return *this;
    }

    friend WJR_CONST WJR_CONSTEXPR20 int128_t operator-(int128_t lhs, int128_t rhs) noexcept {
        return lhs -= rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST WJR_CONSTEXPR20 int128_t operator-(int128_t lhs, T rhs) noexcept {
        return lhs -= int128_t(rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST WJR_CONSTEXPR20 int128_t operator-(T lhs, int128_t rhs) noexcept {
        return int128_t(lhs) -= rhs;
    }

    friend WJR_CONST WJR_CONSTEXPR20 int128_t operator-(int128_t value) noexcept {
        return from_bits(uint128_t(0, 0) - value.m_bits);
    }

    WJR_CONSTEXPR20 int128_t &operator*=(int128_t other) noexcept {
        m_bits *= other.m_bits;
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    WJR_CONSTEXPR20 int128_t &operator*=(T value) noexcept {
        return *this *= int128_t(value);
    }

    WJR_CONSTEXPR20 int128_t &operator*=(uint128_t value) noexcept {
        m_bits *= value;
        return *this;
    }

    friend WJR_CONST WJR_CONSTEXPR20 int128_t operator*(int128_t lhs, int128_t rhs) noexcept {
        return lhs *= rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST WJR_CONSTEXPR20 int128_t operator*(int128_t lhs, T rhs) noexcept {
        return lhs *= int128_t(rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend WJR_CONST WJR_CONSTEXPR20 int128_t operator*(T lhs, int128_t rhs) noexcept {
        return rhs *= int128_t(lhs);
    }

    int128_t &operator/=(int128_t other) noexcept {
        const bool negative = is_negative() != other.is_negative();
        const auto quotient = magnitude(*this) / magnitude(other);
        *this = apply_sign(quotient, negative);
        return *this;
    }

    friend int128_t operator/(int128_t lhs, int128_t rhs) noexcept { return lhs /= rhs; }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    int128_t &operator/=(T value) noexcept {
        return *this /= int128_t(value);
    }

    int128_t &operator/=(uint128_t value) noexcept {
        m_bits /= value;
        return *this;
    }

    int128_t &operator%=(int128_t other) noexcept {
        const auto remainder = magnitude(*this) % magnitude(other);
        *this = apply_sign(remainder, is_negative());
        return *this;
    }

    friend int128_t operator%(int128_t lhs, int128_t rhs) noexcept { return lhs %= rhs; }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    int128_t &operator%=(T value) noexcept {
        return *this %= int128_t(value);
    }

    int128_t &operator%=(uint128_t value) noexcept {
        m_bits %= value;
        return *this;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend int128_t operator/(int128_t lhs, T rhs) noexcept {
        return lhs / int128_t(rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend int128_t operator%(int128_t lhs, T rhs) noexcept {
        return lhs % int128_t(rhs);
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend int128_t operator/(T lhs, int128_t rhs) noexcept {
        return int128_t(lhs) / rhs;
    }

    template <typename T, WJR_REQUIRES(is_nonbool_integral_v<T>)>
    friend int128_t operator%(T lhs, int128_t rhs) noexcept {
        return int128_t(lhs) % rhs;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator==(int128_t lhs, int128_t rhs) noexcept {
        return lhs.m_bits == rhs.m_bits;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator!=(int128_t lhs, int128_t rhs) noexcept {
        return !(lhs == rhs);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator<(int128_t lhs, int128_t rhs) noexcept {
        if (lhs.is_negative() != rhs.is_negative()) {
            return lhs.is_negative();
        }

        if (lhs.is_negative()) {
            return lhs.m_bits < rhs.m_bits;
        }

        return lhs.m_bits < rhs.m_bits;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator>(int128_t lhs, int128_t rhs) noexcept {
        return rhs < lhs;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator<=(int128_t lhs, int128_t rhs) noexcept {
        return !(rhs < lhs);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator>=(int128_t lhs, int128_t rhs) noexcept {
        return !(lhs < rhs);
    }

    constexpr int128_t &operator&=(int128_t other) noexcept {
        m_bits.low &= other.m_bits.low;
        m_bits.high &= other.m_bits.high;
        return *this;
    }

    constexpr int128_t &operator&=(uint128_t other) noexcept {
        m_bits &= other;
        return *this;
    }

    constexpr int128_t &operator|=(int128_t other) noexcept {
        m_bits.low |= other.m_bits.low;
        m_bits.high |= other.m_bits.high;
        return *this;
    }

    constexpr int128_t &operator|=(uint128_t other) noexcept {
        m_bits |= other;
        return *this;
    }

    constexpr int128_t &operator^=(int128_t other) noexcept {
        m_bits.low ^= other.m_bits.low;
        m_bits.high ^= other.m_bits.high;
        return *this;
    }

    constexpr int128_t &operator^=(uint128_t other) noexcept {
        m_bits ^= other;
        return *this;
    }

    friend constexpr int128_t operator~(int128_t value) noexcept {
        value.m_bits.low = ~value.m_bits.low;
        value.m_bits.high = ~value.m_bits.high;
        return value;
    }

    friend constexpr int128_t operator&(int128_t lhs, int128_t rhs) noexcept { return lhs &= rhs; }
    friend constexpr int128_t operator|(int128_t lhs, int128_t rhs) noexcept { return lhs |= rhs; }
    friend constexpr int128_t operator^(int128_t lhs, int128_t rhs) noexcept { return lhs ^= rhs; }

    constexpr int128_t &operator<<=(unsigned int shift) noexcept {
        if (shift >= 128) {
            m_bits = uint128_t(0, 0);
        } else if (shift >= 64) {
            m_bits.high = m_bits.low << (shift - 64);
            m_bits.low = 0;
        } else if (shift != 0) {
            m_bits.high = (m_bits.high << shift) | (m_bits.low >> (64 - shift));
            m_bits.low <<= shift;
        }

        return *this;
    }

    friend constexpr int128_t operator<<(int128_t value, unsigned int shift) noexcept {
        return value <<= shift;
    }

    constexpr int128_t &operator>>=(unsigned int shift) noexcept {
        const bool negative = is_negative();
        const uint64_t fill = negative ? UINT64_MAX : uint64_t{0};

        if (shift >= 128) {
            m_bits.low = fill;
            m_bits.high = fill;
        } else if (shift >= 64) {
            m_bits.low = m_bits.high >> (shift - 64);
            m_bits.high = fill;
            if (negative && shift != 64) {
                m_bits.low |= UINT64_MAX << (128 - shift);
            }
        } else if (shift != 0) {
            m_bits.low = (m_bits.low >> shift) | (m_bits.high << (64 - shift));
            m_bits.high = (m_bits.high >> shift) | (negative ? UINT64_MAX << (64 - shift) : 0);
        }

        return *this;
    }

    friend constexpr int128_t operator>>(int128_t value, unsigned int shift) noexcept {
        return value >>= shift;
    }

    constexpr int128_t operator+() const noexcept { return *this; }

    WJR_CONSTEXPR20 int128_t &operator++() noexcept { return *this += 1; }
    WJR_CONSTEXPR20 int128_t operator++(int) noexcept {
        const auto value = *this;
        ++*this;
        return value;
    }

    WJR_CONSTEXPR20 int128_t &operator--() noexcept { return *this -= 1; }
    WJR_CONSTEXPR20 int128_t operator--(int) noexcept {
        const auto value = *this;
        --*this;
        return value;
    }

private:
    uint128_t m_bits;
};

} // namespace wjr

namespace wjr {

inline uint128_t operator+(uint128_t lhs, int128_t rhs) noexcept { return lhs + rhs.bits(); }

inline uint128_t operator+(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() + rhs; }

inline uint128_t operator-(uint128_t lhs, int128_t rhs) noexcept { return lhs - rhs.bits(); }

inline uint128_t operator-(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() - rhs; }

inline uint128_t operator*(uint128_t lhs, int128_t rhs) noexcept { return lhs * rhs.bits(); }

inline uint128_t operator*(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() * rhs; }

inline uint128_t operator/(uint128_t lhs, int128_t rhs) noexcept { return lhs / rhs.bits(); }

inline uint128_t operator/(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() / rhs; }

inline uint128_t operator%(uint128_t lhs, int128_t rhs) noexcept { return lhs % rhs.bits(); }

inline uint128_t operator%(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() % rhs; }

inline uint128_t operator&(uint128_t lhs, int128_t rhs) noexcept { return lhs & rhs.bits(); }

inline uint128_t operator&(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() & rhs; }

inline uint128_t operator|(uint128_t lhs, int128_t rhs) noexcept { return lhs | rhs.bits(); }

inline uint128_t operator|(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() | rhs; }

inline uint128_t operator^(uint128_t lhs, int128_t rhs) noexcept { return lhs ^ rhs.bits(); }

inline uint128_t operator^(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() ^ rhs; }

inline uint128_t &uint128_t::operator+=(int128_t other) noexcept { return *this += other.bits(); }

inline uint128_t &uint128_t::operator-=(int128_t other) noexcept { return *this -= other.bits(); }

inline uint128_t &uint128_t::operator*=(int128_t other) noexcept { return *this *= other.bits(); }

inline uint128_t &uint128_t::operator/=(int128_t other) noexcept { return *this /= other.bits(); }

inline uint128_t &uint128_t::operator%=(int128_t other) noexcept { return *this %= other.bits(); }

inline uint128_t &uint128_t::operator&=(int128_t other) noexcept { return *this &= other.bits(); }

inline uint128_t &uint128_t::operator|=(int128_t other) noexcept { return *this |= other.bits(); }

inline uint128_t &uint128_t::operator^=(int128_t other) noexcept { return *this ^= other.bits(); }

inline bool operator==(uint128_t lhs, int128_t rhs) noexcept { return lhs == rhs.bits(); }
inline bool operator==(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() == rhs; }
inline bool operator!=(uint128_t lhs, int128_t rhs) noexcept { return !(lhs == rhs); }
inline bool operator!=(int128_t lhs, uint128_t rhs) noexcept { return !(lhs == rhs); }
inline bool operator<(uint128_t lhs, int128_t rhs) noexcept { return lhs < rhs.bits(); }
inline bool operator<(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() < rhs; }
inline bool operator<=(uint128_t lhs, int128_t rhs) noexcept { return lhs <= rhs.bits(); }
inline bool operator<=(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() <= rhs; }
inline bool operator>(uint128_t lhs, int128_t rhs) noexcept { return lhs > rhs.bits(); }
inline bool operator>(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() > rhs; }
inline bool operator>=(uint128_t lhs, int128_t rhs) noexcept { return lhs >= rhs.bits(); }
inline bool operator>=(int128_t lhs, uint128_t rhs) noexcept { return lhs.bits() >= rhs; }

} // namespace wjr

namespace std {

template <>
class numeric_limits<wjr::int128_t> {
public:
    static constexpr bool is_specialized = true;
    static constexpr int digits = 127;
    static constexpr int digits10 = 38;
    static constexpr int max_digits10 = 0;
    static constexpr bool is_signed = true;
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
    static constexpr bool is_modulo = false;
    static constexpr int traps = 0;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;

    static constexpr wjr::int128_t min() noexcept {
        return wjr::int128_t(0, UINT64_C(0x8000000000000000));
    }
    static constexpr wjr::int128_t lowest() noexcept { return min(); }
    static constexpr wjr::int128_t max() noexcept {
        return wjr::int128_t(UINT64_MAX, UINT64_C(0x7fffffffffffffff));
    }
    static constexpr wjr::int128_t epsilon() noexcept { return wjr::int128_t(0); }
    static constexpr wjr::int128_t round_error() noexcept { return wjr::int128_t(0); }
    static constexpr wjr::int128_t infinity() noexcept { return wjr::int128_t(0); }
    static constexpr wjr::int128_t quiet_NaN() noexcept { return wjr::int128_t(0); }
    static constexpr wjr::int128_t signaling_NaN() noexcept { return wjr::int128_t(0); }
    static constexpr wjr::int128_t denorm_min() noexcept { return wjr::int128_t(0); }
};

} // namespace std

#endif // WJR_MATH_INT128_T_HPP__
