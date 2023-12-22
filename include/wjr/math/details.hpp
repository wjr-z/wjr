#ifndef WJR_MATH_DETAILS_HPP__
#define WJR_MATH_DETAILS_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {
namespace math_details {
inline constexpr uint32_t de_bruijn32 = 0x077C'B531;
static constexpr uint8_t de_bruijn32_lookup[] = {
    0,  1,  28, 2,  29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4,  8,
    31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6,  11, 5,  10, 9,
};

inline constexpr uint64_t de_bruijn64 = 0x03f7'9d71'b4ca'8b09;
static constexpr uint8_t de_bruijn64_lookup[] = {
    0,  1,  56, 2,  57, 49, 28, 3,  61, 58, 42, 50, 38, 29, 17, 4,
    62, 47, 59, 36, 45, 43, 51, 22, 53, 39, 33, 30, 24, 18, 12, 5,
    63, 55, 48, 27, 60, 41, 37, 16, 46, 35, 44, 21, 52, 32, 23, 11,
    54, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9,  13, 8,  7,  6,
};

} // namespace math_details

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
bool is_power_of_two(T x) {
    return (x & (x - 1)) == 0;
}

template <typename T>
WJR_ATTRIBUTES(CONST, INTRINSIC_CONSTEXPR)
T lowbit(T x) {
    return x & -x;
}

} // namespace wjr

#endif // WJR_MATH_DETAILS_HPP__