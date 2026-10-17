#ifndef WJR_NP_EXPRESSION_HPP__
#define WJR_NP_EXPRESSION_HPP__

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

#include <wjr/assert.hpp>
#include <wjr/compressed_pair.hpp>
#include <wjr/np/view.hpp>

namespace wjr::np {

namespace detail {

enum class expression_kind { view, scalar, unary, binary };

template <class T>
concept expression_element = std::is_arithmetic_v<T> && !std::is_volatile_v<T>;

template <class Layout>
concept expression_layout = std::same_as<Layout, layout_left> || std::same_as<Layout, layout_right>;

template <class View>
inline constexpr bool is_expression_view = false;
template <class T, class Extents, class Layout>
requires(expression_element<T> && expression_layout<Layout>)
inline constexpr bool is_expression_view<tensor_view<T, Extents, Layout>> = true;

template <class E>
inline constexpr bool is_expression = false;

template <class L, class R>
inline constexpr bool compatible_expression_extents = [] {
    if constexpr (L::rank() != R::rank()) {
        return false;
    } else {
        for (size_t axis = 0; axis != L::rank(); ++axis) {
            const auto left = L::static_extent(axis);
            const auto right = R::static_extent(axis);
            if (left != dynamic_extent && right != dynamic_extent && left != right)
                return false;
        }
        return true;
    }
}();

template <class L, class R>
inline constexpr bool compatible_expressions = [] {
    if constexpr (L::is_scalar || R::is_scalar) {
        return true;
    } else {
        return compatible_expression_extents<typename L::extents_type, typename R::extents_type> &&
               (L::rank() <= 1 || std::same_as<typename L::layout_type, typename R::layout_type>);
    }
}();

template <class L, class R>
struct expression_domain {
    using left_extents = typename L::extents_type;
    using right_extents = typename R::extents_type;
    using index_type =
        std::common_type_t<typename left_extents::index_type, typename right_extents::index_type>;
    template <size_t... I>
    static auto merge(std::index_sequence<I...>)
        -> extents<index_type, (left_extents::static_extent(I) == dynamic_extent
                                    ? right_extents::static_extent(I)
                                    : left_extents::static_extent(I))...>;
    using extents_type = decltype(merge(std::make_index_sequence<L::rank()>{}));
    using layout_type = typename L::layout_type;
};

template <class L, class R>
requires L::is_scalar
struct expression_domain<L, R> {
    using extents_type = typename R::extents_type;
    using layout_type = typename R::layout_type;
};

template <class L, class R>
requires(!L::is_scalar && R::is_scalar)
struct expression_domain<L, R> {
    using extents_type = typename L::extents_type;
    using layout_type = typename L::layout_type;
};

struct add_operation {
    template <class L, class R>
    static constexpr auto apply(const L &left, const R &right) noexcept {
        return left + right;
    }
};
struct subtract_operation {
    template <class L, class R>
    static constexpr auto apply(const L &left, const R &right) noexcept {
        return left - right;
    }
};
struct multiply_operation {
    template <class L, class R>
    static constexpr auto apply(const L &left, const R &right) noexcept {
        return left * right;
    }
};
struct divide_operation {
    template <class L, class R>
    static constexpr auto apply(const L &left, const R &right) noexcept {
        return left / right;
    }
};
struct positive_operation {
    template <class T>
    static constexpr auto apply(const T &value) noexcept {
        return +value;
    }
};
struct negative_operation {
    template <class T>
    static constexpr auto apply(const T &value) noexcept {
        return -value;
    }
};

} // namespace detail

template <class E>
concept tensor_expression = detail::is_expression<std::remove_cvref_t<E>>;

// These experimental node types carry values and borrowed descriptors only.
// Their representation and internal access protocol are not a stable ABI.
template <class View>
requires detail::is_expression_view<View>
class view_expression {
public:
    using view_type = View;
    using value_type = typename View::value_type;
    using extents_type = typename View::extents_type;
    using layout_type = typename View::layout_type;
    static constexpr auto kind = detail::expression_kind::view;
    static constexpr bool is_scalar = false;

