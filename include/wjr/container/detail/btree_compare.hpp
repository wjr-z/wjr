#ifndef WJR_CONTAINER_DETAIL_BTREE_COMPARE_HPP__
#define WJR_CONTAINER_DETAIL_BTREE_COMPARE_HPP__

#include <algorithm>
#include <compare>
#include <concepts>
#include <functional>

#include <wjr/config.hpp>

namespace wjr::btree_detail {
/// @brief Ordering needs value comparisons, independently of equality and key_compare.
template <typename Value>
concept ordered_value = std::predicate<std::less<>, const Value &, const Value &>;

/// @brief Preserve comparison categories, synthesizing weak ordering from less-than when needed.
struct value_three_way {
    template <ordered_value Value>
    constexpr auto operator()(const Value &lhs, const Value &rhs) const {
        if constexpr (std::three_way_comparable_with<Value, Value>) {
            return lhs <=> rhs;
        } else {
            return lhs < rhs   ? std::weak_ordering::less
                   : rhs < lhs ? std::weak_ordering::greater
                               : std::weak_ordering::equivalent;
        }
    }
};

template <typename Tree>
WJR_INTRINSIC_INLINE bool equal_sequence(const Tree &lhs, const Tree &rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Tree>
requires ordered_value<typename Tree::value_type>
WJR_INTRINSIC_INLINE auto compare_sequence(const Tree &lhs, const Tree &rhs) {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                                                  value_three_way{});
}
} // namespace wjr::btree_detail

#endif // WJR_CONTAINER_DETAIL_BTREE_COMPARE_HPP__
