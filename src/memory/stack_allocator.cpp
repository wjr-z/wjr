#include "wjr/memory/asan.hpp"
#include <wjr/memory/automatic_free_pool.hpp>
#include <wjr/memory/stack_allocator.hpp>

namespace wjr {

namespace {
inline constexpr size_t stack_allocator_cache_size =
    align_up(std::max<size_t>(16_KB, stack_allocator_threshold), mem::default_new_alignment);

static thread_local automatic_free_pool _stack_free_pool;
} // namespace

#if defined(WJR_HAS_ADDRESS_SANITIZER)
void stack_allocator_object::_unpoison_small(const stack_context &context) noexcept {
    auto *const old_ptr = context.m_ptr == nullptr ? m_first : context.m_ptr;
    const auto old_idx = context.m_idx;

    auto *const new_ptr = m_cache.ptr;

    if (WJR_LIKELY(old_idx == m_idx)) {
        WJR_ASAN_UNPOISON_MEMORY_REGION(old_ptr, static_cast<size_t>(new_ptr - old_ptr));
        return;
    }

    // last block: [m_ptr[m_idx].ptr, m_cache.ptr)
    WJR_ASAN_UNPOISON_MEMORY_REGION(m_ptr[m_idx].ptr,
                                    static_cast<size_t>(new_ptr - m_ptr[m_idx].ptr));

    // ranges: [m_ptr[i].ptr, m_ptr[i].end) for i in [old_idx + 1, m_idx)
    for (uint_fast32_t i = old_idx + 1; i < m_idx; ++i) {
        WJR_ASAN_UNPOISON_MEMORY_REGION(m_ptr[i].ptr,
                                        static_cast<size_t>(m_ptr[i].end - m_ptr[i].ptr));
    }

    // first block: [old_ptr, m_ptr[context.m_idx].end)
    WJR_ASAN_UNPOISON_MEMORY_REGION(old_ptr,
                                    static_cast<size_t>(m_ptr[context.m_idx].end - old_ptr));
}
#endif

void stack_allocator_object::_small_redeallocate() noexcept {
    const uint_fast32_t new_size = m_idx + bufsize - 1;
    for (uint_fast32_t i = new_size; i < m_size; ++i) {
        _stack_free_pool.deallocate(m_ptr[i].ptr);
    }

    m_size = new_size;
}

void stack_allocator_object::_large_deallocate(large_memory *buffer) noexcept {
    WJR_ASSERT(buffer != nullptr);
    do {
        auto *const prev = buffer->prev;
        mem::_default_deallocate(buffer, std::nothrow);
        buffer = prev;
    } while (buffer != nullptr);
}

void stack_allocator_object::_small_reallocate() noexcept {
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
            const size_t capacity = stack_allocator_cache_size;
            auto *const buffer = static_cast<std::byte *>(_stack_free_pool.allocate(capacity));
            m_cache = m_ptr[0] = {buffer, buffer + capacity};
            WJR_ASAN_POISON_MEMORY_REGION(buffer, capacity);
        }

        m_first = m_cache.ptr;
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

        const size_t capacity = stack_allocator_cache_size << ((2 * m_idx + 3) / 5);
        auto *const buffer = static_cast<std::byte *>(_stack_free_pool.allocate(capacity));
        m_ptr[m_idx] = {buffer, buffer + capacity};
        WJR_ASAN_POISON_MEMORY_REGION(buffer, capacity);
    }

    m_cache = m_ptr[m_idx];
}

} // namespace wjr