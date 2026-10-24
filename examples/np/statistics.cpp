#include <iostream>
#include <limits>

#include <wjr/np/statistics.hpp>

int main() {
    namespace np = wjr::np;
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double x[] = {1, 2, nan, 3};
    const double y[] = {2, 4, 99, 6};
    using shape = np::extents<size_t, 4>;
    const np::tensor_view<const double, shape> a(x), b(y);
    const auto covariance = np::nancovariance(a, b);
    const auto correlation = np::nanpearson_correlation(a, b);
    std::cout << "sample covariance: " << covariance << '\n';
    std::cout << "Pearson correlation: " << correlation << '\n';
    return covariance == 2 && std::abs(correlation - 1) < 1e-14 ? 0 : 1;
}
