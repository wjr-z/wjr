#ifndef WJR_JSON_FORMATTER_HPP__
#define WJR_JSON_FORMATTER_HPP__

#include <string>

#include <wjr/container/bitset.hpp>
#include <wjr/format/charconv.hpp>
#include <wjr/format/dragonbox.hpp>
#include <wjr/json/detail.hpp>
#include <wjr/vector.hpp>

#if WJR_HAS_SIMD(SSE2)
    #include <emmintrin.h>
#endif

namespace wjr::json {

namespace formatter_detail {

namespace {

template <typename Container>
WJR_INTRINSIC_INLINE void append_string(Container &cont, const char *str, size_t length) {
    if constexpr (has_container_append_v<Container, const char *, size_t>) {
        cont.append(str, length);
    } else if constexpr (has_container_append_v<Container, const char *, const char *>) {
        cont.append(str, str + length);
    } else {
        cont.insert(cont.end(), str, str + length);
    }
}

/// @private Append bytes and publish only the prefix initialized by the writer.
template <typename Container, typename Writer>
WJR_INTRINSIC_INLINE void append_overwrite(Container &cont, size_t capacity, Writer &&writer) {
    const auto old_size = cont.size();
#if defined(__cpp_lib_string_resize_and_overwrite) &&                                              \
    __cpp_lib_string_resize_and_overwrite >= 202110L
    auto write = [&](auto *data, size_t) noexcept { return old_size + writer(data + old_size); };
    if constexpr (requires { cont.resize_and_overwrite(old_size + capacity, write); }) {
        cont.resize_and_overwrite(old_size + capacity, write);
        return;
    }
#endif
    try_uninitialized_append(cont, capacity);
    const auto actual = writer(cont.data() + old_size);
    if (actual != capacity) {
        try_uninitialized_resize(cont, old_size + actual);
    }
}

template <typename Container>
WJR_INTRINSIC_INLINE void append_indents(Container &cont, size_t length) {
    if constexpr (has_container_append_v<Container, size_t, char>) {
        cont.append(length, ' ');
    } else {
        cont.insert(cont.end(), length, ' ');
    }
}

struct escape_sequence {
    uint8_t length;
    const char string[7]; // technically, we only ever need 6 characters, we pad to 8
};

/// @private Bytes requiring JSON string escaping.
constexpr bool needs_escaping(unsigned char ch) noexcept {
    return ch < 0x20 || ch == '"' || ch == '\\';
}

// If packed, this uses 8 * 32 bytes.
// Note that we expect most compilers to embed this code in the
// data section.
inline constexpr escape_sequence escaped[32] = {
    {6, "\\u0000"}, {6, "\\u0001"}, {6, "\\u0002"}, {6, "\\u0003"}, {6, "\\u0004"}, {6, "\\u0005"},
    {6, "\\u0006"}, {6, "\\u0007"}, {2, "\\b"},     {2, "\\t"},     {2, "\\n"},     {6, "\\u000b"},
    {2, "\\f"},     {2, "\\r"},     {6, "\\u000e"}, {6, "\\u000f"}, {6, "\\u0010"}, {6, "\\u0011"},
    {6, "\\u0012"}, {6, "\\u0013"}, {6, "\\u0014"}, {6, "\\u0015"}, {6, "\\u0016"}, {6, "\\u0017"},
    {6, "\\u0018"}, {6, "\\u0019"}, {6, "\\u001a"}, {6, "\\u001b"}, {6, "\\u001c"}, {6, "\\u001d"},
    {6, "\\u001e"}, {6, "\\u001f"}};

/// @private Scan a full block without branching on individual characters.
WJR_INTRINSIC_INLINE bool block_needs_escaping(const char *ptr) noexcept {
#if WJR_HAS_SIMD(SSE2)
    const auto bytes = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(ptr));
    const auto lower = _mm_set1_epi8(0x1f);
    const auto control = _mm_cmpeq_epi8(_mm_max_epu8(bytes, lower), lower);
    const auto quote = _mm_cmpeq_epi8(bytes, _mm_set1_epi8('"'));
    const auto backslash = _mm_cmpeq_epi8(bytes, _mm_set1_epi8('\\'));
    return (_mm_movemask_epi8(_mm_or_si128(control, _mm_or_si128(quote, backslash))) & 0xff) != 0;
#else
    uint64_t bytes;
    std::memcpy(&bytes, ptr, sizeof(bytes));
    const auto quote = bytes ^ UINT64_C(0x2222222222222222);
    const auto backslash = bytes ^ UINT64_C(0x5c5c5c5c5c5c5c5c);
    constexpr auto ones = UINT64_C(0x0101010101010101);
    constexpr auto high_bits = UINT64_C(0x8080808080808080);
    /// @note Borrow propagation can mark extra bytes after a match. Only existence is tested.
    const auto controls = (bytes - UINT64_C(0x2020202020202020)) & ~bytes;
    const auto quotes = (quote - ones) & ~quote;
    const auto backslashes = (backslash - ones) & ~backslash;
    return ((controls | quotes | backslashes) & high_bits) != 0;
#endif
}

template <typename Container>
WJR_INTRINSIC_INLINE void append_escaped_range(Container &cont, const char *first,
                                               const char *last) {
    for (; first != last; ++first) {
        switch (*first) {
        case '"':
            append_string(cont, "\\\"", 2);
            break;
        case '\\':
            append_string(cont, "\\\\", 2);
            break;
        default:
            if (uint8_t(*first) <= 0x1F) {
                const auto u = escaped[uint8_t(*first)];
                if (u.length == 2) {
                    append_string(cont, u.string, 2);
                } else {
                    append_string(cont, u.string, 6);
                }
            } else {
                cont.push_back(*first);
            }
        }
    }
}

/// @private Write through the first escape, or write the complete quoted string.
struct string_prefix_writer {
    const char *first;
    size_t length;
    size_t pos = 0;

