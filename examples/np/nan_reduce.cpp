#include <array>
#include <cmath>
#include <iostream>
#include <limits>

#include <wjr/np/nan_reduce.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::extents<size_t, 5>;
    const std::array observations{1.0, std::numeric_limits<double>::quiet_NaN(), 3.0,
                                  std::numeric_limits<double>::quiet_NaN(), 5.0};
    const np::tensor_view<const double, shape> source(observations.data());
    const auto total = np::nansum(source);
    const auto average = np::nanmean(source);
    std::cout << "nansum: " << total << "\nnanmean: " << average << '\n';
    return total == 9.0 && average == 3.0 && !std::isnan(average) ? 0 : 1;
}
