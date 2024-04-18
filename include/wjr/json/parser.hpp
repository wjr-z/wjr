#ifndef WJR_JSON_PARSER_HPP__
#define WJR_JSON_PARSER_HPP__

#include <bitset>
#include <charconv>

#include <wjr/json/lexer.hpp>
#include <wjr/math.hpp>

namespace wjr::json {

enum error_code : unsigned int {
    SUCCESS,
    FAILURE,
};

struct in_place_token_null_t {};
struct in_place_token_true_t {};
struct in_place_token_false_t {};
struct in_place_token_number_t {};
struct in_place_token_string_t {};
struct in_place_token_left_bracket_t {};
struct in_place_token_left_brace_t {};
struct in_place_token_left_brace_string_t {};
struct in_place_token_right_bracket_back_bracket_t {};
struct in_place_token_right_bracket_back_brace_t {};
struct in_place_token_right_bracket_back_root_t {};
struct in_place_token_right_brace_back_bracket_t {};
struct in_place_token_right_brace_back_brace_t {};
struct in_place_token_right_brace_back_root_t {};

struct in_place_token_success_t {};

template <error_code ec>
struct in_place_token_failure_t {};

struct in_place_token_repeat_t {};

inline constexpr in_place_token_null_t in_place_token_null{};
inline constexpr in_place_token_true_t in_place_token_true{};
inline constexpr in_place_token_false_t in_place_token_false{};
inline constexpr in_place_token_number_t in_place_token_number{};
inline constexpr in_place_token_string_t in_place_token_string{};
inline constexpr in_place_token_left_bracket_t in_place_token_left_bracket{};
inline constexpr in_place_token_left_brace_t in_place_token_left_brace{};
inline constexpr in_place_token_left_brace_string_t in_place_token_left_brace_string{};
inline constexpr in_place_token_right_bracket_back_bracket_t
    in_place_token_right_bracket_back_bracket{};
inline constexpr in_place_token_right_bracket_back_brace_t
    in_place_token_right_bracket_back_brace{};
inline constexpr in_place_token_right_bracket_back_root_t
    in_place_token_right_bracket_back_root{};
inline constexpr in_place_token_right_brace_back_bracket_t
    in_place_token_right_brace_back_bracket{};
inline constexpr in_place_token_right_brace_back_brace_t
    in_place_token_right_brace_back_brace{};
inline constexpr in_place_token_right_brace_back_root_t
    in_place_token_right_brace_back_root{};

inline constexpr in_place_token_success_t in_place_token_success{};

template <error_code ec>
inline constexpr in_place_token_failure_t<ec> in_place_token_failure{};

inline constexpr in_place_token_repeat_t in_place_token_repeat{};

template <uint32_t token_buf_size>
class forward_token_reader {
public:
    forward_token_reader(span<const char> sp, uint32_t *token_buf)
        : token_first(token_buf), token_last(token_buf), token_buf(token_buf),
          first(sp.data()), lexer(sp) {}

    bool operator()(uint32_t &token) {
        if (WJR_LIKELY(token_first != token_last)) {
            token = *token_first++;
            return true;
        }

        uint32_t count = lexer.read(token_buf);
        if (WJR_UNLIKELY(count == 0)) {
            return false;
        }

        token_first = token_buf;
        token_last = token_buf + count;
        token = *token_first++;
        return true;
    }

    WJR_PURE const char *begin() const noexcept { return first; }
    WJR_PURE const char *end() const noexcept { return lexer.end(); }

private:
    uint32_t *token_first;
    uint32_t *token_last;
    uint32_t *token_buf;
    const char *first;
    basic_lexer<token_buf_size> lexer;
};

/**
 * @details Return true if parsing needs to be terminated.
 *
 */
struct empty_parser {
    bool operator()(in_place_token_null_t, const char *, const char *) { return false; }

    bool operator()(in_place_token_true_t, const char *, const char *) { return false; }

    bool operator()(in_place_token_false_t, const char *, const char *) { return false; }

