#ifndef WJR_NP_EXPRESSION_REDUCE_HPP__
#define WJR_NP_EXPRESSION_REDUCE_HPP__

#include <wjr/np/detail/expression_domain.hpp>
#include <wjr/np/reduce.hpp>

namespace wjr::np {

namespace detail {

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

// Each lane is an independent ordered fold. N changes interleaving only;
// it does not regroup additions within a column.
template <size_t N, bool Unit, tensor_expression E, class Accumulator, class D, class Values,
          size_t Count, size_t... I>
requires(N > 0)
WJR_INTRINSIC_INLINE constexpr void
sum_expression_columns_block(D *output, const Values &values, size_t rows, size_t first,
                             size_t output_step, const std::array<size_t, Count> &row_steps,
                             const std::array<size_t, Count> &column_steps,
                             std::index_sequence<I...>) noexcept {
    if constexpr (N == 4) {
        // Preserve the reviewed default's register allocation on Clang.
        std::array<size_t, Count> offsets{(Unit ? first : first * column_steps[I])...};
        Accumulator a{}, b{}, c{}, d{};
        for (size_t row = 0; row != rows; ++row) {
            a += static_cast<Accumulator>(read_bound_expression<E>(values, offsets));
            b += static_cast<Accumulator>(read_bound_expression<E>(
                values, std::array<size_t, Count>{(offsets[I] + (Unit ? 1 : column_steps[I]))...}));
            c += static_cast<Accumulator>(read_bound_expression<E>(
                values,
                std::array<size_t, Count>{(offsets[I] + (Unit ? 2 : 2 * column_steps[I]))...}));
            d += static_cast<Accumulator>(read_bound_expression<E>(
                values,
                std::array<size_t, Count>{(offsets[I] + (Unit ? 3 : 3 * column_steps[I]))...}));
            ((offsets[I] += row_steps[I]), ...);
        }
        output[first * output_step] = a;
        output[(first + 1) * output_step] = b;
        output[(first + 2) * output_step] = c;
        output[(first + 3) * output_step] = d;
    } else {
        std::array<size_t, Count> offsets{(Unit ? first : first * column_steps[I])...};
        std::array<Accumulator, N> states{};
        auto add = [&]<size_t Column>() {
            states[Column] += static_cast<Accumulator>(read_bound_expression<E>(
                values, std::array<size_t, Count>{
                            (offsets[I] + (Unit ? Column : Column * column_steps[I]))...}));
        };
        for (size_t row = 0; row != rows; ++row) {
            [&]<size_t... Column>(std::index_sequence<Column...>) {
                (add.template operator()<Column>(), ...);
            }(std::make_index_sequence<N>{});
            ((offsets[I] += row_steps[I]), ...);
        }
        [&]<size_t... Column>(std::index_sequence<Column...>) {
            ((output[(first + Column) * output_step] = states[Column]), ...);
        }(std::make_index_sequence<N>{});
    }
}

template <size_t N, bool Unit, tensor_expression E, class Accumulator, class D, class Values,
          size_t Count, size_t... I>
requires(N > 0)
WJR_INTRINSIC_INLINE constexpr void
sum_expression_columns_to(D *output, const Values &values, size_t rows, size_t columns,
                          size_t output_step, const std::array<size_t, Count> &row_steps,
                          const std::array<size_t, Count> &column_steps,
                          std::index_sequence<I...> operands) noexcept {
    const auto bound = columns - columns % N;
    size_t first = 0;
    for (; first != bound; first += N)
        sum_expression_columns_block<N, Unit, E, Accumulator>(
            output, values, rows, first, output_step, row_steps, column_steps, operands);
    if constexpr (N > 4) {
        for (; columns - first >= 4; first += 4)
            sum_expression_columns_block<4, Unit, E, Accumulator>(
                output, values, rows, first, output_step, row_steps, column_steps, operands);
    }
    for (; first != columns; ++first)
        output[first * output_step] = sum_expression_line<E, Accumulator>(
            values, rows, std::array<size_t, Count>{first * column_steps[I]...}, row_steps,
            operands);
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
    if constexpr (rank == 2 && Axis == 0 && noexcept(expression.read(size_t{})) &&
                  interleavable_sum_expression<E>) {
        const auto columns = domain.extent(1);
        if (columns >= 4) {
            std::array<size_t, inputs> column_steps{};
            bool unit_or_splat = true;
            for (size_t input = 0; input != inputs; ++input) {
                column_steps[input] = input_strides[input][1];
                unit_or_splat &= column_steps[input] <= 1;
            }
            if (unit_or_splat) {
                const bool unit = [&]<size_t... I>(std::index_sequence<I...>) {
                    using data_type = std::remove_cvref_t<decltype(expression_values(values))>;
                    return ((!std::is_pointer_v<std::tuple_element_t<I, data_type>> ||
                             column_steps[I] == 1) &&
                            ...);
                }(std::make_index_sequence<inputs>{});
                const auto output_step = static_cast<size_t>(dst.stride(Keep ? 1 : 0));
                if (unit)
                    sum_expression_columns_to<4, true, E, accumulator_type>(
                        dst.data_handle(), values, count, columns, output_step, steps, column_steps,
                        std::make_index_sequence<inputs>{});
                else
                    sum_expression_columns_to<4, false, E, accumulator_type>(
                        dst.data_handle(), values, count, columns, output_step, steps, column_steps,
                        std::make_index_sequence<inputs>{});
                return;
            }
        }
    }
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
