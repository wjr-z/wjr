#include <wjr/json/yyjson.hpp>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace json = wjr::json::yyjson;

/** @brief Application records own their strings independently of the parsed document. */
struct status_record {
    uint64_t id, user_id;
    std::string text, user_name;
};

bool project(json::value_view source, status_record &output) {
    json::object_view status, user;
    std::string_view text, name;
    if (!source.try_get_object(status) || !status.find_last("id").try_get_uint(output.id) ||
        !status.find_last("text").try_get_string(text) ||
        !status.find_last("user").try_get_object(user) ||
        !user.find_last("id").try_get_uint(output.user_id) ||
        !user.find_last("name").try_get_string(name))
        return false;
    output.text.assign(text);
    output.user_name.assign(name);
    return true;
}

int main(int argc, char **argv) {
    std::string input;
    if (argc > 1) {
        std::ifstream file(argv[1], std::ios::binary);
        if (!file) {
            std::cerr << "cannot open input\n";
            return 1;
        }
        input.assign(std::istreambuf_iterator<char>(file), {});
    } else {
        input = R"({"statuses":[{"id":1,"text":"first","user":{"id":11,"name":"one"}},)"
                R"({"id":2,"text":"second","user":{"id":12,"name":"two"}}]})";
    }
    yyjson_read_err error{};
    auto document = json::document::parse(input, &error);
    if (!document) {
        std::cerr << "parse error " << error.code << " at byte " << error.pos << ": " << error.msg
                  << '\n';
        return 2;
    }
    input.clear();
    input.shrink_to_fit();
    json::object_view root;
    json::array_view statuses;
    if (!document.root().try_get_object(root) ||
        !root.find_last("statuses").try_get_array(statuses)) {
        std::cerr << "expected a statuses array\n";
        return 3;
    }
    std::vector<status_record> records;
    records.reserve(statuses.size());
    for (auto value : statuses) {
        if (!project(value, records.emplace_back())) {
            std::cerr << "status fields have the wrong schema\n";
            return 4;
        }
    }
    document.reset();
    uint64_t checksum = 0;
    for (const auto &record : records)
        checksum += record.id + record.user_id + record.text.size() + record.user_name.size();
    std::cout << "records=" << records.size() << " checksum=" << checksum << '\n';
    return 0;
}
