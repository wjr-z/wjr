#ifndef WJR_CONTAINER_BTREE_HPP__
#define WJR_CONTAINER_BTREE_HPP__

/**
 * @file btree.hpp
 * @brief B+ tree engine with a compile-time slot capacity in [4, 64].
 *
 * @details The btree_map/set and multimap/multiset adapters share this engine.
 * NodeSize == 0 selects capacity from the stored slot types, which are at most
 * eight bytes on ordinary x64 targets, giving the existing default of 16.
 * Small trivial keys/values are inline; other values are stored by pointer.
 * Maps can therefore allocate each value separately from the tree nodes.
 *
 * The first two slots use storage inside the tree object. This avoids node
 * allocation, but indirect values still require separate allocations. A third
 * element creates an allocated leaf. Nodes use the same capacity for leaves and
 * inner separators, not a shared byte limit. Experimental byte configuration is
 * available separately in <wjr/container/experimental/byte_btree.hpp>.
 *
 * @note Insertion and deletion can invalidate iterators and references. Operations
 * declared noexcept terminate if a comparator, allocator or value operation
 * throws. These containers do not promise std::map/set exception guarantees or
 * reference stability. Capacity and key type alone do not establish a performance
 * advantage over the standard containers; measure the intended workload.
 *
 * @note Recursive/incomplete key declarations can specialize wjr::is_complete to
 * std::false_type, as required by the existing indirect-key trait path.
 */

#include <concepts>
#include <iterator>

#include <wjr/assert.hpp>
#include <wjr/compressed_pair.hpp>
#include <wjr/container/container_fn.hpp>
#include <wjr/container/detail/btree_slot.hpp>
#include <wjr/container/forward_list.hpp>
#include <wjr/container/list.hpp>
#include <wjr/memory/copy.hpp>
#include <wjr/memory/small_copy.hpp>
#include <wjr/memory/uninitialized.hpp>

namespace wjr {
namespace btree_detail {
struct transfer_reference_less : std::less<> {};

/// @brief Admit transparent lookup types comparable with keys in both directions.
/// @note Callability does not imply noexcept; B-tree query comparisons must not throw.
template <typename Compare, typename Key, typename Lookup>
concept transparent_lookup =
    has_compare_is_transparent_v<Compare> && !std::same_as<remove_cvref_t<Lookup>, Key> &&
    std::predicate<const Compare &, const Key &, const Lookup &> &&
    std::predicate<const Compare &, const Lookup &, const Key &>;

/// @brief Admit transparent erase queries without taking the iterator erase overload.
template <typename Lookup, typename Compare, typename Key, typename Iterator>
concept transparent_erase_lookup =
    transparent_lookup<Compare, Key, Lookup> && !std::convertible_to<const Lookup &, Iterator>;

/// @brief Restrict the fixed-capacity integer search to cheap, same-key set comparisons.
template <typename Key, typename Value, typename Compare, typename Lookup>
concept standard_integral_set_lookup =
    std::integral<Key> && std::same_as<Key, Value> && std::same_as<Key, Lookup> &&
    (std::same_as<Compare, std::less<Key>> || std::same_as<Compare, std::less<>>);

/// @brief Single-pass input whose references can construct stored values.
template <typename Iter, typename Value>
concept insert_iterator =
    std::input_iterator<Iter> && std::constructible_from<Value, std::iter_reference_t<Iter>>;

/// @brief Admit a mapped source for both insertion and assignment, without common-reference rules.
template <typename Source, typename Mapped>
concept upsert_source =
    std::constructible_from<Mapped, Source &&> &&
    requires(Mapped &target, Source &&source) { target = std::forward<Source>(source); };

/// @brief Permit default-order allocator convenience without hiding a same-type comparator.
template <typename Compare, typename Alloc>
concept default_allocator_order =
    std::default_initializable<Compare> && !std::same_as<Compare, Alloc>;

/**
 * @brief SIMD-optimized copy for small ranges within B+tree nodes
 *
 * Uses small_copy which leverages SSE/AVX instructions for better performance.
 * Critical for node split and merge operations.
 *
 * @tparam Min Minimum number of elements (compile-time optimization)
 * @tparam Max Maximum number of elements (compile-time optimization)
 */
template <size_t Min, size_t Max, typename T>
WJR_INTRINSIC_INLINE void copy(const T *first, const T *last, T *dest) noexcept {
    small_copy<Min, Max>(first, last, dest);
}

/**
 * @brief SIMD-optimized backward copy for overlapping ranges
 *
 * Used when source and destination overlap and we need to copy right-to-left.
 * Essential for element insertion where we shift existing elements right.
 */
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
struct is_inline_key {
    static constexpr bool value = false;
};

template <typename Key>
struct is_inline_key<Key, std::enable_if_t<is_complete_v<Key>>> {
    static constexpr bool value = std::is_trivial_v<Key> && sizeof(Key) <= 8;
};

template <typename Key>
inline constexpr bool is_inline_key_v = is_inline_key<Key>::value;

template <typename Key, typename Value>
struct btree_inline_traits {
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
struct btree_inline_traits<Key, void> {
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

/// @brief Compare map values by key while retaining a copy of comparator state.
template <typename Traits>
class map_value_compare {
    using key_compare = typename Traits::key_compare;
    using value_type = typename Traits::value_type;

public:
    explicit map_value_compare(const key_compare &compare) noexcept(
        std::is_nothrow_copy_constructible_v<key_compare>)
        : m_compare(compare) {}

    bool operator()(const value_type &lhs, const value_type &rhs) const
        noexcept(noexcept(m_compare(Traits::get_key(lhs), Traits::get_key(rhs)))) {
        return m_compare(Traits::get_key(lhs), Traits::get_key(rhs));
    }

private:
    key_compare m_compare;
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
          typename Alloc = std::allocator<char>, size_t NodeSize = 0>
struct btree_traits : btree_detail::btree_inline_traits<Key, Value> {
private:
    using Mybase = btree_detail::btree_inline_traits<Key, Value>;

public:
    // Allocator
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<char>;
    using _Alty_traits = std::allocator_traits<_Alty>;
    using storage_fn_type = container_fn<_Alty>;

    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;

    // Value types
    using key_type = typename Mybase::key_type;
    using mapped_type = typename Mybase::mapped_type;
    using value_type = typename Mybase::value_type;
    using key_compare = Compare;

    // Inline storage: trivial types <= 8 bytes are kept directly in nodes;
    // larger types are stored by pointer.
    static constexpr bool is_inline_key = btree_detail::is_inline_key_v<key_type>;
    static constexpr bool is_inline_value = btree_detail::is_inline_key_v<value_type>;

    using ikey_type = std::conditional_t<is_inline_key, key_type, const key_type *>;
    using ivalue_type = std::conditional_t<is_inline_value, value_type, value_type *>;

    // Structural
    static constexpr bool is_map = Mybase::is_map;
    static constexpr bool multi = Multi;

    using node_type = btree_node<btree_traits>;
    using inner_node_type = btree_inner_node<btree_traits>;
    using leaf_node_type = btree_leaf_node<btree_traits>;

    using slot_size_type = int;
    using slot_usize_type = unsigned int;

private:
    // Auto-select node_size based on inline element size:
    //   <= 8 bytes  ->  16 keys/node  (better cache utilization)
    //   >  8 bytes  ->   8 keys/node  (balanced)
    static constexpr size_t _auto_node_size() noexcept {
        if constexpr (NodeSize != 0) {
            return NodeSize;
        } else {
            constexpr size_t max_stored = std::max(sizeof(ikey_type), sizeof(ivalue_type));
            return max_stored <= 8 ? 16 : 8;
        }
    }

public:
    static constexpr size_t node_size = _auto_node_size();
    static_assert(node_size >= 4 && node_size <= 64, "node_size must be in range [4, 64]");

    // Inline/pointer conversion helpers
    WJR_INTRINSIC_INLINE static key_type &from_ikey(ikey_type &key) noexcept {
        if constexpr (is_inline_key) {
            return key;
        } else {
            return *key;
        }
    }

    WJR_INTRINSIC_INLINE static const key_type &from_ikey(const ikey_type &key) noexcept {
        if constexpr (is_inline_key) {
            return key;
        } else {
            return *key;
        }
    }

    WJR_INTRINSIC_INLINE static ikey_type to_ikey(const key_type &key) noexcept {
        if constexpr (is_inline_key) {
            return key;
        } else {
            return std::addressof(key);
        }
    }

    WJR_INTRINSIC_INLINE static value_type &from_ivalue(ivalue_type &value) noexcept {
        if constexpr (is_inline_value) {
            return value;
        } else {
            return *value;
        }
    }

