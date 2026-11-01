#ifndef WJR_CONTAINER_EXPERIMENTAL_BTREE_BULK_HPP__
#define WJR_CONTAINER_EXPERIMENTAL_BTREE_BULK_HPP__

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <vector>

namespace wjr::btree_detail {

template <typename Iter>
inline constexpr bool forward_move_iterator = false;
template <std::forward_iterator Iter>
inline constexpr bool forward_move_iterator<std::move_iterator<Iter>> = true;

/// @brief Input whose length can be obtained without consuming a single-pass source.
template <typename Sentinel, typename Iter>
concept ordered_build_sentinel =
    std::sentinel_for<Sentinel, Iter> &&
    (std::forward_iterator<Iter> || std::sized_sentinel_for<Sentinel, Iter> ||
     forward_move_iterator<Iter>);

/// @brief Opt-in packed construction; ordinary tree headers only declare this access point.
struct ordered_builder_access {
    template <typename Traits>
    static std::type_identity<Traits> traits_of(const basic_btree<Traits> &);

    template <typename Traits>
    using pointer_allocator = typename std::allocator_traits<
        typename Traits::_Alty>::template rebind_alloc<typename Traits::node_type *>;

    template <typename Traits>
    struct forest {
        basic_btree<Traits> &tree;
        std::vector<typename Traits::node_type *, pointer_allocator<Traits>> roots;
        intrusive::list_node<typename Traits::leaf_node_type> leaves;
        bool committed = false;

        explicit forest(basic_btree<Traits> &tree)
            : tree(tree), roots(pointer_allocator<Traits>(tree._get_allocator())) {
            leaves.init_self();
        }
        ~forest() noexcept {
            if (!committed) {
                for (auto *root : roots) {
                    if (root != nullptr)
                        tree._rec_destroy_and_deallocate(root);
                }
            }
        }
    };

    template <typename Traits>
    struct leaf_guard {
        basic_btree<Traits> &tree;
        typename Traits::leaf_node_type *leaf;
        typename Traits::slot_usize_type initialized = 0;
        bool allocated;

        ~leaf_guard() noexcept {
            if (leaf != nullptr) {
                for (typename Traits::slot_usize_type i = 0; i < initialized; ++i)
                    tree._destroy_slot(leaf->m_values[i]);
                if (allocated)
                    tree._drop_leaf_node(leaf);
            }
        }
    };

    template <typename Iter, ordered_build_sentinel<Iter> Sentinel>
    static auto length(Iter first, Sentinel last) {
        if constexpr (std::sized_sentinel_for<Sentinel, Iter>) {
            const auto count = last - first;
            WJR_ASSERT(count >= 0);
            return count;
        } else {
            std::iter_difference_t<Iter> count = 0;
            for (; first != last; ++first)
                ++count;
            return count;
        }
    }

    template <typename Traits>
    static bool ordered(const basic_btree<Traits> &tree, const typename Traits::key_type &lhs,
                        const typename Traits::key_type &rhs) noexcept {
        return tree.key_comp()(lhs, rhs);
    }

    template <typename Traits, typename Iter>
    static void fill(basic_btree<Traits> &tree, leaf_guard<Traits> &guard, Iter &first,
                     typename Traits::slot_usize_type count,
                     const typename Traits::leaf_node_type *previous) {
        for (typename Traits::slot_usize_type i = 0; i < count; ++i) {
            tree._construct_leaf_value(guard.leaf, i, *first);
            ++guard.initialized;
            if (i != 0) {
                WJR_ASSERT(ordered(tree, guard.leaf->get_key(i - 1), guard.leaf->get_key(i)));
            } else if (previous != nullptr) {
                WJR_ASSERT(ordered(tree, previous->get_key(-previous->size() - 1),
                                   guard.leaf->get_key(0)));
            }
            ++first;
        }
    }

    template <typename Traits>
    static auto minimum(typename Traits::node_type *node) noexcept {
        while (node->size() >= 0)
            node = node->as_inner()->m_sons[0];
        return Traits::to_ikey(node->as_leaf()->get_key(0));
    }

    template <typename Traits>
    static auto
    parent(basic_btree<Traits> &tree,
           const std::vector<typename Traits::node_type *, pointer_allocator<Traits>> &children,
           size_t offset, size_t count) noexcept {
        auto *node = tree._create_inner_node();
        node->size() = static_cast<typename Traits::slot_size_type>(count - 1);
        node->m_parent = nullptr;
        for (size_t i = 0; i < count; ++i) {
            auto *child = children[offset + i];
            node->m_sons[i] = child;
            child->m_parent = node;
            child->pos() = static_cast<typename Traits::slot_usize_type>(i);
            if (i != 0)
                node->m_keys[i - 1] = minimum<Traits>(child);
        }
        return node;
    }

