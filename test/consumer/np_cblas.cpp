#include <wjr/np/cblas.hpp>

int main() {
    namespace np = wjr::np;
    const double a[]{1, 2, 3, 4}, b[]{5, 6, 7, 8};
    double c[4];
    using square = np::extents<size_t, 2, 2>;
    np::cblas::gemm_to(np::tensor_view<double, square>(c), np::tensor_view<const double, square>(a),
                       np::tensor_view<const double, square>(b));
    const auto x = np::tensor_view<const double, np::extents<size_t, 4>>(a);
    return c[0] == 19. && c[3] == 50. && np::cblas::dot(x, x) == 30. ? 0 : 1;
}
