#include "detail.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <wjr/json/json.hpp>

using namespace wjr;

const auto current_path = std::filesystem::current_path();
const auto twitter_json = []() {
    std::ifstream input(current_path / "..\\src\\data\\success\\twitter.json");
    std::stringstream buffer;
    buffer << input.rdbuf();
    return std::move(buffer).str();
}();

TEST(json, json) {
    {
        json::reader rd(twitter_json);
        WJR_ASSERT(json::check(rd).has_value());
        auto ret = json::json::parse(rd);
        WJR_ASSERT(ret.has_value());
        auto &j = *ret;
        WJR_ASSERT(j.is_object());
    }
}

TEST(json, constructor) {
    WJR_TRY {
        do {
            json::json j(nullptr);
            WJR_ASSERT(j.is_null());
            std::nullptr_t a(j);
            WJR_ASSERT(a == nullptr);
        } while (0);

        do {
            json::json a(true);
            json::json b(false);

            WJR_ASSERT(a.template get<json::boolean_t>() == true);
            WJR_ASSERT(b.template get<json::boolean_t>() == false);

            bool c(a);
            bool d(b);

            WJR_ASSERT(c == true);
            WJR_ASSERT(d == false);
        } while (0);

        do {
            json::json a(0u);
            json::json b(-3);

            WJR_ASSERT((int)a == 0);
            WJR_ASSERT((int)b == -3);

            int c(a);
            int d(b);

            WJR_ASSERT(c == 0);
            WJR_ASSERT(d == -3);
        } while (0);

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
        } while (0);

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
        } while (0);

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
        } while (0);
    }
    WJR_CATCH(...) { WJR_ASSERT_L0(false); }
}