#include <wjr/memory/arena.hpp>

namespace wjr {
void *arena::__allocate_large(size_t n) noexcept {
    if (n > m_cache / 4) {
        return m_pool.allocate(n);
    }

    void *const raw = m_pool.allocate(m_cache);
    m_start = static_cast<std::byte *>(raw);
    m_end = m_start + m_cache;
    m_start += align_up(n, mem::default_new_alignment);
    return raw;
}
} // namespace wjr