    constexpr explicit view_expression(View view) noexcept : m_view(view) {}
    static constexpr size_t rank() noexcept { return extents_type::rank(); }
    static constexpr size_t static_extent(size_t axis) noexcept {
        return extents_type::static_extent(axis);
    }
    constexpr const View &view() const noexcept { return m_view; }
    WJR_INTRINSIC_INLINE constexpr const value_type &read(size_t i) const noexcept {
        return m_view.data_handle()[i];
    }

private:
    View m_view;
};

template <detail::expression_element T>
class scalar_expression {
public:
    using value_type = T;
    using extents_type = extents<size_t>;
    using layout_type = layout_right;
    static constexpr auto kind = detail::expression_kind::scalar;
    static constexpr bool is_scalar = true;

    constexpr explicit scalar_expression(T value) noexcept : m_value(value) {}
    static constexpr size_t rank() noexcept { return 0; }
    static constexpr size_t static_extent(size_t axis) noexcept {
        return extents_type::static_extent(axis);
    }
    constexpr const T &value() const noexcept { return m_value; }
    constexpr const T &read(size_t) const noexcept { return m_value; }

private:
    T m_value;
};

template <class Operation, tensor_expression Child>
class unary_expression {
public:
    using operation_type = Operation;
    using child_type = Child;
    using value_type = decltype(Operation::apply(std::declval<typename Child::value_type>()));
    using extents_type = typename Child::extents_type;
    using layout_type = typename Child::layout_type;
    static constexpr auto kind = detail::expression_kind::unary;
    static constexpr bool is_scalar = Child::is_scalar;

    constexpr explicit unary_expression(Child child) noexcept : m_child(std::move(child)) {}
    static constexpr size_t rank() noexcept { return extents_type::rank(); }
    static constexpr size_t static_extent(size_t axis) noexcept {
        return extents_type::static_extent(axis);
    }
    constexpr const Child &child() const noexcept { return m_child; }
    WJR_INTRINSIC_INLINE constexpr value_type read(size_t i) const
        noexcept(noexcept(Operation::apply(m_child.read(i)))) {
        return Operation::apply(m_child.read(i));
    }

private:
    Child m_child;
};

template <class Operation, tensor_expression Left, tensor_expression Right>
requires detail::compatible_expressions<Left, Right>
class binary_expression {
public:
    using operation_type = Operation;
    using left_type = Left;
    using right_type = Right;
    using value_type = decltype(Operation::apply(std::declval<typename Left::value_type>(),
                                                 std::declval<typename Right::value_type>()));
    using extents_type = typename detail::expression_domain<Left, Right>::extents_type;
    using layout_type = typename detail::expression_domain<Left, Right>::layout_type;
    static constexpr auto kind = detail::expression_kind::binary;
    static constexpr bool is_scalar = Left::is_scalar && Right::is_scalar;

    constexpr binary_expression(Left left, Right right) noexcept
        : m_children(std::move(left), std::move(right)) {}
    static constexpr size_t rank() noexcept { return extents_type::rank(); }
    static constexpr size_t static_extent(size_t axis) noexcept {
        return extents_type::static_extent(axis);
    }
    constexpr const Left &left() const noexcept { return m_children.first(); }
    constexpr const Right &right() const noexcept { return m_children.second(); }
    WJR_INTRINSIC_INLINE constexpr value_type read(size_t i) const
        noexcept(noexcept(Operation::apply(left().read(i), right().read(i)))) {
        return Operation::apply(left().read(i), right().read(i));
    }

private:
    compressed_pair<Left, Right> m_children;
};

namespace detail {
template <class V>
requires is_expression_view<V>
inline constexpr bool is_expression<view_expression<V>> = true;
template <expression_element T>
inline constexpr bool is_expression<scalar_expression<T>> = true;
template <class Op, tensor_expression E>
inline constexpr bool is_expression<unary_expression<Op, E>> = true;
template <class Op, tensor_expression L, tensor_expression R>
requires compatible_expressions<L, R>
inline constexpr bool is_expression<binary_expression<Op, L, R>> = true;
} // namespace detail

template <class T, class Extents, class Layout>
requires(detail::expression_element<T> && detail::expression_layout<Layout>)
[[nodiscard]] constexpr auto as_expr(tensor_view<T, Extents, Layout> view) noexcept {
    using readonly_view = tensor_view<const std::remove_const_t<T>, Extents, Layout>;
    return view_expression<readonly_view>(readonly_view(view));
}

template <class T>
requires detail::expression_element<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto as_expr(T &&value) noexcept {
    return scalar_expression<std::remove_cvref_t<T>>(value);
}

template <tensor_expression E>
requires std::constructible_from<std::remove_cvref_t<E>, E>
[[nodiscard]] constexpr auto
as_expr(E &&expression) noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<E>, E>) {
    return std::forward<E>(expression);
}

