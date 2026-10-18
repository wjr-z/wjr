#ifndef WJR_NP_PAIRWISE_HPP__
#define WJR_NP_PAIRWISE_HPP__

// Partial pairwise grouping adapted from NumPy 2.2.0, BSD-3-Clause.
// Attribution and license: docs/licenses/numpy-pairwise.txt.

#include <wjr/np/reduce.hpp>

namespace wjr::np {

namespace detail {

template <class Accumulator, class T, class... Stride>
requires(sizeof...(Stride) <= 1)
constexpr Accumulator pairwise_sum_leaf(const T *input, size_t count, Stride... strides) noexcept {
    const size_t step = (size_t{1} * ... * strides);
    if (count < 8) {
        Accumulator result = -Accumulator{0};
        for (size_t i = 0; i != count; ++i)
            result += static_cast<Accumulator>(input[i * step]);
        return result;
    }
    Accumulator a = static_cast<Accumulator>(input[0]);
    Accumulator b = static_cast<Accumulator>(input[step]);
    Accumulator c = static_cast<Accumulator>(input[2 * step]);
    Accumulator d = static_cast<Accumulator>(input[3 * step]);
    Accumulator e = static_cast<Accumulator>(input[4 * step]);
    Accumulator f = static_cast<Accumulator>(input[5 * step]);
    Accumulator g = static_cast<Accumulator>(input[6 * step]);
    Accumulator h = static_cast<Accumulator>(input[7 * step]);
    const auto bound = count - count % 8;
    size_t i = 8;
    for (; i != bound; i += 8) {
        a += static_cast<Accumulator>(input[i * step]);
        b += static_cast<Accumulator>(input[(i + 1) * step]);
        c += static_cast<Accumulator>(input[(i + 2) * step]);
        d += static_cast<Accumulator>(input[(i + 3) * step]);
        e += static_cast<Accumulator>(input[(i + 4) * step]);
        f += static_cast<Accumulator>(input[(i + 5) * step]);
        g += static_cast<Accumulator>(input[(i + 6) * step]);
        h += static_cast<Accumulator>(input[(i + 7) * step]);
    }
    Accumulator result = ((a + b) + (c + d)) + ((e + f) + (g + h));
    for (; i != count; ++i)
        result += static_cast<Accumulator>(input[i * step]);
    return result;
}

// Bound recursive inlining without preventing small, fixed-size leaves from
// being optimized in their caller. The contiguous signature has no stride.
template <class Accumulator, class T, class... Stride>
requires(sizeof...(Stride) <= 1)
WJR_NOINLINE constexpr Accumulator pairwise_sum_tree(const T *input, size_t count,
                                                     Stride... strides) noexcept {
    if (count <= 128)
        return pairwise_sum_leaf<Accumulator>(input, count, strides...);
    const size_t step = (size_t{1} * ... * strides);
    const auto half = count / 2 - count / 2 % 8;
    const auto left = pairwise_sum_tree<Accumulator>(input, half, strides...);
    const auto right =
        pairwise_sum_tree<Accumulator>(input + half * step, count - half, strides...);
    return left + right;
}

template <class Accumulator, class T, class... Stride>
requires(sizeof...(Stride) <= 1)
constexpr Accumulator pairwise_sum_kernel(const T *input, size_t count,
                                          Stride... strides) noexcept {
    if (count <= 128)
        return pairwise_sum_leaf<Accumulator>(input, count, strides...);
    return pairwise_sum_tree<Accumulator>(input, count, strides...);
}

} // namespace detail

// Explicit partial pairwise grouping; the default ordered sum is unchanged.
// Preconditions: a valid view, representable logical size and addressed span.
template <class Accumulator = void, class T, class Extents, class Layout>
requires(detail::kernel_element<T> &&
         detail::sum_accumulator_type<detail::sum_accumulator<Accumulator, T>> &&
         std::floating_point<detail::sum_accumulator<Accumulator, T>> &&
         (std::same_as<Layout, layout_right> ||
          (Extents::rank() <= 1 && detail::kernel_layout<Layout>)))
[[nodiscard]] constexpr auto pairwise_sum(tensor_view<T, Extents, Layout> source) noexcept {
    using accumulator_type = detail::sum_accumulator<Accumulator, T>;
    if (source.empty())
        return accumulator_type{0};
    const auto count = static_cast<size_t>(source.size());
    if constexpr (std::same_as<Layout, layout_right> || Extents::rank() == 0 ||
                  std::same_as<Layout, layout_left>)
        return detail::pairwise_sum_kernel<accumulator_type>(source.data_handle(), count);
    else
        return detail::pairwise_sum_kernel<accumulator_type>(source.data_handle(), count,
                                                             static_cast<size_t>(source.stride(0)));
}

} // namespace wjr::np

#endif // WJR_NP_PAIRWISE_HPP__
