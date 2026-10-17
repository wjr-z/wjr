#ifndef WJR_NP_EXPRESSION_REDUCE_HPP__
#define WJR_NP_EXPRESSION_REDUCE_HPP__

#include <wjr/np/expression.hpp>
#include <wjr/np/reduce.hpp>

namespace wjr::np {

namespace detail {

// Metadata only: no dummy mdspan or fictitious pointer range is constructed.
template <class Extents>
struct expression_sum_domain {
    using layout_type = layout_right;
    std::array<size_t, Extents::rank()> shape{};
    static constexpr size_t rank() noexcept { return Extents::rank(); }
    constexpr size_t extent(size_t axis) const noexcept { return shape[axis]; }
    constexpr bool empty() const noexcept {
        for (auto count : shape)
            if (count == 0)
                return true;
        return false;
    }
    constexpr size_t linear_size() const noexcept {
        if constexpr (Extents::rank_dynamic() == 0) {
            constexpr auto count = [] {
                size_t result = 1;
                for (size_t axis = 0; axis != Extents::rank(); ++axis)
                    result *= Extents::static_extent(axis);
                return result;
            }();
            return count;
        } else {
            size_t result = 1;
            for (auto count : shape)
                result *= count;
            return result;
        }
    }
};

template <tensor_expression E>
WJR_INTRINSIC_INLINE constexpr auto bind_expression_sum_domain(const E &expression) noexcept {
    using extents_type = typename E::extents_type;
    expression_sum_domain<extents_type> domain;
    if constexpr (extents_type::rank_dynamic() == 0) {
        for (size_t axis = 0; axis != E::rank(); ++axis)
            domain.shape[axis] = extents_type::static_extent(axis);
        assert_expression_shape(domain, expression);
    } else {
        std::array<typename extents_type::index_type, E::rank()> shape{};
        [[maybe_unused]] const bool compatible = try_expression_shape(expression, shape);
        WJR_ASSERT_L2(compatible);
        for (size_t axis = 0; axis != E::rank(); ++axis)
            domain.shape[axis] = static_cast<size_t>(shape[axis]);
    }
    return domain;
}

template <tensor_expression E, class Accumulator, class Values, size_t Count, size_t... I>
WJR_INTRINSIC_INLINE constexpr Accumulator
sum_expression_line(Values values, size_t count, std::array<size_t, Count> offsets,
                    std::array<size_t, Count> steps, std::index_sequence<I...>,
                    Accumulator result = {}) noexcept(noexcept(std::declval<const E &>().read(0))) {
    for (size_t i = 0; i != count; ++i) {
        result += static_cast<Accumulator>(read_bound_expression<E>(values, offsets));
        ((offsets[I] += steps[I]), ...);
    }
    return result;
}

template <tensor_expression E, class Accumulator, class Values>
WJR_INTRINSIC_INLINE constexpr Accumulator
sum_linear_expression(Values values,
                      size_t count) noexcept(noexcept(std::declval<const E &>().read(0))) {
    Accumulator result{};
    for (size_t i = 0; i != count; ++i)
        result += static_cast<Accumulator>(read_bound_expression<E>(values, i));
    return result;
}

template <size_t Rank, size_t Count, class Function, size_t... I>
WJR_INTRINSIC_INLINE constexpr void walk_expression_sum(
    const std::array<size_t, Rank> &shape,
    const std::array<std::array<size_t, Rank>, Count> &strides, Function &function,
    std::index_sequence<I...>
        operands) noexcept(std::is_nothrow_invocable_v<Function &, decltype(I)...>) {
    walk_kernel<0, false>(shape, strides, function, operands, decltype(I){}...);
}

template <class Accumulator, size_t Axis, bool Keep, class Dst, tensor_expression E>
WJR_INTRINSIC_INLINE constexpr void
sum_expression_axis_to(Dst dst, const E &expression) noexcept(noexcept(expression.read(size_t{}))) {
    using accumulator_type = sum_accumulator<Accumulator, typename E::value_type>;
    constexpr auto rank = E::rank();
    constexpr auto inputs = expression_leaf_count<E>;
    const auto domain = bind_expression_sum_domain(expression);
    for (size_t axis = 0; axis != Dst::rank(); ++axis) {
        [[maybe_unused]] const auto expected =
            Keep && axis == Axis ? size_t{1} : domain.extent(axis + (!Keep && axis >= Axis));
        WJR_ASSERT_L2(static_cast<size_t>(dst.extent(axis)) == expected);
    }
    if (dst.empty())
        return;
    const auto count = domain.extent(Axis);
    if (count == 0) {
        apply_kernel(dst, [](typename Dst::reference value) noexcept { value = {}; });
        return;
    }
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
            outer_shape[axis] = domain.extent(source_axis);
            outer_strides[0][axis] = static_cast<size_t>(dst.stride(Keep ? source_axis : axis));
            for (size_t input = 0; input != inputs; ++input)
                outer_strides[1 + input][axis] = input_strides[input][source_axis];
        }
    }
    auto reduce = [output = dst.data_handle(), values, count,
                   steps](size_t output_offset,
                          auto... input_offsets) noexcept(noexcept(expression.read(size_t{}))) {
        constexpr auto static_count = E::static_extent(Axis);
        const auto bound = static_count == dynamic_extent ? count : static_count;
        output[output_offset] = sum_expression_line<E, accumulator_type>(
            values, bound, std::array<size_t, expression_leaf_count<E>>{input_offsets...}, steps,
            std::make_index_sequence<expression_leaf_count<E>>{});
    };
    walk_expression_sum(outer_shape, outer_strides, reduce, std::make_index_sequence<1 + inputs>{});
}

} // namespace detail

