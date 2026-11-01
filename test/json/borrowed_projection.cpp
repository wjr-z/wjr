#include <wjr/json/document.hpp>
#include <wjr/json/pmr.hpp>

#include <map>
#include <memory_resource>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace borrowed_projection_test {

using document = wjr::json::document;

struct row_view {
    row_view() = default;
    std::string_view name;
    std::vector<std::string_view> tags;
    WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER(row_view, name, tags)
};

struct owned_row {
    owned_row() = default;
    std::string name;
    WJR_REGISTER_FROM_DOCUMENT_OBJECT_SERIALIZER(owned_row, name)
};

/** @brief A user serializer may deliberately accept only mutable rvalues. */
struct moving_value {
    std::string text;
};

/** @brief A user's const-reference constructor can produce an owning result. */
struct copy_value {
    std::string text;
    explicit copy_value(const document &doc, wjr::json::in_place_document_serializer_from_t)
        : text(static_cast<std::string>(doc)) {}
    copy_value &operator=(const document &doc) {
        doc.get_to(text);
        return *this;
    }
};

/** @brief Dictionary conversion also accepts ordinary user element constructors. */
struct dictionary_value {
    std::string text;
    explicit dictionary_value(const document &doc) : text(static_cast<std::string>(doc)) {}
};

} // namespace borrowed_projection_test

namespace wjr::json {

template <>
struct document_serializer_impl<borrowed_projection_test::moving_value> {
    using value_type = borrowed_projection_test::moving_value;
    static value_type construct(document &&doc, in_place_document_serializer_from_t) {
        return {static_cast<std::string>(std::move(doc))};
    }
    static void assign(document &&doc, value_type &value, in_place_document_serializer_from_t) {
        std::move(doc).get_to(value.text);
    }
};

} // namespace wjr::json

