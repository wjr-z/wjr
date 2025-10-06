/**
 * @file rotate.hpp
 * @brief Bit rotation operations
 * @author wjr
 *
 * Provides circular bit shift operations (rotate left and rotate right).
 * Similar to C++20 std::rotl and std::rotr.
 */

#ifndef WJR_MATH_ROTATE_HPP__
#define WJR_MATH_ROTATE_HPP__

#include <wjr/math/detail.hpp>

namespace wjr {

/// @private
template <typename T>
WJR_CONST constexpr T fallback_rotl(T x, unsigned int s) noexcept {
    constexpr auto mask = static_cast<unsigned int>(std::numeric_limits<T>::digits) - 1;
    s &= mask;
    return (x << s) | (x >> ((-s) & mask));
}

/**
 * @brief Rotate left (circular left shift)
 *
 * Rotates the bits of x left by s positions. Bits shifted out on the left
 * are rotated back in on the right. Equivalent to C++20 std::rotl.
 *
 * @tparam T Unsigned integral type
 * @param[in] x Value to rotate
 * @param[in] s Rotation count
 * @return T Rotated value
 */
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

/**
 * @brief Rotate right (circular right shift)
 *
 * Rotates the bits of x right by s positions. Bits shifted out on the right
 * are rotated back in on the left. Equivalent to C++20 std::rotr.
 *
 * @tparam T Unsigned integral type
 * @param[in] x Value to rotate
 * @param[in] s Rotation count
 * @return T Rotated value
 */
template <typename T>
WJR_CONST constexpr T rotr(T x, int s) noexcept {
    return fallback_rotr(x, s);
}

} // namespace wjr

#endif // WJR_MATH_ROTATE_HPP__