#ifndef WJR_JSON_READER_HPP__
#define WJR_JSON_READER_HPP__

#include <wjr/json/lexer.hpp>
#include <wjr/vector.hpp>

namespace wjr::json {

/**
 * @brief Save position of all tokens.
 *
 */
class reader {
    using Vector = vector<uint32_t>;

public:
    using value_type = uint32_t;
    using const_pointer = const char *;
    using size_type = typename lexer::size_type;
    using const_iterator = typename Vector::const_iterator;

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(reader);

    reader(span<const char> sp) noexcept { read(sp); }

    WJR_CONSTEXPR20 const_iterator token_begin() const noexcept {
        return m_tokens.begin();
    }

    WJR_CONSTEXPR20 const_iterator token_end() const noexcept { return m_tokens.end(); }

    WJR_CONSTEXPR20 const_pointer data() const noexcept { return m_str.data(); }

    WJR_CONSTEXPR20 size_type size() const noexcept {
        return static_cast<size_type>(m_str.size());
    }

    void read(span<const char> sp) noexcept {
        m_str = sp;

        lexer lex(m_str);
        const size_type n = m_str.size();
        size_type capacity = n <= 2048 ? n : std::max<size_type>(2048, n / 20);
        size_type buf_size = capacity;
        json::lexer::result_type result;
        m_tokens.clear();

        do {
            m_tokens.reserve(capacity + 64);
            result = lex.read(m_tokens.end_unsafe(), buf_size);
            m_tokens.get_storage().size() += result.get();
            buf_size = capacity;
            capacity <<= 1;
        } while (!result.done());
    }

    void clear() noexcept { m_tokens.clear(); }
    void shrink_to_fit() noexcept { m_tokens.shrink_to_fit(); }

private:
    span<const char> m_str;
    Vector m_tokens;
};

} // namespace wjr::json

#endif // WJR_JSON_READER_HPP__