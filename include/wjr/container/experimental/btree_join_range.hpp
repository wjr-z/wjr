#ifndef WJR_CONTAINER_EXPERIMENTAL_BTREE_JOIN_RANGE_HPP__
#define WJR_CONTAINER_EXPERIMENTAL_BTREE_JOIN_RANGE_HPP__

#include <wjr/container/experimental/btree_join.hpp>

#include <vector>

namespace wjr::btree_detail {

/// @brief Multipass owner references that remain live while their graphs are consumed.
template <typename Iter, typename Traits>
concept disjoint_shard_iterator =
    std::forward_iterator<Iter> && std::is_lvalue_reference_v<std::iter_reference_t<Iter>> &&
    std::convertible_to<std::iter_reference_t<Iter>, basic_btree<Traits> &>;

/// @brief Normalize several equal-height shard roots before creating higher levels.
struct disjoint_range_access {
    template <typename Traits, typename T>
    using allocator =
        typename std::allocator_traits<typename Traits::_Alty>::template rebind_alloc<T>;

    template <typename Traits>
    struct descriptor {
        typename Traits::node_type *child;
        typename Traits::ikey_type minimum;
    };

    template <typename Traits>
    using source_vector =
        std::vector<basic_btree<Traits> *, allocator<Traits, basic_btree<Traits> *>>;

    template <typename Traits>
    using root_vector = std::vector<typename Traits::inner_node_type *,
                                    allocator<Traits, typename Traits::inner_node_type *>>;

    template <typename Traits>
    struct extra_roots {
        basic_btree<Traits> &tree;
        root_vector<Traits> &roots;
        size_t borrowed;
        bool published = false;
        ~extra_roots() noexcept {
            if (!published) {
                for (size_t i = borrowed; i < roots.size(); ++i)
                    tree._drop_inner_node(roots[i]);
            }
        }
    };

    template <typename Traits>
    struct fresh_root {
        basic_btree<Traits> &tree;
        typename Traits::inner_node_type *node;
        ~fresh_root() noexcept {
            if (node != nullptr)
                tree._drop_inner_node(node);
        }
    };

    template <typename Traits>
    struct layout {
        bool compatible;
        size_t children;
        typename Traits::size_type total;
    };

    template <typename Traits>
    static layout<Traits> classify(basic_btree<Traits> &destination,
                                   const source_vector<Traits> &sources) {
        auto *root = destination._get_root();
        bool height_known = root != nullptr && root->size() >= 0;
        bool compatible = destination.empty() || height_known;
        unsigned height = height_known ? disjoint_join_access::depth<Traits>(root) : 0;
        size_t children = height_known ? static_cast<size_t>(root->size()) + 1 : 0;
        typename Traits::size_type total = destination.size();
        const typename Traits::key_type *previous =
            destination.empty() ? nullptr : std::addressof(Traits::get_key(*destination.rbegin()));
        for (auto *source : sources) {
            WJR_ASSERT(source != std::addressof(destination));
            if constexpr (!Traits::_Alty_traits::is_always_equal::value)
                WJR_ASSERT(destination._get_allocator() == source->_get_allocator());
            WJR_ASSERT(previous == nullptr ||
                       destination.key_comp()(*previous, Traits::get_key(*source->begin())));
            previous = std::addressof(Traits::get_key(*source->rbegin()));
            total += source->size();
            auto *candidate = source->_get_root();
            if (candidate == nullptr || candidate->size() < 0) {
                compatible = false;
                continue;
            }
            const auto candidate_height = disjoint_join_access::depth<Traits>(candidate);
            if (!height_known) {
                height = candidate_height;
                height_known = true;
            } else if (height != candidate_height) {
                compatible = false;
            }
            children += static_cast<size_t>(candidate->size()) + 1;
        }
        return {compatible, children, total};
    }

