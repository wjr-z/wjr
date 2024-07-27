#ifndef WJR_JSON_READER_HPP__
#define WJR_JSON_READER_HPP__

#include <wjr/json/lexer.hpp>
#include <wjr/vector.hpp>

namespace wjr::json {

/**
 * @brief Save position of all m_tokens.
 *
 */
class reader {
    using Storage = vector<uint32_t>;

public:
    using value_type = uint32_t;
    using const_pointer = const char *;
    using size_type = typename lexer::size_type;
    using const_iterator = typename Storage::const_iterator;

    reader() = default;
    reader(const reader &) = default;
    reader(reader &&) = default;
    reader &operator=(const reader &) = default;
    reader &operator=(reader &&) = default;
    ~reader() = default;

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
        m_tokens.clear();

        if (WJR_BUILTIN_CONSTANT_P_TRUE(sp.size() == 0)) {
            return;
        }

        __read_impl();
    }

    void shrink_to_fit() noexcept { m_tokens.shrink_to_fit(); }

private:
    void __read_impl() noexcept {
        lexer lex(m_str);
        const size_type n = m_str.size();
        size_type capacity = n <= 2048 ? n : std::max<size_type>(2048, n / 20);
        size_type buf_size = capacity;
        json::lexer::result_type result;

        do {
            m_tokens.reserve(capacity + 64);
            result = lex.read(m_tokens.end_unsafe(), buf_size);
            m_tokens.get_storage().size() += result.get();
            buf_size = capacity;
            capacity <<= 1;
        } while (!result.done());
    }

    span<const char> m_str;
    Storage m_tokens;
};

} // namespace wjr::json

#endif // WJR_JSON_READER_HPP__