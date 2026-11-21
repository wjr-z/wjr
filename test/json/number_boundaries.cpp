#include <array>
#include <charconv>
#include <cstdint>
#include <limits>
#include <string>

#include <gtest/gtest.h>

#include <wjr/json.hpp>

TEST(json, unsigned_integer_full_width) {
    constexpr std::array<uint64_t, 8> values = {
        9999999999999999999ULL,  10000000000000000000ULL,
        10000000000000000001ULL, 12345678901234567890ULL,
        18446744073709551609ULL, 18446744073709551610ULL,
        18446744073709551614ULL, std::numeric_limits<uint64_t>::max()};
    for (const auto expected : values) {
        const auto text = std::to_string(expected);
        SCOPED_TRACE(text);
        wjr::json::basic_value value(wjr::default_construct);
        const auto converted = wjr::json::number_detail::from_chars_json(
            text.data(), text.data() + text.size(), value);
        ASSERT_TRUE(converted);
        EXPECT_EQ(converted.ptr, text.data() + text.size());
        EXPECT_EQ(value.m_type, wjr::json::value_t::number_unsigned);
        EXPECT_EQ(value.m_number_unsigned, expected);

        wjr::json::ondemand_reader reader;
        reader.read(text);
        auto typed = wjr::json::parse<uint64_t>(reader);
        ASSERT_TRUE(typed);
        EXPECT_EQ(*typed, expected);
        reader.read(text);
        auto document = wjr::json::document::parse(reader);
        ASSERT_TRUE(document);
        EXPECT_EQ(document->get<wjr::json::number_unsigned_t>(), expected);

        const auto array_text = '[' + text + ']';
        reader.read(array_text);
        document = wjr::json::document::parse(reader);
        ASSERT_TRUE(document);
        EXPECT_EQ(document->get<wjr::json::array_t>()[0].get<wjr::json::number_unsigned_t>(),
                  expected);
    }
    wjr::json::ondemand_reader reader;
    reader.read("18446744073709551616");
    auto overflow = wjr::json::parse<uint64_t>(reader);
    ASSERT_FALSE(overflow);
    EXPECT_EQ(overflow.error().code, wjr::json::error_code::NUMBER_OUT_OF_RANGE);
}
