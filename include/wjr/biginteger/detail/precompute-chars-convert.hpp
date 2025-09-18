#ifndef WJR_BIGINTEGER_DETAIL_PRECOMPUTE_CHARS_CONVERT_HPP__
#define WJR_BIGINTEGER_DETAIL_PRECOMPUTE_CHARS_CONVERT_HPP__

#include <array>
#include <cstddef>
#include <cstdint>
#include <wjr/config.hpp>

namespace wjr {

inline constexpr std::array<uint32_t, 37> precompute_chars_convert_digits_in_one_base = {
    UINT32_C(0),  UINT32_C(0),  UINT32_C(0),  UINT32_C(40), UINT32_C(0),  UINT32_C(27),
    UINT32_C(24), UINT32_C(22), UINT32_C(0),  UINT32_C(20), UINT32_C(19), UINT32_C(18),
    UINT32_C(17), UINT32_C(17), UINT32_C(16), UINT32_C(16), UINT32_C(0),  UINT32_C(15),
    UINT32_C(15), UINT32_C(15), UINT32_C(14), UINT32_C(14), UINT32_C(14), UINT32_C(14),
    UINT32_C(13), UINT32_C(13), UINT32_C(13), UINT32_C(13), UINT32_C(13), UINT32_C(13),
    UINT32_C(13), UINT32_C(12), UINT32_C(0),  UINT32_C(12), UINT32_C(12), UINT32_C(12),
    UINT32_C(12)};

struct precompute_chars_convert_t {
    const uint64_t *ptr;
    uint32_t size;
    uint32_t shift;
    uint32_t digits_in_base;
    unsigned int base;
};

extern WJR_ALL_NONNULL WJR_RETURNS_NONNULL precompute_chars_convert_t *
precompute_chars_convert(precompute_chars_convert_t *pre_table, size_t n, unsigned int base,
                         uint64_t *mem_table) noexcept;

} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_PRECOMPUTE_CHARS_CONVERT_HPP__