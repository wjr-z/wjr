#ifndef WJR_NP_EVALUATE_HPP__
#define WJR_NP_EVALUATE_HPP__

#include <wjr/np/expression.hpp>
#include <wjr/np/tensor.hpp>

namespace wjr::np {

namespace detail {

template <class E>
concept owning_expression = tensor_expression<E> && expression_element<typename E::value_type> &&
                            !std::same_as<typename E::value_type, bool>;

template <tensor_expression E>
constexpr auto natural_expression_extent(const E &expression, size_t axis) noexcept ->
    typename E::extents_type::index_type {
    if constexpr (E::kind == expression_kind::view) {
        return expression.view().extent(axis);
    } else if constexpr (E::kind == expression_kind::unary) {
        return natural_expression_extent(expression.child(), axis);
    } else if constexpr (E::kind == expression_kind::binary) {
        if constexpr (E::left_type::is_scalar)
            return natural_expression_extent(expression.right(), axis);
        else
            return natural_expression_extent(expression.left(), axis);
    } else {
        return 0; // A scalar's rank is zero; no axis is queried.
    }
}

template <tensor_expression E, class Integer, size_t Rank>
constexpr bool matches_expression_shape(const E &expression,
                                        const std::array<Integer, Rank> &shape) noexcept {
    if constexpr (E::kind == expression_kind::view) {
        for (size_t axis = 0; axis != Rank; ++axis) {
            if (!wjr::cmp_equal(expression.view().extent(axis), shape[axis]))
                return false;
        }
        return true;
    } else if constexpr (E::kind == expression_kind::unary) {
        return matches_expression_shape(expression.child(), shape);
    } else if constexpr (E::kind == expression_kind::binary) {
        return matches_expression_shape(expression.left(), shape) &&
               matches_expression_shape(expression.right(), shape);
    } else {
        return true;
    }
}

template <class Allocator, class E>
concept expression_allocator = requires {
    typename Allocator::value_type;
} && std::same_as<typename std::allocator_traits<Allocator>::value_type, typename E::value_type>;

template <owning_expression E, class Allocator>
requires expression_allocator<Allocator, E>
using evaluation_tensor =
    tensor<typename E::value_type, typename E::extents_type, typename E::layout_type,
           std::vector<typename E::value_type, Allocator>>;

} // namespace detail

// Materializes an independent owner with the expression's dtype, extents and
// packed layout. Allocation failure follows the allocator's exception policy.
template <detail::owning_expression E, class Allocator>
requires detail::expression_allocator<Allocator, E>
[[nodiscard]] constexpr auto
eval(const E &expression, std::allocator_arg_t,
     const Allocator &allocator) -> expected<detail::evaluation_tensor<E, Allocator>, shape_error> {
    using owner_type = detail::evaluation_tensor<E, Allocator>;
    using index_type = typename owner_type::index_type;
    std::array<index_type, E::rank()> shape{};
    for (size_t axis = 0; axis != E::rank(); ++axis)
        shape[axis] = detail::natural_expression_extent(expression, axis);
    if (!detail::matches_expression_shape(expression, shape))
        return unexpected(shape_error::expression_shape_mismatch);
    auto result = make_tensor<owner_type>(shape, std::allocator_arg, allocator);
    if (result)
        eval_to(result->view(), expression);
    return result;
}

template <detail::owning_expression E>
[[nodiscard]] constexpr auto eval(const E &expression)
    -> expected<detail::evaluation_tensor<E, std::allocator<typename E::value_type>>, shape_error> {
    return eval(expression, std::allocator_arg, std::allocator<typename E::value_type>{});
}

} // namespace wjr::np

#endif // WJR_NP_EVALUATE_HPP__
