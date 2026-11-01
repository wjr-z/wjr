#include <gtest/gtest.h>

#include <array>
#include <concepts>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <wjr/json/array.hpp>
#include <wjr/json/document.hpp>
#include <wjr/json/optional.hpp>
#include <wjr/json/pmr.hpp>

namespace serializer_concepts_test {

using document = wjr::json::document;

struct construct_only {
    explicit construct_only(int number) : value(number) {}
    int value;
};

struct assign_only {
    int value = 0;
};

struct throwing_destructor {
    explicit throwing_destructor(int number) : value(number) {}
    ~throwing_destructor() noexcept(false) {}
    int value;
};

struct move_source_only {
    std::unique_ptr<int> value;
};

struct record {
    record() = default;
    int id = 0;
    std::optional<std::string> name;
    std::array<int, 2> coordinate{};
    WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER(record, id, name, coordinate)
};

} // namespace serializer_concepts_test

namespace wjr::json {

template <>
struct document_serializer_impl<serializer_concepts_test::construct_only> {
    using value_type = serializer_concepts_test::construct_only;
    static value_type construct(const document &source, in_place_document_serializer_from_t) {
        return value_type(static_cast<int>(source));
    }
};

template <>
struct document_serializer_impl<serializer_concepts_test::assign_only> {
    using value_type = serializer_concepts_test::assign_only;
    static void assign(const document &source, value_type &target,
                       in_place_document_serializer_from_t) {
        target.value = static_cast<int>(source);
    }
};

template <>
struct document_serializer_impl<serializer_concepts_test::throwing_destructor> {
    using value_type = serializer_concepts_test::throwing_destructor;
    static value_type construct(const document &source, in_place_document_serializer_from_t) {
        return value_type(static_cast<int>(source));
    }
};

template <>
struct document_serializer_impl<serializer_concepts_test::move_source_only> {
    using value_type = serializer_concepts_test::move_source_only;
    static value_type construct(document &&source, in_place_document_serializer_from_t) {
        return {std::make_unique<int>(static_cast<int>(source))};
    }
};

} // namespace wjr::json

namespace {
using namespace serializer_concepts_test;
namespace json = wjr::json;

template <json::from_document_constructible<const document &> T>
T read_value(const document &source) {
    return json::value_to<T>(source);
}

template <json::from_document_assignable<const document &> T>
void read_into(const document &source, T &target) {
    source.get_to(target);
}

template <typename T>
requires json::to_document_constructible<document, T &&>
document write_value(T &&source) {
    return document(std::forward<T>(source));
}

static_assert(json::from_document_constructible<construct_only, const document &>);
static_assert(!json::from_document_assignable<construct_only, const document &>);
static_assert(json::from_document_assignable<assign_only, const document &>);
static_assert(!json::from_document_constructible<assign_only, const document &>);
static_assert(json::from_document_constructible<throwing_destructor, const document &>);
static_assert(!std::constructible_from<throwing_destructor, int>);
static_assert(json::from_document_constructible<move_source_only, document &&>);
static_assert(!json::from_document_constructible<move_source_only, const document &>);
static_assert(!json::from_document_constructible<move_source_only, const document &&>);
static_assert(json::from_document_constructible<std::string_view, const document &>);
static_assert(!json::from_document_constructible<std::string_view, document &&>);
static_assert(!json::from_document_assignable<std::string_view, const document &&>);
static_assert(json::from_document_constructible<std::vector<std::string_view>, document &>);
static_assert(!json::from_document_constructible<std::vector<std::string_view>, document &&>);
static_assert(json::from_document_constructible<int, const json::pmr_document &>);
static_assert(json::to_document_constructible<document, int>);
static_assert(json::to_document_assignable<document, const std::string &>);
static_assert(json::from_document_constructible<std::optional<int>, const document &>);
static_assert(json::from_document_constructible<std::array<int, 2>, const document &>);
static_assert(json::from_document_constructible<record, const document &>);
static_assert(std::is_assignable_v<document &, document &>);
static_assert(std::is_assignable_v<document &, const document &>);
static_assert(std::is_assignable_v<document &, document &&>);
static_assert(std::is_assignable_v<json::pmr_document &, json::pmr_document &>);
static_assert(std::is_assignable_v<json::pmr_document &, const json::pmr_document &>);
static_assert(std::is_assignable_v<json::pmr_document &, json::pmr_document &&>);
static_assert(json::to_document_assignable<json::pmr_document, json::pmr_document &>);

} // namespace

TEST(json_serializer_concepts, constrained_generic_read_and_assignment) {
    const document source(42);
    EXPECT_EQ(read_value<construct_only>(source).value, 42);
    EXPECT_EQ(read_value<throwing_destructor>(source).value, 42);
    assign_only target;
    read_into(source, target);
    EXPECT_EQ(target.value, 42);
    EXPECT_EQ(read_value<std::optional<int>>(source), 42);
    const document null(nullptr);
    EXPECT_FALSE(read_value<std::optional<int>>(null));
}

TEST(json_serializer_concepts, constrained_generic_document_target) {
    auto scalar = write_value(42);
    EXPECT_EQ(static_cast<int>(scalar), 42);
    std::string text(80, 'x');
    auto string = write_value(text);
    EXPECT_EQ(static_cast<std::string>(string), text);
    string = std::string("updated");
    EXPECT_EQ(static_cast<std::string>(string), "updated");
}

TEST(json_serializer_concepts, registered_nullable_array_and_move_only_source) {
    document::object_type fields;
    fields.emplace("id", document(7));
    fields.emplace("name", document(nullptr));
    document::array_type coordinate;
    coordinate.emplace_back(3);
    coordinate.emplace_back(4);
    fields.emplace("coordinate", document(std::move(coordinate)));
    const document source(std::move(fields));
    const auto result = read_value<record>(source);
    EXPECT_EQ(result.id, 7);
    EXPECT_FALSE(result.name);
    EXPECT_EQ(result.coordinate, (std::array<int, 2>{3, 4}));
    auto moved = json::value_to<move_source_only>(document(19));
    ASSERT_TRUE(moved.value);
    EXPECT_EQ(*moved.value, 19);
}
