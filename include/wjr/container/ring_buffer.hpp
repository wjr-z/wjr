#ifndef WJR_CONTAINER_RING_BUFFER_HPP__
#define WJR_CONTAINER_RING_BUFFER_HPP__

/**
 * @file ring_buffer.hpp
 * @version 0.1
 * @date 2024-11-02
 *
 */

#include <wjr/compressed_pair.hpp>
#include <wjr/container/container_fn.hpp>
#include <wjr/container/detail/storage_traits.hpp>
#include <wjr/container/detail/vector_size_reference.hpp>
#include <wjr/iterator/contiguous_iterator_adapter.hpp>
#include <wjr/math/detail.hpp>
#include <wjr/memory/allocate_at_least.hpp>
#include <wjr/memory/copy.hpp>
#include <wjr/memory/memory_pool.hpp>
#include <wjr/memory/temporary_value_allocator.hpp>

#include <range/v3/view/subrange.hpp>

namespace wjr {

template <typename RB, typename Traits>
class ring_buffer_const_iterator {
    using __pointer = typename Traits::pointer;

    template <typename Ptr>
    friend struct std::pointer_traits;

public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename Traits::value_type;
    using difference_type = typename Traits::difference_type;
    using pointer = typename Traits::const_pointer;
    using reference = typename Traits::const_reference;

    ring_buffer_const_iterator(const RB *rb, __pointer ptr) noexcept
        : m_rb(const_cast<RB *>(rb)), m_ptr(ptr) {}

    WJR_NODISCARD WJR_CONSTEXPR20 pointer operator->() const noexcept {
        return const_cast<pointer>(m_ptr);
    }

    WJR_NODISCARD WJR_CONSTEXPR20 reference operator*() const noexcept { return *operator->(); }

    WJR_CONSTEXPR20 ring_buffer_const_iterator &operator++() noexcept {
        m_rb->increment(m_ptr);
        if (m_ptr == m_rb->__get_tail())
            m_ptr = nullptr;
        return *this;
    }

