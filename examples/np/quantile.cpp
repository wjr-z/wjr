#include <array>
#include <iostream>

#include <wjr/np/quantile.hpp>

int main() {
    namespace np = wjr::np;
    // Several requested ranks share the same partitioning work.
    int values[] = {9, 1, 7, 3, 5};
    const double probabilities[] = {.75, .25, .5};
    std::array<double, 3> result;
    std::array<size_t, 6> ranks;
    using source_shape = np::extents<size_t, 5>;
    using query_shape = np::extents<size_t, 3>;
    np::quantile_inplace_to(np::tensor_view<double, query_shape>(result.data()),
                            np::tensor_view<int, source_shape>(values),
                            np::tensor_view<const double, query_shape>(probabilities),
                            std::span(ranks));
    for (size_t i = 0; i != result.size(); ++i)
        std::cout << probabilities[i] << ": " << result[i] << '\n';
    return result == std::array<double, 3>{7, 3, 5} ? 0 : 1;
}
