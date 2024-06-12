#ifndef WJR_MATH_DIV_IMPL_HPP__
#define WJR_MATH_DIV_IMPL_HPP__

#include <utility>

#include <wjr/tuple.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

template <typename T>
class div2by1_divider;

template <typename T>
class div3by2_divider;

template <typename T>
class divexact1_divider;

WJR_INLINE_CONSTEXPR20 uint64_t
div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
               const div2by1_divider<uint64_t> &divider) noexcept;

WJR_INLINE_CONSTEXPR20 uint64_t div128by64to64(uint64_t &rem, uint64_t lo, uint64_t hi,
                                               uint64_t div) noexcept;

inline tuple<uint64_t, uint64_t>
div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                const div2by1_divider<uint64_t> &divider) noexcept;

inline tuple<uint64_t, uint64_t> div128by64to128(uint64_t &rem, uint64_t lo, uint64_t hi,
                                                 uint64_t div) noexcept;

WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(uint64_t *dst, uint64_t &rem, const uint64_t *src,
                                        size_t n,
                                        const div2by1_divider<uint64_t> &div) noexcept;

WJR_INTRINSIC_CONSTEXPR20 void div_qr_1(uint64_t *dst, uint64_t &rem, const uint64_t *src,
                                        size_t n, uint64_t div) noexcept;

WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                                        size_t n,
                                        const div3by2_divider<uint64_t> &div) noexcept;

WJR_INTRINSIC_CONSTEXPR20 void div_qr_2(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                                        size_t n, const uint64_t *div) noexcept;

WJR_INTRINSIC_CONSTEXPR20 void div_qr_s(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                                        size_t n, const uint64_t *div, size_t m) noexcept;

WJR_INTRINSIC_CONSTEXPR20 uint64_t divexact_dbm1c(uint64_t *dst, const uint64_t *src,
                                                  size_t n, uint64_t bd,
                                                  uint64_t h) noexcept;

WJR_INTRINSIC_CONSTEXPR20 void divexact_dbm1c_shift(uint64_t *dst, const uint64_t *src,
                                                    size_t n, uint64_t bd, uint64_t cl,
                                                    uint64_t hicf) noexcept;

template <uint64_t c>
WJR_INTRINSIC_CONSTEXPR20 void divexact_byc(uint64_t *dst, const uint64_t *src, size_t n,
                                            integral_constant<uint64_t, c>,
                                            WJR_MAYBE_UNUSED uint64_t cf) noexcept;

WJR_INTRINSIC_CONSTEXPR20 void
divexact_1(uint64_t *dst, const uint64_t *src, size_t n,
           const divexact1_divider<uint64_t> &div) noexcept;

WJR_INTRINSIC_CONSTEXPR20 void divexact_1(uint64_t *dst, const uint64_t *src, size_t n,
                                          uint64_t div) noexcept;

WJR_PURE WJR_INTRINSIC_CONSTEXPR20 uint64_t
mod_1(const uint64_t *src, size_t n, const div2by1_divider<uint64_t> &div) noexcept;

WJR_PURE WJR_INTRINSIC_CONSTEXPR20 uint64_t mod_1(const uint64_t *src, size_t n,
                                                  uint64_t div) noexcept;

} // namespace wjr

#endif // WJR_MATH_DIV_IMPL_HPP__