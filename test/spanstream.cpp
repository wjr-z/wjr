#include <limits>

#include <gtest/gtest.h>

#include <wjr/spanstream.hpp>

#ifndef __cpp_lib_spanstream

using namespace wjr;

WJR_REGISTER_HAS_TYPE(input, (std::declval<std::istream &>() >> std::declval<T &>()), T);
WJR_REGISTER_HAS_TYPE(output, (std::declval<std::ostream &>() << std::declval<const T &>()), T);

template <typename T>
class BaseSpanStreamTest : public testing::Test {
public:
    static constexpr std::string_view test_string = "Hello, World! 123 45.67";

    static wjr::span<char> to_span(std::string_view str) {
        return wjr::span<char>(const_cast<char *>(str.data()), str.size());
    }

    static wjr::span<char> test_span() { return to_span(test_string); }

    static void constructor() {
        wjr::span sp(test_span());
        T ss(sp);
        EXPECT_TRUE(strcmp(ss.span().data(), sp.data()) == 0);
    }

    static void move_constructor() {
        wjr::span sp(test_span());
        T ss1(sp);
        T ss2(std::move(ss1));
        EXPECT_TRUE(strcmp(ss2.span().data(), sp.data()) == 0);
    }
};

template <typename T>
class InputSpanStreamTest : virtual public BaseSpanStreamTest<T> {
public:
    static void input() {
        T ss(BaseSpanStreamTest<T>::test_span());

        std::string s0, s1;
        int x;
        double d;

        ss >> s0 >> s1 >> x >> d;
        EXPECT_TRUE(s0 == "Hello,");
        EXPECT_TRUE(s1 == "World!");
        EXPECT_TRUE(x == 123);
        EXPECT_TRUE(d == 45.67);
    }
};

template <typename T>
class OutputSpanStreamTest : virtual public BaseSpanStreamTest<T> {
public:
    static void output() {
        char buf[32] = {};
        wjr::span sp(buf);
        T ss(sp);

        ss << "Hello, World! " << 123 << ' ' << 45.67 << '\0';
        EXPECT_TRUE(strcmp(buf, BaseSpanStreamTest<T>::test_span().data()) == 0);
    }
};

template <typename T>
class InputOutputSpanStreamTest : public InputSpanStreamTest<T>, public OutputSpanStreamTest<T> {};

using BaseTypes = ::testing::Types<ispanstream, ospanstream, spanstream>;
using InputTypes = ::testing::Types<ispanstream, spanstream>;
using OutputTypes = ::testing::Types<ospanstream, spanstream>;

TYPED_TEST_SUITE(BaseSpanStreamTest, BaseTypes);
TYPED_TEST_SUITE(InputSpanStreamTest, InputTypes);
TYPED_TEST_SUITE(OutputSpanStreamTest, OutputTypes);

TYPED_TEST(BaseSpanStreamTest, constructor) { this->constructor(); }

TYPED_TEST(BaseSpanStreamTest, move_constructor) { this->move_constructor(); }

TYPED_TEST(InputSpanStreamTest, input) { this->input(); }

TYPED_TEST(OutputSpanStreamTest, output) { this->output(); }

TEST(spanstream, spanbuf_seek) {
    char data[] = "abcdef";
    spanbuf buffer(span<char>(data, 6));

    EXPECT_EQ(buffer.pubseekpos(2, std::ios_base::in), spanbuf::pos_type(2));
    EXPECT_EQ(buffer.sgetc(), 'c');
    EXPECT_EQ(buffer.pubseekoff(1, std::ios_base::cur, std::ios_base::in), spanbuf::pos_type(3));
    EXPECT_EQ(buffer.sgetc(), 'd');
    EXPECT_EQ(buffer.pubseekoff(-1, std::ios_base::end, std::ios_base::in), spanbuf::pos_type(5));
    EXPECT_EQ(buffer.sgetc(), 'f');
    EXPECT_EQ(buffer.pubseekoff(-1, std::ios_base::beg, std::ios_base::in),
              spanbuf::pos_type(std::streamoff(-1)));
    EXPECT_EQ(buffer.pubseekoff(0, std::ios_base::cur, std::ios_base::in | std::ios_base::out),
              spanbuf::pos_type(std::streamoff(-1)));
}

