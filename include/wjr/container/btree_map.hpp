#ifndef WJR_CONTAINER_BTREE_MAP_HPP__
#define WJR_CONTAINER_BTREE_MAP_HPP__

#include <wjr/container/btree.hpp>

namespace wjr {

template <typename Key, typename Value, typename Pr = std::less<Key>,
          typename Alloc = std::allocator<char>>
class btree_map : public basic_btree<btree_traits<Key, Value, false, Pr, Alloc>> {
    using Traits = btree_traits<Key, Value, false, Pr, Alloc>;
    using Mybase = basic_btree<Traits>;

public:
    using key_type = typename Mybase::key_type;
    using mapped_type = typename Mybase::mapped_type;
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
    WJR_PURE bool __is_lower_bound_same(const_iterator iter, const key_type &key) const {
        return iter != this->cend() && !this->key_comp()(Traits::get_key(*iter), key);
    }

    WJR_PURE bool __count_unique(const key_type &key) const {
        auto iter = this->lower_bound(key);
        return __is_lower_bound_same(iter, key);
    }

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(btree_map);

    template <typename Iter>
    btree_map(Iter first, Iter last, const key_compare &comp = key_compare()) noexcept
        : Mybase(comp) {
        for (; first != last; ++first) {
            emplace(*first);
        }
    }

    btree_map(std::initializer_list<value_type> il,
              const key_compare &comp = key_compare()) noexcept
        : btree_map(il.begin(), il.end(), comp) {}

    mapped_type &at(const key_type &key) {
        auto iter = this->lower_bound(key);
        if (!__is_lower_bound_same(iter, key)) {
            WJR_THROW(std::out_of_range("invalid map key"));
        }

        return iter->second;
    }

    const mapped_type &at(const key_type &key) const {
        return const_cast<btree_map &>(*this).at(key);
    }

    mapped_type &operator[](const key_type &key) { return try_emplace(key).first->second; }

    std::pair<iterator, bool> insert(const value_type &val) noexcept {
        return this->__emplace_unique(val);
    }

    std::pair<iterator, bool> insert(value_type &&val) noexcept {
        return this->__emplace_unique(std::move(val));
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args &&...args) noexcept {
        return this->__emplace_unique(std::forward<Args>(args)...);
    }

    template <typename... Args>
    std::pair<iterator, bool> try_emplace(const key_type &k, Args &&...args) {
        return this->__try_emplace_unique(k, std::forward<Args>(args)...);
    }

    template <typename... Args>
    std::pair<iterator, bool> try_emplace(key_type &&k, Args &&...args) {
        return this->__try_emplace_unique(std::move(k), std::forward<Args>(args)...);
    }

    size_type count(const key_type &key) const noexcept { return __count_unique(key) ? 1 : 0; }
};

template <typename Key, typename Value, typename Pr>
WJR_NODISCARD bool operator==(const btree_map<Key, Value, Pr> &lhs,
                              const btree_map<Key, Value, Pr> &rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Key, typename Value, typename Pr>
WJR_NODISCARD bool operator!=(const btree_map<Key, Value, Pr> &lhs,
                              const btree_map<Key, Value, Pr> &rhs) {
    return !(lhs == rhs);
}

template <typename Key, typename Value, typename Pr>
WJR_NODISCARD bool operator<(const btree_map<Key, Value, Pr> &lhs,
                             const btree_map<Key, Value, Pr> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Key, typename Value, typename Pr>
WJR_NODISCARD bool operator>(const btree_map<Key, Value, Pr> &lhs,
                             const btree_map<Key, Value, Pr> &rhs) {
    return rhs < lhs;
}

template <typename Key, typename Value, typename Pr>
WJR_NODISCARD bool operator<=(const btree_map<Key, Value, Pr> &lhs,
                              const btree_map<Key, Value, Pr> &rhs) {
    return !(rhs < lhs);
}

template <typename Key, typename Value, typename Pr>
WJR_NODISCARD bool operator>=(const btree_map<Key, Value, Pr> &lhs,
                              const btree_map<Key, Value, Pr> &rhs) {
    return !(lhs < rhs);
}

} // namespace wjr

#endif // WJR_CONTAINER_BTREE_MAP_HPP__