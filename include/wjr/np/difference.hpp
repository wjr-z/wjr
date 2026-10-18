#ifndef WJR_NP_DIFFERENCE_HPP__
#define WJR_NP_DIFFERENCE_HPP__

#include <wjr/np/reduce.hpp>

namespace wjr::np {
namespace detail {

template <class Dst, class Src, size_t Axis>
inline constexpr bool compatible_difference_shape = [] {
    if constexpr (Dst::rank() != Src::rank() || Axis >= Src::rank()) {
        return false;
    } else {
        for (size_t i = 0; i != Src::rank(); ++i) {
            auto source = Src::static_extent(i);
            const auto destination = Dst::static_extent(i);
            if (source != dynamic_extent && destination != dynamic_extent) {
                if (i == Axis && source != 0)
                    --source;
                if (source != destination)
                    return false;
            }
        }
        return true;
    }
}();

// The public contract already requires disjoint logical storage. Packed views
// have no holes, so their complete input/output spans satisfy restrict as well.
template <class D, class T>
WJR_INTRINSIC_INLINE constexpr void
difference_blocks_to(D *WJR_RESTRICT output, const T *WJR_RESTRICT input, size_t outer,
                     size_t output_block, size_t input_block, size_t step) noexcept {
    size_t output_offset = 0, input_offset = 0;
    for (size_t block = 0; block != outer; ++block) {
        for (size_t i = 0; i != output_block; ++i) {
            if constexpr (std::same_as<std::remove_cv_t<T>, bool>)
                output[output_offset + i] =
                    input[input_offset + i + step] != input[input_offset + i];
            else
                output[output_offset + i] =
                    input[input_offset + i + step] - input[input_offset + i];
        }
        output_offset += output_block;
        input_offset += input_block;
    }
}

} // namespace detail

// First differences along one compile-time axis. Output has the same rank,
// with max(source.extent(Axis) - 1, 0) on that axis. Storage must be disjoint.
// Bool input computes inequality; other input uses its ordinary C++ subtraction.
template <class D, class DstExtents, class DstLayout, class T, class SrcExtents, class SrcLayout,
          size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_element<T> &&
         detail::kernel_layout<DstLayout> && detail::kernel_layout<SrcLayout> &&
         detail::compatible_difference_shape<DstExtents, SrcExtents, Axis>)
constexpr void diff_to(tensor_view<D, DstExtents, DstLayout> destination,
                       tensor_view<T, SrcExtents, SrcLayout> source, axes_t<Axis>) noexcept {
    for (size_t i = 0; i != SrcExtents::rank(); ++i) {
        auto expected = static_cast<size_t>(source.extent(i));
        if (i == Axis && expected != 0)
            --expected;
        WJR_ASSERT_L2(static_cast<size_t>(destination.extent(i)) == expected);
    }
    const auto axis_count = static_cast<size_t>(source.extent(Axis));
    if (axis_count < 2 || source.empty())
        return;
    const auto step = static_cast<size_t>(detail::axis_stride<Axis>(source));
    if constexpr (detail::packed_layout<SrcLayout> && std::same_as<DstLayout, SrcLayout>) {
        size_t outer = 1;
        for (size_t i = 0; i != SrcExtents::rank(); ++i)
            if (std::same_as<SrcLayout, layout_right> ? i < Axis : i > Axis)
                outer *= static_cast<size_t>(source.extent(i));
        const auto output_block = (axis_count - 1) * step;
        const auto input_block = axis_count * step;
        detail::difference_blocks_to(destination.data_handle(), source.data_handle(), outer,
                                     output_block, input_block, step);
    } else {
        auto subtract = [output = destination.data_handle(), input = source.data_handle(),
                         step](size_t output_offset, size_t input_offset) noexcept {
            if constexpr (std::same_as<std::remove_cv_t<T>, bool>)
                output[output_offset] = input[input_offset + step] != input[input_offset];
            else
                output[output_offset] = input[input_offset + step] - input[input_offset];
        };
        [&]<size_t... Dimensions>(std::index_sequence<Dimensions...>) {
            constexpr auto rank = SrcExtents::rank();
            const std::array<size_t, rank> shape{
                static_cast<size_t>(destination.extent(Dimensions))...};
            const std::array<std::array<size_t, rank>, 2> strides{
                std::array<size_t, rank>{
                    static_cast<size_t>(detail::axis_stride<Dimensions>(destination))...},
                std::array<size_t, rank>{
                    static_cast<size_t>(detail::axis_stride<Dimensions>(source))...}};
            detail::walk_kernel<0, detail::left_layout<DstLayout>>(
                shape, strides, subtract, std::index_sequence<0, 1>{}, size_t{0}, size_t{0});
        }(std::make_index_sequence<SrcExtents::rank()>{});
    }
}

} // namespace wjr::np

#endif // WJR_NP_DIFFERENCE_HPP__
