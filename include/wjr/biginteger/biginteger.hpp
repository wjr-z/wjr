#ifndef WJR_BIGINTEGER_BIGINTEGER_HPP__
#define WJR_BIGINTEGER_BIGINTEGER_HPP__

#include <wjr/compressed_pair.hpp>
#include <wjr/math.hpp>
#include <wjr/vector.hpp>

namespace wjr {

class default_biginteger_size_reference {
public:
    default_biginteger_size_reference() = delete;
    WJR_CONSTEXPR20 explicit default_biginteger_size_reference(int32_t &size) noexcept
        : m_size(size) {}
    default_biginteger_size_reference(const default_biginteger_size_reference &) = delete;
    default_biginteger_size_reference &
    operator=(const default_biginteger_size_reference &) = delete;
    ~default_biginteger_size_reference() = default;

    WJR_CONSTEXPR20 default_biginteger_size_reference &operator=(uint32_t size) noexcept {
        m_size = (m_size < 0) ? -size : size;
        return *this;
    }

    WJR_CONSTEXPR20 operator uint32_t() const noexcept {
        return (m_size < 0) ? -m_size : m_size;
    }

    WJR_CONSTEXPR20 default_biginteger_size_reference &operator++() noexcept {
        ++m_size;
        return *this;
    }

    WJR_CONSTEXPR20 default_biginteger_size_reference &operator--() noexcept {
        --m_size;
        return *this;
    }

    WJR_CONSTEXPR20 default_biginteger_size_reference &
    operator+=(uint32_t size) noexcept {
        m_size += (m_size < 0) ? -size : size;
        return *this;
    }

    WJR_CONSTEXPR20 default_biginteger_size_reference &
    operator-=(uint32_t size) noexcept {
        m_size -= (m_size < 0) ? -size : size;
        return *this;
    }

private:
    int32_t &m_size;
};

/**
 * @struct biginteger_data
 * @brief The data structure for biginteger
 *
 */
template <typename Alloc>
class default_biginteger_vector_storage {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<uint64_t>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = uint64_t;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = uint32_t;
    using difference_type = int32_t;
    using allocator_type = Alloc;
    using is_reallocatable = std::true_type;

private:
    struct Data {
        pointer m_data = {};
        int32_t m_size = 0;
        uint32_t m_capacity = 0;
    };

    using data_type = Data;

public:
    default_biginteger_vector_storage() noexcept = default;

    template <typename _Alloc>
    WJR_CONSTEXPR20 default_biginteger_vector_storage(_Alloc &&al) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {}

    template <typename _Alloc>
    WJR_CONSTEXPR20 default_biginteger_vector_storage(_Alloc &&al, size_type size,
                                                      size_type capacity,
                                                      reallocate_t) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {
        uninitialized_construct(size, capacity);
    }

    default_biginteger_vector_storage(const default_biginteger_vector_storage &) = delete;
    default_biginteger_vector_storage &
    operator=(const default_biginteger_vector_storage &) = delete;

    ~default_biginteger_vector_storage() noexcept = default;

    WJR_CONSTEXPR20 _Alty &get_allocator() noexcept { return m_pair.first(); }
    WJR_CONSTEXPR20 const _Alty &get_allocator() const noexcept { return m_pair.first(); }

    WJR_CONSTEXPR20 void destroy() noexcept {
        const size_type __size = size();
        destroy_n_using_allocator(data(), __size, get_allocator());
    }

    WJR_CONSTEXPR20 void deallocate() noexcept {
        get_allocator().deallocate(data(), capacity());
    }

    WJR_CONSTEXPR20 void uninitialized_construct(size_type _size, size_type capacity) {
        auto &al = get_allocator();
        auto &m_storage = __get_data();
        m_storage.m_data = al.allocate(capacity);
        size() = _size;
        m_storage.m_capacity = capacity;
    }

    WJR_CONSTEXPR20 void
    take_storage(default_biginteger_vector_storage &&other) noexcept {
        auto &other_storage = other.__get_data();
        __get_data() = std::move(other_storage);
        other_storage = {};
    }

    WJR_CONSTEXPR20 void swap_storage(default_biginteger_vector_storage &other) noexcept {
        std::swap(__get_data(), other.__get_data());
    }

    WJR_CONSTEXPR20 default_biginteger_size_reference size() noexcept {
        return default_biginteger_size_reference(__get_data().m_size);
    }
    WJR_CONSTEXPR20 size_type size() const noexcept {
        int32_t __size = get_xsize();
        return (__size < 0) ? -__size : __size;
    }
    WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return __get_data().m_capacity;
    }

    WJR_CONSTEXPR20 pointer data() noexcept { return __get_data().m_data; }
    WJR_CONSTEXPR20 const_pointer data() const noexcept { return __get_data().m_data; }

    // extension

    WJR_CONSTEXPR20 int32_t get_xsize() const noexcept { return __get_data().m_size; }
    WJR_CONSTEXPR20 void set_xsize(int32_t size) noexcept { __get_data().m_size = size; }

private:
    WJR_CONSTEXPR20 data_type &__get_data() noexcept { return m_pair.second(); }
    WJR_CONSTEXPR20 const data_type &__get_data() const noexcept {
        return m_pair.second();
    }

    compressed_pair<_Alty, data_type> m_pair;
};

template <>
struct unref_wrapper<default_biginteger_size_reference> {
    using type = int32_t &;
};

template <typename Storage>
class basic_biginteger {
    using vector_type = basic_vector<Storage>;

public:
    using value_type = typename Storage::value_type;

    basic_biginteger() = default;
    basic_biginteger(const basic_biginteger &other);
    basic_biginteger(basic_biginteger &&other);
    basic_biginteger &operator=(const basic_biginteger &other);
    basic_biginteger &operator=(basic_biginteger &&other);
    ~basic_biginteger();

private:
    vector_type m_vec;
};

using biginteger =
    basic_biginteger<default_biginteger_vector_storage<std::allocator<uint64_t>>>;

} // namespace wjr

#endif