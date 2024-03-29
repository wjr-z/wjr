#ifndef WJR_CONTAINER_GENERIC_CONTAINER_VECTOR_HPP__
#define WJR_CONTAINER_GENERIC_CONTAINER_VECTOR_HPP__

/**
 * @file vector.hpp
 * @brief Vector container with definable internal structure
 *
 * @details
 * Customized internal structure needs to follow the following function signature: \n
 * -# storage()
 * -# template <typename _Alloc>  \n
 * storage(_Alloc&& al)
 * -# template <typename _Alloc> \n
 * storage(_Alloc&& al, size_type size, size_type capacity, in_place_reallocate_t)
 * -# ~storage() noexcept
 * -# auto& get_allocator() noexcept
 * -# const auto& get_allocator() const noexcept
 * -# void destroy() noexcept
 * -# void destroy_and_deallocate()
 * -# void uninitialized_construct(size_type size, size_type capacity)
 * -# void take_storage(storage&& other)
 * -# void swap_storage(storage& other)
 * -# decltype(auto) size() noexcept
 * -# size_type capacity() const noexcept
 * -# pointer data() noexcept
 * -# const_pointer data() const noexcept
 *
 * 1 : should not allocate memory. \n
 * 2 : just store the allocator. \n
 * 3 : allocate memory and set the size and capacity. this function is used for
 * reallocation. if the storage is not reallocatable, this function won't be
 * implemented. \n
 * 4 : don't need to destroy or deallocate. \n
 * 5-6 : \n
 * 7 : destroy all elements. don't change ptr, size and capacity. \n
 * 8 : destroy and deallocate. \n
 * 9 : uninitialized construct the storage. allocate memory and set the size and
 * capacity. \n
 * 10 : take the storage from other. set other to empty. \n
 * 11 : swap the storage with other. \n
 * 12 : get the size. the return type must be reference,
 * such as size_type&, std::reference_wrapper<size_type> and so on. \n
 * 13 : get the capacity. \n
 * 14-15 : get the pointer. \n
 *
 * the size type of 14 need to implement the following function signature: \n
 * -# auto& operator=(size_type) noexcept
 * -# operator size_type() const noexcept
 * -# size_type operator++() noexcept
 * -# size_type operator--() noexcept
 * -# size_type operator+=(size_type) noexcept
 * -# size_type operator-=(size_type) noexcept
 *
 * @version 0.1
 * @date 2024-03-19
 *
 */

#include <wjr/assert.hpp>
#include <wjr/compressed_pair.hpp>
#include <wjr/container/generic/container_traits.hpp>
#include <wjr/memory/temporary_value_allocator.hpp>

namespace wjr {

/**
 * @brief Default vector storage
 *
 * @details Use one pointer ans two size_type currently.
 *
 */
template <typename T, typename Alloc>
class default_vector_storage : noncopyable {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = T;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using allocator_type = Alloc;
    using is_reallocatable = std::true_type;

private:
    struct Data {
        pointer m_data = {};
        size_type m_size = 0;
        size_type m_capacity = 0;
    };

    using data_type = Data;

public:
    default_vector_storage() noexcept = default;

    template <typename _Alloc>
    WJR_CONSTEXPR20 default_vector_storage(_Alloc &&al) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {}

    template <typename _Alloc>
    WJR_CONSTEXPR20 default_vector_storage(_Alloc &&al, size_type size,
                                           size_type capacity,
                                           in_place_reallocate_t) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {
        uninitialized_construct(size, capacity);
    }

    ~default_vector_storage() noexcept = default;

    WJR_PURE WJR_CONSTEXPR20 _Alty &get_allocator() noexcept { return m_pair.first(); }
    WJR_PURE WJR_CONSTEXPR20 const _Alty &get_allocator() const noexcept {
        return m_pair.first();
    }

    WJR_CONSTEXPR20 void destroy() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(data() == nullptr) && data() == nullptr) {
            return;
        }

        if (WJR_BUILTIN_CONSTANT_P(size() == 0) && size() == 0) {
            return;
        }

        destroy_n_using_allocator(data(), size(), get_allocator());
    }

    WJR_CONSTEXPR20 void destroy_and_deallocate() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(capacity() == 0) && capacity() == 0) {
            return;
        }

        if (data()) {
            destroy();
            get_allocator().deallocate(data(), capacity());
        }
    }

    WJR_CONSTEXPR20 void uninitialized_construct(size_type size, size_type capacity) {
        auto &al = get_allocator();
        auto &m_storage = __get_data();
        auto result = allocate_at_least(al, capacity);

        m_storage.m_data = result.ptr;
        m_storage.m_size = size;
        m_storage.m_capacity = result.count;
    }

    WJR_CONSTEXPR20 void take_storage(default_vector_storage &&other) noexcept {
        auto &other_storage = other.__get_data();
        __get_data() = std::move(other_storage);
        other_storage = {};
    }

    WJR_CONSTEXPR20 void swap_storage(default_vector_storage &other) noexcept {
        std::swap(__get_data(), other.__get_data());
    }

    WJR_PURE WJR_CONSTEXPR20 size_type &size() noexcept { return __get_data().m_size; }
    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return __get_data().m_size;
    }
    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return __get_data().m_capacity;
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return __get_data().m_data; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return __get_data().m_data;
    }

private:
    WJR_PURE WJR_CONSTEXPR20 data_type &__get_data() noexcept { return m_pair.second(); }
    WJR_PURE WJR_CONSTEXPR20 const data_type &__get_data() const noexcept {
        return m_pair.second();
    }

    compressed_pair<_Alty, data_type> m_pair;
};

/**
 * @brief Static vector storage
 *
 * @details Use static storage for small size. \n
 *
 * @tparam Capacity Static capacity
 */
template <typename T, size_t Capacity, typename Alloc>
class static_vector_storage : noncopyable {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = T;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using allocator_type = Alloc;
    using is_reallocatable = std::false_type;

private:
    static constexpr auto max_alignment =
        std::max<size_type>(alignof(T), alignof(size_type));

