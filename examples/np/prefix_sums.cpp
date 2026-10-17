#include <array>
#include <iostream>

#include <wjr/np/scan.hpp>

namespace np = wjr::np;

int main() {
    const std::array<double, 6> increments{1, 2, 3, 4, 5, 6};
    std::array<double, 6> prefix_storage;
    using line_shape = np::extents<size_t, 6>;
    np::tensor_view<const double, line_shape> input(increments.data());
    np::tensor_view<double, line_shape> prefix(prefix_storage.data());
    np::cumsum_to(prefix, input, np::axes<0>);

    // Inclusive prefix differences give a window total without summing it again.
    constexpr size_t width = 3;
    std::array<double, 4> windows;
    for (size_t start = 0; start != windows.size(); ++start)
        windows[start] = prefix(start + width - 1) - (start == 0 ? 0 : prefix(start - 1));
    std::cout << "inclusive prefix:";
    for (auto value : prefix_storage)
        std::cout << ' ' << value;
    std::cout << "\nwidth-3 window totals:";
    for (auto value : windows)
        std::cout << ' ' << value;
    std::cout << '\n';
    const std::array<double, 4> expected_windows{6, 9, 12, 15};
    if (windows != expected_windows)
        return 1;

    // Prefix columns, then prefix rows in place to form a 2D integral table.
    using matrix_shape = np::extents<size_t, 2, 3>;
    std::array<double, 6> integral_storage;
    np::tensor_view<const double, matrix_shape> matrix(increments.data());
    np::tensor_view<double, matrix_shape> integral(integral_storage.data());
    np::cumsum_to(integral, matrix, np::axes<0>);
    np::cumsum_to(integral, integral, np::axes<1>);
    std::cout << "2D integral table:\n";
    for (size_t row = 0; row != integral.extent(0); ++row) {
        for (size_t column = 0; column != integral.extent(1); ++column)
            std::cout << integral(row, column) << ' ';
        std::cout << '\n';
    }
    const std::array<double, 6> expected_integral{1, 3, 6, 5, 12, 21};
    return integral_storage == expected_integral ? 0 : 1;
}
