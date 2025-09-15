#ifndef WJR_CONTAINER_DETAIL_VECTOR_SIZE_REFERENCE_HPP__
#define WJR_CONTAINER_DETAIL_VECTOR_SIZE_REFERENCE_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {
template <typename pointer, typename size_type>
class default_vector_size_reference {
public:
    default_vector_size_reference() = delete;
    default_vector_size_reference(const default_vector_size_reference &) = delete;
    default_vector_size_reference(default_vector_size_reference &&) = default;
    default_vector_size_reference &operator=(const default_vector_size_reference &) = delete;
    default_vector_size_reference &operator=(default_vector_size_reference &&) = default;
    ~default_vector_size_reference() = default;

    constexpr explicit default_vector_size_reference(pointer ptr, pointer &pos) noexcept
        : m_ptr(ptr), m_pos(pos) {}

    constexpr default_vector_size_reference &operator=(size_type size) noexcept {
        m_pos = m_ptr + size;
        return *this;
    }

    constexpr operator size_type() const noexcept { return static_cast<size_type>(m_pos - m_ptr); }

    constexpr default_vector_size_reference &operator++() noexcept {
        ++m_pos;
        return *this;
    }

    constexpr default_vector_size_reference &operator--() noexcept {
        --m_pos;
        return *this;
    }

    constexpr default_vector_size_reference &operator+=(size_type size) noexcept {
        m_pos += size;
        return *this;
    }

    constexpr default_vector_size_reference &operator-=(size_type size) noexcept {
        m_pos -= size;
        return *this;
    }

private:
    pointer m_ptr;
    pointer &m_pos;
};

/// @private
template <typename pointer, typename size_type>
struct __unref_wrapper_helper<default_vector_size_reference<pointer, size_type>> {
    using type = size_type &;
};
} // namespace wjr

#endif // WJR_CONTAINER_DETAIL_VECTOR_SIZE_REFERENCE_HPP__