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
}
