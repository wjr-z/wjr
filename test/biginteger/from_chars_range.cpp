#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

#include <wjr/biginteger.hpp>

namespace {

std::string make_digits(unsigned int base, std::size_t count) {
    std::string digits(count, '0');
    for (std::size_t i = 0; i != count; ++i) {
        const auto digit = static_cast<unsigned int>((i * 7 + 1) % base);
        digits[i] = digit >= 10 && i % 2 == 0 ? static_cast<char>('A' + digit - 10)
                                              : wjr::char_converter.to(digit);
    }
    return digits;
}

wjr::biginteger digit_value(unsigned int base, std::size_t count) {
    wjr::biginteger value(0);
    for (std::size_t i = 0; i != count; ++i) {
        value *= base;
        value += (i * 7 + 1) % base;
    }
    return value;
}

} // namespace

TEST(biginteger_from_chars_range, stops_at_each_bounded_numeric_prefix) {
    for (const unsigned int base : {2, 8, 10, 16}) {
        const auto digits = make_digits(base, 96);
        for (const bool negative : {false, true}) {
            const std::string input = (negative ? "-" : "") + digits + '!';
            for (std::size_t length = 1; length <= 40; ++length) {
                SCOPED_TRACE(::testing::Message() << "base=" << base << " length=" << length
                                                  << " negative=" << negative);
                const auto *const first = input.data();
                const auto *const last = first + length + negative;
                wjr::biginteger actual(42);
                const auto result = wjr::from_chars(first, last, actual, base);
                auto expected = digit_value(base, length);
                if (negative) {
                    expected.negate();
                }
                EXPECT_EQ(result.ec, std::errc{});
                EXPECT_EQ(static_cast<const void *>(result.ptr), static_cast<const void *>(last));
                EXPECT_EQ(actual, expected);
            }
        }
    }
}

TEST(biginteger_from_chars_range, exact_buffers_do_not_require_a_sentinel) {
    for (const unsigned int base : {2, 8, 10, 16}) {
        for (const std::size_t length : {1, 2, 8, 9, 16, 17, 32, 33}) {
            for (const bool negative : {false, true}) {
                SCOPED_TRACE(::testing::Message() << "base=" << base << " length=" << length
                                                  << " negative=" << negative);
                const auto size = length + negative;
                auto input = std::make_unique<char[]>(size);
                if (negative) {
                    input[0] = '-';
                }
                const auto digits = make_digits(base, length);
                std::copy(digits.begin(), digits.end(), input.get() + negative);
                wjr::biginteger actual;
                const auto result = wjr::from_chars(input.get(), input.get() + size, actual, base);
                auto expected = digit_value(base, length);
                if (negative) {
                    expected.negate();
                }
                EXPECT_EQ(result.ec, std::errc{});
                EXPECT_EQ(static_cast<const void *>(result.ptr),
                          static_cast<const void *>(input.get() + size));
                EXPECT_EQ(actual, expected);
            }
        }
    }
    constexpr std::string_view spaces = " \t\n";
    auto input = std::make_unique<char[]>(spaces.size());
    std::copy(spaces.begin(), spaces.end(), input.get());
    wjr::biginteger value(42);
    const auto result = wjr::from_chars(input.get(), input.get() + spaces.size(), value);
    EXPECT_EQ(result.ec, std::errc::invalid_argument);
    EXPECT_EQ(result.ptr, input.get());
    EXPECT_EQ(value, 0);
}

TEST(biginteger_from_chars_range, bounded_whitespace_sign_and_auto_base_lookahead) {
    struct example {
        std::string_view text;
        std::size_t length;
        long long value;
        std::size_t consumed;
        bool valid;
    };
    constexpr example examples[] = {
        {"123456!", 0, 0, 0, false},      {"-123456!", 1, 0, 0, false},
        {" \t123456!", 2, 0, 0, false},   {" \t123456!", 4, 12, 4, true},
        {" \t-123456!", 5, -12, 5, true}, {"0x1234!", 1, 0, 1, true},
        {"0x1234!", 2, 0, 0, false},      {"0x1234!", 4, 18, 4, true},
        {"0Xabc!", 4, 171, 4, true},      {"0b10101!", 2, 0, 0, false},
        {"0b10101!", 5, 5, 5, true},      {"-0x123!", 2, 0, 2, true},
        {"-0x123!", 3, 0, 0, false},      {"0!", 2, 0, 1, true},
        {"09!", 2, 0, 1, true},           {" \t-0!", 4, 0, 4, true},
    };
    for (const auto &item : examples) {
        SCOPED_TRACE(::testing::Message() << "input=" << item.text << " length=" << item.length);
        wjr::biginteger value(42);
        const auto result =
            wjr::from_chars(item.text.data(), item.text.data() + item.length, value, 0);
        EXPECT_EQ(result.ec, item.valid ? std::errc{} : std::errc::invalid_argument);
        EXPECT_EQ(static_cast<const void *>(result.ptr),
                  static_cast<const void *>(item.text.data() + item.consumed));
        EXPECT_EQ(value, item.value);
    }
}

TEST(biginteger_from_chars_range, trusted_character_path_keeps_its_supported_inputs) {
    for (const unsigned int base : {2, 8, 10, 16}) {
        const auto digits = make_digits(base, 33);
        wjr::biginteger value;
        const auto result = wjr::from_chars_characters_unchecked(
            digits.data(), digits.data() + digits.size(), value, base);
        EXPECT_EQ(result.ec, std::errc{});
        EXPECT_EQ(result.ptr, digits.data() + digits.size());
        EXPECT_EQ(value, digit_value(base, digits.size()));
    }
}
