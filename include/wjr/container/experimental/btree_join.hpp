#ifndef WJR_CONTAINER_EXPERIMENTAL_BTREE_JOIN_HPP__
#define WJR_CONTAINER_EXPERIMENTAL_BTREE_JOIN_HPP__

#include <wjr/container/btree.hpp>

namespace wjr::btree_detail {

/// @brief Join storage only when ordering has no differing comparator instance state.
template <typename Traits>
concept disjoint_join_traits = merge_standard_order<Traits, Traits>;

} // namespace wjr::btree_detail

namespace wjr::experimental {
/**
 * @brief Append a strictly later, disjoint shard, consuming its elements.
 * @pre Allocators compare equal. When both trees are nonempty, destination's last
 * key strictly precedes source's first key according to their standard ordering.
 * @note Source becomes empty and reusable; self-append is a no-op. Equal-height
 * eligible heap graphs join without relocating values. Other shapes use merge.
 * Ordinary merge's allocator, failure and invalidation contracts remain in force.
 */
template <btree_detail::disjoint_join_traits Traits>
void append_btree_disjoint(basic_btree<Traits> &destination, basic_btree<Traits> &source) {
    if (std::addressof(destination) == std::addressof(source) || source.empty())
        return;
    if (destination.empty()) {
        destination.merge(source);
        return;
    }
    WJR_ASSERT(destination.key_comp()(Traits::get_key(*destination.rbegin()),
                                      Traits::get_key(*source.begin())));
    if (!btree_detail::disjoint_join_access::join(destination, source))
        destination.merge(source);
}
} // namespace wjr::experimental

#endif // WJR_CONTAINER_EXPERIMENTAL_BTREE_JOIN_HPP__
