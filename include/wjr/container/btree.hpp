#ifndef WJR_CONTAINER_BTREE_HPP__
#define WJR_CONTAINER_BTREE_HPP__

/**
 * @file btree.hpp
 * @brief B+ tree implementation.
 *
 * @details The multiset/multimap/set/map adapter has not been implemented yet.
 * Only use when key is trivial. Otherwise, this maybe won't faster than
 * std::map because its search complexity is O((k - 1) * (log n / log k)).
 * SSO optimization for tree node is enabled now!
 * Default node_size is 8, because most optimizations have been made for size 8. But it is also
 * possible to manually adjust the node_size during compilation. Currently, setting node_size
 * through templates is not supported.
 *
 * @note Insertion and deletion may cause iterators to fail.
 *
 * @todo
 * 1. Erase a range with optimization.
 * 2. Construct with a range with optimization.
 * 3. Merge with optimization.
 * 4. [x] Code size optimization.
 * 5. GCC(low version) optimization on unpacked struct failed?
 * 6. Clang optimization on memcpy is so stupid.
 * 7. MSVC bug on memcpy order
 * (https://developercommunity.visualstudio.com/t/incorrect-memcpy-optimization/1151407).
 * 8. [x] Optimization on small size.
 * 9. Use a simple GC-arena to optimiza memory usage.
 *
 * @version 0.3
 * @date 2025-06-18
 *
 */

#include <wjr/assert.hpp>
#include <wjr/compressed_pair.hpp>
#include <wjr/container/container_fn.hpp>
#include <wjr/container/forward_list.hpp>
#include <wjr/container/list.hpp>
#include <wjr/memory/copy.hpp>
#include <wjr/memory/small_copy.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr {
namespace btree_detail {

template <size_t Min, size_t Max, typename T>
WJR_INTRINSIC_INLINE void copy(const T *first, const T *last, T *dest) noexcept {
    small_copy<Min, Max>(first, last, dest);
}

template <size_t Min, size_t Max, typename T>
WJR_INTRINSIC_INLINE void copy_backward(const T *first, const T *last, T *dest) noexcept {
    small_copy<Min, Max>(first, last, dest - (last - first));
}

template <typename Key, typename... Args>
struct inplace_key_extract_map {
    static constexpr bool extractable = false;
};

template <typename Key, typename Second>
struct inplace_key_extract_map<Key, Key, Second> {
    static constexpr bool extractable = true;

    static const Key &extract(const Key &Val, const Second &) noexcept { return Val; }
};

template <typename Key, typename First, typename Second>
struct inplace_key_extract_map<Key, std::pair<First, Second>> {
    static constexpr bool extractable = std::is_same_v<Key, remove_cvref_t<First>>;

    static const Key &extract(const std::pair<First, Second> &Val) noexcept { return Val.first; }
};

template <typename Key, typename... Args>
struct inplace_key_extract_set {
    // by default we can't extract the key in the emplace family and must construct a node we might
    // not use
    static constexpr bool extractable = false;
};

template <typename Key>
struct inplace_key_extract_set<Key, Key> {
    // we can extract the key in emplace if the emplaced type is identical to the key type
    static constexpr bool extractable = true;

    static const Key &extract(const Key &Val) noexcept { return Val; }
};

template <typename Key, typename = void>
struct __is_inline_key {
    static constexpr bool value = false;
};

template <typename Key>
struct __is_inline_key<Key, std::enable_if_t<is_complete_v<Key>>> {
    static constexpr bool value = std::is_trivial_v<Key> && sizeof(Key) <= 8;
};

template <typename Key>
inline constexpr bool __is_inline_key_v = __is_inline_key<Key>::value;

template <typename Key, typename Value>
struct __btree_inline_traits {
    using key_type = Key;
    using mapped_type = Value;
    static constexpr bool is_map = true;
    using value_type = std::pair<const key_type, mapped_type>;

    template <typename... Args>
    using inplace_key_extractor =
        btree_detail::inplace_key_extract_map<Key, remove_cvref_t<Args>...>;

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

    template <typename... Args>
    using inplace_key_extractor = btree_detail::inplace_key_extract_set<Key, Args...>;

    WJR_INTRINSIC_INLINE static const key_type &get_key(const value_type &value) noexcept {
        return value;
    }
};

} // namespace btree_detail

template <typename Traits>
class btree_node;

template <typename Traits>
class btree_inner_node;

template <typename Traits>
class btree_leaf_node;

template <typename Traits>
class btree_root_node;

template <typename Key, typename Value, bool Multi, typename Compare = std::less<>,
          typename Alloc = std::allocator<char>>
struct btree_traits : btree_detail::__btree_inline_traits<Key, Value> {
private:
    using Mybase = btree_detail::__btree_inline_traits<Key, Value>;

public:
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<char>;
    using _Alty_traits = std::allocator_traits<_Alty>;
    using storage_fn_type = container_fn<_Alty>;

    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;

    using key_type = typename Mybase::key_type;
    using mapped_type = typename Mybase::mapped_type;
    using value_type = typename Mybase::value_type;
    using key_compare = Compare;

    static constexpr bool is_inline_key = btree_detail::__is_inline_key_v<key_type>;
    static constexpr bool is_inline_value = btree_detail::__is_inline_key_v<value_type>;

    using ikey_type = std::conditional_t<is_inline_key, key_type, const key_type *>;
    using ivalue_type = std::conditional_t<is_inline_value, value_type, value_type *>;

    static constexpr bool is_map = Mybase::is_map;

    using node_type = btree_node<btree_traits>;
    using inner_node_type = btree_inner_node<btree_traits>;
    using leaf_node_type = btree_leaf_node<btree_traits>;
    static constexpr bool multi = Multi;

    using slot_size_type = int;
    using slot_usize_type = unsigned int;

    static constexpr size_t node_size = 8;

    static key_type &from_ikey(ikey_type &key) {
        if constexpr (is_inline_key) {
            return key;
        } else {
            return *key;
        }
    }

    static const key_type &from_ikey(const ikey_type &key) {
        if constexpr (is_inline_key) {
            return key;
        } else {
            return *key;
        }
    }

    static ikey_type to_ikey(const key_type &key) {
        if constexpr (is_inline_key) {
            return key;
        } else {
            return std::addressof(key);
        }
    }

    static value_type &from_ivalue(ivalue_type &value) {
        if constexpr (is_inline_value) {
            return value;
        } else {
            return *value;
        }
    }

    static const value_type &from_ivalue(const ivalue_type &value) {
        if constexpr (is_inline_value) {
            return value;
        } else {
            return *value;
        }
    }
};

