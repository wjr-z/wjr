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

    using Mybase::insert;
    using Mybase::Mybase;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(btree_set);

    /// @brief Copy values into the supplied allocator/resource.
    btree_set(const btree_set &other,
              const allocator_type &alloc) noexcept(noexcept(Mybase(other, alloc)))

        : Mybase(other, alloc) {}

    /// @brief Move using a supplied allocator, migrating elements when resources differ.
    btree_set(btree_set &&other,
              const allocator_type &alloc) noexcept(noexcept(Mybase(std::move(other), alloc)))

        : Mybase(std::move(other), alloc) {}

    /// @brief Construct a single-pass range using the supplied ordering.
    template <btree_detail::insert_iterator<value_type> Iter, std::sentinel_for<Iter> Sentinel>
    btree_set(Iter first, Sentinel last, const key_compare &comp = key_compare()) : Mybase(comp) {
        this->insert(first, last);
    }

    /// @brief Construct list values using the supplied ordering.
    btree_set(std::initializer_list<value_type> il,
              const key_compare &comp = key_compare()) noexcept(noexcept(Mybase(comp)))
        : btree_set(il.begin(), il.end(), comp) {}

    /// @brief Construct a single-pass range using explicit ordering and allocator state.
    template <btree_detail::insert_iterator<value_type> Iter, std::sentinel_for<Iter> Sentinel>
    btree_set(Iter first, Sentinel last, const key_compare &comp, const allocator_type &alloc)
        : Mybase(comp, alloc) {
        this->insert(first, last);
    }

    /// @brief Construct a range with the allocator and default ordering.
    template <btree_detail::insert_iterator<value_type> Iter, std::sentinel_for<Iter> Sentinel>
    requires btree_detail::default_allocator_order<key_compare, allocator_type>
    btree_set(Iter first, Sentinel last, const allocator_type &alloc)
        : btree_set(first, last, key_compare(), alloc) {}

    /// @brief Construct list values using explicit ordering and allocator state.
    btree_set(std::initializer_list<value_type> il, const key_compare &comp,
              const allocator_type &alloc) noexcept(noexcept(Mybase(comp, alloc)))
        : btree_set(il.begin(), il.end(), comp, alloc) {}

    /// @brief Construct list values using the allocator and default ordering.
    btree_set(std::initializer_list<value_type> il,
              const allocator_type &alloc) noexcept(noexcept(Mybase(key_compare(), alloc)))
    requires btree_detail::default_allocator_order<key_compare, allocator_type>
        : btree_set(il.begin(), il.end(), key_compare(), alloc) {}

    /// @brief Construct an empty tree using explicit allocator and default ordering.
    explicit btree_set(const allocator_type &alloc) noexcept(noexcept(Mybase(key_compare(), alloc)))
    requires btree_detail::default_allocator_order<key_compare, allocator_type>
        : Mybase(key_compare(), alloc) {}

    /// @brief Replace list values while retaining this tree's allocator and ordering.
    btree_set &operator=(std::initializer_list<value_type> il) noexcept {
        Mybase::operator=(il);
        return *this;
    }

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

    /// @brief Count an equivalent key using the transparent comparator without constructing a key.
    /// @pre Comparator calls for keys and lookup values do not throw.
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    size_type count(const Lookup &key) const noexcept {
        return _count_unique(key) ? 1 : 0;
    }
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

    using Mybase::insert;
    using Mybase::Mybase;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(btree_multiset);

    /// @brief Copy values into the supplied allocator/resource.
    btree_multiset(const btree_multiset &other,
                   const allocator_type &alloc) noexcept(noexcept(Mybase(other, alloc)))

        : Mybase(other, alloc) {}

    /// @brief Move using a supplied allocator, migrating elements when resources differ.
    btree_multiset(btree_multiset &&other,
                   const allocator_type &alloc) noexcept(noexcept(Mybase(std::move(other), alloc)))

        : Mybase(std::move(other), alloc) {}

    /// @brief Construct a single-pass range using the supplied ordering.
    template <btree_detail::insert_iterator<value_type> Iter, std::sentinel_for<Iter> Sentinel>
    btree_multiset(Iter first, Sentinel last, const key_compare &comp = key_compare())
        : Mybase(comp) {
        this->insert(first, last);
    }

    /// @brief Construct list values using the supplied ordering.
    btree_multiset(std::initializer_list<value_type> il,
                   const key_compare &comp = key_compare()) noexcept(noexcept(Mybase(comp)))
        : btree_multiset(il.begin(), il.end(), comp) {}

    /// @brief Construct a single-pass range using explicit ordering and allocator state.
    template <btree_detail::insert_iterator<value_type> Iter, std::sentinel_for<Iter> Sentinel>
    btree_multiset(Iter first, Sentinel last, const key_compare &comp, const allocator_type &alloc)
        : Mybase(comp, alloc) {
        this->insert(first, last);
    }

    /// @brief Construct a range with the allocator and default ordering.
    template <btree_detail::insert_iterator<value_type> Iter, std::sentinel_for<Iter> Sentinel>
    requires btree_detail::default_allocator_order<key_compare, allocator_type>
    btree_multiset(Iter first, Sentinel last, const allocator_type &alloc)
        : btree_multiset(first, last, key_compare(), alloc) {}

    /// @brief Construct list values using explicit ordering and allocator state.
    btree_multiset(std::initializer_list<value_type> il, const key_compare &comp,
                   const allocator_type &alloc) noexcept(noexcept(Mybase(comp, alloc)))
        : btree_multiset(il.begin(), il.end(), comp, alloc) {}

    /// @brief Construct list values using the allocator and default ordering.
    btree_multiset(std::initializer_list<value_type> il,
                   const allocator_type &alloc) noexcept(noexcept(Mybase(key_compare(), alloc)))
    requires btree_detail::default_allocator_order<key_compare, allocator_type>
        : btree_multiset(il.begin(), il.end(), key_compare(), alloc) {}

    /// @brief Construct an empty tree using explicit allocator and default ordering.
    explicit btree_multiset(const allocator_type &alloc) noexcept(noexcept(Mybase(key_compare(),
                                                                                  alloc)))
    requires btree_detail::default_allocator_order<key_compare, allocator_type>
        : Mybase(key_compare(), alloc) {}

    /// @brief Replace list values while retaining this tree's allocator and ordering.
    btree_multiset &operator=(std::initializer_list<value_type> il) noexcept {
        Mybase::operator=(il);
        return *this;
    }

    iterator insert(const value_type &val) noexcept { return this->_emplace_multi(val); }

    iterator insert(value_type &&val) noexcept { return this->_emplace_multi(std::move(val)); }

    template <typename... Args>
    WJR_INTRINSIC_INLINE iterator emplace(Args &&...args) noexcept {
        return this->_emplace_multi(std::forward<Args>(args)...);
    }
};

} // namespace wjr

#endif // WJR_CONTAINER_BTREE_SET_HPP__