    bool operator()(in_place_token_number_t, const char *, const char *) { return false; }

    bool operator()(in_place_token_string_t, const char *, const char *) { return false; }

    bool operator()(in_place_token_left_bracket_t) { return false; }

    bool operator()(in_place_token_left_brace_t) { return false; }

    bool operator()(in_place_token_left_brace_string_t, const char *, const char *) {
        return false;
    }

    bool operator()(in_place_token_right_bracket_back_bracket_t) { return false; }

    bool operator()(in_place_token_right_bracket_back_brace_t, const char *,
                    const char *) {
        return false;
    }

    bool operator()(in_place_token_right_bracket_back_root_t) { return false; }

    bool operator()(in_place_token_right_brace_back_bracket_t) { return false; }

    bool operator()(in_place_token_right_brace_back_brace_t, const char *, const char *) {
        return false;
    }

    bool operator()(in_place_token_right_brace_back_root_t) { return false; }

    void operator()(in_place_token_success_t) {}

    template <error_code E>
    void operator()(in_place_token_failure_t<E>) {}

    bool operator()(in_place_token_repeat_t) { return false; }
};

struct check_parser : empty_parser {
    bool operator()(in_place_token_null_t, const char *first, const char *last) {
        if (WJR_LIKELY(last - first == 4 && std::memcmp(first, "null", 4) == 0)) {
            return false;
        }

        ec = FAILURE;
        return true;
    }

    bool operator()(in_place_token_true_t, const char *first, const char *last) {
        if (WJR_LIKELY(last - first == 4 && std::memcmp(first, "true", 4) == 0)) {
            return false;
        }

        ec = FAILURE;
        return true;
    }

    bool operator()(in_place_token_false_t, const char *first, const char *last) {
        if (WJR_LIKELY(last - first == 5 && std::memcmp(first + 1, "alse", 4) == 0)) {
            return false;
        }

        ec = FAILURE;
        return true;
    }

    WJR_INTRINSIC_INLINE bool operator()(in_place_token_number_t, const char *first,
                                         const char *last) {
        constexpr auto __matches = [](uint8_t ch) { return '0' <= ch && ch <= '9'; };

        WJR_ASSERT_ASSUME_L1(first < last);

        if (*first == '-') {
            if (++first == last) {
                goto FAILED;
            }
        }

        if (*first++ == '0') {
            if (first == last) {
                return false;
            }
        } else {
            if (first == last) {
                return false;
            }

            do {
                if (WJR_UNLIKELY(!__matches(*first))) {
                    goto NEXT0;
                }
            } while (++first != last);
            return false;
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
            return false;
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

        return false;
    FAILED : {
        ec = FAILURE;
        return true;
    }
    }

    WJR_INTRINSIC_INLINE bool operator()(in_place_token_string_t, const char *first,
                                         const char *last) {
        while (first != last) {
            uint8_t ch = *first++;
            if (WJR_UNLIKELY(ch < 32 || ch == 127)) {
                goto FAILED;
            }

            if (WJR_UNLIKELY(ch == '\\')) {
                if (WJR_UNLIKELY(first == last)) {
                    goto FAILED;
                }

                ch = *first++;
                switch (ch) {
                case '"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't': {
                    break;
                }
                case 'u': {
                    if (WJR_UNLIKELY(first + 4 > last)) {
                        goto FAILED;
                    }

                    for (int i = 0; i < 4; ++i) {
                        ch = *first++;
                        if (WJR_UNLIKELY(!('0' <= ch && ch <= '9') &&
                                         !('a' <= ch && ch <= 'f') &&
                                         !('A' <= ch && ch <= 'F'))) {
                            goto FAILED;
                        }
                    }

                    break;
                }
                default: {
                    goto FAILED;
                }
                }
            }
        }

        return false;

    FAILED : {
        ec = FAILURE;
        return true;
    }
    }

    bool operator()(in_place_token_left_bracket_t) { return false; }

