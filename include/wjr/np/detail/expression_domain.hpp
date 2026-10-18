#ifndef WJR_NP_DETAIL_EXPRESSION_DOMAIN_HPP__
#define WJR_NP_DETAIL_EXPRESSION_DOMAIN_HPP__

#include <wjr/np/expression.hpp>

namespace wjr::np::detail {

struct minimum_operation;
struct maximum_operation;

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

// Nothrow alone does not permit reordering math/unknown operations: e.g. std
// math may write errno. Pure map callbacks already require order independence.
template <tensor_expression Expression>
inline constexpr bool interleavable_sum_expression = [] {
    using E = std::remove_cvref_t<Expression>;
    if constexpr (E::kind == expression_kind::view || E::kind == expression_kind::scalar) {
        return true;
    } else {
        constexpr bool operation = [] {
            if constexpr (E::kind == expression_kind::mapped)
                return true; // User callable follows the pure map contract.
            else if constexpr (E::kind == expression_kind::unary ||
                               E::kind == expression_kind::binary) {
                using op = typename E::operation_type;
                return std::same_as<op, add_operation> || std::same_as<op, subtract_operation> ||
                       std::same_as<op, multiply_operation> || std::same_as<op, divide_operation> ||
                       std::same_as<op, minimum_operation> || std::same_as<op, maximum_operation> ||
                       std::same_as<op, positive_operation> || std::same_as<op, negative_operation>;
            } else
                return false;
        }();
        return operation && []<size_t... I>(std::index_sequence<I...>) {
            return (interleavable_sum_expression<expression_child_type<E, I>> && ...);
        }(expression_child_indices<E>{});
    }
}();

template <size_t Rank, size_t Count, class Function, size_t... I>
WJR_INTRINSIC_INLINE constexpr void walk_expression_sum(
    const std::array<size_t, Rank> &shape,
    const std::array<std::array<size_t, Rank>, Count> &strides, Function &function,
    std::index_sequence<I...>
        operands) noexcept(std::is_nothrow_invocable_v<Function &, decltype(I)...>) {
    walk_kernel<0, false>(shape, strides, function, operands, decltype(I){}...);
}

} // namespace wjr::np::detail

#endif // WJR_NP_DETAIL_EXPRESSION_DOMAIN_HPP__