    WJR_CONSTEXPR20 ring_buffer_const_iterator operator++(int) noexcept {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    WJR_CONSTEXPR20 ring_buffer_const_iterator &operator--() noexcept {
        if (m_ptr == nullptr)
            m_ptr = m_rb->__get_tail();
        m_rb->decrement(m_ptr);
        return *this;
    }

    WJR_CONSTEXPR20 ring_buffer_const_iterator operator--(int) noexcept {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    WJR_CONSTEXPR20 ring_buffer_const_iterator &operator+=(difference_type n) noexcept {
        if (n > 0)
            __next(n);
        else if (n < 0)
            __prev(-n);
        return *this;
    }

    WJR_CONSTEXPR20 ring_buffer_const_iterator operator+(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp += n;
    }

    WJR_NODISCARD friend WJR_CONSTEXPR20 ring_buffer_const_iterator
    operator+(difference_type n, const ring_buffer_const_iterator &rhs) noexcept {
        return rhs + n;
    }

    WJR_CONSTEXPR20 ring_buffer_const_iterator &operator-=(difference_type n) noexcept {
        if (n > 0)
            __prev(n);
        else if (n < 0)
            __next(-n);
        return *this;
    }

    WJR_NODISCARD WJR_CONSTEXPR20 ring_buffer_const_iterator
    operator-(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp -= n;
    }

    WJR_NODISCARD WJR_CONSTEXPR20 reference operator[](difference_type n) const noexcept {
        return *(*this + n);
    }

    WJR_NODISCARD WJR_PURE WJR_CONSTEXPR20 difference_type
    operator-(const ring_buffer_const_iterator &rhs) const noexcept {
        WJR_ASSERT_L2(m_rb == rhs.m_rb, "can't subtract iterators from different ring_buffer");
        return linearize_pointer(m_ptr) - linearize_pointer(rhs.m_ptr);
    }

    WJR_NODISCARD WJR_PURE WJR_CONSTEXPR20 bool
    operator==(const ring_buffer_const_iterator &rhs) const noexcept {
        WJR_ASSERT_L2(m_rb == rhs.m_rb, "can't compare iterators from different ring_buffer");
        return m_ptr == rhs.m_ptr;
    }

    WJR_NODISCARD WJR_PURE WJR_CONSTEXPR20 bool
    operator!=(const ring_buffer_const_iterator &rhs) const noexcept {
        return !(*this == rhs);
    }

private:
    void __next(difference_type n) noexcept {
        m_ptr = m_rb->__next(m_ptr, n);
        if (m_ptr == m_rb->__get_tail())
            m_ptr = nullptr;
    }

    void __prev(difference_type n) noexcept {
        m_ptr = m_rb->__prev(m_ptr == nullptr ? m_rb->__get_tail() : m_ptr, n);
    }

    __pointer linearize_pointer(__pointer it) const {
        return it == nullptr
                   ? m_rb->data() + m_rb->size()
                   : (it < m_rb->__get_head() ? it + (m_rb->__get_buf_end() - m_rb->__get_head())
                                              : m_rb->data() + (it - m_rb->__get_head()));
    }

    RB *m_rb = nullptr;
    __pointer m_ptr = nullptr;
};

template <typename RB, typename Traits>
class ring_buffer_iterator : public ring_buffer_const_iterator<RB, Traits> {
    using Mybase = ring_buffer_const_iterator<RB, Traits>;
    using __pointer = typename Traits::pointer;

    template <typename Ptr>
    friend struct std::pointer_traits;

public:
    using iterator_category = typename Mybase::iterator_category;
    using value_type = typename Mybase::value_type;
    using difference_type = typename Mybase::difference_type;
    using pointer = typename Traits::pointer;
    using reference = typename Traits::reference;

    using Mybase::Mybase;

    WJR_NODISCARD WJR_CONSTEXPR20 pointer operator->() const noexcept {
        return const_cast<pointer>(Mybase::operator->());
    }

    WJR_NODISCARD WJR_CONSTEXPR20 reference operator*() const noexcept { return *operator->(); }

    WJR_CONSTEXPR20 ring_buffer_iterator &operator++() noexcept {
        Mybase::operator++();
        return *this;
    }

    WJR_CONSTEXPR20 ring_buffer_iterator operator++(int) noexcept {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    WJR_CONSTEXPR20 ring_buffer_iterator &operator--() noexcept {
        Mybase::operator--();
        return *this;
    }

    WJR_CONSTEXPR20 ring_buffer_iterator operator--(int) noexcept {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    WJR_CONSTEXPR20 ring_buffer_iterator &operator+=(difference_type n) noexcept {
        Mybase::operator+=(n);
        return *this;
    }

    WJR_NODISCARD WJR_CONSTEXPR20 ring_buffer_iterator operator+(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp += n;
    }

    WJR_NODISCARD friend WJR_CONSTEXPR20 ring_buffer_iterator
    operator+(difference_type n, const ring_buffer_iterator &rhs) noexcept {
        return rhs + n;
    }

    WJR_CONSTEXPR20 ring_buffer_iterator &operator-=(difference_type n) noexcept {
        Mybase::operator-=(n);
        return *this;
    }

    WJR_NODISCARD WJR_CONSTEXPR20 ring_buffer_iterator operator-(difference_type n) const noexcept {
        auto tmp = *this;
        return tmp -= n;
    }

    using Mybase::operator-;

    WJR_NODISCARD WJR_CONSTEXPR20 reference operator[](difference_type n) const noexcept {
        return *(*this + n);
    }
};

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
    using size_ref = default_vector_size_reference<pointer, size_type>;

public:
    __default_ring_buffer_storage() = default;

    __default_ring_buffer_storage(__default_ring_buffer_storage &&) = delete;
    __default_ring_buffer_storage(const __default_ring_buffer_storage &) = delete;
    __default_ring_buffer_storage &operator=(const __default_ring_buffer_storage &) = delete;
    __default_ring_buffer_storage &operator=(__default_ring_buffer_storage &&) = delete;

    ~__default_ring_buffer_storage() = default;

    WJR_CONSTEXPR20 void deallocate_nonnull(_Alty &al) noexcept(
        noexcept(_Alty_traits::deallocate(al, this->m_storage.m_data, this->capacity()))) {
        WJR_ASSERT_ASSUME_L2(m_storage.m_data != nullptr);
        WJR_ASSERT_ASSUME_L2(capacity() != 0);
        _Alty_traits::deallocate(al, m_storage.m_data, capacity());
    }

    WJR_CONSTEXPR20 void deallocate(_Alty &al) noexcept(
        noexcept(_Alty_traits::deallocate(al, this->m_storage.m_data, this->capacity()))) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(data() == nullptr)) {
            return;
        }

        if (WJR_LIKELY(m_storage.m_data != nullptr)) {
            deallocate_nonnull(al);
        }
    }

    WJR_INTRINSIC_CONSTEXPR20 static void
    uninitialized_construct(Mybase &other, size_type size, size_type capacity,
                            _Alty &al) noexcept(noexcept(allocate_at_least(al, capacity))) {
        const auto ptr = _Alty_traits::allocate(al, capacity);
        other.m_storage = {ptr, ptr, ptr + (size == capacity ? 0 : size), size, ptr + capacity};
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
    WJR_PURE WJR_CONSTEXPR20 pointer buffer() noexcept { return m_storage.m_buf_end; }

    WJR_PURE WJR_CONSTEXPR20 const_pointer head() const noexcept { return m_storage.m_head; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer tail() const noexcept { return m_storage.m_tail; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer buffer() const noexcept { return m_storage.m_buf_end; }

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

/**
 * @brief Customized ring_buffer by storage.
 *
 * @details Type of pointer is same as iterator.
 *
 */
template <typename Storage>
class basic_ring_buffer {
    template <typename RB, typename Traits>
    friend class ring_buffer_const_iterator;

    template <typename RB, typename Traits>
    friend class ring_buffer_iterator;

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

    using IteratorTraits = storage_traits<storage_type>;

public:
    static_assert(std::is_same_v<typename _Alty_traits::value_type, value_type>,
                  "allocator_type::value_type must be the same as value_type");

    using size_type = typename storage_type::size_type;
    using difference_type = typename storage_type::difference_type;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = typename Storage::pointer;
    using const_pointer = typename Storage::const_pointer;
    using iterator = ring_buffer_iterator<basic_ring_buffer, IteratorTraits>;
    using const_iterator = ring_buffer_const_iterator<basic_ring_buffer, IteratorTraits>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using storage_traits_type = typename storage_type::storage_traits_type;
    using is_trivially_contiguous = typename storage_traits_type::is_trivially_contiguous;
    using is_reallocatable = typename storage_type::is_reallocatable;

    using self_type = basic_ring_buffer<storage_type>;
    using array_range = ranges::subrange<pointer>;
    using const_array_range = ranges::subrange<const_pointer>;

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
            std::declval<_Alty &>()));
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
                                                                       const allocator_type &> &&
                                       noexcept(__construct_n(n, value_construct)))
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        __construct_n(n, value_construct);
    }

    WJR_CONSTEXPR20
    basic_ring_buffer(
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
    WJR_CONSTEXPR20
    basic_ring_buffer(const basic_ring_buffer &other, _Alloc &&al, in_place_empty_t) noexcept(
        std::is_nothrow_constructible_v<_Alty, _Alloc &&> && __is_nothrow_uninitialized_construct &&
        std::is_nothrow_copy_constructible_v<value_type>)
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        const size_type __size = other.size();
        if (WJR_LIKELY(__size != 0)) {
            uninitialized_construct(__size, __size);

            pointer dst = data();
            const const_pointer __head = other.__get_head();
            const const_pointer __tail = other.__get_tail();

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
        std::is_nothrow_constructible_v<_Alty, _Alloc &&> && __is_nothrow_take_storage)
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
        __range_construct(wjr::__iter_base(first), wjr::__iter_base(last),
                          iterator_category_t<Iter>());
    }

    WJR_CONSTEXPR20
    basic_ring_buffer(std::initializer_list<value_type> il,
                      const allocator_type &al = allocator_type())
        : basic_ring_buffer(il.begin(), il.end(), al) {}

    WJR_CONSTEXPR20 ~basic_ring_buffer() noexcept { __destroy_and_deallocate(); }

    basic_ring_buffer &operator=(const basic_ring_buffer &other) noexcept(
        noexcept(storage_fn_type::copy_assign(std::declval<self_type &>(),
                                              std::declval<const self_type &>()))) {
        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_fn_type::copy_assign(*this, other);
        }

        return *this;
    }

