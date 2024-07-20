#ifndef WJR_JSON_TOKEN_READER_HPP__
#define WJR_JSON_TOKEN_READER_HPP__

#include <wjr/json/lexer.hpp>
#include <wjr/vector.hpp>

namespace wjr::json {

template <typename Reader>
using token_reader_enabler =
    enable_special_members_base<false, true, true, true, true, true, Reader>;

class token_reader : token_reader_enabler<token_reader> {
    using Mybase = token_reader_enabler<token_reader>;
    using storage_type = vector<uint32_t>;

public:
    using value_type = uint32_t;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = typename lexer::size_type;

    using iterator = typename storage_type::iterator;
    using const_iterator = typename storage_type::const_iterator;
    using reverse_iterator = typename storage_type::reverse_iterator;
    using const_reverse_iterator = typename storage_type::const_reverse_iterator;

    using Mybase::Mybase;

    WJR_CONSTEXPR20 token_reader(span<const char> sp) noexcept
        : Mybase(enable_default_constructor) {
        read(sp);
    }

    constexpr iterator begin() noexcept { return tokens.begin(); }
    constexpr const_iterator begin() const noexcept { return tokens.begin(); }
    constexpr const_iterator cbegin() const noexcept { return tokens.begin(); }

    constexpr iterator end() noexcept { return tokens.end(); }
    constexpr const_iterator end() const noexcept { return tokens.end(); }
    constexpr const_iterator cend() const noexcept { return tokens.end(); }

    constexpr reverse_iterator rbegin() noexcept { return tokens.rbegin(); }
    constexpr const_reverse_iterator rbegin() const noexcept { return tokens.rbegin(); }
    constexpr const_reverse_iterator crbegin() const noexcept { return tokens.rbegin(); }

    constexpr reverse_iterator rend() noexcept { return tokens.rend(); }
    constexpr const_reverse_iterator rend() const noexcept { return tokens.rend(); }
    constexpr const_reverse_iterator crend() const noexcept { return tokens.rend(); }

    constexpr pointer data() noexcept { return tokens.data(); }
    constexpr const_pointer data() const noexcept { return tokens.data(); }

    constexpr size_type size() const noexcept {
        return static_cast<size_type>(tokens.size());
    }

private:
    void read(span<const char> sp) noexcept {
        lexer lex(sp);
        const size_type n = sp.size();
        size_type capacity = n <= 2048 ? n : std::max<size_type>(2048, n / 32);
        size_type buf_size = capacity;
        json::lexer::result_type result;

        do {
            tokens.reserve(capacity + 64);
            result = lex.read(tokens.end_unsafe(), buf_size);
            tokens.get_storage().size() += result.get();
            buf_size = capacity;
            capacity <<= 1;
        } while (!result.done());
    }

    vector<uint32_t> tokens;
};

} // namespace wjr::json

#endif // WJR_JSON_TOKEN_READER_HPP__