#include <array>
#include <iostream>
#include <string_view>

#include <wjr/np/copy.hpp>
#include <wjr/np/subview.hpp>
#include <wjr/np/transform.hpp>
#include <wjr/np/transpose.hpp>

namespace np = wjr::np;

void print_matrix(std::string_view name, auto matrix) {
    std::cout << name << '\n';
    for (size_t row = 0; row != matrix.extent(0); ++row) {
        for (size_t column = 0; column != matrix.extent(1); ++column)
            std::cout << matrix(row, column) << ' ';
        std::cout << '\n';
    }
}

int main() {
    using shape = np::extents<size_t, 2, 3>;
    const std::array<double, 6> a{2, 4, 6, 8, 10, 12};
    const std::array<double, 6> b{1, 2, 3, 4, 5, 6};
    std::array<double, 6> output;
    std::array<double, 6> transposed_storage;
    np::tensor_view<const double, shape> lhs(a.data()), rhs(b.data());
    np::tensor_view<double, shape> out(output.data());

    // All buffers already exist. One fused loop writes each affine result once.
    np::fill_to(out, 0.0);
    constexpr double scale = 2.0;
    constexpr double bias = 1.0;
    np::transform_to(out, lhs, rhs,
                     [](double x, double y) noexcept { return (x - y) * scale + bias; });
    print_matrix("fused (a-b)*2+1:", out);

    // This column borrows output with stride 3. Same-index in-place is allowed.
    auto column = np::try_column_view(out, 1);
    if (!column)
        return 1;
    np::transform_to(*column, *column, [](double value) noexcept { return value / 2; });
    std::cout << "column stride: " << column->stride(0) << '\n';
    print_matrix("after scaling column 1:", out);

    // The first operation changes metadata; the copy materializes row-major data.
    const auto borrowed_transpose = np::transpose_view(out);
    np::tensor_view<double, np::extents<size_t, 3, 2>> transposed(transposed_storage.data());
    np::copy_to(transposed, borrowed_transpose);
    print_matrix("materialized transpose:", transposed);

    const std::array<double, 6> expected{3, 9, 2.5, 5.5, 7, 13};
    if (transposed_storage != expected || column->data_handle() != output.data() + 1)
        return 1;
    return 0;
}