    bool operator()(in_place_token_left_brace_t) { return false; }

    bool operator()(in_place_token_left_brace_string_t, const char *, const char *) {
        return false;
    }

    bool operator()(in_place_token_right_bracket_back_bracket_t) { return false; }

    bool operator()(in_place_token_right_bracket_back_brace_t, const char *,
                    const char *) {
        return false;
    }

    bool operator()(in_place_token_right_bracket_back_root_t) { return false; }

    bool operator()(in_place_token_right_brace_back_bracket_t) { return false; }

    bool operator()(in_place_token_right_brace_back_brace_t, const char *, const char *) {
        return false;
    }

    bool operator()(in_place_token_right_brace_back_root_t) { return false; }

    void operator()(in_place_token_success_t) {}

    template <error_code E>
    void operator()(in_place_token_failure_t<E>) {
        ec = E;
    }

    bool operator()(in_place_token_repeat_t) { return false; }

    error_code ec = SUCCESS;
};

/**
 * @param parser Return type of
 * success_t/failure_t/left_bracket_t/right_bracket_t/left_brace_t/right_brace_t must be
 * void. Otherwise, return type must be bool.
 *
 */
template <typename TokenReader, typename Parser>
WJR_NOINLINE void reader_parse(TokenReader &reader, Parser &parser) {
    unique_stack_allocator stkal(math_details::stack_alloc);
    struct stack {
        uint8_t type;
        const char *first;
        const char *last;
    };

    stack *stk = static_cast<stack *>(stkal.allocate(1024 * sizeof(stack)));
    stack *current = stk;
    uint8_t type;

    uint32_t token, next_token;
    const char *const ptr = reader.begin();
    const char *const last = reader.end();
    const auto size = last - ptr;

    // empty json
    if (WJR_UNLIKELY(!reader(token))) {
        return parser(in_place_token_failure<FAILURE>);
    }

REPEAT_TOKEN : {
    switch (ptr[token]) {
    case 'n': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            next_token = size;
        }

        if (WJR_UNLIKELY(parser(in_place_token_null, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 't': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            next_token = size;
        }

        if (WJR_UNLIKELY(parser(in_place_token_true, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 'f': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            next_token = size;
        }

        if (WJR_UNLIKELY(parser(in_place_token_false, ptr + token, ptr + next_token))) {
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

        if (WJR_UNLIKELY(parser(in_place_token_number, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case '"': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            next_token = size;
        }

        if (WJR_UNLIKELY(
                next_token - token < 2 ||
                parser(in_place_token_string, ptr + token + 1, ptr + next_token - 1))) {
            return;
        }

        break;
    }
    case '[': {
        if (WJR_UNLIKELY(parser(in_place_token_left_bracket))) {
            return;
        }

        goto ARRAY;
    }
    case '{': {
        if (WJR_UNLIKELY(parser(in_place_token_left_brace))) {
            return;
        }

        goto OBJECT;
    }
    default: {
        return parser(in_place_token_failure<FAILURE>);
    }
    }

    if (next_token == size) {
        return parser(in_place_token_success);
    }

    // non-repeated callback
    if (parser(in_place_token_repeat)) {
        return parser(in_place_token_failure<FAILURE>);
    }

    // non-structural token, must be followed by space
    switch (ptr[next_token]) {
    case ' ':
    case '\n':
    case '\t':
    case '\r': {
        if (!reader(token)) {
            return parser(in_place_token_success);
        }
        break;
    }
    default: {
        return parser(in_place_token_failure<FAILURE>);
    }
    }

    goto REPEAT_TOKEN;
}

ARRAY : {
    if (WJR_UNLIKELY(!reader(token))) {
        return parser(in_place_token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case 'n': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_null, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 't': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_true, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 'f': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_false, ptr + token, ptr + next_token))) {
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
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_number, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case '"': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(
                next_token - token < 2 ||
                parser(in_place_token_string, ptr + token + 1, ptr + next_token - 1))) {
            return;
        }

        break;
    }
    case '[': {
        if (WJR_UNLIKELY(parser(in_place_token_left_bracket))) {
            return;
        }

        (current++)->type = 0;
        goto ARRAY;
    }
    case '{': {
        if (WJR_UNLIKELY(parser(in_place_token_left_brace))) {
            return;
        }

        (current++)->type = 0;
        goto OBJECT;
    }
    case ']': {
        goto ARRAY_BACK;
    }
    case '}': {
        return parser(in_place_token_failure<FAILURE>);
    }
    default: {
        return parser(in_place_token_failure<FAILURE>);
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
            return parser(in_place_token_failure<FAILURE>);
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
            if (parser(in_place_token_right_bracket_back_root) ||
                parser(in_place_token_repeat)) {
                return;
            }

            if (reader(token)) {
                goto REPEAT_TOKEN;
            }

            return parser(in_place_token_success);
        }

        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        type = (--current)->type;

        if (type == 0) {
            if (WJR_UNLIKELY(parser(in_place_token_right_bracket_back_bracket))) {
                return;
            }

            goto ARRAY_ELEMENT;
        } else {
            if (WJR_UNLIKELY(parser(in_place_token_right_bracket_back_brace,
                                    current->first, current->last))) {
                return;
            }

            goto OBJECT_ELEMENT;
        }
    }
    default: {
        return parser(in_place_token_failure<FAILURE>);
    }
    }

    if (WJR_UNLIKELY(!reader(token))) {
        return parser(in_place_token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case 'n': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_null, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 't': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_true, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 'f': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_false, ptr + token, ptr + next_token))) {
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
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_number, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case '"': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(
                next_token - token < 2 ||
                parser(in_place_token_string, ptr + token + 1, ptr + next_token - 1))) {
            return;
        }

        break;
    }
    case '[': {
        if (WJR_UNLIKELY(parser(in_place_token_left_bracket))) {
            return;
        }

        (current++)->type = 0;
        goto ARRAY;
    }
    case '{': {
        if (WJR_UNLIKELY(parser(in_place_token_left_brace))) {
            return;
        }

        (current++)->type = 0;
        goto OBJECT;
    }
    case ']': {
        return parser(in_place_token_failure<FAILURE>);
    }
    case '}': {
        return parser(in_place_token_failure<FAILURE>);
    }
    default: {
        return parser(in_place_token_failure<FAILURE>);
    }
    }

