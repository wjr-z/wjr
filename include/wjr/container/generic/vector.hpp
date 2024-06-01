#ifndef WJR_CONTAINER_GENERIC_CONTAINER_VECTOR_HPP__
#define WJR_CONTAINER_GENERIC_CONTAINER_VECTOR_HPP__

/**
 * @file vector.hpp
 * @brief Vector container with definable internal structure
 *
 * @details
 * Customized internal structure needs to follow the following function signature: \n
 * -# storage() noexcept
 * -# ~storage() noexcept
 * -# void destroy(_Alty& al) noexcept(optional)
 * -# void destroy_and_deallocate(_Alty& al) noexcept(optional)
 * -# void uninitialized_construct(storage_type &other, size_type size, size_type
 * capacity, _Alty& al) noexcept
 * -# void take_storage(storage& other, _Alty& al) noexcept(optional)
 * -# void swap_storage(storage& other, _Alty& al) noexcept(optional)
 * -# decltype(auto) size() noexcept
 * -# size_type capacity() const noexcept
 * -# pointer data() noexcept
 * -# const_pointer data() const noexcept
 *
 * 1 : should not allocate memory. \n
 * 2 : don't need to destroy or deallocate. \n
 * 3 : destroy all elements. don't change ptr, size and capacity. \n
 * 4 : destroy and deallocate. \n
 * 5 : uninitialized construct the storage. allocate memory and set the size and
 * capacity. \n
 * 6 : take the storage from other. set other to empty. \n
 * 7 : swap the storage with other. \n
 * 8 : get the size. the return type must be reference,
 * such as size_type&, std::reference_wrapper<size_type> and so on. \n
 * 9 : get the capacity. \n
 * 10-11 : get the pointer. \n
 *
 * the size type of 8 need to implement the following function signature: \n
 * -# auto& operator=(size_type) noexcept
 * -# operator size_type() const noexcept
 * -# size_type operator++() noexcept
 * -# size_type operator--() noexcept
 * -# size_type operator+=(size_type) noexcept
 * -# size_type operator-=(size_type) noexcept
 *
 * @version 0.2
 * @date 2024-04-29
 *
 */

#include <wjr/assert.hpp>
#include <wjr/compressed_pair.hpp>
#include <wjr/container/generic/container_fn.hpp>
#include <wjr/iterator/contiguous_iterator_adpater.hpp>
#include <wjr/math/details.hpp>
#include <wjr/memory/copy.hpp>
#include <wjr/memory/memory_pool.hpp>
#include <wjr/memory/temporary_value_allocator.hpp>

namespace wjr {

template <typename T, typename Alloc>
class vector_storage_traits {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = T;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using allocator_type = Alloc;
    using is_trivially_contiguous = std::true_type;

    template <typename... Args>
    WJR_CONSTEXPR20 static void
    uninitialized_construct_using_allocator(pointer ptr, _Alty &al, Args &&...args) {
        wjr::uninitialized_construct_using_allocator(ptr, al,
                                                     std::forward<Args>(args)...);
    }

    WJR_CONSTEXPR20 static void fill(pointer first, pointer last, const value_type &val) {
        std::fill(first, last, val);
    }

    WJR_CONSTEXPR20 static pointer fill_n(pointer first, size_type n,
                                          const value_type &val) {
        return std::fill_n(first, n, val);
    }

    template <typename Val>
    WJR_CONSTEXPR20 static void
    uninitialized_fill_n_using_allocator(pointer first, size_type n, _Alty &al,
                                         const Val &val) {
        wjr::uninitialized_fill_n_using_allocator(first, n, al, val);
    }

    template <typename InputIt>
    WJR_CONSTEXPR20 static pointer copy(InputIt first, InputIt last, pointer result) {
        return std::copy(first, last, result);
    }

    template <typename InputIt, typename Size>
    WJR_CONSTEXPR20 static void copy_n(InputIt first, Size size, pointer result) {
        std::copy_n(first, size, result);
    }

    template <typename InputIt>
    WJR_CONSTEXPR20 static void
    uninitialized_copy_using_allocator(InputIt first, InputIt last, pointer result,
                                       _Alty &al) {
        wjr::uninitialized_copy_using_allocator(first, last, result, al);
    }

    template <typename InputIt, typename Size>
    WJR_CONSTEXPR20 static void
    uninitialized_copy_n_using_allocator(InputIt first, Size size, pointer result,
                                         _Alty &al) {
        wjr::uninitialized_copy_n_using_allocator(first, size, result, al);
    }

    template <typename InputIt>
    WJR_CONSTEXPR20 static void move(InputIt first, InputIt last, pointer result) {
        std::move(first, last, result);
    }

    template <typename InputIt>
    WJR_CONSTEXPR20 static void
    uninitialized_move_using_allocator(InputIt first, InputIt last, pointer result,
                                       _Alty &al) {
        wjr::uninitialized_move_using_allocator(first, last, result, al);
    }

    template <typename InputIt, typename Size>
    WJR_CONSTEXPR20 static void
    uninitialized_move_n_using_allocator(InputIt first, Size size, pointer result,
                                         _Alty &al) {
        wjr::uninitialized_move_n_using_allocator(first, size, result, al);
    }

    template <typename InputIt>
    WJR_CONSTEXPR20 static void move_backward(InputIt first, InputIt last,
                                              pointer result) {
        std::move_backward(first, last, result);
    }

    template <typename InputIt>
    WJR_CONSTEXPR20 static pointer copy_restrict(InputIt first, InputIt last,
                                                 pointer result) {
        return wjr::copy_restrict(first, last, result);
    }

    template <typename InputIt, typename Size>
    WJR_CONSTEXPR20 static pointer copy_n_restrict(InputIt first, Size n,
                                                   pointer result) {
        return wjr::copy_n_restrict(first, n, result);
    }

    template <typename InputIt>
    WJR_CONSTEXPR20 static void
    uninitialized_copy_restrict_using_allocator(InputIt first, InputIt last,
                                                pointer result, _Alty &al) {
        wjr::uninitialized_copy_restrict_using_allocator(first, last, result, al);
    }

    template <typename InputIt, typename Size>
    WJR_CONSTEXPR20 static void
    uninitialized_copy_n_restrict_using_allocator(InputIt first, Size size,
                                                  pointer result, _Alty &al) {
        wjr::uninitialized_copy_n_restrict_using_allocator(first, size, result, al);
    }

    template <typename InputIt>
    WJR_CONSTEXPR20 static void
    uninitialized_move_restrict_using_allocator(InputIt first, InputIt last,
                                                pointer result, _Alty &al) {
        wjr::uninitialized_move_restrict_using_allocator(first, last, result, al);
    }

    template <typename InputIt, typename Size>
    WJR_CONSTEXPR20 static void
    uninitialized_move_n_restrict_using_allocator(InputIt first, Size size,
                                                  pointer result, _Alty &al) {
        wjr::uninitialized_move_n_restrict_using_allocator(first, size, result, al);
    }
};

template <typename pointer, typename size_type>
class default_vector_size_reference {
public:
    default_vector_size_reference() = delete;
    default_vector_size_reference(const default_vector_size_reference &) = delete;
    default_vector_size_reference(default_vector_size_reference &&) = default;
    default_vector_size_reference &
    operator=(const default_vector_size_reference &) = delete;
    default_vector_size_reference &operator=(default_vector_size_reference &&) = default;

    explicit default_vector_size_reference(pointer ptr, pointer &pos) noexcept
        : m_ptr(ptr), m_pos(pos) {}
    ~default_vector_size_reference() = default;

    default_vector_size_reference &operator=(size_type size) noexcept {
        m_pos = m_ptr + size;
        return *this;
    }

    WJR_PURE operator size_type() const noexcept {
        return static_cast<size_type>(m_pos - m_ptr);
    }

    default_vector_size_reference &operator++() noexcept {
        ++m_pos;
        return *this;
    }

    default_vector_size_reference &operator--() noexcept {
        --m_pos;
        return *this;
    }

    default_vector_size_reference &operator+=(uint32_t size) noexcept {
        m_pos += size;
        return *this;
    }

    default_vector_size_reference &operator-=(uint32_t size) noexcept {
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
    using type = uint32_t &;
};

/**
 * @brief Default vector storage
 *
 * @details Use one pointer ans two size_type currently.
 *
 */
template <typename T, typename Alloc, typename STraits>
class __default_vector_storage_impl {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using storage_traits_type = STraits;
    using value_type = typename storage_traits_type::value_type;
    using pointer = typename storage_traits_type::pointer;
    using const_pointer = typename storage_traits_type::const_pointer;
    using size_type = typename storage_traits_type::size_type;
    using difference_type = typename storage_traits_type::difference_type;
    using allocator_type = typename storage_traits_type::allocator_type;
    using is_reallocatable = std::true_type;

private:
    struct Data {
        pointer m_data = nullptr;
        pointer m_end = nullptr;
        pointer m_buffer = nullptr;
    };

    using data_type = Data;
    using SizeRef = default_vector_size_reference<pointer, size_type>;

public:
    __default_vector_storage_impl() noexcept = default;

    __default_vector_storage_impl(const __default_vector_storage_impl &) = delete;
    __default_vector_storage_impl(__default_vector_storage_impl &&) = delete;
    __default_vector_storage_impl &
    operator=(const __default_vector_storage_impl &) = delete;
    __default_vector_storage_impl &operator=(__default_vector_storage_impl &&) = delete;

