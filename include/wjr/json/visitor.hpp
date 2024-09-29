#ifndef WJR_JSON_VISITOR_HPP__
#define WJR_JSON_VISITOR_HPP__

#include <wjr/container/bitset.hpp>
#include <wjr/json/number.hpp>
#include <wjr/json/reader.hpp>
#include <wjr/json/string.hpp>

namespace wjr::json {

namespace detail {

WJR_PURE WJR_INTRINSIC_INLINE result<void> check_null(const char *first) noexcept {
    if (WJR_UNLIKELY(std::memcmp(first, "null", 4) != 0)) {
        return unexpected(error_code::N_ATOM_ERROR);
    }

    return {};
}

WJR_PURE WJR_INTRINSIC_INLINE result<void> check_true(const char *first) noexcept {
    if (WJR_UNLIKELY(std::memcmp(first, "true", 4) != 0)) {
        return unexpected(error_code::T_ATOM_ERROR);
    }

    return {};
}

WJR_PURE WJR_INTRINSIC_INLINE result<void> check_false(const char *first) noexcept {
    if (WJR_UNLIKELY(std::memcmp(first + 1, "alse", 4) != 0)) {
        return unexpected(error_code::N_ATOM_ERROR);
    }

    return {};
}

} // namespace detail

namespace visitor_detail {

template <typename Parser>
WJR_NOINLINE result<void> parse(Parser &&par, const reader &rd) noexcept {
    constexpr unsigned int max_depth = 256;

    bitset<max_depth> stk(default_construct);
    unsigned int depth = 0;
    uint8_t type;

    // token reader
    auto read = [&rd]() {
        auto __begin = rd.begin();
        auto __end = rd.end();
        return [__begin,
                __end](uint32_t &token,
                       error_code err = error_code::TAPE_ERROR) mutable -> result<void> {
            if (WJR_LIKELY(__begin != __end)) {
                token = *__begin++;
                return {};
            }

            return unexpected(err);
        };
    }();

    uint32_t token;
    uint32_t next_token;

    const char *const ptr = rd.data();
    WJR_EXPECTED_TRY(read(token, error_code::EMPTY));

    if (uint8_t ch = ptr[token]; WJR_LIKELY(ch == '{')) {
        WJR_EXPECTED_TRY(par.visit_root_start_object(token));
        WJR_EXPECTED_TRY(read(token));

        if (ch = ptr[token]; WJR_UNLIKELY(ch != '"')) {
            if (WJR_UNLIKELY(ch != '}')) {
                return unexpected(error_code::TAPE_ERROR);
            }

            WJR_EXPECTED_TRY(par.visit_end_object_to_root(token));
            return {};
        }

        goto OBJECT_ELEMENT;
    } else if (WJR_LIKELY(ch == '[')) {
        WJR_EXPECTED_TRY(par.visit_root_start_array(token));
        WJR_EXPECTED_TRY(read(token));

        if (ptr[token] == ']') {
            WJR_EXPECTED_TRY(par.visit_end_array_to_root(token));
            return {};
        }

        goto ARRAY_ELEMENT;
    } else {
        const auto size = rd.size();

        if (ch == '"') {
            if (WJR_UNLIKELY(!read(next_token))) {
                return unexpected(error_code::TAPE_ERROR);
            }

            if (WJR_UNLIKELY(ptr[next_token] != '"')) {
                return unexpected(error_code::UNCLOSED_STRING);
            }

            WJR_EXPECTED_TRY(par.visit_root_string(ptr + token + 1, ptr + next_token));

            if (WJR_UNLIKELY(read(next_token))) {
                return unexpected(error_code::TAPE_ERROR);
            }

            return {};
        }

        if (WJR_UNLIKELY(read(next_token))) {
            return unexpected(error_code::TAPE_ERROR);
        }

        switch (ch) {
        case 'n': {
            if (WJR_UNLIKELY(size - token < 4)) {
                return unexpected(error_code::TAPE_ERROR);
            }

            WJR_EXPECTED_TRY(par.visit_root_null(ptr + token));
            break;
        }
        case 't': {
            if (WJR_UNLIKELY(size - token < 4)) {
                return unexpected(error_code::TAPE_ERROR);
            }

            WJR_EXPECTED_TRY(par.visit_root_true(ptr + token));
            break;
        }
        case 'f': {
            if (WJR_UNLIKELY(size - token < 5)) {
                return unexpected(error_code::TAPE_ERROR);
            }

            WJR_EXPECTED_TRY(par.visit_root_false(ptr + token));
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
            WJR_EXPECTED_TRY(par.visit_root_number(ptr + token, ptr + size));
            break;
        }
        default: {
            return unexpected(error_code::TAPE_ERROR);
        }
        }

        return {};
    }

OBJECT_ELEMENTS : {
    if (const uint8_t ch = ptr[next_token]; WJR_UNLIKELY(ch != ',')) {
        if (WJR_UNLIKELY(ch != '}')) {
            return unexpected(error_code::TAPE_ERROR);
        }

        if (depth == 0) {
            WJR_EXPECTED_TRY(par.visit_end_object_to_root(next_token));
            return {};
        }

        token = next_token;
        type = stk[--depth];
        WJR_EXPECTED_TRY(read(next_token));

        if (type == 0) {
            WJR_EXPECTED_TRY(par.visit_end_object_to_object(token));
            goto OBJECT_ELEMENTS;
        } else {
            WJR_EXPECTED_TRY(par.visit_end_object_to_array(token));
            goto ARRAY_ELEMENTS;
        }
    }

    WJR_EXPECTED_TRY(read(token));

    if (WJR_UNLIKELY(ptr[token] != '"')) {
        return unexpected(error_code::TAPE_ERROR);
    }

OBJECT_ELEMENT : {
    WJR_EXPECTED_TRY(read(next_token, error_code::UNCLOSED_STRING));
    WJR_EXPECTED_TRY(par.visit_object_key_string(ptr + token + 1, ptr + next_token));
    WJR_EXPECTED_TRY(read(next_token));

    if (const uint8_t ch = ptr[next_token]; WJR_UNLIKELY(ch != ':')) {
        return unexpected(error_code::TAPE_ERROR);
    }

    WJR_EXPECTED_TRY(read(token));

    switch (static_cast<uint8_t>(ptr[token])) {
    case 'n': {
        WJR_EXPECTED_TRY(read(next_token));

        if (WJR_UNLIKELY(next_token - token < 4)) {
            return unexpected(error_code::TAPE_ERROR);
        }

        WJR_EXPECTED_TRY(par.visit_object_null(ptr + token));
        goto OBJECT_ELEMENTS;
    }
    case 't': {
        WJR_EXPECTED_TRY(read(next_token));

        if (WJR_UNLIKELY(next_token - token < 4)) {
            return unexpected(error_code::TAPE_ERROR);
        }

        WJR_EXPECTED_TRY(par.visit_object_true(ptr + token));
        goto OBJECT_ELEMENTS;
    }
    case 'f': {
        WJR_EXPECTED_TRY(read(next_token));

        if (WJR_UNLIKELY(next_token - token < 5)) {
            return unexpected(error_code::TAPE_ERROR);
        }

        WJR_EXPECTED_TRY(par.visit_object_false(ptr + token));
        goto OBJECT_ELEMENTS;
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
        WJR_EXPECTED_TRY(par.visit_object_number(ptr + token, ptr + next_token));
        goto OBJECT_ELEMENTS;
    }
    case '"': {
        WJR_EXPECTED_TRY(read(next_token, error_code::UNCLOSED_STRING));
        WJR_EXPECTED_TRY(par.visit_object_string(ptr + token + 1, ptr + next_token));
        WJR_EXPECTED_TRY(read(next_token));
        goto OBJECT_ELEMENTS;
    }
    case '{': {
        if (WJR_UNLIKELY(depth == max_depth)) {
            return unexpected(error_code::DEPTH_ERROR);
        }

        WJR_EXPECTED_TRY(par.visit_object_start_object(token));
        WJR_EXPECTED_TRY(read(token));

        if (const uint8_t ch = ptr[token]; ch != '"') {
            if (WJR_UNLIKELY(ch != '}')) {
                return unexpected(error_code::TAPE_ERROR);
            }

            WJR_EXPECTED_TRY(read(next_token));
            WJR_EXPECTED_TRY(par.visit_end_object_to_object(token));
            goto OBJECT_ELEMENTS;
        }

        stk[depth++] = false;
        goto OBJECT_ELEMENT;
    }
    case '[': {
        if (WJR_UNLIKELY(depth == max_depth)) {
            return unexpected(error_code::DEPTH_ERROR);
        }

        WJR_EXPECTED_TRY(par.visit_object_start_array(token));
        WJR_EXPECTED_TRY(read(token));

        if (ptr[token] == ']') {
            WJR_EXPECTED_TRY(read(next_token));
            WJR_EXPECTED_TRY(par.visit_end_array_to_object(token));
            goto OBJECT_ELEMENTS;
        }

        stk[depth++] = false;
        goto ARRAY_ELEMENT;
    }
    default: {
        return unexpected(error_code::TAPE_ERROR);
    }
    }
}
}

ARRAY_ELEMENTS : {
    if (const uint8_t ch = ptr[next_token]; WJR_UNLIKELY(ch != ',')) {
        if (WJR_UNLIKELY(ch != ']')) {
            return unexpected(error_code::TAPE_ERROR);
        }

        if (depth == 0) {
            WJR_EXPECTED_TRY(par.visit_end_array_to_root(next_token));

            return {};
        }

        token = next_token;
        WJR_EXPECTED_TRY(read(next_token));

        type = stk[--depth];

        if (type == 0) {
            WJR_EXPECTED_TRY(par.visit_end_array_to_object(token));
            goto OBJECT_ELEMENTS;
        } else {
            WJR_EXPECTED_TRY(par.visit_end_array_to_array(token));
            goto ARRAY_ELEMENTS;
        }
    }

    WJR_EXPECTED_TRY(read(token));

ARRAY_ELEMENT:
    switch (static_cast<uint8_t>(ptr[token])) {
    case 'n': {
        WJR_EXPECTED_TRY(read(next_token));

        if (WJR_UNLIKELY(next_token - token < 4)) {
            return unexpected(error_code::TAPE_ERROR);
        }

        WJR_EXPECTED_TRY(par.visit_array_null(ptr + token));
        goto ARRAY_ELEMENTS;
    }
    case 't': {
        WJR_EXPECTED_TRY(read(next_token));

        if (WJR_UNLIKELY(next_token - token < 4)) {
            return unexpected(error_code::TAPE_ERROR);
        }

        WJR_EXPECTED_TRY(par.visit_array_true(ptr + token));
        goto ARRAY_ELEMENTS;
    }
    case 'f': {
        WJR_EXPECTED_TRY(read(next_token));

        if (WJR_UNLIKELY(next_token - token < 5)) {
            return unexpected(error_code::TAPE_ERROR);
        }

        WJR_EXPECTED_TRY(par.visit_array_false(ptr + token));
        goto ARRAY_ELEMENTS;
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
        WJR_EXPECTED_TRY(par.visit_array_number(ptr + token, ptr + next_token));
        goto ARRAY_ELEMENTS;
    }
    case '"': {
        WJR_EXPECTED_TRY(read(next_token, error_code::UNCLOSED_STRING));
        WJR_EXPECTED_TRY(par.visit_array_string(ptr + token + 1, ptr + next_token));
        WJR_EXPECTED_TRY(read(next_token));
        goto ARRAY_ELEMENTS;
    }
    case '{': {
        if (WJR_UNLIKELY(depth == max_depth)) {
            return unexpected(error_code::DEPTH_ERROR);
        }

        WJR_EXPECTED_TRY(par.visit_array_start_object(token));
        WJR_EXPECTED_TRY(read(token));

        if (const uint8_t ch = ptr[token]; ch != '"') {
            if (WJR_UNLIKELY(ch != '}')) {
                return unexpected(error_code::TAPE_ERROR);
            }

            WJR_EXPECTED_TRY(read(next_token));
            WJR_EXPECTED_TRY(par.visit_end_object_to_array(token));

            goto ARRAY_ELEMENTS;
        }

        stk[depth++] = true;
        goto OBJECT_ELEMENT;
    }
    case '[': {
        if (WJR_UNLIKELY(depth == max_depth)) {
            return unexpected(error_code::DEPTH_ERROR);
        }

        WJR_EXPECTED_TRY(par.visit_array_start_array(token));

        WJR_EXPECTED_TRY(read(token));

        if (ptr[token] == ']') {
            WJR_EXPECTED_TRY(read(next_token));
            WJR_EXPECTED_TRY(par.visit_end_array_to_array(token));
            goto ARRAY_ELEMENTS;
        }

        stk[depth++] = true;
        goto ARRAY_ELEMENT;
    }
    default: {
        return unexpected(error_code::TAPE_ERROR);
    }
    }
}
}

} // namespace visitor_detail

} // namespace wjr::json

#endif // WJR_JSON_VISITOR_HPP__