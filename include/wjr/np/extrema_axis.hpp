#ifndef WJR_NP_EXTREMA_AXIS_HPP__
#define WJR_NP_EXTREMA_AXIS_HPP__

#include <wjr/np/reduce.hpp>

namespace wjr::np {
namespace detail {

template <class D, class T>
concept axis_extremum_index_types = std::integral<D> && std::same_as<D, std::remove_cv_t<D>> &&
                                    !std::same_as<D, bool> && kernel_element<T>;

// Nonempty line. Comparisons stay in the original dtype and strict ties keep
// the first index. A NaN wins immediately, including a NaN at position zero.
template <bool Maximum, class T>
constexpr size_t argextremum_line(const T *input, size_t count, size_t step) noexcept {
    using value_type = std::remove_cv_t<T>;
    value_type best = input[0];
    size_t selected = 0;
    if constexpr (std::floating_point<value_type>)
        if (best != best)
            return 0;
    for (size_t i = 1; i != count; ++i) {
        const value_type value = input[i * step];
        if constexpr (std::floating_point<value_type>)
            if (value != value)
                return i;
        if (Maximum ? best < value : value < best) {
            best = value;
            selected = i;
        }
    }
    return selected;
}

template <bool Maximum, size_t Axis, bool Keep, class Dst, class Src>
constexpr void argextremum_axis_to(const Dst &dst, const Src &src) noexcept {
    using index_type = typename Dst::element_type;
    for (size_t axis = 0; axis != Dst::rank(); ++axis) {
        [[maybe_unused]] const auto expected =
            Keep && axis == Axis ? size_t{1}
                                 : static_cast<size_t>(src.extent(axis + (!Keep && axis >= Axis)));
        WJR_ASSERT_L2(static_cast<size_t>(dst.extent(axis)) == expected);
    }
    if (dst.empty())
        return;
    const auto count = static_cast<size_t>(src.extent(Axis));
    WJR_ASSERT_L2(count != 0);
    // Do not narrow the destination's limit to size_t before comparing it.
    WJR_ASSERT_L2(count - 1 <= static_cast<std::make_unsigned_t<index_type>>(
                                   std::numeric_limits<index_type>::max()));
    const auto step = static_cast<size_t>(axis_stride<Axis>(src));
    auto reduce = [input = src.data_handle(), output = dst.data_handle(), count,
                   step](size_t output_offset, size_t input_offset) constexpr noexcept {
        using layout = typename Src::layout_type;
        constexpr bool unit_step = (packed_layout<layout> || padded_layout<layout>) &&
                                   (left_layout<layout> ? Axis == 0 : Axis + 1 == Src::rank());
        constexpr auto static_count = Src::static_extent(Axis);
        const auto bound = static_count == dynamic_extent ? count : static_count;
        output[output_offset] = static_cast<index_type>(
            argextremum_line<Maximum>(input + input_offset, bound, unit_step ? size_t{1} : step));
    };
    walk_axis_lines<0, Axis, Keep>(dst, src, reduce, 0, 0);
}

} // namespace detail

// Return the index within the selected axis for each remaining coordinate.
// First NaN wins; strict comparisons retain the first equal value (including
// signed zeros). Input and output storage are disjoint. Nonempty output requires
// a nonempty axis and an index dtype capable of representing axis length-1.
template <class D, class DE, class DL, class T, class SE, class SL, size_t Axis>
requires(detail::axis_extremum_index_types<D, T> && detail::kernel_layout<DL> &&
         detail::kernel_layout<SL> && detail::compatible_sum_shape<DE, SE, Axis, false>)
constexpr void argmin_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> src,
                         axes_t<Axis>) noexcept {
    detail::argextremum_axis_to<false, Axis, false>(dst, src);
}

template <class D, class DE, class DL, class T, class SE, class SL, size_t Axis>
requires(detail::axis_extremum_index_types<D, T> && detail::kernel_layout<DL> &&
         detail::kernel_layout<SL> && detail::compatible_sum_shape<DE, SE, Axis, true>)
constexpr void argmin_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> src, axes_t<Axis>,
                         keepdims_t) noexcept {
    detail::argextremum_axis_to<false, Axis, true>(dst, src);
}

template <class D, class DE, class DL, class T, class SE, class SL, size_t Axis>
requires(detail::axis_extremum_index_types<D, T> && detail::kernel_layout<DL> &&
         detail::kernel_layout<SL> && detail::compatible_sum_shape<DE, SE, Axis, false>)
constexpr void argmax_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> src,
                         axes_t<Axis>) noexcept {
    detail::argextremum_axis_to<true, Axis, false>(dst, src);
}

template <class D, class DE, class DL, class T, class SE, class SL, size_t Axis>
requires(detail::axis_extremum_index_types<D, T> && detail::kernel_layout<DL> &&
         detail::kernel_layout<SL> && detail::compatible_sum_shape<DE, SE, Axis, true>)
constexpr void argmax_to(tensor_view<D, DE, DL> dst, tensor_view<T, SE, SL> src, axes_t<Axis>,
                         keepdims_t) noexcept {
    detail::argextremum_axis_to<true, Axis, true>(dst, src);
}

} // namespace wjr::np

#endif // WJR_NP_EXTREMA_AXIS_HPP__
