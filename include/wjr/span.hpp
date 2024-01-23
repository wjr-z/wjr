#ifndef WJR_SPAN_HPP__
#define WJR_SPAN_HPP__

#include <wjr/compressed_pair.hpp>

namespace wjr {

inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

template <typename T, size_t Extend>
struct __span_static_storage {

    __span_static_storage() = default;
    __span_static_storage(const __span_static_storage &) = default;
    __span_static_storage &operator=(const __span_static_storage &) = default;

    __span_static_storage(T *p, size_t s) : ptr(p) { WJR_ASSERT(s == size); }

    T *ptr;
    static constexpr size_t size = Extend;
};

template <typename T>
struct __span_dynamic_storage {

    __span_dynamic_storage() = default;
    __span_dynamic_storage(const __span_dynamic_storage &) = default;
    __span_dynamic_storage &operator=(const __span_dynamic_storage &) = default;

    __span_dynamic_storage(T *p, size_t s) : ptr(p), size(s) {}

    T *ptr;
    size_t size = 0;
};

template <typename Iter, typename T>
struct __is_span_iterator
    : std::conjunction<is_contiguous_iterator<Iter>,
                       std::is_convertible<iter_reference_t<Iter>, T>> {};

template <typename T, size_t Extend = dynamic_extent>
class span {
    static constexpr bool __is_dynamic = Extend == dynamic_extent;
    using __storage = std::conditional_t<__is_dynamic, __span_dynamic_storage<T>,
                                         __span_static_storage<T, Extend>>;

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

    template <size_t Ex = Extend,
              std::enable_if_t<Ex == dynamic_extent || Ex == 0, int> = 0>
    constexpr span() noexcept : storage() {}

    template <typename It = T,
              std::enable_if_t<__is_span_iterator<It, T>::value && __is_dynamic, int> = 0>
    constexpr span(It First, size_type Count) : storage(to_address(First), Count) {}

    template <
        typename It,
        std::enable_if_t<__is_span_iterator<It, T>::value && !__is_dynamic, int> = 0>
    constexpr explicit span(It First, size_type Count)
        : storage(to_address(First), Count) {}

private:
    __storage storage;
};

} // namespace wjr

#endif // WJR_SPAN_HPP__