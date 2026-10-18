#include <array>
#include <cmath>
#include <iostream>

#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_reduce.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::extents<size_t, 4>;
    const std::array<double, 4> input{-1., 0., 3., 4.};
    std::array<double, 4> magnitude{}, output{};
    const auto x = np::as_expr(np::tensor_view<const double, shape>(input.data()));
    const np::tensor_view<double, shape> magnitudes(magnitude.data()), out(output.data());
    np::eval_to(magnitudes, np::abs(x));
    const double total = np::sum(magnitudes);
    if (total == 0.)
        return 1;
    const auto print = [&](const char *label) {
        std::cout << label;
        for (auto value : output)
            std::cout << ' ' << value;
        std::cout << '\n';
    };
    np::eval_to(out, np::sqrt(np::as_expr(magnitudes) / total));
    print("rooted relative magnitudes:");
    np::eval_to(out, np::exp(-np::as_expr(magnitudes)));
    print("magnitude decay:");
    np::eval_to(out, np::log(np::as_expr(magnitudes) + 1.));
    print("log(1+magnitude):");

    // Pass raw inputs. A log child passed to map would run before its callback;
    // eager select would likewise evaluate an invalid, unselected log branch.
    np::eval_to(out, np::map(
                         [](double value) {
                             if (value > 0.)
                                 return std::log(value);
                             return 0.;
                         },
                         x));
    print("guarded log of raw inputs:");
    return magnitude == std::array{1., 0., 3., 4.} && output[0] == 0. && output[1] == 0. &&
                   output[2] == std::log(3.) && output[3] == std::log(4.)
               ? 0
               : 1;
}