namespace detail {
template <class L, class R>
concept expression_operands =
    (tensor_expression<L> || tensor_expression<R>) && requires(L &&l, R &&r) {
        as_expr(std::forward<L>(l));
        as_expr(std::forward<R>(r));
        requires compatible_expressions<decltype(as_expr(std::forward<L>(l))),
                                        decltype(as_expr(std::forward<R>(r)))>;
    };

template <class Op, class L, class R>
[[nodiscard]] constexpr auto make_binary_expression(L &&left, R &&right) noexcept {
    using left_type = decltype(as_expr(std::forward<L>(left)));
    using right_type = decltype(as_expr(std::forward<R>(right)));
    return binary_expression<Op, left_type, right_type>(as_expr(std::forward<L>(left)),
                                                        as_expr(std::forward<R>(right)));
}

// The AST remains the type-level operation plan. Only pointers and captured
// scalar values are copied into the per-call tuple; no shape state enters the loop.
template <tensor_expression E>
inline constexpr size_t expression_leaf_count = [] {
    if constexpr (E::kind == expression_kind::unary)
        return expression_leaf_count<typename E::child_type>;
    else if constexpr (E::kind == expression_kind::binary)
        return expression_leaf_count<typename E::left_type> +
               expression_leaf_count<typename E::right_type>;
    else
        return size_t{1};
}();

template <size_t I, tensor_expression E>
WJR_INTRINSIC_INLINE constexpr auto expression_leaf_value(const E &expression) noexcept {
    if constexpr (E::kind == expression_kind::view) {
        static_assert(I == 0);
        return expression.view().data_handle();
    } else if constexpr (E::kind == expression_kind::scalar) {
        static_assert(I == 0);
        return expression.value();
    } else if constexpr (E::kind == expression_kind::unary) {
        return expression_leaf_value<I>(expression.child());
    } else if constexpr (I < expression_leaf_count<typename E::left_type>) {
        return expression_leaf_value<I>(expression.left());
    } else {
        return expression_leaf_value<I - expression_leaf_count<typename E::left_type>>(
            expression.right());
    }
}

template <tensor_expression E, size_t... I>
WJR_INTRINSIC_INLINE constexpr auto bind_expression_values(const E &expression,
                                                           std::index_sequence<I...>) noexcept {
    return std::tuple{expression_leaf_value<I>(expression)...};
}

template <tensor_expression E, size_t Offset = 0, class Values>
WJR_INTRINSIC_INLINE constexpr decltype(auto)
read_bound_expression(const Values &values,
                      size_t i) noexcept(noexcept(std::declval<const E &>().read(i))) {
    if constexpr (E::kind == expression_kind::view) {
        return (std::get<Offset>(values)[i]);
    } else if constexpr (E::kind == expression_kind::scalar) {
        return (std::get<Offset>(values));
    } else if constexpr (E::kind == expression_kind::unary) {
        using operation = typename E::operation_type;
        return operation::apply(read_bound_expression<typename E::child_type, Offset>(values, i));
    } else {
        using operation = typename E::operation_type;
        return operation::apply(
            read_bound_expression<typename E::left_type, Offset>(values, i),
            read_bound_expression<typename E::right_type,
                                  Offset + expression_leaf_count<typename E::left_type>>(values,
                                                                                         i));
    }
}

template <class Dst, tensor_expression E>
constexpr void assert_expression_shape(Dst dst, const E &expression) noexcept {
    if constexpr (E::kind == expression_kind::view) {
        for (size_t axis = 0; axis != Dst::rank(); ++axis) {
            WJR_ASSERT_L2(static_cast<size_t>(dst.extent(axis)) ==
                          static_cast<size_t>(expression.view().extent(axis)));
        }
    } else if constexpr (E::kind == expression_kind::unary) {
        assert_expression_shape(dst, expression.child());
    } else if constexpr (E::kind == expression_kind::binary) {
        assert_expression_shape(dst, expression.left());
        assert_expression_shape(dst, expression.right());
    }
}
} // namespace detail

