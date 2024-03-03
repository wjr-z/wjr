#include <wjr/math/div.hpp>

namespace wjr {

// mul.hpp

template void
toom_interpolation_5p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_5p_struct<uint64_t> &&flag);

template void
toom_interpolation_6p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_6p_struct<uint64_t> &&flag);

template void toom43_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                     size_t n, const uint64_t *src1, size_t m,
                                     uint64_t *stk);

template void
toom_interpolation_7p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_7p_struct<uint64_t> &&flag);

template void toom53_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                     size_t n, const uint64_t *src1, size_t m,
                                     uint64_t *stk);

template void toom44_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                     size_t n, const uint64_t *src1, size_t m,
                                     uint64_t *stk);

// div.hpp

template uint64_t dc_div_qr_s<uint64_t>(uint64_t *dst, uint64_t *src, size_t n,
                                        const uint64_t *div, size_t m, uint64_t dinv);

} // namespace wjr