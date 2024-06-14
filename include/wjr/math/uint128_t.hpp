#ifndef WJR_MATH_UINT128_T_HPP__
#define WJR_MATH_UINT128_T_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

/**
 * @brief temporary uint128_t for divide 128
 *
 * @todo implement more functions
 *
 */
struct uint128_t {
    WJR_CONSTEXPR20 uint128_t() noexcept = default;
    constexpr uint128_t(const uint128_t &) noexcept = default;
    constexpr uint128_t(uint128_t &&) noexcept = default;
    constexpr uint128_t &operator=(const uint128_t &) noexcept = default;
    constexpr uint128_t &operator=(uint128_t &&) noexcept = default;
    ~uint128_t() noexcept = default;

    constexpr uint128_t(uint64_t lo, uint64_t hi) noexcept : lo(lo), hi(hi) {}

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

    uint64_t lo;
    uint64_t hi;
};

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