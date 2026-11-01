#include <wjr/json/document.hpp>
#include <wjr/json/pmr.hpp>

#include <array>
#include <cstdint>
#include <limits>
#include <string_view>
#include <utility>

#include <gtest/gtest.h>

namespace {

template <typename T>
class json_equality : public testing::Test {};

using documents = testing::Types<wjr::json::document, wjr::json::pmr_document>;
TYPED_TEST_SUITE(json_equality, documents);

/** @brief Check symmetry and both inequality directions at the public DOM boundary. */
template <typename Document>
void expect_comparison(const Document &left, const Document &right, bool equal) {
    EXPECT_EQ(left == right, equal);
    EXPECT_EQ(right == left, equal);
    EXPECT_EQ(left != right, !equal);
    EXPECT_EQ(right != left, !equal);
}

template <typename Document>
Document parse(std::string_view input) {
    wjr::json::ondemand_reader reader;
    reader.read(input);
    auto result = Document::parse(reader);
    EXPECT_TRUE(result);
    return std::move(result).value();
}

TYPED_TEST(json_equality, boolean_values_are_compared) {
    expect_comparison(TypeParam(false), TypeParam(false), true);
    expect_comparison(TypeParam(true), TypeParam(true), true);
    expect_comparison(TypeParam(false), TypeParam(true), false);
    expect_comparison(TypeParam(false), TypeParam(nullptr), false);
    expect_comparison(TypeParam(true), TypeParam(1), false);
}

TYPED_TEST(json_equality, signed_unsigned_boundaries_are_not_modular) {
    constexpr auto signed_min = std::numeric_limits<std::int64_t>::min();
    constexpr auto signed_max = std::numeric_limits<std::int64_t>::max();
    constexpr auto unsigned_max = std::numeric_limits<std::uint64_t>::max();
    expect_comparison(TypeParam(std::int64_t(-1)), TypeParam(unsigned_max), false);
    expect_comparison(TypeParam(signed_min), TypeParam(std::uint64_t(1) << 63), false);
    expect_comparison(TypeParam(signed_max), TypeParam(std::uint64_t(signed_max)), true);
    expect_comparison(TypeParam(signed_max), TypeParam(std::uint64_t(signed_max) + 1), false);
    expect_comparison(TypeParam(std::int64_t(0)), TypeParam(std::uint64_t(0)), true);
    expect_comparison(TypeParam(std::int64_t(1)), TypeParam(std::uint64_t(1)), true);
    expect_comparison(TypeParam(std::int64_t(-1)), TypeParam(std::uint64_t(0)), false);
}

TYPED_TEST(json_equality, same_integer_kind_compares_full_payload) {
    constexpr std::array<std::int64_t, 5> signed_values{std::numeric_limits<std::int64_t>::min(),
                                                        -1, 0, 1,
                                                        std::numeric_limits<std::int64_t>::max()};
    for (const auto left : signed_values) {
        for (const auto right : signed_values) {
            expect_comparison(TypeParam(left), TypeParam(right), left == right);
        }
    }
    constexpr std::array<std::uint64_t, 4> unsigned_values{
        0, 1, std::uint64_t(1) << 63, std::numeric_limits<std::uint64_t>::max()};
    for (const auto left : unsigned_values) {
        for (const auto right : unsigned_values) {
            expect_comparison(TypeParam(left), TypeParam(right), left == right);
        }
    }
}

TYPED_TEST(json_equality, recursive_array_and_object_comparisons_use_scalar_values) {
    const auto source = parse<TypeParam>(R"({"flags":[false,true],"id":-1})");
    auto copy = source;
    expect_comparison(source, copy, true);
    copy.at("flags")[0] = true;
    expect_comparison(source, copy, false);
    copy = source;
    copy.at("id") = std::numeric_limits<std::uint64_t>::max();
    expect_comparison(source, copy, false);
    const auto reordered = parse<TypeParam>(R"({"id":-1,"flags":[false,true]})");
    expect_comparison(source, reordered, true);
}

TYPED_TEST(json_equality, other_tags_and_existing_numeric_cross_kind_behavior) {
    expect_comparison(TypeParam(nullptr), TypeParam(nullptr), true);
    expect_comparison(TypeParam(7.0), TypeParam(std::int64_t(7)), true);
    expect_comparison(TypeParam(7.0), TypeParam(std::uint64_t(7)), true);
    expect_comparison(TypeParam(7.0), TypeParam(8.0), false);
    const auto string = TypeParam::_construct_string("same");
    expect_comparison(string, TypeParam::_construct_string("same"), true);
    expect_comparison(string, TypeParam::_construct_string("different"), false);
    expect_comparison(string, TypeParam(true), false);
    expect_comparison(parse<TypeParam>("[]"), parse<TypeParam>("{}"), false);
    expect_comparison(parse<TypeParam>("[true]"), parse<TypeParam>("[]"), false);
}

} // namespace
