#ifndef WJR_JSON_PARSER_HPP__
#define WJR_JSON_PARSER_HPP__

#include <array>
#include <bitset>

#include <wjr/expected.hpp>
#include <wjr/format/charconv.hpp>
#include <wjr/json/reader.hpp>
#include <wjr/math/stack_allocator.hpp>

#if defined(WJR_X86)
#include <wjr/x86/json/parser.hpp>
#endif

namespace wjr::json {

enum class error_code {
    SUCCESS = 0,     ///< No error
    CAPACITY,        ///< This parser can't support a document that big
    MEMALLOC,        ///< Error allocating memory, most likely out of memory
    TAPE_ERROR,      ///< Something went wrong, this is a generic error
    DEPTH_ERROR,     ///< Your document exceeds the user-specified depth limitation
    STRING_ERROR,    ///< Problem while parsing a string
    T_ATOM_ERROR,    ///< Problem while parsing an atom starting with the letter 't'
    F_ATOM_ERROR,    ///< Problem while parsing an atom starting with the letter 'f'
    N_ATOM_ERROR,    ///< Problem while parsing an atom starting with the letter 'n'
    NUMBER_ERROR,    ///< Problem while parsing a number
    BIGINT_ERROR,    ///< The integer value exceeds 64 bits
    UTF8_ERROR,      ///< the input is not valid UTF-8
    UNINITIALIZED,   ///< unknown error, or uninitialized document
    EMPTY,           ///< no structural element found
    UNESCAPED_CHARS, ///< found unescaped characters in a string.
    UNCLOSED_STRING, ///< missing quote at the end
    UNSUPPORTED_ARCHITECTURE, ///< unsupported architecture
    INCORRECT_TYPE,           ///< JSON element has a different type than user expected
    NUMBER_OUT_OF_RANGE,      ///< JSON number does not fit in 64 bits
    INDEX_OUT_OF_BOUNDS,      ///< JSON array index too large
    NO_SUCH_FIELD,            ///< JSON field not found in object
    IO_ERROR,                 ///< Error reading a file
    INVALID_JSON_POINTER,     ///< Invalid JSON pointer reference
    INVALID_URI_FRAGMENT,     ///< Invalid URI fragment
    UNEXPECTED_ERROR,         ///< indicative of a bug in simdjson
    PARSER_IN_USE,            ///< parser is already in use.
    OUT_OF_ORDER_ITERATION, ///< tried to iterate an array or object out of order (checked
                            ///< when SIMDJSON_DEVELOPMENT_CHECKS=1)
    INCOMPLETE_ARRAY_OR_OBJECT, ///< The document ends early.
    SCALAR_DOCUMENT_AS_VALUE,   ///< A scalar document is treated as a value.
    OUT_OF_BOUNDS,              ///< Attempted to access location outside of document.
    TRAILING_CONTENT,           ///< Unexpected trailing content in the JSON input
    NUM_ERROR_CODES
};

template <typename T>
using result = expected<T, inlined_unexpected<error_code, error_code::SUCCESS>>;

namespace parser_detail {

WJR_PURE WJR_INTRINSIC_INLINE result<void> parse_null(const char *first,
                                                      const char *last) noexcept {
    if (WJR_LIKELY(last - first == 4 && std::memcmp(first, "null", 4)) == 0) {
        return {};
    }

    return unexpected(error_code::N_ATOM_ERROR);
}

WJR_PURE WJR_INTRINSIC_INLINE result<void> parse_true(const char *first,
                                                      const char *last) noexcept {
    if (WJR_LIKELY(last - first == 4 && std::memcmp(first, "true", 4)) == 0) {
        return {};
    }

    return unexpected(error_code::T_ATOM_ERROR);
}

WJR_PURE WJR_INTRINSIC_INLINE result<void> parse_false(const char *first,
                                                       const char *last) noexcept {
    if (WJR_LIKELY(last - first == 5 && std::memcmp(first + 1, "alse", 4)) == 0) {
        return {};
    }

    return unexpected(error_code::F_ATOM_ERROR);
}

WJR_PURE WJR_INTRINSIC_INLINE result<void> check_number(const char *first,
                                                        const char *last) noexcept {
    int64_t ivalue;
    auto ret = from_chars(first, last, ivalue);

    if (WJR_UNLIKELY(!ret)) {
        return unexpected(error_code::NUMBER_ERROR);
    }

    first = ret.ptr;

    if (first == last) {
        return {};
    }

    if (*first == '.') {
        if (++first == last) {
            return unexpected(error_code::NUMBER_ERROR);
        }

        uint64_t uvalue;
        ret = from_chars(first, last, uvalue);
        if (WJR_UNLIKELY(!ret)) {
            return unexpected(error_code::NUMBER_ERROR);
        }

        first = ret.ptr;

        if (first == last) {
            return {};
        }
    }

    switch (*first) {
    case 'e':
    case 'E': {
        break;
    }
    default: {
        return unexpected(error_code::NUMBER_ERROR);
    }
    }

    if (++first == last) {
        return unexpected(error_code::NUMBER_ERROR);
    }

    if (*first == '+' || *first == '-') {
        if (++first == last) {
            return unexpected(error_code::NUMBER_ERROR);
        }
    }

    uint64_t uvalue;
    ret = from_chars(first, last, uvalue);
    if (WJR_UNLIKELY(!ret)) {
        return unexpected(error_code::NUMBER_ERROR);
    }

    first = ret.ptr;

    if (first == last) {
        return {};
    }

    return unexpected(error_code::NUMBER_ERROR);
}

/**
 * @todo Move to other header.
 *
 */
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

WJR_PURE WJR_INTRINSIC_INLINE result<const char *>
check_unicode_codepoint(WJR_MAYBE_UNUSED const char *first,
                        WJR_MAYBE_UNUSED const char *last) noexcept {
    return first;
}

WJR_PURE WJR_INTRINSIC_INLINE result<void>
generic_check_string(const char *first, const char *last) noexcept {
    if (WJR_UNLIKELY(first == last)) {
        return {};
    }

    do {
        uint8_t ch = *first++;

        if (WJR_UNLIKELY(ch == '\\')) {
            WJR_ASSERT(first != last);
            ch = *first++;

            if (ch == 'u') {
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
extern WJR_PURE result<void> check_string(const char *first, const char *last) noexcept;
#else
WJR_PURE inline result<void> check_string(const char *first, const char *last) noexcept {
    return generic_check_string(first, last);
}
#endif

/// @todo complete this function
WJR_INTRINSIC_INLINE result<const char *>
parse_unicode_codepoint(WJR_MAYBE_UNUSED char *&dst, WJR_MAYBE_UNUSED const char *first,
                        WJR_MAYBE_UNUSED const char *last) noexcept {
    return first;
}

WJR_INTRINSIC_INLINE result<char *> generic_parse_string(char *dst, const char *first,
                                                         const char *last) noexcept {
    if (WJR_UNLIKELY(first == last)) {
        return dst;
    }

    do {
        uint8_t ch = *first++;

        if (WJR_UNLIKELY(ch == '\\')) {
            WJR_ASSERT(first != last);
            ch = *first++;

            if (ch == 'u') {
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

} // namespace parser_detail

class parser_visitor {
public:
    template <typename Parser>
    WJR_NOINLINE static result<void> parse(Parser &par, const reader &rd) noexcept {
        constexpr unsigned int depth_size = 32;
        constexpr unsigned int max_depth = depth_size * 64;

        uint64_t __stk[depth_size];
        unsigned int depth = 0;
        uint8_t type;

        const auto set_bitset = [stk = __stk](unsigned int depth, bool f) {
            auto &value = stk[depth / 64];
            const auto bit = static_cast<uint64_t>(1) << (depth % 64);
            if (f) {
                value |= bit;
            } else {
                value &= ~bit;
            }
        };

        const auto get_bitset = [stk = __stk](unsigned int depth) {
            return (stk[depth / 64] & (static_cast<uint64_t>(1) << (depth % 64))) != 0;
        };

        // token reader
        auto read = [&rd]() {
            auto __begin = rd.token_begin();
            auto __end = rd.token_end();
            return [__begin, __end](uint32_t &token,
                                    error_code err =
                                        error_code::TAPE_ERROR) mutable -> result<void> {
                if (WJR_LIKELY(__begin != __end)) {
                    token = *__begin++;
                    return {};
                }

                return unexpected(err);
            };
        }();

        uint32_t token;
        uint32_t next_token;

        WJR_EXPECTED_TRY(read(token, error_code::EMPTY));

        const char *const ptr = rd.data();

        if (const uint8_t ch = ptr[token]; WJR_LIKELY(ch == '{')) {
            WJR_EXPECTED_TRY(par.visit_start_object(token));
            goto OBJECT;
        } else if (WJR_LIKELY(ch == ']')) {
            WJR_EXPECTED_TRY(par.visit_start_array(token));
            goto ARRAY;
        } else {
            const auto size = rd.size();

            switch (ch) {
            case 'n': {
                if (WJR_UNLIKELY(!read(next_token))) {
                    next_token = size;
                }

                WJR_EXPECTED_TRY(par.visit_null(ptr + token, ptr + next_token));
                break;
            }
            case 't': {
                if (WJR_UNLIKELY(!read(next_token))) {
                    next_token = size;
                }

                WJR_EXPECTED_TRY(par.visit_true(ptr + token, ptr + next_token));
                break;
            }
            case 'f': {
                if (WJR_UNLIKELY(!read(next_token))) {
                    next_token = size;
                }

                WJR_EXPECTED_TRY(par.visit_false(ptr + token, ptr + next_token));
                break;
            }
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case '-': {
                if (WJR_UNLIKELY(!read(next_token))) {
                    next_token = size;
                }

                WJR_EXPECTED_TRY(par.visit_number(ptr + token, ptr + next_token));
                break;
            }
            case '"': {
                if (WJR_UNLIKELY(!read(next_token))) {
                    next_token = size;
                }

                if (WJR_UNLIKELY(next_token - token < 2)) {
                    return unexpected(error_code::UNCLOSED_STRING);
                }

                if (WJR_UNLIKELY(ptr[next_token - 1] != '"')) {
                    return unexpected(error_code::UNCLOSED_STRING);
                }

                WJR_EXPECTED_TRY(par.visit_string(ptr + token + 1, ptr + next_token - 1));
                break;
            }
            default: {
                return unexpected(error_code::TAPE_ERROR);
            }
            }

            if (WJR_UNLIKELY(read(next_token))) {
                return unexpected(error_code::TAPE_ERROR);
            }

            return {};
        }

    OBJECT : {
        WJR_EXPECTED_TRY(read(token));

        if (const uint8_t ch = ptr[token]; ch != '"') {
            if (WJR_UNLIKELY(ch != '}')) {
                return unexpected(error_code::TAPE_ERROR);
            }

            goto OBJECT_BACK;
        }

        WJR_EXPECTED_TRY(read(next_token, error_code::UNCLOSED_STRING));

        WJR_EXPECTED_TRY(par.visit_object_string(ptr + token + 1, ptr + next_token - 1));

        if (uint8_t ch = ptr[next_token]; WJR_UNLIKELY(ch != ':')) {
            if (WJR_UNLIKELY(!charconv_detail::isspace(ch))) {
                return unexpected(error_code::TAPE_ERROR);
            }

            WJR_EXPECTED_TRY(read(next_token));
            ch = ptr[next_token];

            if (WJR_UNLIKELY(ch != ':')) {
                return unexpected(error_code::TAPE_ERROR);
            }
        }

        goto OBJECT_ELEMENT;
    }

    OBJECT_ELEMENTS_SPACE:
        if (charconv_detail::isspace(ptr[next_token])) {
            WJR_EXPECTED_TRY(read(next_token));
        }

    OBJECT_ELEMENTS : {
        do {
            uint8_t ch;
            ch = ptr[next_token];

            if (ch == ',') {
                break;
            }

            if (WJR_UNLIKELY(ch != '}')) {
                return unexpected(error_code::TAPE_ERROR);
            }

        OBJECT_BACK:
            if (depth == 0) {
                WJR_EXPECTED_TRY(par.visit_end_object_to_root(next_token));

                return {};
            }

            token = next_token;
            WJR_EXPECTED_TRY(read(next_token));

            type = get_bitset(--depth);

            if (type == 0) {
                WJR_EXPECTED_TRY(par.visit_end_object_to_object(token));

                goto OBJECT_ELEMENTS;
            } else {
                WJR_EXPECTED_TRY(par.visit_end_object_to_array(token));

                goto ARRAY_ELEMENTS;
            }
        } while (0);

        WJR_EXPECTED_TRY(read(token));

        if (WJR_UNLIKELY(ptr[token] != '"')) {
            return unexpected(error_code::TAPE_ERROR);
        }

        WJR_EXPECTED_TRY(read(next_token, error_code::UNCLOSED_STRING));

        WJR_EXPECTED_TRY(par.visit_object_string(ptr + token + 1, ptr + next_token - 1));

        if (uint8_t ch = ptr[next_token]; WJR_UNLIKELY(ch != ':')) {
            if (WJR_UNLIKELY(charconv_detail::isspace(ch))) {
                return unexpected(error_code::TAPE_ERROR);
            }

            WJR_EXPECTED_TRY(read(next_token));
            ch = ptr[next_token];

            if (WJR_UNLIKELY(ch != ':')) {
                return unexpected(error_code::TAPE_ERROR);
            }
        }

    OBJECT_ELEMENT:
        WJR_EXPECTED_TRY(read(token));

        switch (ptr[token]) {
        case 'n': {
            WJR_EXPECTED_TRY(read(next_token));

            WJR_EXPECTED_TRY(par.visit_null(ptr + token, ptr + next_token));
            break;
        }
        case 't': {
            WJR_EXPECTED_TRY(read(next_token));

            WJR_EXPECTED_TRY(par.visit_true(ptr + token, ptr + next_token));
            break;
        }
        case 'f': {
            WJR_EXPECTED_TRY(read(next_token));

            WJR_EXPECTED_TRY(par.visit_false(ptr + token, ptr + next_token));
            break;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-': {
            WJR_EXPECTED_TRY(read(next_token));

            WJR_EXPECTED_TRY(par.visit_number(ptr + token, ptr + next_token));
            break;
        }
        case '"': {
            WJR_EXPECTED_TRY(read(next_token, error_code::UNCLOSED_STRING));

            WJR_EXPECTED_TRY(par.visit_string(ptr + token + 1, ptr + next_token - 1));
            break;
        }
        case '{': {
            if (WJR_UNLIKELY(depth == max_depth)) {
                return unexpected(error_code::DEPTH_ERROR);
            }

            WJR_EXPECTED_TRY(par.visit_start_object(token));

            set_bitset(depth++, false);
            goto OBJECT;
        }
        case '[': {
            if (WJR_UNLIKELY(depth == max_depth)) {
                return unexpected(error_code::DEPTH_ERROR);
            }

            WJR_EXPECTED_TRY(par.visit_start_array(token));

            set_bitset(depth++, false);
            goto ARRAY;
        }
        default: {
            return unexpected(error_code::TAPE_ERROR);
        }
        }

        goto OBJECT_ELEMENTS_SPACE;
    }

    ARRAY : {
        WJR_EXPECTED_TRY(read(token));

        if (ptr[token] == ']') {
            goto ARRAY_BACK;
        }

        goto ARRAY_ELEMENT;
    }

    ARRAY_ELEMENTS_SPACE:
        if (charconv_detail::isspace(ptr[next_token])) {
            WJR_EXPECTED_TRY(read(next_token));
        }

    ARRAY_ELEMENTS : {
        do {
            uint8_t ch;
            ch = ptr[next_token];

            if (ch == ',') {
                break;
            }

            if (WJR_UNLIKELY(ch != ']')) {
                return unexpected(error_code::TAPE_ERROR);
            }

        ARRAY_BACK:
            if (depth == 0) {
                WJR_EXPECTED_TRY(par.visit_end_array_to_root(next_token));

                return {};
            }

            token = next_token;
            WJR_EXPECTED_TRY(read(next_token));

            type = get_bitset(--depth);

            if (type == 0) {
                WJR_EXPECTED_TRY(par.visit_end_array_to_object(token));

                goto OBJECT_ELEMENTS;
            } else {
                WJR_EXPECTED_TRY(par.visit_end_array_to_array(token));

                goto ARRAY_ELEMENTS;
            }
        } while (0);

        WJR_EXPECTED_TRY(read(token));
    ARRAY_ELEMENT:

        switch (ptr[token]) {
        case 'n': {
            WJR_EXPECTED_TRY(read(next_token));

            WJR_EXPECTED_TRY(par.visit_null(ptr + token, ptr + next_token));
            break;
        }
        case 't': {
            WJR_EXPECTED_TRY(read(next_token));

            WJR_EXPECTED_TRY(par.visit_true(ptr + token, ptr + next_token));
            break;
        }
        case 'f': {
            WJR_EXPECTED_TRY(read(next_token));

            WJR_EXPECTED_TRY(par.visit_false(ptr + token, ptr + next_token));
            break;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-': {
            WJR_EXPECTED_TRY(read(next_token));

            WJR_EXPECTED_TRY(par.visit_number(ptr + token, ptr + next_token));
            break;
        }
        case '"': {
            WJR_EXPECTED_TRY(read(next_token, error_code::UNCLOSED_STRING));

            WJR_EXPECTED_TRY(par.visit_string(ptr + token + 1, ptr + next_token - 1));
            break;
        }
        case '{': {
            if (WJR_UNLIKELY(depth == max_depth)) {
                return unexpected(error_code::DEPTH_ERROR);
            }

            WJR_EXPECTED_TRY(par.visit_start_object(token));

            set_bitset(depth++, true);
            goto OBJECT;
        }
        case '[': {
            if (WJR_UNLIKELY(depth == max_depth)) {
                return unexpected(error_code::DEPTH_ERROR);
            }

            WJR_EXPECTED_TRY(par.visit_start_array(token));

            set_bitset(depth++, true);
            goto ARRAY;
        }
        default: {
            return unexpected(error_code::TAPE_ERROR);
        }
        }

        goto ARRAY_ELEMENTS_SPACE;
    }
    }
};

class check_parser {
    friend class parser_visitor;

public:
    result<void> parse(const reader &rd) noexcept {
        return parser_visitor::parse(*this, rd);
    }

protected:
    WJR_INTRINSIC_INLINE static result<void> visit_null(const char *first,
                                                        const char *last) noexcept {
        return parser_detail::parse_null(first, last);
    }

    WJR_INTRINSIC_INLINE static result<void> visit_true(const char *first,
                                                        const char *last) noexcept {
        return parser_detail::parse_true(first, last);
    }

    WJR_INTRINSIC_INLINE static result<void> visit_false(const char *first,
                                                         const char *last) noexcept {
        return parser_detail::parse_false(first, last);
    }

    WJR_INTRINSIC_INLINE static result<void> visit_number(const char *first,
                                                          const char *last) noexcept {
        return parser_detail::check_number(first, last);
    }

    WJR_INTRINSIC_INLINE static result<void> visit_string(const char *first,
                                                          const char *last) noexcept {
        return parser_detail::check_string(first, last);
    }

    WJR_INTRINSIC_INLINE static result<void>
    visit_object_string(const char *first, const char *last) noexcept {
        return visit_string(first, last);
    }

    WJR_INTRINSIC_INLINE static result<void>
    visit_start_object(WJR_MAYBE_UNUSED uint32_t pos) noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE static result<void>
    visit_end_object_to_object(WJR_MAYBE_UNUSED uint32_t pos) noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE static result<void>
    visit_end_object_to_array(WJR_MAYBE_UNUSED uint32_t pos) noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE static result<void>
    visit_end_object_to_root(WJR_MAYBE_UNUSED uint32_t pos) noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE static result<void>
    visit_start_array(WJR_MAYBE_UNUSED uint32_t pos) noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE static result<void>
    visit_end_array_to_object(WJR_MAYBE_UNUSED uint32_t pos) noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE static result<void>
    visit_end_array_to_array(WJR_MAYBE_UNUSED uint32_t pos) noexcept {
        return {};
    }

    WJR_INTRINSIC_INLINE static result<void>
    visit_end_array_to_root(WJR_MAYBE_UNUSED uint32_t pos) noexcept {
        return {};
    }
};

extern template result<void> parser_visitor::parse<check_parser>(check_parser &,
                                                                 const reader &) noexcept;

} // namespace wjr::json

#endif // WJR_JSON_PARSER_HPP__