#include <array>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include <wjr/json/detail/utf8.hpp>

namespace {
using wjr::json::detail::find_invalid_utf8;

TEST(json_utf8_validation, accepts_scalar_boundaries_and_unaligned_input) {
    constexpr std::array valid = {std::string_view{},
                                  std::string_view("ascii"),
                                  std::string_view("\xc2\x80"),
                                  std::string_view("\xdf\xbf"),
                                  std::string_view("\xe0\xa0\x80"),
                                  std::string_view("\xed\x9f\xbf"),
                                  std::string_view("\xee\x80\x80"),
                                  std::string_view("\xef\xbf\xbf"),
                                  std::string_view("\xf0\x90\x80\x80"),
                                  std::string_view("\xf4\x8f\xbf\xbf")};

    for (const auto input : valid) {
        EXPECT_EQ(find_invalid_utf8(input), std::string_view::npos);
        for (std::size_t prefix = 0; prefix != 17; ++prefix) {
            std::string padded(prefix, 'a');
            padded.append(input);
            padded.append(17, 'z');
            EXPECT_EQ(find_invalid_utf8(padded), std::string_view::npos);
        }
    }
    const std::string nulls(65, '\0');
    EXPECT_EQ(find_invalid_utf8(nulls), std::string_view::npos);
}

TEST(json_utf8_validation, rejects_malformed_sequences_at_their_start) {
    constexpr std::array invalid = {std::string_view("\x80"),
                                    std::string_view("\xbf"),
                                    std::string_view("\xc0\x80"),
                                    std::string_view("\xc1\xbf"),
                                    std::string_view("\xc2"),
                                    std::string_view("\xc2\x7f"),
                                    std::string_view("\xc2\xc0"),
                                    std::string_view("\xe0\x9f\xbf"),
                                    std::string_view("\xe1\x80"),
                                    std::string_view("\xe1\x80\x7f"),
                                    std::string_view("\xed\xa0\x80"),
                                    std::string_view("\xf0\x8f\xbf\xbf"),
                                    std::string_view("\xf1\x80\x80"),
                                    std::string_view("\xf1\x80\x80\xc0"),
                                    std::string_view("\xf4\x90\x80\x80"),
                                    std::string_view("\xf5\x80\x80\x80"),
                                    std::string_view("\xfe"),
                                    std::string_view("\xff")};

    for (const auto input : invalid) {
        EXPECT_EQ(find_invalid_utf8(input), 0);
        for (std::size_t prefix = 1; prefix != 17; ++prefix) {
            std::string padded(prefix, 'a');
            padded.append(input);
            EXPECT_EQ(find_invalid_utf8(padded), prefix);
        }
    }
}

TEST(json_utf8_validation, checks_every_two_byte_combination) {
    std::array<char, 2> bytes;
    for (unsigned int first = 0; first != 256; ++first) {
        for (unsigned int second = 0; second != 256; ++second) {
            bytes[0] = static_cast<char>(first);
            bytes[1] = static_cast<char>(second);
            const bool valid = (first < 0x80 && second < 0x80) ||
                               (first >= 0xc2 && first <= 0xdf && second >= 0x80 && second <= 0xbf);
            const auto input = std::string_view(bytes.data(), bytes.size());
            EXPECT_EQ(find_invalid_utf8(input) == std::string_view::npos, valid);
        }
    }
}

} // namespace
