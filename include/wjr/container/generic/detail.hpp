#ifndef WJR_CONTAINER_GENERIC_DETAIL_HPP__
#define WJR_CONTAINER_GENERIC_DETAIL_HPP__

#include <string>

#include <wjr/iterator/detail.hpp>

namespace wjr {

WJR_REGISTER_HAS_TYPE(__container_resize,
                      std::declval<std::add_lvalue_reference_t<Container>>().resize(
                          std::declval<Size>(), std::declval<Args>()...),
                      Container, Size);
WJR_REGISTER_HAS_TYPE(__container_append,
                      std::declval<Container>().append(std::declval<Args>()...),
                      Container);

template <typename Container>
struct resize_fn_impl_base {
    template <typename... Args,
              WJR_REQUIRES(has___container_resize_v<Container, Args...>)>
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
    void operator()(Container &cont, Args &&...args) const noexcept(
        noexcept(resize_fn_impl<Container>::resize(cont, std::forward<Args>(args)...))) {
        resize_fn_impl<Container>::resize(cont, std::forward<Args>(args)...);
    }
};

inline constexpr resize_fn resize{};

template <typename Container>
struct append_fn_impl_base {
    template <typename... Args,
              WJR_REQUIRES(has___container_append_v<Container, Args...>)>
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
    void operator()(Container &cont, Args &&...args) const noexcept(
        noexcept(append_fn_impl<Container>::append(cont, std::forward<Args>(args)...))) {
        append_fn_impl<Container>::append(cont, std::forward<Args>(args)...);
    }
};

inline constexpr append_fn append{};

#define WJR_HAS_FEATURE_STRING_UNINITIALIZED_RESIZE WJR_HAS_DEF

#ifdef __cpp_lib_string_resize_and_overwrite
#define WJR_STRINF_RESIZE_AND_OVERWRITE __cpp_lib_string_resize_and_overwrite
#else
#define WJR_STRINF_RESIZE_AND_OVERWRITE 0
#endif

#if WJR_STRINF_RESIZE_AND_OVERWRITE >= 202110L
template <typename CharT, typename Traits, typename Alloc>
WJR_INTRINSIC_INLINE void
__uninitialized_resize(std::basic_string<CharT, Traits, Alloc> &str,
                       typename std::basic_string<CharT, Traits, Alloc>::size_type sz) {
    using Size = typename std::basic_string<CharT, Traits, Alloc>::size_type;
    str.resize_and_overwrite(sz, [](char *, Size sz) { return sz; });
}

#define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_CLASS(...)
#define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(...)
#define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_HACKER(...)

#elif (defined(__clang_major__) && __clang_major__ <= 11) ||                             \
    (defined(_MSC_VER) && _MSC_VER <= 1920)
#undef WJR_HAS_FEATURE_STRING_UNINITIALIZED_RESIZE
#elif defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(_MSVC_STL_VERSION)

template <typename Container>
void string_set_length_hacker(Container &bank, typename Container::size_type sz);

#if defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)
#define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_CLASS(Name, Container)                \
    inline void WJR_PP_CONCAT(string_set_length_hacker_of_,                              \
                              Name)(Container & bank, typename Container::size_type sz); \
    template <typename Money_t, Money_t Container::*p>                                   \
    struct WJR_PP_CONCAT(string_thief_of_, Name) {                                       \
        friend void WJR_PP_CONCAT(string_set_length_hacker_of_,                          \
                                  Name)(Container & bank,                                \
                                        typename Container::size_type sz) {              \
            (bank.*p)(sz);                                                               \
        }                                                                                \
    }
#else
#define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_CLASS(Name, Container)                \
    inline void WJR_PP_CONCAT(string_set_length_hacker_of_,                              \
                              Name)(Container & bank, typename Container::size_type sz); \
    template <typename Money_t, Money_t Container::*p>                                   \
    struct WJR_PP_CONCAT(string_thief_of_, Name) {                                       \
        friend void WJR_PP_CONCAT(string_set_length_hacker_of_,                          \
                                  Name)(Container & bank,                                \
                                        typename Container::size_type sz) {              \
            (bank.*p)._Myval2._Mysize = sz;                                              \
        }                                                                                \
    }
#endif

#define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_HACKER(Name, Container)               \
    template <>                                                                          \
    inline void string_set_length_hacker<Container>(Container & bank,                    \
                                                    typename Container::size_type sz) {  \
        WJR_PP_CONCAT(string_set_length_hacker_of_, Name)(bank, sz);                     \
    };

#if defined(__GLIBCXX__)
#define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(Name, Container)             \
    template struct WJR_PP_CONCAT(                                                       \
        string_thief_of_, Name)<void(Container::size_type), &Container::_M_set_length>
#elif defined(_LIBCPP_VERSION)
#define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(Name, Container)             \
    template struct WJR_PP_CONCAT(                                                       \
        string_thief_of_, Name)<void(Container::size_type), &Container::__set_size>
#else
#define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(Name, Container)             \
    template struct WJR_PP_CONCAT(                                                       \
        string_thief_of_, Name)<decltype(Container::_Mypair), &Container::_Mypair>
#endif

template <typename CharT, typename Traits, typename Alloc>
WJR_INTRINSIC_INLINE void
__uninitialized_resize(std::basic_string<CharT, Traits, Alloc> &str,
                       typename std::basic_string<CharT, Traits, Alloc>::size_type sz) {
    str.reserve(sz);
    string_set_length_hacker(str, sz);
    WJR_ASSERT_L2(str.size() == sz);
    str[sz] = '\0';
}

#else
#undef WJR_HAS_FEATURE_STRING_UNINITIALIZED_RESIZE
#define WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(Name, Container)
#endif

#if WJR_HAS_FEATURE(STRING_UNINITIALIZED_RESIZE)

template <typename Container>
struct __uninitialized_resize_fn_impl : resize_fn_impl_base<Container> {
    using resize_fn_impl_base<Container>::resize;
    WJR_INTRINSIC_INLINE static void resize(Container &cont,
                                            typename Container::size_type sz, dctor_t) {
        __uninitialized_resize(cont, sz);
    }
};

template <typename Container>
struct __uninitialized_append_fn_impl : append_fn_impl_base<Container> {
    using append_fn_impl_base<Container>::append;
    WJR_INTRINSIC_INLINE static void append(Container &cont,
                                            typename Container::size_type sz, dctor_t) {
        __uninitialized_resize(cont, cont.size() + sz);
    }
};

#define WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(Name, Container)                        \
    __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_CLASS(Name, Container);                   \
    __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(Name, Container);                \
    __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_HACKER(Name, Container);                  \
    template <>                                                                          \
    struct resize_fn_impl<Container> : __uninitialized_resize_fn_impl<Container> {};     \
    template <>                                                                          \
    struct append_fn_impl<Container> : __uninitialized_append_fn_impl<Container> {}
#else
#define WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(Name, Container)
#endif

WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(std_string, std::string);

WJR_REGISTER_HAS_TYPE(container_resize,
                      resize_fn_impl<Container>::resize(std::declval<Container &>(),
                                                        std::declval<Size>(),
                                                        std::declval<Args>()...),
                      Container, Size);

WJR_REGISTER_HAS_TYPE(container_reserve,
                      std::declval<Container>().reserve(std::declval<Size>()), Container,
                      Size);
WJR_REGISTER_HAS_TYPE(container_append,
                      append_fn_impl<Container>::append(std::declval<Container &>(),
                                                        std::declval<Args>()...),
                      Container);
WJR_REGISTER_HAS_TYPE(container_insert,
                      (std::declval<Container>().insert(
                           std::declval<Container>().cbegin(), std::declval<Args>()...),
                       std::declval<Container>().insert(std::declval<Container>().cend(),
                                                        std::declval<Args>()...)),
                      Container);

template <typename Container, typename Size,
          WJR_REQUIRES(has_container_resize_v<Container, Size>)>
WJR_INTRINSIC_INLINE void try_uninitialized_resize(Container &cont, Size sz) {
    if constexpr (has_container_resize_v<Container, Size, dctor_t>) {
        resize(cont, sz, dctor);
    } else {
        resize(cont, sz);
    }
}

/// @private
template <typename T, typename = void>
struct __container_traits_base_iterator_helper {
    using iterator = T;
};

/// @private
template <typename T>
struct __container_traits_base_iterator_helper<T, std::void_t<typename T::iterator>> {
    using iterator = typename T::iterator;
};

/// @private
template <typename T, typename = void>
struct __container_traits_base_size_type_helper {
    using size_type = size_t;
};

/// @private
template <typename T>
struct __container_traits_base_size_type_helper<T, std::void_t<typename T::size_type>> {
    using size_type = typename T::size_type;
};

/// @private
template <typename Container>
struct __container_traits_base {
private:
    using iterator =
        typename __container_traits_base_iterator_helper<Container>::iterator;
    using size_type =
        typename __container_traits_base_size_type_helper<Container>::size_type;

public:
    constexpr static bool is_contiguous_v = is_contiguous_iterator_v<iterator>;

    /**
     * @details Trivially contiguous means that the container can be resized and then
     * filled, and the result should be consistent with the element by element push_back
     * result. It does not verify whether the element is trial. Because different
     * containers may have different ways of constructing elements. The main purpose is
     * for types like std::basic_string<CharT, Traits, Alloc>, and for unknown
     * Traits, it should not be assumed that filling after resizing yields the same
     * result as using Traits::copy.
     *
     */
    constexpr static bool is_trivially_contiguous_v = false;
};

template <typename Container>
struct container_traits;

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_DETAIL_HPP__