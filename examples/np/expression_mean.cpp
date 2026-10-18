#include <array>
#include <cmath>
#include <iostream>

#include <wjr/np/expression_mean.hpp>

int main() {
    namespace np = wjr::np;
    const std::array<double, 6> samples{1, 2, 3, 4, 5, 6};
    const auto a =
        np::as_expr(np::tensor_view<const double, np::extents<size_t, 2, 3>>(samples.data()));
    const auto mean_square = np::mean(a * a);
    const auto nonlinear =
        np::mean(np::map([](double value) noexcept { return value / (1 + value * value); }, a));
    double reference = 0;
    for (auto value : samples)
        reference += value / (1 + value * value);
    reference /= samples.size();
    std::cout << "mean square: " << mean_square << "\nmean bounded transform: " << nonlinear
              << '\n';
    return mean_square == 91.0 / 6 && std::abs(nonlinear - reference) < 1e-14 ? 0 : 1;
}
