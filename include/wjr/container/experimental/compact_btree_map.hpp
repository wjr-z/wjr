#ifndef WJR_CONTAINER_EXPERIMENTAL_COMPACT_BTREE_MAP_HPP__
#define WJR_CONTAINER_EXPERIMENTAL_COMPACT_BTREE_MAP_HPP__

#include <stdexcept>

#include <wjr/container/btree.hpp>

namespace wjr::btree_detail {

/// @brief Inline real const-key pairs with scalar separators; legacy traits remain unchanged.
template <typename Key, typename Value, typename Compare, typename Alloc, size_t NodeSize>
struct compact_map_traits : btree_traits<Key, Value, false, Compare, Alloc, NodeSize> {
    using base = btree_traits<Key, Value, false, Compare, Alloc, NodeSize>;
    using value_type = typename base::value_type;
    using compact_storage_tag = void;
    using ivalue_type = compact_slot<value_type>;
    static constexpr bool is_inline_value = true;
    using node_type = btree_node<compact_map_traits>;
    using inner_node_type = btree_inner_node<compact_map_traits>;
    using leaf_node_type = btree_leaf_node<compact_map_traits>;

    static value_type &from_ivalue(ivalue_type &slot) noexcept { return *slot.get(); }
    static const value_type &from_ivalue(const ivalue_type &slot) noexcept { return *slot.get(); }
};

/// @brief Restrict the prototype to inline scalar keys and nonthrowing value relocation.
template <typename Key, typename Value>
concept compact_map_domain =
    std::is_scalar_v<Key> && is_inline_key_v<Key> && std::is_same_v<Key, std::remove_cv_t<Key>> &&
    std::is_object_v<Value> && std::is_same_v<Value, std::remove_cv_t<Value>> &&
    std::is_nothrow_move_constructible_v<std::pair<const Key, Value>> &&
    std::is_nothrow_destructible_v<Value>;

} // namespace wjr::btree_detail

namespace wjr::experimental {

/**
 * @brief Experimental map with no separate allocation for each value.
 * @details Stores actual pair<const Key, Value> objects in leaves and the two-slot root.
 * Every mutation may invalidate iterators, references and pointers. Value relocation and
 * destruction must not throw; other throwing operations follow the tree's termination contract.
 * Scalar separator keys remain inline. NodeSize is a slot count, not a byte budget.
 */
template <typename Key, typename Value, typename Pr = std::less<Key>,
          typename Alloc = std::allocator<char>, size_t NodeSize = 16>
requires btree_detail::compact_map_domain<Key, Value>
class compact_btree_map
    : public basic_btree<btree_detail::compact_map_traits<Key, Value, Pr, Alloc, NodeSize>> {
    using Traits = btree_detail::compact_map_traits<Key, Value, Pr, Alloc, NodeSize>;
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
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(compact_btree_map);

    template <typename Iter>
    compact_btree_map(Iter first, Iter last,
                      const key_compare &comp = key_compare()) noexcept(noexcept(Mybase(comp)))
        : Mybase(comp) {
        for (; first != last; ++first) {
            emplace(*first);
        }
    }

    compact_btree_map(std::initializer_list<value_type> il,
                      const key_compare &comp = key_compare()) noexcept(noexcept(Mybase(comp)))
        : compact_btree_map(il.begin(), il.end(), comp) {}

    mapped_type &at(const key_type &key) {
        auto iter = this->lower_bound(key);
        if (!_is_lower_bound_same(iter, key)) {
            WJR_THROW(std::out_of_range("invalid map key"));
        }

        return iter->second;
    }

    const mapped_type &at(const key_type &key) const {
        return const_cast<compact_btree_map &>(*this).at(key);
    }

    compact_btree_map &operator=(std::initializer_list<value_type> il) noexcept {
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
        return const_cast<compact_btree_map &>(*this).at(key);
    }
};

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
requires btree_detail::compact_map_domain<Key, Value>
WJR_NODISCARD bool operator==(const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                              const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
requires btree_detail::compact_map_domain<Key, Value>
WJR_NODISCARD bool operator!=(const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                              const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return !(lhs == rhs);
}

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
requires btree_detail::compact_map_domain<Key, Value>
WJR_NODISCARD bool operator<(const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                             const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
requires btree_detail::compact_map_domain<Key, Value>
WJR_NODISCARD bool operator>(const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                             const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return rhs < lhs;
}

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
requires btree_detail::compact_map_domain<Key, Value>
WJR_NODISCARD bool operator<=(const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                              const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return !(rhs < lhs);
}

template <typename Key, typename Value, typename Pr, typename Alloc, size_t NodeSize>
requires btree_detail::compact_map_domain<Key, Value>
WJR_NODISCARD bool operator>=(const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &lhs,
                              const compact_btree_map<Key, Value, Pr, Alloc, NodeSize> &rhs) {
    return !(lhs < rhs);
}

} // namespace wjr::experimental

#endif // WJR_CONTAINER_EXPERIMENTAL_COMPACT_BTREE_MAP_HPP__