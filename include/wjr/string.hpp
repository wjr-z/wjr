#ifndef WJR_STRING_HPP__
#define WJR_STRING_HPP__

#include <wjr/container/detail.hpp>
#include <wjr/memory/memory_pool.hpp>

namespace wjr {

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

#elif (defined(__clang_major__) && __clang_major__ <= 11) || (defined(_MSC_VER) && _MSC_VER <= 1920)
    #undef WJR_HAS_FEATURE_STRING_UNINITIALIZED_RESIZE
#elif defined(__GLIBCXX__) || defined(_LIBCPP_VERSION) || defined(_MSVC_STL_VERSION)

template <typename Container>
void string_set_length_hacker(Container &bank, typename Container::size_type sz);

    #if defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)
        #define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_CLASS(Name, Container)                  \
            inline void WJR_PP_CONCAT(string_set_length_hacker_of_,                                \
                                      Name)(Container & bank, typename Container::size_type sz);   \
            template <typename Money_t, Money_t Container::*p>                                     \
            struct WJR_PP_CONCAT(string_thief_of_, Name) {                                         \
                friend void WJR_PP_CONCAT(string_set_length_hacker_of_,                            \
                                          Name)(Container & bank,                                  \
                                                typename Container::size_type sz) {                \
                    (bank.*p)(sz);                                                                 \
                }                                                                                  \
            }
    #else
        #define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_CLASS(Name, Container)                  \
            inline void WJR_PP_CONCAT(string_set_length_hacker_of_,                                \
                                      Name)(Container & bank, typename Container::size_type sz);   \
            template <typename Money_t, Money_t Container::*p>                                     \
            struct WJR_PP_CONCAT(string_thief_of_, Name) {                                         \
                friend void WJR_PP_CONCAT(string_set_length_hacker_of_,                            \
                                          Name)(Container & bank,                                  \
                                                typename Container::size_type sz) {                \
                    (bank.*p)._Myval2._Mysize = sz;                                                \
                }                                                                                  \
            }
    #endif

    #define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_HACKER(Name, Container)                     \
        template <>                                                                                \
        inline void string_set_length_hacker<Container>(Container & bank,                          \
                                                        typename Container::size_type sz) {        \
            WJR_PP_CONCAT(string_set_length_hacker_of_, Name)(bank, sz);                           \
        };

    #if defined(__GLIBCXX__)
        #define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(Name, Container)               \
            template struct WJR_PP_CONCAT(                                                         \
                string_thief_of_, Name)<void(Container::size_type), &Container::_M_set_length>
    #elif defined(_LIBCPP_VERSION)
        #define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(Name, Container)               \
            template struct WJR_PP_CONCAT(                                                         \
                string_thief_of_, Name)<void(Container::size_type), &Container::__set_size>
    #else
        #define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(Name, Container)               \
            template struct WJR_PP_CONCAT(string_thief_of_,                                        \
                                          Name)<decltype(Container::_Mypair), &Container::_Mypair>
    #endif

template <typename CharT, typename Traits, typename Alloc>
WJR_INTRINSIC_INLINE void
__uninitialized_resize(std::basic_string<CharT, Traits, Alloc> &str,
                       typename std::basic_string<CharT, Traits, Alloc>::size_type sz) {
    #if !defined(WJR_CXX_20)
    if (sz > str.capacity()) {
    #endif
        str.reserve(sz);
    #if !defined(WJR_CXX_20)
    }
    #endif

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
    WJR_INTRINSIC_INLINE static void resize(Container &cont, typename Container::size_type sz,
                                            default_construct_t) {
        __uninitialized_resize(cont, sz);
    }
};

template <typename Container>
struct __uninitialized_append_fn_impl : append_fn_impl_base<Container> {
    using append_fn_impl_base<Container>::append;
    WJR_INTRINSIC_INLINE static void append(Container &cont, typename Container::size_type sz,
                                            default_construct_t) {
        __uninitialized_resize(cont, cont.size() + sz);
    }
};

    #if WJR_STRINF_RESIZE_AND_OVERWRITE >= 202110L
template <typename Char, typename Traits, typename Alloc>
struct resize_fn_impl<std::basic_string<Char, Traits, Alloc>>
    : __uninitialized_resize_fn_impl<std::basic_string<Char, Traits, Alloc>> {};
template <typename Char, typename Traits, typename Alloc>
struct append_fn_impl<std::basic_string<Char, Traits, Alloc>>
    : __uninitialized_append_fn_impl<std::basic_string<Char, Traits, Alloc>> {};

        #define WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(Name, Container)
    #else

        #define WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(Name, Container)                          \
            __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_CLASS(Name, Container);                     \
            __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(Name, Container);                  \
            __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_HACKER(Name, Container);                    \
            template <>                                                                            \
            struct resize_fn_impl<Container> : __uninitialized_resize_fn_impl<Container> {};       \
            template <>                                                                            \
            struct append_fn_impl<Container> : __uninitialized_append_fn_impl<Container> {}

    #endif

#else
    #define WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(Name, Container)
#endif

using default_string = std::basic_string<char, std::char_traits<char>, memory_pool<char>>;

WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(default_string, default_string);

template <typename Char, typename Traits, typename Alloc>
struct get_relocate_mode<std::basic_string<Char, Traits, Alloc>> {
    static constexpr relocate_t value = relocate_t::maybe_trivial;
};

} // namespace wjr

#endif // WJR_STRING_HPP__