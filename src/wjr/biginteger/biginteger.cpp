#include <wjr/biginteger.hpp>

namespace wjr {

namespace biginteger_detail {

template from_chars_result<const char *> __from_chars_impl<default_biginteger_storage>(
    const char *first, const char *last,
    basic_biginteger<default_biginteger_storage> *dst, unsigned int base) noexcept;

template void __addsub_impl<false, default_biginteger_storage>(
    basic_biginteger<default_biginteger_storage> *dst, const biginteger_data *lhs,
    const biginteger_data *rhs) noexcept;

template void __addsub_impl<true, default_biginteger_storage>(
    basic_biginteger<default_biginteger_storage> *dst, const biginteger_data *lhs,
    const biginteger_data *rhs) noexcept;

template void
__mul_impl<default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                       const biginteger_data *lhs,
                                       const biginteger_data *rhs) noexcept;

template void
__sqr_impl<default_biginteger_storage>(basic_biginteger<default_biginteger_storage> *dst,
                                       const biginteger_data *src) noexcept;

template void __addsubmul_impl<default_biginteger_storage>(
    basic_biginteger<default_biginteger_storage> *dst, const biginteger_data *lhs,
    const biginteger_data *rhs, int32_t xmask) noexcept;

} // namespace biginteger_detail

} // namespace wjr