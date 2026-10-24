#ifndef WJR_NP_PARTITION_HPP__
#define WJR_NP_PARTITION_HPP__

#include <algorithm>
#include <bit>
#include <span>

#include <wjr/np/detail/elementwise.hpp>

namespace wjr::np {
namespace detail {

inline constexpr size_t partition_sort_cutoff = 16;
inline constexpr unsigned partition_depth_factor = 2;

struct partition_less {
    template <class T>
    constexpr bool operator()(T left, T right) const noexcept {
        if constexpr (std::is_floating_point_v<T>)
            return left < right || (left == left && right != right);
        else
            return left < right;
    }
};

template <class Compare, class T, class Index>
constexpr void select_ranks(T *data, size_t first, size_t last, std::span<Index> ranks,
                            unsigned budget) noexcept {
    const Compare compare{};
    while (!ranks.empty()) {
        if (ranks.front() == ranks.back()) {
            const auto rank = static_cast<size_t>(ranks.front());
            if (rank == first)
                std::iter_swap(data + rank, std::min_element(data + first, data + last, compare));
            else if (rank + 1 == last)
                std::iter_swap(data + rank, std::max_element(data + first, data + last, compare));
            else
                std::nth_element(data + first, data + rank, data + last, compare);
            return;
        }
        if (last - first <= partition_sort_cutoff || budget == 0) {
            std::sort(data + first, data + last, compare);
            return;
        }
        --budget;
        // Keep a value snapshot: partitioning can exchange all sample slots.
        const auto a = data[first], b = data[first + (last - first) / 2], c = data[last - 1];
        const auto pivot = compare(a, b) ? (compare(b, c)   ? b
                                            : compare(a, c) ? c
                                                            : a)
                                         : (compare(a, c)   ? a
                                            : compare(b, c) ? c
                                                            : b);
        const auto lower = std::partition(data + first, data + last,
                                          [pivot](T value) { return Compare{}(value, pivot); });
        const auto low = static_cast<size_t>(lower - data);
        // Ordered endpoints classify a whole request range in constant work.
        if (static_cast<size_t>(ranks.back()) < low) {
            last = low;
            continue;
        }
        const auto upper = std::partition(lower, data + last,
                                          [pivot](T value) { return !Compare{}(pivot, value); });
        const auto high = static_cast<size_t>(upper - data);
        if (static_cast<size_t>(ranks.front()) >= high) {
            first = high;
            continue;
        }
        const auto left_end = std::lower_bound(ranks.begin(), ranks.end(), low);
        const auto right_begin = std::lower_bound(left_end, ranks.end(), high);
        const std::span<Index> left(ranks.begin(), left_end), right(right_begin, ranks.end());
        // Recurse on fewer requests, not fewer data elements, to bound depth.
        if (left.size() < right.size()) {
            select_ranks<Compare>(data, first, low, left, budget);
            ranks = right;
            first = high;
        } else {
            select_ranks<Compare>(data, high, last, right, budget);
            ranks = left;
            last = low;
        }
    }
}

} // namespace detail

// In-place multi-selection. Ranks are nondecreasing, nonnegative, in range,
// and disjoint from data. Duplicate requests are allowed. Every requested
// position has its sorted value; each side is partitioned but not sorted.
// NaNs sort last, signed zeros compare equal, and no stable order is promised.
// Empty ranks do not access data. The live contiguous range fits ptrdiff_t.
template <class T, class E, class L, class Index, size_t Extent>
requires(detail::kernel_element<T> && !std::is_const_v<T> && E::rank() == 1 &&
         detail::packed_layout<L> && std::integral<Index> &&
         !std::same_as<std::remove_cv_t<Index>, bool> && !std::is_volatile_v<Index>)
constexpr void partition_at(tensor_view<T, E, L> data, std::span<Index, Extent> ranks) noexcept {
    if (ranks.empty())
        return;
    [[maybe_unused]] size_t previous = 0;
    for ([[maybe_unused]] const auto index : ranks) {
        WJR_ASSERT_L2(static_cast<size_t>(index) >= previous &&
                      static_cast<size_t>(index) < static_cast<size_t>(data.extent(0)));
        previous = static_cast<size_t>(index);
    }
    detail::select_ranks<detail::partition_less>(
        data.data_handle(), 0, static_cast<size_t>(data.extent(0)), std::span<Index>(ranks),
        detail::partition_depth_factor * std::bit_width(static_cast<size_t>(data.extent(0))));
}

} // namespace wjr::np

#endif // WJR_NP_PARTITION_HPP__
