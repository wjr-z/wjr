#ifndef WJR_NP_EXPRESSION_XSIMD_HPP__
#define WJR_NP_EXPRESSION_XSIMD_HPP__

#include <wjr/np/detail/xsimd_operation.hpp>

namespace wjr::np {

// Explicit backend selection keeps ISA-dependent code out of the scalar API.
// Link both wjr::np and wjr::simd_xsimd when using this header.
template <class Arch = xsimd::default_arch>
struct xsimd_policy {};

namespace detail {

template <class T>
inline constexpr bool xsimd_numeric_lane = std::same_as<T, float> || std::same_as<T, double>;

// Numeric intermediates must retain the selected lane type. Predicate results
// instead use that numeric batch's mask, independently of scalar bool storage.
template <class E, class T, xsimd_result_kind Result>
inline constexpr auto xsimd_typed_result =
    Result == xsimd_result_kind::numeric && !std::same_as<typename E::value_type, T>
        ? xsimd_result_kind::unsupported
        : Result;

template <class E, class T>
inline constexpr xsimd_result_kind xsimd_expression_result = xsimd_result_kind::unsupported;

template <class V, class T>
inline constexpr xsimd_result_kind xsimd_expression_result<view_expression<V>, T> =
    xsimd_numeric_lane<T> && std::same_as<typename V::value_type, T> &&
            std::same_as<typename V::accessor_type, default_accessor<typename V::element_type>> &&
            expression_layout<typename V::layout_type>
        ? xsimd_result_kind::numeric
        : xsimd_result_kind::unsupported;
template <class S, class T>
inline constexpr xsimd_result_kind xsimd_expression_result<scalar_expression<S>, T> =
    !xsimd_numeric_lane<T>  ? xsimd_result_kind::unsupported
    : std::same_as<S, T>    ? xsimd_result_kind::numeric
    : std::same_as<S, bool> ? xsimd_result_kind::predicate
                            : xsimd_result_kind::unsupported;
template <class Op, class C, class T>
inline constexpr xsimd_result_kind xsimd_expression_result<unary_expression<Op, C>, T> =
    xsimd_typed_result<unary_expression<Op, C>, T,
                       xsimd_operation<Op>::template result<xsimd_expression_result<C, T>>>;

template <class Op, class L, class R, class T>
inline constexpr xsimd_result_kind xsimd_expression_result<binary_expression<Op, L, R>, T> =
    xsimd_typed_result<binary_expression<Op, L, R>, T,
                       xsimd_operation<Op>::template result<xsimd_expression_result<L, T>,
                                                            xsimd_expression_result<R, T>>>;

template <class M, class Y, class N, class T>
inline constexpr xsimd_result_kind xsimd_expression_result<select_expression<M, Y, N>, T> =
    xsimd_typed_result<select_expression<M, Y, N>, T,
                       xsimd_operation<select_operation>::template result<
                           xsimd_expression_result<M, T>, xsimd_expression_result<Y, T>,
                           xsimd_expression_result<N, T>>>;

template <class E, class T>
inline constexpr bool homogeneous_xsimd_expression =
    xsimd_expression_result<E, T> == xsimd_result_kind::numeric;

template <tensor_expression E, class Batch, size_t Offset = 0, class Values>
WJR_INTRINSIC_INLINE auto read_expression_batch(const Values &values, size_t i) noexcept {
    if constexpr (E::kind == expression_kind::view) {
        return Batch::load_unaligned(std::get<Offset>(values) + i);
    } else if constexpr (E::kind == expression_kind::scalar) {
        if constexpr (std::same_as<typename E::value_type, bool>)
            return typename Batch::batch_bool_type(std::get<Offset>(values));
        else
            return Batch(std::get<Offset>(values));
    } else if constexpr (E::kind == expression_kind::unary) {
        using operation = typename E::operation_type;
        return xsimd_operation<operation>::apply(
            read_expression_batch<typename E::child_type, Batch, Offset>(values, i));
    } else if constexpr (E::kind == expression_kind::binary) {
        using operation = typename E::operation_type;
        return xsimd_operation<operation>::apply(
            read_expression_batch<typename E::left_type, Batch, Offset>(values, i),
            read_expression_batch<typename E::right_type, Batch,
                                  Offset + expression_leaf_count<typename E::left_type>>(values,
                                                                                         i));
    } else {
        static_assert(std::same_as<typename E::operation_type, select_operation>);
        return xsimd_operation<select_operation>::apply(
            read_expression_batch<expression_child_type<E, 0>, Batch, Offset>(values, i),
            read_expression_batch<expression_child_type<E, 1>, Batch,
                                  Offset + preceding_expression_slots<E, 1>>(values, i),
            read_expression_batch<expression_child_type<E, 2>, Batch,
                                  Offset + preceding_expression_slots<E, 2>>(values, i));
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
    } else {
        return []<size_t... I>(std::index_sequence<I...>) {
            if constexpr (((expression_row_load<expression_child_type<E, I>> ==
                            row_load_kind::unsupported) ||
                           ...))
                return row_load_kind::unsupported;
            else if constexpr (((expression_row_load<expression_child_type<E, I>> ==
                                 row_load_kind::splat) &&
                                ...))
                return row_load_kind::splat;
            else
                return row_load_kind::unit;
        }(expression_child_indices<E>{});
    }
}();

template <size_t Leaf, tensor_expression E, class Batch, size_t Offset = 0, class Values,
          size_t Count>
WJR_INTRINSIC_INLINE auto xsimd_row_slot(const Values &values,
                                         const std::array<size_t, Count> &offsets) noexcept {
    if constexpr (expression_children<E>::arity != 0) {
        constexpr auto child = expression_slot_child<Leaf, false, 0, E>();
        return xsimd_row_slot<Leaf - preceding_expression_slots<E, child>,
                              expression_child_type<E, child>, Batch,
                              Offset + preceding_expression_slots<E, child>>(values, offsets);
    } else {
        static_assert(Leaf == 0);
        if constexpr (E::kind == expression_kind::scalar) {
            if constexpr (std::same_as<typename E::value_type, bool>)
                return typename Batch::batch_bool_type(std::get<Offset>(values));
            else
                return Batch(std::get<Offset>(values));
        } else if constexpr (expression_row_load<E> == row_load_kind::splat)
            return Batch(std::get<Offset>(values)[offsets[Offset]]);
        else
            return std::get<Offset>(values) + offsets[Offset];
    }
}

template <tensor_expression E, class Batch, size_t Offset = 0, class Values>
WJR_INTRINSIC_INLINE auto read_xsimd_row(const Values &values, size_t i) noexcept {
    if constexpr (E::kind == expression_kind::scalar || E::kind == expression_kind::view) {
        if constexpr (expression_row_load<E> == row_load_kind::splat)
            return std::get<Offset>(values);
        else
            return Batch::load_unaligned(std::get<Offset>(values) + i);
    } else if constexpr (E::kind == expression_kind::unary) {
        return xsimd_operation<typename E::operation_type>::apply(
            read_xsimd_row<typename E::child_type, Batch, Offset>(values, i));
    } else if constexpr (E::kind == expression_kind::binary) {
        return xsimd_operation<typename E::operation_type>::apply(
            read_xsimd_row<typename E::left_type, Batch, Offset>(values, i),
            read_xsimd_row<typename E::right_type, Batch,
                           Offset + expression_leaf_count<typename E::left_type>>(values, i));
    } else {
        static_assert(std::same_as<typename E::operation_type, select_operation>);
        return xsimd_operation<select_operation>::apply(
            read_xsimd_row<expression_child_type<E, 0>, Batch, Offset>(values, i),
            read_xsimd_row<expression_child_type<E, 1>, Batch,
                           Offset + preceding_expression_slots<E, 1>>(values, i),
            read_xsimd_row<expression_child_type<E, 2>, Batch,
                           Offset + preceding_expression_slots<E, 2>>(values, i));
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
    } else if constexpr (E::kind == expression_kind::binary) {
        return E::operation_type::apply(
            read_xsimd_row_scalar<typename E::left_type, Offset>(values, i),
            read_xsimd_row_scalar<typename E::right_type,
                                  Offset + expression_leaf_count<typename E::left_type>>(values,
                                                                                         i));
    } else {
        static_assert(std::same_as<typename E::operation_type, select_operation>);
        return E::operation_type::apply(
            read_xsimd_row_scalar<expression_child_type<E, 0>, Offset>(values, i),
            read_xsimd_row_scalar<expression_child_type<E, 1>,
                                  Offset + preceding_expression_slots<E, 1>>(values, i),
            read_xsimd_row_scalar<expression_child_type<E, 2>,
                                  Offset + preceding_expression_slots<E, 2>>(values, i));
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
// Mixed dtypes and unsupported nodes retain scalar per-node semantics.
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
