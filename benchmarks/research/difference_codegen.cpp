
#include <wjr/np/difference.hpp>
namespace np = wjr::np;
extern "C" void api_vector(double *out, const double *in, size_t n) {
    np::diff_to(np::tensor_view<double, np::dextents<size_t, 1>>(out, n - (n != 0)),
                np::tensor_view<const double, np::dextents<size_t, 1>>(in, n), np::axes<0>);
}
extern "C" void hand_vector(double *WJR_RESTRICT out, const double *WJR_RESTRICT in, size_t n) {
    if (n < 2)
        return;
    for (size_t i = 0; i < n - 1; ++i)
        out[i] = in[i + 1] - in[i];
}
extern "C" void api_rows(double *out, const double *in, size_t r, size_t c) {
    np::diff_to(np::tensor_view<double, np::dextents<size_t, 2>>(out, r, c - (c != 0)),
                np::tensor_view<const double, np::dextents<size_t, 2>>(in, r, c), np::axes<1>);
}
extern "C" void hand_rows(double *WJR_RESTRICT out, const double *WJR_RESTRICT in, size_t r,
                          size_t c) {
    if (!r || c < 2)
        return;
    for (size_t i = 0; i < r; ++i)
        for (size_t j = 0; j < c - 1; ++j)
            out[i * (c - 1) + j] = in[i * c + j + 1] - in[i * c + j];
}
extern "C" void api_static(double *out, const double *in) {
    np::diff_to(np::tensor_view<double, np::extents<size_t, 2, 3>>(out),
                np::tensor_view<const double, np::extents<size_t, 2, 4>>(in), np::axes<1>);
}
extern "C" void hand_static(double *WJR_RESTRICT out, const double *WJR_RESTRICT in) {
    for (size_t i = 0; i < 2; ++i)
        for (size_t j = 0; j < 3; ++j)
            out[i * 3 + j] = in[i * 4 + j + 1] - in[i * 4 + j];
}