    ~__default_vector_storage_impl() noexcept = default;

    WJR_CONSTEXPR20 void
    destroy(_Alty &al) noexcept(std::is_nothrow_destructible_v<value_type>) {
        if (WJR_BUILTIN_CONSTANT_P(data() == nullptr) && data() == nullptr) {
            return;
        }

        if (WJR_BUILTIN_CONSTANT_P(size() == 0) && size() == 0) {
            return;
        }

        destroy_using_allocator(m_storage.m_data, m_storage.m_end, al);
    }

    WJR_CONSTEXPR20 void destroy_and_deallocate(_Alty &al) noexcept(
        std::is_nothrow_destructible_v<value_type>) {
        if (WJR_BUILTIN_CONSTANT_P(data() == nullptr) && data() == nullptr) {
            return;
        }

        if (WJR_BUILTIN_CONSTANT_P(capacity() == 0) && capacity() == 0) {
            return;
        }

        if (m_storage.m_data != nullptr) {
            destroy_using_allocator(m_storage.m_data, m_storage.m_end, al);
            al.deallocate(m_storage.m_data, capacity());
        }
    }

    WJR_CONSTEXPR20 static void
    uninitialized_construct(__default_vector_storage_impl &other, size_type size,
                            size_type capacity, _Alty &al) {
        const auto result = allocate_at_least(al, capacity);

        other.m_storage = {
            result.ptr,
            result.ptr + size,
            result.ptr + capacity,
        };
    }

    WJR_CONSTEXPR20 void take_storage(__default_vector_storage_impl &other,
                                      _Alty &) noexcept {
        auto &other_storage = other.m_storage;
        m_storage = std::move(other_storage);
        other_storage = {};
    }

    WJR_CONSTEXPR20 void swap_storage(__default_vector_storage_impl &other,
                                      _Alty &) noexcept {
        std::swap(m_storage, other.m_storage);
    }

    WJR_PURE WJR_CONSTEXPR20 SizeRef size() noexcept {
        return SizeRef(m_storage.m_data, m_storage.m_end);
    }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return m_storage.m_end - m_storage.m_data;
    }

    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return m_storage.m_buffer - m_storage.m_data;
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return m_storage.m_data; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return m_storage.m_data;
    }

private:
    data_type m_storage;
};

template <typename T, typename Alloc>
using default_vector_storage =
    __default_vector_storage_impl<T, Alloc, vector_storage_traits<T, Alloc>>;

template <typename T, size_t Capacity, typename Alloc, typename STraits>
class __static_vector_storage_impl {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using storage_traits_type = STraits;
    using value_type = typename storage_traits_type::value_type;
    using pointer = typename storage_traits_type::pointer;
    using const_pointer = typename storage_traits_type::const_pointer;
    using size_type = typename storage_traits_type::size_type;
    using difference_type = typename storage_traits_type::difference_type;
    using allocator_type = typename storage_traits_type::allocator_type;
    using is_reallocatable = std::false_type;

private:
    static constexpr auto max_alignment =
        std::max<size_type>(alignof(T), alignof(size_type));
    static constexpr bool __use_memcpy = is_trivially_allocator_constructible_v<Alloc> &&
                                         std::is_trivially_copyable_v<T> &&
                                         Capacity * sizeof(T) <= 64;

    struct Data {
        size_type m_size = 0;
        alignas(max_alignment) char m_data[Capacity * sizeof(T)];
    };

    using data_type = Data;

public:
    __static_vector_storage_impl() noexcept = default;

    __static_vector_storage_impl(const __static_vector_storage_impl &) = delete;
    __static_vector_storage_impl(__static_vector_storage_impl &&) = delete;
    __static_vector_storage_impl &
    operator=(const __static_vector_storage_impl &) = delete;
    __static_vector_storage_impl &operator=(__static_vector_storage_impl &&) = delete;

    ~__static_vector_storage_impl() noexcept = default;

    WJR_CONSTEXPR20 void
    destroy(_Alty &al) noexcept(std::is_nothrow_destructible_v<value_type>) {
        if (WJR_BUILTIN_CONSTANT_P(size() == 0) && size() == 0) {
            return;
        }

        destroy_n_using_allocator(data(), size(), al);
    }

    WJR_CONSTEXPR20 void destroy_and_deallocate(_Alty &al) noexcept(
        std::is_nothrow_destructible_v<value_type>) {
        destroy(al);
    }

    WJR_CONSTEXPR20 static void
    uninitialized_construct(__static_vector_storage_impl &other, size_type size,
                            WJR_MAYBE_UNUSED size_type capacity, _Alty &) noexcept {
        WJR_ASSERT_ASSUME(capacity <= Capacity,
                          "capacity must be less than or equal to Capacity");
        other.m_storage.m_size = size;
    }

    WJR_CONSTEXPR20 void take_storage(__static_vector_storage_impl &other, _Alty &al) {
        auto &other_storage = other.m_storage;
        const auto lhs = data();
        const auto rhs = other.data();

        m_storage.m_size = other_storage.m_size;

        if constexpr (__use_memcpy) {
            if (other.size()) {
                __memcpy(lhs, rhs, Capacity);
            }
        } else {
            STraits::uninitialized_move_n_restrict_using_allocator(
                rhs, other_storage.m_size, lhs, al);
        }

        other_storage.m_size = 0;
    }

    WJR_CONSTEXPR20 void swap_storage(__static_vector_storage_impl &other, _Alty &al) {
        auto &other_storage = other.m_storage;
        auto lhs = data();
        auto lsize = size();
        auto rhs = other.data();
        auto rsize = other.size();

        if (lsize && rsize) {
            m_storage.m_size = rsize;
            other_storage.m_size = lsize;

            T tmp[Capacity];
            if constexpr (__use_memcpy) {
                __memcpy(tmp, lhs, Capacity);
                __memcpy(lhs, rhs, Capacity);
                __memcpy(rhs, tmp, Capacity);
            } else {
                if (lsize > rsize) {
                    std::swap(lhs, rhs);
                    std::swap(lsize, rsize);
                }

                STraits::uninitialized_move_n_restrict_using_allocator(lhs, lsize, tmp,
                                                                       al);
                STraits::uninitialized_move_n_restrict_using_allocator(rhs, rsize, lhs,
                                                                       al);
                STraits::uninitialized_move_n_restrict_using_allocator(tmp, lsize, rhs,
                                                                       al);
            }
            return;
        } else if (rsize) {
            if constexpr (__use_memcpy) {
                __memcpy(lhs, rhs, Capacity);
            } else {
                STraits::uninitialized_move_n_restrict_using_allocator(rhs, rsize, lhs,
                                                                       al);
            }
            m_storage.m_size = rsize;
            other_storage.m_size = 0;
            return;
        } else if (lsize) {
            if constexpr (__use_memcpy) {
                __memcpy(rhs, lhs, Capacity);
            } else {
                STraits::uninitialized_move_n_restrict_using_allocator(lhs, lsize, rhs,
                                                                       al);
            }
            other_storage.m_size = lsize;
            m_storage.m_size = 0;
            return;
        } else {
            return;
        }
    }

    WJR_PURE WJR_CONSTEXPR20 size_type &size() noexcept { return m_storage.m_size; }
    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept { return m_storage.m_size; }
    WJR_CONST constexpr size_type capacity() const noexcept { return Capacity; }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept {
        return reinterpret_cast<pointer>(m_storage.m_data);
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return reinterpret_cast<const_pointer>(m_storage.m_data);
    }

private:
    static void __memcpy(pointer dst, const_pointer src, size_type count) {
        std::memcpy(dst, src, count * sizeof(T));
    }

    data_type m_storage;
};

template <typename T, size_t Capacity, typename Alloc>
using static_vector_storage =
    __static_vector_storage_impl<T, Capacity, Alloc, vector_storage_traits<T, Alloc>>;

template <typename T, typename Alloc, typename STraits>
class __fixed_vector_storage_impl {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using storage_traits_type = STraits;
    using value_type = typename storage_traits_type::value_type;
    using pointer = typename storage_traits_type::pointer;
    using const_pointer = typename storage_traits_type::const_pointer;
    using size_type = typename storage_traits_type::size_type;
    using difference_type = typename storage_traits_type::difference_type;
    using allocator_type = typename storage_traits_type::allocator_type;
    using is_reallocatable = std::false_type;

private:
    struct Data {
        pointer m_data = nullptr;
        pointer m_end = nullptr;
        pointer m_buffer = nullptr;
    };

    using data_type = Data;
    using SizeRef = default_vector_size_reference<pointer, size_type>;

public:
    __fixed_vector_storage_impl() noexcept = default;

    __fixed_vector_storage_impl(const __fixed_vector_storage_impl &) = delete;
    __fixed_vector_storage_impl(__fixed_vector_storage_impl &&) = delete;
    __fixed_vector_storage_impl &operator=(const __fixed_vector_storage_impl &) = delete;
    __fixed_vector_storage_impl &operator=(__fixed_vector_storage_impl &&) = delete;

    ~__fixed_vector_storage_impl() noexcept = default;

private:
    WJR_PURE WJR_INTRINSIC_INLINE bool __is_null_data() const {
        return WJR_BUILTIN_CONSTANT_P(data() == nullptr) && data() == nullptr;
    }

    WJR_PURE WJR_INTRINSIC_INLINE bool __is_zero_size() const {
        return WJR_BUILTIN_CONSTANT_P(size() == 0) && size() == 0;
    }