    template <typename Traits, insert_iterator<typename Traits::value_type> Iter,
              ordered_build_sentinel<Iter> Sentinel>
    static void build(basic_btree<Traits> &tree, Iter first, Sentinel last) {
        using size_type = typename Traits::size_type;
        using slot_type = typename Traits::slot_usize_type;
        const auto distance = length(first, last);
        const auto count = static_cast<size_type>(distance);
        WJR_ASSERT(count == distance);
        if (count <= Traits::node_size) {
            const bool allocated = count > 2;
            auto *leaf = allocated ? tree._create_leaf_node() : tree._get_base();
            leaf_guard<Traits> guard{tree, leaf, 0, allocated};
            fill(tree, guard, first, static_cast<slot_type>(count), nullptr);
            if (allocated) {
                leaf->size() = -static_cast<typename Traits::slot_size_type>(count);
                leaf->m_parent = nullptr;
                tree._get_root() = leaf;
                tree._get_sentry()->push_back(leaf);
            }
            tree._get_size() = count;
            guard.leaf = nullptr;
            return;
        }

        forest<Traits> owner(tree);
        const size_type leaf_count = count / Traits::node_size + (count % Traits::node_size != 0);
        owner.roots.resize(leaf_count, nullptr);
        const auto each = count / leaf_count;
        const auto extra = count % leaf_count;
        typename Traits::leaf_node_type *previous = nullptr;
        for (size_type i = 0; i < leaf_count; ++i) {
            auto *leaf = tree._create_leaf_node();
            leaf_guard<Traits> guard{tree, leaf, 0, true};
            const auto elements = static_cast<slot_type>(each + (i < extra));
            fill(tree, guard, first, elements, previous);
            leaf->size() = -static_cast<typename Traits::slot_size_type>(elements);
            leaf->m_parent = nullptr;
            owner.roots[i] = leaf;
            owner.leaves.push_back(leaf);
            guard.leaf = nullptr;
            previous = leaf;
        }

        constexpr size_t fanout = Traits::node_size + 1;
        while (owner.roots.size() > 1) {
            const auto children = owner.roots.size();
            const auto groups = children / fanout + (children % fanout != 0);
            std::vector<typename Traits::node_type *, pointer_allocator<Traits>> next(
                groups, nullptr, owner.roots.get_allocator());
            const auto per_parent = children / groups;
            const auto remainder = children % groups;
            size_t offset = 0;
            for (size_t i = 0; i < groups; ++i) {
                const auto child_count = per_parent + (i < remainder);
                next[i] = parent(tree, owner.roots, offset, child_count);
                offset += child_count;
            }
            owner.roots.swap(next);
        }
        auto *root = owner.roots[0];
        root->m_parent = nullptr;
        tree._get_root() = root;
        tree._get_size() = count;
        replace(std::addressof(owner.leaves), tree._get_sentry());
        owner.committed = true;
    }
};
} // namespace wjr::btree_detail

namespace wjr::experimental {
namespace btree_bulk_detail {
template <typename Tree>
concept btree_type =
    requires(const Tree &tree) {
        typename Tree::value_type;
        typename decltype(btree_detail::ordered_builder_access::traits_of(tree))::type;
    } &&
    std::constructible_from<Tree, const typename Tree::key_compare &,
                            const typename Tree::allocator_type &> &&
    std::move_constructible<Tree>;
} // namespace btree_bulk_detail

/// @brief Build a packed tree from strictly ordered, unique converted keys.
/// @pre Stored keys are strictly ordered by comp; input remains valid throughout construction.
/// @note Sized single-pass input is supported. Other input must be multipass; streaming unsized
/// input can use the existing hinted importer. Workspace uses the supplied rebound allocator.
template <btree_bulk_detail::btree_type Tree,
          btree_detail::insert_iterator<typename Tree::value_type> Iter,
          btree_detail::ordered_build_sentinel<Iter> Sentinel>
Tree make_btree_from_sorted_unique(Iter first, Sentinel last,
                                   const typename Tree::key_compare &comp,
                                   const typename Tree::allocator_type &alloc) {
    Tree result(comp, alloc);
    btree_detail::ordered_builder_access::build(result, first, last);
    return result;
}

/// @copydoc make_btree_from_sorted_unique(Iter, Sentinel, const Tree::key_compare&, const
/// Tree::allocator_type&)
template <btree_bulk_detail::btree_type Tree,
          btree_detail::insert_iterator<typename Tree::value_type> Iter,
          btree_detail::ordered_build_sentinel<Iter> Sentinel>
requires std::default_initializable<typename Tree::key_compare> &&
         std::default_initializable<typename Tree::allocator_type>
Tree make_btree_from_sorted_unique(Iter first, Sentinel last) {
    return make_btree_from_sorted_unique<Tree>(first, last, typename Tree::key_compare{},
                                               typename Tree::allocator_type{});
}
} // namespace wjr::experimental

#endif // WJR_CONTAINER_EXPERIMENTAL_BTREE_BULK_HPP__
