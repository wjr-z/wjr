#ifndef WJR_NP_HISTOGRAM_BEFORE_HPP__
#define WJR_NP_HISTOGRAM_BEFORE_HPP__

#include <algorithm>

#include <wjr/np/searchsorted.hpp>

namespace wjr::np::histogram_before {
namespace detail {
using namespace wjr::np::detail;

template <bool Contiguous, class D, class T, class OutputStep, class ValueStep, class EdgeStep>
constexpr void histogram_line(D *WJR_RESTRICT output, size_t bins, OutputStep output_step,
                              const T *values, size_t count, ValueStep value_step, const T *edges,
                              EdgeStep edge_step) noexcept {
    for (size_t i = 0; i != bins; ++i)
        output[i * output_step] = D{};
    if (count == 0)
        return;
    const auto first = edges[0];
    const auto last = edges[bins * edge_step];
    const auto *interior = edges + edge_step;
    for (size_t i = 0; i != count; ++i) {
        const auto value = values[i * value_step];
        if (!(first <= value && value <= last))
            continue;
        const auto bin = [&]() constexpr noexcept {
            if constexpr (Contiguous)
                return static_cast<size_t>(
                    std::upper_bound(interior, interior + (bins - 1), value) - interior);
            else
                return searchsorted_index<search_side::right, false>(interior, bins - 1, edge_step,
                                                                     value);
        }();
        ++output[bin * output_step];
    }
}

} // namespace detail

// Count [edge[i], edge[i+1]), with the last bin including the final edge.
// Edges are nondecreasing and contain no NaNs; at least two edges are required.
// NaN and out-of-domain values are ignored. Duplicate edges form empty bins,
// except the last zero-width bin still includes its right endpoint.
// Counts are cleared first, including for empty values. Their storage is disjoint
// from both inputs; inputs may overlap each other and remain stable. Each final
// bin count must be representable in D. No storage is allocated or retained.
template <class D, class DE, class DL, class T, class VE, class VL, class E, class EE, class EL>
requires(std::integral<D> && std::same_as<D, std::remove_cv_t<D>> && !std::same_as<D, bool> &&
         detail::kernel_element<T> && detail::kernel_element<E> &&
         !std::same_as<std::remove_cv_t<T>, bool> &&
         std::same_as<std::remove_cv_t<T>, std::remove_cv_t<E>> && DE::rank() == 1 &&
         VE::rank() == 1 && EE::rank() == 1 && detail::kernel_layout<DL> &&
         detail::kernel_layout<VL> && detail::kernel_layout<EL>)
constexpr void histogram_to(tensor_view<D, DE, DL> counts, tensor_view<T, VE, VL> values,
                            tensor_view<E, EE, EL> edges) noexcept {
    const auto bins = static_cast<size_t>(counts.extent(0));
    [[maybe_unused]] const auto edge_count = static_cast<size_t>(edges.extent(0));
    WJR_ASSERT_L2(edge_count >= 2 && edge_count - 1 == bins);
    constexpr bool contiguous = detail::packed_layout<EL> || detail::padded_layout<EL>;
    detail::histogram_line<contiguous>(counts.data_handle(), bins, detail::axis_stride<0>(counts),
                                       values.data_handle(), static_cast<size_t>(values.extent(0)),
                                       detail::axis_stride<0>(values), edges.data_handle(),
                                       contiguous ? size_t{1} : detail::axis_stride<0>(edges));
}

} // namespace wjr::np::histogram_before

#endif // WJR_NP_HISTOGRAM_BEFORE_HPP__
