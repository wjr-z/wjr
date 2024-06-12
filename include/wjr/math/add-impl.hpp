#ifndef WJR_MATH_ADD_IMPL_HPP__
#define WJR_MATH_ADD_IMPL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T, typename U,
          WJR_REQUIRES(is_nonbool_unsigned_integral_v<T> &&is_unsigned_integral_v<U>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR_E T addc(T a, T b, type_identity_t<U> c_in,
                                               U &c_out) noexcept;

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR_E T addc_cc(T a, T b, uint8_t c_in,
                                                  uint8_t &c_out) noexcept;

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR_E bool
add_overflow(type_identity_t<T> a, type_identity_t<T> b, T &ret) noexcept;

template <typename U = uint64_t, WJR_REQUIRES(is_unsigned_integral_v<U>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR_E U addc_1(uint64_t *dst, const uint64_t *src0,
                                                 size_t n, uint64_t src1,
                                                 U c_in = 0) noexcept;

template <typename U = uint64_t, WJR_REQUIRES(is_unsigned_integral_v<U>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR_E U addc_n(uint64_t *dst, const uint64_t *src0,
                                                 const uint64_t *src1, size_t n,
                                                 U c_in = 0) noexcept;

template <typename U = uint64_t, WJR_REQUIRES(is_unsigned_integral_v<U>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR_E U addc_s(uint64_t *dst, const uint64_t *src0,
                                                 size_t n, const uint64_t *src1, size_t m,
                                                 U c_in = 0) noexcept;

// m can be zero
template <typename U = uint64_t, WJR_REQUIRES(is_unsigned_integral_v<U>)>
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR_E U addc_sz(uint64_t *dst, const uint64_t *src0,
                                                  size_t n, const uint64_t *src1,
                                                  size_t m, U c_in = 0) noexcept;

WJR_INTRINSIC_CONSTEXPR_E void __add_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                         uint64_t hi0, uint64_t lo1,
                                         uint64_t hi1) noexcept;

WJR_INTRINSIC_CONSTEXPR_E uint64_t __addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                              uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                              uint64_t c_in) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR_E uint8_t __addc_cc_128(uint64_t &al, uint64_t &ah,
                                                              uint64_t lo0, uint64_t hi0,
                                                              uint64_t lo1, uint64_t hi1,
                                                              uint8_t c_in) noexcept;

} // namespace wjr

#endif // WJR_MATH_ADD_IMPL_HPP__