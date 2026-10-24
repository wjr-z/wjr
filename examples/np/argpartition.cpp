#include <array>
#include <iostream>

#include <wjr/np/argpartition.hpp>
#include <wjr/np/take.hpp>

int main() {
    namespace np = wjr::np;
    const double values[] = {4, 1, 9, 2, 7, 3};
    std::array<unsigned, 6> permutation;
    const size_t rank = 2;
    using shape = np::extents<size_t, 6>;
    np::argpartition_to<np::sort_order::descending>(
        np::tensor_view<unsigned, shape>(permutation.data()),
        np::tensor_view<const double, shape>(values), std::span<const size_t>(&rank, 1));
    // The first three indices select the largest values, without sorting them.
    // Reuse that prefix to gather values or matching records from other arrays.
    std::array<double, 3> largest;
    np::take_to(np::tensor_view<double, np::extents<size_t, 3>>(largest.data()),
                np::tensor_view<const double, shape>(values),
                np::tensor_view<const unsigned, np::extents<size_t, 3>>(permutation.data()));
    for (auto value : largest)
        std::cout << value << ' ';
    std::cout << '\n';
    return values[permutation[rank]] == 4 && largest[0] >= 4 && largest[1] >= 4 && largest[2] >= 4
               ? 0
               : 1;
}