    WJR_INTRINSIC_INLINE static const value_type &from_ivalue(const ivalue_type &value) noexcept {
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
    struct _normal_node {
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
        _normal_node m_node;
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
class alignas(std::max(size_t(16), alignof(typename Traits::ivalue_type))) btree_leaf_node
    : public btree_node<Traits>,
      public intrusive::list_node<btree_leaf_node<Traits>> {
private:
    using Mybase = btree_node<Traits>;
    using key_type = typename Traits::key_type;
    using value_type = typename Traits::value_type;

public:
    using ivalue_type = typename Traits::ivalue_type;
    using slot_ops = btree_detail::slot_ops<Traits>;
    using slot_allocator_type = typename Traits::_Alty;
    using slot_usize_type = typename Traits::slot_usize_type;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(btree_leaf_node);

    constexpr btree_leaf_node(btree_node_constructor_t) noexcept : Mybase(btree_node_constructor) {}

    /**
     * @brief Get the key at specified position
     *
     * Handles both inline and pointer-based value storage.
     * For map types, extracts key from key-value pair.
     *
     * @param pos Position in the node (0 to size-1)
     * @return Reference to the key
     */
    WJR_INTRINSIC_INLINE const key_type &get_key(slot_usize_type pos) const noexcept {
        return Traits::get_key(Traits::from_ivalue(m_values[pos]));
    }

    /**
     * @brief Assign a value to a position in the leaf node
     *
     * Simple assignment operation, no construction/destruction.
     * Value must be already constructed.
     *
     * @param idx Target position
     * @param value Value to assign (may be pointer or inline value)
     */
    WJR_INTRINSIC_INLINE void assign(slot_usize_type idx, typename slot_ops::insert_type value,
                                     slot_allocator_type &al) noexcept {
        slot_ops::assign(m_values + idx, value, al);
    }

    /// @brief Transfer an existing slot into an unoccupied destination.
    WJR_INTRINSIC_INLINE void transfer_one(slot_usize_type idx, ivalue_type *src,
                                           slot_allocator_type &al) noexcept {
        slot_ops::transfer_one(src, m_values + idx, al);
    }

    /**
     * @brief SIMD-optimized copy of value range
     *
     * Copies values from [start, end) to dst starting at dst_start.
     * Uses small_copy for optimal performance on small ranges.
     *
     * @tparam Min Minimum expected range size (for optimization)
     * @tparam Max Maximum expected range size (for optimization)
     * @param start Source start position (inclusive)
     * @param end Source end position (exclusive)
     * @param dst_start Destination start position
     * @param dst Destination leaf node (can be this)
     */
    template <size_t Min = 0, size_t Max = SIZE_MAX>
    WJR_INTRINSIC_INLINE void copy(slot_usize_type start, slot_usize_type end,
                                   slot_usize_type dst_start, btree_leaf_node *dst,
                                   slot_allocator_type &al) noexcept {
        slot_ops::template transfer<Min, Max>(m_values + start, m_values + end,
                                              dst->m_values + dst_start, al);
    }

    /**
     * @brief SIMD-optimized backward copy for overlapping ranges
     *
     * Copies values from [start, end) to dst ending at dst_end.
     * Copies in reverse order, safe for overlapping src/dst when dst > src.
     * Used when shifting elements right for insertion.
     *
     * @tparam Min Minimum expected range size (for optimization)
     * @tparam Max Maximum expected range size (for optimization)
     * @param start Source start position (inclusive)
     * @param end Source end position (exclusive)
     * @param dst_end Destination end position (exclusive)
     * @param dst Destination leaf node (can be this)
     */
    template <size_t Min = 0, size_t Max = SIZE_MAX>
    WJR_INTRINSIC_INLINE void copy_backward(slot_usize_type start, slot_usize_type end,
                                            slot_usize_type dst_end, btree_leaf_node *dst,
                                            slot_allocator_type &al) noexcept {
        slot_ops::template transfer_backward<Min, Max>(m_values + start, m_values + end,
                                                       dst->m_values + dst_end, al);
    }

#if WJR_HAS_GCC(6, 0, 0)
    alignas(std::max(size_t(16), alignof(ivalue_type))) ivalue_type m_values[];
#else
    alignas(std::max(size_t(16), alignof(ivalue_type))) ivalue_type m_values[0];
#endif
};

namespace btree_detail {
/** @brief Exact allocation storage shared by the tree and its layout diagnostics. */
template <typename Traits>
struct alignas(typename Traits::leaf_node_type) leaf_storage {
    unsigned char bytes[sizeof(typename Traits::leaf_node_type) +
                        sizeof(typename Traits::ivalue_type) * Traits::node_size];
};
} // namespace btree_detail

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

    ~btree_root_node() noexcept { std::destroy_at(_get_base()); }

    constexpr btree_root_node(btree_node_constructor_t) noexcept : m_base(btree_node_constructor) {}

    Mybase *_get_base() noexcept { return std::addressof(m_base); }
    const Mybase *_get_base() const noexcept { return std::addressof(m_base); }

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
        return _adjust_next();
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
            m_leaf = m_leaf->prev()->self();
            m_pos = _get_usize() - 1;
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

    WJR_PURE slot_usize_type pos() const noexcept { return m_pos; }

protected:
    WJR_PURE slot_usize_type _get_usize() const noexcept {
        return static_cast<slot_usize_type>(-m_leaf->size());
    }

    WJR_INTRINSIC_INLINE btree_const_iterator &_adjust_next() noexcept {
        if (m_pos == _get_usize()) {
            m_leaf = static_cast<leaf_node_type *>(m_leaf->next());
            m_pos = 0;
        }

        return *this;
    }

private:
    /// @brief Node access is reserved for the owning tree's mutation paths.
    WJR_PURE leaf_node_type *get_leaf() const noexcept { return m_leaf; }

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
    using pointer = std::conditional_t<Traits::is_map, value_type *, const value_type *>;
    using reference = std::conditional_t<Traits::is_map, value_type &, const value_type &>;

private:
    /// @brief Only the owning mutable tree may recover a mutable position.
    btree_iterator(const Mybase &other) noexcept : Mybase(other) {}

protected:
    btree_iterator(const list_node_type *list_node, slot_usize_type pos) noexcept
        : Mybase(list_node, pos) {}

    btree_iterator(const leaf_node_type *leaf, slot_usize_type pos) noexcept : Mybase(leaf, pos) {}

public:
    reference operator*() const noexcept { return *operator->(); }

    pointer operator->() const noexcept { return const_cast<pointer>(Mybase::operator->()); }

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

    bool operator==(const Mybase &other) const noexcept { return Mybase::operator==(other); }
    bool operator!=(const Mybase &other) const noexcept { return Mybase::operator!=(other); }

protected:
    WJR_INTRINSIC_INLINE btree_iterator &_adjust_next() noexcept {
        Mybase::_adjust_next();
        return *this;
    }
};

template <size_t N, typename Enable = void>
struct basic_btree_searcher_impl;

namespace btree_detail {
/**
 * @brief Linear search with step=2 optimization for small nodes
 *
 * This search strategy provides optimal performance for node_size <= 8:
 * 1. Step by 2 to reduce branch mispredictions
 * 2. Check backward one position when found (50% probability)
 * 3. Better instruction-level parallelism
 *
 * Average comparisons: ~3.5 for node_size=8
 *
 * @param size Number of elements to search (1 to node_size)
 * @param offset Starting position for search
 * @param comp Comparison lambda: comp(i) returns true if key < element[i]
 * @return Position where key should be inserted
 */
template <typename Compare>
WJR_INTRINSIC_INLINE unsigned int linear_search_step2(unsigned int size, unsigned int offset,
                                                      const Compare &comp) {
    unsigned int i = offset + 1;

    // Step by 2 for better ILP (instruction-level parallelism)
    while (WJR_LIKELY(i < size)) {
        if (comp(i)) {
            // Found element >= key, check previous position
            // This gives ~50% probability of returning i-1
            return i - (int)comp(i - 1);
        }
        i += 2;
    }

    // Handle boundary: either at end or last element needs checking
    return i - (int)(size != i || comp(i - 1));
}
} // namespace btree_detail

/**
 * @brief Search implementation for small node sizes (N <= 8)
 *
 * Uses simple linear search with step=2 optimization.
 * Best performance for node sizes in range [4, 8].
 *
 * Performance characteristics:
 * - Average case: ~3.5 comparisons for node_size=8
 * - Best case: 1-2 comparisons
 * - Worst case: ~4 comparisons
 * - Excellent cache behavior
 */
template <size_t N>
struct basic_btree_searcher_impl<N, std::enable_if_t<(N <= 8)>> {
    static constexpr size_t node_size = N;

public:
    template <size_t Min, typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search(unsigned int size,
                                                    const Compare &comp) noexcept {
        static_assert(Min == 1, "Minimum search size must be 1");

        WJR_ASSERT_ASSUME_L2(size >= Min);
        WJR_ASSERT_ASSUME_L2(size <= node_size);

        return btree_detail::linear_search_step2(size, 0, comp);
    }
};

/**
 * @brief Search implementation for medium node sizes (8 < N <= 16)
 *
 * Hybrid approach combining mid-point check with linear search:
 * 1. Check mid-point to determine which half to search
 * 2. Use linear search (step=2) within selected half
 *
 * This balances:
 * - Reducing average comparisons (via mid-point check)
 * - Maintaining good branch prediction (via linear search)
 * - Excellent cache locality
 *
 * Best performance for node sizes in range [9, 16].
 *
 * Performance characteristics:
 * - Average case: ~4.5 comparisons for node_size=16
 * - Better than pure linear (would be ~8)
 * - Similar to binary search but better branch prediction
 */
template <size_t N>
struct basic_btree_searcher_impl<N, std::enable_if_t<(N > 8 && N <= 16)>> {
    static constexpr size_t node_size = N;

private:
    template <typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int hybrid_search_impl(unsigned int size,
                                                                const Compare &comp) noexcept {
        constexpr unsigned int mid = node_size / 2;

        // Determine which half to search based on mid-point comparison
        unsigned int offset;
        if (size <= mid || comp(mid)) {
            offset = 0; // Search lower half [0, mid)
        } else {
            offset = mid; // Search upper half [mid, size)
        }

        // Use linear search within selected half
        return btree_detail::linear_search_step2(size, offset, comp);
    }

public:
    template <size_t Min, typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search(unsigned int size,
                                                    const Compare &comp) noexcept {
        static_assert(Min == 1, "Minimum search size must be 1");

        WJR_ASSERT_ASSUME_L2(size >= Min);
        WJR_ASSERT_ASSUME_L2(size <= node_size);

        return hybrid_search_impl(size, comp);
    }
};

/**
 * @brief Search implementation for large node sizes (N > 16)
 *
 * Uses standard binary search for optimal O(log N) performance.
 * For large node sizes, the logarithmic advantage outweighs
 * the slightly worse branch prediction.
 *
 * Best performance for node sizes >= 17.
 *
 * Performance characteristics:
 * - Average case: ~log2(N) comparisons
 * - For node_size=32: ~5 comparisons
 * - For node_size=64: ~6 comparisons
 * - Predictable worst case
 *
 * @note For very large node sizes (> 32), cache miss cost may dominate
 */
template <size_t N>
struct basic_btree_searcher_impl<N, std::enable_if_t<(N > 16)>> {
    static constexpr size_t node_size = N;

private:
    template <typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int binary_search_impl(unsigned int size,
                                                                const Compare &comp) noexcept {
        unsigned int L = 0, R = size;

        // Standard binary search: find first position where comp(mid) is true
        while (L < R) {
            unsigned int mid = (L + R) >> 1; // Equivalent to (L + R) / 2

            if (comp(mid)) {
                R = mid; // Key < element[mid], search left half
            } else {
                L = mid + 1; // Key >= element[mid], search right half
            }
        }

        return R; // Returns insertion position
    }

public:
    template <size_t Min, typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search(unsigned int size,
                                                    const Compare &comp) noexcept {
        static_assert(Min == 1, "Minimum search size must be 1");

        WJR_ASSERT_ASSUME(size >= Min);
        WJR_ASSERT_ASSUME(size <= node_size);

        return binary_search_impl(size, comp);
    }
};

namespace btree_detail {
/// @brief Search a 32-slot integer set node using two pivots and the existing step-two scan.
/// @pre 1 <= size <= 32; comp is false before its insertion position and true afterwards.
struct integral_set_searcher32 {
    template <size_t Min, typename Compare>
    WJR_INTRINSIC_INLINE static unsigned int search(unsigned int size,
                                                    const Compare &comp) noexcept {
        static_assert(Min == 1, "Minimum search size must be 1");
        WJR_ASSERT(size >= Min);
        WJR_ASSERT(size <= 32);

        unsigned int offset = 0;
        if (size > 16 && !comp(16)) {
            offset = 16;
        }
        if (size > offset + 8 && !comp(offset + 8)) {
            offset += 8;
        }
        return linear_search_step2(size, offset, comp);
    }
};
} // namespace btree_detail

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

    using slot_ops = btree_detail::slot_ops<Traits>;
    using insert_type = typename slot_ops::insert_type;
    static constexpr bool compact_slots = btree_detail::compact_slots_v<Traits>;

public:
    using key_type = typename Traits::key_type;
    using mapped_type = typename Traits::mapped_type;
    using value_type = typename Traits::value_type;
    using key_compare = typename Traits::key_compare;
    using value_compare =
        std::conditional_t<Traits::is_map, btree_detail::map_value_compare<Traits>, key_compare>;
    using allocator_type = _Alty;
    using size_type = typename Traits::size_type;
    using difference_type = typename Traits::difference_type;
    using reference = std::conditional_t<Traits::is_map, value_type &, const value_type &>;
    using const_reference = const value_type &;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using iterator = btree_iterator<Traits>;
    using const_iterator = btree_const_iterator<Traits>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    WJR_INTRINSIC_INLINE static key_type &from_ikey(ikey_type &key) noexcept {
        return Traits::from_ikey(key);
    }
    WJR_INTRINSIC_INLINE static const key_type &from_ikey(const ikey_type &key) noexcept {
        return Traits::from_ikey(key);
    }
    WJR_INTRINSIC_INLINE static ikey_type to_ikey(const key_type &key) noexcept {
        return Traits::to_ikey(key);
    }
    WJR_INTRINSIC_INLINE static value_type &from_ivalue(ivalue_type &value) noexcept {
        return Traits::from_ivalue(value);
    }
    WJR_INTRINSIC_INLINE static const value_type &from_ivalue(const ivalue_type &value) noexcept {
        return Traits::from_ivalue(value);
    }

public:
    basic_btree() noexcept(std::is_nothrow_default_constructible_v<key_compare> &&
                           std::is_nothrow_default_constructible_v<_Alty>)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(),
                 wjr::forward_as_tuple(std::piecewise_construct, wjr::forward_as_tuple(),
                                       wjr::forward_as_tuple(btree_node_constructor))) {}

