#include <wjr/json/optional.hpp>
#include <wjr/json/pmr.hpp>

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <memory_resource>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace json_optional_test {

using document = wjr::json::document;

struct reply {
    std::optional<std::int64_t> in_reply_to_status_id;
    std::optional<std::string> in_reply_to_screen_name;
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(reply, in_reply_to_status_id, in_reply_to_screen_name)
};

struct reply_view {
    std::optional<std::string_view> name;
    WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER(reply_view, name)
};

struct moving_value {
    std::unique_ptr<int> value;
};

struct copy_only {
    int value;
    explicit copy_only(int number) : value(number) {}
    copy_only(const copy_only &) = default;
    copy_only(copy_only &&) = delete;
    copy_only &operator=(const copy_only &) = default;
};

struct immovable {
    immovable() = default;
    immovable(const immovable &) = delete;
    immovable(immovable &&) = delete;
};

struct counting_resource : std::pmr::memory_resource {
    std::size_t allocations = 0;
    std::size_t live = 0;
    void *do_allocate(std::size_t bytes, std::size_t alignment) override {
        void *pointer = std::pmr::new_delete_resource()->allocate(bytes, alignment);
        ++allocations;
        ++live;
        return pointer;
    }
    void do_deallocate(void *pointer, std::size_t bytes, std::size_t alignment) override {
        --live;
        std::pmr::new_delete_resource()->deallocate(pointer, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
};

/** @brief Restore the borrowed process default even when an assertion or conversion fails. */
struct default_resource_scope {
    explicit default_resource_scope(std::pmr::memory_resource *resource)
        : previous(std::pmr::set_default_resource(resource)) {}
    ~default_resource_scope() { std::pmr::set_default_resource(previous); }
    std::pmr::memory_resource *previous;
};

} // namespace json_optional_test

namespace wjr::json {

template <>
struct document_serializer_impl<json_optional_test::moving_value> {
    using value_type = json_optional_test::moving_value;
    static value_type construct(document &&doc, in_place_document_serializer_from_t) {
        return {std::make_unique<int>(value_to<int>(doc))};
    }
    static void assign(document &&doc, value_type &value, in_place_document_serializer_from_t) {
        *value.value = value_to<int>(doc);
    }
};

template <>
struct document_serializer_impl<json_optional_test::copy_only> {
    using value_type = json_optional_test::copy_only;
    static value_type construct(const document &doc, in_place_document_serializer_from_t) {
        return value_type(value_to<int>(doc));
    }
    static void assign(const document &doc, value_type &value,
                       in_place_document_serializer_from_t) {
        value.value = value_to<int>(doc);
    }
};

template <>
struct document_serializer_impl<json_optional_test::immovable> {
    using value_type = json_optional_test::immovable;
    static value_type construct(const document &, in_place_document_serializer_from_t) {
        return {};
    }
};

} // namespace wjr::json

namespace {

using namespace json_optional_test;
using wjr::json::value_to;

template <typename T, typename Source>
concept can_value_to = requires(Source &&source) { value_to<T>(std::forward<Source>(source)); };

static_assert(can_value_to<int, const document &>);
static_assert(!can_value_to<int &, document &>);
static_assert(!can_value_to<int, int>);
static_assert(can_value_to<std::optional<std::string_view>, const document &>);
static_assert(!can_value_to<std::optional<std::string_view>, document>);
static_assert(!can_value_to<std::optional<std::string_view>, const document>);
static_assert(!can_value_to<std::vector<std::optional<std::string_view>>, document>);
static_assert(!can_value_to<std::map<std::string, std::optional<std::string_view>>, document>);
static_assert(!can_value_to<reply_view, document>);
static_assert(can_value_to<std::optional<moving_value>, document>);
static_assert(!can_value_to<std::optional<moving_value>, const document &>);
static_assert(can_value_to<std::optional<copy_only>, const document &>);
static_assert(!can_value_to<std::optional<immovable>, const document &>);
static_assert(!can_value_to<std::map<std::string, std::optional<immovable>>, const document &>);
static_assert(!can_value_to<std::map<std::string, std::optional<moving_value>>, const document &>);

document parse(std::string_view input) {
    wjr::json::ondemand_reader reader;
    reader.read(input);
    auto result = document::parse(reader);
    EXPECT_TRUE(result);
    return std::move(result).value();
}

TEST(json_optional, canonical_scalar_entry_uses_serializer) {
    const document null(nullptr);
    EXPECT_FALSE(value_to<std::optional<int>>(null).has_value());
    EXPECT_EQ(value_to<std::optional<int>>(document(42)), 42);
    EXPECT_EQ(value_to<int>(document(42)), 42);
    EXPECT_EQ(value_to<bool>(document(false)), false);
    EXPECT_EQ(value_to<std::nullptr_t>(null), nullptr);
}

TEST(json_optional, assign_engage_update_reset) {
    std::optional<int> value;
    document(7).get_to(value);
    EXPECT_EQ(value, 7);
    document(0).get_to(value);
    EXPECT_EQ(value, 0);
    document(nullptr).get_to(value);
    EXPECT_FALSE(value.has_value());
}

TEST(json_optional, serialize_empty_zero_false_and_value) {
    EXPECT_TRUE(document(std::optional<int>{}).is_null());
    const document zero(std::optional<int>{0});
    EXPECT_TRUE(zero.is_number_signed());
    EXPECT_EQ(value_to<int>(zero), 0);
    const document no(std::optional<bool>{false});
    EXPECT_TRUE(no.is_boolean());
    EXPECT_FALSE(value_to<bool>(no));
    EXPECT_EQ(value_to<std::string>(document(std::optional<std::string>{"name"})), "name");
    document output(17);
    output = std::optional<bool>{false};
    EXPECT_FALSE(value_to<bool>(output));
    output = std::optional<bool>{};
    EXPECT_TRUE(output.is_null());
}

TEST(json_optional, registered_nullable_fields_roundtrip) {
    auto source = parse(R"({"in_reply_to_status_id":null,"in_reply_to_screen_name":"alice"})");
    const auto copy = value_to<reply>(std::as_const(source));
    EXPECT_FALSE(copy.in_reply_to_status_id);
    EXPECT_EQ(copy.in_reply_to_screen_name, "alice");
    const document roundtrip(copy);
    EXPECT_EQ(roundtrip, source);
    auto moved = value_to<reply>(std::move(source));
    EXPECT_EQ(moved.in_reply_to_screen_name, "alice");
    auto replacement = parse(R"({"in_reply_to_status_id":123,"in_reply_to_screen_name":null})");
    replacement.get_to(moved);
    EXPECT_EQ(moved.in_reply_to_status_id, 123);
    EXPECT_FALSE(moved.in_reply_to_screen_name);
}

template <typename T>
class json_optional_array : public testing::Test {};

using arrays =
    testing::Types<std::vector<std::optional<int>>, wjr::vector<std::optional<int>>,
                   wjr::small_vector<std::optional<int>, 4>, wjr::fixed_vector<std::optional<int>>>;
TYPED_TEST_SUITE(json_optional_array, arrays);

TYPED_TEST(json_optional_array, construct_assign_and_roundtrip) {
    const auto source = parse("[null,0,42]");
    auto output = value_to<TypeParam>(source);
    ASSERT_EQ(output.size(), 3);
    EXPECT_FALSE(output[0]);
    EXPECT_EQ(output[1], 0);
    EXPECT_EQ(output[2], 42);
    EXPECT_EQ(document(output), source);
    source.get_to(output);
    EXPECT_FALSE(output[0]);
    auto shorter = parse("[9]");
    shorter.get_to(output);
    ASSERT_EQ(output.size(), 1);
    EXPECT_EQ(output[0], 9);
    parse("[null,3,4]").get_to(output);
    ASSERT_EQ(output.size(), 3);
    EXPECT_FALSE(output[0]);
    EXPECT_EQ(output[1], 3);
    EXPECT_EQ(output[2], 4);
    auto moved = value_to<TypeParam>(parse("[null,1]"));
    EXPECT_FALSE(moved[0]);
    EXPECT_EQ(moved[1], 1);
    EXPECT_TRUE(value_to<TypeParam>(parse("[]")).empty());
}

TEST(json_optional, nested_arrays_and_objects) {
    using rows = std::vector<std::optional<std::map<std::string, std::optional<int>>>>;
    const auto source = parse(R"([null,{"a":null,"b":7}])");
    const auto output = value_to<rows>(source);
    ASSERT_EQ(output.size(), 2);
    EXPECT_FALSE(output[0]);
    ASSERT_TRUE(output[1]);
    EXPECT_FALSE(output[1]->at("a"));
    EXPECT_EQ(output[1]->at("b"), 7);
    EXPECT_EQ(document(output), source);
}

template <typename T>
class json_optional_object : public testing::Test {};

using objects = testing::Types<std::map<std::string, std::optional<int>>,
                               std::multimap<std::string, std::optional<int>>,
                               std::unordered_map<std::string, std::optional<int>>>;
TYPED_TEST_SUITE(json_optional_object, objects);

TYPED_TEST(json_optional_object, construct_assign_and_move) {
    auto source = parse(R"({"a":null,"b":7,"b":9})");
    auto output = value_to<TypeParam>(std::as_const(source));
    EXPECT_FALSE(output.find("a")->second);
    EXPECT_EQ(output.find("b")->second, 9);
    EXPECT_EQ(document(output), source);
    auto moved = value_to<TypeParam>(std::move(source));
    EXPECT_FALSE(moved.find("a")->second);
    EXPECT_EQ(moved.find("b")->second, 9);
    parse(R"({"c":null})").get_to(output);
    ASSERT_EQ(output.size(), 1);
    EXPECT_FALSE(output.find("c")->second);
}

TEST(json_optional, borrowed_views_refer_to_live_document) {
    const auto source = parse(R"({"name":"a long borrowed string that stays in the document"})");
    const auto row = value_to<reply_view>(source);
    ASSERT_TRUE(row.name);
    EXPECT_EQ(row.name->data(), source.at("name").get<wjr::json::string_t>().data());
    const auto array = parse(R"([null,"borrowed"])");
    const auto views = value_to<std::vector<std::optional<std::string_view>>>(array);
    EXPECT_FALSE(views[0]);
    EXPECT_EQ(views[1]->data(), array[1].get<wjr::json::string_t>().data());
}

TEST(json_optional, custom_moving_and_copy_only_children) {
    auto moving = value_to<std::optional<moving_value>>(document(7));
    ASSERT_TRUE(moving);
    EXPECT_EQ(*moving->value, 7);
    document(9).get_to(moving);
    EXPECT_EQ(*moving->value, 9);
    const auto copied = value_to<std::optional<copy_only>>(document(13));
    ASSERT_TRUE(copied);
    EXPECT_EQ(copied->value, 13);
}

TEST(json_optional, nested_null_collapses_to_outer_disengagement) {
    using nested = std::optional<std::optional<int>>;
    EXPECT_FALSE(value_to<nested>(document(nullptr)));
    const nested inner_empty(std::in_place, std::nullopt);
    EXPECT_TRUE(document(inner_empty).is_null());
    const auto value = value_to<nested>(document(7));
    ASSERT_TRUE(value);
    EXPECT_EQ(*value, 7);
}

TEST(json_optional, assignment_keeps_engaged_pmr_string_resource) {
    std::pmr::monotonic_buffer_resource resource;
    std::optional<std::pmr::string> output(std::in_place, &resource);
    const auto source = document::_construct_string("a string longer than the small string buffer");
    source.get_to(output);
    ASSERT_TRUE(output);
    EXPECT_EQ(output->get_allocator().resource(), &resource);
    EXPECT_EQ(*output, "a string longer than the small string buffer");
    using pmr_document = wjr::json::pmr_document;
    pmr_document target(std::allocator_arg, pmr_document::allocator_type(&resource), 7);
    target = std::optional<std::string>{"owned by destination resource"};
    EXPECT_EQ(target.get_allocator().resource(), &resource);
    EXPECT_EQ(target.get<wjr::json::string_t>().get_allocator().resource(), &resource);
    target = std::optional<std::string>{};
    EXPECT_TRUE(target.is_null());
    EXPECT_EQ(target.get_allocator().resource(), &resource);
}

TEST(json_optional, array_has_only_its_container_allocation_and_reuses_capacity) {
    const auto source = parse("[null,0,1,null,2,3]");
    counting_resource resource;
    default_resource_scope scope(&resource);
    {
        auto output = value_to<std::pmr::vector<std::optional<int>>>(source);
        EXPECT_EQ(resource.allocations, 1);
        EXPECT_EQ(resource.live, 1);
        const auto *storage = output.data();
        source.get_to(output);
        EXPECT_EQ(output.data(), storage);
        EXPECT_EQ(resource.allocations, 1);
        EXPECT_EQ(output[5], 3);
    }
    EXPECT_EQ(resource.live, 0);
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(json_optional, wrong_type_does_not_become_disengaged) {
    const document wrong(false);
    EXPECT_THROW((void)value_to<std::optional<int>>(wrong),
                 wjr::bad_expected_access<wjr::json::error_code>);
    std::optional<int> engaged(7);
    EXPECT_THROW(wrong.get_to(engaged), wjr::bad_expected_access<wjr::json::error_code>);
    EXPECT_EQ(engaged, 7);
    std::optional<int> empty;
    EXPECT_THROW(wrong.get_to(empty), wjr::bad_expected_access<wjr::json::error_code>);
    EXPECT_FALSE(empty);
}

TEST(json_optional, missing_registered_field_keeps_existing_error) {
    const auto source = parse(R"({"in_reply_to_status_id":null})");
    EXPECT_THROW((void)value_to<reply>(source), std::out_of_range);
}

TEST(json_optional, failed_array_construction_releases_partial_result) {
    const auto source = parse("[1,false]");
    counting_resource resource;
    default_resource_scope scope(&resource);
    using output_type = std::pmr::vector<std::optional<int>>;
    EXPECT_THROW((void)value_to<output_type>(source),
                 wjr::bad_expected_access<wjr::json::error_code>);
    EXPECT_EQ(resource.allocations, 1);
    EXPECT_EQ(resource.live, 0);
}
#endif

} // namespace
