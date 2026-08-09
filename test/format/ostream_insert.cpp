#include "../detail.hpp"

#include <sstream>

#include <wjr/format/ostream_insert.hpp>

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
