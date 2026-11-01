#include <array>
#include <ranges>
#include <sstream>

#include <wjr/vector.hpp>

int main() {
    wjr::vector<int> queued{10, 40};
    std::istringstream incoming("20 30");
    auto first = queued.insert_range(queued.cbegin() + 1, std::ranges::istream_view<int>(incoming));
    if (first != queued.begin() + 1 || *first != 20 ||
        !std::ranges::equal(queued, std::array{10, 20, 30, 40}))
        return 1;
    auto batch =
        std::views::iota(50, 54) | std::views::filter([](int value) { return value % 2 == 0; });
    queued.insert_range(queued.cend(), batch);
    return std::ranges::equal(queued, std::array{10, 20, 30, 40, 50, 52}) ? 0 : 2;
}
