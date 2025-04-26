#ifndef WJR_MEMORY_ARENA_HPP__
#define WJR_MEMORY_ARENA_HPP__

#include <wjr/math/literals.hpp>
#include <wjr/memory/automatic_free_pool.hpp>

namespace wjr {

class arena {
public:
    static constexpr size_t default_cache_size = 4_KB;

    arena(size_t cache = default_cache_size) : m_cache(cache) {}

    arena(const arena &) = delete;
    arena(arena &&other)
        : m_start(other.m_start), m_end(other.m_end), m_cache(other.m_cache),
          m_pool(std::move(other.m_pool)) {}
    arena &operator=(const arena &) = delete;
    arena &operator=(arena &&) = delete;

    WJR_MALLOC void *allocate(size_t n) noexcept {
        if (WJR_UNLIKELY(to_unsigned(m_end - m_start) < n)) {
            return __allocate_large(n);
        }

        void *raw = m_start;
        m_start += align_up(n, mem::default_new_alignment);
        return raw;
    }

private:
    void *__allocate_large(size_t n) noexcept;

    std::byte *m_start = nullptr;
    std::byte *m_end = nullptr;
    size_t m_cache;
    automatic_free_pool m_pool;
};

} // namespace wjr

#endif // WJR_MEMORY_ARENA_HPP__