/**
 * @file vector.hpp
 * @author wjr
 * @brief
 * @details
 * Functions that need to be implemented:
 * - storage() noexcept
 *  Should not allocate any memmory. By default, it is just trivial.
 *
 * - ~storage() noexcept
 *  Don't need to destroy/deallocate. By default, it is just trivial.
 *
 * - [optional] void destroy(_Alty& al) noexcept(optional)
 *  Use `al` to destroy all elements, don't need to set `empty` state. \n
 *  If this function is not provided, it will be used by default: \n
 * ```cpp
 *  destroy_using_allocator(begin_unsafe(), end_unsafe(), get_allocator());
 * ```
 *
 * - void deallocate(_Alty& al) noexcept(optional)
 *  Use `al` to deallocate, don't need to set `empty` state.
 *
 * - [optional] void deallocate_nonnul(_Alty& al) noexcept(optional)
 *  Similar to deallocate , but ensure that it is definitely not an `empty` state.
 *  If this function is not provided, it will be used by default: \n
 *  ```cpp
 *  deallocate();
 *  ```
 *  Low frequency of use, therefore it is an optional implementation
 *
 * - [optional] void destroy_and_deallocate(_Alty& al) noexcept(optional)
 *  Use `al` to destroy and then deallocate, don't need to set `empty` state. \n
 *  For some cases, perhaps destroy and dispose can be merged for optimization. \n
 *  If this function is not provided, it will be used by default: \n
 *  ```cpp
 *  destroy(); deallocate();
 *  ```
 *
 * - [static] void uninitialized_construct(storage_type &other, size_type size,
 * size_type capacity, _Alty& al) noexcept
 *  Construct a storage of other from this storage, this function can be static. \n
 *  Ensure that the capacity is not zero.
 *
 * - void take_storage(storage& other, _Alty& al) noexcept(optional)
 *  take the storage from other and set other to `empty` state. \n
 *  The `other` storage, it should be able to ensure the correct execution of
 * destroy_and_deallocate! But it do not guarantee any other functions, such as
 * destroy/deallocate/deallocate_nonnull, size(), capacity() ...
 *
 * - void swap_storage(storage& other, _Alty& al) noexcept(optional)
 *  Swap the storage with other. \n
 *  For most cases, it should comply with standards, such as not causing iterators to fail. \n
 *  But for more specialized containers like small-vector, it cannot be guaranteed!!!
 *
 * - [optinal] void shrink_to_fit(storage& other) noexcept(optional)
 *
 * - [optinal] void empty(storage& other) noexcept(optional)
 *
 * Traits that need to be implemented:
 * - is_reallocatable
 *  If false, then the capacity can only be modified in some special cases.
 *
 * @todo
 * 1. small vector. Not in accordance with standards.
 * 2. realloc. It is necessary for weak_stack_allcoator. Because this allocator can easily expand in
 * place.
 * 3. take_storage from init state?
 *
 * @version 0.3
 * @date 2025-03-09
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef WJR_CONTAINER_CONTAINER_VECTOR_HPP__
#define WJR_CONTAINER_CONTAINER_VECTOR_HPP__

#include <wjr/compressed_pair.hpp>
#include <wjr/container/container_fn.hpp>
#include <wjr/iterator/contiguous_iterator_adpater.hpp>
#include <wjr/math/detail.hpp>
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
};

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

template <typename T, typename Alloc, typename Mybase, typename IsReallocatable>
class __default_vector_storage {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using storage_traits_type = vector_storage_traits<T, Alloc>;
    using value_type = typename storage_traits_type::value_type;
    using pointer = typename storage_traits_type::pointer;
    using const_pointer = typename storage_traits_type::const_pointer;
    using size_type = typename storage_traits_type::size_type;
    using difference_type = typename storage_traits_type::difference_type;
    using allocator_type = typename storage_traits_type::allocator_type;
    using is_reallocatable = IsReallocatable;

protected:
    struct Data {
        pointer m_data = nullptr;
        pointer m_end = nullptr;
        pointer m_buffer = nullptr;
    };

    using size_ref = default_vector_size_reference<pointer, size_type>;

public:
    __default_vector_storage() = default;

    __default_vector_storage(__default_vector_storage &&) = delete;
    __default_vector_storage(const __default_vector_storage &) = delete;
    __default_vector_storage &operator=(const __default_vector_storage &) = delete;
    __default_vector_storage &operator=(__default_vector_storage &&) = delete;

    ~__default_vector_storage() = default;

    WJR_CONSTEXPR20 void deallocate_nonnull(_Alty &al) noexcept(
        noexcept(_Alty_traits::deallocate(al, this->data(), this->capacity()))) {
        WJR_ASSERT_ASSUME_L2(data() != nullptr);
        WJR_ASSERT_ASSUME_L2(capacity() != 0);
        _Alty_traits::deallocate(al, data(), capacity());
    }

    WJR_CONSTEXPR20 void deallocate(_Alty &al) noexcept(noexcept(deallocate_nonnull(al))) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(data() == nullptr)) {
            return;
        }

        if (data() != nullptr) {
            deallocate_nonnull(al);
        }
    }

    WJR_CONSTEXPR20 void
    destroy_and_deallocate(_Alty &al) noexcept(std::is_nothrow_destructible_v<value_type> &&
                                               noexcept(deallocate_nonnull(al))) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(data() == nullptr)) {
            return;
        }

        if (data() != nullptr) {
            destroy_using_allocator(m_storage.m_data, m_storage.m_end, al);
            deallocate_nonnull(al);
        }
    }

    WJR_CONSTEXPR20 static void
    uninitialized_construct(Mybase &other, size_type size, size_type capacity,
                            _Alty &al) noexcept(noexcept(allocate_at_least(al, capacity))) {
        const auto result = allocate_at_least(al, capacity);

        other.m_storage = {
            result.ptr,
            result.ptr + size,
            result.ptr + result.count,
        };
    }

    WJR_CONSTEXPR20 void take_storage(Mybase &other, _Alty &) noexcept {
        auto &other_storage = other.m_storage;
        m_storage = std::move(other_storage);
        other_storage.m_data = nullptr;
    }

    WJR_CONSTEXPR20 void swap_storage(Mybase &other, _Alty &) noexcept {
        std::swap(m_storage, other.m_storage);
    }

    WJR_PURE WJR_CONSTEXPR20 size_ref size() noexcept {
        return size_ref(m_storage.m_data, m_storage.m_end);
    }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return m_storage.m_end - m_storage.m_data;
    }

    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return m_storage.m_buffer - m_storage.m_data;
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return m_storage.m_data; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept { return m_storage.m_data; }

protected:
    Data m_storage;
};

/**
 * @brief Default vector storage
 *
 * @details Use one pointer ans two size_type currently.
 *
 */
