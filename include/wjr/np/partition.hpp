#ifndef WJR_NP_PARTITION_HPP__
#define WJR_NP_PARTITION_HPP__

#include <algorithm>
#include <bit>
#include <span>

#include <wjr/np/detail/elementwise.hpp>

namespace wjr::np {

enum class partition_method { nth_element, shared_pivot };

namespace detail {

template <partition_method Method>
concept supported_partition_method =
    Method == partition_method::nth_element || Method == partition_method::shared_pivot;

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

template <partition_method Method>
struct rank_selector;

// Only indirect selection supplies construction state. Empty packs keep the
// original value-selection signatures and pivot-only predicate captures.
template <>
struct rank_selector<partition_method::nth_element> {
    template <class Compare, class T, class Index, class... State>
    static constexpr void apply(T *data, size_t first, size_t last, std::span<Index> ranks,
                                State... state) noexcept {
        const Compare compare{state...};
        while (!ranks.empty()) {
            const auto middle = ranks.size() / 2;
            const auto rank = static_cast<size_t>(ranks[middle]);
            if (rank == first)
                std::iter_swap(data + rank, std::min_element(data + first, data + last, compare));
            else if (rank + 1 == last)
                std::iter_swap(data + rank, std::max_element(data + first, data + last, compare));
            else
                std::nth_element(data + first, data + rank, data + last, compare);
            if (ranks.front() == ranks.back())
                return;
            const auto lower =
                std::lower_bound(ranks.begin(), ranks.begin() + middle, ranks[middle]);
            const auto upper =
                std::upper_bound(ranks.begin() + middle + 1, ranks.end(), ranks[middle]);
            apply<Compare>(data, first, rank, std::span<Index>(ranks.begin(), lower), state...);
            first = rank + 1;
            ranks = std::span<Index>(upper, ranks.end());
        }
    }
};

template <class Compare, class T, class Index, class... State>
constexpr void select_ranks(T *data, size_t first, size_t last, std::span<Index> ranks,
                            unsigned budget, State... state) noexcept {
    const Compare compare{state...};
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
        const auto lower = std::partition(data + first, data + last, [pivot, state...](T value) {
            return Compare{state...}(value, pivot);
        });
        const auto low = static_cast<size_t>(lower - data);
        // Ordered endpoints classify a whole request range in constant work.
        if (static_cast<size_t>(ranks.back()) < low) {
            last = low;
            continue;
        }
        const auto upper = std::partition(lower, data + last, [pivot, state...](T value) {
            return !Compare{state...}(pivot, value);
        });
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
            select_ranks<Compare>(data, first, low, left, budget, state...);
            ranks = right;
            first = high;
        } else {
            select_ranks<Compare>(data, high, last, right, budget, state...);
            ranks = left;
            last = low;
        }
    }
}

template <>
struct rank_selector<partition_method::shared_pivot> {
    template <class Compare, class T, class Index, class... State>
    static constexpr void apply(T *data, size_t first, size_t last, std::span<Index> ranks,
                                State... state) noexcept {
        select_ranks<Compare>(data, first, last, ranks,
                              partition_depth_factor * std::bit_width(last - first), state...);
    }
};

} // namespace detail

// In-place multi-selection. Ranks are nondecreasing, nonnegative, in range,
// and disjoint from data. Duplicate requests are allowed. Every requested
// position has its sorted value; each side is partitioned but not sorted.
// NaNs sort last, signed zeros compare equal, and no stable order is promised.
// Empty ranks do not access data. The live contiguous range fits ptrdiff_t.
// The default balances requested ranks using nth_element; shared_pivot is an
// explicit alternative. Both share work, with different partition traffic.
template <partition_method Method = partition_method::nth_element, class T, class E, class L,
          class Index, size_t Extent>
requires(detail::supported_partition_method<Method> && detail::kernel_element<T> &&
         !std::is_const_v<T> && E::rank() == 1 && detail::packed_layout<L> &&
         std::integral<Index> && !std::same_as<std::remove_cv_t<Index>, bool> &&
         !std::is_volatile_v<Index>)
constexpr void partition_at(tensor_view<T, E, L> data, std::span<Index, Extent> ranks) noexcept {
    if (ranks.empty())
        return;
    [[maybe_unused]] size_t previous = 0;
    for ([[maybe_unused]] const auto index : ranks) {
        WJR_ASSERT_L2(static_cast<size_t>(index) >= previous &&
                      static_cast<size_t>(index) < static_cast<size_t>(data.extent(0)));
        previous = static_cast<size_t>(index);
    }
    detail::rank_selector<Method>::template apply<detail::partition_less>(
        data.data_handle(), 0, static_cast<size_t>(data.extent(0)), std::span<Index>(ranks));
}

} // namespace wjr::np

#endif // WJR_NP_PARTITION_HPP__
