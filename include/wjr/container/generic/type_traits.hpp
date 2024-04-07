#ifndef WJR_CONTAINER_GENERIC_TYPE_TRAITS_HPP__
#define WJR_CONTAINER_GENERIC_TYPE_TRAITS_HPP__

#include <array>
#include <string>
#include <vector>

#include <wjr/container/generic/details.hpp>
#include <wjr/iterator/details.hpp>
#include <wjr/vector.hpp>

namespace wjr {

template <typename T, typename = void>
struct __container_traits_base_iterator_helper {
    using iterator = T;
};

template <typename T>
struct __container_traits_base_iterator_helper<T, std::void_t<typename T::iterator>> {
    using iterator = typename T::iterator;
};

template <typename T, typename = void>
struct __container_traits_base_size_type_helper {
    using size_type = size_t;
};

template <typename T>
struct __container_traits_base_size_type_helper<T, std::void_t<typename T::size_type>> {
    using size_type = typename T::size_type;
};

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
     * @details For std::basic_string<CharT, Tratis, Alloc>, if Traits =
     * std::char_traits<CharT>, then is trivially contiguous, otherwise can't
     * copy/construct as trivial.
     *
     */
    constexpr static bool is_trivially_contiguous_v = false;

    template <typename... Args>
    constexpr static bool is_resize_v =
        container_details::has_container_resize_v<Container, size_type, Args...>;

    constexpr static bool is_reserve_v =
        container_details::has_container_reserve_v<Container, size_type>;

    template <typename... Args>
    constexpr static bool is_append_v =
        container_details::has_container_append_v<Container, Args...>;
};

template <typename Container>
struct container_traits : __container_traits_base<Container> {};

template <typename T, size_t N>
struct container_traits<std::array<T, N>> : __container_traits_base<std::array<T, N>> {
    constexpr static bool is_contiguous_v = true;
    constexpr static bool is_trivially_contiguous_v = true;
};

template <typename T, typename Alloc>
struct container_traits<std::vector<T, Alloc>>
    : __container_traits_base<std::vector<T, Alloc>> {
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
struct container_traits<basic_vector<Storage>>
    : __container_traits_base<basic_vector<Storage>> {
    constexpr static bool is_contiguous_v = true;
    constexpr static bool is_trivially_contiguous_v = true;
};

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_TYPE_TRAITS_HPP__