TEST(spanstream, spanbuf_output_position_and_setbuf) {
    char data[6] = {};
    spanbuf buffer(span<char>(data, 6), std::ios_base::out);

    EXPECT_EQ(buffer.sputn("abc", 3), 3);
    EXPECT_EQ(buffer.span().size(), 3);
    EXPECT_EQ(buffer.pubseekoff(-1, std::ios_base::cur, std::ios_base::out), spanbuf::pos_type(2));
    EXPECT_EQ(buffer.sputc('X'), 'X');
    EXPECT_EQ(std::string(data, 3), "abX");

    char replacement[4] = {};
    EXPECT_EQ(buffer.pubsetbuf(replacement, 4), &buffer);
    EXPECT_EQ(buffer.sputn("xy", 2), 2);
    EXPECT_EQ(buffer.span().size(), 2);
    EXPECT_EQ(std::string(replacement, 2), "xy");

    char at_data[4] = {};
    spanbuf at_buffer(span<char>(at_data, 4), std::ios_base::out | std::ios_base::ate);
    EXPECT_EQ(at_buffer.span().size(), 4);
}

TEST(spanstream, end_seek_uses_buffer_mode) {
    using ios = std::ios_base;
    char data[] = "abcdef";
    spanbuf both(span<char>(data, 6), ios::in | ios::out);
    EXPECT_EQ(both.pubseekpos(1, ios::in), spanbuf::pos_type(1));
    EXPECT_EQ(both.sputn("AB", 2), 2);
    EXPECT_EQ(both.pubseekoff(-1, ios::end, ios::out), spanbuf::pos_type(5));
    EXPECT_EQ(both.sgetc(), 'B');
    EXPECT_EQ(both.sputc('X'), 'X');
    EXPECT_EQ(std::string_view(data, 6), "ABcdeX");

    spanbuf output(span<char>(data, 6), ios::out);
    EXPECT_EQ(output.sputn("pq", 2), 2);
    EXPECT_EQ(output.pubseekoff(-1, ios::end, ios::out), spanbuf::pos_type(1));
    EXPECT_EQ(output.span().size(), 1);
}

TEST(spanstream, failed_seek_does_not_update_either_sequence) {
    using ios = std::ios_base;
    const spanbuf::pos_type failure(spanbuf::off_type(-1));
    char data[] = "abcdef";
    spanbuf output(span<char>(data, 6), ios::out);
    EXPECT_EQ(output.sputn("AB", 2), 2);
    EXPECT_EQ(output.pubseekpos(3, ios::in | ios::out), failure);
    EXPECT_EQ(output.pubseekoff(0, ios::cur, ios::out), spanbuf::pos_type(2));
    EXPECT_EQ(output.pubseekoff(1, ios::end, ios::in), failure);

    spanbuf input(span<char>(data, 6), ios::in);
    EXPECT_EQ(input.pubseekpos(1, ios::in), spanbuf::pos_type(1));
    EXPECT_EQ(input.pubseekpos(3, ios::in | ios::out), failure);
    EXPECT_EQ(input.sgetc(), 'B');
    EXPECT_EQ(input.pubseekpos(0, ios::out), spanbuf::pos_type(0));
    EXPECT_EQ(input.sgetc(), 'B');

    spanbuf both(span<char>(data, 6));
    EXPECT_EQ(both.pubseekpos(2, ios::in | ios::out), spanbuf::pos_type(2));
    EXPECT_EQ(both.pubseekoff((std::numeric_limits<spanbuf::off_type>::max)(), ios::end), failure);
    EXPECT_EQ(both.pubseekoff((std::numeric_limits<spanbuf::off_type>::min)(), ios::end), failure);
    EXPECT_EQ(both.sgetc(), 'c');
    EXPECT_EQ(both.span().size(), 2);
}

TEST(spanstream, empty_buffer_can_seek_to_zero) {
    using ios = std::ios_base;
    const spanbuf::pos_type failure(spanbuf::off_type(-1));
    spanbuf empty;
    EXPECT_EQ(empty.pubseekpos(0), spanbuf::pos_type(0));
    EXPECT_EQ(empty.pubseekoff(0, ios::end), spanbuf::pos_type(0));
    EXPECT_EQ(empty.pubseekoff(1, ios::beg), failure);
    EXPECT_EQ(empty.pubseekoff(0, ios::cur), failure);
    EXPECT_TRUE(empty.span().empty());
}

#endif