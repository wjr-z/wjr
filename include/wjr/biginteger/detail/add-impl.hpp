#ifndef WJR_BIGINTEGER_DETAIL_ADD_IMPL_HPP__
#define WJR_BIGINTEGER_DETAIL_ADD_IMPL_HPP__

#include <wjr/math/add-impl.hpp>

namespace wjr {

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 uint64_t addc_1(uint64_t *dst, const uint64_t *src0,
                                                        size_t n, uint64_t src1,
                                                        uint64_t c_in = 0) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 uint64_t addc_n(uint64_t *dst, const uint64_t *src0,
                                                        const uint64_t *src1, size_t n,
                                                        uint64_t c_in = 0) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 uint64_t addc_s(uint64_t *dst, const uint64_t *src0,
                                                        size_t n, const uint64_t *src1, size_t m,
                                                        uint64_t c_in = 0) noexcept;

/// @note m can be zero
WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 uint64_t addc_sz(uint64_t *dst, const uint64_t *src0,
                                                         size_t n, const uint64_t *src1, size_t m,
                                                         uint64_t c_in = 0) noexcept;

} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_ADD_IMPL_HPP__