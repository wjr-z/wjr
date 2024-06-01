#ifndef WJR_SPAN_HPP__
#define WJR_SPAN_HPP__

#include <stdexcept>

#include <wjr/assert.hpp>
#include <wjr/iterator/contiguous_iterator_adpater.hpp>

namespace wjr {

/**
 * @brief A type representing a static-sized span.
 *
 * @tparam Extent The number of elements in the span.
 */
template <typename T, size_t Extent>
struct __span_static_storage {

    __span_static_storage() noexcept = default;
    __span_static_storage(const __span_static_storage &) noexcept = default;
    __span_static_storage &operator=(const __span_static_storage &) noexcept = default;

    __span_static_storage(T *p, WJR_MAYBE_UNUSED size_t s) noexcept : ptr(p) {
        WJR_ASSERT_L2(s == size);
    }

    T *ptr = nullptr;
    static constexpr size_t size = Extent;
};

/**
 * @brief A type representing a dynamic-sized span.
 */
template <typename T>
struct __span_dynamic_storage {

    __span_dynamic_storage() noexcept = default;
    __span_dynamic_storage(const __span_dynamic_storage &) noexcept = default;
    __span_dynamic_storage &operator=(const __span_dynamic_storage &) noexcept = default;

    __span_dynamic_storage(T *p, size_t s) noexcept : ptr(p), size(s) {}

    T *ptr = nullptr;
    size_t size = 0;
};

template <typename Iter, typename Elem>
struct __is_span_iterator
    : std::conjunction<is_contiguous_iterator<Iter>,
                       std::is_convertible<iterator_contiguous_pointer_t<Iter>, Elem *>> {
};

template <typename T, size_t Extent = dynamic_extent>
class span;

namespace span_details {

WJR_REGISTER_HAS_TYPE(data, std::data(std::declval<Container &>()), Container);
WJR_REGISTER_HAS_TYPE(size, std::size(std::declval<Container &>()), Container);

/// @private
template <typename T>
struct __is_std_array : std::false_type {};

/// @private
template <typename T, size_t N>
struct __is_std_array<std::array<T, N>> : std::true_type {};

/// @private
template <typename T>
inline constexpr bool __is_std_array_v = __is_std_array<T>::value;

template <typename T>
struct __is_span : std::false_type {};

template <typename T, size_t Extent>
struct __is_span<span<T, Extent>> : std::true_type {};

template <typename T>
inline constexpr bool __is_span_v = __is_span<T>::value;

/// @private
template <typename Container, typename = void>
struct __is_container_like : std::false_type {};

/// @private
template <typename Container>
struct __is_container_like<
    Container, std::enable_if_t<has_data_v<Container &> && has_size_v<Container &>>>
    : std::conjunction<
          std::negation<std::is_array<remove_cvref_t<Container>>>,
          std::negation<__is_std_array<remove_cvref_t<Container>>>,
          std::negation<__is_span<remove_cvref_t<Container>>>,
          std::is_pointer<decltype(std::data(std::declval<Container &>()))>> {};

/// @private
template <typename Container>
inline constexpr bool __is_container_like_v = __is_container_like<Container>::value;

template <typename Container, typename Elem, typename = void>
struct __is_span_like : std::false_type {};

template <typename Container, typename Elem>
struct __is_span_like<
    Container, Elem, std::enable_if_t<has_data_v<Container &> && has_size_v<Container &>>>
    : std::conjunction<
          __is_container_like<Container>,
          std::is_convertible<decltype(std::data(std::declval<Container &>())), Elem *>> {
};

template <typename Container, typename Elem>
inline constexpr bool __is_span_like_v = __is_span_like<Container, Elem>::value;

/// @private
template <typename T>
struct basic_span_traits {
    using value_type = std::remove_cv_t<T>;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
};

} // namespace span_details

/**
 * @class span
 *
 * @brief A view over a contiguous sequence of objectsd.
 *
 * @tparam Extent if Extent is `dynamic_extent`, the span is a runtime-sized view.
 * Otherwise, the span is a compile-time-sized view.
 */
template <typename T, size_t Extent>
class span {
    static constexpr bool __is_dynamic = Extent == dynamic_extent;
    using __storage = std::conditional_t<__is_dynamic, __span_dynamic_storage<T>,
                                         __span_static_storage<T, Extent>>;

