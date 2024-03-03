#ifndef WJR_MATH_PRECOMPUTE_CONVERT_IMPL_HPP__
#define WJR_MATH_PRECOMPUTE_CONVERT_IMPL_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {

struct precompute_to_chars_16n_t {
    uint64_t big_base;
    size_t n;
    size_t digits_in_one_base;
    uint64_t arr[16];
};

struct precompute_to_chars_t {
    const uint64_t *ptr;
    size_t n;
    size_t shift;
    size_t digits_in_base;
    unsigned int base;
};

// exclue 0
extern "C" precompute_to_chars_16n_t *precompute_to_chars_16n_ptr[37];

inline precompute_to_chars_t *precompute_to_chars(precompute_to_chars_t *pre,
                                                  unsigned int base, size_t n,
                                                  uint64_t *table_mem);

} // namespace wjr

#endif // WJR_MATH_PRECOMPUTE_CONVERT_IMPL_HPP__