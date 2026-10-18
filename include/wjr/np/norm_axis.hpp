#ifndef WJR_NP_NORM_AXIS_HPP__
#define WJR_NP_NORM_AXIS_HPP__

#include <wjr/np/norm.hpp>
#include <wjr/np/reduce.hpp>

namespace wjr::np {
namespace detail {

template <class Accumulator, class D, class T>
concept axis_norm_types =
    kernel_element<T> && std::floating_point<D> && std::same_as<D, std::remove_cv_t<D>> &&
    (std::is_void_v<Accumulator> || (std::floating_point<Accumulator> &&
                                     std::same_as<Accumulator, std::remove_cv_t<Accumulator>>)) &&
    std::numeric_limits<norm_accumulator<Accumulator, T>>::radix == 2;

template <class Accumulator, size_t Axis, bool Keep, class Dst, class Src>
void norm_axis_to(const Dst &dst, const Src &src) noexcept {
    using accumulator_type = norm_accumulator<Accumulator, typename Src::element_type>;
    for (size_t axis = 0; axis != Dst::rank(); ++axis) {
        const auto expected = Keep && axis == Axis
                                  ? size_t{1}
                                  : static_cast<size_t>(src.extent(axis + (!Keep && axis >= Axis)));
        WJR_ASSERT_L2(static_cast<size_t>(dst.extent(axis)) == expected);
    }
    if (dst.empty())
        return;
    const auto count = static_cast<size_t>(src.extent(Axis));
    if (count == 0) {
        detail::apply_kernel(dst, [](typename Dst::reference value) noexcept { value = 0; });
        return;
    }
    const auto step = static_cast<size_t>(detail::axis_stride<Axis>(src));
    auto reduce = [input = src.data_handle(), output = dst.data_handle(), count,
                   step](size_t output_offset, size_t input_offset) noexcept {
        // Retain static arguments before the line callback is inlined, so GCC
        // can specialize the existing kernel without runtime stride metadata.
        using layout = typename Src::layout_type;
        constexpr bool unit_step = (packed_layout<layout> || padded_layout<layout>) &&
                                   (left_layout<layout> ? Axis == 0 : Axis + 1 == Src::rank());
        constexpr auto static_count = Src::static_extent(Axis);
        const auto bound = static_count == dynamic_extent ? count : static_count;
        output[output_offset] = detail::blue_norm<accumulator_type>(input + input_offset, bound,
                                                                    unit_step ? size_t{1} : step);
    };
    detail::walk_axis_lines<0, Axis, Keep>(dst, src, reduce, 0, 0);
}

} // namespace detail

// Stable L2 norm of each selected axis, in increasing axis index order.
// Preconditions: valid views, matching reduced shape, disjoint logical storage,
// and finite input conversions representable in the accumulator. A rare scaled
// recovery pass can reread a line; its input must remain stable during the call.
template <class Accumulator = void, class D, class DstExtents, class DstLayout, class T,
          class SrcExtents, class SrcLayout, size_t Axis>
requires(detail::axis_norm_types<Accumulator, D, T> && detail::kernel_layout<DstLayout> &&
         detail::kernel_layout<SrcLayout> &&
         detail::compatible_sum_shape<DstExtents, SrcExtents, Axis, false>)
void norm_l2_to(tensor_view<D, DstExtents, DstLayout> dst,
                tensor_view<T, SrcExtents, SrcLayout> src, axes_t<Axis>) noexcept {
    detail::norm_axis_to<Accumulator, Axis, false>(dst, src);
}

template <class Accumulator = void, class D, class DstExtents, class DstLayout, class T,
          class SrcExtents, class SrcLayout, size_t Axis>
requires(detail::axis_norm_types<Accumulator, D, T> && detail::kernel_layout<DstLayout> &&
         detail::kernel_layout<SrcLayout> &&
         detail::compatible_sum_shape<DstExtents, SrcExtents, Axis, true>)
void norm_l2_to(tensor_view<D, DstExtents, DstLayout> dst,
                tensor_view<T, SrcExtents, SrcLayout> src, axes_t<Axis>, keepdims_t) noexcept {
    detail::norm_axis_to<Accumulator, Axis, true>(dst, src);
}

} // namespace wjr::np

#endif // WJR_NP_NORM_AXIS_HPP__