template <typename T, typename Alloc>
class default_vector_storage
    : public __default_vector_storage<T, Alloc, default_vector_storage<T, Alloc>, std::true_type> {
};

template <typename T, typename Alloc>
struct get_relocate_mode<default_vector_storage<T, Alloc>> {
    static constexpr relocate_t value = relocate_t::trivial;
};

template <typename T, typename Alloc>
class fixed_vector_storage
    : public __default_vector_storage<T, Alloc, fixed_vector_storage<T, Alloc>, std::false_type> {};

template <typename T, typename Alloc>
struct get_relocate_mode<fixed_vector_storage<T, Alloc>> {
    static constexpr relocate_t value = relocate_t::trivial;
};

template <typename T, size_t Capacity>
class inplace_vector_storage {
    using Alloc = std::allocator<T>;
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using storage_traits_type = vector_storage_traits<T, Alloc>;
    using value_type = typename storage_traits_type::value_type;
    using pointer = typename storage_traits_type::pointer;
    using const_pointer = typename storage_traits_type::const_pointer;
    using size_type = typename storage_traits_type::size_type;
    using difference_type = typename storage_traits_type::difference_type;
    using allocator_type = typename storage_traits_type::allocator_type;
    using is_reallocatable = std::false_type;

private:
    static constexpr auto max_alignment = std::max<size_type>(alignof(T), alignof(size_type));

    struct Data {
        size_type m_size = 0;
        alignas(max_alignment) std::byte m_data[Capacity * sizeof(T)];
    };

    static constexpr bool __use_memcpy = is_trivially_allocator_construct_v<Alloc, T, T &&> &&
                                         std::is_trivially_copyable_v<Data> &&
                                         (Capacity == 1 || sizeof(Data) <= 64);

public:
    static constexpr bool is_trivially_relocate_v = __use_memcpy;

    inplace_vector_storage() = default;

    inplace_vector_storage(const inplace_vector_storage &) = delete;
    inplace_vector_storage(inplace_vector_storage &&) = delete;
    inplace_vector_storage &operator=(const inplace_vector_storage &) = delete;
    inplace_vector_storage &operator=(inplace_vector_storage &&) = delete;

    ~inplace_vector_storage() = default;

    WJR_CONSTEXPR20 void deallocate(_Alty &) noexcept { /* do nothing */ }

    WJR_CONSTEXPR20 static void uninitialized_construct(inplace_vector_storage &other,
                                                        size_type size,
                                                        WJR_MAYBE_UNUSED size_type capacity,
                                                        _Alty &) noexcept {
        WJR_ASSERT_ASSUME(capacity <= Capacity, "capacity must be less than or equal to Capacity");
        other.m_storage.m_size = size;
    }

    WJR_CONSTEXPR20 void
    take_storage(inplace_vector_storage &other,
                 _Alty &al) noexcept(std::is_nothrow_move_constructible_v<value_type>) {
        auto &other_storage = other.m_storage;
        const auto lhs = data();
        const auto rhs = other.data();

        m_storage.m_size = other_storage.m_size;

        if constexpr (__use_memcpy) {
            if (other.size()) {
                __memcpy(lhs, rhs, Capacity);
            }
        } else {
            wjr::uninitialized_move_n_restrict_using_allocator(rhs, other_storage.m_size, lhs, al);
        }

        other_storage.m_size = 0;
    }

    WJR_CONSTEXPR20 void
    swap_storage(inplace_vector_storage &other,
                 _Alty &al) noexcept(std::is_nothrow_move_constructible_v<value_type>) {
        auto &other_storage = other.m_storage;
        auto *lhs = data();
        auto lsize = size();
        auto *rhs = other.data();
        auto rsize = other.size();

        if (lsize && rsize) {
            m_storage.m_size = rsize;
            other_storage.m_size = lsize;

            alignas(sizeof(T)) std::byte __tmp_storage[sizeof(T) * Capacity];
            T *tmp = reinterpret_cast<T *>(__tmp_storage);
            if constexpr (__use_memcpy) {
                __memcpy(tmp, lhs, Capacity);
                __memcpy(lhs, rhs, Capacity);
                __memcpy(rhs, tmp, Capacity);
            } else {
                if (lsize > rsize) {
                    std::swap(lhs, rhs);
                    std::swap(lsize, rsize);
                }

                wjr::uninitialized_move_n_restrict_using_allocator(lhs, lsize, tmp, al);
                wjr::uninitialized_move_n_restrict_using_allocator(rhs, rsize, lhs, al);
                wjr::uninitialized_move_n_restrict_using_allocator(tmp, lsize, rhs, al);
            }
        } else if (rsize) {
            if constexpr (__use_memcpy) {
                __memcpy(lhs, rhs, Capacity);
            } else {
                wjr::uninitialized_move_n_restrict_using_allocator(rhs, rsize, lhs, al);
            }
            m_storage.m_size = rsize;
            other_storage.m_size = 0;
        } else if (lsize) {
            if constexpr (__use_memcpy) {
                __memcpy(rhs, lhs, Capacity);
            } else {
                wjr::uninitialized_move_n_restrict_using_allocator(lhs, lsize, rhs, al);
            }
            other_storage.m_size = lsize;
            m_storage.m_size = 0;
        }
    }

    WJR_CONSTEXPR20 size_type &size() noexcept { return m_storage.m_size; }
    WJR_CONSTEXPR20 size_type size() const noexcept { return m_storage.m_size; }
    WJR_CONST constexpr size_type capacity() const noexcept { return Capacity; }

    WJR_CONSTEXPR20 pointer data() noexcept { return reinterpret_cast<pointer>(m_storage.m_data); }
    WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return reinterpret_cast<const_pointer>(m_storage.m_data);
    }

private:
    static void __memcpy(pointer dst, const_pointer src, size_type count) noexcept {
        std::memcpy(dst, src, count * sizeof(T));
    }

    Data m_storage;
};

template <typename T, size_t Capacity>
struct get_relocate_mode<inplace_vector_storage<T, Capacity>> {
    static constexpr relocate_t value = inplace_vector_storage<T, Capacity>::is_trivially_relocate_v
                                            ? relocate_t::trivial
                                            : relocate_t::maybe_trivial;
};

template <typename T, size_t Capacity, typename Alloc>
class small_vector_storage {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

    static_assert(Capacity >= 1);

public:
    using storage_traits_type = vector_storage_traits<T, Alloc>;
    using value_type = typename storage_traits_type::value_type;
    using pointer = typename storage_traits_type::pointer;
    using const_pointer = typename storage_traits_type::const_pointer;
    using size_type = typename storage_traits_type::size_type;
    using difference_type = typename storage_traits_type::difference_type;
    using allocator_type = typename storage_traits_type::allocator_type;
    using is_reallocatable = std::true_type;

    small_vector_storage() = default;

