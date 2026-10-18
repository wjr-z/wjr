#include <iostream>

#include <wjr/np/cblas.hpp>

int main() {
    namespace np = wjr::np;
    const double a[]{1, 2, 3, 4}, b[]{5, 6, 7, 8};
    double c[4]; // Objects are live; beta zero does not require initial values.
    using square = np::extents<size_t, 2, 2>;
    np::cblas::gemm_to(np::tensor_view<double, square>(c), np::tensor_view<const double, square>(a),
                       np::tensor_view<const double, square>(b));
    const auto x = np::tensor_view<const double, np::extents<size_t, 4>>(a);
    const auto norm_squared = np::cblas::dot(x, x);
    using line = np::extents<size_t, 2>;
    double y[2];
    np::cblas::gemv_to(np::tensor_view<double, line>(y), np::tensor_view<const double, square>(a),
                       np::tensor_view<const double, line>(b));
    std::cout << "A*B:\n"
              << c[0] << ' ' << c[1] << '\n'
              << c[2] << ' ' << c[3] << "\ndot(A,A): " << norm_squared << '\n';
    std::cout << "A*[5,6]: " << y[0] << ' ' << y[1] << '\n';
    return c[0] == 19. && c[3] == 50. && norm_squared == 30. && y[0] == 17. && y[1] == 39. ? 0 : 1;
}
