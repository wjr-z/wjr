#ifndef WJR_MATH_DIV_IMPL_HPP__
#define WJR_MATH_DIV_IMPL_HPP__

#include <utility>

#include <wjr/tuple.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T>
class div1by1_divider;

template <typename T>
class div2by1_divider;

template <typename T>
class div3by2_divider;

template <typename T>
class divexact1_divider;

inline uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                               const div2by1_divider<uint64_t> &divider);

inline uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi, uint64_t div);

inline std::pair<uint64_t, uint64_t>
div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                const div2by1_divider<uint64_t> &divider);

inline std::pair<uint64_t, uint64_t> div128by64to128(uint64_t &rem, uint64_t lo,
                                                     uint64_t hi, uint64_t div);

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        const div2by1_divider<T> &div);

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(T *dst, T &rem, const T *src, size_t n,
                                        type_identity_t<T> div);

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(T *dst, T *rem, const T *src, size_t n,
                                        const div3by2_divider<T> &div);

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(T *dst, T *rem, const T *src, size_t n,
                                        const T *div);

template <typename T>
void div_qr_s(T *dst, T *rem, const T *src, size_t n, const T *div, size_t m);

template <typename T, WJR_REQUIRES(std::is_same_v<T, uint64_t>)>
WJR_CONSTEXPR_E T divexact_dbm1c(T *dst, const T *src, size_t n, T bd, T h);

template <typename T, WJR_REQUIRES(std::is_same_v<T, uint64_t>)>
WJR_CONSTEXPR_E void divexact_by3(T *dst, const T *src, size_t n);

template <typename T, WJR_REQUIRES(std::is_same_v<T, uint64_t>)>
WJR_CONSTEXPR_E void divexact_by5(T *dst, const T *src, size_t n);

template <typename T, WJR_REQUIRES(std::is_same_v<T, uint64_t>)>
WJR_CONSTEXPR_E void divexact_by15(T *dst, const T *src, size_t n);

template <typename T, T c, WJR_REQUIRES(std::is_same_v<T, uint64_t>)>
WJR_CONSTEXPR_E void divexact_byc(T *dst, const T *src, size_t n,
                                  std::integral_constant<T, c>);

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          const divexact1_divider<T> &div);

template <typename T, WJR_REQUIRES(is_nonbool_unsigned_integral_v<T>)>
WJR_INTRINSIC_CONSTEXPR_E void divexact_1(T *dst, const T *src, size_t n,
                                          type_identity_t<T> div);

} // namespace wjr

#endif // WJR_MATH_DIV_IMPL_HPP__