    WJR_PURE WJR_INTRINSIC_INLINE bool __is_zero_capacity() const {
        return WJR_BUILTIN_CONSTANT_P(capacity() == 0) && capacity() == 0;
    }

public:
    WJR_CONSTEXPR20 void
    destroy(_Alty &al) noexcept(std::is_nothrow_destructible_v<value_type>) {
        if (__is_null_data() || __is_zero_size()) {
            return;
        }

        destroy_using_allocator(m_storage.m_data, m_storage.m_end, al);
    }

    WJR_CONSTEXPR20 void destroy_and_deallocate(_Alty &al) noexcept(
        std::is_nothrow_destructible_v<value_type>) {
        if (__is_null_data() || __is_zero_capacity()) {
            return;
        }

        if (m_storage.m_data != nullptr) {
            destroy_using_allocator(m_storage.m_data, m_storage.m_end, al);
            al.deallocate(m_storage.m_data, capacity());
        }
    }

    WJR_CONSTEXPR20 static void
    uninitialized_construct(__fixed_vector_storage_impl &other, size_type size,
                            size_type capacity, _Alty &al) {
        const auto result = allocate_at_least(al, capacity);

        other.m_storage = {
            result.ptr,
            result.ptr + size,
            result.ptr + capacity,
        };
    }

    WJR_CONSTEXPR20 void take_storage(__fixed_vector_storage_impl &other,
                                      _Alty &) noexcept {
        auto &other_storage = other.m_storage;
        m_storage = std::move(other_storage);
        other_storage = {};
    }

    WJR_CONSTEXPR20 void swap_storage(__fixed_vector_storage_impl &other,
                                      _Alty &) noexcept {
        std::swap(m_storage, other.m_storage);
    }

    WJR_PURE WJR_CONSTEXPR20 SizeRef size() noexcept {
        return SizeRef(m_storage.m_data, m_storage.m_end);
    }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return m_storage.m_end - m_storage.m_data;
    }

    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return m_storage.m_buffer - m_storage.m_data;
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return m_storage.m_data; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return m_storage.m_data;
    }

private:
    data_type m_storage;
};

template <typename T, typename Alloc>
using fixed_vector_storage =
    __fixed_vector_storage_impl<T, Alloc, vector_storage_traits<T, Alloc>>;

template <typename T, size_t Capacity, typename Alloc, typename STraits>
class __sso_vector_storage_impl {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using storage_traits_type = STraits;
    using value_type = typename storage_traits_type::value_type;
    using pointer = typename storage_traits_type::pointer;
    using const_pointer = typename storage_traits_type::const_pointer;
    using size_type = typename storage_traits_type::size_type;
    using difference_type = typename storage_traits_type::difference_type;
    using allocator_type = typename storage_traits_type::allocator_type;
    using is_reallocatable = std::true_type;

private:
    static constexpr auto max_alignment = std::max<size_type>(
        alignof(T), std::max<size_type>(alignof(pointer), alignof(size_type)));

    struct __Data_test {
        pointer m_data;
        size_type m_size;
        union {
            size_type m_capacity;
            alignas(max_alignment) T m_storage[Capacity];
        };
    };

    static constexpr auto __sizeof_data = sizeof(__Data_test);
    static constexpr auto __offsetof_storage =
        __align_up(__align_up(sizeof(pointer), alignof(pointer)) +
                       __align_up(sizeof(size_type), alignof(size_type)),
                   max_alignment);

    static constexpr auto __max_capacity =
        (__sizeof_data - __offsetof_storage) / sizeof(T);
    static constexpr bool __use_memcpy = is_trivially_allocator_constructible_v<Alloc> &&
                                         std::is_trivially_copyable_v<T> &&
                                         __max_capacity * sizeof(T) <= 64;

    struct Data {
        Data() : m_capacity() {}
        Data(const Data &) = delete;
        Data &operator=(const Data &) = delete;
        ~Data() {}

        pointer m_data = m_storage;
        size_type m_size = 0;
        union {
            size_type m_capacity;
            alignas(max_alignment) T m_storage[__max_capacity];
        };
    };

    using data_type = Data;

public:
    __sso_vector_storage_impl() noexcept = default;

    __sso_vector_storage_impl(const __sso_vector_storage_impl &) = delete;
    __sso_vector_storage_impl(__sso_vector_storage_impl &&) = delete;
    __sso_vector_storage_impl &operator=(const __sso_vector_storage_impl &) = delete;
    __sso_vector_storage_impl &operator=(__sso_vector_storage_impl &&) = delete;

    ~__sso_vector_storage_impl() noexcept = default;

    WJR_CONSTEXPR20 void
    destroy(_Alty &al) noexcept(std::is_nothrow_destructible_v<value_type>) {
        if (WJR_BUILTIN_CONSTANT_P(size() == 0) && size() == 0) {
            return;
        }

        destroy_n_using_allocator(data(), size(), al);
    }

    WJR_CONSTEXPR20 void destroy_and_deallocate(_Alty &al) noexcept(
        std::is_nothrow_destructible_v<value_type>) {

        destroy(al);
        if (!__is_sso()) {
            al.deallocate(data(), capacity());
            m_storage.m_data = m_storage.m_storage;
        }
    }

    WJR_CONSTEXPR20 static void uninitialized_construct(__sso_vector_storage_impl &other,
                                                        size_type size,
                                                        size_type capacity, _Alty &al) {
        auto &storage = other.m_storage;
        if (capacity <= __max_capacity) {
            storage.m_size = size;
        } else {
            const auto result = allocate_at_least(al, capacity);

            storage.m_data = result.ptr;
            storage.m_size = size;
            storage.m_capacity = result.count;
        }
    }

    WJR_CONSTEXPR20 void take_storage(__sso_vector_storage_impl &other, _Alty &al) {
        auto &other_storage = other.m_storage;

        WJR_ASSERT_ASSUME_L2(__is_sso());

        if (other.__is_sso()) {
            m_storage.m_size = other_storage.m_size;

            if constexpr (__use_memcpy) {
                if (other.size()) {
                    __memcpy(m_storage.m_storage, other_storage.m_storage,
                             __max_capacity);
                }
            } else {
                STraits::uninitialized_move_n_restrict_using_allocator(
                    other_storage.m_storage, other.size(), m_storage.m_storage, al);
            }
        } else {
            m_storage.m_data = other_storage.m_data;
            m_storage.m_size = other_storage.m_size;
            m_storage.m_capacity = other_storage.m_capacity;

            other_storage.m_data = other_storage.m_storage;
        }

        other_storage.m_size = 0;
        WJR_ASSUME(other.__is_sso());
    }

    WJR_CONSTEXPR20 void swap_storage(__sso_vector_storage_impl &other, _Alty &al) {
        auto &storage = m_storage;
        auto &other_storage = other.m_storage;

        if (__is_sso()) {
            if (other.__is_sso()) {
                auto lhs = storage.m_storage;
                auto lsize = size();
                auto rhs = other_storage.m_storage;
                auto rsize = other.size();

                if (lsize && rsize) {
                    T tmp[__max_capacity];
                    if constexpr (__use_memcpy) {
                        __memcpy(tmp, lhs, __max_capacity);
                        __memcpy(lhs, rhs, __max_capacity);
                        __memcpy(rhs, tmp, __max_capacity);
                    } else {
                        if (lsize > rsize) {
                            std::swap(lhs, rhs);
                            std::swap(lsize, rsize);
                        }

                        STraits::uninitialized_move_n_restrict_using_allocator(lhs, lsize,
                                                                               tmp, al);
                        STraits::uninitialized_move_n_restrict_using_allocator(rhs, rsize,
                                                                               lhs, al);
                        STraits::uninitialized_move_n_restrict_using_allocator(tmp, lsize,
                                                                               rhs, al);
                    }
                } else if (rsize) {
                    if constexpr (__use_memcpy) {
                        __memcpy(lhs, rhs, __max_capacity);
                    } else {
                        STraits::uninitialized_move_n_restrict_using_allocator(rhs, rsize,
                                                                               lhs, al);
                    }
                    storage.m_size = rsize;
                    other_storage.m_size = 0;
                    return;
                } else if (lsize) {
                    if constexpr (__use_memcpy) {
                        __memcpy(rhs, lhs, __max_capacity);
                    } else {
                        STraits::uninitialized_move_n_restrict_using_allocator(lhs, lsize,
                                                                               rhs, al);
                    }
                    other_storage.m_size = lsize;
                    storage.m_size = 0;
                    return;
                } else {
                    return;
                }
            } else {
                const size_type __tmp_capacity = other_storage.m_capacity;
                if constexpr (__use_memcpy) {
                    if (size()) {
                        __memcpy(other_storage.m_storage, storage.m_storage,
                                 __max_capacity);
                    }
                } else {
                    STraits::uninitialized_move_n_restrict_using_allocator(
                        storage.m_storage, size(), other_storage.m_storage, al);
                }

                storage.m_data = other_storage.m_data;
                other_storage.m_data = other_storage.m_storage;
                storage.m_capacity = __tmp_capacity;
            }
        } else {
            const size_type __tmp_capacity = storage.m_capacity;
            if (other.__is_sso()) {
                if constexpr (__use_memcpy) {
                    if (other.size()) {
                        __memcpy(storage.m_storage, other_storage.m_storage,
                                 __max_capacity);
                    }
                } else {
                    STraits::uninitialized_move_n_restrict_using_allocator(
                        other_storage.m_storage, other.size(), storage.m_storage, al);
                }

                other_storage.m_data = storage.m_data;
                storage.m_data = storage.m_storage;
            } else {
                const auto __tmp_data = storage.m_data;
                storage.m_data = other_storage.m_data;
                other_storage.m_data = __tmp_data;
                storage.m_capacity = other_storage.m_capacity;
            }
            other_storage.m_capacity = __tmp_capacity;
        }

        const size_type __tmp_size = size();
        storage.m_size = other.size();
        other_storage.m_size = __tmp_size;
    }

