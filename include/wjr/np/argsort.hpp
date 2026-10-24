#ifndef WJR_NP_ARGSORT_HPP__
#define WJR_NP_ARGSORT_HPP__

#include <algorithm>
#include <limits>

#include <wjr/np/detail/elementwise.hpp>

namespace wjr::np {

enum class sort_order { ascending, descending };
enum class sort_stability { unstable, stable };

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
         (Order == sort_order::ascending || Order == sort_order::descending) &&
         (Stability == sort_stability::unstable || Stability == sort_stability::stable))
constexpr void argsort_to(tensor_view<Index, DE, DL> dst, tensor_view<T, SE, SL> src) noexcept {
    const auto count = static_cast<size_t>(src.extent(0));
    WJR_ASSERT_L2(dst.extent(0) == src.extent(0));
    if (count == 0)
        return;
    WJR_ASSERT_L2(count - 1 <= static_cast<size_t>(std::numeric_limits<Index>::max()));
    auto *const output = dst.data_handle();
    for (size_t i = 0; i != count; ++i)
        output[i] = static_cast<Index>(i);
    const auto read = [src] {
        const auto *const input = src.data_handle();
        if constexpr (detail::packed_layout<SL> || detail::padded_layout<SL>)
            return [input](Index i) noexcept { return input[static_cast<size_t>(i)]; };
        else
            return [input, step = src.stride(0)](Index i) noexcept {
                return input[static_cast<size_t>(i) * step];
            };
    }();
    std::sort(output, output + count, [read](Index a, Index b) noexcept {
        const auto left = read(a);
        const auto right = read(b);
        if constexpr (std::is_floating_point_v<std::remove_cv_t<T>>) {
            if (left != left)
                return Stability == sort_stability::stable && right != right && a < b;
            if (right != right)
                return true;
        }
        if constexpr (Stability == sort_stability::stable)
            if (left == right)
                return a < b;
        if constexpr (Order == sort_order::ascending)
            return left < right;
        else
            return left > right;
    });
}

} // namespace wjr::np

#endif // WJR_NP_ARGSORT_HPP__
