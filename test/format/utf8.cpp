#include "../detail.hpp"

#include <string>

#include <wjr/format/utf8/utf8.hpp>

using namespace wjr;

TEST(utf8, codepoint_encoding) {
    struct Case {
        uint32_t code_point;
        int size;
        std::string value;
    };

    const Case cases[] = {
        {0x24, 1, "$"},
        {0xA2, 2, "\xC2\xA2"},
        {0x20AC, 3, "\xE2\x82\xAC"},
        {0x1F600, 4, "\xF0\x9F\x98\x80"},
        {0x110000, 0, ""},
    };

    for (const auto &test_case : cases) {
        char buffer[4] = {};
        const auto size = utf8::detail::unicode_codepoint_to_utf8(buffer, test_case.code_point);
        EXPECT_EQ(size, test_case.size);
        EXPECT_EQ(std::string(buffer, size), test_case.value);
    }
}

TEST(utf8, unicode_escape_conversion) {
    const std::pair<const char *, std::string> cases[] = {
        {R"(\u0041)", "A"},
        {R"(\u00e9)", "\xC3\xA9"},
        {R"(\ud83d\ude00)", "\xF0\x9F\x98\x80"},
        {R"(\n\t\"\\)", "\n\t\"\\"},
        {R"(plain\u0041\u00e9\ud83d\ude00\n\t\"\\)", "plainA\xC3\xA9\xF0\x9F\x98\x80\n\t\"\\"},
    };

    for (const auto &[input, expected] : cases) {
        SCOPED_TRACE(input);
        const auto last = input + std::char_traits<char>::length(input);
        char buffer[64] = {};

        if (input[0] == '\\' && input[1] == 'u') {
            EXPECT_TRUE(utf8::check_unicode_codepoint(input + 2, last));
        }
        EXPECT_TRUE(utf8::check_unicode(input, last));
        auto result = utf8::unicode_to_utf8(buffer, input, last);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(*result, buffer + expected.size());
        EXPECT_EQ(std::string(buffer, expected.size()), expected);
    }
}

TEST(utf8, invalid_unicode_escapes) {
    const char *invalid[] = {
        R"(\q)", R"(\u00)", R"(\uD800)", R"(\uD800\u0041)", R"(\uDC00)",
    };

    for (const char *input : invalid) {
        const auto last = input + std::char_traits<char>::length(input);
        EXPECT_FALSE(utf8::check_unicode(input, last));
        char buffer[16] = {};
        EXPECT_FALSE(utf8::unicode_to_utf8(buffer, input, last));
    }

    EXPECT_TRUE(utf8::check_unicode("", ""));
}
