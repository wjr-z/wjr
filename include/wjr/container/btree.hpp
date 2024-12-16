#ifndef WJR_CONTAINER_BTREE_HPP__
#define WJR_CONTAINER_BTREE_HPP__

/**
 * @file btree.hpp
 * @brief B+ tree implementation.
 *
 * @details The multiset/multimap/set/map adapter has not been implemented yet.
 * Only use when key is trivial. Otherwise, this maybe won't faster than
 * std::map because its search complexity is O((k - 1) * (log n / log k)).
 * Insertion and deletion may cause iterators to fail.
 *
 * @todo
 * 1. Erase a range with optimization.
 * 2. Construct with a range with optimization.
 * 3. Merge with optimization.
 * 4. Code size optimization.
 *
 * @version 0.2
 * @date 2024-12-15
 *
 */

#include <wjr/assert.hpp>
#include <wjr/compressed_pair.hpp>
#include <wjr/container/container_fn.hpp>
#include <wjr/container/list.hpp>
#include <wjr/memory/memory_pool.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr {

template <typename Traits>
struct btree_node;

template <typename Traits>
struct btree_inner_node;

template <typename Traits>
struct btree_list_base;

template <typename Traits>
struct btree_leaf_node;

namespace btree_detail {

/**
 * @brief I have temporarily abandoned the number of dynamic nodes for optimization purposes
 *
 */
inline constexpr size_t node_size = 8;

template <typename T>
WJR_INTRINSIC_INLINE void btree_assign(T &dst, const T &from) {
    dst = from;
}

template <size_t Min, size_t Max, typename Other>
WJR_INTRINSIC_INLINE void builtin_btree_copy(const Other *first, const Other *last,
                                             Other *dst) noexcept {
    constexpr auto Size = sizeof(Other);

    if constexpr (Min == 0) {
        if (WJR_UNLIKELY(first == last)) {
            return;
        }
    }

    const auto n = to_unsigned(last - first);
    WJR_ASSUME(n >= Min && n <= Max);

    if constexpr (Max > 4) {
        if (WJR_LIKELY(n >= 4)) {
            char x0[Size * 4];
            char x1[Size * 4];
            std::memcpy(x0, first, Size * 4);
            std::memcpy(x1, last - 4, Size * 4);
            std::memcpy(dst, x0, Size * 4);
            std::memcpy(dst + n - 4, x1, Size * 4);
            return;
        }
    }

    if constexpr (Max > 2) {
        if (n >= 2) {
            char x0[Size * 2];
            char x1[Size * 2];
            std::memcpy(x0, first, Size * 2);
            std::memcpy(x1, last - 2, Size * 2);
            std::memcpy(dst, x0, Size * 2);
            std::memcpy(dst + n - 2, x1, Size * 2);
            return;
        }
    }

    dst[0] = first[0];
    if constexpr (Max == 2) {
        if (n == 1)
            return;
        dst[1] = first[1];
    }
}

template <size_t Min, size_t Max, typename Other>
WJR_INTRINSIC_INLINE void copy(const Other *first, const Other *last, Other *dest) noexcept {
    builtin_btree_copy<Min, Max>(first, last, dest);
}

template <size_t Min, size_t Max, typename Other>
WJR_INTRINSIC_INLINE void copy_backward(const Other *first, const Other *last,
                                        Other *dest) noexcept {
    builtin_btree_copy<Min, Max>(first, last, dest - (last - first));
}

} // namespace btree_detail

template <typename Key, typename Value>
struct __btree_inline_traits {
    using key_type = Key;
    using mapped_type = Value;
    static constexpr bool is_map = true;
    using value_type = std::pair<const key_type, mapped_type>;

    WJR_INTRINSIC_INLINE static const key_type &get_key(const value_type &value) noexcept {
        return value.first;
    }
};

template <typename Key>
struct __btree_inline_traits<Key, void> {
    using key_type = Key;
    using mapped_type = void;
    static constexpr bool is_map = false;
    using value_type = key_type;

    WJR_INTRINSIC_INLINE static const key_type &get_key(const value_type &value) noexcept {
        return value;
    }
};

template <typename Key, typename Value, bool Multi, typename Compare = std::less<>>
struct btree_traits : __btree_inline_traits<Key, Value> {
private:
    using Mybase = __btree_inline_traits<Key, Value>;

public:
    using _Alty = typename std::allocator_traits<memory_pool<char>>::template rebind_alloc<char>;
    using _Alty_traits = std::allocator_traits<_Alty>;
    using storage_fn_type = container_fn<_Alty>;

    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;

    using key_type = typename Mybase::key_type;
    using mapped_type = typename Mybase::mapped_type;
    using value_type = typename Mybase::value_type;
    using key_compare = Compare;

    static constexpr bool is_map = Mybase::is_map;

    using node_type = btree_node<btree_traits>;
    using inner_node_type = btree_inner_node<btree_traits>;
    using list_base_type = btree_list_base<btree_traits>;
    using leaf_node_type = btree_leaf_node<btree_traits>;
    static constexpr bool multi = Multi;

    template <size_t Min, size_t Max, typename Other>
    WJR_INTRINSIC_INLINE static void copy(const Other *first, const Other *last,
                                          Other *dest) noexcept {
        return btree_detail::copy<Min, Max>(first, last, dest);
    }

    template <size_t Min, size_t Max, typename Other>
    WJR_INTRINSIC_INLINE static void copy_backward(const Other *first, const Other *last,
                                                   Other *dest) noexcept {
        return btree_detail::copy_backward<Min, Max>(first, last, dest);
    }
};

struct btree_node_constructor_t {};
inline constexpr btree_node_constructor_t btree_node_constructor{};

template <typename Traits>
struct btree_node {
private:
    struct __normal_node {
        int m_size;
        unsigned int m_pos;
    };

public:
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;
    using size_type = typename Traits::size_type;

    constexpr inner_node_type *as_inner() noexcept;
    constexpr const inner_node_type *as_inner() const noexcept;

    constexpr leaf_node_type *as_leaf() noexcept;
    constexpr const leaf_node_type *as_leaf() const noexcept;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(btree_node);

    constexpr btree_node(btree_node_constructor_t) noexcept : m_root_size(0), m_parent(nullptr) {}

    WJR_PURE constexpr int &size() noexcept { return m_node.m_size; }
    WJR_PURE constexpr const int &size() const noexcept { return m_node.m_size; }

    WJR_PURE constexpr unsigned int &pos() noexcept { return m_node.m_pos; }
    WJR_PURE constexpr const unsigned int &pos() const noexcept { return m_node.m_pos; }

    union {
        __normal_node m_node;
        size_type m_root_size;
    };

