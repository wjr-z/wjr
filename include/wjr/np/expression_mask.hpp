#ifndef WJR_NP_EXPRESSION_MASK_HPP__
#define WJR_NP_EXPRESSION_MASK_HPP__

#include <tuple>

#include <wjr/np/expression.hpp>

namespace wjr::np {

namespace detail {

template <class E>
concept arithmetic_operand = requires {
    typename captured_expression<E>::value_type;
    requires expression_element<typename captured_expression<E>::value_type>;
};
template <class E>
concept boolean_operand =
    arithmetic_operand<E> && std::same_as<typename captured_expression<E>::value_type, bool>;
template <class L, class R>
concept comparison_operands =
    expression_operands<L, R> && arithmetic_operand<L> && arithmetic_operand<R>;
template <class L, class R>
concept logical_operands = boolean_operand<L> && boolean_operand<R> &&
                           compatible_expressions<captured_expression<L>, captured_expression<R>>;

struct less_operation {
    template <class L, class R>
    static constexpr bool apply(L left, R right) noexcept {
        return left < right;
    }
};
struct less_equal_operation {
    template <class L, class R>
    static constexpr bool apply(L left, R right) noexcept {
        return left <= right;
    }
};
struct greater_operation {
    template <class L, class R>
    static constexpr bool apply(L left, R right) noexcept {
        return left > right;
    }
};
struct greater_equal_operation {
    template <class L, class R>
    static constexpr bool apply(L left, R right) noexcept {
        return left >= right;
    }
};
struct equal_operation {
    template <class L, class R>
    static constexpr bool apply(L left, R right) noexcept {
        return left == right;
    }
};
struct not_equal_operation {
    template <class L, class R>
    static constexpr bool apply(L left, R right) noexcept {
        return left != right;
    }
};
struct logical_not_operation {
    static constexpr bool apply(bool value) noexcept { return !value; }
};
struct logical_and_operation {
    static constexpr bool apply(bool left, bool right) noexcept { return left && right; }
};
struct logical_or_operation {
    static constexpr bool apply(bool left, bool right) noexcept { return left || right; }
};
struct select_operation {
    template <class Yes, class No>
    static constexpr auto apply(bool mask, Yes yes, No no) noexcept {
        return mask ? yes : no;
    }
};

template <class Mask, class Yes, class No>
concept selectable_expressions =
    tensor_expression<Mask> && tensor_expression<Yes> && tensor_expression<No> &&
    std::same_as<typename Mask::value_type, bool> && expression_element<typename Yes::value_type> &&
    expression_element<typename No::value_type> &&
    requires { typename expression_pack_domain<Mask, Yes, No>::extents_type; };

} // namespace detail

// Both branches are evaluated before scalar selection. This node never guards
// an invalid operation or an invalid access in either branch.
template <tensor_expression Mask, tensor_expression Yes, tensor_expression No>
requires detail::selectable_expressions<Mask, Yes, No>
class select_expression {
    using children_type = std::tuple<Mask, Yes, No>;
    using domain = detail::expression_pack_domain<Mask, Yes, No>;

public:
    using operation_type = detail::select_operation;
    using value_type = decltype(operation_type::apply(
        bool{}, std::declval<typename Yes::value_type>(), std::declval<typename No::value_type>()));
    using extents_type = typename domain::extents_type;
    using layout_type = typename domain::layout_type;
    static constexpr auto kind = detail::expression_kind::static_node;
    static constexpr bool is_scalar = Mask::is_scalar && Yes::is_scalar && No::is_scalar;

