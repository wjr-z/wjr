#ifndef WJR_JSON_NUMBER_HPP__
#define WJR_JSON_NUMBER_HPP__

#include <wjr/format/charconv.hpp>
#include <wjr/json/detail.hpp>

namespace wjr::json {

namespace detail {

WJR_PURE WJR_INTRINSIC_INLINE result<void> check_number(const char *first,
                                                        const char *last) noexcept {
    WJR_ASSERT_ASSUME(first != last);
    return {};
}

WJR_PURE WJR_INTRINSIC_INLINE basic_value parse_number(
    WJR_MAYBE_UNUSED const char *first, WJR_MAYBE_UNUSED const char *last) noexcept {
    return basic_value(null_v);
}

} // namespace detail

} // namespace wjr::json

#endif // WJR_JSON_NUMBER_HPP__