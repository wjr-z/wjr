#include <array>
#include <iostream>

#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression_math.hpp>

int main() {
    namespace np = wjr::np;
    using shape = np::extents<size_t, 4>;
    const int input[]{-1, 0, 2, 4};
    const unsigned lower[]{0, 1, 1, 1};
    double output[4]{};
    const auto values = np::as_expr(np::tensor_view<const int, shape>(input));
    const auto bounds = np::as_expr(np::tensor_view<const unsigned, shape>(lower));
    np::eval_to(np::tensor_view<double, shape>(output), np::clip(values, bounds, 1.5));
    std::cout << "clip: ";
    const std::array expected{0., 1., 1.5, 1.5};
    bool correct = true;
    for (size_t i = 0; i != expected.size(); ++i) {
        const auto value = output[i];
        std::cout << value << ' ';
        correct = correct && value == expected[i];
    }
    std::cout << '\n';
    return correct ? 0 : 1;
}
