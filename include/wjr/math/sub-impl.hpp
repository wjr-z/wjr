#ifndef WJR_MATH_SUB_IMPL_HPP__
#define WJR_MATH_SUB_IMPL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {
namespace math {
template <typename T, typename U>
requires(is_nonbool_unsigned_integral_v<T> && is_unsigned_integral_v<U>)
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 T subc(T a, T b, type_identity_t<U> c_in,
                                               U &c_out) noexcept;

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 T subc_cc(T a, T b, uint8_t c_in, uint8_t &c_out) noexcept;

template <typename T>
requires(is_nonbool_unsigned_integral_v<T>)
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool sub_overflow(type_identity_t<T> a,
                                                          type_identity_t<T> b, T &ret) noexcept;

WJR_INTRINSIC_CONSTEXPR void sub_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                     uint64_t lo1, uint64_t hi1) noexcept;

WJR_INTRINSIC_CONSTEXPR20 uint64_t _subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                             uint64_t lo1, uint64_t hi1, uint64_t c_in) noexcept;

WJR_INTRINSIC_CONSTEXPR20 uint8_t _subc_cc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                               uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                               uint8_t c_in) noexcept;
} // namespace math

using math::sub_128;
using math::sub_overflow;
using math::subc;
using math::subc_cc;

} // namespace wjr

#endif // WJR_MATH_SUB_IMPL_HPP__