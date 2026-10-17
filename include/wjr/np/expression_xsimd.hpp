#ifndef WJR_NP_EXPRESSION_XSIMD_HPP__
#define WJR_NP_EXPRESSION_XSIMD_HPP__

#include <wjr/np/expression.hpp>
#include <wjr/simd/xsimd.hpp>

namespace wjr::np {

// Explicit backend selection keeps ISA-dependent code out of the scalar API.
// Link both wjr::np and wjr::simd_xsimd when using this header.
template <class Arch = xsimd::default_arch>
struct xsimd_policy {};

namespace detail {

template <class Op>
inline constexpr bool xsimd_unary_operation =
    std::same_as<Op, positive_operation> || std::same_as<Op, negative_operation>;

template <class Op>
inline constexpr bool xsimd_binary_operation =
    std::same_as<Op, add_operation> || std::same_as<Op, subtract_operation> ||
    std::same_as<Op, multiply_operation> || std::same_as<Op, divide_operation>;

template <class E, class T>
inline constexpr bool homogeneous_xsimd_expression = false;

template <class View, class T>
inline constexpr bool homogeneous_xsimd_expression<view_expression<View>, T> =
    std::same_as<typename View::value_type, T> &&
    std::same_as<typename View::accessor_type, default_accessor<typename View::element_type>> &&
    expression_layout<typename View::layout_type>;

template <class S, class T>
inline constexpr bool homogeneous_xsimd_expression<scalar_expression<S>, T> = std::same_as<S, T>;

template <class Op, class Child, class T>
inline constexpr bool homogeneous_xsimd_expression<unary_expression<Op, Child>, T> =
    xsimd_unary_operation<Op> &&
    std::same_as<typename unary_expression<Op, Child>::value_type, T> &&
    homogeneous_xsimd_expression<Child, T>;

template <class Op, class Left, class Right, class T>
inline constexpr bool homogeneous_xsimd_expression<binary_expression<Op, Left, Right>, T> =
    xsimd_binary_operation<Op> &&
    std::same_as<typename binary_expression<Op, Left, Right>::value_type, T> &&
    homogeneous_xsimd_expression<Left, T> && homogeneous_xsimd_expression<Right, T>;

template <tensor_expression E, class Batch, size_t Offset = 0, class Values>
WJR_INTRINSIC_INLINE Batch read_expression_batch(const Values &values, size_t i) noexcept {
    if constexpr (E::kind == expression_kind::view) {
        return Batch::load_unaligned(std::get<Offset>(values) + i);
    } else if constexpr (E::kind == expression_kind::scalar) {
        return Batch(std::get<Offset>(values));
    } else if constexpr (E::kind == expression_kind::unary) {
        using operation = typename E::operation_type;
        return operation::apply(
            read_expression_batch<typename E::child_type, Batch, Offset>(values, i));
    } else {
        using operation = typename E::operation_type;
        return operation::apply(
            read_expression_batch<typename E::left_type, Batch, Offset>(values, i),
            read_expression_batch<typename E::right_type, Batch,
                                  Offset + expression_leaf_count<typename E::left_type>>(values,
                                                                                         i));
    }
}

// Every projected row uses only statically known unit loads or invariant splats.
enum class row_load_kind { unsupported, unit, splat };

template <tensor_expression E>
inline constexpr row_load_kind expression_row_load = [] {
    if constexpr (E::kind == expression_kind::scalar || E::rank() == 0)
        return row_load_kind::splat;
    else if constexpr (E::kind == expression_kind::view) {
        if constexpr (!std::same_as<typename E::view_type::layout_type, layout_right>)
            return row_load_kind::unsupported;
        else if constexpr (E::static_extent(E::rank() - 1) == 1)
            return row_load_kind::splat;
        else if constexpr (E::static_extent(E::rank() - 1) != dynamic_extent &&
                           E::static_extent(E::rank() - 1) > 1)
            return row_load_kind::unit;
        else
            return row_load_kind::unsupported;
    } else if constexpr (E::kind == expression_kind::unary) {
        return expression_row_load<typename E::child_type>;
    } else {
        constexpr auto left = expression_row_load<typename E::left_type>;
        constexpr auto right = expression_row_load<typename E::right_type>;
        if constexpr (left == row_load_kind::unsupported || right == row_load_kind::unsupported)
            return row_load_kind::unsupported;
        else if constexpr (left == row_load_kind::splat && right == row_load_kind::splat)
            return row_load_kind::splat;
        else
            return row_load_kind::unit;
    }
}();

template <size_t Leaf, tensor_expression E, class Batch, size_t Offset = 0, class Values,
          size_t Count>
WJR_INTRINSIC_INLINE auto xsimd_row_slot(const Values &values,
                                         const std::array<size_t, Count> &offsets) noexcept {
    if constexpr (E::kind == expression_kind::unary) {
        return xsimd_row_slot<Leaf, typename E::child_type, Batch, Offset>(values, offsets);
    } else if constexpr (E::kind == expression_kind::binary) {
        if constexpr (Leaf < expression_leaf_count<typename E::left_type>)
            return xsimd_row_slot<Leaf, typename E::left_type, Batch, Offset>(values, offsets);
        else
            return xsimd_row_slot<Leaf - expression_leaf_count<typename E::left_type>,
                                  typename E::right_type, Batch,
                                  Offset + expression_leaf_count<typename E::left_type>>(values,
                                                                                         offsets);
    } else {
        static_assert(Leaf == 0);
        if constexpr (E::kind == expression_kind::scalar)
            return Batch(std::get<Offset>(values));
        else if constexpr (expression_row_load<E> == row_load_kind::splat)
            return Batch(std::get<Offset>(values)[offsets[Offset]]);
        else
            return std::get<Offset>(values) + offsets[Offset];
    }
}

template <tensor_expression E, class Batch, size_t Offset = 0, class Values>
WJR_INTRINSIC_INLINE Batch read_xsimd_row(const Values &values, size_t i) noexcept {
    if constexpr (E::kind == expression_kind::scalar || E::kind == expression_kind::view) {
        if constexpr (expression_row_load<E> == row_load_kind::splat)
            return std::get<Offset>(values);
        else
            return Batch::load_unaligned(std::get<Offset>(values) + i);
    } else if constexpr (E::kind == expression_kind::unary) {
        return E::operation_type::apply(
            read_xsimd_row<typename E::child_type, Batch, Offset>(values, i));
    } else {
        return E::operation_type::apply(
            read_xsimd_row<typename E::left_type, Batch, Offset>(values, i),
            read_xsimd_row<typename E::right_type, Batch,
                           Offset + expression_leaf_count<typename E::left_type>>(values, i));
    }
}

template <tensor_expression E, size_t Offset = 0, class Values>
WJR_INTRINSIC_INLINE typename E::value_type read_xsimd_row_scalar(const Values &values,
                                                                  size_t i) noexcept {
    if constexpr (E::kind == expression_kind::scalar || E::kind == expression_kind::view) {
        if constexpr (expression_row_load<E> == row_load_kind::splat)
            return std::get<Offset>(values).first();
        else
            return std::get<Offset>(values)[i];
    } else if constexpr (E::kind == expression_kind::unary) {
        return E::operation_type::apply(
            read_xsimd_row_scalar<typename E::child_type, Offset>(values, i));
    } else {
        return E::operation_type::apply(
            read_xsimd_row_scalar<typename E::left_type, Offset>(values, i),
            read_xsimd_row_scalar<typename E::right_type,
                                  Offset + expression_leaf_count<typename E::left_type>>(values,
                                                                                         i));
    }
}

template <class Arch>
struct xsimd_row_executor {
    template <tensor_expression E, class T, class Values, size_t Count, size_t... I>
    static WJR_INTRINSIC_INLINE void apply(T *output, size_t count, Values values,
                                           std::array<size_t, Count> offsets,
                                           std::index_sequence<I...>) noexcept {
        using batch = xsimd::batch<T, Arch>;
        const auto row = std::tuple{xsimd_row_slot<I, E, batch>(values, offsets)...};
        const auto last_batch = count - count % batch::size;
        size_t i = 0;
        for (; i != last_batch; i += batch::size)
            read_xsimd_row<E, batch>(row, i).store_unaligned(output + i);
        for (; i != count; ++i)
            output[i] = read_xsimd_row_scalar<E>(row, i);
    }