namespace {

using namespace borrowed_projection_test;

template <typename Source, typename Destination>
concept can_project =
    requires(Source &&source) { static_cast<Destination>(std::forward<Source>(source)); };

template <typename Source, typename Destination>
concept can_assign_projection = requires(Source &&source, Destination &destination) {
    std::forward<Source>(source).get_to(destination);
};

template <typename T, typename Document = document>
constexpr bool borrows_only_lvalues =
    can_project<Document &, T> && can_project<const Document &, T> &&
    can_assign_projection<Document &, T> && can_assign_projection<const Document &, T> &&
    !can_project<Document, T> && !can_project<const Document, T> &&
    !can_assign_projection<Document, T> && !can_assign_projection<const Document, T>;

template <typename T>
constexpr bool accepts_all_categories =
    can_project<document &, T> && can_project<const document &, T> && can_project<document, T> &&
    can_project<const document, T> && can_assign_projection<document &, T> &&
    can_assign_projection<const document &, T> && can_assign_projection<document, T> &&
    can_assign_projection<const document, T>;

static_assert(borrows_only_lvalues<std::string_view>);
static_assert(borrows_only_lvalues<std::vector<std::string_view>>);
static_assert(borrows_only_lvalues<std::vector<std::vector<std::string_view>>>);
static_assert(borrows_only_lvalues<std::map<std::string, std::string_view>>);
static_assert(borrows_only_lvalues<std::unordered_map<std::string, std::vector<std::string_view>>>);
static_assert(borrows_only_lvalues<row_view>);
static_assert(borrows_only_lvalues<std::vector<row_view>>);
static_assert(borrows_only_lvalues<std::string_view, wjr::json::pmr_document>);
static_assert(borrows_only_lvalues<row_view, wjr::json::pmr_document>);
static_assert(accepts_all_categories<std::string>);
static_assert(accepts_all_categories<std::vector<std::string>>);
static_assert(accepts_all_categories<std::map<std::string, std::string>>);
static_assert(accepts_all_categories<owned_row>);
static_assert(accepts_all_categories<copy_value>);
static_assert(accepts_all_categories<std::map<std::string, dictionary_value>>);
static_assert(!can_project<const document, moving_value>);
static_assert(!can_assign_projection<const document, moving_value>);
static_assert(can_project<document, moving_value>);
static_assert(can_assign_projection<document, moving_value>);

document text(std::string_view value) { return document::_construct_string(value); }

TEST(json_borrowed_projection, const_string_view_points_into_live_document) {
    const auto source = text("a string longer than the usual small string buffer");
    const auto view = static_cast<std::string_view>(source);
    std::string_view assigned;
    source.get_to(assigned);
    EXPECT_EQ(view, source.get<wjr::json::string_t>());
    EXPECT_EQ(view.data(), source.get<wjr::json::string_t>().data());
    EXPECT_EQ(assigned.data(), view.data());
}

TEST(json_borrowed_projection, nested_arrays_and_dictionaries_preserve_borrowed_bytes) {
    using views = std::vector<std::string_view>;
    const document arrays(std::vector<std::vector<std::string>>{{"alpha", "beta"}, {"gamma"}});
    const auto projected = static_cast<std::vector<views>>(arrays);
    std::vector<views> assigned(3);
    arrays.get_to(assigned);
    ASSERT_EQ(projected.size(), 2);
    EXPECT_EQ(projected[0][1], "beta");
    EXPECT_EQ(projected[1][0].data(), arrays[1][0].get<wjr::json::string_t>().data());
    EXPECT_EQ(assigned[0][0].data(), projected[0][0].data());

    const document object(
        std::map<std::string, std::vector<std::string>>{{"names", {"one", "two"}}});
    const auto dict = static_cast<std::map<std::string, views>>(object);
    std::unordered_map<std::string, views> hash_dict;
    object.get_to(hash_dict);
    EXPECT_EQ(dict.at("names")[1], "two");
    EXPECT_EQ(hash_dict.at("names")[0].data(),
              object.at("names")[0].get<wjr::json::string_t>().data());
}

TEST(json_borrowed_projection, registered_view_copy_constructs_and_assigns) {
    auto source = document::_construct_object();
    source["name"] = std::string("registered name");
    source["tags"] = std::vector<std::string>{"first", "second"};
    const auto &input = source;
    const auto row = static_cast<row_view>(input);
    row_view assigned;
    input.get_to(assigned);
    EXPECT_EQ(row.name, "registered name");
    EXPECT_EQ(row.name.data(), input.at("name").get<wjr::json::string_t>().data());
    EXPECT_EQ(assigned.tags[1].data(), input.at("tags")[1].get<wjr::json::string_t>().data());
}

TEST(json_borrowed_projection, pmr_projection_borrows_the_document_string) {
    std::pmr::monotonic_buffer_resource resource;
    using pmr_document = wjr::json::pmr_document;
    const pmr_document source(std::allocator_arg, pmr_document::allocator_type(&resource),
                              std::string_view("resource-backed document string"));
    const auto view = static_cast<std::string_view>(source);
    EXPECT_EQ(view, "resource-backed document string");
    EXPECT_EQ(view.data(), source.get<wjr::json::string_t>().data());
}

TEST(json_borrowed_projection, const_rvalue_owning_conversions_and_user_contracts) {
    const auto source = text("owned after source lifetime");
    EXPECT_EQ(static_cast<std::string>(std::move(source)), "owned after source lifetime");
    copy_value copied(std::move(source), wjr::json::in_place_document_serializer_from);
    std::move(source).get_to(copied);
    EXPECT_EQ(copied.text, "owned after source lifetime");

    auto movable = text("mutable rvalue only");
    const auto moved = static_cast<moving_value>(std::move(movable));
    EXPECT_EQ(moved.text, "mutable rvalue only");

    const document array(std::vector<std::string>{"a", "b"});
    EXPECT_EQ(static_cast<std::vector<std::string>>(std::move(array)),
              (std::vector<std::string>{"a", "b"}));
    const document object(std::map<std::string, std::string>{{"key", "value"}});
    EXPECT_EQ((static_cast<std::map<std::string, std::string>>(std::move(object))).at("key"),
              "value");
    auto custom = static_cast<std::map<std::string, dictionary_value>>(std::move(object));
    EXPECT_EQ(custom.at("key").text, "value");
    std::move(object).get_to(custom);
    EXPECT_EQ(custom.at("key").text, "value");
}

#if !defined(WJR_DISABLE_EXCEPTIONS)
TEST(json_borrowed_projection, incorrect_type_and_missing_field_keep_existing_errors) {
    const document number(42);
    EXPECT_THROW((void)static_cast<std::string_view>(number), wjr::json::bad_document_access);
    const auto empty = document::_construct_object();
    EXPECT_THROW((void)static_cast<row_view>(empty), std::out_of_range);
}
#endif

} // namespace
