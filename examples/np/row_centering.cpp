#include <array>
#include <iostream>

#include <wjr/np/reduce.hpp>
#include <wjr/np/subview.hpp>
#include <wjr/np/transform.hpp>

namespace np = wjr::np;

int main() {
    using shape = np::extents<size_t, 2, 3>;
    const std::array<double, 6> input{1, 2, 3, 4, 6, 8};
    std::array<double, 2> row_sums;
    std::array<double, 6> centered;
    np::tensor_view<const double, shape> source(input.data());
    np::tensor_view<double, np::extents<size_t, 2, 1>> sums(row_sums.data());
    np::tensor_view<double, shape> output(centered.data());

    // Materialize the reduction once in explicit, preallocated workspace.
    np::sum_to<double>(sums, source, np::axes<1>, np::keepdims);
    for (size_t row = 0; row != source.extent(0); ++row) {
        const double mean = sums(row, 0) / source.extent(1);
        np::transform_to(*np::try_row_view(output, row), *np::try_row_view(source, row),
                         [mean](double value) noexcept { return value - mean; });
    }

    std::cout << "row sums: " << sums(0, 0) << ' ' << sums(1, 0) << '\n';
    std::cout << "centered rows:\n";
    for (size_t row = 0; row != output.extent(0); ++row) {
        for (size_t column = 0; column != output.extent(1); ++column)
            std::cout << output(row, column) << ' ';
        std::cout << '\n';
    }
    const std::array<double, 6> expected{-1, 0, 1, -2, 0, 2};
    if (centered != expected)
        return 1;

    // Reuse the same output for squared residuals, then reduce to a scalar.
    np::transform_to(output, output, [](double value) noexcept { return value * value; });
    const double squared_residual = np::sum(output);
    std::cout << "sum of squared residuals: " << squared_residual << '\n';
    return squared_residual == 10 ? 0 : 1;
}