    goto ARRAY_ELEMENT_SPACE;
}

OBJECT : {
    if (WJR_UNLIKELY(!reader(token))) {
        return parser(in_place_token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case '"': {
        break;
    }
    case '}': {
        goto OBJECT_BACK;
    }
    default: {
        return parser(in_place_token_failure<FAILURE>);
    }
    }

    if (WJR_UNLIKELY(!reader(next_token))) {
        return parser(in_place_token_failure<FAILURE>);
    }

    if (WJR_UNLIKELY(next_token - token < 2 ||
                     parser(in_place_token_left_brace_string, ptr + token + 1,
                            ptr + next_token - 1))) {
        return;
    }

    switch (ptr[next_token]) {
    case ' ':
    case '\n':
    case '\r':
    case '\t': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        break;
    }
    }

    if (WJR_UNLIKELY(ptr[next_token] != ':')) {
        return parser(in_place_token_failure<FAILURE>);
    }

    if (WJR_UNLIKELY(!reader(token))) {
        return parser(in_place_token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case 'n': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_null, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 't': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_true, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 'f': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_false, ptr + token, ptr + next_token))) {
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
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_number, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case '"': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(
                next_token - token < 2 ||
                parser(in_place_token_string, ptr + token + 1, ptr + next_token - 1))) {
            return;
        }

        break;
    }
    case '[': {
        if (WJR_UNLIKELY(parser(in_place_token_left_bracket))) {
            return;
        }

        (current++)->type = 1;
        goto ARRAY;
    }
    case '{': {
        if (WJR_UNLIKELY(parser(in_place_token_left_brace))) {
            return;
        }

        (current++)->type = 1;
        goto OBJECT;
    }
    case ']': {
        return parser(in_place_token_failure<FAILURE>);
    }
    case '}': {
        return parser(in_place_token_failure<FAILURE>);
    }
    default: {
        return parser(in_place_token_failure<FAILURE>);
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
            return parser(in_place_token_failure<FAILURE>);
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
            if (parser(in_place_token_right_brace_back_root) ||
                parser(in_place_token_repeat)) {
                return;
            }

            if (reader(token)) {
                goto REPEAT_TOKEN;
            }

            return parser(in_place_token_success);
        }

        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        type = (--current)->type;

        if (type == 0) {
            if (WJR_UNLIKELY(parser(in_place_token_right_brace_back_bracket))) {
                return;
            }

            goto ARRAY_ELEMENT;
        } else {
            if (WJR_UNLIKELY(parser(in_place_token_right_brace_back_brace, current->first,
                                    current->last))) {
                return;
            }

            goto OBJECT_ELEMENT;
        }
    }
    default: {
        return parser(in_place_token_failure<FAILURE>);
    }
    }

    if (WJR_UNLIKELY(!reader(token))) {
        return parser(in_place_token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case '"': {
        break;
    }
    default: {
        return parser(in_place_token_failure<FAILURE>);
    }
    }

    if (WJR_UNLIKELY(!reader(next_token))) {
        return parser(in_place_token_failure<FAILURE>);
    }

    if (WJR_UNLIKELY(next_token - token < 2 ||
                     parser(in_place_token_left_brace_string, ptr + token + 1,
                            ptr + next_token - 1))) {
        return;
    }

    switch (ptr[next_token]) {
    case ' ':
    case '\n':
    case '\r':
    case '\t': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        break;
    }
    }

    if (WJR_UNLIKELY(ptr[next_token] != ':')) {
        return parser(in_place_token_failure<FAILURE>);
    }

    if (WJR_UNLIKELY(!reader(token))) {
        return parser(in_place_token_failure<FAILURE>);
    }

    switch (ptr[token]) {
    case 'n': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_null, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 't': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_true, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case 'f': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_false, ptr + token, ptr + next_token))) {
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
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(parser(in_place_token_number, ptr + token, ptr + next_token))) {
            return;
        }

        break;
    }
    case '"': {
        if (WJR_UNLIKELY(!reader(next_token))) {
            return parser(in_place_token_failure<FAILURE>);
        }

        if (WJR_UNLIKELY(
                next_token - token < 2 ||
                parser(in_place_token_string, ptr + token + 1, ptr + next_token - 1))) {
            return;
        }

        break;
    }
    case '[': {
        if (WJR_UNLIKELY(parser(in_place_token_left_bracket))) {
            return;
        }

        (current++)->type = 1;
        goto ARRAY;
    }
    case '{': {
        if (WJR_UNLIKELY(parser(in_place_token_left_brace))) {
            return;
        }

        (current++)->type = 1;
        goto OBJECT;
    }
    case ']': {
        return parser(in_place_token_failure<FAILURE>);
    }
    case '}': {
        return parser(in_place_token_failure<FAILURE>);
    }
    default: {
        return parser(in_place_token_failure<FAILURE>);
    }
    }

    goto OBJECT_ELEMENT_SPACE;
}
}

template <typename Parser>
WJR_INTRINSIC_INLINE void parse(span<const char> sp, Parser &parser) {
    constexpr uint32_t token_buf_size = 1024;

    unique_stack_allocator stkal(math_details::stack_alloc);

    uint32_t *token_buf = static_cast<uint32_t *>(
        stkal.allocate((token_buf_size * 2 - 1) * sizeof(uint32_t)));

    forward_token_reader<token_buf_size> reader(sp, token_buf);
    reader_parse(reader, parser);
}

} // namespace wjr::json

#endif // WJR_JSON_PARSER_HPP__