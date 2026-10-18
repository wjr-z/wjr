#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <vector>

#ifdef ONLY_FULL_NORM
    #include <wjr/np/norm.hpp>
#else
    #include <wjr/np/norm_axis.hpp>
#endif

namespace np = wjr::np;
template <class T, size_t Rank>
using view = np::tensor_view<T, np::dextents<size_t, Rank>>;

extern "C" double full_norm(const double *input, size_t count) {
    return np::norm_l2(view<const double, 1>(input, count));
}
extern "C" double stride_norm(const double *input, size_t count, size_t step) {
    using shape = np::dextents<size_t, 1>;
    return np::norm_l2(np::tensor_view<const double, shape, np::layout_stride>(
        input, np::layout_stride::mapping<shape>(shape(count), std::array{step})));
}
#ifndef ONLY_FULL_NORM
extern "C" void axis_rows(double *output, const double *input, size_t rows, size_t cols) {
    np::norm_l2_to(view<double, 1>(output, rows), view<const double, 2>(input, rows, cols),
                   np::axes<1>);
}
extern "C" void hand_rows(double *output, const double *input, size_t rows, size_t cols) {
    for (size_t row = 0; row != rows; ++row)
        output[row] = cols ? np::detail::blue_norm<double>(input + row * cols, cols, 1) : 0;
}
extern "C" void repeated_rows(double *output, const double *input, size_t rows, size_t cols) {
    for (size_t row = 0; row != rows; ++row)
        output[row] = cols ? np::norm_l2(view<const double, 1>(input + row * cols, cols)) : 0;
}
extern "C" void axis_cols(double *output, const double *input, size_t rows, size_t cols) {
    np::norm_l2_to(view<double, 1>(output, cols), view<const double, 2>(input, rows, cols),
                   np::axes<0>);
}
extern "C" void hand_cols(double *output, const double *input, size_t rows, size_t cols) {
    for (size_t col = 0; col != cols; ++col)
        output[col] = rows ? np::detail::blue_norm<double>(input + col, rows, cols) : 0;
}
extern "C" void axis_float(float *output, const float *input, size_t rows, size_t cols) {
    np::norm_l2_to(view<float, 1>(output, rows), view<const float, 2>(input, rows, cols),
                   np::axes<1>);
}
extern "C" void hand_float(float *output, const float *input, size_t rows, size_t cols) {
    for (size_t row = 0; row != rows; ++row)
        output[row] = cols ? np::detail::blue_norm<float>(input + row * cols, cols, 1) : 0;
}
extern "C" void axis_wide(double *output, const float *input, size_t rows, size_t cols) {
    np::norm_l2_to<double>(view<double, 1>(output, rows), view<const float, 2>(input, rows, cols),
                           np::axes<1>);
}
extern "C" void hand_wide(double *output, const float *input, size_t rows, size_t cols) {
    for (size_t row = 0; row != rows; ++row)
        output[row] = cols ? np::detail::blue_norm<double>(input + row * cols, cols, 1) : 0;
}
extern "C" void axis_padded(double *output, const double *input, size_t rows, size_t cols,
                            size_t pitch, size_t outstep) {
    using shape = np::dextents<size_t, 2>;
    using outshape = np::dextents<size_t, 1>;
    np::tensor_view<const double, shape, np::layout_stride> src(
        input, np::layout_stride::mapping<shape>(shape(rows, cols), std::array{pitch, size_t{1}}));
    np::tensor_view<double, outshape, np::layout_stride> dst(
        output, np::layout_stride::mapping<outshape>(outshape(rows), std::array{outstep}));
    np::norm_l2_to(dst, src, np::axes<1>);
}
extern "C" void hand_padded(double *output, const double *input, size_t rows, size_t cols,
                            size_t pitch, size_t outstep) {
    for (size_t row = 0; row != rows; ++row)
        output[row * outstep] =
            cols ? np::detail::blue_norm<double>(input + row * pitch, cols, 1) : 0;
}
extern "C" void axis_static(double *output, const double *input) {
    np::norm_l2_to(np::tensor_view<double, np::extents<size_t, 2>>(output),
                   np::tensor_view<const double, np::extents<size_t, 2, 3>>(input), np::axes<1>);
}
extern "C" void hand_static(double *output, const double *input) {
    for (size_t row = 0; row != 2; ++row)
        output[row] = np::detail::blue_norm<double>(input + row * 3, 3, 1);
}
#endif

