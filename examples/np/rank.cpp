#include <array>
#include <iostream>
#include <limits>

#include <wjr/np/rank.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::extents<size_t, 2, 4>;
    const double values[] = {0, 2, 3, 2, 4, std::numeric_limits<double>::quiet_NaN(), 1, 4};
    std::array<double, 8> ranks;
    std::array<size_t, 4> workspace;
    np::tensor_view<double, shape> output(ranks.data());
    const np::tensor_view<const double, shape> input(values);
    np::rankdata_to<np::rank_method::average, np::rank_nan_policy::omit>(output, input, workspace,
                                                                         np::axes<1>);
    for (size_t row = 0; row != 2; ++row) {
        for (size_t column = 0; column != 4; ++column)
            std::cout << output(row, column) << ' ';
        std::cout << '\n';
    }
    // The same four indices are reused for both rows and for later calls.
    return output(0, 1) == 2.5 && output(1, 0) == 2.5 && output(1, 2) == 1 ? 0 : 1;
}