    struct Data {
        size_type m_size = 0;
        alignas(
            max_alignment) std::aligned_storage_t<sizeof(T), alignof(T)> m_data[Capacity];
    };

    using data_type = Data;

public:
    static_vector_storage() noexcept = default;

    template <typename _Alloc>
    WJR_CONSTEXPR20 static_vector_storage(_Alloc &&al) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {}

    ~static_vector_storage() noexcept = default;

    WJR_PURE WJR_CONSTEXPR20 _Alty &get_allocator() noexcept { return m_pair.first(); }
    WJR_PURE WJR_CONSTEXPR20 const _Alty &get_allocator() const noexcept {
        return m_pair.first();
    }

    WJR_CONSTEXPR20 void destroy() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(size() == 0) && size() == 0) {
            return;
        }

        destroy_n_using_allocator(data(), size(), get_allocator());
    }

    WJR_CONSTEXPR20 void destroy_and_deallocate() noexcept { destroy(); }

    WJR_CONSTEXPR20 void uninitialized_construct(size_type size,
                                                 WJR_MAYBE_UNUSED size_type capacity) {
        WJR_ASSERT_ASSUME(capacity <= Capacity,
                          "capacity must be less than or equal to Capacity");
        auto &m_storage = __get_data();
        m_storage.m_size = size;
    }

    WJR_CONSTEXPR20 void take_storage(static_vector_storage &&other) noexcept {
        auto &al = get_allocator();
        auto &m_storage = __get_data();
        auto &other_storage = other.__get_data();
        m_storage.m_size = other_storage.m_size;
        uninitialized_move_n_using_allocator(other_storage.m_data, m_storage.m_size,
                                             m_storage.m_data, al);
        other_storage.m_size = 0;
    }

    WJR_CONSTEXPR20 void swap_storage(static_vector_storage &other) noexcept {
        auto &al = get_allocator();
        static_vector_storage tmp;
        tmp.take_storage(std::move(other));
        other.take_storage(std::move(*this));
        take_storage(std::move(tmp));
    }

    WJR_PURE WJR_CONSTEXPR20 size_type &size() noexcept { return __get_data().m_size; }
    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return __get_data().m_size;
    }
    WJR_CONST WJR_CONSTEXPR20 size_type capacity() const noexcept { return Capacity; }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept {
        return reinterpret_cast<pointer>(__get_data().m_data);
    }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return reinterpret_cast<const_pointer>(__get_data().m_data);
    }

private:
    WJR_PURE WJR_CONSTEXPR20 data_type &__get_data() noexcept { return m_pair.second(); }
    WJR_PURE WJR_CONSTEXPR20 const data_type &__get_data() const noexcept {
        return m_pair.second();
    }

    compressed_pair<_Alty, data_type> m_pair;
};

template <typename T, typename Alloc>
class fixed_vector_storage : noncopyable {
    using _Alty = typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using _Alty_traits = std::allocator_traits<_Alty>;

public:
    using value_type = T;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using allocator_type = Alloc;
    using is_reallocatable = std::false_type;

private:
    struct Data {
        pointer m_data = {};
        size_type m_size = 0;
        size_type m_capacity = 0;
    };

    using data_type = Data;

public:
    fixed_vector_storage() noexcept = default;

    template <typename _Alloc>
    WJR_CONSTEXPR20 fixed_vector_storage(_Alloc &&al) noexcept
        : m_pair(std::forward<_Alloc>(al), {}) {}

    ~fixed_vector_storage() noexcept = default;

    WJR_PURE WJR_CONSTEXPR20 _Alty &get_allocator() noexcept { return m_pair.first(); }
    WJR_PURE WJR_CONSTEXPR20 const _Alty &get_allocator() const noexcept {
        return m_pair.first();
    }

    WJR_CONSTEXPR20 void destroy() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(data() == nullptr) && data() == nullptr) {
            return;
        }

        if (WJR_BUILTIN_CONSTANT_P(size() == 0) && size() == 0) {
            return;
        }

        destroy_n_using_allocator(data(), size(), get_allocator());
    }

    WJR_CONSTEXPR20 void destroy_and_deallocate() noexcept {
        if (WJR_BUILTIN_CONSTANT_P(capacity() == 0) && capacity() == 0) {
            return;
        }

        if (data()) {
            destroy();
            get_allocator().deallocate(data(), capacity());
        }
    }

    WJR_CONSTEXPR20 void uninitialized_construct(size_type size, size_type capacity) {
        auto &al = get_allocator();
        auto &m_storage = __get_data();
        auto result = allocate_at_least(al, capacity);

        m_storage.m_data = result.ptr;
        m_storage.m_size = size;
        m_storage.m_capacity = result.count;
    }

    WJR_CONSTEXPR20 void take_storage(fixed_vector_storage &&other) noexcept {
        auto &other_storage = other.__get_data();
        __get_data() = std::move(other_storage);
        other_storage = {};
    }

    WJR_CONSTEXPR20 void swap_storage(fixed_vector_storage &other) noexcept {
        std::swap(__get_data(), other.__get_data());
    }

    WJR_PURE WJR_CONSTEXPR20 size_type &size() noexcept { return __get_data().m_size; }
    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept {
        return __get_data().m_size;
    }
    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return __get_data().m_capacity;
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return __get_data().m_data; }
    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return __get_data().m_data;
    }

private:
    WJR_PURE WJR_CONSTEXPR20 data_type &__get_data() noexcept { return m_pair.second(); }
    WJR_PURE WJR_CONSTEXPR20 const data_type &__get_data() const noexcept {
        return m_pair.second();
    }

    compressed_pair<_Alty, data_type> m_pair;
};