    using IteratorTraits = span_details::basic_span_traits<T>;

public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = contiguous_iterator_adapter<span, IteratorTraits>;
    using const_iterator = contiguous_const_iterator_adapter<span, IteratorTraits>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    template <size_t Ex = Extent, WJR_REQUIRES(Ex == dynamic_extent || Ex == 0)>
    constexpr span() noexcept : storage() {}

    template <typename It,
              WJR_REQUIRES(__is_span_iterator<It, element_type>::value &&__is_dynamic)>
    constexpr span(It first, size_type count) noexcept
        : storage((to_address)(first), count) {}

    template <typename It,
              WJR_REQUIRES(__is_span_iterator<It, element_type>::value && !__is_dynamic)>
    constexpr explicit span(It first, size_type count) noexcept
        : storage((to_address)(first), count) {}

    template <typename It,
              WJR_REQUIRES(__is_span_iterator<It, element_type>::value &&__is_dynamic)>
    constexpr span(It first, It last) noexcept
        : storage((to_address)(first), static_cast<size_type>(last - first)) {}

    template <typename It,
              WJR_REQUIRES(__is_span_iterator<It, element_type>::value && !__is_dynamic)>
    constexpr explicit span(It first, It last) noexcept
        : storage((to_address)(first), static_cast<size_type>(last - first)) {}

    template <size_t N, WJR_REQUIRES((__is_dynamic || N == Extent))>
    constexpr span(type_identity_t<element_type> (&arr)[N]) noexcept
        : storage(std::data(arr), N) {}

    template <typename U, size_t N,
              WJR_REQUIRES((__is_dynamic || N == Extent) &&
                           std::is_convertible_v<U *, T *>)>
    constexpr span(std::array<U, N> &arr) noexcept
        : storage(std::data(arr), std::size(arr)) {}

    template <typename U, size_t N,
              WJR_REQUIRES((__is_dynamic || N == Extent) &&
                           std::is_convertible_v<const U *, T *>)>
    constexpr span(const std::array<U, N> &arr) noexcept
        : storage(std::data(arr), std::size(arr)) {}

    template <typename U, size_t N,
              WJR_REQUIRES((__is_dynamic || N == dynamic_extent || N == Extent) &&
                           std::is_convertible_v<U *, T *> && __is_dynamic)>
    constexpr span(const span<U, N> &source) noexcept
        : storage(source.data(), source.size()) {}

    template <typename U, size_t N,
              WJR_REQUIRES((__is_dynamic || N == dynamic_extent || N == Extent) &&
                           std::is_convertible_v<U *, T *> && !__is_dynamic)>
    constexpr explicit span(const span<U, N> &source) noexcept
        : storage(source.data(), source.size()) {}

    constexpr span(const span &other) noexcept = default;
    constexpr span &operator=(const span &other) noexcept = default;

    ~span() = default;

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
    constexpr iterator begin() noexcept { return __make_iterator(begin_unsafe()); }
    constexpr const_iterator begin() const noexcept {
        return __make_iterator(begin_unsafe());
    }
    constexpr const_iterator cbegin() const noexcept {
        return __make_iterator(begin_unsafe());
    }

    constexpr iterator end() noexcept { return __make_iterator(end_unsafe()); }
    constexpr const_iterator end() const noexcept {
        return __make_iterator(end_unsafe());
    }
    constexpr const_iterator cend() const noexcept {
        return __make_iterator(end_unsafe());
    }

