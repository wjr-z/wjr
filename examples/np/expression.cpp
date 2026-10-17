#include <array>
#include <cstdio>

#include <wjr/np/expression.hpp>
#include <wjr/np/tensor.hpp>

namespace np = wjr::np;
using shape = np::extents<size_t, 2, 3>;

auto squared_difference(np::tensor_view<const double, shape> input, double center) {
    auto difference = np::as_expr(input) - center;
    return difference * difference;
}

int main() {
    const std::array<double, 6> first{1, 2, 3, 4, 5, 6};
    const std::array<double, 6> second{6, 5, 4, 3, 2, 1};
    np::tensor<double, shape> output;
    auto x = np::as_expr(np::tensor_view<const double, shape>(first.data()));
    auto y = np::as_expr(np::tensor_view<const double, shape>(second.data()));

    // One traversal writes directly into already allocated storage.
    np::eval_to(output.view(), 0.5 * x + 0.25 * y);
    for (size_t row = 0; row != 2; ++row) {
        for (size_t column = 0; column != 3; ++column)
            std::printf("%.2f ", output.view()(row, column));
        std::puts("");
    }
    // Local expression nodes are values. The original input storage stays alive.
    // Exact pointwise in-place evaluation needs no temporary tensor.
    np::eval_to(output.view(), squared_difference(output.view(), 2.0));
    return output.view()(0, 0) == 0 && output.view()(1, 2) == 1.5625 ? 0 : 1;
}
