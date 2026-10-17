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
    std::same_as<typename View::value_type, T>;

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