    template <typename Traits>
    static void combine(basic_btree<Traits> &destination, source_vector<Traits> &sources) {
        constexpr size_t fanout = Traits::node_size + 1;
        const auto shape = classify(destination, sources);
        if (!shape.compatible) {
            for (auto *source : sources)
                experimental::append_btree_disjoint(destination, *source);
            return;
        }

        size_t needed = 0;
        for (size_t level = shape.children; level > 1;) {
            level = level / fanout + (level % fanout != 0);
            needed += level;
        }
        const size_t borrowed = sources.size() + !destination.empty();
        auto &al = destination._get_allocator();
        std::vector<descriptor<Traits>, allocator<Traits, descriptor<Traits>>> frontier{
            allocator<Traits, descriptor<Traits>>(al)};
        frontier.reserve(shape.children);
        root_vector<Traits> roots{allocator<Traits, typename Traits::inner_node_type *>(al)};
        roots.reserve(needed > borrowed ? needed : borrowed);
        const auto capture = [&](basic_btree<Traits> &tree) {
            auto *inner = tree._get_root()->as_inner();
            roots.push_back(inner);
            const auto minimum = Traits::to_ikey(Traits::get_key(*tree.begin()));
            for (unsigned i = 0; i <= static_cast<unsigned>(inner->size()); ++i)
                frontier.push_back({inner->m_sons[i], i == 0 ? minimum : inner->m_keys[i - 1]});
        };
        if (!destination.empty())
            capture(destination);
        for (auto *source : sources)
            capture(*source);

        extra_roots<Traits> owner{destination, roots, borrowed};
        while (roots.size() < needed) {
            fresh_root<Traits> guard{destination, destination._create_inner_node()};
            roots.push_back(guard.node);
            guard.node = nullptr;
        }

        size_t used = 0;
        size_t active = shape.children;
        while (active > 1) {
            const size_t groups = active / fanout + (active % fanout != 0);
            const size_t per_group = active / groups;
            const size_t extra = active % groups;
            size_t offset = 0;
            for (size_t group = 0; group < groups; ++group) {
                auto *parent = roots[used++];
                const auto minimum = frontier[offset].minimum;
                const size_t count = per_group + (group < extra);
                for (unsigned i = 0; i < count; ++i) {
                    const auto entry = frontier[offset + i];
                    parent->m_sons[i] = entry.child;
                    entry.child->m_parent = parent;
                    entry.child->pos() = i;
                    if (i != 0)
                        parent->m_keys[i - 1] = entry.minimum;
                }
                parent->size() = static_cast<typename Traits::slot_size_type>(count - 1);
                frontier[group] = {parent, minimum};
                offset += count;
            }
            active = groups;
        }
        auto *complete = frontier[0].child;
        complete->m_parent = nullptr;
        destination._get_root() = complete;
        destination._get_size() = shape.total;
        for (auto *source : sources) {
            destination._get_sentry()->splice_all_before(source->_get_sentry());
            source->_get_root() = nullptr;
            source->_get_size() = 0;
        }
        owner.published = true;
        for (size_t i = used; i < roots.size(); ++i)
            destination._drop_inner_node(roots[i]);
    }

    template <typename Traits, disjoint_shard_iterator<Traits> Iter,
              std::sentinel_for<Iter> Sentinel>
    static void append(basic_btree<Traits> &destination, Iter first, Sentinel last) {
        size_t nonempty = 0;
        basic_btree<Traits> *single = nullptr;
        for (auto it = first; it != last; ++it) {
            basic_btree<Traits> &source = *it;
            if (!source.empty()) {
                ++nonempty;
                single = std::addressof(source);
            }
        }
        if (nonempty == 0)
            return;
        if (nonempty == 1) {
            experimental::append_btree_disjoint(destination, *single);
            return;
        }
        source_vector<Traits> sources{
            allocator<Traits, basic_btree<Traits> *>(destination._get_allocator())};
        sources.reserve(nonempty);
        for (; first != last; ++first) {
            basic_btree<Traits> &source = *first;
            if (!source.empty())
                sources.push_back(std::addressof(source));
        }
        combine(destination, sources);
    }
};
} // namespace wjr::btree_detail

namespace wjr::experimental {
/**
 * @brief Consume an ordered owner range, normalizing equal-height inner roots in one batch.
 * @pre Nonempty sources are distinct, exclude destination, compare allocator-equal,
 * and follow destination and one another in strictly disjoint comparator order.
 * Their objects/references remain valid for this call; the final size is representable.
 * @note Empty/single-source ranges avoid workspace. Forward references are collected
 * before mutation. Same-height inner graphs preserve values; other shapes append serially.
 * Workspace and unused parents are cleaned up on preparation failure. Existing
 * tree node/value no-throw and reference-invalidation contracts remain unchanged.
 */
template <btree_detail::disjoint_join_traits Traits,
          btree_detail::disjoint_shard_iterator<Traits> Iter, std::sentinel_for<Iter> Sentinel>
void append_btree_disjoint(basic_btree<Traits> &destination, Iter first, Sentinel last) {
    btree_detail::disjoint_range_access::append(destination, first, last);
}
} // namespace wjr::experimental

#endif // WJR_CONTAINER_EXPERIMENTAL_BTREE_JOIN_RANGE_HPP__
