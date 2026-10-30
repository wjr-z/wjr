#include <wjr/json.hpp>
#include <wjr/json/pmr.hpp>
#include <wjr/ring_buffer.hpp>

#include <memory_resource>

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

    std::pmr::unsynchronized_pool_resource pool;
    reader.read("{\"message\":\"ready\"}");
    auto pooled = wjr::json::pmr_document::parse_with_allocator(
        reader, wjr::json::pmr_document::allocator_type(&pool));
    if (!pooled || pooled->get_allocator().resource() != &pool ||
        pooled->at("message").get<wjr::json::string_t>().get_allocator().resource() != &pool) {
        return 1;
    }

    return document->to_string().empty() || pooled->to_string().empty() ? 1 : 0;
}
