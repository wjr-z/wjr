#ifndef WJR_MEMORY_SAFE_POINTER_HPP__
#define WJR_MEMORY_SAFE_POINTER_HPP__

#include <wjr/span.hpp>

namespace wjr {

#if WJR_DEBUG_LEVEL > 2
#define WJR_HAS_DEBUG_SAFE_POINTER WJR_HAS_DEF
#endif

#if WJR_HAS_DEBUG(SAFE_POINTER)

template <typename T, typename Tag = void>
class safe_pointer {
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using pointer = T *;
    using reference = T &;
    using difference_type = ptrdiff_t;
    using size_type = size_t;

    constexpr safe_pointer() noexcept = default;
    constexpr safe_pointer(const safe_pointer &) noexcept = default;
    constexpr safe_pointer(safe_pointer &&) noexcept = default;
    constexpr safe_pointer &operator=(const safe_pointer &) noexcept = default;
    constexpr safe_pointer &operator=(safe_pointer &&) noexcept = default;
    ~safe_pointer() noexcept = default;

    constexpr safe_pointer &reset_range() noexcept {
        m_offset = 0;
        return *this;
    }

    constexpr pointer get() const noexcept { return m_ptr; }
    constexpr pointer data() const noexcept { return m_ptr; }
    constexpr reference operator[](size_type i) const noexcept { return m_ptr[i]; }

    constexpr safe_pointer(span<T> s) noexcept
        : m_ptr(s.data()), m_offset(0), m_size(s.size()) {}

    constexpr safe_pointer first(size_type count) const noexcept {
        WJR_ASSERT_L1(m_ptr != nullptr, "safe_pointer: nullptr");
        WJR_ASSERT_L1(count <= m_size, "safe_pointer: out of range");
        return {m_ptr, m_offset, count};
    }

    constexpr safe_pointer subspan(size_type offset, size_type count) const noexcept {
        WJR_ASSERT_L1(m_ptr != nullptr, "safe_pointer: nullptr");
        WJR_ASSERT_L1(offset + count <= m_size, "safe_pointer: out of range");
        return {m_ptr + offset, m_offset + offset, count};
    }

    constexpr safe_pointer &operator=(span<T> s) noexcept {
        m_ptr = s.data();
        m_offset = 0;
        m_size = s.size();
        return *this;
    }

    constexpr safe_pointer &operator=(std::nullptr_t) noexcept {
        m_ptr = nullptr;
        m_offset = 0;
        m_size = 0;
        return *this;
    }

    constexpr safe_pointer &operator+=(size_type n) noexcept {
        WJR_ASSERT_L1(m_ptr != nullptr, "safe_pointer: nullptr");
        WJR_ASSERT_L1(n <= m_size, "safe_pointer: out of range");
        m_ptr += n;
        m_offset += n;
        m_size -= n;
        return *this;
    }

    constexpr friend safe_pointer operator+(safe_pointer lhs, size_type rhs) noexcept {
        return lhs += rhs;
    }

    constexpr friend safe_pointer operator+(size_type lhs, safe_pointer rhs) noexcept {
        return rhs += lhs;
    }

    constexpr safe_pointer &operator-=(size_type n) noexcept {
        WJR_ASSERT_L1(m_ptr != nullptr, "safe_pointer: nullptr");
        WJR_ASSERT_L1(m_offset >= n, "safe_pointer: out of range");
        m_ptr -= n;
        m_offset -= n;
        m_size += n;
        return *this;
    }

    constexpr friend safe_pointer operator-(safe_pointer lhs, size_type rhs) noexcept {
        return lhs -= rhs;
    }

    constexpr friend ptrdiff_t operator-(const safe_pointer &lhs,
                                         const safe_pointer &rhs) noexcept {
        return lhs.m_ptr - rhs.m_ptr;
    }

private:
    constexpr safe_pointer(pointer ptr, size_type offset, size_type size) noexcept
        : m_ptr(ptr), m_offset(offset), m_size(size) {}

    pointer m_ptr = nullptr;
    size_type m_offset;
    size_type m_size;
};

#else

template <typename T, typename Tag = void>
class safe_pointer {
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using pointer = T *;
    using reference = T &;
    using difference_type = ptrdiff_t;
    using size_type = size_t;

    constexpr safe_pointer() noexcept = default;
    constexpr safe_pointer(const safe_pointer &) noexcept = default;
    constexpr safe_pointer(safe_pointer &&) noexcept = default;
    constexpr safe_pointer &operator=(const safe_pointer &) noexcept = default;
    constexpr safe_pointer &operator=(safe_pointer &&) noexcept = default;
    ~safe_pointer() noexcept = default;

    constexpr safe_pointer &reset_range() noexcept { return *this; }

    constexpr pointer get() const noexcept { return m_ptr; }
    constexpr pointer data() const noexcept { return m_ptr; }
    constexpr reference operator[](size_type i) const noexcept { return m_ptr[i]; }

    constexpr safe_pointer(span<T> s) noexcept : m_ptr(s.data()) {}

    constexpr safe_pointer first(WJR_MAYBE_UNUSED size_type count) const noexcept {
        return safe_pointer{m_ptr};
    }

    constexpr safe_pointer subspan(size_type offset,
                                   WJR_MAYBE_UNUSED size_type count) const noexcept {
        return safe_pointer{m_ptr + offset};
    }

    constexpr safe_pointer &operator=(span<T> s) noexcept {
        m_ptr = s.data();
        return *this;
    }

    constexpr safe_pointer &operator=(std::nullptr_t) noexcept {
        m_ptr = nullptr;
        return *this;
    }

    constexpr safe_pointer &operator+=(size_type n) noexcept {
        m_ptr += n;
        return *this;
    }

    constexpr friend safe_pointer operator+(safe_pointer lhs, size_type rhs) noexcept {
        return lhs += rhs;
    }

    constexpr friend safe_pointer operator+(size_type lhs, safe_pointer rhs) noexcept {
        return rhs += lhs;
    }

    constexpr safe_pointer &operator-=(size_type n) noexcept {
        m_ptr -= n;
        return *this;
    }

    constexpr friend safe_pointer operator-(safe_pointer lhs, size_type rhs) noexcept {
        return lhs -= rhs;
    }

    constexpr friend ptrdiff_t operator-(const safe_pointer &lhs,
                                         const safe_pointer &rhs) noexcept {
        return lhs.m_ptr - rhs.m_ptr;
    }

private:
    T *m_ptr;
};

#endif

} // namespace wjr

#endif // WJR_MEMORY_SAFE_POINTER_HPP__