    small_vector_storage(const small_vector_storage &) = delete;
    small_vector_storage(small_vector_storage &&) = delete;
    small_vector_storage &operator=(const small_vector_storage &) = delete;
    small_vector_storage &operator=(small_vector_storage &&) = delete;

    ~small_vector_storage() = default;

    WJR_CONSTEXPR20 void deallocate(_Alty &al) noexcept(
        noexcept(_Alty_traits::deallocate(al, data(), this->capacity()))) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(__is_small())) {
            return;
        }

        if (__is_large()) {
            _Alty_traits::deallocate(al, data(), capacity());
        }
    }

    WJR_CONSTEXPR20 static void
    uninitialized_construct(small_vector_storage &other, size_type size, size_type capacity,
                            _Alty &al) noexcept(noexcept(allocate_at_least(al, capacity))) {
        other.m_size = size;
        if (__is_small_capacity(capacity)) {
            other.m_capacity = capacity;
            return;
        }

        const auto result = allocate_at_least(al, capacity);
        other.m_data = result.ptr;
        other.m_capacity = result.count;
    }

    WJR_CONSTEXPR20 void
    take_storage(small_vector_storage &other,
                 _Alty &al) noexcept(std::is_nothrow_move_constructible_v<value_type>) {
        m_size = other.size();
        m_capacity = other.capacity();
        if (other.__is_small()) {
            __reset_data();
            wjr::uninitialized_move_n_restrict_using_allocator(other.data(), other.size(), data(),
                                                               al);
        } else {
            m_data = other.m_data;
        }

        other.m_size = 0;
        other.__reset_data();
    }

    WJR_CONSTEXPR20 void
    swap_storage(small_vector_storage &other,
                 _Alty &al) noexcept(std::is_nothrow_move_constructible_v<value_type>) {
        if (__is_small()) {
            if (other.__is_small()) {
                alignas(alignof(T)) std::byte __tmp_storage[Capacity * sizeof(T)];
                T *tmp = reinterpret_cast<T *>(__tmp_storage);

                auto *lhs = data();
                auto lsize = size();
                auto *rhs = other.data();
                auto rsize = other.size();

                if (lsize > rsize) {
                    std::swap(lhs, rhs);
                    std::swap(lsize, rsize);
                }

                wjr::uninitialized_move_n_restrict_using_allocator(lhs, lsize, tmp, al);
                wjr::uninitialized_move_n_restrict_using_allocator(rhs, rsize, lhs, al);
                wjr::uninitialized_move_n_restrict_using_allocator(tmp, lsize, rhs, al);
            } else {
                wjr::uninitialized_move_n_restrict_using_allocator(data(), size(),
                                                                   other.__get_storage(), al);
                m_data = other.m_data;
                other.__reset_data();
            }
        } else {
            if (other.__is_small()) {
                wjr::uninitialized_move_n_restrict_using_allocator(other.data(), other.size(),
                                                                   __get_storage(), al);
                other.m_data = m_data;
                __reset_data();
            } else {
                std::swap(m_data, other.m_data);
            }
        }

        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
    }

    WJR_CONSTEXPR20 size_type &size() noexcept { return m_size; }
    WJR_CONSTEXPR20 size_type size() const noexcept { return m_size; }
    constexpr size_type capacity() const noexcept { return m_capacity; }

    WJR_CONSTEXPR20 pointer data() noexcept { return m_data; }
    WJR_CONSTEXPR20 const_pointer data() const noexcept { return m_data; }

private:
    constexpr bool __is_small() const noexcept { return m_data == __get_storage(); }

    constexpr bool __is_large() const noexcept { return !__is_small(); }

    constexpr void __reset_data() noexcept { m_data = __get_storage(); }

    static constexpr bool __is_small_capacity(size_type capacity) noexcept {
        return capacity <= Capacity;
    }

    static constexpr bool __is_large_capacity(size_type capacity) noexcept {
        return !__is_small_capacity(capacity);
    }

    constexpr pointer __get_storage() noexcept { return reinterpret_cast<pointer>(m_storage); }
    constexpr const_pointer __get_storage() const noexcept {
        return reinterpret_cast<const_pointer>(m_storage);
    }

    pointer m_data = reinterpret_cast<pointer>(m_storage);
    size_type m_size = 0;
    size_type m_capacity = Capacity;

    alignas(alignof(T)) std::byte m_storage[Capacity * sizeof(T)];
};

template <typename T, size_t Capacity, typename Alloc>
struct get_relocate_mode<small_vector_storage<T, Capacity, Alloc>> {
    /// @todo May be trivial.
    static constexpr relocate_t value = relocate_t::normal;
};

WJR_REGISTER_HAS_TYPE(vector_storage_destroy,
                      std::declval<Storage>().destroy(std::declval<Alty &>()), Storage, Alty);
WJR_REGISTER_HAS_TYPE(vector_storage_deallocate_nonnull,
                      std::declval<Storage>().deallocate_nonnull(std::declval<Alty &>()), Storage,
                      Alty);
WJR_REGISTER_HAS_TYPE(vector_storage_destroy_and_deallocate,
                      std::declval<Storage>().destroy_and_deallocate(std::declval<Alty &>()),
                      Storage, Alty);
WJR_REGISTER_HAS_TYPE(vector_storage_shrink_to_fit, std::declval<Storage>().shrink_to_fit(),
                      Storage);
WJR_REGISTER_HAS_TYPE(vector_storage_empty, std::declval<Storage>().empty(), Storage);

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
    using _Alty = typename std::allocator_traits<allocator_type>::template rebind_alloc<value_type>;
    using _Alty_traits = std::allocator_traits<_Alty>;

    using __get_size_t = decltype(std::declval<storage_type>().size());
    using IteratorTraits = basic_vector_traits<storage_type>;

    using storage_fn_type = container_fn<_Alty>;
    friend class container_fn<_Alty>;

    template <typename T>
    friend struct get_relocate_mode;

    static constexpr relocate_t relocate_mode = get_common_relocate_mode_v<storage_type, _Alty>;

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
    using const_iterator = contiguous_const_iterator_adapter<basic_vector, IteratorTraits>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using storage_traits_type = typename storage_type::storage_traits_type;
    using is_trivially_contiguous = typename storage_traits_type::is_trivially_contiguous;
    using is_reallocatable = typename storage_type::is_reallocatable;

private:
    using STraits = storage_traits_type;

    static_assert(std::is_nothrow_default_constructible_v<storage_type>);

    static constexpr bool __is_nothrow_uninitialized_construct =
        noexcept(std::declval<storage_type &>().uninitialized_construct(
            std::declval<storage_type &>(), std::declval<size_type>(), std::declval<size_type>(),
            std::declval<_Alty &>()));
    static constexpr bool __is_nothrow_take_storage =
        noexcept(std::declval<storage_type &>().take_storage(std::declval<storage_type &>(),
                                                             std::declval<_Alty &>()));
    static constexpr bool __is_nothrow_swap_storage =
        noexcept(std::declval<storage_type &>().swap_storage(std::declval<storage_type &>(),
                                                             std::declval<_Alty &>()));

