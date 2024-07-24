#ifndef WJR_JSON_PARSER_HPP__
#define WJR_JSON_PARSER_HPP__

#include <array>

#include <wjr/expected.hpp>
#include <wjr/json/reader.hpp>

#if defined(WJR_X86)
#include <wjr/x86/json/parser.hpp>
#endif

namespace wjr::json {

enum error_code {
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
    UNSUPPORTED_ARCHITECTURE,   ///< unsupported architecture
    INCORRECT_TYPE,             ///< JSON element has a different type than user expected
    NUMBER_OUT_OF_RANGE,        ///< JSON number does not fit in 64 bits
    INDEX_OUT_OF_BOUNDS,        ///< JSON array index too large
    NO_SUCH_FIELD,              ///< JSON field not found in object
    IO_ERROR,                   ///< Error reading a file
    INVALID_JSON_POINTER,       ///< Invalid JSON pointer reference
    INVALID_URI_FRAGMENT,       ///< Invalid URI fragment
    UNEXPECTED_ERROR,           ///< indicative of a bug in simdjson
    PARSER_IN_USE,              ///< parser is already in use.
    OUT_OF_ORDER_ITERATION,     ///< tried to iterate an array or object out of order
                                ///< (checked when SIMDJSON_DEVELOPMENT_CHECKS=1)
    INSUFFICIENT_PADDING,       ///< The JSON doesn't have enough padding for simdjson to
                                ///< safely parse it.
    INCOMPLETE_ARRAY_OR_OBJECT, ///< The document ends early.
    SCALAR_DOCUMENT_AS_VALUE,   ///< A scalar document is treated as a value.
    OUT_OF_BOUNDS,              ///< Attempted to access location outside of document.
    TRAILING_CONTENT,           ///< Unexpected trailing content in the JSON input
    NUM_ERROR_CODES,
};

template <typename T>
using result = expected<T, inlined_unexpected<error_code, error_code::SUCCESS>>;

namespace parser_detail {

WJR_INTRINSIC_INLINE result<void> parse_null(const char *first,
                                             const char *last) noexcept {
    if (WJR_LIKELY(last - first == 4 && std::memcmp(first, "null", 4)) == 0) {
        return {};
    }

    return unexpected(error_code::N_ATOM_ERROR);
}

WJR_INTRINSIC_INLINE result<void> parse_true(const char *first,
                                             const char *last) noexcept {
    if (WJR_LIKELY(last - first == 4 && std::memcmp(first, "true", 4)) == 0) {
        return {};
    }

    return unexpected(error_code::T_ATOM_ERROR);
}

WJR_INTRINSIC_INLINE result<void> parse_false(const char *first,
                                              const char *last) noexcept {
    if (WJR_LIKELY(last - first == 5 && std::memcmp(first + 1, "alse", 4)) == 0) {
        return {};
    }

    return unexpected(error_code::F_ATOM_ERROR);
}

WJR_INTRINSIC_INLINE result<void> parse_number(const char *first,
                                               const char *last) noexcept {
    constexpr auto __matches = [](uint8_t ch) { return '0' <= ch && ch <= '9'; };

    WJR_ASSERT_ASSUME_L2(first < last);

    if (*first == '-') {
        if (++first == last) {
            goto FAILED;
        }
    }

    if (*first++ == '0') {
        if (first == last) {
            return {};
        }
    } else {
        if (first == last) {
            return {};
        }

        do {
            if (WJR_UNLIKELY(!__matches(*first))) {
                goto NEXT0;
            }
        } while (++first != last);
        return {};
    NEXT0 : {}
    }

    if (*first == '.') {
        if (++first == last) {
            goto FAILED;
        }

        if (WJR_UNLIKELY(!__matches(*first))) {
            goto FAILED;
        }

        while (++first != last) {
            if (WJR_UNLIKELY(!__matches(*first))) {
                goto NEXT1;
            }
        }
        return {};
    NEXT1 : {}
    }

    switch (*first) {
    case 'e':
    case 'E': {
        break;
    }
    default: {
        goto FAILED;
    }
    }

    if (++first == last) {
        goto FAILED;
    }

    if (*first == '+' || *first == '-') {
        if (++first == last) {
            goto FAILED;
        }
    }

    if (WJR_UNLIKELY(!__matches(*first))) {
        goto FAILED;
    }

    while (++first != last) {
        if (WJR_UNLIKELY(!__matches(*first))) {
            goto FAILED;
        }
    }

    return {};

FAILED:
    return unexpected(error_code::NUMBER_ERROR);
}

/**
 * @todo Move to other header.
 *
 */
static constexpr std::array<uint8_t, 256> escape_table = {
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

/// @todo complete this function
WJR_INTRINSIC_INLINE result<char *>
parse_unicode_codepoint(char *dst, WJR_MAYBE_UNUSED const char *ptr) noexcept {
    return dst;
}

WJR_INTRINSIC_INLINE result<char *> generic_parse_string(char *dst, const char *first,
                                                         const char *last) noexcept {
    if (WJR_UNLIKELY(first == last)) {
        return dst;
    }

    do {
        uint8_t ch = *first++;

        if (WJR_LIKELY(ch != '\\')) {
            *dst++ = ch;
        } else {
            WJR_ASSERT(first != last);
            ch = *first++;

            if (ch == 'u') {
                if (WJR_UNLIKELY(first + 4 > last)) {
                    return unexpected(error_code::STRING_ERROR);
                }

                WJR_EXPECTED_SET(dst, parse_unicode_codepoint(dst, first));
                first += 4;
            } else {
                const uint8_t code = escape_table[ch];

                if (WJR_UNLIKELY(code == 0)) {
                    return unexpected(error_code::STRING_ERROR);
                }

                *dst++ = code;
            }
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

class default_parser_base {
public:
protected:
private:
};

/*
template <typename TokenReader, typename Parser>
WJR_NOINLINE void reader_parse(TokenReader &reader, Parser &parser) {
    unique_stack_allocator stkal(math_detail::stack_alloc);

    struct stack {
        uint8_t type;
        const char *first;
        const char *last;
    };

    stack *stk = static_cast<stack *>(stkal.allocate(512 * sizeof(stack)));
    stack *current = stk;
    uint8_t type;

    uint32_t token, next_token;
    const char *const ptr = reader.begin();
    const char *const last = reader.end();
    const auto size = last - ptr;

    // empty json
    if (WJR_UNLIKELY(!reader(token))) {
        return parser(token_failure<FAILURE>);
    }

REPEAT_TOKEN : {
    switch (ptr[token]) {
    case 'n': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            next_token = size;
        }

        if (WJR_UNLIKELY(parser(token_null, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 't': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            next_token = size;
        }

        if (WJR_UNLIKELY(parser(token_true, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 'f': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            next_token = size;
        }

        if (WJR_UNLIKELY(parser(token_false, ptr + token, ptr + next_token))) {
            return;
        }

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
        if (WJR_UNLIKELY(!reader(next_token))) {
            next_token = size;
        }

        type = 0;
        goto NUMBER;
    }
    case '"': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            next_token = size;
        }

        type = 0;
        goto STRING;
    }
    case '[': {
        if (WJR_UNLIKELY(parser(token_left_bracket))) {
            return;
        }

        goto ARRAY;
    }
    case '{': {
        if (WJR_UNLIKELY(parser(token_left_brace))) {
            return;
        }

        goto OBJECT;
    }
    default: {
        return parser(token_failure<FAILURE>);
    }
    }

REPEAT_TOKEN_NEXT:

    if (next_token == size) {
        return parser(token_success);
    }

    // non-repeated callback
    if (parser(token_repeat)) {
        return parser(token_failure<FAILURE>);
    }

    // non-structural token, must be followed by space
    switch (ptr[next_token]) {
    case ' ':
    case '\n':
    case '\t':
    case '\r': {
        if (!reader(token)) {
            return parser(token_success);
        }
        break;
    }
    default: {
        return parser(token_failure<FAILURE>);
    }
    }

    goto REPEAT_TOKEN;
}

ARRAY : {
    if (WJR_UNLIKELY(!reader(token))) {
        return parser(token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case 'n': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_null, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 't': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_true, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 'f': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_false, ptr + token, ptr + next_token))) {
            return;
        }

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
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        type = 1;
        goto NUMBER;
    }
    case '"': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        type = 1;
        goto STRING;
    }
    case '[': {
        if (WJR_UNLIKELY(parser(token_left_bracket))) {
            return;
        }

        (current++)->type = 0;
        goto ARRAY;
    }
    case '{': {
        if (WJR_UNLIKELY(parser(token_left_brace))) {
            return;
        }

        (current++)->type = 0;
        goto OBJECT;
    }
    case ']': {
        goto ARRAY_BACK;
    }
    case '}': {
        return parser(token_failure<FAILURE>);
    }
    default: {
        return parser(token_failure<FAILURE>);
    }
    }
}

ARRAY_ELEMENT_SPACE:
    switch (ptr[next_token]) {
    case ' ':
    case '\n':
    case '\t':
    case '\r': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        break;
    }
    }

ARRAY_ELEMENT : {
    switch (ptr[next_token]) {
    case ',': {
        break;
    }
    case ']': {
    ARRAY_BACK:
        if (current == stk) {
            if (parser(token_right_bracket_back_root) || parser(token_repeat)) {
                return;
            }

            if (reader(token)) {
                goto REPEAT_TOKEN;
            }

            return parser(token_success);
        }

        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        type = (--current)->type;

        if (type == 0) {
            if (WJR_UNLIKELY(parser(token_right_bracket_back_bracket))) {
                return;
            }

            goto ARRAY_ELEMENT;
        } else {
            if (WJR_UNLIKELY(parser(token_right_bracket_back_brace, current->first,
                                    current->last))) {
                return;
            }

            goto OBJECT_ELEMENT;
        }
    }
    default: {
        return parser(token_failure<FAILURE>);
    }
    }

    if (WJR_UNLIKELY(!reader(token))) {
        return parser(token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case 'n': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_null, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 't': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_true, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 'f': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_false, ptr + token, ptr + next_token))) {
            return;
        }

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
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        type = 1;
        goto NUMBER;
    }
    case '"': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        type = 1;
        goto STRING;
    }
    case '[': {
        if (WJR_UNLIKELY(parser(token_left_bracket))) {
            return;
        }

        (current++)->type = 0;
        goto ARRAY;
    }
    case '{': {
        if (WJR_UNLIKELY(parser(token_left_brace))) {
            return;
        }

        (current++)->type = 0;
        goto OBJECT;
    }
    case ']': {
        return parser(token_failure<FAILURE>);
    }
    case '}': {
        return parser(token_failure<FAILURE>);
    }
    default: {
        return parser(token_failure<FAILURE>);
    }
    }

    goto ARRAY_ELEMENT_SPACE;
}

OBJECT : {
    if (WJR_UNLIKELY(!reader(token))) {
        return parser(token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case '"': {
        break;
    }
    case '}': {
        goto OBJECT_BACK;
    }
    default: {
        return parser(token_failure<FAILURE>);
    }
    }

    if (WJR_UNLIKELY(!reader(next_token))) {
        return parser(token_failure<FAILURE>);
    }

    if (WJR_UNLIKELY(
            next_token - token < 2 ||
            parser(token_left_brace_string, ptr + token + 1, ptr + next_token - 1))) {
        return;
    }

    switch (ptr[next_token]) {
    case ' ':
    case '\n':
    case '\r':
    case '\t': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        break;
    }
    }

    if (WJR_UNLIKELY(ptr[next_token] != ':')) {
        return parser(token_failure<FAILURE>);
    }

    if (WJR_UNLIKELY(!reader(token))) {
        return parser(token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case 'n': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_null, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 't': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_true, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 'f': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_false, ptr + token, ptr + next_token))) {
            return;
        }

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
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        type = 2;
        goto NUMBER;
    }
    case '"': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        type = 2;
        goto STRING;
    }
    case '[': {
        if (WJR_UNLIKELY(parser(token_left_bracket))) {
            return;
        }

        (current++)->type = 1;
        goto ARRAY;
    }
    case '{': {
        if (WJR_UNLIKELY(parser(token_left_brace))) {
            return;
        }

        (current++)->type = 1;
        goto OBJECT;
    }
    case ']': {
        return parser(token_failure<FAILURE>);
    }
    case '}': {
        return parser(token_failure<FAILURE>);
    }
    default: {
        return parser(token_failure<FAILURE>);
    }
    }
}

OBJECT_ELEMENT_SPACE:
    switch (ptr[next_token]) {
    case ' ':
    case '\n':
    case '\t':
    case '\r': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        break;
    }
    }

OBJECT_ELEMENT : {
    switch (ptr[next_token]) {
    case ',': {
        break;
    }
    case '}': {
    OBJECT_BACK:
        if (WJR_UNLIKELY(current == stk)) {
            if (parser(token_right_brace_back_root) || parser(token_repeat)) {
                return;
            }

            if (reader(token)) {
                goto REPEAT_TOKEN;
            }

            return parser(token_success);
        }

        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        type = (--current)->type;

        if (type == 0) {
            if (WJR_UNLIKELY(parser(token_right_brace_back_bracket))) {
                return;
            }

            goto ARRAY_ELEMENT;
        } else {
            if (WJR_UNLIKELY(parser(token_right_brace_back_brace, current->first,
                                    current->last))) {
                return;
            }

            goto OBJECT_ELEMENT;
        }
    }
    default: {
        return parser(token_failure<FAILURE>);
    }
    }

    if (WJR_UNLIKELY(!reader(token))) {
        return parser(token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case '"': {
        break;
    }
    default: {
        return parser(token_failure<FAILURE>);
    }
    }

    if (WJR_UNLIKELY(!reader(next_token))) {
        return parser(token_failure<FAILURE>);
    }

    if (WJR_UNLIKELY(
            next_token - token < 2 ||
            parser(token_left_brace_string, ptr + token + 1, ptr + next_token - 1))) {
        return;
    }

    switch (ptr[next_token]) {
    case ' ':
    case '\n':
    case '\r':
    case '\t': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        break;
    }
    }

    if (WJR_UNLIKELY(ptr[next_token] != ':')) {
        return parser(token_failure<FAILURE>);
    }

    if (WJR_UNLIKELY(!reader(token))) {
        return parser(token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case 'n': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_null, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 't': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_true, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 'f': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(token_false, ptr + token, ptr + next_token))) {
            return;
        }

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
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        type = 2;
        goto NUMBER;
    }
    case '"': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(token_failure<FAILURE>);
        }

        type = 2;
        goto STRING;
    }
    case '[': {
        if (WJR_UNLIKELY(parser(token_left_bracket))) {
            return;
        }

        (current++)->type = 1;
        goto ARRAY;
    }
    case '{': {
        if (WJR_UNLIKELY(parser(token_left_brace))) {
            return;
        }

        (current++)->type = 1;
        goto OBJECT;
    }
    case ']': {
        return parser(token_failure<FAILURE>);
    }
    case '}': {
        return parser(token_failure<FAILURE>);
    }
    default: {
        return parser(token_failure<FAILURE>);
    }
    }

    goto OBJECT_ELEMENT_SPACE;
}

NUMBER:
    if (WJR_UNLIKELY(parser(token_number, ptr + token, ptr + next_token))) {
        return;
    }

    if (type == 1) {
        goto ARRAY_ELEMENT_SPACE;
    } else if (type == 2) {
        goto OBJECT_ELEMENT_SPACE;
    } else {
        goto REPEAT_TOKEN_NEXT;
    }

STRING:
    if (WJR_UNLIKELY(next_token - token < 2 ||
                     parser(token_string, ptr + token + 1, ptr + next_token - 1))) {
        return;
    }

    if (type == 1) {
        goto ARRAY_ELEMENT_SPACE;
    } else if (type == 2) {
        goto OBJECT_ELEMENT_SPACE;
    } else {
        goto REPEAT_TOKEN_NEXT;
    }
}

template <typename Parser>
WJR_INTRINSIC_INLINE void parse(span<const char> sp, Parser &parser) {
    constexpr uint32_t token_buf_size = 1024;

    unique_stack_allocator stkal(math_detail::stack_alloc);

    uint32_t *token_buf = static_cast<uint32_t *>(
        stkal.allocate((token_buf_size * 2 - 1) * sizeof(uint32_t)));

    token_reader<token_buf_size> reader(sp, token_buf);
    reader_parse(reader, parser);
}

*/

} // namespace wjr::json

#endif // WJR_JSON_PARSER_HPP__