#ifdef NORM_RUNTIME
template <class T>
bool same(T a, T b) {
    return (std::isnan(a) && std::isnan(b)) || (a == b && std::signbit(a) == std::signbit(b));
}

int main() {
    size_t checks = 0;
    const std::array<size_t, 7> sizes{0, 1, 2, 3, 7, 17, 300};
    for (size_t rows : sizes)
        for (size_t cols : sizes)
            for (unsigned mode = 0; mode != 5; ++mode) {
                const auto pitch = cols + 3;
                std::vector<double> input(rows * cols),
                    padded(rows * pitch, std::numeric_limits<double>::quiet_NaN());
                for (size_t r = 0; r != rows; ++r)
                    for (size_t c = 0; c != cols; ++c) {
                        double value = double(int((r * 11 + c * 7) % 37) - 18);
                        if (mode == 1)
                            value = std::ldexp(value, -900);
                        if (mode == 2)
                            value = std::ldexp(value, 900);
                        if (mode == 3 && (r + c) % 7 == 0)
                            value = std::numeric_limits<double>::infinity();
                        if (mode == 4 && (r + c) % 11 == 0)
                            value = std::numeric_limits<double>::quiet_NaN();
                        input[r * cols + c] = padded[r * pitch + c] = value;
                    }
                std::vector<double> a(rows), b(rows), columns(cols), reference(cols),
                    pad(rows * 2, -1), padref(rows * 2, -1);
                axis_rows(a.data(), input.data(), rows, cols);
                hand_rows(b.data(), input.data(), rows, cols);
                for (size_t r = 0; r != rows; ++r)
                    if (!same(a[r], b[r]))
                        return 1;
                repeated_rows(a.data(), input.data(), rows, cols);
                for (size_t r = 0; r != rows; ++r)
                    if (!same(a[r], b[r]))
                        return 2;
                axis_cols(columns.data(), input.data(), rows, cols);
                hand_cols(reference.data(), input.data(), rows, cols);
                for (size_t c = 0; c != cols; ++c)
                    if (!same(columns[c], reference[c]))
                        return 3;
                axis_padded(pad.data(), padded.data(), rows, cols, pitch, 2);
                hand_padded(padref.data(), padded.data(), rows, cols, pitch, 2);
                for (size_t i = 0; i != pad.size(); ++i)
                    if (!same(pad[i], padref[i]))
                        return 4;
                ++checks;
            }
    const float small[] = {1e-30f, 2e-30f, 3e-30f, 4e-30f, 1e30f, 2e30f};
    float af[2], bf[2];
    double ad[2], bd[2];
    axis_float(af, small, 2, 3);
    hand_float(bf, small, 2, 3);
    axis_wide(ad, small, 2, 3);
    hand_wide(bd, small, 2, 3);
    for (size_t i = 0; i != 2; ++i)
        if (!same(af[i], bf[i]) || !same(ad[i], bd[i]))
            return 5;
    const double fixed[] = {3, 4, 0, 5, 12, 0};
    double as[2], bs[2];
    axis_static(as, fixed);
    hand_static(bs, fixed);
    for (size_t i = 0; i != 2; ++i)
        if (!same(as[i], bs[i]))
            return 6;
    std::printf(
        "PASS %zu shape/distribution sets: actual axis API, hand Blue and repeated public norm; float/wide/static\n",
        checks);
}
#endif
