#include <array>
#include <cmath>
#include <iostream>

#include <wjr/np/expression.hpp>
#include <wjr/np/norm_axis.hpp>

int main() {
    namespace np = wjr::np;
    // Every row has a nonzero finite norm. Squaring the input directly would
    // overflow the first row and underflow the second row.
    std::array<double, 6> values{3e200, 4e200, 0, 0, 3e-200, 4e-200};
    std::array<double, 2> norms{};
    np::tensor_view<double, np::extents<size_t, 2, 3>> samples(values.data());
    np::tensor_view<double, np::extents<size_t, 2, 1>> lengths(norms.data());
    np::norm_l2_to(lengths, samples, np::axes<1>, np::keepdims);
    std::cout << "row norms: " << norms[0] << ' ' << norms[1] << '\n';

    // keepdims lets the existing expression evaluator broadcast the workspace.
    // The explicit nonzero-norm precondition avoids division by zero.
    np::eval_to(samples, np::as_expr(samples) / np::as_expr(lengths));
    std::cout << "normalized rows:";
    for (const auto value : values)
        std::cout << ' ' << value;
    std::cout << '\n';
    return std::abs(values[0] - 0.6) < 1e-14 && std::abs(values[1] - 0.8) < 1e-14 &&
                   std::abs(values[4] - 0.6) < 1e-14 && std::abs(values[5] - 0.8) < 1e-14
               ? 0
               : 1;
}
