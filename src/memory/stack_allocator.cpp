#include <wjr/memory/automatic_free_pool.hpp>
#include <wjr/memory/stack_allocator.hpp>

namespace wjr {

namespace {
static thread_local automatic_free_pool __stack_free_pool;
}

void stack_allocator_object::__large_deallocate(large_memory *buffer) noexcept {
    WJR_ASSERT_ASSUME(buffer != nullptr);
    do {
        auto *const prev = buffer->prev;
        mem::__default_deallocate(buffer, std::nothrow);
        buffer = prev;
    } while (buffer != nullptr);
}

void *stack_allocator_object::__reallocate_impl(size_t n, stack_context &context) noexcept {
    static constexpr uint_fast32_t capacity_grow = 16;

    if (n >= stack_allocator_threshold) {
        return __large_allocate(n, context.m_large);
    }

    do {
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
                const size_t capacity = cache_size;
                auto *const buffer = static_cast<std::byte *>(__stack_free_pool.allocate(capacity));
                m_cache = m_ptr[0] = {buffer, buffer + capacity};
            }

            context.m_ptr = m_cache.ptr;
            break;
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

            const size_t capacity = cache_size << ((2 * m_idx + 3) / 5);
            auto *const buffer = static_cast<std::byte *>(__stack_free_pool.allocate(capacity));
            m_ptr[m_idx] = {buffer, buffer + capacity};
        }

        m_cache = m_ptr[m_idx];
    } while (0);

    auto *const ret = wjr::assume_aligned<mem::default_new_alignment>(m_cache.ptr);
    m_cache.ptr += align_up(n, mem::default_new_alignment);
    return ret;
}

void stack_allocator_object::__redeallocate() noexcept {
    const uint_fast32_t new_size = m_idx + bufsize - 1;
    auto &pool = __stack_free_pool;
    for (uint_fast32_t i = new_size; i < m_size; ++i) {
        pool.deallocate(m_ptr[i].ptr);
    }

    m_size = new_size;
}

} // namespace wjr