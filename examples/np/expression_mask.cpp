#include <array>
#include <iostream>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression_mask.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::extents<size_t, 5>;
    const std::array<double, 5> input{-2., -0., .5, 1., 3.};
    std::array<double, 5> output{};
    const auto x = np::as_expr(np::tensor_view<const double, shape>(input.data()));
    const auto valid = np::logical_and(x >= 0., x <= 1.);
    auto flags = np::eval(valid); // An independent owner of real bool elements.
    if (!flags)
        return 1;
    // Every branch is valid for every element; select is eager, not a guard.
    np::eval_to(np::tensor_view<double, shape>(output.data()),
                np::select(x < 0., 0., np::select(x > 1., 1., x)));
    std::cout << "within [0,1]:";
    for (size_t i = 0; i != input.size(); ++i)
        std::cout << ' ' << flags->view()(i);
    std::cout << "\nclipped:";
    for (auto value : output)
        std::cout << ' ' << value;
    std::cout << '\n';
    return output == std::array{0., -0., .5, 1., 1.} ? 0 : 1;
}