struct btree_node_constructor_t {};
inline constexpr btree_node_constructor_t btree_node_constructor{};

template <typename Traits>
class btree_node {
public:
    using slot_size_type = typename Traits::slot_size_type;
    using slot_usize_type = typename Traits::slot_usize_type;

private:
    struct __normal_node {
        slot_size_type m_size;
        slot_usize_type m_pos;
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

    WJR_PURE constexpr slot_size_type &size() noexcept { return m_node.m_size; }
    WJR_PURE constexpr const slot_size_type &size() const noexcept { return m_node.m_size; }

    WJR_PURE constexpr slot_usize_type &pos() noexcept { return m_node.m_pos; }
    WJR_PURE constexpr const slot_usize_type &pos() const noexcept { return m_node.m_pos; }

    union {
        __normal_node m_node;
        size_type m_root_size;
    };

    btree_node *m_parent;
};

template <typename Traits>
class btree_inner_node : public btree_node<Traits> {
public:
    using key_type = typename Traits::key_type;
    using ikey_type = typename Traits::ikey_type;
    using slot_usize_type = typename Traits::slot_usize_type;

    alignas(16) ikey_type m_keys[Traits::node_size];
    alignas(16) btree_node<Traits> *m_sons[Traits::node_size + 1];
};

template <typename Traits>
class alignas(16) btree_leaf_node : public btree_node<Traits>,
                                    public intrusive::list_node<btree_leaf_node<Traits>> {
private:
    using Mybase = btree_node<Traits>;
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;

public:
    using ivalue_type = typename Traits::ivalue_type;
    using slot_usize_type = typename Traits::slot_usize_type;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(btree_leaf_node);

    constexpr btree_leaf_node(btree_node_constructor_t) noexcept : Mybase(btree_node_constructor) {}

    WJR_INTRINSIC_INLINE const key_type &get_key(slot_usize_type pos) const noexcept {
        return Traits::get_key(Traits::from_ivalue(m_values[pos]));
    }

    WJR_INTRINSIC_INLINE void assign(slot_usize_type idx, ivalue_type value) noexcept {
        m_values[idx] = value;
    }

    template <size_t Min, size_t Max>
    WJR_INTRINSIC_INLINE void copy(slot_usize_type start, slot_usize_type end,
                                   slot_usize_type dst_start, btree_leaf_node *dst) const noexcept {
        btree_detail::copy<Min, Max>(m_values + start, m_values + end, dst->m_values + dst_start);
    }

    template <size_t Min, size_t Max>
    WJR_INTRINSIC_INLINE void copy_backward(slot_usize_type start, slot_usize_type end,
                                            slot_usize_type dst_end,
                                            btree_leaf_node *dst) const noexcept {
        btree_detail::copy_backward<Min, Max>(m_values + start, m_values + end,
                                              dst->m_values + dst_end);
    }

#if WJR_HAS_GCC(6, 0, 0)
    alignas(16) ivalue_type m_values[];
#else
    alignas(16) ivalue_type m_values[0];
#endif
};

template <typename Traits>
class btree_root_node {
private:
    using Mybase = typename Traits::leaf_node_type;
    using ivalue_type = typename Traits::ivalue_type;

public:
    btree_root_node() noexcept : m_base() {}
    btree_root_node(const btree_root_node &other) noexcept : m_base(other.m_base) {}
    btree_root_node(btree_root_node &&other) noexcept : m_base(std::move(other.m_base)) {}

    btree_root_node &operator=(const btree_root_node &other) noexcept {
        m_base = other.m_base;
        return *this;
    }

    btree_root_node &operator=(btree_root_node &&other) noexcept {
        m_base = std::move(other.m_base);
        return *this;
    }

    ~btree_root_node() noexcept { std::destroy_at(__get_base()); }

    constexpr btree_root_node(btree_node_constructor_t) noexcept : m_base(btree_node_constructor) {}

    Mybase *__get_base() noexcept { return std::addressof(m_base); }
    const Mybase *__get_base() const noexcept { return std::addressof(m_base); }

    union {
        Mybase m_base;
        char m_buffer[sizeof(Mybase) + sizeof(typename Traits::ivalue_type) * 2];
    };

    /** @todo static_assert of offsetof */
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
    using leaf_node_type = typename Traits::leaf_node_type;

    template <typename Other>
    friend class basic_btree;

    using list_node_type = intrusive::list_node<leaf_node_type>;

    using slot_usize_type = typename Traits::slot_usize_type;

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
    btree_const_iterator(const leaf_node_type *leaf, slot_usize_type pos) noexcept
        : m_leaf(const_cast<leaf_node_type *>(leaf)), m_pos(pos) {}

    btree_const_iterator(const list_node_type *list_node, slot_usize_type pos) noexcept
        : btree_const_iterator(static_cast<const leaf_node_type *>(list_node), pos) {}

public:
    reference operator*() const noexcept { return Traits::from_ivalue(m_leaf->m_values[m_pos]); }
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
            m_leaf = m_leaf = m_leaf->prev()->self();
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
        return m_leaf == other.m_leaf && m_pos == other.m_pos;
    }

    bool operator!=(const btree_const_iterator &other) const noexcept { return !(*this == other); }

    WJR_PURE leaf_node_type *get_leaf() const noexcept { return m_leaf; }
    WJR_PURE slot_usize_type pos() const noexcept { return m_pos; }

protected:
    WJR_PURE slot_usize_type __get_usize() const noexcept {
        return static_cast<slot_usize_type>(-m_leaf->size());
    }

    WJR_INTRINSIC_INLINE btree_const_iterator &__adjust_next() noexcept {
        if (m_pos == __get_usize()) {
            m_leaf = static_cast<leaf_node_type *>(m_leaf->next());
            m_pos = 0;
        }

        return *this;
    }

private:
    leaf_node_type *m_leaf = nullptr;
    slot_usize_type m_pos = 0;
};

template <typename Traits>
class btree_iterator : public btree_const_iterator<Traits> {
    using Mybase = btree_const_iterator<Traits>;
    using leaf_node_type = typename Traits::leaf_node_type;

    template <typename Other>
    friend class basic_btree;

    using list_node_type = intrusive::list_node<leaf_node_type>;

    using slot_usize_type = typename Traits::slot_usize_type;

public:
    using Mybase::Mybase;

    using iterator_category = typename Mybase::iterator_category;
    using value_type = typename Mybase::value_type;
    using difference_type = typename Traits::difference_type;
    using pointer = value_type *;
    using reference = value_type &;

    btree_iterator(const Mybase &other) noexcept : Mybase(other) {}

protected:
    btree_iterator(const list_node_type *list_node, slot_usize_type pos) noexcept
        : Mybase(list_node, pos) {}

