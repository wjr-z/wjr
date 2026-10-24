#include <array>
#include <iostream>

#include <wjr/np/argsort.hpp>
#include <wjr/np/take.hpp>

int main() {
    namespace np = wjr::np;
    const double values[] = {4, 1, 4, 2};
    std::array<unsigned, 4> order;
    using shape = np::extents<size_t, 4>;
    np::argsort_to<np::sort_order::descending, np::sort_stability::stable>(
        np::tensor_view<unsigned, shape>(order.data()),
        np::tensor_view<const double, shape>(values));
    // Reuse the permutation for a separate array without sorting its values.
    const int labels[] = {10, 20, 30, 40};
    std::array<int, 4> reordered;
    np::take_to(np::tensor_view<int, shape>(reordered.data()),
                np::tensor_view<const int, shape>(labels),
                np::tensor_view<const unsigned, shape>(order.data()));
    for (size_t i = 0; i != order.size(); ++i)
        std::cout << values[order[i]] << ": " << reordered[i] << '\n';
    return order == std::array<unsigned, 4>{0, 2, 3, 1} && reordered == std::array{10, 30, 40, 20}
               ? 0
               : 1;
}
