#ifndef WJR_CONTAINER_BTREE_SET_HPP__
#define WJR_CONTAINER_BTREE_SET_HPP__

#include <wjr/container/btree.hpp>

namespace wjr {

template <typename Key, typename Pr = std::less<Key>, typename Alloc = std::allocator<char>,
          size_t NodeSize = 0>
class btree_set : public basic_btree<btree_traits<Key, void, false, Pr, Alloc, NodeSize>> {
    using Traits = btree_traits<Key, void, false, Pr, Alloc, NodeSize>;
    using Mybase = basic_btree<Traits>;

public:
    using key_type = typename Mybase::key_type;
    using value_type = typename Mybase::value_type;
    using size_type = typename Mybase::size_type;
    using difference_type = typename Mybase::difference_type;
    using key_compare = typename Mybase::key_compare;
    using allocator_type = typename Mybase::allocator_type;
    using reference = typename Mybase::reference;
    using const_reference = typename Mybase::const_reference;
    using pointer = typename Mybase::pointer;
    using const_pointer = typename Mybase::const_pointer;
    using iterator = typename Mybase::iterator;
    using const_iterator = typename Mybase::const_iterator;
    using reverse_iterator = typename Mybase::reverse_iterator;
    using const_reverse_iterator = typename Mybase::const_reverse_iterator;

    using Mybase::Mybase;

private:
    bool _is_lower_bound_same(const_iterator iter, const key_type &key) const {
        return iter != this->cend() && !this->key_comp()(Traits::get_key(*iter), key);
    }

    bool _count_unique(const key_type &key) const {
        auto iter = this->lower_bound(key);
        return _is_lower_bound_same(iter, key);
    }

public:
    std::pair<iterator, bool> insert(const value_type &val) noexcept {
        return this->_emplace_unique(val);
    }

    std::pair<iterator, bool> insert(value_type &&val) noexcept {
        return this->_emplace_unique(std::move(val));
    }

    template <typename... Args>
    WJR_INTRINSIC_INLINE std::pair<iterator, bool> emplace(Args &&...args) noexcept {
        return this->_emplace_unique(std::forward<Args>(args)...);
    }

    size_type count(const key_type &key) const noexcept { return _count_unique(key) ? 1 : 0; }
};

template <typename Key, typename Pr = std::less<Key>, typename Alloc = std::allocator<char>,
          size_t NodeSize = 0>
class btree_multiset : public basic_btree<btree_traits<Key, void, true, Pr, Alloc, NodeSize>> {
    using Traits = btree_traits<Key, void, true, Pr, Alloc, NodeSize>;
    using Mybase = basic_btree<Traits>;

public:
    using key_type = typename Mybase::key_type;
    using value_type = typename Mybase::value_type;
    using size_type = typename Mybase::size_type;
    using difference_type = typename Mybase::difference_type;
    using key_compare = typename Mybase::key_compare;
    using allocator_type = typename Mybase::allocator_type;
    using reference = typename Mybase::reference;
    using const_reference = typename Mybase::const_reference;
    using pointer = typename Mybase::pointer;
    using const_pointer = typename Mybase::const_pointer;
    using iterator = typename Mybase::iterator;
    using const_iterator = typename Mybase::const_iterator;
    using reverse_iterator = typename Mybase::reverse_iterator;
    using const_reverse_iterator = typename Mybase::const_reverse_iterator;

    using Mybase::Mybase;

    std::pair<iterator, bool> insert(const value_type &val) noexcept {
        return this->_emplace_multi(val);
    }

    std::pair<iterator, bool> insert(value_type &&val) noexcept {
        return this->_emplace_multi(std::move(val));
    }

    template <typename... Args>
    WJR_INTRINSIC_INLINE std::pair<iterator, bool> emplace(Args &&...args) noexcept {
        return this->_emplace_multi(std::forward<Args>(args)...);
    }
};

} // namespace wjr

#endif // WJR_CONTAINER_BTREE_SET_HPP__