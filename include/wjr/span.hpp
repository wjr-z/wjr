/**
 * @file span.hpp
 * @brief Non-owning view over a contiguous sequence of objects
 * @author wjr
 *
 * Provides span, a lightweight view similar to C++20 std::span. Can be used
 * with both static and dynamic extent for compile-time or runtime sizing.
 */

#ifndef WJR_SPAN_HPP__
#define WJR_SPAN_HPP__

#include <stdexcept>

#include <wjr/assert.hpp>
#include <wjr/iterator/contiguous_iterator_adapter.hpp>

#include <range/v3/range/concepts.hpp>

namespace wjr {

/// @private Internal storage for static-sized span
template <typename T, size_t Extent>
struct _span_static_storage {

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(_span_static_storage);

    constexpr _span_static_storage(T *p, WJR_MAYBE_UNUSED size_t s) noexcept : ptr(p) {
        WJR_ASSERT_L2(s == size);
    }

    T *ptr = nullptr;
    static constexpr size_t size = Extent;
};

/// @private Internal storage for dynamic-sized span
template <typename T>
struct _span_dynamic_storage {

    WJR_ENABLE_DEFAULT_SPECIAL_MEMBERS(_span_dynamic_storage);

    constexpr _span_dynamic_storage(T *p, size_t s) noexcept : ptr(p), size(s) {}

    T *ptr = nullptr;
    size_t size = 0;
};

template <typename Iter, typename Elem>
struct _is_span_iterator
    : std::conjunction<is_contiguous_iterator<Iter>,
                       std::is_convertible<iterator_contiguous_pointer_t<Iter>, Elem *>> {};

template <typename T, size_t Extent = dynamic_extent>
class span;

namespace span_detail {

/// @private
template <typename T>
struct _is_std_array : std::false_type {};

/// @private
template <typename T, size_t N>
struct _is_std_array<std::array<T, N>> : std::true_type {};

/// @private
template <typename T>
inline constexpr bool _is_std_array_v = _is_std_array<T>::value;

template <typename T>
struct _is_span : std::false_type {};

template <typename T, size_t Extent>
struct _is_span<span<T, Extent>> : std::true_type {};

template <typename T>
inline constexpr bool _is_span_v = _is_span<T>::value;

template <typename Elem, typename Ref>
using _is_compatible_ref = _is_array_convertible<Elem, std::remove_reference_t<Ref>>;

CPP_template(typename Range,
             typename Elem)(concept(_is_range_compatible_ref_)(Range, Elem),
                            _is_compatible_ref<Elem, ranges::range_reference_t<Range>>::value);

template <typename Range, typename Elem>
CPP_concept _is_range_like_v =
    (!_is_span_v<remove_cvref_t<Range>>) && (!_is_std_array_v<remove_cvref_t<Range>>) &&
    (!_is_std_array_v<remove_cvref_t<Range>>) && ranges::contiguous_range<Range> &&
    ranges::sized_range<Range> && (ranges::borrowed_range<Range> || std::is_const_v<Elem>) &&
    CPP_concept_ref(_is_range_compatible_ref_, Range, Elem);

/// @private
template <typename T>
struct basic_span_traits {
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
};

} // namespace span_detail

/**
 * @class span
 *
 * @brief A view over a contiguous sequence of objectsd.
 *
 * @tparam Extent if Extent is `dynamic_extent`, the span is a runtime-sized
 * view. Otherwise, the span is a compile-time-sized view.
 */
template <typename T, size_t Extent>
class span {
    static constexpr bool _is_dynamic = Extent == dynamic_extent;
    using _storage =
        std::conditional_t<_is_dynamic, _span_dynamic_storage<T>, _span_static_storage<T, Extent>>;

    using IteratorTraits = span_detail::basic_span_traits<T>;

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

#if defined(__cpp_conditional_explicit)
    template <typename It, WJR_REQUIRES(_is_span_iterator<It, element_type>::value)>
    constexpr explicit(!_is_dynamic) span(It first, size_type count) noexcept
        : storage(wjr::to_address(first), count) {}
#else
    template <typename It, WJR_REQUIRES(_is_span_iterator<It, element_type>::value &&_is_dynamic)>
    constexpr span(It first, size_type count) noexcept : storage(wjr::to_address(first), count) {}

    template <typename It, WJR_REQUIRES(_is_span_iterator<It, element_type>::value && !_is_dynamic)>
    constexpr explicit span(It first, size_type count) noexcept
        : storage(wjr::to_address(first), count) {}
#endif

#if defined(__cpp_conditional_explicit)
    template <typename It, WJR_REQUIRES(_is_span_iterator<It, element_type>::value)>
    constexpr explicit(!_is_dynamic) span(It first, It last) noexcept
        : storage(wjr::to_address(first), static_cast<size_type>(last - first)) {}
#else
    template <typename It, WJR_REQUIRES(_is_span_iterator<It, element_type>::value &&_is_dynamic)>
    constexpr span(It first, It last) noexcept
        : storage(wjr::to_address(first), static_cast<size_type>(last - first)) {}