    WJR_PURE WJR_CONSTEXPR20 size_type &size() noexcept { return m_storage.m_size; }
    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept { return m_storage.m_size; }
    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        const size_type ret = __is_sso() ? __max_capacity : m_storage.m_capacity;
        WJR_ASSERT_ASSUME_L2(ret >= __max_capacity);
        return ret;
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return m_storage.m_data; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return m_storage.m_data;
    }

private:
    static void __memcpy(pointer dst, const_pointer src, size_type count) {
        std::memcpy(dst, src, count * sizeof(T));
    }

    WJR_PURE bool __is_sso() const noexcept {
        return m_storage.m_data == m_storage.m_storage;
    }

    data_type m_storage;
};

template <typename T, size_t Capacity, typename Alloc>
using sso_vector_storage =
    __sso_vector_storage_impl<T, Capacity, Alloc, vector_storage_traits<T, Alloc>>;

WJR_REGISTER_HAS_TYPE(vector_storage_shrink_to_fit,
                      std::declval<Storage>().shrink_to_fit(), Storage);

WJR_REGISTER_HAS_TYPE(vector_storage_empty, std::declval<Storage>().empty(), Storage);

WJR_REGISTER_HAS_TYPE(vector_storage_uninitialized_construct,
                      std::declval<Storage>().uninitialized_construct(
                          std::declval<Size>(), std::declval<Size>(),
                          std::declval<Alloc &>()),
                      Storage, Size, Alloc);

template <typename Storage>
struct basic_vector_traits {
    using value_type = typename Storage::value_type;
    using difference_type = typename Storage::difference_type;
    using pointer = typename Storage::pointer;
    using const_pointer = typename Storage::const_pointer;
    using reference = value_type &;
    using const_reference = const value_type &;
};

/**
 * @brief Customized vector by storage.
 *
 * @details Type of pointer is same as iterator.
 *
 */
template <typename Storage>
class basic_vector {
public:
    using value_type = typename Storage::value_type;
    using allocator_type = typename Storage::allocator_type;
    using storage_type = Storage;

private:
    using _Alty =
        typename std::allocator_traits<allocator_type>::template rebind_alloc<value_type>;
    using _Alty_traits = std::allocator_traits<_Alty>;

    using storage_fn_type = container_fn<_Alty>;
    using __get_size_t = decltype(std::declval<storage_type>().size());
    using IteratorTraits = basic_vector_traits<storage_type>;

    friend class container_fn<_Alty>;

public:
    static_assert(std::is_same_v<typename _Alty_traits::value_type, value_type>,
                  "allocator_type::value_type must be the same as value_type");

    using size_type = typename storage_type::size_type;
    using difference_type = typename storage_type::difference_type;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = typename Storage::pointer;
    using const_pointer = typename Storage::const_pointer;
    using iterator = contiguous_iterator_adapter<basic_vector, IteratorTraits>;
    using const_iterator =
        contiguous_const_iterator_adapter<basic_vector, IteratorTraits>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using storage_traits_type = typename storage_type::storage_traits_type;
    using is_trivially_contiguous = typename storage_traits_type::is_trivially_contiguous;
    using is_reallocatable = typename storage_type::is_reallocatable;

private:
    using STraits = storage_traits_type;

    static_assert(is_contiguous_iterator_v<iterator>, "");
    static_assert(is_contiguous_iterator_v<const_iterator>, "");

    static constexpr bool __storage_noexcept_destroy =
        noexcept(std::declval<storage_type>().destroy(std::declval<_Alty &>()));
    static constexpr bool __storage_noexcept_destroy_and_deallocate = noexcept(
        std::declval<storage_type>().destroy_and_deallocate(std::declval<_Alty &>()));
    static constexpr bool __storage_noexcept_take_storage =
        noexcept(std::declval<storage_type>().take_storage(std::declval<storage_type &>(),
                                                           std::declval<_Alty &>()));
    static constexpr bool __storage_noexcept_swap_storage =
        noexcept(std::declval<storage_type>().swap_storage(std::declval<storage_type &>(),
                                                           std::declval<_Alty &>()));

public:
    WJR_CONSTEXPR20
    basic_vector() noexcept(std::is_nothrow_default_constructible_v<_Alty>) = default;

    WJR_CONSTEXPR20 explicit basic_vector(const allocator_type &al) noexcept(
        std::is_nothrow_constructible_v<_Alty, const allocator_type &>)
        : m_pair(std::piecewise_construct, std::forward_as_tuple(al),
                 std::forward_as_tuple()) {}

    WJR_CONSTEXPR20 explicit basic_vector(const size_type n,
                                          const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, std::forward_as_tuple(al),
                 std::forward_as_tuple()) {
        __construct_n(n, vctor);
    }

    WJR_CONSTEXPR20 basic_vector(size_type n, const value_type &val,
                                 const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, std::forward_as_tuple(al),
                 std::forward_as_tuple()) {
        __construct_n(n, val);
    }

private:
    template <typename _Alloc>
    WJR_CONSTEXPR20 basic_vector(const basic_vector &other, _Alloc &&al, in_place_empty_t)
        : m_pair(std::piecewise_construct, std::forward_as_tuple(al),
                 std::forward_as_tuple()) {
        const auto size = other.size();
        uninitialized_construct(size, other.capacity());
        STraits::uninitialized_copy_n_restrict_using_allocator(other.data(), size, data(),
                                                               __get_allocator());
    }

    template <typename _Alloc>
    WJR_CONSTEXPR20
    basic_vector(basic_vector &&other, _Alloc &&al, in_place_empty_t) noexcept(
        std::is_nothrow_constructible_v<storage_type, _Alloc &&>
            &&__storage_noexcept_take_storage)
        : m_pair(std::piecewise_construct, std::forward_as_tuple(al),
                 std::forward_as_tuple()) {
        __take_storage(std::move(other));
    }

public:
    WJR_CONSTEXPR20 basic_vector(const basic_vector &other)
        : basic_vector(other,
                       _Alty_traits::select_on_container_copy_construction(
                           other.__get_allocator()),
                       in_place_empty) {}

    WJR_CONSTEXPR20 basic_vector(const basic_vector &other, const allocator_type &al)
        : basic_vector(other, al, in_place_empty) {}

    WJR_CONSTEXPR20 basic_vector(basic_vector &&other) noexcept(noexcept(basic_vector(
        std::move(other), std::move(other.__get_allocator()), in_place_empty)))
        : basic_vector(std::move(other), std::move(other.__get_allocator()),
                       in_place_empty) {}

    WJR_CONSTEXPR20 basic_vector(basic_vector &&other, const allocator_type &al) noexcept(
        noexcept(basic_vector(std::move(other), al, in_place_empty)))
        : basic_vector(std::move(other), al, in_place_empty) {}

    template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    WJR_CONSTEXPR20 basic_vector(Iter first, Iter last,
                                 const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, std::forward_as_tuple(al),
                 std::forward_as_tuple()) {
        __range_construct(try_to_address(first), try_to_address(last),
                          iterator_category_t<Iter>());
    }

    WJR_CONSTEXPR20 basic_vector(std::initializer_list<value_type> il,
                                 const allocator_type &al = allocator_type())
        : basic_vector(il.begin(), il.end(), al) {}

    WJR_CONSTEXPR20 ~basic_vector() noexcept(__storage_noexcept_destroy_and_deallocate) {
        __destroy_and_deallocate();
    }

    WJR_CONSTEXPR20 basic_vector &operator=(const basic_vector &other) noexcept(
        noexcept(storage_fn_type::copy_assign(*this, other))) {
        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_fn_type::copy_assign(*this, other);
        }

        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &operator=(basic_vector &&other) noexcept(
        noexcept(storage_fn_type::move_assign(*this, std::move(other)))) {
        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_fn_type::move_assign(*this, std::move(other));
        }

        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &operator=(std::initializer_list<value_type> il) {
        return assign(il);
    }

    WJR_CONSTEXPR20 basic_vector &assign(size_type n, const value_type &val) {
        __fill_assign(n, val);
        return *this;
    }

