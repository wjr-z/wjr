#include <cmath>
#include <iostream>

#include <wjr/np/variance.hpp>

int main() {
    namespace np = wjr::np;
    const double observations[] = {1, 2, 3, 4};
    const np::tensor_view<const double, np::extents<size_t, 4>> input(observations);
    const auto population = np::variance(input);
    const auto sample = np::variance(input, 1);
    std::cout << "population variance: " << population << '\n'
              << "sample variance: " << sample << '\n'
              << "sample standard deviation: " << std::sqrt(sample) << '\n';
    return population == 1.25 && sample == 5.0 / 3.0 ? 0 : 1;
}
