#ifndef WJR_BIGINTEGER_EXPRESSION_INTEGRATION_HPP__
#define WJR_BIGINTEGER_EXPRESSION_INTEGRATION_HPP__

#include <wjr/biginteger/biginteger.hpp>
#include <wjr/biginteger/expression/expression.hpp>
#include <wjr/macros/requires.hpp>

namespace wjr {

namespace biginteger_expression_detail {

// Use the __to_expr from expression.hpp

} // namespace biginteger_expression_detail

// Implementation of expression assignment operators for basic_biginteger
template <typename Storage>
template <typename Expr, WJR_REQUIRES_I(biginteger_expression_detail::__is_biginteger_expr_v<Expr>)>
basic_biginteger<Storage>& basic_biginteger<Storage>::operator=(const Expr& expression) {
    // Default use stack allocator
    unique_stack_allocator stack_alloc;
    biginteger_expression_detail::__evaluate_biginteger_expr<Storage>(*this, expression, &stack_alloc);
    
    return *this;
}

template <typename Storage>
template <typename Expr, WJR_REQUIRES_I(biginteger_expression_detail::__is_biginteger_expr_v<Expr>)>
basic_biginteger<Storage>& basic_biginteger<Storage>::operator+=(const Expr& expression) {
    auto result_expr = biginteger_expression_detail::__add(
        biginteger_expression_detail::__to_expr(*this), expression);
    return *this = result_expr;
}

template <typename Storage>
template <typename Expr, WJR_REQUIRES_I(biginteger_expression_detail::__is_biginteger_expr_v<Expr>)>
basic_biginteger<Storage>& basic_biginteger<Storage>::operator-=(const Expr& expression) {
    auto result_expr = biginteger_expression_detail::__sub(
        biginteger_expression_detail::__to_expr(*this), expression);
    return *this = result_expr;
}

template <typename Storage>
template <typename Expr, WJR_REQUIRES_I(biginteger_expression_detail::__is_biginteger_expr_v<Expr>)>
basic_biginteger<Storage>& basic_biginteger<Storage>::operator*=(const Expr& expression) {
    auto result_expr = biginteger_expression_detail::__mul(
        biginteger_expression_detail::__to_expr(*this), expression);
    return *this = result_expr;
}

template <typename Storage>
auto basic_biginteger<Storage>::to_expr() const noexcept {
    return biginteger_expression_detail::__to_expr(*this);
}

// Binary operators with scalars
template <typename Storage, typename T, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator+(const basic_biginteger<Storage>& lhs, T rhs) noexcept {
    return biginteger_expression_detail::__add(
        biginteger_expression_detail::__to_expr(lhs), 
        biginteger_expression_detail::__to_expr(rhs));
}

template <typename Storage, typename T, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator+(T lhs, const basic_biginteger<Storage>& rhs) noexcept {
    return biginteger_expression_detail::__add(
        biginteger_expression_detail::__to_expr(lhs), 
        biginteger_expression_detail::__to_expr(rhs));
}

template <typename Storage, typename T, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator-(const basic_biginteger<Storage>& lhs, T rhs) noexcept {
    return biginteger_expression_detail::__sub(
        biginteger_expression_detail::__to_expr(lhs), 
        biginteger_expression_detail::__to_expr(rhs));
}

template <typename Storage, typename T, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator-(T lhs, const basic_biginteger<Storage>& rhs) noexcept {
    return biginteger_expression_detail::__sub(
        biginteger_expression_detail::__to_expr(lhs), 
        biginteger_expression_detail::__to_expr(rhs));
}

template <typename Storage, typename T, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator*(const basic_biginteger<Storage>& lhs, T rhs) noexcept {
    return biginteger_expression_detail::__mul(
        biginteger_expression_detail::__to_expr(lhs), 
        biginteger_expression_detail::__to_expr(rhs));
}

template <typename Storage, typename T, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator*(T lhs, const basic_biginteger<Storage>& rhs) noexcept {
    return biginteger_expression_detail::__mul(
        biginteger_expression_detail::__to_expr(lhs), 
        biginteger_expression_detail::__to_expr(rhs));
}

// Binary operators between bigintegers
template <typename Storage1, typename Storage2>
auto operator+(const basic_biginteger<Storage1>& lhs, const basic_biginteger<Storage2>& rhs) noexcept {
    return biginteger_expression_detail::__add(
        biginteger_expression_detail::__to_expr(lhs), 
        biginteger_expression_detail::__to_expr(rhs));
}

template <typename Storage1, typename Storage2>
auto operator-(const basic_biginteger<Storage1>& lhs, const basic_biginteger<Storage2>& rhs) noexcept {
    return biginteger_expression_detail::__sub(
        biginteger_expression_detail::__to_expr(lhs), 
        biginteger_expression_detail::__to_expr(rhs));
}

template <typename Storage1, typename Storage2>
auto operator*(const basic_biginteger<Storage1>& lhs, const basic_biginteger<Storage2>& rhs) noexcept {
    return biginteger_expression_detail::__mul(
        biginteger_expression_detail::__to_expr(lhs), 
        biginteger_expression_detail::__to_expr(rhs));
}

// Unary operators
template <typename D>
auto operator-(const biginteger_expression_detail::__biginteger_expr_base<D>& expr) noexcept {
    return biginteger_expression_detail::__negate(expr.derived());
}

template <typename Storage>
auto operator-(const basic_biginteger<Storage>& expr) noexcept {
    return biginteger_expression_detail::__negate(biginteger_expression_detail::__to_expr(expr));
}

// Expression operators for chaining - using base class for cleaner overload resolution
template <typename D1, typename D2>
auto operator+(const biginteger_expression_detail::__biginteger_expr_base<D1>& left,
               const biginteger_expression_detail::__biginteger_expr_base<D2>& right) noexcept {
    return biginteger_expression_detail::__add(left.derived(), right.derived());
}

template <typename D, typename T, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator+(const biginteger_expression_detail::__biginteger_expr_base<D>& left, T right) noexcept {
    return biginteger_expression_detail::__add(left.derived(), biginteger_expression_detail::__to_expr(right));
}

template <typename T, typename D, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator+(T left, const biginteger_expression_detail::__biginteger_expr_base<D>& right) noexcept {
    return biginteger_expression_detail::__add(biginteger_expression_detail::__to_expr(left), right.derived());
}

// biginteger with expression
template <typename D, typename Storage>
auto operator+(const biginteger_expression_detail::__biginteger_expr_base<D>& left,
               const basic_biginteger<Storage>& right) noexcept {
    return biginteger_expression_detail::__add(left.derived(), biginteger_expression_detail::__to_expr(right));
}

template <typename Storage, typename D>
auto operator+(const basic_biginteger<Storage>& left,
               const biginteger_expression_detail::__biginteger_expr_base<D>& right) noexcept {
    return biginteger_expression_detail::__add(biginteger_expression_detail::__to_expr(left), right.derived());
}

template <typename D1, typename D2>
auto operator-(const biginteger_expression_detail::__biginteger_expr_base<D1>& left,
               const biginteger_expression_detail::__biginteger_expr_base<D2>& right) noexcept {
    return biginteger_expression_detail::__sub(left.derived(), right.derived());
}

template <typename D, typename T, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator-(const biginteger_expression_detail::__biginteger_expr_base<D>& left, T right) noexcept {
    return biginteger_expression_detail::__sub(left.derived(), biginteger_expression_detail::__to_expr(right));
}

template <typename T, typename D, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator-(T left, const biginteger_expression_detail::__biginteger_expr_base<D>& right) noexcept {
    return biginteger_expression_detail::__sub(biginteger_expression_detail::__to_expr(left), right.derived());
}

// biginteger with expression
template <typename D, typename Storage>
auto operator-(const biginteger_expression_detail::__biginteger_expr_base<D>& left,
               const basic_biginteger<Storage>& right) noexcept {
    return biginteger_expression_detail::__sub(left.derived(), biginteger_expression_detail::__to_expr(right));
}

template <typename Storage, typename D>
auto operator-(const basic_biginteger<Storage>& left,
               const biginteger_expression_detail::__biginteger_expr_base<D>& right) noexcept {
    return biginteger_expression_detail::__sub(biginteger_expression_detail::__to_expr(left), right.derived());
}

template <typename D1, typename D2>
auto operator*(const biginteger_expression_detail::__biginteger_expr_base<D1>& left,
               const biginteger_expression_detail::__biginteger_expr_base<D2>& right) noexcept {
    return biginteger_expression_detail::__mul(left.derived(), right.derived());
}

template <typename D, typename T, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator*(const biginteger_expression_detail::__biginteger_expr_base<D>& left, T right) noexcept {
    return biginteger_expression_detail::__mul(left.derived(), biginteger_expression_detail::__to_expr(right));
}

template <typename T, typename D, WJR_REQUIRES(biginteger_expression_detail::__is_scalar_integral_v<T>)>
auto operator*(T left, const biginteger_expression_detail::__biginteger_expr_base<D>& right) noexcept {
    return biginteger_expression_detail::__mul(biginteger_expression_detail::__to_expr(left), right.derived());
}

// biginteger with expression
template <typename D, typename Storage>
auto operator*(const biginteger_expression_detail::__biginteger_expr_base<D>& left,
               const basic_biginteger<Storage>& right) noexcept {
    return biginteger_expression_detail::__mul(left.derived(), biginteger_expression_detail::__to_expr(right));
}

template <typename Storage, typename D>
auto operator*(const basic_biginteger<Storage>& left,
               const biginteger_expression_detail::__biginteger_expr_base<D>& right) noexcept {
    return biginteger_expression_detail::__mul(biginteger_expression_detail::__to_expr(left), right.derived());
}

// Square function for expressions and bigintegers
template <typename Expr, WJR_REQUIRES(biginteger_expression_detail::__is_biginteger_expr_v<std::decay_t<Expr>>)>
auto square(Expr&& expr) noexcept {
    return biginteger_expression_detail::__square(std::forward<Expr>(expr));
}

template <typename Storage>
auto square(const basic_biginteger<Storage>& expr) noexcept {
    return biginteger_expression_detail::__square(
        biginteger_expression_detail::__to_expr(expr));
}

} // namespace wjr

#endif // WJR_BIGINTEGER_EXPRESSION_INTEGRATION_HPP__