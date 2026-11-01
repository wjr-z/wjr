#ifndef WJR_CONTAINER_EXPERIMENTAL_BTREE_JOIN_HPP__
#define WJR_CONTAINER_EXPERIMENTAL_BTREE_JOIN_HPP__

#include <wjr/container/btree.hpp>

namespace wjr::btree_detail {

/// @brief Join storage only when ordering has no differing comparator instance state.
template <typename Traits>
concept disjoint_join_traits = merge_standard_order<Traits, Traits>;

/// @brief Explicit ordered-shard join; ordinary merge does not instantiate this path.
struct disjoint_join_access {
    template <typename Traits>
    static unsigned depth(typename Traits::node_type *node) noexcept {
        unsigned result = 0;
        while (node->size() >= 0) {
            ++result;
            node = node->as_inner()->m_sons[0];
        }
        return result;
    }

    template <typename Traits>
    static void fill_inner(typename Traits::inner_node_type *node,
                           typename Traits::node_type *const *children,
                           const typename Traits::ikey_type *separators, unsigned first,
                           unsigned count) noexcept {
        for (unsigned i = 0; i < count; ++i) {
            auto *child = children[first + i];
            node->m_sons[i] = child;
            child->m_parent = node;
            child->pos() = i;
            if (i != 0)
                node->m_keys[i - 1] = separators[first + i - 1];
        }
        node->size() = static_cast<typename Traits::slot_size_type>(count - 1);
    }

    template <typename Traits>
    static void publish(basic_btree<Traits> &destination, basic_btree<Traits> &source,
                        typename Traits::node_type *root) noexcept {
        root->m_parent = nullptr;
        destination._get_root() = root;
        destination._get_size() += source._get_size();
        destination._get_sentry()->splice_all_before(source._get_sentry());
        source._get_root() = nullptr;
        source._get_size() = 0;
    }

    template <typename Traits>
    static bool join(basic_btree<Traits> &destination, basic_btree<Traits> &source) {
        constexpr unsigned capacity = Traits::node_size;
        constexpr unsigned minimum = capacity / 2;
        auto *left = destination._get_root();
        auto *right = source._get_root();
        if (left == nullptr || right == nullptr)
            return false;
        const auto left_size = left->size();
        const auto right_size = right->size();
        if ((left_size < 0) != (right_size < 0))
            return false;
        if (left_size < 0) {
            if (-left_size < static_cast<int>(minimum) || -right_size < static_cast<int>(minimum) ||
                -left_size - right_size <= static_cast<int>(capacity))
                return false;
        } else if (depth<Traits>(left) != depth<Traits>(right)) {
            return false;
        }
        if constexpr (!Traits::_Alty_traits::is_always_equal::value)
            WJR_ASSERT(destination._get_allocator() == source._get_allocator());

        const auto boundary = Traits::to_ikey(Traits::get_key(*source.begin()));
        if (left_size < 0) {
            auto *parent = destination._create_inner_node();
            parent->size() = 1;
            parent->m_sons[0] = left;
            parent->m_sons[1] = right;
            parent->m_keys[0] = boundary;
            left->m_parent = right->m_parent = parent;
            left->pos() = 0;
            right->pos() = 1;
            publish(destination, source, parent);
            return true;
        }

        const unsigned left_count = static_cast<unsigned>(left_size) + 1;
        const unsigned right_count = static_cast<unsigned>(right_size) + 1;
        const unsigned total = left_count + right_count;
        typename Traits::node_type *children[2 * (capacity + 1)];
        typename Traits::ikey_type separators[2 * capacity + 1];
        for (unsigned i = 0; i < left_count; ++i) {
            children[i] = left->as_inner()->m_sons[i];
            if (i != 0)
                separators[i - 1] = left->as_inner()->m_keys[i - 1];
        }
        separators[left_count - 1] = boundary;
        for (unsigned i = 0; i < right_count; ++i) {
            children[left_count + i] = right->as_inner()->m_sons[i];
            if (i != 0)
                separators[left_count + i - 1] = right->as_inner()->m_keys[i - 1];
        }

        if (total <= capacity + 1) {
            fill_inner<Traits>(left->as_inner(), children, separators, 0, total);
            publish(destination, source, left);
            destination._drop_inner_node(right->as_inner());
        } else {
            auto *parent = destination._create_inner_node();
            const unsigned first = (total + 1) / 2;
            fill_inner<Traits>(left->as_inner(), children, separators, 0, first);
            fill_inner<Traits>(right->as_inner(), children, separators, first, total - first);
            parent->size() = 1;
            parent->m_keys[0] = separators[first - 1];
            parent->m_sons[0] = left;
            parent->m_sons[1] = right;
            left->m_parent = right->m_parent = parent;
            left->pos() = 0;
            right->pos() = 1;
            publish(destination, source, parent);
        }
        return true;
    }
};
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
