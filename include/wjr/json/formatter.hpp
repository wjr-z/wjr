#ifndef WJR_FORMATTER_HPP__
#define WJR_FORMATTER_HPP__

#include <wjr/container/generic/bitset.hpp>
#include <wjr/format/charconv.hpp>
#include <wjr/format/dragonbox.hpp>
#include <wjr/json/detail.hpp>
#include <wjr/vector.hpp>

namespace wjr::json {

namespace formatter_detail {

template <typename Container>
WJR_INTRINSIC_INLINE void append_string(Container &cont, const char *str, size_t length) {
    try_uninitialized_append(cont, length);
    std::memcpy(cont.data() + cont.size() - length, str, length);
}

struct escape_sequence {
    uint8_t length;
    const char string[7]; // technically, we only ever need 6 characters, we pad to 8
};

template <typename Container>
WJR_INTRINSIC_INLINE void format_string(Container &cont, std::string_view str) {
    const auto length = str.size();
    const auto old_size = cont.size();
    try_uninitialized_append(cont, length + 2);
    auto *ptr = cont.data() + old_size;
    *ptr++ = '\"';

    auto first = str.data();

    size_t i = 0;
    // Fast path for the case where we have no control character, no ", and no backslash.
    // This should include most keys.
    //
    // We would like to use 'bool' but some compilers take offense to bitwise operation
    // with bool types.
    constexpr static char needs_escaping[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (; i + 8 <= length; i += 8) {
        // Poor's man vectorization. This could get much faster if we used SIMD.
        //
        // It is not the case that replacing '|' with '||' would be neutral
        // performance-wise.
        if (needs_escaping[uint8_t(first[i])] | needs_escaping[uint8_t(first[i + 1])] |
            needs_escaping[uint8_t(first[i + 2])] |
            needs_escaping[uint8_t(first[i + 3])] |
            needs_escaping[uint8_t(first[i + 4])] |
            needs_escaping[uint8_t(first[i + 5])] |
            needs_escaping[uint8_t(first[i + 6])] |
            needs_escaping[uint8_t(first[i + 7])]) {
            goto SLOW_PATH;
        }
    }

    for (; i < length; i++) {
        if (needs_escaping[uint8_t(first[i])]) {
            goto SLOW_PATH;
        }
    }

    // The following is also possible and omits a 256-byte table, but it is slower:
    // for (; (i < unescaped.length()) && (uint8_t(unescaped[i]) > 0x1F)
    //      && (unescaped[i] != '\"') && (unescaped[i] != '\\'); i++) {}

    // At least for long strings, the following should be fast. We could
    // do better by integrating the checks and the insertion.
    std::memcpy(ptr, first, length);
    ptr += length;
    *ptr++ = '\"';
    return;

SLOW_PATH : {
    std::memcpy(ptr, first, i);
    ptr += i;
    try_uninitialized_resize(cont, ptr - cont.data());

    // We caught a control character if we enter this loop (slow).
    // Note that we are do not restart from the beginning, but rather we continue
    // from the point where we encountered something that requires escaping.
    for (; i < length; i++) {
        switch (first[i]) {
        case '\"': {
            append_string(cont, "\\\"", 2);
            break;
        }
        case '\\': {
            append_string(cont, "\\\\", 2);
            break;
        }
        default:
            if (uint8_t(first[i]) <= 0x1F) {
                // If packed, this uses 8 * 32 bytes.
                // Note that we expect most compilers to embed this code in the data
                // section.
                constexpr static escape_sequence escaped[32] = {
                    {6, "\\u0000"}, {6, "\\u0001"}, {6, "\\u0002"}, {6, "\\u0003"},
                    {6, "\\u0004"}, {6, "\\u0005"}, {6, "\\u0006"}, {6, "\\u0007"},
                    {2, "\\b"},     {2, "\\t"},     {2, "\\n"},     {6, "\\u000b"},
                    {2, "\\f"},     {2, "\\r"},     {6, "\\u000e"}, {6, "\\u000f"},
                    {6, "\\u0010"}, {6, "\\u0011"}, {6, "\\u0012"}, {6, "\\u0013"},
                    {6, "\\u0014"}, {6, "\\u0015"}, {6, "\\u0016"}, {6, "\\u0017"},
                    {6, "\\u0018"}, {6, "\\u0019"}, {6, "\\u001a"}, {6, "\\u001b"},
                    {6, "\\u001c"}, {6, "\\u001d"}, {6, "\\u001e"}, {6, "\\u001f"}};
                auto u = escaped[uint8_t(first[i])];
                if (u.length == 2) {
                    append_string(cont, u.string, 2);
                } else {
                    append_string(cont, u.string, 6);
                }
            } else {
                cont.push_back(first[i]);
            }
        } // switch
    }     // for

    cont.push_back('\"');
    return;
}
}

} // namespace formatter_detail

template <typename Inserter, typename formatter>
class base_formatter {
protected:
    WJR_INTRINSIC_INLINE void __append_str(const char *str, size_t length) {
        formatter_detail::append_string(get_inserter_container(m_iter), str, length);
    }