    explicit basic_btree(const key_compare &comp) noexcept(
        std::is_nothrow_copy_constructible_v<key_compare> &&
        std::is_nothrow_default_constructible_v<_Alty>)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(comp),
                 wjr::forward_as_tuple(std::piecewise_construct, wjr::forward_as_tuple(),
                                       wjr::forward_as_tuple(btree_node_constructor))) {}

    /// @brief Construct an empty tree using the supplied comparator and allocator instance.
    basic_btree(const key_compare &comp, const allocator_type &alloc) noexcept(
        std::is_nothrow_copy_constructible_v<key_compare> &&
        std::is_nothrow_copy_constructible_v<allocator_type>)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(comp),
                 wjr::forward_as_tuple(std::piecewise_construct, wjr::forward_as_tuple(alloc),
                                       wjr::forward_as_tuple(btree_node_constructor))) {}

    basic_btree(const basic_btree &other)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(other.key_comp()),
                 wjr::forward_as_tuple(
                     std::piecewise_construct,
                     wjr::tuple<_Alty &&>(_Alty_traits::select_on_container_copy_construction(
                         other._get_allocator())),
                     wjr::forward_as_tuple(btree_node_constructor))) {
        _copy_assign_tree(other);
    }

    basic_btree(basic_btree &&other) noexcept(std::is_nothrow_move_constructible_v<key_compare> &&
                                              std::is_nothrow_move_constructible_v<_Alty>)
        : m_pair(std::piecewise_construct, wjr::tuple<key_compare &&>(std::move(other.key_comp())),
                 wjr::forward_as_tuple(std::piecewise_construct,
                                       wjr::tuple<_Alty &&>(std::move(other._get_allocator())),
                                       wjr::forward_as_tuple(btree_node_constructor))) {
        _move_tree(std::move(other));
    }

    /// @brief Copy values using the supplied allocator rather than allocator copy selection.
    basic_btree(const basic_btree &other,
                const allocator_type &alloc) noexcept(noexcept(basic_btree(other.key_comp(),
                                                                           alloc)))

        : basic_btree(other.key_comp(), alloc) {
        _copy_assign_tree(other);
    }

    /// @brief Transfer compatible storage, or migrate elements and clear the source through its
    /// owner.
    basic_btree(basic_btree &&other, const allocator_type &alloc) noexcept(
        _Alty_traits::is_always_equal::value && std::is_nothrow_move_constructible_v<key_compare> &&
        std::is_nothrow_copy_constructible_v<allocator_type>)

        : m_pair(std::piecewise_construct, wjr::tuple<key_compare &&>(std::move(other.key_comp())),
                 wjr::forward_as_tuple(std::piecewise_construct, wjr::forward_as_tuple(alloc),
                                       wjr::forward_as_tuple(btree_node_constructor))) {
        if constexpr (_Alty_traits::is_always_equal::value) {
            _move_tree(std::move(other));
        } else if (_get_allocator() == other._get_allocator()) {
            _move_tree(std::move(other));
        } else {
            _move_assign_tree(std::move(other));
        }
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
            if constexpr (multi) {
                _emplace_multi(item);
            } else {
                _emplace_unique(item);
            }
        }
        return *this;
    }

    ~basic_btree() noexcept { _destroy_and_deallocate(); }

    constexpr key_compare &key_comp() noexcept { return m_pair.first(); }
    constexpr const key_compare &key_comp() const noexcept { return m_pair.first(); }

    /// @brief Return a value comparator with the tree's current ordering state.
    value_compare value_comp() const noexcept(std::is_nothrow_copy_constructible_v<key_compare>) {
        return value_compare(key_comp());
    }

    /// @brief Return a copy of the allocator used by this tree.
    allocator_type get_allocator() const
        noexcept(std::is_nothrow_copy_constructible_v<allocator_type>) {
        return _get_allocator();
    }

    iterator begin() noexcept { return iterator(_get_sentry()->next(), 0); }
    const_iterator begin() const noexcept { return const_iterator(_get_sentry()->next(), 0); }
    const_iterator cbegin() const noexcept { return const_iterator(_get_sentry()->next(), 0); }

    iterator end() noexcept {
        return iterator(_get_sentry(), _get_root() == nullptr ? _get_size() : 0);
    }
    const_iterator end() const noexcept {
        return const_iterator(_get_sentry(), _get_root() == nullptr ? _get_size() : 0);
    }
    const_iterator cend() const noexcept {
        return const_iterator(_get_sentry(), _get_root() == nullptr ? _get_size() : 0);
    }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

    void clear() {
        _destroy_and_deallocate();
        _get_root() = nullptr;
        _get_size() = 0;
        _get_sentry()->init_self(); // Reset sentry node pointers
    }

