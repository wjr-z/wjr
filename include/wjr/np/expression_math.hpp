#ifndef WJR_NP_EXPRESSION_MATH_HPP__
#define WJR_NP_EXPRESSION_MATH_HPP__

#include <cmath>
#include <cstdlib>

#include <wjr/np/expression.hpp>

namespace wjr::np {

namespace detail {
struct sqrt_operation {
    template <class T>
    static constexpr auto
    apply(T value) noexcept(noexcept(std::sqrt(value))) -> decltype(std::sqrt(value)) {
        return std::sqrt(value);
    }
};
struct exp_operation {
    template <class T>
    static constexpr auto
    apply(T value) noexcept(noexcept(std::exp(value))) -> decltype(std::exp(value)) {
        return std::exp(value);
    }
};
struct log_operation {
    template <class T>
    static constexpr auto
    apply(T value) noexcept(noexcept(std::log(value))) -> decltype(std::log(value)) {
        return std::log(value);
    }
};
struct abs_operation {
    template <class T>
    requires(std::signed_integral<T> || std::floating_point<T>)
    static constexpr auto
    apply(T value) noexcept(noexcept(std::abs(value))) -> decltype(std::abs(value)) {
        return std::abs(value);
    }
};

template <class Operation, class E>
concept math_operand = requires {
    typename captured_expression<E>::value_type;
    requires expression_element<typename captured_expression<E>::value_type>;
    Operation::apply(std::declval<typename captured_expression<E>::value_type>());
    requires expression_element<decltype(Operation::apply(
        std::declval<typename captured_expression<E>::value_type>()))>;
};
template <class Operation, class E>
requires math_operand<Operation, E>
[[nodiscard]] constexpr auto make_math_expression(E &&value) noexcept(noexcept(
    unary_expression<Operation, captured_expression<E>>(as_expr(std::forward<E>(value))))) {
    return unary_expression<Operation, captured_expression<E>>(as_expr(std::forward<E>(value)));
}
} // namespace detail

// Each node calls the actual std overload on its child's arithmetic value.
// Domain, range, errno and floating-point behavior follow that implementation.
template <class E>
requires detail::math_operand<detail::sqrt_operation, E>
[[nodiscard]] constexpr auto sqrt(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::sqrt_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::sqrt_operation>(std::forward<E>(value));
}
template <class E>
requires detail::math_operand<detail::exp_operation, E>
[[nodiscard]] constexpr auto exp(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::exp_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::exp_operation>(std::forward<E>(value));
}
template <class E>
requires detail::math_operand<detail::log_operation, E>
[[nodiscard]] constexpr auto log(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::log_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::log_operation>(std::forward<E>(value));
}
template <class E>
requires detail::math_operand<detail::abs_operation, E>
[[nodiscard]] constexpr auto abs(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::abs_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::abs_operation>(std::forward<E>(value));
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_MATH_HPP__
