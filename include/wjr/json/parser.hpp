#ifndef WJR_JSON_PARSER_HPP__
#define WJR_JSON_PARSER_HPP__

#include <wjr/json/token_reader.hpp>

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
    NUM_ERROR_CODES
};

class default_parser_base {
public:
private:
};

/**
 * @param parser Return type of
 * success_t/failure_t/left_bracket_t/right_bracket_t/left_brace_t/right_brace_t must be
 * void. Otherwise, return type must be bool.
 *
 */
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

} // namespace wjr::json

#endif // WJR_JSON_PARSER_HPP__