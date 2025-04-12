#ifndef WJR_MEMORY_OBJECT_POOL_HPP__
#define WJR_MEMORY_OBJECT_POOL_HPP__

#include <wjr/container/forward_list.hpp>
#include <wjr/memory/arena.hpp>

namespace wjr {

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