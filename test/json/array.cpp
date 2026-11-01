#include <wjr/json/array.hpp>
#include <wjr/json/optional.hpp>
#include <wjr/json/pmr.hpp>

#include <array>
#include <cstddef>
#include <map>
#include <memory>
#include <memory_resource>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace json_array_test {

using document = wjr::json::document;

struct point {
    std::array<double, 2> coordinates;
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(point, coordinates)
};

struct label_view {
    std::array<std::string_view, 2> labels;
    WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER(label_view, labels)
};

struct number {
    int value;
    explicit number(int input) : value(input) {}
};

struct immovable {
    int value;
    explicit immovable(int input) : value(input) {}
    immovable(const immovable &) = delete;
    immovable(immovable &&) = delete;
};

struct unsupported {};

struct lifetime_value {
    static inline int live = 0;
    int value;
    explicit lifetime_value(int input) : value(input) { ++live; }
    ~lifetime_value() { --live; }
    lifetime_value(const lifetime_value &) = delete;
    lifetime_value(lifetime_value &&) = delete;
};

} // namespace json_array_test

namespace wjr::json {

template <>
struct document_serializer_impl<json_array_test::number> {
    using value_type = json_array_test::number;
    static value_type construct(const document &doc, in_place_document_serializer_from_t) {
        return value_type(value_to<int>(doc));
    }
    static void assign(const document &doc, value_type &value,
                       in_place_document_serializer_from_t) {
        value.value = value_to<int>(doc);
    }
};

template <>
struct document_serializer_impl<json_array_test::immovable> {
    using value_type = json_array_test::immovable;
    static value_type construct(const document &doc, in_place_document_serializer_from_t) {
        return value_type(value_to<int>(doc));
    }
};

template <>
struct document_serializer_impl<json_array_test::lifetime_value> {
    using value_type = json_array_test::lifetime_value;
    static value_type construct(const document &doc, in_place_document_serializer_from_t) {
        return value_type(value_to<int>(doc));
    }
};

} // namespace wjr::json

