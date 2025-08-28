#ifndef WJR_BIGINTEGER_DETAIL_POW_HPP__
#define WJR_BIGINTEGER_DETAIL_POW_HPP__

#include <cstddef>
#include <cstdint>

#include <wjr/preprocessor.hpp>

namespace wjr {
extern WJR_ALL_NONNULL size_t pow_1(uint64_t *dst, const uint64_t *src, size_t n, uint64_t exp,
                                    uint64_t *tp) noexcept;
} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_POW_HPP__