// Terminal ordered fold; no intermediate tensor, allocation or lazy reducer.
template <class Accumulator = void, tensor_expression E>
requires(detail::expression_element<typename E::value_type> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>))
[[nodiscard]] WJR_INTRINSIC_INLINE constexpr auto
sum(const E &expression) noexcept(noexcept(expression.read(size_t{}))) {
    using accumulator_type = detail::sum_accumulator<Accumulator, typename E::value_type>;
    constexpr auto inputs = detail::expression_leaf_count<E>;
    const auto domain = detail::bind_expression_sum_domain(expression);
    if (domain.empty())
        return accumulator_type{};
    const auto values =
        detail::bind_expression_values(expression, std::make_index_sequence<inputs>{});
    if constexpr (E::rank() == 0) {
        return detail::sum_linear_expression<E, accumulator_type>(values, domain.linear_size());
    } else {
        if (detail::is_linear_expression(domain, expression))
            return detail::sum_linear_expression<E, accumulator_type>(values, domain.linear_size());
        constexpr auto rank = E::rank();
        std::array<std::array<size_t, rank>, inputs> strides{};
        detail::bind_expression_strides(expression, strides);
        const auto count = domain.extent(rank - 1);
        std::array<size_t, inputs> steps{};
        for (size_t input = 0; input != inputs; ++input)
            steps[input] = strides[input][rank - 1];
        std::array<size_t, rank - 1> outer_shape{};
        std::array<std::array<size_t, rank - 1>, inputs> outer_strides{};
        for (size_t axis = 0; axis != rank - 1; ++axis) {
            outer_shape[axis] = domain.extent(axis);
            for (size_t input = 0; input != inputs; ++input)
                outer_strides[input][axis] = strides[input][axis];
        }
        accumulator_type result{};
        auto add_row = [values, count, steps,
                        &result](auto... offsets) noexcept(noexcept(expression.read(size_t{}))) {
            result = detail::sum_expression_line<E, accumulator_type>(
                values, count, std::array<size_t, detail::expression_leaf_count<E>>{offsets...},
                steps, std::make_index_sequence<detail::expression_leaf_count<E>>{}, result);
        };
        detail::walk_expression_sum(outer_shape, outer_strides, add_row,
                                    std::make_index_sequence<inputs>{});
        return result;
    }
}

// Input leaves and output must be logically disjoint; output shape is the
// expression's natural broadcast domain with exactly one axis reduced.
template <class Accumulator = void, class D, class Extents, class Layout, tensor_expression E,
          size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<Layout> &&
         detail::expression_element<typename E::value_type> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>) &&
         detail::compatible_sum_shape<Extents, typename E::extents_type, Axis, false>)
WJR_INTRINSIC_INLINE constexpr void
sum_to(tensor_view<D, Extents, Layout> dst, const E &expression,
       axes_t<Axis>) noexcept(noexcept(expression.read(size_t{}))) {
    detail::sum_expression_axis_to<Accumulator, Axis, false>(dst, expression);
}

template <class Accumulator = void, class D, class Extents, class Layout, tensor_expression E,
          size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<Layout> &&
         detail::expression_element<typename E::value_type> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>) &&
         detail::compatible_sum_shape<Extents, typename E::extents_type, Axis, true>)
WJR_INTRINSIC_INLINE constexpr void
sum_to(tensor_view<D, Extents, Layout> dst, const E &expression, axes_t<Axis>,
       keepdims_t) noexcept(noexcept(expression.read(size_t{}))) {
    detail::sum_expression_axis_to<Accumulator, Axis, true>(dst, expression);
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_REDUCE_HPP__