    btree_node *m_parent;
};

template <typename Traits>
struct btree_inner_node : btree_node<Traits> {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;

    alignas(16) const key_type *m_keys[btree_detail::node_size];
    alignas(16) btree_node<Traits> *m_sons[btree_detail::node_size + 1];
};

template <typename Traits>
struct btree_list_base : btree_node<Traits>, intrusive::list_node<btree_list_base<Traits>> {
private:
    using Mybase = btree_node<Traits>;

public:
    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(btree_list_base);

    constexpr btree_list_base(btree_node_constructor_t) noexcept : Mybase(btree_node_constructor) {}
};

template <typename Traits>
struct btree_leaf_node : btree_list_base<Traits> {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;

    const key_type &__get_key(unsigned int pos) const noexcept {
        return Traits::get_key(*m_values[pos]);
    }

    template <size_t Min, size_t Max>
    WJR_INTRINSIC_INLINE void __copy(unsigned int start, unsigned int end, unsigned int dst_start,
                                     btree_leaf_node *dst) const noexcept {
        Traits::template copy<Min, Max>(m_values + start, m_values + end,
                                        dst->m_values + dst_start);
    }

    template <size_t Min, size_t Max>
    WJR_INTRINSIC_INLINE void __copy_backward(unsigned int start, unsigned int end,
                                              unsigned int dst_end,
                                              btree_leaf_node *dst) const noexcept {
        Traits::template copy_backward<Min, Max>(m_values + start, m_values + end,
                                                 dst->m_values + dst_end);
    }

    WJR_INTRINSIC_INLINE void __assign(unsigned int idx, value_type *value) noexcept {
        btree_detail::btree_assign(m_values[idx], value);
    }

    alignas(16) value_type *m_values[btree_detail::node_size];
};

template <typename Traits>
constexpr typename btree_node<Traits>::inner_node_type *btree_node<Traits>::as_inner() noexcept {
    return static_cast<inner_node_type *>(this);
}

template <typename Traits>
constexpr const typename btree_node<Traits>::inner_node_type *
btree_node<Traits>::as_inner() const noexcept {
    return static_cast<const inner_node_type *>(this);
}

template <typename Traits>
constexpr typename btree_node<Traits>::leaf_node_type *btree_node<Traits>::as_leaf() noexcept {
    return static_cast<leaf_node_type *>(this);
}

template <typename Traits>
constexpr const typename btree_node<Traits>::leaf_node_type *
btree_node<Traits>::as_leaf() const noexcept {
    return static_cast<const leaf_node_type *>(this);
}

template <typename Traits>
class basic_btree;

template <typename Traits>
class btree_const_iterator {
    using node_type = typename Traits::node_type;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;
    using list_base_type = typename Traits::list_base_type;

    template <typename Other>
    friend class basic_btree;

    using list_node_type = intrusive::list_node<list_base_type>;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename Traits::value_type;
    using difference_type = typename Traits::difference_type;
    using pointer = const value_type *;
    using reference = const value_type &;

    btree_const_iterator() = default;
    btree_const_iterator(const btree_const_iterator &) = default;
    btree_const_iterator(btree_const_iterator &&) = default;
    btree_const_iterator &operator=(const btree_const_iterator &) = default;
    btree_const_iterator &operator=(btree_const_iterator &&) = default;
    ~btree_const_iterator() = default;

protected:
    btree_const_iterator(const list_node_type *list_node, unsigned int pos) noexcept
        : m_node(const_cast<list_node_type *>(list_node)), m_pos(pos) {}

    btree_const_iterator(const leaf_node_type *leaf, unsigned int pos) noexcept
        : btree_const_iterator(static_cast<const list_node_type *>(leaf), pos) {}

public:
    reference operator*() const noexcept { return *get_leaf()->m_values[m_pos]; }

    pointer operator->() const noexcept { return std::addressof(this->operator*()); }

    btree_const_iterator &operator++() noexcept {
        ++m_pos;
        return __adjust_next();
    }

