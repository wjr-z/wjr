#include <gtest/gtest.h>

#include <charconv>
#include <deque>
#include <limits>
#include <list>
#include <string>

#include <wjr/format/charconv.hpp>

namespace {

template <typename T>
void expect_minimum_magnitude() {
    constexpr T value = std::numeric_limits<T>::min();
    for (const int base : {2, 4, 8, 10, 16, 32}) {
        SCOPED_TRACE(base);
        char reference[80];
        const auto expected_end =
            std::to_chars(std::begin(reference), std::end(reference), value, base);
        ASSERT_EQ(expected_end.ec, std::errc{});
        const std::string expected(reference, expected_end.ptr);

        char buffer[80];
        const auto checked = wjr::to_chars(std::begin(buffer), std::end(buffer), value, base);
        ASSERT_EQ(checked.ec, std::errc{});
        EXPECT_EQ(std::string(buffer, checked.ptr), expected);
        EXPECT_EQ(std::string(buffer, wjr::to_chars_unchecked(buffer, value, base)), expected);
        EXPECT_EQ(std::string(wjr::to_chars_backward_unchecked(std::end(buffer), value, base),
                              std::end(buffer)),
                  expected);

        std::list<char> sequential(expected.size(), 'x');
        const auto forward = wjr::to_chars(sequential.begin(), sequential.end(), value, base);
        ASSERT_EQ(forward.ec, std::errc{});
        EXPECT_EQ(std::string(sequential.begin(), forward.ptr), expected);

        std::deque<char> segmented(expected.size(), 'x');
        const auto random = wjr::to_chars(segmented.begin(), segmented.end(), value, base);
        ASSERT_EQ(random.ec, std::errc{});
        EXPECT_EQ(std::string(segmented.begin(), random.ptr), expected);

        std::string inserted = ":";
        wjr::to_chars_unchecked(std::back_inserter(inserted), value, base);
        EXPECT_EQ(inserted, ":" + expected);
    }
}

} // namespace

TEST(charconv, signed_minimum_magnitude_is_representable_in_unsigned_arithmetic) {
    expect_minimum_magnitude<int8_t>();
    expect_minimum_magnitude<int16_t>();
    expect_minimum_magnitude<int32_t>();
    expect_minimum_magnitude<int64_t>();
}
