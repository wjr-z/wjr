#ifndef WJR_MATH_ADD_IMPL_HPP__
#define WJR_MATH_ADD_IMPL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T, typename U,
          WJR_REQUIRES(is_nonbool_unsigned_integral_v<T> &&is_unsigned_integral_v<U>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 T addc(T a, T b, type_identity_t<U> c_in,
                                               U &c_out) noexcept;

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 T addc_cc(T a, T b, uint8_t c_in, uint8_t &c_out) noexcept;

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 bool add_overflow(type_identity_t<T> a,
                                                          type_identity_t<T> b, T &ret) noexcept;

WJR_INTRINSIC_CONSTEXPR void __add_128(uint64_t &al, uint64_t &ah, uint64_t lo0, uint64_t hi0,
                                         uint64_t lo1, uint64_t hi1) noexcept;

WJR_INTRINSIC_CONSTEXPR20 uint64_t __addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                              uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                              uint64_t c_in) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 uint8_t __addc_cc_128(uint64_t &al, uint64_t &ah,
                                                              uint64_t lo0, uint64_t hi0,
                                                              uint64_t lo1, uint64_t hi1,
                                                              uint8_t c_in) noexcept;

} // namespace wjr

#endif // WJR_MATH_ADD_IMPL_HPP__