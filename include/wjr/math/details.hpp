#ifndef WJR_MATH_DETAILS_HPP__
#define WJR_MATH_DETAILS_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {
namespace math_details {

template <typename T, T seed>
class de_bruijn {
public:
    constexpr static uint8_t digits = std::numeric_limits<T>::digits;
    constexpr static uint8_t mv = digits == 32 ? 27 : 58;
    constexpr de_bruijn() : lookup(), lookupr() { initial(); }

    WJR_INTRINSIC_CONSTEXPR int get(T idx) const { return lookup[(idx * seed) >> mv]; }
    WJR_INTRINSIC_CONSTEXPR int getr(T idx) const { return lookupr[(idx * seed) >> mv]; }

private:
    constexpr void initial() {
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

} // namespace math_details

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
bool is_power_of_two(T x) {
    return (x & (x - 1)) == 0;
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
T lowbit(T x) {
    if (is_constant_p(is_power_of_two(x)) && is_power_of_two(x)) {
        return x;
    }

    return x & -x;
}

} // namespace wjr

#endif // WJR_MATH_DETAILS_HPP__