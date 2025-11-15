#ifndef WJR_MATH_ADD_IMPL_HPP__
#define WJR_MATH_ADD_IMPL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {
namespace math {
template <nonbool_unsigned_integral T, nonbool_unsigned_integral U>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR T addc(T a, T b, type_identity_t<U> c_in,
                                               U &c_out) noexcept;

template <nonbool_unsigned_integral T>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR T addc_cc(T a, T b, uint8_t c_in, uint8_t &c_out) noexcept;

template <nonbool_unsigned_integral T>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR bool add_overflow(type_identity_t<T> a,
                                                          type_identity_t<T> b, T &ret) noexcept;

WJR_INTRINSIC_CONSTEXPR void add_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                     uint64_t lo1, uint64_t hi1) noexcept;

WJR_INTRINSIC_CONSTEXPR uint64_t _addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                             uint64_t lo1, uint64_t hi1, uint64_t c_in) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR uint8_t _addc_cc_128(uint64_t &al, uint64_t &ah,
                                                             uint64_t lo0, uint64_t hi0,
                                                             uint64_t lo1, uint64_t hi1,
                                                             uint8_t c_in) noexcept;
} // namespace math

using math::add_128;
using math::add_overflow;
using math::addc;
using math::addc_cc;

} // namespace wjr

#endif // WJR_MATH_ADD_IMPL_HPP__