    btree_iterator(const leaf_node_type *leaf, slot_usize_type pos) noexcept : Mybase(leaf, pos) {}

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

template <size_t N>
struct basic_btree_searcher_impl<N, std::enable_if_t<(N <= 8)>> {
    static constexpr size_t node_size = N;

private:
    template <typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search_1_impl(unsigned int size,
                                                           const Compare &comp) noexcept {
        unsigned int i = 1;
        while (WJR_LIKELY(i < size)) {
            if (comp(i)) {
                // prop : 50%
                return i - (int)comp(i - 1);
            }

            i += 2;
        }

        return i - (int)(size != i || comp(i - 1));
    }

public:
    template <size_t Min, typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search(unsigned int size,
                                                    const Compare &comp) noexcept {
        static_assert(Min == 1);

        WJR_ASSERT_ASSUME(size >= Min);
        WJR_ASSERT_ASSUME(size <= node_size);

        return search_1_impl(size, comp);
    }
};

template <size_t N>
struct basic_btree_searcher_impl<N, std::enable_if_t<(N > 8)>> {
    static constexpr size_t node_size = N;

private:
    template <typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search_1_impl(unsigned int size,
                                                           const Compare &comp) noexcept {
        unsigned int L = 0, R = size;
        while (L < R) {
            unsigned int mid = (L + R) >> 1;
            if (comp(mid)) {
                R = mid;
            } else {
                L = mid + 1;
            }
        }

        return R;
    }

public:
    template <size_t Min, typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search(unsigned int size,
                                                    const Compare &comp) noexcept {
        static_assert(Min == 1);

        WJR_ASSERT_ASSUME(size >= Min);
        WJR_ASSERT_ASSUME(size <= node_size);

        return search_1_impl(size, comp);
    }
};

template <typename Traits>
class basic_btree {
    using _Alty = typename Traits::_Alty;
    using _Alty_traits = typename Traits::_Alty_traits;
    using storage_fn_type = container_fn<_Alty>;

    friend class container_fn<_Alty>;

    static constexpr size_t node_size = Traits::node_size;
    static constexpr size_t floor_half = node_size / 2;
    static constexpr size_t ceil_half = node_size - floor_half;
    static constexpr size_t max_moved_elements = (ceil_half + 1) / 2;
    static constexpr bool multi = Traits::multi;

    using node_type = typename Traits::node_type;
    using inner_node_type = typename Traits::inner_node_type;
    using leaf_node_type = typename Traits::leaf_node_type;
    using list_node_type = intrusive::list_node<leaf_node_type>;
    using ikey_type = typename Traits::ikey_type;
    using ivalue_type = typename Traits::ivalue_type;

    using slot_size_type = typename Traits::slot_size_type;
    using slot_usize_type = typename Traits::slot_usize_type;

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

private:
    static key_type &from_ikey(ikey_type &key) { return Traits::from_ikey(key); }
    static const key_type &from_ikey(const ikey_type &key) { return Traits::from_ikey(key); }
    static ikey_type to_ikey(const key_type &key) { return Traits::to_ikey(key); }

    static value_type &from_ivalue(ivalue_type &value) { return Traits::from_ivalue(value); }
    static const value_type &from_ivalue(const ivalue_type &value) {
        return Traits::from_ivalue(value);
    }

public:
    basic_btree() noexcept : basic_btree(key_compare()) {}

    explicit basic_btree(const key_compare &comp) noexcept(
        std::is_nothrow_copy_constructible_v<key_compare>)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(comp),
                 wjr::forward_as_tuple(std::piecewise_construct, wjr::forward_as_tuple(),
                                       wjr::forward_as_tuple(btree_node_constructor))) {}

    // not implemented currently
    basic_btree(const basic_btree &other) noexcept(
        std::is_nothrow_copy_constructible_v<key_compare>)
        : basic_btree(other.key_comp()) {
        __copy_assign_tree(other);
    }

    basic_btree(basic_btree &&other) noexcept(std::is_nothrow_move_constructible_v<key_compare>)
        : basic_btree(std::move(other.key_comp())) {
        __move_tree(std::move(other));
    }

    basic_btree &operator=(const basic_btree &other) noexcept {
        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_fn_type::copy_assign(*this, other);
        }

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

    iterator end() noexcept {
        return iterator(__get_sentry(), __get_root() == nullptr ? __get_size() : 0);
    }
    const_iterator end() const noexcept {
        return const_iterator(__get_sentry(), __get_root() == nullptr ? __get_size() : 0);
    }
    const_iterator cend() const noexcept {
        return const_iterator(__get_sentry(), __get_root() == nullptr ? __get_size() : 0);
    }

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

    void swap(basic_btree &other) {
        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_fn_type::swap(*this, other);
        }
    }