    void swap(basic_btree &other) {
        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_fn_type::swap(*this, other);
        }
    }

protected:
    using _Leaf_storage = btree_detail::leaf_storage<Traits>;

    template <typename T>
    T *_allocate_object() noexcept {
        using allocator = typename _Alty_traits::template rebind_alloc<T>;
        allocator al(_get_allocator());
        return std::allocator_traits<allocator>::allocate(al, 1);
    }

    template <typename T>
    void _deallocate_object(T *ptr) noexcept {
        using allocator = typename _Alty_traits::template rebind_alloc<T>;
        allocator al(_get_allocator());
        std::allocator_traits<allocator>::deallocate(al, ptr, 1);
    }

    inner_node_type *_create_inner_node() noexcept {
        auto &al = _get_allocator();
        auto *const node = _allocate_object<inner_node_type>();
        uninitialized_construct_using_allocator(node, al, default_construct);
        return node;
    }

    leaf_node_type *_create_leaf_node() noexcept {
        auto &al = _get_allocator();
        auto *const node = reinterpret_cast<leaf_node_type *>(_allocate_object<_Leaf_storage>());
        uninitialized_construct_using_allocator(node, al, default_construct);
        return node;
    }

    template <typename... Args>
    ivalue_type _create_node(Args &&...args) noexcept {
        ivalue_type node;
        auto &al = _get_allocator();
        if constexpr (Traits::is_inline_value) {
            uninitialized_construct_using_allocator(std::addressof(node), al,
                                                    std::forward<Args>(args)...);
        } else {
            node = _allocate_object<value_type>();
            uninitialized_construct_using_allocator(node, al, std::forward<Args>(args)...);
        }

        return node;
    }

    void _drop_inner_node(inner_node_type *node) noexcept {
        auto &al = _get_allocator();
        destroy_at_using_allocator(node, al);
        _deallocate_object(node);
    }

    void _drop_leaf_node(leaf_node_type *node) noexcept {
        auto &al = _get_allocator();
        destroy_at_using_allocator(node, al);
        _deallocate_object(reinterpret_cast<_Leaf_storage *>(node));
    }

    void _drop_node(ivalue_type node) noexcept {
        auto &al = _get_allocator();
        if constexpr (Traits::is_inline_value) {
            destroy_at_using_allocator(std::addressof(node), al);
        } else {
            destroy_at_using_allocator(node, al);
            _deallocate_object(node);
        }
    }

    void _destroy_slot(ivalue_type &slot) noexcept {
        if constexpr (compact_slots) {
            slot_ops::destroy(std::addressof(slot), _get_allocator());
        } else {
            _drop_node(slot);
        }
    }

    template <typename... Args>
    void _construct_leaf_value(leaf_node_type *leaf, slot_usize_type pos, Args &&...args) noexcept {
        auto &al = _get_allocator();
        if constexpr (compact_slots) {
            slot_ops::construct(leaf->m_values + pos, al, std::forward<Args>(args)...);
        } else {
            leaf->assign(pos, _create_node(std::forward<Args>(args)...), al);
        }
    }

    WJR_INTRINSIC_INLINE static iterator _get_same_iterator(const_iterator iter) noexcept {
        --iter.m_pos;
        return iterator(iter);
    }

    WJR_INTRINSIC_INLINE const_iterator _get_insert_multi_pos(const key_type &key) const noexcept {
        return _search<true, false>(key);
    }

    struct _hint_position {
        const_iterator position;
        bool duplicate;
    };

    /// @brief Preserve the insertion engine's separator invariant at a leaf boundary.
    const_iterator _before_hint(const_iterator position) const noexcept {
        if (position.pos() == 0 && position != cbegin()) {
            auto previous = position;
            --previous;
            return const_iterator(previous.get_leaf(), previous.pos() + 1);
        }
        return position;
    }

    /// @brief Validate the hinted gap and its immediate neighbor before using root search.
    /// @pre hint is a valid iterator belonging to this tree, including end().
    _hint_position _locate_hint(const_iterator hint, const key_type &key) noexcept {
        if (empty())
            return {hint, false};
        const auto &compare = key_comp();
        const auto finish = cend();
        if (hint == finish) {
            auto previous = hint;
            --previous;
            const auto &before = Traits::get_key(*previous);
            if (!compare(key, before)) {
                if constexpr (!multi) {
                    if (!compare(before, key))
                        return {previous, true};
                }
                return {_before_hint(hint), false};
            }
        } else if (!compare(Traits::get_key(*hint), key)) {
            if constexpr (!multi) {
                if (!compare(key, Traits::get_key(*hint)))
                    return {hint, true};
            }
            if (hint == cbegin())
                return {hint, false};
            auto previous = hint;
            --previous;
            const auto &before = Traits::get_key(*previous);
            if (!compare(key, before)) {
                if constexpr (!multi) {
                    if (!compare(before, key))
                        return {previous, true};
                }
                return {_before_hint(hint), false};
            }
        } else {
            auto next = hint;
            ++next;
            if (next == finish)
                return {_before_hint(next), false};
            const auto &after = Traits::get_key(*next);
            if (!compare(after, key)) {
                if constexpr (!multi) {
                    if (!compare(key, after))
                        return {next, true};
                }
                return {_before_hint(next), false};
            }
            /// An explicit multi hint before the group is closest to its lower bound.
            if constexpr (multi) {
                return {_before_hint(_search<false, false>(key)), false};
            }
        }
        const auto position = _get_insert_multi_pos(key);
        if constexpr (!multi) {
            if (_same_insert_key(position, key))
                return {_get_same_iterator(position), true};
        }
        return {position, false};
    }

    /// @brief Validate the next range gap, avoiding predecessor work for an unsorted input.
    /// @pre hint is end() initially, then the element returned by this range's previous insertion.
    WJR_INTRINSIC_INLINE _hint_position _locate_range_hint(const_iterator hint,
                                                           const key_type &key) noexcept {
        if (hint == cend())
            return _locate_hint(hint, key);
        const auto &compare = key_comp();
        const auto &before = Traits::get_key(*hint);
        if (compare(before, key)) {
            auto next = hint;
            ++next;
            if (next == cend())
                return {const_iterator(hint.get_leaf(), hint.pos() + 1), false};
            const auto &after = Traits::get_key(*next);
            if (!compare(after, key)) {
                if constexpr (!multi) {
                    if (!compare(key, after))
                        return {next, true};
                } else {
                    if (!compare(key, after))
                        return {_get_insert_multi_pos(key), false};
                }
                return {const_iterator(hint.get_leaf(), hint.pos() + 1), false};
            }
        } else if (!compare(key, before)) {
            if constexpr (!multi)
                return {hint, true};
            else {
                /// This cursor was inserted at the group's upper bound, so equivalent input
                /// appends.
                return {const_iterator(hint.get_leaf(), hint.pos() + 1), false};
            }
        }
        const auto position = _get_insert_multi_pos(key);
        if constexpr (!multi) {
            if (_same_insert_key(position, key))
                return {_get_same_iterator(position), true};
        }
        return {position, false};
    }

    template <bool Range>
    WJR_INTRINSIC_INLINE _hint_position _hint_location(const_iterator hint,
                                                       const key_type &key) noexcept {
        if constexpr (Range)
            return _locate_range_hint(hint, key);
        else
            return _locate_hint(hint, key);
    }

    template <bool Range = false, typename... Args>
    iterator _emplace_hint_multi(const_iterator hint, Args &&...args) noexcept {
        ivalue_type value = _create_node(std::forward<Args>(args)...);
        const auto location = _hint_location<Range>(hint, Traits::get_key(from_ivalue(value)));
        return _insert_iter(location.position, value);
    }

    template <bool Range = false, typename... Args>
    std::pair<iterator, bool> _emplace_hint_unique(const_iterator hint, Args &&...args) noexcept {
        using extractor = typename Traits::template inplace_key_extractor<remove_cvref_t<Args>...>;
        if constexpr (extractor::extractable) {
            const auto &key = extractor::extract(std::forward<Args>(args)...);
            const auto location = _hint_location<Range>(hint, key);
            if (location.duplicate)
                return {iterator(location.position), false};
            if constexpr (compact_slots) {
                btree_detail::compact_temporary<value_type, _Alty> value(
                    _get_allocator(), std::forward<Args>(args)...);
                return {_insert_iter(location.position, value.get()), true};
            } else {
                return {_insert_iter(location.position, _create_node(std::forward<Args>(args)...)),
                        true};
            }
        } else if constexpr (compact_slots) {
            btree_detail::compact_temporary<value_type, _Alty> value(_get_allocator(),
                                                                     std::forward<Args>(args)...);
            const auto location = _hint_location<Range>(hint, Traits::get_key(*value.get()));
            if (location.duplicate)
                return {iterator(location.position), false};
            return {_insert_iter(location.position, value.get()), true};
        } else {
            ivalue_type value = _create_node(std::forward<Args>(args)...);
            const auto location = _hint_location<Range>(hint, Traits::get_key(from_ivalue(value)));
            if (location.duplicate) {
                _drop_node(value);
                return {iterator(location.position), false};
            }
            return {_insert_iter(location.position, value), true};
        }
    }

