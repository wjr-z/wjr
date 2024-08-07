#ifndef WJR_JSON_DETAIL_HPP__
#define WJR_JSON_DETAIL_HPP__

#include <wjr/expected.hpp>
#include <wjr/math/integral_constant.hpp>

namespace wjr::json {

enum class error_code : uint8_t {
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
    NUM_ERROR_CODES,
};

using result_error = compressed_unexpected<error_code, error_code::SUCCESS>;

template <typename T>
using result = expected<T, result_error>;

enum class value_t : uint8_t {
    number_unsigned,
    number_signed,
    number_float,
    null,
    boolean,
    string,
    object,
    array,
};

using number_unsigned_t = integral_constant<value_t, value_t::number_unsigned>;
inline constexpr number_unsigned_t number_unsigned_v = {};

using number_signed_t = integral_constant<value_t, value_t::number_signed>;
inline constexpr number_signed_t number_signed_v = {};

using number_float_t = integral_constant<value_t, value_t::number_float>;
inline constexpr number_float_t number_float_v = {};

using null_t = integral_constant<value_t, value_t::null>;
inline constexpr null_t null_v = {};

using boolean_t = integral_constant<value_t, value_t::boolean>;
inline constexpr boolean_t boolean_v = {};

using string_t = integral_constant<value_t, value_t::string>;
inline constexpr string_t string_v = {};

using object_t = integral_constant<value_t, value_t::object>;
inline constexpr object_t object_v = {};

using array_t = integral_constant<value_t, value_t::array>;
inline constexpr array_t array_v = {};

struct basic_value {
    basic_value() = default;
    basic_value(const basic_value &) = default;
    basic_value(basic_value &&) = default;
    basic_value &operator=(const basic_value &) = default;
    basic_value &operator=(basic_value &&) = default;
    ~basic_value() = default;

    basic_value(null_t) noexcept : m_type(value_t::null) {}
    basic_value(boolean_t, bool f) noexcept : m_boolean(f), m_type(value_t::boolean) {}
    basic_value(number_signed_t, int64_t value) noexcept
        : m_number_signed(value), m_type(value_t::number_signed) {}
    basic_value(number_unsigned_t, uint64_t value) noexcept
        : m_number_unsigned(value), m_type(value_t::number_signed) {}
    basic_value(number_float_t, double value) noexcept
        : m_number_float(value), m_type(value_t::number_float) {}
    basic_value(string_t, void *ptr) noexcept : m_ptr(ptr), m_type(value_t::string) {}
    basic_value(object_t, void *ptr) noexcept : m_ptr(ptr), m_type(value_t::object) {}
    basic_value(array_t, void *ptr) noexcept : m_ptr(ptr), m_type(value_t::array) {}

    union {
        uint64_t m_number_unsigned;
        int64_t m_number_signed;
        double m_number_float;
        std::nullptr_t m_null;
        bool m_boolean;
        void *m_ptr;
    };

    value_t m_type;
};

} // namespace wjr::json

#endif // WJR_JSON_DETAIL_HPP__