protected:
    inner_node_type *__create_inner_node() noexcept {
        auto &al = __get_allocator();
        auto *const node = reinterpret_cast<inner_node_type *>(
            _Alty_traits::allocate(al, sizeof(inner_node_type)));
        uninitialized_construct_using_allocator(node, al, default_construct);
        return node;
    }

    leaf_node_type *__create_leaf_node() noexcept {
        auto &al = __get_allocator();
        auto *const node = reinterpret_cast<leaf_node_type *>(_Alty_traits::allocate(
            al, sizeof(leaf_node_type) + sizeof(value_type *) * Traits::node_size));
        uninitialized_construct_using_allocator(node, al, default_construct);
        return node;
    }

    template <typename... Args>
    ivalue_type __create_node(Args &&...args) noexcept {
        ivalue_type node;
        auto &al = __get_allocator();
        if constexpr (Traits::is_inline_value) {
            uninitialized_construct_using_allocator(std::addressof(node), al,
                                                    std::forward<Args>(args)...);
        } else {
            node = reinterpret_cast<value_type *>(_Alty_traits::allocate(al, sizeof(value_type)));
            uninitialized_construct_using_allocator(node, al, std::forward<Args>(args)...);
        }

        return node;
    }

    void __drop_inner_node(inner_node_type *node) noexcept {
        auto &al = __get_allocator();
        destroy_at_using_allocator(node, al);
        _Alty_traits::deallocate(al, reinterpret_cast<char *>(node), sizeof(inner_node_type));
    }

    void __drop_leaf_node(leaf_node_type *node) noexcept {
        auto &al = __get_allocator();
        destroy_at_using_allocator(node, al);
        _Alty_traits::deallocate(al, reinterpret_cast<char *>(node),
                                 sizeof(leaf_node_type) + sizeof(value_type *) * Traits::node_size);
    }

    void __drop_node(ivalue_type node) noexcept {
        auto &al = __get_allocator();
        if constexpr (Traits::is_inline_value) {
            destroy_at_using_allocator(std::addressof(node), al);
        } else {
            destroy_at_using_allocator(node, al);
            _Alty_traits::deallocate(al, reinterpret_cast<char *>(node), sizeof(value_type));
        }
    }

    WJR_INTRINSIC_INLINE const_iterator __get_insert_multi_pos(const key_type &key) const noexcept {
        return __search<true, false>(key);
    }

    template <typename... Args>
    iterator __emplace_multi(Args &&...args) noexcept {
        ivalue_type xval = __create_node(std::forward<Args>(args)...);
        const auto iter = __get_insert_multi_pos(Traits::get_key(*xval));
        return __insert_iter(iter, xval);
    }

    WJR_INTRINSIC_INLINE bool __same_insert_key(const_iterator iter, const key_type &key) noexcept {
        auto pos = iter.pos();
        return pos != 0 && !key_comp()(iter.get_leaf()->get_key(pos - 1), key);
    }

    template <typename... Args>
    std::pair<iterator, bool> __emplace_unique(Args &&...args) noexcept {
        using inplace_key_extractor =
            typename Traits::template inplace_key_extractor<remove_cvref_t<Args>...>;

        const_iterator iter;
        ivalue_type xval;

        if constexpr (inplace_key_extractor::extractable) {
            const auto &key = inplace_key_extractor::extract(std::forward<Args>(args)...);
            iter = __get_insert_multi_pos(key);

            if (WJR_UNLIKELY(__same_insert_key(iter, key))) {
                return {iterator(iter).__adjust_next(), false};
            }

            xval = __create_node(std::forward<Args>(args)...);
        } else {
            xval = __create_node(std::forward<Args>(args)...);
            const auto &key = Traits::get_key(from_ivalue(xval));
            iter = __get_insert_multi_pos(key);

            if (WJR_UNLIKELY(__same_insert_key(iter, key))) {
                __drop_node(xval);
                return {iterator(iter).__adjust_next(), false};
            }
        }

        return {__insert_iter(iter, xval), true};
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
        const_iterator iter = __get_insert_multi_pos(key);

        if (WJR_UNLIKELY(__same_insert_key(iter, key))) {
            return {iterator(iter).__adjust_next(), false};
        }

        ivalue_type xval = __create_node(std::piecewise_construct, std::forward_as_tuple(key),
                                         std::forward_as_tuple(std::forward<Args>(args)...));
        return {__insert_iter(iter, xval), true};
    }

    template <typename... Args>
    std::pair<iterator, bool> __try_emplace_unique(key_type &&key, Args &&...args) {
        const_iterator iter = __get_insert_multi_pos(key);

        if (WJR_UNLIKELY(__same_insert_key(iter, key))) {
            return {iterator(iter).__adjust_next(), false};
        }

        ivalue_type xval =
            __create_node(std::piecewise_construct, std::forward_as_tuple(std::move(key)),
                          std::forward_as_tuple(std::forward<Args>(args)...));
        return {__insert_iter(iter, xval), true};
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
        return __erase_iter(iter);
    }

    WJR_PURE bool empty() const noexcept { return size() == 0; }
    WJR_PURE size_type size() const noexcept { return __get_size(); }

