#ifndef WJR_MATH_PRECOMPUTE_CONVERT_HPP__
#define WJR_MATH_PRECOMPUTE_CONVERT_HPP__

#include <wjr/math/div.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

struct precompute_to_chars_8n_t {
    uint64_t big_base;
    size_t digits_in_one_base;
    uint64_t arr[8];
};

// exclue 0, 1, 2, 4, 8, 16, 32
extern "C" precompute_to_chars_8n_t precompute_to_chars_8n[36 - 6];
// exclue 0
extern "C" precompute_to_chars_8n_t *precompute_to_chars_8n_ptr[37];

struct precompute_to_chars_t {
    const uint64_t *ptr;
    size_t n;
    size_t digits_in_base;
    unsigned int base;
};

inline precompute_to_chars_t *precompute_to_chars(precompute_to_chars_t *pre,
                                                  unsigned int base, size_t n,
                                                  uint64_t *table_mem) {
    const precompute_to_chars_8n_t *p8n = precompute_to_chars_8n_ptr[base];
    const uint64_t big_base = p8n->big_base;
    const size_t digits_in_one_base = p8n->digits_in_one_base;

    auto set = [base](precompute_to_chars_t *pre, const uint64_t *ptr, size_t n,
                      size_t digits_in_base) {
        *pre = {ptr, n, digits_in_base, base};
    };

    size_t digits = 8;
    size_t digits_in_base = digits_in_one_base;

    set(pre, p8n->arr, digits, digits_in_base);

    while (n >= (digits << 1)) {
        sqr(table_mem, pre->ptr, digits);
        digits <<= 1;
        digits_in_base <<= 1;
        if (WJR_UNLIKELY(table_mem[digits - 1] == 0)) {
            table_mem[digits - 1] = mul_1(table_mem, table_mem, digits - 1, big_base);
            digits_in_base += digits_in_one_base;
        }
        ++pre;
        set(pre, table_mem, digits, digits_in_base);
        table_mem += digits;
    }

    return pre;
}

} // namespace wjr

#endif // WJR_MATH_PRECOMPUTE_CONVERT_HPP__