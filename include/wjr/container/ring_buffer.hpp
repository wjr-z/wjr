#ifndef WJR_CONTAINER_CONTAINER_RING_BUFFER_HPP__
#define WJR_CONTAINER_CONTAINER_RING_BUFFER_HPP__

/**
 * @file ring_buffer.hpp
 * @version 0.1
 * @date 2024-11-02
 *
 */

#include <wjr/compressed_pair.hpp>
#include <wjr/container/container_fn.hpp>
#include <wjr/iterator/contiguous_iterator_adpater.hpp>
#include <wjr/math/detail.hpp>
#include <wjr/memory/copy.hpp>
#include <wjr/memory/memory_pool.hpp>
#include <wjr/memory/temporary_value_allocator.hpp>

namespace wjr {

template <typename T, typename Alloc>
class ring_buffer_storage_traits {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = T;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using allocator_type = Alloc;
    using is_trivially_contiguous = std::false_type;
};

template <typename pointer, typename size_type>
class default_ring_buffer_size_reference {
public:
    default_ring_buffer_size_reference() = delete;
    default_ring_buffer_size_reference(const default_ring_buffer_size_reference &) = delete;
    default_ring_buffer_size_reference(default_ring_buffer_size_reference &&) = default;
    default_ring_buffer_size_reference &
    operator=(const default_ring_buffer_size_reference &) = delete;
    default_ring_buffer_size_reference &operator=(default_ring_buffer_size_reference &&) = default;
    ~default_ring_buffer_size_reference() = default;

    constexpr explicit default_ring_buffer_size_reference(pointer ptr, pointer &pos) noexcept
        : m_ptr(ptr), m_pos(pos) {}

    constexpr default_ring_buffer_size_reference &operator=(size_type size) noexcept {
        m_pos = m_ptr + size;
        return *this;
    }

    constexpr operator size_type() const noexcept { return static_cast<size_type>(m_pos - m_ptr); }

    constexpr default_ring_buffer_size_reference &operator++() noexcept {
        ++m_pos;
        return *this;
    }

    constexpr default_ring_buffer_size_reference &operator--() noexcept {
        --m_pos;
        return *this;
    }

    constexpr default_ring_buffer_size_reference &operator+=(size_type size) noexcept {
        m_pos += size;
        return *this;
    }

    constexpr default_ring_buffer_size_reference &operator-=(size_type size) noexcept {
        m_pos -= size;
        return *this;
    }

private:
    pointer m_ptr;
    pointer &m_pos;
};

/// @private
template <typename pointer, typename size_type>
struct __unref_wrapper_helper<default_ring_buffer_size_reference<pointer, size_type>> {
    using type = size_type &;
};

template <typename T, typename Alloc, typename Mybase, typename IsReallocatable>
class __default_ring_buffer_storage {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using storage_traits_type = ring_buffer_storage_traits<T, Alloc>;
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
        pointer m_head = nullptr;
        pointer m_tail = nullptr;
        size_type m_size = 0;
        pointer m_buf_end = nullptr;
    };

    using data_type = Data;
    using size_ref = default_ring_buffer_size_reference<pointer, size_type>;

public:
    __default_ring_buffer_storage() = default;

    __default_ring_buffer_storage(__default_ring_buffer_storage &&) = delete;
    __default_ring_buffer_storage(const __default_ring_buffer_storage &) = delete;
    __default_ring_buffer_storage &operator=(const __default_ring_buffer_storage &) = delete;
    __default_ring_buffer_storage &operator=(__default_ring_buffer_storage &&) = delete;

    ~__default_ring_buffer_storage() = default;

    WJR_CONSTEXPR20 void deallocate(_Alty &al) noexcept(
        noexcept(_Alty_traits::deallocate(al, this->m_storage.m_data, this->capacity()))) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(data() == nullptr)) {
            return;
        }

        if (m_storage.m_data != nullptr) {
            WJR_ASSERT_ASSUME_L2(capacity() != 0);
            _Alty_traits::deallocate(al, m_storage.m_data, capacity());
        }
    }

    WJR_CONSTEXPR20 void deallocate_nonnull(_Alty &al) noexcept(
        noexcept(_Alty_traits::deallocate(al, this->m_storage.m_data, this->capacity()))) {
        WJR_ASSERT_ASSUME_L2(m_storage.m_data != nullptr);
        WJR_ASSERT_ASSUME_L2(capacity() != 0);
        _Alty_traits::deallocate(al, m_storage.m_data, capacity());
    }

    WJR_INTRINSIC_CONSTEXPR20 static void
    uninitialized_construct(Mybase &other, size_type head, size_type tail, size_type size,
                            size_type capacity,
                            _Alty &al) noexcept(noexcept(allocate_at_least(al, capacity))) {
        WJR_ASSERT_ASSUME_L2(has_single_bit(capacity));
        const auto ptr = _Alty_traits::allocate(al, capacity);
        other.m_storage = {ptr, ptr + head, ptr + tail, size, ptr + capacity};
    }

    WJR_CONSTEXPR20 void take_storage(Mybase &other, _Alty &) noexcept {
        auto &other_storage = other.m_storage;
        m_storage = std::move(other_storage);
        other_storage = {};
    }

    WJR_CONSTEXPR20 void swap_storage(Mybase &other, _Alty &) noexcept {
        std::swap(m_storage, other.m_storage);
    }

    WJR_CONSTEXPR20 pointer &head() noexcept { return m_storage.m_head; }
    WJR_CONSTEXPR20 pointer &tail() noexcept { return m_storage.m_tail; }
    WJR_CONSTEXPR20 size_type &size() noexcept { return m_storage.m_size; }

    WJR_PURE WJR_CONSTEXPR20 pointer head() const noexcept { return m_storage.m_head; }
    WJR_PURE WJR_CONSTEXPR20 pointer tail() const noexcept { return m_storage.m_tail; }
    WJR_PURE WJR_CONSTEXPR20 pointer buffer() const noexcept { return m_storage.m_buf_end; }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept { return m_storage.m_size; }

    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return m_storage.m_buf_end - m_storage.m_data;
    }

    WJR_CONSTEXPR20 pointer data() noexcept { return m_storage.m_data; }
    WJR_CONSTEXPR20 const_pointer data() const noexcept { return m_storage.m_data; }

protected:
    data_type m_storage;
};

/**
 * @brief Default ring_buffer storage
 *
 * @details Use one pointer ans two size_type currently.
 *
 */
template <typename T, typename Alloc>
class default_ring_buffer_storage
    : public __default_ring_buffer_storage<T, Alloc, default_ring_buffer_storage<T, Alloc>,
                                           std::true_type> {};

template <typename T, typename Alloc>
struct get_relocate_mode<default_ring_buffer_storage<T, Alloc>> {
    static constexpr relocate_t value = relocate_t::trivial;
};

template <typename T, typename Alloc>
class fixed_ring_buffer_storage
    : public __default_ring_buffer_storage<T, Alloc, fixed_ring_buffer_storage<T, Alloc>,
                                           std::false_type> {};

template <typename T, typename Alloc>
struct get_relocate_mode<fixed_ring_buffer_storage<T, Alloc>> {
    static constexpr relocate_t value = relocate_t::trivial;
};

WJR_REGISTER_HAS_TYPE(ring_buffer_storage_shrink_to_fit, std::declval<Storage>().shrink_to_fit(),
                      Storage);

WJR_REGISTER_HAS_TYPE(ring_buffer_storage_uninitialized_construct,
                      std::declval<Storage>().uninitialized_construct(
                          std::declval<Size>(), std::declval<Size>(), std::declval<Size>(),
                          std::declval<Size>(), std::declval<Alloc &>()),
                      Storage, Size, Alloc);

