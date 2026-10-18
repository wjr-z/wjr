#include <array>
#include <iostream>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression_reduce.hpp>

int main() {
    namespace np = wjr::np;
    using panel = np::extents<size_t, 2, 3>;
    using feature = np::extents<size_t, 1, 3>;
    const std::array samples{1.0, 4.0, 10.0, 5.0, 8.0, 14.0};
    const std::array centers{3.0, 6.0, 12.0};
    const std::array inverse_scales{0.5, 0.25, 1.0};
    // Parameters are explicit operands: no opaque capture state to reload.
    // Shapes broadcast once; normalization and clipping fuse into one pass.
    const auto normalized = np::map(
        [](double x, double center, double inverse_scale, double limit) noexcept {
            const auto value = (x - center) * inverse_scale;
            return value < -limit ? -limit : (value > limit ? limit : value);
        },
        np::tensor_view<const double, panel>(samples.data()),
        np::tensor_view<const double, feature>(centers.data()),
        np::tensor_view<const double, feature>(inverse_scales.data()), 1.5);
    std::array<double, 6> output{};
    np::eval_to(np::tensor_view<double, panel>(output.data()), normalized);
    const std::array expected{-1.0, -0.5, -1.5, 1.0, 0.5, 1.5};
    if (output != expected)
        return 1;
    // Terminal consumers read the same lazy expression without a temporary.
    const auto energy = np::sum(np::map([](double x) noexcept { return x * x; }, normalized));
    std::cout << "clipped normalized panel energy: " << energy << '\n';
    return energy == 7.0 ? 0 : 2;
}