namespace {

using namespace json_array_test;
using wjr::json::value_to;

template <typename T, typename Source>
concept can_project = requires(Source &&doc) { value_to<T>(std::forward<Source>(doc)); };

static_assert(can_project<std::array<std::string_view, 2>, const document &>);
static_assert(!can_project<std::array<std::string_view, 2>, document>);
static_assert(!can_project<std::array<std::string_view, 2>, const document>);
static_assert(!can_project<label_view, document>);
static_assert(!can_project<std::vector<std::array<std::string_view, 2>>, document>);
static_assert(can_project<std::array<unsupported, 0>, document>);
static_assert(!can_project<std::array<unsupported, 1>, document>);
static_assert(can_project<std::array<immovable, 2>, const document &>);
static_assert(!can_project<std::array<std::string_view, 2>, wjr::json::pmr_document>);

template <typename Document = document>
Document parse(std::string_view input) {
    wjr::json::ondemand_reader reader;
    reader.read(input);
    auto result = Document::parse(reader);
    EXPECT_TRUE(result);
    return std::move(result).value();
}

TEST(json_fixed_array, coordinates_and_registered_roundtrip) {
    const auto source = parse(R"({"coordinates":[-123.5,49.25]})");
    auto output = value_to<point>(source);
    EXPECT_EQ(output.coordinates, (std::array{-123.5, 49.25}));
    EXPECT_EQ(document(output), source);
    parse(R"({"coordinates":[1,2]})").get_to(output);
    EXPECT_EQ(output.coordinates, (std::array{1.0, 2.0}));
}

TEST(json_fixed_array, nested_variable_rings_have_inline_points) {
    using polygon = std::vector<std::vector<std::array<double, 2>>>;
    const auto source = parse("[[[0,0],[1,0],[1,1],[0,0]],[[0.1,0.1],[0.2,0.1],[0.1,0.1]]]");
    auto output = value_to<polygon>(source);
    ASSERT_EQ(output.size(), 2);
    ASSERT_EQ(output[0].size(), 4);
    EXPECT_EQ(output[0][1], (std::array{1.0, 0.0}));
    EXPECT_EQ(document(output), source);
    source.get_to(output);
    EXPECT_EQ(document(output), source);
}

TEST(json_fixed_array, nested_fixed_arrays_and_nullable_elements) {
    using grid = std::array<std::array<std::optional<int>, 2>, 2>;
    const auto source = parse("[[null,1],[2,null]]");
    const auto output = value_to<grid>(source);
    EXPECT_FALSE(output[0][0]);
    EXPECT_EQ(output[0][1], 1);
    EXPECT_EQ(output[1][0], 2);
    EXPECT_FALSE(output[1][1]);
    EXPECT_EQ(document(output), source);
    const auto optional = value_to<std::optional<std::array<int, 2>>>(parse("[1,2]"));
    ASSERT_TRUE(optional);
    EXPECT_EQ(*optional, (std::array{1, 2}));
}

TEST(json_fixed_array, dictionary_values_use_exact_fixed_shape) {
    using positions = std::map<std::string, std::array<double, 2>>;
    auto source = parse(R"({"first":[1,2],"second":[3,4]})");
    auto output = value_to<positions>(std::as_const(source));
    EXPECT_EQ(output.at("first"), (std::array{1.0, 2.0}));
    EXPECT_EQ(document(output), source);
    const auto moved = value_to<positions>(std::move(source));
    EXPECT_EQ(moved.at("second"), (std::array{3.0, 4.0}));
}

TEST(json_fixed_array, zero_length_needs_no_child_serializer) {
    using empty = std::array<unsupported, 0>;
    auto output = value_to<empty>(parse("[]"));
    EXPECT_TRUE(output.empty());
    parse("[]").get_to(output);
    EXPECT_EQ(document(output).to_string(), "[]");
    document target(7);
    target = output;
    EXPECT_EQ(target.to_string(), "[]");
}

TEST(json_fixed_array, nondefault_elements_construct_without_initialization_pass) {
    const auto output = value_to<std::array<number, 2>>(parse("[3,4]"));
    EXPECT_EQ(output[0].value, 3);
    EXPECT_EQ(output[1].value, 4);
    using rows = std::vector<std::array<number, 2>>;
    auto existing = value_to<rows>(parse("[[1,2],[3,4]]"));
    parse("[[7,8]]").get_to(existing);
    ASSERT_EQ(existing.size(), 1);
    EXPECT_EQ(existing[0][1].value, 8);
}

TEST(json_fixed_array, immovable_elements_are_directly_initialized) {
    const auto output = value_to<std::array<immovable, 2>>(parse("[3,4]"));
    EXPECT_EQ(output[0].value, 3);
    EXPECT_EQ(output[1].value, 4);
}

TEST(json_fixed_array, borrowed_elements_keep_live_document_storage) {
    const auto source = parse(R"({"labels":["first long label stored in the document","second"]})");
    const auto output = value_to<label_view>(source);
    EXPECT_EQ(output.labels[0].data(), source.at("labels")[0].get<wjr::json::string_t>().data());
    EXPECT_EQ(output.labels[1], "second");
}

TEST(json_fixed_array, moving_strings_and_const_rvalue_copy) {
    const std::string content(100, 'x');
    auto source = document::_construct_array();
    source.get<wjr::json::array_t>().emplace_back(content);
    const auto *storage = source[0].get<wjr::json::string_t>().data();
    const auto moved = value_to<std::array<std::string, 1>>(std::move(source));
    EXPECT_EQ(moved[0].data(), storage);
    const auto immutable = parse(R"(["copy"])");
    EXPECT_EQ((value_to<std::array<std::string, 1>>(std::move(immutable))[0]), "copy");
}

TEST(json_fixed_array, assignment_preserves_pmr_resources) {
    std::pmr::monotonic_buffer_resource resource;
    std::array<std::pmr::string, 2> output{std::pmr::string(&resource),
                                           std::pmr::string(&resource)};
    const auto source = parse(R"(["first string longer than a small string buffer","second"])");
    source.get_to(output);
    EXPECT_EQ(output[0].get_allocator().resource(), &resource);
    using pmr_document = wjr::json::pmr_document;
    pmr_document target(std::allocator_arg, pmr_document::allocator_type(&resource), 7);
    target = output;
    EXPECT_EQ(target.get_allocator().resource(), &resource);
    EXPECT_EQ(target[0].get<wjr::json::string_t>().get_allocator().resource(), &resource);
}

TEST(json_fixed_array, pmr_source_supports_copy_move_and_borrow) {
    auto source = parse<wjr::json::pmr_document>("[1,2]");
    EXPECT_EQ((value_to<std::array<int, 2>>(std::as_const(source))), (std::array{1, 2}));
    EXPECT_EQ((value_to<std::array<int, 2>>(std::move(source))), (std::array{1, 2}));
    const auto strings = parse<wjr::json::pmr_document>(R"(["first","second"])");
    const auto views = value_to<std::array<std::string_view, 2>>(strings);
    EXPECT_EQ(views[0].data(), strings[0].get<wjr::json::string_t>().data());
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(json_fixed_array, wrong_length_is_rejected_before_assignment_or_move) {
    std::array<int, 2> output{7, 8};
    EXPECT_THROW(parse("[1]").get_to(output), std::length_error);
    EXPECT_EQ(output, (std::array{7, 8}));
    EXPECT_THROW(((void)value_to<std::array<int, 2>>(parse("[1,2,3]"))), std::length_error);
    auto strings = parse(R"(["must not move"])");
    EXPECT_THROW(((void)value_to<std::array<std::string, 2>>(std::move(strings))),
                 std::length_error);
    EXPECT_EQ(value_to<std::string>(strings[0]), "must not move");
}

TEST(json_fixed_array, wrong_kind_and_child_error_remain_distinct) {
    EXPECT_THROW(((void)value_to<std::array<int, 2>>(document(nullptr))),
                 wjr::json::bad_document_access);
    EXPECT_THROW(((void)value_to<std::array<int, 2>>(parse("[1,false]"))),
                 wjr::bad_expected_access<wjr::json::error_code>);
}

TEST(json_fixed_array, partial_construction_is_destroyed_and_assignment_is_not_transactional) {
    using output_type = std::array<lifetime_value, 2>;
    EXPECT_EQ(lifetime_value::live, 0);
    EXPECT_THROW((void)value_to<output_type>(parse("[1,false]")),
                 wjr::bad_expected_access<wjr::json::error_code>);
    EXPECT_EQ(lifetime_value::live, 0);
    std::array<int, 2> output{7, 8};
    EXPECT_THROW(parse("[1,false]").get_to(output),
                 wjr::bad_expected_access<wjr::json::error_code>);
    EXPECT_EQ(output, (std::array{1, 8}));
}
#endif

} // namespace
