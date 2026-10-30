#ifndef WJR_CONTAINER_BTREE_MAP_HPP__
#define WJR_CONTAINER_BTREE_MAP_HPP__

#include <stdexcept>

#include <wjr/container/btree.hpp>

namespace wjr {

template <typename Key, typename Value, typename Pr = std::less<Key>,
          typename Alloc = std::allocator<char>, size_t NodeSize = 0>
class btree_map : public basic_btree<btree_traits<Key, Value, false, Pr, Alloc, NodeSize>> {
    using Traits = btree_traits<Key, Value, false, Pr, Alloc, NodeSize>;
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

    using Mybase::insert;
    using Mybase::Mybase;

private:
    template <typename Lookup>
    bool _is_lower_bound_same(const_iterator iter, const Lookup &key) const {
        return iter != this->cend() && !this->key_comp()(key, Traits::get_key(*iter));
    }

    template <typename Lookup>
    bool _count_unique(const Lookup &key) const {
        auto iter = this->lower_bound(key);
        return _is_lower_bound_same(iter, key);
    }

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(btree_map);

    template <typename Iter>
    btree_map(Iter first, Iter last,
              const key_compare &comp = key_compare()) noexcept(noexcept(Mybase(comp)))
        : Mybase(comp) {
        for (; first != last; ++first) {
            emplace(*first);
        }
    }

    btree_map(std::initializer_list<value_type> il,
              const key_compare &comp = key_compare()) noexcept(noexcept(Mybase(comp)))
        : btree_map(il.begin(), il.end(), comp) {}

    mapped_type &at(const key_type &key) {
        auto iter = this->lower_bound(key);
        if (!_is_lower_bound_same(iter, key)) {
            WJR_THROW(std::out_of_range("invalid map key"));
        }

        return iter->second;
    }

    const mapped_type &at(const key_type &key) const {
        return const_cast<btree_map &>(*this).at(key);
    }

    btree_map &operator=(std::initializer_list<value_type> il) noexcept {
        Mybase::operator=(il);
        return *this;
    }

    mapped_type &operator[](const key_type &key) { return try_emplace(key).first->second; }

    /// @brief Insert a missing moved key with a default mapped value.
    mapped_type &operator[](key_type &&key) { return try_emplace(std::move(key)).first->second; }

    /// @brief Assign an existing mapped value, or insert a new const-key pair.
    template <btree_detail::upsert_source<mapped_type> Source>
    std::pair<iterator, bool> insert_or_assign(const key_type &key, Source &&value) {
        auto result = try_emplace(key, std::forward<Source>(value));
        if (!result.second)
            result.first->second = std::forward<Source>(value);
        return result;
    }

    /// @copydoc insert_or_assign(const key_type &, Source &&)
    template <btree_detail::upsert_source<mapped_type> Source>
    std::pair<iterator, bool> insert_or_assign(key_type &&key, Source &&value) {
        auto result = try_emplace(std::move(key), std::forward<Source>(value));
        if (!result.second)
            result.first->second = std::forward<Source>(value);
        return result;
    }

    std::pair<iterator, bool> insert(const value_type &val) noexcept {
        return this->_emplace_unique(val);
    }

    std::pair<iterator, bool> insert(value_type &&val) noexcept {
        return this->_emplace_unique(std::move(val));
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args &&...args) noexcept {
        return this->_emplace_unique(std::forward<Args>(args)...);
    }

    template <typename... Args>
    std::pair<iterator, bool> try_emplace(const key_type &k, Args &&...args) {
        return this->_try_emplace_unique(k, std::forward<Args>(args)...);
    }

    template <typename... Args>
    std::pair<iterator, bool> try_emplace(key_type &&k, Args &&...args) {
        return this->_try_emplace_unique(std::move(k), std::forward<Args>(args)...);
    }

    size_type count(const key_type &key) const noexcept { return _count_unique(key) ? 1 : 0; }

    /// @brief Count an equivalent key using the transparent comparator without constructing a key.
    /// @pre Comparator calls for keys and lookup values do not throw.
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    size_type count(const Lookup &key) const noexcept {
        return _count_unique(key) ? 1 : 0;
    }

    /// @brief Access the mapped value using a transparent lookup value.
    /// @pre Comparator calls for keys and lookup values do not throw.
    /// @throws std::out_of_range if no equivalent key is present.
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    mapped_type &at(const Lookup &key) {
        auto iter = this->lower_bound(key);
        if (!_is_lower_bound_same(iter, key)) {
            WJR_THROW(std::out_of_range("invalid map key"));
        }
        return iter->second;
    }

    /// @copydoc at(const Lookup &)
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    const mapped_type &at(const Lookup &key) const {
        return const_cast<btree_map &>(*this).at(key);
    }
};

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
WJR_NODISCARD bool operator==(const btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                              const btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
WJR_NODISCARD bool operator!=(const btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                              const btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return !(lhs == rhs);
}

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
WJR_NODISCARD bool operator<(const btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                             const btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
WJR_NODISCARD bool operator>(const btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                             const btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return rhs < lhs;
}

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
WJR_NODISCARD bool operator<=(const btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                              const btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return !(rhs < lhs);
}

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
WJR_NODISCARD bool operator>=(const btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                              const btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return !(lhs < rhs);
}

} // namespace wjr

#endif // WJR_CONTAINER_BTREE_MAP_HPP__