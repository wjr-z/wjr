#ifndef WJR_JSON_IMMUTABLE_DOCUMENT_HPP__
#define WJR_JSON_IMMUTABLE_DOCUMENT_HPP__

#include <bit>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <new>
#include <string_view>
#include <type_traits>
#include <utility>

#include <wjr/json/detail.hpp>

namespace wjr::json {

class ondemand_reader;
class immutable_document;
class value_view;
struct object_member_view;

namespace detail {
class immutable_builder;
struct immutable_node {
    uint64_t payload;
    uint32_t count;
    uint32_t kind;
};
struct immutable_member {
    uint32_t key_offset;
    uint32_t key_length;
    uint32_t value_index;
};
static_assert(sizeof(immutable_node) == 16 && alignof(immutable_node) <= alignof(std::max_align_t));
static_assert(sizeof(immutable_member) == 12);
static_assert(std::is_trivially_copyable_v<immutable_node> &&
              std::is_trivially_copyable_v<immutable_member>);
} // namespace detail

/// @brief Multipass array access borrowing an immutable document.
class array_view {
public:
    class iterator {
    public:
        using value_type = value_view;
        using difference_type = std::ptrdiff_t;
        using iterator_concept = std::forward_iterator_tag;
        using iterator_category = std::input_iterator_tag;
        iterator() noexcept = default;
        value_view operator*() const noexcept;
        iterator &operator++() noexcept;
        iterator operator++(int) noexcept {
            auto previous = *this;
            ++*this;
            return previous;
        }
        friend bool operator==(const iterator &, const iterator &) = default;

    private:
        friend class array_view;
        iterator(const immutable_document *owner, uint32_t index) noexcept
            : m_owner(owner), m_index(index) {}
        const immutable_document *m_owner = nullptr;
        uint32_t m_index = 0;
    };

    uint32_t size() const noexcept;
    iterator begin() const noexcept;
    iterator end() const noexcept;
    /// @brief Locate a direct child in linear time in index.
    result<value_view> at(uint32_t index) const noexcept;

private:
    friend class value_view;
    array_view(const immutable_document *owner, uint32_t index) noexcept
        : m_owner(owner), m_index(index) {}
    const immutable_document *m_owner;
    uint32_t m_index;
};

/// @brief Sorted, unique decoded keys borrowing an immutable document.
class object_view {
public:
    class iterator {
    public:
        using value_type = object_member_view;
        using difference_type = std::ptrdiff_t;
        using iterator_concept = std::forward_iterator_tag;
        using iterator_category = std::input_iterator_tag;
        iterator() noexcept = default;
        object_member_view operator*() const noexcept;
        iterator &operator++() noexcept {
            ++m_index;
            return *this;
        }
        iterator operator++(int) noexcept {
            auto previous = *this;
            ++*this;
            return previous;
        }
        friend bool operator==(const iterator &, const iterator &) = default;

    private:
        friend class object_view;
        iterator(const immutable_document *owner, uint32_t index) noexcept
            : m_owner(owner), m_index(index) {}
        const immutable_document *m_owner = nullptr;
        uint32_t m_index = 0;
    };

    uint32_t size() const noexcept;
    iterator begin() const noexcept;
    iterator end() const noexcept;
    result<value_view> find(std::string_view key) const noexcept;

private:
    friend class value_view;
    object_view(const immutable_document *owner, uint32_t index) noexcept
        : m_owner(owner), m_index(index) {}
    const immutable_document *m_owner;
    uint32_t m_index;
};

/// @brief A non-owning immutable value. Owner destruction, replacement or move invalidates it.
class value_view {
public:
    value_t type() const noexcept;
    result<bool> get_boolean() const noexcept;
    result<uint64_t> get_unsigned() const noexcept;
    result<int64_t> get_signed() const noexcept;
    result<double> get_float() const noexcept;
    result<std::string_view> get_string() const noexcept;
    result<array_view> get_array() const noexcept;
    result<object_view> get_object() const noexcept;

private:
    friend class immutable_document;
    friend class array_view;
    friend class array_view::iterator;
    friend class object_view;
    friend class object_view::iterator;
    value_view(const immutable_document *owner, uint32_t index) noexcept
        : m_owner(owner), m_index(index) {}
    const detail::immutable_node &node() const noexcept;
    const immutable_document *m_owner;
    uint32_t m_index;
};

struct object_member_view {
    [[maybe_unused]] std::string_view key;
    value_view value;
};

/// @brief An input-independent, move-only JSON snapshot owning one packed allocation.
class immutable_document {
public:
    immutable_document(const immutable_document &) = delete;
    immutable_document &operator=(const immutable_document &) = delete;
    immutable_document(immutable_document &&) noexcept = default;
    immutable_document &operator=(immutable_document &&) noexcept = default;
    ~immutable_document() = default;

