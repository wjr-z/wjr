#ifndef WJR_MATH_PRECOMPUTE_CONVERT_HPP__
#define WJR_MATH_PRECOMPUTE_CONVERT_HPP__

#include <wjr/math/mul.hpp>

namespace wjr {

struct precompute_to_chars_16n_t {
    uint64_t big_base;
    size_t n;
    int digits_in_one_base;
    int digits_in_sixteen_base;
    uint64_t arr[16];
};

struct precompute_to_chars_t {
    const uint64_t *ptr;
    size_t n;
    size_t shift;
    size_t digits_in_base;
    unsigned int base;
};

extern "C" precompute_to_chars_16n_t *precompute_to_chars_16n_ptr[37];

extern "C" precompute_to_chars_t *precompute_to_chars(precompute_to_chars_t *pre,
                                                      size_t n, unsigned int base,
                                                      uint64_t *table_mem);

} // namespace wjr

#endif // WJR_MATH_PRECOMPUTE_CONVERT_HPP__