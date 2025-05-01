#ifndef WJR_BIGINTEGER_DETAIL_PRECOMPUTE_CHARS_CONVERT_HPP__
#define WJR_BIGINTEGER_DETAIL_PRECOMPUTE_CHARS_CONVERT_HPP__

#include <wjr/biginteger/detail/mul.hpp>

namespace wjr {

struct precompute_chars_convert_16n_t {
    uint64_t big_base;
    uint32_t n;
    uint32_t digits_in_one_base;
    uint32_t digits_in_sixteen_base;
    uint64_t arr[16];
};

struct precompute_chars_convert_t {
    const uint64_t *ptr;
    uint32_t n;
    uint32_t shift;
    uint32_t digits_in_base;
    unsigned int base;
};

extern const std::array<const precompute_chars_convert_16n_t *, 37>
    precompute_chars_convert_16n_ptr;

extern WJR_ALL_NONNULL WJR_RETURNS_NONNULL precompute_chars_convert_t *
precompute_chars_convert(precompute_chars_convert_t *pre_table, size_t n, unsigned int base,
                         uint64_t *mem_table) noexcept;

} // namespace wjr

#endif // WJR_BIGINTEGER_DETAIL_PRECOMPUTE_CHARS_CONVERT_HPP__