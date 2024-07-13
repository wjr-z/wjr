#ifndef WJR_CONTAINER_GENERIC_BPLUS_TREE_HPP__
#define WJR_CONTAINER_GENERIC_BPLUS_TREE_HPP__

/**
 * @file bplus_tree.hpp
 * @brief B+ tree implementation.
 *
 * @details The multiset/multimap/set/map adapter has not been implemented yet. The
 * node_size should be set to 16 by default, and optimization has been made for queries
 * less than or equal to 16. \n
 * After improvement, the number of queries for the i-th query is
 * [1, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10], the average number of queries
 * is 6.56 times. In fact, the probability of querying smaller nodes is slightly greater
 * than that of larger nodes, so the actual number of queries will be less. If the
 * comparison operation of key_type is more complex, it is not recommended to use B+ tree,
 * because the number of queries of B+ tree will be more, thus offsetting the advantages
 * of B+ tree.
 *
 * @todo
 *
 * @version 0.1
 * @date 2024-05-06
 *
 */

#include <wjr/assert.hpp>
#include <wjr/compressed_pair.hpp>
#include <wjr/container/generic/container_fn.hpp>
#include <wjr/container/intrusive/list.hpp>
#include <wjr/memory/uninitialized.hpp>

#if defined(WJR_X86)
#include <wjr/x86/container/generic/bplus_tree.hpp>
#endif

namespace wjr {

template <typename Traits>
struct bplus_tree_node;

template <typename Traits>
struct bplus_tree_inner_node;

template <typename Traits, bool InlineKeys>
struct bplus_tree_leaf_node;

namespace bplus_tree_details {

template <typename T, bool Inlined>
class inline_key {
public:
    static_assert(!std::is_const_v<T>, "");

    using value_type = T;
    using reference = std::add_const_t<T> &;
    using pointer = std::add_const_t<T> *;

    constexpr inline_key() = default;
    constexpr inline_key(const inline_key &other) = default;
    constexpr inline_key(inline_key &&other) = default;
    constexpr inline_key &operator=(const inline_key &other) = default;
    constexpr inline_key &operator=(inline_key &&other) = default;
    ~inline_key() = default;

    constexpr inline_key(reference value) noexcept(
        std::is_nothrow_constructible_v<algined_storage<T>, reference>)
        : m_storage(value) {}

    constexpr reference operator*() const noexcept { return *m_storage; }
    constexpr reference get() const noexcept { return m_storage.get(); }
    constexpr pointer operator->() const noexcept { return get(); }

private:
    // no need to check
    algined_storage<T> m_storage;
};

template <typename T>
class inline_key<T, false> {
public:
    static_assert(!std::is_const_v<T>, "");

    using value_type = T;
    using reference = std::add_const_t<T> &;
    using pointer = std::add_const_t<T> *;

    constexpr inline_key() = default;
    constexpr inline_key(const inline_key &other) = default;
    constexpr inline_key(inline_key &&other) = default;
    constexpr inline_key &operator=(const inline_key &other) = default;
    constexpr inline_key &operator=(inline_key &&other) = default;
    ~inline_key() = default;

    constexpr inline_key(reference value) noexcept : m_ptr(std::addressof(value)) {}

    constexpr reference operator*() const noexcept { return *m_ptr; }
    constexpr pointer operator->() const noexcept { return m_ptr; }
    constexpr reference get() const noexcept { return *m_ptr; }

private:
    pointer m_ptr;
};

template <typename T>
struct is_possible_inline_key : std::is_trivially_copyable<algined_storage<T>> {};

template <typename T>
inline constexpr bool is_possible_inline_key_v = is_possible_inline_key<T>::value;

template <size_t Min, size_t Max, typename Other>
WJR_INTRINSIC_INLINE static void copy(Other *first, Other *last, Other *dest) noexcept {
#if WJR_HAS_BUILTIN(BPLUS_TREE_COPY)
    if constexpr (Max <= 16 && std::is_trivially_copyable_v<Other>) {
        builtin_bplus_tree_copy<Min, Max>(first, last, dest);
    } else {
#endif
        (void)std::copy(first, last, dest);
#if WJR_HAS_BUILTIN(BPLUS_TREE_COPY)
    }
#endif
}

template <size_t Min, size_t Max, typename Other>
WJR_INTRINSIC_INLINE static void copy_backward(Other *first, Other *last,
                                               Other *dest) noexcept {
#if WJR_HAS_BUILTIN(BPLUS_TREE_COPY)
    if constexpr (Max <= 16 && std::is_trivially_copyable_v<Other>) {
        builtin_bplus_tree_copy_backward<Min, Max>(first, last, dest);
    } else {
#endif
        (void)std::copy_backward(first, last, dest);
#if WJR_HAS_BUILTIN(BPLUS_TREE_COPY)
    }
#endif
}

} // namespace bplus_tree_details

template <typename Key, typename Value, typename Compare, size_t Size, bool Multi>
struct bplus_tree_traits {
    using key_type = Key;
    using mapped_type = Value;
    static constexpr bool is_map = !std::is_same_v<mapped_type, void>;
    using value_type =
        std::conditional_t<is_map, std::pair<const key_type, mapped_type>, key_type>;
    using key_compare = Compare;

    static constexpr size_t node_size = Size;
    static constexpr bool is_inline_key =
        bplus_tree_details::is_possible_inline_key_v<std::remove_const_t<key_type>> &&
        sizeof(key_type) <= 8;
    static constexpr bool is_inline_value =
        bplus_tree_details::is_possible_inline_key_v<std::remove_const_t<value_type>> &&
        sizeof(value_type) <= 8;

    using InlineKey =
        bplus_tree_details::inline_key<std::remove_const_t<key_type>, is_inline_key>;
    using InlineValue = std::conditional_t<
        is_inline_value,
        bplus_tree_details::inline_key<std::remove_const_t<value_type>, true>,
        value_type *>;

    using node_type = bplus_tree_node<bplus_tree_traits>;
    using inner_node_type = bplus_tree_inner_node<bplus_tree_traits>;
    using leaf_node_type =
        bplus_tree_leaf_node<bplus_tree_traits, is_inline_key && !is_inline_value>;
    static constexpr bool multi = Multi;

    WJR_INTRINSIC_INLINE static const key_type &
    get_key(const value_type &value) noexcept {
        if constexpr (is_map) {
            return value.first;
        } else {
            return value;
        }
    }

public:
    template <size_t Min = 0, size_t Max = node_size, typename Other = void>
    WJR_INTRINSIC_INLINE static void copy(Other *first, Other *last,
                                          Other *dest) noexcept {
        return bplus_tree_details::copy<Min, Max>(first, last, dest);
    }

