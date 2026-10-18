#include <array>
#include <iostream>

#include <wjr/np/difference.hpp>

int main() {
    namespace np = wjr::np;
    const double samples[] = {1, 4, 9, 16, 2, 5, 10, 17};
    const np::tensor_view<const double, np::extents<size_t, 2, 4>> input(samples);
    std::array<double, 6> changes;
    const np::tensor_view<double, np::extents<size_t, 2, 3>> output(changes.data());
    np::diff_to(output, input, np::axes<1>);
    for (size_t row = 0; row != 2; ++row) {
        for (size_t col = 0; col != 3; ++col)
            std::cout << (col == 0 ? "" : " ") << output(row, col);
        std::cout << '\n';
    }
    return changes == std::array{3., 5., 7., 3., 5., 7.} ? 0 : 1;
}