    WJR_INTRINSIC_INLINE void __append_char(char ch) { *m_iter++ = ch; }

public:
    base_formatter(Inserter iter) noexcept : m_iter(iter) {}

    base_formatter() = delete;
    base_formatter(const base_formatter &) = default;
    base_formatter(base_formatter &&) = default;
    base_formatter &operator=(const base_formatter &) = default;
    base_formatter &operator=(base_formatter &&) = default;
    ~base_formatter() = default;

    /// @brief Prints a null
    WJR_INTRINSIC_INLINE void format_null() { __append_str("null", 4); }
    /// @brief Prints a true
    WJR_INTRINSIC_INLINE void format_true() { __append_str("true", 4); }
    /// @brief Prints a false
    WJR_INTRINSIC_INLINE void format_false() { __append_str("false", 5); }
    /// @brief Start an object
    WJR_INTRINSIC_INLINE void format_start_object() { __append_char('{'); }
    /// @brief End an object
    WJR_INTRINSIC_INLINE void format_end_object() { __append_char('}'); }
    /// @brief Start an array
    WJR_INTRINSIC_INLINE void format_start_array() { __append_char('['); }
    /// @brief End an array
    WJR_INTRINSIC_INLINE void format_end_array() { __append_char(']'); }
    /// @brief Add a comma
    WJR_INTRINSIC_INLINE void format_comma() { __append_char(','); }
    /// @brief Prints a number
    WJR_INTRINSIC_INLINE void format_number_unsigned(uint64_t x) {
        to_chars_unchecked(m_iter, x);
    }
    /// @brief Prints a number
    WJR_INTRINSIC_INLINE void format_number_signed(int64_t x) {
        to_chars_unchecked(m_iter, x);
    }
    /// @brief Prints a number
    WJR_INTRINSIC_INLINE void format_number_float(double x) {
        auto &cont = get_inserter_container(m_iter);
        const auto old_size = cont.size();
        try_uninitialized_append(cont, dragonbox::max_output_string_length_of<double>);
        auto *const ptr = cont.data() + old_size;
        try_uninitialized_resize(cont, (dragonbox::to_chars(x, ptr) - cont.data()));
    }
    /// @brief Prints a key (string + colon)
    WJR_INTRINSIC_INLINE void format_key(std::string_view str) {
        format_string(str);
        __append_char(':');
    }

    /// @brief Prints a string. The string is escaped as needed.
    WJR_INTRINSIC_INLINE void format_string(std::string_view str) {
        formatter_detail::format_string(get_inserter_container(m_iter), str);
    }

    WJR_INTRINSIC_INLINE void format_newline() { this->print_newline(); }

    WJR_INTRINSIC_INLINE void format_indents(size_t depth) { this->print_indents(depth); }

    WJR_INTRINSIC_INLINE void format_space() { this->print_space(); }

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

} // namespace wjr::json

#endif // WJR_FORMATTER_HPP__