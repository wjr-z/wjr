#include "detail.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <wjr/json/json.hpp>

using namespace wjr;

const auto current_path = std::filesystem::current_path();
const auto twitter_json = []() {
    std::ifstream input(current_path / "../src/data/success/twitter.json");
    std::stringstream buffer;
    buffer << input.rdbuf();
    return std::move(buffer).str();
}();

TEST(json, parse) {
    {
        json::reader rd(twitter_json);
        WJR_ASSERT(json::check(rd).has_value());
        auto ret = json::json::parse(rd);
        WJR_ASSERT(ret.has_value());
        auto &j = *ret;
        auto str = j.to_string();
        rd.read(str);
        ret = json::json::parse(rd);
        WJR_ASSERT(ret.has_value());
        auto& j2 = *ret;
        auto str2 = j2.to_string();
        WJR_ASSERT(j == j2);
        WJR_ASSERT(str == str2);
    }
}

struct test_struct0 {
    WJR_REGISTER_FROM_JSON_SERIALIZER_DEFAULT(test_struct0, name, version, age)

    std::string name;
    std::string version;
    int age;
};

TEST(json, constructor) {
    WJR_TRY {
        do {
            json::json j(nullptr);
            WJR_ASSERT(j.is_null());
            std::nullptr_t a(j);
            WJR_ASSERT(a == nullptr);
        } while (false);

        do {
            json::json a(true);
            json::json b(false);

            WJR_ASSERT(a.template get<json::boolean_t>() == true);
            WJR_ASSERT(b.template get<json::boolean_t>() == false);

            bool c(a);
            bool d(b);

            WJR_ASSERT(c == true);
            WJR_ASSERT(d == false);
        } while (false);

        do {
            json::json a(0u);
            json::json b(-3);

            WJR_ASSERT((int)a == 0);
            WJR_ASSERT((int)b == -3);

            int c(a);
            int d(b);

            WJR_ASSERT(c == 0);
            WJR_ASSERT(d == -3);
        } while (false);

        do {
            json::json a(std::string_view("name"));
            json::json b(std::string("version"));

            WJR_ASSERT((std::string_view)a == "name");
            WJR_ASSERT((std::string_view)b == "version");

            static_assert(!std::is_constructible_v<std::string_view, json::json &&>, "");

            std::string_view c(a);
            std::string d(b);
            std::string e(std::move(b));

            WJR_ASSERT(c == "name");
            WJR_ASSERT(d == "version");
            WJR_ASSERT(e == "version");
        } while (false);

        do {
            std::map<std::string, std::string> a = {
                {"name", "wjr"}, {"version", "1.0.0"}, {"age", "22"}};

            json::json b(a);

            auto iter = a.begin();
            for (auto &[key, value] : b.template get<json::object_t>()) {
                WJR_ASSERT((std::string_view)(key) == iter->first);
                WJR_ASSERT((std::string_view)(value) == iter->second);
                ++iter;
            }
        } while (false);

        do {
            std::vector<int> a = {1, 2, 3, 0, 7, 3, 4, 6};
            json::json b(a);

            auto iter = a.begin();
            for (auto &elem : b.template get<json::array_t>()) {
                WJR_ASSERT((int)(elem) == *iter);
                ++iter;
            }

            std::vector<json::json> c(b);

            iter = a.begin();
            for (auto &elem : c) {
                WJR_ASSERT((int)(elem) == *iter);
                ++iter;
            }
        } while (false);

        do {
            std::string str = R"({"name" : "wjr", "version" : "1.0.0", "age" : 22})";
            json::reader rd(str);

            test_struct0 it(json::json::parse(rd).value());
            WJR_ASSERT(it.name == "wjr");
            WJR_ASSERT(it.version == "1.0.0");
            WJR_ASSERT(it.age == 22);
        } while (false);
    }
    WJR_CATCH(...) { WJR_ASSERT_L0(false); }
}