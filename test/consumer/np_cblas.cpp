#include <wjr/np/cblas.hpp>

int main() {
    namespace np = wjr::np;
    const double a[]{1, 2, 3, 4}, b[]{5, 6, 7, 8};
    double c[4];
    using square = np::extents<size_t, 2, 2>;
    np::cblas::gemm_to(np::tensor_view<double, square>(c), np::tensor_view<const double, square>(a),
                       np::tensor_view<const double, square>(b));
    const auto x = np::tensor_view<const double, np::extents<size_t, 4>>(a);
    using line = np::extents<size_t, 2>;
    double y[2];
    np::cblas::gemv_to(np::tensor_view<double, line>(y), np::tensor_view<const double, square>(a),
                       np::tensor_view<const double, line>(b));
    return c[0] == 19. && c[3] == 50. && np::cblas::dot(x, x) == 30. && y[0] == 17. && y[1] == 39.
               ? 0
               : 1;
}
