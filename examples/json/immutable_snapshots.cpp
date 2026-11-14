#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include <wjr/json/immutable_document.hpp>
#include <wjr/json/reader.hpp>

struct status_record {
    std::string text;
    std::string screen_name;
};

wjr::json::immutable_document load_snapshot(const char *path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open JSON input");
    }
    const std::string input((std::istreambuf_iterator<char>(file)), {});
    wjr::json::ondemand_reader reader;
    reader.read(input);
    auto parsed = wjr::json::immutable_document::parse(reader);
    if (!parsed) {
        throw std::runtime_error("JSON error " + std::to_string(int(parsed.error().code)) +
                                 " at byte " + std::to_string(parsed.error().offset));
    }
    return std::move(*parsed);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: json_immutable_snapshots twitter.json\n";
        return 1;
    }
    try {
        const auto snapshot = load_snapshot(argv[1]);
        const auto root = snapshot.root().get_object();
        if (!root) {
            throw std::runtime_error("Expected a root object");
        }
        const auto statuses_value = root->find("statuses");
        if (!statuses_value) {
            throw std::runtime_error("Missing statuses field");
        }
        const auto statuses = statuses_value->get_array();
        if (!statuses) {
            throw std::runtime_error("Expected a statuses array");
        }
        std::vector<status_record> records;
        records.reserve(statuses->size());
        for (const auto status : *statuses) {
            const auto object = status.get_object();
            if (!object) {
                throw std::runtime_error("Expected a status object");
            }
            const auto text = object->find("text");
            const auto user = object->find("user");
            if (!text || !user) {
                throw std::runtime_error("Missing status fields");
            }
            const auto user_object = user->get_object();
            if (!user_object) {
                throw std::runtime_error("Expected a user object");
            }
            const auto screen_name = user_object->find("screen_name");
            const auto text_string = text->get_string();
            if (!screen_name || !text_string) {
                throw std::runtime_error("Invalid status fields");
            }
            const auto name_string = screen_name->get_string();
            if (!name_string) {
                throw std::runtime_error("Expected a screen_name string");
            }
            records.push_back({std::string(*text_string), std::string(*name_string)});
        }
        size_t bytes = 0;
        for (const auto &record : records) {
            bytes += record.text.size() + record.screen_name.size();
        }
        std::cout << records.size() << " records, " << bytes << " decoded projected bytes, "
                  << snapshot.storage_size() << " packed document bytes\n";
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
