#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

#include <wjr/np/segmented_reduce.hpp>

int main() {
    namespace np = wjr::np;
    const double values[] = {1, 2, 3, 4, 5, 6};
    const uint32_t offsets[] = {0, 2, 2, 6};
    const np::tensor_view<const double, np::extents<size_t, 6>> input(values);
    std::array<double, 3> sums, means, minima;
    using output = np::tensor_view<double, np::extents<size_t, 3>>;
    np::segmented_sum_to(output(sums.data()), input, std::span(offsets));
    np::segmented_mean_to(output(means.data()), input, std::span(offsets));
    np::segmented_reduce_to(
        output(minima.data()), input, std::span(offsets), std::numeric_limits<double>::infinity(),
        [](double total, double value) noexcept { return total < value ? total : value; });
    for (size_t group = 0; group != sums.size(); ++group)
        std::cout << "group " << group << ": sum=" << sums[group] << " mean=" << means[group]
                  << " min=" << minima[group] << '\n';
    return sums[0] == 3 && sums[1] == 0 && sums[2] == 18 && std::isnan(means[1]) &&
                   means[2] == 4.5 && std::isinf(minima[1])
               ? 0
               : 1;
}
