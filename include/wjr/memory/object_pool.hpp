#ifndef WJR_MEMORY_OBJECT_POOL_HPP__
#define WJR_MEMORY_OBJECT_POOL_HPP__

#include <wjr/container/forward_list.hpp>
#include <wjr/memory/arena.hpp>

namespace wjr {

namespace mem::detail {
class __dynamic_object_pool_impl {
    using node_type = intrusive::hlist_node<>;

public:
    __dynamic_object_pool_impl() = default;
    __dynamic_object_pool_impl(const __dynamic_object_pool_impl &) = delete;
    __dynamic_object_pool_impl(__dynamic_object_pool_impl &&other) {
        replace(&other.m_head, &m_head);
    }

    __dynamic_object_pool_impl(const __dynamic_object_pool_impl &) = delete;
    __dynamic_object_pool_impl &operator=(__dynamic_object_pool_impl &&other) {
        replace(&other.m_head, &m_head);
        return *this;
    }

    ~__dynamic_object_pool_impl() = default;

    WJR_MALLOC void *allocate(arena &m_arena, size_t n) noexcept {
        if (m_head.next() != nullptr) {
            return m_head.pop_front();
        }

        return m_arena.allocate(n);
    }

    void deallocate(void *ptr) noexcept { m_head.push_front(static_cast<node_type *>(ptr)); }

private:
    node_type m_head;
};
} // namespace mem::detail

template <typename T, typename Traits>
class dynamic_object_pool {
    static constexpr size_t tableSize = Traits::size;

public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;
    using is_always_equal = std::false_type;
    using is_trivially_allocator = std::true_type;

    template <typename Other>
    struct rebind {
        using other = dynamic_object_pool<Other, Traits>;
    };

    dynamic_object_pool(uint32_t cache = arena::default_cache_size,
                        Rule rule = arena::Rule::Expansion_Constant)
        : m_arena(cache, rule) {}

    dynamic_object_pool(const dynamic_object_pool &) = delete;
    dynamic_object_pool(dynamic_object_pool &&) = default;
    dynamic_object_pool &operator=(const dynamic_object_pool &) = delete;
    dynamic_object_pool &operator=(dynamic_object_pool &&) = default;
    ~dynamic_object_pool() = default;

    WJR_NODISCARD WJR_MALLOC T *allocate(size_type n) noexcept {
        if (WJR_UNLIKELY(n == 0))
            return nullptr;

        n *= sizeof(T);

        if (auto [idx, size] = Traits::get(n); WJR_LIKELY(idx != tableSize)) {
            return static_cast<T *>(m_pool[idx].allocate(m_arena, size));
        }

        return static_cast<T *>(Traits::allocate_large(n));
    }

    void deallocate(T *ptr, size_type n) noexcept {
        n *= sizeof(T);

        if (auto [idx, _] = Traits::get(n); WJR_LIKELY(idx != tableSize)) {
            return m_pool[idx].deallocate(ptr);
        }

        return Traits::deallocate_large(ptr, n);
    }

    friend bool operator==(const xalloc &lhs, const xalloc &rhs) noexcept {
        return std::addressof(lhs) == std::addressof(rhs);
    }

    friend bool operator!=(const xalloc &lhs, const xalloc &rhs) noexcept { return !(lhs == rhs); }

private:
    arena m_arena;
    mem::detail::__dynamic_object_pool_impl m_pool[tableSize];
};

template <typename T>
class object_pool {
    using node_type = intrusive::hlist_node<>;

public:
    static constexpr size_t default_cache_size = arena::default_cache_size;

    object_pool(size_t cache = default_cache_size) : m_arena(cache) {}

    WJR_MALLOC T *allocate() noexcept {
        if (m_head.next() != nullptr) {
            auto *ptr = m_head.pop_front();
            return reinterpret_cast<T *>(ptr);
        }

        return static_cast<T *>(m_arena.allocate(sizeof(T)));
    }

    void deallocate(T *ptr) noexcept {
        auto *node = reinterpret_cast<node_type *>(ptr);
        m_head.push_front(node);
    }

private:
    node_type m_head;
    arena m_arena;
};

} // namespace wjr

#endif // WJR_MEMORY_OBJECT_POOL_HPP__