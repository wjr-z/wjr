#include <wjr/memory/stack_allocator.hpp>

namespace wjr {

void stack_allocator_object::__large_deallocate(large_memory *buffer) noexcept {
    WJR_ASSERT(buffer != nullptr);
    do {
        auto *const prev = buffer->prev;
        free(buffer);
        buffer = prev;
    } while (buffer != nullptr);
}

void stack_allocator_object::__small_reallocate(char *&restore_ptr) noexcept {
    static constexpr uint_fast32_t capacity_grow = 16;

    // This is the initial state
    if (WJR_UNLIKELY(m_capacity == 0)) {
        {
            const uint_fast32_t new_capacity = capacity_grow;
            std::allocator<alloc_node> pool;
            m_ptr = pool.allocate(new_capacity);
            m_size = 1;
            m_capacity = new_capacity;
        }

        {
            const size_t capacity = Cache;
            __default_alloc_template__ pool;
            auto *const buffer = static_cast<char *>(pool.chunk_allocate(capacity));
            m_cache = m_ptr[0] = {buffer, buffer + capacity};
        }

        restore_ptr = m_cache.ptr;
        return;
    }

    ++m_idx;
    if (WJR_UNLIKELY(m_idx == m_size)) {
        if (WJR_UNLIKELY(m_size == m_capacity)) {
            const uint_fast32_t new_capacity = m_capacity + capacity_grow;
            std::allocator<alloc_node> pool;
            auto *const new_ptr = pool.allocate(new_capacity);

            std::copy_n(m_ptr, m_idx, new_ptr);
            pool.deallocate(m_ptr, m_capacity);

            m_ptr = new_ptr;
            m_capacity = new_capacity;
        }

        ++m_size;

        const size_t capacity = Cache << ((2 * m_idx + 3) / 5);
        __default_alloc_template__ pool;
        auto *const buffer = static_cast<char *>(pool.chunk_allocate(capacity));
        m_ptr[m_idx] = {buffer, buffer + capacity};
    }

    m_cache = m_ptr[m_idx];
}

} // namespace wjr