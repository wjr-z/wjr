#ifndef WJR_SPAN_HPP__
#define WJR_SPAN_HPP__

#include <stdexcept>

#include <wjr/compressed_pair.hpp>

namespace wjr {

inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

template <typename T, size_t Extent>
struct __span_static_storage {

    __span_static_storage() = default;
    __span_static_storage(const __span_static_storage &) = default;
    __span_static_storage &operator=(const __span_static_storage &) = default;

    __span_static_storage(T *p, size_t s) : ptr(p) { WJR_ASSERT(s == size); }

    T *ptr = nullptr;
    static constexpr size_t size = Extent;
};

template <typename T>
struct __span_dynamic_storage {

    __span_dynamic_storage() = default;
    __span_dynamic_storage(const __span_dynamic_storage &) = default;
    __span_dynamic_storage &operator=(const __span_dynamic_storage &) = default;

    __span_dynamic_storage(T *p, size_t s) : ptr(p), size(s) {}

    T *ptr = nullptr;
    size_t size = 0;
};

template <typename Iter, typename Elem>
struct __is_span_iterator
    : std::conjunction<is_contiguous_iterator<Iter>,
                       std::is_convertible<
                           std::remove_reference_t<iter_reference_t<Iter>> *, Elem *>> {};

template <typename Array, typename Elem, typename = void>
struct __is_span_array_helper : std::false_type {};

template <typename Array, typename Elem>
struct __is_span_array_helper<Array, Elem,
                              std::void_t<decltype(std::data(std::declval<Array>()))>>
    : std::is_convertible<std::remove_reference_t<std::remove_pointer_t<
                              decltype(std::data(std::declval<Array>()))>> *,
                          Elem *> {};

template <typename Array, typename Elem>
struct __is_span_array : __is_span_array_helper<Array, Elem, void> {};

template <typename T, size_t Extent = dynamic_extent>
class span {
    static constexpr bool __is_dynamic = Extent == dynamic_extent;
    using __storage = std::conditional_t<__is_dynamic, __span_dynamic_storage<T>,
                                         __span_static_storage<T, Extent>>;

public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    template <size_t Ex = Extent,
              std::enable_if_t<Ex == dynamic_extent || Ex == 0, int> = 0>
    constexpr span() noexcept : storage() {}

    template <typename It,
              std::enable_if_t<
                  __is_span_iterator<It, element_type>::value && __is_dynamic, int> = 0>
    constexpr span(It first, size_type count) : storage(to_address(first), count) {}

    template <typename It,
              std::enable_if_t<
                  __is_span_iterator<It, element_type>::value && !__is_dynamic, int> = 0>
    constexpr explicit span(It first, size_type count)
        : storage(to_address(first), count) {}

    template <typename It,
              std::enable_if_t<
                  __is_span_iterator<It, element_type>::value && __is_dynamic, int> = 0>
    constexpr span(It first, It last)
        : storage(to_address(first), static_cast<size_type>(last - first)) {}

    template <typename It,
              std::enable_if_t<
                  __is_span_iterator<It, element_type>::value && !__is_dynamic, int> = 0>
    constexpr explicit span(It first, It last)
        : storage(to_address(first), static_cast<size_type>(last - first)) {}

    template <size_t N,
              std::enable_if_t<(__is_dynamic || N == Extent) &&
                                   __is_span_array<type_identity_t<element_type> (&)[N],
                                                   element_type>::value,
                               int> = 0>
    constexpr span(type_identity_t<element_type> (&arr)[N]) noexcept
        : storage(std::data(arr), N) {}

    template <
        typename U, size_t N,
        std::enable_if_t<(__is_dynamic || N == Extent) &&
                             __is_span_array<std::array<U, N> &, element_type>::value,
                         int> = 0>
    constexpr span(std::array<U, N> &arr) noexcept
        : storage(std::data(arr), std::size(arr)) {}

