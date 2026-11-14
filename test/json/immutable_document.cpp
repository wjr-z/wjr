#include <gtest/gtest.h>

#include <cmath>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

#include <wjr/json/document.hpp>
#include <wjr/json/immutable_document.hpp>

namespace {
using namespace wjr::json;

static_assert(std::forward_iterator<array_view::iterator>);
static_assert(std::forward_iterator<object_view::iterator>);
static_assert(std::is_trivially_copyable_v<value_view>);
static_assert(!std::is_copy_constructible_v<immutable_document>);
static_assert(std::is_nothrow_move_constructible_v<immutable_document>);
template <typename T>
concept temporary_root = requires(T &&owner) { std::move(owner).root(); };
static_assert(!temporary_root<immutable_document>);

auto parse_snapshot(std::string_view input) {
    ondemand_reader reader;
    reader.read(input);
    return immutable_document::parse(reader);
}

document materialize(value_view value) {
    switch (value.type()) {
    case value_t::null:
        return document{};
    case value_t::boolean:
        return document(*value.get_boolean());
    case value_t::number_unsigned:
        return document(*value.get_unsigned());
    case value_t::number_signed:
        return document(*value.get_signed());
    case value_t::number_float:
        return document(*value.get_float());
    case value_t::string:
        return document(std::string(*value.get_string()));
    case value_t::array: {
        auto output = document::_construct_array();
        const auto array = value.get_array();
        for (auto child : *array)
            output.get<array_t>().push_back(materialize(child));
        return output;
    }
    case value_t::object: {
        auto output = document::_construct_object();
        const auto object = value.get_object();
        for (auto field : *object)
            output.get<object_t>().emplace(std::string(field.key), materialize(field.value));
        return output;
    }
    }
    std::terminate();
}

TEST(json_immutable, complete_tree_matches_mutable_semantics) {
    const char *inputs[] = {
        "null",
        "true",
        "false",
        "0",
        "-1",
        "1.25",
        "\"\"",
        "[]",
        "{}",
        R"({"z":[null,true,-1,1,1.5,"hello"],"a":{"x":1},"a":{"y":2},"empty":""})",
        R"({"\u0061":0,"a":[{"nested":[]},{}],"b":{"empty":"","\u0000":"\u0000"}})",
    };
    for (const auto *input : inputs) {
        SCOPED_TRACE(input);
        auto snapshot = parse_snapshot(input);
        ASSERT_TRUE(snapshot);
        ondemand_reader reader;
        reader.read(input);
        auto reference = document::parse(reader);
        ASSERT_TRUE(reference);
        EXPECT_EQ(materialize(snapshot->root()), *reference);
    }
}

TEST(json_immutable, input_and_reader_lifetime_are_independent) {
    auto snapshot = [] {
        std::string input = R"({"name":"hello\n\u4e2d","items":[1,{"x":2},3]})";
        ondemand_reader reader;
        reader.read(input);
        auto result = immutable_document::parse(reader);
        input.assign(input.size(), 'x');
        reader.read("null");
        return result;
    }();
    ASSERT_TRUE(snapshot);
    auto object = snapshot->root().get_object();
    ASSERT_TRUE(object);
    auto name = object->find("name");
    ASSERT_TRUE(name);
    EXPECT_EQ(*name->get_string(), "hello\n\xe4\xb8\xad");
    auto moved = std::move(*snapshot);
    EXPECT_EQ(snapshot->storage_size(), 0);
    EXPECT_EQ(*moved.root().get_object()->find("name")->get_string(), "hello\n\xe4\xb8\xad");
}

TEST(json_immutable, arrays_skip_nested_subtrees_and_iterators_are_independent) {
    auto snapshot = parse_snapshot(R"([1,{"a":[2,3]},[],4])");
    ASSERT_TRUE(snapshot);
    auto array = snapshot->root().get_array();
    ASSERT_TRUE(array);
    ASSERT_EQ(array->size(), 4);
    auto first = array->begin();
    auto second = first;
    ++second;
    EXPECT_EQ(*(*first).get_unsigned(), 1);
    EXPECT_EQ((*second).type(), value_t::object);
    EXPECT_EQ(*array->at(3)->get_unsigned(), 4);
    EXPECT_EQ(array->at(4).error(), error_code::INDEX_OUT_OF_BOUNDS);
    auto third = second++;
    EXPECT_EQ((*third).type(), value_t::object);
    EXPECT_EQ((*second).get_array()->size(), 0);
    EXPECT_EQ(std::distance(array->begin(), array->end()), 4);
}

TEST(json_immutable, objects_have_sorted_unique_decoded_keys_and_checked_access) {
    auto snapshot = parse_snapshot(R"({"z":true,"\u0061":1,"a":2,"b\u0000":false,"":null})");
    ASSERT_TRUE(snapshot);
    auto object = snapshot->root().get_object();
    ASSERT_TRUE(object);
    EXPECT_EQ(object->size(), 4);
    std::vector<std::string> keys;
    for (auto field : *object)
        keys.emplace_back(field.key);
    EXPECT_EQ(keys, (std::vector<std::string>{"", "a", std::string("b\0", 2), "z"}));
    EXPECT_EQ(*object->find("a")->get_unsigned(), 2);
    EXPECT_FALSE(*object->find(std::string_view("b\0", 2))->get_boolean());
    EXPECT_EQ(object->find("missing").error(), error_code::NO_SUCH_FIELD);
    EXPECT_EQ(object->find("a")->get_string().error(), error_code::INCORRECT_TYPE);
    EXPECT_EQ(snapshot->root().get_array().error(), error_code::INCORRECT_TYPE);
}

TEST(json_immutable, duplicates_do_not_retain_superseded_nodes_or_bytes) {
    const auto minimal = parse_snapshot(R"({"x":"last"})");
    ASSERT_TRUE(minimal);
    std::string input = "{";
    for (unsigned int i = 0; i != 40; ++i) {
        input += "\"x\":{\"discard\":[\"" + std::string(4096, 'x') + "\",0]},";
    }
    input += R"("x":"last"})";
    auto snapshot = parse_snapshot(input);
    ASSERT_TRUE(snapshot);
    EXPECT_EQ(snapshot->storage_size(), minimal->storage_size());
    EXPECT_EQ(snapshot->storage_size(), 2 * 16 + 12 + 1 + 4);
    EXPECT_EQ(*snapshot->root().get_object()->find("x")->get_string(), "last");
}

TEST(json_immutable, scalar_access_is_exact_and_preserves_numeric_bits) {
    auto values = parse_snapshot("[-9223372036854775808,18446744073709551615,-0.0,0]");
    ASSERT_TRUE(values);
    auto array = values->root().get_array();
    ASSERT_TRUE(array);
    EXPECT_EQ(*array->at(0)->get_signed(), std::numeric_limits<int64_t>::min());
    EXPECT_EQ(*array->at(1)->get_unsigned(), std::numeric_limits<uint64_t>::max());
    EXPECT_TRUE(std::signbit(*array->at(2)->get_float()));
    EXPECT_EQ(array->at(3)->get_signed().error(), error_code::INCORRECT_TYPE);
    EXPECT_EQ(array->at(3)->get_float().error(), error_code::INCORRECT_TYPE);
}

TEST(json_immutable, deep_objects_grow_scratch_without_dangling_offsets) {
    std::string input;
    for (unsigned int i = 0; i != 100; ++i)
        input += "{\"id\":" + std::to_string(i) + ",\"child\":";
    input += "null";
    input += std::string(100, '}');
    auto snapshot = parse_snapshot(input);
    ASSERT_TRUE(snapshot);
    auto value = snapshot->root();
    for (unsigned int i = 0; i != 100; ++i) {
        auto object = value.get_object();
        ASSERT_TRUE(object);
        EXPECT_EQ(*object->find("id")->get_unsigned(), i);
        value = *object->find("child");
    }
    EXPECT_EQ(value.type(), value_t::null);
    const auto too_deep = std::string(300, '[') + std::string(300, ']');
    auto rejected = parse_snapshot(too_deep);
    ASSERT_FALSE(rejected);
    EXPECT_EQ(rejected.error().code, error_code::DEPTH_ERROR);
}

TEST(json_immutable, malformed_input_and_overwritten_values_are_checked) {
    const std::string invalid[] = {
        "",
        "[0,]",
        "{\"x\":}",
        "\"ok\"junk",
        "true\vgarbage",
        "1\fgarbage",
        "{\"x\"junk:0}",
        "\"raw\nnewline\"",
        "\"\xff\"",
        "\"\\uD800\"",
        R"({"x":"\uD800","x":0})",
        R"({"x":1e9999,"x":0})",
        "[1] null",
    };
    for (const auto &input : invalid) {
        SCOPED_TRACE(input);
        EXPECT_FALSE(parse_snapshot(input));
    }
    auto unicode = parse_snapshot("\"\xff\"");
    ASSERT_FALSE(unicode);
    EXPECT_EQ(unicode.error(), (parse_error{error_code::UTF8_ERROR, 1}));
    auto boundary = parse_snapshot("\"ok\"junk");
    ASSERT_FALSE(boundary);
    EXPECT_EQ(boundary.error(), (parse_error{error_code::TAPE_ERROR, 4}));
    EXPECT_TRUE(parse_snapshot("[1,true,null,\"\\u0000\\n\"] \t\r\n"));
}
} // namespace
