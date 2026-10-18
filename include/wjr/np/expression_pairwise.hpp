#ifndef WJR_NP_EXPRESSION_PAIRWISE_HPP__
#define WJR_NP_EXPRESSION_PAIRWISE_HPP__

#include <wjr/np/detail/expression_domain.hpp>
#include <wjr/np/pairwise.hpp>

namespace wjr::np {

namespace detail {

template <tensor_expression E, class Domain = typename E::extents_type>
inline constexpr bool pairwise_expression_layout = [] {
    if constexpr (E::kind == expression_kind::view) {
        using layout = typename E::view_type::layout_type;
        if constexpr (E::rank() != Domain::rank() ||
                      !(std::same_as<layout, layout_right> ||
                        (E::rank() <= 1 && std::same_as<layout, layout_left>))) {
            return false;
        } else {
            for (size_t axis = 0; axis != E::rank(); ++axis) {
                const auto source = E::static_extent(axis);
                const auto target = Domain::static_extent(axis);
                if (source != dynamic_extent && target != dynamic_extent && source != target)
                    return false;
            }
            return true;
        }
    } else {
        return []<size_t... I>(std::index_sequence<I...>) {
            return (pairwise_expression_layout<expression_child_type<E, I>, Domain> && ...);
        }(expression_child_indices<E>{});
    }
}();

template <tensor_expression E, class Values>
struct pairwise_expression_cursor {
    const Values *values;
    size_t offset;

    WJR_INTRINSIC_INLINE constexpr auto operator[](size_t i) const noexcept {
        return detail::read_bound_expression<E>(*values, offset + i);
    }
    constexpr auto operator+(size_t count) const noexcept {
        return pairwise_expression_cursor{values, offset + count};
    }
};

} // namespace detail

// Narrow fused terminal: pure nothrow operations over equally shaped packed
// leaves. Scalar operands are allowed; array broadcasting is a precondition violation.
template <class Accumulator = void, tensor_expression E>
requires(
    detail::interleavable_sum_expression<E> && noexcept(std::declval<const E &>().read(0)) &&
    detail::pairwise_expression_layout<E> &&
    detail::sum_accumulator_type<detail::sum_accumulator<Accumulator, typename E::value_type>> &&
    std::floating_point<detail::sum_accumulator<Accumulator, typename E::value_type>>)
[[nodiscard]] constexpr auto pairwise_sum(const E &expression) noexcept {
    using accumulator_type = detail::sum_accumulator<Accumulator, typename E::value_type>;
    const auto domain = detail::bind_expression_sum_domain(expression);
    WJR_ASSERT_L2(detail::is_linear_expression(domain, expression));
    if (domain.empty())
        return accumulator_type{0};
    const auto values = detail::bind_expression_values(
        expression, std::make_index_sequence<detail::expression_leaf_count<E>>{});
    return detail::pairwise_sum_kernel<accumulator_type>(
        detail::pairwise_expression_cursor<E, decltype(values)>{&values, 0}, domain.linear_size());
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_PAIRWISE_HPP__
