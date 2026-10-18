#ifndef WJR_NP_EXPRESSION_HPP__
#define WJR_NP_EXPRESSION_HPP__

#include <array>
#include <concepts>
#include <functional>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include <wjr/assert.hpp>
#include <wjr/compressed_pair.hpp>
#include <wjr/np/detail/elementwise.hpp>
#include <wjr/np/view.hpp>

namespace wjr::np {

namespace detail {

enum class expression_kind { view, scalar, unary, binary, mapped, static_node };

template <class T>
concept expression_element = std::is_arithmetic_v<T> && !std::is_volatile_v<T>;

template <class Layout>
concept expression_layout = std::same_as<Layout, layout_left> || std::same_as<Layout, layout_right>;

template <class Layout>
concept expression_view_layout = kernel_layout<Layout>;

template <class View>
inline constexpr bool is_expression_view = false;
template <class T, class Extents, class Layout>
requires(expression_element<T> && expression_view_layout<Layout>)
inline constexpr bool is_expression_view<tensor_view<T, Extents, Layout>> = true;

template <class E>
inline constexpr bool is_expression = false;

template <class Extents>
constexpr size_t projected_static_extent(size_t rank, size_t axis) noexcept {
    return axis < rank - Extents::rank() ? size_t{1}
                                         : Extents::static_extent(axis - (rank - Extents::rank()));
}

constexpr size_t merged_static_extent(size_t left, size_t right) noexcept {
    if (left == dynamic_extent)
        return right == 1 ? dynamic_extent : right;
    if (right == dynamic_extent)
        return left == 1 ? dynamic_extent : left;
    return left == 1 ? right : left;
}

template <class L, class R>
inline constexpr bool compatible_expression_extents = [] {
    constexpr auto rank = L::rank() > R::rank() ? L::rank() : R::rank();
    for (size_t axis = 0; axis != rank; ++axis) {
        const auto left = projected_static_extent<L>(rank, axis);
        const auto right = projected_static_extent<R>(rank, axis);
        if (left != dynamic_extent && right != dynamic_extent && left != 1 && right != 1 &&
            left != right)
            return false;
    }
    return true;
}();

template <class L, class R>
inline constexpr bool compatible_expressions =
    compatible_expression_extents<typename L::extents_type, typename R::extents_type>;

template <class Source, class Target>
inline constexpr bool expression_extents_fit = [] {
    if constexpr (Source::rank() > Target::rank())
        return false;
    else {
        for (size_t axis = 0; axis != Target::rank(); ++axis) {
            const auto source = projected_static_extent<Source>(Target::rank(), axis);
            const auto target = Target::static_extent(axis);
            if (source != dynamic_extent && target != dynamic_extent && source != 1 &&
                source != target)
                return false;
        }
        return true;
    }
}();

template <class L, class R>
struct expression_domain {
    using left_extents = typename L::extents_type;
    using right_extents = typename R::extents_type;
    using index_type =
        std::common_type_t<typename left_extents::index_type, typename right_extents::index_type>;
    static constexpr auto rank = L::rank() > R::rank() ? L::rank() : R::rank();
    template <size_t... I>
    static auto merge(std::index_sequence<I...>)
        -> extents<index_type,
                   merged_static_extent(projected_static_extent<left_extents>(rank, I),
                                        projected_static_extent<right_extents>(rank, I))...>;
    using extents_type = decltype(merge(std::make_index_sequence<rank>{}));
    using layout_type =
        std::conditional_t<std::same_as<typename L::layout_type, typename R::layout_type>,
                           typename L::layout_type, layout_right>;
};

template <class L, class R>
requires L::is_scalar
struct expression_domain<L, R> {
    using extents_type = typename R::extents_type;
    using layout_type = typename R::layout_type;
};

template <class L, class R>
struct combined_expression_domain : expression_domain<L, R> {
    static constexpr bool is_scalar = L::is_scalar && R::is_scalar;
    static constexpr size_t rank() noexcept {
        return expression_domain<L, R>::extents_type::rank();
    }
};

template <class... Children>
struct expression_pack_domain {};
template <class Child>
struct expression_pack_domain<Child> : Child {};
template <class Left, class Right, class... Rest>
requires(compatible_expressions<Left, Right> &&
         requires {
             typename expression_pack_domain<combined_expression_domain<Left, Right>,
                                             Rest...>::extents_type;
         })
struct expression_pack_domain<Left, Right, Rest...>
    : expression_pack_domain<combined_expression_domain<Left, Right>, Rest...> {};

template <class F, class... Children>
concept expression_function =
    std::invocable<const F &, typename Children::value_type...> &&
    !std::is_reference_v<std::invoke_result_t<const F &, typename Children::value_type...>> &&
    expression_element<
        std::remove_cv_t<std::invoke_result_t<const F &, typename Children::value_type...>>>;

template <class L, class R>
requires(!L::is_scalar && R::is_scalar)
struct expression_domain<L, R> {
    using extents_type = typename L::extents_type;
    using layout_type = typename L::layout_type;
};

struct add_operation {
    template <class L, class R>
    static constexpr auto apply(const L &left, const R &right) noexcept(noexcept(left + right)) {
        return left + right;
    }
};
struct subtract_operation {
    template <class L, class R>
    static constexpr auto apply(const L &left, const R &right) noexcept(noexcept(left - right)) {
        return left - right;
    }
};
struct multiply_operation {
    template <class L, class R>
    static constexpr auto apply(const L &left, const R &right) noexcept(noexcept(left * right)) {
        return left * right;
    }
};
struct divide_operation {
    template <class L, class R>
    static constexpr auto apply(const L &left, const R &right) noexcept(noexcept(left / right)) {
        return left / right;
    }
};
struct positive_operation {
    template <class T>
    static constexpr auto apply(const T &value) noexcept(noexcept(+value)) {
        return +value;
    }
};
struct negative_operation {
    template <class T>
    static constexpr auto apply(const T &value) noexcept(noexcept(-value)) {
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
    using layout_type = std::conditional_t<detail::left_layout<typename View::layout_type>,
                                           layout_left, layout_right>;
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

    template <class E>
    requires std::constructible_from<Child, E>
    constexpr explicit unary_expression(E &&child) noexcept(
        std::is_nothrow_constructible_v<Child, E>)
        : m_child(std::forward<E>(child)) {}
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

    template <class L, class R>
    requires(std::constructible_from<Left, L> && std::constructible_from<Right, R>)
    constexpr binary_expression(L &&left,
                                R &&right) noexcept(std::is_nothrow_constructible_v<Left, L> &&
                                                    std::is_nothrow_constructible_v<Right, R>)
        : m_children(std::forward<L>(left), std::forward<R>(right)) {}
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

// The function is a real value-owned object, including when its type is empty.
// Evaluation borrows it through const F& and never copies or moves it.
template <class F, tensor_expression... Children>
requires(sizeof...(Children) != 0 && detail::expression_function<F, Children...> &&
         requires { typename detail::expression_pack_domain<Children...>::extents_type; })
class mapped_expression {
public:
    using function_type = F;
    using children_type = std::tuple<Children...>;
    using value_type =
        std::remove_cv_t<std::invoke_result_t<const F &, typename Children::value_type...>>;
    using extents_type = typename detail::expression_pack_domain<Children...>::extents_type;
    using layout_type = typename detail::expression_pack_domain<Children...>::layout_type;
    static constexpr auto kind = detail::expression_kind::mapped;
    static constexpr bool is_scalar = (Children::is_scalar && ...);

    template <class Function, class... E>
    requires(std::constructible_from<F, Function> && std::constructible_from<children_type, E...>)
    constexpr explicit mapped_expression(Function &&function, E &&...children) noexcept(
        std::is_nothrow_constructible_v<F, Function> &&
        std::is_nothrow_constructible_v<children_type, E...>)
        : m_state(std::piecewise_construct,
                  wjr::tuple<Function &&>(std::forward<Function>(function)),
                  wjr::tuple<E &&...>(std::forward<E>(children)...)) {}
    static constexpr size_t rank() noexcept { return extents_type::rank(); }
    static constexpr size_t static_extent(size_t axis) noexcept {
        return extents_type::static_extent(axis);
    }
    constexpr const F &function() const noexcept { return m_state.first(); }
    template <size_t I>
    constexpr const auto &child() const noexcept {
        return std::get<I>(m_state.second());
    }
    WJR_INTRINSIC_INLINE constexpr value_type read(size_t i) const
        noexcept(std::is_nothrow_invocable_v<const F &, typename Children::value_type...> &&
                 (noexcept(std::declval<const Children &>().read(i)) && ...)) {
        return [&]<size_t... I>(std::index_sequence<I...>) {
            return std::invoke(function(),
                               static_cast<typename Children::value_type>(child<I>().read(i))...);
        }(std::index_sequence_for<Children...>{});
    }

private:
    compressed_pair<F, children_type> m_state;
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
template <class F, tensor_expression... Children>
requires requires { typename mapped_expression<F, Children...>; }
inline constexpr bool is_expression<mapped_expression<F, Children...>> = true;

// Private static structure, shared by metadata and binding. No runtime visitor.
template <class E>
struct expression_children {
    static constexpr size_t arity = 0;
    template <size_t I>
    static void get(const E &) = delete;
};
template <class Op, tensor_expression Child>
struct expression_children<unary_expression<Op, Child>> {
    static constexpr size_t arity = 1;
    template <size_t I>
    static constexpr const Child &get(const unary_expression<Op, Child> &expression) noexcept {
        static_assert(I == 0);
        return expression.child();
    }
};
template <class Op, tensor_expression Left, tensor_expression Right>
struct expression_children<binary_expression<Op, Left, Right>> {
    static constexpr size_t arity = 2;
    template <size_t I>
    static constexpr const auto &
    get(const binary_expression<Op, Left, Right> &expression) noexcept {
        static_assert(I < 2);
        if constexpr (I == 0)
            return expression.left();
        else
            return expression.right();
    }
};
template <class F, tensor_expression... Children>
struct expression_children<mapped_expression<F, Children...>> {
    static constexpr size_t arity = sizeof...(Children);
    template <size_t I>
    static constexpr const auto &get(const mapped_expression<F, Children...> &expression) noexcept {
        return expression.template child<I>();
    }
};
template <class E, size_t I>
using expression_child_type = std::remove_cvref_t<decltype(expression_children<E>::template get<I>(
    std::declval<const E &>()))>;
template <class E>
using expression_child_indices = std::make_index_sequence<expression_children<E>::arity>;

template <class E, class Function>
constexpr bool all_expression_children(const E &expression, Function function) noexcept {
    return [&]<size_t... I>(std::index_sequence<I...>) {
        return (function(expression_children<E>::template get<I>(expression)) && ...);
    }(expression_child_indices<E>{});
}
} // namespace detail

template <class T, class Extents, class Layout>
requires(detail::expression_element<T> && detail::expression_view_layout<Layout>)
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
template <class T>
using captured_expression = decltype(as_expr(std::declval<T>()));

template <class F, class... Operands>
concept mapped_operands = sizeof...(Operands) != 0 && requires {
    typename mapped_expression<std::decay_t<F>, captured_expression<Operands>...>;
} && std::constructible_from<std::decay_t<F>, F>;

template <class L, class R>
concept expression_operands =
    (tensor_expression<L> || tensor_expression<R>) && requires(L &&l, R &&r) {
        as_expr(std::forward<L>(l));
        as_expr(std::forward<R>(r));
        requires compatible_expressions<decltype(as_expr(std::forward<L>(l))),
                                        decltype(as_expr(std::forward<R>(r)))>;
    };

template <class Op, class L, class R>
[[nodiscard]] constexpr auto make_binary_expression(L &&left, R &&right) noexcept(
    noexcept(binary_expression<Op, captured_expression<L>, captured_expression<R>>(
        as_expr(std::forward<L>(left)), as_expr(std::forward<R>(right))))) {
    using left_type = decltype(as_expr(std::forward<L>(left)));
    using right_type = decltype(as_expr(std::forward<R>(right)));
    return binary_expression<Op, left_type, right_type>(as_expr(std::forward<L>(left)),
                                                        as_expr(std::forward<R>(right)));
}

// The AST remains the type-level operation plan. Only pointers and captured
// scalar values are copied into the per-call tuple; no shape state enters the loop.
template <tensor_expression E>
inline constexpr size_t expression_leaf_count = [] {
    if constexpr (expression_children<E>::arity == 0)
        return size_t{1};
    else
        return []<size_t... I>(std::index_sequence<I...>) {
            return (size_t{} + ... + expression_leaf_count<expression_child_type<E, I>>);
        }(expression_child_indices<E>{});
}();

template <tensor_expression E>
inline constexpr size_t expression_function_count = []<size_t... I>(std::index_sequence<I...>) {
    return (size_t{E::kind == expression_kind::mapped} + ... +
            expression_function_count<expression_child_type<E, I>>);
}(expression_child_indices<E>{});

template <class E, size_t I, bool Functions = false>
inline constexpr size_t preceding_expression_slots = []<size_t... J>(std::index_sequence<J...>) {
    if constexpr (Functions)
        return (size_t{} + ... + expression_function_count<expression_child_type<E, J>>);
    else
        return (size_t{} + ... + expression_leaf_count<expression_child_type<E, J>>);
}(std::make_index_sequence<I>{});

template <size_t Slot, bool Functions, size_t Child = 0, class E>
constexpr size_t expression_slot_child() noexcept {
    using child_type = expression_child_type<E, Child>;
    constexpr auto count =
        Functions ? expression_function_count<child_type> : expression_leaf_count<child_type>;
    if constexpr (Slot < count)
        return Child;
    else
        return expression_slot_child<Slot - count, Functions, Child + 1, E>();
}

template <size_t I, tensor_expression E>
WJR_INTRINSIC_INLINE constexpr auto expression_leaf_value(const E &expression) noexcept {
    if constexpr (E::kind == expression_kind::view) {
        static_assert(I == 0);
        return expression.view().data_handle();
    } else if constexpr (E::kind == expression_kind::scalar) {
        static_assert(I == 0);
        return expression.value();
    } else {
        constexpr auto child = expression_slot_child<I, false, 0, E>();
        return expression_leaf_value<I - preceding_expression_slots<E, child>>(
            expression_children<E>::template get<child>(expression));
    }
}

template <size_t I, tensor_expression E>
WJR_INTRINSIC_INLINE constexpr auto expression_function_pointer(const E &expression) noexcept {
    if constexpr (E::kind == expression_kind::mapped && I == 0)
        return std::addressof(expression.function());
    else {
        constexpr auto slot = I - size_t{E::kind == expression_kind::mapped};
        constexpr auto child = expression_slot_child<slot, true, 0, E>();
        return expression_function_pointer<slot - preceding_expression_slots<E, child, true>>(
            expression_children<E>::template get<child>(expression));
    }
}

template <class Values, class Functions>
struct expression_binding {
    Values values;
    Functions functions;
};
template <class Values>
WJR_INTRINSIC_INLINE constexpr const Values &expression_values(const Values &values) noexcept {
    return values;
}
template <class Values, class Functions>
WJR_INTRINSIC_INLINE constexpr const Values &
expression_values(const expression_binding<Values, Functions> &binding) noexcept {
    return binding.values;
}

template <tensor_expression E, size_t... I>
WJR_INTRINSIC_INLINE constexpr auto bind_expression_values(const E &expression,
                                                           std::index_sequence<I...>) noexcept {
    if constexpr (expression_function_count<E> == 0)
        return std::tuple{expression_leaf_value<I>(expression)...};
    else
        return [&]<size_t... F>(std::index_sequence<F...>) {
            return expression_binding{std::tuple{expression_leaf_value<I>(expression)...},
                                      std::tuple{expression_function_pointer<F>(expression)...}};
        }(std::make_index_sequence<expression_function_count<E>>{});
}

// Keep unary/binary reads direct: generic invocation inhibits MSVC vectorization.
// Metadata and arbitrary-arity nodes still share the static child protocol.
template <tensor_expression E, size_t Offset = 0, size_t Function = 0, class Values>
WJR_INTRINSIC_INLINE constexpr decltype(auto)
read_bound_expression(const Values &binding,
                      size_t i) noexcept(noexcept(std::declval<const E &>().read(i))) {
    const auto &values = expression_values(binding);
    if constexpr (E::kind == expression_kind::view) {
        return (std::get<Offset>(values)[i]);
    } else if constexpr (E::kind == expression_kind::scalar) {
        return (std::get<Offset>(values));
    } else if constexpr (E::kind == expression_kind::unary) {
        return E::operation_type::apply(
            read_bound_expression<typename E::child_type, Offset, Function>(binding, i));
    } else if constexpr (E::kind == expression_kind::binary) {
        return E::operation_type::apply(
            read_bound_expression<typename E::left_type, Offset, Function>(binding, i),
            read_bound_expression<typename E::right_type,
                                  Offset + expression_leaf_count<typename E::left_type>,
                                  Function + expression_function_count<typename E::left_type>>(
                binding, i));
    } else {
        return [&]<size_t... I>(std::index_sequence<I...>) WJR_FORCEINLINE_LAMBDA {
            if constexpr (E::kind == expression_kind::mapped)
                return std::invoke(
                    *std::get<Function>(binding.functions),
                    static_cast<typename expression_child_type<E, I>::value_type>(
                        read_bound_expression<
                            expression_child_type<E, I>, Offset + preceding_expression_slots<E, I>,
                            Function + 1 + preceding_expression_slots<E, I, true>>(binding, i))...);
            else
                return E::operation_type::apply(
                    read_bound_expression<expression_child_type<E, I>,
                                          Offset + preceding_expression_slots<E, I>,
                                          Function + preceding_expression_slots<E, I, true>>(
                        binding, i)...);
        }(expression_child_indices<E>{});
    }
}

template <tensor_expression E, size_t Offset = 0, size_t Function = 0, class Values, size_t Count>
WJR_INTRINSIC_INLINE constexpr decltype(auto)
read_bound_expression(const Values &binding, const std::array<size_t, Count> &offsets) noexcept(
    noexcept(std::declval<const E &>().read(size_t{}))) {
    const auto &values = expression_values(binding);
    if constexpr (E::kind == expression_kind::view) {
        return (std::get<Offset>(values)[offsets[Offset]]);
    } else if constexpr (E::kind == expression_kind::scalar) {
        return (std::get<Offset>(values));
    } else if constexpr (E::kind == expression_kind::unary) {
        return E::operation_type::apply(
            read_bound_expression<typename E::child_type, Offset, Function>(binding, offsets));
    } else if constexpr (E::kind == expression_kind::binary) {
        return E::operation_type::apply(
            read_bound_expression<typename E::left_type, Offset, Function>(binding, offsets),
            read_bound_expression<typename E::right_type,
                                  Offset + expression_leaf_count<typename E::left_type>,
                                  Function + expression_function_count<typename E::left_type>>(
                binding, offsets));
    } else {
        return [&]<size_t... I>(std::index_sequence<I...>) WJR_FORCEINLINE_LAMBDA {
            if constexpr (E::kind == expression_kind::mapped)
                return std::invoke(
                    *std::get<Function>(binding.functions),
                    static_cast<typename expression_child_type<E, I>::value_type>(
                        read_bound_expression<
                            expression_child_type<E, I>, Offset + preceding_expression_slots<E, I>,
                            Function + 1 + preceding_expression_slots<E, I, true>>(binding,
                                                                                   offsets))...);
            else
                return E::operation_type::apply(
                    read_bound_expression<expression_child_type<E, I>,
                                          Offset + preceding_expression_slots<E, I>,
                                          Function + preceding_expression_slots<E, I, true>>(
                        binding, offsets)...);
        }(expression_child_indices<E>{});
    }
}

template <tensor_expression E, class Index, size_t Rank>
constexpr bool accumulate_expression_shape(const E &expression,
                                           std::array<Index, Rank> &shape) noexcept {
    if constexpr (E::kind == expression_kind::view && E::rank() != 0) {
        constexpr auto first_axis = Rank - E::rank();
        for (size_t axis = 0; axis != E::rank(); ++axis) {
            const auto extent = static_cast<Index>(expression.view().extent(axis));
            auto &resolved = shape[first_axis + axis];
            if (resolved == 1)
                resolved = extent;
            else if (extent != 1 && extent != resolved)
                return false;
        }
        return true;
    } else {
        return all_expression_children(expression, [&](const auto &child) noexcept {
            return accumulate_expression_shape(child, shape);
        });
    }
}

// No storage is accessed. Recoverable owning evaluation uses the same algebra;
// direct eval_to retains its trusted-input shape precondition.
template <tensor_expression E>
constexpr bool
try_expression_shape(const E &expression,
                     std::array<typename E::extents_type::index_type, E::rank()> &shape) noexcept {
    shape.fill(1);
    return accumulate_expression_shape(expression, shape);
}

template <class Dst, tensor_expression E>
WJR_INTRINSIC_INLINE constexpr void assert_expression_shape(Dst dst, const E &expression) noexcept {
    if constexpr (E::kind == expression_kind::view) {
        constexpr auto first_axis = Dst::rank() - E::rank();
        for (size_t axis = 0; axis != E::rank(); ++axis) {
            WJR_ASSERT_L2(expression.view().extent(axis) == 1 ||
                          static_cast<size_t>(dst.extent(first_axis + axis)) ==
                              static_cast<size_t>(expression.view().extent(axis)));
        }
    } else {
        all_expression_children(expression, [&](const auto &child) noexcept {
            assert_expression_shape(dst, child);
            return true;
        });
    }
}

template <class Dst, tensor_expression E>
constexpr bool is_linear_expression(Dst dst, const E &expression) noexcept {
    if constexpr (!expression_layout<typename Dst::layout_type>) {
        return false;
    } else if constexpr (E::kind == expression_kind::view) {
        using view = typename E::view_type;
        if constexpr (!expression_layout<typename view::layout_type> || Dst::rank() != E::rank() ||
                      (Dst::rank() > 1 &&
                       !std::same_as<typename Dst::layout_type, typename view::layout_type>)) {
            return false;
        } else {
            for (size_t axis = 0; axis != E::rank(); ++axis) {
                if (static_cast<size_t>(dst.extent(axis)) !=
                    static_cast<size_t>(expression.view().extent(axis)))
                    return false;
            }
            return true;
        }
    } else {
        return all_expression_children(expression, [&](const auto &child) noexcept {
            return is_linear_expression(dst, child);
        });
    }
}

// The caller value-initializes strides and has a valid nonempty execution domain.
// Offset permits a caller-owned output slot without creating a dummy input leaf.
template <size_t Offset = 0, tensor_expression E, size_t Rank, size_t Count>
constexpr void
bind_expression_strides(const E &expression,
                        std::array<std::array<size_t, Rank>, Count> &strides) noexcept {
    if constexpr (E::kind == expression_kind::view && E::rank() != 0) {
        constexpr auto first_axis = Rank - E::rank();
        for (size_t axis = 0; axis != E::rank(); ++axis) {
            strides[Offset][first_axis + axis] =
                expression.view().extent(axis) == 1
                    ? size_t{0}
                    : static_cast<size_t>(expression.view().stride(axis));
        }
    } else {
        [&]<size_t... I>(std::index_sequence<I...>) {
            (bind_expression_strides<Offset + preceding_expression_slots<E, I>>(
                 expression_children<E>::template get<I>(expression), strides),
             ...);
        }(expression_child_indices<E>{});
    }
}

// Snapshot the row cursor by value so output stores cannot alias its metadata.
template <tensor_expression E, class T, class Values, size_t Count, size_t... I>
WJR_INTRINSIC_INLINE constexpr void eval_expression_row(
    T *output, size_t count, size_t output_step, Values values, std::array<size_t, Count> offsets,
    std::array<size_t, Count> steps,
    std::index_sequence<I...>) noexcept(noexcept(output[size_t{}] =
                                                     std::declval<const E &>().read(size_t{}))) {
    for (size_t i = 0; i != count; ++i) {
        output[i * output_step] = read_bound_expression<E>(values, offsets);
        ((offsets[I] += steps[I]), ...);
    }
}

// Shape/alias preconditions are established and output is nonempty. Both the
// scalar API and optional backends reuse this fallback without repeating checks.
template <class RowExecutor = void, class Dst, tensor_expression E, class Values>
WJR_INTRINSIC_INLINE constexpr void
eval_projected_expression(Dst dst, const E &expression, const Values &values) noexcept(
    noexcept(dst.data_handle()[size_t{}] = expression.read(size_t{}))) {
    constexpr auto rank = Dst::rank();
    constexpr auto inputs = expression_leaf_count<E>;
    std::array<size_t, rank> shape{};
    std::array<std::array<size_t, rank>, 1 + inputs> strides{};
    if constexpr (rank != 0) {
        for (size_t axis = 0; axis != rank; ++axis) {
            shape[axis] = static_cast<size_t>(dst.extent(axis));
            strides[0][axis] = static_cast<size_t>(dst.stride(axis));
        }
    }
    bind_expression_strides<1>(expression, strides);
    if constexpr (rank == 0) {
        dst.data_handle()[0] = read_bound_expression<E>(values, std::array<size_t, inputs>{});
    } else {
        constexpr auto inner_axis = left_layout<typename Dst::layout_type> ? size_t{0} : rank - 1;
        const auto inner_count = shape[inner_axis];
        const auto output_step = strides[0][inner_axis];
        std::array<size_t, inputs> steps{};
        for (size_t input = 0; input != inputs; ++input)
            steps[input] = strides[1 + input][inner_axis];
        std::array<size_t, rank - 1> outer_shape{};
        std::array<std::array<size_t, rank - 1>, 1 + inputs> outer_strides{};
        for (size_t axis = 0; axis != rank - 1; ++axis) {
            const auto source_axis = axis + (axis >= inner_axis);
            outer_shape[axis] = shape[source_axis];
            for (size_t operand = 0; operand != 1 + inputs; ++operand)
                outer_strides[operand][axis] = strides[operand][source_axis];
        }
        auto point = [&] {
            if constexpr (std::is_void_v<RowExecutor>) {
                return
                    [output = dst.data_handle(), values, steps, inner_count, output_step](
                        size_t output_offset,
                        auto... input_offsets) noexcept(noexcept(dst.data_handle()[size_t{}] =
                                                                     expression.read(size_t{}))) {
                        eval_expression_row<E>(
                            output + output_offset, inner_count, output_step, values,
                            std::array<size_t, expression_leaf_count<E>>{input_offsets...}, steps,
                            std::make_index_sequence<expression_leaf_count<E>>{});
                    };
            } else {
                return RowExecutor::template bind<E>(dst.data_handle(), values, inner_count);
            }
        }();
        [&]<size_t... I>(std::index_sequence<I...> operands) {
            walk_kernel<0, left_layout<typename Dst::layout_type>>(
                outer_shape, outer_strides, point, operands, decltype(I){}...);
        }(std::make_index_sequence<1 + inputs>{});
    }
}
} // namespace detail

// Pure pointwise invocation: captured memory remains stable and disjoint from
// output during evaluation. Explicit data operands retain exact-inplace support.
template <class F, class... Operands>
requires detail::mapped_operands<F, Operands...>
[[nodiscard]] constexpr auto map(F &&function, Operands &&...operands) noexcept(
    noexcept(mapped_expression<std::decay_t<F>, detail::captured_expression<Operands>...>(
        std::forward<F>(function), as_expr(std::forward<Operands>(operands))...))) {
    return mapped_expression<std::decay_t<F>, detail::captured_expression<Operands>...>(
        std::forward<F>(function), as_expr(std::forward<Operands>(operands))...);
}

template <class L, class R>
requires detail::expression_operands<L, R>
[[nodiscard]] constexpr auto operator+(L &&left, R &&right) noexcept(
    noexcept(detail::make_binary_expression<detail::add_operation>(std::forward<L>(left),
                                                                   std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::add_operation>(std::forward<L>(left),
                                                                 std::forward<R>(right));
}
template <class L, class R>
requires detail::expression_operands<L, R>
[[nodiscard]] constexpr auto operator-(L &&left, R &&right) noexcept(
    noexcept(detail::make_binary_expression<detail::subtract_operation>(std::forward<L>(left),
                                                                        std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::subtract_operation>(std::forward<L>(left),
                                                                      std::forward<R>(right));
}
template <class L, class R>
requires detail::expression_operands<L, R>
[[nodiscard]] constexpr auto operator*(L &&left, R &&right) noexcept(
    noexcept(detail::make_binary_expression<detail::multiply_operation>(std::forward<L>(left),
                                                                        std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::multiply_operation>(std::forward<L>(left),
                                                                      std::forward<R>(right));
}
template <class L, class R>
requires detail::expression_operands<L, R>
[[nodiscard]] constexpr auto operator/(L &&left, R &&right) noexcept(
    noexcept(detail::make_binary_expression<detail::divide_operation>(std::forward<L>(left),
                                                                      std::forward<R>(right)))) {
    return detail::make_binary_expression<detail::divide_operation>(std::forward<L>(left),
                                                                    std::forward<R>(right));
}
template <tensor_expression E>
requires std::constructible_from<std::remove_cvref_t<E>, E>
[[nodiscard]] constexpr auto
operator+(E &&expression) noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<E>, E>) {
    return unary_expression<detail::positive_operation, std::remove_cvref_t<E>>(
        std::forward<E>(expression));
}
template <tensor_expression E>
requires std::constructible_from<std::remove_cvref_t<E>, E>
[[nodiscard]] constexpr auto
operator-(E &&expression) noexcept(std::is_nothrow_constructible_v<std::remove_cvref_t<E>, E>) {
    return unary_expression<detail::negative_operation, std::remove_cvref_t<E>>(
        std::forward<E>(expression));
}

// Preconditions: valid views and compatible right-aligned assignment broadcast.
// Every read leaf is disjoint from dst or refers to the same object at each index.
// No hidden temporary, allocation, or ownership is introduced.
template <class T, class Extents, class Layout, tensor_expression E>
requires(detail::expression_element<T> && !std::is_const_v<T> &&
         detail::expression_view_layout<Layout> &&
         detail::expression_extents_fit<typename E::extents_type, Extents> &&
         requires(T &out, const E &expression) { out = expression.read(size_t{}); })
WJR_INTRINSIC_INLINE constexpr void
eval_to(tensor_view<T, Extents, Layout> dst, const E &expression) noexcept(
    noexcept(dst.data_handle()[size_t{}] = expression.read(size_t{}))) {
    detail::assert_expression_shape(dst, expression);
    const auto count = static_cast<size_t>(dst.size());
    if (count == 0)
        return;
    if (detail::is_linear_expression(dst, expression)) {
        const auto values = detail::bind_expression_values(
            expression, std::make_index_sequence<detail::expression_leaf_count<E>>{});
        for (size_t i = 0; i != count; ++i)
            dst.data_handle()[i] = detail::read_bound_expression<E>(values, i);
        return;
    }
    const auto values = detail::bind_expression_values(
        expression, std::make_index_sequence<detail::expression_leaf_count<E>>{});
    detail::eval_projected_expression(dst, expression, values);
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_HPP__
