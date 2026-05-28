/**
 * @file json/detail.hpp
 * @brief JSON parsing core types and error codes
 * @author wjr
 *
 * Provides error codes, result types, value type enumerations,
 * and basic JSON value structures for the JSON parsing library.
 */

#ifndef WJR_JSON_DETAIL_HPP__
#define WJR_JSON_DETAIL_HPP__

#include <wjr/expected.hpp>
#include <wjr/math/integral_constant.hpp>

namespace wjr::json {

enum class error_code : uint8_t {
    SUCCESS = 0,                ///< No error
    CAPACITY,                   ///< This parser can't support a document that big
    MEMALLOC,                   ///< Error allocating memory, most likely out of memory
    TAPE_ERROR,                 ///< Something went wrong, this is a generic error
    DEPTH_ERROR,                ///< Your document exceeds the user-specified depth limitation
    STRING_ERROR,               ///< Problem while parsing a string
    T_ATOM_ERROR,               ///< Problem while parsing an atom starting with 't'
    F_ATOM_ERROR,               ///< Problem while parsing an atom starting with 'f'
    N_ATOM_ERROR,               ///< Problem while parsing an atom starting with 'n'
    NUMBER_ERROR,               ///< Problem while parsing a number
    BIGINT_ERROR,               ///< The integer value exceeds 64 bits
    UTF8_ERROR,                 ///< The input is not valid UTF-8
    UNINITIALIZED,              ///< Unknown error, or uninitialized document
    EMPTY,                      ///< No structural element found
    UNESCAPED_CHARS,            ///< Found unescaped characters in a string
    UNCLOSED_STRING,            ///< Missing quote at the end
    UNSUPPORTED_ARCHITECTURE,   ///< unsupported architecture
    INCORRECT_TYPE,             ///< JSON element has a different type than user expected
    NUMBER_OUT_OF_RANGE,        ///< JSON number does not fit in 64 bits
    INDEX_OUT_OF_BOUNDS,        ///< JSON array index too large
    NO_SUCH_FIELD,              ///< JSON field not found in object
    IO_ERROR,                   ///< Error reading a file
    INVALID_JSON_POINTER,       ///< Invalid JSON pointer reference
    INVALID_URI_FRAGMENT,       ///< Invalid URI fragment
    UNEXPECTED_ERROR,           ///< Indicative of an internal bug
    PARSER_IN_USE,              ///< Parser is already in use
    OUT_OF_ORDER_ITERATION,     ///< Tried to iterate an array or object out of order
    INCOMPLETE_ARRAY_OR_OBJECT, ///< The document ends early.
    SCALAR_DOCUMENT_AS_VALUE,   ///< A scalar document is treated as a value.
    OUT_OF_BOUNDS,              ///< Attempted to access location outside of document.
    TRAILING_CONTENT,           ///< Unexpected trailing content in the JSON input
    NUM_ERROR_CODES,
};

using result_error = compressed_value<error_code, error_code::SUCCESS>;

template <typename T>
using result = expected<T, result_error>;

enum class value_t : uint8_t {
    null = 0,
    boolean = 1,
    number_unsigned = 2,
    number_signed = 3,
    number_float = 4,
    string = 5,
    object = 6,
    array = 7,
};

/// @brief Tag type identifying null values
using null_t = integral_constant<value_t, value_t::null>;
/// @brief Tag type identifying boolean values
using boolean_t = integral_constant<value_t, value_t::boolean>;
/// @brief Tag type identifying unsigned integer values (uint64_t)
using number_unsigned_t = integral_constant<value_t, value_t::number_unsigned>;
/// @brief Tag type identifying signed integer values (int64_t)
using number_signed_t = integral_constant<value_t, value_t::number_signed>;
/// @brief Tag type identifying floating-point values (double)
using number_float_t = integral_constant<value_t, value_t::number_float>;
/// @brief Tag type identifying string values
using string_t = integral_constant<value_t, value_t::string>;
/// @brief Tag type identifying object values
using object_t = integral_constant<value_t, value_t::object>;
/// @brief Tag type identifying array values
using array_t = integral_constant<value_t, value_t::array>;

/**
 * @brief Non-templated value container shared by all JSON value variants.
 *
 * Stores the active type tag and a union of all scalar payloads.
 * String, object, and array types are held as opaque pointers.
 */
struct basic_value {
    basic_value() noexcept : m_type(value_t::null) {}
    basic_value(const basic_value &) = default;
    basic_value(basic_value &&) = default;
    basic_value &operator=(const basic_value &) = default;
    basic_value &operator=(basic_value &&) = default;
    ~basic_value() = default;

    basic_value(default_construct_t) noexcept {}

    basic_value(null_t) noexcept : m_type(value_t::null) {}
    basic_value(boolean_t, bool f) noexcept : m_boolean(f), m_type(value_t::boolean) {}
    basic_value(number_unsigned_t, uint64_t value) noexcept
        : m_number_unsigned(value), m_type(value_t::number_unsigned) {}
    basic_value(number_signed_t, int64_t value) noexcept
        : m_number_signed(value), m_type(value_t::number_signed) {}
    basic_value(number_float_t, double value) noexcept
        : m_number_float(value), m_type(value_t::number_float) {}
    basic_value(string_t, void *ptr) noexcept : m_ptr(ptr), m_type(value_t::string) {}
    basic_value(object_t, void *ptr) noexcept : m_ptr(ptr), m_type(value_t::object) {}
    basic_value(array_t, void *ptr) noexcept : m_ptr(ptr), m_type(value_t::array) {}

    void set(null_t) noexcept { m_type = value_t::null; }
    void set(null_t, std::nullptr_t) noexcept { m_type = value_t::null; }

    void set(boolean_t, bool f) noexcept {
        m_boolean = f;
        m_type = value_t::boolean;
    }

    void set(number_unsigned_t, uint64_t value) noexcept {
        m_number_unsigned = value;
        m_type = value_t::number_unsigned;
    }

    void set(number_signed_t, int64_t value) noexcept {
        m_number_signed = value;
        m_type = value_t::number_signed;
    }

    void set(number_float_t, double value) noexcept {
        m_number_float = value;
        m_type = value_t::number_float;
    }

    void set(string_t, void *ptr) noexcept {
        m_ptr = ptr;
        m_type = value_t::string;
    }

    void set(object_t, void *ptr) noexcept {
        m_ptr = ptr;
        m_type = value_t::object;
    }

    void set(array_t, void *ptr) noexcept {
        m_ptr = ptr;
        m_type = value_t::array;
    }

    union {
        bool m_boolean;
        uint64_t m_number_unsigned;
        int64_t m_number_signed;
        double m_number_float;
        void *m_ptr;
    };

    value_t m_type;
};

} // namespace wjr::json

#endif // WJR_JSON_DETAIL_HPP__