#include <array>
#include <iostream>

#include <wjr/np/expression_xsimd.hpp>

int main() {
    namespace np = wjr::np;
    constexpr size_t count = 19;
    using view = np::tensor_view<double, np::extents<size_t, count>>;
    std::array<double, count> x{}, y{}, output{};
    for (size_t i = 0; i != count; ++i) {
        x[i] = static_cast<double>(i);
        y[i] = 2.0;
    }
    const auto expression = 0.5 * np::as_expr(view(x.data())) + np::as_expr(view(y.data()));
    np::eval_to(view(output.data()), expression, np::xsimd_policy<>{});
    for (size_t i = 0; i != count; ++i) {
        if (output[i] != 0.5 * x[i] + y[i])
            return 1;
    }
    std::cout << "lanes=" << xsimd::batch<double>::size << " first=" << output.front()
              << " last=" << output.back() << '\n';

    using matrix_shape = np::extents<size_t, 2, 7>;
    using row_shape = np::extents<size_t, 7>;
    std::array<double, 14> broadcast_output{};
    const auto matrix = np::as_expr(np::tensor_view<const double, matrix_shape>(x.data()));
    const auto bias = np::as_expr(np::tensor_view<const double, row_shape>(y.data()));
    np::eval_to(np::tensor_view<double, matrix_shape>(broadcast_output.data()), matrix + bias * 0.5,
                np::xsimd_policy<>{});
    for (size_t i = 0; i != broadcast_output.size(); ++i) {
        if (broadcast_output[i] != x[i] + 1.)
            return 1;
    }
    std::cout << "broadcast first=" << broadcast_output.front()
              << " last=" << broadcast_output.back() << '\n';

    // Numeric leaves use batch<double>; comparisons use its matching mask.
    // Both select branches are eager, so every branch must be valid.
    const auto a = np::as_expr(view(x.data()));
    const auto clipped = np::select(a < 3., 3., np::select(a > 12., 12., a));
    np::eval_to(view(output.data()), clipped, np::xsimd_policy<>{});
    for (size_t i = 0; i != count; ++i) {
        if (output[i] != (x[i] < 3. ? 3. : (x[i] > 12. ? 12. : x[i])))
            return 1;
    }
    std::cout << "clip first=" << output.front() << " last=" << output.back() << '\n';
}