    basic_ring_buffer &operator=(basic_ring_buffer &&other) noexcept(noexcept(
        storage_fn_type::move_assign(std::declval<self_type &>(), std::declval<self_type &&>()))) {
        WJR_ASSERT(this != std::addressof(other));
        storage_fn_type::move_assign(*this, std::move(other));
        return *this;
    }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept { return get_storage().size(); }

    /**
     * @todo designed shrink_to_fit for storage.
     */
    WJR_CONSTEXPR20 void shrink_to_fit() {
        if constexpr (has_ring_buffer_storage_shrink_to_fit_v<storage_type>) {
            get_storage().shrink_to_fit();
        } else if constexpr (is_reallocatable::value) {
            const size_type __size = size();
            if (__size == 0) {
                __deallocate();
                storage_type new_storage;
                __take_storage(new_storage);
            } else if (__size < capacity()) {
                storage_type new_storage;
                uninitialized_construct(new_storage, __size, __size);

                pointer dst = data();
                const const_pointer __head = __get_head();
                const const_pointer __tail = __get_tail();

                auto &__al = __get_allocator();

                if (__head < __tail) {
                    uninitialized_relocate_restrict_using_allocator(__head, __tail, dst, __al);
                } else {
                    const_pointer __data = data();
                    const_pointer __buf_end = __get_buf_end();
                    dst = uninitialized_relocate_restrict_using_allocator(__head, __buf_end, dst,
                                                                          __al);
                    uninitialized_relocate_restrict_using_allocator(__data, __tail, dst, __al);
                }

                __deallocate_nonnull();
                __take_storage(new_storage);
            }
        }
    }

    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return get_storage().capacity();
    }

