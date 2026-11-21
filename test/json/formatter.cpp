#include <gtest/gtest.h>

#include <iterator>
#include <string>
#include <string_view>
#include <vector>

#include <wjr/json/formatter.hpp>
#include <wjr/string.hpp>

namespace {

template <typename Output>
void expect_quoted(std::string_view input, std::string_view expected) {
    Output output;
    output.push_back(':');
    wjr::json::minify_formatter formatter(std::back_inserter(output));
    formatter.format_string(input);
    ASSERT_EQ(output.front(), ':');
    EXPECT_EQ(
        std::string_view(reinterpret_cast<const char *>(output.data() + 1), output.size() - 1),
        expected);
}

} // namespace

TEST(json_formatter, strings_cover_clean_blocks_escapes_and_tail) {
    expect_quoted<std::string>({}, R"("")");
    expect_quoted<std::string>("0123456789", R"("0123456789")");
    expect_quoted<std::string>("01234567\t123456701234567\\\n",
                               R"("01234567\t123456701234567\\\n")");
    expect_quoted<wjr::vector<char>>("\r0123456789\"\b", R"("\r0123456789\"\b")");
    expect_quoted<wjr::vector<char>>("0123456789012345\f", R"("0123456789012345\f")");
    expect_quoted<std::vector<unsigned char>>("0123456789\t", R"("0123456789\t")");
    expect_quoted<std::string>(std::string_view("\0\x01\x1f", 3), R"("\u0000\u0001\u001f")");
}

TEST(json_formatter, escape_at_each_position_preserves_surrounding_bytes) {
    for (size_t index = 0; index < 25; ++index) {
        std::string input(25, 'a');
        input[index] = '"';
        std::string expected = '"' + input + '"';
        expected.insert(index + 1, 1, '\\');
        expect_quoted<std::string>(input, expected);
    }
}

TEST(json_formatter, floating_format_commits_only_the_written_bytes) {
    constexpr size_t capacity = wjr::dragonbox::max_output_string_length_of<double>;
    constexpr double number = -1.2345678901234567e-300;
    char expected[capacity];
    const auto length = wjr::dragonbox::to_chars_n(number, expected) - expected;
    ASSERT_EQ(length, capacity);
    wjr::fixed_vector<char> output(capacity + 1, wjr::in_place_reserve);
    output.push_back(':');
    wjr::json::minify_formatter formatter(std::back_inserter(output));
    formatter.format_number_float(number);
    ASSERT_EQ(output.front(), ':');
    EXPECT_EQ(std::string_view(output.data() + 1, output.size() - 1),
              std::string_view(expected, static_cast<size_t>(length)));

    std::string text = "prefix:";
    wjr::json::minify_formatter text_formatter(std::back_inserter(text));
    text_formatter.format_number_float(number);
    text_formatter.format_comma();
    EXPECT_EQ(text, "prefix:" + std::string(expected, static_cast<size_t>(length)) + ",");
}

TEST(json_formatter, block_scan_matches_byte_classification_at_each_position) {
    for (size_t offset = 0; offset < 8; ++offset) {
        for (unsigned code = 0; code < 256; ++code) {
            char bytes[8] = {'A', ' ', '0', 'Z', 'g', ']', 'B', '1'};
            bytes[offset] = static_cast<char>(code);
            EXPECT_EQ(wjr::json::formatter_detail::block_needs_escaping(bytes),
                      code < 0x20 || code == '"' || code == '\\');
        }
    }
}
