#ifndef WJR_NP_MERGE_HPP__
#define WJR_NP_MERGE_HPP__

#include <algorithm>
#include <functional>
#include <utility>

#include <wjr/np/partition.hpp>
#include <wjr/np/reduce.hpp>

namespace wjr::np {

// Stable merge into disjoint storage. Both inputs are sorted according to compare;
// equivalent values from left precede those from right. The callable is borrowed.
template <class D, class DE, class DL, class T, class LE, class LL, class U, class RE, class RL,
          class Compare = detail::partition_less>
requires(detail::kernel_element<D> && !std::is_const_v<D> &&
         std::same_as<D, std::remove_const_t<T>> && std::same_as<D, std::remove_const_t<U>> &&
         DE::rank() == 1 && LE::rank() == 1 && RE::rank() == 1 && detail::kernel_layout<DL> &&
         detail::kernel_layout<LL> && detail::kernel_layout<RL> &&
         requires(Compare &compare, const D &a, const D &b) {
             static_cast<bool>(std::invoke(compare, a, b));
         })
constexpr void merge_sorted_to(
    tensor_view<D, DE, DL> destination, tensor_view<T, LE, LL> left, tensor_view<U, RE, RL> right,
    Compare &&compare =
        {}) noexcept(noexcept(static_cast<bool>(std::invoke(compare, std::declval<const D &>(),
                                                            std::declval<const D &>())))) {
    const auto left_count = static_cast<size_t>(left.extent(0));
    const auto right_count = static_cast<size_t>(right.extent(0));
    WJR_ASSERT_L2(destination.size() >= left_count &&
                  destination.size() - left_count == right_count);
    if (left_count == 0 && right_count == 0)
        return;
    auto *output = destination.data_handle();
    const auto *a = left.data_handle();
    const auto *b = right.data_handle();
    if constexpr (detail::packed_layout<DL> && detail::packed_layout<LL> &&
                  detail::packed_layout<RL>) {
        // Do not form an end pointer by adding zero to a null empty input.
        if (left_count == 0) {
            std::copy_n(b, right_count, output);
        } else if (right_count == 0) {
            std::copy_n(a, left_count, output);
        } else {
            // Standard algorithms may copy predicates. Only this reference capture
            // is copied; explicit-bool proxy results are normalized here as well.
            std::merge(a, a + left_count, b, b + right_count, output,
                       [&compare](const D &x, const D &y) {
                           return static_cast<bool>(std::invoke(compare, x, y));
                       });
        }
    } else {
        const auto ds = static_cast<size_t>(detail::axis_stride<0>(destination));
        const auto ls = static_cast<size_t>(detail::axis_stride<0>(left));
        const auto rs = static_cast<size_t>(detail::axis_stride<0>(right));
        size_t i = 0, j = 0, k = 0;
        while (i != left_count && j != right_count) {
            if (static_cast<bool>(
                    std::invoke(compare, std::as_const(b[j * rs]), std::as_const(a[i * ls]))))
                output[k++ * ds] = b[j++ * rs];
            else
                output[k++ * ds] = a[i++ * ls];
        }
        while (i != left_count)
            output[k++ * ds] = a[i++ * ls];
        while (j != right_count)
            output[k++ * ds] = b[j++ * rs];
    }
}

} // namespace wjr::np

#endif // WJR_NP_MERGE_HPP__
