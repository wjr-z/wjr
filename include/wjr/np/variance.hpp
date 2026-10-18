#ifndef WJR_NP_VARIANCE_HPP__
#define WJR_NP_VARIANCE_HPP__

#include <wjr/np/expression_reduce.hpp>
#include <wjr/np/mean.hpp>

namespace wjr::np {

// Two ordered passes: mean, then squared deviations. ddof is nonnegative;
// size <= ddof returns quiet NaN without reading the input.
template <class Accumulator = void, class T, class Extents, class Layout>
requires(detail::kernel_element<T> && detail::kernel_layout<Layout> &&
         (std::is_void_v<Accumulator> || detail::mean_accumulator_type<Accumulator>))
[[nodiscard]] WJR_INTRINSIC_INLINE constexpr auto variance(tensor_view<T, Extents, Layout> source,
                                                           size_t ddof = 0) noexcept {
    using accumulator_type = detail::mean_accumulator<Accumulator, T>;
    const auto count = static_cast<size_t>(source.size());
    if (count <= ddof)
        return std::numeric_limits<accumulator_type>::quiet_NaN();
    const auto center = np::mean<accumulator_type>(source);
    const auto deviations = as_expr(source) - center;
    return np::sum<accumulator_type>(deviations * deviations) /
           static_cast<accumulator_type>(count - ddof);
}

} // namespace wjr::np

#endif // WJR_NP_VARIANCE_HPP__
