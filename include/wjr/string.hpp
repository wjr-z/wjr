/**
 * @file string.hpp
 * @brief String utilities and optimizations
 * @author wjr
 *
 * Provides string manipulation utilities and optimizations for std::basic_string,
 * including uninitialized resize operations and character type conversions.
 */

#ifndef WJR_STRING_HPP__
#define WJR_STRING_HPP__

#include <memory>
#include <string>
#include <string_view>

#include <wjr/container/detail.hpp>
#include <wjr/format/ctype.hpp>
#include <wjr/math/broadcast.hpp>
#include <wjr/math/integral_constant.hpp>

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
_uninitialized_resize(std::basic_string<CharT, Traits, Alloc> &str,
                      typename std::basic_string<CharT, Traits, Alloc>::size_type sz) {
    using Size = typename std::basic_string<CharT, Traits, Alloc>::size_type;
    str.resize_and_overwrite(sz, [](char *, Size _sz) { return _sz; });
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
            template struct WJR_PP_CONCAT(string_thief_of_,                                        \
                                          Name)<void(Container::size_type), &Container::_set_size>
    #else
        #define __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(Name, Container)               \
            template struct WJR_PP_CONCAT(string_thief_of_,                                        \
                                          Name)<decltype(Container::_Mypair), &Container::_Mypair>
    #endif

template <typename CharT, typename Traits, typename Alloc>
WJR_INTRINSIC_INLINE void
_uninitialized_resize(std::basic_string<CharT, Traits, Alloc> &str,
                      typename std::basic_string<CharT, Traits, Alloc>::size_type sz) {
        // Before C++20, reserve may shrink capacity, so only reserve when sz > capacity()
    #if !defined(WJR_CPP_20)
    if (sz > str.capacity()) {
    #endif
        str.reserve(sz);
    #if !defined(WJR_CPP_20)
    }
    #endif

    string_set_length_hacker(str, sz);
    #if defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)
    #else
    str[sz] = '\0';
    #endif
}

#else
    #undef WJR_HAS_FEATURE_STRING_UNINITIALIZED_RESIZE
    #define WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(Name, Container)
#endif

#if WJR_HAS_FEATURE(STRING_UNINITIALIZED_RESIZE)

/// @private Implementation for uninitialized resize operation
template <typename Container>
struct _uninitialized_resize_fn_impl : resize_fn_impl_base<Container> {
    using resize_fn_impl_base<Container>::resize;
    WJR_INTRINSIC_INLINE static void resize(Container &cont, typename Container::size_type sz,
                                            default_construct_t) {
        _uninitialized_resize(cont, sz);
    }
};

/// @private Implementation for uninitialized append operation
template <typename Container>
struct _uninitialized_append_fn_impl : append_fn_impl_base<Container> {
    using append_fn_impl_base<Container>::append;
    WJR_INTRINSIC_INLINE static void append(Container &cont, typename Container::size_type sz,
                                            default_construct_t) {
        _uninitialized_resize(cont, cont.size() + sz);
    }
};

    #if WJR_STRINF_RESIZE_AND_OVERWRITE >= 202110L
template <typename Char, typename Traits, typename Alloc>
struct resize_fn_impl<std::basic_string<Char, Traits, Alloc>>
    : _uninitialized_resize_fn_impl<std::basic_string<Char, Traits, Alloc>> {};
template <typename Char, typename Traits, typename Alloc>
struct append_fn_impl<std::basic_string<Char, Traits, Alloc>>
    : _uninitialized_append_fn_impl<std::basic_string<Char, Traits, Alloc>> {};

        #define WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(Name, Container)
    #else

        #define WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(Name, Container)                          \
            __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_CLASS(Name, Container);                     \
            __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_TEMPLATE(Name, Container);                  \
            __WJR_REGISTER_STRING_UNINITIALIZED_RESIZE_HACKER(Name, Container);                    \
            template <>                                                                            \
            struct resize_fn_impl<Container> : _uninitialized_resize_fn_impl<Container> {};        \
            template <>                                                                            \
            struct append_fn_impl<Container> : _uninitialized_append_fn_impl<Container> {}

    #endif

#else
    #define WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(Name, Container)
#endif

namespace string_detail {
using default_string = std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
}

WJR_REGISTER_STRING_UNINITIALIZED_RESIZE(default_string, string_detail::default_string);

template <typename Char, typename Traits, typename Alloc>
struct get_relocate_mode<std::basic_string<Char, Traits, Alloc>> {
    static constexpr relocate_t value = relocate_t::maybe_trivial;
};

template <typename CharT, typename Traits>
constexpr bool starts_with(std::basic_string_view<CharT, Traits> str,
                           std::basic_string_view<CharT, Traits> sv) noexcept {
    const auto length = sv.size();
    if (str.size() < length) {
        return false;
    }

    return Traits::compare(str.data(), sv.data(), length) == 0;
}

template <typename CharT, typename Traits>
constexpr bool ends_with(std::basic_string_view<CharT, Traits> str,
                         std::basic_string_view<CharT, Traits> sv) noexcept {
    const auto n = str.size();
    const auto length = sv.size();
    if (n < length) {
        return false;
    }

    return Traits::compare(str.data() + n - length, sv.data(), length) == 0;
}

template <unsigned int Length, WJR_REQUIRES(Length <= 8)>
WJR_PURE WJR_INTRINSIC_INLINE bool
constant_length_strncaseequal(const char *a, const char *b,
                              integral_constant<unsigned int, Length>) {
    if constexpr (Length == 1) {
        const char diff = (a[0] ^ b[0]) & 0xDF;
        return diff == 0;
    } else if constexpr (Length == 2 || Length == 4 || Length == 8) {
        using diff_type = uint_t<Length * 8>;
        constexpr diff_type mask = broadcast<uint8_t, diff_type>(0xDF);
        diff_type diff = (read_memory<diff_type>(a) ^ read_memory<diff_type>(b)) & mask;
        return diff == 0;
    } else if constexpr (Length == 3) {
        return constant_length_strncaseequal(a, b, 2_u) &
               constant_length_strncaseequal(a + 2, b + 2, 1_u);
    } else if constexpr (Length >= 5 && Length <= 6) {
        constexpr auto RestLength = Length - 4;
        return constant_length_strncaseequal(a, b, 4_u) &
               constant_length_strncaseequal(a + 4, b + 4,
                                             integral_constant<unsigned int, RestLength>());
    } else {
        static_assert(Length == 7);
        return constant_length_strncaseequal(a, b, 4_u) &
               constant_length_strncaseequal(a + 3, b + 3, 4_u);
    }
}

WJR_PURE constexpr int strncasecmp(const char *a, const char *b, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        const char diff = (a[i] ^ b[i]) & 0xDF;
        if (diff != 0) {
            return a[i] < b[i] ? -1 : 1;
        }
    }

    return 0;
}

WJR_PURE constexpr int compare_insensitive(std::string_view lhs, std::string_view rhs) {
    if (int ret = strncasecmp(lhs.data(), rhs.data(), std::min(lhs.length(), rhs.length()))) {
        return ret;
    }

    return lhs.size() == rhs.size() ? 0 : (lhs.size() < rhs.size() ? -1 : 1);
}

WJR_PURE constexpr bool starts_with_insensitive(std::string_view base, std::string_view sv) {
    return base.size() >= sv.size() && strncasecmp(base.data(), sv.data(), sv.size()) == 0;
}

WJR_PURE constexpr bool ends_with_insensitive(std::string_view base, std::string_view sv) {
    return base.size() >= sv.size() &&
           strncasecmp(base.data() + base.size() - sv.size(), sv.data(), sv.size()) == 0;
}

} // namespace wjr

#endif // WJR_STRING_HPP__