    template <typename U, size_t N,
              std::enable_if_t<
                  (__is_dynamic || N == Extent) &&
                      __is_span_array<const std::array<U, N> &, element_type>::value,
                  int> = 0>
    constexpr span(const std::array<U, N> &arr) noexcept
        : storage(std::data(arr), std::size(arr)) {}

    template <
        size_t Ex = Extent,
        std::enable_if_t<std::is_const_v<element_type> && Ex == dynamic_extent, int> = 0>
    constexpr span(std::initializer_list<value_type> il) noexcept
        : storage(std::data(il), std::size(il)) {}

    template <
        size_t Ex = Extent,
        std::enable_if_t<std::is_const_v<element_type> && Ex != dynamic_extent, int> = 0>
    constexpr explicit span(std::initializer_list<value_type> il) noexcept
        : storage(std::data(il), std::size(il)) {}

    template <typename U, size_t N,
              std::enable_if_t<(__is_dynamic || N == dynamic_extent || N == Extent) &&
                                   std::is_convertible_v<U *, T *> && __is_dynamic,
                               int> = 0>
    constexpr span(const span<U, N> &source) noexcept
        : storage(source.data(), source.size()) {}

    template <typename U, size_t N,
              std::enable_if_t<(__is_dynamic || N == dynamic_extent || N == Extent) &&
                                   std::is_convertible_v<U *, T *> && !__is_dynamic,
                               int> = 0>
    constexpr explicit span(const span<U, N> &source) noexcept
        : storage(source.data(), source.size()) {}

    constexpr span(const span &other) noexcept = default;

    constexpr span &operator=(const span &other) noexcept = default;

    ~span() = default;

    constexpr iterator begin() const noexcept { return data(); }
    constexpr const_iterator cbegin() const noexcept { return begin(); }

    constexpr iterator end() const noexcept { return begin() + size(); }
    constexpr const_iterator cend() const noexcept { return end(); }

    constexpr reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(end());
    }
    constexpr const_reverse_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    constexpr reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(begin());
    }
    constexpr const_reverse_iterator crend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    constexpr reference front() const { return *begin(); }
    constexpr reference back() const { return *(--end()); }

    constexpr reference at(size_type pos) const {
        if (WJR_UNLIKELY(pos >= size())) {
            throw std::out_of_range("span at out of range");
        }

        return this->operator[](pos);
    }

    constexpr reference operator[](size_type pos) const { return data()[pos]; }

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
        WJR_ASSERT(Count <= size());

        return {begin(), Count};
    }

    template <size_t Count>
    constexpr span<element_type, Count> last() const {
        static_assert(Count <= Extent, "");

        return {end() - Count, Count};
    }

    constexpr span<element_type, dynamic_extent> last(size_type Count) const {
        WJR_ASSERT(Count <= size());

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
            WJR_ASSERT(Offset <= size());
            if constexpr (Count != dynamic_extent) {
                WJR_ASSERT(Count <= size() - Offset);
            }
        }
        return {begin() + Offset, Count == dynamic_extent ? size() - Offset : Count};
    }

    constexpr span<element_type, dynamic_extent>
    subspan(size_type Offset, size_type Count = dynamic_extent) const {
        WJR_ASSERT(Offset <= size());

        return {begin() + Offset, Count == dynamic_extent ? size() - Offset : Count};
    }

private:
    __storage storage;
};

template <typename T, size_t Extent>
span(T (&)[Extent]) -> span<T, Extent>;

template <typename T, size_t Size>
span(std::array<T, Size> &) -> span<T, Size>;

template <typename T, size_t Size>
span(const std::array<T, Size> &) -> span<T, Size>;

template <typename It, typename End,
          std::enable_if_t<is_contiguous_iterator_v<It>, int> = 0>
span(It, End) -> span<std::remove_reference_t<iter_reference_t<It>>>;

} // namespace wjr

#endif // WJR_SPAN_HPP__