    template <size_t Min = 0, size_t Max = node_size, typename Other = void>
    WJR_INTRINSIC_INLINE static void copy_backward(Other *first, Other *last,
                                                   Other *dest) noexcept {
        return bplus_tree_details::copy_backward<Min, Max>(first, last, dest);
    }
};

template <typename Traits>
struct bplus_tree_node {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    using InlineKey = typename Traits::InlineKey;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;

    constexpr inner_node_type *as_inner() noexcept;
    constexpr const inner_node_type *as_inner() const noexcept;

    constexpr leaf_node_type *as_leaf() noexcept;
    constexpr const leaf_node_type *as_leaf() const noexcept;

    int m_size;
    unsigned int m_pos;
    bplus_tree_node *m_parent;
};

template <typename Traits>
struct bplus_tree_inner_node : bplus_tree_node<Traits> {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    using InlineKey = typename Traits::InlineKey;

    alignas(16) InlineKey m_keys[node_size];
    alignas(16) bplus_tree_node<Traits> *m_sons[node_size + 1];
};

template <typename Traits, bool InlineKeys>
struct bplus_tree_leaf_node : bplus_tree_node<Traits>, list_node<> {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    using InlineKey = typename Traits::InlineKey;
    using ListNode = list_node<>;

    const key_type &__get_key(unsigned int pos) const noexcept { return *m_keys[pos]; }

    template <size_t Min = 0, size_t Max = node_size>
    WJR_INTRINSIC_INLINE void __copy(unsigned int start, unsigned int end,
                                     unsigned int dst_start,
                                     bplus_tree_leaf_node *dst) noexcept {
        Traits::template copy<Min, Max>(m_keys + start, m_keys + end,
                                        dst->m_keys + dst_start);
        Traits::template copy<Min, Max>(m_values + start, m_values + end,
                                        dst->m_values + dst_start);
    }

    template <size_t Min = 0, size_t Max = node_size>
    WJR_INTRINSIC_INLINE void __copy_backward(unsigned int start, unsigned int end,
                                              unsigned int dst_end,
                                              bplus_tree_leaf_node *dst) noexcept {
        Traits::template copy_backward<Min, Max>(m_keys + start, m_keys + end,
                                                 dst->m_keys + dst_end);
        Traits::template copy_backward<Min, Max>(m_values + start, m_values + end,
                                                 dst->m_values + dst_end);
    }

    WJR_INTRINSIC_INLINE void __assign(unsigned int idx,
                                       value_type *const value) noexcept {
        m_keys[idx] = Traits::get_key(*value);
        m_values[idx] = value;
    }

    constexpr ListNode *__get_list() noexcept { return this; }
    constexpr const ListNode *__get_list() const noexcept { return this; }

    alignas(16) InlineKey m_keys[node_size];
    alignas(16) value_type *m_values[node_size];
};

template <typename Traits>
struct bplus_tree_leaf_node<Traits, false> : bplus_tree_node<Traits>, list_node<> {
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    constexpr static size_t node_size = Traits::node_size;
    constexpr static bool is_inline_value = Traits::is_inline_value;
    using InlineValue = typename Traits::InlineValue;
    using ListNode = list_node<>;

    const key_type &__get_key(unsigned int pos) const noexcept {
        return Traits::get_key(*m_values[pos]);
    }

    template <size_t Min = 0, size_t Max = node_size>
    WJR_INTRINSIC_INLINE void __copy(unsigned int start, unsigned int end,
                                     unsigned int dst_start,
                                     bplus_tree_leaf_node *dst) noexcept {
        Traits::template copy<Min, Max>(m_values + start, m_values + end,
                                        dst->m_values + dst_start);
    }

    template <size_t Min = 0, size_t Max = node_size>
    WJR_INTRINSIC_INLINE void __copy_backward(unsigned int start, unsigned int end,
                                              unsigned int dst_end,
                                              bplus_tree_leaf_node *dst) noexcept {
        Traits::template copy_backward<Min, Max>(m_values + start, m_values + end,
                                                 dst->m_values + dst_end);
    }

    WJR_INTRINSIC_INLINE void __assign(unsigned int idx, InlineValue value) noexcept {
        m_values[idx] = value;
    }

    constexpr ListNode *__get_list() noexcept { return this; }
    constexpr const ListNode *__get_list() const noexcept { return this; }

    alignas(16) InlineValue m_values[node_size];
};

template <typename Traits>
constexpr typename bplus_tree_node<Traits>::inner_node_type *
bplus_tree_node<Traits>::as_inner() noexcept {
    return static_cast<inner_node_type *>(this);
}

template <typename Traits>
constexpr const typename bplus_tree_node<Traits>::inner_node_type *
bplus_tree_node<Traits>::as_inner() const noexcept {
    return static_cast<const inner_node_type *>(this);
}

template <typename Traits>
constexpr typename bplus_tree_node<Traits>::leaf_node_type *
bplus_tree_node<Traits>::as_leaf() noexcept {
    return static_cast<leaf_node_type *>(this);
}

template <typename Traits>
constexpr const typename bplus_tree_node<Traits>::leaf_node_type *
bplus_tree_node<Traits>::as_leaf() const noexcept {
    return static_cast<const leaf_node_type *>(this);
}

template <typename Traits, typename Alloc>
class basic_bplus_tree;

template <typename Traits>
class bplus_tree_const_iterator {
    using node_type = typename Traits::node_type;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;

    template <typename Other, typename Alloc>
    friend class basic_bplus_tree;

    using ListNode = list_node<>;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename Traits::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type *;
    using reference = const value_type &;

    bplus_tree_const_iterator() = default;
    bplus_tree_const_iterator(const bplus_tree_const_iterator &) = default;
    bplus_tree_const_iterator(bplus_tree_const_iterator &&) = default;
    bplus_tree_const_iterator &
    operator=(const bplus_tree_const_iterator &) = default;
    bplus_tree_const_iterator &operator=(bplus_tree_const_iterator &&) = default;
    ~bplus_tree_const_iterator() = default;

protected:
    bplus_tree_const_iterator(const ListNode *list_node, unsigned int pos) noexcept
        : m_node(const_cast<ListNode *>(list_node)), m_pos(pos) {}

    bplus_tree_const_iterator(const leaf_node_type *leaf, unsigned int pos) noexcept
        : bplus_tree_const_iterator(leaf->__get_list(), pos) {}

public:
    reference operator*() const noexcept { return *get_leaf()->m_values[m_pos]; }