    template <tensor_expression E, class T, class Values>
    static WJR_INTRINSIC_INLINE auto bind(T *output, Values values, size_t count) noexcept {
        constexpr auto fixed_count = [] {
            if constexpr (E::rank() != 0) {
                constexpr auto extent = E::static_extent(E::rank() - 1);
                if constexpr (extent != dynamic_extent && extent > 1)
                    return extent;
            }
            return dynamic_extent;
        }();
        if constexpr (fixed_count != dynamic_extent) {
            return [output, values](size_t output_offset, auto... input_offsets) noexcept {
                apply<E>(output + output_offset, E::static_extent(E::rank() - 1), values,
                         std::array<size_t, expression_leaf_count<E>>{input_offsets...},
                         std::make_index_sequence<expression_leaf_count<E>>{});
            };
        } else {
            return [output, values, count](size_t output_offset, auto... input_offsets) noexcept {
                apply<E>(output + output_offset, count, values,
                         std::array<size_t, expression_leaf_count<E>>{input_offsets...},
                         std::make_index_sequence<expression_leaf_count<E>>{});
            };
        }
    }
};

} // namespace detail

// Same trusted shape and pointwise alias preconditions as scalar eval_to.
// Mixed dtypes deliberately retain scalar per-node promotions and conversions.
template <class T, class Extents, class Layout, class Expr, class Arch>
requires requires(tensor_view<T, Extents, Layout> dst, const Expr &expr) { eval_to(dst, expr); }
WJR_INTRINSIC_INLINE void eval_to(tensor_view<T, Extents, Layout> dst, const Expr &expr,
                                  xsimd_policy<Arch>) noexcept(noexcept(eval_to(dst, expr))) {
    if constexpr ((std::same_as<T, float> || std::same_as<T, double>) &&
                  xsimd::has_simd_register<T, Arch>::value &&
                  detail::homogeneous_xsimd_expression<Expr, T>) {
        // The scalar entry point's shape binding is shared below; the loop does
        // not perform metadata checks, allocate storage, or assume alignment.
        detail::assert_expression_shape(dst, expr);
        if (dst.empty())
            return;
        if (!detail::is_linear_expression(dst, expr)) {
            const auto values = detail::bind_expression_values(
                expr, std::make_index_sequence<detail::expression_leaf_count<Expr>>{});
            if constexpr (std::same_as<Layout, layout_right> && Extents::rank() != 0 &&
                          detail::expression_row_load<Expr> != detail::row_load_kind::unsupported) {
                detail::eval_projected_expression<detail::xsimd_row_executor<Arch>>(dst, expr,
                                                                                    values);
            } else {
                detail::eval_projected_expression(dst, expr, values);
            }
            return;
        }
        const auto values = detail::bind_expression_values(
            expr, std::make_index_sequence<detail::expression_leaf_count<Expr>>{});
        using batch = xsimd::batch<T, Arch>;
        const auto count = static_cast<size_t>(dst.size());
        auto *output = dst.data_handle();
        size_t offset = 0;
        for (; count - offset >= batch::size; offset += batch::size) {
            detail::read_expression_batch<Expr, batch>(values, offset)
                .store_unaligned(output + offset);
        }
        for (; offset != count; ++offset)
            output[offset] = detail::read_bound_expression<Expr>(values, offset);
    } else {
        eval_to(dst, expr);
    }
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_XSIMD_HPP__