    template <typename Byte>
    WJR_INTRINSIC_INLINE size_t operator()(Byte *ptr) noexcept {
        *ptr++ = '"';
        for (; pos + 8 <= length; pos += 8) {
            if (block_needs_escaping(first + pos)) {
                std::memcpy(ptr, first, pos);
                return pos + 1;
            }
        }
        // The final full block can overlap bytes already classified as clean.
        if (pos != length && length >= 8 && !block_needs_escaping(first + length - 8)) {
            pos = length;
        }
        for (; pos != length; ++pos) {
            if (needs_escaping(static_cast<unsigned char>(first[pos]))) {
                std::memcpy(ptr, first, pos);
                return pos + 1;
            }
        }
        std::memcpy(ptr, first, length);
        ptr[length] = '"';
        return length + 2;
    }
};

template <typename Container>
WJR_INTRINSIC_INLINE void format_string(Container &cont, std::string_view str) {
    const auto first = str.data();
    const auto length = str.size();
    if (length == 0) {
        append_string(cont, "\"\"", 2);
        return;
    }

    string_prefix_writer prefix{first, length};
    append_overwrite(cont, length + 2, prefix);
    auto pos = prefix.pos;
    if (pos == length) {
        return;
    }

    while (pos + 8 <= length) {
        append_escaped_range(cont, first + pos, first + pos + 8);
        pos += 8;

        const auto clean_start = pos;
        while (pos + 8 <= length && !block_needs_escaping(first + pos)) {
            pos += 8;
        }
        if (pos != clean_start) {
            append_string(cont, first + clean_start, pos - clean_start);
        }
    }
    if (pos != length) {
        append_escaped_range(cont, first + pos, first + length);
    }
    cont.push_back('"');
}

} // namespace

} // namespace formatter_detail

template <typename Inserter, typename Formatter>
class base_formatter {
protected:
    WJR_INTRINSIC_INLINE void _append_str(const char *str, size_t length) {
        formatter_detail::append_string(get_inserter_container(m_iter), str, length);
    }

    WJR_INTRINSIC_INLINE void _append_char(char ch) { *m_iter++ = ch; }

public:
    base_formatter(Inserter iter) noexcept : m_iter(iter) {}

