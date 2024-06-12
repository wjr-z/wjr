#ifndef WJR_MATH_CONVERT_IMPL_HPP__
#define WJR_MATH_CONVERT_IMPL_HPP__

namespace wjr {

class char_converter_t {
    static constexpr std::array<uint8_t, 36> to_table = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

    static constexpr std::array<uint8_t, 256> from_table = {
        127, 127, 127, 127, 127, 127, 127, 127, 127, 64,  64,  64,  64,  64,  127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        64,  127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   127, 127, 127, 127, 127, 127,
        127, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
        25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  127, 127, 127, 127, 127,
        127, 10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
        25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
        127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};

public:
    template <uint64_t Base = 0>
    WJR_CONST static constexpr uint8_t to(uint8_t x) noexcept {
        if constexpr (Base == 0) {
            WJR_ASSERT_L3(x < 36);
        } else {
            WJR_ASSERT_L3(x < Base);
        }

        if constexpr (Base == 0 || Base > 10) {

            if (WJR_BUILTIN_CONSTANT_P(x < 10) && x < 10) {
                return x + '0';
            }

            return to_table[x];
        } else {
            return x + '0';
        }
    }

    template <uint64_t Base = 0>
    WJR_CONST static constexpr uint8_t from(uint8_t x) noexcept {
        if constexpr (Base == 0 || Base > 10) {
            return from_table[x];
        } else {
            return x - '0';
        }
    }

private:
};

inline constexpr char_converter_t char_converter;

class origin_converter_t {
public:
    template <uint64_t Base = 0>
    WJR_CONST static constexpr uint64_t to(uint64_t x) noexcept {
        return x;
    }

    template <uint64_t Base = 0>
    WJR_CONST static constexpr uint64_t from(uint64_t x) noexcept {
        return x;
    }
};

inline constexpr origin_converter_t origin_converter;

} // namespace wjr

#endif // WJR_MATH_CONVERT_IMPL_HPP__