#ifndef WJR_CONTAINER_DETAIL_BTREE_JOIN_HPP__
#define WJR_CONTAINER_DETAIL_BTREE_JOIN_HPP__

#include <wjr/assert.hpp>

namespace wjr {
template <typename Traits>
class basic_btree;

namespace btree_detail {
/// @brief Join eligible disjoint graphs without relocating stored values.
struct disjoint_join_access {
    template <typename Traits>
    WJR_INTRINSIC_INLINE static unsigned
    collect_inner(const typename Traits::inner_node_type *left,
                  const typename Traits::inner_node_type *right,
                  typename Traits::ikey_type boundary, typename Traits::node_type **children,
                  typename Traits::ikey_type *separators) noexcept {
        const unsigned left_count = static_cast<unsigned>(left->size()) + 1;
        const unsigned right_count = static_cast<unsigned>(right->size()) + 1;
        for (unsigned i = 0; i < left_count; ++i) {
            children[i] = left->m_sons[i];
            if (i != 0)
                separators[i - 1] = left->m_keys[i - 1];
        }
        separators[left_count - 1] = boundary;
        for (unsigned i = 0; i < right_count; ++i) {
            children[left_count + i] = right->m_sons[i];
            if (i != 0)
                separators[left_count + i - 1] = right->m_keys[i - 1];
        }
        return left_count + right_count;
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
    static bool join_unequal(basic_btree<Traits> &destination, basic_btree<Traits> &source,
                             typename Traits::node_type *left, typename Traits::node_type *right,
                             unsigned left_depth, unsigned right_depth,
                             typename Traits::ikey_type boundary, bool prepend) {
        constexpr unsigned capacity = Traits::node_size;
        constexpr unsigned minimum = capacity / 2;
        const bool add_left = left_depth < right_depth;
        auto *root = add_left ? right : left;
        auto *short_root = add_left ? left : right;
        auto *edge = root;
        unsigned levels = add_left ? right_depth - left_depth : left_depth - right_depth;
        while (levels-- != 0)
            edge = edge->as_inner()->m_sons[add_left ? 0 : edge->size()];
        auto *parent = edge->m_parent;
        typename Traits::node_type *children[2 * (capacity + 1)];
        typename Traits::ikey_type separators[2 * capacity + 1];
        unsigned total = 0;
        if (short_root->size() < 0) {
            if (-short_root->size() < static_cast<int>(minimum) ||
                -short_root->size() - edge->size() <= static_cast<int>(capacity))
                return false;
        } else {
            total = static_cast<unsigned>(edge->size() + short_root->size()) + 2;
            if (total <= capacity + 1) {
                collect_inner<Traits>((add_left ? short_root : edge)->as_inner(),
                                      (add_left ? edge : short_root)->as_inner(), boundary,
                                      children, separators);
                fill_inner<Traits>(edge->as_inner(), children, separators, 0, total);
                publish(destination, source, root, prepend);
                destination._drop_inner_node(short_root->as_inner());
                return true;
            }
        }

        /// Allocate every possible split before changing either published graph.
        unsigned needed = 0;
        auto *scan = parent;
        while (scan != nullptr && scan->size() == static_cast<int>(capacity)) {
            ++needed;
            scan = scan->m_parent;
        }
        needed += scan == nullptr;
        typename Traits::node_type *reserved = nullptr;
        for (unsigned i = 0; i < needed; ++i) {
            auto *node = destination._create_inner_node();
            node->m_parent = reserved;
            reserved = node;
        }

        auto *extra = short_root;
        auto key = boundary;
        if (short_root->size() >= 0) {
            auto *lower = (add_left ? short_root : edge)->as_inner();
            auto *upper = (add_left ? edge : short_root)->as_inner();
            collect_inner<Traits>(lower, upper, boundary, children, separators);
            const unsigned first = (total + 1) / 2;
            fill_inner<Traits>(lower, children, separators, 0, first);
            fill_inner<Traits>(upper, children, separators, first, total - first);
            key = separators[first - 1];
        }

        while (parent != nullptr) {
            auto *inner = parent->as_inner();
            const unsigned count = static_cast<unsigned>(inner->size()) + 1;
            if (count <= capacity) {
                if (add_left) {
                    for (unsigned i = count; i != 0; --i) {
                        inner->m_sons[i] = inner->m_sons[i - 1];
                        inner->m_sons[i]->pos() = i;
                    }
                    for (unsigned i = count - 1; i != 0; --i)
                        inner->m_keys[i] = inner->m_keys[i - 1];
                    inner->m_sons[0] = extra;
                    inner->m_keys[0] = key;
                    extra->pos() = 0;
                } else {
                    inner->m_sons[count] = extra;
                    inner->m_keys[count - 1] = key;
                    extra->pos() = count;
                }
                extra->m_parent = inner;
                inner->size() = static_cast<typename Traits::slot_size_type>(count);
                break;
            }

            const unsigned offset = add_left;
            for (unsigned i = 0; i < count; ++i) {
                children[i + offset] = inner->m_sons[i];
                if (i != 0)
                    separators[i + offset - 1] = inner->m_keys[i - 1];
            }
            children[add_left ? 0 : count] = extra;
            separators[add_left ? 0 : count - 1] = key;
            auto *sibling = reserved->as_inner();
            reserved = reserved->m_parent;
            auto *next = parent->m_parent;
            const unsigned first = (count + 2) / 2;
            fill_inner<Traits>(add_left ? sibling : inner, children, separators, 0, first);
            fill_inner<Traits>(add_left ? inner : sibling, children, separators, first,
                               count + 1 - first);
            key = separators[first - 1];
            extra = sibling;
            parent = next;
        }
        if (parent == nullptr) {
            auto *new_root = reserved->as_inner();
            reserved = reserved->m_parent;
            new_root->size() = 1;
            new_root->m_keys[0] = key;
            new_root->m_sons[0] = add_left ? extra : root;
            new_root->m_sons[1] = add_left ? root : extra;
            new_root->m_sons[0]->m_parent = new_root;
            new_root->m_sons[0]->pos() = 0;
            new_root->m_sons[1]->m_parent = new_root;
            new_root->m_sons[1]->pos() = 1;
            root = new_root;
        }
        WJR_ASSERT(reserved == nullptr);
        publish(destination, source, root, prepend);
        return true;
    }

    template <typename Traits>
    static bool join(basic_btree<Traits> &destination, basic_btree<Traits> &source,
                     bool check_boundary = false) {
        constexpr unsigned capacity = Traits::node_size;
        constexpr unsigned minimum = capacity / 2;
        auto *destination_root = destination._get_root();
        auto *source_root = source._get_root();
        if (destination_root == nullptr || source_root == nullptr)
            return false;
        bool prepend = false;
        if (check_boundary) {
            const auto destination_size = destination_root->size();
            const auto source_size = source_root->size();
            if (destination_size > 0 && source_size > 0) {
                const auto *destination_inner = destination_root->as_inner();
                const auto *source_inner = source_root->as_inner();
                if (!destination.key_comp()(
                        Traits::from_ikey(destination_inner->m_keys[destination_size - 1]),
                        Traits::from_ikey(source_inner->m_keys[0])) &&
                    !destination.key_comp()(
                        Traits::from_ikey(source_inner->m_keys[source_size - 1]),
                        Traits::from_ikey(destination_inner->m_keys[0])))
                    return false;
            }
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
        const auto left_depth = depth<Traits>(left);
        const auto right_depth = depth<Traits>(right);
        if constexpr (!Traits::_Alty_traits::is_always_equal::value)
            WJR_ASSERT(destination._get_allocator() == source._get_allocator());
        auto &right_tree = prepend ? destination : source;
        const auto boundary = Traits::to_ikey(Traits::get_key(*right_tree.begin()));
        if (left_depth != right_depth)
            return join_unequal(destination, source, left, right, left_depth, right_depth, boundary,
                                prepend);
        if (left_size < 0) {
            if (-left_size < static_cast<int>(minimum) || -right_size < static_cast<int>(minimum) ||
                -left_size - right_size <= static_cast<int>(capacity))
                return false;
        }
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

        typename Traits::node_type *children[2 * (capacity + 1)];
        typename Traits::ikey_type separators[2 * capacity + 1];
        const auto total = collect_inner<Traits>(left->as_inner(), right->as_inner(), boundary,
                                                 children, separators);

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