public:
    basic_vector() noexcept(std::is_nothrow_default_constructible_v<allocator_type>) = default;

    WJR_CONSTEXPR20 explicit basic_vector(const allocator_type &al) noexcept(
        std::is_nothrow_constructible_v<_Alty, const allocator_type &>)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {}

    WJR_CONSTEXPR20 explicit basic_vector(
        const size_type n,
        const allocator_type &al =
            allocator_type()) noexcept(std::is_nothrow_constructible_v<_Alty,
                                                                       const allocator_type &> &&
                                       noexcept(__construct_n(n, value_construct)))
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __construct_n(n, value_construct);
    }

    WJR_CONSTEXPR20
    basic_vector(
        size_type n, const value_type &val,
        const allocator_type &al =
            allocator_type()) noexcept(std::is_nothrow_constructible_v<_Alty,
                                                                       const allocator_type &> &&
                                       noexcept(__construct_n(n, val)))
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __construct_n(n, val);
    }

private:
    template <typename _Alloc>
    WJR_CONSTEXPR20 basic_vector(const basic_vector &other, _Alloc &&al, in_place_empty_t) noexcept(
        std::is_nothrow_constructible_v<_Alty, _Alloc &&> && __is_nothrow_uninitialized_construct &&
        std::is_nothrow_copy_constructible_v<value_type>)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        const auto size = other.size();
        if (WJR_LIKELY(size != 0)) {
            uninitialized_construct(size, size);
            wjr::uninitialized_copy_n_restrict_using_allocator(other.data(), size, data(),
                                                               __get_allocator());
        }
    }

    template <typename _Alloc>
    WJR_CONSTEXPR20 basic_vector(basic_vector &&other, _Alloc &&al, in_place_empty_t) noexcept(
        std::is_nothrow_constructible_v<_Alty, _Alloc &&> && __is_nothrow_take_storage)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __take_storage(std::move(other));
    }

public:
    WJR_CONSTEXPR20
    basic_vector(const basic_vector &other) noexcept(noexcept(basic_vector(
        other, _Alty_traits::select_on_container_copy_construction(other.__get_allocator()),
        in_place_empty)))
        : basic_vector(other,
                       _Alty_traits::select_on_container_copy_construction(other.__get_allocator()),
                       in_place_empty) {}

    WJR_CONSTEXPR20
    basic_vector(const basic_vector &other,
                 const allocator_type &al) noexcept(noexcept(basic_vector(other, al,
                                                                          in_place_empty)))
        : basic_vector(other, al, in_place_empty) {}

    WJR_CONSTEXPR20
    basic_vector(basic_vector &&other) noexcept(noexcept(
        basic_vector(std::move(other), std::move(other.__get_allocator()), in_place_empty)))
        : basic_vector(std::move(other), std::move(other.__get_allocator()), in_place_empty) {}

    WJR_CONSTEXPR20
    basic_vector(basic_vector &&other,
                 const allocator_type &al) noexcept(noexcept(basic_vector(std::move(other), al,
                                                                          in_place_empty)))
        : basic_vector(std::move(other), al, in_place_empty) {}

    template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    WJR_CONSTEXPR20 basic_vector(Iter first, Iter last, const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __range_construct(to_contiguous_address(first), to_contiguous_address(last),
                          iterator_category_t<Iter>());
    }

    WJR_CONSTEXPR20 basic_vector(std::initializer_list<value_type> il,
                                 const allocator_type &al = allocator_type())
        : basic_vector(il.begin(), il.end(), al) {}

    WJR_CONSTEXPR20 ~basic_vector() noexcept { __destroy_and_deallocate(); }

    WJR_CONSTEXPR20 basic_vector &operator=(const basic_vector &other) noexcept(
        noexcept(storage_fn_type::copy_assign(std::declval<basic_vector &>(),
                                              std::declval<const basic_vector &>()))) {
        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_fn_type::copy_assign(*this, other);
        }

        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &operator=(basic_vector &&other) noexcept(
        noexcept(storage_fn_type::move_assign(std::declval<basic_vector &>(),
                                              std::declval<basic_vector &&>()))) {
        WJR_ASSERT(this != std::addressof(other));
        storage_fn_type::move_assign(*this, std::move(other));
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
        __range_assign(to_contiguous_address(first), to_contiguous_address(last),
                       iterator_category_t<Iter>());
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &assign(std::initializer_list<value_type> il) {
        return assign(il.begin(), il.end());
    }

    WJR_CONSTEXPR20 pointer begin_unsafe() noexcept { return data(); }
    WJR_CONSTEXPR20 const_pointer begin_unsafe() const noexcept { return data(); }
    WJR_CONSTEXPR20 const_pointer cbegin_unsafe() const noexcept { return data(); }

    WJR_CONSTEXPR20 pointer end_unsafe() noexcept { return data() + size(); }
    WJR_CONSTEXPR20 const_pointer end_unsafe() const noexcept { return data() + size(); }
    WJR_CONSTEXPR20 const_pointer cend_unsafe() const noexcept { return end_unsafe(); }

    WJR_CONSTEXPR20 pointer buf_end_unsafe() noexcept { return data() + capacity(); }
    WJR_CONSTEXPR20 const_pointer buf_end_unsafe() const noexcept { return data() + capacity(); }
    WJR_CONSTEXPR20 const_pointer cbuf_end_unsafe() const noexcept { return buf_end_unsafe(); }

private:
    WJR_PURE WJR_CONSTEXPR20 iterator __make_iterator(const_pointer ptr) const noexcept {
        return iterator(const_cast<pointer>(ptr), this);
    }

    WJR_PURE WJR_CONSTEXPR20 pointer __get_pointer(iterator ptr) const noexcept {
        ptr.check_same_container(this);
        return wjr::to_address(ptr);
    }

    WJR_PURE WJR_CONSTEXPR20 pointer __get_pointer(const_iterator ptr) const noexcept {
        ptr.check_same_container(this);
        return const_cast<pointer>(wjr::to_address(ptr));
    }

public:
    WJR_CONSTEXPR20 iterator begin() noexcept { return __make_iterator(begin_unsafe()); }

    WJR_CONSTEXPR20 const_iterator begin() const noexcept {
        return __make_iterator(begin_unsafe());
    }

    WJR_CONSTEXPR20 const_iterator cbegin() const noexcept { return begin(); }

    WJR_CONSTEXPR20 iterator end() noexcept { return __make_iterator(end_unsafe()); }

    WJR_CONSTEXPR20 const_iterator end() const noexcept { return __make_iterator(end_unsafe()); }

    WJR_CONSTEXPR20 const_iterator cend() const noexcept { return end(); }

    WJR_CONSTEXPR20 reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    WJR_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept { return rbegin(); }

    WJR_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    WJR_CONSTEXPR20 reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    WJR_CONSTEXPR20 const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    WJR_CONSTEXPR20 const_reverse_iterator crend() const noexcept { return rend(); }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept { return get_storage().size(); }

    WJR_CONSTEXPR20 void resize(const size_type new_size) { __resize(new_size, value_construct); }

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
            if (__size == 0) {
                __deallocate();
                storage_type new_storage;
                __take_storage(new_storage);
            } else if (__size < capacity()) {
                auto &al = __get_allocator();

                storage_type new_storage;
                uninitialized_construct(new_storage, __size, __size);

                WJR_ASSUME(data() != nullptr);
                WJR_ASSUME(__size != 0);
                uninitialized_relocate_n_restrict_using_allocator(data(), __size,
                                                                  new_storage.data(), al);
                __deallocate_nonnull();
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

    WJR_CONST WJR_CONSTEXPR20 static size_type get_growth_capacity(size_type old_capacity,
                                                                   size_type new_size) noexcept {
        return std::max<size_type>(old_capacity * 2, new_size);
    }

private:
    WJR_CONSTEXPR20 void __reserve_impl(size_type n) {
        if constexpr (is_reallocatable::value) {
            const size_type old_capacity = capacity();
            if (WJR_UNLIKELY(old_capacity < n)) {
                auto &al = __get_allocator();
                const size_type old_size = size();
                const size_type new_capacity = get_growth_capacity(old_capacity, n);

                storage_type new_storage;
                uninitialized_construct(new_storage, old_size, new_capacity);

                uninitialized_relocate_n_restrict_using_allocator(data(), old_size,
                                                                  new_storage.data(), al);

                __deallocate();
                __take_storage(new_storage);
            }
        } else {
            WJR_ASSERT_ASSUME(n <= capacity());
        }
    }

    WJR_CONSTEXPR20 void __empty_reserve_impl(size_type n) {
        if constexpr (is_reallocatable::value) {
            const size_type old_capacity = capacity();
            if (WJR_UNLIKELY(old_capacity < n)) {
                const size_type new_capacity = get_growth_capacity(old_capacity, n);

                storage_type new_storage;
                uninitialized_construct(new_storage, 0, new_capacity);

                __deallocate();
                __take_storage(new_storage);
            }
        } else {
            WJR_ASSERT_ASSUME(n <= capacity());
        }
    }

public:
    WJR_CONSTEXPR20 void reserve(size_type n) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(empty())) {
            __empty_reserve_impl(n);
            return;
        }

        __reserve_impl(n);
    }

    WJR_CONSTEXPR20 reference operator[](size_type pos) noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_ALWAYS(pos < size(), "basic_vector::operator[]: out of range");
