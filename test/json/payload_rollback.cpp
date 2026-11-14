#include <array>
#include <map>
#include <string>

#include <gtest/gtest.h>

#include <wjr/json/document.hpp>

namespace {

struct construction_failure {};

struct throwing_compare {
    static inline bool fail_default = false;
    static inline bool fail_copy = false;

    throwing_compare() {
        if (fail_default) {
            throw construction_failure{};
        }
    }

    throwing_compare(const throwing_compare &) {
        if (fail_copy) {
            throw construction_failure{};
        }
    }

    bool operator()(const std::string &left, const std::string &right) const noexcept {
        return left < right;
    }
};

template <typename Key, typename Value, typename>
using throwing_object = std::map<Key, Value, throwing_compare>;

using throwing_document = wjr::json::basic_document<
    wjr::json::detail::basic_document_traits<std::basic_string, throwing_object, wjr::vector>>;

TEST(json_payload_rollback, failed_object_copy_preserves_source) {
    throwing_document source;
    source.emplace_object();
    source.get<wjr::json::object_t>().emplace("answer", throwing_document(42));

    throwing_compare::fail_copy = true;
    EXPECT_THROW((throwing_document(source)), construction_failure);
    throwing_compare::fail_copy = false;

    const auto &object = source.get<wjr::json::object_t>();
    ASSERT_EQ(object.size(), 1);
    EXPECT_EQ(object.at("answer").get<wjr::json::number_signed_t>(), 42);
    throwing_document copy(source);
    EXPECT_EQ(copy.get<wjr::json::object_t>().size(), 1);
}

TEST(json_payload_rollback, failed_object_emplacement_leaves_reusable_null) {
    throwing_document value(42);
    throwing_compare::fail_default = true;
    EXPECT_THROW(value.emplace_object(), construction_failure);
    throwing_compare::fail_default = false;

    EXPECT_TRUE(value.is_null());
    value.emplace_object();
    EXPECT_TRUE(value.is_object());
    EXPECT_TRUE(value.get<wjr::json::object_t>().empty());
}

TEST(json_payload_rollback, failed_type_change_preserves_owned_destination) {
    throwing_document source;
    source.emplace_object();
    source.get<wjr::json::object_t>().emplace("answer", throwing_document(42));

    throwing_document string_value(std::string(96, 'x'));
    throwing_document array_value;
    array_value.emplace_array();
    array_value.get<wjr::json::array_t>().emplace_back(17);

    throwing_compare::fail_copy = true;
    EXPECT_THROW(string_value = source, construction_failure);
    EXPECT_THROW(array_value = source, construction_failure);
    throwing_compare::fail_copy = false;

    EXPECT_EQ(string_value.get<wjr::json::string_t>(), std::string(96, 'x'));
    const auto &array = array_value.get<wjr::json::array_t>();
    ASSERT_EQ(array.size(), 1);
    EXPECT_EQ(array.front().get<wjr::json::number_signed_t>(), 17);

    string_value = source;
    array_value = source;
    EXPECT_EQ(string_value.get<wjr::json::object_t>().size(), 1);
    EXPECT_EQ(array_value.get<wjr::json::object_t>().size(), 1);
}

TEST(json_payload_rollback, copy_assignment_covers_all_value_type_pairs) {
    using namespace wjr::json;
    std::array<document, 8> values;
    values[1] = true;
    values[2] = int64_t(-17);
    values[3] = uint64_t(23);
    values[4] = 3.5;
    values[5] = std::string(96, 'x');
    values[6].emplace_array();
    values[6].get<array_t>().emplace_back(17);
    values[7].emplace_object();
    values[7]["answer"] = 42;

    for (const auto &source : values) {
        const auto expected = source.dump();
        for (const auto &initial : values) {
            document destination(initial);
            destination = source;
            EXPECT_EQ(destination.dump(), expected);
            EXPECT_EQ(source.dump(), expected);
        }
    }
}

} // namespace
