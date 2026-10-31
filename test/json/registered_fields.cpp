#include "../detail.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory_resource>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <wjr/json/document.hpp>
#include <wjr/json/pmr.hpp>

namespace {
using namespace wjr::json;

struct field_record {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(field_record);
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(field_record, id,
                                            descriptive_name_larger_than_small_string,
                                            release_revision_larger_than_small_string)
    uint64_t id = 0;
    std::string descriptive_name_larger_than_small_string;
    std::string release_revision_larger_than_small_string;
};

struct counting_resource : std::pmr::memory_resource {
    size_t calls = 0;
    void *do_allocate(size_t bytes, size_t alignment) override {
        ++calls;
        return std::pmr::new_delete_resource()->allocate(bytes, alignment);
    }
    void do_deallocate(void *pointer, size_t bytes, size_t alignment) override {
        std::pmr::new_delete_resource()->deallocate(pointer, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
};

struct default_resource_scope {
    explicit default_resource_scope(std::pmr::memory_resource *resource)
        : previous(std::pmr::set_default_resource(resource)) {}
    ~default_resource_scope() { std::pmr::set_default_resource(previous); }
    std::pmr::memory_resource *previous;
};

template <typename Document>
Document parsed(std::string_view input) {
    ondemand_reader reader;
    reader.read(input);
    auto value = Document::parse(reader);
    EXPECT_TRUE(value);
    return std::move(*value);
}

template <typename Document>
void check_copy_and_move() {
    const std::string description(96, 'd');
    const std::string revision(80, 'r');
    const std::string input = "{\"id\":7,\"descriptive_name_larger_than_small_string\":\"" +
                              description + "\",\"release_revision_larger_than_small_string\":\"" +
                              revision + "\"}";
    const auto source = parsed<Document>(input);
    const auto expect = [&](const field_record &value) {
        EXPECT_EQ(value.id, 7);
        EXPECT_EQ(value.descriptive_name_larger_than_small_string, description);
        EXPECT_EQ(value.release_revision_larger_than_small_string, revision);
    };
    expect(field_record(source));
    field_record assigned;
    assigned = source;
    expect(assigned);
    auto move_source = parsed<Document>(input);
    expect(field_record(std::move(move_source)));
    move_source = parsed<Document>(input);
    assigned = std::move(move_source);
    expect(assigned);
}

struct reverse_compare {
    using is_transparent = void;
    static inline size_t view_calls = 0;
    bool operator()(const std::string &left, const std::string &right) const {
        return left > right;
    }
    bool operator()(std::string_view left, std::string_view right) const {
        ++view_calls;
        return left > right;
    }
};

template <typename Compare>
struct custom_traits : detail::default_document_traits {
    using value_type = basic_document<custom_traits>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using object_type = std::map<string_type, value_type, Compare>;
    using array_type = wjr::vector<value_type>;
};

template <typename Document>
constexpr bool preserves_field_references =
    std::is_same_v<decltype(detail::document_field_at(
                       std::declval<typename Document::object_type &>(), "id")),
                   Document &> &&
    std::is_same_v<decltype(detail::document_field_at(
                       std::declval<const typename Document::object_type &>(), "id")),
                   const Document &>;
static_assert(preserves_field_references<document>);
static_assert(preserves_field_references<pmr_document>);
static_assert(preserves_field_references<basic_document<custom_traits<std::less<std::string>>>>);
static_assert(preserves_field_references<basic_document<custom_traits<reverse_compare>>>);
} // namespace

TEST(json_registered_fields, copy_and_move_preserve_short_keys_and_long_owned_values) {
    check_copy_and_move<document>();
    check_copy_and_move<pmr_document>();
}

TEST(json_registered_fields, transparent_pmr_map_does_not_materialize_lookup_keys) {
    auto source = parsed<pmr_document>(
        R"({"id":7,"descriptive_name_larger_than_small_string":"name","release_revision_larger_than_small_string":"v1"})");
    counting_resource resource;
    {
        default_resource_scope scope(&resource);
        const field_record copied(std::as_const(source));
        field_record assigned;
        assigned = std::as_const(source);
        EXPECT_EQ(copied.id, 7);
        EXPECT_EQ(assigned.release_revision_larger_than_small_string, "v1");
    }
    EXPECT_EQ(resource.calls, 0);
    {
        default_resource_scope scope(&resource);
        const field_record moved(std::move(source));
        EXPECT_EQ(moved.descriptive_name_larger_than_small_string, "name");
    }
    EXPECT_EQ(resource.calls, 0);
}

TEST(json_registered_fields, missing_field_retains_backend_exception_and_partial_assignment) {
    auto source = parsed<document>(R"({"id":7,"descriptive_name_larger_than_small_string":"new"})");
    std::string expected;
    try {
        (void)source.get<object_t>().at("release_revision_larger_than_small_string");
        FAIL();
    } catch (const std::out_of_range &error) {
        expected = error.what();
    }
    try {
        (void)field_record(std::as_const(source));
        FAIL();
    } catch (const std::out_of_range &error) {
        EXPECT_EQ(error.what(), expected);
    }
    field_record target;
    target.release_revision_larger_than_small_string = "unchanged";
    EXPECT_THROW(target = std::as_const(source), std::out_of_range);
    EXPECT_EQ(target.id, 7);
    EXPECT_EQ(target.descriptive_name_larger_than_small_string, "new");
    EXPECT_EQ(target.release_revision_larger_than_small_string, "unchanged");
}

TEST(json_registered_fields, missing_pmr_field_keeps_the_original_key_construction_path) {
    auto source =
        parsed<pmr_document>(R"({"id":7,"descriptive_name_larger_than_small_string":"new"})");
    counting_resource resource;
    {
        default_resource_scope scope(&resource);
        EXPECT_THROW((void)field_record(std::as_const(source)), std::out_of_range);
    }
    EXPECT_EQ(resource.calls, 1);
}

TEST(json_registered_fields, decoded_duplicate_fields_still_select_the_last_value) {
    const auto source = parsed<document>(
        R"({"id":1,"id":7,"descriptive_name_larger_than_small_string":"first","descriptive_name_larger_than_small_strin\u0067":"last","release_revision_larger_than_small_string":"v1"})");
    const field_record result(source);
    EXPECT_EQ(result.id, 7);
    EXPECT_EQ(result.descriptive_name_larger_than_small_string, "last");
}

TEST(json_registered_fields, custom_and_nontransparent_comparators_keep_backend_at) {
    check_copy_and_move<basic_document<custom_traits<std::less<std::string>>>>();
    reverse_compare::view_calls = 0;
    check_copy_and_move<basic_document<custom_traits<reverse_compare>>>();
    EXPECT_EQ(reverse_compare::view_calls, 0);
}
