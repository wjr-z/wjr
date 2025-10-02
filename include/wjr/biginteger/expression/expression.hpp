/**
 * @file expression.hpp
 * @brief Biginteger expression system with lazy evaluation
 */

#ifndef WJR_BIGINTEGER_EXPRESSION_EXPRESSION_HPP__
#define WJR_BIGINTEGER_EXPRESSION_EXPRESSION_HPP__

#include <type_traits>

#include <wjr/macros/requires.hpp>
#include <wjr/memory/stack_allocator.hpp>
#include <wjr/type_traits.hpp>

namespace wjr {

// Forward declaration
template <typename Storage>
class basic_biginteger;

struct biginteger_view;

template <typename Derived>
struct biginteger_expression {
    // Tag to identify expression types
    using biginteger_expression_tag = void;

    constexpr Derived &derived() noexcept { return static_cast<Derived &>(*this); }
    constexpr const Derived &derived() const noexcept {
        return static_cast<const Derived &>(*this);
    }

protected:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(biginteger_expression);
};

namespace biginteger_detail {

// ============================================================================
// Type traits for expression system
// ============================================================================

/**
 * @brief Check if T is an expression node type.
 *
 * An expression node type inherits from biginteger_expression<Derived>.
 * This trait returns true for ref_expr<T>, binary_expr<...>, etc.
 * This trait returns false for biginteger, int, biginteger_view, etc.
 *
 * Note: This is NOT about implicit conversion - it checks if T itself IS an expression.
 */
template <typename T, typename = void>
struct is_biginteger_expression : std::false_type {};

template <typename T>
struct is_biginteger_expression<T, std::void_t<typename T::biginteger_expression_tag>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_biginteger_expression_v =
    is_biginteger_expression<remove_cvref_t<T>>::value;

/**
 * @brief Check if T can be wrapped in a ref_expr<T>.
 *
 * This trait returns true for types that can be used as operands by wrapping them
 * in ref_expr: int, biginteger, biginteger_view, etc.
 *
 * Note: This does NOT mean T can be implicitly converted to an expression type.
 * It means we can construct ref_expr<expr_value_category_t<T>>(value).
 */
template <typename T, typename = void>
struct is_convertible_to_expr : std::false_type {};

template <typename T>
struct is_convertible_to_expr<T, std::enable_if_t<is_nonbool_integral_v<std::decay_t<T>>>>
    : std::true_type {};

template <typename Storage>
struct is_convertible_to_expr<basic_biginteger<Storage>> : std::true_type {};

template <>
struct is_convertible_to_expr<biginteger_view> : std::true_type {};

template <typename T>
inline constexpr bool is_convertible_to_expr_v = is_convertible_to_expr<std::decay_t<T>>::value;

/**
 * @brief Check if T is a valid operand for expression operators.
 *
 * Returns true if T is either:
 * 1. Already an expression node (is_biginteger_expression_v<T>), OR
 * 2. Can be wrapped into ref_expr (is_convertible_to_expr_v<T>)
 */
template <typename T>
struct is_expr_operand
    : std::bool_constant<is_biginteger_expression_v<T> || is_convertible_to_expr_v<T>> {};

template <typename T>
inline constexpr bool is_expr_operand_v = is_expr_operand<T>::value;

template <typename T>
class ref_expr;

/**
 * @brief Determine the storage type for ref_expr based on value category.
 *
 * Rules:
 * - Integral types: stored by value
 * - biginteger_view: stored by value (cheap to copy)
 * - biginteger lvalue: stored by const reference (avoid copy)
 * - biginteger rvalue: stored by value (move semantics)
 */
template <typename T, typename = void>
struct expr_value_category {
    using type = std::decay_t<T>;
};

template <typename T>
struct expr_value_category<T, std::enable_if_t<is_nonbool_integral_v<std::decay_t<T>>>> {
    using type = std::decay_t<T>;
};

template <>
struct expr_value_category<biginteger_view> {
    using type = biginteger_view;
};

template <typename Storage>
struct expr_value_category<basic_biginteger<Storage> &> {
    using type = const basic_biginteger<Storage> &;
};

template <typename Storage>
struct expr_value_category<const basic_biginteger<Storage> &> {
    using type = const basic_biginteger<Storage> &;
};

template <typename Storage>
struct expr_value_category<basic_biginteger<Storage> &&> {
    using type = basic_biginteger<Storage>;
};

template <typename T>
using expr_value_category_t = typename expr_value_category<T>::type;

/**
 * @brief Convert operand to expression node type.
 *
 * This trait handles two cases:
 * 1. If T is already an expression node, forward it as-is (no wrapping)
 * 2. If T can be wrapped in ref_expr, wrap it with appropriate value category
 *
 * Examples:
 *   to_expr<int>              -> ref_expr<int>
 *   to_expr<biginteger&>      -> ref_expr<const biginteger&>
 *   to_expr<biginteger&&>     -> ref_expr<biginteger>
 *   to_expr<ref_expr<int>>    -> ref_expr<int> (no additional wrapping)
 *   to_expr<binary_expr<...>> -> binary_expr<...> (forwarded as-is)
 */
template <typename T, typename = void>
struct to_expr;

template <typename T>
struct to_expr<T, std::enable_if_t<is_biginteger_expression_v<T>>> {
    using type = T;

