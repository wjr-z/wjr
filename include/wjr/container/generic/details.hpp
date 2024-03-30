#ifndef WJR_CONTAINER_GENERIC_DETAILS_HPP__
#define WJR_CONTAINER_GENERIC_DETAILS_HPP__

#include <wjr/type_traits.hpp>

namespace wjr {
namespace container_details {

template <typename Enable, typename Container, typename Size, typename... Args>
struct __has_container_reserve : std::false_type {};
template <typename Container, typename Size, typename... Args>
struct __has_container_reserve<
    std::void_t<decltype(std::declval<Container>().reserve(std::declval<Size>()))>,
    Container, Size, Args...> : std::true_type {};
template <typename Container, typename Size, typename... Args>
struct has_container_reserve : __has_container_reserve<void, Container, Size, Args...> {};
template <typename Container, typename Size, typename... Args>
constexpr bool has_container_reserve_v =
    has_container_reserve<Container, Size, Args...>::value;

WJR_REGISTER_HAS_TYPE(container_begin, std::declval<Container>().begin(), Container);
WJR_REGISTER_HAS_TYPE(container_cbegin, std::declval<Container>().cbegin(), Container);
WJR_REGISTER_HAS_TYPE(container_end, std::declval<Container>().end(), Container);
WJR_REGISTER_HAS_TYPE(container_cend, std::declval<Container>().cend(), Container);
WJR_REGISTER_HAS_TYPE(container_size, std::declval<Container>().size(), Container);
WJR_REGISTER_HAS_TYPE(container_insert,
                      (std::declval<Container>().insert(
                           std::declval<Container>().cbegin(), std::declval<Args>()...),
                       std::declval<Container>().insert(std::declval<Container>().cend(),
                                                        std::declval<Args>()...)),
                      Container);
WJR_REGISTER_HAS_TYPE(container_append,
                      std::declval<Container>().append(std::declval<Args>()...),
                      Container);

} // namespace container_details

template <typename Container, typename Size>
void try_reserve(Container &c, Size s) {
    if constexpr (container_details::has_container_reserve_v<Container, Size>) {
        c.reserve(s);
    }
}

} // namespace wjr

#endif // WJR_CONTAINER_GENERIC_DETAILS_HPP__