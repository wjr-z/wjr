#include <array>
#include <iostream>

#include <wjr/np/evaluate.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::extents<size_t, 2, 3>;
    std::array<double, 6> samples{1., 2., 3., 4., 5., 6.};
    const auto input = np::as_expr(np::tensor_view<const double, shape>(samples.data()));
    const auto centered = input - 3.5;
    auto squares = np::eval(centered * centered);
    if (!squares)
        return 1;

    std::cout << "materialized squared deviations:\n";
    const auto values = squares->view();
    for (size_t row = 0; row != values.extent(0); ++row) {
        for (size_t column = 0; column != values.extent(1); ++column)
            std::cout << (column == 0 ? "" : " ") << values(row, column);
        std::cout << '\n';
    }
    // The result owns its elements. Further edits to samples do not change it.
    samples[0] = 100.;
    return values(0, 0) == 6.25 && values(1, 2) == 6.25 ? 0 : 1;
}