    template <typename KeyArg, typename... Args>
    std::pair<iterator, bool> _try_emplace_hint_unique(const_iterator hint, KeyArg &&key,
                                                       Args &&...args) {
        const auto location = _locate_hint(hint, key);
        if (location.duplicate)
            return {iterator(location.position), false};
        if constexpr (compact_slots) {
            btree_detail::compact_temporary<value_type, _Alty> value(
                _get_allocator(), std::piecewise_construct,
                std::forward_as_tuple(std::forward<KeyArg>(key)),
                std::forward_as_tuple(std::forward<Args>(args)...));
            return {_insert_iter(location.position, value.get()), true};
        } else {
            ivalue_type value = _create_node(std::piecewise_construct,
                                             std::forward_as_tuple(std::forward<KeyArg>(key)),
                                             std::forward_as_tuple(std::forward<Args>(args)...));
            return {_insert_iter(location.position, value), true};
        }
    }

    template <typename... Args>
    iterator _emplace_multi(Args &&...args) noexcept {
        ivalue_type xval = _create_node(std::forward<Args>(args)...);
        const auto iter = _get_insert_multi_pos(Traits::get_key(from_ivalue(xval)));
        return _insert_iter(iter, xval);
    }

    WJR_INTRINSIC_INLINE bool _same_insert_key(const_iterator iter, const key_type &key) noexcept {
        auto pos = iter.pos();
        return pos != 0 && !key_comp()(iter.get_leaf()->get_key(pos - 1), key);
    }

    template <typename... Args>
    std::pair<iterator, bool> _emplace_unique(Args &&...args) noexcept {
        if constexpr (compact_slots) {
            return _emplace_compact_unique(std::forward<Args>(args)...);
        } else {
            using inplace_key_extractor =
                typename Traits::template inplace_key_extractor<remove_cvref_t<Args>...>;

            const_iterator iter;
            ivalue_type xval;

            if constexpr (inplace_key_extractor::extractable) {
                const auto &key = inplace_key_extractor::extract(std::forward<Args>(args)...);
                iter = _get_insert_multi_pos(key);

                if (WJR_UNLIKELY(_same_insert_key(iter, key))) {
                    return {_get_same_iterator(iter), false};
                }

                xval = _create_node(std::forward<Args>(args)...);
            } else {
                xval = _create_node(std::forward<Args>(args)...);
                const auto &key = Traits::get_key(from_ivalue(xval));
                iter = _get_insert_multi_pos(key);

                if (WJR_UNLIKELY(_same_insert_key(iter, key))) {
                    _drop_node(xval);
                    return {_get_same_iterator(iter), false};
                }
            }

            return {_insert_iter(iter, xval), true};
        }
    }

    template <typename... Args>
    std::pair<iterator, bool> _emplace_compact_unique(Args &&...args) noexcept {
        using extractor = typename Traits::template inplace_key_extractor<remove_cvref_t<Args>...>;
        using temporary = btree_detail::compact_temporary<value_type, _Alty>;
        if constexpr (extractor::extractable) {
            const auto &key = extractor::extract(std::forward<Args>(args)...);
            const auto iter = _get_insert_multi_pos(key);
            if (WJR_UNLIKELY(_same_insert_key(iter, key)))
                return {_get_same_iterator(iter), false};
            temporary value(_get_allocator(), std::forward<Args>(args)...);
            return {_insert_iter(iter, value.get()), true};
        } else {
            temporary value(_get_allocator(), std::forward<Args>(args)...);
            const auto &key = Traits::get_key(*value.get());
            const auto iter = _get_insert_multi_pos(key);
            if (WJR_UNLIKELY(_same_insert_key(iter, key)))
                return {_get_same_iterator(iter), false};
            return {_insert_iter(iter, value.get()), true};
        }
    }

    iterator _insert_multi(const value_type &val) noexcept { return _emplace_multi(val); }
    iterator _insert_multi(value_type &&val) noexcept { return _emplace_multi(std::move(val)); }

    std::pair<iterator, bool> _insert_unique(const value_type &val) noexcept {
        return _emplace_unique(val);
    }

    std::pair<iterator, bool> _insert_unique(value_type &&val) noexcept {
        return _emplace_unique(std::move(val));
    }

    template <typename... Args>
    std::pair<iterator, bool> _try_emplace_unique(const key_type &key, Args &&...args) {
        const_iterator iter = _get_insert_multi_pos(key);

        if (WJR_UNLIKELY(_same_insert_key(iter, key))) {
            return {_get_same_iterator(iter), false};
        }

        if constexpr (compact_slots) {
            btree_detail::compact_temporary<value_type, _Alty> value(
                _get_allocator(), std::piecewise_construct, std::forward_as_tuple(key),
                std::forward_as_tuple(std::forward<Args>(args)...));
            return {_insert_iter(iter, value.get()), true};
        } else {
            ivalue_type xval = _create_node(std::piecewise_construct, std::forward_as_tuple(key),
                                            std::forward_as_tuple(std::forward<Args>(args)...));
            return {_insert_iter(iter, xval), true};
        }
    }

    template <typename... Args>
    std::pair<iterator, bool> _try_emplace_unique(key_type &&key, Args &&...args) {
        const_iterator iter = _get_insert_multi_pos(key);

        if (WJR_UNLIKELY(_same_insert_key(iter, key))) {
            return {_get_same_iterator(iter), false};
        }

        if constexpr (compact_slots) {
            btree_detail::compact_temporary<value_type, _Alty> value(
                _get_allocator(), std::piecewise_construct, std::forward_as_tuple(std::move(key)),
                std::forward_as_tuple(std::forward<Args>(args)...));
            return {_insert_iter(iter, value.get()), true};
        } else {
            ivalue_type xval =
                _create_node(std::piecewise_construct, std::forward_as_tuple(std::move(key)),
                             std::forward_as_tuple(std::forward<Args>(args)...));
            return {_insert_iter(iter, xval), true};
        }
    }

public:
    /// @brief Find a key equivalent under the comparator, or return end().
    iterator find(const key_type &key) noexcept { return iterator(_find_key(key)); }
    const_iterator find(const key_type &key) const noexcept { return _find_key(key); }

    /// @brief Find an equivalent key without constructing a key object.
    /// @pre Transparent comparisons are consistent with key ordering and do not throw.
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    iterator find(const Lookup &key) noexcept {
        return iterator(_find_key(key));
    }

    /// @copydoc find(const Lookup &)
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    const_iterator find(const Lookup &key) const noexcept {
        return _find_key(key);
    }

    bool contains(const key_type &key) const noexcept { return _find_key(key) != cend(); }

    /// @brief Test key equivalence without constructing a key object.
    /// @pre Transparent comparisons are consistent with key ordering and do not throw.
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    bool contains(const Lookup &key) const noexcept {
        return _find_key(key) != cend();
    }

    /// @brief Return the half-open range of keys equivalent under the comparator.
    std::pair<iterator, iterator> equal_range(const key_type &key) noexcept {
        const auto range = _equal_keys(key);
        return {iterator(range.first), iterator(range.second)};
    }
    std::pair<const_iterator, const_iterator> equal_range(const key_type &key) const noexcept {
        return _equal_keys(key);
    }

    /// @brief Return equivalent bounds without constructing a key object.
    /// @pre Transparent comparisons are consistent with key ordering and do not throw.
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    std::pair<iterator, iterator> equal_range(const Lookup &key) noexcept {
        const auto range = _equal_keys(key);
        return {iterator(range.first), iterator(range.second)};
    }

    /// @copydoc equal_range(const Lookup &)
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    std::pair<const_iterator, const_iterator> equal_range(const Lookup &key) const noexcept {
        return _equal_keys(key);
    }

    /// @brief Count every equivalent element in a tree permitting duplicate keys.
    size_type count(const key_type &key) const noexcept
    requires multi
    {
        const auto range = _equal_keys(key);
        return static_cast<size_type>(std::distance(range.first, range.second));
    }

    /// @copydoc count(const key_type &)
    template <typename Lookup>
    requires multi && btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    size_type count(const Lookup &key) const noexcept {
        const auto range = _equal_keys(key);
        return static_cast<size_type>(std::distance(range.first, range.second));
    }

    /// @brief Insert a single-pass range, retaining unique/duplicate-key container semantics.
    /// @pre Input iterators remain valid while this tree is modified.
    template <btree_detail::insert_iterator<value_type> Iter, std::sentinel_for<Iter> Sentinel>
    void insert(Iter first, Sentinel last) {
        for (; first != last; ++first) {
            if constexpr (multi)
                _emplace_multi(*first);
            else
                _emplace_unique(*first);
        }
    }

    /// @brief Explicitly use local hints for range import; unsorted input remains valid via
    /// fallback.
    /// @note Sorted/local runs can benefit; random input may pay extra validation work.
    template <btree_detail::insert_iterator<value_type> Iter, std::sentinel_for<Iter> Sentinel>
    void insert_with_hint(Iter first, Sentinel last) {
        auto hint = cend();
        for (; first != last; ++first) {
            if constexpr (multi) {
                hint = _emplace_hint_multi<true>(hint, *first);
            } else {
                hint = _emplace_hint_unique<true>(hint, *first).first;
            }
        }
    }

    /// @brief Insert adjacent to a valid hint, falling back to ordinary search for a wrong hint.
    /// @pre hint belongs to this tree; insertion has the existing invalidation and failure
    /// contract.
    template <typename... Args>
    iterator emplace_hint(const_iterator hint, Args &&...args) noexcept {
        if constexpr (multi)
            return _emplace_hint_multi(hint, std::forward<Args>(args)...);
        else
            return _emplace_hint_unique(hint, std::forward<Args>(args)...).first;
    }

