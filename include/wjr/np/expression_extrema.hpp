#ifndef WJR_NP_EXPRESSION_EXTREMA_HPP__
#define WJR_NP_EXPRESSION_EXTREMA_HPP__

#include <wjr/np/detail/expression_domain.hpp>
#include <wjr/np/extrema.hpp>
#include <wjr/np/reduce.hpp>

namespace wjr::np {

namespace detail {

template <size_t Level, size_t Rank, size_t Count, class Function, size_t... I, class... Offsets>
WJR_INTRINSIC_INLINE constexpr bool walk_expression_extremum(
    const std::array<size_t, Rank> &shape,
    const std::array<std::array<size_t, Rank>, Count> &strides, Function &function,
    std::index_sequence<I...>, bool first,
    Offsets... offsets) noexcept(std::is_nothrow_invocable_r_v<bool, Function &, Offsets...>) {
    if constexpr (Level == Rank) {
        if (first)
            return true;
        return function(offsets...);
    } else if constexpr (Level + 1 == Rank) {
        size_t i = first ? 1 : 0;
        ((offsets += i * strides[I][Level]), ...);
        for (; i != shape[Level]; ++i) {
            if (!function(offsets...))
                return false;
            ((offsets += strides[I][Level]), ...);
        }
        return true;
    } else {
        for (size_t i = 0; i != shape[Level]; ++i) {
            if (!walk_expression_extremum<Level + 1>(
                    shape, strides, function, std::index_sequence<I...>{}, first, offsets...))
                return false;
            first = false;
            ((offsets += strides[I][Level]), ...);
        }
        return true;
    }
}

template <bool Maximum, tensor_expression E>
WJR_INTRINSIC_INLINE constexpr auto
extremum_expression(const E &expression) noexcept(noexcept(expression.read(size_t{}))) {
    using value_type = typename E::value_type;
    const auto domain = bind_expression_sum_domain(expression);
    WJR_ASSERT_L2(!domain.empty());
    constexpr auto inputs = expression_leaf_count<E>;
    const auto values = bind_expression_values(expression, std::make_index_sequence<inputs>{});
    value_type best = read_bound_expression<E>(values, size_t{});
    if constexpr (std::floating_point<value_type>)
        if (best != best)
            return best;
    if (is_linear_expression(domain, expression)) {
        for (size_t i = 1; i != domain.linear_size(); ++i) {
            const value_type value = read_bound_expression<E>(values, i);
            if constexpr (std::floating_point<value_type>)
                if (value != value)
                    return value;
            if (Maximum ? best < value : value < best)
                best = value;
        }
    } else {
        std::array<std::array<size_t, E::rank()>, inputs> strides{};
        bind_expression_strides(expression, strides);
        auto visit = [&](auto... offsets) noexcept(noexcept(expression.read(size_t{}))) {
            const value_type value = read_bound_expression<E>(
                values, std::array<size_t, inputs>{static_cast<size_t>(offsets)...});
            if constexpr (std::floating_point<value_type>)
                if (value != value) {
                    best = value;
                    return false;
                }
            if (Maximum ? best < value : value < best)
                best = value;
            return true;
        };
        [&]<size_t... I>(std::index_sequence<I...> operands) {
            walk_expression_extremum<0>(domain.shape, strides, visit, operands, true,
                                        decltype(I){}...);
        }(std::make_index_sequence<inputs>{});
    }
    return best;
}

template <bool Maximum, class Dst, tensor_expression E, size_t Axis, bool Keep>
WJR_INTRINSIC_INLINE constexpr void
extremum_expression_axis_to(Dst dst, const E &expression) noexcept(
    noexcept(expression.read(size_t{})) &&
    noexcept(dst.data_handle()[size_t{}] = expression.read(size_t{}))) {
    using value_type = typename E::value_type;
    constexpr auto rank = E::rank();
    constexpr auto inputs = expression_leaf_count<E>;
    const auto domain = bind_expression_sum_domain(expression);
    for (size_t axis = 0; axis != Dst::rank(); ++axis) {
        const auto source_axis = axis + (!Keep && axis >= Axis);
        const auto expected = Keep && axis == Axis ? size_t{1} : domain.extent(source_axis);
        WJR_ASSERT_L2(static_cast<size_t>(dst.extent(axis)) == expected);
    }
    if (dst.empty())
        return;
    const auto count = domain.extent(Axis);
    WJR_ASSERT_ASSUME_L2(count != 0);

    const auto values = bind_expression_values(expression, std::make_index_sequence<inputs>{});
    std::array<std::array<size_t, rank>, inputs> input_strides{};
    bind_expression_strides(expression, input_strides);
    std::array<size_t, inputs> steps{};
    for (size_t input = 0; input != inputs; ++input)
        steps[input] = input_strides[input][Axis];

    std::array<size_t, rank - 1> outer_shape{};
    std::array<std::array<size_t, rank - 1>, 1 + inputs> outer_strides{};
    if constexpr (rank > 1) {
        for (size_t axis = 0; axis != rank - 1; ++axis) {
            const auto source_axis = axis + (axis >= Axis);
            const auto output_axis = Keep && axis >= Axis ? axis + 1 : axis;
            outer_shape[axis] = domain.extent(source_axis);
            outer_strides[0][axis] = static_cast<size_t>(dst.stride(output_axis));
            for (size_t input = 0; input != inputs; ++input)
                outer_strides[1 + input][axis] = input_strides[input][source_axis];
        }
    }
    auto reduce_line =
        [output = dst.data_handle(), values, count, steps](
            size_t output_offset,
            auto... input_offsets) noexcept(noexcept(std::declval<const E &>().read(size_t{}))
                                                && noexcept(
                                                    std::declval<typename Dst::element_type &>() =
                                                        std::declval<value_type>())) {
            std::array<size_t, inputs> offsets{static_cast<size_t>(input_offsets)...};
            value_type best = read_bound_expression<E>(values, offsets);
            if constexpr (std::floating_point<value_type>) {
                if (best != best) {
                    output[output_offset] = best;
                    return;
                }
            }
            for (size_t input = 0; input != inputs; ++input)
                offsets[input] += steps[input];
            for (size_t i = 1; i != count; ++i) {
                const value_type value = read_bound_expression<E>(values, offsets);
                if constexpr (std::floating_point<value_type>) {
                    if (value != value) {
                        best = value;
                        output[output_offset] = best;
                        return;
                    }
                }
                if (Maximum ? best < value : value < best)
                    best = value;
                for (size_t input = 0; input != inputs; ++input)
                    offsets[input] += steps[input];
            }
            output[output_offset] = best;
        };
    walk_expression_sum(outer_shape, outer_strides, reduce_line,
                        std::make_index_sequence<1 + inputs>{});
}

} // namespace detail

// The trusted input domain must be nonempty. Values retain their expression
// dtype; the first equal value and first NaN follow the full-view extrema rules.
template <tensor_expression E>
requires detail::expression_element<typename E::value_type>
[[nodiscard]] WJR_INTRINSIC_INLINE constexpr auto(min)(const E &expression) noexcept(
    noexcept(expression.read(size_t{}))) {
    return detail::extremum_expression<false>(expression);
}

template <tensor_expression E>
requires detail::expression_element<typename E::value_type>
[[nodiscard]] WJR_INTRINSIC_INLINE constexpr auto(max)(const E &expression) noexcept(
    noexcept(expression.read(size_t{}))) {
    return detail::extremum_expression<true>(expression);
}

// Input leaves and output must be logically disjoint. An empty destination
// performs no expression reads; every reduced line must be nonempty.
template <class D, class Extents, class Layout, tensor_expression E, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<Layout> &&
         detail::expression_element<typename E::value_type> &&
         detail::compatible_sum_shape<Extents, typename E::extents_type, Axis, false> &&
         requires(D &out, typename E::value_type value) { out = value; })
