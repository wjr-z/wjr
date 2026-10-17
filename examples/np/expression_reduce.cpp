#include <array>
#include <iostream>

#include <wjr/np/expression_reduce.hpp>

int main() {
    namespace np = wjr::np;
    std::array<double, 6> input{1, 2, 3, 4, 6, 8};
    std::array<double, 2> means{}, row_energy{};
    using matrix = np::extents<size_t, 2, 3>;
    using retained_axis = np::extents<size_t, 2, 1>;
    using rows = np::extents<size_t, 2>;
    const auto x = np::as_expr(np::tensor_view<const double, matrix>(input.data()));
    const np::tensor_view<double, retained_axis> mean_view(means.data());

    // Only the small, caller-owned row workspace is materialized.
    np::sum_to(mean_view, x, np::axes<1>, np::keepdims);
    np::eval_to(mean_view, np::as_expr(mean_view) / 3.);
    const auto difference = x - np::as_expr(mean_view);

    // Pointwise subtraction, multiplication and reduction share one traversal.
    // The row accumulator and final scalar never need a matrix-sized temporary.
    const double total = np::sum(difference * difference);
    np::sum_to(np::tensor_view<double, rows>(row_energy.data()), difference * difference,
               np::axes<1>);

    std::cout << "row means: " << means[0] << ' ' << means[1] << '\n';
    std::cout << "row squared residuals: " << row_energy[0] << ' ' << row_energy[1] << '\n';
    std::cout << "sum of squared residuals: " << total << '\n';
    return means == std::array{2., 6.} && row_energy == std::array{2., 8.} && total == 10. ? 0 : 1;
}
