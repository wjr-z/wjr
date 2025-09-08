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
    // todo: use executable path
    std::ifstream input(current_path / "build/test/units/data/success/twitter.json");
    std::stringstream buffer;
    buffer << input.rdbuf();
    return std::move(buffer).str();
}();

TEST(json, parse) {
    using namespace json;

    {
        ondemand_reader rd;
        rd.read(twitter_json);
        WJR_CHECK(check(rd).has_value());
        rd.read(twitter_json);
        auto ret = document::parse(rd);
        WJR_CHECK(ret.has_value());
        auto j = std::move(*ret);
        auto str = j.to_string();
        rd.read(str);
        ret = document::parse(rd);
        WJR_CHECK(ret.has_value());
        auto j2 = std::move(*ret);
        auto str2 = j2.to_string();
        WJR_CHECK(j == j2);
        WJR_CHECK(str == str2);
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
        WJR_CHECK((T)doc == expected);
        doc = expected;
        WJR_CHECK((T)doc == expected);
        T val(doc);
        WJR_CHECK(val == expected);
        doc.get_to(val);
        WJR_CHECK(val == expected);
    } while (0);

    do {
        T c0(expected);
        T c1(expected);

        document doc(std::move(c0));
        WJR_CHECK((T)doc == expected);
        doc = std::move(c1);
        WJR_CHECK((T)doc == expected);
        T val(std::move(doc));
        WJR_CHECK(val == expected);
        document doc2(expected);
        std::move(doc2).get_to(val);
        WJR_CHECK(val == expected);
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

            WJR_CHECK((std::string_view)a == "name");
            WJR_CHECK((std::string_view)b == "version");

            static_assert(!std::is_constructible_v<std::string_view, document &&>, "");

            std::string_view c(a);
            std::string d(b);
            std::string e(std::move(b));

            WJR_CHECK(c == "name");
            WJR_CHECK(d == "version");
            WJR_CHECK(e == "version");
        } while (false);

        do {
            std::map<std::string, std::string> a = {
                {"name", "wjr"}, {"version", "1.0.0"}, {"age", "22"}};

            document b(a);

            auto iter = a.begin();
            for (auto &[key, value] : b.template get<object_t>()) {
                WJR_CHECK((std::string_view)(key) == iter->first);
                WJR_CHECK((std::string_view)(value) == iter->second);
                ++iter;
            }
        } while (false);

        do {
            std::vector<int> a = {1, 2, 3, 0, 7, 3, 4, 6};
            document b(a);

            auto iter = a.begin();
            for (auto &elem : b.template get<array_t>()) {
                WJR_CHECK((int)(elem) == *iter);
                ++iter;
            }

            std::vector<document> c(b);

            iter = a.begin();
            for (auto &elem : c) {
                WJR_CHECK((int)(elem) == *iter);
                ++iter;
            }
        } while (false);

        do {
            std::string str = R"({"name" : "wjr", "version" : "1.0.0", "age" : 22})";
            ondemand_reader rd;
            rd.read(str);

            test_struct0 it(document::parse(rd).value());
            WJR_CHECK(it.name == "wjr");
            WJR_CHECK(it.version == "1.0.0");
            WJR_CHECK(it.age == 22);
        } while (false);
    }
    WJR_CATCH(...) { WJR_CHECK(false); }
}