#ifndef WJR_NP_COMPACT_HPP__
#define WJR_NP_COMPACT_HPP__

#include <limits>

#include <wjr/np/reduce.hpp>

namespace wjr::np {

namespace detail {

template <class D, class T, class M, class OutputStep, class InputStep, class MaskStep>
constexpr size_t compress_line(D *WJR_RESTRICT output, [[maybe_unused]] size_t capacity,
                               OutputStep output_step, const T *input, size_t count,
                               InputStep input_step, const M *mask, MaskStep mask_step) noexcept {
    size_t written = 0;
    for (size_t i = 0; i != count; ++i) {
        if (mask[i * mask_step] != 0) {
            WJR_ASSERT_L2(written < capacity);
            output[written * output_step] = static_cast<D>(input[i * input_step]);
            ++written;
        }
    }
    return written;
}

template <class D, class T, class Step>
constexpr size_t flatnonzero_line(D *WJR_RESTRICT output, [[maybe_unused]] size_t capacity,
                                  const T *input, size_t count, Step step) noexcept {
    size_t written = 0;
    for (size_t i = 0; i != count; ++i) {
        if (input[i * step] != 0) {
            WJR_ASSERT_L2(written < capacity);
            output[written++] = static_cast<D>(i);
        }
    }
    return written;
}

} // namespace detail

// Preserve input order. Destination storage is disjoint from inputs and has
// room for every selected element; its unused suffix is left untouched.
template <class D, class DE, class DL, class T, class TE, class TL, class M, class ME, class ML>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_element<T> &&
         detail::kernel_element<M> && DE::rank() == 1 && TE::rank() == 1 &&
         detail::compatible_kernel_shape<TE, ME> && detail::kernel_layout<DL> &&
         detail::kernel_layout<TL> && detail::kernel_layout<ML>)
constexpr size_t compress_to(tensor_view<D, DE, DL> dst, tensor_view<T, TE, TL> src,
                             tensor_view<M, ME, ML> mask) noexcept {
    detail::assert_kernel_shape(src, mask);
    return detail::compress_line(dst.data_handle(), static_cast<size_t>(dst.extent(0)),
                                 detail::axis_stride<0>(dst), src.data_handle(),
                                 static_cast<size_t>(src.extent(0)), detail::axis_stride<0>(src),
                                 mask.data_handle(), detail::axis_stride<0>(mask));
}

// Return row-major logical positions, independently of the physical layout.
// Destination is packed, disjoint from source, and has room for every result.
template <class D, class DE, class DL, class T, class TE, class TL>
requires(std::is_integral_v<D> && !std::same_as<D, bool> && !std::is_const_v<D> &&
         !std::is_volatile_v<D> && detail::kernel_element<T> && DE::rank() == 1 &&
         detail::packed_layout<DL> && detail::kernel_layout<TL>)
constexpr size_t flatnonzero_to(tensor_view<D, DE, DL> dst, tensor_view<T, TE, TL> src) noexcept {
    const auto count = static_cast<size_t>(src.size());
    if (count == 0)
        return 0;
    WJR_ASSERT_L2(count - 1 <= static_cast<std::make_unsigned_t<D>>(std::numeric_limits<D>::max()));
    const auto capacity = static_cast<size_t>(dst.extent(0));
    if constexpr (TE::rank() == 0 || std::same_as<TL, layout_right> ||
                  (TE::rank() == 1 && detail::packed_layout<TL>)) {
        return detail::flatnonzero_line(dst.data_handle(), capacity, src.data_handle(), count, 1);
    } else if constexpr (TE::rank() == 1) {
        return detail::flatnonzero_line(dst.data_handle(), capacity, src.data_handle(), count,
                                        detail::axis_stride<0>(src));
    } else {
        std::array<size_t, TE::rank()> shape;
        std::array<std::array<size_t, TE::rank()>, 1> strides;
        for (size_t axis = 0; axis != TE::rank(); ++axis) {
            shape[axis] = static_cast<size_t>(src.extent(axis));
            strides[0][axis] = static_cast<size_t>(src.stride(axis));
        }
        size_t flat = 0, written = 0;
        auto visit = [&](size_t offset) constexpr noexcept {
            if (src.data_handle()[offset] != 0) {
                WJR_ASSERT_L2(written < capacity);
                dst.data_handle()[written++] = static_cast<D>(flat);
            }
            ++flat;
        };
        detail::walk_kernel<0, false>(shape, strides, visit, std::index_sequence<0>{}, size_t{});
        return written;
    }
}

} // namespace wjr::np

#endif // WJR_NP_COMPACT_HPP__
