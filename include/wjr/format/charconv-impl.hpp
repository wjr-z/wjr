#ifndef WJR_FORMAT_CHARCONV_IMPL_HPP__
#define WJR_FORMAT_CHARCONV_IMPL_HPP__

#include <array>

#include <wjr/math/integral_constant.hpp>

namespace wjr {

enum class convert_option : uint8_t {
    none = 0x00,
    no_leading_zeros = 0x01,
};

template <convert_option option>
using convert_option_t = integral_constant<convert_option, option>;

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
            WJR_ASSERT_ASSUME_L3(x < 36);
        } else {
            WJR_ASSERT_ASSUME_L3(x < Base);
        }

        if constexpr (Base == 0 || Base > 10) {
            if (WJR_BUILTIN_CONSTANT_P_TRUE(x < 10)) {
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

enum class chars_format : uint8_t {
    scientific = 0x01,
    fixed = 0x02,
    hex = 0x04,
    general = fixed | scientific,
    // only used in integeral_constant
    __json_format = 0x08,
    json = general | __json_format,
};

template <typename Iter>
struct to_chars_result {
    Iter ptr;
    std::errc ec;

    friend bool operator==(const to_chars_result &lhs,
                           const to_chars_result &rhs) noexcept {
        return lhs.ptr == rhs.ptr && lhs.ec == rhs.ec;
    }

    constexpr explicit operator bool() const noexcept { return ec == std::errc{}; }
};

template <typename Iter = const char *>
struct from_chars_result {
    Iter ptr;
    std::errc ec;

    friend bool operator==(const from_chars_result &lhs,
                           const from_chars_result &rhs) noexcept {
        return lhs.ptr == rhs.ptr && lhs.ec == rhs.ec;
    }

    constexpr explicit operator bool() const noexcept { return ec == std::errc{}; }
};

WJR_PURE WJR_INTRINSIC_INLINE bool
is_made_of_eight_digits_fast(const char *src) noexcept {
    const auto val = read_memory<uint64_t>(src);
    return ((val & 0xF0F0F0F0F0F0F0F0) & (val + 0x0606060606060606)) ==
           0x3030303030303030;
}

WJR_PURE WJR_INTRINSIC_INLINE uint32_t
parse_eight_digits_unrolled(const char *src) noexcept;

} // namespace wjr

#endif // WJR_FORMAT_CHARCONV_IMPL_HPP__