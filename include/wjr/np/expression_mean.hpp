#ifndef WJR_NP_EXPRESSION_MEAN_HPP__
#define WJR_NP_EXPRESSION_MEAN_HPP__

#include <limits>

#include <wjr/np/expression_reduce.hpp>
#include <wjr/np/mean.hpp>

namespace wjr::np {

// Ordered terminal sum in the selected floating accumulator, divided once.
// Each child keeps its own dtype; empty input returns quiet NaN without reads.
template <class Accumulator = void, tensor_expression E>
requires(detail::expression_element<typename E::value_type> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>))
[[nodiscard]] WJR_INTRINSIC_INLINE constexpr auto
mean(const E &expression) noexcept(noexcept(expression.read(size_t{}))) {
    using accumulator_type = detail::mean_accumulator<Accumulator, typename E::value_type>;
    const auto domain = detail::bind_expression_sum_domain(expression);
    if (domain.empty())
        return std::numeric_limits<accumulator_type>::quiet_NaN();
    const auto result = detail::sum_nonempty_expression<accumulator_type>(expression, domain);
    return result / static_cast<accumulator_type>(domain.linear_size());
}

} // namespace wjr::np

#endif // WJR_NP_EXPRESSION_MEAN_HPP__
