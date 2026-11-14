#include <gtest/gtest.h>

#include <array>

#include <wjr/expected.hpp>

namespace {
using plain_source = wjr::expected<int, int>;
using compact_source = wjr::compressed_expected<int, int, -1>;
using plain_destination = wjr::expected<bool, long>;
using compact_destination = wjr::compressed_expected<bool, long, -1>;

static_assert(std::is_convertible_v<plain_source &, plain_destination>);
static_assert(std::is_convertible_v<const plain_source &, plain_destination>);
static_assert(std::is_convertible_v<plain_source &&, plain_destination>);
static_assert(std::is_convertible_v<const plain_source &&, plain_destination>);
static_assert(!std::is_constructible_v<plain_destination, wjr::expected<void, int> &>);
static_assert(!std::is_constructible_v<wjr::expected<const bool, int>, wjr::expected<void, int> &>);

template <typename Source, typename Destination>
void check_boolean_conversions() {
    const auto convert_all = [](Source &source) {
        return std::array<Destination, 4>{Destination(source), Destination(std::as_const(source)),
                                          Destination(std::move(source)),
                                          Destination(std::move(std::as_const(source)))};
    };
    for (const int payload : {0, 3}) {
        Source source(payload);
        for (const auto &converted : convert_all(source)) {
            ASSERT_TRUE(converted);
            EXPECT_EQ(*converted, static_cast<bool>(payload));
        }
        ASSERT_TRUE(source);
        EXPECT_EQ(*source, payload);
    }
    Source source(wjr::unexpect, 7);
    for (const auto &converted : convert_all(source)) {
        ASSERT_FALSE(converted);
        EXPECT_EQ(converted.error(), 7);
    }
    ASSERT_FALSE(source);
    EXPECT_EQ(source.error(), 7);
}

TEST(expected_bool_conversion, ordinary_results_preserve_payload_and_error_for_all_cvrefs) {
    check_boolean_conversions<plain_source, plain_destination>();
}

TEST(expected_bool_conversion, compressed_results_preserve_payload_and_error_for_all_cvrefs) {
    check_boolean_conversions<compact_source, plain_destination>();
    check_boolean_conversions<plain_source, compact_destination>();
    check_boolean_conversions<compact_source, compact_destination>();
}

struct result_payload {
    bool has_value;
    explicit result_payload(const plain_source &source) noexcept : has_value(source.has_value()) {}
};

TEST(expected_bool_conversion, nonboolean_payload_can_capture_entire_result) {
    plain_source source(wjr::unexpect, 7);
    wjr::expected<result_payload, int> converted(source);
    ASSERT_TRUE(converted);
    EXPECT_FALSE(converted->has_value);
    const wjr::expected<result_payload, int> const_converted(std::as_const(source));
    ASSERT_TRUE(const_converted);
    EXPECT_FALSE(const_converted->has_value);
}
} // namespace
