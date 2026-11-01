#ifndef WJR_CONTAINER_LRU_CACHE_HPP__
#define WJR_CONTAINER_LRU_CACHE_HPP__

#include <concepts>
#include <cstddef>
#include <functional>
#include <limits>
#include <memory>
#include <scoped_allocator>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <wjr/container/list.hpp>
#include <wjr/macros/exceptions.hpp>

namespace wjr {
namespace lru_detail {

template <typename Hash, typename Key>
concept nothrow_hash_for = std::is_nothrow_invocable_r_v<std::size_t, const Hash &, const Key &>;

template <typename Allocator>
constexpr const Allocator &payload_allocator(const Allocator &allocator) noexcept {
    return allocator;
}

template <typename Outer, typename... Inner>
constexpr const typename std::scoped_allocator_adaptor<Outer, Inner...>::inner_allocator_type &
payload_allocator(const std::scoped_allocator_adaptor<Outer, Inner...> &allocator) noexcept {
    return allocator.inner_allocator();
}

template <typename T, typename Allocator, typename... Args>
concept allocator_constructible =
    (!std::uses_allocator_v<T, Allocator> && std::constructible_from<T, Args...>) ||
    (std::uses_allocator_v<T, Allocator> &&
     (std::constructible_from<T, std::allocator_arg_t, const Allocator &, Args...> ||
      std::constructible_from<T, Args..., const Allocator &>));

template <typename Hash, typename Equal, typename Key, typename Query>
concept transparent_lookup =
    requires {
        typename Hash::is_transparent;
        typename Equal::is_transparent;
    } && nothrow_hash_for<Hash, Query> &&
    std::is_invocable_r_v<bool, const Equal &, const Key &, const Query &> &&
    std::is_invocable_r_v<bool, const Equal &, const Query &, const Key &>;

} // namespace lru_detail

/**
 * @brief Single-threaded owning cache with fixed entry capacity and exact LRU eviction.
 * @pre Hash is nonthrowing, equal keys hash equally, and stored keys retain stable
 * hash/equality. User constructors, assignment, Hash/Equal and allocator operations
 * must not reenter this cache. Key/T and allocator destruction/release do not throw.
 * @note get and successful insertion/assignment promote to MRU; peek does not.
 * Borrowed pointers survive other entries' insertion, promotion and erasure, but
 * expire when their entry is erased/evicted, on clear, or on cache destruction.
 * A nonzero cache reserves for capacity+1; admission may temporarily own one extra
 * entry before eviction. Zero capacity constructs no stored Key/T. Argument
 * evaluation and implicit conversions still occur before the call.
 */
template <typename Key, typename T, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>,
          typename Allocator = std::allocator<std::pair<const Key, T>>>
requires(std::is_object_v<Key> && std::is_object_v<T> && std::is_nothrow_destructible_v<Key> &&
         std::is_nothrow_destructible_v<T> && lru_detail::nothrow_hash_for<Hash, Key> &&
         std::copy_constructible<Hash> && std::copy_constructible<Equal> &&
         std::is_invocable_r_v<bool, const Equal &, const Key &, const Key &>)
class lru_cache {
    struct entry;
    struct recency_tag {};
    using hook_type = intrusive::list_node<entry, recency_tag>;
    using stored_type = std::pair<const Key, entry>;
    using storage_allocator =
        typename std::allocator_traits<Allocator>::template rebind_alloc<stored_type>;
    using payload_allocator_type = std::remove_cvref_t<decltype(lru_detail::payload_allocator(
        std::declval<const storage_allocator &>()))>;

    struct entry : hook_type {
        /// @note key is bound after insertion succeeds and before linking into m_order.
        template <typename... Args>
        // cppcheck-suppress uninitMemberVar symbolName=entry::key
        explicit entry(std::allocator_arg_t, const storage_allocator &allocator, Args &&...args)
            : hook_type(default_construct),
              value(std::make_from_tuple<T>(std::uses_allocator_construction_args<T>(
                  lru_detail::payload_allocator(allocator), std::forward<Args>(args)...))) {}

        const Key *key;
        T value;
    };

    using map_type = std::unordered_map<Key, entry, Hash, Equal, storage_allocator>;

public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const Key, T>;
    using size_type = typename map_type::size_type;
    using hasher = Hash;
    using key_equal = Equal;
    using allocator_type = Allocator;
    using insert_result = std::pair<T *, bool>;

    /// @brief Construct a fixed-capacity cache; reserve/length failures propagate.
    explicit lru_cache(size_type capacity, const Hash &hash = Hash(), const Equal &equal = Equal(),
                       const Allocator &allocator = Allocator())
        : m_capacity(capacity), m_items(0, hash, equal, storage_allocator(allocator)) {
        if (capacity != 0) {
            if (capacity == std::numeric_limits<size_type>::max() ||
                capacity >= m_items.max_size() || capacity + 1 > m_items.max_bucket_count()) {
                WJR_THROW(std::length_error("lru_cache capacity"));
            }
            m_items.reserve(capacity + 1);
        }
    }

    lru_cache(const lru_cache &) = delete;
    lru_cache(lru_cache &&) = delete;
    lru_cache &operator=(const lru_cache &) = delete;
    lru_cache &operator=(lru_cache &&) = delete;

    ~lru_cache() noexcept { clear(); }

    size_type capacity() const noexcept { return m_capacity; }
    size_type size() const noexcept { return m_items.size(); }
    bool empty() const noexcept { return m_items.empty(); }
    allocator_type get_allocator() const { return allocator_type(m_items.get_allocator()); }

    /// @brief Borrow a value and promote its entry, or return nullptr.
    T *get(const Key &key) { return _get(key); }

