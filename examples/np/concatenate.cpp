#include <array>
#include <iostream>

#include <wjr/np/concatenate.hpp>

int main() {
    namespace np = wjr::np;
    const double features[] = {1, 2, 3, 4};
    const double extra_column[] = {10, 20};
    std::array<double, 6> output;
    np::concatenate_to(np::tensor_view<double, np::extents<size_t, 2, 3>>(output.data()),
                       np::axes<1>,
                       np::tensor_view<const double, np::extents<size_t, 2, 2>>(features),
                       np::tensor_view<const double, np::extents<size_t, 2, 1>>(extra_column));
    for (size_t row = 0; row != 2; ++row) {
        for (size_t col = 0; col != 3; ++col)
            std::cout << output[row * 3 + col] << ' ';
        std::cout << '\n';
    }
    return output == std::array{1.0, 2.0, 10.0, 3.0, 4.0, 20.0} ? 0 : 1;
}
