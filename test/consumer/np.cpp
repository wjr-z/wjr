#include <array>

#include <wjr/np/checked_view.hpp>
#include <wjr/np/reshape.hpp>
#include <wjr/np/tensor.hpp>

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
    auto flat = wjr::np::flatten_view(view);
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
