#include "../detail.hpp"

#include <wjr/format/ctype.hpp>

using namespace wjr;

TEST(ctype, classification) {
    struct Case {
        char value;
        bool digit;
        bool hex_digit;
        bool lower;
        bool upper;
        bool alpha;
        bool alnum;
        bool ascii;
        bool space;
    };

    constexpr Case cases[] = {
        {'0', true, true, false, false, false, true, true, false},
        {'9', true, true, false, false, false, true, true, false},
        {'a', false, true, true, false, true, true, true, false},
        {'f', false, true, true, false, true, true, true, false},
        {'A', false, true, false, true, true, true, true, false},
        {'F', false, true, false, true, true, true, true, false},
        {'z', false, false, true, false, true, true, true, false},
        {'Z', false, false, false, true, true, true, true, false},
        {' ', false, false, false, false, false, false, true, true},
        {'/', false, false, false, false, false, false, true, false},
        {static_cast<char>(0x80), false, false, false, false, false, false, false, false},
    };

    for (const auto &test_case : cases) {
        EXPECT_EQ(isDigit(test_case.value), test_case.digit);
        EXPECT_EQ(isHexDigit(test_case.value), test_case.hex_digit);
        EXPECT_EQ(isLower(test_case.value), test_case.lower);
        EXPECT_EQ(isUpper(test_case.value), test_case.upper);
        EXPECT_EQ(isAlpha(test_case.value), test_case.alpha);
        EXPECT_EQ(isAlnum(test_case.value), test_case.alnum);
        EXPECT_EQ(isAscii(test_case.value), test_case.ascii);
        EXPECT_EQ(isSpace(test_case.value), test_case.space);
    }

    EXPECT_TRUE(isSpace('\t'));
    EXPECT_TRUE(isSpace('\n'));
    EXPECT_FALSE(isHexDigit(':'));
}

TEST(ctype, case_conversion) {
    EXPECT_EQ(toLower('A'), 'a');
    EXPECT_EQ(toLower('Z'), 'z');
    EXPECT_EQ(toLower('!'), '!');
    EXPECT_EQ(toUpper('a'), 'A');
    EXPECT_EQ(toUpper('z'), 'Z');
    EXPECT_EQ(toUpper('!'), '!');

    static_assert(isDigit('7'));
    static_assert(isHexDigit('e'));
    static_assert(isAlpha('Q'));
    static_assert(isAlnum('3'));
    static_assert(isAscii('\0'));
    static_assert(isSpace(' '));
}
