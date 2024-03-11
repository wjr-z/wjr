#ifndef WJR_MATH_SUB_IMPL_HPP__
#define WJR_MATH_SUB_IMPL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E T subc(T a, T b, type_identity_t<U> c_in, U &c_out);

template <
    typename T, typename U = unsigned char,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U subc_1(T *dst, const T *src0, size_t n,
                                   type_identity_t<T> src1, U c_in = 0);

template <
    typename T, typename U = unsigned char,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U subc_n(T *dst, const T *src0, const T *src1, size_t n,
                                   U c_in = 0);

template <
    typename T, typename U = unsigned char,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U subc_s(T *dst, const T *src0, size_t n, const T *src1,
                                   size_t m, U c_in = 0);

template <
    typename T, typename U = unsigned char,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E U subc_sz(T *dst, const T *src0, size_t n, const T *src1,
                                    size_t m, U c_in = 0);

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_n(T *dst, const T *src0, const T *src1,
                                             size_t n);

template <typename T, std::enable_if_t<is_unsigned_integral_v<T>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_s(T *dst, const T *src0, size_t n,
                                             const T *src1, size_t m);

template <
    typename T, typename U,
    std::enable_if_t<is_unsigned_integral_v<T> && is_unsigned_integral_v<U>, int> = 0>
WJR_INTRINSIC_CONSTEXPR_E ssize_t abs_subc_n(T *dst, const T *src0, const T *src1,
                                             size_t n, U &c_out, type_identity_t<U> cf0,
                                             type_identity_t<U> cf1);

// preview :

WJR_INTRINSIC_CONSTEXPR_E void __subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                          uint64_t hi0, uint64_t lo1, uint64_t hi1);

WJR_INTRINSIC_CONSTEXPR_E uint64_t __subc_128(uint64_t &al, uint64_t &ah, uint64_t lo0,
                                              uint64_t hi0, uint64_t lo1, uint64_t hi1,
                                              uint64_t c_in);

} // namespace wjr

#endif // WJR_MATH_SUB_IMPL_HPP__