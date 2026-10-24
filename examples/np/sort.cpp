#include <array>
#include <iostream>

#include <wjr/np/sort.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::extents<size_t, 6>;
    const int input[] = {4, 1, 3, 1, 2, 4};
    std::array<int, 6> sorted;
    const np::tensor_view<int, shape> output(sorted.data());
    np::sort_to(output, np::tensor_view<const int, shape>(input));
    for (auto value : sorted)
        std::cout << value << ' ';
    std::cout << '\n';
    if (sorted != std::array{1, 1, 2, 3, 4, 4})
        return 1;
    np::sort_inplace<np::sort_order::descending>(output);
    return sorted == std::array{4, 4, 3, 2, 1, 1} ? 0 : 1;
}
