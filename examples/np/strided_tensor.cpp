#include <array>
#include <iostream>

#include <wjr/np/expression_reduce.hpp>
#include <wjr/np/strided_tensor.hpp>

int main() {
    namespace np = wjr::np;
    using matrix = np::tensor<double, np::extents<size_t, 2, 3>, np::layout_stride>;
    // Allocate the padded span once. Arithmetic objects are ready for overwrite,
    // with no value-initialization pass; holes need not be read or filled.
    auto result =
        np::make_tensor<matrix>(std::array{2, 3}, std::array{8, 1}, wjr::default_construct);
    if (!result)
        return 1;
    const std::array values{1., 2., 3., 4., 5., 6.};
    const auto input =
        np::as_expr(np::tensor_view<const double, np::extents<size_t, 2, 3>>(values.data()));
    np::eval_to(result->view(), input * 2.0 + 1.0);
    const auto energy = np::sum(np::as_expr(result->view()) * np::as_expr(result->view()));
    std::cout << "logical elements: " << result->size()
              << ", addressed span: " << result->required_span_size() << ", energy: " << energy
              << '\n';
    return result->size() == 6 && result->storage_size() == 11 && energy == 454.0 ? 0 : 2;
}
