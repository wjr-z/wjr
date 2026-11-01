#ifndef WJR_JSON_YYJSON_HPP__
#define WJR_JSON_YYJSON_HPP__

#if !defined(WJR_YYJSON_013) || WJR_YYJSON_013 != 1
    #error "Include wjr/json/yyjson.hpp through the wjr::yyjson target."
#endif

#include <yyjson.h>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <string_view>
#include <utility>

#include <wjr/utility/assert.hpp>

static_assert(YYJSON_VERSION_HEX == 0x000D00, "WJR reviewed yyjson 0.13.0 exactly.");
#if YYJSON_DISABLE_READER || YYJSON_DISABLE_UTF8_VALIDATION
    #error "The yyjson component requires the reader and UTF-8 validation."
#endif

namespace wjr::json::yyjson {

class array_view;
class object_view;

/** @brief One borrowed native value; copying a view never retains its document. */
class value_view {
public:
    constexpr value_view() noexcept = default;
    explicit constexpr value_view(const yyjson_val *value) noexcept : m_value(value) {}

    explicit constexpr operator bool() const noexcept { return m_value != nullptr; }
    constexpr const yyjson_val *native_handle() const noexcept { return m_value; }
    yyjson_type type() const noexcept { return yyjson_get_type(m_value); }
    bool is_null() const noexcept { return yyjson_is_null(m_value); }
    bool is_bool() const noexcept { return yyjson_is_bool(m_value); }
    bool is_uint() const noexcept { return yyjson_is_uint(m_value); }
    bool is_sint() const noexcept { return yyjson_is_sint(m_value); }
    bool is_number() const noexcept { return yyjson_is_num(m_value); }
    bool is_string() const noexcept { return yyjson_is_str(m_value); }
    bool is_array() const noexcept { return yyjson_is_arr(m_value); }
    bool is_object() const noexcept { return yyjson_is_obj(m_value); }

    /** @brief Checked extraction leaves output unchanged on a type mismatch or missing value. */
    bool try_get_bool(bool &output) const noexcept {
        if (!is_bool())
            return false;
        output = yyjson_get_bool(m_value);
        return true;
    }
    bool try_get_uint(uint64_t &output) const noexcept {
        if (!is_uint())
            return false;
        output = yyjson_get_uint(m_value);
        return true;
    }
    bool try_get_sint(int64_t &output) const noexcept {
        if (!is_sint())
            return false;
        output = yyjson_get_sint(m_value);
        return true;
    }
    bool try_get_number(double &output) const noexcept {
        if (!is_number())
            return false;
        output = yyjson_get_num(m_value);
        return true;
    }
    bool try_get_string(std::string_view &output) const noexcept {
        if (!is_string())
            return false;
        output = {yyjson_get_str(m_value), yyjson_get_len(m_value)};
        return true;
    }
    bool try_get_array(array_view &output) const noexcept;
    bool try_get_object(object_view &output) const noexcept;

    /** @pre is_bool() is true. */
    bool get_bool_unchecked() const noexcept {
        const bool matches = is_bool();
        WJR_ASSERT_ASSUME_L2(matches);
        return yyjson_get_bool(m_value);
    }
    /** @pre is_uint() is true; signed values are not implicitly reinterpreted. */
    uint64_t get_uint_unchecked() const noexcept {
        const bool matches = is_uint();
        WJR_ASSERT_ASSUME_L2(matches);
        return yyjson_get_uint(m_value);
    }
    /** @pre is_sint() is true; unsigned values are not implicitly reinterpreted. */
    int64_t get_sint_unchecked() const noexcept {
        const bool matches = is_sint();
        WJR_ASSERT_ASSUME_L2(matches);
        return yyjson_get_sint(m_value);
    }
    /** @pre is_number() is true; integer-to-double conversion follows yyjson_get_num(). */
    double get_number_unchecked() const noexcept {
        const bool matches = is_number();
        WJR_ASSERT_ASSUME_L2(matches);
        return yyjson_get_num(m_value);
    }
    /** @pre is_string() is true; the returned bytes borrow the document. */
    std::string_view get_string_unchecked() const noexcept {
        const bool matches = is_string();
        WJR_ASSERT_ASSUME_L2(matches);
        return {yyjson_get_str(m_value), yyjson_get_len(m_value)};
    }
    /** @pre is_array() is true. */
    array_view get_array_unchecked() const noexcept;
    /** @pre is_object() is true. */
    object_view get_object_unchecked() const noexcept;

private:
    const yyjson_val *m_value = nullptr;
};

/** @brief A borrowed forward range over the reviewed native tape traversal protocol. */
class array_view {
public:
    class iterator {
    public:
        using value_type = value_view;
        using reference = value_view;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;

        constexpr iterator() noexcept = default;
        /** @pre The iterator is dereferenceable. */
        value_view operator*() const noexcept {
            WJR_ASSERT_L2(m_current != nullptr);
            return value_view(m_current);
        }
        /** @pre The iterator is dereferenceable. */
        iterator &operator++() noexcept {
            WJR_ASSERT_L2(m_current != nullptr);
            m_current = unsafe_yyjson_get_next(m_current);
            return *this;
        }
        iterator operator++(int) noexcept {
            auto copy = *this;
            ++*this;
            return copy;
        }
        friend bool operator==(const iterator &left, const iterator &right) noexcept {
            return left.m_current == right.m_current;
        }

    private:
        friend class array_view;
        explicit constexpr iterator(yyjson_val *value) noexcept : m_current(value) {}
        yyjson_val *m_current = nullptr;
    };

