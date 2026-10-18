#ifndef WJR_NP_MEAN_HPP__
#define WJR_NP_MEAN_HPP__

#include <wjr/np/reduce.hpp>

namespace wjr::np {

namespace detail {

template <class Accumulator, class T>
using mean_accumulator =
    std::conditional_t<std::is_void_v<Accumulator>,
                       std::conditional_t<std::is_floating_point_v<T>, std::remove_cv_t<T>, double>,
                       Accumulator>;

template <class T>
concept mean_accumulator_type = std::floating_point<T> && std::same_as<T, std::remove_cv_t<T>>;

} // namespace detail

// Ordered sum in the accumulator type, divided once by the logical size.
// Integer/bool input defaults to double; floating input keeps its type.
// Empty input returns quiet NaN. No storage is allocated.
template <class Accumulator = void, class T, class Extents, class Layout>
requires(detail::kernel_element<T> && detail::kernel_layout<Layout> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>))
[[nodiscard]] constexpr auto mean(tensor_view<T, Extents, Layout> source) noexcept {
    return detail::sum_view<detail::mean_accumulator<Accumulator, T>, true>(source);
}

// Preconditions: valid views, matching reduced output shape, and disjoint
// logical input/output storage. Empty reduced lines produce quiet NaN.
template <class Accumulator = void, class D, class DstExtents, class DstLayout, class T,
          class SrcExtents, class SrcLayout, size_t Axis>
requires(detail::mean_accumulator_type<D> && detail::kernel_element<T> &&
         detail::kernel_layout<DstLayout> && detail::kernel_layout<SrcLayout> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>) &&
         detail::compatible_sum_shape<DstExtents, SrcExtents, Axis, false>)
constexpr void mean_to(tensor_view<D, DstExtents, DstLayout> dst,
                       tensor_view<T, SrcExtents, SrcLayout> src, axes_t<Axis>) noexcept {
    detail::sum_axis_to<detail::mean_accumulator<Accumulator, T>, Axis, false, true>(dst, src);
}

template <class Accumulator = void, class D, class DstExtents, class DstLayout, class T,
          class SrcExtents, class SrcLayout, size_t Axis>
requires(detail::mean_accumulator_type<D> && detail::kernel_element<T> &&
         detail::kernel_layout<DstLayout> && detail::kernel_layout<SrcLayout> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>) &&
         detail::compatible_sum_shape<DstExtents, SrcExtents, Axis, true>)
constexpr void mean_to(tensor_view<D, DstExtents, DstLayout> dst,
                       tensor_view<T, SrcExtents, SrcLayout> src, axes_t<Axis>,
                       keepdims_t) noexcept {
    detail::sum_axis_to<detail::mean_accumulator<Accumulator, T>, Axis, true, true>(dst, src);
}

} // namespace wjr::np

#endif // WJR_NP_MEAN_HPP__