    template <typename It, WJR_REQUIRES(_is_span_iterator<It, element_type>::value && !_is_dynamic)>
    constexpr explicit span(It first, It last) noexcept
        : storage(wjr::to_address(first), static_cast<size_type>(last - first)) {}
#endif

    template <size_t N, WJR_REQUIRES((_is_dynamic || N == Extent))>
    constexpr span(type_identity_t<element_type> (&arr)[N]) noexcept : storage(std::data(arr), N) {}

    template <typename U, size_t N,
              WJR_REQUIRES((_is_dynamic || N == Extent) && std::is_convertible_v<U *, T *>)>
    constexpr span(std::array<U, N> &arr) noexcept : storage(std::data(arr), std::size(arr)) {}

    template <typename U, size_t N,
              WJR_REQUIRES((_is_dynamic || N == Extent) && std::is_convertible_v<const U *, T *>)>
    constexpr span(const std::array<U, N> &arr) noexcept
        : storage(std::data(arr), std::size(arr)) {}

#if defined(__cpp_conditional_explicit)
    template <typename U, size_t N,
              WJR_REQUIRES((_is_dynamic || N == dynamic_extent || N == Extent) &&
                           std::is_convertible_v<U *, T *>)>
    constexpr explicit(!_is_dynamic) span(const span<U, N> &source) noexcept
        : storage(source.data(), source.size()) {}
#else
    template <typename U, size_t N,
              WJR_REQUIRES((_is_dynamic || N == dynamic_extent || N == Extent) &&
                           std::is_convertible_v<U *, T *> && _is_dynamic)>
    constexpr span(const span<U, N> &source) noexcept : storage(source.data(), source.size()) {}

    template <typename U, size_t N,
              WJR_REQUIRES((_is_dynamic || N == dynamic_extent || N == Extent) &&
                           std::is_convertible_v<U *, T *> && !_is_dynamic)>
    constexpr explicit span(const span<U, N> &source) noexcept
        : storage(source.data(), source.size()) {}
#endif

#if WJR_HAS_GCC(9, 0, 0)
    // Disable gcc's warning in this constructor as it generates an enormous amount
    // of messages. Anyone using ArrayRef should already be aware of the fact that
    // it does not do lifetime extension.
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Winit-list-lifetime"
#endif

#if defined(__cpp_conditional_explicit)
    constexpr explicit(!_is_dynamic) span(std::initializer_list<value_type> il) noexcept
        : storage(il.begin(), il.size()) {}
#else
    template <size_t E = Extent, WJR_REQUIRES(E == dynamic_extent)>
    constexpr span(std::initializer_list<value_type> il) noexcept
        : storage(il.begin(), il.size()) {}

    template <size_t E = Extent, WJR_REQUIRES(E != dynamic_extent)>
    constexpr explicit span(std::initializer_list<value_type> il) noexcept
        : storage(il.begin(), il.size()) {}
#endif

#if WJR_HAS_GCC(9, 0, 0)
    #pragma GCC diagnostic pop
#endif

    span(const span &) = default;
    span(span &&) = default;
    span &operator=(const span &) = default;
    span &operator=(span &&) = default;
    ~span() = default;

    WJR_CONSTEXPR20 pointer begin_unsafe() noexcept { return data(); }
    WJR_CONSTEXPR20 const_pointer begin_unsafe() const noexcept { return data(); }
    WJR_CONSTEXPR20 const_pointer cbegin_unsafe() const noexcept { return data(); }

    WJR_CONSTEXPR20 pointer end_unsafe() noexcept { return data() + size(); }
    WJR_CONSTEXPR20 const_pointer end_unsafe() const noexcept { return data() + size(); }
    WJR_CONSTEXPR20 const_pointer cend_unsafe() const noexcept { return end_unsafe(); }

private:
    WJR_PURE WJR_CONSTEXPR20 iterator _make_iterator(const_pointer ptr) const noexcept {
        return iterator(const_cast<pointer>(ptr), this);
    }

    WJR_PURE WJR_CONSTEXPR20 pointer _get_pointer(iterator ptr) const noexcept {
        ptr.check_same_container(this);
        return wjr::to_address(ptr);
    }

    WJR_PURE WJR_CONSTEXPR20 pointer _get_pointer(const_iterator ptr) const noexcept {
        ptr.check_same_container(this);
        return const_cast<pointer>(wjr::to_address(ptr));
    }

public:
    constexpr iterator begin() noexcept { return _make_iterator(begin_unsafe()); }
    constexpr const_iterator begin() const noexcept { return _make_iterator(begin_unsafe()); }
    constexpr const_iterator cbegin() const noexcept { return _make_iterator(begin_unsafe()); }

