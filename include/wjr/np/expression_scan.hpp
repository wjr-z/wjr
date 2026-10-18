#ifndef WJR_NP_EXPRESSION_SCAN_HPP__
#define WJR_NP_EXPRESSION_SCAN_HPP__

#include <wjr/np/detail/expression_domain.hpp>
#include <wjr/np/scan.hpp>

namespace wjr::np {

namespace detail {

template <tensor_expression E, class Accumulator, class D, class Values>
WJR_INTRINSIC_INLINE constexpr void
scan_linear_expression(D *output, Values values, size_t lines,
                       size_t count) noexcept(noexcept(std::declval<const E &>().read(size_t{}))) {
    size_t first = 0;
    for (size_t line = 0; line != lines; ++line) {
        Accumulator state{};
        for (size_t i = 0; i != count; ++i) {
            state += static_cast<Accumulator>(read_bound_expression<E>(values, first + i));
            output[first + i] = state;
        }
        first += count;
    }
}

template <tensor_expression E, class Accumulator, class D, class Values, size_t Count, size_t... I>
WJR_INTRINSIC_INLINE constexpr void scan_expression_line(
    D *output, Values values, size_t count, size_t output_step, std::array<size_t, Count> offsets,
    const std::array<size_t, Count> &steps,
    std::index_sequence<I...>) noexcept(noexcept(std::declval<const E &>().read(size_t{}))) {
    Accumulator state{};
    for (size_t i = 0; i != count; ++i) {
        state += static_cast<Accumulator>(read_bound_expression<E>(values, offsets));
        output[i * output_step] = state;
        ((offsets[I] += steps[I]), ...);
    }
}

// Independent columns keep independent fold states. The caller proves that
// interleaving reads cannot change observable side effects or failure order.
template <bool Unit, tensor_expression E, class Accumulator, class D, class Values, size_t Count,
          size_t... I>
requires(noexcept(std::declval<const E &>().read(size_t{})))
WJR_INTRINSIC_INLINE constexpr void
scan_expression_columns_to(D *output, Values values, size_t rows, size_t columns,
                           size_t output_row_step, const std::array<size_t, Count> &row_steps,
                           const std::array<size_t, Count> &column_steps,
                           std::index_sequence<I...> operands) noexcept {
    const auto bound = columns - columns % 4;
    size_t first = 0;
    for (; first != bound; first += 4) {
        std::array<size_t, Count> offsets{(Unit ? first : first * column_steps[I])...};
        Accumulator a{}, b{}, c{}, d{};
        auto output_offset = first;
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
            output[output_offset] = a;
            output[output_offset + 1] = b;
            output[output_offset + 2] = c;
            output[output_offset + 3] = d;
            output_offset += output_row_step;
            ((offsets[I] += row_steps[I]), ...);
        }
    }
    for (; first != columns; ++first)
        scan_expression_line<E, Accumulator>(output + first, values, rows, output_row_step,
                                             std::array<size_t, Count>{first * column_steps[I]...},
                                             row_steps, operands);
}

} // namespace detail

// Preconditions: dst matches the expression's natural broadcast shape. Each
// leaf is logically disjoint from dst or has exactly the same index mapping;
// a broadcast leaf must be disjoint. Each line folds in axis order from zero.
// A throwing read leaves previously completed prefixes committed.
template <class Accumulator = void, class D, class Extents, class Layout, tensor_expression E,
          size_t Axis>
requires(detail::kernel_element<D> && !std::is_const_v<D> && detail::kernel_layout<Layout> &&
         detail::expression_element<typename E::value_type> &&
         (std::is_void_v<Accumulator> || detail::sum_accumulator_type<Accumulator>) &&
         detail::compatible_kernel_shape<Extents, typename E::extents_type> && Axis < E::rank())
