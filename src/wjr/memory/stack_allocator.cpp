#include <wjr/memory/stack_allocator.hpp>

namespace wjr {

void stack_allocator_object::__small_reallocate(stack_top &top) noexcept {
    if (WJR_UNLIKELY(top.idx == UINT_FAST16_MAX)) {
        top.idx = m_idx;
    }

    ++m_idx;
    if (WJR_UNLIKELY(m_idx == m_size)) {
        if (WJR_UNLIKELY(m_size == m_capacity)) {
            const uint16_t new_capacity = m_idx + 16;
            std::allocator<alloc_node> pool;
            auto *const new_ptr = pool.allocate(new_capacity);
            if (WJR_LIKELY(m_idx != 0)) {
                std::copy_n(m_ptr, m_idx, new_ptr);
                pool.deallocate(m_ptr, m_capacity);
            }
            m_ptr = new_ptr;
            m_capacity = new_capacity;
        }

        ++m_size;

        const size_t capacity = Cache << ((2 * m_idx + 3) / 5);
        __default_alloc_template__ pool;
        auto *const buffer = static_cast<char *>(pool.chunk_allocate(capacity));
        if (WJR_UNLIKELY(m_idx == 0)) {
            top.ptr = buffer;
            top.idx = 0;
        }

        m_ptr[m_idx] = {buffer, buffer + capacity};
    }

    m_cache = m_ptr[m_idx];
    WJR_ASSERT(top.ptr != nullptr);
}

} // namespace wjr