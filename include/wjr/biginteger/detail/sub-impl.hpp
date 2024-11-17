#ifndef WJR_BIGINTEGER_DETAIL_SUB_IMPL_HPP__
#define WJR_BIGINTEGER_DETAIL_SUB_IMPL_HPP__

#include <wjr/math/sub-impl.hpp>

namespace wjr {

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 uint64_t subc_1(uint64_t *dst, const uint64_t *src0,
                                                        size_t n, uint64_t src1,
                                                        uint64_t c_in = 0) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 uint64_t subc_n(uint64_t *dst, const uint64_t *src0,
                                                        const uint64_t *src1, size_t n,
                                                        uint64_t c_in = 0) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 uint64_t subc_s(uint64_t *dst, const uint64_t *src0,
                                                        size_t n, const uint64_t *src1, size_t m,
                                                        uint64_t c_in = 0) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 uint64_t subc_sz(uint64_t *dst, const uint64_t *src0,
                                                         size_t n, const uint64_t *src1, size_t m,
                                                         uint64_t c_in = 0) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_n(uint64_t *dst, const uint64_t *src0,
                                                           const uint64_t *src1, size_t n) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_n_pos(uint64_t *dst, const uint64_t *src0,
                                                               const uint64_t *src1,
                                                               size_t n) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_s1(uint64_t *dst, const uint64_t *src0,
                                                            size_t n, const uint64_t *src1,
                                                            size_t m) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_s(uint64_t *dst, const uint64_t *src0,
                                                           size_t n, const uint64_t *src1,
                                                           size_t m) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_s_pos(uint64_t *dst, const uint64_t *src0,
                                                               size_t n, const uint64_t *src1,
                                                               size_t m) noexcept;

WJR_NODISCARD WJR_INTRINSIC_CONSTEXPR20 ssize_t abs_subc_n(uint64_t *dst, const uint64_t *src0,
                                                           const uint64_t *src1, size_t n,
                                                           uint64_t &c_out, uint64_t cf0,
                                                           uint64_t cf1) noexcept;

} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_SUB_IMPL_HPP__