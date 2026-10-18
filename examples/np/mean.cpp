#include <array>
#include <iostream>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression.hpp>
#include <wjr/np/mean.hpp>

int main() {
    namespace np = wjr::np;
    const double values[] = {1, 3, 5, 4, 8, 12};
    np::tensor_view<const double, np::extents<size_t, 2, 3>> input(values);
    std::array<double, 2> row_means;
    np::tensor_view<double, np::extents<size_t, 2, 1>> means(row_means.data());
    np::mean_to(means, input, np::axes<1>, np::keepdims);

    // The small explicit workspace broadcasts over columns. eval allocates
    // the centered result without a redundant value-initialization pass.
    auto centered = np::eval(np::as_expr(input) - np::as_expr(means));
    if (!centered)
        return 1;
    std::cout << "overall mean: " << np::mean(input) << '\n';
    for (size_t row = 0; row != 2; ++row) {
        std::cout << "row " << row << ", mean " << means(row, 0) << ":";
        for (size_t column = 0; column != 3; ++column)
            std::cout << ' ' << centered->view()(row, column);
        std::cout << '\n';
    }
    return means(0, 0) == 3 && means(1, 0) == 8 && centered->view()(1, 2) == 4 ? 0 : 1;
}