#endif
        return data()[pos];
    }

    WJR_CONSTEXPR20 const_reference operator[](size_type pos) const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_ALWAYS(pos < size(), "basic_vector::operator[]: out of range");
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
        WJR_ASSERT_ALWAYS(!empty(), "basic_vector::front: empty");
#endif
        return *data();
    }

    WJR_CONSTEXPR20 const_reference front() const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_ALWAYS(!empty(), "basic_vector::front: empty");
#endif
        return *data();
    }

    WJR_CONSTEXPR20 reference back() noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_ALWAYS(!empty(), "basic_vector::back: empty");
#endif
        return *(end_unsafe() - 1);
    }

    WJR_CONSTEXPR20 const_reference back() const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_ALWAYS(!empty(), "basic_vector::back: empty");
#endif
        return *(end_unsafe() - 1);
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return get_storage().data(); }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept { return get_storage().data(); }
    WJR_PURE WJR_CONSTEXPR20 const_pointer cdata() const noexcept { return data(); }

    template <typename... Args>
    WJR_CONSTEXPR20 reference emplace_back(Args &&...args) {
        const pointer __end = end_unsafe();
        const pointer __buf_end = buf_end_unsafe();

        if (WJR_LIKELY(__end != __buf_end)) {
            wjr::uninitialized_construct_using_allocator(__end, __get_allocator(),
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

    WJR_CONSTEXPR20 iterator insert(const_iterator pos, std::initializer_list<value_type> il) {
        return insert(pos, il.begin(), il.end());
    }

    WJR_CONSTEXPR20 iterator insert(const_iterator pos, size_type n, const value_type &val) {
        const auto old_pos = static_cast<size_type>(pos - cbegin());
        __fill_insert(data() + old_pos, n, val);
        return begin() + old_pos;
    }

    template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    WJR_CONSTEXPR20 iterator insert(const_iterator pos, Iter first, Iter last) {
        const auto old_pos = static_cast<size_type>(pos - cbegin());
        __range_insert(data() + old_pos, to_contiguous_address(first), to_contiguous_address(last),
                       iterator_category_t<Iter>());
        return begin() + old_pos;
    }

    WJR_CONSTEXPR20 iterator erase(const_iterator pos) noexcept(
        std::is_nothrow_move_assignable_v<value_type>) /* strengthened */ {
        return __erase(__get_pointer(pos));
    }

    WJR_CONSTEXPR20 iterator erase(const_iterator first, const_iterator last) noexcept(
        std::is_nothrow_move_assignable_v<value_type>) /* strengthened */ {
        return __erase(__get_pointer(first), __get_pointer(last));
    }

    WJR_CONSTEXPR20 void
    swap(basic_vector &other) noexcept(noexcept(storage_fn_type::swap(*this, other))) {
        storage_fn_type::swap(*this, other);
    }

    WJR_CONSTEXPR20 void clear() noexcept {
        __erase_at_end(data());
        WJR_ASSUME(empty());
    }

    WJR_CONSTEXPR20 allocator_type &get_allocator() noexcept { return __get_allocator(); }
    WJR_CONSTEXPR20 const allocator_type &get_allocator() const noexcept {
        return __get_allocator();
    }

    WJR_CONST static size_type max_size() noexcept { return std::numeric_limits<size_type>::max(); }

    // extension

    WJR_CONSTEXPR20 basic_vector(size_type n, default_construct_t,
                                 const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __construct_n(n, default_construct);
    }

    WJR_CONSTEXPR20 basic_vector(size_type n, in_place_reserve_t,
                                 const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        if (WJR_LIKELY(n != 0)) {
            uninitialized_construct(0, n);
        }
    }

    WJR_CONSTEXPR20 basic_vector(storage_type &&other, const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        take_storage(other);
    }

    WJR_CONSTEXPR20 basic_vector &operator=(storage_type &&other) {
        if (std::addressof(get_storage()) == std::addressof(other)) {
            return *this;
        }

        take_storage(other);
        return *this;
    }

private:
    WJR_CONSTEXPR20 void __clear_if_reserved_impl(size_type n) {
        if constexpr (is_reallocatable::value) {
            const size_type old_capacity = capacity();
            if (WJR_UNLIKELY(old_capacity < n)) {
                const size_type new_capacity = get_growth_capacity(old_capacity, n);

                storage_type new_storage;
                uninitialized_construct(new_storage, 0, new_capacity);

                __destroy_and_deallocate();
                __take_storage(new_storage);

                WJR_ASSUME(empty());
            }
        } else {
            WJR_ASSERT_ASSUME(n <= capacity());
        }
    }

public:
    /**
     * @brief clear() if capacity() < new_capacity.
     *
     * @details Useful when old data unused. If reserved, this function won't
     * move any old data to new pointer. This is also very useful when the move
     * constructor is deleted.
     */
    WJR_CONSTEXPR20 void clear_if_reserved(size_type n) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(empty())) {
            __empty_reserve_impl(n);
            return;
        }

        __clear_if_reserved_impl(n);
    }

    WJR_CONSTEXPR20 void resize(const size_type new_size, default_construct_t) {
        __resize(new_size, default_construct);
    }

    WJR_CONSTEXPR20 void push_back(default_construct_t) { emplace_back(default_construct); }

    WJR_CONSTEXPR20 basic_vector &append(const value_type &val) {
        emplace_back(val);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(value_type &&val) {
        emplace_back(std::move(val));
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(default_construct_t) {
        emplace_back(default_construct);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(const size_type n, const value_type &val) {
        __append(n, val);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(const size_type n, default_construct_t) {
        __append(n, default_construct);
        return *this;
    }

    template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    WJR_CONSTEXPR20 basic_vector &append(Iter first, Iter last) {
        __range_append(to_contiguous_address(first), to_contiguous_address(last),
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
    WJR_CONSTEXPR20 basic_vector &replace(const_iterator from, const_iterator to, Iter first,
                                          Iter last) {
        __range_replace(__get_pointer(from), __get_pointer(to), to_contiguous_address(first),
                        to_contiguous_address(last), iterator_category_t<Iter>());
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &replace(const_iterator from, const_iterator to, const size_type n,
                                          const value_type &val) {
        __fill_replace(__get_pointer(from), __get_pointer(to), n, val);
        return *this;
    }

    WJR_CONSTEXPR20 storage_type &get_storage() noexcept { return m_pair.second(); }

    WJR_CONSTEXPR20 const storage_type &get_storage() const noexcept { return m_pair.second(); }

    WJR_CONSTEXPR20 void take_storage(storage_type &other) noexcept {
        get_storage().take_storage(other, __get_allocator());
    }

    WJR_CONSTEXPR20 void
    uninitialized_construct(storage_type &other, size_type siz,
                            size_type cap) noexcept(__is_nothrow_uninitialized_construct) {
        WJR_ASSERT_ASSUME(cap != 0);
        get_storage().uninitialized_construct(other, siz, cap, __get_allocator());
    }

    WJR_CONSTEXPR20 void
    uninitialized_construct(size_type siz,
                            size_type cap) noexcept(__is_nothrow_uninitialized_construct) {
        uninitialized_construct(get_storage(), siz, cap);
    }

private:
    WJR_CONSTEXPR20 void
    __deallocate() noexcept(noexcept(get_storage().deallocate(__get_allocator()))) {
        get_storage().deallocate(__get_allocator());
    }

    template <typename S, WJR_REQUIRES(has_vector_storage_destroy_v<S, _Alty>)>
    WJR_CONSTEXPR20 void
    __destroy_impl() noexcept(noexcept(get_storage().destroy(__get_allocator()))) {
        get_storage().destroy(__get_allocator());
    }

    template <typename S, WJR_REQUIRES(!has_vector_storage_destroy_v<S, _Alty>)>
    WJR_CONSTEXPR20 void __destroy_impl() noexcept(std::is_nothrow_destructible_v<value_type>) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(empty())) {
            return;
        }

        destroy_using_allocator(begin_unsafe(), end_unsafe(), __get_allocator());
    }

    template <typename S, WJR_REQUIRES(has_vector_storage_destroy_and_deallocate_v<S, _Alty>)>
    WJR_CONSTEXPR20 void __destroy_and_deallocate_impl() noexcept(
        noexcept(get_storage().destroy_and_deallocate(__get_allocator()))) {
        get_storage().destroy_and_deallocate(__get_allocator());
    }

    template <typename S, WJR_REQUIRES(!has_vector_storage_destroy_and_deallocate_v<S, _Alty>)>
    WJR_CONSTEXPR20 void __destroy_and_deallocate_impl() noexcept(noexcept(__destroy()) &&
                                                                  noexcept(__deallocate())) {
        __destroy();
        __deallocate();
    }

    // member function for container_fn (START)

    WJR_CONSTEXPR20 _Alty &__get_allocator() noexcept { return m_pair.first(); }
    WJR_CONSTEXPR20 const _Alty &__get_allocator() const noexcept { return m_pair.first(); }

    WJR_CONSTEXPR20 void __destroy() noexcept(noexcept(__destroy_impl<storage_type>())) {
        __destroy_impl<storage_type>();
    }

    WJR_CONSTEXPR20 void
    __destroy_and_deallocate() noexcept(noexcept(__destroy_and_deallocate_impl<storage_type>())) {
        __destroy_and_deallocate_impl<storage_type>();
    }

    WJR_CONSTEXPR20 void __copy_element(const basic_vector &other) {
        assign(other.begin_unsafe(), other.end_unsafe());
    }

    WJR_CONSTEXPR20 void __take_storage(basic_vector &&other) noexcept(__is_nothrow_take_storage) {
        __take_storage(other.get_storage());
    }

    WJR_CONSTEXPR20 void __move_element(basic_vector &&other) {
        assign(std::make_move_iterator(other.begin_unsafe()),
               std::make_move_iterator(other.end_unsafe()));
    }

    WJR_CONSTEXPR20 void __swap_storage(basic_vector &other) noexcept(__is_nothrow_swap_storage) {
        get_storage().swap_storage(other.get_storage(), __get_allocator());
    }

    // member function for container_fn (END)

    template <typename S, WJR_REQUIRES(has_vector_storage_deallocate_nonnull_v<S, _Alty>)>
    WJR_CONSTEXPR20 void __deallocate_nonnull_impl() noexcept(
        noexcept(get_storage().deallocate_nonnull(__get_allocator()))) {
        get_storage().deallocate_nonnull(__get_allocator());
    }

    template <typename S, WJR_REQUIRES(!has_vector_storage_deallocate_nonnull_v<S, _Alty>)>
    WJR_CONSTEXPR20 void __deallocate_nonnull_impl() noexcept(noexcept(__deallocate())) {
        __deallocate();
    }

    WJR_CONSTEXPR20 void
    __deallocate_nonnull() noexcept(noexcept(__deallocate_nonnull_impl<storage_type>())) {
        __deallocate_nonnull_impl<storage_type>();
    }

    WJR_PURE WJR_CONSTEXPR20 __get_size_t __get_size() noexcept { return get_storage().size(); }

    WJR_CONSTEXPR20 void __take_storage(storage_type &other) noexcept(__is_nothrow_take_storage) {
        take_storage(other);
    }

    WJR_NORETURN WJR_CONSTEXPR20 void
    __unreallocatable_unreachable(WJR_MAYBE_UNUSED size_type new_capacity) const {
        WJR_ASSERT_L1(new_capacity <= capacity(),
                      "new_capacity must be less than or equal to capacity if the "
                      "storage is not reallocatable.\nnew_capacity = ",
                      new_capacity, ", capacity = ", capacity());
        WJR_UNREACHABLE();
    }

    template <typename Ty>
    WJR_CONSTEXPR20 void __construct_n(const size_type n, Ty &&val) {
        if (WJR_LIKELY(n != 0)) {
            auto &al = __get_allocator();
            uninitialized_construct(n, n);
            wjr::uninitialized_fill_n_using_allocator(data(), n, al, std::forward<Ty>(val));
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __construct_n(const size_type n, Iter first, Iter last) {
        if (WJR_LIKELY(n != 0)) {
            auto &al = __get_allocator();
            uninitialized_construct(n, n);
            wjr::uninitialized_copy_restrict_using_allocator(first, last, data(), al);
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_construct(Iter first, Iter last, std::input_iterator_tag) {
        for (; first != last; ++first) {
            emplace_back(*first);
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_construct(Iter first, Iter last, std::forward_iterator_tag) {
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
            std::move(pos + 1, __end, pos);
        }

        destroy_at_using_allocator(__end - 1, __get_allocator());
        --__get_size();
        return __make_iterator(pos);
    }

    WJR_CONSTEXPR20 iterator __erase(pointer first, pointer last) noexcept {
        const pointer __end = end_unsafe();
        if (WJR_LIKELY(first != last)) {
            if (last != __end) {
                std::move(last, __end, first);
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
            __range_insert(pos, tmp.begin_unsafe(), tmp.end_unsafe(), std::forward_iterator_tag());
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
                wjr::uninitialized_move_n_restrict_using_allocator(__end - n, n, __end, al);
                std::move_backward(pos, __end - n, __end);
                wjr::copy_restrict(first, last, pos);
            } else {
                const auto mid = std::next(first, __elements_after);

                wjr::uninitialized_copy_restrict_using_allocator(mid, last, __end, al);
                wjr::uninitialized_move_restrict_using_allocator(pos, __end, pos + n, al);
                wjr::copy_restrict(first, mid, pos);
            }

            __get_size() += n;
        } else {
            if constexpr (is_reallocatable::value) {
                const auto old_size = static_cast<size_type>(__end - __begin);
                const auto old_pos = static_cast<size_type>(pos - __begin);
                const size_type new_capacity = get_growth_capacity(capacity(), old_size + n);

                storage_type new_storage;
                uninitialized_construct(new_storage, old_size + n, new_capacity);

                const pointer __new_begin = new_storage.data();

                /// @todo this can be optimize by using relocate.

                wjr::uninitialized_copy_restrict_using_allocator(first, last, __new_begin + old_pos,
                                                                 al);
                wjr::uninitialized_move_restrict_using_allocator(__begin, pos, __new_begin, al);
                wjr::uninitialized_move_restrict_using_allocator(pos, __end,
                                                                 __new_begin + old_pos + n, al);

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
    WJR_CONSTEXPR20 void __range_append(Iter first, Iter last, std::forward_iterator_tag) {
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
            wjr::uninitialized_copy_n_restrict_using_allocator(first, n, __end, al);
            __get_size() += n;
        } else {
            if constexpr (is_reallocatable::value) {
                const auto old_size = static_cast<size_type>(__end - __begin);
                const size_type new_capacity = get_growth_capacity(capacity(), old_size + n);

                storage_type new_storage;
                uninitialized_construct(new_storage, old_size + n, new_capacity);

                const pointer __new_begin = new_storage.data();

                wjr::uninitialized_copy_restrict_using_allocator(first, last,
                                                                 __new_begin + old_size, al);

                uninitialized_relocate_restrict_using_allocator(__begin, __end, __new_begin, al);
                __deallocate();

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
    WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last, std::forward_iterator_tag) {
        auto n = static_cast<size_type>(std::distance(first, last));
        auto &al = __get_allocator();
        const pointer __begin = data();
        const pointer __end = end_unsafe();

        if (n <= size()) {
            wjr::copy_restrict(first, last, __begin);
            __erase_at_end(__begin + n);
        } else if (WJR_LIKELY(n <= capacity())) {
            auto mid = first;
            std::advance(mid, size());
            wjr::copy_restrict(first, mid, data());
            wjr::uninitialized_copy_restrict_using_allocator(mid, last, __end, al);
            __get_size() = n;
        } else {
            if constexpr (is_reallocatable::value) {
                size_type new_capacity = get_growth_capacity(capacity(), n);

                storage_type new_storage;
                uninitialized_construct(new_storage, n, new_capacity);

                const pointer __new_begin = new_storage.data();
                wjr::uninitialized_copy_n_restrict_using_allocator(first, n, __new_begin, al);

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

                wjr::uninitialized_fill_n_using_allocator(new_storage.data(), n, al, val);
                __take_storage(new_storage);
                return;
            } else {
                __unreallocatable_unreachable(n);
            }
        }

        if (n > size()) {
            std::fill(begin_unsafe(), end_unsafe(), val);
            wjr::uninitialized_fill_n_using_allocator(end_unsafe(), n - size(), al, val);
            __get_size() = n;
        } else {
            __erase_at_end(std::fill_n(begin_unsafe(), n, val));
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

            wjr::uninitialized_construct_using_allocator(new_pos, al, std::forward<Args>(args)...);
            wjr::uninitialized_move_n_restrict_using_allocator(__begin, old_pos_size, __new_begin,
                                                               al);
            wjr::uninitialized_move_restrict_using_allocator(pos, __end, new_pos + 1, al);

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
            wjr::uninitialized_construct_using_allocator(new_pos, al, std::forward<Args>(args)...);

            uninitialized_relocate_restrict_using_allocator(__begin, __end, __new_begin, al);
            __deallocate();

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
                wjr::uninitialized_move_n_restrict_using_allocator(__end - n, n, __end, al);
                std::move_backward(pos, __end - n, __end);
                std::fill_n(pos, n, real_val);
            } else {
                wjr::uninitialized_fill_n_using_allocator(__end, n - __elements_after, al,
                                                          real_val);
                wjr::uninitialized_move_restrict_using_allocator(pos, __end, pos + n, al);
                std::fill(pos, __end, real_val);
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

                wjr::uninitialized_fill_n_using_allocator(__new_begin + old_pos, n, al, val);
                wjr::uninitialized_move_restrict_using_allocator(__begin, pos, __new_begin, al);
                wjr::uninitialized_move_restrict_using_allocator(pos, __end,
                                                                 __new_begin + old_pos + n, al);

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
                wjr::uninitialized_fill_n_using_allocator(__end, new_size - old_size, al, val);
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
            wjr::uninitialized_fill_n_using_allocator(__end, n, al, val);
            __get_size() = new_size;
        } else {
            if constexpr (is_reallocatable::value) {
                auto new_capacity = get_growth_capacity(old_capacity, new_size);

                storage_type new_storage;
                uninitialized_construct(new_storage, new_size, new_capacity);

                const pointer __new_begin = new_storage.data();

                wjr::uninitialized_fill_n_using_allocator(__new_begin + old_size, n, al, val);

                uninitialized_relocate_restrict_using_allocator(__begin, __end, __new_begin, al);
                __deallocate();

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

        wjr::uninitialized_construct_using_allocator(__end, al, std::move(*(__end - 1)));

        std::move_backward(pos, __end - 1, __end);
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
                wjr::uninitialized_construct_using_allocator(__end, al,
                                                             std::forward<Args>(args)...);
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
    WJR_CONSTEXPR20 void __range_replace(pointer old_first, pointer old_last, Iter new_begin,
                                         Iter new_last, std::input_iterator_tag) {
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
    WJR_CONSTEXPR20 void __range_replace(pointer old_first, pointer old_last, Iter new_begin,
                                         Iter new_last, std::forward_iterator_tag) {
        const auto n = static_cast<size_type>(old_last - old_first);
        const auto m = static_cast<size_type>(std::distance(new_begin, new_last));

        if (m <= n) {
            erase(std::copy_n(new_begin, m, old_first), old_last);
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
                    wjr::uninitialized_move_using_allocator(__end - __delta, __end, __end, al);
                    std::move_backward(old_last, __end - __delta, __end);
                    std::copy(new_begin, new_last, old_first);
                } else {
                    auto mid = new_begin;
                    std::advance(mid, __elements_after);
                    wjr::uninitialized_copy_using_allocator(mid, new_last, __end, al);
                    wjr::uninitialized_move_using_allocator(old_last, __end, old_first + m, al);
                    std::copy(new_begin, mid, old_first);
                }
                __get_size() += __delta;
            } else {
                if constexpr (is_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__end - __begin);
                    const auto old_pos = static_cast<size_type>(old_first - __begin);
                    const auto new_capacity = get_growth_capacity(capacity(), old_size + __delta);

                    storage_type new_storage;
                    uninitialized_construct(new_storage, old_size + __delta, new_capacity);

                    const pointer __new_begin = new_storage.data();

                    wjr::uninitialized_copy_restrict_using_allocator(new_begin, new_last,
                                                                     __new_begin + old_pos, al);
                    wjr::uninitialized_move_restrict_using_allocator(__begin, old_first,
                                                                     __new_begin, al);
                    wjr::uninitialized_move_restrict_using_allocator(old_last, __end,
                                                                     __new_begin + old_pos + m, al);

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
            __erase(std::fill_n(old_first, m, val), old_last);
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
                    wjr::uninitialized_move_using_allocator(__end - __delta, __end, __end, al);
                    std::move_backward(old_last, __end - __delta, __end);
                    std::fill_n(old_first, m, real_value);
                } else {
                    wjr::uninitialized_fill_n_using_allocator(__end, m - __elements_after, al,
                                                              real_value);
                    wjr::uninitialized_move_using_allocator(old_last, __end, old_first + m, al);
                    std::fill(old_first, __end, real_value);
                }
                __get_size() += __delta;
            } else {
                if constexpr (is_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__end - __begin);
                    const auto old_pos = static_cast<size_type>(old_first - __begin);
                    const auto new_capacity = get_growth_capacity(capacity(), old_size + __delta);

                    storage_type new_storage;
                    uninitialized_construct(new_storage, old_size + __delta, new_capacity);

                    const pointer __ptr = new_storage.data();

                    wjr::uninitialized_fill_n_using_allocator(__ptr + old_pos, m, al, val);
                    wjr::uninitialized_move_restrict_using_allocator(__begin, old_first, __ptr, al);
                    wjr::uninitialized_move_restrict_using_allocator(old_last, __end,
                                                                     __ptr + old_pos + m, al);

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

template <typename Iter, typename T = iterator_value_t<Iter>, typename Alloc = std::allocator<T>,
          WJR_REQUIRES(is_iterator_v<Iter>)>
basic_vector(Iter, Iter, Alloc = Alloc()) -> basic_vector<default_vector_storage<T, Alloc>>;

template <typename S>
struct get_relocate_mode<basic_vector<S>> {
    static constexpr relocate_t value = basic_vector<S>::relocate_mode;
};

template <typename T, typename Alloc = std::allocator<T>>
using vector = basic_vector<default_vector_storage<T, Alloc>>;

/**
 * @brief A vector with elements stored on the stack.
 *
 */
template <typename T, size_t Capacity>
using inplace_vector = basic_vector<inplace_vector_storage<T, Capacity>>;

/**
 * @brief 
 * 
 * @note Swap does not comply with the C++ specification/
 * 
 */
template <typename T, size_t Capacity, typename Alloc = std::allocator<T>>
using small_vector = basic_vector<small_vector_storage<T, Capacity, Alloc>>;

/**
 * @brief A vector with fixed capacity by construction.
 *
 * @details Only allocate memory on construction and deallocation on
 * destruction. After construction, it cannot be expanded and can only be
 * modified through move assignment. For example, vector that using stack
 * allocator.
 */
template <typename T, typename Alloc = std::allocator<T>>
using fixed_vector = basic_vector<fixed_vector_storage<T, Alloc>>;

template <typename Storage>
void swap(basic_vector<Storage> &lhs, basic_vector<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Storage>
bool operator==(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return std::equal(lhs.begin_unsafe(), lhs.end_unsafe(), rhs.begin_unsafe(), rhs.end_unsafe());
}

template <typename Storage>
bool operator!=(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return !(lhs == rhs);
}

template <typename Storage>
bool operator<(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return std::lexicographical_compare(lhs.begin_unsafe(), lhs.end_unsafe(), rhs.begin_unsafe(),
                                        rhs.end_unsafe());
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

namespace std {

template <typename Storage>
constexpr void swap(wjr::basic_vector<Storage> &lhs,
                    wjr::basic_vector<Storage> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace std

#endif // WJR_CONTAINER_CONTAINER_VECTOR_HPP__