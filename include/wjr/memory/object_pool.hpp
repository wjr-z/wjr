#ifndef WJR_MEMORY_OBJECT_POOL_HPP__
#define WJR_MEMORY_OBJECT_POOL_HPP__

#include <wjr/container/forward_list.hpp>
#include <wjr/memory/arena.hpp>

namespace wjr {

namespace mem::detail {
class _dynamic_object_pool_impl {
    using node_type = intrusive::hlist_node<>;

public:
    _dynamic_object_pool_impl() = default;
    _dynamic_object_pool_impl(const _dynamic_object_pool_impl &) = delete;
    _dynamic_object_pool_impl(_dynamic_object_pool_impl &&other) noexcept {
        replace(&other.m_head, &m_head);
    }

    _dynamic_object_pool_impl &operator=(const _dynamic_object_pool_impl &) = delete;
    _dynamic_object_pool_impl &operator=(_dynamic_object_pool_impl &&other) noexcept {
        replace(&other.m_head, &m_head);
        return *this;
    }

    ~_dynamic_object_pool_impl() = default;

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

template <typename Traits>
class dynamic_object_pool {
    static constexpr size_t tableSize = Traits::size;

public:
    using value_type = typename Traits::value_type;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;
    using is_always_equal = std::false_type;
    using is_trivially_allocator = std::true_type;

    template <typename Other>
    struct rebind {
        using other = dynamic_object_pool<typename Traits::template rebind<Other>::other>;
    };

    dynamic_object_pool(uint32_t cache = arena::default_cache_size,
                        arena::Rule rule = arena::Rule::Expansion_Constant)
        : m_arena(cache, rule) {}

    dynamic_object_pool(const dynamic_object_pool &) = delete;
    dynamic_object_pool(dynamic_object_pool &&) = default;
    dynamic_object_pool &operator=(const dynamic_object_pool &) = delete;
    dynamic_object_pool &operator=(dynamic_object_pool &&) = default;
    ~dynamic_object_pool() = default;

    WJR_NODISCARD WJR_MALLOC value_type *allocate(size_type n) noexcept {
        if (WJR_UNLIKELY(n == 0))
            return nullptr;

        n *= sizeof(value_type);
        if (auto [idx, size] = Traits::get(n); WJR_LIKELY(idx != tableSize)) {
            return static_cast<value_type *>(m_pool[idx].allocate(m_arena, size));
        }

        return static_cast<value_type *>(Traits::allocate_large(n));
    }

    void deallocate(value_type *ptr, size_type n) noexcept {
        n *= sizeof(value_type);
        if (auto [idx, _] = Traits::get(n); WJR_LIKELY(idx != tableSize)) {
            return m_pool[idx].deallocate(ptr);
        }

        return Traits::deallocate_large(ptr, n);
    }

    friend bool operator==(const dynamic_object_pool &lhs,
                           const dynamic_object_pool &rhs) noexcept {
        return std::addressof(lhs) == std::addressof(rhs);
    }

    friend bool operator!=(const dynamic_object_pool &lhs,
                           const dynamic_object_pool &rhs) noexcept {
        return !(lhs == rhs);
    }

private:
    arena m_arena;
    mem::detail::_dynamic_object_pool_impl m_pool[tableSize];
};

namespace mem::detail {
template <typename T>
struct single_object_pool_traits {
    using value_type = T;
    static constexpr size_t size = 1;

    template <typename Other>
    struct rebind {
        using other = single_object_pool_traits<Other>;
    };

    static constexpr size_t get(WJR_MAYBE_UNUSED size_t size) noexcept {
        WJR_ASSERT_L2(size == sizeof(T));
        return 0;
    }
    static constexpr value_type *allocate_large(size_t) noexcept { WJR_UNREACHABLE(); }
    static constexpr value_type *deallocate_large(size_t) noexcept { WJR_UNREACHABLE(); }
};
} // namespace mem::detail

template <typename T>
using object_pool = dynamic_object_pool<mem::detail::single_object_pool_traits<T>>;

} // namespace wjr

#endif // WJR_MEMORY_OBJECT_POOL_HPP__