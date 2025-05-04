#ifndef WJR_JSON_READER_HPP__
#define WJR_JSON_READER_HPP__

#include <wjr/json/lexer.hpp>
#include <wjr/memory/stack_allocator.hpp>
#include <wjr/memory/uninitialized.hpp>
#include <wjr/vector.hpp>

namespace wjr::json {
static_assert(std::is_trivially_destructible_v<lexer>);

class ondemand_reader {
    static constexpr size_t extra_memory = 64;
    static constexpr size_t threshold = 4 * 1024 - extra_memory;

public:
    using const_pointer = const char *;
    using size_type = typename lexer::size_type;

    static constexpr uint32_t npos = UINT32_C(-1);

    void read(std::string_view input) {
        m_input = input;
        m_lexer.emplace(input);
        m_tokens.reserve(std::min<size_t>(size(), threshold) + extra_memory);
        m_ptr = m_tokens.begin_unsafe();
        m_tokens.get_storage().size() = 0;
    }

    WJR_CONSTEXPR20 const_pointer data() const noexcept { return m_input.data(); }
    WJR_CONSTEXPR20 size_type size() const noexcept { return m_input.size(); }

    bool valid() const noexcept { return m_ptr != nullptr; }

    /**
     * @brief
     *
     * @return uint32_t Return -1 if failed.
     */
    uint32_t get() noexcept {
        if (WJR_UNLIKELY(m_ptr == m_tokens.end_unsafe())) {
            if (WJR_UNLIKELY(m_lexer->begin() == m_lexer->end())) {
                return npos;
            }

            auto result =
                m_lexer->read(m_tokens.begin_unsafe(), m_tokens.capacity() - extra_memory);
            WJR_ASSUME(result != 0);

            m_ptr = m_tokens.begin_unsafe();
            m_tokens.get_storage().size() = result;
        }

        return *m_ptr++;
    }

private:
    std::string_view m_input;
    uint32_t *m_ptr = nullptr;
    uninitialized<lexer> m_lexer;
    vector<uint32_t> m_tokens;
};

} // namespace wjr::json

#endif // WJR_JSON_READER_HPP__