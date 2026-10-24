#ifndef WJR_NP_SORT_HPP__
#define WJR_NP_SORT_HPP__

#include <algorithm>

#include <wjr/np/detail/index_order.hpp>
#include <wjr/np/reduce.hpp>

namespace wjr::np {
namespace detail {

template <sort_order Order>
struct value_order {
    template <class T>
    constexpr bool operator()(T left, T right) const noexcept {
        const bool ordered = Order == sort_order::ascending ? left < right : left > right;
        if constexpr (std::is_floating_point_v<T>)
            return ordered || (left == left && right != right);
        else
            return ordered;
    }
};

} // namespace detail

// Unstable value sorting, with NaNs last in either direction. Signed zeros
// compare equal. The packed live range must fit ptrdiff_t; no workspace is allocated.
template <sort_order Order = sort_order::ascending, class T, class E, class L>
requires(detail::kernel_element<T> && !std::is_const_v<T> && E::rank() == 1 &&
         detail::packed_layout<L> && detail::supported_index_order<Order, sort_stability::unstable>)
constexpr void sort_inplace(tensor_view<T, E, L> values) noexcept {
    const auto count = static_cast<size_t>(values.extent(0));
    if (count < 2)
        return;
    auto *data = values.data_handle();
    std::sort(data, data + count, detail::value_order<Order>{});
}

// Preserve source by copying into disjoint caller-owned packed output, then
// sorting that output. Dtype and length match; source may have a positive stride.
template <sort_order Order = sort_order::ascending, class D, class DE, class DL, class T, class SE,
          class SL>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_element<T> &&
         std::same_as<D, std::remove_cv_t<T>> && DE::rank() == 1 && SE::rank() == 1 &&
         detail::packed_layout<DL> && detail::kernel_layout<SL> &&
         detail::compatible_kernel_shape<DE, SE> &&
         detail::supported_index_order<Order, sort_stability::unstable>)
constexpr void sort_to(tensor_view<D, DE, DL> destination, tensor_view<T, SE, SL> source) noexcept {
    detail::assert_kernel_shape(destination, source);
    const auto count = static_cast<size_t>(destination.extent(0));
    if (count == 0)
        return;
    auto *output = destination.data_handle();
    const auto *input = source.data_handle();
    if constexpr (detail::packed_layout<SL> || detail::padded_layout<SL>) {
        std::copy_n(input, count, output);
    } else {
        const auto step = detail::axis_stride<0>(source);
        for (size_t i = 0; i != count; ++i)
            output[i] = input[i * step];
    }
    if (count > 1)
        std::sort(output, output + count, detail::value_order<Order>{});
}

} // namespace wjr::np

#endif // WJR_NP_SORT_HPP__