    template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    WJR_CONSTEXPR20 basic_vector &assign(Iter first, Iter last) {
        __range_assign(try_to_address(first), try_to_address(last),
                       iterator_category_t<Iter>());
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &assign(std::initializer_list<value_type> il) {
        return assign(il.begin(), il.end());
    }

    WJR_PURE WJR_CONSTEXPR20 pointer begin_unsafe() noexcept { return data(); }
    WJR_PURE WJR_CONSTEXPR20 const_pointer begin_unsafe() const noexcept {
        return data();
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer cbegin_unsafe() const noexcept {
        return data();
    }

    WJR_PURE WJR_CONSTEXPR20 pointer end_unsafe() noexcept { return data() + size(); }
    WJR_PURE WJR_CONSTEXPR20 const_pointer end_unsafe() const noexcept {
        return data() + size();
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer cend_unsafe() const noexcept {
        return end_unsafe();
    }

    WJR_PURE WJR_CONSTEXPR20 pointer buf_end_unsafe() noexcept {
        return data() + capacity();
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer buf_end_unsafe() const noexcept {
        return data() + capacity();
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer cbuf_end_unsafe() const noexcept {
        return buf_end_unsafe();
    }

private:
    WJR_PURE WJR_CONSTEXPR20 iterator __make_iterator(const_pointer ptr) const noexcept {
        return iterator(const_cast<pointer>(ptr), this);
    }

    WJR_PURE WJR_CONSTEXPR20 pointer __get_pointer(iterator ptr) const noexcept {
        ptr.check_same_container(this);
        return (to_address)(ptr);
    }

    WJR_PURE WJR_CONSTEXPR20 pointer __get_pointer(const_iterator ptr) const noexcept {
        ptr.check_same_container(this);
        return const_cast<pointer>((to_address)(ptr));
    }

public:
    WJR_PURE WJR_CONSTEXPR20 iterator begin() noexcept {
        return __make_iterator(begin_unsafe());
    }

    WJR_PURE WJR_CONSTEXPR20 const_iterator begin() const noexcept {
        return __make_iterator(begin_unsafe());
    }

    WJR_PURE WJR_CONSTEXPR20 const_iterator cbegin() const noexcept {
        return __make_iterator(cbegin_unsafe());
    }

    WJR_PURE WJR_CONSTEXPR20 iterator end() noexcept {
        return __make_iterator(end_unsafe());
    }

    WJR_PURE WJR_CONSTEXPR20 const_iterator end() const noexcept {
        return __make_iterator(end_unsafe());
    }

    WJR_PURE WJR_CONSTEXPR20 const_iterator cend() const noexcept {
        return __make_iterator(cend_unsafe());
    }

    WJR_PURE WJR_CONSTEXPR20 reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    WJR_PURE WJR_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    WJR_PURE WJR_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    WJR_PURE WJR_CONSTEXPR20 reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    WJR_PURE WJR_CONSTEXPR20 const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    WJR_PURE WJR_CONSTEXPR20 const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return get_storage().size();
    }

    WJR_CONSTEXPR20 void resize(const size_type new_size) { __resize(new_size, vctor); }

    WJR_CONSTEXPR20 void resize(const size_type new_size, const value_type &val) {
        __resize(new_size, val);
    }

    /**
     * @todo designed shrink_to_fit for storage.
     */
    WJR_CONSTEXPR20 void shrink_to_fit() {
        if constexpr (has_vector_storage_shrink_to_fit_v<storage_type>) {
            get_storage().shrink_to_fit();
        } else if constexpr (is_reallocatable::value) {
            const size_type __size = size();
            if (__size < capacity()) {
                auto &al = __get_allocator();

                storage_type new_storage;
                uninitialized_construct(new_storage, __size, __size);

                STraits::uninitialized_move_n_restrict_using_allocator(
                    data(), __size, new_storage.data(), al);
                __destroy_and_deallocate();
                __take_storage(new_storage);
            }
        }
    }

    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return get_storage().capacity();
    }

    WJR_PURE WJR_CONSTEXPR20 bool empty() const noexcept {
        if constexpr (has_vector_storage_empty_v<storage_type>) {
            return get_storage().empty();
        } else {
            return size() == 0;
        }
    }

    WJR_CONST WJR_CONSTEXPR20 static size_type
    get_growth_capacity(size_type old_capacity, size_type new_size) noexcept {
        return std::max(old_capacity + (((old_capacity + 6) >> 3) << 2), new_size);
    }

    WJR_CONSTEXPR20 void reserve(size_type n) {
        if constexpr (is_reallocatable::value) {
            const size_type old_size = size();
            const size_type old_capacity = capacity();
            if (WJR_UNLIKELY(old_capacity < n)) {
                auto &al = __get_allocator();
                const size_type new_capacity = get_growth_capacity(old_capacity, n);

                storage_type new_storage;
                uninitialized_construct(new_storage, old_size, new_capacity);

                STraits::uninitialized_move_n_restrict_using_allocator(
                    data(), old_size, new_storage.data(), al);
                __destroy_and_deallocate();
                __take_storage(new_storage);
            }
        }
    }

    WJR_CONSTEXPR20 reference operator[](size_type pos) noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(pos < size(), "basic_vector::operator[]: out of range");
#endif
        return data()[pos];
    }

    WJR_CONSTEXPR20 const_reference operator[](size_type pos) const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(pos < size(), "basic_vector::operator[]: out of range");
#endif
        return data()[pos];
    }

    WJR_CONSTEXPR20 reference at(size_type pos) {
        if (WJR_UNLIKELY(pos >= size())) {
            WJR_THROW(std::out_of_range("basic_vector::at"));
        }

        return data()[pos];
    }

    WJR_CONSTEXPR20 const_reference at(size_type pos) const {
        if (WJR_UNLIKELY(pos >= size())) {
            WJR_THROW(std::out_of_range("basic_vector::at"));
        }

        return data()[pos];
    }

    WJR_CONSTEXPR20 reference front() noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(size() > 0, "basic_vector::front: empty");
#endif
        return *data();
    }

    WJR_CONSTEXPR20 const_reference front() const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(size() > 0, "basic_vector::front: empty");
#endif
        return *data();
    }

    WJR_CONSTEXPR20 reference back() noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(size() > 0, "basic_vector::back: empty");
#endif
        return *(end_unsafe() - 1);
    }

    WJR_CONSTEXPR20 const_reference back() const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(size() > 0, "basic_vector::back: empty");
