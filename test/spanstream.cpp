#include "detail.hpp"

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

#endif