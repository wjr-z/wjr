/**
 * @file format/ctype.hpp
 * @brief Character type checking and conversion utilities
 * @author wjr
 *
 * Provides constexpr-friendly character classification and conversion functions.
 */

#ifndef WJR_FORMAT_CTYPE_HPP__
#define WJR_FORMAT_CTYPE_HPP__

#include <array>

namespace wjr {
/// @private
namespace cctype {
static constexpr std::array<uint8_t, 36> digit_table = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

static constexpr std::array<uint8_t, 256> code_table = {
    127, 127, 127, 127, 127, 127, 127, 127, 127, 64,  64,  64,  64,  64,  127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 64,  127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 0,   1,   2,   3,   4,   5,   6,   7,   8,
    9,   127, 127, 127, 127, 127, 127, 127, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,
    21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  127, 127, 127, 127,
    127, 127, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,
    27,  28,  29,  30,  31,  32,  33,  34,  35,  127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127};

constexpr uint8_t get_code(char c) { return code_table[static_cast<unsigned>(c)]; }
} // namespace cctype

/// Check if character is decimal digit (0-9)
constexpr bool isDigit(char C) { return C >= '0' && C <= '9'; }

/// Check if character is hexadecimal digit (0-9, a-f, A-F)
constexpr bool isHexDigit(char C) { return cctype::get_code(C) != 127; }

/// Check if character is lowercase letter (a-z)
constexpr bool isLower(char C) { return 'a' <= C && C <= 'z'; }

/// Check if character is uppercase letter (A-Z)
constexpr bool isUpper(char C) { return 'A' <= C && C <= 'Z'; }

/// Check if character is alphabetic (a-z, A-Z)
constexpr bool isAlpha(char C) {
    auto code = cctype::get_code(C);
    return code >= 10 && code < 36;
}

/// Check if character is alphanumeric (0-9, a-z, A-Z)
constexpr bool isAlnum(char C) {
    auto code = cctype::get_code(C);
    return code < 36;
}

/// Check if character is ASCII (0-127)
constexpr bool isAscii(char C) { return static_cast<unsigned char>(C) <= 127; }

/**
 * @brief Convert character to lowercase
 * @param[in] C Character to convert
 * @return Lowercase character if uppercase, otherwise unchanged
 */
constexpr char toLower(char C) {
    if (isUpper(C)) {
        return C - 'A' + 'a';
    }

    return C;
}

/**
 * @brief Convert character to uppercase
 * @param[in] C Character to convert
 * @return Uppercase character if lowercase, otherwise unchanged
 */
constexpr char toUpper(char C) {
    if (isLower(C)) {
        return C - 'a' + 'A';
    }

    return C;
}

/// Check if character is whitespace
constexpr bool isSpace(char C) { return cctype::get_code(C) == 64; }

} // namespace wjr

#endif // WJR_FORMAT_CTYPE_HPP__