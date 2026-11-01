#include <string>
#include <wjr/container/lru_cache.hpp>

int main() {
    wjr::lru_cache<int, std::string> cache(2);
    cache.try_emplace(1, "one");
    cache.try_emplace(2, "two");
    cache.get(1);
    cache.try_emplace(3, "three");
    if (cache.peek(2) || !cache.peek(1) || *cache.peek(1) != "one" || cache.size() != 2)
        return 1;
    cache.clear();
    return cache.empty() ? 0 : 2;
}
