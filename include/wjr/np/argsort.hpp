#ifndef WJR_NP_ARGSORT_HPP__
#define WJR_NP_ARGSORT_HPP__

#include <algorithm>
#include <limits>

#include <wjr/np/detail/index_order.hpp>

namespace wjr::np {

// Write a permutation of [0, size) without moving source values. Both views
// have equal length and disjoint storage. Source strides are positive; output
// is packed. NaNs sort last in either direction; signed zeros compare equal.
// Stable ordering breaks equal-value ties by their original index. No dynamic
// storage is allocated, including for stable ordering.
template <sort_order Order = sort_order::ascending,
          sort_stability Stability = sort_stability::unstable, class Index, class DE, class DL,
          class T, class SE, class SL>
requires(std::integral<Index> && !std::same_as<Index, bool> &&
         std::same_as<Index, std::remove_cv_t<Index>> && detail::kernel_element<T> &&
         DE::rank() == 1 && SE::rank() == 1 && detail::packed_layout<DL> &&
         detail::kernel_layout<SL> && detail::compatible_kernel_shape<DE, SE> &&
         detail::supported_index_order<Order, Stability>)
constexpr void argsort_to(tensor_view<Index, DE, DL> dst, tensor_view<T, SE, SL> src) noexcept {
    const auto count = static_cast<size_t>(src.extent(0));
    WJR_ASSERT_L2(dst.extent(0) == src.extent(0));
    if (count == 0)
        return;
    WJR_ASSERT_L2(count - 1 <= static_cast<size_t>(std::numeric_limits<Index>::max()));
    auto *const output = dst.data_handle();
    for (size_t i = 0; i != count; ++i)
        output[i] = static_cast<Index>(i);
    const auto read = detail::index_reader<Index>(src);
    std::sort(output, output + count, detail::index_order<Order, Stability, decltype(read)>{read});
}

} // namespace wjr::np

#endif // WJR_NP_ARGSORT_HPP__
