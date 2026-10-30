#ifndef WJR_CONTAINER_EXPERIMENTAL_BYTE_BTREE_HPP__
#define WJR_CONTAINER_EXPERIMENTAL_BYTE_BTREE_HPP__

#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>

namespace wjr::experimental {

/** @brief Requested maximum bytes for one allocated leaf, including alignment padding. */
template <size_t Bytes>
struct leaf_bytes {
    static constexpr size_t value = Bytes;
};

namespace byte_btree_detail {
template <typename T>
inline constexpr bool is_leaf_budget = false;
template <size_t Bytes>
inline constexpr bool is_leaf_budget<leaf_bytes<Bytes>> = true;

/** @brief Byte configuration requires complete, unqualified object types. */
template <typename T>
concept complete_object = std::is_object_v<T> && !std::is_array_v<T> &&
                          std::is_same_v<T, std::remove_cv_t<T>> && requires { sizeof(T); };
} // namespace byte_btree_detail

/**
 * @brief Compile-time layout of a byte-configured map, or set when Value is void.
 *
 * Selects the largest capacity in [4, 64] whose actual aligned leaf allocation fits
 * Budget. If none fits, selects four slots and reports budget_satisfied == false.
 * Inner nodes use the same slot capacity; their allocation can exceed the budget.
 * Separate indirect value allocations and allocator bookkeeping are excluded.
 */
template <typename Key, typename Value = void, typename Budget = leaf_bytes<256>,
          typename Alloc = std::allocator<char>>
requires(byte_btree_detail::complete_object<Key> &&
         (std::is_same_v<Value, void> || byte_btree_detail::complete_object<Value>) &&
         byte_btree_detail::is_leaf_budget<Budget>)
struct byte_btree_layout {
private:
    template <size_t Slots>
    using traits = btree_traits<Key, Value, false, std::less<Key>, Alloc, Slots>;

    template <size_t... I>
    static constexpr size_t select_slots(std::index_sequence<I...>) noexcept {
        size_t slots = 4;
        ((sizeof(btree_detail::leaf_storage<traits<I + 4>>) <= Budget::value ? (void)(slots = I + 4)
                                                                             : (void)0),
         ...);
        return slots;
    }

public:
    static constexpr size_t leaf_slots = select_slots(std::make_index_sequence<61>{});
    static constexpr size_t leaf_allocation_bytes =
        sizeof(btree_detail::leaf_storage<traits<leaf_slots>>);
    static constexpr size_t inner_allocation_bytes =
        sizeof(typename traits<leaf_slots>::inner_node_type);
    static constexpr bool budget_satisfied = leaf_allocation_bytes <= Budget::value;
};

/** @brief Existing btree_map selected by leaf allocation budget, with no wrapper or state. */
template <typename Key, typename Value, typename Budget = leaf_bytes<256>,
          typename Compare = std::less<Key>, typename Alloc = std::allocator<char>>
requires byte_btree_detail::complete_object<Value>
using byte_btree_map =
    btree_map<Key, Value, Compare, Alloc, byte_btree_layout<Key, Value, Budget, Alloc>::leaf_slots>;

/** @brief Existing btree_set selected by leaf allocation budget, with no wrapper or state. */
template <typename Key, typename Budget = leaf_bytes<256>, typename Compare = std::less<Key>,
          typename Alloc = std::allocator<char>>
using byte_btree_set =
    btree_set<Key, Compare, Alloc, byte_btree_layout<Key, void, Budget, Alloc>::leaf_slots>;

} // namespace wjr::experimental

#endif // WJR_CONTAINER_EXPERIMENTAL_BYTE_BTREE_HPP__