    WJR_PURE WJR_CONSTEXPR20 bool empty() const noexcept { return size() == 0; }
    WJR_PURE WJR_CONSTEXPR20 bool full() const noexcept { return size() == capacity(); }

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
                WJR_ASSUME(old_size != new_capacity);
                uninitialized_construct(new_storage, old_size, new_capacity);

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

private:
    WJR_PURE WJR_CONSTEXPR20 iterator __make_iterator(const_pointer ptr) const noexcept {
        return iterator(this, const_cast<pointer>(ptr));
    }

public:
    WJR_CONSTEXPR20 iterator begin() noexcept {
        return __make_iterator(empty() ? nullptr : __get_head());
    }

    WJR_CONSTEXPR20 const_iterator begin() const noexcept {
        return __make_iterator(empty() ? nullptr : __get_head());
    }

    WJR_CONSTEXPR20 const_iterator cbegin() const noexcept { return begin(); }

    WJR_CONSTEXPR20 iterator end() noexcept { return __make_iterator(nullptr); }
    WJR_CONSTEXPR20 const_iterator end() const noexcept { return __make_iterator(nullptr); }
    WJR_CONSTEXPR20 const_iterator cend() const noexcept { return end(); }

    WJR_CONSTEXPR20 reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    WJR_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    WJR_CONSTEXPR20 const_reverse_iterator rcbegin() const noexcept { return rbegin(); }

    WJR_CONSTEXPR20 reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    WJR_CONSTEXPR20 const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    WJR_CONSTEXPR20 const_reverse_iterator rcend() const noexcept { return rend(); }

    WJR_CONSTEXPR20 reference operator[](size_type n) noexcept { return *__next(__get_head(), n); }

    WJR_CONSTEXPR20 reference front() noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(!empty(), "basic_ring_buffer::front: empty");
#endif
        return *__get_head();
    }

    WJR_CONSTEXPR20 const_reference front() const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(!empty(), "basic_ring_buffer::front: empty");
#endif
        return *__get_head();
    }

    WJR_CONSTEXPR20 reference back() noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(!empty(), "basic_ring_buffer::back: empty");
#endif
        pointer __tail = __get_tail();
        __tail = __tail == data() ? __get_buf_end() : __tail;
        return *--__tail;
    }

    WJR_CONSTEXPR20 const_reference back() const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(!empty(), "basic_ring_buffer::back: empty");