    /// @brief Fully validate and build an owning document. Allocation failures propagate.
    static expected<immutable_document, parse_error> parse(ondemand_reader &reader);
    /// @pre This owner holds a parsed document. Returned views borrow this owner object.
    value_view root() const & noexcept { return value_view(this, 0); }
    value_view root() const && = delete;
    size_t storage_size() const noexcept {
        return m_storage
                   ? sizeof(detail::immutable_node) * size_t(m_node_count) +
                         sizeof(detail::immutable_member) * size_t(m_member_count) + m_byte_count
                   : 0;
    }

private:
    friend class detail::immutable_builder;
    friend class value_view;
    friend class array_view;
    friend class array_view::iterator;
    friend class object_view;
    friend class object_view::iterator;
    immutable_document(std::unique_ptr<std::byte[]> storage, uint32_t nodes, uint32_t members,
                       uint32_t bytes) noexcept
        : m_storage(std::move(storage)), m_node_count(nodes), m_member_count(members),
          m_byte_count(bytes) {}
    const detail::immutable_node &node_at(uint32_t index) const noexcept {
        return *std::launder(reinterpret_cast<const detail::immutable_node *>(
            m_storage.get() + sizeof(detail::immutable_node) * size_t(index)));
    }
    const detail::immutable_member &member_at(uint32_t index) const noexcept {
        return *std::launder(reinterpret_cast<const detail::immutable_member *>(
            m_storage.get() + sizeof(detail::immutable_node) * size_t(m_node_count) +
            sizeof(detail::immutable_member) * size_t(index)));
    }
    std::string_view string_at(uint32_t offset, uint32_t length) const noexcept {
        const auto *bytes = m_storage.get() +
                            sizeof(detail::immutable_node) * size_t(m_node_count) +
                            sizeof(detail::immutable_member) * size_t(m_member_count);
        return {reinterpret_cast<const char *>(bytes) + offset, length};
    }
    uint32_t next_node(uint32_t index) const noexcept {
        const auto &value = node_at(index);
        return value.kind == static_cast<uint32_t>(value_t::array) ||
                       value.kind == static_cast<uint32_t>(value_t::object)
                   ? static_cast<uint32_t>(value.payload >> 32)
                   : index + 1;
    }
    std::unique_ptr<std::byte[]> m_storage;
    uint32_t m_node_count;
    uint32_t m_member_count;
    uint32_t m_byte_count;
};

inline const detail::immutable_node &value_view::node() const noexcept {
    return m_owner->node_at(m_index);
}
inline value_t value_view::type() const noexcept { return static_cast<value_t>(node().kind); }
inline result<bool> value_view::get_boolean() const noexcept {
    if (type() != value_t::boolean)
        return unexpected(error_code::INCORRECT_TYPE);
    return node().payload != 0;
}
inline result<uint64_t> value_view::get_unsigned() const noexcept {
    if (type() != value_t::number_unsigned)
        return unexpected(error_code::INCORRECT_TYPE);
    return node().payload;
}
inline result<int64_t> value_view::get_signed() const noexcept {
    if (type() != value_t::number_signed)
        return unexpected(error_code::INCORRECT_TYPE);
    return std::bit_cast<int64_t>(node().payload);
}
inline result<double> value_view::get_float() const noexcept {
    if (type() != value_t::number_float)
        return unexpected(error_code::INCORRECT_TYPE);
    return std::bit_cast<double>(node().payload);
}
inline result<std::string_view> value_view::get_string() const noexcept {
    if (type() != value_t::string)
        return unexpected(error_code::INCORRECT_TYPE);
    return m_owner->string_at(static_cast<uint32_t>(node().payload), node().count);
}
inline result<array_view> value_view::get_array() const noexcept {
    if (type() != value_t::array)
        return unexpected(error_code::INCORRECT_TYPE);
    return array_view(m_owner, m_index);
}
inline result<object_view> value_view::get_object() const noexcept {
    if (type() != value_t::object)
        return unexpected(error_code::INCORRECT_TYPE);
    return object_view(m_owner, m_index);
}
inline uint32_t array_view::size() const noexcept { return m_owner->node_at(m_index).count; }
inline array_view::iterator array_view::begin() const noexcept { return {m_owner, m_index + 1}; }
inline array_view::iterator array_view::end() const noexcept {
    return {m_owner, static_cast<uint32_t>(m_owner->node_at(m_index).payload >> 32)};
}
inline value_view array_view::iterator::operator*() const noexcept { return {m_owner, m_index}; }
inline array_view::iterator &array_view::iterator::operator++() noexcept {
    m_index = m_owner->next_node(m_index);
    return *this;
}
inline uint32_t object_view::size() const noexcept { return m_owner->node_at(m_index).count; }
inline object_view::iterator object_view::begin() const noexcept {
    return {m_owner, static_cast<uint32_t>(m_owner->node_at(m_index).payload)};
}
inline object_view::iterator object_view::end() const noexcept {
    return {m_owner, static_cast<uint32_t>(m_owner->node_at(m_index).payload) + size()};
}
inline object_member_view object_view::iterator::operator*() const noexcept {
    const auto &member = m_owner->member_at(m_index);
    return {m_owner->string_at(member.key_offset, member.key_length),
            value_view(m_owner, member.value_index)};
}

} // namespace wjr::json

#endif // WJR_JSON_IMMUTABLE_DOCUMENT_HPP__
