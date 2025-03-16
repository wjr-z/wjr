#ifndef WJR_MATH_DIV_IMPL_HPP__
#define WJR_MATH_DIV_IMPL_HPP__

#include <utility>

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T>
class div2by1_divider;

template <typename T>
class div3by2_divider;

template <typename T>
class divexact1_divider;

class uint128_t;

WJR_INLINE_CONSTEXPR20 uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                                               const div2by1_divider<uint64_t> &divider) noexcept;

/**
 * @todo Using the `div` instruction directly on some machines may be faster
 */
WJR_INLINE_CONSTEXPR20 uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                                               uint64_t div) noexcept;

inline uint128_t div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                                 const div2by1_divider<uint64_t> &divider) noexcept;

/**
 * @todo Using the `div` instruction directly on some machines may be faster
 */
inline uint128_t div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi, uint64_t div) noexcept;

/**
 * @pre
 * divider >= 2^64
 * @details divider >= 2^64, so the result can be represented by uint64_t
 * @todo Using the `div` instruction directly on some machines may be faster
 */
inline uint64_t div128by128to64(uint128_t &rem, uint64_t lo, uint64_t hi,
                                const div3by2_divider<uint64_t> &divider) noexcept;

} // namespace wjr

#endif // WJR_MATH_DIV_IMPL_HPP__