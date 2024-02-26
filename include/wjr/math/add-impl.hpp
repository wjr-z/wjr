#ifndef WJR_MATH_ADD_IMPL_HPP__
#define WJR_MATH_ADD_IMPL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E T addc(T a, T b, type_identity_t<U> c_in, U &c_out);

template <
    typename T, typename U = unsigned char,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U addc_1(T *dst, const T *src0, size_t n,
                                   type_identity_t<T> src1, U c_in = 0);

template <
    typename T, typename U = unsigned char,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U addc_n(T *dst, const T *src0, const T *src1, size_t n,
                                   U c_in = 0);

template <
    typename T, typename U = unsigned char,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U addc_s(T *dst, const T *src0, size_t n, const T *src1,
                                   size_t m, U c_in = 0);

// m can be zero
template <
    typename T, typename U = unsigned char,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U addc_sz(T *dst, const T *src0, size_t n, const T *src1,
                                    size_t m, U c_in = 0);

WJR_INTRINSIC_CONSTEXPR_E void __addc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                          uint64_t hi0, uint64_t lo1, uint64_t hi1);

} // namespace wjr

#endif // WJR_MATH_ADD_IMPL_HPP__