#endif
        return *(end_unsafe() - 1);
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return get_storage().data(); }

    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return get_storage().data();
    }

    WJR_PURE WJR_CONSTEXPR20 const_pointer cdata() const noexcept { return data(); }

    template <typename... Args>
    WJR_CONSTEXPR20 reference emplace_back(Args &&...args) {
        const pointer __end = end_unsafe();
        const pointer __buf_end = buf_end_unsafe();

        if (WJR_LIKELY(__end != __buf_end)) {
            STraits::uninitialized_construct_using_allocator(__end, __get_allocator(),
                                                             std::forward<Args>(args)...);
            ++__get_size();
        } else {
            __realloc_insert_at_end(std::forward<Args>(args)...);
        }

        return back();
    }

    WJR_CONSTEXPR20 void push_back(const value_type &val) { emplace_back(val); }

    WJR_CONSTEXPR20 void push_back(value_type &&val) { emplace_back(std::move(val)); }

    WJR_CONSTEXPR20 void pop_back() noexcept {
        const size_type __size = --__get_size();
        destroy_at_using_allocator(data() + __size, __get_allocator());
    }

    template <typename... Args>
    WJR_CONSTEXPR20 iterator emplace(const_iterator pos, Args &&...args) {
        return __emplace_aux(__get_pointer(pos), std::forward<Args>(args)...);
    }

    WJR_CONSTEXPR20 iterator insert(const_iterator pos, const value_type &val) {
        return emplace(pos, val);
    }

    WJR_CONSTEXPR20 iterator insert(const_iterator pos, value_type &&val) {
        return emplace(pos, std::move(val));
    }

    WJR_CONSTEXPR20 iterator insert(const_iterator pos,
                                    std::initializer_list<value_type> il) {
        return insert(pos, il.begin(), il.end());
    }

    WJR_CONSTEXPR20 iterator insert(const_iterator pos, size_type n,
                                    const value_type &val) {
        const auto old_pos = static_cast<size_type>(pos - cbegin());
        __fill_insert(data() + old_pos, n, val);
        return begin() + old_pos;
    }

    template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    WJR_CONSTEXPR20 iterator insert(const_iterator pos, Iter first, Iter last) {
        const auto old_pos = static_cast<size_type>(pos - cbegin());
        __range_insert(data() + old_pos, try_to_address(first), try_to_address(last),
                       iterator_category_t<Iter>());
        return begin() + old_pos;
    }

    WJR_CONSTEXPR20 iterator erase(const_iterator pos) {
        return __erase(__get_pointer(pos));
    }

    WJR_CONSTEXPR20 iterator erase(const_iterator first, const_iterator last) {
        return __erase(__get_pointer(first), __get_pointer(last));
    }

    WJR_CONSTEXPR20 void swap(basic_vector &other) noexcept {
        storage_fn_type::swap(*this, other);
    }

    WJR_CONSTEXPR20 void clear() {
        __erase_at_end(data());
        WJR_ASSUME(size() == 0);
    }

    WJR_PURE WJR_CONSTEXPR20 allocator_type &get_allocator() noexcept {
        return __get_allocator();
    }
    WJR_PURE WJR_CONSTEXPR20 const allocator_type &get_allocator() const noexcept {
        return __get_allocator();
    }

    // extension

    WJR_CONSTEXPR20 basic_vector(size_type n, dctor_t,
                                 const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, std::forward_as_tuple(al),
                 std::forward_as_tuple()) {
        __construct_n(n, dctor);
    }

    WJR_CONSTEXPR20 basic_vector(size_type n, in_place_reserve_t,
                                 const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, std::forward_as_tuple(al),
                 std::forward_as_tuple()) {
        uninitialized_construct(0, n);
    }

    WJR_CONSTEXPR20 basic_vector(storage_type &&other,
                                 const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, std::forward_as_tuple(al),
                 std::forward_as_tuple()) {
        take_storage(other);
    }

    WJR_CONSTEXPR20 basic_vector &operator=(storage_type &&other) {
        if (std::addressof(get_storage()) == std::addressof(other)) {
            return *this;
        }

        take_storage(other);
        return *this;
    }

    WJR_CONSTEXPR20 void resize(const size_type new_size, dctor_t) {
        __resize(new_size, dctor);
    }

    WJR_CONSTEXPR20 void push_back(dctor_t) { emplace_back(dctor); }

    WJR_CONSTEXPR20 basic_vector &append(const value_type &val) {
        emplace_back(val);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(value_type &&val) {
        emplace_back(std::move(val));
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(dctor_t) {
        emplace_back(dctor);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(const size_type n, const value_type &val) {
        __append(n, val);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(const size_type n, dctor_t) {
        __append(n, dctor);
        return *this;
    }

    template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    WJR_CONSTEXPR20 basic_vector &append(Iter first, Iter last) {
        __range_append(try_to_address(first), try_to_address(last),
                       iterator_category_t<Iter>());
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(std::initializer_list<value_type> il) {
        return append(il.begin(), il.end());
    }

    /**
     * @brief Pop n elements from the end
     *
     */
    WJR_CONSTEXPR20 basic_vector &chop(const size_type n) {
        __erase_at_end(end_unsafe() - n);
        return *this;
    }

    /**
     * @brief Truncate the size to n
     *
     */
    WJR_CONSTEXPR20 basic_vector &truncate(const size_type n) { return chop(size() - n); }

    template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    WJR_CONSTEXPR20 basic_vector &replace(const_iterator from, const_iterator to,
                                          Iter first, Iter last) {
        __range_replace(__get_pointer(from), __get_pointer(to), try_to_address(first),
                        try_to_address(last), iterator_category_t<Iter>());
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &replace(const_iterator from, const_iterator to,
                                          const size_type n, const value_type &val) {
        __fill_replace(__get_pointer(from), __get_pointer(to), n, val);
        return *this;
    }

    WJR_PURE WJR_CONSTEXPR20 storage_type &get_storage() noexcept {
        return m_pair.second();
    }

    WJR_PURE WJR_CONSTEXPR20 const storage_type &get_storage() const noexcept {
        return m_pair.second();
    }

    WJR_CONSTEXPR20 void take_storage(storage_type &other) noexcept {
        get_storage().take_storage(other, __get_allocator());
    }

    WJR_CONSTEXPR20 void uninitialized_construct(storage_type &other, size_type siz,
                                                 size_type cap) noexcept {
        get_storage().uninitialized_construct(other, siz, cap, __get_allocator());
    }

    WJR_CONSTEXPR20 void uninitialized_construct(size_type siz, size_type cap) noexcept {
        if constexpr (has_vector_storage_uninitialized_construct_v<storage_type,
                                                                   size_type, _Alty>) {
            get_storage().uninitialized_construct(siz, cap, __get_allocator());
        } else {
            uninitialized_construct(get_storage(), siz, cap);
        }
    }

private:
    // member function for container_fn (START)

    WJR_PURE WJR_CONSTEXPR20 _Alty &__get_allocator() noexcept { return m_pair.first(); }

    WJR_PURE WJR_CONSTEXPR20 const _Alty &__get_allocator() const noexcept {
        return m_pair.first();
    }

    WJR_CONSTEXPR20 void __destroy() noexcept(__storage_noexcept_destroy) {
        get_storage().destroy(__get_allocator());
    }

    WJR_CONSTEXPR20 void
    __destroy_and_deallocate() noexcept(__storage_noexcept_destroy_and_deallocate) {
        get_storage().destroy_and_deallocate(__get_allocator());
    }

    WJR_CONSTEXPR20 void __copy_element(const basic_vector &other) {
        assign(other.begin_unsafe(), other.end_unsafe());
    }

    WJR_CONSTEXPR20 void __take_storage(basic_vector &&other) noexcept(
        noexcept(__take_storage(other.get_storage()))) {
        __take_storage(other.get_storage());
    }

    WJR_CONSTEXPR20 void __move_element(basic_vector &&other) {
        assign(std::make_move_iterator(other.begin_unsafe()),
               std::make_move_iterator(other.end_unsafe()));
    }

    WJR_CONSTEXPR20 void
    __swap_storage(basic_vector &other) noexcept(__storage_noexcept_swap_storage) {
        get_storage().swap_storage(other.get_storage(), __get_allocator());
    }

    // member function for container_fn (END)

    WJR_PURE WJR_CONSTEXPR20 __get_size_t __get_size() noexcept {
        return get_storage().size();
    }

    WJR_CONSTEXPR20 void
    __take_storage(storage_type &other) noexcept(__storage_noexcept_take_storage) {
        take_storage(other);
    }

    WJR_NORETURN WJR_CONSTEXPR20 void
    __unreallocatable_unreachable(WJR_MAYBE_UNUSED size_type new_capacity) const {
        WJR_ASSERT(
            new_capacity <= capacity(),
            "new_capacity must be less than or equal to capacity if the storage is not reallocatable.\nnew_capacity = ",
            new_capacity, ", capacity = ", capacity());
        WJR_UNREACHABLE();
    }

    template <typename... Args,
              WJR_REQUIRES(sizeof...(Args) == 1 || sizeof...(Args) == 2)>
    WJR_CONSTEXPR20 void __construct_n(const size_type n, Args &&...args) {
        if (n != 0) {
            auto &al = __get_allocator();
            uninitialized_construct(get_storage(), n, n);
            if constexpr (sizeof...(Args) == 1) {
                STraits::uninitialized_fill_n_using_allocator(
                    data(), n, al, std::forward<Args>(args)...);
            } else if constexpr (sizeof...(Args) == 2) {
                STraits::uninitialized_copy_restrict_using_allocator(
                    std::forward<Args>(args)..., data(), al);
            }
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_construct(Iter first, Iter last,
                                           std::input_iterator_tag) {
        for (; first != last; ++first) {
            emplace_back(*first);
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_construct(Iter first, Iter last,
                                           std::forward_iterator_tag) {
        const auto n = static_cast<size_type>(std::distance(first, last));
        __construct_n(n, first, last);
    }

    WJR_CONSTEXPR20 void __erase_at_end(pointer pos) noexcept {
        const pointer __begin = data();
        const pointer __end = end_unsafe();
        WJR_ASSERT_L2(pos >= __begin && pos <= __end,
                      "pos must be in the range of [begin(), end()]");
        const auto new_size = static_cast<size_type>(pos - __begin);
        destroy_using_allocator(__begin + new_size, __end, __get_allocator());
        __get_size() = new_size;
    }

    WJR_CONSTEXPR20 iterator __erase(pointer pos) noexcept {
        const pointer __end = end_unsafe();
        if (pos + 1 != __end) {
            STraits::move(pos + 1, __end, pos);
        }

        destroy_at_using_allocator(__end - 1, __get_allocator());
        --__get_size();
        return __make_iterator(pos);
    }

    WJR_CONSTEXPR20 iterator __erase(pointer first, pointer last) noexcept {
        const pointer __end = end_unsafe();
        if (WJR_LIKELY(first != last)) {
            if (last != __end) {
                STraits::move(last, __end, first);
            }

            __erase_at_end(__end - (last - first));
        }

        return __make_iterator(first);
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_insert(pointer pos, Iter first, Iter last,
                                        std::input_iterator_tag) {
        if (pos == end_unsafe()) {
            __range_append(first, last, std::input_iterator_tag());
        } else if (first != last) {
            basic_vector tmp(first, last, __get_allocator());
            __range_insert(pos, tmp.begin_unsafe(), tmp.end_unsafe(),
                           std::forward_iterator_tag());
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_insert(pointer pos, Iter first, Iter last,
                                        std::forward_iterator_tag) {
        if (WJR_UNLIKELY(first == last)) {
            return;
        }

        auto &al = __get_allocator();
        const pointer __begin = data();
        const pointer __end = end_unsafe();
        const pointer __buf_end = buf_end_unsafe();

        const auto n = static_cast<size_type>(std::distance(first, last));
        const auto __rest = static_cast<size_type>(__buf_end - __end);

        if (WJR_LIKELY(__rest >= n)) {
            const auto __elements_after = static_cast<size_type>(__end - pos);
            if (__elements_after > n) {
                STraits::uninitialized_move_n_restrict_using_allocator(__end - n, n,
                                                                       __end, al);
                STraits::move_backward(pos, __end - n, __end);
                STraits::copy_restrict(first, last, pos);
            } else {
                const auto mid = std::next(first, __elements_after);

                STraits::uninitialized_copy_restrict_using_allocator(mid, last, __end,
                                                                     al);
                STraits::uninitialized_move_restrict_using_allocator(pos, __end, pos + n,
                                                                     al);
                STraits::copy_restrict(first, mid, pos);
            }

            __get_size() += n;
        } else {
            if constexpr (is_reallocatable::value) {
                const auto old_size = static_cast<size_type>(__end - __begin);
                const auto old_pos = static_cast<size_type>(pos - __begin);
                const size_type new_capacity =
                    get_growth_capacity(capacity(), old_size + n);

                storage_type new_storage;
                uninitialized_construct(new_storage, old_size + n, new_capacity);

                const pointer __new_begin = new_storage.data();

                STraits::uninitialized_copy_restrict_using_allocator(
                    first, last, __new_begin + old_pos, al);
                STraits::uninitialized_move_restrict_using_allocator(__begin, pos,
                                                                     __new_begin, al);
                STraits::uninitialized_move_restrict_using_allocator(
                    pos, __end, __new_begin + old_pos + n, al);

                __destroy_and_deallocate();
                __take_storage(new_storage);
            } else {
                __unreallocatable_unreachable(size() + n);
            }
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_append(Iter first, Iter last, std::input_iterator_tag) {
        for (; first != last; ++first) {
            emplace_back(*first);
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_append(Iter first, Iter last,
                                        std::forward_iterator_tag) {
        if (WJR_UNLIKELY(first == last)) {
            return;
        }

        auto &al = __get_allocator();
        const pointer __begin = data();
        const pointer __end = end_unsafe();
        const pointer __buf_end = buf_end_unsafe();

        const auto n = static_cast<size_type>(std::distance(first, last));
        const auto __rest = static_cast<size_type>(__buf_end - __end);

        if (WJR_LIKELY(__rest >= n)) {
            STraits::uninitialized_copy_n_restrict_using_allocator(first, n, __end, al);
            __get_size() += n;
        } else {
            if constexpr (is_reallocatable::value) {
                const auto old_size = static_cast<size_type>(__end - __begin);
                const size_type new_capacity =
                    get_growth_capacity(capacity(), old_size + n);

                storage_type new_storage;
                uninitialized_construct(new_storage, old_size + n, new_capacity);

                const pointer __new_begin = new_storage.data();

                STraits::uninitialized_copy_restrict_using_allocator(
                    first, last, __new_begin + old_size, al);
                STraits::uninitialized_move_restrict_using_allocator(__begin, __end,
                                                                     __new_begin, al);

                __destroy_and_deallocate();
                __take_storage(new_storage);
            } else {
                __unreallocatable_unreachable(size() + n);
            }
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last, std::input_iterator_tag) {
        pointer cur = data();
        const pointer __end = end_unsafe();

        for (; first != last && cur != __end; ++first, ++cur) {
            *cur = *first;
        }

        if (first == last) {
            __erase_at_end(cur);
        } else {
            __range_append(first, last, std::input_iterator_tag());
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last,
                                        std::forward_iterator_tag) {
        auto n = static_cast<size_type>(std::distance(first, last));
        auto &al = __get_allocator();
        const pointer __begin = data();
        const pointer __end = end_unsafe();

        if (n <= size()) {
            STraits::copy_restrict(first, last, __begin);
            __erase_at_end(__begin + n);
        } else if (WJR_LIKELY(n <= capacity())) {
            auto mid = first;
            std::advance(mid, size());
            STraits::copy_restrict(first, mid, data());
            STraits::uninitialized_copy_restrict_using_allocator(mid, last, __end, al);
            __get_size() = n;
        } else {
            if constexpr (is_reallocatable::value) {
                size_type new_capacity = get_growth_capacity(capacity(), n);

                storage_type new_storage;
                uninitialized_construct(new_storage, n, new_capacity);

                const pointer __new_begin = new_storage.data();
                STraits::uninitialized_copy_n_restrict_using_allocator(first, n,
                                                                       __new_begin, al);

                __destroy_and_deallocate();
                __take_storage(new_storage);
            } else {
                __unreallocatable_unreachable(n);
            }
        }
    }

    WJR_CONSTEXPR20 void __fill_assign(size_type n, const value_type &val) {
        auto &al = __get_allocator();

        if (WJR_UNLIKELY(n > capacity())) {
            if constexpr (is_reallocatable::value) {
                __destroy_and_deallocate();

                storage_type new_storage;
                uninitialized_construct(new_storage, n, n);

                STraits::uninitialized_fill_n_using_allocator(new_storage.data(), n, al,
                                                              val);
                __take_storage(new_storage);
                return;
            } else {
                __unreallocatable_unreachable(n);
            }
        }

        if (n > size()) {
            STraits::fill(begin_unsafe(), end_unsafe(), val);
            STraits::uninitialized_fill_n_using_allocator(end_unsafe(), n - size(), al,
                                                          val);
            __get_size() = n;
        } else {
            __erase_at_end(STraits::fill_n(begin_unsafe(), n, val));
        }
    }

    template <typename... Args>
    WJR_CONSTEXPR20 void __realloc_insert(pointer pos, Args &&...args) {
        if constexpr (is_reallocatable::value) {
            auto &al = __get_allocator();
            const pointer __begin = data();
            const pointer __end = end_unsafe();

            const auto old_pos_size = static_cast<size_type>(pos - __begin);
            const auto old_size = static_cast<size_type>(__end - __begin);
            const size_type new_size = old_size + 1;
            const size_type new_capacity = get_growth_capacity(old_size, new_size);

            storage_type new_storage;
            uninitialized_construct(new_storage, new_size, new_capacity);

            const pointer __new_begin = new_storage.data();
            const pointer new_pos = __new_begin + old_pos_size;

            STraits::uninitialized_construct_using_allocator(new_pos, al,
                                                             std::forward<Args>(args)...);

            STraits::uninitialized_move_n_restrict_using_allocator(__begin, old_pos_size,
                                                                   __new_begin, al);
            STraits::uninitialized_move_restrict_using_allocator(pos, __end, new_pos + 1,
                                                                 al);

            __destroy_and_deallocate();
            __take_storage(new_storage);
        } else {
            __unreallocatable_unreachable(size() + 1);
        }
    }

    template <typename... Args>
    WJR_CONSTEXPR20 void __realloc_insert_at_end(Args &&...args) {
        if constexpr (is_reallocatable::value) {
            auto &al = __get_allocator();
            const pointer __begin = data();
            const pointer __end = end_unsafe();

            const auto old_size = static_cast<size_type>(__end - __begin);
            const auto new_size = old_size + 1;
            const size_type new_capacity = get_growth_capacity(old_size, new_size);

            storage_type new_storage;
            uninitialized_construct(new_storage, new_size, new_capacity);

            const pointer __new_begin = new_storage.data();

            const pointer new_pos = __new_begin + old_size;
            STraits::uninitialized_construct_using_allocator(new_pos, al,
                                                             std::forward<Args>(args)...);

            STraits::uninitialized_move_restrict_using_allocator(__begin, __end,
                                                                 __new_begin, al);

            __destroy_and_deallocate();
            __take_storage(new_storage);
        } else {
            __unreallocatable_unreachable(size() + 1);
        }
    }

    WJR_CONSTEXPR20 void __fill_insert(pointer pos, size_type n, const value_type &val) {
        if (WJR_UNLIKELY(n == 0)) {
            return;
        }

        auto &al = __get_allocator();
        const pointer __begin = data();
        const pointer __end = end_unsafe();
        const pointer __buf_end = buf_end_unsafe();

        const auto __rest = static_cast<size_type>(__buf_end - __end);

        if (WJR_LIKELY(__rest >= n)) {
            const temporary_value_allocator tmp(al, val);
            const auto &real_val = *tmp.get();

            const auto __elements_after = static_cast<size_type>(__end - pos);
            if (__elements_after > n) {
                STraits::uninitialized_move_n_restrict_using_allocator(__end - n, n,
                                                                       __end, al);
                STraits::move_backward(pos, __end - n, __end);
                STraits::fill_n(pos, n, real_val);
            } else {
                STraits::uninitialized_fill_n_using_allocator(__end, n - __elements_after,
                                                              al, real_val);
                STraits::uninitialized_move_restrict_using_allocator(pos, __end, pos + n,
                                                                     al);
                STraits::fill(pos, __end, real_val);
            }

            __get_size() += n;
        } else {
            const auto old_size = static_cast<size_type>(__end - __begin);
            if constexpr (is_reallocatable::value) {
                const auto new_capacity = get_growth_capacity(capacity(), old_size + n);

                storage_type new_storage;
                uninitialized_construct(new_storage, old_size + n, new_capacity);

                const pointer __new_begin = new_storage.data();

                const auto old_pos = static_cast<size_type>(pos - __begin);

                STraits::uninitialized_fill_n_using_allocator(__new_begin + old_pos, n,
                                                              al, val);
                STraits::uninitialized_move_restrict_using_allocator(__begin, pos,
                                                                     __new_begin, al);
                STraits::uninitialized_move_restrict_using_allocator(
                    pos, __end, __new_begin + old_pos + n, al);

                __destroy_and_deallocate();
                __take_storage(new_storage);
            } else {
                __unreallocatable_unreachable(old_size + n);
            }
        }
    }

    template <typename Ty>
    WJR_CONSTEXPR20 void __resize(const size_type new_size, const Ty &val) {
        const auto old_size = size();

        if constexpr (is_reallocatable::value) {
            if (new_size > old_size) {
                __append(new_size - old_size, val);
            } else if (new_size < old_size) {
                __erase_at_end(data() + new_size);
            }
        } else {
            auto &al = __get_allocator();

            const pointer __begin = data();
            const pointer __end = data() + old_size;

            if (WJR_UNLIKELY(new_size > capacity())) {
                __unreallocatable_unreachable(new_size);
            }

            if (new_size > old_size) {
                STraits::uninitialized_fill_n_using_allocator(__end, new_size - old_size,
                                                              al, val);
            } else if (new_size < old_size) {
                destroy_using_allocator(__begin + new_size, __end, al);
            }

            __get_size() = new_size;
        }
    }

    template <typename Ty>
    WJR_CONSTEXPR20 void __append(size_type n, const Ty &val) {
        auto &al = __get_allocator();

        const auto old_size = size();
        const auto old_capacity = capacity();

        const pointer __begin = data();
        const pointer __end = __begin + old_size;

        const auto __rest = old_capacity - old_size;
        const auto new_size = old_size + n;

        if (WJR_LIKELY(__rest >= n)) {
            STraits::uninitialized_fill_n_using_allocator(__end, n, al, val);
            __get_size() = new_size;
        } else {
            if constexpr (is_reallocatable::value) {
                auto new_capacity = get_growth_capacity(old_capacity, new_size);

                storage_type new_storage;
                uninitialized_construct(new_storage, new_size, new_capacity);

                const pointer __new_begin = new_storage.data();

                STraits::uninitialized_fill_n_using_allocator(__new_begin + old_size, n,
                                                              al, val);
                STraits::uninitialized_move_restrict_using_allocator(__begin, __end,
                                                                     __new_begin, al);

                __destroy_and_deallocate();
                __take_storage(new_storage);
            } else {
                __unreallocatable_unreachable(new_size);
            }
        }
    }

    template <typename Args>
    WJR_CONSTEXPR20 void __insert_aux(pointer pos, Args &&args) {
        auto &al = __get_allocator();
        const pointer __end = end_unsafe();

        STraits::uninitialized_construct_using_allocator(__end, al,
                                                         std::move(*(__end - 1)));

        STraits::move_backward(pos, __end - 1, __end);
        *pos = std::forward<Args>(args);

        ++__get_size();
    }

    template <typename... Args>
    WJR_CONSTEXPR20 iterator __emplace_aux(pointer pos, Args &&...args) {
        auto &al = __get_allocator();
        const pointer __end = end_unsafe();
        const pointer __buf_end = buf_end_unsafe();

        if (WJR_LIKELY(__end != __buf_end)) {
            if (pos == __end) {
                STraits::uninitialized_construct_using_allocator(
                    __end, al, std::forward<Args>(args)...);
                ++__get_size();
            } else {
                temporary_value_allocator tmp(al, std::forward<Args>(args)...);
                __insert_aux(pos, std::move(*tmp.get()));
            }

            return __make_iterator(pos);
        }

        const size_type offset = static_cast<size_type>(pos - data());
        __realloc_insert(pos, std::forward<Args>(args)...);
        return begin() + offset;
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_replace(pointer old_first, pointer old_last,
                                         Iter new_begin, Iter new_last,
                                         std::input_iterator_tag) {
        for (; old_first != old_last && new_begin != new_last; ++old_first, ++new_begin) {
            *old_first = *new_begin;
        }

        if (new_begin == new_last) {
            __erase_at_end(old_first, old_last);
        } else {
            __range_insert(old_last, new_begin, new_last, std::input_iterator_tag());
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_replace(pointer old_first, pointer old_last,
                                         Iter new_begin, Iter new_last,
                                         std::forward_iterator_tag) {
        const auto n = static_cast<size_type>(old_last - old_first);
        const auto m = static_cast<size_type>(std::distance(new_begin, new_last));

        if (m <= n) {
            erase(STraits::copy_n(new_begin, m, old_first), old_last);
        } else {
            const auto __delta = m - n;

            auto &al = __get_allocator();
            const auto __begin = data();
            const auto __end = end_unsafe();
            const auto __buf_end = buf_end_unsafe();

            const auto __rest = static_cast<size_type>(__buf_end - __end);

            if (WJR_LIKLELY(__rest >= __delta)) {
                const auto __elements_after = static_cast<size_type>(__end - old_first);
                if (__elements_after > m) {
                    STraits::uninitialized_move_using_allocator(__end - __delta, __end,
                                                                __end, al);
                    STraits::move_backward(old_last, __end - __delta, __end);
                    STraits::copy(new_begin, new_last, old_first);
                } else {
                    auto mid = new_begin;
                    std::advance(mid, __elements_after);
                    STraits::uninitialized_copy_using_allocator(mid, new_last, __end, al);
                    STraits::uninitialized_move_using_allocator(old_last, __end,
                                                                old_first + m, al);
                    STraits::copy(new_begin, mid, old_first);
                }
                __get_size() += __delta;
            } else {
                if constexpr (is_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__end - __begin);
                    const auto old_pos = static_cast<size_type>(old_first - __begin);
                    const auto new_capacity =
                        get_growth_capacity(capacity(), old_size + __delta);

                    storage_type new_storage;
                    uninitialized_construct(new_storage, old_size + __delta,
                                            new_capacity);

                    const pointer __new_begin = new_storage.data();

                    STraits::uninitialized_copy_restrict_using_allocator(
                        new_begin, new_last, __new_begin + old_pos, al);
                    STraits::uninitialized_move_restrict_using_allocator(
                        __begin, old_first, __new_begin, al);
                    STraits::uninitialized_move_restrict_using_allocator(
                        old_last, __end, __new_begin + old_pos + m, al);

                    __destroy_and_deallocate();
                    __take_storage(new_storage);
                } else {
                    __unreallocatable_unreachable(size() + __delta);
                }
            }
        }
    }

    WJR_CONSTEXPR20 void __fill_replace(pointer old_first, pointer old_last, size_type m,
                                        const value_type &val) {
        const auto n = static_cast<size_type>(old_last - old_first);

        if (m <= n) {
            __erase(STraits::fill_n(old_first, m, val), old_last);
        } else {
            const auto __delta = m - n;

            auto &al = __get_allocator();
            const auto __begin = data();
            const auto __end = end_unsafe();
            const auto __buf_end = buf_end_unsafe();

            const auto __rest = static_cast<size_type>(__buf_end - __end);

            if (WJR_LIKELY(__rest >= __delta)) {
                const temporary_value_allocator tmp(al, val);
                const auto &real_value = *tmp.get();

                const auto __elements_after = static_cast<size_type>(__end - old_first);
                if (__elements_after > m) {
                    STraits::uninitialized_move_using_allocator(__end - __delta, __end,
                                                                __end, al);
                    STraits::move_backward(old_last, __end - __delta, __end);
                    STraits::fill_n(old_first, m, real_value);
                } else {
                    STraits::uninitialized_fill_n_using_allocator(
                        __end, m - __elements_after, al, real_value);
                    STraits::uninitialized_move_using_allocator(old_last, __end,
                                                                old_first + m, al);
                    STraits::fill(old_first, __end, real_value);
                }
                __get_size() += __delta;
            } else {
                if constexpr (is_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__end - __begin);
                    const auto old_pos = static_cast<size_type>(old_first - __begin);
                    const auto new_capacity =
                        get_growth_capacity(capacity(), old_size + __delta);

                    storage_type new_storage;
                    uninitialized_construct(new_storage, old_size + __delta,
                                            new_capacity);

                    const pointer __ptr = new_storage.data();

                    STraits::uninitialized_fill_n_using_allocator(__ptr + old_pos, m, al,
                                                                  val);
                    STraits::uninitialized_move_restrict_using_allocator(
                        __begin, old_first, __ptr, al);
                    STraits::uninitialized_move_restrict_using_allocator(
                        old_last, __end, __ptr + old_pos + m, al);

                    __destroy_and_deallocate();
                    __take_storage(new_storage);
                } else {
                    __unreallocatable_unreachable(size() + __delta);
                }
            }
        }
    }

private:
    compressed_pair<_Alty, storage_type> m_pair;
};

template <typename T, typename Alloc = std::allocator<T>>
using vector = basic_vector<default_vector_storage<T, Alloc>>;

/**
 * @brief A vector with elements stored on the stack.
 *
 */
template <typename T, size_t Capacity, typename Alloc = std::allocator<T>>
using static_vector = basic_vector<static_vector_storage<T, Capacity, Alloc>>;

/**
 * @brief A vector with fixed capacity by construction.
 *
 * @details Only allocate memory on construction and deallocation on destruction.
 * After construction, it cannot be expanded and can only be modified through move
 * assignment. For example, vector that using stack allocator.
 */
template <typename T, typename Alloc = std::allocator<T>>
using fixed_vector = basic_vector<fixed_vector_storage<T, Alloc>>;

template <typename T, size_t Capacity, typename Alloc = std::allocator<T>>
using sso_vector = basic_vector<sso_vector_storage<T, Capacity, Alloc>>;

template <typename Iter, typename T = iterator_value_t<Iter>,
          typename Alloc = std::allocator<T>, WJR_REQUIRES(is_iterator_v<Iter>)>
basic_vector(Iter, Iter, Alloc = Alloc())
    -> basic_vector<default_vector_storage<T, Alloc>>;

template <typename Storage>
void swap(basic_vector<Storage> &lhs, basic_vector<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Storage>
bool operator==(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return std::equal(lhs.begin_unsafe(), lhs.end_unsafe(), rhs.begin_unsafe(),
                      rhs.end_unsafe());
}

template <typename Storage>
bool operator!=(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return !(lhs == rhs);
}

template <typename Storage>
bool operator<(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return std::lexicographical_compare(lhs.begin_unsafe(), lhs.end_unsafe(),
                                        rhs.begin_unsafe(), rhs.end_unsafe());
}

template <typename Storage>
bool operator>(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return rhs < lhs;
}

template <typename Storage>
bool operator<=(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return !(rhs < lhs);
}

template <typename Storage>
bool operator>=(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return !(lhs < rhs);
}

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_CONTAINER_VECTOR_HPP__