#endif
        const_pointer __tail = __get_tail();
        __tail = __tail == data() ? __get_buf_end() : __tail;
        return *--__tail;
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return get_storage().data(); }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept { return get_storage().data(); }
    WJR_PURE WJR_CONSTEXPR20 const_pointer cdata() const noexcept { return data(); }

    template <typename... Args>
    WJR_CONSTEXPR20 void emplace_back(Args &&...args) {
        if (WJR_LIKELY(!full())) {
            pointer &__tail = __get_tail();
            auto &al = __get_allocator();

            wjr::uninitialized_construct_using_allocator(__tail, al, std::forward<Args>(args)...);
            increment(__tail);
            ++__get_size();
        } else {
            __realloc_insert_at_end(std::forward<Args>(args)...);
        }
    }

    WJR_CONSTEXPR20 void push_back(const value_type &val) { emplace_back(val); }
    WJR_CONSTEXPR20 void push_back(value_type &&val) { emplace_back(std::move(val)); }

    void pop_front() {
        WJR_ASSERT(!empty());
        pointer &__head = __get_head();
        destroy_at_using_allocator(__head, __get_allocator());
        increment(__head);
        --__get_size();
    }

    WJR_CONSTEXPR20 void pop_back() noexcept {
        WJR_ASSERT(!empty());
        pointer &__tail = __get_tail();
        decrement(__tail);
        destroy_at_using_allocator(__tail, __get_allocator());
        --__get_size();
    }

    WJR_CONSTEXPR20 void
    swap(basic_ring_buffer &other) noexcept(noexcept(storage_fn_type::swap(*this, other))) {
        storage_fn_type::swap(*this, other);
    }

    WJR_CONSTEXPR20 void clear() noexcept {
        __destroy();
        __get_tail() = __get_head();
        __get_size() = 0;
    }

    WJR_CONSTEXPR20 allocator_type &get_allocator() noexcept { return __get_allocator(); }
    WJR_CONSTEXPR20 const allocator_type &get_allocator() const noexcept {
        return __get_allocator();
    }

    WJR_CONST static size_type max_size() noexcept { return std::numeric_limits<size_type>::max(); }

    template <typename Iter, WJR_REQUIRES(is_iterator_v<Iter>)>
    WJR_CONSTEXPR20 self_type &assign(Iter first, Iter last) {
        __range_assign(wjr::__iter_base(first), wjr::__iter_base(last),
                       iterator_category_t<Iter>());
        return *this;
    }

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
            uninitialized_construct(0, n);
        }
    }

    WJR_CONSTEXPR20
    basic_ring_buffer(storage_type &&other, const allocator_type &al = allocator_type())
        : m_pair(std::piecewise_construct, wjr::forward_as_tuple(al), wjr::forward_as_tuple()) {
        take_storage(other);
    }

    WJR_CONSTEXPR20 self_type &operator=(storage_type &&other) {
        if (std::addressof(get_storage()) == std::addressof(other)) {
            return *this;
        }

        take_storage(other);
        return *this;
    }

    WJR_CONSTEXPR20 array_range array_one() noexcept {
        const auto __head = __get_head();
        const auto __tail = __get_tail();
        return array_range(__head, (__tail <= __head && !empty()) ? __get_buf_end() : __tail);
    }

    WJR_CONSTEXPR20 array_range array_two() noexcept {
        const auto __head = __get_head();
        const auto __tail = __get_tail();
        return array_range(data(), (__tail > __head || empty()) ? data() : __tail);
    }

    WJR_CONSTEXPR20 const_array_range array_one() const noexcept {
        return const_cast<self_type *>(this)->array_one();
    }

    WJR_CONSTEXPR20 const_array_range array_two() const noexcept {
        return const_cast<self_type *>(this)->array_two();
    }

private:
    WJR_CONSTEXPR20 void __clear_if_reserved_impl(size_type n) {
        if constexpr (is_reallocatable::value) {
            const size_type old_capacity = capacity();
            if (WJR_UNLIKELY(old_capacity < n)) {
                const size_type old_size = size();
                const size_type new_capacity = get_growth_capacity(old_capacity, n);

                storage_type new_storage;
                WJR_ASSUME(old_size != new_capacity);
                uninitialized_construct(new_storage, old_size, new_capacity);

                __destroy_and_deallocate();
                __take_storage(new_storage);
            }
        } else {
            WJR_ASSERT_ASSUME(n <= capacity());
        }
    }

