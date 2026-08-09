#include <gtest/gtest.h>

#include <string_view>

#include <wjr/string.hpp>

using namespace wjr;
using namespace std::string_view_literals;

namespace {

template <unsigned int Length>
void expect_constant_case_equal(const char *lhs, const char *rhs, bool expected) {
    EXPECT_EQ(constant_length_strncaseequal(lhs, rhs, integral_constant<unsigned int, Length>()),
              expected);
}

} // namespace

TEST(string, prefix_and_suffix) {
    EXPECT_TRUE(starts_with("wjr-string"sv, "wjr"sv));
    EXPECT_TRUE(starts_with("wjr-string"sv, "wjr-string"sv));
    EXPECT_TRUE(starts_with("wjr-string"sv, ""sv));
    EXPECT_FALSE(starts_with("wjr"sv, "wjr-string"sv));
    EXPECT_FALSE(starts_with("wjr-string"sv, "WJR"sv));

    EXPECT_TRUE(ends_with("wjr-string"sv, "string"sv));
    EXPECT_TRUE(ends_with("wjr-string"sv, "wjr-string"sv));
    EXPECT_TRUE(ends_with("wjr-string"sv, ""sv));
    EXPECT_FALSE(ends_with("string"sv, "wjr-string"sv));
    EXPECT_FALSE(ends_with("wjr-string"sv, "STRING"sv));

    EXPECT_TRUE(starts_with_insensitive("WJR-String", "wjr"));
    EXPECT_TRUE(ends_with_insensitive("wjr-STRING", "string"));
    EXPECT_FALSE(starts_with_insensitive("wjr", "wjr-string"));
    EXPECT_FALSE(ends_with_insensitive("string", "wjr-string"));
}

TEST(string, compare_insensitive) {
    EXPECT_EQ(string_detail::compare_nocase_n("WJR", "wjr", 3), 0);
    EXPECT_EQ(string_detail::compare_nocase_n("abc", "abd", 3), -1);
    EXPECT_EQ(string_detail::compare_nocase_n("abd", "abc", 3), 1);
    EXPECT_EQ(string_detail::compare_nocase_n("abc", "abd", 0), 0);

    EXPECT_EQ(compare_insensitive("WJR", "wjr"), 0);
    EXPECT_EQ(compare_insensitive("wjr", "wjr-test"), -1);
    EXPECT_EQ(compare_insensitive("wjr-test", "WJR"), 1);
    EXPECT_EQ(compare_insensitive("wjr-a", "WJR-b"), -1);
}

TEST(string, constant_length_case_equal) {
    expect_constant_case_equal<1>("A", "a", true);
    expect_constant_case_equal<2>("AB", "ab", true);
    expect_constant_case_equal<3>("ABC", "abc", true);
    expect_constant_case_equal<4>("ABCD", "abcd", true);
    expect_constant_case_equal<5>("ABCDE", "abcde", true);
    expect_constant_case_equal<6>("ABCDEF", "abcdef", true);
    expect_constant_case_equal<7>("ABCDEFG", "abcdefg", true);
    expect_constant_case_equal<8>("ABCDEFGH", "abcdefgh", true);

    expect_constant_case_equal<1>("A", "b", false);
    expect_constant_case_equal<4>("ABCD", "abce", false);
    expect_constant_case_equal<7>("ABCDEFG", "abcxefg", false);
}