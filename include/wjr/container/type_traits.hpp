#ifndef WJR_CONTAINER_TYPE_TRAITS_HPP__
#define WJR_CONTAINER_TYPE_TRAITS_HPP__

#include <initializer_list>
#include <memory>
#include <string>

#include <wjr/container/detail.hpp>

#ifdef _LIBCPP_BEGIN_NAMESPACE_STD
_LIBCPP_BEGIN_NAMESPACE_STD
#else
namespace std {
#endif

#ifdef _WIN32
template <typename T, size_t N>
class array;
#else
template <typename T, size_t N>
struct array;
#endif
template <typename T, typename Allocator>
class vector;

#ifdef _LIBCPP_END_NAMESPACE_STD
_LIBCPP_END_NAMESPACE_STD
#else
} // namespace std
#endif

namespace wjr {

template <typename Storage>
class basic_vector;

template <typename Container>
struct container_traits : __container_traits_base<Container> {};

template <typename T, size_t N>
struct container_traits<std::array<T, N>> : __container_traits_base<std::array<T, N>> {
    constexpr static bool is_contiguous_v = true;
    constexpr static bool is_trivially_contiguous_v = true;
};

template <typename T, typename Alloc>
struct container_traits<std::vector<T, Alloc>> : __container_traits_base<std::vector<T, Alloc>> {
    constexpr static bool is_contiguous_v = true;
    constexpr static bool is_trivially_contiguous_v = true;
};

template <typename CharT, typename Traits, typename Alloc>
struct container_traits<std::basic_string<CharT, Traits, Alloc>>
    : __container_traits_base<std::basic_string<CharT, Traits, Alloc>> {
    constexpr static bool is_contiguous_v = true;
    constexpr static bool is_trivially_contiguous_v =
        std::is_same_v<Traits, std::char_traits<CharT>>;
};

template <typename Storage>
struct container_traits<basic_vector<Storage>> : __container_traits_base<basic_vector<Storage>> {
    constexpr static bool is_contiguous_v = true;
    constexpr static bool is_trivially_contiguous_v =
        basic_vector<Storage>::is_trivially_contiguous::value;
};

} // namespace wjr

#endif // WJR_CONTAINER_TYPE_TRAITS_HPP__