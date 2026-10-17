#include <algorithm>
#include <ranges>
#include <span>

#include <wjr/container/vector.hpp>
#include <wjr/ring_buffer.hpp>

int main() {
    wjr::ring_buffer<int> queue;
    queue.push_back(7);
    queue.push_back(11);
    queue.pop_front();
    static_assert(std::same_as<decltype(queue.array_one()), std::ranges::subrange<int *>>);

    wjr::vector<int> values{3, 1, 2};
    static_assert(std::ranges::contiguous_range<decltype(values)>);
    std::ranges::sort(values);
    const std::span<int> view(values);
    return queue.front() == 11 && queue.size() == 1 && view.front() == 1 ? 0 : 1;
}
