#include <gtest/gtest.h>

#include <sstream>

#include <wjr/biginteger.hpp>
#include <wjr/format/ostream_insert.hpp>
#include <wjr/json/document.hpp>

using namespace wjr;

namespace {
class failing_streambuf : public std::streambuf {
protected:
    int_type overflow(int_type) override { return traits_type::eof(); }

    std::streamsize xsputn(const char *, std::streamsize) override { return 0; }
};
} // namespace

TEST(ostream_insert, width_and_alignment) {
    std::ostringstream right;
    EXPECT_EQ(right.fill('.'), ' ');
    right.width(5);
    EXPECT_EQ(&_ostream_insert(right, "abc", 3), &right);
    EXPECT_EQ(right.str(), "..abc");
    EXPECT_EQ(right.width(), 0);

    std::ostringstream left;
    EXPECT_EQ(left.fill('.'), ' ');
    left.setf(std::ios_base::left, std::ios_base::adjustfield);
    left.width(5);
    _ostream_insert(left, "abc", 3);
    EXPECT_EQ(left.str(), "abc..");

    std::ostringstream exact;
    exact.width(2);
    _ostream_insert(exact, "abc", 3);
    EXPECT_EQ(exact.str(), "abc");
}

TEST(ostream_insert, failed_stream_sets_badbit) {
    failing_streambuf buffer;
    std::ostream output(&buffer);
    _ostream_insert(output, "abc", 3);
    EXPECT_TRUE(output.bad());

    failing_streambuf padded_buffer;
    std::ostream padded_output(&padded_buffer);
    padded_output.width(4);
    _ostream_insert(padded_output, "a", 1);
    EXPECT_TRUE(padded_output.bad());
}

TEST(ostream_insert, failed_sentry_does_not_write) {
    std::ostringstream output;
    output.setstate(std::ios_base::failbit);
    _ostream_insert(output, "abc", 3);

    EXPECT_TRUE(output.fail());
    EXPECT_TRUE(output.str().empty());
}

namespace {
struct custom_traits : std::char_traits<char> {};

#ifndef WJR_DISABLE_EXCEPTIONS
struct output_error {};

template <typename Traits>
class throwing_streambuf : public std::basic_streambuf<char, Traits> {
protected:
    typename Traits::int_type overflow(typename Traits::int_type) override { throw output_error{}; }

    std::streamsize xsputn(const char *, std::streamsize) override { throw output_error{}; }
};

template <typename Traits, typename Insert>
void check_output_exceptions(Insert insert) {
    for (const auto mask :
         {std::ios_base::goodbit, std::ios_base::badbit, std::ios_base::failbit}) {
        throwing_streambuf<Traits> buffer;
        std::basic_ostream<char, Traits> output(&buffer);
        output.exceptions(mask);
        if (mask & std::ios_base::badbit) {
            EXPECT_THROW(insert(output), output_error);
        } else {
            EXPECT_NO_THROW(insert(output));
        }
        EXPECT_TRUE(output.bad());
        EXPECT_EQ(output.exceptions(), mask);
    }
}

template <typename Traits>
void check_public_inserters() {
    check_output_exceptions<Traits>([](auto &output) { _ostream_insert(output, "abc", 3); });
    check_output_exceptions<Traits>([](auto &output) {
        output.width(5);
        _ostream_insert(output, "abc", 3);
    });
    const biginteger integer(123);
    check_output_exceptions<Traits>([&](auto &output) { output << integer; });
    check_output_exceptions<Traits>([&](auto &output) {
        output.width(5);
        output << integer;
    });
    json::document document;
    document = 123;
    check_output_exceptions<Traits>([&](auto &output) { output << document; });
}
#endif
} // namespace

#ifndef WJR_DISABLE_EXCEPTIONS
TEST(ostream_insert, streambuf_exceptions_follow_mask) {
    check_public_inserters<std::char_traits<char>>();
    check_public_inserters<custom_traits>();
}

TEST(ostream_insert, short_write_exceptions_follow_mask) {
    const biginteger integer(123);
    json::document document;
    document = 123;
    for (int entry = 0; entry != 3; ++entry) {
        for (const bool padded : {false, true}) {
            failing_streambuf buffer;
            std::ostream output(&buffer);
            output.exceptions(std::ios_base::badbit);
            output.width(padded ? 5 : 0);
            EXPECT_THROW(
                {
                    if (entry == 0) {
                        _ostream_insert(output, "abc", 3);
                    } else if (entry == 1) {
                        output << integer;
                    } else {
                        output << document;
                    }
                },
                std::ios_base::failure);
            EXPECT_TRUE(output.bad());
        }
    }
}
#endif

TEST(ostream_insert, public_inserters_preserve_failed_sentry) {
    const biginteger integer(123);
    json::document document;
    document = 123;
    std::ostringstream output;
    output.width(5);
    output.exceptions(std::ios_base::badbit);
    output.setstate(std::ios_base::failbit);
    output << integer << document;
    EXPECT_EQ(output.rdstate(), std::ios_base::failbit);
    EXPECT_EQ(output.width(), 5);
    EXPECT_TRUE(output.str().empty());
}

TEST(ostream_insert, wide_stream_sentry) {
    std::wostringstream output;
    output.width(5);
    output.fill(L'.');
    _ostream_insert(output, L"abc", 3);
    EXPECT_EQ(output.str(), L"..abc");
    EXPECT_EQ(output.width(), 0);
}

TEST(ostream_insert, public_inserters_reset_width) {
    const biginteger integer(123);
    json::document document;
    document = 123;
    std::ostringstream output;
    output.width(5);
    output.fill('.');
    output << integer;
    EXPECT_EQ(output.str(), "..123");
    EXPECT_EQ(output.width(), 0);
    output.setf(std::ios_base::left, std::ios_base::adjustfield);
    output.width(5);
    output << integer;
    EXPECT_EQ(output.str(), "..123123..");
    EXPECT_EQ(output.width(), 0);
    output.width(2);
    output << document;
    EXPECT_EQ(output.str(), "..123123..123");
    EXPECT_EQ(output.width(), 0);
}