/**
 * @brief Customized ring_buffer by storage.
 *
 * @details Type of pointer is same as iterator.
 *
 */
template <typename Storage>
class basic_ring_buffer {
public:
    using value_type = typename Storage::value_type;
    using allocator_type = typename Storage::allocator_type;
    using storage_type = Storage;

private:
    using _Alty = typename std::allocator_traits<allocator_type>::template rebind_alloc<value_type>;
    using _Alty_traits = std::allocator_traits<_Alty>;

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

    using storage_traits_type = typename storage_type::storage_traits_type;
    using is_trivially_contiguous = typename storage_traits_type::is_trivially_contiguous;
    using is_reallocatable = typename storage_type::is_reallocatable;

private:
    using STraits = storage_traits_type;

    static_assert(std::is_nothrow_default_constructible_v<storage_type>);

    static constexpr bool __is_nothrow_deallocate =
        noexcept(std::declval<storage_type &>().deallocate(std::declval<_Alty &>()));
    static constexpr bool __is_nothrow_deallocate_nonnull =
        noexcept(std::declval<storage_type &>().deallocate_nonnull(std::declval<_Alty &>()));
    static constexpr bool __is_nothrow_uninitialized_construct =
        noexcept(std::declval<storage_type &>().uninitialized_construct(
            std::declval<storage_type &>(), std::declval<size_type>(), std::declval<size_type>(),
            std::declval<size_type>(), std::declval<size_type>(), std::declval<_Alty &>()));
    static constexpr bool __is_nothrow_take_storage =
        noexcept(std::declval<storage_type &>().take_storage(std::declval<storage_type &>(),
                                                             std::declval<_Alty &>()));
    static constexpr bool __is_nothrow_swap_storage =
        noexcept(std::declval<storage_type &>().swap_storage(std::declval<storage_type &>(),
                                                             std::declval<_Alty &>()));

public:
    basic_ring_buffer() noexcept(std::is_nothrow_default_constructible_v<allocator_type>) = default;

    WJR_CONSTEXPR20 explicit basic_ring_buffer(const allocator_type &al) noexcept(
        std::is_nothrow_constructible_v<_Alty, const allocator_type &>)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {}

    WJR_CONSTEXPR20 explicit basic_ring_buffer(
        const size_type n,
        const allocator_type &al =
            allocator_type()) noexcept(std::is_nothrow_constructible_v<_Alty,
                                                                       const allocator_type &>
                                           && noexcept(__construct_n(n, value_construct)))
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __construct_n(n, value_construct);
    }

    WJR_CONSTEXPR20
    basic_ring_buffer(
        size_type n, const value_type &val,
        const allocator_type &al =
            allocator_type()) noexcept(std::is_nothrow_constructible_v<_Alty,
                                                                       const allocator_type &>
                                           && noexcept(__construct_n(n, val)))
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __construct_n(n, val);
    }

private:
    template <typename _Alloc>
    WJR_CONSTEXPR20
    basic_ring_buffer(const basic_ring_buffer &other, _Alloc &&al, in_place_empty_t) noexcept(
        std::is_nothrow_constructible_v<_Alty, _Alloc &&> &&__is_nothrow_uninitialized_construct
            &&std::is_nothrow_copy_constructible_v<value_type>)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        const size_type __size = other.size();
        if (WJR_LIKELY(__size != 0)) {
            uninitialized_construct(0, __size, __size, __size);
            pointer dst = data();
            const const_pointer __head = other.__get_head();
            const const_pointer __tail = other.__get_head();

            auto &__al = __get_allocator();

            if (__head < __tail) {
                wjr::uninitialized_copy_restrict_using_allocator(__head, __tail, dst, __al);
            } else {
                const_pointer __data = other.data();
                const_pointer __buf_end = other.__get_buf_end();
                dst =
                    wjr::uninitialized_copy_restrict_using_allocator(__head, __buf_end, dst, __al);
                wjr::uninitialized_copy_restrict_using_allocator(__data, __tail, dst, __al);
            }
        }
    }

    template <typename _Alloc>
    WJR_CONSTEXPR20
    basic_ring_buffer(basic_ring_buffer &&other, _Alloc &&al, in_place_empty_t) noexcept(
        std::is_nothrow_constructible_v<_Alty, _Alloc &&> &&__is_nothrow_take_storage)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __take_storage(std::move(other));
    }

public:
    WJR_CONSTEXPR20
    basic_ring_buffer(const basic_ring_buffer &other) noexcept(noexcept(basic_ring_buffer(
        other, _Alty_traits::select_on_container_copy_construction(other.__get_allocator()),
        in_place_empty)))
        : basic_ring_buffer(
              other, _Alty_traits::select_on_container_copy_construction(other.__get_allocator()),
              in_place_empty) {}

    WJR_CONSTEXPR20
    basic_ring_buffer(const basic_ring_buffer &other, const allocator_type &al) noexcept(
        noexcept(basic_ring_buffer(other, al, in_place_empty)))
        : basic_ring_buffer(other, al, in_place_empty) {}

    WJR_CONSTEXPR20
    basic_ring_buffer(basic_ring_buffer &&other) noexcept(noexcept(
        basic_ring_buffer(std::move(other), std::move(other.__get_allocator()), in_place_empty)))
        : basic_ring_buffer(std::move(other), std::move(other.__get_allocator()), in_place_empty) {}

    WJR_CONSTEXPR20
    basic_ring_buffer(basic_ring_buffer &&other, const allocator_type &al) noexcept(
        noexcept(basic_ring_buffer(std::move(other), al, in_place_empty)))
        : basic_ring_buffer(std::move(other), al, in_place_empty) {}

    template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    WJR_CONSTEXPR20 basic_ring_buffer(Iter first, Iter last,
                                      const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __range_construct(to_contiguous_address(first), to_contiguous_address(last),
                          iterator_category_t<Iter>());
    }

    WJR_CONSTEXPR20
    basic_ring_buffer(std::initializer_list<value_type> il,
                      const allocator_type &al = allocator_type())
        : basic_ring_buffer(il.begin(), il.end(), al) {}

    WJR_CONSTEXPR20 ~basic_ring_buffer() noexcept { __destroy_and_deallocate(); }

    //     WJR_CONSTEXPR20 basic_ring_buffer &operator=(const basic_ring_buffer
    //     &other) noexcept(
    //         noexcept(storage_fn_type::copy_assign(std::declval<basic_vector
    //         &>(),
    //   std::declval<const basic_vector &>()))) {
    //         if (WJR_LIKELY(this != std::addressof(other))) {
    //             storage_fn_type::copy_assign(*this, other);
    //         }

    //         return *this;
    //     }

    // WJR_CONSTEXPR20 basic_ring_buffer &operator=(basic_ring_buffer &&other)
    // noexcept(
    //     noexcept(storage_fn_type::move_assign(std::declval<basic_vector &>(),
    //   std::declval<basic_vector &&>()))) {
    //     WJR_ASSERT(this != std::addressof(other));
    //     storage_fn_type::move_assign(*this, std::move(other));
    //     return *this;
    // }

    //     WJR_CONSTEXPR20 basic_ring_buffer
    //     &operator=(std::initializer_list<value_type> il) {
    //         return assign(il);
    //     }

    //     WJR_CONSTEXPR20 basic_ring_buffer &assign(size_type n, const
    //     value_type &val) {
    //         __fill_assign(n, val);
    //         return *this;
    //     }

    //     template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    //     WJR_CONSTEXPR20 basic_ring_buffer &assign(Iter first, Iter last) {
    //         __range_assign(to_contiguous_address(first),
    //         to_contiguous_address(last),
    //                        iterator_category_t<Iter>());
    //         return *this;
    //     }

    //     WJR_CONSTEXPR20 basic_ring_buffer
    //     &assign(std::initializer_list<value_type> il)
    //     {
    //         return assign(il.begin(), il.end());
    //     }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept { return get_storage().size(); }

    //     WJR_CONSTEXPR20 void resize(const size_type new_size) {
    //     __resize(new_size, value_construct); }

    //     WJR_CONSTEXPR20 void resize(const size_type new_size, const
    //     value_type &val) {
    //         __resize(new_size, val);
    //     }

    //     /**
    //      * @todo designed shrink_to_fit for storage.
    //      */
    //     WJR_CONSTEXPR20 void shrink_to_fit() {
    //         if constexpr
    //         (has_ring_buffer_storage_shrink_to_fit_v<storage_type>) {
    //             get_storage().shrink_to_fit();
    //         } else if constexpr (is_reallocatable::value) {
    //             const size_type __size = size();
    //             if (__size == 0) {
    //                 __deallocate();
    //                 storage_type new_storage;
    //                 __take_storage(new_storage);
    //             } else if (__size < capacity()) {
    //                 auto &al = __get_allocator();

    //                 storage_type new_storage;
    //                 uninitialized_construct(new_storage, __size, __size);

    //                 WJR_ASSUME(data() != nullptr);
    //                 WJR_ASSUME(__size != 0);
    //                 uninitialized_relocate_n_restrict_using_allocator(data(),
    //                 __size,
    //                                                                   new_storage.data(),
    //                                                                   al);
    //                 __deallocate_nonnull();
    //                 __take_storage(new_storage);
    //             }
    //         }
    //     }

    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return get_storage().capacity();
    }

    WJR_PURE WJR_CONSTEXPR20 bool empty() const noexcept { return size() == 0; }

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
                uninitialized_construct(new_storage, 0, old_size, old_size, new_capacity);

                const pointer __head = __get_head();
                const pointer __tail = __get_tail();
                pointer dst = new_storage.data();

                if (__head < __tail) {
                    uninitialized_relocate_restrict_using_allocator(__head, __tail, dst, al);
                } else {
                    const pointer __data = data();
                    const pointer __buf_end = __get_buf_end();
                    if (__head == __tail) {
                        uninitialized_relocate_n_restrict_using_allocator(__data, size(), dst, al);
                    } else {
                        dst = uninitialized_relocate_restrict_using_allocator(__head, __buf_end,
                                                                              dst, al);
                        uninitialized_relocate_restrict_using_allocator(__data, __tail, dst, al);
                    }
                }

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
                uninitialized_construct(new_storage, 0, 0, 0, new_capacity);

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

    WJR_CONSTEXPR20 reference front() noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(!empty(), "basic_ring_buffer::front: empty");
