#include <array>

#include <wjr/np/checked_strided_view.hpp>
#include <wjr/np/checked_view.hpp>
#include <wjr/np/copy.hpp>
#include <wjr/np/expression.hpp>
#include <wjr/np/reduce.hpp>
#include <wjr/np/reshape.hpp>
#include <wjr/np/subview.hpp>
#include <wjr/np/tensor.hpp>
#include <wjr/np/transform.hpp>
#include <wjr/np/transpose.hpp>

using shape = wjr::np::extents<size_t, wjr::np::dynamic_extent, 3>;
using result_type = wjr::expected<wjr::np::tensor_view<int, shape>, wjr::np::shape_error>;

// Keep debug expected access out of line so its assertion linkage is exercised
// even when the factory can be inlined into the successful call site.
WJR_NOINLINE int read_value(const result_type &result) { return (*result)(1, 2); }
WJR_NOINLINE wjr::np::shape_error read_error(const result_type &result) { return result.error(); }

int main(int argc, char **) {
    int values[] = {0, 1, 2, 3, 4, 5};
    wjr::np::tensor_view<int, shape> view(values, 2);
    view(1, 2) = 42;
    const size_t available = argc == 1 ? 6 : 5;
    auto checked = wjr::np::try_make_tensor_view<shape>(values, available, std::array{2, 3});
    auto insufficient =
        wjr::np::try_make_tensor_view<shape>(values, available - 1, std::array{2, 3});
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
    auto pitched = wjr::np::try_make_strided_tensor_view<strided_shape>(values, 6, std::array{2, 2},
                                                                        std::array{4, 1});
    auto overlap = wjr::np::try_make_strided_tensor_view<strided_shape>(values, 6, std::array{2, 2},
                                                                        std::array{1, 1});
    if (!pitched || (*pitched)(1, 1) != 42 || overlap ||
        overlap.error() != wjr::np::shape_error::unsupported_stride_order) {
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
    wjr::np::fill_to(output, 0);
    if (materialized[5] != 0) {
        return 1;
    }
    wjr::np::eval_to(output, wjr::np::as_expr(output) + 2);
    if (materialized[5] != 2) {
        return 1;
    }
    auto reshaped =
        wjr::np::try_reshape_view<wjr::np::extents<size_t, 3, 2>>(view, std::array{3, 2});
    auto mismatch = wjr::np::try_reshape_view<shape>(view, std::array{1, 3});
    return checked && !insufficient && read_value(checked) == 42 &&
                   read_error(insufficient) == wjr::np::shape_error::insufficient_storage &&
                   view.size() == 6 && flat(5) == 42 && reshaped && (*reshaped)(2, 1) == 42 &&
                   !mismatch && mismatch.error() == wjr::np::shape_error::element_count_mismatch &&
                   owned->try_view()->operator()(1, 2) == 9
               ? 0
               : 1;
}
