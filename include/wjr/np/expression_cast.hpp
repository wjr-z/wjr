#ifndef WJR_NP_EXPRESSION_CAST_HPP__
#define WJR_NP_EXPRESSION_CAST_HPP__

#include <wjr/np/expression.hpp>

namespace wjr::np {

namespace detail {
template <class To>
struct cast_operation {
    template <class From>
    static constexpr To apply(const From &value) noexcept {
        return static_cast<To>(value);
    }
};
} // namespace detail

// Conversion occurs at this node, before its parent's arithmetic. The child
// retains the usual value closure and borrowed-data lifetime contract.
template <class To, class E>
requires(detail::expression_element<To> && std::same_as<To, std::remove_cv_t<To>> &&
         requires(E &&value) {
             as_expr(std::forward<E>(value));
             requires detail::expression_element<typename decltype(as_expr(
                 std::forward<E>(value)))::value_type>;
         })
[[nodiscard]] constexpr auto cast(E &&value) noexcept(noexcept(as_expr(std::forward<E>(value)))) {
    using child_type = decltype(as_expr(std::forward<E>(value)));
    return unary_expression<detail::cast_operation<To>, child_type>(
        as_expr(std::forward<E>(value)));
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_CAST_HPP__
