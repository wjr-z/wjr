#ifndef WJR_MATH_CONVERT_IMPL_HPP__
#define WJR_MATH_CONVERT_IMPL_HPP__

#include <array>

#include <wjr/assert.hpp>

namespace wjr {

struct char_converter_t {
private:
    static constexpr std::array<char, 36> to_table = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    static constexpr std::array<uint8_t, 256> from_table = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  0,  0,
        0,  0,  0,  0,  0,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
        25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0,  0,  0,  0,  0,  0,  10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
        33, 34, 35, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};

public:
    WJR_CONST constexpr static char to(uint8_t x) {
        WJR_ASSERT_L2(x < 36);
        
        if (WJR_BUILTIN_CONSTANT_P(x < 10) && x < 10) {
            return x + '0';
        }

        return to_table[x];
    }
    WJR_CONST constexpr static uint8_t from(char x) {
        WJR_ASSERT_L2((x >= '0' && x <= '9') || (x >= 'a' && x <= 'z') ||
                      (x >= 'A' && x <= 'Z'));
        return from_table[x];
    }
};

/**
 * @brief 36 hexadecimal character<->number converter
 *
 * @note Range of characters: `[0-9a-zA-Z]`, range of numbers: `[0-35]`
 */
inline constexpr char_converter_t char_converter = {};

struct origin_converter_t {
    WJR_CONST constexpr static char to(uint8_t x) { return x; }
    WJR_CONST constexpr static uint8_t from(char x) { return x; }
};

/**
 * @brief Original converter that does not change the number or character.
 *
 * @note Range of characters: `[0-35]`, range of numbers: `[0-35]`
 */
inline constexpr origin_converter_t origin_converter = {};

} // namespace wjr

#endif // WJR_MATH_CONVERT_IMPL_HPP__