    pointer operator->() const noexcept { return get_leaf()->m_values[m_pos]; }

    bplus_tree_const_iterator &operator++() noexcept {
        ++m_pos;
        return __adjust_next();
    }

    bplus_tree_const_iterator operator++(int) noexcept {
        bplus_tree_const_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bplus_tree_const_iterator &operator--() noexcept {
        if (m_pos != 0) {
            --m_pos;
        } else {
            m_node = prev(m_node);
            m_pos = -get_leaf()->m_size - 1;
        }

        return *this;
    }

    bplus_tree_const_iterator operator--(int) noexcept {
        bplus_tree_const_iterator tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(const bplus_tree_const_iterator &other) const noexcept {
        return m_node == other.m_node && m_pos == other.m_pos;
    }

    bool operator!=(const bplus_tree_const_iterator &other) const noexcept {
        return !(*this == other);
    }

    leaf_node_type *get_leaf() const noexcept {
        return static_cast<leaf_node_type *>(m_node);
    }

    ListNode *get_node() const noexcept { return m_node; }
    unsigned int get_pos() const noexcept { return m_pos; }

protected:
    bplus_tree_const_iterator &__adjust_next() noexcept {
        if (m_pos == static_cast<unsigned int>(-get_leaf()->m_size)) {
            m_node = next(m_node);
            m_pos = 0;
        }

        return *this;
    }

private:
    ListNode *m_node = nullptr;
    unsigned int m_pos = 0;
};

template <typename Traits>
class bplus_tree_iterator : public bplus_tree_const_iterator<Traits> {
    using Mybase = bplus_tree_const_iterator<Traits>;
    using leaf_node_type = typename Traits::leaf_node_type;

    template <typename Other, typename Alloc>
    friend class basic_bplus_tree;

    using ListNode = list_node<>;

public:
    using Mybase::Mybase;

    using iterator_category = typename Mybase::iterator_category;
    using value_type = typename Mybase::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

    bplus_tree_iterator(const Mybase &other) noexcept : Mybase(other) {}

protected:
    bplus_tree_iterator(const ListNode *list_node, unsigned int pos) noexcept
        : Mybase(list_node, pos) {}

    bplus_tree_iterator(const leaf_node_type *leaf, unsigned int pos) noexcept
        : Mybase(leaf, pos) {}

public:
    value_type &operator*() const noexcept {
        return const_cast<value_type &>(Mybase::operator*());
    }

    value_type *operator->() const noexcept {
        return const_cast<value_type *>(Mybase::operator->());
    }

    bplus_tree_iterator &operator++() noexcept {
        Mybase::operator++();
        return *this;
    }

    bplus_tree_iterator operator++(int) noexcept {
        bplus_tree_iterator tmp = *this;
        ++*this;
        return tmp;
    }

    bplus_tree_iterator &operator--() noexcept {
        Mybase::operator--();
        return *this;
    }

    bplus_tree_iterator operator--(int) noexcept {
        bplus_tree_iterator tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(const bplus_tree_iterator &other) const noexcept {
        return Mybase::operator==(other);
    }

    bool operator!=(const bplus_tree_iterator &other) const noexcept {
        return Mybase::operator!=(other);
    }

protected:
    bplus_tree_iterator &__adjust_next() noexcept {
        Mybase::__adjust_next();
        return *this;
    }
};

template <typename Traits, typename Alloc>
class basic_bplus_tree {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<uint8_t>;
    using _Alty_traits = std::allocator_traits<_Alty>;
    using storage_fn_type = container_fn<_Alty>;

    friend class container_fn<_Alty>;

    using mapped_type = typename Traits::mapped_type;
    static constexpr size_t node_size = Traits::node_size;
    static constexpr bool is_inline_key = Traits::is_inline_key;
    static constexpr bool is_inline_value = Traits::is_inline_value;
    using InlineKey = typename Traits::InlineKey;
    using InlineValue = typename Traits::InlineValue;
    static constexpr size_t floor_half = node_size / 2;
    static constexpr size_t ceil_half = node_size - floor_half;
    static constexpr bool Multi = Traits::Multi;

    using node_type = typename Traits::node_type;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;

    using ListNode = list_node<>;

public:
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;
    using key_compare = typename Traits::key_compare;
    using allocator_type = Alloc;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using iterator = bplus_tree_iterator<Traits>;
    using const_iterator = bplus_tree_const_iterator<Traits>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert(node_size >= 3, "node_size must be greater than or equal to 3.");
    static_assert(node_size <= 255, "node_size must be less than or equal to 255.");

    basic_bplus_tree() noexcept(std::is_nothrow_default_constructible_v<_Alty>) {
        init(&m_sentry);
    }

    // not implemented currently
    basic_bplus_tree(const basic_bplus_tree &) = delete;

    basic_bplus_tree(basic_bplus_tree &&other) noexcept(
        std::is_nothrow_move_constructible_v<key_compare>
            &&std::is_nothrow_move_constructible_v<_Alty>)
        : m_pair(std::piecewise_construct,
                 wjr::forward_as_tuple(std::move(other.key_comp())),
                 wjr::forward_as_tuple(
                     std::piecewise_construct,
                     wjr::forward_as_tuple(std::move(other.__get_allocator())),
                     wjr::forward_as_tuple())) {
        __take_tree(std::move(other));
    }

    basic_bplus_tree &operator=(const basic_bplus_tree &) = delete;

    basic_bplus_tree &operator=(basic_bplus_tree &&other) noexcept(
        noexcept(storage_fn_type::move_assign(*this, std::move(other)))) {
        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_fn_type::move_assign(*this, std::move(other));
        }

        return *this;
    }

    ~basic_bplus_tree() noexcept { __destroy_and_deallocate(); }

    constexpr key_compare &key_comp() noexcept { return m_pair.first(); }
    constexpr const key_compare &key_comp() const noexcept { return m_pair.first(); }

    iterator begin() noexcept { return iterator(m_sentry.next(), 0); }
    const_iterator begin() const noexcept { return const_iterator(m_sentry.next(), 0); }
    const_iterator cbegin() const noexcept { return const_iterator(m_sentry.next(), 0); }

    iterator end() noexcept { return iterator(&m_sentry, 0); }
    const_iterator end() const noexcept { return const_iterator(&m_sentry, 0); }
    const_iterator cend() const noexcept { return const_iterator(&m_sentry, 0); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

private:
    template <typename... Args>
    InlineValue __create_node(Args &&...args) noexcept {
        if constexpr (is_inline_value) {
            InlineValue ret(std::forward<Args>(args)...);
            return ret;
        } else {
            auto &al = __get_allocator();
            value_type *const xval =
                (value_type *)_Alty_traits::allocate(al, sizeof(value_type));
            uninitialized_construct_using_allocator(xval, al,
                                                    std::forward<Args>(args)...);
            return xval;
        }
    }

    template <typename... Args>
    void __drop_node(InlineValue xval) noexcept {
        if constexpr (!is_inline_value) {
            auto &al = __get_allocator();
            _Alty_traits::destroy(al, xval);
            _Alty_traits::deallocate(al, (uint8_t *)xval, sizeof(value_type));
        }
    }

    const_iterator __get_insert_multi_pos(const key_type &key) const noexcept {
        return __search<true>(key);
    }

    std::pair<const_iterator, bool>
    __get_insert_unique_pos(const key_type &key) const noexcept {
        const const_iterator iter = __search<true>(key);
        const auto pos = iter.get_pos();
        const bool inserted =
            pos == 0 || key_comp()(*iter.get_leaf()->m_values[pos - 1], key);
        return {iter, inserted};
    }

public:
    template <typename... Args>
    iterator __emplace_multi(Args &&...args) noexcept {
        const auto xval = __create_node(std::forward<Args>(args)...);
        const auto iter = __get_insert_multi_pos(Traits::get_key(*xval));
        return __insert_iter(iter, xval);
    }

    template <typename... Args>
    std::pair<iterator, bool> __emplace_unique(Args &&...args) noexcept {
        const auto xval = __create_node(std::forward<Args>(args)...);
        const auto [iter, inserted] = __get_insert_unique_pos(Traits::get_key(*xval));

        if (inserted) {
            return {__insert_iter(iter, xval), true};
        }

        __drop_node(xval);
        return {iterator(iter).__adjust_next(), false};
    }

    const_iterator __insert_multi(const value_type &val) noexcept {
        return __emplace_multi(val);
    }

    const_iterator __insert_multi(value_type &&val) noexcept {
        return __emplace_multi(std::move(val));
    }

    std::pair<const_iterator, bool> __insert_unique(const value_type &val) noexcept {
        return __emplace_unique(val);
    }

    std::pair<const_iterator, bool> __insert_unique(value_type &&val) noexcept {
        return __emplace_unique(std::move(val));
    }

    iterator lower_bound(const key_type &key) noexcept {
        return __search<false>(key).__adjust_next();
    }

    const_iterator lower_bound(const key_type &key) const noexcept {
        return __search<false>(key).__adjust_next();
    }

    iterator upper_bound(const key_type &key) noexcept {
        return __search<true>(key).__adjust_next();
    }

    const_iterator upper_bound(const key_type &key) const noexcept {
        return __search<true>(key).__adjust_next();
    }

    iterator erase(const_iterator iter) noexcept {
        return __erase_iter(iter).__adjust_next();
    }

private:
    void __take_tree(basic_bplus_tree &&other) noexcept {
        const auto root = other.__get_root();
        if (root == nullptr) {
            init(&m_sentry);
            return;
        }

        __get_root() = root;
        other.__get_root() = nullptr;
        replace_uninit(&other.m_sentry, &m_sentry);
        init(&other.m_sentry);
    }

    // member function for container_fn (START)

    WJR_PURE WJR_INTRINSIC_CONSTEXPR _Alty &__get_allocator() noexcept {
        return m_pair.second().first();
    }

    WJR_PURE WJR_INTRINSIC_CONSTEXPR const _Alty &__get_allocator() const noexcept {
        return m_pair.second().first();
    }

    void __destroy_and_deallocate() noexcept {
        node_type *current = __get_root();

        if (WJR_UNLIKELY(current == nullptr)) {
            return;
        }

        auto &al = __get_allocator();
        int cur_size = current->m_size;

        // If root is leaf
        if (cur_size < 0) {
            const auto leaf = current->as_leaf();
            const unsigned int cur_usize = -cur_size;

            for (unsigned int i = 0; i < cur_usize; ++i) {
                __drop_node(leaf->m_values[i]);
            }

            _Alty_traits::deallocate(al, (uint8_t *)leaf, sizeof(leaf_node_type));
            return;
        }

        // skip to the leftmost leaf
        current = begin().get_leaf();
        cur_size = -current->m_size;

        // cache of parent and parent's size
        node_type *parent = current->m_parent;
        unsigned int par_size = parent->m_size;

        // cache of `current' node's position in parent
        unsigned int pos = 0;

        do {
            const auto leaf = current->as_leaf();
            const unsigned int cur_usize = cur_size;

            for (unsigned int i = 0; i < cur_usize; ++i) {
                __drop_node(leaf->m_values[i]);
            }

            ListNode *next = wjr::next(leaf);
            _Alty_traits::deallocate(al, (uint8_t *)leaf, sizeof(leaf_node_type));

            // if `current' is the last child of parent
            if (WJR_UNLIKELY(pos++ == par_size)) {
                do {
                    current = parent;
                    parent = current->m_parent;
                    pos = current->m_pos;
                    _Alty_traits::deallocate(al, (uint8_t *)current,
                                             sizeof(inner_node_type));

                    // if `current' is the rightmost leaf
                    if (parent == nullptr) {
                        return;
                    }

                    // if `current' is the last child of parent
                } while (pos == (unsigned int)parent->m_size);

                // update cache of parent and parent's size
                parent = static_cast<leaf_node_type *>(next)->m_parent;
                par_size = parent->m_size;
                pos = 0;
            }

            WJR_ASSERT(next != &m_sentry);

            current = static_cast<leaf_node_type *>(next);
            cur_size = -current->m_size;
        } while (true);
    }

    void __take_storage(basic_bplus_tree &&other) noexcept {
        key_comp() = std::move(other.key_comp());
        __take_tree(std::move(other));
    }

    // member function for container_fn (END)

    WJR_NOINLINE void __rec_insert_iter(node_type *current, node_type *inst) noexcept {
        auto &al = __get_allocator();

        node_type *parent = current->m_parent;
        InlineKey key = inst->as_leaf()->__get_key(0);

        while (parent != nullptr) {
            inst->m_parent = parent;
            unsigned int pos = current->m_pos + 1;
            current = parent;
            const auto inner = current->as_inner();

            unsigned int cur_size = inner->m_size + 1;
            InlineKey *const keys = inner->m_keys;
            node_type **const sons = inner->m_sons;

            // non-full inner
            if (WJR_LIKELY(cur_size != node_size + 1)) {
                Traits::copy_backward(keys + pos - 1, keys + cur_size - 1,
                                      keys + cur_size);
                Traits::copy_backward(sons + pos, sons + cur_size, sons + cur_size + 1);

                inner->m_size = cur_size;
                keys[pos - 1] = key;
                sons[pos] = inst;

                inst->m_pos = pos;
                for (unsigned int i = pos + 1; i <= cur_size; ++i) {
                    sons[i]->m_pos = i;
                }

                return;
            }

            parent = inner->m_parent;

            const auto tmp_inst =
                (inner_node_type *)_Alty_traits::allocate(al, sizeof(inner_node_type));

            inner->m_size = (int)(ceil_half);
            tmp_inst->m_size = (int)(floor_half);

            InlineKey next_key;

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

                inst->m_pos = pos;
                for (unsigned int i = pos + 1; i <= ceil_half; ++i) {
                    sons[i]->m_pos = i;
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

                    Traits::template copy<0, floor_half - 1>(
                        keys + ceil_half + 1, keys + pos - 1, tmp_inst->m_keys);
                    Traits::template copy<1, floor_half>(sons + ceil_half + 1, sons + pos,
                                                         tmp_inst->m_sons);

                    const unsigned int rpos = pos - ceil_half - 1;

                    Traits::template copy<0, floor_half - 1>(
                        keys + pos - 1, keys + node_size, tmp_inst->m_keys + rpos);
                    Traits::template copy<0, floor_half>(sons + pos, sons + node_size + 1,
                                                         tmp_inst->m_sons + rpos + 1);

                    tmp_inst->m_keys[rpos - 1] = key;
                    tmp_inst->m_sons[rpos] = inst;
                }
            }

            for (unsigned int i = 0; i <= floor_half; ++i) {
                tmp_inst->m_sons[i]->m_parent = tmp_inst;
                tmp_inst->m_sons[i]->m_pos = i;
            }

            key = next_key;
            inst = tmp_inst;
        }

        const auto new_root =
            (inner_node_type *)_Alty_traits::allocate(al, sizeof(inner_node_type));
        new_root->m_size = 1;
        new_root->m_parent = nullptr;
        new_root->m_keys[0] = key;
        new_root->m_sons[0] = current;
        new_root->m_sons[1] = inst;
        current->m_pos = 0;
        inst->m_pos = 1;

        current->m_parent = new_root;
        inst->m_parent = new_root;

        __get_root() = new_root;
        return;
    }

    WJR_NODISCARD iterator __insert_iter(const_iterator iter, InlineValue xval) noexcept {
        auto &al = __get_allocator();

        leaf_node_type *leaf;
        do {
            ListNode *const node = iter.get_node();

            // empty
            if (WJR_UNLIKELY(node == &m_sentry)) {
                leaf = (leaf_node_type *)_Alty_traits::allocate(__get_allocator(),
                                                                sizeof(leaf_node_type));

                __get_root() = leaf;

                leaf->m_size = -1;
                leaf->m_parent = nullptr;
                leaf->__assign(0, xval);
                wjr::push_front(&m_sentry, leaf);
                return iterator(leaf, 0);
            }

            leaf = static_cast<leaf_node_type *>(node);
        } while (0);

        unsigned int pos = iter.get_pos();
        unsigned int cur_size = -leaf->m_size;

        // non-full leaf
        if (WJR_LIKELY(cur_size != node_size)) {
            WJR_ASSERT_ASSUME(pos <= cur_size);

            leaf->__copy_backward(pos, cur_size, cur_size + 1, leaf);

            leaf->m_size = -(cur_size + 1);
            leaf->__assign(pos, xval);
            return iterator(leaf, pos);
        }

        const auto inst =
            (leaf_node_type *)_Alty_traits::allocate(al, sizeof(leaf_node_type));
        push_front(leaf, inst);

        leaf->m_size = -(int)(floor_half + 1);
        inst->m_size = -(int)(node_size - floor_half);

        leaf_node_type *result;

        if (pos <= floor_half) {
            leaf->template __copy<ceil_half, ceil_half>(floor_half, node_size, 0, inst);
            leaf->template __copy_backward<0, floor_half>(pos, floor_half, floor_half + 1,
                                                          leaf);
            leaf->__assign(pos, xval);
            result = leaf;
        } else {
            // pos in inst
            const unsigned int rpos = pos - floor_half - 1;
            leaf->template __copy<0, ceil_half - 1>(floor_half + 1, pos, 0, inst);
            leaf->template __copy<0, ceil_half - 1>(pos, node_size, rpos + 1, inst);
            inst->__assign(rpos, xval);
            result = inst;
            pos = rpos;
        }

        __rec_insert_iter(leaf, inst);
        return iterator(result, pos);
    }

    template <bool Upper>
    WJR_PURE WJR_INTRINSIC_INLINE static bool
    __compare(const key_type &a, const key_type &key, const key_compare &comp) noexcept {
        if constexpr (Upper) {
            return comp(key, a);
        } else {
            return !comp(a, key);
        }
    }

    template <bool Upper>
    WJR_PURE WJR_NOINLINE const_iterator __search(const key_type &key) const noexcept {
        const node_type *current = __get_root();

        if (WJR_UNLIKELY(current == nullptr)) {
            return cend();
        }

        unsigned int pos;

        int cur_size = current->m_size;
        const auto &comp = key_comp();

        // root search
        if (WJR_UNLIKELY(cur_size < 0)) {
            pos = __search<Upper, 1, node_size, 0>(current->as_leaf(), -cur_size, key,
                                                   comp);
            return const_iterator(current->as_leaf(), pos);
        }

        if (!__compare<Upper>(*current->as_inner()->m_keys[0], key, comp)) {
            goto NOT_LEFTMOST_AT_ROOT;
        }

        current = current->as_inner()->m_sons[0];
        cur_size = current->m_size;

        while (cur_size >= 0) {
            if (!__compare<Upper>(*current->as_inner()->m_keys[0], key, comp)) {
                goto NOT_LEFTMOST_AT_INNER;
            }

            current = current->as_inner()->m_sons[0];
            cur_size = current->m_size;
        }

        // leftmost leaf need to check first key
        if (__compare<Upper>(current->as_leaf()->__get_key(0), key, comp)) {
            return const_iterator(current->as_leaf(), 0);
        }

        goto LEAF;

        do {

        NOT_LEFTMOST_AT_ROOT:
            pos = __search<Upper, 1, node_size, 1>(current->as_inner(), cur_size, key,
                                                   comp);

            break;

        NOT_LEFTMOST_AT_INNER:
            pos = __search<Upper, floor_half, node_size, 1>(current->as_inner(), cur_size,
                                                            key, comp);
        } while (0);

        current = current->as_inner()->m_sons[pos];
        cur_size = current->m_size;

        if (cur_size < 0) {
            goto LEAF;
        }

        do {
            pos = __search<Upper, floor_half, node_size, 0>(current->as_inner(), cur_size,
                                                            key, comp);

            current = current->as_inner()->m_sons[pos];
            cur_size = current->m_size;
        } while (cur_size >= 0);

    LEAF:
        pos = __search<Upper, floor_half, node_size, 1>(current->as_leaf(), -cur_size,
                                                        key, comp);
        return const_iterator(current->as_leaf(), pos);
    }

    template <size_t Min, size_t Max, size_t Offset, typename Compare>
    WJR_PURE WJR_INTRINSIC_INLINE static unsigned int
    __search(const node_type *current, unsigned int size, Compare &&comp) noexcept {
        static_assert(Offset == 0 || Offset == 1, "");
        static_assert(Min != 0, "");

        WJR_ASSERT_ASSUME(size >= Min);
        WJR_ASSERT_ASSUME(size <= Max);

        if constexpr (Min == 1 && Offset == 1) {
            if (WJR_UNLIKELY(size == 1)) {
                return 1;
            }
        }

        if constexpr (Max <= 16) {
            if constexpr (Offset == 0) {
                if (comp(current, 0)) {
                    return 0;
                }
            }

#define WJR_REGISTER_BLPUS_SEARCH_2(A, B, C)                                             \
    do {                                                                                 \
        if constexpr (A == Max) {                                                        \
            return A;                                                                    \
        } else if constexpr (B == Max) {                                                 \
            if (size == A || comp(current, A)) {                                         \
                return A;                                                                \
            }                                                                            \
            return B;                                                                    \
        } else if constexpr (C <= Max) {                                                 \
            if constexpr (Min < C) {                                                     \
                if (size < C) {                                                          \
                    if constexpr (Min <= A) {                                            \
                        if (size == A || comp(current, A)) {                             \
                            return A;                                                    \
                        }                                                                \
                    } else {                                                             \
                        if (comp(current, A)) {                                          \
                            return A;                                                    \
                        }                                                                \
                    }                                                                    \
                    return B;                                                            \
                }                                                                        \
            }                                                                            \
            if (comp(current, B)) {                                                      \
                if (comp(current, A)) {                                                  \
                    return A;                                                            \
                }                                                                        \
                return B;                                                                \
            }                                                                            \
        }                                                                                \
    } while (0)
#define WJR_REGISTER_BLPUS_SEARCH_4(A, B, C, D, E)                                       \
    do {                                                                                 \
        if constexpr (E > Max) {                                                         \
            WJR_REGISTER_BLPUS_SEARCH_2(A, B, C);                                        \
            WJR_REGISTER_BLPUS_SEARCH_2(C, D, E);                                        \
        } else {                                                                         \
            if constexpr (Min < E) {                                                     \
                if (size < E) {                                                          \
                    WJR_REGISTER_BLPUS_SEARCH_2(A, B, C);                                \
                    if constexpr (Min <= C) {                                            \
                        if (size == C || comp(current, C)) {                             \
                            return C;                                                    \
                        }                                                                \
                    } else {                                                             \
                        if (comp(current, C)) {                                          \
                            return C;                                                    \
                        }                                                                \
                    }                                                                    \
                    return D;                                                            \
                }                                                                        \
            }                                                                            \
            if (comp(current, D)) {                                                      \
                if (comp(current, B)) {                                                  \
                    if (comp(current, A)) {                                              \
                        return A;                                                        \
                    }                                                                    \
                    return B;                                                            \
                }                                                                        \
                if (comp(current, C)) {                                                  \
                    return C;                                                            \
                }                                                                        \
                return D;                                                                \
            }                                                                            \
        }                                                                                \
    } while (0)

            WJR_REGISTER_BLPUS_SEARCH_2(1, 2, 3);
            WJR_REGISTER_BLPUS_SEARCH_2(3, 4, 5);
            WJR_REGISTER_BLPUS_SEARCH_2(5, 6, 7);
            WJR_REGISTER_BLPUS_SEARCH_2(7, 8, 9);
            WJR_REGISTER_BLPUS_SEARCH_2(9, 10, 11);
            WJR_REGISTER_BLPUS_SEARCH_2(11, 12, 13);
            WJR_REGISTER_BLPUS_SEARCH_2(13, 14, 15);

            if constexpr (Max == 15) {
                return 15;
            } else if constexpr (Max == 16) {
                if (size == 15 || comp(current, 15)) {
                    return 15;
                }

                return 16;
            }

#undef WJR_REGISTER_BLPUS_SEARCH_4
#undef WJR_REGISTER_BLPUS_SEARCH_2
        } else {
            unsigned int pos = Offset;
            do {
                if (comp(current, pos)) {
                    break;
                }
            } while (++pos != size);
            return pos;
        }
    }

    template <bool Upper, size_t Min, size_t Max, size_t Offset>
    WJR_PURE WJR_INTRINSIC_INLINE static unsigned int
    __search(const inner_node_type *current, unsigned int size, const key_type &key,
             const key_compare &comp) noexcept {
        return __search<Min, Max, Offset>(
            current, size, [&key, &comp](const node_type *current, unsigned int pos) {
                return __compare<Upper>(*current->as_inner()->m_keys[pos], key, comp);
            });
    }

    template <bool Upper, size_t Min, size_t Max, size_t Offset>
    WJR_PURE WJR_INTRINSIC_INLINE static unsigned int
    __search(const leaf_node_type *current, unsigned int size, const key_type &key,
             const key_compare &comp) noexcept {
        return __search<Min, Max, Offset>(
            current, size, [&key, &comp](const node_type *current, unsigned int pos) {
                return __compare<Upper>(current->as_leaf()->__get_key(pos), key, comp);
            });
    }

    template <typename T>
    WJR_INTRINSIC_INLINE static unsigned int
    __init_remove_rotate(const inner_node_type *parent, unsigned int pos,
                         unsigned int par_size, T *&lhs, T *&rhs) noexcept {
        unsigned int size;

        do {
            if (pos != par_size) {
                const auto tmp = static_cast<T *>(parent->m_sons[pos + 1]);
                unsigned int tmp_size;

                if constexpr (std::is_same_v<T, leaf_node_type>) {
                    tmp_size = -tmp->m_size;
                } else {
                    tmp_size = tmp->m_size;
                }

                WJR_ASSERT_ASSUME(tmp_size >= floor_half);

                rhs = tmp;
                size = tmp_size;
            } else {
                auto tmp = static_cast<T *>(parent->m_sons[pos - 1]);
                lhs = tmp;

                if constexpr (std::is_same_v<T, leaf_node_type>) {
                    return -tmp->m_size;
                } else {
                    return tmp->m_size;
                }
            }
        } while (0);

        do {
            if (pos != 0) {
                const auto tmp = static_cast<T *>(parent->m_sons[pos - 1]);
                unsigned int tmp_size;

                if constexpr (std::is_same_v<T, leaf_node_type>) {
                    tmp_size = -tmp->m_size;
                } else {
                    tmp_size = tmp->m_size;
                }

                if (tmp_size >= size) {
                    lhs = tmp;
                    size = tmp_size;
                    break;
                }
            }

            lhs = nullptr;
        } while (0);

        return size;
    }

    /**
     * @todo use <Min, Max> to optimize
     *
     */
    WJR_NOINLINE void __rec_erase_iter(node_type *parent, unsigned int par_pos,
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

            InlineKey *const keys = inner->m_keys;
            node_type **const sons = inner->m_sons;

            if (cur_size > floor_half) {
                Traits::copy(keys + pos, keys + cur_size, keys + pos - 1);
                Traits::copy(sons + pos + 1, sons + cur_size + 1, sons + pos);

                for (unsigned int i = pos; i < cur_size; ++i) {
                    sons[i]->m_pos = i;
                }

                inner->m_size = cur_size - 1;
                return;
            }

            WJR_ASSERT_ASSUME(cur_size == floor_half);

            const auto par_inner = parent->as_inner();
            par_pos = inner->m_pos;
            par_size = par_inner->m_size;
            inner_node_type *lhs;
            inner_node_type *rhs;

            unsigned int next_size =
                __init_remove_rotate(par_inner, par_pos, par_size, lhs, rhs);

            do {
                if (lhs != nullptr) {
                    rhs = inner;

                    if (next_size == floor_half) {
                        Traits::copy(keys, keys + pos - 1, lhs->m_keys + floor_half + 1);
                        Traits::copy(sons, sons + pos, lhs->m_sons + floor_half + 1);
                        Traits::copy(keys + pos, keys + floor_half,
                                     lhs->m_keys + floor_half + pos);
                        Traits::copy(sons + pos + 1, sons + floor_half + 1,
                                     lhs->m_sons + floor_half + pos + 1);

                        for (unsigned int i = floor_half; i <= merge_size; ++i) {
                            lhs->m_sons[i]->m_parent = lhs;
                            lhs->m_sons[i]->m_pos = i;
                        }

                        lhs->m_keys[floor_half] = par_inner->m_keys[par_pos - 1];
                        break;
                    }

                    const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                    InlineKey key = lhs->m_keys[next_size - moved_elements];

                    if (moved_elements != 1) {
                        Traits::copy_backward(keys + pos, keys + floor_half,
                                              keys + floor_half + moved_elements - 1);
                        Traits::copy_backward(sons + pos + 1, sons + floor_half + 1,
                                              sons + floor_half + moved_elements);
                        for (unsigned int i = pos + moved_elements;
                             i < floor_half + moved_elements; ++i) {
                            sons[i]->m_pos = i;
                        }
                    }

                    Traits::copy_backward(keys, keys + pos - 1,
                                          keys + pos + moved_elements - 1);
                    Traits::copy_backward(sons, sons + pos, sons + pos + moved_elements);
                    Traits::copy(lhs->m_keys + next_size - moved_elements + 1,
                                 lhs->m_keys + next_size, keys);
                    Traits::copy(lhs->m_sons + next_size - moved_elements + 1,
                                 lhs->m_sons + next_size + 1, sons);

                    keys[moved_elements - 1] = par_inner->m_keys[par_pos - 1];
                    par_inner->m_keys[par_pos - 1] = key;

                    for (unsigned int i = 0; i < moved_elements; ++i) {
                        sons[i]->m_parent = inner;
                        sons[i]->m_pos = i;
                    }

                    for (unsigned int i = moved_elements; i < pos + moved_elements; ++i) {
                        sons[i]->m_pos = i;
                    }

                    lhs->m_size = next_size - moved_elements;
                    inner->m_size = floor_half + moved_elements - 1;
                } else {
                    WJR_ASSERT_ASSUME(rhs != nullptr);

                    lhs = inner;

                    if (next_size == floor_half) {
                        Traits::copy(keys + pos, keys + floor_half, keys + pos - 1);
                        Traits::copy(sons + pos + 1, sons + floor_half + 1, sons + pos);
                        Traits::copy(rhs->m_keys, rhs->m_keys + floor_half,
                                     keys + floor_half);
                        Traits::copy(rhs->m_sons, rhs->m_sons + floor_half + 1,
                                     sons + floor_half);

                        for (unsigned int i = pos; i < floor_half; ++i) {
                            inner->m_sons[i]->m_pos = i;
                        }

                        for (unsigned int i = floor_half; i <= merge_size; ++i) {
                            inner->m_sons[i]->m_parent = inner;
                            inner->m_sons[i]->m_pos = i;
                        }

                        lhs->m_keys[floor_half - 1] = par_inner->m_keys[par_pos];
                        ++par_pos;
                        break;
                    }

                    const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                    InlineKey key = rhs->m_keys[moved_elements - 1];

                    Traits::copy(keys + pos, keys + floor_half, keys + pos - 1);
                    Traits::copy(sons + pos + 1, sons + floor_half + 1, sons + pos);
                    Traits::copy(rhs->m_keys, rhs->m_keys + moved_elements - 1,
                                 keys + floor_half);
                    Traits::copy(rhs->m_sons, rhs->m_sons + moved_elements,
                                 sons + floor_half);
                    Traits::copy(rhs->m_keys + moved_elements, rhs->m_keys + next_size,
                                 rhs->m_keys);
                    Traits::copy(rhs->m_sons + moved_elements,
                                 rhs->m_sons + next_size + 1, rhs->m_sons);

                    keys[floor_half - 1] = par_inner->m_keys[par_pos];
                    par_inner->m_keys[par_pos] = key;

                    for (unsigned int i = pos; i < floor_half; ++i) {
                        sons[i]->m_pos = i;
                    }

                    for (unsigned int i = floor_half; i < floor_half + moved_elements;
                         ++i) {
                        sons[i]->m_parent = inner;
                        sons[i]->m_pos = i;
                    }

                    for (unsigned int i = 0; i <= next_size - moved_elements; ++i) {
                        rhs->m_sons[i]->m_pos = i;
                    }

                    rhs->m_size = next_size - moved_elements;
                    inner->m_size = floor_half + moved_elements - 1;
                }

                return;
            } while (0);

            lhs->m_size = merge_size;
            _Alty_traits::deallocate(__get_allocator(), (uint8_t *)rhs,
                                     sizeof(inner_node_type));

            pos = par_pos;
            cur_size = par_size;
            current = parent;
            parent = current->m_parent;
        }

        const auto inner = current->as_inner();

        if (cur_size == 1) {
            _Alty_traits::deallocate(__get_allocator(), (uint8_t *)inner,
                                     sizeof(inner_node_type));
            node_type *root = inner->m_sons[0];
            __get_root() = root;
            root->m_parent = nullptr;
            return;
        }

        Traits::copy(inner->m_keys + pos, inner->m_keys + cur_size,
                     inner->m_keys + pos - 1);
        Traits::copy(inner->m_sons + pos + 1, inner->m_sons + cur_size + 1,
                     inner->m_sons + pos);

        for (unsigned int i = pos; i < cur_size; ++i) {
            inner->m_sons[i]->m_pos = i;
        }

        inner->m_size = cur_size - 1;
    }

    iterator __erase_iter(const_iterator iter) noexcept {
        constexpr unsigned int merge_size = floor_half * 2;

        leaf_node_type *leaf = iter.get_leaf();
        unsigned int pos = iter.get_pos();
        unsigned int cur_size = -leaf->m_size;
        node_type *parent = leaf->m_parent;

        __drop_node(leaf->m_values[pos]);

        if (WJR_LIKELY(cur_size > floor_half)) {
            leaf->__copy(pos + 1, cur_size, pos, leaf);
            leaf->m_size = -(cur_size - 1);

            // first key in leaf is changed
            if (pos == 0 && parent != nullptr) {
                node_type *current = leaf;
                unsigned int tmp_pos;

                do {
                    tmp_pos = current->m_pos;
                    current = parent;
                    parent = current->m_parent;
                } while (tmp_pos == 0 && parent != nullptr);

                if (tmp_pos != 0) {
                    current->as_inner()->m_keys[tmp_pos - 1] = leaf->__get_key(0);
                }
            }

            return iterator(leaf, pos);
        }

        if (parent == nullptr) {
            if (cur_size == 1) {
                _Alty_traits::deallocate(__get_allocator(), (uint8_t *)leaf,
                                         sizeof(leaf_node_type));
                __get_root() = nullptr;
                init(&m_sentry);
                return cend();
            }

            leaf->__copy(pos + 1, cur_size, pos, leaf);
            leaf->m_size = -(cur_size - 1);
            return iterator(leaf, pos);
        }

        WJR_ASSERT_ASSUME(cur_size == floor_half);

        const auto inner = parent->as_inner();
        unsigned int par_pos = leaf->m_pos;
        cur_size = inner->m_size;
        leaf_node_type *lhs;
        leaf_node_type *rhs;

        const unsigned int next_size =
            __init_remove_rotate(inner, par_pos, cur_size, lhs, rhs);

        do {
            constexpr unsigned int max_moved_elements = (ceil_half + 1) / 2;

            if (lhs != nullptr) {
                rhs = leaf;

                if (next_size == floor_half) {
                    leaf->template __copy<0, floor_half>(0, pos, floor_half, lhs);
                    leaf->template __copy<0, floor_half>(pos + 1, floor_half,
                                                         pos + floor_half, lhs);

                    leaf = lhs;
                    pos += floor_half;
                    break;
                }

                const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                if (moved_elements != 1) {
                    leaf->template __copy_backward<0, floor_half>(
                        pos + 1, floor_half, floor_half + moved_elements - 1, leaf);
                }

                leaf->template __copy_backward<0, floor_half>(0, pos,
                                                              pos + moved_elements, leaf);
                lhs->template __copy<1, max_moved_elements>(next_size - moved_elements,
                                                            next_size, 0, leaf);

                lhs->m_size = -(next_size - moved_elements);
                leaf->m_size = -(floor_half + moved_elements - 1);

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
                        tmp_pos = current->m_pos;
                        current = tmp_parent;
                        tmp_parent = current->m_parent;
                    } while (tmp_pos == 0 && tmp_parent != nullptr);

                    if (tmp_pos != 0) {
                        current->as_inner()->m_keys[tmp_pos - 1] = leaf->__get_key(0);
                    }
                }

                if (next_size == floor_half) {
                    rhs->template __copy<0, floor_half>(0, floor_half, floor_half - 1,
                                                        leaf);

                    ++par_pos;
                    break;
                }

                const unsigned int moved_elements = (next_size - floor_half + 1) / 2;

                rhs->template __copy<1, max_moved_elements>(0, moved_elements,
                                                            floor_half - 1, leaf);
                rhs->template __copy<1, node_size - max_moved_elements>(
                    moved_elements, next_size, 0, rhs);

                rhs->m_size = -(next_size - moved_elements);
                leaf->m_size = -(floor_half + moved_elements - 1);
            }

            node_type *current = rhs;

            unsigned int tmp_pos = current->m_pos;
            current = parent;
            parent = current->m_parent;

            current->as_inner()->m_keys[tmp_pos - 1] = rhs->__get_key(0);

            return iterator(leaf, pos);
        } while (0);

        lhs->m_size = -(merge_size - 1);
        remove_uninit(rhs);
        _Alty_traits::deallocate(__get_allocator(), (uint8_t *)rhs,
                                 sizeof(leaf_node_type));

        __rec_erase_iter(parent, par_pos, cur_size);

        return iterator(leaf, pos);
    }

    WJR_INTRINSIC_CONSTEXPR node_type *&__get_root() noexcept {
        return m_pair.second().second();
    }

    WJR_INTRINSIC_CONSTEXPR const node_type *__get_root() const noexcept {
        return m_pair.second().second();
    }

    compressed_pair<key_compare, compressed_pair<_Alty, node_type *>> m_pair;
    ListNode m_sentry;
};

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_BPLUS_TREE_HPP__