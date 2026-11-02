#include <wjr/json/pmr.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <map>
#include <memory>
#include <memory_resource>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace json_destroy_test {
enum class kind { scalar, string, array, object };
struct source {
    kind type;
    int64_t value;
};
inline int calls = 0;

struct resource final : std::pmr::memory_resource {
    size_t live = 0;
    void *do_allocate(size_t bytes, size_t alignment) override {
        auto *pointer = std::pmr::new_delete_resource()->allocate(bytes, alignment);
        ++live;
        return pointer;
    }
    void do_deallocate(void *pointer, size_t bytes, size_t alignment) override {
        --live;
        std::pmr::new_delete_resource()->deallocate(pointer, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
};
} // namespace json_destroy_test

namespace wjr::json {
template <>
struct document_serializer_impl<json_destroy_test::source> {
    using value_type = json_destroy_test::source;
    template <typename Document>
    static void assign(value_type value, Document &output, in_place_document_serializer_to_t tag) {
        output = construct<Document>(value, tag);
    }
    template <typename Document>
    static Document construct(json_destroy_test::source value, in_place_document_serializer_to_t) {
        ++json_destroy_test::calls;
        switch (value.type) {
        case json_destroy_test::kind::scalar:
            return Document(number_signed_t{}, value.value);
        case json_destroy_test::kind::string:
            return Document(std::string(96, 'x'));
        case json_destroy_test::kind::array:
            return Document(std::vector<int64_t>{value.value, value.value + 1});
        case json_destroy_test::kind::object:
            return Document(std::map<std::string, int64_t>{{"value", value.value}});
        }
        WJR_UNREACHABLE();
    }
};
} // namespace wjr::json

namespace {
constexpr std::array kinds{json_destroy_test::kind::scalar, json_destroy_test::kind::string,
                           json_destroy_test::kind::array, json_destroy_test::kind::object};

template <typename Document>
void check_value(const Document &value, json_destroy_test::kind kind) {
    using namespace wjr::json;
    switch (kind) {
    case json_destroy_test::kind::scalar:
        EXPECT_EQ(value.template get<number_signed_t>(), 37);
        break;
    case json_destroy_test::kind::string:
        EXPECT_EQ(value.template get<string_t>().size(), 96);
        break;
    case json_destroy_test::kind::array:
        EXPECT_EQ(value.template get<array_t>().size(), 2);
        break;
    case json_destroy_test::kind::object:
        EXPECT_EQ(static_cast<int64_t>(value.at("value")), 37);
        break;
    }
}

TEST(json_destroy_dispatch, generic_construction_keeps_custom_serializer_kind_and_side_effects) {
    for (auto kind : kinds) {
        const int before = json_destroy_test::calls;
        wjr::json::document value(json_destroy_test::source{kind, 37});
        EXPECT_EQ(json_destroy_test::calls, before + 1);
        check_value(value, kind);
        auto moved = std::move(value);
        EXPECT_TRUE(value.is_null());
        check_value(moved, kind);
        moved.reset();
        EXPECT_TRUE(moved.is_null());
    }
}

TEST(json_destroy_dispatch, pmr_custom_materialization_reset_and_destruction_release_every_kind) {
    using namespace wjr::json;
    json_destroy_test::resource resource;
    for (const bool reset : {false, true}) {
        for (auto kind : kinds) {
            const int before = json_destroy_test::calls;
            {
                pmr_document value(std::allocator_arg, &resource,
                                   json_destroy_test::source{kind, 37});
                EXPECT_EQ(json_destroy_test::calls, before + 1);
                check_value(value, kind);
                if (kind != json_destroy_test::kind::scalar)
                    EXPECT_GT(resource.live, 0);
                if (reset) {
                    value.reset();
                    EXPECT_TRUE(value.is_null());
                }
            }
            EXPECT_EQ(resource.live, 0);
        }
    }
}
} // namespace
