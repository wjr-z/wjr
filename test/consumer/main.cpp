#include <wjr/json.hpp>
#include <wjr/ring_buffer.hpp>

int main() {
    wjr::ring_buffer<int> queue;
    queue.push_back(7);
    queue.push_back(11);

    wjr::json::ondemand_reader reader;
    reader.read("{\"ok\":true}");
    auto document = wjr::json::document::parse(reader);

    if (!document.has_value() || queue.front() != 7 || queue.back() != 11) {
        return 1;
    }

    return document->to_string().empty() ? 1 : 0;
}