    btree_const_iterator operator++(int) noexcept {
        btree_const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    btree_const_iterator &operator--() noexcept {
        if (m_pos != 0) {
            --m_pos;
        } else {
            m_node = m_node->prev;
            m_pos = __get_usize() - 1;
        }

        return *this;
    }

    btree_const_iterator operator--(int) noexcept {
        btree_const_iterator tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(const btree_const_iterator &other) const noexcept {
        return m_node == other.m_node && m_pos == other.m_pos;
    }

    bool operator!=(const btree_const_iterator &other) const noexcept { return !(*this == other); }

    WJR_PURE leaf_node_type *get_leaf() const noexcept {
        return static_cast<leaf_node_type *>(m_node);
    }

    WJR_PURE list_base_type *get_base() const noexcept {
        return static_cast<list_base_type *>(m_node);
    }

    WJR_PURE unsigned int pos() const noexcept { return m_pos; }

protected:
    WJR_PURE unsigned int __get_usize() const noexcept {
        return static_cast<unsigned int>(-get_base()->size());
    }

    WJR_INTRINSIC_INLINE btree_const_iterator &__adjust_next() noexcept {
        if (WJR_UNLIKELY(m_pos == __get_usize())) {
            m_node = m_node->next();
            m_pos = 0;
        }

        return *this;
    }

private:
    list_node_type *m_node = nullptr;
    unsigned int m_pos = 0;
};

template <typename Traits>
class btree_iterator : public btree_const_iterator<Traits> {
    using Mybase = btree_const_iterator<Traits>;
    using leaf_node_type = typename Traits::leaf_node_type;
    using list_base_type = typename Traits::list_base_type;

    template <typename Other>
    friend class basic_btree;

    using list_node_type = intrusive::list_node<list_base_type>;

public:
    using Mybase::Mybase;

    using iterator_category = typename Mybase::iterator_category;
    using value_type = typename Mybase::value_type;
    using difference_type = typename Traits::difference_type;
    using pointer = value_type *;
    using reference = value_type &;

    btree_iterator(const Mybase &other) noexcept : Mybase(other) {}

protected:
    btree_iterator(const list_node_type *list_node, unsigned int pos) noexcept
        : Mybase(list_node, pos) {}

    btree_iterator(const leaf_node_type *leaf, unsigned int pos) noexcept : Mybase(leaf, pos) {}

public:
    value_type &operator*() const noexcept { return const_cast<value_type &>(Mybase::operator*()); }

    value_type *operator->() const noexcept {
        return const_cast<value_type *>(Mybase::operator->());
    }

    btree_iterator &operator++() noexcept {
        Mybase::operator++();
        return *this;
    }

    btree_iterator operator++(int) noexcept {
        btree_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    btree_iterator &operator--() noexcept {
        Mybase::operator--();
        return *this;
    }

    btree_iterator operator--(int) noexcept {
        btree_iterator tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(const btree_iterator &other) const noexcept {
        return Mybase::operator==(other);
    }

    bool operator!=(const btree_iterator &other) const noexcept {
        return Mybase::operator!=(other);
    }

protected:
    WJR_INTRINSIC_INLINE btree_iterator &__adjust_next() noexcept {
        Mybase::__adjust_next();
        return *this;
    }
};

template <size_t N, typename Enable = void>
struct basic_btree_searcher_impl;

template <>
struct basic_btree_searcher_impl<8, void> {
    static constexpr size_t node_size = 8;

private:
    template <typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search_1_impl(unsigned int size,
                                                           const Compare &comp) noexcept {
        if (size == 1 || comp(1)) {
            if (comp(0)) {
                return 0;
            }
            return 1;
        }
        if (size <= 3) {
            if (size != 3 || comp(2)) {
                return 2;
            }
            return 3;
        }
        if (comp(3)) {
            if (comp(2)) {
                return 2;
            }
            return 3;
        }
        if (size <= 5) {
            if (size != 5 || comp(4)) {
                return 4;
            }
            return 5;
        }
        if (comp(5)) {
            if (comp(4)) {
                return 4;
            }
            return 5;
        }
        if (size == 6 || comp(6)) {
            return 6;
        }
        if (size == 7 || comp(7)) {
            return 7;
        }
        return 8;
    }

    template <typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search_4_impl(unsigned int size,
                                                           const Compare &comp) noexcept {
        if (comp(1)) {
            if (comp(0)) {
                return 0;
            }
            return 1;
        }
        if (comp(3)) {
            if (comp(2)) {
                return 2;
            }
            return 3;
        }
        if (size <= 5) {
            if (size != 5 || comp(4)) {
                return 4;
            }
            return 5;
        }
        if (comp(5)) {
            if (comp(4)) {
                return 4;
            }
            return 5;
        }
        if (size == 6 || comp(6)) {
            return 6;
        }
        if (size == 7 || comp(7)) {
            return 7;
        }
        return 8;
    }

public:
    template <size_t Min, typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search(unsigned int size,
                                                    const Compare &comp) noexcept {
        static_assert(Min == 1 || Min == 4);

        WJR_ASSERT_ASSUME(size >= Min);
        WJR_ASSERT_ASSUME(size <= node_size);

        if constexpr (Min == 1) {
            return search_1_impl(size, comp);
        } else {
            return search_4_impl(size, comp);
        }
    }
};

template <typename Traits>
class basic_btree {
    using _Alty = typename Traits::_Alty;
    using _Alty_traits = typename Traits::_Alty_traits;
    using storage_fn_type = container_fn<_Alty>;

    friend class container_fn<_Alty>;

    static constexpr size_t node_size = 8;
    static constexpr size_t floor_half = node_size / 2;
    static constexpr size_t ceil_half = node_size - floor_half;
    static constexpr size_t max_moved_elements = (ceil_half + 1) / 2;
    static constexpr bool multi = Traits::multi;

    using node_type = typename Traits::node_type;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;
    using list_base_type = typename Traits::list_base_type;
    using list_node_type = intrusive::list_node<list_base_type>;

public:
    using key_type = typename Traits::key_type;
    using mapped_type = typename Traits::mapped_type;
    using value_type = typename Traits::value_type;
    using key_compare = typename Traits::key_compare;
    using allocator_type = _Alty;
    using size_type = typename Traits::size_type;
    using difference_type = typename Traits::difference_type;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using iterator = btree_iterator<Traits>;
    using const_iterator = btree_const_iterator<Traits>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert(node_size >= 3, "node_size must be greater than or equal to 3.");
    static_assert(node_size <= 255, "node_size must be less than or equal to 255.");

    basic_btree() noexcept : basic_btree(key_compare()) {}

    explicit basic_btree(const key_compare &comp) noexcept : m_pair(comp, btree_node_constructor) {}

    // not implemented currently
    basic_btree(const basic_btree &other) noexcept(
        std::is_nothrow_copy_constructible_v<key_compare>)
        : m_pair(other.key_comp(), btree_node_constructor) {
        __copy_tree(other);
    }

    basic_btree(basic_btree &&other) noexcept(std::is_nothrow_move_constructible_v<key_compare>)
        : m_pair(std::move(other.key_comp()), btree_node_constructor) {
        __move_tree(std::move(other));
    }

    basic_btree &operator=(const basic_btree &other) noexcept {
        if (WJR_UNLIKELY(this == std::addressof(other))) {
            return *this;
        }

        clear();
        __copy_tree(other);
        return *this;
    }

    basic_btree &operator=(basic_btree &&other) noexcept(
        noexcept(storage_fn_type::move_assign(*this, std::move(other)))) {
        WJR_ASSERT(this != std::addressof(other));
        storage_fn_type::move_assign(*this, std::move(other));
        return *this;
    }

    basic_btree &operator=(std::initializer_list<value_type> il) noexcept {
        clear();
        for (auto &item : il) {
            emplace(item);
        }
        return *this;
    }

    ~basic_btree() noexcept { __destroy_and_deallocate(); }

    constexpr key_compare &key_comp() noexcept { return m_pair.first(); }
    constexpr const key_compare &key_comp() const noexcept { return m_pair.first(); }

    iterator begin() noexcept { return iterator(__get_sentry()->next(), 0); }
    const_iterator begin() const noexcept { return const_iterator(__get_sentry()->next(), 0); }
    const_iterator cbegin() const noexcept { return const_iterator(__get_sentry()->next(), 0); }

    iterator end() noexcept { return iterator(__get_sentry(), 0); }
    const_iterator end() const noexcept { return const_iterator(__get_sentry(), 0); }
    const_iterator cend() const noexcept { return const_iterator(__get_sentry(), 0); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    void clear() {
        __destroy_and_deallocate();
        __get_root() = nullptr;
        __get_size() = 0;
    }

protected:
    WJR_INTRINSIC_INLINE static inner_node_type *__create_inner_node() noexcept {
        _Alty al;
        auto *const node = reinterpret_cast<inner_node_type *>(
            _Alty_traits::allocate(al, sizeof(inner_node_type)));
        uninitialized_construct_using_allocator(node, al, default_construct);
        return node;
    }

    WJR_INTRINSIC_INLINE static leaf_node_type *__create_leaf_node() noexcept {
        _Alty al;
        auto *const node =
            reinterpret_cast<leaf_node_type *>(_Alty_traits::allocate(al, sizeof(leaf_node_type)));
        uninitialized_construct_using_allocator(node, al, default_construct);
        return node;
    }

    template <typename... Args>
    WJR_INTRINSIC_INLINE static value_type *__create_node(Args &&...args) noexcept {
        _Alty al;
        auto *const node =
            reinterpret_cast<value_type *>(_Alty_traits::allocate(al, sizeof(value_type)));
        uninitialized_construct_using_allocator(node, al, std::forward<Args>(args)...);
        return node;
    }

    WJR_INTRINSIC_INLINE static void __drop_inner_node(inner_node_type *node) noexcept {
        _Alty al;
        destroy_at_using_allocator(node, al);
        _Alty_traits::deallocate(al, reinterpret_cast<char *>(node), sizeof(inner_node_type));
    }

    WJR_INTRINSIC_INLINE static void __drop_leaf_node(leaf_node_type *node) noexcept {
        _Alty al;
        destroy_at_using_allocator(node, al);
        _Alty_traits::deallocate(al, reinterpret_cast<char *>(node), sizeof(leaf_node_type));
    }

    WJR_INTRINSIC_INLINE static void __drop_node(value_type *node) noexcept {
        _Alty al;
        destroy_at_using_allocator(std::addressof(node), al);
        _Alty_traits::deallocate(al, reinterpret_cast<char *>(node), sizeof(value_type));
    }

    WJR_INTRINSIC_INLINE const_iterator __get_insert_multi_pos(const key_type &key) const noexcept {
        return __search<true, false>(key);
    }

    template <typename... Args>
    iterator __emplace_multi(Args &&...args) noexcept {
        ++__get_size();
        auto *const xval = __create_node(std::forward<Args>(args)...);
        const auto iter = __get_insert_multi_pos(Traits::get_key(*xval));
        return __insert_iter(iter, xval);
    }

    template <typename... Args>
    std::pair<iterator, bool> __emplace_unique(Args &&...args) noexcept {
        auto *const xval = __create_node(std::forward<Args>(args)...);
        const auto &key = Traits::get_key(*xval);
        const const_iterator iter = __get_insert_multi_pos(key);
        const auto pos = iter.pos();
        if (WJR_LIKELY(pos == 0 || key_comp()(iter.get_leaf()->__get_key(pos - 1), key))) {
            ++__get_size();
            return {__insert_iter(iter, xval), true};
        }

        __drop_node(xval);
        return {iterator(iter).__adjust_next(), false};
    }

    iterator __insert_multi(const value_type &val) noexcept { return __emplace_multi(val); }
    iterator __insert_multi(value_type &&val) noexcept { return __emplace_multi(std::move(val)); }

    std::pair<iterator, bool> __insert_unique(const value_type &val) noexcept {
        return __emplace_unique(val);
    }

    std::pair<iterator, bool> __insert_unique(value_type &&val) noexcept {
        return __emplace_unique(std::move(val));
    }

    template <typename... Args>
    std::pair<iterator, bool> __try_emplace_unique(const key_type &key, Args &&...args) {
        const const_iterator iter = __get_insert_multi_pos(key);
        const auto pos = iter.pos();
        if (WJR_LIKELY(pos == 0 || key_comp()(iter.get_leaf()->__get_key(pos - 1), key))) {
            ++__get_size();
            auto *const xval = __create_node(std::piecewise_construct, std::forward_as_tuple(key),
                                             std::forward_as_tuple(std::forward<Args>(args)...));
            return {__insert_iter(iter, xval), true};
        }

        return {iterator(iter).__adjust_next(), false};
    }

    template <typename... Args>
    std::pair<iterator, bool> __try_emplace_unique(key_type &&key, Args &&...args) {
        const const_iterator iter = __get_insert_multi_pos(key);
        const auto pos = iter.pos();
        if (WJR_LIKELY(pos == 0 || key_comp()(iter.get_leaf()->__get_key(pos - 1), key))) {
            ++__get_size();
            auto *const xval =
                __create_node(std::piecewise_construct, std::forward_as_tuple(std::move(key)),
                              std::forward_as_tuple(std::forward<Args>(args)...));
            return {__insert_iter(iter, xval), true};
        }

        return {iterator(iter).__adjust_next(), false};
    }

public:
    iterator lower_bound(const key_type &key) noexcept { return __search<false, true>(key); }
    const_iterator lower_bound(const key_type &key) const noexcept {
        return __search<false, true>(key);
    }

    iterator upper_bound(const key_type &key) noexcept { return __search<true, true>(key); }
    const_iterator upper_bound(const key_type &key) const noexcept {
        return __search<true, true>(key);
    }

    iterator erase(const_iterator iter) noexcept {
        WJR_ASSERT_L2(iter != cend());

        --__get_size();
        return __erase_iter(iter);
    }

    WJR_PURE bool empty() const noexcept { return __get_root() == nullptr; }
    WJR_PURE size_type size() const noexcept { return __get_size(); }

private:
    void __move_tree(basic_btree &&other) noexcept {
        node_type *&root = other.__get_root();
        if (WJR_UNLIKELY(root == nullptr)) {
            __get_sentry()->init_self();
            return;
        }

        __get_root() = std::exchange(root, nullptr);
        __get_size() = std::exchange(other.__get_size(), 0);
        replace(other.__get_sentry(), __get_sentry());
        other.__get_sentry()->init_self();
    }

    std::pair<const key_type *, node_type *> __rec_copy_tree(const node_type *current) noexcept {
        int cur_size = current->size();

        if (cur_size < 0) {
            auto *const leaf = current->as_leaf();
            const unsigned int cur_usize = -cur_size;

            auto *const this_leaf = __create_leaf_node();
            this_leaf->size() = cur_size;
            for (unsigned i = 0; i < cur_usize; ++i) {
                this_leaf->__assign(i, __create_node(*leaf->m_values[i]));
            }

            __get_sentry()->push_back(this_leaf);
            return {std::addressof(this_leaf->__get_key(0)), this_leaf};
        }

        auto *const this_inner = __create_inner_node();
        this_inner->size() = cur_size;
        const key_type *Key;
        const unsigned int cur_usize = cur_size;

        for (unsigned i = 0; i <= cur_usize; ++i) {
            auto [key, son] = __rec_copy_tree(current->as_inner()->m_sons[i]);
            son->m_parent = this_inner;
            son->pos() = i;

            if (i != 0) {
                this_inner->m_keys[i - 1] = key;
            } else {
                Key = key;
            }

            this_inner->m_sons[i] = son;
        }

        return {Key, this_inner};
    }

    void __copy_tree(const basic_btree &other) noexcept {
        const node_type *current = other.__get_root();
        __get_sentry()->init_self();
        if (WJR_UNLIKELY(current == nullptr)) {
            return;
        }

        auto [key, root] = __rec_copy_tree(current);
        __get_root() = root;
        __get_size() = other.__get_size();
        root->m_parent = nullptr;
    }

    // member function for container_fn (START)

    void __destroy_and_deallocate() noexcept {
        node_type *current = __get_root();

        if (WJR_UNLIKELY(current == nullptr)) {
            return;
        }

        int cur_size = current->size();

        // If root is leaf
        if (cur_size < 0) {
            auto *const leaf = current->as_leaf();
            const unsigned int cur_usize = -cur_size;

            for (unsigned int i = 0; i < cur_usize; ++i) {
                __drop_node(leaf->m_values[i]);
            }

            __drop_leaf_node(leaf);
            return;
        }

        // skip to the leftmost leaf
        current = begin().get_leaf();
        cur_size = -current->size();

        // cache of parent and parent's size
        node_type *parent = current->m_parent;
        unsigned int par_size = parent->size();

        // cache of `current' node's position in parent
        unsigned int pos = 0;

        do {
            auto *const leaf = current->as_leaf();
            const unsigned int cur_usize = cur_size;

            for (unsigned int i = 0; i < cur_usize; ++i) {
                __drop_node(leaf->m_values[i]);
            }

            list_node_type *next = leaf->next()->self();
            __drop_leaf_node(leaf);

            // if `current' is the last child of parent
            if (WJR_UNLIKELY(pos++ == par_size)) {
                do {
                    current = parent;
                    parent = current->m_parent;
                    pos = current->pos();
                    __drop_inner_node(current->as_inner());

                    // if `current' is the rightmost leaf
                    if (parent == nullptr) {
                        return;
                    }

                    // if `current' is the last child of parent
                } while (pos == (unsigned int)parent->size());

                // update cache of parent and parent's size
                parent = static_cast<leaf_node_type *>(next)->m_parent;
                par_size = parent->size();
                pos = 0;
            }

            WJR_ASSERT(next != __get_sentry());

            current = static_cast<leaf_node_type *>(next);
            cur_size = -current->size();
        } while (true);
    }

    void __take_storage(basic_btree &&other) noexcept {
        key_comp() = std::move(other.key_comp());
        __move_tree(std::move(other));
    }

    // member function for container_fn (END)

    WJR_NODISCARD WJR_NOINLINE WJR_HOT WJR_FLATTEN iterator
    __insert_iter(const_iterator iter, value_type *xval) noexcept {
        leaf_node_type *leaf;

        // empty
        if (WJR_UNLIKELY(empty())) {
            leaf = __create_leaf_node();
            __get_root() = leaf;

            leaf->size() = -1;
            leaf->m_parent = nullptr;
            leaf->__assign(0, xval);
            __get_sentry()->push_front(leaf);
            return iterator(leaf, 0);
        }

        leaf = iter.get_leaf();

        iterator result;
        node_type *inst;

        do {
            const unsigned int pos = iter.pos();
            const unsigned int cur_size = static_cast<unsigned int>(-leaf->size());

            // non-full leaf
            if (WJR_LIKELY(cur_size != node_size)) {
                WJR_ASSERT_ASSUME(pos <= cur_size);
                leaf->template __copy_backward<0, node_size - 1>(pos, cur_size, cur_size + 1, leaf);

                leaf->size() = -(cur_size + 1);
                leaf->__assign(pos, xval);
                return iterator(leaf, pos);
            }

            auto *const inst_leaf = __create_leaf_node();
            leaf->push_front(inst_leaf);
            leaf->size() = -(int)(floor_half + 1);
            inst_leaf->size() = -(int)(node_size - floor_half);

            if (pos <= floor_half) {
                leaf->template __copy<ceil_half, ceil_half>(floor_half, node_size, 0, inst_leaf);
                leaf->template __copy_backward<0, floor_half>(pos, floor_half, floor_half + 1,
                                                              leaf);
                leaf->__assign(pos, xval);
                result = iterator(leaf, pos);
            } else {
                // pos in inst_leaf
                const unsigned int rpos = pos - floor_half - 1;
                leaf->template __copy<0, ceil_half - 1>(floor_half + 1, pos, 0, inst_leaf);
                leaf->template __copy<0, ceil_half - 1>(pos, node_size, rpos + 1, inst_leaf);
                inst_leaf->__assign(rpos, xval);
                result = iterator(inst_leaf, rpos);
            }

            inst = inst_leaf;
        } while (false);

        {
            node_type *current = leaf;

            node_type *parent = current->m_parent;
            const key_type *key = std::addressof(inst->as_leaf()->__get_key(0));

            while (parent != nullptr) {
                inst->m_parent = parent;
                const unsigned int pos = current->pos() + 1;
                current = parent;
                auto *const inner = current->as_inner();

                unsigned int cur_size = inner->size() + 1;
                const key_type **const keys = inner->m_keys;
                node_type **const sons = inner->m_sons;

                // non-full inner
                if (WJR_LIKELY(cur_size != node_size + 1)) {
                    Traits::template copy_backward<0, node_size - 1>(
                        keys + pos - 1, keys + cur_size - 1, keys + cur_size);
                    Traits::template copy_backward<0, node_size - 1>(sons + pos, sons + cur_size,
                                                                     sons + cur_size + 1);

                    inner->size() = cur_size;
                    keys[pos - 1] = key;
                    sons[pos] = inst;

                    inst->pos() = pos;
                    for (unsigned int i = pos + 1; i <= cur_size; ++i) {
                        sons[i]->pos() = i;
                    }

                    return result;
                }

                parent = inner->m_parent;

                auto *const tmp_inst = __create_inner_node();

                inner->size() = (int)(ceil_half);
                tmp_inst->size() = (int)(floor_half);

                const key_type *next_key;

                if (pos <= ceil_half) {
                    next_key = keys[ceil_half - 1];

                    Traits::template copy<floor_half, floor_half>(
                        keys + ceil_half, keys + node_size, tmp_inst->m_keys);
                    Traits::template copy<floor_half + 1, floor_half + 1>(
                        sons + ceil_half, sons + node_size + 1, tmp_inst->m_sons);
                    Traits::template copy_backward<0, ceil_half>(
                        keys + pos - 1, keys + ceil_half - 1, keys + ceil_half);
                    Traits::template copy_backward<0, ceil_half>(sons + pos, sons + ceil_half,
                                                                 sons + ceil_half + 1);

                    keys[pos - 1] = key;
                    sons[pos] = inst;

                    inst->pos() = pos;
                    for (unsigned int i = pos + 1; i <= ceil_half; ++i) {
                        sons[i]->pos() = i;
                    }
                } else {
                    if (pos == ceil_half + 1) {
                        next_key = key;

                        Traits::template copy<floor_half, floor_half>(
                            keys + ceil_half, keys + node_size, tmp_inst->m_keys);
                        Traits::template copy<floor_half, floor_half>(
                            sons + ceil_half + 1, sons + node_size + 1, tmp_inst->m_sons + 1);

                        tmp_inst->m_sons[0] = inst;
                    } else {
                        next_key = keys[ceil_half];

                        Traits::template copy<0, floor_half - 1>(keys + ceil_half + 1,
                                                                 keys + pos - 1, tmp_inst->m_keys);
                        Traits::template copy<1, floor_half>(sons + ceil_half + 1, sons + pos,
                                                             tmp_inst->m_sons);

                        const unsigned int rpos = pos - ceil_half - 1;

                        Traits::template copy<0, floor_half - 1>(keys + pos - 1, keys + node_size,
                                                                 tmp_inst->m_keys + rpos);
                        Traits::template copy<0, floor_half - 1>(sons + pos, sons + node_size + 1,
                                                                 tmp_inst->m_sons + rpos + 1);

                        tmp_inst->m_keys[rpos - 1] = key;
                        tmp_inst->m_sons[rpos] = inst;
                    }
                }

                for (unsigned int i = 0; i <= floor_half; ++i) {
                    tmp_inst->m_sons[i]->m_parent = tmp_inst;
                    tmp_inst->m_sons[i]->pos() = i;
                }

                key = next_key;
                inst = tmp_inst;
            }

            auto *const new_root = __create_inner_node();
            new_root->size() = 1;
            new_root->m_parent = nullptr;
            new_root->m_keys[0] = key;
            new_root->m_sons[0] = current;
            new_root->m_sons[1] = inst;
            current->pos() = 0;
            inst->pos() = 1;

            current->m_parent = new_root;
            inst->m_parent = new_root;

            __get_root() = new_root;
        }

        return result;
    }

    template <bool Upper, bool Adjust>
    WJR_NOINLINE WJR_HOT WJR_FLATTEN const_iterator __search(const key_type &key) const noexcept {
        const node_type *current = __get_root();
        if (WJR_UNLIKELY(current == nullptr)) {
            return cend();
        }

        int cur_size = current->size();
        const auto &comp = key_comp();
        unsigned int pos;

        // root search
        if (WJR_UNLIKELY(cur_size < 0)) {
            const unsigned int cur_usize = static_cast<unsigned int>(-cur_size);
            pos = __search<Upper, 1>(current->as_leaf(), cur_usize, key, comp);

            if constexpr (Adjust) {
                if (WJR_UNLIKELY(pos == cur_usize)) {
                    return cend();
                }
            }

            return const_iterator(current->as_leaf(), pos);
        }

        pos = __search<Upper, 1>(current->as_inner(), cur_size, key, comp);
        current = current->as_inner()->m_sons[pos];
        cur_size = current->size();

        if (cur_size >= 0) {
            do {
                pos = __search<Upper, floor_half>(current->as_inner(), cur_size, key, comp);
                current = current->as_inner()->m_sons[pos];
                cur_size = current->size();
            } while (cur_size >= 0);
        }

        const unsigned int cur_usize = static_cast<unsigned int>(-cur_size);
        pos = __search<Upper, floor_half>(current->as_leaf(), cur_usize, key, comp);

        if constexpr (Adjust) {
            if (WJR_UNLIKELY(pos == cur_usize)) {
                return const_iterator(current->as_leaf()->next(), 0);
            }
        }

        return const_iterator(current->as_leaf(), pos);
    }

    template <bool Upper>
    WJR_INTRINSIC_INLINE static bool __compare(const key_type &node, const key_type &key,
                                               const key_compare &comp) noexcept {
        if constexpr (Upper) {
            return comp(key, node);
        } else {
            return !comp(node, key);
        }
    }

    template <bool Upper, size_t Min>
    WJR_INTRINSIC_INLINE static unsigned int __search(const inner_node_type *current,
                                                      unsigned int size, const key_type &key,
                                                      const key_compare &comp) noexcept {
        return basic_btree_searcher_impl<node_size>::template search<Min>(
            size, [current, &key, &comp](unsigned int pos) {
                return __compare<Upper>(*current->m_keys[pos], key, comp);
            });
    }

    template <bool Upper, size_t Min>
    WJR_INTRINSIC_INLINE static unsigned int __search(const leaf_node_type *current,
                                                      unsigned int size, const key_type &key,
                                                      const key_compare &comp) noexcept {
        return basic_btree_searcher_impl<node_size>::template search<Min>(
            size, [current, &key, &comp](unsigned int pos) {
                return __compare<Upper>(current->__get_key(pos), key, comp);
            });
    }

    template <typename T>
    WJR_INTRINSIC_INLINE static unsigned int
    __init_remove_rotate(const inner_node_type *parent, unsigned int pos, unsigned int par_size,
                         T *&lhs, T *&rhs) noexcept {
        unsigned int size;

        if (pos != 0) {
            auto *const tmp = static_cast<T *>(parent->m_sons[pos - 1]);
            unsigned int tmp_size;

            if constexpr (std::is_same_v<T, leaf_node_type>) {
                tmp_size = -tmp->size();
            } else {
                tmp_size = tmp->size();
            }

            WJR_ASSERT_ASSUME(tmp_size >= floor_half);
            lhs = tmp;
            size = tmp_size;
        } else {
            auto *const tmp = static_cast<T *>(parent->m_sons[pos + 1]);
            lhs = nullptr;
            rhs = tmp;
            if constexpr (std::is_same_v<T, leaf_node_type>) {
                return -tmp->size();
            } else {
                return tmp->size();
            }
        }

        if (pos != par_size) {
            auto *const tmp = static_cast<T *>(parent->m_sons[pos + 1]);
            unsigned int tmp_size;

            if constexpr (std::is_same_v<T, leaf_node_type>) {
                tmp_size = -tmp->size();
            } else {
                tmp_size = tmp->size();
            }

            WJR_ASSERT_ASSUME(tmp_size >= floor_half);

            if (tmp_size > size) {
                lhs = nullptr;
                rhs = tmp;
                return tmp_size;
            }
        }

        WJR_ASSERT_ASSUME_L2(lhs != nullptr);
        return size;
    }

    /**
     * @todo use <Min, Max> to optimize
     *
     */
    WJR_INTRINSIC_INLINE void __rec_erase_iter(node_type *parent, unsigned int par_pos,
                                               unsigned int par_size) noexcept {
        constexpr unsigned int merge_size = floor_half * 2;

        unsigned int pos;
        unsigned int cur_size;
        node_type *current;

        current = parent;
        pos = par_pos;
        cur_size = par_size;
        parent = current->m_parent;

        while (parent != nullptr) {
            WJR_ASSERT_ASSUME(pos > 0);

            const auto inner = current->as_inner();

            key_type **const keys = inner->m_keys;
            node_type **const sons = inner->m_sons;

            if (cur_size > floor_half) {
                Traits::template copy<0, node_size - 1>(keys + pos, keys + cur_size,
                                                        keys + pos - 1);
                Traits::template copy<0, node_size - 1>(sons + pos + 1, sons + cur_size + 1,
                                                        sons + pos);

                for (unsigned int i = pos; i < cur_size; ++i) {
                    sons[i]->pos() = i;
                }

                inner->size() = cur_size - 1;
                return;
            }

            WJR_ASSERT_ASSUME(cur_size == floor_half);

            const auto par_inner = parent->as_inner();
            par_pos = inner->pos();
            par_size = par_inner->size();
            inner_node_type *lhs;
            inner_node_type *rhs;

            unsigned int next_size = __init_remove_rotate(par_inner, par_pos, par_size, lhs, rhs);

            do {
                if (lhs != nullptr) {
                    rhs = inner;

                    if (next_size == floor_half) {
                        Traits::template copy<0, floor_half - 1>(keys, keys + pos - 1,
                                                                 lhs->m_keys + floor_half + 1);
                        Traits::template copy<1, floor_half>(sons, sons + pos,
                                                             lhs->m_sons + floor_half + 1);
                        Traits::template copy<0, floor_half - 1>(keys + pos, keys + floor_half,
                                                                 lhs->m_keys + floor_half + pos);
                        Traits::template copy<0, floor_half - 1>(
                            sons + pos + 1, sons + floor_half + 1,
                            lhs->m_sons + floor_half + pos + 1);

                        for (unsigned int i = floor_half; i <= merge_size; ++i) {
                            lhs->m_sons[i]->m_parent = lhs;
                            lhs->m_sons[i]->pos() = i;
                        }

                        lhs->m_keys[floor_half] = par_inner->m_keys[par_pos - 1];
                        break;
                    }

                    const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                    key_type *key = lhs->m_keys[next_size - moved_elements];

                    if (moved_elements != 1) {
                        Traits::template copy_backward<0, floor_half - 1>(
                            keys + pos, keys + floor_half, keys + floor_half + moved_elements - 1);
                        Traits::template copy_backward<0, floor_half - 1>(
                            sons + pos + 1, sons + floor_half + 1,
                            sons + floor_half + moved_elements);
                        for (unsigned int i = pos + moved_elements; i < floor_half + moved_elements;
                             ++i) {
                            sons[i]->pos() = i;
                        }
                    }

                    Traits::template copy_backward<0, floor_half - 1>(
                        keys, keys + pos - 1, keys + pos + moved_elements - 1);
                    Traits::template copy_backward<1, floor_half>(sons, sons + pos,
                                                                  sons + pos + moved_elements);
                    Traits::template copy<0, max_moved_elements - 1>(lhs->m_keys + next_size -
                                                                         moved_elements + 1,
                                                                     lhs->m_keys + next_size, keys);
                    Traits::template copy<1, max_moved_elements>(lhs->m_sons + next_size -
                                                                     moved_elements + 1,
                                                                 lhs->m_sons + next_size + 1, sons);

                    keys[moved_elements - 1] = par_inner->m_keys[par_pos - 1];
                    par_inner->m_keys[par_pos - 1] = key;

                    for (unsigned int i = 0; i < moved_elements; ++i) {
                        sons[i]->m_parent = inner;
                        sons[i]->pos() = i;
                    }

                    for (unsigned int i = moved_elements; i < pos + moved_elements; ++i) {
                        sons[i]->pos() = i;
                    }

                    lhs->size() = next_size - moved_elements;
                    inner->size() = floor_half + moved_elements - 1;
                } else {
                    WJR_ASSERT_ASSUME(rhs != nullptr);

                    lhs = inner;

                    if (next_size == floor_half) {
                        Traits::template copy<0, floor_half - 1>(keys + pos, keys + floor_half,
                                                                 keys + pos - 1);
                        Traits::template copy<0, floor_half - 1>(sons + pos + 1,
                                                                 sons + floor_half + 1, sons + pos);
                        Traits::template copy<0, floor_half>(rhs->m_keys, rhs->m_keys + floor_half,
                                                             keys + floor_half);
                        Traits::template copy<0, floor_half + 1>(
                            rhs->m_sons, rhs->m_sons + floor_half + 1, sons + floor_half);

                        for (unsigned int i = pos; i < floor_half; ++i) {
                            inner->m_sons[i]->pos() = i;
                        }

                        for (unsigned int i = floor_half; i <= merge_size; ++i) {
                            inner->m_sons[i]->m_parent = inner;
                            inner->m_sons[i]->pos() = i;
                        }

                        lhs->m_keys[floor_half - 1] = par_inner->m_keys[par_pos];
                        ++par_pos;
                        break;
                    }

                    const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                    key_type *key = rhs->m_keys[moved_elements - 1];

                    Traits::template copy<0, floor_half - 1>(keys + pos, keys + floor_half,
                                                             keys + pos - 1);
                    Traits::template copy<0, floor_half - 1>(sons + pos + 1, sons + floor_half + 1,
                                                             sons + pos);
                    Traits::template copy<0, max_moved_elements - 1>(
                        rhs->m_keys, rhs->m_keys + moved_elements - 1, keys + floor_half);
                    Traits::template copy<1, max_moved_elements>(
                        rhs->m_sons, rhs->m_sons + moved_elements, sons + floor_half);
                    Traits::template copy<node_size - max_moved_elements, node_size - 1>(
                        rhs->m_keys + moved_elements, rhs->m_keys + next_size, rhs->m_keys);
                    Traits::template copy<node_size - max_moved_elements + 1, node_size>(
                        rhs->m_sons + moved_elements, rhs->m_sons + next_size + 1, rhs->m_sons);

                    keys[floor_half - 1] = par_inner->m_keys[par_pos];
                    par_inner->m_keys[par_pos] = key;

                    for (unsigned int i = pos; i < floor_half; ++i) {
                        sons[i]->pos() = i;
                    }

                    for (unsigned int i = floor_half; i < floor_half + moved_elements; ++i) {
                        sons[i]->m_parent = inner;
                        sons[i]->pos() = i;
                    }

                    for (unsigned int i = 0; i <= next_size - moved_elements; ++i) {
                        rhs->m_sons[i]->pos() = i;
                    }

                    rhs->size() = next_size - moved_elements;
                    inner->size() = floor_half + moved_elements - 1;
                }

                return;
            } while (false);

            lhs->size() = merge_size;
            __drop_inner_node(rhs);

            pos = par_pos;
            cur_size = par_size;
            current = parent;
            parent = current->m_parent;
        }

        const auto inner = current->as_inner();

        if (cur_size == 1) {
            __drop_inner_node(inner);
            node_type *root = inner->m_sons[0];
            __get_root() = root;
            root->m_parent = nullptr;
            return;
        }

        Traits::template copy<0, node_size>(inner->m_keys + pos, inner->m_keys + cur_size,
                                            inner->m_keys + pos - 1);
        Traits::template copy<0, node_size>(inner->m_sons + pos + 1, inner->m_sons + cur_size + 1,
                                            inner->m_sons + pos);

        for (unsigned int i = pos; i < cur_size; ++i) {
            inner->m_sons[i]->pos() = i;
        }

        inner->size() = cur_size - 1;
    }

    WJR_NODISCARD WJR_NOINLINE WJR_HOT WJR_FLATTEN iterator
    __erase_iter(const_iterator iter) noexcept {
        constexpr unsigned int merge_size = floor_half * 2;

        leaf_node_type *leaf = iter.get_leaf();
        unsigned int pos = iter.pos();
        unsigned int cur_size = -leaf->size();
        node_type *parent = leaf->m_parent;

        __drop_node(leaf->m_values[pos]);

        if (WJR_LIKELY(cur_size > floor_half)) {
            leaf->template __copy<0, node_size - 1>(pos + 1, cur_size, pos, leaf);
            leaf->size() = -(cur_size - 1);

            // first key in leaf is changed
            if (pos == 0 && parent != nullptr) {
                node_type *current = leaf;
                unsigned int tmp_pos;

                do {
                    tmp_pos = current->pos();
                    current = parent;
                    parent = current->m_parent;

                    if (tmp_pos != 0) {
                        current->as_inner()->m_keys[tmp_pos - 1] =
                            std::addressof(leaf->__get_key(0));
                        break;
                    }

                } while (parent != nullptr);
            }

            return iterator(leaf, pos).__adjust_next();
        }

        if (parent == nullptr) {
            if (cur_size == 1) {
                __drop_leaf_node(leaf);
                __get_root() = nullptr;
                __get_sentry()->init_self();
                return cend();
            }

            leaf->template __copy<0, node_size - 1>(pos + 1, cur_size, pos, leaf);
            leaf->size() = -(cur_size - 1);
            return iterator(leaf, pos).__adjust_next();
        }

        WJR_ASSERT_ASSUME(cur_size == floor_half);

        const auto inner = parent->as_inner();
        unsigned int par_pos = leaf->pos();
        cur_size = inner->size();
        leaf_node_type *lhs;
        leaf_node_type *rhs;

        const unsigned int next_size = __init_remove_rotate(inner, par_pos, cur_size, lhs, rhs);

        do {
            if (lhs != nullptr) {
                rhs = leaf;

                if (next_size == floor_half) {
                    leaf->template __copy<0, floor_half>(0, pos, floor_half, lhs);
                    leaf->template __copy<0, floor_half>(pos + 1, floor_half, pos + floor_half,
                                                         lhs);

                    leaf = lhs;
                    pos += floor_half;
                    break;
                }

                const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                if (moved_elements != 1) {
                    leaf->template __copy_backward<0, floor_half>(
                        pos + 1, floor_half, floor_half + moved_elements - 1, leaf);
                }

                leaf->template __copy_backward<0, floor_half>(0, pos, pos + moved_elements, leaf);
                lhs->template __copy<1, max_moved_elements>(next_size - moved_elements, next_size,
                                                            0, leaf);

                lhs->size() = -(next_size - moved_elements);
                leaf->size() = -(floor_half + moved_elements - 1);

                pos += moved_elements;
            } else {
                WJR_ASSERT_ASSUME(rhs != nullptr);

                lhs = leaf;

                leaf->template __copy<0, floor_half>(pos + 1, floor_half, pos, leaf);

                // merge rhs to leaf, and pos of iter is zero, then
                // need to update key in parent
                if (pos == 0) {
                    node_type *current = leaf;

                    unsigned int tmp_pos;
                    node_type *tmp_parent = parent;

                    do {
                        tmp_pos = current->pos();
                        current = tmp_parent;
                        tmp_parent = current->m_parent;
                    } while (tmp_pos == 0 && tmp_parent != nullptr);

                    if (tmp_pos != 0) {
                        current->as_inner()->m_keys[tmp_pos - 1] =
                            std::addressof(leaf->__get_key(0));
                    }
                }

                if (next_size == floor_half) {
                    rhs->template __copy<0, floor_half>(0, floor_half, floor_half - 1, leaf);

                    ++par_pos;
                    break;
                }

                const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                rhs->template __copy<1, max_moved_elements>(0, moved_elements, floor_half - 1,
                                                            leaf);
                rhs->template __copy<1, node_size - max_moved_elements>(moved_elements, next_size,
                                                                        0, rhs);

                rhs->size() = -(next_size - moved_elements);
                leaf->size() = -(floor_half + moved_elements - 1);
            }

            node_type *current = rhs;

            unsigned int tmp_pos = current->pos();
            current = parent;
            parent = current->m_parent;

            current->as_inner()->m_keys[tmp_pos - 1] = std::addressof(rhs->__get_key(0));

            return iterator(leaf, pos).__adjust_next();
        } while (false);

        lhs->size() = -(merge_size - 1);
        remove(rhs);
        __drop_leaf_node(rhs);

        __rec_erase_iter(parent, par_pos, cur_size);
        return iterator(leaf, pos).__adjust_next();
    }

    WJR_INTRINSIC_CONSTEXPR node_type *&__get_root() noexcept { return m_pair.second().m_parent; }

    WJR_INTRINSIC_CONSTEXPR const node_type *__get_root() const noexcept {
        return m_pair.second().m_parent;
    }

    WJR_INTRINSIC_CONSTEXPR list_node_type *__get_sentry() noexcept {
        return m_pair.second().self();
    }

    WJR_INTRINSIC_CONSTEXPR const list_node_type *__get_sentry() const noexcept {
        return m_pair.second().self();
    }

    WJR_INTRINSIC_CONSTEXPR size_type &__get_size() noexcept { return m_pair.second().m_root_size; }

    WJR_INTRINSIC_CONSTEXPR const size_type &__get_size() const noexcept {
        return m_pair.second().m_root_size;
    }

    compressed_pair<key_compare, list_base_type> m_pair;
};

} // namespace wjr

#endif // WJR_CONTAINER_BTREE_HPP__