#endif
        return *__get_head();
    }

    WJR_CONSTEXPR20 const_reference front() const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(!empty(), "basic_ring_buffer::front: empty");
#endif
        return *__get_head();
    }

    //     WJR_CONSTEXPR20 reference back() noexcept {
    // #if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
    //         WJR_ASSERT_L0(!empty(), "basic_ring_buffer::back: empty");
    // #endif
    //         return *(end_unsafe() - 1);
    //     }

    //     WJR_CONSTEXPR20 const_reference back() const noexcept {
    // #if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
    //         WJR_ASSERT_L0(!empty(), "basic_ring_buffer::back: empty");
    // #endif
    //         return *(end_unsafe() - 1);
    //     }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return get_storage().data(); }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept { return get_storage().data(); }
    WJR_PURE WJR_CONSTEXPR20 const_pointer cdata() const noexcept { return data(); }

    template <typename... Args>
    WJR_CONSTEXPR20 void emplace_back(Args &&...args) {
        const pointer __head = __get_head();
        pointer __tail = __get_tail();
        const pointer __buf_end = __get_buf_end();
        auto &al = __get_allocator();

        if (__head == __tail && WJR_UNLIKELY(__head == nullptr || size() != 0)) {
            __realloc_insert_at_end(std::forward<Args>(args)...);
        } else {
            wjr::uninitialized_construct_using_allocator(__tail, al, std::forward<Args>(args)...);
            ++__tail;
            __get_tail() = __tail == __buf_end ? data() : __tail;
            ++__get_size();
        }
    }

    WJR_CONSTEXPR20 void push_back(const value_type &val) { emplace_back(val); }
    WJR_CONSTEXPR20 void push_back(value_type &&val) { emplace_back(std::move(val)); }

    void pop_front() {
        WJR_ASSERT(!empty());
        pointer __head = __get_head();
        destroy_at_using_allocator(__head, __get_allocator());
        const pointer __buf_end = __get_buf_end();
        ++__head;
        __get_head() = __head == __buf_end ? data() : __head;
        --__get_size();
    }

    //     WJR_CONSTEXPR20 void pop_back() noexcept {
    //         const size_type __size = --__get_size();
    //         destroy_at_using_allocator(data() + __size, __get_allocator());
    //     }

    //     template <typename... Args>
    //     WJR_CONSTEXPR20 iterator emplace(const_iterator pos, Args &&...args)
    //     {
    //         return __emplace_aux(__get_pointer(pos),
    //         std::forward<Args>(args)...);
    //     }

    //     WJR_CONSTEXPR20 iterator insert(const_iterator pos, const value_type
    //     &val) {
    //         return emplace(pos, val);
    //     }

    //     WJR_CONSTEXPR20 iterator insert(const_iterator pos, value_type &&val)
    //     {
    //         return emplace(pos, std::move(val));
    //     }

    //     WJR_CONSTEXPR20 iterator insert(const_iterator pos,
    //     std::initializer_list<value_type> il)
    //     {
    //         return insert(pos, il.begin(), il.end());
    //     }

    //     WJR_CONSTEXPR20 iterator insert(const_iterator pos, size_type n,
    //     const value_type &val) {
    //         const auto old_pos = static_cast<size_type>(pos - cbegin());
    //         __fill_insert(data() + old_pos, n, val);
    //         return begin() + old_pos;
    //     }

    //     template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    //     WJR_CONSTEXPR20 iterator insert(const_iterator pos, Iter first, Iter
    //     last) {
    //         const auto old_pos = static_cast<size_type>(pos - cbegin());
    //         __range_insert(data() + old_pos, to_contiguous_address(first),
    //         to_contiguous_address(last),
    //                        iterator_category_t<Iter>());
    //         return begin() + old_pos;
    //     }

    //     WJR_CONSTEXPR20 iterator erase(const_iterator pos) noexcept(
    //         std::is_nothrow_move_assignable_v<value_type>) /* strengthened */
    //         { return __erase(__get_pointer(pos));
    //     }

    //     WJR_CONSTEXPR20 iterator erase(const_iterator first, const_iterator
    //     last) noexcept(
    //         std::is_nothrow_move_assignable_v<value_type>) /* strengthened */
    //         { return __erase(__get_pointer(first), __get_pointer(last));
    //     }

    WJR_CONSTEXPR20 void
    swap(basic_ring_buffer &other) noexcept(noexcept(storage_fn_type::swap(*this, other))) {
        storage_fn_type::swap(*this, other);
    }

    WJR_CONSTEXPR20 void clear() noexcept {
        const pointer __head = __get_head();
        const pointer __tail = __get_tail();
        auto &al = __get_allocator();
        if (__head < __tail) {
            destroy_using_allocator(__head, __tail, al);
        } else {
            const pointer __data = data();
            const pointer __buf_end = __get_buf_end();
            if (__head == __tail) {
                destroy_n_using_allocator(__data, size(), al);
                __get_size() = 0;
                return;
            } else {
                destroy_using_allocator(__head, __buf_end, al);
                destroy_using_allocator(__data, __tail, al);
            }
        }

        __get_tail() = __head;
        __get_size() = 0;
        WJR_ASSUME(empty());
    }

    WJR_CONSTEXPR20 allocator_type &get_allocator() noexcept { return __get_allocator(); }
    WJR_CONSTEXPR20 const allocator_type &get_allocator() const noexcept {
        return __get_allocator();
    }

    WJR_CONST static size_type max_size() noexcept { return std::numeric_limits<size_type>::max(); }

    // extension

    WJR_CONSTEXPR20
    basic_ring_buffer(size_type n, default_construct_t, const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __construct_n(n, default_construct);
    }

    WJR_CONSTEXPR20
    basic_ring_buffer(size_type n, in_place_reserve_t, const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        if (WJR_LIKELY(n != 0)) {
            uninitialized_construct(0, 0, 0, n);
        }
    }

    WJR_CONSTEXPR20
    basic_ring_buffer(storage_type &&other, const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        take_storage(other);
    }

    WJR_CONSTEXPR20 basic_ring_buffer &operator=(storage_type &&other) {
        if (std::addressof(get_storage()) == std::addressof(other)) {
            return *this;
        }

        take_storage(other);
        return *this;
    }

