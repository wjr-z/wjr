#ifndef WJR_NP_EXPRESSION_MEAN_HPP__
#define WJR_NP_EXPRESSION_MEAN_HPP__

#include <limits>

#include <wjr/np/expression_reduce.hpp>
#include <wjr/np/mean.hpp>

namespace wjr::np {

// Ordered terminal sum in the selected floating accumulator, divided once.
// Each child keeps its own dtype; empty input returns quiet NaN without reads.
template <class Accumulator = void, tensor_expression E>
requires(detail::expression_element<typename E::value_type> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>))
[[nodiscard]] WJR_INTRINSIC_INLINE constexpr auto
mean(const E &expression) noexcept(noexcept(expression.read(size_t{}))) {
    using accumulator_type = detail::mean_accumulator<Accumulator, typename E::value_type>;
    const auto domain = detail::bind_expression_sum_domain(expression);
    if (domain.empty())
        return std::numeric_limits<accumulator_type>::quiet_NaN();
    const auto result = detail::sum_nonempty_expression<accumulator_type>(expression, domain);
    return result / static_cast<accumulator_type>(domain.linear_size());
}

// Reduce one expression axis into caller-provided floating storage. Each line
// is accumulated in order and divided once before its output is committed.
template <class Accumulator = void, class D, class Extents, class Layout, tensor_expression E,
          size_t Axis>
requires(detail::mean_accumulator_type<D> && detail::kernel_layout<Layout> &&
         detail::expression_element<typename E::value_type> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>) &&
         detail::compatible_sum_shape<Extents, typename E::extents_type, Axis, false>)
WJR_INTRINSIC_INLINE constexpr void
mean_to(tensor_view<D, Extents, Layout> dst, const E &expression,
        axes_t<Axis>) noexcept(noexcept(expression.read(size_t{}))) {
    using accumulator_type = detail::mean_accumulator<Accumulator, typename E::value_type>;
    detail::sum_expression_axis_to<accumulator_type, Axis, false, true>(dst, expression);
}

template <class Accumulator = void, class D, class Extents, class Layout, tensor_expression E,
          size_t Axis>
requires(detail::mean_accumulator_type<D> && detail::kernel_layout<Layout> &&
         detail::expression_element<typename E::value_type> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>) &&
         detail::compatible_sum_shape<Extents, typename E::extents_type, Axis, true>)
WJR_INTRINSIC_INLINE constexpr void
mean_to(tensor_view<D, Extents, Layout> dst, const E &expression, axes_t<Axis>,
        keepdims_t) noexcept(noexcept(expression.read(size_t{}))) {
    using accumulator_type = detail::mean_accumulator<Accumulator, typename E::value_type>;
    detail::sum_expression_axis_to<accumulator_type, Axis, true, true>(dst, expression);
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_MEAN_HPP__
