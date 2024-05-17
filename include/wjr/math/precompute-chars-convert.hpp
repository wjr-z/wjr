#ifndef WJR_MATH_PRECOMPUTE_CHARS_CONVERT_HPP__
#define WJR_MATH_PRECOMPUTE_CHARS_CONVERT_HPP__

#include <wjr/math/mul.hpp>

namespace wjr {

struct precompute_chars_convert_16n_t {
    uint64_t big_base;
    size_t n;
    int digits_in_one_base;
    int digits_in_sixteen_base;
    uint64_t arr[16];
};

struct precompute_chars_convert_t {
    const uint64_t *ptr;
    size_t n;
    size_t shift;
    size_t digits_in_base;
    unsigned int base;
};

extern const std::array<const precompute_chars_convert_16n_t *, 37>
    precompute_chars_convert_16n_ptr;

extern precompute_chars_convert_t *
precompute_chars_convert(precompute_chars_convert_t *pre, size_t n, unsigned int base,
                         uint64_t *table_mem) noexcept;

} // namespace wjr

#endif // WJR_MATH_PRECOMPUTE_CHARS_CONVERT_HPP__