#include <wjr/json/optional.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

struct reply {
    std::optional<std::int64_t> in_reply_to_status_id;
    std::optional<std::string> in_reply_to_screen_name;
    WJR_REGISTER_DOCUMENT_OBJECT_SERIALIZER(reply, in_reply_to_status_id, in_reply_to_screen_name)
};

int main() {
    wjr::json::ondemand_reader reader;
    reader.read(R"([{"in_reply_to_status_id":null,"in_reply_to_screen_name":"alice"},
                   {"in_reply_to_status_id":42,"in_reply_to_screen_name":null}])");
    auto parsed = wjr::json::document::parse(reader);
    if (!parsed) {
        return 1;
    }
    const auto output = wjr::json::value_to<std::vector<reply>>(*parsed);
    if (output.size() != 2 || output[0].in_reply_to_status_id ||
        output[0].in_reply_to_screen_name != "alice" || output[1].in_reply_to_status_id != 42 ||
        output[1].in_reply_to_screen_name) {
        return 2;
    }
    return wjr::json::document(output) == *parsed ? 0 : 3;
}