namespace vector_details {

template <typename Enable, typename Storage, typename... Args>
struct __has_vector_storage_shrink_to_fit : std::false_type {};
template <typename Storage, typename... Args>
struct __has_vector_storage_shrink_to_fit<
    std::void_t<decltype(std::declval<Storage>().shrink_to_fit())>, Storage, Args...>
    : std::true_type {};
template <typename Storage, typename... Args>
struct has_vector_storage_shrink_to_fit
    : __has_vector_storage_shrink_to_fit<void, Storage, Args...> {};
template <typename Storage, typename... Args>
constexpr bool has_vector_storage_shrink_to_fit_v =
    has_vector_storage_shrink_to_fit<Storage, Args...>::value;

} // namespace vector_details

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

private:
    using _Alty =
        typename std::allocator_traits<allocator_type>::template rebind_alloc<value_type>;
    using _Alty_traits = std::allocator_traits<_Alty>;

    using storage_type = Storage;
    using storage_traits_type = container_traits<_Alty>;
    using is_storage_reallocatable = typename storage_type::is_reallocatable;
    using __get_size_t = decltype(std::declval<storage_type>().size());

    static_assert(std::is_reference_v<unref_wrapper_t<__get_size_t>>,
                  "return type of "
                  "storage::size() must be "
                  "reference type");

    friend class container_traits<_Alty>;

public:
    static_assert(std::is_same_v<typename _Alty_traits::value_type, value_type>,
                  "allocator_type::value_type must be the same as value_type");

    using size_type = typename storage_type::size_type;
    using difference_type = typename storage_type::difference_type;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static_assert(std::is_pointer_v<pointer>, "");
    static_assert(std::is_pointer_v<const_pointer>, "");

    WJR_CONSTEXPR20
    basic_vector() noexcept(std::is_nothrow_default_constructible_v<storage_type>) =
        default;

    WJR_CONSTEXPR20 explicit basic_vector(const allocator_type &al) noexcept(
        std::is_nothrow_default_constructible_v<storage_type>)
        : m_storage(al) {}

    WJR_CONSTEXPR20 explicit basic_vector(const size_type n,
                                          const allocator_type &al = allocator_type())
        : basic_vector(al) {
        __construct_n(n, in_place_value_construct);
    }

    WJR_CONSTEXPR20 basic_vector(size_type n, const value_type &val,
                                 const allocator_type &al = allocator_type())
        : basic_vector(al) {
        __construct_n(n, val);
    }

