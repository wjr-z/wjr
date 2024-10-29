#include <wjr/format/fastfloat.hpp>

namespace wjr::fastfloat {

template from_chars_result<>
__from_chars_impl<default_writer<float>, integral_constant<chars_format, chars_format::general>>(
    const char *first, const char *last, default_writer<float> wr,
    integral_constant<chars_format, chars_format::general> options) noexcept;

template from_chars_result<>
__from_chars_impl<default_writer<double>, integral_constant<chars_format, chars_format::general>>(
    const char *first, const char *last, default_writer<double> wr,
    integral_constant<chars_format, chars_format::general> options) noexcept;

template from_chars_result<> __from_chars_impl<default_writer<float>, chars_format>(
    const char *first, const char *last, default_writer<float> wr, chars_format fmt) noexcept;

template from_chars_result<> __from_chars_impl<default_writer<double>, chars_format>(
    const char *first, const char *last, default_writer<double> wr, chars_format fmt) noexcept;

} // namespace wjr::fastfloat