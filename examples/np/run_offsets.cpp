#include <array>
#include <iostream>
#include <span>

#include <wjr/np/run_offsets.hpp>
#include <wjr/np/segmented_reduce.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::dextents<size_t, 1>;
    const int keys[] = {1, 1, 2, 2, 2, 1};
    const double values[] = {2., 3., 4., 5., 6., 7.};
    std::array<size_t, 7> offsets;
    const auto runs =
        np::run_offsets_to(np::tensor_view<size_t, shape>(offsets.data(), offsets.size()),
                           np::tensor_view<const int, shape>(keys, 6));
    std::array<double, 6> sums;
    np::segmented_sum_to(np::tensor_view<double, shape>(sums.data(), runs),
                         np::tensor_view<const double, shape>(values, 6),
                         std::span<const size_t>(offsets.data(), runs + 1));
    for (size_t i = 0; i != runs; ++i)
        std::cout << '[' << offsets[i] << ", " << offsets[i + 1] << "): " << sums[i] << '\n';
    return runs == 3 && sums[0] == 5. && sums[1] == 15. && sums[2] == 7. ? 0 : 1;
}
