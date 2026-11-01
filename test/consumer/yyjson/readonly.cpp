#include <wjr/json/yyjson.hpp>

#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace json = wjr::json::yyjson;
namespace {

template <typename T>
concept has_root = requires(T &&value) { std::forward<T>(value).root(); };
template <typename T>
concept has_native_handle = requires(T &&value) { std::forward<T>(value).native_handle(); };

static_assert(sizeof(json::document) == sizeof(void *));
static_assert(sizeof(json::value_view) == sizeof(void *));
static_assert(sizeof(json::array_view) == sizeof(void *));
static_assert(sizeof(json::object_view) == sizeof(void *));
static_assert(sizeof(json::array_view::iterator) == sizeof(yyjson_arr_iter));
static_assert(sizeof(json::object_view::iterator) == sizeof(yyjson_obj_iter));
static_assert(std::is_nothrow_move_constructible_v<json::document>);
static_assert(!std::is_copy_constructible_v<json::document>);
static_assert(std::is_trivially_copyable_v<json::value_view>);
static_assert(std::ranges::forward_range<json::array_view>);
static_assert(std::ranges::borrowed_range<json::array_view>);
static_assert(std::ranges::forward_range<json::object_view>);
static_assert(std::ranges::borrowed_range<json::object_view>);
static_assert(has_root<json::document &> && has_root<const json::document &>);
static_assert(!has_root<json::document> && !has_root<const json::document>);
static_assert(!has_native_handle<json::document> && !has_native_handle<const json::document>);
static_assert(has_native_handle<json::value_view>);

TEST(yyjson_readonly, empty_owner_and_missing_value_are_distinct_from_json_null) {
    EXPECT_EQ(yyjson_version(), YYJSON_VERSION_HEX);
    json::document empty;
    EXPECT_FALSE(empty);
    EXPECT_FALSE(empty.root());
    EXPECT_FALSE(empty.root().is_null());
    auto null = json::document::parse("null");
    ASSERT_TRUE(null);
    EXPECT_TRUE(null.root());
    EXPECT_TRUE(null.root().is_null());
}

TEST(yyjson_readonly, parse_errors_preserve_native_codes_offsets_and_messages) {
    std::vector<std::string> inputs{"",         "[1,]",        "{\"a\":}",   "true false",
                                    "1e100000", "\"\\uD800\"", "\"\\uDC00\""};
    inputs.push_back(std::string("\"") + char(0xff) + "\"");
    for (const auto &input : inputs) {
        yyjson_read_err reference{}, actual{};
        auto *raw = yyjson_read_opts(const_cast<char *>(input.data()), input.size(),
                                     YYJSON_READ_NOFLAG, nullptr, &reference);
        ASSERT_EQ(raw, nullptr);
        const auto document = json::document::parse(input, &actual);
        EXPECT_FALSE(document);
        EXPECT_EQ(actual.code, reference.code);
        EXPECT_EQ(actual.pos, reference.pos);
        EXPECT_STREQ(actual.msg, reference.msg);
    }
}

TEST(yyjson_readonly, checked_getters_leave_outputs_unchanged_and_follow_native_tags) {
    auto document =
        json::document::parse("[false,18446744073709551615,-9223372036854775808,-0.0,\"x\",null]");
    ASSERT_TRUE(document);
    const auto array = document.root().get_array_unchecked();
    bool boolean = true;
    uint64_t unsigned_value = 7;
    int64_t signed_value = 8;
    double number = 9;
    std::string_view string = "unchanged";
    EXPECT_FALSE(array.at(5).try_get_bool(boolean));
    EXPECT_TRUE(boolean);
    EXPECT_FALSE(array.at(2).try_get_uint(unsigned_value));
    EXPECT_EQ(unsigned_value, 7);
    EXPECT_FALSE(array.at(1).try_get_sint(signed_value));
    EXPECT_EQ(signed_value, 8);
    EXPECT_FALSE(array.at(4).try_get_number(number));
    EXPECT_EQ(number, 9);
    EXPECT_FALSE(array.at(0).try_get_string(string));
    EXPECT_EQ(string, "unchanged");
    ASSERT_TRUE(array.at(0).try_get_bool(boolean));
    EXPECT_FALSE(boolean);
    EXPECT_FALSE(array.at(0).get_bool_unchecked());
    ASSERT_TRUE(array.at(1).try_get_uint(unsigned_value));
    EXPECT_EQ(unsigned_value, std::numeric_limits<uint64_t>::max());
    EXPECT_EQ(array.at(1).get_uint_unchecked(), unsigned_value);
    ASSERT_TRUE(array.at(2).try_get_sint(signed_value));
    EXPECT_EQ(signed_value, std::numeric_limits<int64_t>::min());
    EXPECT_EQ(array.at(2).get_sint_unchecked(), signed_value);
    for (size_t index : {1u, 2u, 3u}) {
        ASSERT_TRUE(array.at(index).try_get_number(number));
        EXPECT_EQ(std::bit_cast<uint64_t>(number),
                  std::bit_cast<uint64_t>(yyjson_get_num(array.at(index).native_handle())));
    }
    EXPECT_TRUE(std::signbit(array.at(3).get_number_unchecked()));
    ASSERT_TRUE(array.at(4).try_get_string(string));
    EXPECT_EQ(string, "x");
    EXPECT_EQ(array.at(4).get_string_unchecked(), string);
}

TEST(yyjson_readonly, duplicate_lookup_is_explicit_and_iteration_keeps_decoded_input_order) {
    auto document =
        json::document::parse(R"({"z":0,"\u0061":"first","a":"last","":"empty","x\u0000z":5})");
    ASSERT_TRUE(document);
    const auto object = document.root().get_object_unchecked();
    EXPECT_EQ(object.size(), 5);
    EXPECT_EQ(object.find_first("a").get_string_unchecked(), "first");
    EXPECT_EQ(object.find_last("a").get_string_unchecked(), "last");
    EXPECT_EQ(object.find_first({}).get_string_unchecked(), "empty");
    EXPECT_EQ(object.find_last({}).get_string_unchecked(), "empty");
    EXPECT_EQ(object.find_first(std::string_view("x\0z", 3)).get_uint_unchecked(), 5);
    EXPECT_FALSE(object.find_first("missing"));
    EXPECT_FALSE(object.find_last("missing"));
    std::vector<std::string_view> keys;
    for (auto member : object)
        keys.push_back(member.key);
    EXPECT_EQ(keys, (std::vector<std::string_view>{"z", "a", "a", "", {"x\0z", 3}}));
}

TEST(yyjson_readonly, iterators_are_multipass_and_temporary_ranges_borrow_the_owner) {
    auto document = json::document::parse(R"({"a":[1,{"nested":[2,3]},4]})");
    ASSERT_TRUE(document);
    const auto object = document.root().get_object_unchecked();
    auto member = *object.begin();
    const auto array = member.value.get_array_unchecked();
    auto first = array.begin(), copy = first;
    EXPECT_EQ((*first).get_uint_unchecked(), 1);
    ++first;
    EXPECT_EQ((*copy).get_uint_unchecked(), 1);
    EXPECT_TRUE((*first).is_object());
    EXPECT_EQ(array.at(2).get_uint_unchecked(), 4);
    EXPECT_FALSE(array.at(3));
    EXPECT_FALSE(array.at(std::numeric_limits<size_t>::max()));
    const auto found =
        std::ranges::find_if(member.value.get_array_unchecked(),
                             [](json::value_view value) { return value.is_object(); });
    EXPECT_TRUE((*found).is_object());
    EXPECT_EQ(std::ranges::distance(array), 3);
    EXPECT_EQ(std::ranges::distance(object), 1);
    EXPECT_EQ(member.key, "a");
}

TEST(yyjson_readonly, failed_container_extraction_preserves_an_existing_view) {
    auto document = json::document::parse("[[1],{\"a\":2}]");
    ASSERT_TRUE(document);
    const auto root = document.root().get_array_unchecked();
    auto array = root.at(0).get_array_unchecked();
    auto object = root.at(1).get_object_unchecked();
    EXPECT_FALSE(root.at(1).try_get_array(array));
    EXPECT_EQ(array.size(), 1);
    EXPECT_FALSE(root.at(0).try_get_object(object));
    EXPECT_EQ(object.size(), 1);
    EXPECT_TRUE(root.at(0).try_get_array(array));
    EXPECT_TRUE(root.at(1).try_get_object(object));
    EXPECT_EQ(std::ranges::distance(json::array_view{}), 0);
    EXPECT_EQ(std::ranges::distance(json::object_view{}), 0);
}

TEST(yyjson_readonly, parsed_storage_outlives_input_and_owner_moves_transfer_existing_borrows) {
    json::document document;
    const std::string expected(4096, 'v');
    {
        std::string input = "{\"key\":\"" + expected + "\\u0000tail\"}";
        const auto unchanged = input;
        document = json::document::parse(input);
        ASSERT_TRUE(document);
        EXPECT_EQ(input, unchanged);
    }
    const auto value = document.root().get_object_unchecked().find_last("key");
    const auto text = value.get_string_unchecked();
    EXPECT_EQ(text.substr(0, expected.size()), expected);
    EXPECT_EQ(text.substr(expected.size()), std::string_view("\0tail", 5));
    auto moved = std::move(document);
    EXPECT_FALSE(document);
    EXPECT_EQ(value.get_string_unchecked(), text);
    auto replaced = json::document::parse("[0]");
    replaced = std::move(moved);
    EXPECT_FALSE(moved);
    EXPECT_EQ(value.get_string_unchecked(), text);
    auto &alias = replaced;
    replaced = std::move(alias);
    EXPECT_EQ(value.get_string_unchecked(), text);
    replaced.reset();
    EXPECT_FALSE(replaced);
}

} // namespace
