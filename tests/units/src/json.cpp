#include "detail.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <wjr/json/document.hpp>

using namespace wjr;

namespace wjr::json {
static_assert(has_assign_from_document_v<document, const document &>);
static_assert(has_assign_from_document_v<document, document &&>);

static_assert(has_construct_from_document_v<document, const document &>);
static_assert(has_construct_from_document_v<document, document &&>);

static_assert(has_assign_to_document_v<document, const document &>);
static_assert(has_assign_to_document_v<document, document &&>);

static_assert(has_construct_to_document_v<document, const document &>);
static_assert(has_construct_to_document_v<document, document &&>);
} // namespace wjr::json

const auto current_path = std::filesystem::current_path();
const auto twitter_json = []() {
    std::ifstream input(current_path / "../src/data/success/twitter.json");
    std::stringstream buffer;
    buffer << input.rdbuf();
    return std::move(buffer).str();
}();

TEST(json, parse) {
    using namespace json;

    {
        reader rd(twitter_json);
        WJR_ASSERT_L0(check(rd).has_value());
        auto ret = document::parse(rd);
        WJR_ASSERT_L0(ret.has_value());
        auto &j = *ret;
        auto str = j.to_string();
        rd.read(str);
        ret = document::parse(rd);
        WJR_ASSERT_L0(ret.has_value());
        auto &j2 = *ret;
        auto str2 = j2.to_string();
        WJR_ASSERT_L0(j == j2);
        WJR_ASSERT_L0(str == str2);
    }
}

struct test_struct0 {
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(test_struct0);

    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(test_struct0, name, version, age)

    bool operator==(const test_struct0 &other) const noexcept {
        return age == other.age && name == other.name && version == other.version;
    }

    std::string name;
    std::string version;
    int age = 0;
};

template <typename T>
void json_constructor_test(const T &expected) {
    using namespace json;

    do {
        document doc(expected);
        WJR_ASSERT_L0((T)doc == expected);
        doc = expected;
        WJR_ASSERT_L0((T)doc == expected);
        T val(doc);
        WJR_ASSERT_L0(val == expected);
        doc.get_to(val);
        WJR_ASSERT_L0(val == expected);
    } while (0);

    do {
        T c0(expected);
        T c1(expected);

        document doc(std::move(c0));
        WJR_ASSERT_L0((T)doc == expected);
        doc = std::move(c1);
        WJR_ASSERT_L0((T)doc == expected);
        T val(std::move(doc));
        WJR_ASSERT_L0(val == expected);
        document doc2(expected);
        std::move(doc2).get_to(val);
        WJR_ASSERT_L0(val == expected);
    } while (0);
}

TEST(json, constructor) {
    using namespace json;

    WJR_TRY {
        json_constructor_test(nullptr);
        json_constructor_test(true);
        json_constructor_test(false);
        json_constructor_test(0u);
        json_constructor_test(-3);
        json_constructor_test(std::string("name"));
        json_constructor_test(std::vector<int>{1, 2, 3, 4, 5});
        json_constructor_test(test_struct0{});

        do {
            document a(std::string_view("name"));
            document b(std::string("version"));

            WJR_ASSERT_L0((std::string_view)a == "name");
            WJR_ASSERT_L0((std::string_view)b == "version");

            static_assert(!std::is_constructible_v<std::string_view, document &&>, "");

            std::string_view c(a);
            std::string d(b);
            std::string e(std::move(b));

            WJR_ASSERT_L0(c == "name");
            WJR_ASSERT_L0(d == "version");
            WJR_ASSERT_L0(e == "version");
        } while (false);

        do {
            std::map<std::string, std::string> a = {
                {"name", "wjr"}, {"version", "1.0.0"}, {"age", "22"}};

            document b(a);

            auto iter = a.begin();
            for (auto &[key, value] : b.template get<object_t>()) {
                WJR_ASSERT_L0((std::string_view)(key) == iter->first);
                WJR_ASSERT_L0((std::string_view)(value) == iter->second);
                ++iter;
            }
        } while (false);

        do {
            std::vector<int> a = {1, 2, 3, 0, 7, 3, 4, 6};
            document b(a);

            auto iter = a.begin();
            for (auto &elem : b.template get<array_t>()) {
                WJR_ASSERT_L0((int)(elem) == *iter);
                ++iter;
            }

            std::vector<document> c(b);

            iter = a.begin();
            for (auto &elem : c) {
                WJR_ASSERT_L0((int)(elem) == *iter);
                ++iter;
            }
        } while (false);

        do {
            std::string str = R"({"name" : "wjr", "version" : "1.0.0", "age" : 22})";
            reader rd(str);

            test_struct0 it(document::parse(rd).value());
            WJR_ASSERT_L0(it.name == "wjr");
            WJR_ASSERT_L0(it.version == "1.0.0");
            WJR_ASSERT_L0(it.age == 22);
        } while (false);
    }
    WJR_CATCH(...) { WJR_ASSERT_L0(false); }
}