#ifndef WJR_MATH_ROTATE_HPP__
#define WJR_MATH_ROTATE_HPP__

#include <wjr/math/detail.hpp>

namespace wjr {

template <typename T>
WJR_CONST constexpr T fallback_rotl(T x, unsigned int s) noexcept {
    constexpr auto mask = static_cast<unsigned int>(std::numeric_limits<T>::digits) - 1;
    s &= mask;
    return (x << s) | (x >> ((-s) & mask));
}

template <typename T>
WJR_CONST constexpr T rotl(T x, int s) noexcept {
    return fallback_rotl(x, s);
}

template <typename T>
WJR_CONST constexpr T fallback_rotr(T x, unsigned int s) noexcept {
    constexpr auto mask = static_cast<unsigned int>(std::numeric_limits<T>::digits) - 1;
    s &= mask;
    return (x >> s) | (x << ((-s) & mask));
}

template <typename T>
WJR_CONST constexpr T rotr(T x, int s) noexcept {
    return fallback_rotr(x, s);
}

} // namespace wjr

#endif // WJR_MATH_ROTATE_HPP__