WJR_INTRINSIC_INLINE constexpr void
min_to(tensor_view<D, Extents, Layout> dst, const E &expression,
       axes_t<Axis>) noexcept(noexcept(expression.read(size_t{})) &&
                              noexcept(dst.data_handle()[size_t{}] = expression.read(size_t{}))) {
    detail::extremum_expression_axis_to<false, decltype(dst), E, Axis, false>(dst, expression);
}

template <class D, class Extents, class Layout, tensor_expression E, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<Layout> &&
         detail::expression_element<typename E::value_type> &&
         detail::compatible_sum_shape<Extents, typename E::extents_type, Axis, false> &&
         requires(D &out, typename E::value_type value) { out = value; })
WJR_INTRINSIC_INLINE constexpr void
max_to(tensor_view<D, Extents, Layout> dst, const E &expression,
       axes_t<Axis>) noexcept(noexcept(expression.read(size_t{})) &&
                              noexcept(dst.data_handle()[size_t{}] = expression.read(size_t{}))) {
    detail::extremum_expression_axis_to<true, decltype(dst), E, Axis, false>(dst, expression);
}

template <class D, class Extents, class Layout, tensor_expression E, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<Layout> &&
         detail::expression_element<typename E::value_type> &&
         detail::compatible_sum_shape<Extents, typename E::extents_type, Axis, true> &&
         requires(D &out, typename E::value_type value) { out = value; })
