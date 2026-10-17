// This is an executable design sketch, not an installed wjr::np API.
#include <array>
#include <cstdio>
#include <span>

#include "detail/expression.hpp"

int main() {
    namespace ex = expression_experiment;
    const std::array<double, 4> x{1, 2, 3, 4};
    const std::array<double, 4> y{4, 3, 2, 1};
    std::array<double, 4> output{};

    auto a = ex::as_expr(std::span{x});
    auto b = ex::as_expr(std::span{y});
    auto difference = a - b;
    auto expr = 0.5 * difference * (a + b);
    ex::eval_to(std::span{output}, expr);

    // Building and evaluating the expression creates no intermediate arrays.
    // a, b, difference and expr borrow x/y; those arrays must remain alive.
    for (double value : output)
        std::printf("%.1f ", value);
    std::puts("");
    return output == std::array<double, 4>{-7.5, -2.5, 2.5, 7.5} ? 0 : 1;
}
