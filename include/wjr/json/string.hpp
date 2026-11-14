#ifndef WJR_JSON_STRING_HPP__
#define WJR_JSON_STRING_HPP__

#include <array>
#include <cstring>

#include <wjr/container/detail.hpp>
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

/// @private Preserve short decoded strings when their escaped representation exceeds capacity.
template <typename String>
result<bool> try_decode_short_string(String &output, const char *first, const char *last) {
    constexpr size_t scratch_size = 64;
    const auto length = static_cast<size_t>(last - first);
    if (length > scratch_size || length <= output.capacity() ||
        std::memchr(first, '\\', length) == nullptr) {
        return false;
    }
    char scratch[scratch_size];
    auto parsed = parse_string(scratch, first, last);
    if (!parsed) {
        return unexpected(parsed.error());
    }
    output.assign(scratch, static_cast<size_t>(*parsed - scratch));
    return true;
}

/// @private Decode into owned storage while retaining its allocator and capacity.
template <typename String>
result<void> decode_owned_string(String &output, const char *first, const char *last) {
    auto short_string = try_decode_short_string(output, first, last);
    if (!short_string) {
        return unexpected(short_string.error());
    }
    if (*short_string) {
        return {};
    }
    try_uninitialized_resize(output, static_cast<size_t>(last - first));
    auto parsed = parse_string(output.data(), first, last);
    if (!parsed) {
        return unexpected(parsed.error());
    }
    output.resize(static_cast<size_t>(*parsed - output.data()));
    return {};
}

} // namespace detail

} // namespace wjr::json

#endif // WJR_JSON_STRING_HPP__
