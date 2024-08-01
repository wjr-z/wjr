#ifndef WJR_MATH_UINT128_T_HPP__
#define WJR_MATH_UINT128_T_HPP__

#include <wjr/math/div-impl.hpp>
#include <wjr/math/divider.hpp>

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

    template <size_t I>
    constexpr uint64_t &get() & noexcept {
        if constexpr (I == 0) {
            return lo;
        } else {
            return hi;
        }
    }

    template <size_t I>
    constexpr const uint64_t &get() const & noexcept {
        if constexpr (I == 0) {
            return lo;
        } else {
            return hi;
        }
    }

    template <size_t I>
    constexpr uint64_t &&get() && noexcept {
        if constexpr (I == 0) {
            return std::move(lo);
        } else {
            return std::move(hi);
        }
    }

    template <size_t I>
    constexpr const uint64_t &&get() const && noexcept {
        if constexpr (I == 0) {
            return std::move(lo);
        } else {
            return std::move(hi);
        }
    }

    constexpr uint128_t(uint64_t lo_, uint64_t hi_) noexcept : lo(lo_), hi(hi_) {}

    template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
    constexpr uint128_t(T value) noexcept : lo(value), hi(0) {}

    template <typename T, WJR_REQUIRES(is_nonbool_signed_integral_v<T>)>
    constexpr uint128_t(T value) noexcept
        : lo(static_cast<T>(value)), hi(static_cast<T>(value >= 0 ? 0 : -1)) {}

    WJR_CONSTEXPR20 uint128_t &operator+=(uint128_t other) noexcept {
        __add_128(lo, hi, lo, hi, other.lo, other.hi);
        return *this;
    }

    friend WJR_CONST WJR_CONSTEXPR20 uint128_t operator+(uint128_t lhs,
                                                         uint128_t rhs) noexcept {
        return lhs += rhs;
    }

    WJR_CONSTEXPR20 uint128_t &operator-=(uint128_t other) noexcept {
        __sub_128(lo, hi, lo, hi, other.lo, other.hi);
        return *this;
    }

    friend WJR_CONST WJR_CONSTEXPR20 uint128_t operator-(uint128_t lhs,
                                                         uint128_t rhs) noexcept {
        return lhs -= rhs;
    }

    WJR_CONSTEXPR20 uint128_t &operator*=(uint128_t other) noexcept {
        const auto [__lo, __hi] = other;
        const uint64_t tmp = lo * __hi + hi * __lo;
        lo = mul(lo, __lo, hi);
        hi += tmp;
        return *this;
    }

private:
    static WJR_CONST WJR_CONSTEXPR20 uint128_t mul_u64(uint128_t lhs,
                                                       uint64_t value) noexcept {
        const auto [lo, hi] = lhs;
        uint128_t tmp;
        tmp.lo = mul(lo, value, tmp.hi);
        tmp.hi += hi * value;
        return tmp;
    }

    static WJR_CONST WJR_CONSTEXPR20 uint128_t mul_i64(uint128_t lhs,
                                                       int64_t value) noexcept {
        const auto [lo, hi] = lhs;
        const uint64_t uvalue = to_unsigned(value);
        uint128_t tmp;
        tmp.lo = mul(lo, uvalue, tmp.hi);
        tmp.hi += hi * uvalue + (value >= 0 ? 0 : -lo);
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

    friend WJR_CONST WJR_CONSTEXPR20 uint128_t operator*(uint128_t lhs,
                                                         uint128_t rhs) noexcept {
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

    friend WJR_CONST WJR_CONSTEXPR20 bool operator<(uint128_t lhs,
                                                    uint128_t rhs) noexcept {
        return __less_128(lhs.lo, lhs.hi, rhs.lo, rhs.hi);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator>(uint128_t lhs,
                                                    uint128_t rhs) noexcept {
        return rhs < lhs;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator<=(uint128_t lhs,
                                                     uint128_t rhs) noexcept {
        return __less_equal_128(lhs.lo, lhs.hi, rhs.lo, rhs.hi);
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator>=(uint128_t lhs,
                                                     uint128_t rhs) noexcept {
        return rhs <= lhs;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator==(uint128_t lhs,
                                                     uint128_t rhs) noexcept {
        return lhs.lo == rhs.lo && lhs.hi == rhs.hi;
    }

    friend WJR_CONST WJR_CONSTEXPR20 bool operator!=(uint128_t lhs,
                                                     uint128_t rhs) noexcept {
        return !(lhs == rhs);
    }

    uint64_t lo;
    uint64_t hi;
};

WJR_INTRINSIC_CONSTEXPR20 uint128_t mul64x64to128(uint64_t a, uint64_t b) noexcept {
    uint64_t lo, hi;
    lo = mul(a, b, hi);
    return uint128_t(lo, hi);
}

} // namespace wjr

namespace std {

template <>
struct tuple_size<wjr::uint128_t> : std::integral_constant<size_t, 2> {};

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