private:
    void __move_tree(basic_btree &&other) noexcept {
        node_type *&root = other.__get_root();
        if (WJR_UNLIKELY(root == nullptr)) {
            __get_sentry()->init_self();
            auto &size = other.__get_size();
            copy_small_to(other.__get_base()->m_values, __get_base()->m_values);
            __get_size() = std::exchange(size, 0);
            return;
        }

        __get_root() = std::exchange(root, nullptr);
        __get_size() = std::exchange(other.__get_size(), 0);
        replace(other.__get_sentry(), __get_sentry());
        other.__get_sentry()->init_self();
    }

    void __swap_small_impl(basic_btree &other) noexcept {
        // this is small, other is not small
        copy_small_to(__get_base()->m_values, other.__get_base()->m_values);
        std::swap(__get_root(), other.__get_root());
        std::swap(__get_size(), other.__get_size());
        replace(other.__get_sentry(), __get_sentry());
        other.__get_sentry()->init_self();
    }

    void __swap_tree(basic_btree &other) noexcept {
        if (__get_root() == nullptr) {
            if (other.__get_root() == nullptr) {
                __simd_storage_t<ivalue_type, 2> tmp;

                auto *lhs = __get_base();
                auto *rhs = other.__get_base();

                copy_small_to(lhs->m_values, &tmp);
                copy_small_to(rhs->m_values, lhs->m_values);
                copy_small_to(&tmp, rhs->m_values);

                std::swap(__get_size(), other.__get_size());
            } else {
                __swap_small_impl(other);
            }
            return;
        }

        if (other.__get_root() == nullptr) {
            other.__swap_small_impl(*this);
            return;
        }

        std::swap(__get_root(), other.__get_root());
        std::swap(__get_size(), other.__get_size());
        swap_with(__get_sentry(), other.__get_sentry());
    }

    template <typename TV>
    std::pair<ikey_type, node_type *> __rec_assign_tree(node_type *current) noexcept {
        slot_size_type cur_size = current->size();

        if (cur_size < 0) {
            auto *const leaf = current->as_leaf();
            const slot_usize_type cur_usize = -cur_size;

            auto *const this_leaf = __create_leaf_node();
            this_leaf->size() = cur_size;
            for (slot_usize_type i = 0; i < cur_usize; ++i) {
                this_leaf->assign(i,
                                  __create_node(static_cast<TV>(from_ivalue(leaf->m_values[i]))));
            }

            __get_sentry()->push_back(this_leaf);
            return {to_ikey(this_leaf->get_key(0)), this_leaf};
        }

        auto *const this_inner = __create_inner_node();
        this_inner->size() = cur_size;
        ikey_type Key;
        const slot_usize_type cur_usize = cur_size;

        for (slot_usize_type i = 0; i <= cur_usize; ++i) {
            auto [key, son] = __rec_assign_tree<TV>(current->as_inner()->m_sons[i]);
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

    template <typename TV, typename BT>
    void __assign_tree(BT &&other) noexcept {
        node_type *current = const_cast<node_type *>(other.__get_root());
        __get_sentry()->init_self();
        slot_usize_type size = other.__get_size();
        __get_size() = size;
        if (WJR_UNLIKELY(current == nullptr)) {
            for (slot_usize_type i = 0; i < size; ++i) {
                __get_base()->assign(i, __create_node(static_cast<TV>(
                                            from_ivalue(other.__get_base()->m_values[i]))));
            }

            return;
        }

        auto [key, root] = __rec_assign_tree<TV>(current);
        __get_root() = root;
        root->m_parent = nullptr;
    }

    void __copy_assign_tree(const basic_btree &other) noexcept {
        __assign_tree<const value_type &>(other);
    }

    void __move_assign_tree(basic_btree &&other) noexcept {
        __assign_tree<value_type &&>(std::move(other));
    }

    void __rec_destroy_and_deallocate(node_type *current) noexcept;

    // member function for container_fn (START)

    WJR_CONSTEXPR20 _Alty &__get_allocator() noexcept { return m_pair.second().first(); }
    WJR_CONSTEXPR20 const _Alty &__get_allocator() const noexcept {
        return m_pair.second().first();
    }

    void __destroy_and_deallocate() noexcept;

    void __release_before_copy() {}

    void __copy_element(const basic_btree &other) noexcept {
        clear();
        __copy_assign_tree(other);
    }

    void __take_storage(basic_btree &&other) noexcept {
        key_comp() = std::move(other.key_comp());
        __move_tree(std::move(other));
    }

    void __destroy_and_move_element(basic_btree &&other) noexcept { // do nothing
        clear();
        __move_assign_tree(std::move(other));
    }

    void __swap_storage(basic_btree &other) noexcept {
        std::swap(key_comp(), other.key_comp());
        __swap_tree(other);
    }

    // member function for container_fn (END)

    WJR_NODISCARD WJR_NOINLINE WJR_HOT WJR_FLATTEN iterator
    __insert_iter(const_iterator iter, ivalue_type xval) noexcept;

    static constexpr slot_usize_type __search_mask =
        (slot_usize_type)(1) << (std::numeric_limits<slot_usize_type>::digits - 1);
    static_assert(__search_mask > node_size);

    template <bool Upper>
    WJR_NOINLINE WJR_HOT WJR_FLATTEN const_iterator
    __search_impl(const key_type &key) const noexcept {
        const auto &comp = key_comp();
        const node_type *current = __get_root();
        if (current == nullptr) {
            slot_usize_type size = __get_size();
            if (size == 0 || __compare<Upper>(__get_base()->get_key(0), key, comp)) {
                return const_iterator(__get_sentry(), 0);
            }

            if (size == 1 || __compare<Upper>(__get_base()->get_key(1), key, comp)) {
                return const_iterator(__get_sentry(), 1);
            }

            return const_iterator(__get_sentry(), 2);
        }

        slot_size_type cur_size = current->size();
        slot_usize_type pos;

        if (cur_size >= 0) {
            do {
                pos = __search<Upper, 1>(current->as_inner(), cur_size, key, comp);
                current = current->as_inner()->m_sons[pos];
                cur_size = current->size();
            } while (cur_size >= 0);
        }

        const slot_usize_type cur_usize = -cur_size;
        pos = __search<Upper, 1>(current->as_leaf(), cur_usize, key, comp);
        return const_iterator(current->as_leaf(), pos | (pos == cur_usize ? __search_mask : 0));
    }

    template <bool Upper, bool Adjust>
    WJR_HOT WJR_FLATTEN const_iterator __search(const key_type &key) const noexcept {
        const_iterator iter = __search_impl<Upper>(key);

        if constexpr (Adjust) {
            if (WJR_LIKELY(!(iter.pos() & __search_mask))) {
                return iter;
            }

            return const_iterator(iter.get_leaf()->next()->self(), 0);
        } else {
            return const_iterator(iter.get_leaf(), iter.pos() & ~__search_mask);
        }
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
    WJR_INTRINSIC_INLINE static slot_usize_type __search(const inner_node_type *current,
                                                         slot_usize_type size, const key_type &key,
                                                         const key_compare &comp) noexcept {
        return basic_btree_searcher_impl<node_size>::template search<Min>(
            size, [current, &key, &comp](slot_usize_type pos) {
                return __compare<Upper>(from_ikey(current->m_keys[pos]), key, comp);
            });
    }

    template <bool Upper, size_t Min>
    WJR_INTRINSIC_INLINE static slot_usize_type __search(const leaf_node_type *current,
                                                         slot_usize_type size, const key_type &key,
                                                         const key_compare &comp) noexcept {
        return basic_btree_searcher_impl<node_size>::template search<Min>(
            size, [current, &key, &comp](slot_usize_type pos) {
                return __compare<Upper>(current->get_key(pos), key, comp);
            });
    }

    template <typename T>
    WJR_INTRINSIC_INLINE static slot_usize_type
    __init_remove_rotate(const inner_node_type *parent, slot_usize_type pos,
                         slot_usize_type par_size, T *&lhs, T *&rhs) noexcept {
        slot_usize_type size;

        if (pos != 0) {
            auto *const tmp = static_cast<T *>(parent->m_sons[pos - 1]);
            slot_usize_type tmp_size;

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
            slot_usize_type tmp_size;

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
    WJR_INTRINSIC_INLINE void __rec_erase_iter(node_type *parent, slot_usize_type par_pos,
                                               slot_usize_type par_size) noexcept;

    WJR_NODISCARD WJR_NOINLINE WJR_HOT WJR_FLATTEN iterator
    __erase_iter(const_iterator iter) noexcept;

    WJR_INTRINSIC_CONSTEXPR leaf_node_type *__get_base() noexcept {
        return m_pair.second().second().__get_base();
    }

    WJR_INTRINSIC_CONSTEXPR const leaf_node_type *__get_base() const noexcept {
        return m_pair.second().second().__get_base();
    }

    WJR_INTRINSIC_CONSTEXPR node_type *&__get_root() noexcept { return __get_base()->m_parent; }

    WJR_INTRINSIC_CONSTEXPR const node_type *__get_root() const noexcept {
        return __get_base()->m_parent;
    }

    WJR_INTRINSIC_CONSTEXPR list_node_type *__get_sentry() noexcept { return __get_base()->self(); }

    WJR_INTRINSIC_CONSTEXPR const list_node_type *__get_sentry() const noexcept {
        return __get_base()->self();
    }

    WJR_INTRINSIC_CONSTEXPR size_type &__get_size() noexcept { return __get_base()->m_root_size; }

    WJR_INTRINSIC_CONSTEXPR const size_type &__get_size() const noexcept {
        return __get_base()->m_root_size;
    }

    static void copy_small_to(const ivalue_type *from, ivalue_type *to) {
        uninitialized_copy_n_restrict(from, 2, to);
    }

    compressed_pair<key_compare, compressed_pair<_Alty, btree_root_node<Traits>>> m_pair;
};

template <typename Traits>
void basic_btree<Traits>::__rec_destroy_and_deallocate(node_type *current) noexcept {
    slot_size_type cur_size = current->size();

    // If root is leaf
    if (cur_size < 0) {
        auto *const leaf = current->as_leaf();
        const slot_usize_type cur_usize = -cur_size;

        for (slot_usize_type i = 0; i < cur_usize; ++i) {
            __drop_node(leaf->m_values[i]);
        }

        __drop_leaf_node(leaf);
        return;
    }

    auto *const inner = current->as_inner();
    const slot_usize_type cur_usize = cur_size;

    for (slot_usize_type i = 0; i <= cur_usize; ++i) {
        __rec_destroy_and_deallocate(inner->m_sons[i]);
    }

    __drop_inner_node(inner);
}

template <typename Traits>
void basic_btree<Traits>::__destroy_and_deallocate() noexcept {
    node_type *current = __get_root();
    if (WJR_UNLIKELY(current == nullptr)) {
        slot_usize_type size = __get_size();
        for (slot_usize_type i = 0; i < size; ++i) {
            __drop_node(__get_base()->m_values[i]);
        }
        return;
    }

    __rec_destroy_and_deallocate(current);
}

template <typename Traits>
typename basic_btree<Traits>::iterator
basic_btree<Traits>::__insert_iter(const_iterator iter, ivalue_type xval) noexcept {
    ++__get_size();
    slot_usize_type pos = iter.pos();
    leaf_node_type *leaf;
    // empty
    if (__get_root() == nullptr) {
        // size has incresed before __insert_iter
        slot_usize_type size = __get_size();
        if (size <= 2) {
            leaf = __get_base();
            if (size == 1) {
                // do nothing
            } else {
                if (pos == 0) {
                    leaf->assign(1, leaf->m_values[0]);
                }
            }
        } else {
            leaf = __create_leaf_node();
            __get_root() = leaf;
            leaf->size() = -3;
            leaf->m_parent = nullptr;
            leaf->assign(pos == 0 ? 1 : 0, __get_base()->m_values[0]);
            leaf->assign(pos == 2 ? 1 : 2, __get_base()->m_values[1]);
            __get_sentry()->push_front(leaf);
        }

        leaf->assign(pos, xval);
        return iterator(leaf, pos);
    }

    leaf = iter.get_leaf();
    do {
        const slot_usize_type cur_size = static_cast<slot_usize_type>(-leaf->size());

        // non-full leaf
        if (WJR_LIKELY(cur_size != node_size)) {
            WJR_ASSERT_ASSUME(pos <= cur_size);
            leaf->template copy_backward<0, node_size - 1>(pos, cur_size, cur_size + 1, leaf);

            leaf->size() = -(cur_size + 1);
            leaf->assign(pos, xval);
            return iterator(leaf, pos);
        }
    } while (0);

    iterator result;
    node_type *inst;

    do {
        auto *const inst_leaf = __create_leaf_node();
        leaf->push_front(inst_leaf);
        leaf->size() = -(slot_size_type)(floor_half + 1);
        inst_leaf->size() = -(slot_size_type)(node_size - floor_half);

        if (pos <= floor_half) {
            leaf->template copy<ceil_half, ceil_half>(floor_half, node_size, 0, inst_leaf);
            leaf->template copy_backward<0, floor_half>(pos, floor_half, floor_half + 1, leaf);
            leaf->assign(pos, xval);
            result = iterator(iter);
        } else {
            // pos in inst_leaf
            const slot_usize_type rpos = pos - floor_half - 1;
            leaf->template copy<0, ceil_half - 1>(floor_half + 1, pos, 0, inst_leaf);
            leaf->template copy<0, ceil_half - 1>(pos, node_size, rpos + 1, inst_leaf);
            inst_leaf->assign(rpos, xval);
            result = iterator(inst_leaf, rpos);
        }

        inst = inst_leaf;
    } while (0);

    {
        node_type *current = leaf;
        node_type *parent = current->m_parent;
        ikey_type key = to_ikey(inst->as_leaf()->get_key(0));

        while (parent != nullptr) {
            inst->m_parent = parent;
            pos = current->pos() + 1;
            current = parent;
            auto *const inner = current->as_inner();

            slot_usize_type cur_size = inner->size() + 1;
            auto *const keys = inner->m_keys;
            node_type **const sons = inner->m_sons;

            // non-full inner
            if (WJR_LIKELY(cur_size != node_size + 1)) {
                btree_detail::copy_backward<0, node_size - 1>(keys + pos - 1, keys + cur_size - 1,
                                                              keys + cur_size);
                btree_detail::copy_backward<0, node_size - 1>(sons + pos, sons + cur_size,
                                                              sons + cur_size + 1);

                inner->size() = cur_size;
                keys[pos - 1] = key;
                sons[pos] = inst;

                inst->pos() = pos;
                for (slot_usize_type i = pos + 1; i <= cur_size; ++i) {
                    sons[i]->pos() = i;
                }

                return result;
            }

            parent = inner->m_parent;

            auto *const tmp_inst = __create_inner_node();

            inner->size() = (slot_size_type)(ceil_half);
            tmp_inst->size() = (slot_size_type)(floor_half);

            ikey_type next_key;

            if (pos <= ceil_half) {
                next_key = keys[ceil_half - 1];

                btree_detail::copy<floor_half, floor_half>(keys + ceil_half, keys + node_size,
                                                           tmp_inst->m_keys);
                btree_detail::copy<floor_half + 1, floor_half + 1>(
                    sons + ceil_half, sons + node_size + 1, tmp_inst->m_sons);
                btree_detail::copy_backward<0, ceil_half>(keys + pos - 1, keys + ceil_half - 1,
                                                          keys + ceil_half);
                btree_detail::copy_backward<0, ceil_half>(sons + pos, sons + ceil_half,
                                                          sons + ceil_half + 1);

                keys[pos - 1] = key;
                sons[pos] = inst;

                inst->pos() = pos;
                for (slot_usize_type i = pos + 1; i <= ceil_half; ++i) {
                    sons[i]->pos() = i;
                }
            } else {
                if (pos == ceil_half + 1) {
                    next_key = key;

                    btree_detail::copy<floor_half, floor_half>(keys + ceil_half, keys + node_size,
                                                               tmp_inst->m_keys);
                    btree_detail::copy<floor_half, floor_half>(
                        sons + ceil_half + 1, sons + node_size + 1, tmp_inst->m_sons + 1);

                    tmp_inst->m_sons[0] = inst;
                } else {
                    next_key = keys[ceil_half];

                    btree_detail::copy<0, floor_half - 1>(keys + ceil_half + 1, keys + pos - 1,
                                                          tmp_inst->m_keys);
                    btree_detail::copy<1, floor_half>(sons + ceil_half + 1, sons + pos,
                                                      tmp_inst->m_sons);

                    const slot_usize_type rpos = pos - ceil_half - 1;

                    btree_detail::copy<0, floor_half - 1>(keys + pos - 1, keys + node_size,
                                                          tmp_inst->m_keys + rpos);
                    btree_detail::copy<0, floor_half - 1>(sons + pos, sons + node_size + 1,
                                                          tmp_inst->m_sons + rpos + 1);

                    tmp_inst->m_keys[rpos - 1] = key;
                    tmp_inst->m_sons[rpos] = inst;
                }
            }

            for (slot_usize_type i = 0; i <= floor_half; ++i) {
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

template <typename Traits>
void basic_btree<Traits>::__rec_erase_iter(node_type *parent, slot_usize_type par_pos,
                                           slot_usize_type par_size) noexcept {
    constexpr slot_usize_type merge_size = floor_half * 2;

    slot_usize_type pos;
    slot_usize_type cur_size;
    node_type *current;

    current = parent;
    pos = par_pos;
    cur_size = par_size;
    parent = current->m_parent;

    while (parent != nullptr) {
        WJR_ASSERT_ASSUME(pos > 0);

        const auto inner = current->as_inner();

        auto *const keys = inner->m_keys;
        node_type **const sons = inner->m_sons;

        if (cur_size > floor_half) {
            btree_detail::copy<0, node_size - 1>(keys + pos, keys + cur_size, keys + pos - 1);
            btree_detail::copy<0, node_size - 1>(sons + pos + 1, sons + cur_size + 1, sons + pos);

            for (slot_usize_type i = pos; i < cur_size; ++i) {
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

        slot_usize_type next_size = __init_remove_rotate(par_inner, par_pos, par_size, lhs, rhs);

        do {
            if (lhs != nullptr) {
                rhs = inner;

                if (next_size == floor_half) {
                    btree_detail::copy<0, floor_half - 1>(keys, keys + pos - 1,
                                                          lhs->m_keys + floor_half + 1);
                    btree_detail::copy<1, floor_half>(sons, sons + pos,
                                                      lhs->m_sons + floor_half + 1);
                    btree_detail::copy<0, floor_half - 1>(keys + pos, keys + floor_half,
                                                          lhs->m_keys + floor_half + pos);
                    btree_detail::copy<0, floor_half - 1>(sons + pos + 1, sons + floor_half + 1,
                                                          lhs->m_sons + floor_half + pos + 1);

                    for (slot_usize_type i = floor_half; i <= merge_size; ++i) {
                        lhs->m_sons[i]->m_parent = lhs;
                        lhs->m_sons[i]->pos() = i;
                    }

                    lhs->m_keys[floor_half] = par_inner->m_keys[par_pos - 1];
                    break;
                }

                const slot_usize_type moved_elements = (next_size - floor_half + 1) / 2;

                ikey_type key = lhs->m_keys[next_size - moved_elements];

                if (moved_elements != 1) {
                    btree_detail::copy_backward<0, floor_half - 1>(
                        keys + pos, keys + floor_half, keys + floor_half + moved_elements - 1);
                    btree_detail::copy_backward<0, floor_half - 1>(
                        sons + pos + 1, sons + floor_half + 1, sons + floor_half + moved_elements);
                    for (slot_usize_type i = pos + moved_elements; i < floor_half + moved_elements;
                         ++i) {
                        sons[i]->pos() = i;
                    }
                }

                btree_detail::copy_backward<0, floor_half - 1>(keys, keys + pos - 1,
                                                               keys + pos + moved_elements - 1);
                btree_detail::copy_backward<1, floor_half>(sons, sons + pos,
                                                           sons + pos + moved_elements);
                btree_detail::copy<0, max_moved_elements - 1>(
                    lhs->m_keys + next_size - moved_elements + 1, lhs->m_keys + next_size, keys);
                btree_detail::copy<1, max_moved_elements>(lhs->m_sons + next_size - moved_elements +
                                                              1,
                                                          lhs->m_sons + next_size + 1, sons);

                keys[moved_elements - 1] = par_inner->m_keys[par_pos - 1];
                par_inner->m_keys[par_pos - 1] = key;

                for (slot_usize_type i = 0; i < moved_elements; ++i) {
                    sons[i]->m_parent = inner;
                    sons[i]->pos() = i;
                }

                for (slot_usize_type i = moved_elements; i < pos + moved_elements; ++i) {
                    sons[i]->pos() = i;
                }

                lhs->size() = next_size - moved_elements;
                inner->size() = floor_half + moved_elements - 1;
            } else {
                WJR_ASSERT_ASSUME(rhs != nullptr);

                lhs = inner;

                if (next_size == floor_half) {
                    btree_detail::copy<0, floor_half - 1>(keys + pos, keys + floor_half,
                                                          keys + pos - 1);
                    btree_detail::copy<0, floor_half - 1>(sons + pos + 1, sons + floor_half + 1,
                                                          sons + pos);
                    btree_detail::copy<0, floor_half>(rhs->m_keys, rhs->m_keys + floor_half,
                                                      keys + floor_half);
                    btree_detail::copy<0, floor_half + 1>(rhs->m_sons, rhs->m_sons + floor_half + 1,
                                                          sons + floor_half);

                    for (slot_usize_type i = pos; i < floor_half; ++i) {
                        inner->m_sons[i]->pos() = i;
                    }

                    for (slot_usize_type i = floor_half; i <= merge_size; ++i) {
                        inner->m_sons[i]->m_parent = inner;
                        inner->m_sons[i]->pos() = i;
                    }

                    lhs->m_keys[floor_half - 1] = par_inner->m_keys[par_pos];
                    ++par_pos;
                    break;
                }

                const slot_usize_type moved_elements = (next_size - floor_half + 1) / 2;

                ikey_type key = rhs->m_keys[moved_elements - 1];

                btree_detail::copy<0, floor_half - 1>(keys + pos, keys + floor_half,
                                                      keys + pos - 1);
                btree_detail::copy<0, floor_half - 1>(sons + pos + 1, sons + floor_half + 1,
                                                      sons + pos);
                btree_detail::copy<0, max_moved_elements - 1>(
                    rhs->m_keys, rhs->m_keys + moved_elements - 1, keys + floor_half);
                btree_detail::copy<1, max_moved_elements>(rhs->m_sons, rhs->m_sons + moved_elements,
                                                          sons + floor_half);
                btree_detail::copy<node_size - max_moved_elements, node_size - 1>(
                    rhs->m_keys + moved_elements, rhs->m_keys + next_size, rhs->m_keys);
                btree_detail::copy<node_size - max_moved_elements + 1, node_size>(
                    rhs->m_sons + moved_elements, rhs->m_sons + next_size + 1, rhs->m_sons);

                keys[floor_half - 1] = par_inner->m_keys[par_pos];
                par_inner->m_keys[par_pos] = key;

                for (slot_usize_type i = pos; i < floor_half; ++i) {
                    sons[i]->pos() = i;
                }

                for (slot_usize_type i = floor_half; i < floor_half + moved_elements; ++i) {
                    sons[i]->m_parent = inner;
                    sons[i]->pos() = i;
                }

                for (slot_usize_type i = 0; i <= next_size - moved_elements; ++i) {
                    rhs->m_sons[i]->pos() = i;
                }

                rhs->size() = next_size - moved_elements;
                inner->size() = floor_half + moved_elements - 1;
            }

            return;
        } while (0);

        lhs->size() = merge_size;
        __drop_inner_node(rhs);

        pos = par_pos;
        cur_size = par_size;
        current = parent;
        parent = current->m_parent;
    }

    const auto inner = current->as_inner();

    if (cur_size == 1) {
        node_type *root = inner->m_sons[0];
        __drop_inner_node(inner);
        __get_root() = root;
        root->m_parent = nullptr;
        return;
    }

    btree_detail::copy<0, node_size>(inner->m_keys + pos, inner->m_keys + cur_size,
                                     inner->m_keys + pos - 1);
    btree_detail::copy<0, node_size>(inner->m_sons + pos + 1, inner->m_sons + cur_size + 1,
                                     inner->m_sons + pos);

    for (slot_usize_type i = pos; i < cur_size; ++i) {
        inner->m_sons[i]->pos() = i;
    }

    inner->size() = cur_size - 1;
}

template <typename Traits>
typename basic_btree<Traits>::iterator
basic_btree<Traits>::__erase_iter(const_iterator iter) noexcept {
    --__get_size();

    constexpr slot_usize_type merge_size = floor_half * 2;

    leaf_node_type *leaf = iter.get_leaf();
    slot_usize_type pos = iter.pos();
    slot_usize_type cur_size = -leaf->size();
    node_type *parent = leaf->m_parent;

    __drop_node(leaf->m_values[pos]);

    if (WJR_LIKELY(cur_size > floor_half)) {
        leaf->template copy<0, node_size - 1>(pos + 1, cur_size, pos, leaf);
        leaf->size() = -(cur_size - 1);

        // first key in leaf is changed
        if (pos == 0 && parent != nullptr) {
            node_type *current = leaf;
            slot_usize_type tmp_pos;

            do {
                tmp_pos = current->pos();
                current = parent;
                parent = current->m_parent;

                if (tmp_pos != 0) {
                    current->as_inner()->m_keys[tmp_pos - 1] = to_ikey(leaf->get_key(0));
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

        leaf->template copy<0, node_size - 1>(pos + 1, cur_size, pos, leaf);
        leaf->size() = -(cur_size - 1);
        return iterator(leaf, pos).__adjust_next();
    }

    WJR_ASSERT_ASSUME(cur_size == floor_half);

    const auto inner = parent->as_inner();
    slot_usize_type par_pos = leaf->pos();
    cur_size = inner->size();
    leaf_node_type *lhs;
    leaf_node_type *rhs;

    const slot_usize_type next_size = __init_remove_rotate(inner, par_pos, cur_size, lhs, rhs);

    do {
        if (lhs != nullptr) {
            rhs = leaf;

            if (next_size == floor_half) {
                leaf->template copy<0, floor_half>(0, pos, floor_half, lhs);
                leaf->template copy<0, floor_half>(pos + 1, floor_half, pos + floor_half, lhs);

                leaf = lhs;
                pos += floor_half;
                break;
            }

            const slot_usize_type moved_elements = (next_size - floor_half + 1) / 2;

            if (moved_elements != 1) {
                leaf->template copy_backward<0, floor_half>(pos + 1, floor_half,
                                                            floor_half + moved_elements - 1, leaf);
            }

            leaf->template copy_backward<0, floor_half>(0, pos, pos + moved_elements, leaf);
            lhs->template copy<1, max_moved_elements>(next_size - moved_elements, next_size, 0,
                                                      leaf);

            lhs->size() = -(next_size - moved_elements);
            leaf->size() = -(floor_half + moved_elements - 1);

            pos += moved_elements;
        } else {
            WJR_ASSERT_ASSUME(rhs != nullptr);

            lhs = leaf;

            leaf->template copy<0, floor_half>(pos + 1, floor_half, pos, leaf);

            // merge rhs to leaf, and pos of iter is zero, then
            // need to update key in parent
            if (pos == 0) {
                node_type *current = leaf;

                slot_usize_type tmp_pos;
                node_type *tmp_parent = parent;

                do {
                    tmp_pos = current->pos();
                    current = tmp_parent;
                    tmp_parent = current->m_parent;
                } while (tmp_pos == 0 && tmp_parent != nullptr);

                if (tmp_pos != 0) {
                    current->as_inner()->m_keys[tmp_pos - 1] = to_ikey(leaf->get_key(0));
                }
            }

            if (next_size == floor_half) {
                rhs->template copy<0, floor_half>(0, floor_half, floor_half - 1, leaf);

                ++par_pos;
                break;
            }

            const slot_usize_type moved_elements = (next_size - floor_half + 1) / 2;

            rhs->template copy<1, max_moved_elements>(0, moved_elements, floor_half - 1, leaf);
            rhs->template copy<1, node_size - max_moved_elements>(moved_elements, next_size, 0,
                                                                  rhs);

            rhs->size() = -(next_size - moved_elements);
            leaf->size() = -(floor_half + moved_elements - 1);
        }

        node_type *current = rhs;

        slot_usize_type tmp_pos = current->pos();
        current = parent;
        parent = current->m_parent;

        current->as_inner()->m_keys[tmp_pos - 1] = to_ikey(rhs->get_key(0));
        return iterator(leaf, pos).__adjust_next();
    } while (0);

    lhs->size() = -(merge_size - 1);
    rhs->remove();
    __drop_leaf_node(rhs);

    __rec_erase_iter(parent, par_pos, cur_size);
    return iterator(leaf, pos).__adjust_next();
}

} // namespace wjr

#endif // WJR_CONTAINER_BTREE_HPP__