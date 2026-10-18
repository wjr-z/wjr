#ifndef WJR_NP_EXPRESSION_MATH_HPP__
#define WJR_NP_EXPRESSION_MATH_HPP__

#include <cmath>
#include <cstdlib>

#include <wjr/np/expression.hpp>
#include <wjr/np/expression_cast.hpp>

namespace wjr::np {

namespace detail {
struct square_operation {
    template <class T>
    static constexpr auto apply(T value) noexcept -> decltype(value * value) {
        return value * value;
    }
};

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
struct expm1_operation {
    template <class T>
    static constexpr auto
    apply(T value) noexcept(noexcept(std::expm1(value))) -> decltype(std::expm1(value)) {
        return std::expm1(value);
    }
};
struct log1p_operation {
    template <class T>
    static constexpr auto
    apply(T value) noexcept(noexcept(std::log1p(value))) -> decltype(std::log1p(value)) {
        return std::log1p(value);
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
struct isfinite_operation {
    template <class T>
    static constexpr auto
    apply(T value) noexcept(noexcept(std::isfinite(value))) -> decltype(std::isfinite(value)) {
        return std::isfinite(value);
    }
};
struct isnan_operation {
    template <class T>
    static constexpr auto
    apply(T value) noexcept(noexcept(std::isnan(value))) -> decltype(std::isnan(value)) {
        return std::isnan(value);
    }
};
struct isinf_operation {
    template <class T>
    static constexpr auto
    apply(T value) noexcept(noexcept(std::isinf(value))) -> decltype(std::isinf(value)) {
        return std::isinf(value);
    }
};

template <bool IsMinimum>
struct extremum_operation {
    template <class L, class R>
    using result_type = std::common_type_t<L, R>;

    template <class L, class R>
    static constexpr auto apply(L left, R right) noexcept -> result_type<L, R> {
        using value_type = result_type<L, R>;
        const value_type lhs = static_cast<value_type>(left);
        const value_type rhs = static_cast<value_type>(right);
        if constexpr (std::floating_point<value_type>) {
            // Preserve a left NaN and ordered ties, including the left zero sign.
            // Evaluate both predicates so compilers can combine the selection masks.
            const bool keep_left = (lhs != lhs) | (IsMinimum ? rhs >= lhs : lhs >= rhs);
            return keep_left ? lhs : rhs;
        }
        if constexpr (IsMinimum)
            return lhs < rhs ? lhs : rhs;
        else
            return rhs < lhs ? lhs : rhs;
    }
};

struct minimum_operation : extremum_operation<true> {};
struct maximum_operation : extremum_operation<false> {};

template <class Operation, class L, class R>
concept extremum_operands = requires {
    typename captured_expression<L>::value_type;
    typename captured_expression<R>::value_type;
    requires compatible_expressions<captured_expression<L>, captured_expression<R>>;
    typename Operation::template result_type<typename captured_expression<L>::value_type,
                                             typename captured_expression<R>::value_type>;
    requires expression_element<typename Operation::template result_type<
        typename captured_expression<L>::value_type, typename captured_expression<R>::value_type>>;
    Operation::apply(std::declval<typename captured_expression<L>::value_type>(),
                     std::declval<typename captured_expression<R>::value_type>());
};

template <class Operation, class L, class R>
requires extremum_operands<Operation, L, R>
[[nodiscard]] constexpr auto make_extremum_expression(L &&left, R &&right) noexcept(
    noexcept(make_binary_expression<Operation>(std::forward<L>(left), std::forward<R>(right)))) {
    return make_binary_expression<Operation>(std::forward<L>(left), std::forward<R>(right));
}

template <class E, class L, class H>
using clip_value_type = std::common_type_t<typename captured_expression<E>::value_type,
                                           typename captured_expression<L>::value_type,
                                           typename captured_expression<H>::value_type>;

template <class To, class E>
[[nodiscard]] constexpr auto
make_clip_operand(E &&value) noexcept(std::same_as<To, typename captured_expression<E>::value_type>
                                          ? noexcept(as_expr(std::forward<E>(value)))
                                          : noexcept(cast<To>(std::forward<E>(value)))) {
    if constexpr (std::same_as<To, typename captured_expression<E>::value_type>)
        return as_expr(std::forward<E>(value));
    else
        return cast<To>(std::forward<E>(value));
}

template <class L, class M, class H>
concept clip_operands = requires {
    typename captured_expression<L>::value_type;
    typename captured_expression<M>::value_type;
    typename captured_expression<H>::value_type;
    requires compatible_expressions<captured_expression<L>, captured_expression<M>>;
    requires compatible_expressions<captured_expression<L>, captured_expression<H>>;
    requires compatible_expressions<captured_expression<M>, captured_expression<H>>;
    typename clip_value_type<L, M, H>;
    requires expression_element<clip_value_type<L, M, H>>;
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

// Capture and evaluate one child once, then multiply its value by itself.
// Ordinary arithmetic promotions and overflow preconditions apply.
template <class E>
requires detail::math_operand<detail::square_operation, E>
[[nodiscard]] constexpr auto square(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::square_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::square_operation>(std::forward<E>(value));
}

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
requires detail::math_operand<detail::expm1_operation, E>
[[nodiscard]] constexpr auto expm1(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::expm1_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::expm1_operation>(std::forward<E>(value));
}

template <class E>
requires detail::math_operand<detail::log1p_operation, E>
[[nodiscard]] constexpr auto log1p(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::log1p_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::log1p_operation>(std::forward<E>(value));
}
template <class E>
requires detail::math_operand<detail::abs_operation, E>
[[nodiscard]] constexpr auto abs(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::abs_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::abs_operation>(std::forward<E>(value));
}
template <class E>
requires detail::math_operand<detail::isfinite_operation, E>
[[nodiscard]] constexpr auto isfinite(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::isfinite_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::isfinite_operation>(std::forward<E>(value));
}
template <class E>
requires detail::math_operand<detail::isnan_operation, E>
[[nodiscard]] constexpr auto isnan(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::isnan_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::isnan_operation>(std::forward<E>(value));
}
template <class E>
requires detail::math_operand<detail::isinf_operation, E>
[[nodiscard]] constexpr auto isinf(E &&value) noexcept(
    noexcept(detail::make_math_expression<detail::isinf_operation>(std::forward<E>(value)))) {
    return detail::make_math_expression<detail::isinf_operation>(std::forward<E>(value));
}

// NaNs propagate, and equal values keep the left operand (including its zero sign).
// The result type follows std::common_type for the two expression value types.
template <class L, class R>
requires detail::extremum_operands<detail::minimum_operation, L, R>
[[nodiscard]] constexpr auto minimum(L &&left, R &&right) noexcept(
    noexcept(detail::make_extremum_expression<detail::minimum_operation>(std::forward<L>(left),
                                                                         std::forward<R>(right)))) {
    return detail::make_extremum_expression<detail::minimum_operation>(std::forward<L>(left),
                                                                       std::forward<R>(right));
}

template <class L, class R>
requires detail::extremum_operands<detail::maximum_operation, L, R>
[[nodiscard]] constexpr auto maximum(L &&left, R &&right) noexcept(
    noexcept(detail::make_extremum_expression<detail::maximum_operation>(std::forward<L>(left),
                                                                         std::forward<R>(right)))) {
    return detail::make_extremum_expression<detail::maximum_operation>(std::forward<L>(left),
                                                                       std::forward<R>(right));
}

// Promote all three operands together before composing the binary operations.
// This avoids a different intermediate promotion at either clipping boundary.
template <class E, class L, class H>
requires detail::clip_operands<E, L, H>
[[nodiscard]] constexpr auto clip(E &&value, L &&lower, H &&upper) noexcept(
    noexcept(detail::make_extremum_expression<detail::minimum_operation>(
        detail::make_extremum_expression<detail::maximum_operation>(
            detail::make_clip_operand<detail::clip_value_type<E, L, H>>(std::forward<E>(value)),
            detail::make_clip_operand<detail::clip_value_type<E, L, H>>(std::forward<L>(lower))),
        detail::make_clip_operand<detail::clip_value_type<E, L, H>>(std::forward<H>(upper))))) {
    using value_type = detail::clip_value_type<E, L, H>;
    auto converted_value = detail::make_clip_operand<value_type>(std::forward<E>(value));
    auto converted_lower = detail::make_clip_operand<value_type>(std::forward<L>(lower));
    auto converted_upper = detail::make_clip_operand<value_type>(std::forward<H>(upper));
    return minimum(maximum(std::move(converted_value), std::move(converted_lower)),
                   std::move(converted_upper));
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_MATH_HPP__
