#ifndef WJR_BIGINTEGER_DETAIL_DIV_IMPL_HPP__
#define WJR_BIGINTEGER_DETAIL_DIV_IMPL_HPP__

#include <utility>

#include <wjr/math/div-impl.hpp>
#include <wjr/math/integral_constant.hpp>

namespace wjr {

WJR_INTRINSIC_INLINE void div_qr_1(uint64_t *dst, uint64_t &rem, const uint64_t *src, size_t n,
                                   const div2by1_divider<uint64_t> &div) noexcept;

WJR_INTRINSIC_INLINE void div_qr_1(uint64_t *dst, uint64_t &rem, const uint64_t *src, size_t n,
                                   uint64_t div) noexcept;

WJR_INTRINSIC_INLINE void div_qr_2(uint64_t *dst, uint64_t *rem, const uint64_t *src, size_t n,
                                   const div3by2_divider<uint64_t> &div) noexcept;

WJR_INTRINSIC_INLINE void div_qr_2(uint64_t *dst, uint64_t *rem, const uint64_t *src, size_t n,
                                   const uint64_t *div) noexcept;

WJR_INTRINSIC_INLINE void div_qr_s(uint64_t *dst, uint64_t *rem, const uint64_t *src, size_t n,
                                   const uint64_t *div, size_t m) noexcept;

WJR_INTRINSIC_CONSTEXPR20 uint64_t divexact_dbm1c(uint64_t *dst, const uint64_t *src, size_t n,
                                                  uint64_t bd, uint64_t h) noexcept;

/// @brief Automatically select the optimal strategy
template <uint64_t c>
WJR_INTRINSIC_CONSTEXPR20 void divexact_byc(uint64_t *dst, const uint64_t *src, size_t n,
                                            integral_constant<uint64_t, c>,
                                            WJR_MAYBE_UNUSED uint64_t cf) noexcept;

WJR_INTRINSIC_CONSTEXPR20 void divexact_1(uint64_t *dst, const uint64_t *src, size_t n,
                                          const divexact1_divider<uint64_t> &div) noexcept;

WJR_INTRINSIC_CONSTEXPR20 void divexact_1(uint64_t *dst, const uint64_t *src, size_t n,
                                          uint64_t div) noexcept;

WJR_PURE WJR_INTRINSIC_CONSTEXPR20 uint64_t mod_1(const uint64_t *src, size_t n,
                                                  const div2by1_divider<uint64_t> &div) noexcept;

WJR_PURE WJR_INTRINSIC_CONSTEXPR20 uint64_t mod_1(const uint64_t *src, size_t n,
                                                  uint64_t div) noexcept;

} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_DIV_IMPL_HPP__