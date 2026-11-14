#ifndef WJR_CONTAINER_DETAIL_BTREE_JOIN_HPP__
#define WJR_CONTAINER_DETAIL_BTREE_JOIN_HPP__

#include <wjr/assert.hpp>

namespace wjr {
template <typename Traits>
class basic_btree;

namespace btree_detail {
/// @brief Join eligible equal-height disjoint graphs without relocating stored values.
struct disjoint_join_access {
    /// @brief Reject mismatched root/child kinds before loading endpoint values.
    template <typename Traits>
    WJR_INTRINSIC_INLINE static bool
    compatible_root_kinds(const typename Traits::node_type *left,
                          const typename Traits::node_type *right) noexcept {
        const bool leaf = left->size() < 0;
        if (leaf != (right->size() < 0))
            return false;
        return leaf || ((left->as_inner()->m_sons[0]->size() < 0) ==
                        (right->as_inner()->m_sons[0]->size() < 0));
    }

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
                        typename Traits::node_type *root, bool prepend) noexcept {
        root->m_parent = nullptr;
        destination._get_root() = root;
        destination._get_size() += source._get_size();
        auto *position = prepend ? destination._get_sentry()->next() : destination._get_sentry();
        position->splice_all_before(source._get_sentry());
        source._get_root() = nullptr;
        source._get_size() = 0;
    }

    template <typename Traits>
    static bool join(basic_btree<Traits> &destination, basic_btree<Traits> &source,
                     bool check_boundary = false) {
        constexpr unsigned capacity = Traits::node_size;
        constexpr unsigned minimum = capacity / 2;
        auto *destination_root = destination._get_root();
        auto *source_root = source._get_root();
        if (destination_root == nullptr || source_root == nullptr ||
            !compatible_root_kinds<Traits>(destination_root, source_root))
            return false;
        bool prepend = false;
        if (check_boundary) {
            const auto *first = destination._get_sentry()->next()->self();
            const auto *source_first = source._get_sentry()->next()->self();
            prepend = !destination.key_comp()(first->get_key(0), source_first->get_key(0));
            const auto *last = prepend ? source._get_sentry()->prev()->self()
                                       : destination._get_sentry()->prev()->self();
            const auto &lower =
                last->get_key(static_cast<typename Traits::slot_usize_type>(-last->size() - 1));
            const auto &upper = prepend ? first->get_key(0) : source_first->get_key(0);
            if (!destination.key_comp()(lower, upper))
                return false;
        }
        auto *left = prepend ? source_root : destination_root;
        auto *right = prepend ? destination_root : source_root;
        const auto left_size = left->size();
        const auto right_size = right->size();
        if (left_size < 0) {
            if (-left_size < static_cast<int>(minimum) || -right_size < static_cast<int>(minimum) ||
                -left_size - right_size <= static_cast<int>(capacity))
                return false;
        } else if (depth<Traits>(left) != depth<Traits>(right)) {
            return false;
        }
        if constexpr (!Traits::_Alty_traits::is_always_equal::value)
            WJR_ASSERT(destination._get_allocator() == source._get_allocator());

        auto &right_tree = prepend ? destination : source;
        const auto boundary = Traits::to_ikey(Traits::get_key(*right_tree.begin()));
        if (left_size < 0) {
            auto *parent = destination._create_inner_node();
            parent->size() = 1;
            parent->m_sons[0] = left;
            parent->m_sons[1] = right;
            parent->m_keys[0] = boundary;
            left->m_parent = right->m_parent = parent;
            left->pos() = 0;
            right->pos() = 1;
            publish(destination, source, parent, prepend);
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
            publish(destination, source, left, prepend);
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
            publish(destination, source, parent, prepend);
        }
        return true;
    }
};
} // namespace btree_detail

} // namespace wjr

#endif // WJR_CONTAINER_DETAIL_BTREE_JOIN_HPP__