WJR_INTRINSIC_INLINE constexpr void
cumsum_to(tensor_view<D, Extents, Layout> dst, const E &expression,
          axes_t<Axis>) noexcept(noexcept(expression.read(size_t{}))) {
    using accumulator_type = detail::sum_accumulator<Accumulator, typename E::value_type>;
    constexpr auto rank = E::rank();
    constexpr auto inputs = detail::expression_leaf_count<E>;
    const auto domain = detail::bind_expression_sum_domain(expression);
    detail::assert_kernel_shape(dst, domain);
    if (dst.empty())
        return;
    const auto count = domain.extent(Axis);
    const auto output_step = static_cast<size_t>(detail::axis_stride<Axis>(dst));
    const auto values =
        detail::bind_expression_values(expression, std::make_index_sequence<inputs>{});
    if constexpr ((std::same_as<Layout, layout_right> && Axis == rank - 1) ||
                  (std::same_as<Layout, layout_left> && Axis == 0 && rank <= 2)) {
        if (detail::is_linear_expression(dst, expression)) {
            size_t lines = 1;
            for (size_t axis = 0; axis != rank; ++axis)
                if (axis != Axis)
                    lines *= domain.extent(axis);
            constexpr auto static_count = E::static_extent(Axis);
            const auto bound = static_count == dynamic_extent ? count : static_count;
            detail::scan_linear_expression<E, accumulator_type>(dst.data_handle(), values, lines,
                                                                bound);
            return;
        }
    }
    std::array<std::array<size_t, rank>, inputs> input_strides{};
    detail::bind_expression_strides(expression, input_strides);
    std::array<size_t, inputs> steps{};
    for (size_t input = 0; input != inputs; ++input)
        steps[input] = input_strides[input][Axis];
    if constexpr (rank == 2 && Axis == 0 && noexcept(expression.read(size_t{})) &&
                  detail::interleavable_sum_expression<E>) {
        const auto columns = domain.extent(1);
        if (columns >= 4 && detail::axis_stride<1>(dst) == 1) {
            std::array<size_t, inputs> column_steps{};
            bool unit_or_splat = true;
            for (size_t input = 0; input != inputs; ++input) {
                column_steps[input] = input_strides[input][1];
                unit_or_splat &= column_steps[input] <= 1;
            }
            if (unit_or_splat) {
                const bool unit = [&]<size_t... I>(std::index_sequence<I...>) {
                    using data_type =
                        std::remove_cvref_t<decltype(detail::expression_values(values))>;
                    return ((!std::is_pointer_v<std::tuple_element_t<I, data_type>> ||
                             column_steps[I] == 1) &&
                            ...);
                }(std::make_index_sequence<inputs>{});
                if (unit)
                    detail::scan_expression_columns_to<true, E, accumulator_type>(
                        dst.data_handle(), values, count, columns, output_step, steps, column_steps,
                        std::make_index_sequence<inputs>{});
                else
                    detail::scan_expression_columns_to<false, E, accumulator_type>(
                        dst.data_handle(), values, count, columns, output_step, steps, column_steps,
                        std::make_index_sequence<inputs>{});
                return;
            }
        }
    }
    std::array<size_t, rank - 1> outer_shape{};
    std::array<std::array<size_t, rank - 1>, 1 + inputs> outer_strides{};
    for (size_t axis = 0; axis != rank - 1; ++axis) {
        const auto source_axis = axis + (axis >= Axis);
        outer_shape[axis] = domain.extent(source_axis);
        outer_strides[0][axis] = static_cast<size_t>(dst.stride(source_axis));
        for (size_t input = 0; input != inputs; ++input)
            outer_strides[1 + input][axis] = input_strides[input][source_axis];
    }
    auto scan = [output = dst.data_handle(), values, count, output_step,
                 steps](size_t output_offset,
                        auto... input_offsets) noexcept(noexcept(expression.read(size_t{}))) {
        constexpr auto static_count = E::static_extent(Axis);
        const auto bound = static_count == dynamic_extent ? count : static_count;
        detail::scan_expression_line<E, accumulator_type>(
            output + output_offset, values, bound, output_step,
            std::array<size_t, detail::expression_leaf_count<E>>{input_offsets...}, steps,
            std::make_index_sequence<detail::expression_leaf_count<E>>{});
    };
    detail::walk_expression_sum(outer_shape, outer_strides, scan,
                                std::make_index_sequence<1 + inputs>{});
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_SCAN_HPP__
