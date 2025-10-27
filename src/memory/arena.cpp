#include <wjr/memory/arena.hpp>

namespace wjr {
void *arena::_allocate_large(size_t n) noexcept {
    switch (m_rule & Rule::Expansion_Mask) {
    case Rule::Expansion_Constant: {
        break;
    }
    case Rule::Expansion_Factor1_5: {
        m_cache += m_cache / 2;
        break;
    }
    case Rule::Expansion_Factor2: {
        m_cache += m_cache;
        break;
    }
    default: {
        WJR_UNREACHABLE();
    }
    }

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