#ifndef WJR_JSON_STRING_HPP__
#define WJR_JSON_STRING_HPP__

#include <array>

#include <wjr/format/utf8/utf8.hpp>
#include <wjr/json/detail.hpp>

#if defined(WJR_X86)
#include <wjr/x86/json/string.hpp>
#endif

namespace wjr::json {

namespace string_detail {

inline constexpr std::array<uint8_t, 256> escape_table = {
    0, 0, 0,    0, 0,    0, 0,    0, 0, 0,    0, 0, 0,    0, 0,    0, // 0x0.
    0, 0, 0,    0, 0,    0, 0,    0, 0, 0,    0, 0, 0,    0, 0,    0, 0, 0, 0x22, 0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0, 0, 0x2f, 0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0,
    0, 0, 0,    0,

    0, 0, 0,    0, 0,    0, 0,    0, 0, 0,    0, 0, 0,    0, 0,    0, // 0x4.
    0, 0, 0,    0, 0,    0, 0,    0, 0, 0,    0, 0, 0x5c, 0, 0,    0, // 0x5.
    0, 0, 0x08, 0, 0,    0, 0x0c, 0, 0, 0,    0, 0, 0,    0, 0x0a, 0, // 0x6.
    0, 0, 0x0d, 0, 0x09, 0, 0,    0, 0, 0,    0, 0, 0,    0, 0,    0, // 0x7.

    0, 0, 0,    0, 0,    0, 0,    0, 0, 0,    0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0, 0, 0,    0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0, 0, 0,    0, 0, 0,    0, 0,    0, 0, 0, 0,    0,

    0, 0, 0,    0, 0,    0, 0,    0, 0, 0,    0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0, 0, 0,    0, 0, 0,    0, 0,    0, 0, 0, 0,    0, 0, 0,
    0, 0, 0,    0, 0,    0, 0,    0, 0, 0,    0, 0, 0,    0, 0,    0, 0, 0, 0,    0,
};

} // namespace string_detail

namespace detail {

WJR_INTRINSIC_INLINE result<const char *>
parse_unicode_codepoint(char *&dst, const char *first, const char *last) noexcept {
    if (const auto ret = utf8::parse_unicode_codepoint(
            reinterpret_cast<uint8_t *&>(dst), reinterpret_cast<const uint8_t *>(first),
            reinterpret_cast<const uint8_t *>(last));
        WJR_UNLIKELY(!ret)) {
        return unexpected(error_code::UTF8_ERROR);
    } else {
        return reinterpret_cast<const char *>(*ret);
    }
}

WJR_INTRINSIC_INLINE result<char *> generic_parse_string(char *dst, const char *first,
                                                         const char *last) noexcept {
    using namespace string_detail;

    if (WJR_UNLIKELY(first == last)) {
        return dst;
    }

    do {
        uint8_t ch = *first++;

        if (WJR_UNLIKELY(ch == '\\')) {
            WJR_ASSERT(first != last);
            ch = *first++;

            if (WJR_UNLIKELY(ch == 'u')) {
                WJR_EXPECTED_SET(first, parse_unicode_codepoint(dst, first, last));
            } else {
                const uint8_t code = escape_table[ch];

                if (WJR_UNLIKELY(code == 0)) {
                    return unexpected(error_code::STRING_ERROR);
                }

                *dst++ = code;
            }
        } else {
            *dst++ = ch;
        }
    } while (first != last);

    return dst;
}

#if WJR_HAS_BUILTIN(JSON_PARSE_STRING)
extern result<char *> parse_string(char *dst, const char *first,
                                   const char *last) noexcept;
#else
inline result<char *> parse_string(char *dst, const char *first,
                                   const char *last) noexcept {
    return generic_parse_string(dst, first, last);
}
#endif

WJR_INTRINSIC_INLINE result<const char *>
check_unicode_codepoint(const char *first, const char *last) noexcept {
    if (const auto ret =
            utf8::check_unicode_codepoint(reinterpret_cast<const uint8_t *>(first),
                                          reinterpret_cast<const uint8_t *>(last));
        WJR_UNLIKELY(!ret)) {
        return unexpected(error_code::UTF8_ERROR);
    } else {
        return reinterpret_cast<const char *>(*ret);
    }
}

WJR_INTRINSIC_INLINE result<void> generic_check_string(const char *first,
                                                       const char *last) noexcept {
    using namespace string_detail;

    if (WJR_UNLIKELY(first == last)) {
        return {};
    }

    do {
        uint8_t ch = *first++;

        if (WJR_UNLIKELY(ch == '\\')) {
            WJR_ASSERT(first != last);
            ch = *first++;

            if (WJR_UNLIKELY(ch == 'u')) {
                WJR_EXPECTED_SET(first, check_unicode_codepoint(first, last));
            } else {
                const uint8_t code = escape_table[ch];

                if (WJR_UNLIKELY(code == 0)) {
                    return unexpected(error_code::STRING_ERROR);
                }
            }
        }
    } while (first != last);

    return {};
}

#if WJR_HAS_BUILTIN(JSON_CHECK_STRING)
extern result<void> check_string(const char *first, const char *last) noexcept;
#else
inline result<void> check_string(const char *first, const char *last) noexcept {
    return generic_check_string(first, last);
}
#endif

} // namespace detail

} // namespace wjr::json

#endif // WJR_JSON_STRING_HPP__