#include <array>
#include <iostream>

#include <wjr/np/expression_scan.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::extents<size_t, 2, 5>;
    const std::array<double, 10> samples{1, 2, 3, 4, 5, 2, 4, 6, 8, 10};
    const std::array<double, 2> baseline{1, 2};
    std::array<double, 10> prefix_storage;
    const auto x = np::as_expr(np::tensor_view<const double, shape>(samples.data()));
    const auto b =
        np::as_expr(np::tensor_view<const double, np::extents<size_t, 2, 1>>(baseline.data()));
    const auto residual = x - b;
    np::tensor_view<double, shape> prefix(prefix_storage.data());
    // Fuse squared residuals into each row's inclusive scan: no eval temporary.
    np::cumsum_to(prefix, residual * residual, np::axes<1>);

    constexpr size_t width = 3;
    const std::array<double, 6> expected{5, 14, 29, 20, 56, 116};
    for (size_t row = 0; row != 2; ++row) {
        std::cout << "row " << row << " width-3 squared residual totals:";
        for (size_t start = 0; start != 3; ++start) {
            const auto total =
                prefix(row, start + width - 1) - (start == 0 ? 0. : prefix(row, start - 1));
            if (total != expected[row * 3 + start])
                return 1;
            std::cout << ' ' << total;
        }
        std::cout << '\n';
    }
}