    template <class M, class Y, class N>
    requires std::constructible_from<children_type, M, Y, N>
    constexpr select_expression(M &&mask, Y &&yes, N &&no) noexcept(
        std::is_nothrow_constructible_v<children_type, M, Y, N>)
        : m_children(std::forward<M>(mask), std::forward<Y>(yes), std::forward<N>(no)) {}
    static constexpr size_t rank() noexcept { return extents_type::rank(); }
    static constexpr size_t static_extent(size_t axis) noexcept {
        return extents_type::static_extent(axis);
    }
    template <size_t I>
    constexpr const auto &child() const noexcept {
        return std::get<I>(m_children);
    }
    WJR_INTRINSIC_INLINE constexpr value_type read(size_t i) const
        noexcept(noexcept(operation_type::apply(child<0>().read(i), child<1>().read(i),
                                                child<2>().read(i)))) {
        return operation_type::apply(child<0>().read(i), child<1>().read(i), child<2>().read(i));
    }

private:
    children_type m_children;
};

namespace detail {
template <class M, class Y, class N>
inline constexpr bool is_expression<select_expression<M, Y, N>> = true;
template <class M, class Y, class N>
struct expression_children<select_expression<M, Y, N>> {
    static constexpr size_t arity = 3;
    template <size_t I>
    static constexpr const auto &get(const select_expression<M, Y, N> &expression) noexcept {
        return expression.template child<I>();
    }
};
template <class M, class Y, class N>
using captured_select =
    select_expression<captured_expression<M>, captured_expression<Y>, captured_expression<N>>;
} // namespace detail

template <class L, class R>
requires detail::comparison_operands<L, R>
[[nodiscard]] constexpr auto operator<(L &&left, R &&right) noexcept(
    noexcept(detail::make_binary_expression<detail::less_operation>(std::forward<L>(left),
                                                                    std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::less_operation>(std::forward<L>(left),
                                                                  std::forward<R>(right));
}
template <class L, class R>
requires detail::comparison_operands<L, R>
[[nodiscard]] constexpr auto operator<=(L &&left, R &&right) noexcept(noexcept(
    detail::make_binary_expression<detail::less_equal_operation>(std::forward<L>(left),
                                                                 std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::less_equal_operation>(std::forward<L>(left),
                                                                        std::forward<R>(right));
}
template <class L, class R>
requires detail::comparison_operands<L, R>
[[nodiscard]] constexpr auto operator>(L &&left, R &&right) noexcept(
    noexcept(detail::make_binary_expression<detail::greater_operation>(std::forward<L>(left),
                                                                       std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::greater_operation>(std::forward<L>(left),
                                                                     std::forward<R>(right));
}
template <class L, class R>
requires detail::comparison_operands<L, R>
[[nodiscard]] constexpr auto operator>=(L &&left, R &&right) noexcept(noexcept(
    detail::make_binary_expression<detail::greater_equal_operation>(std::forward<L>(left),
                                                                    std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::greater_equal_operation>(std::forward<L>(left),
                                                                           std::forward<R>(right));
}
template <class L, class R>
requires detail::comparison_operands<L, R>
[[nodiscard]] constexpr auto operator==(L &&left, R &&right) noexcept(
    noexcept(detail::make_binary_expression<detail::equal_operation>(std::forward<L>(left),
                                                                     std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::equal_operation>(std::forward<L>(left),
                                                                   std::forward<R>(right));
}
template <class L, class R>
requires detail::comparison_operands<L, R>
[[nodiscard]] constexpr auto operator!=(L &&left, R &&right) noexcept(
    noexcept(detail::make_binary_expression<detail::not_equal_operation>(std::forward<L>(left),
                                                                         std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::not_equal_operation>(std::forward<L>(left),
                                                                       std::forward<R>(right));
}

template <class E>
requires detail::boolean_operand<E>
[[nodiscard]] constexpr auto logical_not(E &&value) noexcept(
    noexcept(unary_expression<detail::logical_not_operation, detail::captured_expression<E>>(
        as_expr(std::forward<E>(value))))) {
    return unary_expression<detail::logical_not_operation, detail::captured_expression<E>>(
        as_expr(std::forward<E>(value)));
}
template <class L, class R>
requires detail::logical_operands<L, R>
[[nodiscard]] constexpr auto logical_and(L &&left, R &&right) noexcept(noexcept(
    detail::make_binary_expression<detail::logical_and_operation>(std::forward<L>(left),
                                                                  std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::logical_and_operation>(std::forward<L>(left),
                                                                         std::forward<R>(right));
}
template <class L, class R>
requires detail::logical_operands<L, R>
[[nodiscard]] constexpr auto logical_or(L &&left, R &&right) noexcept(noexcept(
    detail::make_binary_expression<detail::logical_or_operation>(std::forward<L>(left),
                                                                 std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::logical_or_operation>(std::forward<L>(left),
                                                                        std::forward<R>(right));
}

template <class M, class Y, class N>
requires requires { typename detail::captured_select<M, Y, N>; }
[[nodiscard]] constexpr auto select(M &&mask, Y &&yes, N &&no) noexcept(noexcept(
    detail::captured_select<M, Y, N>(as_expr(std::forward<M>(mask)), as_expr(std::forward<Y>(yes)),
                                     as_expr(std::forward<N>(no))))) {
    return detail::captured_select<M, Y, N>(as_expr(std::forward<M>(mask)),
                                            as_expr(std::forward<Y>(yes)),
                                            as_expr(std::forward<N>(no)));
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_MASK_HPP__
