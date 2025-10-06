/**
 * @file charconv-impl.hpp
 * @brief Basic types and converters for character conversion
 * @author wjr
 *
 * Provides fundamental types, converters, and enumerations used by charconv.hpp
 * for high-performance character-to-number and number-to-character conversions.
 */

#ifndef WJR_FORMAT_CHARCONV_IMPL_HPP__
#define WJR_FORMAT_CHARCONV_IMPL_HPP__

#include <array>

#include <wjr/format/ctype.hpp>
#include <wjr/math/integral_constant.hpp>
#include <wjr/utility/bitmask_enum.hpp>

namespace wjr {

/**
 * @brief Options for integer to string conversion
 *
 * @details Controls conversion behavior such as leading zero handling.
 */
enum class convert_option : uint8_t {
    none = 0x00,             ///< No special options
    no_leading_zeros = 0x01, ///< Suppress leading zeros in output
};

/**
 * @brief Compile-time constant for convert_option
 * @tparam option The conversion option value
 */
template <convert_option option>
using convert_option_t = integral_constant<convert_option, option>;

/**
 * @brief Character converter for converting between digits and ASCII characters
 *
 * @details Converts numeric digits (0-35) to ASCII characters ('0'-'9', 'a'-'z')
 * and vice versa. Uses lookup tables for bases > 10.
 */
class char_converter_t {
public:
    /**
     * @brief Convert digit to ASCII character
     * @tparam Base Numeric base (2-36), or 0 for runtime base
     * @param x Digit value to convert
     * @return ASCII character representation
     */
    template <uint64_t Base = 0>
    WJR_CONST static constexpr uint8_t to(uint8_t x) noexcept {
        if constexpr (Base == 0) {
            WJR_ASSERT_ASSUME_L3(x < 36);
        } else {
            WJR_ASSERT_ASSUME_L3(x < Base);
        }

        if constexpr (Base == 0 || Base > 10) {
            if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(x < 10)) {
                return x + '0';
            }

            return cctype::digit_table[x];
        } else {
            return x + '0';
        }
    }

    /**
     * @brief Convert ASCII character to digit
     * @tparam Base Numeric base (2-36), or 0 for runtime base
     * @param x ASCII character to convert
     * @return Digit value
     */
    template <uint64_t Base = 0>
    WJR_CONST static constexpr uint8_t from(uint8_t x) noexcept {
        if constexpr (Base == 0 || Base > 10) {
            return cctype::code_table[x];
        } else {
            return x - '0';
        }
    }
};

/// Global instance of character converter
inline constexpr char_converter_t char_converter;

/**
 * @brief Identity converter for numeric values
 *
 * @details No conversion is performed. Used for converting between
 * unsigned integer types directly without character encoding.
 */
class origin_converter_t {
public:
    /**
     * @brief Identity conversion (no-op)
     * @tparam Base Numeric base (ignored)
     * @param x Value to convert
     * @return Same value
     */
    template <uint64_t Base = 0>
    WJR_CONST static constexpr uint64_t to(uint64_t x) noexcept {
        return x;
    }

    /**
     * @brief Identity conversion (no-op)
     * @tparam Base Numeric base (ignored)
     * @param x Value to convert
     * @return Same value
     */
    template <uint64_t Base = 0>
    WJR_CONST static constexpr uint64_t from(uint64_t x) noexcept {
        return x;
    }
};

/// Global instance of origin converter
inline constexpr origin_converter_t origin_converter;

/**
 * @brief Format specification for floating-point number conversion
 *
 * @details Controls how floating-point numbers are formatted or parsed.
 * Similar to std::chars_format from C++17 <charconv>.
 */
enum class chars_format : uint8_t {
    scientific = 0x01,            ///< Scientific notation (e.g., 1.23e+10)
    fixed = 0x02,                 ///< Fixed-point notation (e.g., 123.45)
    hex = 0x04,                   ///< Hexadecimal notation
    general = fixed | scientific, ///< Allow either fixed or scientific
    /// @private Internal flag for JSON format
    __json_format = 0x08,
    json = general | __json_format, ///< JSON-compatible number format
};

WJR_DECLARE_ENUM_AS_BITMASK(chars_format);

/**
 * @brief Result of to_chars conversion
 *
 * @tparam Iter Iterator type pointing past the last character written
 *
 * @details Contains the output iterator position and error code.
 * Similar to std::to_chars_result from C++17 <charconv>.
 */
template <typename Iter>
struct to_chars_result {
    Iter ptr;     ///< Iterator pointing past last written character
    std::errc ec; ///< Error code (empty if successful)

    /**
     * @brief Compare two results for equality
     */
    friend bool operator==(const to_chars_result &lhs, const to_chars_result &rhs) noexcept {
        return lhs.ptr == rhs.ptr && lhs.ec == rhs.ec;
    }

    /**
     * @brief Check if conversion succeeded
     * @return true if no error occurred
     */
    constexpr explicit operator bool() const noexcept { return ec == std::errc{}; }
};

/**
 * @brief Result of from_chars conversion
 *
 * @tparam Iter Iterator type pointing past the last character parsed
 *
 * @details Contains the input iterator position and error code.
 * Similar to std::from_chars_result from C++17 <charconv>.
 */
template <typename Iter = const char *>
struct from_chars_result {
    Iter ptr;     ///< Iterator pointing past last parsed character
    std::errc ec; ///< Error code (empty if successful)

    /**
     * @brief Compare two results for equality
     */
    friend bool operator==(const from_chars_result &lhs, const from_chars_result &rhs) noexcept {
        return lhs.ptr == rhs.ptr && lhs.ec == rhs.ec;
    }

    /**
     * @brief Check if conversion succeeded
     * @return true if no error occurred
     */
    constexpr explicit operator bool() const noexcept { return ec == std::errc{}; }
};

/**
 * @brief Fast check if 8 bytes represent 8 ASCII digits
 * @param src Pointer to 8 characters
 * @return true if all 8 characters are digits ('0'-'9')
 */
WJR_PURE WJR_INTRINSIC_INLINE bool is_made_of_eight_digits_fast(const char *src) noexcept;

/**
 * @brief Parse 8 ASCII digits into a 32-bit integer
 * @param src Pointer to 8 digit characters
 * @return Parsed integer value
 * @note Assumes all 8 characters are valid digits
 */
WJR_PURE WJR_INTRINSIC_INLINE uint32_t parse_eight_digits_unrolled(const char *src) noexcept;

/**
 * @brief Skip leading whitespace characters
 * @param first Pointer to start of range
 * @param last Pointer to end of range
 * @return Pointer to first non-whitespace character
 */
WJR_PURE WJR_INTRINSIC_INLINE const char *skip_whitespace(const char *first,
                                                          const char *last) noexcept;

} // namespace wjr

#endif // WJR_FORMAT_CHARCONV_IMPL_HPP__