/**
 * @file container/detail.hpp
 * @brief Container operation function objects
 * @author wjr
 *
 * Provides generic function objects for container operations like
 * resize, append, reserve, and insert with customization point support.
 */

#ifndef WJR_CONTAINER_DETAIL_HPP__
#define WJR_CONTAINER_DETAIL_HPP__

#include <wjr/iterator/detail.hpp>

namespace wjr {

WJR_REGISTER_HAS_TYPE(_container_resize,
                      std::declval<std::add_lvalue_reference_t<Container>>().resize(
                          std::declval<Size>(), std::declval<Args>()...),
                      Container, Size);
WJR_REGISTER_HAS_TYPE(_container_append, std::declval<Container>().append(std::declval<Args>()...),
                      Container);

template <typename Container>
struct resize_fn_impl_base {
    template <typename... Args, WJR_REQUIRES(has__container_resize_v<Container, Args...>)>
    WJR_INTRINSIC_INLINE static void
    resize(Container &cont,
           Args &&...args) noexcept(noexcept(cont.resize(std::forward<Args>(args)...))) {
        cont.resize(std::forward<Args>(args)...);
    }
};

template <typename Container>
struct resize_fn_impl : resize_fn_impl_base<Container> {};

struct resize_fn {
    template <typename Container, typename... Args>
    void operator()(Container &cont, Args &&...args) const
        noexcept(noexcept(resize_fn_impl<Container>::resize(cont, std::forward<Args>(args)...))) {
        resize_fn_impl<Container>::resize(cont, std::forward<Args>(args)...);
    }
};

inline constexpr resize_fn resize{};

template <typename Container>
struct append_fn_impl_base {
    template <typename... Args, WJR_REQUIRES(has__container_append_v<Container, Args...>)>
    WJR_INTRINSIC_INLINE static void
    append(Container &cont,
           Args &&...args) noexcept(noexcept(cont.append(std::forward<Args>(args)...))) {
        cont.append(std::forward<Args>(args)...);
    }
};

template <typename Container>
struct append_fn_impl : append_fn_impl_base<Container> {};

struct append_fn {
    template <typename Container, typename... Args>
    void operator()(Container &cont, Args &&...args) const
        noexcept(noexcept(append_fn_impl<Container>::append(cont, std::forward<Args>(args)...))) {
        append_fn_impl<Container>::append(cont, std::forward<Args>(args)...);
    }
};

inline constexpr append_fn append{};

WJR_REGISTER_HAS_TYPE(container_resize,
                      resize_fn_impl<Container>::resize(std::declval<Container &>(),
                                                        std::declval<Size>(),
                                                        std::declval<Args>()...),
                      Container, Size);

WJR_REGISTER_HAS_TYPE(container_reserve, std::declval<Container>().reserve(std::declval<Size>()),
                      Container, Size);
WJR_REGISTER_HAS_TYPE(container_append,
                      append_fn_impl<Container>::append(std::declval<Container &>(),
                                                        std::declval<Args>()...),
                      Container);
WJR_REGISTER_HAS_TYPE(
    container_insert,
    (std::declval<Container>().insert(std::declval<Container>().cbegin(), std::declval<Args>()...),
     std::declval<Container>().insert(std::declval<Container>().cend(), std::declval<Args>()...)),
    Container);

template <typename Container, typename Size, WJR_REQUIRES(has_container_resize_v<Container, Size>)>
WJR_INTRINSIC_INLINE void try_uninitialized_resize(Container &cont, Size sz) {
    if constexpr (has_container_resize_v<Container, Size, default_construct_t>) {
        resize(cont, sz, default_construct);
    } else {
        resize(cont, sz);
    }
}

template <typename Container, typename Size,
          WJR_REQUIRES(has_container_append_v<Container, Size> ||
                       has_container_resize_v<Container, Size>)>
WJR_INTRINSIC_INLINE void try_uninitialized_append(Container &cont, Size sz) {
    if constexpr (has_container_append_v<Container, Size, default_construct_t>) {
        append(cont, sz, default_construct);
    } else {
        try_uninitialized_resize(cont, cont.size() + sz);
    }
}

/// @private
template <typename T, typename = void>
struct _container_traits_base_iterator_helper {
    using iterator = T;
};

/// @private
template <typename T>
struct _container_traits_base_iterator_helper<T, std::void_t<typename T::iterator>> {
    using iterator = typename T::iterator;
};

/// @private
template <typename Container>
struct _container_traits_base {
private:
    using iterator = typename _container_traits_base_iterator_helper<Container>::iterator;

public:
    /// @todo Use ranges::contiguous_range to check if the container is contiguous
    constexpr static bool is_contiguous_v = is_contiguous_iterator_v<iterator>;

    /**
     * @details Trivially contiguous means that the container can be resized and
     * then filled, and the result should be consistent with the element by
     * element push_back result. It does not verify whether the element is
     * trial. Because different containers may have different ways of
     * constructing elements. The main purpose is for types like
     * std::basic_string<CharT, Traits, Alloc>, and for unknown Traits, it
     * should not be assumed that filling after resizing yields the same result
     * as using Traits::copy.
     *
     */
    constexpr static bool is_trivially_contiguous_v = is_contiguous_v;
};

template <typename Container>
struct container_traits;

} // namespace wjr

#endif // WJR_CONTAINER_DETAIL_HPP__