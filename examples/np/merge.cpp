#include <array>
#include <iostream>

#include <wjr/np/merge.hpp>

int main() {
    namespace np = wjr::np;
    const std::array left{1, 3, 3, 8};
    const std::array right{2, 3, 7};
    std::array<int, 7> output;
    np::merge_sorted_to(np::tensor_view<int, np::extents<size_t, 7>>(output.data()),
                        np::tensor_view<const int, np::extents<size_t, 4>>(left.data()),
                        np::tensor_view<const int, np::extents<size_t, 3>>(right.data()));
    for (int value : output)
        std::cout << value << ' ';
    std::cout << '\n';
    return output == std::array{1, 2, 3, 3, 3, 7, 8} ? 0 : 1;
}
