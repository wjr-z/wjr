#ifndef WJR_FORMAT_CCTYPE_HPP__
#define WJR_FORMAT_CCTYPE_HPP__

#include <array>

namespace wjr {
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

constexpr bool isDigit(char C) { return C >= '0' && C <= '9'; }

constexpr bool isHexDigit(char C) { return cctype::get_code(C) != 127; }

constexpr bool isLower(char C) { return 'a' <= C && C <= 'z'; }
constexpr bool isUpper(char C) { return 'A' <= C && C <= 'Z'; }

constexpr bool isAlpha(char C) {
    auto code = cctype::get_code(C);
    return code >= 10 && code < 36;
}

constexpr bool isAlnum(char C) {
    auto code = cctype::get_code(C);
    return code < 36;
}

constexpr bool isAscii(char C) { return static_cast<unsigned char>(C) <= 127; }

constexpr char toLower(char C) {
    if (isUpper(C)) {
        return C - 'A' + 'a';
    }

    return C;
}

constexpr char toupper(char C) {
    if (isLower(C)) {
        return C - 'a' + 'A';
    }

    return C;
}

} // namespace wjr

#endif // WJR_FORMAT_CCTYPE_HPP__