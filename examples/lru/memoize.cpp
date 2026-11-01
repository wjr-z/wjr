#include <cstddef>
#include <functional>
#include <iostream>
#include <memory_resource>
#include <string>
#include <string_view>
#include <utility>

#include <wjr/container/lru_cache.hpp>

struct text_hash {
    using is_transparent = void;
    size_t operator()(std::string_view text) const noexcept {
        return std::hash<std::string_view>{}(text);
    }
};

int main() {
    wjr::lru_cache<std::string, std::string, text_hash, std::equal_to<>> results(2);
    size_t computations = 0;
    const auto resolve = [&](std::string_view name) -> std::string {
        if (const auto *cached = results.get(name))
            return *cached;
        ++computations;
        std::string rendered = "rendered:" + std::string(name);
        return *results.try_emplace(std::string(name), std::move(rendered)).first;
    };

    const auto first = resolve("orders");
    resolve("users");
    resolve("orders");
    resolve("health");
    if (computations != 3 || results.peek("users") || !results.peek("orders"))
        return 1;
    std::cout << first << "; computations=" << computations << '\n';

    std::pmr::unsynchronized_pool_resource resource;
    using allocator = std::pmr::polymorphic_allocator<std::pair<const int, std::pmr::string>>;
    wjr::lru_cache<int, std::pmr::string, std::hash<int>, std::equal_to<int>, allocator> pooled(
        2, {}, {}, allocator(&resource));
    const auto value = pooled.try_emplace(7, 80, 'x');
    if (value.first->get_allocator().resource() != &resource)
        return 2;
    pooled.clear();
}
