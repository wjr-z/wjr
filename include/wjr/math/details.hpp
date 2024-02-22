#ifndef WJR_MATH_DETAILS_HPP__
#define WJR_MATH_DETAILS_HPP__

#include <wjr/stack_allocator.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {
namespace math_details {

template <typename T, T seed>
class de_bruijn {
public:
    constexpr static uint8_t digits = std::numeric_limits<T>::digits;
    constexpr static uint8_t mv = digits == 32 ? 27 : 58;
    constexpr de_bruijn() : lookup(), lookupr() { initialize(); }

    WJR_INTRINSIC_CONSTEXPR int get(T idx) const { return lookup[(idx * seed) >> mv]; }
    WJR_INTRINSIC_CONSTEXPR int getr(T idx) const { return lookupr[(idx * seed) >> mv]; }

private:
    constexpr void initialize() {
        for (uint8_t i = 0; i < digits; ++i) {
            auto idx = (seed << i) >> mv;
            lookup[idx] = i;
            lookupr[idx] = i == 0 ? 0 : digits - i;
        }
    }

    uint8_t lookup[digits];
    uint8_t lookupr[digits];
};

inline constexpr de_bruijn<uint32_t, 0x077C'B531> de_bruijn32 = {};
inline constexpr de_bruijn<uint64_t, 0x03f7'9d71'b4ca'8b09> de_bruijn64 = {};

using default_stack_allocator =
    stack_allocator<6 * 1024, 2 * 1024, 4, 48 * 1024, 16 * 1024, 2>;
inline constexpr default_stack_allocator stack_alloc = {};

} // namespace math_details

// preview ...

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool is_power_of_two(T n) noexcept {
    return (n & (n - 1)) == 0;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T lowbit(T n) noexcept {
    return n & -n;
}

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR bool __has_high_bit(T n) noexcept {
    return n >> (std::numeric_limits<T>::digits - 1);
}

// align_up (n, alignment) - n
template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_CONST WJR_INTRINSIC_CONSTEXPR T __align_up_offset(T n, type_identity_t<T> alignment) {
    return (-n) & (alignment - 1);
}

} // namespace wjr

#endif // WJR_MATH_DETAILS_HPP__