    iterator insert(const_iterator hint, const value_type &value) noexcept {
        return emplace_hint(hint, value);
    }
    iterator insert(const_iterator hint, value_type &&value) noexcept {
        return emplace_hint(hint, std::move(value));
    }

    /// @brief Insert every initializer-list value using the container's duplicate policy.
    void insert(std::initializer_list<value_type> values) noexcept {
        insert(values.begin(), values.end());
    }

    iterator lower_bound(const key_type &key) noexcept { return _search<false, true>(key); }
    const_iterator lower_bound(const key_type &key) const noexcept {
        return _search<false, true>(key);
    }

    /// @brief Find a lower bound without constructing a key.
    /// @pre The transparent comparator orders keys and lookup values consistently and never throws.
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    iterator lower_bound(const Lookup &key) noexcept {
        return _search<false, true>(key);
    }

    /// @copydoc lower_bound(const Lookup &)
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    const_iterator lower_bound(const Lookup &key) const noexcept {
        return _search<false, true>(key);
    }

    iterator upper_bound(const key_type &key) noexcept { return _search<true, true>(key); }
    const_iterator upper_bound(const key_type &key) const noexcept {
        return _search<true, true>(key);
    }

    /// @brief Find an upper bound without constructing a key.
    /// @pre The transparent comparator orders keys and lookup values consistently and never throws.
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    iterator upper_bound(const Lookup &key) noexcept {
        return _search<true, true>(key);
    }

    /// @copydoc upper_bound(const Lookup &)
    template <typename Lookup>
    requires btree_detail::transparent_lookup<key_compare, key_type, Lookup>
    const_iterator upper_bound(const Lookup &key) const noexcept {
        return _search<true, true>(key);
    }

    iterator erase(const_iterator iter) noexcept {
        WJR_ASSERT_L2(iter != cend());
        return _erase_iter(iter);
    }

    /// @brief Erase [first, last), returning the element that follows the removed range.
    /// @pre Both iterators delimit a valid range in this tree.
    iterator erase(const_iterator first, const_iterator last) noexcept {
        if (first == last)
            return iterator(first);
        if (first == cbegin() && last == cend()) {
            clear();
            return end();
        }
        /// Deletion can invalidate last; preserve the count before the first mutation.
        const auto count = static_cast<size_type>(std::distance(first, last));
        return _erase_n(first, count);
    }

    /// @brief Erase all comparator-equivalent keys and return their count.
    size_type erase(const key_type &key) noexcept { return _erase_key(key); }

    /// @brief Erase equivalent keys without constructing a key object.
    /// @pre Transparent comparisons are consistent with key ordering and do not throw.
    template <btree_detail::transparent_erase_lookup<key_compare, key_type, const_iterator> Lookup>
    size_type erase(const Lookup &key) noexcept {
        return _erase_key(key);
    }

    WJR_PURE bool empty() const noexcept { return size() == 0; }
    WJR_PURE size_type size() const noexcept { return _get_size(); }

private:
    template <typename Lookup>
    const_iterator _find_key(const Lookup &key) const noexcept {
        auto position = lower_bound(key);
        if (position == cend() || key_comp()(key, Traits::get_key(*position)))
            return cend();
        return position;
    }

    template <typename Lookup>
    std::pair<const_iterator, const_iterator> _equal_keys(const Lookup &key) const noexcept {
        auto first = lower_bound(key);
        if constexpr (multi) {
            return {first, upper_bound(key)};
        } else {
            auto last = first;
            if (first != cend() && !key_comp()(key, Traits::get_key(*first)))
                ++last;
            return {first, last};
        }
    }

    template <typename Lookup>
    size_type _erase_key(const Lookup &key) noexcept {
        if constexpr (multi) {
            const auto range = _equal_keys(key);
            if (range.first == range.second)
                return 0;
            if (range.first == cbegin() && range.second == cend()) {
                const auto count = size();
                clear();
                return count;
            }
            const auto count = static_cast<size_type>(std::distance(range.first, range.second));
            _erase_n(range.first, count);
            return count;
        } else {
            const auto position = _find_key(key);
            if (position == cend())
                return 0;
            static_cast<void>(_erase_iter(position));
            return 1;
        }
    }

    /// @pre count is positive and first starts a valid range containing count elements.
    iterator _erase_n(const_iterator first, size_type count) noexcept {
        iterator position(first);
        do {
            position = _erase_iter(position);
        } while (--count != 0);
        return position;
    }

    void _move_tree(basic_btree &&other) noexcept {
        node_type *&root = other._get_root();
        if (WJR_UNLIKELY(root == nullptr)) {
            _get_root() = nullptr;
            _get_sentry()->init_self();
            auto &size = other._get_size();
            if constexpr (compact_slots) {
                slot_ops::template transfer<0, 2>(other._get_base()->m_values,
                                                  other._get_base()->m_values + size,
                                                  _get_base()->m_values, _get_allocator());
            } else {
                copy_small_to(other._get_base()->m_values, _get_base()->m_values);
            }
            _get_size() = std::exchange(size, 0);
            return;
        }

        _get_root() = std::exchange(root, nullptr);
        _get_size() = std::exchange(other._get_size(), 0);
        replace(other._get_sentry(), _get_sentry());
        other._get_sentry()->init_self();
    }

    void _swap_small_impl(basic_btree &other) noexcept {
        // this is small, other is not small
        if constexpr (compact_slots) {
            slot_ops::template transfer<0, 2>(_get_base()->m_values,
                                              _get_base()->m_values + _get_size(),
                                              other._get_base()->m_values, other._get_allocator());
        } else {
            copy_small_to(_get_base()->m_values, other._get_base()->m_values);
        }
        std::swap(_get_root(), other._get_root());
        std::swap(_get_size(), other._get_size());
        replace(other._get_sentry(), _get_sentry());
        other._get_sentry()->init_self();
    }

    void _swap_tree(basic_btree &other) noexcept {
        if (_get_root() == nullptr) {
            if (other._get_root() == nullptr) {
                auto *lhs = _get_base();
                auto *rhs = other._get_base();
                if constexpr (compact_slots) {
                    ivalue_type tmp[2];
                    auto &lhs_al = _get_allocator();
                    auto &rhs_al = other._get_allocator();
                    /// Allocators have already propagated; each destination owns its incoming
                    /// values.
                    slot_ops::template transfer<0, 2>(lhs->m_values, lhs->m_values + _get_size(),
                                                      tmp, rhs_al);
                    slot_ops::template transfer<0, 2>(
                        rhs->m_values, rhs->m_values + other._get_size(), lhs->m_values, lhs_al);
                    slot_ops::template transfer<0, 2>(tmp, tmp + _get_size(), rhs->m_values,
                                                      rhs_al);
                } else {
                    _simd_storage_t<ivalue_type, 2> tmp;
                    copy_small_to(lhs->m_values, &tmp);
                    copy_small_to(rhs->m_values, lhs->m_values);
                    copy_small_to(&tmp, rhs->m_values);
                }

                std::swap(_get_size(), other._get_size());
            } else {
                _swap_small_impl(other);
            }
            return;
        }

        if (other._get_root() == nullptr) {
            other._swap_small_impl(*this);
            return;
        }

        std::swap(_get_root(), other._get_root());
        std::swap(_get_size(), other._get_size());
        swap_with(_get_sentry(), other._get_sentry());
    }

