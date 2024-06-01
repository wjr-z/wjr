#ifndef WJR_MEMORY_SAFE_POINTER_HPP__
#define WJR_MEMORY_SAFE_POINTER_HPP__

#include <wjr/span.hpp>

namespace wjr {

#if WJR_DEBUG_LEVEL > 2
#define WJR_HAS_DEBUG_SAFE_POINTER WJR_HAS_DEF
#endif

#if WJR_HAS_DEBUG(SAFE_POINTER)

template <typename T, typename Tag = void>
class safe_array {
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using pointer = T *;
    using reference = T &;
    using difference_type = ptrdiff_t;
    using size_type = size_t;

    constexpr safe_array() noexcept = default;
    constexpr safe_array(const safe_array &) noexcept = default;
    constexpr safe_array(safe_array &&) noexcept = default;
    constexpr safe_array &operator=(const safe_array &) noexcept = default;
    constexpr safe_array &operator=(safe_array &&) noexcept = default;
    ~safe_array() noexcept = default;

    constexpr safe_array &reset_range() noexcept {
        m_length -= m_offset;
        m_offset = 0;
        return *this;
    }

    constexpr pointer get() const noexcept { return m_ptr; }
    constexpr pointer data() const noexcept { return m_ptr; }
    constexpr reference operator[](size_type i) const noexcept { return m_ptr[i]; }

    constexpr safe_array(span<T> s) noexcept
        : m_ptr(s.data()), m_offset(0), m_size(s.size()), m_length(s.size()) {}

    constexpr safe_array first(size_type count) const noexcept {
        WJR_ASSERT_L3(m_ptr != nullptr, "safe_array: nullptr");
        WJR_ASSERT_L3(count <= m_size, "safe_array: out of range");
        return {m_ptr, m_offset, count, m_length};
    }

    constexpr safe_array subspan(size_type offset, size_type count) const noexcept {
        WJR_ASSERT_L3(m_ptr != nullptr, "safe_array: nullptr");
        WJR_ASSERT_L3(offset + count <= m_size, "safe_array: out of range");
        return {m_ptr + offset, m_offset + offset, count, m_length};
    }

    constexpr safe_array &append(size_type length) noexcept {
        WJR_ASSERT_L3(m_offset + m_size + length <= m_length);
        m_size += length;
        return *this;
    }

    constexpr safe_array &operator=(span<T> s) noexcept {
        m_ptr = s.data();
        m_offset = 0;
        m_size = s.size();
        m_length = s.size();
        return *this;
    }

    constexpr safe_array &operator=(std::nullptr_t) noexcept {
        m_ptr = nullptr;
        m_offset = 0;
        m_size = 0;
        m_length = 0;
        return *this;
    }

    constexpr safe_array &operator+=(size_type n) noexcept {
        WJR_ASSERT_L3(m_ptr != nullptr, "safe_array: nullptr");
        WJR_ASSERT_L3(n <= m_size, "safe_array: out of range");
        m_ptr += n;
        m_offset += n;
        m_size -= n;
        return *this;
    }

    constexpr friend safe_array operator+(safe_array lhs, size_type rhs) noexcept {
        return lhs += rhs;
    }

    constexpr friend safe_array operator+(size_type lhs, safe_array rhs) noexcept {
        return rhs += lhs;
    }

    constexpr safe_array &operator++() noexcept {
        (*this) += 1;
        return *this;
    }

    constexpr safe_array operator++(int) noexcept {
        safe_array tmp = *this;
        ++*this;
        return tmp;
    }

    constexpr safe_array &operator-=(size_type n) noexcept {
        WJR_ASSERT_L3(m_ptr != nullptr, "safe_array: nullptr");
        WJR_ASSERT_L3(m_offset >= n, "safe_array: out of range");
        m_ptr -= n;
        m_offset -= n;
        m_size += n;
        return *this;
    }

    constexpr friend safe_array operator-(safe_array lhs, size_type rhs) noexcept {
        return lhs -= rhs;
    }

    constexpr friend ptrdiff_t operator-(const safe_array &lhs,
                                         const safe_array &rhs) noexcept {
        return lhs.m_ptr - rhs.m_ptr;
    }

    constexpr safe_array &operator--() noexcept {
        (*this) -= 1;
        return *this;
    }

    constexpr safe_array operator--(int) noexcept {
        safe_array tmp = *this;
        --*this;
        return tmp;
    }

private:
    constexpr safe_array(pointer ptr, size_type offset, size_type size,
                         size_type length) noexcept
        : m_ptr(ptr), m_offset(offset), m_size(size), m_length(length) {}

    pointer m_ptr = nullptr;
    size_type m_offset;
    size_type m_size;
    size_type m_length;
};

#else

template <typename T, typename Tag = void>
class safe_array {
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using pointer = T *;
    using reference = T &;
    using difference_type = ptrdiff_t;
    using size_type = size_t;

    constexpr safe_array() noexcept = default;
    constexpr safe_array(const safe_array &) noexcept = default;
    constexpr safe_array(safe_array &&) noexcept = default;
    constexpr safe_array &operator=(const safe_array &) noexcept = default;
    constexpr safe_array &operator=(safe_array &&) noexcept = default;
    ~safe_array() noexcept = default;

    constexpr safe_array &reset_range() noexcept { return *this; }

    constexpr pointer get() const noexcept { return m_ptr; }
    constexpr pointer data() const noexcept { return m_ptr; }
    constexpr reference operator[](size_type i) const noexcept { return m_ptr[i]; }

    constexpr safe_array(span<T> s) noexcept : m_ptr(s.data()) {}

    constexpr safe_array first(WJR_MAYBE_UNUSED size_type count) const noexcept {
        return safe_array{m_ptr};
    }

    constexpr safe_array subspan(size_type offset,
                                 WJR_MAYBE_UNUSED size_type count) const noexcept {
        return safe_array{m_ptr + offset};
    }

    constexpr safe_array &operator=(span<T> s) noexcept {
        m_ptr = s.data();
        return *this;
    }

    constexpr safe_array &operator=(std::nullptr_t) noexcept {
        m_ptr = nullptr;
        return *this;
    }

    constexpr safe_array &operator+=(size_type n) noexcept {
        m_ptr += n;
        return *this;
    }

    constexpr friend safe_array operator+(safe_array lhs, size_type rhs) noexcept {
        return lhs += rhs;
    }

    constexpr friend safe_array operator+(size_type lhs, safe_array rhs) noexcept {
        return rhs += lhs;
    }

    constexpr safe_array &operator-=(size_type n) noexcept {
        m_ptr -= n;
        return *this;
    }

    constexpr friend safe_array operator-(safe_array lhs, size_type rhs) noexcept {
        return lhs -= rhs;
    }

    constexpr friend ptrdiff_t operator-(const safe_array &lhs,
                                         const safe_array &rhs) noexcept {
        return lhs.m_ptr - rhs.m_ptr;
    }

private:
    T *m_ptr;
};

#endif

} // namespace wjr

#endif // WJR_MEMORY_SAFE_POINTER_HPP__