    static constexpr type convert(T &&value) noexcept { return std::forward<T>(value); }
};

template <typename T>
struct to_expr<T, std::enable_if_t<!is_biginteger_expression_v<T> && is_convertible_to_expr_v<T>>> {
    using type = ref_expr<expr_value_category_t<T>>;

    static constexpr type convert(T &&value) noexcept { return type(std::forward<T>(value)); }
};

template <typename T>
using to_expr_t = typename to_expr<T>::type;

template <typename T>
constexpr auto make_expr(T &&value) noexcept -> to_expr_t<T> {
    return to_expr<T>::convert(std::forward<T>(value));
}

/**
 * @brief Leaf expression node that wraps a value or reference.
 *
 * ref_expr stores the operand according to expr_value_category rules.
 * It serves as the leaf node in expression trees.
 */
template <typename T>
class ref_expr : public biginteger_expression<ref_expr<T>> {
public:
    static constexpr bool is_leaf = true;
    using value_type = T;

    template <typename U, WJR_REQUIRES(std::is_constructible_v<T, U &&>)>
    explicit constexpr ref_expr(U &&value) noexcept : m_value(std::forward<U>(value)) {}

    constexpr const T &value() const noexcept { return m_value; }

    template <typename U = T>
    constexpr std::enable_if_t<!std::is_reference_v<U>, T &> value() noexcept {
        return m_value;
    }

private:
    T m_value;
};

/**
 * @brief Operation tags for binary expressions.
 */
struct add_tag {};
struct sub_tag {};
struct mul_tag {};

/**
 * @brief Check if an expression type is a leaf node.
 */
template <typename T>
struct is_leaf_expr : std::false_type {};

template <typename T>
struct is_leaf_expr<ref_expr<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_leaf_expr_v = is_leaf_expr<std::decay_t<T>>::value;

/**
 * @brief Binary expression node for operations like +, -, *.
 *
 * Stores two sub-expressions (which can be leaf or binary nodes)
 * and an operation tag to identify the operation type.
 */
template <typename OpTag, typename LeftExpr, typename RightExpr>
class binary_expr : public biginteger_expression<binary_expr<OpTag, LeftExpr, RightExpr>> {
public:
    static constexpr bool is_leaf = false;
    using left_type = LeftExpr;
    using right_type = RightExpr;
    using op_tag = OpTag;

    constexpr binary_expr(LeftExpr left, RightExpr right) noexcept
        : m_left(std::move(left)), m_right(std::move(right)) {}

    constexpr const LeftExpr &left() const noexcept { return m_left; }
    constexpr const RightExpr &right() const noexcept { return m_right; }

    constexpr LeftExpr &left() noexcept { return m_left; }
    constexpr RightExpr &right() noexcept { return m_right; }

private:
    LeftExpr m_left;
    RightExpr m_right;
};

/**
 * @brief Helper to create binary expression with automatic operand wrapping.
 */
template <typename OpTag, typename L, typename R>
constexpr auto make_binary_expr(L &&left, R &&right) noexcept {
    using left_expr_t = to_expr_t<L>;
    using right_expr_t = to_expr_t<R>;

    return binary_expr<OpTag, left_expr_t, right_expr_t>(make_expr(std::forward<L>(left)),
                                                         make_expr(std::forward<R>(right)));
}

} // namespace biginteger_detail

// ============================================================================
// Expression operators
// ============================================================================

/**
 * @brief Addition operator for expression templates.
 *
 * Enabled only for valid expression operands (expressions or wrappable types).
 */
template <
    typename L, typename R,
    WJR_REQUIRES(biginteger_detail::is_expr_operand_v<L> &&biginteger_detail::is_expr_operand_v<R>)>
constexpr auto operator+(L &&lhs, R &&rhs) noexcept {
    return biginteger_detail::make_binary_expr<biginteger_detail::add_tag>(std::forward<L>(lhs),
                                                                           std::forward<R>(rhs));
}

/**
 * @brief Subtraction operator for expression templates.
 */
template <
    typename L, typename R,
    WJR_REQUIRES(biginteger_detail::is_expr_operand_v<L> &&biginteger_detail::is_expr_operand_v<R>)>
constexpr auto operator-(L &&lhs, R &&rhs) noexcept {
    return biginteger_detail::make_binary_expr<biginteger_detail::sub_tag>(std::forward<L>(lhs),
                                                                           std::forward<R>(rhs));
}

/**
 * @brief Multiplication operator for expression templates.
 */
template <
    typename L, typename R,
    WJR_REQUIRES(biginteger_detail::is_expr_operand_v<L> &&biginteger_detail::is_expr_operand_v<R>)>
constexpr auto operator*(L &&lhs, R &&rhs) noexcept {
    return biginteger_detail::make_binary_expr<biginteger_detail::mul_tag>(std::forward<L>(lhs),
                                                                           std::forward<R>(rhs));
}

} // namespace wjr

#endif // WJR_BIGINTEGER_EXPRESSION_EXPRESSION_HPP__