#ifndef WJR_NP_ARGPARTITION_HPP__
#define WJR_NP_ARGPARTITION_HPP__

#include <limits>

#include <wjr/np/detail/index_order.hpp>
#include <wjr/np/partition.hpp>

namespace wjr::np {

// Write a permutation whose requested positions have their sorted source
// values, without moving source elements. Each side is partitioned, not sorted.
// Output and ranks are disjoint from each other and output from source.
// Empty ranks still produce the identity permutation. Ties are not stable.
template <sort_order Order = sort_order::ascending,
          partition_method Method = partition_method::nth_element, class Index, class DE, class DL,
          class T, class SE, class SL, class Rank, size_t Extent>
requires(std::integral<Index> && !std::same_as<Index, bool> &&
         std::same_as<Index, std::remove_cv_t<Index>> && detail::kernel_element<T> &&
         DE::rank() == 1 && SE::rank() == 1 && detail::packed_layout<DL> &&
         detail::kernel_layout<SL> && detail::compatible_kernel_shape<DE, SE> &&
         std::integral<Rank> && !std::same_as<std::remove_cv_t<Rank>, bool> &&
         !std::is_volatile_v<Rank> && detail::supported_partition_method<Method> &&
         detail::supported_index_order<Order, sort_stability::unstable>)
constexpr void argpartition_to(tensor_view<Index, DE, DL> destination,
                               tensor_view<T, SE, SL> source,
                               std::span<Rank, Extent> ranks) noexcept {
    detail::assert_kernel_shape(destination, source);
    const auto count = static_cast<size_t>(source.extent(0));
    [[maybe_unused]] size_t previous = 0;
    for ([[maybe_unused]] const auto rank : ranks) {
        WJR_ASSERT_L2(static_cast<size_t>(rank) >= previous && static_cast<size_t>(rank) < count);
        previous = static_cast<size_t>(rank);
    }
    if (count == 0)
        return;
    WJR_ASSERT_L2(count - 1 <= static_cast<size_t>(std::numeric_limits<Index>::max()));
    auto *output = destination.data_handle();
    for (size_t i = 0; i != count; ++i)
        output[i] = static_cast<Index>(i);
    if (ranks.empty())
        return;
    const auto read = detail::index_reader<Index>(source);
    using compare = detail::index_order<Order, sort_stability::unstable, decltype(read)>;
    detail::rank_selector<Method>::template apply<compare>(output, 0, count, std::span<Rank>(ranks),
                                                           read);
}

} // namespace wjr::np

#endif // WJR_NP_ARGPARTITION_HPP__
