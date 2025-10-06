#ifndef WJR_JSON_STRING_HPP__
#define WJR_JSON_STRING_HPP__

#include <array>

#include <wjr/format/utf8/utf8.hpp>
#include <wjr/json/detail.hpp>

namespace wjr::json {
namespace string_detail {

inline constexpr std::array<uint8_t, 256> escape_table = {
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, // 0x0.
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0x22, 0, 0, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0x2f, 0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0, 0, 0,

    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, // 0x4.
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0x5c, 0, 0,    0, // 0x5.
    0, 0, 0x08, 0, 0,    0, 0x0c, 0,    0, 0, 0, 0, 0,    0, 0x0a, 0, // 0x6.
    0, 0, 0x0d, 0, 0x09, 0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, // 0x7.

    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0,

    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0,    0, 0, 0, 0, 0,    0, 0,    0,
};

} // namespace string_detail

/// @private String parsing implementation
namespace detail {

inline result<char *> parse_string(char *dst, const char *first, const char *last) noexcept {
    if (auto ret = utf8::unicode_to_utf8(dst, first, last); WJR_UNLIKELY(!ret)) {
        return unexpected(error_code::UTF8_ERROR);
    } else {
        return *ret;
    }
}

inline result<void> check_string(const char *first, const char *last) noexcept {
    if (auto ret = utf8::check_unicode(first, last); WJR_UNLIKELY(!ret)) {
        return unexpected(error_code::UTF8_ERROR);
    } else {
        return {};
    }
}

} // namespace detail

} // namespace wjr::json

#endif // WJR_JSON_STRING_HPP__