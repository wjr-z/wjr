#ifndef WJR_BIGINTEGER_EXPRESSION_INTEGRATION_HPP__
#define WJR_BIGINTEGER_EXPRESSION_INTEGRATION_HPP__

#ifndef WJR_BIGINTEGER_BIGINTEGER_HPP__
    #error "Include <wjr/biginteger/biginteger.hpp> instead of including this file directly."
#endif

#include <wjr/biginteger/expression/expression.hpp>
#include <wjr/macros/requires.hpp>

namespace wjr {

namespace biginteger_expression_detail {

// Forward declarations for all evaluate_expr overloads
// This ensures MSVC can find the correct overload during template instantiation

template <typename S, typename T>
void evaluate_expr(basic_biginteger<S> &dst, const biginteger_detail::ref_expr<T> &expr) noexcept;

template <typename S, typename LeftExpr, typename RightExpr>
void evaluate_expr(basic_biginteger<S> &dst,
                   const biginteger_detail::binary_expr<biginteger_detail::add_tag, LeftExpr,
                                                        RightExpr> &expr) noexcept;

template <typename S, typename LeftExpr, typename RightExpr>
void evaluate_expr(basic_biginteger<S> &dst,
                   const biginteger_detail::binary_expr<biginteger_detail::sub_tag, LeftExpr,
                                                        RightExpr> &expr) noexcept;

template <typename S, typename LeftExpr, typename RightExpr>
void evaluate_expr(basic_biginteger<S> &dst,
                   const biginteger_detail::binary_expr<biginteger_detail::mul_tag, LeftExpr,
                                                        RightExpr> &expr) noexcept;

template <typename S, typename Expr>
void evaluate_expr(
    basic_biginteger<S> &dst,
    const biginteger_detail::unary_expr<biginteger_detail::shift_left_tag, Expr> &expr) noexcept;

template <typename S, typename Expr>
void evaluate_expr(
    basic_biginteger<S> &dst,
    const biginteger_detail::negate_unary_expr<biginteger_detail::negate_tag, Expr> &expr) noexcept;

/**
 * @brief Evaluate leaf expression (ref_expr).
 *
 * For leaf nodes, directly assign the wrapped value to destination.
 */
template <typename S, typename T>
void evaluate_expr(basic_biginteger<S> &dst, const biginteger_detail::ref_expr<T> &expr) noexcept {
    dst = expr.value();
}

template <typename S, typename Tag, typename LeftExpr, typename RightExpr, typename BinaryOp>
void evaluate_binary_expr(basic_biginteger<S> &dst,
                          const biginteger_detail::binary_expr<Tag, LeftExpr, RightExpr> &expr,
                          BinaryOp op) noexcept {
    using namespace biginteger_detail;

    constexpr bool left_is_leaf = is_leaf_expr_v<LeftExpr>;
    constexpr bool right_is_leaf = is_leaf_expr_v<RightExpr>;

    if constexpr (left_is_leaf && right_is_leaf) {
        const auto &left_val = expr.left().value();
        const auto &right_val = expr.right().value();
        op(dst, left_val, right_val);
    } else {
        unique_stack_allocator_selector<S> stkal(&dst);
        if constexpr (left_is_leaf && !right_is_leaf) {
            weak_stack_biginteger temp(*stkal);
            evaluate_expr(temp, expr.right());
            const auto &left_val = expr.left().value();
            op(dst, left_val, temp);
        } else if constexpr (!left_is_leaf && right_is_leaf) {
            weak_stack_biginteger temp(*stkal);
            evaluate_expr(temp, expr.left());
            const auto &right_val = expr.right().value();
            op(dst, temp, right_val);
        } else {
            weak_stack_biginteger temp_left(*stkal), temp_right(*stkal);
            evaluate_expr(temp_left, expr.left());
            evaluate_expr(temp_right, expr.right());
            op(dst, temp_left, temp_right);
        }
    }
}

#define WJR_DEFINE_BINARY_EVAL_OP(TAG, FUNC)                                                       \
    template <typename S, typename LeftExpr, typename RightExpr>                                   \
    void evaluate_expr(basic_biginteger<S> &dst,                                                   \
                       const biginteger_detail::binary_expr<biginteger_detail::TAG, LeftExpr,      \
                                                            RightExpr> &expr) noexcept {           \
        evaluate_binary_expr(dst, expr, [](basic_biginteger<S> &d, const auto &l, const auto &r) { \
            FUNC(d, l, r);                                                                         \
        });                                                                                        \
    }

WJR_DEFINE_BINARY_EVAL_OP(add_tag, add)
WJR_DEFINE_BINARY_EVAL_OP(sub_tag, sub)
WJR_DEFINE_BINARY_EVAL_OP(mul_tag, mul)

#undef WJR_DEFINE_BINARY_EVAL_OP

/**
 * @brief Evaluate unary shift left expression.
 *
 * Calls mul_2exp with the shift amount.
 */
template <typename S, typename Expr>
void evaluate_expr(
    basic_biginteger<S> &dst,
    const biginteger_detail::unary_expr<biginteger_detail::shift_left_tag, Expr> &expr) noexcept {
    using namespace biginteger_detail;

    constexpr bool operand_is_leaf = is_leaf_expr_v<Expr>;

    if constexpr (operand_is_leaf) {
        // Direct evaluation: dst = operand << shift
        const auto &operand_val = expr.operand().value();
        mul_2exp(dst, operand_val, expr.param());
    } else {
        unique_stack_allocator_selector<S> stkal(&dst);
        // Evaluate sub-expression first, then shift
        weak_stack_biginteger temp(*stkal);
        evaluate_expr(temp, expr.operand());
        mul_2exp(dst, temp, expr.param());
    }
}

/**
 * @brief Evaluate unary negation expression.
 *
 * Computes the negation of the operand.
 */
template <typename S, typename Expr>
void evaluate_expr(basic_biginteger<S> &dst,
                   const biginteger_detail::negate_unary_expr<biginteger_detail::negate_tag, Expr>
                       &expr) noexcept {
    using namespace biginteger_detail;

    constexpr bool operand_is_leaf = is_leaf_expr_v<Expr>;

    if constexpr (operand_is_leaf) {
        // Direct evaluation: dst = -operand
        const auto &operand_val = expr.operand().value();
        dst = operand_val;
        dst.negate();
    } else {
        // Evaluate sub-expression first, then negate
        evaluate_expr(dst, expr.operand());
        dst.negate();
    }
}

} // namespace biginteger_expression_detail

/**
 * @brief Assignment operator from expression template.
 */
template <typename Storage>
template <typename Expr,
          WJR_REQUIRES_I(biginteger_detail::is_biginteger_expression_v<std::decay_t<Expr>>)>
basic_biginteger<Storage> &basic_biginteger<Storage>::operator=(Expr &&expr) noexcept {
    biginteger_expression_detail::evaluate_expr(*this, expr.derived());
    return *this;
}

/**
 * @brief Constructor from expression template.
 */
template <typename Storage>
template <typename Expr,
          WJR_REQUIRES_I(biginteger_detail::is_biginteger_expression_v<std::decay_t<Expr>>)>
basic_biginteger<Storage>::basic_biginteger(Expr &&expr) {
    biginteger_expression_detail::evaluate_expr(*this, expr.derived());
}

} // namespace wjr

#endif // WJR_BIGINTEGER_EXPRESSION_INTEGRATION_HPP__