    template <typename Query>
    requires(!std::same_as<std::remove_cvref_t<Query>, Key> &&
             lru_detail::transparent_lookup<Hash, Equal, Key, Query>)
    T *get(const Query &key) {
        return _get(key);
    }

    /// @brief Borrow read-only access without changing recency, or return nullptr.
    const T *peek(const Key &key) const { return _peek(key); }

    template <typename Query>
    requires(!std::same_as<std::remove_cvref_t<Query>, Key> &&
             lru_detail::transparent_lookup<Hash, Equal, Key, Query>)
    const T *peek(const Query &key) const {
        return _peek(key);
    }

    /// @brief Admit a missing key; duplicates construct no T and still promote.
    template <typename... Args>
    requires(std::constructible_from<Key, const Key &> &&
             lru_detail::allocator_constructible<T, payload_allocator_type, Args && ...>)
    insert_result try_emplace(const Key &key, Args &&...args) {
        return _try_emplace(key, std::forward<Args>(args)...);
    }

    template <typename... Args>
    requires(std::constructible_from<Key, Key &&> &&
             lru_detail::allocator_constructible<T, payload_allocator_type, Args && ...>)
    insert_result try_emplace(Key &&key, Args &&...args) {
        return _try_emplace(std::move(key), std::forward<Args>(args)...);
    }

    /** @brief Assign a hit before promotion, or admit a new value.
     * @note On assignment failure recency is unchanged; T retains its own assignment
     * guarantee. Insertion failure preserves membership and order, excluding user
     * side effects on arguments (including values borrowed from this cache).
     */
    template <typename Value>
    requires(std::constructible_from<Key, const Key &> && std::is_assignable_v<T &, Value &&> &&
             lru_detail::allocator_constructible<T, payload_allocator_type, Value &&>)
    insert_result insert_or_assign(const Key &key, Value &&value) {
        return _insert_or_assign(key, std::forward<Value>(value));
    }

    template <typename Value>
    requires(std::constructible_from<Key, Key &&> && std::is_assignable_v<T &, Value &&> &&
             lru_detail::allocator_constructible<T, payload_allocator_type, Value &&>)
    insert_result insert_or_assign(Key &&key, Value &&value) {
        return _insert_or_assign(std::move(key), std::forward<Value>(value));
    }

    bool erase(const Key &key) { return _erase(key); }

    template <typename Query>
    requires(!std::same_as<std::remove_cvref_t<Query>, Key> &&
             lru_detail::transparent_lookup<Hash, Equal, Key, Query>)
    bool erase(const Query &key) {
        return _erase(key);
    }

    /// @brief Destroy entries and reset recency; retain capacity and backend buckets.
    void clear() noexcept {
        m_order.init_self();
        m_items.clear();
    }

private:
    /// @note Avoid relinking stores on repeated hits to the current MRU entry.
    void _promote(entry &value) noexcept {
        if (m_order.front() != std::addressof(value)) {
            value.remove();
            m_order.push_front(std::addressof(value));
        }
    }

    template <typename Query>
    T *_get(const Query &key) {
        const auto iterator = m_items.find(key);
        if (iterator == m_items.end())
            return nullptr;
        _promote(iterator->second);
        return std::addressof(iterator->second.value);
    }

    template <typename Query>
    const T *_peek(const Query &key) const {
        const auto iterator = m_items.find(key);
        return iterator == m_items.end() ? nullptr : std::addressof(iterator->second.value);
    }

    template <typename KeyArg, typename... Args>
    insert_result _try_emplace(KeyArg &&key, Args &&...args) {
        if (m_capacity == 0)
            return {nullptr, false};
        const auto iterator = m_items.find(key);
        if (iterator != m_items.end()) {
            _promote(iterator->second);
            return {std::addressof(iterator->second.value), false};
        }
        return _admit(std::forward<KeyArg>(key), std::forward<Args>(args)...);
    }

    template <typename KeyArg, typename Value>
    insert_result _insert_or_assign(KeyArg &&key, Value &&value) {
        if (m_capacity == 0)
            return {nullptr, false};
        const auto iterator = m_items.find(key);
        if (iterator != m_items.end()) {
            iterator->second.value = std::forward<Value>(value);
            _promote(iterator->second);
            return {std::addressof(iterator->second.value), false};
        }
        return _admit(std::forward<KeyArg>(key), std::forward<Value>(value));
    }

    template <typename KeyArg, typename... Args>
    insert_result _admit(KeyArg &&key, Args &&...args) {
        auto victim = m_items.end();
        if (m_items.size() == m_capacity) {
            victim = m_items.find(*m_order.back()->self()->key);
            WJR_ASSERT_L2(victim != m_items.end());
        }
        const auto [iterator, inserted] =
            m_items.try_emplace(std::forward<KeyArg>(key), std::allocator_arg,
                                m_items.get_allocator(), std::forward<Args>(args)...);
        auto &value = iterator->second;
        if (!inserted) {
            _promote(value);
            return {std::addressof(value.value), false};
        }
        value.key = std::addressof(iterator->first);
        m_order.push_front(std::addressof(value));
        if (victim != m_items.end()) {
            victim->second.remove();
            m_items.erase(victim);
        }
        return {std::addressof(value.value), true};
    }

    template <typename Query>
    bool _erase(const Query &key) {
        const auto iterator = m_items.find(key);
        if (iterator == m_items.end())
            return false;
        iterator->second.remove();
        m_items.erase(iterator);
        return true;
    }

    const size_type m_capacity;
    map_type m_items;
    hook_type m_order;
};

} // namespace wjr

#endif // WJR_CONTAINER_LRU_CACHE_HPP__