WJR_INTRINSIC_INLINE constexpr void
min_to(tensor_view<D, Extents, Layout> dst, const E &expression, axes_t<Axis>,
       keepdims_t) noexcept(noexcept(expression.read(size_t{})) &&
                            noexcept(dst.data_handle()[size_t{}] = expression.read(size_t{}))) {
    detail::extremum_expression_axis_to<false, decltype(dst), E, Axis, true>(dst, expression);
}

template <class D, class Extents, class Layout, tensor_expression E, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<Layout> &&
         detail::expression_element<typename E::value_type> &&
         detail::compatible_sum_shape<Extents, typename E::extents_type, Axis, true> &&
         requires(D &out, typename E::value_type value) { out = value; })
WJR_INTRINSIC_INLINE constexpr void
max_to(tensor_view<D, Extents, Layout> dst, const E &expression, axes_t<Axis>,
       keepdims_t) noexcept(noexcept(expression.read(size_t{})) &&
                            noexcept(dst.data_handle()[size_t{}] = expression.read(size_t{}))) {
    detail::extremum_expression_axis_to<true, decltype(dst), E, Axis, true>(dst, expression);
}

// View overloads avoid requiring callers to wrap an eager source manually.
template <class D, class DE, class DL, class T, class SE, class SL, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<DL> &&
         detail::kernel_element<T> && detail::kernel_layout<SL> &&
         detail::compatible_sum_shape<DE, SE, Axis, false> &&
         requires(D &out, T value) { out = value; })
WJR_INTRINSIC_INLINE constexpr void min_to(tensor_view<D, DE, DL> dst,
                                           tensor_view<T, SE, SL> source, axes_t<Axis>) noexcept {
    min_to(dst, as_expr(source), axes<Axis>);
}

template <class D, class DE, class DL, class T, class SE, class SL, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<DL> &&
         detail::kernel_element<T> && detail::kernel_layout<SL> &&
         detail::compatible_sum_shape<DE, SE, Axis, false> &&
         requires(D &out, T value) { out = value; })
WJR_INTRINSIC_INLINE constexpr void max_to(tensor_view<D, DE, DL> dst,
                                           tensor_view<T, SE, SL> source, axes_t<Axis>) noexcept {
    max_to(dst, as_expr(source), axes<Axis>);
}

template <class D, class DE, class DL, class T, class SE, class SL, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<DL> &&
         detail::kernel_element<T> && detail::kernel_layout<SL> &&
         detail::compatible_sum_shape<DE, SE, Axis, true> &&
         requires(D &out, T value) { out = value; })
WJR_INTRINSIC_INLINE constexpr void min_to(tensor_view<D, DE, DL> dst,
                                           tensor_view<T, SE, SL> source, axes_t<Axis>,
                                           keepdims_t) noexcept {
    min_to(dst, as_expr(source), axes<Axis>, keepdims);
}

template <class D, class DE, class DL, class T, class SE, class SL, size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<DL> &&
         detail::kernel_element<T> && detail::kernel_layout<SL> &&
         detail::compatible_sum_shape<DE, SE, Axis, true> &&
         requires(D &out, T value) { out = value; })
WJR_INTRINSIC_INLINE constexpr void max_to(tensor_view<D, DE, DL> dst,
                                           tensor_view<T, SE, SL> source, axes_t<Axis>,
                                           keepdims_t) noexcept {
    max_to(dst, as_expr(source), axes<Axis>, keepdims);
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_EXTREMA_HPP__