    constexpr array_view() noexcept = default;
    size_t size() const noexcept { return yyjson_arr_size(m_value); }
    iterator begin() const noexcept {
        return iterator(m_value ? unsafe_yyjson_get_first(m_value) : nullptr);
    }
    iterator end() const noexcept {
        return iterator(m_value ? unsafe_yyjson_get_next(m_value) : nullptr);
    }
    /** @brief Linear indexed access; a missing/out-of-bounds value is an invalid view. */
    value_view at(size_t index) const noexcept {
        return value_view(yyjson_arr_get(m_value, index));
    }

private:
    friend class value_view;
    explicit constexpr array_view(const yyjson_val *value) noexcept : m_value(value) {}
    const yyjson_val *m_value = nullptr;
};

/** @brief An input-order key and value pair; both components borrow the document. */
struct member_view {
    std::string_view key;
    value_view value;
};

/** @brief A borrowed object retaining duplicate keys and their input order. */
class object_view {
public:
    class iterator {
    public:
        using value_type = member_view;
        using reference = member_view;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;

        constexpr iterator() noexcept = default;
        /** @pre The iterator is dereferenceable. */
        member_view operator*() const noexcept {
            WJR_ASSERT_L2(m_current != nullptr);
            return {{yyjson_get_str(m_current), yyjson_get_len(m_current)},
                    value_view(yyjson_obj_iter_get_val(m_current))};
        }
        /** @pre The iterator is dereferenceable. */
        iterator &operator++() noexcept {
            WJR_ASSERT_L2(m_current != nullptr);
            m_current = unsafe_yyjson_get_next(yyjson_obj_iter_get_val(m_current));
            return *this;
        }
        iterator operator++(int) noexcept {
            auto copy = *this;
            ++*this;
            return copy;
        }
        friend bool operator==(const iterator &left, const iterator &right) noexcept {
            return left.m_current == right.m_current;
        }

    private:
        friend class object_view;
        explicit constexpr iterator(yyjson_val *value) noexcept : m_current(value) {}
        yyjson_val *m_current = nullptr;
    };

    constexpr object_view() noexcept = default;
    size_t size() const noexcept { return yyjson_obj_size(m_value); }
    iterator begin() const noexcept {
        return iterator(m_value ? unsafe_yyjson_get_first(m_value) : nullptr);
    }
    iterator end() const noexcept {
        return iterator(m_value ? unsafe_yyjson_get_next(m_value) : nullptr);
    }
    /** @brief Linear native lookup, choosing the first duplicate; missing returns an invalid view.
     */
    value_view find_first(std::string_view key) const noexcept {
        return value_view(yyjson_obj_getn(m_value, key.data() ? key.data() : "", key.size()));
    }
    /** @brief Linear lookup choosing the last duplicate without allocating an index. */
    value_view find_last(std::string_view key) const noexcept {
        value_view result;
        for (auto member : *this)
            if (member.key == key)
                result = member.value;
        return result;
    }

private:
    friend class value_view;
    explicit constexpr object_view(const yyjson_val *value) noexcept : m_value(value) {}
    const yyjson_val *m_value = nullptr;
};

inline bool value_view::try_get_array(array_view &output) const noexcept {
    if (!is_array())
        return false;
    output = array_view(m_value);
    return true;
}
inline bool value_view::try_get_object(object_view &output) const noexcept {
    if (!is_object())
        return false;
    output = object_view(m_value);
    return true;
}
inline array_view value_view::get_array_unchecked() const noexcept {
    const bool matches = is_array();
    WJR_ASSERT_ASSUME_L2(matches);
    return array_view(m_value);
}
inline object_view value_view::get_object_unchecked() const noexcept {
    const bool matches = is_object();
    WJR_ASSERT_ASSUME_L2(matches);
    return object_view(m_value);
}

/** @brief A move-only, independently owning strict yyjson document; no shared control block. */
class document {
public:
    constexpr document() noexcept = default;
    document(const document &) = delete;
    document &operator=(const document &) = delete;
    document(document &&other) noexcept : m_document(std::exchange(other.m_document, nullptr)) {}
    document &operator=(document &&other) noexcept {
        auto *next = std::exchange(other.m_document, nullptr);
        reset();
        m_document = next;
        return *this;
    }
    ~document() { reset(); }

    /** @brief Parse copied/decoded storage; on failure return an empty owner and optional native
     * error. */
    [[nodiscard]] static document parse(std::string_view input,
                                        yyjson_read_err *error = nullptr) noexcept {
        return document(yyjson_read_opts(const_cast<char *>(input.data()), input.size(),
                                         YYJSON_READ_NOFLAG, nullptr, error));
    }
    explicit constexpr operator bool() const noexcept { return m_document != nullptr; }
    /** @brief Release storage and invalidate every view/iterator/string borrowed from it. */
    void reset() noexcept {
        if (m_document)
            yyjson_doc_free(std::exchange(m_document, nullptr));
    }
    value_view root() const & noexcept { return value_view(yyjson_doc_get_root(m_document)); }
    value_view root() && = delete;
    value_view root() const && = delete;
    const yyjson_doc *native_handle() const & noexcept { return m_document; }
    const yyjson_doc *native_handle() && = delete;
    const yyjson_doc *native_handle() const && = delete;

private:
    explicit document(yyjson_doc *value) noexcept : m_document(value) {}
    yyjson_doc *m_document = nullptr;
};

} // namespace wjr::json::yyjson

template <>
inline constexpr bool std::ranges::enable_view<wjr::json::yyjson::array_view> = true;
template <>
inline constexpr bool std::ranges::enable_borrowed_range<wjr::json::yyjson::array_view> = true;
template <>
inline constexpr bool std::ranges::enable_view<wjr::json::yyjson::object_view> = true;
template <>
inline constexpr bool std::ranges::enable_borrowed_range<wjr::json::yyjson::object_view> = true;

#endif // WJR_JSON_YYJSON_HPP__