    constexpr iterator end() noexcept { return _make_iterator(end_unsafe()); }
    constexpr const_iterator end() const noexcept { return _make_iterator(end_unsafe()); }
    constexpr const_iterator cend() const noexcept { return _make_iterator(end_unsafe()); }

    constexpr reverse_iterator rbegin() noexcept { return std::make_reverse_iterator(end()); }
    constexpr reverse_iterator rbegin() const noexcept { return std::make_reverse_iterator(end()); }
    constexpr const_reverse_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    constexpr reverse_iterator rend() noexcept { return std::make_reverse_iterator(begin()); }
    constexpr reverse_iterator rend() const noexcept { return std::make_reverse_iterator(begin()); }
    constexpr const_reverse_iterator crend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

    constexpr reference front() const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(size() > 0, "span::front: empty");
#endif
        return *data();
    }
    constexpr reference back() const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(size() > 0, "span::front: empty");
#endif
        return *(end_unsafe() - 1);
    }

    constexpr reference at(size_type pos) const {
        if (WJR_UNLIKELY(pos >= size())) {
            WJR_THROW(std::out_of_range("span at out of range"));
        }

        return data()[pos];
    }

    constexpr reference operator[](size_type pos) const noexcept {
#if WJR_HAS_DEBUG(CONTIGUOUS_ITERATOR_CHECKER)
        WJR_CHECK(pos < size(), "span::operator[]: out of range");
#endif
        return data()[pos];
    }

    WJR_PURE constexpr pointer data() const noexcept { return storage.ptr; }

    WJR_PURE constexpr size_type size() const noexcept { return storage.size; }

    WJR_PURE constexpr size_type size_bytes() const noexcept {
        return size() * sizeof(element_type);
    }

    WJR_PURE constexpr bool empty() const noexcept { return size() == 0; }

    template <size_t Count>
    constexpr span<element_type, Count> first() const noexcept {
        static_assert(Count <= Extent);

        return {begin(), Count};
    }

    constexpr span<element_type, dynamic_extent> first(size_type Count) const noexcept {
        WJR_ASSERT_L2(Count <= size());

        return {begin(), Count};
    }

    template <size_t Count>
    constexpr span<element_type, Count> last() const noexcept {
        static_assert(Count <= Extent);

        return {end() - Count, Count};
    }

    constexpr span<element_type, dynamic_extent> last(size_type Count) const noexcept {
        WJR_ASSERT_L2(Count <= size());

        return {data() - Count, Count};
    }

    template <size_t Offset, size_t Count = dynamic_extent>
    constexpr span<element_type, Count != dynamic_extent    ? Count
                                 : Extent != dynamic_extent ? Extent - Offset
                                                            : dynamic_extent>
    subspan() const noexcept {
        if constexpr (Extent != dynamic_extent) {
            static_assert(Offset <= Extent);
            static_assert(Count == dynamic_extent || Count <= Extent - Offset);
        } else {
            WJR_ASSERT_L2(Offset <= size());
            if constexpr (Count != dynamic_extent) {
                WJR_ASSERT_L2(Count <= size() - Offset);
            }
        }
        return {begin() + Offset, Count == dynamic_extent ? size() - Offset : Count};
    }

    constexpr span<element_type, dynamic_extent>
    subspan(size_type Offset, size_type Count = dynamic_extent) const noexcept {
        WJR_ASSERT_L2(Offset <= size());

        return {begin() + Offset, Count == dynamic_extent ? size() - Offset : Count};
    }

    // extension :

    template <typename Range,
              WJR_REQUIRES(span_detail::_is_range_like_v<Range, element_type> &&_is_dynamic)>
    constexpr span(Range &&rg) noexcept : storage(ranges::data(rg), ranges::size(rg)) {}

    template <typename Range,
              WJR_REQUIRES(span_detail::_is_range_like_v<Range, element_type> && !_is_dynamic)>
    constexpr explicit span(Range &&rg) noexcept : storage(ranges::data(rg), ranges::size(rg)) {}

private:
    _storage storage;
};

template <typename T, size_t Extent>
span(T (&)[Extent]) -> span<T, Extent>;

template <typename T, size_t Size>
span(std::array<T, Size> &) -> span<T, Size>;

template <typename T, size_t Size>
span(const std::array<T, Size> &) -> span<const T, Size>;

template <typename It, typename End, WJR_REQUIRES(is_contiguous_iterator_v<It>)>
span(It, End) -> span<iterator_contiguous_value_t<It>>;

template <typename Range, WJR_REQUIRES(ranges::contiguous_range<Range>)>
span(Range &&) -> span<std::remove_reference_t<ranges::range_reference_t<Range &>>>;

} // namespace wjr

#endif // WJR_SPAN_HPP__