    template <typename TV>
    std::pair<ikey_type, node_type *> _rec_assign_tree(node_type *current) noexcept {
        slot_size_type cur_size = current->size();

        if (cur_size < 0) {
            auto *const leaf = current->as_leaf();
            const slot_usize_type cur_usize = -cur_size;

            auto *const this_leaf = _create_leaf_node();
            this_leaf->size() = cur_size;
            for (slot_usize_type i = 0; i < cur_usize; ++i) {
                _construct_leaf_value(this_leaf, i,
                                      static_cast<TV>(from_ivalue(leaf->m_values[i])));
            }

            _get_sentry()->push_back(this_leaf);
            return {to_ikey(this_leaf->get_key(0)), this_leaf};
        }

        auto *const this_inner = _create_inner_node();
        this_inner->size() = cur_size;
        ikey_type Key;
        const slot_usize_type cur_usize = cur_size;

        for (slot_usize_type i = 0; i <= cur_usize; ++i) {
            auto [key, son] = _rec_assign_tree<TV>(current->as_inner()->m_sons[i]);
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
    void _assign_tree(BT &&other) noexcept {
        node_type *current = const_cast<node_type *>(other._get_root());
        _get_sentry()->init_self();
        slot_usize_type size = other._get_size();
        _get_size() = size;
        if (WJR_UNLIKELY(current == nullptr)) {
            for (slot_usize_type i = 0; i < size; ++i) {
                _construct_leaf_value(_get_base(), i,
                                      static_cast<TV>(from_ivalue(other._get_base()->m_values[i])));
            }

            return;
        }

        auto [key, root] = _rec_assign_tree<TV>(current);
        _get_root() = root;
        root->m_parent = nullptr;
    }

    void _copy_assign_tree(const basic_btree &other) noexcept {
        _assign_tree<const value_type &>(other);
    }

    /// @brief Rebuild a detached ownership graph without searching its retired keys.
    /// @pre current is owned by source's allocator, but is no longer in source's active tree.
    std::pair<ikey_type, node_type *> _rec_move_detached_tree(node_type *current,
                                                              basic_btree &source,
                                                              list_node_type &leaves) noexcept {
        const slot_size_type cur_size = current->size();
        if (cur_size < 0) {
            auto *const leaf = current->as_leaf();
            const slot_usize_type count = -cur_size;
            auto *const destination = _create_leaf_node();
            destination->size() = cur_size;
            for (slot_usize_type i = 0; i < count; ++i) {
                _construct_leaf_value(destination, i, std::move(from_ivalue(leaf->m_values[i])));
                source._destroy_slot(leaf->m_values[i]);
            }
            leaves.push_back(destination);
            source._drop_leaf_node(leaf);
            return {to_ikey(destination->get_key(0)), destination};
        }

        auto *const inner = current->as_inner();
        auto *const destination = _create_inner_node();
        destination->size() = cur_size;
        ikey_type first_key;
        const slot_usize_type count = cur_size;
        for (slot_usize_type i = 0; i <= count; ++i) {
            auto [key, child] = _rec_move_detached_tree(inner->m_sons[i], source, leaves);
            child->m_parent = destination;
            child->pos() = i;
            if (i == 0) {
                first_key = key;
            } else {
                destination->m_keys[i - 1] = key;
            }
            destination->m_sons[i] = child;
        }
        source._drop_inner_node(inner);
        return {first_key, destination};
    }

    void _move_assign_tree(basic_btree &&other) noexcept {
        if constexpr (std::is_same_v<key_compare, btree_detail::transfer_reference_less>) {
            _assign_tree<value_type &&>(std::move(other));
            other.clear();
            return;
        }
        /// Detach before invoking allocation/value hooks, so moved keys are never active keys.
        auto *const retired_root = std::exchange(other._get_root(), nullptr);
        const auto count = std::exchange(other._get_size(), 0);
        other._get_sentry()->init_self();
        if (retired_root == nullptr) {
            for (slot_usize_type i = 0; i < count; ++i) {
                _construct_leaf_value(_get_base(), i,
                                      std::move(from_ivalue(other._get_base()->m_values[i])));
                other._destroy_slot(other._get_base()->m_values[i]);
            }
        } else {
            /// Publish the new root/list only after the complete graph has been rebuilt.
            list_node_type leaves;
            leaves.init_self();
            auto [key, root] = _rec_move_detached_tree(retired_root, other, leaves);
            _get_root() = root;
            root->m_parent = nullptr;
            replace(std::addressof(leaves), _get_sentry());
        }
        _get_size() = count;
    }

    void _rec_destroy_and_deallocate(node_type *current) noexcept;

    // member function for container_fn (START)

    WJR_CONSTEXPR20 _Alty &_get_allocator() noexcept { return m_pair.second().first(); }
    WJR_CONSTEXPR20 const _Alty &_get_allocator() const noexcept { return m_pair.second().first(); }

    void _destroy_and_deallocate() noexcept;

    /// Release nodes through their original allocator before allocator propagation.
    void _release_before_copy() { clear(); }

    void _copy_element(const basic_btree &other) noexcept {
        clear();
        key_comp() = other.key_comp();
        _copy_assign_tree(other);
    }

    void _take_storage(basic_btree &&other) noexcept {
        key_comp() = std::move(other.key_comp());
        _move_tree(std::move(other));
    }

    void _destroy_and_move_element(basic_btree &&other) noexcept {
        clear();
        key_comp() = std::move(other.key_comp());
        _move_assign_tree(std::move(other));
    }

    void _swap_storage(basic_btree &other) noexcept {
        std::swap(key_comp(), other.key_comp());
        _swap_tree(other);
    }

    // member function for container_fn (END)

    WJR_NODISCARD WJR_NOINLINE WJR_HOT WJR_FLATTEN iterator _insert_iter(const_iterator iter,
                                                                         insert_type xval) noexcept;

    static constexpr slot_usize_type _search_mask =
        (slot_usize_type)(1) << (std::numeric_limits<slot_usize_type>::digits - 1);
    static_assert(_search_mask > node_size);

    template <bool Upper, typename Lookup>
    WJR_NOINLINE WJR_HOT WJR_FLATTEN const_iterator _search_impl(const Lookup &key) const noexcept {
        const auto &comp = key_comp();
        const node_type *current = _get_root();

        if (WJR_UNLIKELY(current == nullptr)) {
            slot_usize_type size = _get_size();
            if (size == 0 || _compare<Upper>(_get_base()->get_key(0), key, comp)) {
                return const_iterator(_get_sentry(), 0);
            }

            if (size == 1 || _compare<Upper>(_get_base()->get_key(1), key, comp)) {
                return const_iterator(_get_sentry(), 1);
            }

            return const_iterator(_get_sentry(), 2);
        }

        slot_size_type cur_size = current->size();
        slot_usize_type pos;

        if (cur_size >= 0) {
            do {
                pos = _search<Upper, 1>(current->as_inner(), cur_size, key, comp);
                current = current->as_inner()->m_sons[pos];
                cur_size = current->size();
            } while (cur_size >= 0);
        }

        const slot_usize_type cur_usize = -cur_size;
        pos = _search<Upper, 1>(current->as_leaf(), cur_usize, key, comp);
        return const_iterator(current->as_leaf(), pos | (pos == cur_usize ? _search_mask : 0));
    }

    template <bool Upper, bool Adjust, typename Lookup>
    WJR_HOT WJR_FLATTEN const_iterator _search(const Lookup &key) const noexcept {
        const_iterator iter = _search_impl<Upper>(key);

        if constexpr (Adjust) {
            if (WJR_LIKELY(!(iter.pos() & _search_mask))) {
                return iter;
            }

            return const_iterator(iter.get_leaf()->next()->self(), 0);
        } else {
            return const_iterator(iter.get_leaf(), iter.pos() & ~_search_mask);
        }
    }

    template <bool Upper, typename Lookup>
    WJR_INTRINSIC_INLINE static bool _compare(const key_type &node, const Lookup &key,
                                              const key_compare &comp) noexcept {
        if constexpr (Upper) {
            return comp(key, node);
        } else {
            return !comp(node, key);
        }
    }

    template <bool Upper, size_t Min, typename Lookup>
    WJR_INTRINSIC_INLINE static slot_usize_type _search(const inner_node_type *current,
                                                        slot_usize_type size, const Lookup &key,
                                                        const key_compare &comp) noexcept {
        using searcher = std::conditional_t<
            node_size == 32 && btree_detail::standard_integral_set_lookup<key_type, value_type,
                                                                          key_compare, Lookup>,
            btree_detail::integral_set_searcher32, basic_btree_searcher_impl<node_size>>;
        return searcher::template search<Min>(size, [current, &key, &comp](slot_usize_type pos) {
            return _compare<Upper>(from_ikey(current->m_keys[pos]), key, comp);
        });
    }

    template <bool Upper, size_t Min, typename Lookup>
    WJR_INTRINSIC_INLINE static slot_usize_type _search(const leaf_node_type *current,
                                                        slot_usize_type size, const Lookup &key,
                                                        const key_compare &comp) noexcept {
        using searcher = std::conditional_t<
            node_size == 32 && btree_detail::standard_integral_set_lookup<key_type, value_type,
                                                                          key_compare, Lookup>,
            btree_detail::integral_set_searcher32, basic_btree_searcher_impl<node_size>>;
        return searcher::template search<Min>(size, [current, &key, &comp](slot_usize_type pos) {
            return _compare<Upper>(current->get_key(pos), key, comp);
        });
    }

    template <typename T>
    WJR_INTRINSIC_INLINE static slot_usize_type
    _init_remove_select(const inner_node_type *parent, slot_usize_type pos,
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
    void _erase_iter_reblance(node_type *parent, slot_usize_type par_pos,
                              slot_usize_type par_size) noexcept;

    WJR_NODISCARD WJR_NOINLINE WJR_HOT WJR_FLATTEN iterator
    _erase_iter(const_iterator iter) noexcept;

    WJR_INTRINSIC_CONSTEXPR leaf_node_type *_get_base() noexcept {
        return m_pair.second().second()._get_base();
    }

    WJR_INTRINSIC_CONSTEXPR const leaf_node_type *_get_base() const noexcept {
        return m_pair.second().second()._get_base();
    }

    WJR_INTRINSIC_CONSTEXPR node_type *&_get_root() noexcept { return _get_base()->m_parent; }

    WJR_INTRINSIC_CONSTEXPR const node_type *_get_root() const noexcept {
        return _get_base()->m_parent;
    }

    WJR_INTRINSIC_CONSTEXPR list_node_type *_get_sentry() noexcept { return _get_base()->self(); }

    WJR_INTRINSIC_CONSTEXPR const list_node_type *_get_sentry() const noexcept {
        return _get_base()->self();
    }

    WJR_INTRINSIC_CONSTEXPR size_type &_get_size() noexcept { return _get_base()->m_root_size; }

    WJR_INTRINSIC_CONSTEXPR const size_type &_get_size() const noexcept {
        return _get_base()->m_root_size;
    }

    static void copy_small_to(const ivalue_type *from, ivalue_type *to) {
        uninitialized_copy_n_restrict(from, 2, to);
    }

    cpair<key_compare, cpair<_Alty, btree_root_node<Traits>>> m_pair;
};

template <typename Traits>
void basic_btree<Traits>::_rec_destroy_and_deallocate(node_type *current) noexcept {
    slot_size_type cur_size = current->size();

    // If root is leaf
    if (cur_size < 0) {
        auto *const leaf = current->as_leaf();
        const slot_usize_type cur_usize = -cur_size;

        for (slot_usize_type i = 0; i < cur_usize; ++i) {
            _destroy_slot(leaf->m_values[i]);
        }

        _drop_leaf_node(leaf);
        return;
    }

    auto *const inner = current->as_inner();
    const slot_usize_type cur_usize = cur_size;

    for (slot_usize_type i = 0; i <= cur_usize; ++i) {
        _rec_destroy_and_deallocate(inner->m_sons[i]);
    }

    _drop_inner_node(inner);
}

template <typename Traits>
void basic_btree<Traits>::_destroy_and_deallocate() noexcept {
    node_type *current = _get_root();
    if (WJR_UNLIKELY(current == nullptr)) {
        slot_usize_type size = _get_size();
        for (slot_usize_type i = 0; i < size; ++i) {
            _destroy_slot(_get_base()->m_values[i]);
        }
        return;
    }

    _rec_destroy_and_deallocate(current);
}

template <typename Traits>
typename basic_btree<Traits>::iterator
basic_btree<Traits>::_insert_iter(const_iterator iter, insert_type xval) noexcept {
    auto &al = _get_allocator();
    ++_get_size();
    slot_usize_type pos = iter.pos();
    leaf_node_type *leaf;

    // empty
    if (WJR_UNLIKELY(_get_root() == nullptr)) {
        // size has incresed before _insert_iter
        slot_usize_type size = _get_size();
        if (size <= 2) {
            leaf = _get_base();
            if (size == 1) {
                // do nothing
            } else {
                if (pos == 0) {
                    leaf->transfer_one(1, leaf->m_values, al);
                }
            }
        } else {
            leaf = _create_leaf_node();
            _get_root() = leaf;
            leaf->size() = -3;
            leaf->m_parent = nullptr;
            leaf->transfer_one(pos == 0 ? 1 : 0, _get_base()->m_values, al);
            leaf->transfer_one(pos == 2 ? 1 : 2, _get_base()->m_values + 1, al);
            _get_sentry()->push_front(leaf);
        }

        leaf->assign(pos, xval, al);
        return iterator(leaf, pos);
    }

    leaf = iter.get_leaf();
    do {
        const slot_usize_type cur_size = static_cast<slot_usize_type>(-leaf->size());

        // non-full leaf
        if (WJR_LIKELY(cur_size != node_size)) {
            WJR_ASSERT_ASSUME(pos <= cur_size);
            leaf->template copy_backward<0, node_size - 1>(pos, cur_size, cur_size + 1, leaf, al);

            leaf->size() = -(cur_size + 1);
            leaf->assign(pos, xval, al);
            return iterator(leaf, pos);
        }
    } while (0);

    iterator result;
    node_type *inst;

    do {
        auto *const inst_leaf = _create_leaf_node();
        leaf->push_front(inst_leaf);
        leaf->size() = -(slot_size_type)(floor_half + 1);
        inst_leaf->size() = -(slot_size_type)(node_size - floor_half);

        if (pos <= floor_half) {
            leaf->template copy<ceil_half, ceil_half>(floor_half, node_size, 0, inst_leaf, al);
            leaf->template copy_backward<0, floor_half>(pos, floor_half, floor_half + 1, leaf, al);
            leaf->assign(pos, xval, al);
            result = iterator(iter);
        } else {
            // pos in inst_leaf
            const slot_usize_type rpos = pos - floor_half - 1;
            leaf->template copy<0, ceil_half - 1>(floor_half + 1, pos, 0, inst_leaf, al);
            leaf->template copy<0, ceil_half - 1>(pos, node_size, rpos + 1, inst_leaf, al);
            inst_leaf->assign(rpos, xval, al);
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

            auto *const tmp_inst = _create_inner_node();

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

        auto *const new_root = _create_inner_node();
        new_root->size() = 1;
        new_root->m_parent = nullptr;
        new_root->m_keys[0] = key;
        new_root->m_sons[0] = current;
        new_root->m_sons[1] = inst;
        current->pos() = 0;
        inst->pos() = 1;

        current->m_parent = new_root;
        inst->m_parent = new_root;

        _get_root() = new_root;
    }

    return result;
}

template <typename Traits>
void basic_btree<Traits>::_erase_iter_reblance(node_type *parent, slot_usize_type par_pos,
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

        const slot_usize_type next_size =
            _init_remove_select(par_inner, par_pos, par_size, lhs, rhs);

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
                btree_detail::copy<floor_half, node_size - 1>(rhs->m_keys + moved_elements,
                                                              rhs->m_keys + next_size, rhs->m_keys);
                btree_detail::copy<floor_half + 1, node_size>(
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
        _drop_inner_node(rhs);

        pos = par_pos;
        cur_size = par_size;
        current = parent;
        parent = current->m_parent;
    }

    const auto inner = current->as_inner();

    if (cur_size == 1) {
        node_type *root = inner->m_sons[0];
        _drop_inner_node(inner);
        _get_root() = root;
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
basic_btree<Traits>::_erase_iter(const_iterator iter) noexcept {
    auto &al = _get_allocator();
    --_get_size();

    constexpr slot_usize_type merge_size = floor_half * 2;

    leaf_node_type *leaf = iter.get_leaf();
    slot_usize_type pos = iter.pos();
    node_type *parent = leaf->m_parent;

    _destroy_slot(leaf->m_values[pos]);

    union {
        slot_size_type cur_ssize;
        slot_usize_type cur_size;
    };

    cur_ssize = -leaf->size();
    if (WJR_LIKELY(cur_ssize > (slot_size_type)floor_half)) {
        leaf->template copy<0, node_size - 1>(pos + 1, cur_size, pos, leaf, al);
        leaf->size() = -(cur_size - 1);

        // first key in leaf is changed
        if (pos == 0 && parent != nullptr && leaf != _get_sentry()->next()->self()) {
            node_type *current = leaf;
            slot_usize_type tmp_pos;

            do {
                tmp_pos = current->pos();
                current = parent;
                parent = current->m_parent;
                WJR_ASSERT_L3(tmp_pos != 0 || parent != nullptr);
            } while (tmp_pos == 0);

            current->as_inner()->m_keys[tmp_pos - 1] = to_ikey(leaf->get_key(0));
        }

        return iterator(leaf, pos)._adjust_next();
    }

    if (parent == nullptr) {
        if (_get_root() == nullptr) {
            cur_size = -(cur_size - 1);
            leaf->template copy<0, node_size - 1>(pos + 1, cur_size, pos, leaf, al);
            return iterator(leaf, pos);
        }

        if (cur_size == 1) {
            _drop_leaf_node(leaf);
            _get_root() = nullptr;
            _get_sentry()->init_self();
            return cend();
        }

        leaf->template copy<0, node_size - 1>(pos + 1, cur_size, pos, leaf, al);
        leaf->size() = -(cur_size - 1);
        return iterator(leaf, pos)._adjust_next();
    }

    WJR_ASSERT_ASSUME(cur_size == floor_half);

    const auto inner = parent->as_inner();
    slot_usize_type par_pos = leaf->pos();
    cur_size = inner->size();
    leaf_node_type *lhs;
    leaf_node_type *rhs;

    const slot_usize_type next_size = _init_remove_select(inner, par_pos, cur_size, lhs, rhs);

    do {
        if (lhs != nullptr) {
            rhs = leaf;

            if (next_size == floor_half) {
                leaf->template copy<0, floor_half>(0, pos, floor_half, lhs, al);
                leaf->template copy<0, floor_half>(pos + 1, floor_half, pos + floor_half, lhs, al);

                leaf = lhs;
                pos += floor_half;
                break;
            }

            const slot_usize_type moved_elements = (next_size - floor_half + 1) / 2;

            if (moved_elements != 1) {
                leaf->template copy_backward<0, floor_half>(
                    pos + 1, floor_half, floor_half + moved_elements - 1, leaf, al);
            }

            leaf->template copy_backward<0, floor_half>(0, pos, pos + moved_elements, leaf, al);
            lhs->template copy<1, max_moved_elements>(next_size - moved_elements, next_size, 0,
                                                      leaf, al);

            lhs->size() = -(next_size - moved_elements);
            leaf->size() = -(floor_half + moved_elements - 1);

            pos += moved_elements;
        } else {
            WJR_ASSERT_ASSUME(rhs != nullptr);

            lhs = leaf;

            leaf->template copy<0, floor_half>(pos + 1, floor_half, pos, leaf, al);

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
                rhs->template copy<0, floor_half>(0, floor_half, floor_half - 1, leaf, al);

                ++par_pos;
                break;
            }

            const slot_usize_type moved_elements = (next_size - floor_half + 1) / 2;

            rhs->template copy<1, max_moved_elements>(0, moved_elements, floor_half - 1, leaf, al);
            rhs->template copy<1, node_size - max_moved_elements>(moved_elements, next_size, 0, rhs,
                                                                  al);

            rhs->size() = -(next_size - moved_elements);
            leaf->size() = -(floor_half + moved_elements - 1);
        }

        node_type *current = rhs;

        slot_usize_type tmp_pos = current->pos();
        current = parent;
        parent = current->m_parent;

        current->as_inner()->m_keys[tmp_pos - 1] = to_ikey(rhs->get_key(0));
        return iterator(leaf, pos)._adjust_next();
    } while (0);

    lhs->size() = -(merge_size - 1);
    rhs->remove();
    _drop_leaf_node(rhs);

    _erase_iter_reblance(parent, par_pos, cur_size);
    return iterator(leaf, pos)._adjust_next();
}

} // namespace wjr

#endif // WJR_CONTAINER_BTREE_HPP__