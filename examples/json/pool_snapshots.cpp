#include <iostream>
#include <memory_resource>
#include <string_view>
#include <utility>

#include <wjr/json/pmr.hpp>

int main() {
    using document = wjr::json::pmr_document;
    using allocator = document::allocator_type;

    std::pmr::unsynchronized_pool_resource storage;
    document active(std::allocator_arg, allocator(&storage));
    wjr::json::ondemand_reader reader;
    const std::string_view snapshots[] = {
        R"({"service":"orders","generation":1,"routes":["/orders"]})",
        R"({"service":"orders","generation":2,"routes":["/orders","/health"]})",
        R"({"service":"orders","generation":3,"routes":["/orders","/health","/metrics"]})",
    };

    for (const auto input : snapshots) {
        reader.read(input);
        auto incoming = document::parse_with_allocator(reader, allocator(&storage));
        if (!incoming) {
            std::cerr << "JSON error " << static_cast<unsigned>(incoming.error()) << '\n';
            return 1;
        }
        active = std::move(*incoming);
    }

    active.get<wjr::json::object_t>()["status"] = std::string_view("ready");
    std::cout << active.to_string() << '\n';
}
