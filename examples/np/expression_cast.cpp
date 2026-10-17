#include <array>
#include <iostream>

#include <wjr/np/expression_cast.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::extents<size_t, 3>;
    const std::array<int, 3> numerator{3, 7, 8}, denominator{2, 2, 3};
    std::array<double, 3> before{}, after{};
    const auto a = np::as_expr(np::tensor_view<const int, shape>(numerator.data()));
    const auto b = np::as_expr(np::tensor_view<const int, shape>(denominator.data()));

    // Only this operand converts before division; there is no converted array.
    np::eval_to(np::tensor_view<double, shape>(before.data()), np::cast<double>(a) / b);
    // Integer division has already taken place at the child node.
    np::eval_to(np::tensor_view<double, shape>(after.data()), np::cast<double>(a / b));
    std::cout << "cast before division: " << before[0] << ' ' << before[1] << ' ' << before[2]
              << '\n';
    std::cout << "cast after division: " << after[0] << ' ' << after[1] << ' ' << after[2] << '\n';
    return before[0] == 1.5 && before[1] == 3.5 && before[2] == 8. / 3. &&
                   after == std::array{1., 3., 2.}
               ? 0
               : 1;
}