public:
    /**
     * @brief clear() if capacity() < new_capacity.
     *
     * @details Useful when old data unused. If reserved, this function
     won't
     * move any old data to new pointer.
     */
    WJR_CONSTEXPR20 void clear_if_reserved(size_type n) {
        if WJR_BUILTIN_CONSTANT_CONSTEXPR (WJR_BUILTIN_CONSTANT_P_TRUE(empty())) {
            __empty_reserve_impl(n);
            return;
        }

        __clear_if_reserved_impl(n);
    }

    WJR_CONSTEXPR20 void push_back(default_construct_t) { emplace_back(default_construct); }

    WJR_CONSTEXPR20 self_type &append(const value_type &val) {
        emplace_back(val);
        return *this;
    }

    WJR_CONSTEXPR20 self_type &append(value_type &&val) {
        emplace_back(std::move(val));
        return *this;
    }

    WJR_CONSTEXPR20 self_type &append(default_construct_t) {
        emplace_back(default_construct);
        return *this;
    }

    WJR_CONSTEXPR20 storage_type &get_storage() noexcept { return m_pair.second(); }

    WJR_CONSTEXPR20 const storage_type &get_storage() const noexcept { return m_pair.second(); }

    WJR_CONSTEXPR20 void take_storage(storage_type &other) noexcept {
        get_storage().take_storage(other, __get_allocator());
    }

    WJR_INTRINSIC_CONSTEXPR20 void
    uninitialized_construct(storage_type &other, size_type size,
                            size_type cap) noexcept(__is_nothrow_uninitialized_construct) {
        WJR_ASSERT_ASSUME(cap != 0);
        get_storage().uninitialized_construct(other, size, cap, __get_allocator());
    }

    WJR_INTRINSIC_CONSTEXPR20 void
    uninitialized_construct(size_type size,
                            size_type cap) noexcept(__is_nothrow_uninitialized_construct) {
        WJR_ASSERT_ASSUME(cap != 0);
        uninitialized_construct(get_storage(), size, cap);
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
            if (__head == __tail) {
                destroy_n_using_allocator(data(), size(), al);
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
                                           WJR_BUILTIN_CONSTANT_P_TRUE(data() == nullptr)) {
            return;
        }

        __destroy();
        __deallocate();
    }

    WJR_CONSTEXPR20 void __release_before_copy() noexcept(noexcept(__destroy_and_deallocate())) {
        __destroy_and_deallocate();
        storage_type new_storage;
        __take_storage(new_storage);
    }

    WJR_CONSTEXPR20 void __copy_element(const basic_ring_buffer &other) {
        assign(other.begin(), other.end());
    }

    WJR_CONSTEXPR20 void
    __take_storage(basic_ring_buffer &&other) noexcept(__is_nothrow_take_storage) {
        __take_storage(other.get_storage());
    }

    WJR_CONSTEXPR20 void __destroy_and_move_element(basic_ring_buffer &&other) {
        // clear but not deallocate
        clear();
        assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
    }

    WJR_CONSTEXPR20 void
    __swap_storage(basic_ring_buffer &other) noexcept(__is_nothrow_swap_storage) {
        get_storage().swap_storage(other.get_storage(), __get_allocator());
    }

    // member function for container_fn (END)

    WJR_CONSTEXPR20 void __deallocate_nonnull() noexcept(__is_nothrow_deallocate_nonnull) {
        get_storage().deallocate_nonnull(__get_allocator());
    }

    template <typename Ptr>
    WJR_INTRINSIC_CONSTEXPR20 void increment(Ptr &it) const noexcept {
        if (WJR_LIKELY(++it != __get_buf_end())) {
            return;
        }

        it = const_cast<Ptr>(data());
    }

    template <typename Ptr>
    WJR_INTRINSIC_CONSTEXPR20 void decrement(Ptr &it) const noexcept {
        if (WJR_UNLIKELY(it == data())) {
            it = const_cast<Ptr>(__get_buf_end());
        }

        --it;
    }

    template <typename Ptr>
    WJR_INTRINSIC_CONSTEXPR20 Ptr __next(Ptr it, difference_type n) const noexcept {
        WJR_ASSUME(n >= 0);
        const auto dist = __get_buf_end() - it;
        return n < dist ? (it + n) : (const_cast<Ptr>(data()) + (n - dist));
    }

    template <typename Ptr>
    WJR_INTRINSIC_CONSTEXPR20 Ptr __prev(Ptr it, difference_type n) const noexcept {
        WJR_ASSUME(n > 0);
        const auto dist = it - data();
        return n <= dist ? (it - n) : (const_cast<Ptr>(__get_buf_end()) - (n - dist));
    }

    WJR_PURE WJR_CONSTEXPR20 pointer &__get_head() noexcept { return get_storage().head(); }
    WJR_PURE WJR_CONSTEXPR20 pointer &__get_tail() noexcept { return get_storage().tail(); }
    WJR_PURE WJR_CONSTEXPR20 pointer __get_buf_end() noexcept { return get_storage().buffer(); }
    WJR_PURE WJR_CONSTEXPR20 size_type &__get_size() noexcept { return get_storage().size(); }

    WJR_PURE WJR_CONSTEXPR20 const_pointer __get_head() const noexcept {
        return get_storage().head();
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer __get_tail() const noexcept {
        return get_storage().tail();
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer __get_buf_end() const noexcept {
        return get_storage().buffer();
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
            uninitialized_construct(new_storage, new_size, new_capacity);
            pointer __new_begin = new_storage.data();

            const pointer new_pos = __new_begin + old_size;
            wjr::uninitialized_construct_using_allocator(new_pos, al, std::forward<Args>(args)...);

            if (__head < __tail) {
                uninitialized_relocate_restrict_using_allocator(__head, __tail, __new_begin, al);
            } else if (!empty()) {
                __new_begin = uninitialized_relocate_restrict_using_allocator(
                    __head, __get_buf_end(), __new_begin, al);
                uninitialized_relocate_restrict_using_allocator(data(), __tail, __new_begin, al);
            }

            __deallocate();
            __take_storage(new_storage);
        } else {
            __unreallocatable_unreachable(size() + 1);
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last, std::input_iterator_tag) {
        // For input iterators, process elements one by one
        auto current_iter = first;
        const size_type old_size = size();
        size_type assigned_count = 0;

        // First phase: assign to existing elements using iterators
        auto it = begin();
        auto end_it = end();

        while (current_iter != last && it != end_it) {
            *it = *current_iter;
            ++current_iter;
            ++it;
            ++assigned_count;
        }

        if (current_iter == last) {
            // We have fewer new elements than existing ones
            // Need to erase the remaining elements
            if (assigned_count < old_size) {
                // Destroy remaining elements and update size/tail
                auto &al = __get_allocator();
                const pointer __head = __get_head();
                const pointer __tail = __get_tail();

                if (__head < __tail) {
                    // Simple contiguous case
                    destroy_using_allocator(__head + assigned_count, __tail, al);
                } else if (assigned_count > 0) {
                    // Wrap-around case
                    const auto first_segment_size = __get_buf_end() - __head;
                    if (assigned_count <= first_segment_size) {
                        // Destroy remaining in first segment and all in second segment
                        destroy_using_allocator(__head + assigned_count, __get_buf_end(), al);
                        destroy_using_allocator(data(), __tail, al);
                    } else {
                        // Only destroy remaining in second segment
                        const auto second_destroy_start = assigned_count - first_segment_size;
                        destroy_using_allocator(data() + second_destroy_start, __tail, al);
                    }
                }

                // Update tail and size
                __get_tail() = __next(__head, assigned_count);
                __get_size() = assigned_count;
            }
        } else {
            // We have more new elements than existing ones
            // Use emplace_back for the remaining elements
            while (current_iter != last) {
                emplace_back(*current_iter);
                ++current_iter;
            }
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last, std::forward_iterator_tag) {
        const auto n = static_cast<size_type>(std::distance(first, last));
        auto &al = __get_allocator();
        const pointer __head = __get_head();
        const pointer __tail = __get_tail();

        if (n <= size()) {
            const auto __one = static_cast<size_type>(__get_buf_end() - __head);
            const auto n1 = n <= __one ? n : __one;

            wjr::copy_n_restrict(first, n1, __head);
            destroy_using_allocator(__head + n1, __head < __tail ? __tail : __get_buf_end(), al);

            if (n <= __one) {
                __get_tail() = __head + n;
            } else {
                const auto n2 = n - n1;
                wjr::copy_restrict(first + n1, first + n, data());
                destroy_using_allocator(data() + n2, __tail, al);
                __get_tail() = data() + n2;
            }

            __get_size() = n;
        } else if (WJR_LIKELY(n <= capacity())) {
            // n > size() but n <= capacity()
            // Need to assign to existing elements and construct new ones
            const auto old_size = size();
            const auto __one = static_cast<size_type>(__get_buf_end() - __head);
            const auto old_size_one = old_size <= __one ? old_size : __one;

            // First, assign to existing elements in first segment
            auto iter = first;
            wjr::copy_restrict(iter, iter + old_size_one, __head);
            iter += old_size_one;

            if (old_size <= __one) {
                // All existing elements are in first segment
                // Construct new elements in first segment
                const auto new_in_first = n <= __one ? (n - old_size) : (__one - old_size);
                if (new_in_first > 0) {
                    wjr::uninitialized_copy_restrict_using_allocator(iter, iter + new_in_first,
                                                                     __head + old_size, al);
                    iter += new_in_first;
                }

                // If we need more space, use second segment
                if (n > __one) {
                    const auto new_in_second = n - __one;
                    wjr::uninitialized_copy_restrict_using_allocator(iter, iter + new_in_second,
                                                                     data(), al);
                    __get_tail() = data() + new_in_second;
                } else {
                    __get_tail() = __head + n;
                }
            } else {
                // Existing elements span both segments
                const auto old_size_two = old_size - old_size_one;

                // Assign to existing elements in second segment
                wjr::copy_restrict(iter, iter + old_size_two, data());
                iter += old_size_two;

                // Determine where to place new elements
                const auto remaining = n - old_size;
                const auto space_in_first = __one - old_size_one;

                if (remaining <= space_in_first) {
                    // All new elements fit in first segment
                    wjr::uninitialized_copy_restrict_using_allocator(iter, iter + remaining,
                                                                     __head + old_size_one, al);
                    __get_tail() = data() + old_size_two;
                } else {
                    // New elements span both segments
                    if (space_in_first > 0) {
                        wjr::uninitialized_copy_restrict_using_allocator(
                            iter, iter + space_in_first, __head + old_size_one, al);
                        iter += space_in_first;
                    }
                    const auto new_in_second = remaining - space_in_first;
                    wjr::uninitialized_copy_restrict_using_allocator(iter, iter + new_in_second,
                                                                     data() + old_size_two, al);
                    __get_tail() = data() + old_size_two + new_in_second;
                }
            }

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

private:
    cpair<_Alty, storage_type> m_pair;
};

template <typename Iter, typename T = iterator_value_t<Iter>, typename Alloc = std::allocator<T>,
          WJR_REQUIRES(is_iterator_v<Iter>)>
basic_ring_buffer(Iter, Iter,
                  Alloc = Alloc()) -> basic_ring_buffer<default_ring_buffer_storage<T, Alloc>>;

template <typename S>
struct get_relocate_mode<basic_ring_buffer<S>> {
    static constexpr relocate_t value = basic_ring_buffer<S>::relocate_mode;
};

template <typename T, typename Alloc = std::allocator<T>>
using ring_buffer = basic_ring_buffer<default_ring_buffer_storage<T, Alloc>>;

/**
 * @brief A ring_buffer with fixed capacity by construction.
 *
 * @details Only allocate memory on construction and deallocation on
 * destruction. After construction, it cannot be expanded and can only be
 * modified through move assignment. For example, ring_buffer that using stack
 * allocator.
 */
template <typename T, typename Alloc = std::allocator<T>>
using fixed_ring_buffer = basic_ring_buffer<fixed_ring_buffer_storage<T, Alloc>>;

template <typename Storage>
void swap(basic_ring_buffer<Storage> &lhs, basic_ring_buffer<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace wjr

namespace std {

template <typename Storage>
constexpr void swap(wjr::basic_ring_buffer<Storage> &lhs,
                    wjr::basic_ring_buffer<Storage> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace std

#endif // WJR_CONTAINER_RING_BUFFER_HPP__