private:
    template <typename _Alloc>
    WJR_CONSTEXPR20
    basic_vector(const basic_vector &other, _Alloc &&al, in_place_empty_t) noexcept(
        std::is_nothrow_constructible_v<storage_type, const storage_type &, _Alloc &&>)
        : m_storage(std::forward<_Alloc>(al)) {
        auto size = other.size();
        m_storage.uninitialized_construct(size, other.capacity());
        uninitialized_copy_n_using_allocator(other.data(), size, data(),
                                             __get_allocator());
    }

    template <typename _Alloc>
    WJR_CONSTEXPR20
    basic_vector(basic_vector &&other, _Alloc &&al, in_place_empty_t) noexcept(
        std::is_nothrow_constructible_v<storage_type, const storage_type &, _Alloc &&>)
        : m_storage(std::forward<_Alloc>(al)) {
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

    WJR_CONSTEXPR20 basic_vector(basic_vector &&other)
        : basic_vector(std::move(other), std::move(other.__get_allocator()),
                       in_place_empty) {}

    WJR_CONSTEXPR20 basic_vector(basic_vector &&other, const allocator_type &al) noexcept
        : basic_vector(std::move(other), al, in_place_empty) {}

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    WJR_CONSTEXPR20 basic_vector(Iter first, Iter last,
                                 const allocator_type &al = allocator_type())
        : basic_vector(al) {
        __range_construct(first, last,
                          typename std::iterator_traits<Iter>::iterator_category());
    }

    WJR_CONSTEXPR20 basic_vector(std::initializer_list<value_type> il,
                                 const allocator_type &al = allocator_type())
        : basic_vector(il.begin(), il.end(), al) {}

    WJR_CONSTEXPR20 ~basic_vector() noexcept { __destroy_and_deallocate(); }

    WJR_CONSTEXPR20 basic_vector &operator=(const basic_vector &other) {
        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_traits_type::copy_assign(*this, other);
        }

        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &operator=(basic_vector &&other) noexcept(
        _Alty_traits::propagate_on_container_move_assignment::value ||
        _Alty_traits::is_always_equal::value) {

        if (WJR_LIKELY(this != std::addressof(other))) {
            storage_traits_type::move_assign(*this, std::move(other));
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

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    WJR_CONSTEXPR20 basic_vector &assign(Iter first, Iter last) {
        __range_assign(first, last,
                       typename std::iterator_traits<Iter>::iterator_category());
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &assign(std::initializer_list<value_type> il) {
        return assign(il.begin(), il.end());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 iterator begin() noexcept { return data(); }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_iterator begin() const noexcept {
        return data();
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_iterator cbegin() const noexcept {
        return data();
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 iterator end() noexcept { return data() + size(); }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_iterator end() const noexcept {
        return data() + size();
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_iterator cend() const noexcept {
        return data() + size();
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(cbegin());
    }

    WJR_PURE WJR_CONSTEXPR20 size_type size() const noexcept { return m_storage.size(); }

    WJR_CONSTEXPR20 void resize(const size_type new_size) {
        __resize(new_size, in_place_value_construct);
    }

    WJR_CONSTEXPR20 void resize(const size_type new_size, const value_type &val) {
        __resize(new_size, val);
    }

    /**
     * @todo designed shrink_to_fit for storage.
     */
    WJR_CONSTEXPR20 void shrink_to_fit() {
        if constexpr (vector_details::has_vector_storage_shrink_to_fit_v<storage_type>) {
            m_storage.shrink_to_fit();
        } else if constexpr (is_storage_reallocatable::value) {
            if (size() < capacity()) {
                auto &al = __get_allocator();
                storage_type new_storage(al, size(), size(), in_place_reallocate);
                uninitialized_move_n_using_allocator(data(), size(), new_storage.data(),
                                                     al);
                __destroy_and_deallocate();
                __take_storage(std::move(new_storage));
            }
        }
    }

    WJR_PURE WJR_CONSTEXPR20 size_type capacity() const noexcept {
        return m_storage.capacity();
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 bool empty() const noexcept { return size() == 0; }

    WJR_CONST WJR_CONSTEXPR20 static size_type
    get_growth_capacity(size_type old_capacity, size_type new_size) noexcept {
        return std::max(old_capacity + old_capacity / 2, new_size);
    }

    WJR_CONSTEXPR20 void reserve(size_type n) {
        if constexpr (is_storage_reallocatable::value) {
            const size_type old_size = size();
            const size_type old_capacity = capacity();
            if (WJR_UNLIKELY(old_capacity < n)) {
                auto &al = __get_allocator();
                const size_type new_capacity = get_growth_capacity(old_capacity, n);

                storage_type new_storage(al, old_size, new_capacity, in_place_reallocate);
                uninitialized_move_n_using_allocator(data(), old_size, new_storage.data(),
                                                     al);
                __destroy_and_deallocate();
                __take_storage(std::move(new_storage));
            }
        }
    }

    WJR_INLINE_CONSTEXPR20 reference operator[](size_type pos) noexcept {
        return *(data() + pos);
    }

    WJR_INLINE_CONSTEXPR20 const_reference operator[](size_type pos) const noexcept {
        return *(data() + pos);
    }

    WJR_INLINE_CONSTEXPR20 reference at(size_type pos) {
        if (WJR_UNLIKELY(pos >= size())) {
            WJR_THROW(std::out_of_range("basic_vector::at"));
        }

        return (*this)[pos];
    }

    WJR_INLINE_CONSTEXPR20 const_reference at(size_type pos) const {
        if (WJR_UNLIKELY(pos >= size())) {
            WJR_THROW(std::out_of_range("basic_vector::at"));
        }

        return (*this)[pos];
    }

    WJR_INLINE_CONSTEXPR20 reference front() noexcept { return *data(); }

    WJR_INLINE_CONSTEXPR20 const_reference front() const noexcept { return *data(); }

    WJR_INLINE_CONSTEXPR20 reference back() noexcept { return *(data() + size() - 1); }

    WJR_INLINE_CONSTEXPR20 const_reference back() const noexcept {
        return *(data() + size() - 1);
    }

    WJR_PURE WJR_CONSTEXPR20 pointer data() noexcept { return m_storage.data(); }

    WJR_PURE WJR_CONSTEXPR20 const_pointer data() const noexcept {
        return m_storage.data();
    }

    WJR_PURE WJR_CONSTEXPR20 const_pointer cdata() const noexcept { return data(); }

    template <typename... Args>
    WJR_INLINE_CONSTEXPR20 reference emplace_back(Args &&...args) {
        const pointer __last = data() + size();
        const pointer __end = data() + capacity();

        if (WJR_LIKELY(__last != __end)) {
            uninitialized_construct_using_allocator(__last, __get_allocator(),
                                                    std::forward<Args>(args)...);
            ++__get_size();
        } else {
            __realloc_insert_at_end(std::forward<Args>(args)...);
        }

        return back();
    }

    WJR_INLINE_CONSTEXPR20 void push_back(const value_type &val) { emplace_back(val); }

    WJR_INLINE_CONSTEXPR20 void push_back(value_type &&val) {
        emplace_back(std::move(val));
    }

    WJR_INLINE_CONSTEXPR20 void pop_back() noexcept {
        size_type __size = --__get_size();
        destroy_at_using_allocator(data() + __size, __get_allocator());
    }

    template <typename... Args>
    WJR_INLINE_CONSTEXPR20 iterator emplace(const_iterator pos, Args &&...args) {
        return __emplace_aux(pos, std::forward<Args>(args)...);
    }

    WJR_INLINE_CONSTEXPR20 iterator insert(const_iterator pos, const value_type &val) {
        return emplace(pos, val);
    }

    WJR_INLINE_CONSTEXPR20 iterator insert(const_iterator pos, value_type &&val) {
        return emplace(pos, std::move(val));
    }

    WJR_CONSTEXPR20 iterator insert(const_iterator pos,
                                    std::initializer_list<value_type> il) {
        return insert(pos, il.begin(), il.end());
    }

    WJR_CONSTEXPR20 iterator insert(const_iterator pos, size_type n,
                                    const value_type &val) {
        const auto old_pos = static_cast<size_type>(pos - cbegin());
        __fill_insert(begin() + old_pos, n, val);
        return begin() + old_pos;
    }

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    WJR_CONSTEXPR20 iterator insert(const_iterator pos, Iter first, Iter last) {
        const auto old_pos = static_cast<size_type>(pos - cbegin());
        __range_insert(begin() + old_pos, first, last,
                       typename std::iterator_traits<Iter>::iterator_category());
        return begin() + old_pos;
    }

    WJR_INLINE_CONSTEXPR20 iterator erase(const_iterator pos) {
        return __erase(begin() + (pos - cbegin()));
    }

    WJR_INLINE_CONSTEXPR20 iterator erase(const_iterator first, const_iterator last) {
        const iterator __beg = begin();
        const const_iterator __cbeg = cbegin();
        return __erase(__beg + (first - __cbeg), __beg + (last - __cbeg));
    }

    WJR_INLINE_CONSTEXPR20 void swap(basic_vector &other) noexcept {
        storage_traits_type::swap(*this, other);
    }

    WJR_CONSTEXPR20 void clear() {
        __erase_at_end(data());
        WJR_ASSUME(size() == 0);
    }

    WJR_PURE WJR_INLINE_CONSTEXPR20 allocator_type &get_allocator() noexcept {
        return __get_allocator();
    }
    WJR_PURE WJR_INLINE_CONSTEXPR20 const allocator_type &get_allocator() const noexcept {
        return __get_allocator();
    }

    // extension

    WJR_CONSTEXPR20 basic_vector(size_type n, in_place_default_construct_t,
                                 const allocator_type &al = allocator_type())
        : basic_vector(al) {
        __construct_n(n, in_place_default_construct);
    }

    WJR_CONSTEXPR20 void resize(const size_type new_size, in_place_default_construct_t) {
        __resize(new_size, in_place_default_construct);
    }

    WJR_CONSTEXPR20 void push_back(in_place_default_construct_t) {
        emplace_back(in_place_default_construct);
    }

    WJR_CONSTEXPR20 basic_vector &append(const value_type &val) {
        emplace_back(val);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(value_type &&val) {
        emplace_back(std::move(val));
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(in_place_default_construct_t) {
        emplace_back(in_place_default_construct);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(const size_type n, const value_type &val) {
        __append(n, val);
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &append(const size_type n,
                                         in_place_default_construct_t) {
        __append(n, in_place_default_construct);
        return *this;
    }

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    WJR_CONSTEXPR20 basic_vector &append(Iter first, Iter last) {
        __range_append(first, last,
                       typename std::iterator_traits<Iter>::iterator_category());
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
        __erase_at_end(end() - n);
        return *this;
    }

    /**
     * @brief Truncate the size to n
     *
     */
    WJR_CONSTEXPR20 basic_vector &truncate(const size_type n) { return chop(size() - n); }

    template <typename Iter, std::enable_if_t<is_iterator_v<Iter>, int> = 0>
    WJR_CONSTEXPR20 basic_vector &replace(const_iterator from, const_iterator to,
                                          Iter first, Iter last) {
        const pointer __beg = begin();
        const const_iterator __cbeg = cbegin();
        const auto __offset1 = static_cast<size_type>(from - __cbeg);
        const auto __offset2 = static_cast<size_type>(to - __cbeg);
        __range_replace(__beg + __offset1, __beg + __offset2, first, last,
                        typename std::iterator_traits<Iter>::iterator_category());
        return *this;
    }

    WJR_CONSTEXPR20 basic_vector &replace(const_iterator from, const_iterator to,
                                          const size_type n, const value_type &val) {
        const pointer __beg = begin();
        const const_iterator __cbeg = cbegin();
        const auto __offset1 = static_cast<size_type>(from - __cbeg);
        const auto __offset2 = static_cast<size_type>(to - __cbeg);
        __fill_replace(__beg + __offset1, __beg + __offset2, n, val);
        return *this;
    }

    WJR_PURE WJR_CONSTEXPR20 storage_type &get_storage() noexcept { return m_storage; }
    WJR_PURE WJR_CONSTEXPR20 const storage_type &get_storage() const noexcept {
        return m_storage;
    }

private:
    // member function for container_traits (START)

    WJR_PURE WJR_CONSTEXPR20 _Alty &__get_allocator() noexcept {
        return m_storage.get_allocator();
    }

    WJR_PURE WJR_CONSTEXPR20 const _Alty &__get_allocator() const noexcept {
        return m_storage.get_allocator();
    }

    WJR_CONSTEXPR20 void __destroy() noexcept { m_storage.destroy(); }

    WJR_CONSTEXPR20 void __destroy_and_deallocate() noexcept {
        m_storage.destroy_and_deallocate();
    }

    WJR_CONSTEXPR20 void __copy_element(const basic_vector &other) {
        assign(other.begin(), other.end());
    }

    WJR_CONSTEXPR20 void __take_storage(basic_vector &&other) {
        __take_storage(std::move(other.m_storage));
    }

    WJR_CONSTEXPR20 void __move_element(basic_vector &&other) {
        assign(std::make_move_iterator(other.begin()),
               std::make_move_iterator(other.end()));
    }

    WJR_CONSTEXPR20 void __swap_storage(basic_vector &other) {
        m_storage.swap_storage(other.m_storage);
    }

    // member function for container_traits (END)

    WJR_PURE WJR_CONSTEXPR20 __get_size_t __get_size() noexcept {
        return m_storage.size();
    }

    WJR_CONSTEXPR20 void __take_storage(storage_type &&other) {
        m_storage.take_storage(std::move(other));
    }

    WJR_CONSTEXPR20 void
    __unreallocatable_unreachable(WJR_MAYBE_UNUSED size_type new_capacity) const {
        WJR_ASSERT(
            new_capacity <= capacity(),
            "new_capacity must be less than or equal to capacity if the storage is not reallocatable.\nnew_capacity = ",
            new_capacity, ", capacity = ", capacity());
        WJR_UNREACHABLE();
    }

    template <typename... Args,
              std::enable_if_t<sizeof...(Args) == 1 || sizeof...(Args) == 2, int> = 0>
    WJR_CONSTEXPR20 void __construct_n(const size_type n, Args &&...args) {
        if (n != 0) {
            auto &al = __get_allocator();
            m_storage.uninitialized_construct(n, n);
            if constexpr (sizeof...(Args) == 1) {
                uninitialized_fill_n_using_allocator(data(), n, al,
                                                     std::forward<Args>(args)...);
            } else if constexpr (sizeof...(Args) == 2) {
                uninitialized_copy_using_allocator(std::forward<Args>(args)..., data(),
                                                   al);
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

    WJR_CONSTEXPR20 void __erase_at_end(const_pointer pos) noexcept {
        const pointer __first = data();
        const pointer __last = data() + size();
        WJR_ASSERT(pos >= __first && pos <= __last,
                   "pos must be in the range of [begin(), end()]");
        const auto new_size = static_cast<size_type>(pos - __first);
        destroy_using_allocator(__first + new_size, __last, __get_allocator());
        __get_size() = new_size;
    }

    WJR_CONSTEXPR20 iterator __erase(iterator pos) noexcept {
        const pointer __last = data() + size();
        if (pos + 1 != __last) {
            std::move(pos + 1, __last, pos);
        }

        destroy_at_using_allocator(__last - 1, __get_allocator());
        --__get_size();
        return pos;
    }

    WJR_CONSTEXPR20 iterator __erase(iterator first, iterator last) noexcept {
        const pointer __last = data() + size();
        if (first != last) {
            if (last != __last) {
                std::move(last, __last, first);
            }

            __erase_at_end(__last - (last - first));
        }

        return first;
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_insert(iterator pos, Iter first, Iter last,
                                        std::input_iterator_tag) {
        if (pos == end()) {
            __range_append(first, last, std::input_iterator_tag{});
        } else if (first != last) {
            basic_vector tmp(first, last, __get_allocator());
            insert(pos, tmp.begin(), tmp.end());
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_insert(iterator pos, Iter first, Iter last,
                                        std::forward_iterator_tag) {
        if (first != last) {
            auto &al = __get_allocator();
            const pointer __first = data();
            const pointer __last = data() + size();
            const pointer __end = data() + capacity();

            const auto n = static_cast<size_type>(std::distance(first, last));
            const auto __rest = static_cast<size_type>(__end - __last);

            if (WJR_LIKELY(__rest >= n)) {
                const auto __elements_after = static_cast<size_type>(__last - pos);
                if (__elements_after > n) {
                    uninitialized_move_n_using_allocator(__last - n, n, __last, al);
                    std::move_backward(pos, __last - n, __last);
                    std::copy(first, last, pos);
                } else {
                    auto mid = first;
                    std::advance(mid, __elements_after);

                    uninitialized_copy_using_allocator(mid, last, __last, al);
                    uninitialized_move_using_allocator(pos, __last,
                                                       __last + n - __elements_after, al);
                    std::copy(first, mid, pos);
                }

                __get_size() += n;
            } else {
                if constexpr (is_storage_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__last - __first);
                    const auto old_pos = static_cast<size_type>(pos - __first);
                    const size_type new_capacity =
                        get_growth_capacity(capacity(), old_size + n);
                    storage_type new_storage(al, old_size + n, new_capacity,
                                             in_place_reallocate);
                    const pointer new_first = new_storage.data();

                    uninitialized_copy_using_allocator(first, last, new_first + old_pos,
                                                       al);
                    uninitialized_move_using_allocator(__first, pos, new_first, al);
                    uninitialized_move_using_allocator(pos, __last,
                                                       new_first + old_pos + n, al);

                    __destroy_and_deallocate();
                    __take_storage(std::move(new_storage));
                } else {
                    __unreallocatable_unreachable(size() + n);
                }
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
        if (first != last) {
            auto &al = __get_allocator();
            const pointer __first = data();
            const pointer __last = data() + size();
            const pointer __end = data() + capacity();

            const auto n = static_cast<size_type>(std::distance(first, last));
            const auto __rest = static_cast<size_type>(__end - __last);

            if (WJR_LIKELY(__rest >= n)) {
                uninitialized_copy_n_using_allocator(first, n, __last, al);
                __get_size() += n;
            } else {
                if constexpr (is_storage_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__last - __first);
                    const size_type new_capacity =
                        get_growth_capacity(capacity(), old_size + n);

                    storage_type new_storage(al, old_size + n, new_capacity,
                                             in_place_reallocate);
                    const pointer new_first = new_storage.data();

                    uninitialized_copy_using_allocator(first, last, new_first + old_size,
                                                       al);
                    uninitialized_move_using_allocator(__first, __last, new_first, al);

                    __destroy_and_deallocate();
                    __take_storage(std::move(new_storage));
                } else {
                    __unreallocatable_unreachable(size() + n);
                }
            }
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last, std::input_iterator_tag) {
        const pointer __first = data();
        const pointer __last = data() + size();

        pointer cur = __first;

        for (; first != last && cur != __last; ++first, ++cur) {
            *cur = *first;
        }

        if (first == last) {
            __erase_at_end(cur);
        } else {
            __range_append(first, last, std::input_iterator_tag{});
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_assign(Iter first, Iter last,
                                        std::forward_iterator_tag) {
        auto n = static_cast<size_type>(std::distance(first, last));
        auto &al = __get_allocator();
        const pointer __first = data();
        const pointer __last = data() + size();

        if (n <= size()) {
            std::copy(first, last, __first);
            __erase_at_end(__first + n);
        } else if (WJR_LIKELY(n <= capacity())) {
            auto mid = first;
            std::advance(mid, size());
            std::copy(first, mid, begin());
            uninitialized_copy_using_allocator(mid, last, __last, al);
            __get_size() = n;
        } else {
            if constexpr (is_storage_reallocatable::value) {
                size_type new_capacity = get_growth_capacity(capacity(), n);
                storage_type new_storage(al, n, new_capacity, in_place_reallocate);
                const pointer new_first = new_storage.data();
                uninitialized_copy_n_using_allocator(first, n, new_first, al);

                __destroy_and_deallocate();
                __take_storage(std::move(new_storage));
            } else {
                __unreallocatable_unreachable(n);
            }
        }
    }

    WJR_CONSTEXPR20 void __fill_assign(size_type n, const value_type &val) {
        auto &al = __get_allocator();

        if (WJR_UNLIKELY(n > capacity())) {
            if constexpr (is_storage_reallocatable::value) {
                __destroy_and_deallocate();
                storage_type new_storage(al, n, n, in_place_reallocate);
                uninitialized_fill_n_using_allocator(new_storage.data(), n, al, val);
                __take_storage(std::move(new_storage));
                return;
            } else {
                __unreallocatable_unreachable(n);
            }
        }

        if (n > size()) {
            std::fill(begin(), end(), val);
            uninitialized_fill_n_using_allocator(end(), n - size(), al, val);
            __get_size() = n;
        } else {
            __erase_at_end(std::fill_n(begin(), n, val));
        }
    }

    template <typename... Args>
    WJR_CONSTEXPR20 void __realloc_insert(iterator pos, Args &&...args) {
        if constexpr (is_storage_reallocatable::value) {
            auto &al = __get_allocator();
            const pointer __first = data();
            const pointer __last = data() + size();

            const auto old_pos_size = static_cast<size_type>(pos - __first);
            const auto old_size = static_cast<size_type>(__last - __first);
            const size_type new_size = old_size + 1;
            const size_type new_capacity = get_growth_capacity(old_size, new_size);

            storage_type new_storage(al, new_size, new_capacity, in_place_reallocate);

            const pointer new_first = new_storage.data();
            const pointer new_pos = new_first + old_pos_size;

            uninitialized_construct_using_allocator(new_pos, al,
                                                    std::forward<Args>(args)...);

            uninitialized_move_n_using_allocator(__first, old_pos_size, new_first, al);
            uninitialized_move_using_allocator(pos, __last, new_pos + 1, al);

            __destroy_and_deallocate();
            __take_storage(std::move(new_storage));
        } else {
            __unreallocatable_unreachable(size() + 1);
        }
    }

    template <typename... Args>
    WJR_CONSTEXPR20 void __realloc_insert_at_end(Args &&...args) {
        if constexpr (is_storage_reallocatable::value) {
            auto &al = __get_allocator();
            const pointer __first = data();
            const pointer __last = data() + size();

            const auto old_size = static_cast<size_type>(__last - __first);
            const auto new_size = old_size + 1;
            const size_type new_capacity = get_growth_capacity(old_size, new_size);

            storage_type new_storage(al, new_size, new_capacity, in_place_reallocate);
            const pointer new_first = new_storage.data();

            const pointer new_pos = new_first + old_size;
            uninitialized_construct_using_allocator(new_pos, al,
                                                    std::forward<Args>(args)...);

            uninitialized_move_using_allocator(__first, __last, new_first, al);

            __destroy_and_deallocate();
            __take_storage(std::move(new_storage));
        } else {
            __unreallocatable_unreachable(size() + 1);
        }
    }

    WJR_CONSTEXPR20 void __fill_insert(iterator pos, size_type n, const value_type &val) {
        if (n == 0) {
            return;
        }

        auto &al = __get_allocator();
        const pointer __first = data();
        const pointer __last = data() + size();
        const pointer __end = data() + capacity();

        const auto __rest = static_cast<size_type>(__end - __last);

        if (WJR_LIKELY(__rest >= n)) {
            const temporary_value_allocator tmp(al, val);
            const auto &real_val = *tmp.get();

            const auto __elements_after = static_cast<size_type>(__last - pos);
            if (__elements_after > n) {
                uninitialized_move_n_using_allocator(__last - n, n, __last, al);
                std::move_backward(pos, __last - n, __last);
                std::fill_n(pos, n, real_val);
            } else {
                uninitialized_fill_n_using_allocator(__last, n - __elements_after, al,
                                                     real_val);
                uninitialized_move_using_allocator(pos, __last, pos + n, al);
                std::fill(pos, __last, real_val);
            }

            __get_size() += n;
        } else {
            if constexpr (is_storage_reallocatable::value) {
                const auto new_capacity = get_growth_capacity(capacity(), size() + n);
                storage_type new_storage(al, size() + n, new_capacity,
                                         in_place_reallocate);
                const pointer new_first = new_storage.data();

                const auto old_pos = static_cast<size_type>(pos - __first);

                uninitialized_fill_n_using_allocator(new_first + old_pos, n, al, val);
                uninitialized_move_using_allocator(__first, pos, new_first, al);
                uninitialized_move_using_allocator(pos, __last, new_first + old_pos + n,
                                                   al);

                __destroy_and_deallocate();
                __take_storage(std::move(new_storage));
            } else {
                __unreallocatable_unreachable(size() + n);
            }
        }
    }

    template <typename Ty>
    WJR_CONSTEXPR20 void __resize(const size_type new_size, const Ty &val) {
        const auto old_size = size();
        if (new_size > old_size) {
            __append(new_size - old_size, val);
        } else if (new_size < old_size) {
            __erase_at_end(data() + new_size);
        }
    }

    template <typename Ty>
    WJR_CONSTEXPR20 void __append(size_type n, const Ty &val) {
        auto &al = __get_allocator();

        const auto old_size = size();
        const auto old_capacity = capacity();

        const pointer __first = data();
        const pointer __last = __first + old_size;

        const auto __rest = old_capacity - old_size;
        const auto new_size = old_size + n;

        if (WJR_LIKELY(__rest >= n)) {
            uninitialized_fill_n_using_allocator(__last, n, al, val);
            __get_size() = new_size;
        } else {
            if constexpr (is_storage_reallocatable::value) {
                auto new_capacity = get_growth_capacity(old_capacity, new_size);
                storage_type new_storage(al, new_size, new_capacity, in_place_reallocate);
                const pointer new_first = new_storage.data();

                uninitialized_fill_n_using_allocator(new_first + old_size, n, al, val);
                uninitialized_move_using_allocator(__first, __last, new_first, al);

                __destroy_and_deallocate();
                __take_storage(std::move(new_storage));
            } else {
                __unreallocatable_unreachable(new_size);
            }
        }
    }

    template <typename Args>
    WJR_CONSTEXPR20 void __insert_aux(iterator pos, Args &&args) {
        auto &al = __get_allocator();
        const pointer __last = data() + size();

        uninitialized_construct_using_allocator(__last, al, std::move(*(__last - 1)));

        std::move_backward(pos, __last - 1, __last);
        *pos = std::forward<Args>(args);

        ++__get_size();
    }

    template <typename... Args>
    WJR_CONSTEXPR20 iterator __emplace_aux(const_iterator pos, Args &&...args) {
        const auto __offset = pos - cbegin();

        auto &al = __get_allocator();
        const pointer __last = data() + size();
        const pointer __end = data() + capacity();

        if (WJR_LIKELY(__last != __end)) {
            if (pos == __last) {
                uninitialized_construct_using_allocator(__last, al,
                                                        std::forward<Args>(args)...);
                ++__get_size();
            } else {
                temporary_value_allocator tmp(al, std::forward<Args>(args)...);
                __insert_aux(begin() + __offset, std::move(*tmp.get()));
            }
        } else {
            __realloc_insert(begin() + __offset, std::forward<Args>(args)...);
        }

        return begin() + __offset;
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_replace(iterator old_first, iterator old_last,
                                         Iter new_first, Iter new_last,
                                         std::input_iterator_tag) {
        for (; old_first != old_last && new_first != new_last; ++old_first, ++new_first) {
            *old_first = *new_first;
        }

        if (new_first == new_last) {
            __erase_at_end(old_first, old_last);
        } else {
            __range_insert(old_last, new_first, new_last, std::input_iterator_tag());
        }
    }

    template <typename Iter>
    WJR_CONSTEXPR20 void __range_replace(iterator old_first, iterator old_last,
                                         Iter new_first, Iter new_last,
                                         std::forward_iterator_tag) {
        const auto n = static_cast<size_type>(old_last - old_first);
        const auto m = static_cast<size_type>(std::distance(new_first, new_last));

        if (m <= n) {
            erase(std::copy_n(new_first, m, old_first), old_last);
        } else {
            const auto __delta = m - n;

            auto &al = __get_allocator();
            const auto __first = data();
            const auto __last = data() + size();
            const auto __end = data() + capacity();

            const auto __rest = static_cast<size_type>(__end - __last);

            if (WJR_LIKLELY(__rest >= __delta)) {
                const auto __elements_after = static_cast<size_type>(__last - old_first);
                if (__elements_after > m) {
                    // uninitialized_move(al, __last - __delta, __last, __last);
                    uninitialized_move_using_allocator(__last - __delta, __last, __last,
                                                       al);
                    std::move_backward(old_last, __last - __delta, __last);
                    std::copy(new_first, new_last, old_first);
                } else {
                    auto mid = new_first;
                    std::advance(mid, __elements_after);
                    uninitialized_copy_using_allocator(mid, new_last, __last, al);
                    uninitialized_move_using_allocator(old_last, __last, old_first + m,
                                                       al);
                    std::copy(new_first, mid, old_first);
                }
                __get_size() += __delta;
            } else {
                if constexpr (is_storage_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__last - __first);
                    const auto old_pos = static_cast<size_type>(old_first - __first);
                    const auto new_capacity =
                        get_growth_capacity(capacity(), old_size + __delta);
                    storage_type new_storage(al, old_size + __delta, new_capacity,
                                             in_place_reallocate);
                    const pointer __ptr = new_storage.data();

                    uninitialized_copy_using_allocator(new_first, new_last,
                                                       __ptr + old_pos, al);
                    uninitialized_move_using_allocator(__first, old_first, __ptr, al);
                    uninitialized_move_using_allocator(old_last, __last,
                                                       __ptr + old_pos + m, al);

                    __destroy_and_deallocate();
                    __take_storage(std::move(new_storage));
                } else {
                    __unreallocatable_unreachable(size() + __delta);
                }
            }
        }
    }

    WJR_CONSTEXPR20 void __fill_replace(iterator old_first, iterator old_last,
                                        size_type m, const value_type &val) {

        const auto n = static_cast<size_type>(old_last - old_first);

        if (m <= n) {
            erase(std::fill_n(old_first, m, val), old_last);
        } else {
            const auto __delta = m - n;

            auto &al = __get_allocator();
            const auto __first = data();
            const auto __last = data() + size();
            const auto __end = data() + capacity();

            const auto __rest = static_cast<size_type>(__end - __last);

            if (WJR_LILKELY(__rest >= __delta)) {
                const temporary_value_allocator tmp(al, val);
                const auto &real_value = *tmp.get();

                const auto __elements_after = static_cast<size_type>(__last - old_first);
                if (__elements_after > m) {
                    // uninitialized_move(al, __last - __delta, __last, __last);
                    uninitialized_move_using_allocator(__last - __delta, __last, __last,
                                                       al);
                    std::move_backward(old_last, __last - __delta, __last);
                    std::fill_n(old_first, m, real_value);
                } else {
                    uninitialized_fill_n_using_allocator(__last, m - __elements_after, al,
                                                         real_value);
                    uninitialized_move_using_allocator(old_last, __last, old_first + m,
                                                       al);
                    std::fill(old_first, __last, real_value);
                }
                __get_size() += __delta;
            } else {
                if constexpr (is_storage_reallocatable::value) {
                    const auto old_size = static_cast<size_type>(__last - __first);
                    const auto old_pos = static_cast<size_type>(old_first - __first);
                    const auto new_capacity =
                        get_growth_capacity(capacity(), old_size + __delta);
                    storage_type new_storage(al, old_size + __delta, new_capacity,
                                             in_place_reallocate);
                    const pointer __ptr = new_storage.data();

                    uninitialized_fill_n_using_allocator(__ptr + old_pos, m, al, val);
                    uninitialized_move_using_allocator(__first, old_first, __ptr, al);
                    uninitialized_move_using_allocator(old_last, __last,
                                                       __ptr + old_pos + m, al);

                    __destroy_and_deallocate();
                    __take_storage(std::move(new_storage));
                } else {
                    __unreallocatable_unreachable(size() + __delta);
                }
            }
        }
    }

private:
    storage_type m_storage;
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
 * assignment.
 */
template <typename T, typename Alloc = std::allocator<T>>
using fixed_vector = basic_vector<fixed_vector_storage<T, Alloc>>;

template <typename Iter, typename T = typename std::iterator_traits<Iter>::value_type,
          typename Alloc = std::allocator<T>,
          std::enable_if_t<is_iterator_v<Iter>, int> = 0>
basic_vector(Iter, Iter, Alloc = Alloc())
    -> basic_vector<default_vector_storage<T, Alloc>>;

template <typename Storage>
void swap(basic_vector<Storage> &lhs, basic_vector<Storage> &rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Storage>
bool operator==(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Storage>
bool operator!=(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return !(lhs == rhs);
}

template <typename Storage>
bool operator<(const basic_vector<Storage> &lhs, const basic_vector<Storage> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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