    base_formatter() = delete;
    base_formatter(const base_formatter &) = default;
    base_formatter(base_formatter &&) = default;
    base_formatter &operator=(const base_formatter &) = default;
    base_formatter &operator=(base_formatter &&) = default;
    ~base_formatter() = default;

    /// @brief Prints a null
    WJR_INTRINSIC_INLINE void format_null() { _append_str("null", 4); }
    /// @brief Prints a true
    WJR_INTRINSIC_INLINE void format_true() { _append_str("true", 4); }
    /// @brief Prints a false
    WJR_INTRINSIC_INLINE void format_false() { _append_str("false", 5); }
    /// @brief Start an object
    WJR_INTRINSIC_INLINE void format_start_object() { _append_char('{'); }
    /// @brief End an object
    WJR_INTRINSIC_INLINE void format_end_object() { _append_char('}'); }
    /// @brief Start an array
    WJR_INTRINSIC_INLINE void format_start_array() { _append_char('['); }
    /// @brief End an array
    WJR_INTRINSIC_INLINE void format_end_array() { _append_char(']'); }
    /// @brief Add a comma
    WJR_INTRINSIC_INLINE void format_comma() { _append_char(','); }
    /// @brief Prints a number
    WJR_INTRINSIC_INLINE void format_number_unsigned(uint64_t x) { to_chars_unchecked(m_iter, x); }
    /// @brief Prints a number
    WJR_INTRINSIC_INLINE void format_number_signed(int64_t x) { to_chars_unchecked(m_iter, x); }
    /// @brief Prints a number
    WJR_INTRINSIC_INLINE void format_number_float(double x) {
        auto &cont = get_inserter_container(m_iter);
        formatter_detail::append_overwrite(
            cont, dragonbox::max_output_string_length_of<double>, [x](char *ptr) noexcept {
                return static_cast<size_t>(dragonbox::to_chars_n(x, ptr) - ptr);
            });
    }
    /// @brief Prints a key (string + colon)
    WJR_INTRINSIC_INLINE void format_key(std::string_view str) {
        format_string(str);
        _append_char(':');
    }

    /// @brief Prints a string. The string is escaped as needed.
    WJR_INTRINSIC_INLINE void format_string(std::string_view str) {
        formatter_detail::format_string(get_inserter_container(m_iter), str);
    }

    WJR_INTRINSIC_INLINE void format_newline() { static_cast<Formatter &>(*this).print_newline(); }

    WJR_INTRINSIC_INLINE void format_indents(size_t depth) {
        static_cast<Formatter &>(*this).print_indents(depth);
    }

    WJR_INTRINSIC_INLINE void format_space() { static_cast<Formatter &>(*this).print_space(); }

protected:
    Inserter m_iter;
};

template <typename Inserter>
class minify_formatter : public base_formatter<Inserter, minify_formatter<Inserter>> {
    using Mybase = base_formatter<Inserter, minify_formatter>;

public:
    using Mybase::Mybase;

    WJR_INTRINSIC_INLINE void print_newline() {}

    WJR_INTRINSIC_INLINE void print_indents(size_t) {}

    WJR_INTRINSIC_INLINE void print_space() {}
};

template <typename Inserter>
minify_formatter(Inserter iter) -> minify_formatter<Inserter>;

template <typename Inserter>
class pretty_formatter : public base_formatter<Inserter, pretty_formatter<Inserter>> {
    using Mybase = base_formatter<Inserter, pretty_formatter>;

public:
    using Mybase::Mybase;

    pretty_formatter(Inserter iter, unsigned indent_step)
        : Mybase(iter), m_indent_step(indent_step) {}

    WJR_INTRINSIC_INLINE void print_newline() { this->_append_char('\n'); }

    WJR_INTRINSIC_INLINE void print_indents(size_t depth) {
        formatter_detail::append_indents(get_inserter_container(*this->m_iter),
                                         depth * m_indent_step);
    }

    WJR_INTRINSIC_INLINE void print_space() { this->_append_char(' '); }

private:
    unsigned m_indent_step = 4;
};

template <typename Inserter>
pretty_formatter(Inserter iter) -> pretty_formatter<Inserter>;

} // namespace wjr::json

#endif // WJR_JSON_FORMATTER_HPP__