#include <array>
#include <iostream>

#include <wjr/np/argsort.hpp>

int main() {
    namespace np = wjr::np;
    const double values[] = {4, 1, 4, 2};
    std::array<unsigned, 4> order;
    using shape = np::extents<size_t, 4>;
    np::argsort_to<np::sort_order::descending, np::sort_stability::stable>(
        np::tensor_view<unsigned, shape>(order.data()),
        np::tensor_view<const double, shape>(values));
    for (auto index : order)
        std::cout << index << ": " << values[index] << '\n';
    return order == std::array<unsigned, 4>{0, 2, 3, 1} ? 0 : 1;
}
