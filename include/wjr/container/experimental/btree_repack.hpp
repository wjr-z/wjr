#ifndef WJR_CONTAINER_EXPERIMENTAL_BTREE_REPACK_HPP__
#define WJR_CONTAINER_EXPERIMENTAL_BTREE_REPACK_HPP__

#include <wjr/container/btree.hpp>

#include <vector>

namespace wjr::btree_detail {
/// @brief Restrict moving compaction to the existing real-pair compact storage policy.
template <typename Traits>
concept compact_repack_traits = compact_slots_v<Traits>;

/// @brief Stream old leaves into fewer ordinary leaves without changing public storage layout.
struct compact_repack_access {
    template <typename Traits>
    using pointer_allocator = typename std::allocator_traits<
        typename Traits::_Alty>::template rebind_alloc<typename Traits::node_type *>;

    template <typename Traits>
    struct writer {
        basic_btree<Traits> &tree;
        std::vector<typename Traits::node_type *, pointer_allocator<Traits>> &roots;
        typename Traits::slot_usize_type per_leaf;
        size_t extra;
        size_t index = 0;
        typename Traits::leaf_node_type *current = nullptr;
        typename Traits::slot_usize_type position = 0;
        intrusive::list_node<typename Traits::leaf_node_type> leaves;

        void append(typename Traits::value_type &value) noexcept {
            if (current == nullptr)
                current = tree._create_leaf_node();
            tree._construct_leaf_value(current, position, std::move(value));
            ++position;
            if (position == per_leaf + (index < extra)) {
                current->size() = -static_cast<typename Traits::slot_size_type>(position);
                current->m_parent = nullptr;
                roots[index++] = current;
                leaves.push_back(current);
                current = nullptr;
                position = 0;
            }
        }
    };

    template <typename Traits>
    static void consume(typename Traits::node_type *node, writer<Traits> &output) noexcept {
        const auto size = node->size();
        if (size < 0) {
            auto *leaf = node->as_leaf();
            for (unsigned i = 0; i < static_cast<unsigned>(-size); ++i) {
                output.append(Traits::from_ivalue(leaf->m_values[i]));
                output.tree._destroy_slot(leaf->m_values[i]);
            }
            output.tree._drop_leaf_node(leaf);
        } else {
            auto *inner = node->as_inner();
            for (unsigned i = 0; i <= static_cast<unsigned>(size); ++i)
                consume<Traits>(inner->m_sons[i], output);
            output.tree._drop_inner_node(inner);
        }
    }

    template <typename Traits>
    static auto minimum(typename Traits::node_type *node) noexcept {
        while (node->size() >= 0)
            node = node->as_inner()->m_sons[0];
        return Traits::to_ikey(node->as_leaf()->get_key(0));
    }

    template <typename Traits>
    static bool repack(basic_btree<Traits> &tree) {
        auto *original = tree._get_root();
        if (original == nullptr || original->size() < 0)
            return false;
        const auto total = tree.size();
        constexpr size_t capacity = Traits::node_size;
        const size_t desired = total / capacity + (total % capacity != 0);
        size_t present = 0;
        for (auto *leaf = tree._get_sentry()->next(); leaf != tree._get_sentry();
             leaf = leaf->next())
            ++present;
        if (present == desired)
            return false;

        std::vector<typename Traits::node_type *, pointer_allocator<Traits>> roots(
            desired, nullptr, pointer_allocator<Traits>(tree._get_allocator()));
        writer<Traits> output{tree, roots,
                              static_cast<typename Traits::slot_usize_type>(total / desired),
                              static_cast<size_t>(total % desired)};
        tree._get_root() = nullptr;
        tree._get_size() = 0;
        tree._get_sentry()->init_self();
        consume<Traits>(original, output);
        WJR_ASSERT(output.index == desired && output.current == nullptr);

        size_t active = desired;
        constexpr size_t fanout = capacity + 1;
        while (active > 1) {
            const size_t groups = active / fanout + (active % fanout != 0);
            const size_t each = active / groups;
            const size_t extra = active % groups;
            size_t offset = 0;
            for (size_t group = 0; group < groups; ++group) {
                auto *parent = tree._create_inner_node();
                const size_t count = each + (group < extra);
                for (unsigned i = 0; i < count; ++i) {
                    auto *child = roots[offset + i];
                    parent->m_sons[i] = child;
                    child->m_parent = parent;
                    child->pos() = i;
                    if (i != 0)
                        parent->m_keys[i - 1] = minimum<Traits>(child);
                }
                parent->size() = static_cast<typename Traits::slot_size_type>(count - 1);
                roots[group] = parent;
                offset += count;
            }
            active = groups;
        }
        auto *complete = roots[0];
        complete->m_parent = nullptr;
        tree._get_root() = complete;
        tree._get_size() = total;
        replace(std::addressof(output.leaves), tree._get_sentry());
        return true;
    }
};
} // namespace wjr::btree_detail

namespace wjr::experimental {
/**
 * @brief Move compact pairs into the minimum number of ordinary leaves.
 * @return Whether rebuilding occurred; false leaves all values and references unchanged.
 * @note Preserves allocator/comparator instances and node capacity. Rebuilding invalidates
 * all element references/iterators. SSO, a single leaf, or a minimum leaf count are no-ops.
 * Workspace failures precede mutation. Rebuilding keeps the public tree empty until
 * publication and uses the existing node/value no-throw domain, including fatal OOM.
 */
template <btree_detail::compact_repack_traits Traits>
bool repack_compact_btree(basic_btree<Traits> &tree) {
    return btree_detail::compact_repack_access::repack(tree);
}
} // namespace wjr::experimental

#endif // WJR_CONTAINER_EXPERIMENTAL_BTREE_REPACK_HPP__