template <class L, class R>
requires detail::expression_operands<L, R>
[[nodiscard]] constexpr auto operator+(L &&left, R &&right) noexcept {
    return detail::make_binary_expression<detail::add_operation>(std::forward<L>(left),
                                                                 std::forward<R>(right));
}
template <class L, class R>
requires detail::expression_operands<L, R>
[[nodiscard]] constexpr auto operator-(L &&left, R &&right) noexcept {
    return detail::make_binary_expression<detail::subtract_operation>(std::forward<L>(left),
                                                                      std::forward<R>(right));
}
template <class L, class R>
requires detail::expression_operands<L, R>
[[nodiscard]] constexpr auto operator*(L &&left, R &&right) noexcept {
    return detail::make_binary_expression<detail::multiply_operation>(std::forward<L>(left),
                                                                      std::forward<R>(right));
}
template <class L, class R>
requires detail::expression_operands<L, R>
[[nodiscard]] constexpr auto operator/(L &&left, R &&right) noexcept {
    return detail::make_binary_expression<detail::divide_operation>(std::forward<L>(left),
                                                                    std::forward<R>(right));
}
template <tensor_expression E>
requires std::constructible_from<std::remove_cvref_t<E>, E>
[[nodiscard]] constexpr auto operator+(E &&expression) noexcept {
    return unary_expression<detail::positive_operation, std::remove_cvref_t<E>>(
        std::forward<E>(expression));
}
template <tensor_expression E>
requires std::constructible_from<std::remove_cvref_t<E>, E>
[[nodiscard]] constexpr auto operator-(E &&expression) noexcept {
    return unary_expression<detail::negative_operation, std::remove_cvref_t<E>>(
        std::forward<E>(expression));
}

// Preconditions: valid packed views with equal shapes and common traversal order.
// Every read leaf is disjoint from dst or refers to the same object at each index.
// No hidden temporary, allocation, or ownership is introduced.
template <class T, class Extents, class Layout, tensor_expression E>
requires(detail::expression_element<T> && !std::is_const_v<T> &&
         detail::expression_layout<Layout> &&
         (E::is_scalar ||
          (detail::compatible_expression_extents<Extents, typename E::extents_type> &&
           (Extents::rank() <= 1 || std::same_as<Layout, typename E::layout_type>))) &&
         requires(T &out, const E &expression) { out = expression.read(size_t{}); })
WJR_INTRINSIC_INLINE constexpr void
eval_to(tensor_view<T, Extents, Layout> dst, const E &expression) noexcept(
    noexcept(dst.data_handle()[size_t{}] = expression.read(size_t{}))) {
    detail::assert_expression_shape(dst, expression);
    const auto count = static_cast<size_t>(dst.size());
    if (count == 0)
        return;
    const auto values = detail::bind_expression_values(
        expression, std::make_index_sequence<detail::expression_leaf_count<E>>{});
    for (size_t i = 0; i != count; ++i)
        dst.data_handle()[i] = detail::read_bound_expression<E>(values, i);
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_HPP__