private:
    //     WJR_CONSTEXPR20 void __clear_if_reserved_impl(size_type n) {
    //         if constexpr (is_reallocatable::value) {
    //             const size_type old_capacity = capacity();
    //             if (WJR_UNLIKELY(old_capacity < n)) {
    //                 const size_type new_capacity =
    //                 get_growth_capacity(old_capacity, n);

    //                 storage_type new_storage;
    //                 uninitialized_construct(new_storage, 0, new_capacity);

    //                 __destroy_and_deallocate();
    //                 __take_storage(new_storage);

    //                 WJR_ASSUME(empty());
    //             }
    //         } else {
    //             WJR_ASSERT_ASSUME(n <= capacity());
    //         }
    //     }

public:
    //     /**
    //      * @brief clear() if capacity() < new_capacity.
    //      *
    //      * @details Useful when old data unused. If reserved, this function
    //      won't
    //      * move any old data to new pointer.
    //      */
    //     WJR_CONSTEXPR20 void clear_if_reserved(size_type n) {
    //         if WJR_BUILTIN_CONSTANT_CONSTEXPR
    //         (WJR_BUILTIN_CONSTANT_P_TRUE(empty())) {
    //             __empty_reserve_impl(n);
    //             return;
    //         }

    //         __clear_if_reserved_impl(n);
    //     }

    //     WJR_CONSTEXPR20 void resize(const size_type new_size,
    //     default_construct_t) {
    //         __resize(new_size, default_construct);
    //     }

    WJR_CONSTEXPR20 void push_back(default_construct_t) { emplace_back(default_construct); }

    WJR_CONSTEXPR20 basic_ring_buffer &append(const value_type &val) {
        emplace_back(val);
        return *this;
    }

    WJR_CONSTEXPR20 basic_ring_buffer &append(value_type &&val) {
        emplace_back(std::move(val));
        return *this;
    }

    WJR_CONSTEXPR20 basic_ring_buffer &append(default_construct_t) {
        emplace_back(default_construct);
        return *this;
    }

    //     WJR_CONSTEXPR20 basic_ring_buffer &append(const size_type n, const
    //     value_type &val) {
    //         __append(n, val);
    //         return *this;
    //     }

    //     WJR_CONSTEXPR20 basic_ring_buffer &append(const size_type n,
    //     default_construct_t) {
    //         __append(n, default_construct);
    //         return *this;
    //     }

    //     template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    //     WJR_CONSTEXPR20 basic_ring_buffer &append(Iter first, Iter last) {
    //         __range_append(to_contiguous_address(first),
    //         to_contiguous_address(last),
    //                        iterator_category_t<Iter>());
    //         return *this;
    //     }

    //     WJR_CONSTEXPR20 basic_ring_buffer
    //     &append(std::initializer_list<value_type> il)
    //     {
    //         return append(il.begin(), il.end());
    //     }

    //     /**
    //      * @brief Pop n elements from the end
    //      *
    //      */
    //     WJR_CONSTEXPR20 basic_ring_buffer &chop(const size_type n) {
    //         __erase_at_end(end_unsafe() - n);
    //         return *this;
    //     }

    //     /**
    //      * @brief Truncate the size to n
    //      *
    //      */
    //     WJR_CONSTEXPR20 basic_ring_buffer &truncate(const size_type n) {
    //     return chop(size() - n);
    //     }

    //     template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    //     WJR_CONSTEXPR20 basic_ring_buffer &replace(const_iterator from,
    //     const_iterator to, Iter first,
    //                                                Iter last) {
    //         __range_replace(__get_pointer(from), __get_pointer(to),
    //         to_contiguous_address(first),
    //                         to_contiguous_address(last),
    //                         iterator_category_t<Iter>());
    //         return *this;
    //     }

    //     WJR_CONSTEXPR20 basic_ring_buffer &replace(const_iterator from,
    //     const_iterator to,
    //                                                const size_type n, const
    //                                                value_type &val) {
    //         __fill_replace(__get_pointer(from), __get_pointer(to), n, val);
    //         return *this;
    //     }

    WJR_CONSTEXPR20 storage_type &get_storage() noexcept { return m_pair.second(); }

    WJR_CONSTEXPR20 const storage_type &get_storage() const noexcept { return m_pair.second(); }

    WJR_CONSTEXPR20 void take_storage(storage_type &other) noexcept {
        get_storage().take_storage(other, __get_allocator());
    }

    WJR_INTRINSIC_CONSTEXPR20 void
    uninitialized_construct(storage_type &other, size_type head, size_type tail, size_type size,
                            size_type cap) noexcept(__is_nothrow_uninitialized_construct) {
        WJR_ASSERT_ASSUME(cap != 0);
        get_storage().uninitialized_construct(other, head, tail, size, cap, __get_allocator());
    }

    WJR_INTRINSIC_CONSTEXPR20 void
    uninitialized_construct(size_type head, size_type tail, size_type size,
                            size_type cap) noexcept(__is_nothrow_uninitialized_construct) {
        WJR_ASSERT_ASSUME(cap != 0);
        if constexpr (has_ring_buffer_storage_uninitialized_construct_v<storage_type, size_type,
                                                                        _Alty>) {
            get_storage().uninitialized_construct(head, tail, size, cap, __get_allocator());
        } else {
            uninitialized_construct(get_storage(), head, tail, size, cap);
        }
    }

