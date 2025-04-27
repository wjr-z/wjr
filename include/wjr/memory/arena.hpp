#ifndef WJR_MEMORY_ARENA_HPP__
#define WJR_MEMORY_ARENA_HPP__

#include <wjr/math/literals.hpp>
#include <wjr/memory/automatic_free_pool.hpp>

namespace wjr {

class arena {
public:
    enum Rule : uint32_t {
        Expansion_Constant = 0x00,
        Expansion_Factor1_5 = 0x01,
        Expansion_Factor2 = 0x02,
        Expansion_Mask = 0x03,
    };

    static constexpr uint32_t default_cache_size = 4_KB;

    arena(uint32_t cache = default_cache_size, Rule rule = Rule::Expansion_Constant)
        : m_cache(cache), m_rule(rule) {}

    arena(const arena &) = delete;
    arena(arena &&other) = default;
    arena &operator=(const arena &) = delete;
    arena &operator=(arena &&other) = default;

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
    uint32_t m_cache;
    Rule m_rule;
    automatic_free_pool m_pool;
};

} // namespace wjr

#endif // WJR_MEMORY_ARENA_HPP__