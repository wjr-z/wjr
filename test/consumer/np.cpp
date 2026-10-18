#include <array>

#include <wjr/np/copy.hpp>
#include <wjr/np/evaluate.hpp>
#include <wjr/np/expression.hpp>
#include <wjr/np/expression_cast.hpp>
#include <wjr/np/expression_mask.hpp>
#include <wjr/np/expression_math.hpp>
#include <wjr/np/expression_reduce.hpp>
#include <wjr/np/reduce.hpp>
#include <wjr/np/reshape.hpp>
#include <wjr/np/scan.hpp>
#include <wjr/np/strided_tensor.hpp>
#include <wjr/np/subview.hpp>
#include <wjr/np/tensor.hpp>
#include <wjr/np/transform.hpp>
#include <wjr/np/transpose.hpp>

using shape = wjr::np::extents<size_t, wjr::np::dynamic_extent, 3>;
int main() {
    int values[] = {0, 1, 2, 3, 4, 5};
    wjr::np::tensor_view<int, shape> view(values, 2);
    view(1, 2) = 42;
    using owner = wjr::np::tensor<int, shape>;
    auto owned = wjr::np::make_tensor<owner>(std::array{2, 3});
    auto invalid = wjr::np::make_tensor<owner>(std::array{-1, 3});
    if (!owned || invalid || invalid.error() != wjr::np::shape_error::negative_extent) {
        return 1;
    }
    owned->view()(1, 2) = 9;
    auto selected_row = wjr::np::try_row_view(view, 1);
    auto selected_column = wjr::np::try_column_view(view, 2);
    if (!selected_row || !selected_column || (*selected_row)(2) != 42 ||
        (*selected_column)(1) != 42 || wjr::np::try_row_view(view, -1)) {
        return 1;
    }
    using strided_shape = wjr::np::dextents<size_t, 2>;
    using strided_mapping = wjr::np::layout_stride::mapping<strided_shape>;
    const strided_mapping mapping(strided_shape(2, 2), std::array<size_t, 2>{4, 1});
    const wjr::np::tensor_view<int, strided_shape, wjr::np::layout_stride> pitched(values, mapping);
    if (pitched(1, 1) != 42) {
        return 1;
    }
    auto flat = wjr::np::flatten_view(view);
    auto transposed = wjr::np::transpose_view(view);
    if (&transposed(2, 1) != &view(1, 2)) {
        return 1;
    }
    int materialized[6]{};
    wjr::np::tensor_view<int, wjr::np::extents<size_t, 3, 2>> output(materialized);
    wjr::np::copy_to(output, transposed);
    wjr::np::transform_to(output, output, [](int value) noexcept { return value * 2; });
    if (output(2, 1) != 84 || materialized[1] != 6) {
        return 1;
    }
    if (wjr::np::sum(output) != 104) {
        return 1;
    }
    wjr::np::cumsum_to(output, output, wjr::np::axes<1>);
    if (output(2, 1) != 88) {
        return 1;
    }
    wjr::np::fill_to(output, 0);
    if (materialized[5] != 0) {
        return 1;
    }
    wjr::np::eval_to(output, wjr::np::as_expr(output) + 2);
    if (materialized[5] != 2) {
        return 1;
    }
    auto evaluated = wjr::np::eval(wjr::np::as_expr(output) * 3);
    if (!evaluated || evaluated->view()(2, 1) != 6) {
        return 1;
    }
    const int row_bias[] = {3, 5};
    wjr::np::eval_to(
        output, wjr::np::as_expr(output) +
                    wjr::np::as_expr(
                        wjr::np::tensor_view<const int, wjr::np::extents<size_t, 2>>(row_bias)));
    if (output(0, 0) != 5 || output(2, 1) != 7) {
        return 1;
    }
    if (wjr::np::sum(wjr::np::as_expr(output) * wjr::np::as_expr(output)) != 222) {
        return 1;
    }
    if (wjr::np::cast<double>(3).read(0) != 3.) {
        return 1;
    }
    if (wjr::np::sum(wjr::np::map([](int value, int scale) noexcept { return value * scale + 1; },
                                  output, 2)) != 78) {
        return 1;
    }
    if (wjr::np::select(wjr::np::as_expr(3) < 4, 7, 9).read(0) != 7) {
        return 1;
    }
    using padded_owner = wjr::np::tensor<int, shape, wjr::np::layout_stride>;
    auto padded = wjr::np::make_tensor<padded_owner>(std::array{2, 3}, std::array{8, 1},
                                                     wjr::default_construct);
    if (!padded || padded->size() != 6 || padded->storage_size() != 11) {
        return 1;
    }
    wjr::np::copy_to(padded->view(), view);
    if (padded->view()(1, 2) != 42) {
        return 1;
    }
    if (wjr::np::sqrt(9).read(0) != 3.) {
        return 1;
    }
    auto reshaped =
        wjr::np::try_reshape_view<wjr::np::extents<size_t, 3, 2>>(view, std::array{3, 2});
    auto mismatch = wjr::np::try_reshape_view<shape>(view, std::array{1, 3});
    return view.size() == 6 && flat(5) == 42 && reshaped && (*reshaped)(2, 1) == 42 && !mismatch &&
                   mismatch.error() == wjr::np::shape_error::element_count_mismatch &&
                   owned->try_view()->operator()(1, 2) == 9
               ? 0
               : 1;
}
