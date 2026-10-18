#include <array>
#include <iostream>

#include <wjr/np/expression_reduce.hpp>
#include <wjr/np/slice.hpp>
#include <wjr/np/strided_tensor.hpp>

int main() {
    namespace np = wjr::np;
    using owner = np::tensor<double, np::extents<size_t, 3, 6>, np::layout_stride>;
    auto matrix =
        np::make_tensor<owner>(std::array{3, 6}, std::array{8, 1}, wjr::default_construct);
    if (!matrix)
        return 1;
    // Write logical values only; padding is never read or initialized.
    for (size_t row = 0; row != 3; ++row)
        for (size_t col = 0; col != 6; ++col)
            matrix->view()(row, col) = static_cast<double>(10 * row + col);

    // extent=5 covers source columns [1,6); step=2 selects columns 1,3,5.
    auto selected = np::slice_view(matrix->view(), std::pair{1, 3}, np::strided_slice{1, 5, 2});
    const auto input = np::as_expr(selected);
    np::eval_to(selected, input * 2.0 + 1.0);
    const auto total = np::sum(np::as_expr(selected));
    std::cout << "selected shape: " << selected.extent(0) << 'x' << selected.extent(1)
              << ", strides: " << selected.stride(0) << ',' << selected.stride(1)
              << ", total: " << total << '\n';
    return total == 222.0 && matrix->view()(1, 2) == 12.0 ? 0 : 2;
}