private:
    WJR_CONSTEXPR20 void __deallocate() noexcept(__is_nothrow_deallocate) {
        get_storage().deallocate(__get_allocator());
    }

    // member function for container_fn (START)

    WJR_CONSTEXPR20 _Alty &__get_allocator() noexcept { return m_pair.first(); }

    WJR_CONSTEXPR20 const _Alty &__get_allocator() const noexcept { return m_pair.first(); }

    WJR_CONSTEXPR20 void __destroy() noexcept(std::is_nothrow_destructible_v<value_type>) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(empty())) {
            return;
        }

        pointer __head = __get_head();
        pointer __tail = __get_tail();

        auto &al = __get_allocator();

        if (__head < __tail) {
            destroy_using_allocator(__head, __tail, al);
        } else {
            if (WJR_UNLIKELY(empty())) {
                return;
            }

            pointer __buf_end = __get_buf_end();
            destroy_using_allocator(__head, __buf_end, al);
            destroy_using_allocator(data(), __tail, al);
        }
    }

    WJR_CONSTEXPR20 void __destroy_and_deallocate() noexcept(noexcept(__destroy()) &&
                                                             __is_nothrow_deallocate) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(capacity() == 0) ||
                                           WJR_BUILTIN_CONSTANT_P_TRUE(__get_data() == nullptr)) {
            return;
        }

        __destroy();
        __deallocate();
    }

    //     WJR_CONSTEXPR20 void __copy_element(const basic_ring_buffer &other) {
    //         assign(other.begin_unsafe(), other.end_unsafe());
    //     }

    WJR_CONSTEXPR20 void
    __take_storage(basic_ring_buffer &&other) noexcept(__is_nothrow_take_storage) {
        __take_storage(other.get_storage());
    }

    // WJR_CONSTEXPR20 void __move_element(basic_ring_buffer &&other) {
    //     assign(std::make_move_iterator(other.begin_unsafe()),
    //            std::make_move_iterator(other.end_unsafe()));
    // }

    WJR_CONSTEXPR20 void
    __swap_storage(basic_ring_buffer &other) noexcept(__is_nothrow_swap_storage) {
        get_storage().swap_storage(other.get_storage(), __get_allocator());
    }

    //     // member function for container_fn (END)

    WJR_CONSTEXPR20 void __deallocate_nonnull() noexcept(__is_nothrow_deallocate_nonnull) {
        get_storage().deallocate_nonnull(__get_allocator());
    }

    WJR_PURE WJR_CONSTEXPR20 pointer __get_data() noexcept { return get_storage().data(); }
    WJR_PURE WJR_CONSTEXPR20 const_pointer __get_data() const noexcept {
        return get_storage().data();
    }

    WJR_PURE WJR_CONSTEXPR20 pointer &__get_head() noexcept { return get_storage().head(); }
    WJR_PURE WJR_CONSTEXPR20 pointer &__get_tail() noexcept { return get_storage().tail(); }
    WJR_PURE WJR_CONSTEXPR20 pointer __get_buf_end() noexcept { return get_storage().tail(); }
    WJR_PURE WJR_CONSTEXPR20 size_type &__get_size() noexcept { return get_storage().size(); }

    WJR_PURE WJR_CONSTEXPR20 const_pointer __get_head() const noexcept {
        return get_storage().head();
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer __get_tail() const noexcept {
        return get_storage().tail();
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer __get_buf_end() const noexcept {
        return get_storage().tail();
    }

    WJR_CONSTEXPR20 void __take_storage(storage_type &other) noexcept(__is_nothrow_take_storage) {
        take_storage(other);
    }

    WJR_NORETURN WJR_CONSTEXPR20 void
    __unreallocatable_unreachable(WJR_MAYBE_UNUSED size_type new_capacity) const {
        WJR_ASSERT(new_capacity <= capacity(),
                   "new_capacity must be less than or equal to capacity if the "
                   "storage is not reallocatable.\nnew_capacity = ",
                   new_capacity, ", capacity = ", capacity());
        WJR_UNREACHABLE();
    }

    template <typename Ty>
    WJR_CONSTEXPR20 void __construct_n(const size_type n, Ty &&val) {
        if (WJR_LIKELY(n != 0)) {
            auto &al = __get_allocator();
            uninitialized_construct(0, 0, n, n);
            wjr::uninitialized_fill_n_using_allocator(__get_data(), n, al, std::forward<Ty>(val));
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __construct_n(const size_type n, Iter first, Iter last) {
        if (WJR_LIKELY(n != 0)) {
            auto &al = __get_allocator();
            uninitialized_construct(0, 0, n, n);
            wjr::uninitialized_copy_restrict_using_allocator(first, last, __get_data(), al);
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

    //     WJR_CONSTEXPR20 iterator __erase(pointer pos) noexcept {
    //         const pointer __end = end_unsafe();
    //         if (pos + 1 != __end) {
    //             std::move(pos + 1, __end, pos);
    //         }

    //         destroy_at_using_allocator(__end - 1, __get_allocator());
    //         --__get_size();
    //         return __make_iterator(pos);
    //     }

    //     WJR_CONSTEXPR20 iterator __erase(pointer first, pointer last)
    //     noexcept {
    //         const pointer __end = end_unsafe();
    //         if (WJR_LIKELY(first != last)) {
    //             if (last != __end) {
    //                 std::move(last, __end, first);
    //             }

    //             __erase_at_end(__end - (last - first));
    //         }

    //         return __make_iterator(first);
    //     }

    //     template <typename Iter>
    //     WJR_CONSTEXPR20 void __range_insert(pointer pos, Iter first, Iter
    //     last,
    //                                         std::input_iterator_tag) {
    //         if (pos == end_unsafe()) {
    //             __range_append(first, last, std::input_iterator_tag());
    //         } else if (first != last) {
    //             basic_ring_buffer tmp(first, last, __get_allocator());
    //             __range_insert(pos, tmp.begin_unsafe(), tmp.end_unsafe(),
    //             std::forward_iterator_tag());
    //         }
    //     }

    //     template <typename Iter>
    //     WJR_CONSTEXPR20 void __range_insert(pointer pos, Iter first, Iter
    //     last,
    //                                         std::forward_iterator_tag) {
    //         if (WJR_UNLIKELY(first == last)) {
    //             return;
    //         }

    //         auto &al = __get_allocator();
    //         const pointer __begin = data();
    //         const pointer __end = end_unsafe();
    //         const pointer __buf_end = buf_end_unsafe();

    //         const auto n = static_cast<size_type>(std::distance(first,
    //         last)); const auto __rest = static_cast<size_type>(__buf_end -
    //         __end);

    //         if (WJR_LIKELY(__rest >= n)) {
    //             const auto __elements_after = static_cast<size_type>(__end -
    //             pos); if (__elements_after > n) {
    //                 wjr::uninitialized_move_n_restrict_using_allocator(__end
    //                 - n, n,
    //                 __end, al); std::move_backward(pos, __end - n, __end);
    //                 wjr::copy_restrict(first, last, pos);
    //             } else {
    //                 const auto mid = std::next(first, __elements_after);

    //                 wjr::uninitialized_copy_restrict_using_allocator(mid,
    //                 last, __end, al);
    //                 wjr::uninitialized_move_restrict_using_allocator(pos,
    //                 __end, pos + n, al); wjr::copy_restrict(first, mid, pos);
    //             }

    //             __get_size() += n;
    //         } else {
    //             if constexpr (is_reallocatable::value) {
    //                 const auto old_size = static_cast<size_type>(__end -
    //                 __begin); const auto old_pos = static_cast<size_type>(pos
    //                 - __begin); const size_type new_capacity =
    //                 get_growth_capacity(capacity(), old_size + n);

    //                 storage_type new_storage;
    //                 uninitialized_construct(new_storage, old_size + n,
    //                 new_capacity);

    //                 const pointer __new_begin = new_storage.data();

    //                 /// @todo this can be optimize by using relocate.

    //                 wjr::uninitialized_copy_restrict_using_allocator(first,
    //                 last,
    //                 __new_begin + old_pos,
    //                                                                  al);
    //                 wjr::uninitialized_move_restrict_using_allocator(__begin,
    //                 pos,
    //                 __new_begin, al);
    //                 wjr::uninitialized_move_restrict_using_allocator(pos,
    //                 __end,
    //                                                                  __new_begin
    //                                                                  +
    //                                                                  old_pos
    //                                                                  + n,
    //                                                                  al);

    //                 __destroy_and_deallocate();
    //                 __take_storage(new_storage);
    //             } else {
    //                 __unreallocatable_unreachable(size() + n);
    //             }
    //         }
    //     }

    //     template <typename Iter>
    //     WJR_CONSTEXPR20 void __range_append(Iter first, Iter last,
    //     std::input_iterator_tag) {
    //         for (; first != last; ++first) {
    //             emplace_back(*first);
    //         }
    //     }

    //     template <typename Iter>
    //     WJR_CONSTEXPR20 void __range_append(Iter first, Iter last,
    //     std::forward_iterator_tag) {
    //         if (WJR_UNLIKELY(first == last)) {
    //             return;
    //         }

    //         auto &al = __get_allocator();
    //         const pointer __begin = data();
    //         const pointer __end = end_unsafe();
    //         const pointer __buf_end = buf_end_unsafe();

    //         const auto n = static_cast<size_type>(std::distance(first,
    //         last)); const auto __rest = static_cast<size_type>(__buf_end -
    //         __end);

    //         if (WJR_LIKELY(__rest >= n)) {
    //             wjr::uninitialized_copy_n_restrict_using_allocator(first, n,
    //             __end, al);
    //             __get_size() += n;
    //         } else {
    //             if constexpr (is_reallocatable::value) {
    //                 const auto old_size = static_cast<size_type>(__end -
    //                 __begin); const size_type new_capacity =
    //                 get_growth_capacity(capacity(), old_size + n);

    //                 storage_type new_storage;
    //                 uninitialized_construct(new_storage, old_size + n,
    //                 new_capacity);

    //                 const pointer __new_begin = new_storage.data();

    //                 wjr::uninitialized_copy_restrict_using_allocator(first,
    //                 last,
    //                                                                  __new_begin
    //                                                                  +
    //                                                                  old_size,
    //                                                                  al);

    //                 uninitialized_relocate_restrict_using_allocator(__begin,
    //                 __end,
    //                 __new_begin, al);
    //                 __deallocate();

    //                 __take_storage(new_storage);
    //             } else {
    //                 __unreallocatable_unreachable(size() + n);
    //             }
    //         }
    //     }

    //     template <typename Iter>
    //     WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last,
    //     std::input_iterator_tag) {
    //         pointer cur = data();
    //         const pointer __end = end_unsafe();

    //         for (; first != last && cur != __end; ++first, ++cur) {
    //             *cur = *first;
    //         }

    //         if (first == last) {
    //             __erase_at_end(cur);
    //         } else {
    //             __range_append(first, last, std::input_iterator_tag());
    //         }
    //     }

    //     template <typename Iter>
    //     WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last,
    //     std::forward_iterator_tag) {
    //         auto n = static_cast<size_type>(std::distance(first, last));
    //         auto &al = __get_allocator();
    //         const pointer __begin = data();
    //         const pointer __end = end_unsafe();

    //         if (n <= size()) {
    //             wjr::copy_restrict(first, last, __begin);
    //             __erase_at_end(__begin + n);
    //         } else if (WJR_LIKELY(n <= capacity())) {
    //             auto mid = first;
    //             std::advance(mid, size());
    //             wjr::copy_restrict(first, mid, data());
    //             wjr::uninitialized_copy_restrict_using_allocator(mid, last,
    //             __end, al);
    //             __get_size() = n;
    //         } else {
    //             if constexpr (is_reallocatable::value) {
    //                 size_type new_capacity = get_growth_capacity(capacity(),
    //                 n);

    //                 storage_type new_storage;
    //                 uninitialized_construct(new_storage, n, new_capacity);

    //                 const pointer __new_begin = new_storage.data();
    //                 wjr::uninitialized_copy_n_restrict_using_allocator(first,
    //                 n,
    //                 __new_begin, al);

    //                 __destroy_and_deallocate();
    //                 __take_storage(new_storage);
    //             } else {
    //                 __unreallocatable_unreachable(n);
    //             }
    //         }
    //     }

    //     WJR_CONSTEXPR20 void __fill_assign(size_type n, const value_type
    //     &val) {
    //         auto &al = __get_allocator();

    //         if (WJR_UNLIKELY(n > capacity())) {
    //             if constexpr (is_reallocatable::value) {
    //                 __destroy_and_deallocate();

    //                 storage_type new_storage;
    //                 uninitialized_construct(new_storage, n, n);

    //                 wjr::uninitialized_fill_n_using_allocator(new_storage.data(),
    //                 n, al, val);
    //                 __take_storage(new_storage);
    //                 return;
    //             } else {
    //                 __unreallocatable_unreachable(n);
    //             }
    //         }

    //         if (n > size()) {
    //             std::fill(begin_unsafe(), end_unsafe(), val);
    //             wjr::uninitialized_fill_n_using_allocator(end_unsafe(), n -
    //             size(), al, val);
    //             __get_size() = n;
    //         } else {
    //             __erase_at_end(std::fill_n(begin_unsafe(), n, val));
    //         }
    //     }

    //     template <typename... Args>
    //     WJR_CONSTEXPR20 void __realloc_insert(pointer pos, Args &&...args) {
    //         if constexpr (is_reallocatable::value) {
    //             auto &al = __get_allocator();
    //             const pointer __begin = data();
    //             const pointer __end = end_unsafe();

    //             const auto old_pos_size = static_cast<size_type>(pos -
    //             __begin); const auto old_size = static_cast<size_type>(__end
    //             - __begin); const size_type new_size = old_size + 1; const
    //             size_type new_capacity = get_growth_capacity(old_size,
    //             new_size);

    //             storage_type new_storage;
    //             uninitialized_construct(new_storage, new_size, new_capacity);

    //             const pointer __new_begin = new_storage.data();
    //             const pointer new_pos = __new_begin + old_pos_size;

    //             wjr::uninitialized_construct_using_allocator(new_pos, al,
    //             std::forward<Args>(args)...);
    //             wjr::uninitialized_move_n_restrict_using_allocator(__begin,
    //             old_pos_size,
    //             __new_begin,
    //                                                                al);
    //             wjr::uninitialized_move_restrict_using_allocator(pos, __end,
    //             new_pos + 1, al);

    //             __destroy_and_deallocate();

    //             __take_storage(new_storage);
    //         } else {
    //             __unreallocatable_unreachable(size() + 1);
    //         }
    //     }

    template <typename... Args>
    WJR_CONSTEXPR20 void __realloc_insert_at_end(Args &&...args) {
        if constexpr (is_reallocatable::value) {
            auto &al = __get_allocator();
            const pointer __head = __get_head();
            const pointer __tail = __get_tail();

            const auto old_size = size();
            const auto new_size = old_size + 1;
            const size_type new_capacity = get_growth_capacity(old_size, new_size);

            storage_type new_storage;
            uninitialized_construct(new_storage, 0, 1, new_size, new_capacity);
            const pointer __new_begin = new_storage.data();

            const pointer new_pos = __new_begin + old_size;
            wjr::uninitialized_construct_using_allocator(new_pos, al, std::forward<Args>(args)...);

            uninitialized_relocate_restrict_using_allocator(__head, __get_buf_end(), __new_begin,
                                                            al);
            uninitialized_relocate_restrict_using_allocator(data(), __tail, __new_begin, al);
            __deallocate();

            __take_storage(new_storage);
        } else {
            __unreallocatable_unreachable(size() + 1);
        }
    }

    //     WJR_CONSTEXPR20 void __fill_insert(pointer pos, size_type n, const
    //     value_type &val) {
    //         if (WJR_UNLIKELY(n == 0)) {
    //             return;
    //         }

    //         auto &al = __get_allocator();
    //         const pointer __begin = data();
    //         const pointer __end = end_unsafe();
    //         const pointer __buf_end = buf_end_unsafe();

    //         const auto __rest = static_cast<size_type>(__buf_end - __end);

    //         if (WJR_LIKELY(__rest >= n)) {
    //             const temporary_value_allocator tmp(al, val);
    //             const auto &real_val = *tmp.get();

    //             const auto __elements_after = static_cast<size_type>(__end -
    //             pos); if (__elements_after > n) {
    //                 wjr::uninitialized_move_n_restrict_using_allocator(__end
    //                 - n, n,
    //                 __end, al); std::move_backward(pos, __end - n, __end);
    //                 std::fill_n(pos, n, real_val);
    //             } else {
    //                 wjr::uninitialized_fill_n_using_allocator(__end, n -
    //                 __elements_after, al,
    //                                                           real_val);
    //                 wjr::uninitialized_move_restrict_using_allocator(pos,
    //                 __end, pos + n, al); std::fill(pos, __end, real_val);
    //             }

    //             __get_size() += n;
    //         } else {
    //             const auto old_size = static_cast<size_type>(__end -
    //             __begin); if constexpr (is_reallocatable::value) {
    //                 const auto new_capacity = get_growth_capacity(capacity(),
    //                 old_size
    //                 + n);

    //                 storage_type new_storage;
    //                 uninitialized_construct(new_storage, old_size + n,
    //                 new_capacity);

    //                 const pointer __new_begin = new_storage.data();

    //                 const auto old_pos = static_cast<size_type>(pos -
    //                 __begin);

    //                 wjr::uninitialized_fill_n_using_allocator(__new_begin +
    //                 old_pos, n, al, val);
    //                 wjr::uninitialized_move_restrict_using_allocator(__begin,
    //                 pos, __new_begin, al);
    //                 wjr::uninitialized_move_restrict_using_allocator(pos,
    //                 __end,
    //                                                                  __new_begin
    //                                                                  +
    //                                                                  old_pos
    //                                                                  + n,
    //                                                                  al);

    //                 __destroy_and_deallocate();

    //                 __take_storage(new_storage);
    //             } else {
    //                 __unreallocatable_unreachable(old_size + n);
    //             }
    //         }
    //     }

    //     template <typename Ty>
    //     WJR_CONSTEXPR20 void __resize(const size_type new_size, const Ty
    //     &val) {
    //         const auto old_size = size();

    //         if constexpr (is_reallocatable::value) {
    //             if (new_size > old_size) {
    //                 __append(new_size - old_size, val);
    //             } else if (new_size < old_size) {
    //                 __erase_at_end(data() + new_size);
    //             }
    //         } else {
    //             auto &al = __get_allocator();

    //             const pointer __begin = data();
    //             const pointer __end = data() + old_size;

    //             if (WJR_UNLIKELY(new_size > capacity())) {
    //                 __unreallocatable_unreachable(new_size);
    //             }

    //             if (new_size > old_size) {
    //                 wjr::uninitialized_fill_n_using_allocator(__end, new_size
    //                 - old_size, al, val);
    //             } else if (new_size < old_size) {
    //                 destroy_using_allocator(__begin + new_size, __end, al);
    //             }

    //             __get_size() = new_size;
    //         }
    //     }

    //     template <typename Ty>
    //     WJR_CONSTEXPR20 void __append(size_type n, const Ty &val) {
    //         auto &al = __get_allocator();

    //         const auto old_size = size();
    //         const auto old_capacity = capacity();

    //         const pointer __begin = data();
    //         const pointer __end = __begin + old_size;

    //         const auto __rest = old_capacity - old_size;
    //         const auto new_size = old_size + n;

    //         if (WJR_LIKELY(__rest >= n)) {
    //             wjr::uninitialized_fill_n_using_allocator(__end, n, al, val);
    //             __get_size() = new_size;
    //         } else {
    //             if constexpr (is_reallocatable::value) {
    //                 auto new_capacity = get_growth_capacity(old_capacity,
    //                 new_size);

    //                 storage_type new_storage;
    //                 uninitialized_construct(new_storage, new_size,
    //                 new_capacity);

    //                 const pointer __new_begin = new_storage.data();

    //                 wjr::uninitialized_fill_n_using_allocator(__new_begin +
    //                 old_size, n, al, val);

    //                 uninitialized_relocate_restrict_using_allocator(__begin,
    //                 __end,
    //                 __new_begin, al);
    //                 __deallocate();

    //                 __take_storage(new_storage);
    //             } else {
    //                 __unreallocatable_unreachable(new_size);
    //             }
    //         }
    //     }

    //     template <typename Args>
    //     WJR_CONSTEXPR20 void __insert_aux(pointer pos, Args &&args) {
    //         auto &al = __get_allocator();
    //         const pointer __end = end_unsafe();

    //         wjr::uninitialized_construct_using_allocator(__end, al,
    //         std::move(*(__end - 1)));

    //         std::move_backward(pos, __end - 1, __end);
    //         *pos = std::forward<Args>(args);

    //         ++__get_size();
    //     }

    //     template <typename... Args>
    //     WJR_CONSTEXPR20 iterator __emplace_aux(pointer pos, Args &&...args) {
    //         auto &al = __get_allocator();
    //         const pointer __end = end_unsafe();
    //         const pointer __buf_end = buf_end_unsafe();

    //         if (WJR_LIKELY(__end != __buf_end)) {
    //             if (pos == __end) {
    //                 wjr::uninitialized_construct_using_allocator(__end, al,
    //                                                              std::forward<Args>(args)...);
    //                 ++__get_size();
    //             } else {
    //                 temporary_value_allocator tmp(al,
    //                 std::forward<Args>(args)...);
    //                 __insert_aux(pos, std::move(*tmp.get()));
    //             }

    //             return __make_iterator(pos);
    //         }

    //         const size_type offset = static_cast<size_type>(pos - data());
    //         __realloc_insert(pos, std::forward<Args>(args)...);
    //         return begin() + offset;
    //     }

    //     template <typename Iter>
    //     WJR_CONSTEXPR20 void __range_replace(pointer old_first, pointer
    //     old_last, Iter new_begin,
    //                                          Iter new_last,
    //                                          std::input_iterator_tag) {
    //         for (; old_first != old_last && new_begin != new_last;
    //         ++old_first,
    //         ++new_begin) {
    //             *old_first = *new_begin;
    //         }

    //         if (new_begin == new_last) {
    //             __erase_at_end(old_first, old_last);
    //         } else {
    //             __range_insert(old_last, new_begin, new_last,
    //             std::input_iterator_tag());
    //         }
    //     }

    //     template <typename Iter>
    //     WJR_CONSTEXPR20 void __range_replace(pointer old_first, pointer
    //     old_last, Iter new_begin,
    //                                          Iter new_last,
    //                                          std::forward_iterator_tag)
    //                                          {
    //         const auto n = static_cast<size_type>(old_last - old_first);
    //         const auto m = static_cast<size_type>(std::distance(new_begin,
    //         new_last));

    //         if (m <= n) {
    //             erase(std::copy_n(new_begin, m, old_first), old_last);
    //         } else {
    //             const auto __delta = m - n;

    //             auto &al = __get_allocator();
    //             const auto __begin = data();
    //             const auto __end = end_unsafe();
    //             const auto __buf_end = buf_end_unsafe();

    //             const auto __rest = static_cast<size_type>(__buf_end -
    //             __end);

    //             if (WJR_LIKLELY(__rest >= __delta)) {
    //                 const auto __elements_after =
    //                 static_cast<size_type>(__end - old_first); if
    //                 (__elements_after > m) {
    //                     wjr::uninitialized_move_using_allocator(__end -
    //                     __delta, __end,
    //                     __end, al); std::move_backward(old_last, __end -
    //                     __delta,
    //                     __end); std::copy(new_begin, new_last, old_first);
    //                 } else {
    //                     auto mid = new_begin;
    //                     std::advance(mid, __elements_after);
    //                     wjr::uninitialized_copy_using_allocator(mid,
    //                     new_last, __end, al);
    //                     wjr::uninitialized_move_using_allocator(old_last,
    //                     __end, old_first + m, al); std::copy(new_begin, mid,
    //                     old_first);
    //                 }
    //                 __get_size() += __delta;
    //             } else {
    //                 if constexpr (is_reallocatable::value) {
    //                     const auto old_size = static_cast<size_type>(__end -
    //                     __begin); const auto old_pos =
    //                     static_cast<size_type>(old_first -
    //                     __begin); const auto new_capacity =
    //                     get_growth_capacity(capacity(), old_size +
    //                     __delta);

    //                     storage_type new_storage;
    //                     uninitialized_construct(new_storage, old_size +
    //                     __delta, new_capacity);

    //                     const pointer __new_begin = new_storage.data();

    //                     wjr::uninitialized_copy_restrict_using_allocator(new_begin,
    //                     new_last,
    //                                                                      __new_begin
    //                                                                      +
    //                                                                      old_pos,
    //                                                                      al);
    //                     wjr::uninitialized_move_restrict_using_allocator(__begin,
    //                     old_first,
    //                                                                      __new_begin,
    //                                                                      al);
    //                     wjr::uninitialized_move_restrict_using_allocator(old_last,
    //                     __end,
    //                                                                      __new_begin
    //                                                                      +
    //                                                                      old_pos
    //                                                                      + m,
    //                                                                      al);

    //                     __destroy_and_deallocate();
    //                     __take_storage(new_storage);
    //                 } else {
    //                     __unreallocatable_unreachable(size() + __delta);
    //                 }
    //             }
    //         }
    //     }

    //     WJR_CONSTEXPR20 void __fill_replace(pointer old_first, pointer
    //     old_last, size_type m,
    //                                         const value_type &val) {
    //         const auto n = static_cast<size_type>(old_last - old_first);

    //         if (m <= n) {
    //             __erase(std::fill_n(old_first, m, val), old_last);
    //         } else {
    //             const auto __delta = m - n;

    //             auto &al = __get_allocator();
    //             const auto __begin = data();
    //             const auto __end = end_unsafe();
    //             const auto __buf_end = buf_end_unsafe();

    //             const auto __rest = static_cast<size_type>(__buf_end -
    //             __end);

    //             if (WJR_LIKELY(__rest >= __delta)) {
    //                 const temporary_value_allocator tmp(al, val);
    //                 const auto &real_value = *tmp.get();

    //                 const auto __elements_after =
    //                 static_cast<size_type>(__end - old_first); if
    //                 (__elements_after > m) {
    //                     wjr::uninitialized_move_using_allocator(__end -
    //                     __delta, __end,
    //                     __end, al); std::move_backward(old_last, __end -
    //                     __delta,
    //                     __end); std::fill_n(old_first, m, real_value);
    //                 } else {
    //                     wjr::uninitialized_fill_n_using_allocator(__end, m -
    //                     __elements_after, al,
    //                                                               real_value);
    //                     wjr::uninitialized_move_using_allocator(old_last,
    //                     __end, old_first + m, al); std::fill(old_first,
    //                     __end, real_value);
    //                 }
    //                 __get_size() += __delta;
    //             } else {
    //                 if constexpr (is_reallocatable::value) {
    //                     const auto old_size = static_cast<size_type>(__end -
    //                     __begin); const auto old_pos =
    //                     static_cast<size_type>(old_first -
    //                     __begin); const auto new_capacity =
    //                     get_growth_capacity(capacity(), old_size +
    //                     __delta);

    //                     storage_type new_storage;
    //                     uninitialized_construct(new_storage, old_size +
    //                     __delta, new_capacity);

    //                     const pointer __ptr = new_storage.data();

    //                     wjr::uninitialized_fill_n_using_allocator(__ptr +
    //                     old_pos, m, al, val);
    //                     wjr::uninitialized_move_restrict_using_allocator(__begin,
    //                     old_first,
    //                     __ptr, al);
    //                     wjr::uninitialized_move_restrict_using_allocator(old_last,
    //                     __end,
    //                                                                      __ptr
    //                                                                      +
    //                                                                      old_pos
    //                                                                      + m,
    //                                                                      al);

    //                     __destroy_and_deallocate();
    //                     __take_storage(new_storage);
    //                 } else {
    //                     __unreallocatable_unreachable(size() + __delta);
    //                 }
    //             }
    //         }
    //     }

private:
    compressed_pair<_Alty, storage_type> m_pair;
};

template <typename Iter, typename T = iterator_value_t<Iter>, typename Alloc = memory_pool<T>,
          WJR_REQUIRES(is_iterator_v<Iter>)>
basic_ring_buffer(Iter, Iter, Alloc = Alloc())
    -> basic_ring_buffer<default_ring_buffer_storage<T, Alloc>>;

template <typename S>
struct get_relocate_mode<basic_ring_buffer<S>> {
    static constexpr relocate_t value = basic_ring_buffer<S>::relocate_mode;
};

template <typename T, typename Alloc = memory_pool<T>>
using ring_buffer = basic_ring_buffer<default_ring_buffer_storage<T, Alloc>>;

/**
 * @brief A ring_buffer with fixed capacity by construction.
 *
 * @details Only allocate memory on construction and deallocation on
 * destruction. After construction, it cannot be expanded and can only be
 * modified through move assignment. For example, ring_buffer that using stack
 * allocator.
 */
template <typename T, typename Alloc = memory_pool<T>>
using fixed_ring_buffer = basic_ring_buffer<fixed_ring_buffer_storage<T, Alloc>>;

template <typename Storage>
void swap(basic_ring_buffer<Storage> &lhs, basic_ring_buffer<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

// template <typename Storage>
// WJR_PURE bool operator==(const basic_ring_buffer<Storage> &lhs,
//                          const basic_ring_buffer<Storage> &rhs) {
//     return std::equal(lhs.begin_unsafe(), lhs.end_unsafe(),
//     rhs.begin_unsafe(), rhs.end_unsafe());
// }

// template <typename Storage>
// WJR_PURE bool operator!=(const basic_ring_buffer<Storage> &lhs,
//                          const basic_ring_buffer<Storage> &rhs) {
//     return !(lhs == rhs);
// }

// template <typename Storage>
// WJR_PURE bool operator<(const basic_ring_buffer<Storage> &lhs,
//                         const basic_ring_buffer<Storage> &rhs) {
//     return std::lexicographical_compare(lhs.begin_unsafe(), lhs.end_unsafe(),
//     rhs.begin_unsafe(),
//                                         rhs.end_unsafe());
// }

// template <typename Storage>
// WJR_PURE bool operator>(const basic_ring_buffer<Storage> &lhs,
//                         const basic_ring_buffer<Storage> &rhs) {
//     return rhs < lhs;
// }

// template <typename Storage>
// WJR_PURE bool operator<=(const basic_ring_buffer<Storage> &lhs,
//                          const basic_ring_buffer<Storage> &rhs) {
//     return !(rhs < lhs);
// }

// template <typename Storage>
// WJR_PURE bool operator>=(const basic_ring_buffer<Storage> &lhs,
//                          const basic_ring_buffer<Storage> &rhs) {
//     return !(lhs < rhs);
// }

} // namespace wjr

namespace std {

template <typename Storage>
constexpr void swap(wjr::basic_ring_buffer<Storage> &lhs,
                    wjr::basic_ring_buffer<Storage> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace std

#endif // WJR_CONTAINER_CONTAINER_RING_BUFFER_HPP__