    constexpr reverse_iterator rbegin() noexcept {
        return std::make_reverse_iterator(end());
    }
    constexpr reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(end());
    }
    constexpr const_reverse_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    constexpr reverse_iterator rend() noexcept {
        return std::make_reverse_iterator(begin());
    }
    constexpr reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(begin());
    }
    constexpr const_reverse_iterator crend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    constexpr reference front() const {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(size() > 0, "basic_vector::front: empty");
#endif
        return *data();
    }
    constexpr reference back() const {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(size() > 0, "basic_vector::front: empty");
#endif
        return *(end_unsafe() - 1);
    }

    constexpr reference at(size_type pos) const {
        if (WJR_UNLIKELY(pos >= size())) {
            WJR_THROW(std::out_of_range("span at out of range"));
        }

        return data()[pos];
    }

    constexpr reference operator[](size_type pos) const {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_ASSERT_L0(pos < size(), "basic_vector::operator[]: out of range");
#endif
        return data()[pos];
    }

    constexpr pointer data() const { return storage.ptr; }
    constexpr size_type size() const { return storage.size; }
    constexpr size_type size_bytes() const { return size() * sizeof(element_type); }
    constexpr bool empty() const { return size() == 0; }

    template <size_t Count>
    constexpr span<element_type, Count> first() const {
        static_assert(Count <= Extent, "");

        return {begin(), Count};
    }

    constexpr span<element_type, dynamic_extent> first(size_type Count) const {
        WJR_ASSERT_L2(Count <= size());

        return {begin(), Count};
    }

    template <size_t Count>
    constexpr span<element_type, Count> last() const {
        static_assert(Count <= Extent, "");

        return {end() - Count, Count};
    }

    constexpr span<element_type, dynamic_extent> last(size_type Count) const {
        WJR_ASSERT_L2(Count <= size());

        return {data() - Count, Count};
    }

    template <size_t Offset, size_t Count = dynamic_extent>
    constexpr span<element_type, Count != dynamic_extent    ? Count
                                 : Extent != dynamic_extent ? Extent - Offset
                                                            : dynamic_extent>
    subspan() const {
        if constexpr (Extent != dynamic_extent) {
            static_assert(Offset <= Extent, "");
            static_assert(Count == dynamic_extent || Count <= Extent - Offset, "");
        } else {
            WJR_ASSERT_L2(Offset <= size());
            if constexpr (Count != dynamic_extent) {
                WJR_ASSERT_L2(Count <= size() - Offset);
            }
        }
        return {begin() + Offset, Count == dynamic_extent ? size() - Offset : Count};
    }

    constexpr span<element_type, dynamic_extent>
    subspan(size_type Offset, size_type Count = dynamic_extent) const {
        WJR_ASSERT_L2(Offset <= size());

        return {begin() + Offset, Count == dynamic_extent ? size() - Offset : Count};
    }

    // extension :

    /**
     * @brief Construct a span from a container.
     *
     * @details The container must have a `data()` member function that returns a @ref
     * __is_span_iterator. The container must also have a `size()` member function that
     * can be converted to `size_type`.
     *
     */
    template <typename Container, WJR_REQUIRES(span_details::__is_span_like_v<
                                               Container, element_type> &&__is_dynamic)>
    constexpr span(Container &&c) noexcept : storage(std::data(c), std::size(c)) {}

    /**
     * @brief Construct a span from a container.
     *
     * @details Like @ref span(Container &&), but the span is not dynamic-sized, so the
     * construct must be explicit.
     *
     */
    template <typename Container,
              WJR_REQUIRES(span_details::__is_span_like_v<Container, element_type> &&
                           !__is_dynamic)>
    constexpr explicit span(Container &&c) noexcept
        : storage(std::data(c), std::size(c)) {}

private:
    __storage storage;
};

template <typename T, size_t Extent>
span(T (&)[Extent]) -> span<T, Extent>;

template <typename T, size_t Size>
span(std::array<T, Size> &) -> span<T, Size>;

template <typename T, size_t Size>
span(const std::array<T, Size> &) -> span<const T, Size>;

template <typename It, typename End, WJR_REQUIRES(is_contiguous_iterator_v<It>)>
span(It, End) -> span<iterator_contiguous_value_t<It>>;

template <typename Container,
          WJR_REQUIRES(span_details::__is_container_like_v<Container>)>
span(Container &&) -> span<
    iterator_contiguous_value_t<decltype(std::data(std::declval<Container &>()))>>;

} // namespace wjr

#endif // WJR_SPAN_HPP__