#ifndef WJR_NP_SCAN_HPP__
#define WJR_NP_SCAN_HPP__

#include <wjr/np/reduce.hpp>

namespace wjr::np {

// Preconditions: valid views with equal shapes. Input/output must be logically
// disjoint or refer to the same object at every corresponding index.
// Each axis line is an inclusive left fold from zero, without reassociation.
template <class Accumulator = void, class D, class DstExtents, class DstLayout, class T,
          class SrcExtents, class SrcLayout, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_element<T> &&
         detail::kernel_layout<DstLayout> && detail::kernel_layout<SrcLayout> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>) &&
         detail::compatible_kernel_shape<DstExtents, SrcExtents> && Axis < SrcExtents::rank())
constexpr void cumsum_to(tensor_view<D, DstExtents, DstLayout> dst,
                         tensor_view<T, SrcExtents, SrcLayout> src, axes_t<Axis>) noexcept {
    using accumulator_type = detail::sum_accumulator<Accumulator, T>;
    detail::assert_kernel_shape(dst, src);
    if (dst.empty())
        return;
    const auto count = static_cast<size_t>(src.extent(Axis));
    if constexpr (Axis == 0 && SrcExtents::rank() == 2 && std::same_as<SrcLayout, layout_right> &&
                  std::same_as<DstLayout, layout_right>) {
        detail::axis_columns_to<true, accumulator_type>(dst.data_handle(), src.data_handle(), count,
                                                        static_cast<size_t>(src.extent(1)));
        return;
    }
    const auto input_step = static_cast<size_t>(detail::axis_stride<Axis>(src));
    const auto output_step = static_cast<size_t>(detail::axis_stride<Axis>(dst));
    const auto input = src.data_handle();
    const auto output = dst.data_handle();
    auto scan = [=](size_t output_offset, size_t input_offset) noexcept {
        accumulator_type state{};
        constexpr auto static_count = SrcExtents::static_extent(Axis);
        const auto bound = static_count == dynamic_extent ? count : static_count;
        for (size_t i = 0; i != bound; ++i) {
            state += static_cast<accumulator_type>(input[input_offset + i * input_step]);
            output[output_offset + i * output_step] = state;
        }
    };
    detail::walk_axis_lines<0, Axis, true>(dst, src, scan, 0, 0);
}

} // namespace wjr::np

#endif // WJR_NP_SCAN_HPP__
