#include <wjr/json/number.hpp>

namespace wjr::fastfloat {

template from_chars_result<>
__from_chars_impl<json::number_detail::number_writer,
                  integral_constant<chars_format, chars_format::json>>(
    const char *first, const char *last, json::number_detail::number_writer wr,
    integral_constant<chars_format, chars_format::json> options) noexcept;

}